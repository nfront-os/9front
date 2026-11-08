// SPDX-License-Identifier: MIT
//
// The pictures of image nodes (nodes/id/image): PNG (the png crate), or
// Plan 9's image(6), uncompressed or compressed, in any channel format
// (color(6): r g b k a m x, depths dividing or multiples of eight; m8
// through Plan 9's colour map, cmap2rgb as libdraw's).  Decoded to
// unpremultiplied RGBA, eight bits a channel. cmap2rgb is libdraw's (9front,
// sys/src/libdraw/rgb.c, MIT).

pub struct Picture {
    pub width: u32,
    pub height: u32,
    pub rgba: Vec<u8>,
}

/// the picture in b: PNG by its signature, else image(6)
pub fn decode(b: &[u8]) -> Result<Picture, String> {
    if b.starts_with(b"\x89PNG\r\n\x1a\n") {
        return png(b);
    }
    image6(b)
}

fn png(b: &[u8]) -> Result<Picture, String> {
    let mut d = png::Decoder::new(std::io::Cursor::new(b));
    d.set_transformations(png::Transformations::normalize_to_color8());
    let mut r = d.read_info().map_err(|e| format!("png: {e}"))?;
    let mut buf = vec![0; r.output_buffer_size().ok_or("png: too big")?];
    let info = r.next_frame(&mut buf).map_err(|e| format!("png: {e}"))?;
    let (w, h) = (info.width as usize, info.height as usize);
    let mut rgba = Vec::with_capacity(w * h * 4);
    for y in 0..h {
        let row = &buf[y * info.line_size..];
        for x in 0..w {
            let px: [u8; 4] = match info.color_type {
                png::ColorType::Grayscale => [row[x], row[x], row[x], 255],
                png::ColorType::GrayscaleAlpha => [row[2 * x], row[2 * x], row[2 * x], row[2 * x + 1]],
                png::ColorType::Rgb => [row[3 * x], row[3 * x + 1], row[3 * x + 2], 255],
                png::ColorType::Rgba => [row[4 * x], row[4 * x + 1], row[4 * x + 2], row[4 * x + 3]],
                png::ColorType::Indexed => return Err("png: palette not expanded".into()),
            };
            rgba.extend_from_slice(&px);
        }
    }
    Ok(Picture { width: w as u32, height: h as u32, rgba })
}

/// a decimal field of the header: right justified in n characters
fn field(b: &[u8], n: usize) -> Result<(&str, &[u8]), String> {
    if b.len() < n {
        return Err("image: short header".into());
    }
    let s = std::str::from_utf8(&b[..n]).map_err(|_| "image: bad header")?;
    Ok((s.trim(), &b[n..]))
}

fn num(s: &str) -> Result<i64, String> {
    s.parse().map_err(|_| format!("image: bad number {s:?}"))
}

/// a channel of a pixel: its kind and where in the pixel's value
struct Chan {
    kind: u8,
    bits: u32,
    shift: u32,
}

/// a channel string (r8g8b8, k1, x8r8g8b8, m8, ...) or an old ldepth; the pixel's depth
fn chans(s: &str) -> Result<(Vec<Chan>, u32), String> {
    let s = match s {
        "0" => "k1",
        "1" => "k2",
        "2" => "k4",
        "3" => "m8",
        _ => s,
    };
    let b = s.as_bytes();
    let mut v = Vec::new();
    let mut i = 0;
    while i < b.len() {
        let kind = b[i];
        i += 1;
        let start = i;
        while i < b.len() && b[i].is_ascii_digit() {
            i += 1;
        }
        let bits: u32 = s[start..i].parse().map_err(|_| format!("image: bad channels {s:?}"))?;
        if !b"rgbkamx".contains(&kind) || bits == 0 || bits > 8 {
            return Err(format!("image: channels {s:?} not known"));
        }
        v.push(Chan { kind, bits, shift: 0 });
    }
    let depth: u32 = v.iter().map(|c| c.bits).sum();
    if depth == 0 || depth > 32 || (depth < 8 && 8 % depth != 0) || (depth > 8 && depth % 8 != 0) {
        return Err(format!("image: depth {depth} of {s:?}"));
    }
    // the first channel is the value's most significant bits
    let mut left = depth;
    for c in v.iter_mut() {
        left -= c.bits;
        c.shift = left;
    }
    Ok((v, depth))
}

