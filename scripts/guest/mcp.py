#!/usr/bin/env python3
"""Talk to 9front-mcp in a guest (scripts/guest/mcp-guest.sh) over HTTP.

usage: scripts/guest/mcp.py rc 'command' [timeout]   (command '-': from stdin)
       scripts/guest/mcp.py read path
       scripts/guest/mcp.py write path < content
       scripts/guest/mcp.py wait [seconds]           until the server answers
Prints the tool's text; exits 1 when the tool reports an error (rc: a non-empty
status). env: MCPURL (http://127.0.0.1:$PORT/mcp, PORT 18091), MCPTOKEN
(mcpguest); the session id is kept in $CLI_DIR/mcp.sid.
"""
import json, os, sys, time, urllib.error, urllib.request

URL = os.environ.get("MCPURL", "http://127.0.0.1:%s/mcp" % os.environ.get("PORT", "18091"))
TOKEN = os.environ.get("MCPTOKEN", "mcpguest")
CLI_DIR = os.environ.get("CLI_DIR") or os.path.join(
    os.environ.get("XDG_CACHE_HOME") or os.path.expanduser("~/.cache"), "9front-cli")
SID = os.path.join(CLI_DIR, "mcp.sid")


def post(body, sid=None, timeout=60):
    h = {"Authorization": "Bearer " + TOKEN, "Content-Type": "application/json",
         "Accept": "application/json, text/event-stream"}
    if sid:
        h["Mcp-Session-Id"] = sid
    r = urllib.request.urlopen(urllib.request.Request(URL, json.dumps(body).encode(), h), timeout=timeout)
    data = r.read()
    return r.headers.get("Mcp-Session-Id"), (json.loads(data) if data.strip() else None)


def init():
    sid, _ = post({"jsonrpc": "2.0", "id": 1, "method": "initialize", "params": {
        "protocolVersion": "2025-06-18", "capabilities": {},
        "clientInfo": {"name": "mcp.py", "version": "0"}}})
    post({"jsonrpc": "2.0", "method": "notifications/initialized"}, sid)
    with open(SID, "w") as f:
        f.write(sid)
    return sid


def session():
    if os.path.exists(SID):
        return open(SID).read().strip()
    return init()


def call(name, args, wait):
    sid = session()
    body = {"jsonrpc": "2.0", "id": 2, "method": "tools/call", "params": {"name": name, "arguments": args}}
    try:
        _, ans = post(body, sid, wait)
    except urllib.error.HTTPError as e:
        if e.code not in (400, 404):  # a session of an earlier server
            raise
        _, ans = post(body, init(), wait)
    return ans


def main():
    if len(sys.argv) < 2:
        sys.exit(__doc__)
    cmd = sys.argv[1]
    if cmd == "wait":
        end = time.time() + (int(sys.argv[2]) if len(sys.argv) > 2 else 300)
        while time.time() < end:
            try:
                init()
                return
            except (OSError, urllib.error.URLError):
                time.sleep(2)
        sys.exit("mcp.py: no server at " + URL)
    if cmd == "rc":
        t = int(sys.argv[3]) if len(sys.argv) > 3 else 120
        c = sys.stdin.read() if sys.argv[2] == "-" else sys.argv[2]
        ans = call("rc", {"command": c, "timeout": t}, t + 30)
    elif cmd == "read":
        ans = call("read", {"path": sys.argv[2]}, 60)
    elif cmd == "write":
        ans = call("write", {"path": sys.argv[2], "content": sys.stdin.read()}, 60)
    else:
        sys.exit(__doc__)
    if "error" in ans:
        sys.exit("mcp.py: %s" % ans["error"])
    res = ans.get("result") or {}
    text = "".join(c.get("text", "") for c in res.get("content", []))
    sys.stdout.write(text if text == "" or text.endswith("\n") else text + "\n")
    # rc: the server ends the output with "status: ..." when the command failed
    last = text.rstrip("\n").rsplit("\n", 1)[-1]
    sys.exit(1 if res.get("isError") or (cmd == "rc" and last.startswith("status: ")) else 0)


main()
