#! /usr/bin/env python
# -*- coding: utf-8 -*-

from common_utils import *
from crypto_utils import *
from random import *
import os

MODE_TYPE = {
        'MODE_UNSPECIFIED'        : 0x0000,
        'MODE_KEYINIT'            : 0x0001,
        'MODE_AESINIT_ENC'        : 0x0002,
        'MODE_AESINIT_DEC'        : 0x0004,
        'MODE_ENC'                : 0x0008,
        'MODE_DEC'                : 0x0010,
        'MODE_RANDOM_KEY_EXT'     : 0x0020,     
        'MODE_RANDOM_AES_EXT'     : 0x0040      
}

STATE_TYPE = {
        'STATE_INITIALIZED'       : 0x00,
        'STATE_KEYINIT'           : 0x01,
        'STATE_AESINIT_ENC'       : 0x02,
	'STATE_AESINIT_DEC'       : 0x04
}

ERR_MODE = {
        'ERR_NO_OPERATION'        : 0x1,
        'ERR_ENC_AND_DEC'         : 0x2,
        'ERR_ENC_AND_DEC_AESINIT' : 0x4,  
        'ERR_AESINIT_AND_OP'      : 0x8 
}

ERR_OPERATION = {
        'ERR_OP_GEN_RANDOM_KEY'   : 0x10,
        'ERR_OP_GEN_RANDOM_AES'   : 0x20,
        'ERR_OP_KEYINIT'          : 0x40,
        'ERR_OP_AESINIT'          : 0x80,
        'ERR_OP_ENC'              : 0x100,
        'ERR_OP_DEC'              : 0x200
}

ERR_INPUT = {
        'ERR_SIZE_KEY'            : 0x400, 
        'ERR_SIZE_INPUT'          : 0x800,
        'ERR_SIZE_OUTPUT'         : 0x1000,
        'ERR_KEY_UNUSED'          : 0x2000,
        'ERR_INPUT_UNUSED'        : 0x4000,
        'ERR_KEY_MISSING'         : 0x8000,
        'ERR_INPUT_MISSING'       : 0x10000

};

ERR_MODE_STATE = { 
        'ERR_AESINIT_MISSING'     : 0x20000,
        'ERR_KEYINIT_MISSING'     : 0x40000,
	'ERR_AESINIT_BAD'         : 0x80000
}

TO_CHECK = {
	'CHECK_OUTPUT' 		  : (0x1 << 0),
	'CHECK_RET'		  : (0x1 << 1),
	'CHECK_STATE'		  : (0x1 << 2),
	'CHECK_KEY_CTR'		  : (0x1 << 3),
	'CHECK_AES_CTR'		  : (0x1 << 4),
	'CHECK_AES_CTX'		  : (0x1 << 5)
}

# Targets to send tests
TARGETS = {
        'QEMU'   		  : 0,
        'UART'   		  : 1,
        'STATIC' 		  : 2,
}

