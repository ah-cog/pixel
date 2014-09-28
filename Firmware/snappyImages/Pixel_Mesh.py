# Pixel mesh communication firmware.
# Author: Michael Gubbels

from synapse.switchboard import *
from synapse.platforms import *

@setHook(HOOK_STARTUP)
def init():
    initUart(1, 9600) # Desired serial baud rate here
    flowControl(1, False) # Set flow control to True or False (as needed)
    #crossConnect(DS_STDIO, DS_UART1)
    crossConnect(DS_UART1, DS_TRANSPARENT) # Forward all data received over the UART1 serial connection to be broadcasted over mesh.
    mcastSerial(1, 2) # Multicast serial data all mesh nodes in "Group 1"

#@setHook(HOOK_1S)
def broadcastAddress():
    print "here"