// SPDX-License-Identifier: MIT
//
// wsys -g: the screen composed on the GPU (../9front-ui PLAN.md stage 3, the
// Vulkan backend). A thread of its own owns Vulkan (Mesa's libvulkan.a, ash):
// a swapchain on the whole screen (VK_PLAN9_surface without rio: the frames go
// to the display, gpu(4) scanout), each frame
//
//	- what devdraw changed on its screen (#G/n/dirty, a reader of its own:
//	  draw(3)'s windows, wsys' borders, text and menus), copied from #G's
//	  framebuffer (gpu(3)) by the processor into a buffer the GPU reads, and
//	  from there into the frame;
//	- over it the clients' GPU buffers (gpubuffer: a dma-buf of gpu(4),
//	  imported into this device), each where its window shows it and is not
//	  covered, copied by the GPU: the processor copies none of their pixels.
//
// Copies, and for a layer with alpha over others (a Wayland subsurface,
// wsys' layers) one pipeline: a triangle cut to the layer's rectangles whose
// fragment shader reads the client's buffer itself (a storage buffer) and is
// blended over the frame (ONE, ONE_MINUS_SRC_ALPHA: premultiplied); its
// SPIR-V is in ../shaders (scripts/build/wsys-shaders.sh). Without the
// pipeline (the swapchain's images cannot be drawn into) such a layer is
// copied: it shows, its alpha ignored. The frames are paced by the display
// (FIFO); what changes meanwhile goes into the next one. The main loop tells
// the thread what is shown (Job::Scene: the GPU windows, bottom to top, their
// visible rectangles) and hears when a frame is on its way (Done::Composed):
// a client's buffer shown before is free from then on. A frame read back
// (Job::Snapshot) is what wsys' screen file gives while the GPU composes.
//
// Should the thread fail (the device lost, the display refusing), it says so
// (Done::Failed) and ends; its swapchain goes, the display shows devdraw's
// screen again, and wsys copies the buffers with the processor as before.

use crate::draw::Rect;
use crate::p9::{Fd, OREAD, Seg};
use ash::vk;
use std::collections::HashMap;
use std::ffi::{CStr, CString, c_char, c_int, c_void};
use std::sync::mpsc;
use std::thread;
use std::time::Duration;

// libvulkan.a's loader (vkGetInstanceProcAddr: a weak alias of vkl_GetInstanceProcAddr)
// and what it needs after it: APE's libdraw (the plan9 WSI) with lib9, libfmt,
// libutf, and libstdc++ (Mesa's C++); Mesa's dma-buf fds of gpu(4) tokens
// (plan9/lib/dmabuf.c)
#[link(name = "vulkan")]
#[link(name = "draw")]
#[link(name = "9")]
#[link(name = "fmt")]
#[link(name = "utf")]
#[link(name = "stdc++")]
unsafe extern "system" {
    fn vkGetInstanceProcAddr(instance: vk::Instance, name: *const c_char) -> vk::PFN_vkVoidFunction;
}
unsafe extern "C" {
    fn plan9_dmabuf_fd(token: *const c_char) -> c_int;
    fn close(fd: c_int) -> c_int;
}

// VK_PLAN9_surface (sys/include/ape/vulkan/vulkan_plan9.h); not in ash's registry
const PLAN9_SURFACE: &CStr = c"VK_PLAN9_surface";
const STRUCTURE_TYPE_PLAN9_SURFACE_CREATE_INFO_PLAN9: vk::StructureType = vk::StructureType::from_raw(1008999000);

#[repr(C)]
struct Plan9SurfaceCreateInfo {
    s_type: vk::StructureType,
    p_next: *const c_void,
    flags: vk::Flags,
    p_window_name: *const c_char, // null: /dev/winname (the screen: wsys is not in a window)
}

type CreatePlan9Surface =
    unsafe extern "system" fn(vk::Instance, *const Plan9SurfaceCreateInfo, *const vk::AllocationCallbacks, *mut vk::SurfaceKHR) -> vk::Result;

/// a client's buffer: its window and its number there
pub type Key = (u32, u32);

/// a GPU window as it is shown: its buffer, where the buffer's top left corner
/// is on the screen, and the rectangles of it that are seen (screen coordinates)
#[derive(Clone, PartialEq)]
pub struct Shown {
    pub key: Key,
    pub at: (i32, i32),
    pub rects: Vec<Rect>,
    pub blend: bool, // blended over what is under it (a8r8g8b8, premultiplied), else copied
}

static BLENDVERT: &[u8] = include_bytes!("../shaders/blend.vert.spv");
static BLENDFRAG: &[u8] = include_bytes!("../shaders/blend.frag.spv");

/// the pipeline blending a layer over the frame, and what draws into the
/// swapchain's images
struct Blend {
    rp: vk::RenderPass,
    dsl: vk::DescriptorSetLayout,
    layout: vk::PipelineLayout,
    pipe: vk::Pipeline,
    dpool: vk::DescriptorPool,
    views: Vec<vk::ImageView>,
    fbs: Vec<vk::Framebuffer>,
    maxrange: u64, // the most a storage buffer may be
}

pub enum Job {
    /// a client's buffer (gpubuffer), imported into the device
    Import { key: Key, token: String, w: i32, h: i32, stride: usize, reply: mpsc::Sender<Result<(), String>> },
    /// let go of it (unbuffer, the window deleted)
    Forget(Key),
    /// what is shown now; a frame follows (a buffer presented, a window moved)
    Scene(Vec<Shown>),
    /// a frame read back: rectangle r as x8r8g8b8 rows
    Snapshot(Rect, mpsc::Sender<Result<Vec<u8>, String>>),
    /// devdraw changed r (the dirty reader)
    Damage(Rect),
}

pub enum Done {
    /// a frame with these buffers is on its way: the ones shown before are free
    Composed(Vec<Key>),
    /// the thread is gone: the processor copies again
    Failed(String),
}

/// the main loop's end of the thread
pub struct Comp {
    tx: mpsc::Sender<Job>,
}

/// devdraw's screen when it is wsys' own (drawcore): its segment's name, set
/// before start; the main loop then tells the changes (damage), no reader of
/// #G's dirty
static CORESCREEN: std::sync::OnceLock<String> = std::sync::OnceLock::new();

pub fn corescreen(seg: &str) {
    let _ = CORESCREEN.set(seg.to_string());
}

// what goes well said too (wsys -g asked for the GPU; by default only what fails:
// a desktop's messages go to the console, which draws over the screen)
static VERBOSE: std::sync::atomic::AtomicBool = std::sync::atomic::AtomicBool::new(false);

macro_rules! say {
    ($($a:tt)*) => {
        if VERBOSE.load(std::sync::atomic::Ordering::Relaxed) {
            eprintln!($($a)*);
        }
    };
}

