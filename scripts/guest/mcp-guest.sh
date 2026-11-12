#!/bin/bash
# A guest (scripts/guest/boot-cli.sh, HOLD=1) that runs ../9front-mcp's server,
# reached from the host over HTTP: a second NIC whose QEMU user network
# forwards 127.0.0.1:$PORT to the guest's port 8090. scripts/guest/mcp.py then
# runs commands there and reads files: one HTTP request each, much faster than
# typing lines on the serial line (guest-cmd.sh). The server runs with -disks
# and no deny list: in QEMU everything may be done.
#
# usage: scripts/guest/mcp-guest.sh ['first command' ...]
#   Runs until killed (kill it by its PID; it takes QEMU along). The commands
#   are typed before the server starts (e.g. aux/virtiogpu).
#   env:  PORT        host port (18091)
#         MCPTOKEN    the token (mcpguest)
#         MCP_SRC     the server's source (../9front-mcp)
#         GO          a Go that builds plan9/amd64 (../9front-go/go/bin/go)
#         QEMU_EXTRA, SMP, MEM, ... as boot-cli.sh (QEMU_EXTRA gets the NIC added)
set -e
ROOT=$(git -C "$(dirname "$0")" rev-parse --show-toplevel)
. "$ROOT/scripts/lib/paths.sh"	# GCC_DIR, RUST_DIR, GO_DIR, MCP_DIR, DISTFILES (the distribution's paths.env)
CLI_DIR=${CLI_DIR:-${XDG_CACHE_HOME:-$HOME/.cache}/9front-cli}
MCP_SRC=${MCP_SRC:-$MCP_DIR}
GO=${GO:-$GO_DIR/go/bin/go}
PORT=${PORT:-18091}
TOKEN=${MCPTOKEN:-mcpguest}
A=${objtype:-amd64}

ss -ltnH "sport = :$PORT" | grep -q . && { echo "mcp-guest: port $PORT is taken (another guest?)" >&2; exit 1; }
# the root's additions: the caller's (UFS_EXTRA, e.g. tests/gpu/root) and the server
X=$CLI_DIR/mcp/root
rm -rf "$X"
mkdir -p "$X/tests"
# hard links where they can be (the same file system: GBs of test programs), else a copy
[ -z "$UFS_EXTRA" ] || cp -al "$UFS_EXTRA"/. "$X/" 2>/dev/null || cp -r "$UFS_EXTRA"/. "$X/"
GOOS=plan9 GOARCH=$A CGO_ENABLED=0 GOCACHE=$CLI_DIR/mcp/gocache \
	"$GO" build -C "$MCP_SRC" -o "$X/tests/9front-mcp" .
rm -f "$CLI_DIR/mcp.sid"

export HOLD=1 UFS_EXTRA=$X SMP=${SMP:-4} MEM=${MEM:-2048}
# QMP for a guest that hangs (scripts/debug/qemu-ctl.py $CLI_DIR hmp ...), unless boot-cli makes one (SCREENSHOT)
qmp=
[ -n "$SCREENSHOT" ] || { rm -f "$CLI_DIR/qmp.sock"; qmp="-qmp unix:$CLI_DIR/qmp.sock,server,nowait"; }
export QEMU_EXTRA="$QEMU_EXTRA $qmp -netdev user,id=mcpnet,net=10.0.3.0/24,hostfwd=tcp:127.0.0.1:$PORT-10.0.3.15:8090 -device e1000,netdev=mcpnet"
exec bash "$ROOT/scripts/guest/boot-cli.sh" "$@" \
	"bind -a '#l1' /net; ip/ipconfig ether /net/ether1 10.0.3.15 255.255.255.0" \
	"mkdir -p /tmp/mcp; /tests/9front-mcp -http 'tcp!*!8090' -token $TOKEN -dir /tmp/mcp -disks >[2]/tmp/mcp.log </dev/null &" \
	"sleep 1; cat /tmp/mcp.log"
