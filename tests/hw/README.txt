9front hardware tests (tests/hw in the 9front gcc port)

This stick boots 9front and runs the tests by itself, in the
configurations of hwtest/configs, one after another; the machine resets
between them. Logs: hwtest/run<N>/<configuration>/ on this partition
(<step>.log, summary.txt, kmesg.txt); hwtest/state says where it is.
At the end plan9.ini is set back (no hwtest) and the machine halts.

The tests only read the machine's internal disks.

If the machine hangs:
- while a test runs: power off and on; that step is logged as HANG
  and the run goes on with the next one.
- while a configuration boots (the kernel hangs): power off and on,
  press a key when 9boot prints plan9.ini, type
	hwskip=1
	boot
  The configuration is logged as BOOTHANG and the run goes on.

To start a new run: put hwtest=1 back into plan9.ini (as in
hwtest/plan9.base) and boot the stick.
