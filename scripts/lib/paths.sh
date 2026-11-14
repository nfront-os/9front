# scripts/lib/paths.sh: where nfront's other parts are (the toolchains,
# the other worktrees, the archives).  Sourced by the scripts after they
# set ROOT (the tree); sets each variable of the distribution's paths.env
# that the environment does not set already, as an absolute path.
# paths.env: $NFRONT_PATHS, else ROOT/../dist/paths.env (the umbrella's
# layout), else ROOT/../9front-distribution/paths.env (the old one);
# without it the old defaults (../9front-gcc, ...; the caches in
# $XDG_CACHE_HOME/nfront).
__nfront_paths(){
	local f top line k v
	for f in "$NFRONT_PATHS" "$ROOT/../dist/paths.env" "$ROOT/../9front-distribution/paths.env"; do
		[ -n "$f" ] && [ -f "$f" ] && break
		f=
	done
	if [ -n "$f" ]; then
		top=$(cd -P "$(dirname "$f")/.." && pwd -P)	# physical: through a symlink of the old layout too
		while IFS= read -r line; do
			case "$line" in ''|'#'*) continue;; esac
			k=${line%%=*}
			v=${line#*=}
			case "$v" in /*) ;; *) v=$top/$v;; esac
			v=$(realpath -m "$v")
			[ -z "${!k}" ] && export "$k=$v"
		done < "$f"
	fi
	# the defaults of the old layout, for what paths.env does not name
	top=$(cd "$ROOT/.." && pwd)
	: "${GCC_DIR:=$top/9front-gcc}" "${RUST_DIR:=$top/9front-rust}" "${GO_DIR:=$top/9front-go}"
	: "${MCP_DIR:=$top/9front-mcp}" "${DISTFILES:=$top/9front-ui/src}" "${TREE_GCC:=$top/9front-gccwt}"
	: "${PLAN9PORT:=$top/plan9port}"
	: "${CACHE:=${XDG_CACHE_HOME:-$HOME/.cache}/nfront}"
	: "${CLI_DIR:=$CACHE/cli}"
	: "${REPLICA_ROOT:=$CACHE/replica}"
	export GCC_DIR RUST_DIR GO_DIR MCP_DIR DISTFILES TREE_GCC PLAN9PORT CACHE CLI_DIR REPLICA_ROOT
}
__nfront_paths
