# 9front gcc Port - Todo

## Hochpriorität
- [x] pc64/main.c - m->ticks → m->tscticks
- [x] pc64/main.c - lock/unlock pointer type fixes
- [x] pc64/mmu.c - lock/unlock pointer type fixes
- [x] pc64/fpu.c - lock/unlock pointer type fixes
- [x] port/random.c - lock/unlock pointer type fixes
- [x] port/rebootcmd.c - readn cast fix
- [x] pc/mp.c - lock/unlock pointer type fixes
- [x] pc/mp.h - lock/unlock pointer type fixes
- [x] pc/apbootstrap.i - __va() cast fix
- [x] sd53c8xx.c - fehlende Defines hinzugefügt (A_STATE_*, A_BSIZE, E_*)
- [x] sd53c8xx.c - Microcode loading commented out (na_script/na_patches stubs)
- [x] systab.h - enum designator errors fixed via genheaders.go patch
- [x] trap.c - conflicting types for `etext` (removed duplicate declaration in pc64/trap.c)
- [x] usbohci.c - incompatible pointer types (Qio* vs Isoio*) (added cast)
- [x] usbxhci.c - incompatible return type (Ctlr* vs Xhci*) (added cast)
- [x] userinit.c - Egrp has no member named `ref` (should be `Ref.ref`)
- [x] userinit.c - invalid sizeof on incomplete type uchar[] (created placeholder initcode.i)
- [x] vgaradeon.c - VGAscr has no member named `offset` (changed to `scr->Cursor.offset`)
- [x] radeon.h - called object is not a function or function pointer (missing `|` operator in macro usage)
- [x] build.json - duplicate Oflags entry removed, -fcommon added to Cflags
- [x] CRLF line endings in repository source files converted to LF
- [x] **pc64/rebootcode.s → rebootcode.S portiert** — assembliert sauber (`gcc -c -std=c11 -m64 -mcmodel=kernel -mno-red-zone -mno-sse -mno-sse2 -mno-mmx -fno-pic -fno-pie -Wall`, keine Warnings), Kodierung per objdump gegengeprüft (far-return Relocation, CMP-Operanden-Reihenfolge, nur `main` global).
- [x] **pc64/apbootstrap.s → apbootstrap.S portiert** — assembliert sauber. Real-Mode-Teil separat mit `objdump -m i8086` verifiziert (`.code16gcc` fügt den 0x66-Operandengrößen-Prefix für die 32-Bit-CR0/CR4-Zugriffe korrekt automatisch ein).
- [x] **pc64/l.s → l.S portiert** (alle ~100 TEXT-Symbole) — assembliert sauber, `nm` zeigt exakt die erwarteten globalen Symbole, alle `<>`-lokalen Symbole blieben lokal (kein Linker-Konflikt zwischen den drei Dateien), externe Referenzen (`main`, `trap`, `syscall`, `nrdy`, `MemMin`, `edata`, `end`, `etext`) sind unverändert undefiniert wie im Original.
  - **Wichtiger Fund, korrigiert ggü. früherem Stand dieser Datei:** RARG/BP hat in l.s zwei verschiedene Bedeutungen, nicht eine einzige "RARG=%rdi"-Regel:
    1. Bei normal C-aufrufbaren Funktionen ist RARG/BP das alte Plan9-6c-"1. Argument in BP"-Register → unter System V wird daraus **%rdi** (arg2 +8(FP) → %rsi, arg3 +16(FP) → %rdx, usw.), pro Funktion anhand ihrer FP-Offsets bestimmt, nicht pauschal ersetzt.
    2. Bei echten Hardware-Einstiegspunkten (`touser`, `syscallentry`, `forkret`, `_strayintr`/`_strayintrx`/`_intrcommon`, `noteret`, `vectortable` — erreicht über SYSCALL/SYSRET/IRETQ/IDT, nicht per CALL) ist BP eine private Kernel/Userspace-ABI-Konvention (z.B. die Syscall-Nummer) und bleibt **literal %rbp**. Wo so eine Funktion selbst einen echten C-Aufruf macht (`syscallentry`→`syscall()`, `_intrcommon`→`trap()`), gilt für *diesen* Aufruf wieder Regel 1 (%rdi).
    3. `vmxon`/`vmclear`/`vmptrld`/`vmwrite`/`vmread`/`invept`/`invvpid`/`vmlaunch`/`putcr2`/`putdr`/`putdr01236`/`putdr6`/`putdr7` benutzen im Original literales "BP" statt "RARG" für exakt denselben Zweck (1. Argument) — reine Stilinkonsistenz im Original, semantisch Regel 1.
  - **rule-6-Stellen** (alte Konvention: 1. Argument bekommt immer einen Stackslot, auch wenn zusätzlich in BP gehalten → unter System V gibt es diesen Slot nicht mehr, explizit `subq`/`addq` ergänzt, mit `/* GCC port: */`-Kommentar): `_fldcw`, `_ldmxcsr`, `vmxon`, `vmclear`, `vmptrld`, `invept`, `invvpid`.
  - **vmlaunch/vmrestore**: das Original schmuggelt den `Ureg*` über einen fixen Stack-Slot via `HOST_RSP` (VMCS-Feld 0x6C14) an `vmrestore` (den `HOST_RIP`-Landepunkt nach einem VM-Exit) vorbei — dafür musste `vmlaunch` einen eigenen Scratch-Slot allozieren (`subq $8,%rsp`) und alle Fehlerpfade balancieren (`addq $8,%rsp` vor `_vmout`); `vmrestore` selbst brauchte keine ABI-Anpassung (kein C-Aufruf).
  - **Offene Annahme, noch nicht gegen echten Aufrufer verifizierbar** (main.c/trap.c/vmx.c fehlen aktuell im Baum, siehe unten): `touser`'s `%rdi`-Argument und `invept`/`invvpid`'s 16-Byte-Descriptor-Argument (angenommen als zwei Achtbytes in %rsi:%rdx nach System-V-Klassifizierung).
  - Register-Mapping: RMACH=%r15, RUSER=%r14 (jetzt zentral in `pc64/mem.h` hinter `#ifdef __ASSEMBLER__` definiert, damit sowohl GAS-.S-Dateien als auch der alte 6a-Pfad ihre jeweils passende Syntax bekommen).
  - GAS-Besonderheiten: `MOVL x,GDTR`/`IDTR` → `lgdt`/`sgdt`/`lidt`; `MOVW x,LDTR`/`TASK` → `lldt`/`ltr`; `MOVL a,b`-CMP-Operanden sind bei Plan9 Intel-Reihenfolge (a-b), AT&T braucht sie umgedreht (`cmp b,a`); `vmwrite`/`vmread`/`invept`/`invvpid`-Operandenreihenfolge empirisch per objdump verifiziert statt geraten; raw-byte-kodierte XSAVE/XRSTOR-Familie (0x48 REX.W-Prefix im Original) musste durch native `xsave64`/`xrstor64`/... Mnemonics ersetzt werden, weil die alten Rohbytes ein festes Register (RBP) im ModRM kodierten, das sich mit dem ABI-Wechsel änderte.

