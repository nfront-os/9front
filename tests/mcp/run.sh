#!/bin/bash
# mcp: the MCP server on the install CD (/$objtype/bin/9front-mcp, tools/mcp;
# dist/bin/mkcd -mcp, stage-dist.sh MCP=1): initialize, an rc command through it, its
# deny list refusing fshalt (tests/mcp/mcp.py).
# Skips a CD without it. Uses ISO, else the last CD scripts/guest/mkiso.sh made.
. "$(dirname "$0")/../lib.sh"
IMG=${ISO:-$CLI_DIR/9front.$objtype.iso}
[ -f "$IMG" ] || { skip "mcp" "no CD $IMG, run dist/bin/mkcd -mcp ... or set ISO"; done_tests; exit; }

python3 "$ROOT/tests/mcp/mcp.py" "$IMG" "$TMPD/vm" $objtype > "$TMPD/results" 2>&1
while read -r r name; do
	case $r in
	ok)	pass "$name" ;;
	fail)	fail "$name" ;;
	skip)	skip "mcp" "$name" ;;
	*)	fail "mcp.py: $r $name" ;;
	esac
done < "$TMPD/results"
grep -q . "$TMPD/results" || fail "mcp.py printed nothing"
done_tests