# Returns a serialized test for AES
def gen_aes_test(name=None, mode=None, to_check=None, msg=None, key=None, random_key=None, random_aes=None, exp_out=None, exp_ret=None, exp_state=None, exp_ctr_key=None, exp_ctr_aes=None, comm_line_output=None):
    aes_test_string = "t"
    # Name
    if name != None:
        if len(name) > 32:
            print("Error: unsupported message length %d > 32" % len(name))
        aes_test_string += name + ((32-len(name))*'\x00')
    else:
        aes_test_string += (32*'\x00')
    # Mode
    if mode != None:
        aes_test_string += chr(mode)
    else:
        aes_test_string += chr(0) 
    # To check
    if to_check != None:
        aes_test_string += chr(to_check)
    else:
        aes_test_string += chr(0)
    # Message
    if msg != None:
        if len(msg) != 16:
            print("Error: unsupported message length %d != 16" % len(msg))
            sys.exit(-1)
        aes_test_string += chr(16) + msg
    else:
        aes_test_string += chr(0) + ('\x00'*16)
    # Key
    if key != None:
        if (len(key) != 16) and (len(key) != 24) and (len(key) != 32):
            print("Error: unsupported key length %d != 16, 24 or 32" % len(key))
            sys.exit(-1)
        aes_test_string += chr(len(key)) + key + ((32-len(key))*'\x00')
    else:
        aes_test_string += chr(0) + ('\x00'*32)
    # Random Key
    if random_key != None:
        if len(random_key) != 19:
            print("Error: unsupported random_key length %d != 19" % len(random_key))
            sys.exit(-1)
        aes_test_string += chr(19) + random_key
    else:
        aes_test_string += chr(0) + ('\x00'*19)
    # Random AES
    if random_aes != None:
        if len(random_aes) != 19:
            print("Error: unsupported random_aes length %d != 19" % len(random_aes))
            sys.exit(-1)
        aes_test_string += chr(19) + random_aes
    else:
        aes_test_string += chr(0) + ('\x00'*19)
    # Expected output
    if exp_out != None:
        if len(exp_out) != 16:
            print("Error: unsupported expected output length %d != 16" % len(exp_out))
            sys.exit(-1)
        aes_test_string += chr(16) + exp_out
    else:
        aes_test_string += chr(0) + ('\x00'*16)
    # Expected ret
    if exp_ret != None:
        if exp_ret >= (0x1 << 32):
            print("Error: unsupported expected ret (too big) %d" % exp_ret)
            sys.exit(-1)
        aes_test_string += chr(exp_ret & 0xff) + chr((exp_ret & 0xff00) >> 8) + chr((exp_ret & 0xff0000) >> 16) + chr((exp_ret & 0xff000000) >> 24)
    else:
        aes_test_string += ('\x00'*4)
    # Expected state
    if exp_state != None:
        aes_test_string += chr(exp_state)
    else:
        aes_test_string += chr(0)
    # Expected key counter
    if exp_ctr_key != None:
        if exp_ret >= (0x1 << 32):
            print("Error: unsupported expected counter Key (too big) %d" % exp_ctr_key)
            sys.exit(-1)
        aes_test_string += chr(exp_ctr_key & 0xff) + chr((exp_ctr_key & 0xff00) >> 8) + chr((exp_ctr_key & 0xff0000) >> 16) + chr((exp_ctr_key & 0xff000000) >> 24)
    else:
        aes_test_string += ('\x00'*4)
    # Expected AES counter
    if exp_ctr_aes != None:
        if exp_ctr_aes >= (0x1 << 32):
            print("Error: unsupported expected counter AES (too big) %d" % exp_ctr_aes)
            sys.exit(-1)
        aes_test_string += chr(exp_ctr_aes & 0xff) + chr((exp_ctr_aes & 0xff00) >> 8) + chr((exp_ctr_aes & 0xff0000) >> 16) + chr((exp_ctr_aes & 0xff000000) >> 24)
    else:
        aes_test_string += ('\x00'*4)
    # Comm line output
    if comm_line_output != None:
        aes_test_string += chr(comm_line_output)
    else:
        # No output by default
        aes_test_string += chr(0)

    return aes_test_string