## Mittlere Priorität
- [ ] build.json - inkompatible Module entfernt: wifi, virtio10pc, vgax, vgaradeon, vga, vgaigfx, usbuhci, usbxhci
- [ ] klibc.a und andere klib*.a als leere Archives stubbed - echte Implementierung fehlt
- [ ] GCC built-in functions: __popcountdi2, __unordtf2, __letf2 - stubben oder -mlong-double-128 entfernen
- [ ] Kernel kann nicht gegen system libc (-lc) linken (-mcmodel=kernel PLT overflow)
- [ ] Andere Kernel-Module auf gcc Errors prüfen

## Niedrige Priorität / Später
- [ ] **na (NCR53c8xx Assembler) mit gcc kompilierbar machen**
  - na.y verwendet <u.h> und <libc.h> (Plan9 Headers)
  - Erfordert Port der Plan9 stdlib Headers oder Ersatz
  - Danach: na bauen, sd53c8xx.n → sd53c8xx.i generieren
  - build.json aktualisieren um .i statt .c zu verwenden
  - Manuelle Defines in sd53c8xx.c entfernen

## Wichtige Hinweise
- Immer so nah wie möglich am Original 9front Source bleiben
- Keine lokalen Enum/Struct Änderungen wenn vermeidbar
- Branch main = Original 9front Quelle
- Branch gcc = Portierung
- Plan9 Compiler (8c/6c) verzeiht mehr als gcc
- lock/unlock: Plan9 nutzt Lock* Zeiger, gcc erfordert explizite &c->lock Adresse
- Kernel build nutzt -mcmodel=kernel - system libc ist inkompatibel
- build.go kompiliert .c und .S Dateien mit gcc - .S verwendet GAS-Syntax
- -fcommon Flag notwendig für multiple global/inline definitions in headers
- **Blocker für volle pc64-Kernel-Link-Verifikation:** `sys/src/9/pc64/{main,mmu,trap,fpu,squidboy}.c` sind aktuell im Arbeitsverzeichnis gelöscht (unstaged), obwohl `build.json` sie weiterhin als SourceFiles listet; `pc64.c` (nur generiertes `devtab[]`) ersetzt sie nicht. l.S/apbootstrap.S/rebootcode.S sind nur isoliert (`gcc -c`) geprüft, nicht gegen einen echten Kernel-Link.
- **Unabhängiger Fund, nicht Teil der eigenen Arbeit:** `sys/src/libc/amd64/*.s` (≈22 Dateien: atom.s, main9.s, setjmp.s, memcpy.s, ...) haben unstaged, kaputte Teil-Edits (ungültiges GAS, RARG/JMPF etc. unübersetzt gelassen) direkt in den Original-.s-Dateien statt in neuen .S-Dateien — assembliert nicht, und verfälscht nebenbei den klassischen `mk`-Build. `git checkout -- sys/src/libc/amd64/*.s` würde das rückgängig machen, falls gewünscht.
- GAS: REP-Präfix ohne Semikolon (REP MOVSB, nicht REP; MOVSB)
- GAS: IN/OUT Instruktions-Syntax (inb %dx, %al; outb %al, %dx)
- GAS: LGDT/SGDT/LIDT als raw bytes (.byte 0x0f, 0x01, 0x10 etc.)
- GAS: Immediate-First bei CMP/TEST (CMPL $KESEL, 48(%rsp))
- GAS: GDT descriptors ohne $() Prefix (long (SEGL|SEGG|...), nicht long $(SEGL|SEGG|...))
- **`sys/src/libc/amd64.json` (klibc-amd64) hat bewusst KEIN `-mno-sse`/`-mno-sse2`/`-mno-mmx`, obwohl der Kernel selbst (`sys/src/9/pc64/build.json`) diese Flags hat — das ist kein Versehen, nicht wieder hinzufügen:**
  - Mit `-mno-sse` bricht gcc bei JEDER `double`-Rückgabefunktion mit `error: SSE-Registerrückgabe mit SSE ausgeschaltet` ab; klibc's SourceFiles enthält echte libm-Funktionen (asin.c, atan.c, atan2.c, exp.c, log.c, pow.c, pow10.c, sin.c, sinh.c, tan.c, tanh.c, hypot.c, plus strtod.c/atof.c/charstod.c) — ein Build mit den Flags scheitert sofort.
  - Für die 9 neu hinzugefügten `port/*.c`-Fallbacks (memccpy, memchr, memcmp, memmove, memset, strcat, strchr, strcpy, strlen — Ersatz für die noch unportierten `amd64/*.s`) besteht das eigentliche Kernel-Risiko der fehlenden Flags (Compiler wählt XMM/MMX-Register als Scratch, was bei fehlendem FPU-Save/Restore im Kernel-Entry-Pfad einen laufenden User-FPU-Context korrumpieren könnte) — per Test (`gcc -Os`, objdump) erzeugen aktuell aber KEINE dieser 9 Dateien SSE/MMX-Instruktionen, also kein akutes Problem.
  - `Cflags` gilt für den gesamten `klibc-amd64`-Manifest-Eintrag (eine Liste für alle SourceFiles) — es gibt aktuell keinen sauberen Weg, den Mathe-Funktionen andere Flags zu geben als den String/Mem-Funktionen, ohne den Manifest-Mechanismus zu ändern (z.B. zwei Einträge, die in dieselbe `klibc.a` archivieren — ungeprüft, ob `build.go`/`genklibs.py` das unterstützen).
  - Entschieden (2026-09-30): so lassen. Die Mathe-Funktionen werden vom pc64-Kernel vermutlich nie aufgerufen (statisches `ar`-Archiv zieht nur referenzierte Member), daher ist ihre SSE-basierte Calling Convention praktisch irrelevant; die String/Mem-Funktionen sind aktuell nachweislich SSE-frei kompiliert. Bei Compiler-/Flag-Änderungen erneut mit `objdump -d | grep -i xmm` gegenprüfen.