impl Comp {
    /// the thread for the screen of size w x h, telling done what it did; an
    /// error if Vulkan, the swapchain on the screen or the framebuffer are not
    /// there (wsys then composes with the processor); verbose: what goes well
    /// is said too
    pub fn start(done: Box<dyn Fn(Done) + Send>, verbose: bool) -> Result<Comp, String> {
        VERBOSE.store(verbose, std::sync::atomic::Ordering::Relaxed);
        if std::env::var("WSI_PLAN9_COPY").is_ok_and(|v| v.trim() != "0" && !v.trim().is_empty()) {
            // the frames would go through draw(3), onto the screen they are made of
            return Err("WSI_PLAN9_COPY: not on the display".into());
        }
        let (tx, rx) = mpsc::channel::<Job>();
        let (itx, irx) = mpsc::channel::<Result<(), String>>();
        let dtx = tx.clone();
        thread::Builder::new()
            .name("comp".into())
            .stack_size(8 << 20)
            .spawn(move || {
                let mut c = match Vkc::new() {
                    Ok(c) => c,
                    Err(e) => {
                        let _ = itx.send(Err(e));
                        return;
                    }
                };
                let gdir = c.gdir.clone();
                let _ = itx.send(Ok(()));
                // devdraw's changes, read by a thread of their own (the read blocks);
                // wsys' own devdraw's the main loop tells
                if CORESCREEN.get().is_none() {
                    thread::spawn(move || dirtyreader(&gdir, dtx));
                }
                let e = c.run(rx, &*done);
                c.destroy();
                done(Done::Failed(e));
            })
            .map_err(|e| e.to_string())?;
        match irx.recv_timeout(Duration::from_secs(30)) {
            Ok(Ok(())) => Ok(Comp { tx }),
            Ok(Err(e)) => Err(e),
            Err(_) => Err("Vulkan did not start in 30 s".into()),
        }
    }

    /// a client's buffer into the device; the answer within 10 s (a GPU that
    /// hangs must not hang wsys)
    pub fn import(&self, key: Key, token: &str, w: i32, h: i32, stride: usize) -> Result<(), String> {
        let (reply, r) = mpsc::channel();
        self.tx.send(Job::Import { key, token: token.to_string(), w, h, stride, reply }).map_err(|_| "the GPU's thread is gone")?;
        r.recv_timeout(Duration::from_secs(10)).map_err(|_| "the GPU's thread did not answer".to_string())?
    }

    pub fn forget(&self, key: Key) {
        let _ = self.tx.send(Job::Forget(key));
    }

    /// devdraw (wsys' own) drew in r
    pub fn damage(&self, r: Rect) {
        let _ = self.tx.send(Job::Damage(r));
    }

    pub fn scene(&self, s: Vec<Shown>) -> bool {
        self.tx.send(Job::Scene(s)).is_ok()
    }

    /// rectangle r of the next frame, x8r8g8b8
    pub fn snapshot(&self, r: Rect) -> Result<Vec<u8>, String> {
        let (reply, rx) = mpsc::channel();
        self.tx.send(Job::Snapshot(r, reply)).map_err(|_| "the GPU's thread is gone")?;
        rx.recv_timeout(Duration::from_secs(10)).map_err(|_| "the GPU's thread did not answer".to_string())?
    }
}

/// #G/n/dirty: what devdraw changed, as damage for the thread
fn dirtyreader(gdir: &str, tx: mpsc::Sender<Job>) {
    let path = format!("{gdir}/dirty");
    let fd = match Fd::open(&path, OREAD) {
        Ok(fd) => fd,
        Err(e) => {
            eprintln!("wsys: {path}: {e}");
            return;
        }
    };
    let mut b = [0u8; 64];
    loop {
        let n = match fd.read(&mut b) {
            Ok(n) if n > 0 => n,
            _ => return,
        };
        let v: Vec<i32> = String::from_utf8_lossy(&b[..n]).split_whitespace().filter_map(|x| x.parse().ok()).collect();
        if v.len() == 4 && tx.send(Job::Damage(Rect::new(v[0], v[1], v[2], v[3]))).is_err() {
            return;
        }
    }
}

/// the union's bounding box
fn join(a: Option<Rect>, b: Rect) -> Option<Rect> {
    if b.empty() {
        return a;
    }
    Some(match a {
        None => b,
        Some(a) => Rect::new(a.x0.min(b.x0), a.y0.min(b.y0), a.x1.max(b.x1), a.y1.max(b.y1)),
    })
}

fn vkerr(what: &str) -> impl Fn(vk::Result) -> String + '_ {
    move |r| format!("{what}: {r:?}")
}

/// a client's buffer, imported
struct Imp {
    buf: vk::Buffer,
    mem: vk::DeviceMemory,
    set: Option<vk::DescriptorSet>, // the buffer to the blending shader
    w: i32,
    h: i32,
    stride: usize,
}

/// a buffer of the device's the processor sees (host visible, coherent)
struct HostBuf {
    buf: vk::Buffer,
    mem: vk::DeviceMemory,
    ptr: *mut u8,
    size: usize,
}

struct Vkc {
    _entry: ash::Entry,
    inst: ash::Instance,
    surface: ash::khr::surface::Instance,
    surf: vk::SurfaceKHR,
    pd: vk::PhysicalDevice,
    dev: ash::Device,
    q: vk::Queue,
    swapchain: ash::khr::swapchain::Device,
    memfd: ash::khr::external_memory_fd::Device,
    memprops: vk::PhysicalDeviceMemoryProperties,
    sc: vk::SwapchainKHR,
    imgs: Vec<vk::Image>,
    fresh: Vec<bool>,           // not drawn into yet: everything is damage
    pending: Vec<Option<Rect>>, // what changed since each image was drawn into
    w: i32,
    h: i32,
    pool: vk::CommandPool,
    cb: vk::CommandBuffer,
    acquired: vk::Semaphore,
    done: vk::Semaphore,
    fence: vk::Fence,
    busy: bool, // the fence is the frame's on the GPU
    staging: Option<HostBuf>, // devdraw's screen, as the GPU reads it
    readback: Option<HostBuf>,
    gdir: String,     // #G/n of the display
    fb: Option<Seg>,  // #G's framebuffer: devdraw's screen, w*4 bytes a row
    fbdamage: Option<Rect>, // changed in fb since staging got it
    imps: HashMap<Key, Imp>,
    scene: Vec<Shown>,
    frames: u64,
    checked: bool, // the first frame read back and compared with devdraw's screen
    blend: Option<Blend>,
    drawable: bool, // the swapchain's images may be drawn into (COLOR_ATTACHMENT)
}

