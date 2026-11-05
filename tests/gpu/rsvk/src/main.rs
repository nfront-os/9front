// SPDX-License-Identifier: MIT
//
// rsvk: vktri in Rust. A red triangle on dark blue, drawn with a graphics
// pipeline (shaders/tri.vert, tri.frag; SPIR-V compiled on the host with glslc)
// into a VK_PLAN9_surface swapchain, through ash and the static loader of
// libvulkan.a. Runs for the seconds given (default 3), then reads pixels back
// through draw(3) (screenpeek.c) and checks them: before printing, the kernel's
// console draws on the screen.
//
// usage: rsvk [seconds]

use ash::vk;
use std::ffi::{CStr, c_char, c_int, c_uint, c_void};
use std::process::exit;
use std::time::Instant;

// libvulkan.a's loader: its vkGetInstanceProcAddr (a weak alias of vkl_GetInstanceProcAddr)
#[link(name = "vulkan")]
unsafe extern "system" {
    fn vkGetInstanceProcAddr(instance: vk::Instance, name: *const c_char) -> vk::PFN_vkVoidFunction;
}

// screenpeek.c: a pixel of the window (or screen) as 0xRRGGBB
unsafe extern "C" {
    fn screenpeek(x: c_int, y: c_int, rgb: *mut c_uint) -> c_int;
}

// VK_PLAN9_surface (sys/include/ape/vulkan/vulkan_plan9.h); not in ash's registry
const PLAN9_SURFACE: &CStr = c"VK_PLAN9_surface";
const STRUCTURE_TYPE_PLAN9_SURFACE_CREATE_INFO_PLAN9: vk::StructureType =
    vk::StructureType::from_raw(1008999000);

#[repr(C)]
struct Plan9SurfaceCreateInfo {
    s_type: vk::StructureType,
    p_next: *const c_void,
    flags: vk::Flags,
    p_window_name: *const c_char, // null: /dev/winname, or the screen without rio
}

type CreatePlan9Surface = unsafe extern "system" fn(
    vk::Instance,
    *const Plan9SurfaceCreateInfo,
    *const vk::AllocationCallbacks,
    *mut vk::SurfaceKHR,
) -> vk::Result;

static VERT: &[u8] = include_bytes!(concat!(env!("RSVK_SPV"), "/tri.vert.spv"));
static FRAG: &[u8] = include_bytes!(concat!(env!("RSVK_SPV"), "/tri.frag.spv"));

fn fail(what: String) -> ! {
    println!("FAIL {what}");
    exit(1);
}

trait Check<T> {
    fn check(self, what: &str) -> T;
}

impl<T> Check<T> for Result<T, vk::Result> {
    fn check(self, what: &str) -> T {
        self.unwrap_or_else(|r| fail(format!("{what}: {r:?}")))
    }
}

// the same colour, give or take what a 16-bit screen loses
fn near(a: u32, b: u32) -> bool {
    (0..24).step_by(8).all(|i| ((a >> i & 0xff) as i32 - (b >> i & 0xff) as i32).abs() <= 8)
}

fn shader(dev: &ash::Device, spv: &[u8]) -> vk::ShaderModule {
    let code = ash::util::read_spv(&mut std::io::Cursor::new(spv))
        .unwrap_or_else(|e| fail(format!("SPIR-V: {e}")));
    let ci = vk::ShaderModuleCreateInfo::default().code(&code);
    unsafe { dev.create_shader_module(&ci, None) }.check("vkCreateShaderModule")
}

