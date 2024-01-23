/*
 * Shift_Reg_SPI.cpp
 *
 *  Created on: Dec 21, 2023
 *      Author: Jon Freeman
 *
 *      SPI drives daisy chains of 8 bit shift registers providing many digital In (74HC597)/Out (74HC595)
 *      Daisy chain length is any, see 'IO_DAISY_CHAIN_BYTE_LEN'
 *
 *	Default spi is 'hspi1' and assumes only 1 spi. Edit interrupt handlers to suit multiple spi if needed
 *
 */
//#include 	"main.h"

#include 	"main.h"
#include	<cstdio>
#include	<cstdbool>

#include	"Serial.hpp"

extern	SPI_HandleTypeDef hspi1;	//	May use other spi port
extern	UartComChan	COM_PORT	;//, *Com_ptrs[];

//#define	IO_DAISY_CHAIN_BYTE_LEN	8

#define	SPI_CS_PORT	(GPIOA)		//	Chip select pin defaults to Port A, bit 4
#define	SPI_CS_PIN	(GPIO_PIN_4)//	Edit these to change CS port/pin

extern		uint32_t	input_test_error_flags;
uint8_t		spi_txb[IO_DAISY_CHAIN_BYTE_LEN + 2] = {0};
uint8_t		spi_rx[IO_DAISY_CHAIN_BYTE_LEN + 2] = {0};
uint32_t	ins_history[(IO_DAISY_CHAIN_BYTE_LEN << 3) + 2] = {0};	//	One uint32_t per input bit
volatile 	bool	spi_error_flag 			= false;
volatile 	bool	spi_TxRx_complete_flag 	= false;

//void	spi_cs_set	()	;		//	SPI Chip Select line ___-- This pin to be set to '1' from .ioc
//void	spi_cs_clr	()	;		//	SPI Chip Select line --___

/**	bool	get_outreg_1bit	(uint32_t bitnum)	{
 * Only call from class SingleOutput::read()
 */
bool	get_outreg_1bit	(uint32_t bitnum)	{
	uint32_t	which_byte 	= IO_DAISY_CHAIN_BYTE_LEN - (bitnum >> 3) - 1;
	uint32_t	which_bit	= (1 << (bitnum & 0x07));						//	Higher numbered outputs in higher numbered bit positions
	return	((spi_txb[which_byte] & which_bit) != 0L);
}

void	spi_cs_set	()	{	//
	HAL_GPIO_WritePin(SPI_CS_PORT, SPI_CS_PIN, GPIO_PIN_SET);
}

void	spi_cs_clr	(){		//
	HAL_GPIO_WritePin(SPI_CS_PORT, SPI_CS_PIN, GPIO_PIN_RESET);
}


/*uint32_t	find_daisy_length	()	{	//	Jan 2024 This does not work.
	uint32_t	retval = 0L;
	int	len;
	char	t[100];
	pc.write	("find_daisy_length ;_\r\n", 22);
	spi_txb[3] = 0xc1;
	spi_rx[3] = 0xc1;
	len = sprintf(t, " %2x %2x %2x %2x %2x %2x %2x %2x\r\n",
			spi_rx[0], spi_rx[1], spi_rx[2], spi_rx[3], spi_rx[4], spi_rx[5], spi_rx[6], spi_rx[7]);
	pc.write	(t, len);
	HAL_SPI_TransmitReceive_DMA	(&hspi1, spi_txb, spi_rx, IO_DAISY_CHAIN_BYTE_LEN);
	while	(!spi_TxRx_complete_flag)	{}	// Hang waiting for spi to complete
	len = sprintf(t, " %2x %2x %2x %2x %2x %2x %2x %2x\r\n",
			spi_rx[0], spi_rx[1], spi_rx[2], spi_rx[3], spi_rx[4], spi_rx[5], spi_rx[6], spi_rx[7]);
	pc.write	(t, len);
	return	(retval);
}*/
/**
 * void	start_spi_TxRx	()	{
 *
 *
 */
