# Various utils and helpers common to all our scripts

import sys, os, array, time
import binascii
from subprocess import Popen, PIPE, STDOUT
from threading import Timer
import math

### Ctrl-C handler
def handler(signal, frame):
    print("\nSIGINT caught: exiting ...")
    exit(0)

def is_python_2():
    if sys.version_info[0] < 3:
        return True
    else:
        return False

def getbitlen(bint):
    return int(bint).bit_length()

def getbytelen(bint):
    bitsize = getbitlen(bint)
    bytesize = int(bitsize // 8)
    if bitsize % 8 != 0:
        bytesize += 1
    return bytesize

def stringtoint(bitstring):
    acc = 0
    size = len(bitstring)
    for i in range(0, size):
        acc = acc + (ord(bitstring[i]) * (2**(8*(size - 1 - i))))
    return acc

def inttostring(a):
    size = int(getbytelen(a))
    outstr = ""
    for i in range(0, size):
        outstr = outstr + chr((a >> (8*(size - 1 - i))) & 0xFF)
    return outstr

def expand(bitstring, bitlen, direction):
    bytelen = int(math.ceil(bitlen / 8.))
    if len(bitstring) >= bytelen:
        return bitstring
    else:
        if direction == "LEFT":
            return ((bytelen-len(bitstring))*"\x00") + bitstring
        elif direction == "RIGHT":
            return bitstring + ((bytelen-len(bitstring))*"\x00")
        else:
            raise Exception("Error: unknown direction "+direction+" in expand")

# Helper to execute an external command
def sys_cmd(cmd):
    p = Popen(cmd, shell=True, stdin=PIPE, stdout=PIPE, stderr=STDOUT, close_fds=True)
    kill = lambda process: process.kill()
    timer = Timer(100, kill, [p])
    timer.start()
    out = p.stdout.read()
    if timer.is_alive():
        timer.cancel()
    p.wait()
    if p.returncode != 0:
        print("Error when executing command: "+cmd)
        print("Exec Trace:\n"+out)
        sys.exit(-1)
    return out

# Remove a file
def sys_rm_file(file_path):
    if os.path.isfile(file_path):
        os.remove(file_path)
    return

# Read a string from a file
def read_in_file(infilename):
    infile = open(infilename, 'rb')
    data = infile.read()
    infile.close()
    if is_python_2() == False:
        data = data.decode('latin-1')
    return data

# Save a string in a file
def save_in_file(data, outfilename):
    if is_python_2() == False:
        data = data.encode('latin-1')
    outfile = open(outfilename, 'wb')
    outfile.write(data)
    outfile.close()

# Helper to generate a random string with proper entropy
def gen_rand_string(size):
    if is_python_2() == True:
        return os.urandom(size)
    else:
        return os.urandom(size).decode('latin-1')

# Python 2/3 hexlify helper
def local_hexlify(str_in):
    if is_python_2() == True:
        return binascii.hexlify(str_in)
    else:
        return (binascii.hexlify(str_in.encode('latin-1'))).decode('latin-1')
 

# Python 2/3 unhexlify helper
def local_unhexlify(str_in):
    if is_python_2() == True:
        return binascii.unhexlify(str_in)
    else:
        return (binascii.unhexlify(str_in.encode('latin-1'))).decode('latin-1')
        
def encode_string(data):
    if is_python_2() == False:
        data = data.encode('latin-1')
    return data