fn main() {
    let secs: f64 = std::env::args().nth(1).and_then(|s| s.parse().ok()).unwrap_or(3.0);

    let entry = unsafe {
        ash::Entry::from_static_fn(ash::StaticFn { get_instance_proc_addr: vkGetInstanceProcAddr })
    };
    let apiver = unsafe { entry.try_enumerate_instance_version() }
        .check("vkEnumerateInstanceVersion")
        .unwrap_or(vk::API_VERSION_1_0);
    let iexts = unsafe { entry.enumerate_instance_extension_properties(None) }
        .check("instance extensions");
    println!(
        "ok loader: instance version {}.{}, {} instance extensions",
        vk::api_version_major(apiver),
        vk::api_version_minor(apiver),
        iexts.len()
    );
    if !iexts.iter().any(|e| e.extension_name_as_c_str() == Ok(PLAN9_SURFACE)) {
        fail("no VK_PLAN9_surface".into());
    }

    let iext = [ash::khr::surface::NAME.as_ptr(), PLAN9_SURFACE.as_ptr()];
    let app = vk::ApplicationInfo::default()
        .application_name(c"rsvk")
        .api_version(vk::API_VERSION_1_1);
    let ici = vk::InstanceCreateInfo::default()
        .application_info(&app)
        .enabled_extension_names(&iext);
    let inst = unsafe { entry.create_instance(&ici, None) }.check("vkCreateInstance");

    // the plan9 surface, through vkGetInstanceProcAddr as an extension's commands are found
    let create: CreatePlan9Surface = match unsafe {
        entry.get_instance_proc_addr(inst.handle(), c"vkCreatePlan9SurfacePLAN9".as_ptr())
    } {
        Some(f) => unsafe { std::mem::transmute::<unsafe extern "system" fn(), CreatePlan9Surface>(f) },
        None => fail("vkGetInstanceProcAddr(vkCreatePlan9SurfacePLAN9)".into()),
    };
    let sci = Plan9SurfaceCreateInfo {
        s_type: STRUCTURE_TYPE_PLAN9_SURFACE_CREATE_INFO_PLAN9,
        p_next: std::ptr::null(),
        flags: 0,
        p_window_name: std::ptr::null(),
    };
    let mut surf = vk::SurfaceKHR::null();
    unsafe { create(inst.handle(), &sci, std::ptr::null(), &mut surf) }
        .result()
        .check("vkCreatePlan9SurfacePLAN9");
    let surface = ash::khr::surface::Instance::new(&entry, &inst);

    let pds = unsafe { inst.enumerate_physical_devices() }.check("physical devices");
    let pd = *pds.first().unwrap_or_else(|| fail("no physical device".into()));
    let props = unsafe { inst.get_physical_device_properties(pd) };
    println!(
        "ok device {}",
        props.device_name_as_c_str().unwrap_or(c"?").to_string_lossy()
    );
    let qfp = unsafe { inst.get_physical_device_queue_family_properties(pd) };
    let qf = (0..qfp.len() as u32)
        .find(|&i| {
            qfp[i as usize].queue_flags.contains(vk::QueueFlags::GRAPHICS)
                && unsafe { surface.get_physical_device_surface_support(pd, i, surf) }.unwrap_or(false)
        })
        .unwrap_or_else(|| fail("no queue family can present".into()));
    let caps = unsafe { surface.get_physical_device_surface_capabilities(pd, surf) }
        .check("surface capabilities");

    let prio = [1.0f32];
    let qci = [vk::DeviceQueueCreateInfo::default()
        .queue_family_index(qf)
        .queue_priorities(&prio)];
    let dext = [ash::khr::swapchain::NAME.as_ptr()];
    let dci = vk::DeviceCreateInfo::default()
        .queue_create_infos(&qci)
        .enabled_extension_names(&dext);
    let dev = unsafe { inst.create_device(pd, &dci, None) }.check("vkCreateDevice");
    let q = unsafe { dev.get_device_queue(qf, 0) };
    let swapchain = ash::khr::swapchain::Device::new(&inst, &dev);

    let fmt = vk::Format::B8G8R8A8_UNORM;
    let ext = caps.current_extent;
    let swci = vk::SwapchainCreateInfoKHR::default()
        .surface(surf)
        .min_image_count(caps.min_image_count.max(2))
        .image_format(fmt)
        .image_color_space(vk::ColorSpaceKHR::SRGB_NONLINEAR)
        .image_extent(ext)
        .image_array_layers(1)
        .image_usage(vk::ImageUsageFlags::COLOR_ATTACHMENT)
        .image_sharing_mode(vk::SharingMode::EXCLUSIVE)
        .pre_transform(vk::SurfaceTransformFlagsKHR::IDENTITY)
        .composite_alpha(vk::CompositeAlphaFlagsKHR::OPAQUE)
        .present_mode(vk::PresentModeKHR::FIFO)
        .clipped(true);
    let sc = unsafe { swapchain.create_swapchain(&swci, None) }.check("vkCreateSwapchainKHR");
    let imgs = unsafe { swapchain.get_swapchain_images(sc) }.check("swapchain images");
    println!("ok swapchain of {} images {}x{}", imgs.len(), ext.width, ext.height);

    // render pass: clear, draw, present
    let att = [vk::AttachmentDescription::default()
        .format(fmt)
        .samples(vk::SampleCountFlags::TYPE_1)
        .load_op(vk::AttachmentLoadOp::CLEAR)
        .store_op(vk::AttachmentStoreOp::STORE)
        .stencil_load_op(vk::AttachmentLoadOp::DONT_CARE)
        .stencil_store_op(vk::AttachmentStoreOp::DONT_CARE)
        .initial_layout(vk::ImageLayout::UNDEFINED)
        .final_layout(vk::ImageLayout::PRESENT_SRC_KHR)];
    let refs = [vk::AttachmentReference { attachment: 0, layout: vk::ImageLayout::COLOR_ATTACHMENT_OPTIMAL }];
    let sub = [vk::SubpassDescription::default()
        .pipeline_bind_point(vk::PipelineBindPoint::GRAPHICS)
        .color_attachments(&refs)];
    let dep = [vk::SubpassDependency::default()
        .src_subpass(vk::SUBPASS_EXTERNAL)
        .dst_subpass(0)
        .src_stage_mask(vk::PipelineStageFlags::COLOR_ATTACHMENT_OUTPUT)
        .dst_stage_mask(vk::PipelineStageFlags::COLOR_ATTACHMENT_OUTPUT)
        .dst_access_mask(vk::AccessFlags::COLOR_ATTACHMENT_WRITE)];
    let rpci = vk::RenderPassCreateInfo::default()
        .attachments(&att)
        .subpasses(&sub)
        .dependencies(&dep);
    let rp = unsafe { dev.create_render_pass(&rpci, None) }.check("vkCreateRenderPass");

    let fbs: Vec<vk::Framebuffer> = imgs
        .iter()
        .map(|&img| {
            let vci = vk::ImageViewCreateInfo::default()
                .image(img)
                .view_type(vk::ImageViewType::TYPE_2D)
                .format(fmt)
                .subresource_range(vk::ImageSubresourceRange {
                    aspect_mask: vk::ImageAspectFlags::COLOR,
                    base_mip_level: 0,
                    level_count: 1,
                    base_array_layer: 0,
                    layer_count: 1,
                });
            let view = unsafe { dev.create_image_view(&vci, None) }.check("vkCreateImageView");
            let views = [view];
            let fci = vk::FramebufferCreateInfo::default()
                .render_pass(rp)
                .attachments(&views)
                .width(ext.width)
                .height(ext.height)
                .layers(1);
            unsafe { dev.create_framebuffer(&fci, None) }.check("vkCreateFramebuffer")
        })
        .collect();

    // the pipeline
    let pcr = [vk::PushConstantRange { stage_flags: vk::ShaderStageFlags::FRAGMENT, offset: 0, size: 16 }];
    let plci = vk::PipelineLayoutCreateInfo::default().push_constant_ranges(&pcr);
    let pl = unsafe { dev.create_pipeline_layout(&plci, None) }.check("vkCreatePipelineLayout");
    let stages = [
        vk::PipelineShaderStageCreateInfo::default()
            .stage(vk::ShaderStageFlags::VERTEX)
            .module(shader(&dev, VERT))
            .name(c"main"),
        vk::PipelineShaderStageCreateInfo::default()
            .stage(vk::ShaderStageFlags::FRAGMENT)
            .module(shader(&dev, FRAG))
            .name(c"main"),
    ];
    let vis = vk::PipelineVertexInputStateCreateInfo::default();
    let ias = vk::PipelineInputAssemblyStateCreateInfo::default()
        .topology(vk::PrimitiveTopology::TRIANGLE_LIST);
    let vp = [vk::Viewport {
        x: 0.0,
        y: 0.0,
        width: ext.width as f32,
        height: ext.height as f32,
        min_depth: 0.0,
        max_depth: 1.0,
    }];
    let scissor = [vk::Rect2D { offset: vk::Offset2D { x: 0, y: 0 }, extent: ext }];
    let vps = vk::PipelineViewportStateCreateInfo::default()
        .viewports(&vp)
        .scissors(&scissor);
    let rs = vk::PipelineRasterizationStateCreateInfo::default()
        .polygon_mode(vk::PolygonMode::FILL)
        .cull_mode(vk::CullModeFlags::NONE)
        .front_face(vk::FrontFace::COUNTER_CLOCKWISE)
        .line_width(1.0);
    let ms = vk::PipelineMultisampleStateCreateInfo::default()
        .rasterization_samples(vk::SampleCountFlags::TYPE_1);
    let cba = [vk::PipelineColorBlendAttachmentState::default()
        .color_write_mask(vk::ColorComponentFlags::RGBA)];
    let cbs = vk::PipelineColorBlendStateCreateInfo::default().attachments(&cba);
    let gpci = [vk::GraphicsPipelineCreateInfo::default()
        .stages(&stages)
        .vertex_input_state(&vis)
        .input_assembly_state(&ias)
        .viewport_state(&vps)
        .rasterization_state(&rs)
        .multisample_state(&ms)
        .color_blend_state(&cbs)
        .layout(pl)
        .render_pass(rp)
        .subpass(0)];
    let pipe = unsafe { dev.create_graphics_pipelines(vk::PipelineCache::null(), &gpci, None) }
        .map_err(|(_, r)| r)
        .check("vkCreateGraphicsPipelines")[0];
    println!("ok graphics pipeline");

    let pci = vk::CommandPoolCreateInfo::default()
        .flags(vk::CommandPoolCreateFlags::RESET_COMMAND_BUFFER)
        .queue_family_index(qf);
    let pool = unsafe { dev.create_command_pool(&pci, None) }.check("vkCreateCommandPool");
    let cai = vk::CommandBufferAllocateInfo::default()
        .command_pool(pool)
        .level(vk::CommandBufferLevel::PRIMARY)
        .command_buffer_count(1);
    let cb = unsafe { dev.allocate_command_buffers(&cai) }.check("vkAllocateCommandBuffers")[0];
    let semi = vk::SemaphoreCreateInfo::default();
    let acquired = unsafe { dev.create_semaphore(&semi, None) }.check("semaphore");
    let done = unsafe { dev.create_semaphore(&semi, None) }.check("semaphore");
    let fence = unsafe { dev.create_fence(&vk::FenceCreateInfo::default(), None) }.check("fence");

    let t0 = Instant::now();
    let mut frames = 0u32;
    while frames == 0 || t0.elapsed().as_secs_f64() < secs {
        let idx = match unsafe { swapchain.acquire_next_image(sc, u64::MAX, acquired, vk::Fence::null()) } {
            Ok((i, _)) => i,
            Err(vk::Result::ERROR_OUT_OF_DATE_KHR) => {
                println!("ok window changed size after {frames} frames");
                break;
            }
            Err(r) => fail(format!("vkAcquireNextImageKHR: {r:?}")),
        };
        unsafe {
            dev.reset_command_buffer(cb, vk::CommandBufferResetFlags::empty()).check("reset");
            let cbi = vk::CommandBufferBeginInfo::default()
                .flags(vk::CommandBufferUsageFlags::ONE_TIME_SUBMIT);
            dev.begin_command_buffer(cb, &cbi).check("begin");
            let clear = [vk::ClearValue { color: vk::ClearColorValue { float32: [0.0, 0.0, 0.4, 1.0] } }];
            let rbi = vk::RenderPassBeginInfo::default()
                .render_pass(rp)
                .framebuffer(fbs[idx as usize])
                .render_area(scissor[0])
                .clear_values(&clear);
            dev.cmd_begin_render_pass(cb, &rbi, vk::SubpassContents::INLINE);
            dev.cmd_bind_pipeline(cb, vk::PipelineBindPoint::GRAPHICS, pipe);
            let red: [f32; 4] = [1.0, 0.0, 0.0, 1.0];
            let bytes: Vec<u8> = red.iter().flat_map(|f| f.to_ne_bytes()).collect();
            dev.cmd_push_constants(cb, pl, vk::ShaderStageFlags::FRAGMENT, 0, &bytes);
            dev.cmd_draw(cb, 3, 1, 0, 0);
            dev.cmd_end_render_pass(cb);
            dev.end_command_buffer(cb).check("end");

            let ws = [vk::PipelineStageFlags::COLOR_ATTACHMENT_OUTPUT];
            let waits = [acquired];
            let signals = [done];
            let cbs = [cb];
            let si = [vk::SubmitInfo::default()
                .wait_semaphores(&waits)
                .wait_dst_stage_mask(&ws)
                .command_buffers(&cbs)
                .signal_semaphores(&signals)];
            dev.queue_submit(q, &si, fence).check("vkQueueSubmit");
            let scs = [sc];
            let idxs = [idx];
            let pi = vk::PresentInfoKHR::default()
                .wait_semaphores(&signals)
                .swapchains(&scs)
                .image_indices(&idxs);
            match swapchain.queue_present(q, &pi) {
                Ok(_) => {}
                Err(vk::Result::ERROR_OUT_OF_DATE_KHR) => {
                    println!("ok window changed size after {} frames", frames + 1);
                    break;
                }
                Err(r) => fail(format!("vkQueuePresentKHR: {r:?}")),
            }
            dev.wait_for_fences(&[fence], true, u64::MAX).check("fence");
            dev.reset_fences(&[fence]).check("reset fence");
        }
        frames += 1;
    }
    unsafe { dev.device_wait_idle() }.check("vkDeviceWaitIdle");
    let secs = t0.elapsed().as_secs_f64();

    // the triangle spans x -0.5..0.5, y -0.5..0.5 (y down): its centroid is
    // at (0, -1/6); read before printing (the kernel's console draws on the screen)
    let (w, h) = (ext.width as i32, ext.height as i32);
    let want = [
        (w / 2, h * 5 / 12, 0xff0000u32, "triangle"),
        (w / 2, h * 13 / 16, 0x000066, "below the triangle"),
        (w / 8, h / 2, 0x000066, "left of the triangle"),
        (w * 13 / 16, h / 5, 0x000066, "above the triangle"),
    ];
    let got: Vec<(c_int, u32)> = want
        .iter()
        .map(|&(x, y, _, _)| {
            let mut rgb: c_uint = 0;
            let r = unsafe { screenpeek(x, y, &mut rgb) };
            (r, rgb)
        })
        .collect();
    println!("ok {frames} frames in {secs:.1} s");
    for (&(x, y, rgb, what), &(r, got)) in want.iter().zip(&got) {
        if r < 0 {
            fail(format!("screenpeek {x},{y}"));
        }
        if !near(got, rgb) {
            fail(format!("screen at {x},{y} is {got:06x}, want {rgb:06x} ({what})"));
        }
        println!("ok screen at {x},{y}: {got:06x} ({what})");
    }

    unsafe {
        dev.destroy_pipeline(pipe, None);
        swapchain.destroy_swapchain(sc, None);
        dev.destroy_device(None);
        surface.destroy_surface(surf, None);
        inst.destroy_instance(None);
    }
    println!("rsvk: all passed");
}
