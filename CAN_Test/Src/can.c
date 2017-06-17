/**
  ******************************************************************************
  * File Name          : CAN.c
  * Description        : This file provides code for the configuration
  *                      of the CAN instances.
  ******************************************************************************
  *
  * COPYRIGHT(c) 2016 STMicroelectronics
  *
  * Redistribution and use in source and binary forms, with or without modification,
  * are permitted provided that the following conditions are met:
  *   1. Redistributions of source code must retain the above copyright notice,
  *      this list of conditions and the following disclaimer.
  *   2. Redistributions in binary form must reproduce the above copyright notice,
  *      this list of conditions and the following disclaimer in the documentation
  *      and/or other materials provided with the distribution.
  *   3. Neither the name of STMicroelectronics nor the names of its contributors
  *      may be used to endorse or promote products derived from this software
  *      without specific prior written permission.
  *
  * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
  * AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
  * IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
  * DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE LIABLE
  * FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL
  * DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR
  * SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER
  * CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY,
  * OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE
  * OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
  *
  ******************************************************************************
  */

/* Includes ------------------------------------------------------------------*/
#include "can.h"

#include "gpio.h"

/* USER CODE BEGIN 0 */

/* USER CODE END 0 */
uint8_t CAN1dataReceivedFlag;


CAN_HandleTypeDef hcan1;
CanTxMsgTypeDef TxMessage; 
CanRxMsgTypeDef RxMessage;

/* CAN1 init function */
void InitializeCANBUS1(void)
{
	GPIO_InitTypeDef GPIO_InitStruct;
	CAN_FilterConfTypeDef CAN_FilterStruct;
	
	__HAL_RCC_CAN1_CLK_ENABLE(); // Enable CANBUS-1 clock 
	__GPIOD_CLK_ENABLE(); //// Enable GPIOD clock
	/*Configuration the CAN peripheral*/
	
	GPIO_InitStruct.Pin = GPIO_PIN_0|GPIO_PIN_1; /*PD1(CAN1_Tx) & PD0(CAN1_RX)*/
  GPIO_InitStruct.Mode = GPIO_MODE_AF_PP; //Alternate Function Push-Pull mode
  GPIO_InitStruct.Pull = GPIO_NOPULL;//No resistor
  GPIO_InitStruct.Speed = GPIO_SPEED_HIGH;
  GPIO_InitStruct.Alternate = GPIO_AF9_CAN1;//Alternate pin function: AF9 / CAN1
  HAL_GPIO_Init(GPIOD, &GPIO_InitStruct); // Initialize regidters
	
  hcan1.Instance = CAN1;
	hcan1.pTxMsg = &TxMessage;
	hcan1.pRxMsg = &RxMessage;
	/*CAN Baudrate = 1 MBps */
  hcan1.Init.Prescaler = 2;
  hcan1.Init.Mode = CAN_MODE_LOOPBACK;
  hcan1.Init.SJW = CAN_SJW_1TQ;
  hcan1.Init.BS1 = CAN_BS1_6TQ;
  hcan1.Init.BS2 = CAN_BS2_8TQ;
  hcan1.Init.TTCM = DISABLE;
  hcan1.Init.ABOM = DISABLE;
  hcan1.Init.AWUM = DISABLE;
  hcan1.Init.NART = DISABLE;
  hcan1.Init.RFLM = DISABLE;
  hcan1.Init.TXFP = DISABLE;
  HAL_CAN_Init(&hcan1);  // Init CAN1 register
	
	CAN_FilterStruct.FilterIdHigh = 0x0000; /*Upper 16bit filter ID*/
	CAN_FilterStruct.FilterIdLow = 0x0000;/*Filter lower 16bit ID*/
	CAN_FilterStruct.FilterMaskIdHigh = 0x0000;/*Upper 16bit filter*/
	CAN_FilterStruct.FilterMaskIdLow = 0x0000;/*Lower 16bit filter*/
	CAN_FilterStruct.FilterFIFOAssignment = 0;
	CAN_FilterStruct.FilterNumber = 0;
	CAN_FilterStruct.FilterMode = CAN_FILTERMODE_IDMASK;
	CAN_FilterStruct.FilterScale = CAN_FILTERSCALE_32BIT;
	CAN_FilterStruct.FilterActivation = ENABLE;
	CAN_FilterStruct.BankNumber = 14;
	HAL_CAN_ConfigFilter(&hcan1, &CAN_FilterStruct); // Initialize filter
	__HAL_CAN_ENABLE_IT(&hcan1, CAN_IT_FMP0); // ENable message pending in FIFO0
	
	hcan1.pTxMsg->StdId = 0x125; /* Standard ID of this device*/
	hcan1.pTxMsg->ExtId = 0x01; /* Extend ID*/
	hcan1.pTxMsg->RTR = CAN_RTR_DATA; /*Remote transmission request*/
	hcan1.pTxMsg->IDE = CAN_ID_STD; /*Identifier type: standard*/
	
	return;

}
void CAN1SendMessage(uint8_t length, uint8_t *data)
{
	uint8_t i;
	hcan1.pTxMsg->DLC = length; /*Specify the data length */
	
	for (i = 0; i < length; i++)
	{
		hcan1.pTxMsg->Data[i] = *(data + i ); 
	}
	HAL_CAN_Transmit(&hcan1, 10); /* Transmit the packet*/
}

void receiveData()
{
	if(HAL_CAN_Receive(&hcan1, 0, 10) == HAL_OK) /*Packet received from CANBUS successfully*/
	{
		if(RxMessage.Data[0] == 0xAA && RxMessage.Data[2] == 0x55)
		{
			LedState(LED_BLUE, 1); // Blue LED ON
			CAN1dataReceivedFlag = 1; /*Set data received fleg*/
		}
	}
}



/* USER CODE BEGIN 1 */

/* USER CODE END 1 */

/**
  * @}
  */

/**
  * @}
  */

/************************ (C) COPYRIGHT STMicroelectronics *****END OF FILE****/
