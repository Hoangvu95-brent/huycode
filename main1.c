#include <wiringPi.h>
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <errno.h>
#include <wiringSerial.h>
int ser;
ser=serialOpen("/dev/ttyS0",9600);
//Serial pc(USBTX, USBRX);
union convert{              
    char Char[4];               
    int Int; 
} temp;

const int OPEN   = 1;
const int CLOSE  = 0;                           // door status
const int OPEN_TIMEOUT  = 2;

const int sizeOfNotifyDoorOpen              = 8;
const int sizeNotifyStartup                 = 5;
const int sizeOfSetupNode                   = 8;
const int sizeOfCheckNodeOnline             = 4;
const int sizeOfCheckNodeOnline_Response    = 5;        //size of packets

const int NOTIFI_DOOR_OPEN = 1;
const int NOTIFI_STARTUP   = 2;                         // packet type
const int CHECK_NODE_ONLINE_RESPONSE = 3;


const char sync             = 0xAA;
const char opCode           = 0x10;
const char dataSize[2]      = {0x00, 0x04};
const char doorId           = 0x01;
int timeOut = 10000;                                    //time out is 10 000ms

bool TURN_ON = 0;                                       
bool TURN_OFF = 1;

wiringPiSetupGpio();
pinMode(17,OUTPUT); //ALARM
pinMode(4,INPUT); //DOOR
pullUpDnControl(4, PUD_UP)

digitalWrite(17,0);
digitalRead(4);

int statusDoor = CLOSE;                                   
int enableAlarm = 1;                                      //default: alarm status is active

void sendData(char *p, int size){                         
    for(int i = 0; i < size; i++){
        serialPutchar(ser,*p);
        serialFlush(ser);
        //pc.printf("%c", *p);
        p++;
    }
}

void sendData(char *p, int size, int delay){
    wait_us(delay);
    for(int i = 0; i < size; i++){
        serialPutchar(ser,*p);
        serialFlush(ser);
        p++;
    }
}

void initData(int packetType, char *p, int timeOpened){
if(packetType == NOTIFI_DOOR_OPEN ){        
            *p = sync;  p++;                      //sync
            *p = opCode;    p++;                  //opCode
            *p = dataSize[0];   p++;              //dataSize1
            *p = dataSize[1];   p++;              //dataSize2
            *p = doorId;        p++;              //doorId

            temp.Int = timeOpened;
            *p = temp.Char[1]; p++;             // duration
            *p = temp.Char[0]; p++;             // duration2

            if(timeOpened > timeOut ){
                *p = 0x01;
            } else {
                *p = 0x00;
            }
    }
}

void initData(int packetType, char *p){
if(packetType == NOTIFI_STARTUP ){        
            *p = sync;  p++;                      //sync
            *p = 0x02;  p++;                      //opCode
            *p = 0x00;  p++;                      //dataSize1
            *p = 0x01;  p++;                      //dataSize2
            *p = 0x01;  p++;                       //doorId
    }


if(packetType == CHECK_NODE_ONLINE_RESPONSE ){ 
            *p = sync;  p++;                      //sync
            *p = 0x05;  p++;                      //opCode check_node_online
            *p = 0x00;  p++;                      //dataSize1
            *p = 0x00;  p++;                      //dataSize2
            *p = 0x01;  p++;                      //doorId  
    }    
}

Timer t;                                          // init timer t;

void runNotifyDoor(){
    
        if(statusDoor == CLOSE  ){
            if(digitalRead(4)){                 
                statusDoor = OPEN;          // if(door open) => statusDoor = OPEN
                //pc.printf("OPEN\r\n");        
                t.start();                  // start read timer
            }  
            digitalWrite(17,TURN_OFF) ;               // door close -> turn off Alarm          
        }
    
        if(statusDoor == OPEN){
            if(digitalRead(4)){
                if(t.read_ms() > timeOut){          // if(door open and time out)
                     char packetSend[sizeOfNotifyDoorOpen];   
                     initData(NOTIFI_DOOR_OPEN, packetSend, t.read_ms());                        
                     sendData(packetSend, sizeOfNotifyDoorOpen);            //send packet NotifyDoorOpen: timeOut                 
                    //pc.printf("TIME OUT\r\n");
                    t.stop();                                               // stop and reset timer  
                    t.reset();    
                    if(enableAlarm == 1){                                    //turn on alarm
                     digitalWrite(17,TURN_ON) ;    
                    }                     
                    statusDoor = OPEN_TIMEOUT;                               // update door status
                    return;
                }
            }

            if(!digitalRead(4)){
                if(t.read_ms() <= timeOut){         //door close - time in
                    char packetSend[sizeOfNotifyDoorOpen];   
                    initData(NOTIFI_DOOR_OPEN, packetSend, t.read_ms());          //send packet open the door            
                    sendData(packetSend, sizeOfNotifyDoorOpen);
                    wait_ms(2000);                    
                    //pc.printf("CLOSE\r\n");
                    statusDoor = CLOSE;
                    t.stop();                                                      // stop and reset timer
                    t.reset();                    
                    return;
                }
            }
        }

        if(statusDoor == OPEN_TIMEOUT){
            if(!digitalRead(4)){
                statusDoor = CLOSE;                                                 //(the door is closed) -> update door status                      
                //pc.printf("CLOSE X\r\n");                                             
            }           

            if(enableAlarm == 1){                                               
                     digitalWrite(17,TURN_ON) ;       
            }
            if(enableAlarm == 0){                                               
                     digitalWrite(17,TURN_OFF) ;       
            }            
        }
}
 


int main() {
    pc.baud(9600);                                  //baurate is 9600
    char packetSend[sizeNotifyStartup];             // init NotifyStartup packet
    initData(NOTIFI_STARTUP, packetSend);           // insert data for packet
    sendData(packetSend, sizeNotifyStartup);        // send NotifyStartup packet for first time
    static Timer t;                                 // init timer t
    digitalWrite(17,TURN_OFF) ;                               // turn off alarm                              

    while(1) {                                       // loop
        runNotifyDoor();                             // check door status
        if(pc.readable()){
        uint8_t array[9] = {0};
        char x =  pc.getc();
        if(int(x) == 0xAA){                           // if Serial availble -> CheckNodeOnline packet or SetupNode packet
            array[0] = x;
            for(int i = 1; i < 8; i++){
                array[i] = pc.getc();
            }
            
            if( (array[1] == 0X01)         // if packet receive is setting SetupNode command
                && (array[2] == 0X00) && (array[3] == 0X04) && (array[4] == 0X01) ){
                enableAlarm = array[7];     // setup alarm stutus: 0 is mute, 1 is active
                timeOut = (int)array[5];
                timeOut = timeOut << 8;
                timeOut += (int)array[6];
                timeOut *= 1000;         // setup timeOut
            } else {
                if( (array[1] == 0X05)         //if packet receive is setting CheckNodeOnline command 
                && (array[2] == 0X00) && (array[3] == 0X00) ){

                char packetSend[sizeOfCheckNodeOnline_Response];     // init CheckNodeOnline Response  packet
                initData(CHECK_NODE_ONLINE_RESPONSE, packetSend);    // insert data for packet
                sendData(packetSend, sizeOfCheckNodeOnline_Response, doorId * 100);     // send CheckNodeOnline Response packet 
                }
            }
        } 
        }
        
    }
}




