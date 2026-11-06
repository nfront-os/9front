// SPDX-License-Identifier: MIT
//
// uicount: a counter in a window of ui(4), the smallest program of uiview.

use uiview::*;

fn app(n: &mut i64) -> impl UiView<i64> + use<> {
    column((
        label(format!("count {n}")),
        row((
            button("add one", |n: &mut i64| *n += 1),
            button("take one", |n: &mut i64| *n -= 1),
            button("none", |n: &mut i64| *n = 0),
        ))
        .gap(8.0),
    ))
    .gap(8.0)
}

fn main() {
    if let Err(e) = uiview::run("uicount", 0, app) {
        eprintln!("uicount: {e}");
        std::process::exit(1);
    }
}