impl Vkc {
    fn new() -> Result<Vkc, String> {
        let gdir = displaygpu()?;
        let entry = unsafe { ash::Entry::from_static_fn(ash::StaticFn { get_instance_proc_addr: vkGetInstanceProcAddr }) };
        let iexts = unsafe { entry.enumerate_instance_extension_properties(None) }.map_err(vkerr("instance extensions"))?;
        if !iexts.iter().any(|e| e.extension_name_as_c_str() == Ok(PLAN9_SURFACE)) {
            return Err("no VK_PLAN9_surface".into());
        }
        let iext = [ash::khr::surface::NAME.as_ptr(), PLAN9_SURFACE.as_ptr()];
        let app = vk::ApplicationInfo::default().application_name(c"wsys").api_version(vk::API_VERSION_1_1);
        let ici = vk::InstanceCreateInfo::default().application_info(&app).enabled_extension_names(&iext);
        let inst = unsafe { entry.create_instance(&ici, None) }.map_err(vkerr("vkCreateInstance"))?;
        let create: CreatePlan9Surface = match unsafe { entry.get_instance_proc_addr(inst.handle(), c"vkCreatePlan9SurfacePLAN9".as_ptr()) } {
            Some(f) => unsafe { std::mem::transmute::<unsafe extern "system" fn(), CreatePlan9Surface>(f) },
            None => return Err("no vkCreatePlan9SurfacePLAN9".into()),
        };
        let sci = Plan9SurfaceCreateInfo {
            s_type: STRUCTURE_TYPE_PLAN9_SURFACE_CREATE_INFO_PLAN9,
            p_next: std::ptr::null(),
            flags: 0,
            p_window_name: std::ptr::null(),
        };
        let mut surf = vk::SurfaceKHR::null();
        unsafe { create(inst.handle(), &sci, std::ptr::null(), &mut surf) }.result().map_err(vkerr("vkCreatePlan9SurfacePLAN9"))?;
        let surface = ash::khr::surface::Instance::new(&entry, &inst);
        let pds = unsafe { inst.enumerate_physical_devices() }.map_err(vkerr("physical devices"))?;
        let pd = *pds.first().ok_or("no physical device")?;
        let dexts = unsafe { inst.enumerate_device_extension_properties(pd) }.map_err(vkerr("device extensions"))?;
        let want = [ash::khr::swapchain::NAME, ash::khr::external_memory_fd::NAME, ash::ext::external_memory_dma_buf::NAME];
        for w in want {
            if !dexts.iter().any(|e| e.extension_name_as_c_str() == Ok(w)) {
                return Err(format!("the device has no {}", w.to_string_lossy()));
            }
        }
        let qfp = unsafe { inst.get_physical_device_queue_family_properties(pd) };
        let qf = (0..qfp.len() as u32)
            .find(|&i| {
                qfp[i as usize].queue_flags.intersects(vk::QueueFlags::GRAPHICS | vk::QueueFlags::COMPUTE)
                    && unsafe { surface.get_physical_device_surface_support(pd, i, surf) }.unwrap_or(false)
            })
            .ok_or("no queue family presents to the screen")?;
        let prio = [1.0f32];
        let qci = [vk::DeviceQueueCreateInfo::default().queue_family_index(qf).queue_priorities(&prio)];
        let dext: Vec<*const c_char> = want.iter().map(|w| w.as_ptr()).collect();
        let dci = vk::DeviceCreateInfo::default().queue_create_infos(&qci).enabled_extension_names(&dext);
        let dev = unsafe { inst.create_device(pd, &dci, None) }.map_err(vkerr("vkCreateDevice"))?;
        let q = unsafe { dev.get_device_queue(qf, 0) };
        let swapchain = ash::khr::swapchain::Device::new(&inst, &dev);
        let memfd = ash::khr::external_memory_fd::Device::new(&inst, &dev);
        let memprops = unsafe { inst.get_physical_device_memory_properties(pd) };
        let pci = vk::CommandPoolCreateInfo::default().flags(vk::CommandPoolCreateFlags::RESET_COMMAND_BUFFER).queue_family_index(qf);
        let pool = unsafe { dev.create_command_pool(&pci, None) }.map_err(vkerr("vkCreateCommandPool"))?;
        let cai = vk::CommandBufferAllocateInfo::default().command_pool(pool).level(vk::CommandBufferLevel::PRIMARY).command_buffer_count(1);
        let cb = unsafe { dev.allocate_command_buffers(&cai) }.map_err(vkerr("vkAllocateCommandBuffers"))?[0];
        let semi = vk::SemaphoreCreateInfo::default();
        let acquired = unsafe { dev.create_semaphore(&semi, None) }.map_err(vkerr("semaphore"))?;
        let done = unsafe { dev.create_semaphore(&semi, None) }.map_err(vkerr("semaphore"))?;
        let fence = unsafe { dev.create_fence(&vk::FenceCreateInfo::default(), None) }.map_err(vkerr("fence"))?;
        let props = unsafe { inst.get_physical_device_properties(pd) };
        let mut c = Vkc {
            _entry: entry,
            inst,
            surface,
            surf,
            pd,
            dev,
            q,
            swapchain,
            memfd,
            memprops,
            sc: vk::SwapchainKHR::null(),
            imgs: vec![],
            fresh: vec![],
            pending: vec![],
            w: 0,
            h: 0,
            pool,
            cb,
            acquired,
            done,
            fence,
            busy: false,
            staging: None,
            readback: None,
            gdir,
            fb: None,
            fbdamage: None,
            imps: HashMap::new(),
            scene: vec![],
            frames: 0,
            checked: false,
            blend: None,
            drawable: false,
        };
        c.mkswapchain()?;
        match c.mkblend(props.limits.max_storage_buffer_range as u64) {
            Ok(b) => {
                c.blend = Some(b);
                c.mkfbs()?;
            }
            Err(e) => eprintln!("wsys: composing on the GPU: no blending (layers with alpha copied): {e}"),
        }
        say!(
            "wsys: composing on the GPU: {}, {}x{}, {} images",
            props.device_name_as_c_str().unwrap_or(c"?").to_string_lossy(),
            c.w,
            c.h,
            c.imgs.len()
        );
        Ok(c)
    }

    fn memtype(&self, bits: u32, want: vk::MemoryPropertyFlags) -> Option<u32> {
        (0..self.memprops.memory_type_count)
            .find(|&i| bits & (1 << i) != 0 && self.memprops.memory_types[i as usize].property_flags.contains(want))
    }

    fn hostbuf(&self, size: usize, usage: vk::BufferUsageFlags) -> Result<HostBuf, String> {
        let bci = vk::BufferCreateInfo::default().size(size as u64).usage(usage).sharing_mode(vk::SharingMode::EXCLUSIVE);
        let buf = unsafe { self.dev.create_buffer(&bci, None) }.map_err(vkerr("vkCreateBuffer"))?;
        let mr = unsafe { self.dev.get_buffer_memory_requirements(buf) };
        let host = vk::MemoryPropertyFlags::HOST_VISIBLE | vk::MemoryPropertyFlags::HOST_COHERENT;
        let Some(t) = self.memtype(mr.memory_type_bits, host | vk::MemoryPropertyFlags::HOST_CACHED).or_else(|| self.memtype(mr.memory_type_bits, host)) else {
            unsafe { self.dev.destroy_buffer(buf, None) };
            return Err("no host visible memory".into());
        };
        let mai = vk::MemoryAllocateInfo::default().allocation_size(mr.size).memory_type_index(t);
        let mem = match unsafe { self.dev.allocate_memory(&mai, None) } {
            Ok(m) => m,
            Err(r) => {
                unsafe { self.dev.destroy_buffer(buf, None) };
                return Err(format!("vkAllocateMemory: {r:?}"));
            }
        };
        let ptr = unsafe {
            self.dev
                .bind_buffer_memory(buf, mem, 0)
                .and_then(|_| self.dev.map_memory(mem, 0, vk::WHOLE_SIZE, vk::MemoryMapFlags::empty()))
        };
        match ptr {
            Ok(p) => Ok(HostBuf { buf, mem, ptr: p as *mut u8, size }),
            Err(r) => {
                unsafe {
                    self.dev.destroy_buffer(buf, None);
                    self.dev.free_memory(mem, None);
                }
                Err(format!("vkMapMemory: {r:?}"))
            }
        }
    }

    fn freehost(&self, b: Option<HostBuf>) {
        if let Some(b) = b {
            unsafe {
                self.dev.destroy_buffer(b.buf, None);
                self.dev.free_memory(b.mem, None);
            }
        }
    }

