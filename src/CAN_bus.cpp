/*
 * CAN_bus.cpp
 *
 *  Created on: Dec 26, 2023
 *      Author: Jon Freeman
 */
#include 	"main.h"

#include	<cstdio>	//	sprintf
#include	"Serial.hpp"
using namespace std;

extern	uint32_t	millisecs;
extern	UartComChan	pc;
extern	UartComChan	* Com_ptrs[];
volatile 	bool	can_flag  = false;

//extern 	bool	can_flag;
//extern	void	ce_show	()	;



/**
  * @brief  CAN Tx message header structure definition
  */
//typedef struct
//{
//  uint32_t StdId;    /*!< Specifies the standard identifier.
//                          This parameter must be a number between Min_Data = 0 and Max_Data = 0x7FF. */

//  uint32_t ExtId;    /*!< Specifies the extended identifier.
//                          This parameter must be a number between Min_Data = 0 and Max_Data = 0x1FFFFFFF. */

//  uint32_t IDE;      /*!< Specifies the type of identifier for the message that will be transmitted.
//                          This parameter can be a value of @ref CAN_identifier_type */

//  uint32_t RTR;      /*!< Specifies the type of frame for the message that will be transmitted.
//                          This parameter can be a value of @ref CAN_remote_transmission_request */

//  uint32_t DLC;      /*!< Specifies the length of the frame that will be transmitted.
//                          This parameter must be a number between Min_Data = 0 and Max_Data = 8. */

//  FunctionalState TransmitGlobalTime; /*!< Specifies whether the timestamp counter value captured on start
//                          of frame transmission, is sent in DATA6 and DATA7 replacing pData[6] and pData[7].
//                          @note: Time Triggered Communication Mode must be enabled.
//                          @note: DLC must be programmed as 8 bytes, in order these 2 bytes are sent.
//                          This parameter can be set to ENABLE or DISABLE. */

//} CAN_TxHeaderTypeDef;
CAN_TxHeaderTypeDef   CANTxHeader;
uint8_t     CANTxData[8] = {0};
uint32_t    CANTxMailbox;
uint32_t	can_errors = 0;
uint32_t	testindex = 0;
uint32_t	test_addrs[] = {0x0123, 0x0466, 0x0700, 0x0704, 0x755, 0x7ff, 0};
uint32_t	cancount = 0;

extern	CAN_HandleTypeDef hcan1;



extern "C" void	start_CAN_txrx()	{	//	Called from main.c
	  HAL_CAN_Start(&hcan1);

	  if	(HAL_OK != HAL_CAN_ActivateNotification(&hcan1, CAN_IT_RX_FIFO0_MSG_PENDING))
		  cancount++;

	  if	(HAL_OK != HAL_CAN_ActivateNotification(&hcan1, CAN_IT_RX_FIFO1_MSG_PENDING))
		  cancount++;
}


void	can_try	()	{	//	Test CAN bus
	static int tog = 0;
	tog++;
//	if	(!can_flag)
//		return;
	can_flag = false;
	CANTxHeader.IDE = CAN_ID_STD;	//	CAN_ID_STD means that we are using the Standard ID (not extended)
//	CANTxHeader.IDE = CAN_ID_EXT;	//	CAN_ID_STD means that we are using the Standard ID (not extended)

	//	CANTxHeader.StdId = 0x446;		//	0x446 is the Identifier. This is the ID of the Transmitter, and it should be maximum 11 bit wide
	CANTxHeader.StdId = test_addrs[testindex++];		//	0x446 is the Identifier. This is the ID of the Transmitter, and it should be maximum 11 bit wide
	if	(testindex >= sizeof(test_addrs) / sizeof(uint32_t))	testindex = 0;
	if	(tog & 1)
		CANTxHeader.RTR = CAN_RTR_DATA;	//	CAN_RTR_DATA indicates that we are sending a data frame
	else
		CANTxHeader.RTR = CAN_RTR_REMOTE;	//	CAN_RTR_REMOTE indicates that we are requesting remote frame ?
	CANTxHeader.DLC = 3;			//	DLC is the Length of data bytes, and here we are sending 2now3 data Bytes

	CANTxData[0] = 50;  			//
	CANTxData[1] = 0xAA;			//	Two bytes to send
	CANTxData[2]++;
	if (HAL_CAN_AddTxMessage(&hcan1, &CANTxHeader, CANTxData, &CANTxMailbox) != HAL_OK)
	{
		can_errors++;
//	   Error_Handler ();
	}
}


/**
  * @brief  CAN Tx message header structure definition
  */
//typedef struct
//{
//  uint32_t StdId;    /*!< Specifies the standard identifier.
//                          This parameter must be a number between Min_Data = 0 and Max_Data = 0x7FF. */

//  uint32_t ExtId;    /*!< Specifies the extended identifier.
//                          This parameter must be a number between Min_Data = 0 and Max_Data = 0x1FFFFFFF. */

//  uint32_t IDE;      /*!< Specifies the type of identifier for the message that will be transmitted.
//                          This parameter can be a value of @ref CAN_identifier_type */

//  uint32_t RTR;      /*!< Specifies the type of frame for the message that will be transmitted.
//                          This parameter can be a value of @ref CAN_remote_transmission_request */

//  uint32_t DLC;      /*!< Specifies the length of the frame that will be transmitted.
//                          This parameter must be a number between Min_Data = 0 and Max_Data = 8. */

//  FunctionalState TransmitGlobalTime; /*!< Specifies whether the timestamp counter value captured on start
//                          of frame transmission, is sent in DATA6 and DATA7 replacing pData[6] and pData[7].
//                          @note: Time Triggered Communication Mode must be enabled.
//                          @note: DLC must be programmed as 8 bytes, in order these 2 bytes are sent.
//                          This parameter can be set to ENABLE or DISABLE. */

