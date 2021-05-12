import os

"""
    Go to cd contiki-ng/tools/serial-io/ 
    Execute : //172.17.0.2
    sudo ./tunslip6 -a <container-ip> -p <port> bbbb::1/64 
"""
def init():
    #print(os.getcwd())
    os.chdir("../tools/serial-io/")
    os.system("sudo ./tunslip6 -a 172.17.0.2 -p 60001 bbbb::1/64")

init()