#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <errno.h>
#include <wiringSerial.h>
#include <wiringPi.h>
#include <time.h>
#include <stdbool.h>
#include <iostream>
#include <cstdio>

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
const int token    							= 3;
const int NOTIFI_DOOR_OPEN = 1;
const int NOTIFI_STARTUP   = 2;                         // packet type
const int CHECK_NODE_ONLINE_RESPONSE = 3;
const int RES    		   = 4;


const char sync             = 0x69;
const char opCode           = 0x10;
const char dataSize[2]      = {0x00, 0x04};
const char doorId           = 0x01;
int timeOut = 10000;                                    //time out is 10 000ms

bool TURN_ON = 0;                                       
bool TURN_OFF = 1;
///////////////////////////////////////
typedef int item; //kieu du lieu
struct Node
{
    item Data;
    Node * Next;
};
struct Queue
{
    Node * Front, *Rear; //Node dau va Node cuoi
    int count; //dem so phan tu
};
 
void Init (Queue &Q); //khoi tao Queue rong
int Isempty(Queue Q); //kiem tra Queue rong
void Push(Queue &Q, item x); //them phan tu vao cuoi hang doi
int Pop(Queue &Q); //Loai bo phan tu khoi dau hang doi
int Qfront (Queue Q); //xem thong tin phan tu dau hang doi 
Node *MakeNode(item x); //tao 1 Node
void Input (Queue &Q); //Nhap 
void Output(Queue Q); //Xuat 
 
void Init(Queue &Q)
{
    Q.Front = Q.Rear = NULL;
    Q.count = 0;
}
int Isempty (Queue Q) //kiem tra Queue rong
{
    if (Q.count == 0) //so phan tu = 0 => rong
        return 1;
    return 0;
}
 
Node *MakeNode(item x) //tao 1 Node
{
    Node *P = (Node*) malloc(sizeof(Node));
    P->Next = NULL;
    P->Data = x;
    return P;
}
 
void Push(Queue &Q, item x) //them phan tu vao cuoi Queue
{
    Node *P = MakeNode(x); //Neu Q rong
    if (Isempty(Q))
    {
        Q.Front = Q.Rear = P; //dau va cuoi deu tro den P
    }
    else //Khong rong
    { 
        Q.Rear->Next = P;
        Q.Rear = P;
    }
    Q.count ++ ; //tang so phan tu len
}
 
int Pop(Queue &Q) //Loai bo phan tu khoi dau hang doi
{
    if (Isempty(Q)) 
    {
        printf("Hang doi rong !");
        return 0;
    }
    else
    {
        item x = Q.Front->Data;
        if (Q.count == 1) //neu co 1 phan tu
            Init(Q);
        else
            Q.Front = Q.Front->Next;
        Q.count --;
        return x; //tra ve phan tu lay ra
    }
}
 

 

///////////////////////////////////////
void initGpio(){
    wiringPiSetupGpio();
    pinMode(17,OUTPUT); //ALARM
    pinMode(4,INPUT); //DOOR
    pullUpDnControl(4, PUD_UP);

}


int statusDoor = CLOSE;                                   
int enableAlarm = 1;                                      //default: alarm status is active

void sendData(char *p, int size){  
    int ser;
    if((ser=serialOpen("/dev/ttyS0",9600))<0){
        fprintf(stderr,"Unable to open serial device: %s\n",strerror(errno));

    }                       
    for(int i = 0; i < size; i++){
        serialPutchar(ser,*p);
        serialFlush(ser);
        p++;
    }
}

