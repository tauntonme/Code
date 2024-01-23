/*
 * Serial.hpp
 *
 *  Created on: Jun 13, 2023
 *      Author: Jon Freeman  B Eng Hons
 */
//#include	<cctype>
#include	"main.h"
#ifndef INC_SERIAL_HPP_
#define INC_SERIAL_HPP_

#define	INPUT_OVERRUN_ERROR		(1 << 0)
#define	OUTPUT_OVERRUN_ERROR	(1 << 1)
#define	HAL_UART_ERROR			(1 << 2)
#define	SOME_OTHER_ERROR		(1 << 3)	//	etc, bits 0, 1, 2, 3,  ... n

#define	COM_PORT	pc
#define	IO_DAISY_CHAIN_BYTE_LEN	8

/**	class	UartComChan	{
 *	Handles com ports
 */
class	UartComChan	{
#define		RXBUFF_SIZE			100		//	Big enough to catch all chars arriving between calls to 'test_for_message'
#define		LIVE_TXBUFF_SIZE	512		//	This many chars max passed per DMA Transmit
#define		LIN_INBUFF_SIZE		120		//	Long enough for longest command line
#define		RING_OUTBUFF_SIZE	4000	//	Large as possible but not silly - suspect buffer overrun not handled !
	UART_HandleTypeDef * huartn	;		//	Which hardware usart
	volatile	bool	rx_buff_empty = true;
	volatile	bool	tx_buff_empty = true;
	volatile	bool	tx_busy = false;
	char		lin_inbuff		[LIN_INBUFF_SIZE + 4] 	= {0};	//	Command line, not circular buffer
	char		ring_outbuff	[RING_OUTBUFF_SIZE + 4] = {0};	//	Linear, not circular, output buffer
	char		live_tx_buff	[LIVE_TXBUFF_SIZE + 4] 	= {0};	//	buffer handed to DMA Transmit
	char		ch[4] = {0};
	uint32_t	lin_inbuff_onptr 	= 0L;
	uint32_t	lin_inbuff_offptr 	= 0L;
	uint32_t	ring_outbuff_onptr 	= 0L;
	uint32_t	ring_outbuff_offptr = 0L;
	uint32_t	serial_error 		= 0L;
	uint32_t	rx_onptr 			= 0L;
	uint32_t	tx_onptr 			= 0L;
	uint32_t	rx_offptr 			= 0L;
	uint32_t	tx_offptr 			= 0L;
	uint8_t		rxbuff	[RXBUFF_SIZE + 2] = {0};	//	Circular buffer, UART Rx interrupt places data here
	bool	read1	(char * ch);	//	reads 1 char. Returns true on success, false when no char available
public:
	UartComChan	(UART_HandleTypeDef &wot_port)	{	//	Constructor
		huartn = &wot_port;
	}
	char *	test_for_message	();	//	Returns buffer address on receiving '\r', presumably at end of command string, NULL otherwise
	bool	write	(const uint8_t * t, int len)	;	//	Puts all on buffer. Transmit only once per ms
	bool	write	(const char * t, int len)	;
	bool	tx_any_buffered	()	;	//	Call this every 1ms to see if sending complete and send more if there is
	void	rx_intrpt_handler_core	();
	void	start_rx	(){	//	Call from startup. Call from constructor is too early
		  HAL_UART_Receive_DMA	(huartn, rxbuff, 1);	//	huartn and rxbuff 'private'
	}
	void	set_tx_busy	(bool true_or_false)	{
		tx_busy = true_or_false;
	}
	bool	huart_compare	(UART_HandleTypeDef &wot_port)	const	{
		return	(huartn == &wot_port);	//	Return true on match, false otherwise
	}
	bool	test_error	(int mask)	const;	//	returns true if error bits set after masking
	int		clear_error	(int mask);	//	-1 clears all error bits, 0 clears none, INPUT_OVERRUN_ERROR clears INPUT_OVERRUN_ERROR. Returns seial_errors
	void	set_error	(int mask)	{	serial_error |= mask;	}
}	;


#endif /* INC_SERIAL_HPP_ */