    /// the swapchain on the screen as big as it is now; the framebuffer and the
    /// buffers of its size with it
    fn mkswapchain(&mut self) -> Result<(), String> {
        self.idle();
        let caps = unsafe { self.surface.get_physical_device_surface_capabilities(self.pd, self.surf) }.map_err(vkerr("surface capabilities"))?;
        let ext = caps.current_extent;
        let mut usage = vk::ImageUsageFlags::TRANSFER_DST | vk::ImageUsageFlags::TRANSFER_SRC;
        if !caps.supported_usage_flags.contains(usage) {
            return Err("the swapchain's images cannot be copied to and from".into());
        }
        // drawn into too: layers blended (mkblend)
        if caps.supported_usage_flags.contains(vk::ImageUsageFlags::COLOR_ATTACHMENT) {
            usage |= vk::ImageUsageFlags::COLOR_ATTACHMENT;
        }
        self.drawable = usage.contains(vk::ImageUsageFlags::COLOR_ATTACHMENT);
        let fmts = unsafe { self.surface.get_physical_device_surface_formats(self.pd, self.surf) }.map_err(vkerr("surface formats"))?;
        if !fmts.iter().any(|f| f.format == vk::Format::B8G8R8A8_UNORM) {
            return Err("no B8G8R8A8_UNORM on the screen".into());
        }
        let old = self.sc;
        let swci = vk::SwapchainCreateInfoKHR::default()
            .surface(self.surf)
            .min_image_count(caps.min_image_count.max(2))
            .image_format(vk::Format::B8G8R8A8_UNORM)
            .image_color_space(vk::ColorSpaceKHR::SRGB_NONLINEAR)
            .image_extent(ext)
            .image_array_layers(1)
            .image_usage(usage)
            .image_sharing_mode(vk::SharingMode::EXCLUSIVE)
            .pre_transform(vk::SurfaceTransformFlagsKHR::IDENTITY)
            .composite_alpha(vk::CompositeAlphaFlagsKHR::OPAQUE)
            .present_mode(vk::PresentModeKHR::FIFO)
            .clipped(true)
            .old_swapchain(old);
        let sc = unsafe { self.swapchain.create_swapchain(&swci, None) }.map_err(vkerr("vkCreateSwapchainKHR"))?;
        if old != vk::SwapchainKHR::null() {
            unsafe { self.swapchain.destroy_swapchain(old, None) };
        }
        self.sc = sc;
        self.imgs = unsafe { self.swapchain.get_swapchain_images(sc) }.map_err(vkerr("swapchain images"))?;
        if self.blend.is_some() {
            self.mkfbs()?;
        }
        self.fresh = vec![true; self.imgs.len()];
        self.pending = vec![None; self.imgs.len()];
        let (w, h) = (ext.width as i32, ext.height as i32);
        if (w, h) != (self.w, self.h) || self.staging.is_none() {
            self.w = w;
            self.h = h;
            let size = w as usize * h as usize * 4;
            self.fb = Some(match CORESCREEN.get() {
                Some(name) => {
                    let (va, len) = Seg::find(name).map_err(|e| format!("{name}: {e}"))?;
                    if len < size {
                        return Err(format!("{name}: {len} bytes, the screen {size}"));
                    }
                    // inherited when wsys attached it before starting this thread
                    Seg::attach_or_have(name, (va, len)).map_err(|e| format!("{name}: {e}"))?
                }
                None => fbattach(&self.gdir, size)?,
            });
            let s = self.staging.take();
            self.freehost(s);
            let r = self.readback.take();
            self.freehost(r);
            self.staging = Some(self.hostbuf(size, vk::BufferUsageFlags::TRANSFER_SRC)?);
            self.readback = Some(self.hostbuf(size, vk::BufferUsageFlags::TRANSFER_DST)?);
        }
        // everything from devdraw's screen again
        self.fbdamage = Some(Rect::new(0, 0, w, h));
        Ok(())
    }

    /// the frame on the GPU done
    fn idle(&mut self) {
        if self.busy {
            unsafe {
                let _ = self.dev.wait_for_fences(&[self.fence], true, 5_000_000_000);
                let _ = self.dev.reset_fences(&[self.fence]);
            }
            self.busy = false;
        }
    }

    fn import(&mut self, key: Key, token: &str, w: i32, h: i32, stride: usize) -> Result<(), String> {
        if stride % 4 != 0 {
            return Err("stride not a multiple of 4".into());
        }
        self.forget(key);
        let size = (stride * (h as usize - 1) + w as usize * 4) as u64;
        let t = CString::new(token).map_err(|_| "bad token")?;
        let fd = unsafe { plan9_dmabuf_fd(t.as_ptr()) };
        if fd < 0 {
            return Err("no dma-buf fd for the token".into());
        }
        let ht = vk::ExternalMemoryHandleTypeFlags::DMA_BUF_EXT;
        let res = (|| -> Result<Imp, String> {
            let mut fdp = vk::MemoryFdPropertiesKHR::default();
            unsafe { self.memfd.get_memory_fd_properties(ht, fd, &mut fdp) }.map_err(vkerr("vkGetMemoryFdPropertiesKHR"))?;
            let mut ext = vk::ExternalMemoryBufferCreateInfo::default().handle_types(ht);
            let bci = vk::BufferCreateInfo::default()
                .size(size)
                .usage(vk::BufferUsageFlags::TRANSFER_SRC | vk::BufferUsageFlags::STORAGE_BUFFER)
                .sharing_mode(vk::SharingMode::EXCLUSIVE)
                .push_next(&mut ext);
            let buf = unsafe { self.dev.create_buffer(&bci, None) }.map_err(vkerr("vkCreateBuffer"))?;
            let mr = unsafe { self.dev.get_buffer_memory_requirements(buf) };
            let bits = mr.memory_type_bits & fdp.memory_type_bits;
            let Some(t) = (0..32).find(|i| bits & (1 << i) != 0) else {
                unsafe { self.dev.destroy_buffer(buf, None) };
                return Err("no memory type for the buffer".into());
            };
            let mut imp = vk::ImportMemoryFdInfoKHR::default().handle_type(ht).fd(fd);
            let mut ded = vk::MemoryDedicatedAllocateInfo::default().buffer(buf);
            let mai = vk::MemoryAllocateInfo::default().allocation_size(mr.size).memory_type_index(t).push_next(&mut imp).push_next(&mut ded);
            let mem = match unsafe { self.dev.allocate_memory(&mai, None) } {
                Ok(m) => m,
                Err(r) => {
                    unsafe { self.dev.destroy_buffer(buf, None) };
                    return Err(format!("vkAllocateMemory (import): {r:?}"));
                }
            };
            if let Err(r) = unsafe { self.dev.bind_buffer_memory(buf, mem, 0) } {
                unsafe {
                    self.dev.destroy_buffer(buf, None);
                    self.dev.free_memory(mem, None);
                }
                return Err(format!("vkBindBufferMemory: {r:?}"));
            }
            let set = self.blendset(buf, size);
            Ok(Imp { buf, mem, set, w, h, stride })
        })();
        match res {
            Ok(i) => {
                // the fd is Vulkan's now
                self.imps.insert(key, i);
                Ok(())
            }
            Err(e) => {
                unsafe { close(fd) };
                Err(e)
            }
        }
    }

    fn forget(&mut self, key: Key) {
        if let Some(i) = self.imps.remove(&key) {
            // not while a frame reads it
            self.idle();
            unsafe {
                if let (Some(set), Some(b)) = (i.set, &self.blend) {
                    let _ = self.dev.free_descriptor_sets(b.dpool, &[set]);
                }
                self.dev.destroy_buffer(i.buf, None);
                self.dev.free_memory(i.mem, None);
            }
        }
    }

