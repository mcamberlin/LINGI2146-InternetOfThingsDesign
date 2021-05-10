import time
import os
import random


addressTemperatureSensor = "bbbb::206:6:6:6"

def checkBinding():
    print("============ START CHECK BINDING ============")
    #os.system("gnome-terminal --disable-factory")
    #os.system("x-terminal-emulator -e /bin/bash & ")
    os.system("ping6 -c 1 bbbb::c30c:0:0:1") # client
    os.system("ping6 -c 1 bbbb::c30c:0:0:2") # client
    os.system("ping6 -c 1 bbbb::c30c:0:0:3") # client
    os.system("ping6 -c 1 bbbb::c30c:0:0:4") # client
    os.system("ping6 -c 1 bbbb::c30c:0:0:5") # server
    os.system("ping6 -c 1 " + addressTemperatureSensor) # temperature sensor
    print("============ STOP CHECK BINDING ============")

def server():
    print("Start SERVER")
    while(True):
        os.system("echo How hot are you? | nc -u " + addressTemperatureSensor + " 3000")
        time.sleep(3)


checkBinding()
server()
