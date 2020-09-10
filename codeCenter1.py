'''
UART communication on Raspberry Pi using Pyhton
http://www.electronicwings.com
'''
import serial
import requests
from time import sleep
#import paho.mqtt.client as mqtt
import json

sync = "69"
NOTYFI_DOOR_OPEN = bytearray.fromhex("69 10 00 04 01")
NOTIFY_STARTUP = bytearray.fromhex("69 02 00 01 01")
CHECK_NODE_ONLINE_RESPONSE = bytearray.fromhex("69 05 00 00 01")
TEST = bytearray.fromhex("00 01 02 03")
SETUP = 0

SETUP_NODE = bytearray.fromhex("69 01 00 04 00 00 00 00 00")
SETUP_NODE_A = bytearray.fromhex("69 01 00 04 00 00 00 00 00")
CHECK_NODE_ONLINE = bytearray.fromhex("69 05 00 00")
TOKEN=bytearray.fromhex("69 04 00 00")
ser = serial.Serial ("/dev/ttyS0", 9600)    #Open port with baud rate
def token(key):
    TOKEN[2]=key
def on_connect(client, userdata, flags, rc):
    print("Connected with result code "+str(rc))

    # Subscribing in on_connect() means that if we lose the connection and
    # reconnect then subscriptions will be renewed.
    client.subscribe("NODE_SETTING")
    
def on_message(client, userdata, msg):
    #if (msg.startswith('[NODE]')):
    print(msg.topic+" "+str(msg.payload))
    #data_out=json.dumps(str(msg.payload))    
    objData = json.loads(msg.payload)
#     print(objData['doorId'])
#     print(objData['enableAlarm'])
#     print(objData['duration'])
    SETUP_NODE[4] = objData['doorId']
    SETUP_NODE[6] = objData['duration'] % 256
    SETUP_NODE[5] = 0
    #time = objData['duration']
    if(objData['duration'] > 256):
        SETUP_NODE[5] = int(objData['duration'] / 256)
        print(bin(objData['duration']))
    if (objData['type']==1):
        SETUP_NODE[1] = 6
        SETUP_NODE[7] = objData['enableAlarm']
    elif(objData['type']==2):
        SETUP_NODE[1] = 8
        SETUP_NODE[7] = objData['speaker']
    #ser.write(bytearray.fromhex(str('12')))
    ser.write(SETUP_NODE)
    ser.flush()
    #sleep(10)
    return
    #print(objData['doorId'])
    
    
    #client.publish("nodes_server", "[SERVER] sent from python " + str(msg.payload))
    

        
def receiveNotifyStartup(data):
    print("Node startup!\r")
    print("Door ID is: ")
    print(data[4])
    print("\r\n")
    
    
def checkNodeOnlineResponse(data):
    print("Node online!\r")
    print("Door ID is: ")
    print(data[4])
    print("\r\n")
    
def notifyDoor(dataMessenger):
    time = int(dataMessenger[5])
    time = time << 8
    time += int(dataMessenger[6])
    
    
    print("Open the door!\r")
    print("Door ID is: ")
    print(dataMessenger[4])
    print("\r")
    print("time is: ")
    print(time)
    print("\r")

    url = "http://10.116.226.36:5000/api/dooropens"

    payload = "{\r\n  \"doorId\":"
    payload += str(dataMessenger[4])
    payload += ",\r\n  \"duration\": "
    payload += str(time)
    payload += ",\r\n  \"isOverTimeOut\": "
    if(dataMessenger[7] == 1):
        print("timeOut true")
        payload += 'true'
    else:
        print("timeOut false")
        payload += 'false'    
    payload += "\r\n}\r\n"

#     payload = "{\r\n  \"doorId\":1,\r\n  \"duration\": 4,\r\n  \"isOverTimeOut\": true\r\n}\r\n"
    
    headers = {
      'Content-Type': 'application/json'
    }
    
    print('url is: ')
    print(url)
    print('data is: ')
    print(payload)
    
    '''response = requests.request("POST", url, headers=headers, data = payload)

    print('Response is: ')
    print(response.text.encode('utf8'))
    return'''
'''
client = mqtt.Client()
client.on_connect = on_connect
client.on_message = on_message
    #print(client.on_message)

client.connect("10.116.226.36", 1883, 60)
    #client.loop_forever()
client.loop_start()'''

global key
key=1
'''token(key)
ser.write(TOKEN)
ser.flush()'''
token(key)
ser.write(TOKEN)
ser.flush()
while True:
    sleep(0.3) 
    received_data = ser.read()              #read serial port
    sleep(0.03)
    data_left = ser.inWaiting()             #check for remaining byte
    received_data += ser.read(data_left)
    print(received_data)
    if((received_data[0]==105 and received_data[1] == 4 and received_data[2]==0) or (received_data[0]==105 and received_data[1] == 17 and received_data[2]==1)):
        key=key+1
        print("vao 1")
        if (key>2):
            key=1
        token(key)
        sleep(0.3)
        ser.write(TOKEN)
        print(TOKEN)
        ser.flush()
    if (received_data[0]==105 and received_data[1] == 16):
        notifyDoor(received_data)
        print("vao 1")

    '''N=int(input("Moi ban nhap "))
    if (N==1):
        SETUP_NODE_A[4]=1
        SETUP_NODE_A[5]=int(23/256)
        SETUP_NODE_A[6]=int(23%256)
        SETUP_NODE_A[7]=1
        ser.write(SETUP_NODE_A)
        ser.flush()'''
        
    '''
    for i in range(1,3):
        received_data = ser.read()              #read serial port
        sleep(0.03)
        data_left = ser.inWaiting()             #check for remaining byte
        received_data += ser.read(data_left)
        #print (received_data)                   #print received data
        #sleep(0.5)    
        print("received\r")
        token(i)
        
        if((received_data[0] == 105) and (received_data[1] == 2)):
            receiveNotifyStartup(received_data)
    #         lap = [5,4,3,2,1]
    #         for i in lap:    
    #             print(i)
    #             sleep(1)
    #         
    #         ser.write(CHECK_NODE_ONLINE)
    #         print("check node online\r")
            
        if((received_data[0] == 105) and (received_data[1] == 5)):
            checkNodeOnlineResponse(received_data)
    #         lap = [5,4,3,2,1]
    #         for i in lap:    
    #             print(i)
    #             sleep(1)
    #         
    #         ser.write(SETUP_NODE)
    #         print("setup node\r")
            
        if((received_data[0] == 105) and (received_data[1] == 16)):
            notifyDoor(received_data)
            

        #client.loop_forever()'''