    /// the blending pipeline (its render pass on the swapchain's format); an error
    /// if the swapchain's images cannot be drawn into
    fn mkblend(&mut self, maxrange: u64) -> Result<Blend, String> {
        if !self.drawable {
            return Err("the swapchain's images cannot be drawn into".into());
        }
        let dev = &self.dev;
        let att = [vk::AttachmentDescription::default()
            .format(vk::Format::B8G8R8A8_UNORM)
            .samples(vk::SampleCountFlags::TYPE_1)
            .load_op(vk::AttachmentLoadOp::LOAD)
            .store_op(vk::AttachmentStoreOp::STORE)
            .stencil_load_op(vk::AttachmentLoadOp::DONT_CARE)
            .stencil_store_op(vk::AttachmentStoreOp::DONT_CARE)
            .initial_layout(vk::ImageLayout::COLOR_ATTACHMENT_OPTIMAL)
            .final_layout(vk::ImageLayout::COLOR_ATTACHMENT_OPTIMAL)];
        let cref = [vk::AttachmentReference::default().attachment(0).layout(vk::ImageLayout::COLOR_ATTACHMENT_OPTIMAL)];
        let sub = [vk::SubpassDescription::default().pipeline_bind_point(vk::PipelineBindPoint::GRAPHICS).color_attachments(&cref)];
        let rpi = vk::RenderPassCreateInfo::default().attachments(&att).subpasses(&sub);
        let rp = unsafe { dev.create_render_pass(&rpi, None) }.map_err(vkerr("vkCreateRenderPass"))?;
        let bind = [vk::DescriptorSetLayoutBinding::default()
            .binding(0)
            .descriptor_type(vk::DescriptorType::STORAGE_BUFFER)
            .descriptor_count(1)
            .stage_flags(vk::ShaderStageFlags::FRAGMENT)];
        let dsl = unsafe { dev.create_descriptor_set_layout(&vk::DescriptorSetLayoutCreateInfo::default().bindings(&bind), None) }
            .map_err(vkerr("vkCreateDescriptorSetLayout"))?;
        let pcr = [vk::PushConstantRange::default().stage_flags(vk::ShaderStageFlags::FRAGMENT).offset(0).size(12)];
        let dsls = [dsl];
        let layout = unsafe { dev.create_pipeline_layout(&vk::PipelineLayoutCreateInfo::default().set_layouts(&dsls).push_constant_ranges(&pcr), None) }
            .map_err(vkerr("vkCreatePipelineLayout"))?;
        let module = |code: &[u8]| -> Result<vk::ShaderModule, String> {
            let words: Vec<u32> = code.chunks_exact(4).map(|c| u32::from_le_bytes(c.try_into().unwrap())).collect();
            unsafe { dev.create_shader_module(&vk::ShaderModuleCreateInfo::default().code(&words), None) }.map_err(vkerr("vkCreateShaderModule"))
        };
        let (vs, fs) = (module(BLENDVERT)?, module(BLENDFRAG)?);
        let stages = [
            vk::PipelineShaderStageCreateInfo::default().stage(vk::ShaderStageFlags::VERTEX).module(vs).name(c"main"),
            vk::PipelineShaderStageCreateInfo::default().stage(vk::ShaderStageFlags::FRAGMENT).module(fs).name(c"main"),
        ];
        let vin = vk::PipelineVertexInputStateCreateInfo::default();
        let ia = vk::PipelineInputAssemblyStateCreateInfo::default().topology(vk::PrimitiveTopology::TRIANGLE_LIST);
        let vp = vk::PipelineViewportStateCreateInfo::default().viewport_count(1).scissor_count(1);
        let rs = vk::PipelineRasterizationStateCreateInfo::default()
            .polygon_mode(vk::PolygonMode::FILL)
            .cull_mode(vk::CullModeFlags::NONE)
            .front_face(vk::FrontFace::COUNTER_CLOCKWISE)
            .line_width(1.0);
        let ms = vk::PipelineMultisampleStateCreateInfo::default().rasterization_samples(vk::SampleCountFlags::TYPE_1);
        let cba = [vk::PipelineColorBlendAttachmentState::default()
            .blend_enable(true)
            .src_color_blend_factor(vk::BlendFactor::ONE)
            .dst_color_blend_factor(vk::BlendFactor::ONE_MINUS_SRC_ALPHA)
            .color_blend_op(vk::BlendOp::ADD)
            .src_alpha_blend_factor(vk::BlendFactor::ONE)
            .dst_alpha_blend_factor(vk::BlendFactor::ONE_MINUS_SRC_ALPHA)
            .alpha_blend_op(vk::BlendOp::ADD)
            .color_write_mask(vk::ColorComponentFlags::RGBA)];
        let cb = vk::PipelineColorBlendStateCreateInfo::default().attachments(&cba);
        let dyns = [vk::DynamicState::VIEWPORT, vk::DynamicState::SCISSOR];
        let ds = vk::PipelineDynamicStateCreateInfo::default().dynamic_states(&dyns);
        let gpi = [vk::GraphicsPipelineCreateInfo::default()
            .stages(&stages)
            .vertex_input_state(&vin)
            .input_assembly_state(&ia)
            .viewport_state(&vp)
            .rasterization_state(&rs)
            .multisample_state(&ms)
            .color_blend_state(&cb)
            .dynamic_state(&ds)
            .layout(layout)
            .render_pass(rp)
            .subpass(0)];
        let pipes = unsafe { dev.create_graphics_pipelines(vk::PipelineCache::null(), &gpi, None) };
        unsafe {
            dev.destroy_shader_module(vs, None);
            dev.destroy_shader_module(fs, None);
        }
        let pipe = pipes.map_err(|(_, r)| format!("vkCreateGraphicsPipelines: {r:?}"))?[0];
        let sizes = [vk::DescriptorPoolSize::default().ty(vk::DescriptorType::STORAGE_BUFFER).descriptor_count(1024)];
        let dpi = vk::DescriptorPoolCreateInfo::default()
            .flags(vk::DescriptorPoolCreateFlags::FREE_DESCRIPTOR_SET)
            .max_sets(1024)
            .pool_sizes(&sizes);
        let dpool = unsafe { dev.create_descriptor_pool(&dpi, None) }.map_err(vkerr("vkCreateDescriptorPool"))?;
        say!("wsys: composing on the GPU: layers with alpha blended");
        Ok(Blend { rp, dsl, layout, pipe, dpool, views: vec![], fbs: vec![], maxrange })
    }

    /// views and framebuffers of the swapchain's images for the blending
    fn mkfbs(&mut self) -> Result<(), String> {
        let Some(b) = self.blend.as_mut() else { return Ok(()) };
        let dev = &self.dev;
        unsafe {
            for f in b.fbs.drain(..) {
                dev.destroy_framebuffer(f, None);
            }
            for v in b.views.drain(..) {
                dev.destroy_image_view(v, None);
            }
        }
        let range = vk::ImageSubresourceRange { aspect_mask: vk::ImageAspectFlags::COLOR, base_mip_level: 0, level_count: 1, base_array_layer: 0, layer_count: 1 };
        for &img in &self.imgs {
            let vci = vk::ImageViewCreateInfo::default()
                .image(img)
                .view_type(vk::ImageViewType::TYPE_2D)
                .format(vk::Format::B8G8R8A8_UNORM)
                .subresource_range(range);
            let v = unsafe { dev.create_image_view(&vci, None) }.map_err(vkerr("vkCreateImageView"))?;
            b.views.push(v);
            let atts = [v];
            let fci = vk::FramebufferCreateInfo::default()
                .render_pass(b.rp)
                .attachments(&atts)
                .width(self.w as u32)
                .height(self.h as u32)
                .layers(1);
            b.fbs.push(unsafe { dev.create_framebuffer(&fci, None) }.map_err(vkerr("vkCreateFramebuffer"))?);
        }
        Ok(())
    }

