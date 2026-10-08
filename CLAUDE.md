# CLAUDE.md

This file provides guidance to Claude Code (claude.ai/code) when working with code in this repository.

@AGENTS.md

## Branches
- `front` — mirror of upstream `github.com/9front/9front`, branch `front`. Treat as pristine 9front source.
- `gcc` (current work branch) — the gcc cross-build toolchain port described in AGENTS.md. Keep diffs against `front` minimal and reviewable; see the "Porting rules" / "GCC Port Rules" sections in AGENTS.md.

## gcc port status tracking
`MEMORY.md` (repo root) is the running, checked-in todo/status log for the gcc port — per-file fixes done, open items by priority, and accumulated GAS/gcc porting notes. Written in German. Update it when you land or discover something on the gcc branch; check it before starting gcc-port work to avoid redoing already-solved issues.

## Helper scripts
`scripts/` holds the Python scripts that generate the gcc build manifests (`genmanifest.py` for kernel `build.json`, `genklibs.py` for library `klib.json`/`<arch>.json`) plus one-off C source-fixing scripts (`fix2.py`, `fix3.py`, `fixcalls.py`, `fixmemb.py`, `anon.py`). These are the source of truth for manifest generation — regenerate through them rather than hand-editing the JSON manifests.

(`.opencode/` at the repo root is unrelated tooling — config/node_modules for the `opencode` CLI agent — not part of the 9front build.)