//} CAN_TxHeaderTypeDef;
// */



/**
  * @brief  CAN Rx message header structure definition
  */
//typedef struct
//{
//  uint32_t StdId;    /*!< Specifies the standard identifier.
//                          This parameter must be a number between Min_Data = 0 and Max_Data = 0x7FF. */

//  uint32_t ExtId;    /*!< Specifies the extended identifier.
//                          This parameter must be a number between Min_Data = 0 and Max_Data = 0x1FFFFFFF. */

//  uint32_t IDE;      /*!< Specifies the type of identifier for the message that will be transmitted.
//                          This parameter can be a value of @ref CAN_identifier_type */

//  uint32_t RTR;      /*!< Specifies the type of frame for the message that will be transmitted.
//                          This parameter can be a value of @ref CAN_remote_transmission_request */

//  uint32_t DLC;      /*!< Specifies the length of the frame that will be transmitted.
//                          This parameter must be a number between Min_Data = 0 and Max_Data = 8. */

//  uint32_t Timestamp; /*!< Specifies the timestamp counter value captured on start of frame reception.
//                          @note: Time Triggered Communication Mode must be enabled.
//                          This parameter must be a number between Min_Data = 0 and Max_Data = 0xFFFF. */

//  uint32_t FilterMatchIndex; /*!< Specifies the index of matching acceptance filter element.
//                          This parameter must be a number between Min_Data = 0 and Max_Data = 0xFF. */

//} CAN_RxHeaderTypeDef;


CAN_TxHeaderTypeDef	TxHeader;
CAN_RxHeaderTypeDef	RxHeader;

uint8_t	MyCAN_TxData[8];
uint8_t	MyCAN_RxData[8];

void	ce_show	()	{
	char	t[96];
	int	len;
	len = sprintf(t, "Sid 0x%lx, Eid 0x%lx, IDE 0c%lx, DLC 0x%lx, FMI 0x%lx, RTR 0x%ld\r\n",
			/*can_errors, cancount, */
			RxHeader.StdId, RxHeader.ExtId, RxHeader.IDE, RxHeader.DLC, RxHeader.FilterMatchIndex, RxHeader.RTR);
	pc.write(t, len);
//	uint32_t q = CAN1_BASE;
}

extern "C"	  void	can_filter_setup(){	//	called from can init
	  /* USER CODE BEGIN CAN1_Init 2 */
	//  can_filter_setup();
	  CAN_FilterTypeDef	canfilterconfig;	//	Need to configure filters here
	  canfilterconfig.FilterActivation = CAN_FILTER_ENABLE;
	  canfilterconfig.FilterBank = 1;
	  canfilterconfig.FilterFIFOAssignment = CAN_RX_FIFO0;
	  canfilterconfig.FilterIdHigh = 0x0446 << 5;
	  canfilterconfig.FilterIdLow = 0x0000;
	  canfilterconfig.FilterMaskIdHigh = 0x0444 << 5;	//	'1' bit to compare filter bit to incoming bit
	  canfilterconfig.FilterMaskIdLow = 0x0000;			//	'0' do not test bit in this position
	  canfilterconfig.FilterMode = CAN_FILTERMODE_IDMASK;
	  canfilterconfig.FilterScale = CAN_FILTERSCALE_32BIT;
	//  canfilterconfig.SlaveStartFilterBank = 26;	//	Meaningless as only 1 CAN on L432KC
	  HAL_CAN_ConfigFilter(&hcan1, &canfilterconfig);

	  canfilterconfig.FilterBank = 6;
	  canfilterconfig.FilterIdHigh = 0x0123 << 5;
	  canfilterconfig.FilterMaskIdHigh = 0x0123 << 5;
	  HAL_CAN_ConfigFilter(&hcan1, &canfilterconfig);

	  canfilterconfig.FilterBank = 10;
	  canfilterconfig.FilterIdHigh = 0x0700 << 5;
	  canfilterconfig.FilterMaskIdHigh = 0x0700 << 5;
	  HAL_CAN_ConfigFilter(&hcan1, &canfilterconfig);

	  start_CAN_txrx	()	;

	  /* USER CODE END CAN1_Init 2 */

}


//uint32_t	cancount = 0;

void HAL_CAN_RxFifo0FullCallback(CAN_HandleTypeDef *hcan)	//	Not this one
{
	cancount++;
	HAL_CAN_GetRxMessage(&hcan1, CAN_RX_FIFO0, &RxHeader, MyCAN_RxData);
//	stdid[0] = RxHeader.StdId;
}

void HAL_CAN_RxFifo1FullCallback(CAN_HandleTypeDef *hcan)	//	Not this one
{
	cancount++;
	HAL_CAN_GetRxMessage(&hcan1, CAN_RX_FIFO1, &RxHeader, MyCAN_RxData);
//	stdid[1] = RxHeader.StdId;
}

void HAL_CAN_RxFifo0MsgPendingCallback(CAN_HandleTypeDef *hcan)	//	Yes this one
{
	cancount++;
	can_flag = true;
	HAL_CAN_GetRxMessage(&hcan1, CAN_RX_FIFO0, &RxHeader, MyCAN_RxData);
//	stdid[2] = RxHeader.StdId;
}

void HAL_CAN_RxFifo1MsgPendingCallback(CAN_HandleTypeDef *hcan)	//	Yes this one
{
	cancount++;
	HAL_CAN_GetRxMessage(&hcan1, CAN_RX_FIFO1, &RxHeader, MyCAN_RxData);
//	stdid[2] = RxHeader.StdId;
}


