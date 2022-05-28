#!/bin/env python3
import gdb

arg_reg = ['rdi','rsi','rdx','rcx', 'r8', 'r9']

class custom_bp(gdb.Breakpoint):
    def __init__(self, bp, stop_func=None, stop_args={}, **kwargs):
        self.stop_func = stop_func
        self.stop_args = stop_args
        super().__init__(bp, **kwargs)

    def stop(self):
        if self.stop_func:
            return self.stop_func(**self.stop_args)
        else:
            return True 

def delete_all_bps():
    for bp in gdb.breakpoints():
        bp.delete()

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

def readvar(varname):
    gdb.selected_frame().read_var(varname)

def print_read_buf():
    buf_var, _ = gdb.lookup_symbol('buf')
    if buf_var:
        buf_val = buf_var.value(gdb.selected_frame())
        if not buf_val:
            return
        print(f"Thread #{gdb.selected_thread().num} got packet: {str(buf_val)[:20]}")
    else:
        print("Could not read buf")
    return False

def is_gdbstub(name=""):
    gdb.execute('bt')
    if gdb.selected_thread().name == "gdbstub_handler":
        print(f"{name}: Correct handler, arg0={readarg(0)}")
    else:
        print("Wrong handler")
        return True

done = False
def do_after_main():
    global done
    if not done:
        custom_bp("gdb_handle_vcont", is_gdbstub, stop_args={"name": "gdb_handle_vcont"})
        custom_bp("gdb_read_byte", is_gdbstub, stop_args={"name": "gdb_read_byte"})
        custom_bp("put_packet_binary", is_gdbstub, stop_args={"name": "put_packet_binary"})
        done = True

def main():
    delete_all_bps()
    qemu_path = '/usr/local/bin/qemu-ppc'
    gdb.execute(f"file {qemu_path}")
    gdb.execute("set detach-on-fork off")
    '''
    try:
        gdb.execute("set non-stop on")
    except:
        pass
    '''
    gdb.execute("set python print-stack full")
    gdb.execute('run --trace "gdb*" -D ./qemulog.txt test 20')
main()