    /// a descriptor set with buf (size bytes) for the blending shader; none
    /// without the pipeline or if the buffer is too big for one
    fn blendset(&self, buf: vk::Buffer, size: u64) -> Option<vk::DescriptorSet> {
        let b = self.blend.as_ref()?;
        if size > b.maxrange {
            eprintln!("wsys: a buffer of {size} bytes is too big to blend (at most {}): copied", b.maxrange);
            return None;
        }
        let dsls = [b.dsl];
        let ai = vk::DescriptorSetAllocateInfo::default().descriptor_pool(b.dpool).set_layouts(&dsls);
        let set = match unsafe { self.dev.allocate_descriptor_sets(&ai) } {
            Ok(v) => v[0],
            Err(r) => {
                eprintln!("wsys: a descriptor set to blend a buffer: {r:?}: copied");
                return None;
            }
        };
        let bi = [vk::DescriptorBufferInfo::default().buffer(buf).offset(0).range(vk::WHOLE_SIZE)];
        let wr = [vk::WriteDescriptorSet::default()
            .dst_set(set)
            .dst_binding(0)
            .descriptor_type(vk::DescriptorType::STORAGE_BUFFER)
            .buffer_info(&bi)];
        unsafe { self.dev.update_descriptor_sets(&wr, &[]) };
        Some(set)
    }

    /// what the GPU windows covered: devdraw's screen there again, in every image
    fn uncover(&mut self, s: &[Shown]) {
        for sh in s {
            for r in &sh.rects {
                for p in self.pending.iter_mut() {
                    *p = join(*p, *r);
                }
            }
        }
    }

    /// devdraw's changes from the framebuffer into staging (the GPU is done with it)
    fn takefb(&mut self) {
        let Some(d) = self.fbdamage.take() else { return };
        let d = d.clip(&Rect::new(0, 0, self.w, self.h));
        if d.empty() {
            return;
        }
        let (Some(fb), Some(st)) = (&self.fb, &self.staging) else { return };
        let stride = self.w as usize * 4;
        let n = d.dx() as usize * 4;
        for y in d.y0..d.y1 {
            let o = y as usize * stride + d.x0 as usize * 4;
            if o + n <= fb.len && o + n <= st.size {
                unsafe { std::ptr::copy_nonoverlapping((fb.va + o) as *const u8, st.ptr.add(o), n) };
            }
        }
        for p in self.pending.iter_mut() {
            *p = join(*p, d);
        }
    }