void sendData1(char *p, int size, int delay1){
    delay(delay1);
    int ser;
    if((ser=serialOpen("/dev/ttyS0",9600))<0){
        fprintf(stderr,"Unable to open serial device: %s\n",strerror(errno));

    }                       
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

void initData1(int packetType, char *p){
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
	if(packetType == RES){
				*p = syns;p++;
				*p = 0x04;p++
				*p = 0x00;p++;
	}
}


void runNotifyDoor(){
    clock_t start_t, end_t, total_t;                                      // init timer t;
        if(statusDoor == CLOSE  ){
            if(digitalRead(4)){                 
                statusDoor = OPEN;          // if(door open) => statusDoor = OPEN
                //pc.printf("OPEN\r\n");        
                start_t=clock();                  // start read timer
            }  
            digitalWrite(17,1) ;               // door close -> turn off Alarm          
        }
    
        if(statusDoor == OPEN){
            if(digitalRead(4)){
                end_t=clock();
                total_t = ((double)(end_t - start_t) / CLOCKS_PER_SEC)*1000;
                if(total_t> timeOut){          // if(door open and time out)
                	 Push(Q,total_t);
                     char packetSend[sizeOfNotifyDoorOpen];   
                     initData(NOTIFI_DOOR_OPEN, packetSend,total_t);                        
                     sendData(packetSend, sizeOfNotifyDoorOpen);            //send packet NotifyDoorOpen: timeOut                 
                    //pc.printf("TIME OUT\r\n");
                    //t.stop();                                               // stop and reset timer  
                    //t.reset();    
                    if(enableAlarm == 1){                                    //turn on alarm
                     digitalWrite(17,0) ;    
                    }                     
                    statusDoor = OPEN_TIMEOUT;                               // update door status
                    return;
                }
            }

            if(!digitalRead(4)){
                end_t=clock();
                total_t = ((double)(end_t - start_t) / CLOCKS_PER_SEC)*1000;
                if(total_t<= timeOut){         //door close - time in
                	Push(Q,total_t);
                    char packetSend[sizeOfNotifyDoorOpen];   
                    initData(NOTIFI_DOOR_OPEN, packetSend,total_t);          //send packet open the door            
                    sendData(packetSend, sizeOfNotifyDoorOpen);
                    delay(2000); //delay 2s                    
                    //pc.printf("CLOSE\r\n");
                    statusDoor = CLOSE;
                    //t.stop();                                                      // stop and reset timer
                    //t.reset();                    
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
                     digitalWrite(17,0) ;       
            }
            if(enableAlarm == 0){                                               
                     digitalWrite(17,1) ;       
            }            
        }
}



int main() {
	initGpio();
	Queue Q;
    Init(Q);
    //char packetSend[sizeNotifyStartup];             // init NotifyStartup packet
    //initData1(NOTIFI_STARTUP, packetSend);           // insert data for packet
    //sendData(packetSend, sizeNotifyStartup);        // send NotifyStartup packet for first time                              // init timer t
    digitalWrite(17,TURN_OFF) ;                               // turn off alarm   
    int ser,timed;     
    while(1) {                                       // loop
        runNotifyDoor(Q);                             // check door status 
        if((ser=serialOpen("/dev/ttyS0",9600))<0){
            fprintf(stderr,"Unable to open serial device: %s\n",strerror(errno));
        }else{
            u_int8_t array[9]={0};
            char c = serialGetchar(ser);
            if(c==0x69){
                array[0] = c;
                for(int i = 1; i < 8; i++){
                    array[i] = serialGetchar(ser);
                }
                if (array[1]=0x04 and array[2]==1){
                	if (Isempty(Q)){
                		char packetSend[token];   
	                    initData(RES, packetSend);          //send packet open the door            
	                    sendData(packetSend, token);
                		
                	}else{
	                	timed=Pop(Q);
	                	char packetSend[sizeOfNotifyDoorOpen];   
	                    initData(NOTIFI_DOOR_OPEN, packetSend,timed);          //send packet open the door            
	                    sendData(packetSend, sizeOfNotifyDoorOpen);
                    }
                }

                /*	
                if( (array[1] == 0X06)         // if packet receive is setting SetupNode command
                    && (array[2] == 0X00) && (array[3] == 0X04) && (array[4] == 0X01) ){
                    enableAlarm = array[7];     // setup alarm stutus: 0 is mute, 1 is active
                    timeOut = (int)array[5];
                    timeOut = timeOut << 8;
                    timeOut += (int)array[6];
                    timeOut *= 1000;         // setup timeOut
                } /*else {
                    if( (array[1] == 0X05)         //if packet receive is setting CheckNodeOnline command 
                    && (array[2] == 0X00) && (array[3] == 0X00) ){

                    char packetSend[sizeOfCheckNodeOnline_Response];     // init CheckNodeOnline Response  packet
                    initData1(CHECK_NODE_ONLINE_RESPONSE, packetSend);    // insert data for packet
                    sendData1(packetSend, sizeOfCheckNodeOnline_Response, doorId );     // send CheckNodeOnline Response packet 
                    }*/
            }
        } 
    }

    
}                     
        

            
    

