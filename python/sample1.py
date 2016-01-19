import pySerialComm

import threading
import time

pccnt = 0

def test(messageid, data):
    global pccnt
    values = ard.parsedata("i", data)
    print("<- Request received from arduino : %s" % values[0])
    print("-> Sending the integer to arduino : %s" % pccnt)
    ard.sendack(messageid, (pccnt, ))
    pccnt = pccnt + 1
    if pccnt == 32767:
        pccnt = 0
    

ard = pySerialComm.SerialComm('/dev/ttyUSB0', baudrate=115200)

ard.attach(2, test)

thread = threading.Thread(target=ard.listenner, args=())
thread.daemon = True                            # Daemonize thread
thread.start()                                  # Start the execution


for i in range(0, 5):
    try:
        print("-> Sending an integer and a string to arduino")
        resp = ard.sendmessage(2, (i,"This is a string"), ack=True)
        values = ard.parsedata("is", resp)
        print("<- Ack contains two values : %s, %s" % (values[0], values[1]))
        time.sleep(1)
    except TimeoutError:
        print("No ack received")
    

ard.stop()
thread.join()
print("---End of script")
