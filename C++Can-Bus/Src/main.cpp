#include "stm32l4xx_hal.h"
#include "Can.h"
#include "cmsis_os.h"






//void SystemClock_Config(void);

int main()
{
	/* Configure the system clock */

	osKernelInitialize();    //initialize CMSIS-RTOS
	osKernelStart();  // start execution
	
	Can *test = new Can(1000); // bitrate 1000kbit, 500kbits , default 250kbits 

	while(1)
	{
	test->data[0] = 0x25;
	test->data[1] = 0x25;
	test->data[2] = 0x25;
	test->data[3] = 0x25;
	test->data[4] = 0x16;
	test->data[5] = 0x18;
	test->data[6] = 0x14;
	test->data[7] = 0x12;
  /* start data traffic*/
		
		
  test->sendMsg(test, 8, 0x25);  // sendMsg(Can *test, int Length, uint8_t Id);	
	test->sendMsg(test, 7, 0x14);	
	test->sendMsg(test, 2, 0x89);	
	test->sendMsg(test, 9, 0x59);
	test->receiveMsg(test);	
	//HAL_Delay(50);
	}
	
	
	
	
}

