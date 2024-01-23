/*
 * Serial.cpp
 *
 *  Created on: Jun 13, 2023
 *      Author: Jon Freeman
 */
#include 	"main.h"
#include	<cstdio>
#include	<cstdbool>
#include	<cstring>

#include	"Serial.hpp"

using namespace std;

//	Declare all used uarts here
extern	 UART_HandleTypeDef	huart1;
extern	 UART_HandleTypeDef	huart2;	//	uarts used in this project
UartComChan	pc		(huart2);
UartComChan	ctrl	(huart1);	//	No further reference to huart1, huart2
UartComChan	* Com_ptrs	[] = { &ctrl, &pc, nullptr};	//	Table of pointers to uart handlers


extern "C" void	start_uart_rx	()	{	//	Can not start from constructor, extern "C" because call from .c
	  uint32_t	w = 0;			//	Start Rx for all however many uarts we have set up
	  while	(Com_ptrs[w])	{	//	Work through list to NULL list terminator
		  Com_ptrs[w]->start_rx();	//	only call to start_rx(), so 1 line code in Serial.hpp
		  w++;					//	calls HAL_UART_Receive_DMA (); to kick-start uart Rx
	  }
}


void	send_queued_uart_messages	()	{	//	Called from enclosing while(1) loop in main.
	UartComChan ** tp = Com_ptrs;			//	Not in interrupt context.
	while	(*tp)	{
		tp[0]->tx_any_buffered	()	;	//	Send any messages on uart output queues
		tp++;
	}
}


UartComChan * identify_uart	(UART_HandleTypeDef *huart)	{	//	Return pointer to uart handler which owns this uart
	uint32_t	w = 0;
	while	(Com_ptrs[w])	{
		if	(Com_ptrs[w]->huart_compare(*huart))	//	Only call to huart_compare, so 1 line code in Serial.hpp
			return	(Com_ptrs[w]);
		w++;
	}
	return	(nullptr);
}


bool	UartComChan::read1	(char * ch)	{
	if	(rx_buff_empty)
		return	(false);
	*ch = (char)rxbuff[rx_offptr++];
	if	(rx_offptr >= RXBUFF_SIZE)
		rx_offptr = 0;
	if	(rx_onptr == rx_offptr)
		rx_buff_empty	= true;
	return	(true);
}


/**
 * char *	UartComChan::test_for_message	()	{
 *
 * Called from ForeverLoop at repetition rate
 * Returns pointer to lin_inbuff when "\r\n" terminated command line has been assembled in lin_inbuff, NULL otherwise
 */
char *	UartComChan::test_for_message	()	{	//	Read in any received chars into linear command line buffer
	while	(read1(ch))	{	//	Get next received characters, if any have been received.
		if	(lin_inbuff_onptr >= LIN_INBUFF_SIZE)	{
			ch[0] = '\r';		//	Prevent command line buffer overrun
			serial_error |= INPUT_OVERRUN_ERROR;	//	set error flag
		}
		if(ch[0] != '\n')	{	//	Ignore newlines
			lin_inbuff[lin_inbuff_onptr++] = ch[0];
			lin_inbuff[lin_inbuff_onptr] = '\0';
			if(ch[0] == '\r')	{
				lin_inbuff[lin_inbuff_onptr++] = '\n';
				lin_inbuff[lin_inbuff_onptr] = '\0';
				write	(lin_inbuff, lin_inbuff_onptr);	//	echo received command string to originator
				lin_inbuff_onptr = 0;	//	Could return the length here, might be useful
				if	(test_error(INPUT_OVERRUN_ERROR))	{
					write	("INPUT_OVERRUN_ERROR\r\n", 21);
					clear_error	(INPUT_OVERRUN_ERROR);
				}
				return	(lin_inbuff);			//	Got '\r' command terminator
			}
		}
	}
	return	(nullptr);	//	Have not yet found any complete terminated command string to process
}


bool	UartComChan::test_error	(int mask)	const	{	//	Return true for error, false for no error
	return	(serial_error & mask);	//	true for NZ result (error flag set), false for 0 or no error result
}


int		UartComChan::clear_error	(int mask)	{	//	-1 to clear all errors, 0 to clear no errors
	serial_error &= ~mask;
	return	(serial_error);
}


