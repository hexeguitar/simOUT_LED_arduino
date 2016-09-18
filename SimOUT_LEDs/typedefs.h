#ifndef TYPEDEFS_H
#define TYPEDEFS_H

#include <Arduino.h>

// Modes of operations
typedef enum 
{
	RECV_IDLE = 0,
	WAIT4LENGTH = 1,
	WAIT4DATA = 2,
  PACKET_CMPLT = 3
}recvState_t;

enum
{
  LED_ON = 1,
  LED_OFF = 2,
  LED_ON_ALL = 3,
  LED_OFF_ALL = 4,
  LED7_DIGIT = 5,
  SHOW_ID = 200,
  HIDE_ID = 201
};

#endif
