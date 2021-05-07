
import time
import os
import random

def init():
    #print(os.getcwd())
    os.chdir("../tools/serial-io/")
    os.system("sudo ./tunslip6 -a 172.17.0.2 -p 60001 bbbb::1/64")

init()