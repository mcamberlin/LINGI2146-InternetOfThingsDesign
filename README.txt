To run the project,
0. Place this folder in the contiki-ng
1. Open a terminal and RUN: contiker cooja 
2. Load project_simu.csc in contiki
3. Start simulation
4. Open a NEW terminal and RUN: RUN: cd contiki-ng/LINGI2146-InternetOfThingsDesign/ && python3 init.py
    Changes maybe be needed
    (
        Execute the next command to retrieve the name (<container-name>) of the current docker container running contiki :
            docker ps
        Execute the next command to search the ip address (<container-ip> : 172.17.0.1) of this container:
            docker container inspect <container-name>
    )   
5. Open a new terminal and RUN: gcc -Wall -Werror -o server datagram.h datagram.c server.c -lpthread && ./server