/**
 * void	UartComChan::write	(const uint8_t * t, int len)	{
 *
 *
 *
 * Always copy send data into lin buff so that call can return and let code overwrite mem used for message.
 * */
bool	UartComChan::write	(const uint8_t * t, int len)	{	//	Only puts chars on buffer.
	if	(len < 1)
		return	(false);			//	Can not send zero or fewer chars !
	int	buff_space = ring_outbuff_offptr - ring_outbuff_onptr;
	if	(buff_space <= 0)
		buff_space += RING_OUTBUFF_SIZE;
	if	(buff_space < len)	{
		serial_error |= OUTPUT_OVERRUN_ERROR;
		return	(false);
	}
	int	space_to_bufftop = RING_OUTBUFF_SIZE - ring_outbuff_onptr;
	char *	dest1 = ring_outbuff + ring_outbuff_onptr;
	if	(len > space_to_bufftop)	{
		memmove	(dest1, t, space_to_bufftop);
		memmove	(ring_outbuff, t + space_to_bufftop, len - space_to_bufftop);
		ring_outbuff_onptr += len;	//	which takes us beyond end of buffer
		ring_outbuff_onptr -= RING_OUTBUFF_SIZE;
	}
	else	{
		memmove	(dest1, t, len);
		ring_outbuff_onptr += len;
	}
	tx_buff_empty = false;
	return	(true);
}


bool	UartComChan::write	(const char * t, int len)	{	//	Remembering to keep type-casting is such a bore
	return	(write	((uint8_t*)t, len));					//	Overloaded functions take char or uint8_t
}


bool	UartComChan::tx_any_buffered	()	{
	HAL_StatusTypeDef	ret;	//	Used to test return results of HAL functions
	if	(tx_buff_empty || tx_busy)
		return	(false);
	//	To be here, tx_buff has stuff to send, and uart tx chan is not busy

	int	len = 0;
	while	(!tx_buff_empty && (len < LIVE_TXBUFF_SIZE))	{
		//
		live_tx_buff[len++] = ring_outbuff[ring_outbuff_offptr++];
//		tx_buff_full = false;
		if	(ring_outbuff_offptr >= RING_OUTBUFF_SIZE)
			ring_outbuff_offptr = 0;
		if(ring_outbuff_onptr == ring_outbuff_offptr)
			tx_buff_empty = true;
	}
	if	(len > 0)	{
		tx_busy = true;
//		ret = HAL_UART_Transmit_IT	(huartn, (uint8_t *)live_tx_buff, len);
		ret = HAL_UART_Transmit_DMA	(huartn, (uint8_t *)live_tx_buff, len);
		if	(ret == HAL_OK)
			return	(true);
	}
return	(false);
}

//	USART Interrupt Handlers

void	UartComChan::rx_intrpt_handler_core	()	{	//	Interrupt has placed char on circular buffer.
	rx_buff_empty	= false;
	rx_onptr++;		//	update onptr for next char
	if	(rx_onptr >= RXBUFF_SIZE)
		rx_onptr = 0;
//		if	(com1_rx_onptr == com1_rx_offptr)
//			com1_rx_full	= true;			//	onptr now ready for HAL_UART_Receive_DMA()
	HAL_UART_Receive_DMA(huartn, rxbuff + rx_onptr, 1);	//	Enable following read
}


//void HAL_UART_RxHalfCpltCallback(UART_HandleTypeDef *huart)	{
void HAL_UART_RxCpltCallback(UART_HandleTypeDef *huart)	{
	UartComChan * tp = identify_uart(huart);
	if	(tp)	{
		tp->rx_intrpt_handler_core();
	}
}

//void HAL_UART_DMATxCpltCallback(UART_HandleTypeDef *huart)	//	This called as well as HalfCplt
void HAL_UART_TxCpltCallback(UART_HandleTypeDef *huart)	//	This called as well as HalfCplt
{
	UartComChan * tp = identify_uart(huart);
	if	(tp)	{
		tp->set_tx_busy  (false);	//	Only call to 1 line set_tx_busy, so 1 line code in Serial.hpp
	}
}


void HAL_UART_ErrorCallback(UART_HandleTypeDef *huart)
{
	UartComChan * tp = identify_uart(huart);
	if	(tp)	{
		tp->set_error	(HAL_UART_ERROR);
	}
}




