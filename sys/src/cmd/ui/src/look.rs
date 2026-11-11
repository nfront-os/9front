// Copyright 2019 the Xilem Authors and the Druid Authors
// SPDX-License-Identifier: Apache-2.0
//
// ui's look: Masonry's default properties (masonry/src/theme.rs of xilem rev
// dc49687, Apache-2.0) with the colours of the desktop's theme (desk:
// /dev/theme, wsys(4)) instead of Masonry's dark ones: the window's background
// and text, the current border as accent and focus, and greys mixed between
// background and text for surfaces, borders, hover and active (Palette).

use masonry::core::{DefaultProperties, PropertySet, PropertyStack, Selector};
use masonry::layout::{AsUnit, Length};
use masonry::peniko::Color;

// as Masonry's theme: glob imports
use masonry::properties::*;
use masonry::widgets::*;

pub const BORDER_WIDTH: Length = Length::const_px(1.);
pub const DEFAULT_GAP: Length = Length::const_px(10.0);

/// the colours Masonry's default properties take, from a theme
pub struct Palette {
    pub background: Color, // the window's (the frame is cleared with it)
    pub text: Color,
    pub textback: Color,   // a text box's inside
    pub accent: Color,     // what is on: a switch, a progress bar, a slider's track
    pub focus: Color,      // the focused widget's border
    pub disabled: Color,   // text of what is disabled
    pub placeholder: Color,
    pub dim: Color,
    pub off: Color,        // the background of what is disabled
    pub z900: Color,       // Masonry's greys, darkest to lightest on its dark theme: here from
    pub z800: Color,       // the background towards the text (a surface, borders, hover)
    pub z700: Color,
    pub z600: Color,
    pub z500: Color,
}

fn rgb(c: u32) -> Color {
    Color::from_rgb8((c >> 16) as u8, (c >> 8) as u8, c as u8)
}

/// a of the way from b to t (0: b, 1: t)
fn mix(b: u32, t: u32, a: f32) -> Color {
    let ch = |s: u32| {
        let (x, y) = (((b >> s) & 0xFF) as f32, ((t >> s) & 0xFF) as f32);
        (x + (y - x) * a).round() as u8
    };
    Color::from_rgb8(ch(16), ch(8), ch(0))
}

impl Palette {
    pub fn from_theme(t: &desk::theme::Theme) -> Palette {
        let (b, x) = (t.window, t.text);
        Palette {
            background: rgb(b),
            text: rgb(x),
            textback: mix(b, x, 0.03),
            accent: rgb(t.border),
            focus: rgb(t.border),
            disabled: mix(b, x, 0.45),
            placeholder: mix(b, x, 0.5),
            dim: mix(b, x, 0.55),
            off: mix(b, x, 0.12),
            z900: mix(b, x, 0.04),
            z800: mix(b, x, 0.07),
            z700: mix(b, x, 0.22),
            z600: mix(b, x, 0.30),
            z500: mix(b, x, 0.40),
        }
    }
}