/// Plan 9's colour map: the RGB of index c (libdraw's cmap2rgb)
fn cmap2rgb(c: u32) -> [u8; 3] {
    let r = c >> 6;
    let v = (c >> 4) & 3;
    let j = (c.wrapping_sub(v).wrapping_add(r)) & 15;
    let g = j >> 2;
    let b = j & 3;
    let den = r.max(g).max(b);
    if den == 0 {
        let v = (v * 17) as u8;
        return [v, v, v];
    }
    let num = 17 * (4 * den + v);
    [(r * num / den) as u8, (g * num / den) as u8, (b * num / den) as u8]
}

/// the bytes of a row from x0 to x1 (the byte with x0's pixel to the one with x1-1's)
fn rowbytes(x0: i64, x1: i64, depth: u32) -> usize {
    let d = depth as i64;
    if d >= 8 {
        ((x1 - x0) * d / 8) as usize
    } else {
        ((x1 * d + 7) / 8 - x0 * d / 8) as usize
    }
}

fn image6(b: &[u8]) -> Result<Picture, String> {
    let (b, compressed) = match b.strip_prefix(b"compressed\n") {
        Some(rest) => (rest, true),
        None => (b, false),
    };
    let (chan, b) = field(b, 12)?;
    let (x0, b) = field(b, 12)?;
    let (y0, b) = field(b, 12)?;
    let (x1, b) = field(b, 12)?;
    let (y1, b) = field(b, 12)?;
    let (x0, y0, x1, y1) = (num(x0)?, num(y0)?, num(x1)?, num(y1)?);
    let (cs, depth) = chans(chan)?;
    if x1 <= x0 || y1 <= y0 || x1 - x0 > 16384 || y1 - y0 > 16384 {
        return Err(format!("image: rectangle {x0} {y0} {x1} {y1}"));
    }
    let bpl = rowbytes(x0, x1, depth);
    let need = bpl * (y1 - y0) as usize;
    let data = if compressed { uncompress(b, need, y0)? } else { b.to_vec() };
    if data.len() < need {
        return Err(format!("image: {} bytes of {need}", data.len()));
    }
    let (w, h) = ((x1 - x0) as usize, (y1 - y0) as usize);
    let mut rgba = Vec::with_capacity(w * h * 4);
    let d = depth as i64;
    for y in 0..h {
        let row = &data[y * bpl..(y + 1) * bpl];
        for x in x0..x1 {
            // the pixel's value: little endian bytes (r8g8b8: blue, green, red), or
            // depth bits of a byte, the high order first
            let v: u32 = if d >= 8 {
                let o = ((x - x0) * d / 8) as usize;
                (0..(d / 8) as usize).fold(0u32, |a, k| a | (row[o + k] as u32) << (8 * k))
            } else {
                let bit = x * d;
                let byte = row[(bit / 8 - x0 * d / 8) as usize] as u32;
                (byte >> (8 - d - bit % 8)) & ((1 << d) - 1)
            };
            let mut px = [0u8, 0, 0, 255];
            let mut gray = None;
            for c in &cs {
                let max = (1u32 << c.bits) - 1;
                let raw = (v >> c.shift) & max;
                let e = (raw * 255 / max) as u8;
                match c.kind {
                    b'r' => px[0] = e,
                    b'g' => px[1] = e,
                    b'b' => px[2] = e,
                    b'a' => px[3] = e,
                    b'k' => gray = Some(e),
                    b'm' => {
                        let [r, g, bb] = cmap2rgb(raw);
                        px[0] = r;
                        px[1] = g;
                        px[2] = bb;
                    }
                    _ => {}
                }
            }
            if let Some(k) = gray {
                px[0] = k;
                px[1] = k;
                px[2] = k;
            }
            // Plan 9's alpha is premultiplied: undone for the picture
            if px[3] != 255 && cs.iter().any(|c| c.kind == b'a') {
                let a = px[3] as u32;
                for p in &mut px[..3] {
                    *p = if a == 0 { 0 } else { ((*p as u32 * 255 + a / 2) / a).min(255) as u8 };
                }
            }
            rgba.extend_from_slice(&px);
        }
    }
    Ok(Picture { width: w as u32, height: h as u32, rgba })
}

