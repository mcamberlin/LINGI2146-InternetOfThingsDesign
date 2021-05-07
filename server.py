import time
import os
import random


addresseTemperatureSensor = "bbbb::206:6:6:6"

def server():
    print("Start SERVER")
    while(True):
        os.system("echo How hot are you? | nc -u " + addresseTemperatureSensor + " 3000 &")
        time.sleep(3)
            
def temperature():
    os.system("echo How hot are you? | nc -u " + addresseTemperatureSensor + " 3000 &")



server()
