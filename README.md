## README

This repo is a simple test of for multithreading on qemu with usermode emulation.
Testing was done with an x86 host and a ppc guest, but it should be trivial to
convert these files to work on other architectures.

### Building
First you should get/build qemu-ppc, then:
- Run `make ppc`to create guest binary

### Running
- Run `QEMU_GDB=<PORTNUM> QEMU_CPU=ppc gdb-multiarch qemu-ppc`
- Inside gdb run `source ./parent_gdbscript.py`
- In another terminal run `gdb-multiarch --command=./gdbscript.py`
- You should see both scripts running and printing out threads waiting to join.
  A file called qemulog.txt will also be generated with all of the logging of
  gdbstub messages send and recieved by qemu-ppc.
- To enable logging of the gdb messages in the guest process as well, uncomment
  the line `#gdb.execute("set debug remote 1")` in gdbscript.py 
