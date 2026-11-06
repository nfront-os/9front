// SPDX-License-Identifier: MIT
//
// uitodo: a list of things to do in a window of ui(4). A text box takes a
// new one (enter or the button), each has a check box and a button to take
// it away; the list can be written to a file (-f file: read at the start,
// written after each change), a line each: "0 text" or "1 text".

use uiview::*;

struct Todo {
    text: String,
    done: bool,
}

struct State {
    next: String,
    todos: Vec<Todo>,
    file: Option<String>,
}

impl State {
    fn add(&mut self) {
        let t = self.next.trim().to_string();
        if !t.is_empty() {
            self.todos.push(Todo { text: t, done: false });
        }
        self.next.clear();
        self.save();
    }

    fn save(&self) {
        if let Some(f) = &self.file {
            let s: String = self.todos.iter().map(|t| format!("{} {}\n", t.done as u8, t.text)).collect();
            if let Err(e) = std::fs::write(f, s) {
                eprintln!("uitodo: {f}: {e}");
            }
        }
    }
}

fn item(i: usize, t: &Todo) -> impl UiView<State> + use<> {
    row((
        checkbox(&t.text, t.done, move |s: &mut State, d| {
            if let Some(t) = s.todos.get_mut(i) {
                t.done = d;
            }
            s.save();
        })
        .flex(1.0),
        button("away", move |s: &mut State| {
            if i < s.todos.len() {
                s.todos.remove(i);
            }
            s.save();
        }),
    ))
    .gap(8.0)
    .align("center")
}

fn app(s: &mut State) -> impl UiView<State> + use<> {
    let left = s.todos.iter().filter(|t| !t.done).count();
    let items: Vec<_> = s.todos.iter().enumerate().map(|(i, t)| item(i, t)).collect();
    column((
        row((
            textbox(&s.next, |s: &mut State, t| s.next = t)
                .placeholder("something to do")
                .on_enter(|s: &mut State, t| {
                    s.next = t;
                    s.add();
                })
                .flex(1.0),
            button("add", |s: &mut State| s.add()),
        ))
        .gap(8.0),
        column(items).gap(4.0),
        label(format!("{left} of {} to do", s.todos.len())),
    ))
    .gap(8.0)
}

fn main() {
    let mut file = None;
    let mut args = std::env::args().skip(1);
    while let Some(a) = args.next() {
        match a.as_str() {
            "-f" => file = args.next(),
            _ => {
                eprintln!("usage: uitodo [-f file]");
                std::process::exit(1);
            }
        }
    }
    let mut todos = Vec::new();
    if let Some(f) = &file {
        if let Ok(s) = std::fs::read_to_string(f) {
            for l in s.lines() {
                if let Some((d, t)) = l.split_once(' ') {
                    todos.push(Todo { text: t.to_string(), done: d == "1" });
                }
            }
        }
    }
    let st = State { next: String::new(), todos, file };
    if let Err(e) = uiview::run("uitodo", st, app) {
        eprintln!("uitodo: {e}");
        std::process::exit(1);
    }
}
