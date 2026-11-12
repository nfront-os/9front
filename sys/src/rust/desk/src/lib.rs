// SPDX-License-Identifier: MIT
//
// desk: what the desktop's programs share (../9front-ui PLAN stage 7).
// - theme: the desktop's colours and fonts as wsys(4)'s file theme has them
// - wins: the windows of /dev/wsys (winwatch's way), a command run by rc
// - text (feature text): a proportional TrueType font drawn into pixels

pub mod theme;
pub mod wins;
#[cfg(feature = "text")]
pub mod text;
