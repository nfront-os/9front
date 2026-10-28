"""i915-regs.py [LINUXTREE]: aux/intelgpu's register excerpts.

Copies the definitions of Linux's i915 that aux/intelgpu
(sys/src/cmd/aux/gpu/intel) uses, and those they use in turn, into
excerpts of the headers they come from (UPSTREAM's lines with *),
verbatim, in Linux's order, each with its header's notice.  Run it after
the code uses a new name, or for a new Linux tree (default
~/Devel/linux-6.6).  linux.h stands in for the kernel's own headers
(SHIM below)."""
import re,os,sys,glob
D=os.path.join(os.path.dirname(os.path.abspath(__file__)),'../../sys/src/cmd/aux/gpu/intel')
LX=os.path.expanduser(sys.argv[1] if len(sys.argv)>1 else '~/Devel/linux-6.6')
# Linux header -> our excerpt, from UPSTREAM's * lines (in its order:
# the priority for a name defined twice)
SRC=[]
for l in open(os.path.join(D,'UPSTREAM')):
    f=l.rstrip('\n').split('\t')
    if len(f)==3 and f[2]=='*':
        SRC.append((f[1],f[0]))
# what linux.h gives instead (they need the kernel's headers there)
SHIM=set('''_MMIO MCR_REG REG_BIT REG_BIT8 REG_GENMASK REG_GENMASK8 REG_FIELD_PREP REG_FIELD_PREP8 REG_FIELD_GET REG_FIELD_GET8
BIT BIT_ULL GENMASK GENMASK_ULL _MASKED_FIELD _MASKED_BIT_ENABLE _MASKED_BIT_DISABLE DISPLAY_INFO DISPLAY_MMIO_BASE
GMBUS_MMIO_BASE BUILD_BUG_ON_ZERO __is_constexpr _PAGE_PWT _PAGE_PCD _PAGE_PAT U32_MAX ARRAY_SIZE'''.split())
def items(text):
    """(start line, names, text) of each #define and enum"""
    L=text.split('\n'); out=[]; i=0
    while i<len(L):
        l=L[i]
        m=re.match(r'#\s*define\s+(\w+)',l)
        if m:
            j=i
            while L[j].rstrip().endswith('\\'): j+=1
            # a comment left open on the line
            t='\n'.join(L[i:j+1])
            while t.count('/*')>t.count('*/'): j+=1; t='\n'.join(L[i:j+1])
            out.append((i,[m.group(1)],t)); i=j+1; continue
        if re.match(r'enum\s*\w*\s*\{',l):
            j=i
            while not L[j].startswith('}'): j+=1
            t='\n'.join(L[i:j+1])
            body=re.sub(r'/\*.*?\*/','',t,flags=re.S)
            body=body[body.index('{')+1:body.rindex('}')]
            names=[x.group(1) for x in re.finditer(r'^\s*([A-Z_][A-Z0-9_]*)\s*(?:=|,|$)',body,re.M)]
            out.append((i,names,t)); i=j+1; continue
        i+=1
    return out
VER=open(os.path.join(LX,'Makefile')).read()
VER='.'.join(__import__('re').search(r'^'+k+r' = (\d+)',VER,__import__('re').M).group(1) for k in ('VERSION','PATCHLEVEL'))
defs={}  # name -> (src index, line, text)
heads={}
for k,(p,_) in enumerate(SRC):
    t=open(os.path.join(LX,p)).read()
    heads[k]=t[:t.index('\n#')].rstrip()
    for ln,names,txt in items(t):
        for n in names:
            if n not in defs: defs[n]=(k,ln,txt)
ours=set(); mine=set()
for f in glob.glob(D+'/*.[ch]')+glob.glob(D+'/*/*.[ch]'):
    if any(f.endswith(x) for _,x in SRC) or f.endswith(('intelprobe.c','inteltest.c')): continue
    t=re.sub(r'/\*.*?\*/','',open(f).read(),flags=re.S)
    ours|=set(re.findall(r'\b[A-Z_][A-Za-z0-9_]*\b',t))
    mine|=set(re.findall(r'#\s*define\s+(\w+)',t))
want=set(); ours-=mine
todo=[n for n in ours if n in defs and n not in SHIM]
while todo:
    n=todo.pop()
    if n in want: continue
    want.add(n)
    body=re.sub(r'/\*.*?\*/','',defs[n][2],flags=re.S)
    for t in set(re.findall(r'\b[A-Za-z_]\w*\b',body)):
        if t in defs and t not in SHIM and t not in want: todo.append(t)
byfile={}
for n in want:
    k,ln,txt=defs[n]; byfile.setdefault(k,{})[ln]=txt
for k,its in sorted(byfile.items()):
    p,o=SRC[k]
    g='_INTELGPU_'+re.sub(r'\W','_',o.upper())+'_'
    s=heads[k]+'\n\n/*\n * From Linux '+VER+', '+p+',\n * for aux/intelgpu: the definitions it uses, as they are there.\n */\n\n#ifndef '+g+'\n#define '+g+'\n\n'
    s+='\n'.join(its[ln] for ln in sorted(its))+'\n\n#endif\n'
    os.makedirs(os.path.dirname(os.path.join(D,o)) or D,exist_ok=True)
    open(os.path.join(D,o),'w').write(s)
    print(o,len(its))
miss=sorted(n for n in ours if n not in defs and n not in SHIM and n.isupper())
print('not from Linux:',' '.join(miss))
