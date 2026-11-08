// SPDX-License-Identifier: MIT
//
// uitick: a count a timer advances (uiview's Proxy::every), with a button
// that stops and starts it: the views follow changes of the state that come
// from another thread, not only the user's.

use std::time::Duration;
use uiview::*;

struct Tick {
    n: u64,
    running: bool,
}

fn app(t: &mut Tick) -> impl UiView<Tick> + use<> {
    column((
        label(format!("tick {}", t.n)),
        button(if t.running { "stop" } else { "start" }, |t: &mut Tick| t.running = !t.running),
    ))
    .gap(8.0)
}

fn main() {
    let start = |p: Proxy<Tick>| {
        p.every(Duration::from_millis(100), |t: &mut Tick| {
            if t.running {
                t.n += 1;
            }
        })
    };
    if let Err(e) = uiview::run_with("uitick", Tick { n: 0, running: true }, start, app) {
        eprintln!("uitick: {e}");
        std::process::exit(1);
    }
}
