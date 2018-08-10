#!/usr/bin/env python

import sys
from dbb_utils import *


try:
    openHid()
    
    password = '0000'

    # Example JSON commands - refer to shiftcrypto.ch/api
    message = '{"password":"' + password + '"}'
    
    hid_send_plain(messsage)
    #hid_send_encrypt(message, password)


except IOError as ex:
    print(ex)
except(KeyboardInterrupt, SystemExit):
    print("Exiting code")

dbb_hid.close()

