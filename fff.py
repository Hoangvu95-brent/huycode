from queue import Queue
import RPi.GPIO as GPIO
import serial
import requests
from time import sleep
from queue import Queue
#status
OPEN   = 1
CLOSE  = 0
OPEN_TIMEOUT  = 2
#Size
sizeOfNotifyDoorOpen              = 8
sizeNotifyStartup                 = 5
sizeOfSetupNode                   = 8
sizeOfCheckNodeOnline             = 4
sizeOfCheckNodeOnline_Response    = 5
#NodeSatatus
NOTIFI_DOOR_OPEN = 1
NOTIFI_STARTUP   = 2
CHECK_NODE_ONLINE_RESPONSE = 3
#Opcode
Signature        = "0xAA"
CMDOpcode        = "0x10"
doorId           = "0x01"
timeOut = 10
TURN_ON = 0
TURN_OFF = 1
########
statusDoor = CLOSE
enableAlarm = 1
start = time.time()
queue = Queue()
ser = serial.Serial ("/dev/ttyS0", 9600)
#################
GPIO.setmode(GPIO.BCM)
GPIO.setup(11, GPIO.IN)#Cam bien tu cua
GPIO.setup(12, GPIO.IN) # cam bien hong ngoai1
GPIO.setup(13, GPIO.IN) # cam bien hong ngoai2
GPIO.setup(19, GPIO.OUT) # chan trang thai cua chuong

##############
#def sendData():
#def sendData1():
#def initData():
#def initData1():
def runNotifyDoor():
    if (statusDoor == CLOSE):
        if (GPIO.input(11)):
            statusDoor = OPEN
            start = time.time()

    if (statusDoor == OPEN):
        if (GPIO.input(11)):
            Time=time.time()-start
            if (Time>timeOut):
                listData=[]
                #initData(NOTIFI_DOOR_OPEN, listData,Time )
                queue.put(listData)
                if (enableAlarm == 1):
                    GPIO.output(19,TURN_ON)
                statusDoor = OPEN_TIMEOUT
        else:
            Time = time.time() - start
            if (Time<timeOut):
                listData = []
                #initData(NOTIFI_DOOR_OPEN, listData, Time)
                queue.put(listData)
                time.sleep(2)
                statusDoor = CLOSE

    if (statusDoor == OPEN_TIMEOUT):
        if (!GPIO.input(11)):
            statusDoor = CLOSE
            if (enableAlarm == 1):
                GPIO.output(19,TURN_OFF)

#def Reset():
    


while True:
    runNotifyDoor()
    data = ser.read()              #read serial port
    sleep(0.03)
    data_left = ser.inWaiting()             #check for remaining byte
    data += ser.read(data_left)
    #print (received_data)                   #print received data
    #sleep(0.5)
    print("received\r")
    #if (data[0]==0xAA):
       # if (data[1]==0x00):
            #reset()
        #elif(data[1]==0x05):
            
        