/// image(6)'s compression blocks: each "maxy nbytes" (twelve characters each), then
/// code words: high bit set, 1 to 128 literal bytes; else a copy of 3 to 34 bytes
/// from 1 to 1024 back
fn uncompress(mut b: &[u8], need: usize, y0: i64) -> Result<Vec<u8>, String> {
    let mut out: Vec<u8> = Vec::with_capacity(need);
    let mut y = y0;
    while out.len() < need {
        let (maxy, rest) = field(b, 12)?;
        let (n, rest) = field(rest, 12)?;
        let (maxy, n) = (num(maxy)?, num(n)? as usize);
        if maxy <= y || n > 6000 || rest.len() < n {
            return Err("image: bad compression block".into());
        }
        y = maxy;
        let mut c = &rest[..n];
        b = &rest[n..];
        while let Some((&code, more)) = c.split_first() {
            c = more;
            if code & 0x80 != 0 {
                let len = (code & 0x7f) as usize + 1;
                if c.len() < len {
                    return Err("image: short literal".into());
                }
                out.extend_from_slice(&c[..len]);
                c = &c[len..];
            } else {
                let Some((&lo, more)) = c.split_first() else { return Err("image: short copy".into()) };
                c = more;
                let len = ((code >> 2) & 0x1f) as usize + 3;
                let off = (((code & 3) as usize) << 8 | lo as usize) + 1;
                if off > out.len() {
                    return Err("image: copy from before the start".into());
                }
                // the copy may overlap what it makes ("prescient")
                let from = out.len() - off;
                for k in 0..len {
                    let v = out[from + k];
                    out.push(v);
                }
            }
        }
    }
    Ok(out)
}

#[cfg(test)]
mod tests {
    use super::*;

    fn header(chan: &str, x1: i64, y1: i64) -> Vec<u8> {
        format!("{chan:>11} {:>11} {:>11} {:>11} {:>11} ", 0, 0, x1, y1).into_bytes()
    }

    #[test]
    fn uncompressed() {
        // r8g8b8: blue, green, red in the file
        let mut b = header("r8g8b8", 2, 1);
        b.extend_from_slice(&[0x10, 0x20, 0x30, 0xff, 0x00, 0x00]);
        let p = decode(&b).unwrap();
        assert_eq!(p.rgba, vec![0x30, 0x20, 0x10, 255, 0, 0, 0xff, 255]);
        // k1: one bit a pixel, the high order first
        let mut b = header("k1", 8, 1);
        b.push(0b1000_0001);
        let p = decode(&b).unwrap();
        assert_eq!(&p.rgba[..4], &[255, 255, 255, 255]);
        assert_eq!(&p.rgba[4..8], &[0, 0, 0, 255]);
        assert_eq!(&p.rgba[28..32], &[255, 255, 255, 255]);
        // m8: the colour map's white and black
        assert_eq!(cmap2rgb(0), [0, 0, 0]);
        assert_eq!(cmap2rgb(255), [255, 255, 255]);
    }

    #[test]
    fn compressed() {
        // a row of four x8r8g8b8 pixels, all red: one literal pixel, a copy of 12 bytes 4 back
        let mut b = b"compressed\n".to_vec();
        b.extend_from_slice(&header("x8r8g8b8", 4, 1));
        let words = [0x83, 0x00, 0x00, 0xff, 0x00, (12 - 3) << 2, 3];
        b.extend_from_slice(format!("{:>11} {:>11} ", 1, words.len()).as_bytes());
        b.extend_from_slice(&words);
        let p = decode(&b).unwrap();
        assert_eq!(p.width, 4);
        for i in 0..4 {
            assert_eq!(&p.rgba[4 * i..4 * i + 4], &[255, 0, 0, 255]);
        }
    }
}
