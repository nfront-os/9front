#!/usr/bin/env python3
"""
Find Plan9 anonymous-struct-member collisions that gcc's -fplan9-extensions
rejects but the real Plan9 6c compiler silently allows (shadowing).

Background: Plan9 C lets a struct embed another named type anonymously
(`Lock;` as a member declares an unnamed Lock instance whose own members
are promoted up). gcc's -fplan9-extensions implements the same syntax, but
unlike Plan9 6c it hard-errors ("doppeltes Element"/"duplicate member") if
the same promoted name is reachable through two different paths in one
struct -- including the common, intentional case where an outer struct
declares its own field with the same name as one the embedded type
promotes (Plan9 6c just lets the outer, more specific field win).

This script is a best-effort, regex-based C struct parser (good enough for
Plan9's fairly regular style; it is NOT a real C parser and can both miss
things and produce false positives -- always verify against a real gcc
-fplan9-extensions compile, not just this script's output). It reports,
for each struct definition found under sys/src/9, every member name that
is reachable through more than one distinct source (a direct field, or an
anonymously-embedded type's own promoted-name set).

The actual fix at each reported site is almost always: change the
colliding anonymous embed(s) from `Type;` to `Type Type;` (a named field
of the same type -- this still supports `&x.Type`/`&x->Type` access,
which is how this codebase already reaches embedded Lock/QLock/Rendez/etc.
for locking calls almost everywhere; grep for `.Type\b`/`->Type\b` usage
on the specific struct before renaming, to confirm nothing relies on the
promoted name). Watch for cascading collisions: naming an embedded Lock
inside Rendez (say) gives Rendez itself a promoted field literally named
"Lock", which can then collide anywhere Rendez is itself embedded
alongside a separately-named Lock -- re-run this script after each fix.

Usage:
    python3 scripts/find-anon-struct-collisions.py [path ...]

With no arguments, scans all tracked+untracked .c/.h files under
sys/src/9 (via `git ls-files -co --exclude-standard`).
"""
import re
import subprocess
import sys


def strip_comments(s):
    return re.sub(r"/\*.*?\*/", "", s, flags=re.S)


def find_files(paths):
    if paths:
        return paths
    result = subprocess.run(
        ["git", "-c", "core.ignorecase=false", "ls-files", "-co", "--exclude-standard", "sys/src/9"],
        capture_output=True, text=True,
    )
    return [f for f in result.stdout.splitlines() if f.endswith(".h") or f.endswith(".c")]


def parse_fields(body):
    """Split a struct body into ('anon', TypeName) / ('field', name) entries.
    Skips anything containing '{' (nested struct/union/enum) or '(' (function
    pointers) for safety -- those never register as collision sources, which
    only risks false negatives, not false positives.
    """
    fields = []
    depth = 0
    buf = ""
    stmts = []
    for ch in body:
        if ch == "{":
            depth += 1
        elif ch == "}":
            depth -= 1
        if ch == ";" and depth == 0:
            stmts.append(buf)
            buf = ""
        else:
            buf += ch
    for stmt in stmts:
        s = stmt.strip()
        if not s or "{" in s or "}" in s or "(" in s:
            continue
        m = re.match(r"^([A-Za-z_]\w*)$", s)
        if m:
            fields.append(("anon", m.group(1)))
            continue
        for p in s.split(","):
            p = p.strip()
            if not p:
                continue
            ids = re.findall(r"[A-Za-z_]\w*", p)
            if ids:
                fields.append(("field", ids[-1]))
    return fields


def main():
    paths = sys.argv[1:]
    files = find_files(paths)

    structs = {}
    by_name = {}

    for f in files:
        try:
            text = open(f, errors="replace").read()
        except (IsADirectoryError, FileNotFoundError):
            continue
        text_nc = strip_comments(text)
        for m in re.finditer(r"\bstruct\s*(\w*)\s*\{", text_nc):
            name = m.group(1)
            start = m.end()
            depth = 1
            j = start
            while depth > 0 and j < len(text_nc):
                if text_nc[j] == "{":
                    depth += 1
                elif text_nc[j] == "}":
                    depth -= 1
                j += 1
            body = text_nc[start:j - 1]
            line = text_nc[:m.start()].count("\n") + 1
            key = (f, line, name)
            structs[key] = {"file": f, "line": line, "body": body, "name": name}
            if name:
                by_name.setdefault(name, []).append(key)

    direct = {key: parse_fields(info["body"]) for key, info in structs.items()}

    def resolve_anon_type(typename):
        keys = by_name.get(typename)
        return keys[0] if keys else None

    def promoted(key, seen=None):
        if seen is None:
            seen = set()
        if key in seen:
            return set()
        seen = seen | {key}
        names = set()
        for kind, n in direct.get(key, []):
            if kind == "field":
                names.add(n)
            elif kind == "anon":
                tk = resolve_anon_type(n)
                if tk:
                    names |= promoted(tk, seen)
        return names

    collisions = []
    for key, info in structs.items():
        sources = []
        for kind, n in direct[key]:
            if kind == "field":
                sources.append((f"field:{n}", {n}))
            elif kind == "anon":
                tk = resolve_anon_type(n)
                if tk:
                    sources.append((f"embed:{n}", promoted(tk)))
        seen_names = {}
        conflict_names = set()
        for label, names in sources:
            for nm in names:
                if nm in seen_names and seen_names[nm] != label:
                    conflict_names.add(nm)
                seen_names.setdefault(nm, label)
        if conflict_names:
            collisions.append((key, info["file"], info["line"], conflict_names, sources))

    if not collisions:
        print("no collisions found (by this heuristic scan)")
        return

    for key, f, line, names, sources in sorted(collisions, key=lambda c: (c[1], c[2])):
        print(f"{f}:{line}: collisions={sorted(names)}")
        for label, nms in sources:
            overlap = nms & names
            if overlap:
                print(f"    {label}: {sorted(overlap)}")
    print()
    print("TOTAL:", len(collisions))


if __name__ == "__main__":
    main()
