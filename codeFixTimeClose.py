from queue import Queue
import RPi.GPIO as GPIO
import serial
import requests
import time
from time import sleep
from queue import Queue

SETUP_NODE = bytearray.fromhex("69 10 00 04 01 00 00 00 00")
RES_TOKEN = bytearray.fromhex("69 04 00")
RES_STATUS = bytearray.fromhex("69 11 01")

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
queue = [ ]
#queue.append(SETUP_NODE)
ser = serial.Serial ("/dev/ttyS0", 9600)
#################

GPIO.setwarnings(False)
GPIO.setmode(GPIO.BCM)
GPIO.setup(4, GPIO.IN, pull_up_down=GPIO.PUD_UP)#Cam bien tu cua
#GPIO.setup(12, GPIO.IN) # cam bien hong ngoai1
#GPIO.setup(13, GPIO.IN) # cam bien hong ngoai2
GPIO.setup(17, GPIO.OUT) # chan trang thai cua chuong

##############
#def sendData():
#def sendData1():
#def initData():
#def initData1():
def runNotifyDoor():
    global OPEN   
    global CLOSE  
    global OPEN_TIMEOUT
    global timeOut 
    global TURN_ON 
    global TURN_OFF 
########
    global statusDoor
    global enableAlarm
    global start
    global listq
    if (statusDoor == CLOSE):
        if (GPIO.input(4)==1):
            statusDoor = OPEN
            start = time.time()

    if (statusDoor == OPEN):
        if (GPIO.input(4)==1):
            Time=time.time()-start
            if (int(Time)>timeOut):
                listData=[]
                
                #initData(NOTIFI_DOOR_OPEN, listData,Time )
                SETUP_NODE[6]=int(Time%256)
                SETUP_NODE[5]=int(Time/256)
                SETUP_NODE[7]=1
                #ser.write(SETUP_NODE)
                #ser.flush()
                print("Dong qua", Time)
                queue.append(SETUP_NODE)
                if (enableAlarm == 1):
                    GPIO.output(17,TURN_ON)
                statusDoor = OPEN_TIMEOUT
                #statusDoor = CLOSE
                #time.sleep(1)
        else :
        #if (GPIO.input(4)==0):
            Time = time.time() - start
            if (int(Time)<timeOut):
                listData = []
                #initData(NOTIFI_DOOR_OPEN, listData, Time)
                
                SETUP_NODE[6]=int(Time%256)
                SETUP_NODE[5]=int(Time/256)
                SETUP_NODE[7]=0
                #ser.write(SETUP_NODE)
                #ser.flush()
                print("Dong truoc", Time)
                queue.append(SETUP_NODE)
                time.sleep(1)
                statusDoor = CLOSE

    if (statusDoor == OPEN_TIMEOUT):
        if (GPIO.input(4)==0):
            statusDoor = CLOSE
            if (enableAlarm == 1):
                GPIO.output(17,TURN_OFF)

#def Reset():
GPIO.output(17,TURN_OFF)
print(queue)
while True:
    runNotifyDoor()
    time.sleep(0.01)
    #print(queue)
    data = ser.read()              #read serial port
    sleep(0.03)
    data_left = ser.inWaiting()             #check for remaining byte
    data += ser.read(data_left)
    #print (received_data)                   #print received data
    #sleep(0.5)
    #if (data[0]==0xAA):
       # if (data[1]==0x00):
            #reset()
        #elif(data[1]==0x05):'''
    if(data[0]==105 and data[1]==4 and data[3]==1):
        if (len(queue)!=0):
            RES_TOKEN[2]=1
            ser.write(RES_TOKEN)
            ser.flush()
            time.sleep(0.03)
            ser.write(queue[0])
            ser.flush()
            del queue[0]
            time.sleep(0.03)
            ser.write(RES_STATUS)
            ser.flush()
        else:
            RES_TOKEN[2]=0
            ser.write(RES_TOKEN)
            ser.flush()
            
            
        