    /// a frame: devdraw's changes and the GPU windows; with snap, rectangle snap
    /// of it read back
    fn frame(&mut self, snap: Option<Rect>) -> Result<Option<Vec<u8>>, String> {
        self.idle();
        self.takefb();
        let mut idx = None;
        for _ in 0..3 {
            match unsafe { self.swapchain.acquire_next_image(self.sc, 2_000_000_000, self.acquired, vk::Fence::null()) } {
                Ok((i, _)) => {
                    idx = Some(i as usize);
                    break;
                }
                Err(vk::Result::ERROR_OUT_OF_DATE_KHR) => {
                    self.mkswapchain()?;
                    self.takefb();
                }
                Err(vk::Result::TIMEOUT | vk::Result::NOT_READY) => return Err("vkAcquireNextImageKHR: no image in 2 s".into()),
                Err(r) => return Err(format!("vkAcquireNextImageKHR: {r:?}")),
            }
        }
        let idx = idx.ok_or("the swapchain is out of date again and again")?;
        let all = Rect::new(0, 0, self.w, self.h);
        let img = self.imgs[idx];
        let fresh = std::mem::replace(&mut self.fresh[idx], false);
        let base = if fresh { Some(all) } else { self.pending[idx].take().map(|r| r.clip(&all)).filter(|r| !r.empty()) };
        self.pending[idx] = None;
        let range = vk::ImageSubresourceRange { aspect_mask: vk::ImageAspectFlags::COLOR, base_mip_level: 0, level_count: 1, base_array_layer: 0, layer_count: 1 };
        let layers = vk::ImageSubresourceLayers { aspect_mask: vk::ImageAspectFlags::COLOR, mip_level: 0, base_array_layer: 0, layer_count: 1 };
        let barrier = |old, new, src, dst| {
            vk::ImageMemoryBarrier::default()
                .old_layout(old)
                .new_layout(new)
                .src_access_mask(src)
                .dst_access_mask(dst)
                .src_queue_family_index(vk::QUEUE_FAMILY_IGNORED)
                .dst_queue_family_index(vk::QUEUE_FAMILY_IGNORED)
                .image(img)
                .subresource_range(range)
        };
        let copy = |r: &Rect, off: usize, rowlen: u32| {
            vk::BufferImageCopy::default()
                .buffer_offset(off as u64)
                .buffer_row_length(rowlen)
                .buffer_image_height(0)
                .image_subresource(layers)
                .image_offset(vk::Offset3D { x: r.x0, y: r.y0, z: 0 })
                .image_extent(vk::Extent3D { width: r.dx() as u32, height: r.dy() as u32, depth: 1 })
        };
        let (staging, readback) = (self.staging.as_ref().ok_or("no staging")?, self.readback.as_ref().ok_or("no readback")?);
        let dev = &self.dev;
        let cb = self.cb;
        unsafe {
            dev.reset_command_buffer(cb, vk::CommandBufferResetFlags::empty()).map_err(vkerr("reset"))?;
            let cbi = vk::CommandBufferBeginInfo::default().flags(vk::CommandBufferUsageFlags::ONE_TIME_SUBMIT);
            dev.begin_command_buffer(cb, &cbi).map_err(vkerr("begin"))?;
            let old = if fresh { vk::ImageLayout::UNDEFINED } else { vk::ImageLayout::PRESENT_SRC_KHR };
            let b = [barrier(old, vk::ImageLayout::TRANSFER_DST_OPTIMAL, vk::AccessFlags::empty(), vk::AccessFlags::TRANSFER_WRITE)];
            dev.cmd_pipeline_barrier(cb, vk::PipelineStageFlags::TOP_OF_PIPE, vk::PipelineStageFlags::TRANSFER, vk::DependencyFlags::empty(), &[], &[], &b);
            if let Some(r) = base {
                let off = (r.y0 as usize * self.w as usize + r.x0 as usize) * 4;
                dev.cmd_copy_buffer_to_image(cb, staging.buf, img, vk::ImageLayout::TRANSFER_DST_OPTIMAL, &[copy(&r, off, self.w as u32)]);
            }
            // the windows over it, bottom to top (each copy after the one under it)
            for s in &self.scene {
                let Some(i) = self.imps.get(&s.key) else { continue };
                let mine = Rect::new(s.at.0, s.at.1, s.at.0 + i.w, s.at.1 + i.h);
                if let (true, Some(bl), Some(set)) = (s.blend, &self.blend, i.set) {
                    // drawn over the frame: its rectangles as scissors of one triangle each
                    let rects: Vec<Rect> = s.rects.iter().map(|r| r.clip(&mine).clip(&all)).filter(|r| !r.empty()).collect();
                    if rects.is_empty() {
                        continue;
                    }
                    let b = [barrier(
                        vk::ImageLayout::TRANSFER_DST_OPTIMAL,
                        vk::ImageLayout::COLOR_ATTACHMENT_OPTIMAL,
                        vk::AccessFlags::TRANSFER_WRITE,
                        vk::AccessFlags::COLOR_ATTACHMENT_READ | vk::AccessFlags::COLOR_ATTACHMENT_WRITE,
                    )];
                    dev.cmd_pipeline_barrier(cb, vk::PipelineStageFlags::TRANSFER, vk::PipelineStageFlags::COLOR_ATTACHMENT_OUTPUT, vk::DependencyFlags::empty(), &[], &[], &b);
                    let rpb = vk::RenderPassBeginInfo::default()
                        .render_pass(bl.rp)
                        .framebuffer(bl.fbs[idx])
                        .render_area(vk::Rect2D { offset: vk::Offset2D { x: 0, y: 0 }, extent: vk::Extent2D { width: self.w as u32, height: self.h as u32 } });
                    dev.cmd_begin_render_pass(cb, &rpb, vk::SubpassContents::INLINE);
                    dev.cmd_bind_pipeline(cb, vk::PipelineBindPoint::GRAPHICS, bl.pipe);
                    dev.cmd_bind_descriptor_sets(cb, vk::PipelineBindPoint::GRAPHICS, bl.layout, 0, &[set], &[]);
                    let pc: Vec<u8> = [s.at.0, s.at.1, (i.stride / 4) as i32].iter().flat_map(|v| v.to_le_bytes()).collect();
                    dev.cmd_push_constants(cb, bl.layout, vk::ShaderStageFlags::FRAGMENT, 0, &pc);
                    let vp = [vk::Viewport { x: 0.0, y: 0.0, width: self.w as f32, height: self.h as f32, min_depth: 0.0, max_depth: 1.0 }];
                    dev.cmd_set_viewport(cb, 0, &vp);
                    for r in &rects {
                        let sc = [vk::Rect2D { offset: vk::Offset2D { x: r.x0, y: r.y0 }, extent: vk::Extent2D { width: r.dx() as u32, height: r.dy() as u32 } }];
                        dev.cmd_set_scissor(cb, 0, &sc);
                        dev.cmd_draw(cb, 3, 1, 0, 0);
                    }
                    dev.cmd_end_render_pass(cb);
                    let b = [barrier(
                        vk::ImageLayout::COLOR_ATTACHMENT_OPTIMAL,
                        vk::ImageLayout::TRANSFER_DST_OPTIMAL,
                        vk::AccessFlags::COLOR_ATTACHMENT_WRITE,
                        vk::AccessFlags::TRANSFER_WRITE,
                    )];
                    dev.cmd_pipeline_barrier(cb, vk::PipelineStageFlags::COLOR_ATTACHMENT_OUTPUT, vk::PipelineStageFlags::TRANSFER, vk::DependencyFlags::empty(), &[], &[], &b);
                    continue;
                }
                let regions: Vec<vk::BufferImageCopy> = s
                    .rects
                    .iter()
                    .map(|r| r.clip(&mine).clip(&all))
                    .filter(|r| !r.empty())
                    .map(|r| copy(&r, (r.y0 - s.at.1) as usize * i.stride + (r.x0 - s.at.0) as usize * 4, (i.stride / 4) as u32))
                    .collect();
                if !regions.is_empty() {
                    let b = [barrier(vk::ImageLayout::TRANSFER_DST_OPTIMAL, vk::ImageLayout::TRANSFER_DST_OPTIMAL, vk::AccessFlags::TRANSFER_WRITE, vk::AccessFlags::TRANSFER_WRITE)];
                    dev.cmd_pipeline_barrier(cb, vk::PipelineStageFlags::TRANSFER, vk::PipelineStageFlags::TRANSFER, vk::DependencyFlags::empty(), &[], &[], &b);
                    dev.cmd_copy_buffer_to_image(cb, i.buf, img, vk::ImageLayout::TRANSFER_DST_OPTIMAL, &regions);
                }
            }
            let snap = snap.map(|r| r.clip(&all));
            let mut last = vk::ImageLayout::TRANSFER_DST_OPTIMAL;
            let mut lastaccess = vk::AccessFlags::TRANSFER_WRITE;
            if let Some(r) = snap.filter(|r| !r.empty()) {
                let b = [barrier(last, vk::ImageLayout::TRANSFER_SRC_OPTIMAL, lastaccess, vk::AccessFlags::TRANSFER_READ)];
                dev.cmd_pipeline_barrier(cb, vk::PipelineStageFlags::TRANSFER, vk::PipelineStageFlags::TRANSFER, vk::DependencyFlags::empty(), &[], &[], &b);
                let reg = [vk::BufferImageCopy::default()
                    .buffer_offset(0)
                    .buffer_row_length(r.dx() as u32)
                    .image_subresource(layers)
                    .image_offset(vk::Offset3D { x: r.x0, y: r.y0, z: 0 })
                    .image_extent(vk::Extent3D { width: r.dx() as u32, height: r.dy() as u32, depth: 1 })];
                dev.cmd_copy_image_to_buffer(cb, img, vk::ImageLayout::TRANSFER_SRC_OPTIMAL, readback.buf, &reg);
                let hb = [vk::BufferMemoryBarrier::default()
                    .src_access_mask(vk::AccessFlags::TRANSFER_WRITE)
                    .dst_access_mask(vk::AccessFlags::HOST_READ)
                    .src_queue_family_index(vk::QUEUE_FAMILY_IGNORED)
                    .dst_queue_family_index(vk::QUEUE_FAMILY_IGNORED)
                    .buffer(readback.buf)
                    .size(vk::WHOLE_SIZE)];
                dev.cmd_pipeline_barrier(cb, vk::PipelineStageFlags::TRANSFER, vk::PipelineStageFlags::HOST, vk::DependencyFlags::empty(), &[], &hb, &[]);
                last = vk::ImageLayout::TRANSFER_SRC_OPTIMAL;
                lastaccess = vk::AccessFlags::TRANSFER_READ;
            }
            let b = [barrier(last, vk::ImageLayout::PRESENT_SRC_KHR, lastaccess, vk::AccessFlags::empty())];
            dev.cmd_pipeline_barrier(cb, vk::PipelineStageFlags::TRANSFER, vk::PipelineStageFlags::BOTTOM_OF_PIPE, vk::DependencyFlags::empty(), &[], &[], &b);
            dev.end_command_buffer(cb).map_err(vkerr("end"))?;
            let ws = [vk::PipelineStageFlags::TRANSFER];
            let waits = [self.acquired];
            let signals = [self.done];
            let cbs = [cb];
            let si = [vk::SubmitInfo::default().wait_semaphores(&waits).wait_dst_stage_mask(&ws).command_buffers(&cbs).signal_semaphores(&signals)];
            dev.queue_submit(self.q, &si, self.fence).map_err(vkerr("vkQueueSubmit"))?;
            self.busy = true;
            let scs = [self.sc];
            let idxs = [idx as u32];
            let pi = vk::PresentInfoKHR::default().wait_semaphores(&signals).swapchains(&scs).image_indices(&idxs);
            match self.swapchain.queue_present(self.q, &pi) {
                Ok(false) => {}
                // the display refused the frame (the WSI copies it into devdraw's
                // screen, which this composes again): the processor's way
                Ok(true) => return Err("the display refused the frames (scanout): the WSI copies them".into()),
                Err(vk::Result::ERROR_OUT_OF_DATE_KHR) => {
                    // the screen changed size: the next frame makes the swapchain again
                    self.mkswapchain()?;
                }
                Err(r) => return Err(format!("vkQueuePresentKHR: {r:?}")),
            }
            self.frames += 1;
            let Some(r) = snap else { return Ok(None) };
            if r.empty() {
                return Ok(Some(vec![]));
            }
            self.idle();
            let rb = self.readback.as_ref().unwrap();
            let n = r.dx() as usize * r.dy() as usize * 4;
            Ok(Some(std::slice::from_raw_parts(rb.ptr, n.min(rb.size)).to_vec()))
        }
    }