pub fn property_set(p: &Palette) -> DefaultProperties {
    let mut properties = DefaultProperties::new();

    // Badge
    properties.insert::<Badge, _>(Padding::from_vh(3.px(), 5.px()));
    properties.insert::<Badge, _>(CornerRadius { radius: 999.px() });
    properties.insert::<Badge, _>(BorderWidth { width: 0.px() });
    properties.insert::<Badge, _>(Background::Color(p.accent));
    properties.insert::<Badge, _>(BorderColor { color: p.z700 });
    {
        let mut stack = PropertyStack::new();
        stack.push_layer(
            Selector::new().with_disabled(true),
            Background::Color(p.z800),
        );
        properties.insert_stack::<Badge>(stack);
    }

    // Button
    properties.insert::<Button, _>(Padding::from_vh(6.px(), 16.px()));
    properties.insert::<Button, _>(CornerRadius { radius: 6.px() });
    properties.insert::<Button, _>(BorderWidth {
        width: BORDER_WIDTH,
    });
    properties.insert::<Button, _>(Background::Color(p.z800));
    properties.insert::<Button, _>(BorderColor { color: p.z700 });
    {
        let mut stack = PropertyStack::new();
        stack.push_layer(
            Selector::new().with_hovered(true),
            BorderColor { color: p.z500 },
        );
        stack.push_layer(
            Selector::new().with_focused(true),
            BorderColor { color: p.focus },
        );
        stack.push_layer(
            Selector::new().with_active(true),
            Background::Color(p.z700),
        );
        stack.push_layer(
            Selector::new().with_disabled(true),
            Background::Color(p.off),
        );
        properties.insert_stack::<Button>(stack);
    }

    // Checkbox
    properties.insert::<Checkbox, _>(CornerRadius { radius: 4.px() });
    properties.insert::<Checkbox, _>(BorderWidth {
        width: BORDER_WIDTH,
    });
    properties.insert::<Checkbox, _>(Background::Color(p.z800));
    properties.insert::<Checkbox, _>(BorderColor { color: p.z700 });
    properties.insert::<Checkbox, _>(CheckmarkStrokeWidth { width: 2.0 });
    properties.insert::<Checkbox, _>(CheckmarkColor { color: p.text });
    {
        let mut stack = PropertyStack::new();
        stack.push_layer(
            Selector::new().with_hovered(true),
            BorderColor { color: p.z500 },
        );
        stack.push_layer(
            Selector::new().with_focused(true),
            BorderColor { color: p.focus },
        );
        stack.push_layer(
            Selector::new().with_active(true),
            Background::Color(p.z700),
        );
        stack.push_layer(
            Selector::new().with_disabled(true),
            PropertySet::new()
                .with(Background::Color(p.off))
                .with(CheckmarkColor {
                    color: p.disabled,
                }),
        );
        properties.insert_stack::<Checkbox>(stack);
    }

    // DisclosureButton
    properties.insert::<DisclosureButton, _>(ContentColor::new(p.dim));
    properties.insert::<DisclosureButton, _>(Dimensions::fixed(
        Length::const_px(16.),
        Length::const_px(16.),
    ));
    properties.insert::<DisclosureButton, _>(Padding::all(4.px()));

    // Divider
    properties.insert::<Divider, _>(ContentColor::new(p.z500));

    // Switch
    properties.insert::<Switch, _>(CornerRadius { radius: 10.px() }); // Full pill shape
    properties.insert::<Switch, _>(BorderWidth {
        width: BORDER_WIDTH,
    });
    properties.insert::<Switch, _>(Background::Color(p.z700));
    properties.insert::<Switch, _>(BorderColor { color: p.z700 });
    properties.insert::<Switch, _>(ThumbColor(p.text));
    properties.insert::<Switch, _>(ThumbRadius(8.px()));
    properties.insert::<Switch, _>(TrackThickness(20.px()));
    {
        let mut stack = PropertyStack::new();
        stack.push_layer(
            Selector::new().with_hovered(true),
            BorderColor { color: p.z500 },
        );
        stack.push_layer(
            Selector::new().with_focused(true),
            BorderColor { color: p.focus },
        );
        stack.push_layer(
            Selector::classes(&["#toggled"]),
            Background::Color(p.accent),
        );
        stack.push_layer(
            Selector::new().with_active(true),
            Background::Color(p.z600),
        );
        stack.push_layer(
            Selector::new().with_disabled(true),
            Background::Color(p.off),
        );
        properties.insert_stack::<Switch>(stack);
    }

    // FIXME
    use masonry::widgets::Selector as SelectorButton;

    // Selector
    properties.insert::<SelectorButton, _>(Padding::from_vh(6.px(), 16.px()));
    properties.insert::<SelectorButton, _>(CornerRadius { radius: 2.px() });
    properties.insert::<SelectorButton, _>(BorderWidth {
        width: BORDER_WIDTH,
    });

    properties.insert::<SelectorButton, _>(Background::Color(p.z800));
    properties.insert::<SelectorButton, _>(BorderColor { color: p.z700 });
    {
        let mut stack = PropertyStack::new();
        stack.push_layer(
            Selector::new().with_active(true),
            Background::Color(p.z700),
        );
        stack.push_layer(
            Selector::new().with_disabled(true),
            Background::Color(p.off),
        );
        stack.push_layer(
            Selector::new().with_hovered(true),
            BorderColor { color: p.z500 },
        );
        stack.push_layer(
            Selector::new().with_focused(true),
            BorderColor { color: p.focus },
        );
        properties.insert_stack::<SelectorButton>(stack);
    }

    // SelectorItem
    properties.insert::<SelectorItem, _>(Padding::from_vh(6.px(), 16.px()));
    properties.insert::<SelectorItem, _>(Background::Color(p.z900));
    {
        let mut stack = PropertyStack::new();
        stack.push_layer(
            Selector::new().with_active(true),
            Background::Color(p.z800),
        );
        stack.push_layer(
            Selector::new().with_disabled(true),
            Background::Color(p.off),
        );
        properties.insert_stack::<SelectorItem>(stack);
    }

    // Flex
    properties.insert::<Flex, _>(Gap::new(DEFAULT_GAP));

    // Grid
    properties.insert::<Grid, _>(Gap::ZERO);

    // TextInput
    properties.insert::<TextInput, _>(Padding::from_vh(6.px(), 12.px()));
    properties.insert::<TextInput, _>(CornerRadius { radius: 4.px() });
    properties.insert::<TextInput, _>(BorderWidth {
        width: BORDER_WIDTH,
    });
    properties.insert::<TextInput, _>(BorderColor { color: p.z600 });
    properties.insert::<TextInput, _>(PlaceholderColor::new(p.placeholder));
    properties.insert::<TextInput, _>(CaretColor { color: p.text });
    properties.insert::<TextInput, _>(SelectionColor {
        color: p.accent,
    });
    properties.insert::<TextInput, _>(Background::Color(p.textback));
    {
        let mut stack = PropertyStack::new();
        stack.push_layer(
            Selector::classes(&["#unfocused"]),
            SelectionColor {
                color: p.disabled,
            },
        );
        stack.push_layer(
            Selector::new().with_focused(true),
            BorderColor { color: p.focus },
        );
        stack.push_layer(
            Selector::new().with_disabled(true),
            Background::Color(p.textback),
        );
        properties.insert_stack::<TextInput>(stack);
    }

    // TextArea
    properties.insert::<TextArea<false>, _>(ContentColor::new(p.text));
    properties.insert::<TextArea<false>, _>(CaretColor { color: p.text });
    properties.insert::<TextArea<false>, _>(SelectionColor {
        color: p.accent,
    });
    {
        let mut stack = PropertyStack::new();
        stack.push_layer(
            Selector::new().with_disabled(true),
            ContentColor::new(p.disabled),
        );
        properties.insert_stack::<TextArea<false>>(stack);
    }
    properties.insert::<TextArea<true>, _>(ContentColor::new(p.text));
    properties.insert::<TextArea<true>, _>(CaretColor { color: p.text });
    properties.insert::<TextArea<true>, _>(SelectionColor {
        color: p.accent,
    });
    {
        let mut stack = PropertyStack::new();
        stack.push_layer(
            Selector::new().with_disabled(true),
            ContentColor::new(p.disabled),
        );
        properties.insert_stack::<TextArea<true>>(stack);
    }

    // Prose
    properties.insert::<Prose, _>(CaretColor { color: p.text });
    properties.insert::<Prose, _>(ContentColor::new(p.text));
    properties.insert::<Prose, _>(SelectionColor {
        color: p.accent,
    });
    {
        let mut stack = PropertyStack::new();
        stack.push_layer(
            Selector::classes(&["#unfocused"]),
            SelectionColor {
                color: p.disabled,
            },
        );
        properties.insert_stack::<Prose>(stack);
    }

    // Label
    properties.insert::<Label, _>(ContentColor::new(p.text));
    {
        let mut stack = PropertyStack::new();
        stack.push_layer(
            Selector::new().with_disabled(true),
            ContentColor::new(p.disabled),
        );
        properties.insert_stack::<Label>(stack);
    }

    // ProgressBar
    properties.insert::<ProgressBar, _>(CornerRadius { radius: 2.px() });
    properties.insert::<ProgressBar, _>(BorderWidth {
        width: BORDER_WIDTH,
    });

    properties.insert::<ProgressBar, _>(Background::Color(p.z900));
    properties.insert::<ProgressBar, _>(BorderColor { color: p.z800 });
    properties.insert::<ProgressBar, _>(BarColor(p.accent));

    // RadioButton
    properties.insert::<RadioButton, _>(BorderWidth {
        width: BORDER_WIDTH,
    });

    properties.insert::<RadioButton, _>(Background::Color(p.z800));
    properties.insert::<RadioButton, _>(BorderColor { color: p.z700 });
    properties.insert::<RadioButton, _>(CheckmarkColor { color: p.text });
    {
        let mut stack = PropertyStack::new();
        stack.push_layer(
            Selector::new().with_active(true),
            Background::Color(p.z700),
        );
        stack.push_layer(
            Selector::new().with_hovered(true),
            BorderColor { color: p.z500 },
        );
        stack.push_layer(
            Selector::new().with_focused(true),
            BorderColor { color: p.focus },
        );
        stack.push_layer(
            Selector::new().with_disabled(true),
            (
                CheckmarkColor::new(p.disabled),
                Background::Color(p.off),
            ),
        );
        properties.insert_stack::<RadioButton>(stack);
    }

    // Slider
    properties.insert::<Slider, _>(TrackThickness(4.px()));
    properties.insert::<Slider, _>(TrackColor {
        active: p.accent,
        inactive: p.z800,
    });
    properties.insert::<Slider, _>(ThumbColor(p.text));
    properties.insert::<Slider, _>(ThumbRadius(7.px()));
    {
        let mut stack = PropertyStack::new();
        stack.push_layer(
            Selector::new().with_hovered(true),
            BorderColor { color: p.z500 },
        );
        stack.push_layer(
            Selector::new().with_focused(true),
            BorderColor { color: p.focus },
        );
        properties.insert_stack::<Slider>(stack);
    }

    // Spinner
    properties.insert::<Spinner, _>(TrackColor {
        active: p.text,
        inactive: Color::TRANSPARENT,
    });
    properties.insert::<Spinner, _>(TrackThickness(8.px()));
    properties.insert::<Spinner, _>(AnimationDuration { seconds: 1.2 });

    // StepInput
    default_step_input_style::<u8>(p, &mut properties);
    default_step_input_style::<i8>(p, &mut properties);
    default_step_input_style::<u16>(p, &mut properties);
    default_step_input_style::<i16>(p, &mut properties);
    default_step_input_style::<u32>(p, &mut properties);
    default_step_input_style::<i32>(p, &mut properties);
    default_step_input_style::<u64>(p, &mut properties);
    default_step_input_style::<i64>(p, &mut properties);
    default_step_input_style::<usize>(p, &mut properties);
    default_step_input_style::<isize>(p, &mut properties);
    default_step_input_style::<f32>(p, &mut properties);
    default_step_input_style::<f64>(p, &mut properties);

    properties
}

fn default_step_input_style<T: Steppable>(p: &Palette, properties: &mut DefaultProperties) {
    properties.insert::<StepInput<T>, _>(Padding::from_vh(6.px(), 0.px()));
    properties.insert::<StepInput<T>, _>(CornerRadius { radius: 6.px() });
    properties.insert::<StepInput<T>, _>(BorderWidth {
        width: BORDER_WIDTH,
    });

    properties.insert::<StepInput<T>, _>(ContentColor::new(p.text));
    properties.insert::<StepInput<T>, _>(Background::Color(p.z800));
    properties.insert::<StepInput<T>, _>(BorderColor { color: p.z700 });

    {
        let mut stack = PropertyStack::new();
        stack.push_layer(
            Selector::new().with_disabled(true),
            (
                ContentColor::new(p.disabled),
                Background::Color(p.off),
            ),
        );
        stack.push_layer(
            Selector::new().with_hovered(true),
            BorderColor { color: p.z500 },
        );
        stack.push_layer(
            Selector::new().with_focused(true),
            BorderColor { color: p.focus },
        );

        properties.insert_stack::<StepInput<T>>(stack);
    }
}