void	start_spi_TxRx	()	{	//	Double clock to get very latest inputs, see hardware text ffi

/*	spi_txb[0] = 0x55;
	spi_txb[2] = 0x55;
	spi_txb[4] = 0x55;
	spi_txb[6] = 0x55;
*/
	spi_cs_clr();	//	SPI Chip select active low
	spi_TxRx_complete_flag = false;	//	Precaution, should be false already
//	  spi_tx[0] = 0x55;		//	Just for something for the scope to see
	__NOP();
	spi_cs_set();	//	SPI Chip select hi again as also connected to HC597 PL\ as well as STCP.
	  //	STCP is __-- edge trig, PL\ is async.  Therefore, to get latest data, need TWO
	  //	--_--_--. First clocks latest pin data into input registers.
	  //	Second --_-- clock transfers latest pin data to output shift register.
	__NOP();
	spi_cs_clr();	//	SPI Chip select active low
	__NOP();
	spi_cs_set();	//	SPI Chip select active low

//	HAL_SPI_TransmitReceive_IT	(&hspi1, spi_tx, spi_rx, IO_DAISY_CHAIN_BYTE_LEN);
	HAL_SPI_TransmitReceive_DMA	(&hspi1, spi_txb, spi_rx, IO_DAISY_CHAIN_BYTE_LEN);
	  	  //	Once SPI comms has completed, Interrupt handler sets spi_TxRx_complete_flag = true;
	  	  //	Test spi_TxRx_flag in fast area above to call ForeverLoop()
}
/*
bool		spi_TxRx_Complete	()	{	return	(spi_TxRx_complete_flag);	}
uint8_t *	get_spi_Rxbuff_addr	()	{	return	(spi_rx);	}
*/

/**
 * bool	spi_inputs_updated	()	{
 *
 * Run this soon after completion of I/O read/write using HAL_SPI_TransmitReceive
 * Reads all input bits read in over SPI currently stored in spi_rx[]
 * Last 31 reads of each input line are stored in bits 30 downto 0 of uint32_t ins_history[numbered]
 * most recent bit in bit 30 so that read of ins_history[n] can be used compared to some threshold
 */
bool	spi_inputs_updated	()	{
	uint32_t	input_number = 0;
	uint8_t		byte_mask;	//	same type as spi_rx[]
	if	(!spi_TxRx_complete_flag)		//	set 'true' in HAL_SPI_TxRxCpltCallback
		return	(false);
	spi_TxRx_complete_flag = false;	//	clr flag to false
	for	(uint32_t byte_num = 0; byte_num < IO_DAISY_CHAIN_BYTE_LEN; byte_num++)	{
		byte_mask = 1;
		while	(byte_mask)	{
			ins_history[input_number] >>= 1;	//	Oldest input reads pushed right to lsbs
			if	((spi_rx[byte_num] & byte_mask) != 0)
				ins_history[input_number] |= 0x40000000L;	//	set bit 30
			byte_mask <<= 1;	//	do for bit positions 0 to 7
			input_number++;
		}
	}
	return	(true);
}

/*
bool	get_raw_input_bytes	(char * dest)	{
	for	(int i = 0; i < IO_DAISY_CHAIN_BYTE_LEN; i++)
		dest[i] = spi_rx[i];
	return	true;
}*/

/**
 * bool	set_output_bit	(uint32_t which_out, bool hiorlo)	{
 * Number of output BYTES defined as 'IO_DAISY_CHAIN_BYTE_LEN' defined in "Serial.hpp", (probably 8)
 * Number of outputs = BYTES * 8 (numbered 0 to 63 when IO_DAISY_CHAIN_BYTE_LEN == 8)
 *
 * Function returns 'false' if 'which_output' contains invalid output number
 * Using hiorlo 'true' turns output on, 'false' off.
 * Action causing output CHANGE returns true
 * Action causing no output change returns false
 */