    /// the first frame read back: as devdraw's screen (the copy the GPU read)
    /// where no GPU window is; else the frames show something else: back to
    /// the processor, which shows the screen as it is
    fn selfcheck(&mut self) -> Result<(), String> {
        let all = Rect::new(0, 0, self.w, self.h);
        let mut bad = vec![];
        for _ in 0..3 {
            // all of devdraw's screen as it is now (its changes may not have come
            // through the dirty reader yet), then compared with it at once
            self.fbdamage = Some(all);
            let px = self.frame(Some(all))?.unwrap_or_default();
            // what the GPU read: devdraw's screen as copied for the frame
            let Some(st) = &self.staging else { return Err("no staging".into()) };
            bad.clear();
            let mut n = 0;
            for i in 0..8 {
                for j in 0..8 {
                    let (x, y) = (self.w * (2 * j + 1) / 16, self.h * (2 * i + 1) / 16);
                    if self.scene.iter().any(|s| s.rects.iter().any(|r| r.contains(x, y))) {
                        continue;
                    }
                    let o = (y as usize * self.w as usize + x as usize) * 4;
                    if o + 4 > px.len() || o + 4 > st.size {
                        continue;
                    }
                    let got = u32::from_le_bytes(px[o..o + 4].try_into().unwrap()) & 0xFFFFFF;
                    let want = unsafe { std::ptr::read_volatile(st.ptr.add(o) as *const u32) } & 0xFFFFFF;
                    n += 1;
                    if got != want {
                        bad.push(format!("{x},{y} {got:06x} not {want:06x}"));
                    }
                }
            }
            if bad.is_empty() {
                say!("wsys: composing on the GPU: the first frame is devdraw's screen ({n} points)");
                return Ok(());
            }
        }
        Err(format!("the first frame is not devdraw's screen: {}", bad.iter().take(6).cloned().collect::<Vec<_>>().join(", ")))
    }

    fn run(&mut self, rx: mpsc::Receiver<Job>, done: &dyn Fn(Done)) -> String {
        loop {
            // wait for something, then take whatever else came meanwhile
            let Ok(j) = rx.recv() else { return "wsys ended".into() };
            let mut jobs = vec![j];
            while let Ok(j) = rx.try_recv() {
                jobs.push(j);
            }
            let mut need = false;
            let mut snaps = vec![];
            for j in jobs {
                match j {
                    Job::Import { key, token, w, h, stride, reply } => {
                        let _ = reply.send(self.import(key, &token, w, h, stride));
                    }
                    Job::Forget(key) => self.forget(key),
                    Job::Scene(s) => {
                        let old = std::mem::replace(&mut self.scene, s);
                        self.uncover(&old);
                        need = true;
                    }
                    Job::Damage(r) => {
                        self.fbdamage = join(self.fbdamage, r);
                        need = true;
                    }
                    Job::Snapshot(r, reply) => snaps.push((r, reply)),
                }
            }
            if !need && snaps.is_empty() {
                continue;
            }
            // one frame for all, read back as a whole and cut for each
            let whole = snaps.iter().fold(None, |a, (r, _)| join(a, *r));
            match self.frame(whole) {
                Ok(px) => {
                    if let (Some(px), Some(u)) = (px, whole) {
                        let u = u.clip(&Rect::new(0, 0, self.w, self.h));
                        for (r, reply) in snaps {
                            let r = r.clip(&u);
                            let mut out = Vec::with_capacity(r.dx().max(0) as usize * r.dy().max(0) as usize * 4);
                            for y in r.y0..r.y1 {
                                let o = ((y - u.y0) as usize * u.dx() as usize + (r.x0 - u.x0) as usize) * 4;
                                out.extend_from_slice(&px[o..o + r.dx() as usize * 4]);
                            }
                            let _ = reply.send(Ok(out));
                        }
                    }
                    done(Done::Composed(self.scene.iter().map(|s| s.key).collect()));
                    if !self.checked {
                        self.checked = true;
                        if let Err(e) = self.selfcheck() {
                            return e;
                        }
                    }
                    if self.frames % 3600 == 0 {
                        say!("wsys: composing on the GPU: {} frames, {} GPU buffers", self.frames, self.imps.len());
                    }
                }
                Err(e) => {
                    for (_, reply) in snaps {
                        let _ = reply.send(Err(e.clone()));
                    }
                    return e;
                }
            }
        }
    }

    /// everything let go of: the display shows devdraw's screen again
    fn destroy(&mut self) {
        self.idle();
        unsafe {
            let _ = self.dev.device_wait_idle();
        }
        let keys: Vec<Key> = self.imps.keys().copied().collect();
        for k in keys {
            self.forget(k);
        }
        let s = self.staging.take();
        self.freehost(s);
        let r = self.readback.take();
        self.freehost(r);
        if let Some(mut b) = self.blend.take() {
            unsafe {
                for f in b.fbs.drain(..) {
                    self.dev.destroy_framebuffer(f, None);
                }
                for v in b.views.drain(..) {
                    self.dev.destroy_image_view(v, None);
                }
                self.dev.destroy_pipeline(b.pipe, None);
                self.dev.destroy_pipeline_layout(b.layout, None);
                self.dev.destroy_descriptor_set_layout(b.dsl, None);
                self.dev.destroy_descriptor_pool(b.dpool, None);
                self.dev.destroy_render_pass(b.rp, None);
            }
        }
        unsafe {
            if self.sc != vk::SwapchainKHR::null() {
                self.swapchain.destroy_swapchain(self.sc, None);
            }
            self.dev.destroy_fence(self.fence, None);
            self.dev.destroy_semaphore(self.acquired, None);
            self.dev.destroy_semaphore(self.done, None);
            self.dev.destroy_command_pool(self.pool, None);
            self.dev.destroy_device(None);
            self.surface.destroy_surface(self.surf, None);
            self.inst.destroy_instance(None);
        }
        eprintln!("wsys: no longer composing on the GPU ({} frames)", self.frames);
    }
}

/// #G/n of the display: the GPU whose framebuffer devdraw draws into
fn displaygpu() -> Result<String, String> {
    for n in 0..4 {
        let dir = format!("#G/{n}");
        let Ok(s) = std::fs::read_to_string(format!("{dir}/ctl")) else { continue };
        if s.lines().any(|l| l.starts_with("fb 0x")) {
            return Ok(dir);
        }
    }
    Err("no framebuffer of gpu(3): devdraw does not draw for a GPU's driver".into())
}

/// #G's framebuffer, at least size bytes (the driver made it; the "fb pa size
/// seg" line of its ctl)
fn fbattach(gdir: &str, size: usize) -> Result<Seg, String> {
    let s = std::fs::read_to_string(format!("{gdir}/ctl")).map_err(|e| format!("{gdir}/ctl: {e}"))?;
    for l in s.lines() {
        let f: Vec<&str> = l.split_whitespace().collect();
        if f.len() == 4 && f[0] == "fb" {
            let len: usize = f[2].parse().map_err(|_| format!("{gdir}/ctl: {l}"))?;
            if len < size {
                return Err(format!("the framebuffer has {len} bytes, the screen {size}"));
            }
            return Seg::phys(f[3], len).map_err(|e| format!("framebuffer {}: {e}", f[3]));
        }
    }
    Err(format!("{gdir}/ctl: no framebuffer"))
}
