#!/bin/bash
# Type one line into a guest that scripts/guest/boot-cli.sh keeps running
# (HOLD=1) and print what the console shows until rc prompts again.
#
# usage: scripts/guest/guest-cmd.sh 'cmd'
#   env:  CLI_DIR     boot-cli's work dir (default: CLI_DIR of dist/paths.env, nfront/cache/cli)
#         PROMPTWAIT  seconds at most to wait for the prompt (300)
ROOT=${PLAN9:-$(git -C "$(dirname "$0")" rev-parse --show-toplevel)}
. "$ROOT/scripts/lib/paths.sh"	# CLI_DIR, CACHE (dist/paths.env: nfront/cache/cli)
pts=$(cat "$CLI_DIR/pts" 2>/dev/null)
[ -n "$pts" ] && [ -w "$pts" ] || { echo "guest-cmd: no running guest (HOLD=1 scripts/guest/boot-cli.sh)" >&2; exit 1; }
log=$CLI_DIR/console.log
start=$(stat -c %s "$log")
# in pieces, as boot-cli: the serial input buffer drops what exceeds
# about 128 bytes at once
l=$1$'\r'
while [ -n "$l" ]; do
	printf '%s' "${l:0:32}" > "$pts"
	l=${l:32}
	sleep 0.2
done
end=$((SECONDS + ${PROMPTWAIT:-300}))
while [ $SECONDS -lt $end ]; do
	sleep 0.5
	out=$(tail -c +$((start + 1)) "$log" | tr -d '\r')
	case ${out##*$'\n'} in
	*'% '|*'# ')	[ "$out" != "${out%%$'\n'*}" ] && break ;;	# a prompt after the echoed line
	esac
done
printf '%s\n' "$out"
[ $SECONDS -lt $end ] || { echo "guest-cmd: no prompt after ${PROMPTWAIT:-300}s" >&2; exit 2; }
