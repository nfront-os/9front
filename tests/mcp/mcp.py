#!/usr/bin/env python3
"""
The MCP server on the install CD (/$objtype/bin/9front-mcp from tools/mcp,
dist/bin/mkcd -mcp; stage-dist.sh MCP=1): boot to rio, then over the serial rc
9front-mcp -stdio with the CD's deny list (/sys/lib/mcp/deny) gets initialize,
a tools/call of rc that runs a command, and one the deny list refuses (fshalt).

usage: tests/mcp/mcp.py iso workdir objtype
   prints one line per check: ok|fail|skip <name>
"""
import os, re, sys, time
sys.path.insert(0, os.path.join(os.path.dirname(os.path.abspath(__file__)), '..'))
from vm import VM

iso, work, objtype = sys.argv[1:4]
vm = VM(iso, work, objtype)
def result(ok, name):
	print('%s %s' % ('ok' if ok else 'fail', name), flush=True)
	return ok

def run(line, marker, secs):
	"""line on the serial rc, then echo marker; output until it shows"""
	out = vm.serial(line + "; echo %s $status" % marker, 1)
	end = time.time() + secs
	while marker not in out and time.time() < end:
		out += vm.read(2)
	return out.replace('\r', '')

REQ = [
	'{"jsonrpc":"2.0","id":1,"method":"initialize","params":{"protocolVersion":"2025-06-18","capabilities":{},"clientInfo":{"name":"cdtest","version":"1"}}}',
	'{"jsonrpc":"2.0","method":"notifications/initialized"}',
	'{"jsonrpc":"2.0","id":2,"method":"tools/call","params":{"name":"rc","arguments":{"command":"echo mcp-rc-says $sysname"}}}',
	'{"jsonrpc":"2.0","id":3,"method":"tools/call","params":{"name":"rc","arguments":{"command":"fshalt"}}}',
]

try:
	if not result(vm.boot(), 'boot to rio, serial rc'):
		sys.exit(0)
	out = run('test -x /%s/bin/9front-mcp && echo has-mcp' % objtype, 'has-done', 10)
	if 'has-mcp\n' not in out:
		print('skip no /%s/bin/9front-mcp on the CD (dist/bin/mkcd -mcp)' % objtype, flush=True)
		sys.exit(0)
	out = run('test -s /sys/lib/mcp/deny && echo has-deny', 'deny-done', 10)
	result('has-deny\n' in out, 'its deny list /sys/lib/mcp/deny')
	op = '>'
	for r in REQ:
		vm.serial("echo '%s' %s /tmp/mcp.req" % (r, op), 1)
		op = '>>'
	out = run('9front-mcp -stdio -deny /sys/lib/mcp/deny -dir /tmp < /tmp/mcp.req > /tmp/mcp.out; cat /tmp/mcp.out', 'mcp-done', 60)
	result('"serverInfo"' in out and '"id":1' in out, 'initialize: the server answers')
	result(re.search(r'mcp-rc-says [A-Za-z0-9]', out) is not None and '"id":2' in out, 'tools/call rc runs a command ($sysname expanded)')
	result('"id":3' in out and 'deny list' in out, 'the deny list refuses fshalt')
finally:
	vm.close()
