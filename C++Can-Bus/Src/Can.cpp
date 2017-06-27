#include "Can.h"
#include "cmsis_os.h"
#define osObjectsPublic
#include "osObjects.h"



CAN_HandleTypeDef hcan1;  // CAN1 object

CanRxMsgTypeDef RxMessage;
CanTxMsgTypeDef TxMessage;

//osThreadId tid_Thread;
//Can thread;


Can::Can(){m_bitrate = 0;}
Can::Can(int bitrate)
{
	
	m_bitrate = bitrate;
	
	HAL_Init();  // init all
  SystemCoreClockUpdate();  
	
	/*                SystemClock_config                 */
	RCC_OscInitTypeDef RCC_OscInitStruct;
  RCC_ClkInitTypeDef RCC_ClkInitStruct;

    /**Initializes the CPU, AHB and APB busses clocks 
    */
  RCC_OscInitStruct.OscillatorType = RCC_OSCILLATORTYPE_HSI;
  RCC_OscInitStruct.HSIState = RCC_HSI_ON;
  RCC_OscInitStruct.HSICalibrationValue = 16;
  RCC_OscInitStruct.PLL.PLLState = RCC_PLL_ON;
  RCC_OscInitStruct.PLL.PLLSource = RCC_PLLSOURCE_HSI;
  RCC_OscInitStruct.PLL.PLLM = 1;
  RCC_OscInitStruct.PLL.PLLN = 10;
  RCC_OscInitStruct.PLL.PLLP = RCC_PLLP_DIV7;
  RCC_OscInitStruct.PLL.PLLQ = RCC_PLLQ_DIV2;
  RCC_OscInitStruct.PLL.PLLR = RCC_PLLR_DIV2;
  if (HAL_RCC_OscConfig(&RCC_OscInitStruct) != HAL_OK)
  {
    __NVIC_SystemReset();
  }

    /**Initializes the CPU, AHB and APB busses clocks 
    */
  RCC_ClkInitStruct.ClockType = RCC_CLOCKTYPE_HCLK|RCC_CLOCKTYPE_SYSCLK
                              |RCC_CLOCKTYPE_PCLK1|RCC_CLOCKTYPE_PCLK2;
  RCC_ClkInitStruct.SYSCLKSource = RCC_SYSCLKSOURCE_PLLCLK;
  RCC_ClkInitStruct.AHBCLKDivider = RCC_SYSCLK_DIV1;
  RCC_ClkInitStruct.APB1CLKDivider = RCC_HCLK_DIV2;
  RCC_ClkInitStruct.APB2CLKDivider = RCC_HCLK_DIV1;

  if (HAL_RCC_ClockConfig(&RCC_ClkInitStruct, FLASH_LATENCY_4) != HAL_OK)
  {
   __NVIC_SystemReset();
  }

    /**Configure the main internal regulator output voltage 
    */
  if (HAL_PWREx_ControlVoltageScaling(PWR_REGULATOR_VOLTAGE_SCALE1) != HAL_OK)
  {
   __NVIC_SystemReset();
  }

    /**Configure the Systick interrupt time 
    */
  HAL_SYSTICK_Config(HAL_RCC_GetHCLKFreq()/1000);

    /**Configure the Systick 
    */
  HAL_SYSTICK_CLKSourceConfig(SYSTICK_CLKSOURCE_HCLK);

  /* SysTick_IRQn interrupt configuration */
  HAL_NVIC_SetPriority(SysTick_IRQn, 0, 0);
	
	/* GPIO Init*/
	 __HAL_RCC_GPIOA_CLK_ENABLE();
	  
	  HAL_GPIO_WritePin(GPIOA, GPIO_PIN_5, GPIO_PIN_RESET);
	  GPIO_InitTypeDef GPIO_InitLED;
	  GPIO_InitLED.Pin = GPIO_PIN_5;
    GPIO_InitLED.Mode = GPIO_MODE_OUTPUT_PP;
    GPIO_InitLED.Pull = GPIO_NOPULL;
    GPIO_InitLED.Speed = GPIO_SPEED_FREQ_VERY_HIGH;
    HAL_GPIO_Init(GPIOA, &GPIO_InitLED);

		/*  CAN Init */
		
	CAN_FilterConfTypeDef  sFilterConfig;	
  hcan1.pTxMsg = &TxMessage;
  hcan1.pRxMsg = &RxMessage;
	hcan1.Instance = CAN1;
  hcan1.Init.Mode = CAN_MODE_NORMAL;
	
	/* 1 Mbit bitrate */
	if(m_bitrate == 1000)
	{
	hcan1.Init.Prescaler = 4;
  hcan1.Init.BS1 = CAN_BS1_8TQ;
  hcan1.Init.BS2 = CAN_BS2_1TQ;
	}
	/* 500 kbit bitrate */
	else if(m_bitrate == 500)
	{	
	hcan1.Init.Prescaler = 5;
  hcan1.Init.BS1 = CAN_BS1_13TQ;
  hcan1.Init.BS2 = CAN_BS2_2TQ;
	}
	/* 250 kbit bitrate*/
	else
	{
	hcan1.Init.Prescaler = 10;	
  hcan1.Init.BS1 = CAN_BS1_13TQ;
  hcan1.Init.BS2 = CAN_BS2_2TQ;
	}
	hcan1.Init.SJW = CAN_SJW_1TQ;
  hcan1.Init.TTCM = DISABLE;
  hcan1.Init.ABOM = DISABLE;
  hcan1.Init.AWUM = DISABLE;
  hcan1.Init.NART = DISABLE;
  hcan1.Init.RFLM = DISABLE;
  hcan1.Init.TXFP = DISABLE;
	
	__HAL_RCC_CAN1_CLK_ENABLE();
  
	  GPIO_InitTypeDef GPIO_InitStruct;
    /**CAN1 GPIO Configuration    
    PA11     ------> CAN1_RX
    PA12     ------> CAN1_TX 
    */
    GPIO_InitStruct.Pin = GPIO_PIN_11|GPIO_PIN_12;
    GPIO_InitStruct.Mode = GPIO_MODE_AF_PP;
    GPIO_InitStruct.Pull = GPIO_NOPULL;
    GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_VERY_HIGH;
    GPIO_InitStruct.Alternate = GPIO_AF9_CAN1;
    HAL_GPIO_Init(GPIOA, &GPIO_InitStruct);
  if (HAL_CAN_Init(&hcan1) != HAL_OK)
  {
    	__NVIC_SystemReset();
  }
	
	

     /*##-2- Configure the CAN Filter ###########################################*/
    sFilterConfig.FilterNumber = 0;
    sFilterConfig.FilterMode = CAN_FILTERMODE_IDMASK;
    sFilterConfig.FilterScale = CAN_FILTERSCALE_32BIT;
    sFilterConfig.FilterIdHigh = 0x0000;
    sFilterConfig.FilterIdLow = 0x0000;
    sFilterConfig.FilterMaskIdHigh = 0x0000;
    sFilterConfig.FilterMaskIdLow = 0x0000;
    sFilterConfig.FilterFIFOAssignment = 0;
    sFilterConfig.FilterActivation = ENABLE;
    sFilterConfig.BankNumber =14;
    HAL_CAN_ConfigFilter(&hcan1, &sFilterConfig);
  if(HAL_CAN_ConfigFilter(&hcan1, &sFilterConfig) != HAL_OK)
  {
    /* Filter configuration Error */
   	__NVIC_SystemReset();
  }
	
}


