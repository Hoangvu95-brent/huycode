// Compile : gcc -Wall receive.c -o receive -lwiringPi
 
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <errno.h>
#include <wiringPi.h>
#include <wiringSerial.h>
 
int main() {
 
	int fd;
	char c;
	printf("Raspberry's receiving : \n");
	wiringPiSetupGpio();
	pinMode(17, INPUT);
	while(1) {
		if((fd = serialOpen ("/dev/ttyS0", 9600)) < 0 ){
			fprintf (stderr, "Unable to open serial device: %s\n", strerror (errno)) ;
		}else{
			do{
				c = serialGetchar(fd);
				//delay(100);
				printf("%c",c);
				fflush (stdout);
			}while(serialDataAvail(fd));
			//printf("\n");
		}
	}
	return 0;
}
