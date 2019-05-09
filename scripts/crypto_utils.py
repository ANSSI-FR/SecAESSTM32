# Crypto related utils

import math
from Crypto.Cipher import AES

from common_utils import *

# Python 2/3 abstraction layer for AES
class local_AES:
    aes = None
    iv = None
    def  __init__(self, key, mode, iv=None, counter=None):
        if is_python_2() == False: 
            key = key.encode('latin-1')
        if iv != None:
            self.iv = iv
            if is_python_2() == False: 
                iv = iv.encode('latin-1')
            if mode == AES.MODE_CTR:
                if counter == None:
                    self.aes = AES.new(key, mode, counter=self.counter_inc)
                else:
                    self.aes = AES.new(key, mode, counter=counter)
            else:
                self.aes = AES.new(key, mode, iv)
            return
        else:
            if mode == AES.MODE_CTR:
                if counter == None:
                    self.iv = expand(inttostring(0), 128, "LEFT")
                    self.aes = AES.new(key, mode, counter=self.counter_inc)
                else:
                    self.aes = AES.new(key, mode, counter=counter)
            else:
                self.aes = AES.new(key, mode)
            return
    def counter_inc(self):
        curr_iv = expand(inttostring((stringtoint(self.iv))), 128, "LEFT")
        self.iv = expand(inttostring((stringtoint(self.iv)+1)), 128, "LEFT")
        if is_python_2() == False:
            curr_iv = curr_iv.encode('latin-1')
        return curr_iv
    def encrypt(self, data):
        if is_python_2() == False:
            data = data.encode('latin-1')
        ret = self.aes.encrypt(data)
        if is_python_2() == False:
            ret = ret.decode('latin-1')
        return ret
    def decrypt(self, data):
        if is_python_2() == False:
            data = data.encode('latin-1')
        ret = self.aes.decrypt(data)
        if is_python_2() == False:
            ret = ret.decode('latin-1')
        return ret
    @staticmethod
    def new(key, mode, iv=None, counter=None):
        return local_AES(key, mode, iv=iv, counter=counter)
