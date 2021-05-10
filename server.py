import time
import os
import random


addressTemperatureSensor = "bbbb::c30c:0:0:3"

def checkBinding():
    print("============ START CHECK BINDING ============")
    #os.system("gnome-terminal --disable-factory")
    #os.system("x-terminal-emulator -e /bin/bash & ")
    os.system("ping6 -c 1 bbbb::c30c:0:0:1") # non-root
    os.system("ping6 -c 1 bbbb::c30c:0:0:2") # border router
    os.system("ping6 -c 1 bbbb::c30c:0:0:3") # temperature sensor
    print("============ STOP CHECK BINDING ============")

def askTemperature():
    code = 1
    type_info = 0
    id = 0
    payload = 0
    message = code | type_info<<8 | id<<16 | payload<<24
    os.system("echo " + str(message) + " | nc -u " + addressTemperatureSensor + " 3000 &")


def server():
    print("Start SERVER")
    while(True):
        #os.system("echo 000000 | nc -u " + addressTemperatureSensor + " 3000")
        askTemperature()
        time.sleep(3)

checkBinding()
server()
