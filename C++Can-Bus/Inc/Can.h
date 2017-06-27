

#ifndef __CAN_H
#define __CAN_H

#include "stm32l4xx_hal.h"
#include <cmsis_os.h>

class Can
{
	
		int m_bitrate;	
	public:
	
		Can();
	  Can(int bitrate);
	  uint8_t data[8];
	  void sendMsg(Can *CANMessage, int lenght, uint8_t id);
	  void receiveMsg(Can *CANMessage);		
		//static void job(void const *arg);
};

#endif