void Can::sendMsg(Can *CANMessage, int lenght, uint8_t id)
//void Can::sendData()
{
	
	hcan1.pTxMsg->StdId = id;
  hcan1.pTxMsg->ExtId = 0;
  hcan1.pTxMsg->RTR = CAN_RTR_DATA;
  hcan1.pTxMsg->IDE = CAN_ID_STD;
  hcan1.pTxMsg->DLC = lenght;     // data Lenght
	
	
	//hcan1.pTxMsg->Data[1] = 0x25;
	for(int i=0; i < lenght; i++)
	{
		hcan1.pTxMsg->Data[i] = data[i];
	}
	
	
	HAL_CAN_Transmit(&hcan1,100);
	//HAL_CAN_Receive(&hcan1, CAN_FIFO0, 1);
	
}

void Can::receiveMsg(Can *CANMessage)
{
	volatile uint16_t datarx[8];
	
  if(__HAL_CAN_MSG_PENDING(&hcan1, CAN_FILTER_FIFO0)) // check pending bit 
	{
    HAL_CAN_Receive(&hcan1, CAN_FIFO0, 1);
		HAL_GPIO_TogglePin(GPIOA, GPIO_PIN_5);
		datarx[0]=hcan1.pRxMsg->StdId ;
    datarx[1] =hcan1.pRxMsg->DLC;
    datarx[2]= hcan1.pRxMsg->RTR ;
    datarx[3]= hcan1.pRxMsg->DLC ;
	  for(int i = 0; i < hcan1.pRxMsg->DLC; i++)
	   {
      //datarx[i]= hcan1.pRxMsg->Data[i];
			CANMessage->data[i] = hcan1.pRxMsg->Data[i];
	   }
			//osDelay(50);
	 }
	
}
//void Can::job(void const *arg)
//{
//	

//}