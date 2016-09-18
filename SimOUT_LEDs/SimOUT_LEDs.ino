/*
	#############################################################
	#                   simOUT LED Driver                       #
	#                 for M32U4 based Arduino                   #                   
	#                  (c) 2016 Piotr Zapart                    #
	#############################################################

*/
#include "typedefs.h"
#include <TimerOne.h> //https://github.com/PaulStoffregen/TimerOne
#include <avr/pgmspace.h>

/*
	Master module receives the data from USB/HID serial port (Serial1)
	and pases it through to hardware Serial to be sent to other slave modules.
	Only M32U4 based can be used as master.
	
	Slave module receives data from stock Serial port only. 
	
	Comment out the MASTER_MODULE to set the module to master, 
	comment it to set the modula as slave
*/

#define MASTER_MODULE

/*
   ### SEGMENT CONNECTIONS ###
   connect through 220-470R resistors
   A = PB4 = D8
   B = PE6 = D7
   C = PB3 = D14
   D = PB1 = D15
   E = PF7 = D18
   F = PB5 = D9
   G = PB6 = D10
   DP= PB2 = D16
   
   ### ANODE DRIVERS ###
   Pin -> 10k-PNP base
   PNP emitter -> VCC
   PNP collector -> 7 segment display common anode
   
   Digit 0-4, left to right
   Digit0 = D6
   Digit1 = D5
   Digit2 = D4
   Digit3 = D3
   Digit4 = D2

*/

// Device ID address
const uint8_t devAddress = 21;

void processData(uint8_t dataIn);
void initPorts(void);
void displayNumber(uint8_t number, volatile uint8_t *bfPtr);
void allOn(void);
void allOff(void);

const uint8_t digitArray[10] PROGMEM = {0x3F,0x06,0x5B,0x4F,0x66,0x6D,0x7D,0x07,0xFF,0x6F};
const int numOfDigits = 5;
int c_anodeDrivers[numOfDigits] = {2,3,4,5,6};
int ledDrivers[8] = {8,7,14,15,18,9,10,16};
static volatile uint8_t ledState[numOfDigits] = {0x00,0x00,0x00,0x00,0x00};
static volatile uint8_t addrBuff[numOfDigits] = {0x00,0x00,0x00,0x00,0x00};
static volatile uint8_t *ledStatePtr = ledState;


// ###########################################################
void setup() 
{
	initPorts();
	
	Serial.begin(57600,SERIAL_8N2);
#if defined MASTER_MODULE	
    Serial1.begin(57600,SERIAL_8N2);
#endif
	
    Timer1.initialize(1250);
	Timer1.attachInterrupt(ledMux);
	
	displayNumber(devAddress,addrBuff);
	
}
// ###########################################################
void loop() 
{
  if (Serial.available())
  {	  	  
      processData(Serial.read());
  }
}
// ###########################################################
void processData(uint8_t dataIn)
{
    static recvState_t state = RECV_IDLE;
    static uint8_t incBytes = 0, index = 0;    //number of incoming data bytes
    static uint8_t dataBuffer[5];
    uint8_t i;
	
#if defined MASTER_MODULE	
    Serial1.write(dataIn);    //pass the data to Serial1 over, only for Master module
#endif

    switch (state)
    {
        case RECV_IDLE:
                        if ((dataIn == devAddress)||(dataIn==0xFF)) state = WAIT4LENGTH;
                        break;
        case WAIT4LENGTH:
                        incBytes = dataIn;
                        state = WAIT4DATA;
                        break;
        case WAIT4DATA:
                        dataBuffer[index++] = dataIn;
                        if (index == incBytes)  
						{
							state = PACKET_CMPLT;
							index = 0;							
						}	
						
                        break;
       case PACKET_CMPLT: 
                        break;
    }
    // Process data stored in buffer
    if (state == PACKET_CMPLT)
    {
		switch (dataBuffer[0])						//command
		{
			case LED_ON:
						bitSet(ledState[(dataBuffer[1]>>3)],(dataBuffer[1]%8));
						break;
			case LED_OFF:
						bitClear(ledState[(dataBuffer[1]>>3)],(dataBuffer[1]%8));
						break;
			case LED_ON_ALL:
						allOn();
						break;
			case LED_OFF_ALL:
						allOff();
						break;
			case LED7_DIGIT:
						ledState[dataBuffer[1]] = dataBuffer[2];
						break;
			case SHOW_ID:
						ledStatePtr = addrBuff;
						break;
			case HIDE_ID:
						ledStatePtr = ledState;
						break;						
		}
        state = RECV_IDLE;
    }  
}
// ###########################################################
void ledMux(void)
{
	static volatile uint8_t anodeCnt=4;
	uint8_t i;
	
	digitalWrite(c_anodeDrivers[anodeCnt++],HIGH);
	if (anodeCnt >= numOfDigits)	anodeCnt = 0;
	
	for (i=0;i<8;i++)
	{
		if (ledStatePtr[anodeCnt] & (1<<i))	digitalWrite(ledDrivers[i],LOW);
		else								digitalWrite(ledDrivers[i],HIGH);
	}
	// anode switch	
	digitalWrite(c_anodeDrivers[anodeCnt],LOW);
}
// ###########################################################
void initPorts(void)
{
	uint8_t i;
	
	for (i=0;i<8;i++)
	{
		pinMode(ledDrivers[i],OUTPUT);
	}
	for (i=0;i<numOfDigits;i++)
	{
		pinMode(c_anodeDrivers[i],OUTPUT);
	}
}
// ###########################################################
void displayNumber(uint8_t number, volatile uint8_t *bfPtr)
{
	uint8_t i = 0;
	uint8_t digit = 0;
	
	for (i=0;i<numOfDigits;i++)
	{
		bfPtr[i] = 0x00;
	}
	i=0;
	while (i<3)
	{
		digit = number%10;
		if (i && digit==0)
		{
			bfPtr[i] = 0x00;
		}
		else
		{
			bfPtr[i] = pgm_read_byte_near(&digitArray[digit]);
		}
		number /= 10;
		i++;
	}
}
// ###########################################################
void allOn(void)
{
	uint8_t i;
	for (i=0;i<numOfDigits;i++)
	{
		ledState[i] = 0xFF;
	}
}
// ###########################################################
void allOff(void)
{
	uint8_t i;
	for (i=0;i<numOfDigits;i++)
	{
		ledState[i] = 0x00;
	}	
}
// ###########################################################