bool	set_output_bit	(uint32_t which_output, bool hiorlo)	{	//	Big Endian storage - high output numbers in low numbered bytes
	if	(which_output >= (IO_DAISY_CHAIN_BYTE_LEN << 3)){//	'which_output' must be in range of numof inputs
		input_test_error_flags |= 0x10;		//	Set global error flag
		return	(false);
	}
	uint32_t	which_byte 	= IO_DAISY_CHAIN_BYTE_LEN - (which_output >> 3) - 1;
	uint32_t	which_bit	= (1 << (which_output & 0x07));						//	Higher numbered outputs in higher numbered bit positions
	bool	previous_level = ((spi_txb[which_byte] & which_bit) != 0);
	if	(hiorlo == previous_level)	//	No change
		return	(false);
	spi_txb[which_byte] ^= which_bit;
	return	(true);
/*	if	(hiorlo)	{	//	'true'
		spi_txb[which_byte] |= which_bit;	//	set bit to '1'
	}
	else	{	//	'false'
		spi_txb[which_byte] &= ~which_bit;	//	clear bit to '0'
	}*/
	return	(true);
}


/**
 * bool	get_input_bit_debounced
 * A 'De-Bounce' implementation
 * 			Only call is from class SingleInput::read_update().
 *
 * 	Inputs are scanned (and outputs updated) regularly, probably 50 times per second.
 *
 * Most recent 31 input[n] bit reads stored in uint32_t ins_history[n], newest in bit 31, oldest bit 0
 * (chose 31 over 32 so that number positive regardless of 'int' ot 'uint')
 * function takes odd number for samples and tests this many from most recent back in time
 * returns 'true' for more '1's than '0's, false for more '0's than '1's
 * On error sets bits in 'input_test_error_flags' and returns false
 */
bool	get_input_bit_debounced	(uint32_t which_input, uint32_t ave_of_how_many)	{
	uint32_t threshold;
	uint32_t ones_count = 0;
	uint32_t test_bit = 0x40000000L;
	if	(which_input >= (IO_DAISY_CHAIN_BYTE_LEN << 3))	{	//	'which_in' must be in range of numof inputs
		input_test_error_flags |= 1;
		return	(false);
	}
	if	((ave_of_how_many < 1) || (ave_of_how_many > 31))	{	//	test for range error
		input_test_error_flags |= 2;
		return	(false);
	}
	if	((ave_of_how_many & 1) == 0)	{	//	check for 'how_many' is even, should be odd
		ave_of_how_many--;					//	smallest even number possible here was 2, round down to next lower odd
		input_test_error_flags |= 4;	//	set error flag but proceed
	}								//	'how_many' now always odd number
	ones_count	= 0;
	test_bit	= 0x40000000L;	//	First bit to test is bit 30
	threshold = ave_of_how_many / 2;	//	when how_many == 1, threshold == 0
								//	when how_many == 3, threshold == 1 etc
	while	(ave_of_how_many--)	{
		if	((ins_history[which_input] & test_bit) != 0)
			ones_count++;
		test_bit >>= 1;		//	test bit30 first, next bit 29 etc
	}
	return	(ones_count > threshold);
//	if	(ones_count > threshold)
//		return	(true);
//	return	(false);
}


/*
// This is called when SPI transmit is done. Should not happen as using TransmitReceive_IT
void HAL_SPI_TxCpltCallback (SPI_HandleTypeDef * hspi)
{
	spi_cs_clr();	//	SPI Chip select active low
	__NOP();
	spi_cs_set();	//	SPI Chip select active low This clock updates HC595 storage registers
	spi_error_flag = true;
}

// This is called when SPI receive is done. Should not happen as using TransmitReceive_IT
void HAL_SPI_RxCpltCallback (SPI_HandleTypeDef * hspi)
{
	spi_cs_set();
	spi_error_flag = true;
}
*/
void HAL_SPI_TxRxCpltCallback (SPI_HandleTypeDef * hspi)	//	Only SPI interrupt required.
{
	spi_cs_set();			//	SPI chip select line
	spi_TxRx_complete_flag = true;
}