def generate_aes_test_case(i, direction, comm_line_output=None):
    test = ""
    test_idx = []
    old_len = 0
    msg = expand(inttostring(randint(0, 2**128)), 128, "LEFT")
    key = expand(inttostring(randint(0, 2**128)), 128, "LEFT")
    aes_ecb = local_AES(key, AES.MODE_ECB)
    if direction == 'ENC':
        exp_out = aes_ecb.encrypt(msg)
    elif direction == 'DEC':
        exp_out = aes_ecb.decrypt(msg)
    else:
        print("Error: unsupported direction %s for AES" % direction)
        sys.exit(-1)
    # Full AES en/decryption test
    mode = MODE_TYPE['MODE_KEYINIT'] | MODE_TYPE['MODE_AESINIT_'+direction] | MODE_TYPE['MODE_'+direction]
    to_check = TO_CHECK['CHECK_RET'] | TO_CHECK['CHECK_OUTPUT']
    test += "b"
    test_idx.append((old_len,len(test)))
    old_len = len(test)
    test += gen_aes_test(name="["+direction+str(i)+"]full_"+str(i), mode=mode, to_check=to_check, msg=msg, key=key, random_key=None, random_aes=None, exp_out=exp_out,exp_ret=None, exp_state=None, exp_ctr_key=None, exp_ctr_aes=None, comm_line_output=comm_line_output)
    test_idx.append((old_len,len(test)))
    old_len = len(test)
    test += "e"
    test_idx.append((old_len,len(test)))
    old_len = len(test)
    # Split AES en/decryption test, init alone
    mode = MODE_TYPE['MODE_KEYINIT'] | MODE_TYPE['MODE_AESINIT_'+direction]
    to_check = TO_CHECK['CHECK_RET']
    test += "b"
    test_idx.append((old_len,len(test)))
    old_len = len(test)
    test += gen_aes_test(name="["+direction+str(i)+"]split_step0_"+str(i), mode=mode, to_check=to_check, msg=msg, key=key, random_key=None, random_aes=None, exp_out=None,exp_ret=0, exp_state=None, exp_ctr_key=None, exp_ctr_aes=None, comm_line_output=comm_line_output)
    test_idx.append((old_len,len(test)))
    old_len = len(test)
    mode = MODE_TYPE['MODE_'+direction]
    to_check = TO_CHECK['CHECK_RET'] | TO_CHECK['CHECK_OUTPUT']
    test += gen_aes_test(name="["+direction+str(i)+"]split_step1_"+str(i), mode=mode, to_check=to_check, msg=msg, key=key, random_key=None, random_aes=None, exp_out=exp_out,exp_ret=0, exp_state=None, exp_ctr_key=None, exp_ctr_aes=None, comm_line_output=comm_line_output)
    test_idx.append((old_len,len(test)))
    old_len = len(test)
    test += "e"
    test_idx.append((old_len,len(test)))
    old_len = len(test)
    # Split AES en/decryption test, init split
    mode = MODE_TYPE['MODE_KEYINIT']
    to_check = TO_CHECK['CHECK_RET']
    test += "b"
    test_idx.append((old_len,len(test)))
    old_len = len(test)
    test += gen_aes_test(name="["+direction+str(i)+"]splitinit_step0_"+str(i), mode=mode, to_check=to_check, msg=msg, key=key, random_key=None, random_aes=None, exp_out=None,exp_ret=0, exp_state=None, exp_ctr_key=None, exp_ctr_aes=None, comm_line_output=comm_line_output)
    test_idx.append((old_len,len(test)))
    old_len = len(test)
    mode = MODE_TYPE['MODE_AESINIT_'+direction]
    to_check = TO_CHECK['CHECK_RET']
    test += gen_aes_test(name="["+direction+str(i)+"]splitinit_step1_"+str(i), mode=mode, to_check=to_check, msg=msg, key=key, random_key=None, random_aes=None, exp_out=None,exp_ret=0, exp_state=None, exp_ctr_key=None, exp_ctr_aes=None, comm_line_output=comm_line_output)
    test_idx.append((old_len,len(test)))
    old_len = len(test)
    mode = MODE_TYPE['MODE_'+direction]
    to_check = TO_CHECK['CHECK_RET'] | TO_CHECK['CHECK_OUTPUT']
    test += gen_aes_test(name="["+direction+str(i)+"]splitinit_step2_"+str(i), mode=mode, to_check=to_check, msg=msg, key=key, random_key=None, random_aes=None, exp_out=exp_out,exp_ret=0, exp_state=None, exp_ctr_key=None, exp_ctr_aes=None, comm_line_output=comm_line_output)
    test_idx.append((old_len,len(test)))
    old_len = len(test)
    test += "e"
    test_idx.append((old_len,len(test)))
    old_len = len(test)
    return test, test_idx


