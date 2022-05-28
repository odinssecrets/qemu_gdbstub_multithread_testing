#!/bin/env python3
import gdb

arg_reg = [f'r{x}' for x in range(3,11)]

class custom_bp(gdb.Breakpoint):
    def __init__(self, bp, stop_func=None):
        self.stop_func = stop_func
        super().__init__(bp)

    def stop(self):
        if self.stop_func:
            return self.stop_func()
        else:
            return False

def readreg(reg):
    return int(gdb.selected_frame().read_register(reg))

def readarg(argnum):
    if argnum >= len(arg_reg):
        return None
    return readreg(arg_reg[argnum])

def readstring(addr):
    inferior = gdb.selected_inferior()
    string_end = inferior.search_memory(addr, 4096, b'\x00') 
    return inferior.read_memory(addr, string_end-addr).tobytes().decode('utf-8')

def print_info():
    if "Waiting for thread" in readstring(readarg(0)): 
        print(f"Current thread (#{readarg(1)}) {readarg(2)} is waiting for {readarg(3)}")
    return False

def try_stepping():
    return False 

def main():
    gdb.execute("file ./test")
    gdb.execute(" set architecture powerpc:e500")
    gdb.execute("set detach-on-fork off")
    #gdb.execute("set debug remote 1")
    gdb.execute("target remote :5555")
    custom_bp("*printf", stop_func=print_info)
    gdb.execute("continue")
main()