def generate_multiple_aes_tests(num, comm_line_output=None):
    test = ""
    test_idx = []
    # Generate all the asked tests for encryption and decryption
    for i in range(0, num):
        # Encryption
        test_i_enc, test_i_enc_idx = generate_aes_test_case(i, 'ENC', comm_line_output=comm_line_output)
        # Offsets
        for idx in range(len(test_i_enc_idx)):
            b, e = test_i_enc_idx[idx]
            test_i_enc_idx[idx] = (b + len(test), e + len(test))
        test += test_i_enc
        test_idx += test_i_enc_idx
        # Decryption
        test_i_dec, test_i_dec_idx  = generate_aes_test_case(i, 'DEC', comm_line_output=comm_line_output)
        # Offsets
        for idx in range(len(test_i_dec_idx)):
            b, e = test_i_dec_idx[idx]
            test_i_dec_idx[idx] = (b + len(test), e + len(test))
        test += test_i_dec
        test_idx += test_i_dec_idx
    return test, test_idx

def send_comm_aes_tests(t, channel, uart=None):
    tests, tests_idx = t
    script_path = os.path.abspath(os.path.dirname(sys.argv[0])) + "/"
    # The STATIC channel generates an internal test as header to be compiled
    # with the sources. The static test is launched at startup of the firmware/program.    
    if channel == 'STATIC':
        # The communication channel is stdin, we create a flat file
        if not os.path.exists(script_path+"../generated_tests/"):
            os.mkdir(script_path+"../generated_tests/")
        out_file_name = script_path+"../generated_tests/aes_tests.h"
        out_string =  "/* Autogerated static tests for the masked AES. Please do not edit! */"
        out_string += "\nconst char aes_static_tests[] = \""
        for i in range(len(tests)):
            out_string += "\\x%02x" % ord(tests[i])
        out_string += "\";"
        save_in_file(out_string, out_file_name)
    # The QEMU channel is barely a flat file that is fed through stdin
    elif channel == 'QEMU':
        # The communication channel is stdin, we create a flat file
        if not os.path.exists(script_path+"../generated_tests/"):
            os.mkdir(script_path+"../generated_tests/")
        out_file_name = script_path+"../generated_tests/aes_tests.bin"
        save_in_file(tests, out_file_name)
    # The UART channel communicates with a board plugged to an UART
    elif channel == 'UART':
        import serial
        # An uart path must be provided
        if uart == None:
            print("Error: you must provide an uart path!")
            sys.exit(-1)
        # Open the serial handler
        ser = serial.Serial()
        ser.port = uart
        ser.baudrate = 115200
        ser.open()
        # Now that the serial is opened, send our test file
        # We split all the tests
        for (b, e) in tests_idx:
            ser.write(encode_string(tests[b:e]))
            if len(tests[b:e]) > 1:
                print("Testing %s" % tests[b+1:b+33])
                # Wait for the response ('r' + return status + real output when needed)
                ret = ser.read(18)
                ret_err = ret[1]
                ret_out = ret[2:]
                # Check the return error and the output
                exp_out = tests[b+126:b+126+16]
                print("  -> Test returns %s" % (local_hexlify(ret_err)))
                if exp_out != ret_out:
                    print("Error: output mismatch %s != %s" % (local_hexlify(exp_out), local_hexlify(ret_out)))
    else:
        print("Error: unsupported channel %s for AES tests" % channel)
        sys.exit(-1)


def PrintUsage():
    executable = os.path.basename(__file__)
    print("Error when executing %s\n\tUsage:\t%s (STATIC|QEMU|UART) <number> <uart_path>" % (executable, executable))
    sys.exit(-1)

# Get the arguments
if __name__ == '__main__':
    # Get the arguments
    if len(sys.argv) <= 1 or len(sys.argv) > 4:
        PrintUsage()
    test = sys.argv[1]
    # Default number of tests to 10
    num_tests = 10
    if len(sys.argv) >= 3:
        num_tests = int(sys.argv[2])
    uart = '/dev/ttyUSB1'
    if len(sys.argv) >= 4:
        uart = sys.argv[3]
    if test != 'STATIC' and test != 'QEMU' and test != 'UART':
        PrintUsage()
    if test == 'UART':
        send_comm_aes_tests(generate_multiple_aes_tests(num_tests, comm_line_output=1), test, uart=uart)
    else:
        send_comm_aes_tests(generate_multiple_aes_tests(num_tests), test)
