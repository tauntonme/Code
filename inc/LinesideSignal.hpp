/*
 * LinesideSignal.hpp
 *
 *  Created on: Jan 20, 2024
 *      Author: Jon Freeman  B. Eng. Hons
 */
#include	"main.h"
#ifndef INC_LINESIDESIGNALCLAS_HPP_
#define INC_LINESIDESIGNALCLAS_HPP_

#include	"SingleInput.hpp"
#include	"SingleOutput.hpp"

enum	SignalStates	{	SIGNAL_BLACK,	//	Uses 4 bits of space in tolog
							SIGNAL_DANGER,
							SIGNAL_CAUTION,
							SIGNAL_CLEAR,
							SIGNAL_OCCULTING_YELLOW}	;//	States to which LED signals may be set. Semaphores to show DANGER when not set to CLEAR
	//	Note Occulting is flash where on_time > off_time. See https://www.railforums.co.uk/threads/flashing-signals.186537/


/**	class	LinesideSignal	{
 *
 * Lineside Railway Signals, coloured light or semaphore, connect to two output lines from the controller,
 * 		and connect two 'status' output lines back to controller inputs.
 * 	That is, the physical hardware signal has 4 I/O (Input Outputs) :
 * 		Two status Outputs connected to controller Inputs informing the controller of signal status
 * 		Two command Inputs driven by controller Outputs to set signal to 'Danger', 'Clear', 'Caution', 'Black'.
 * 		Semaphores are to show 'Clear' only for the 'Clear' state, 'Danger' otherwise.
 */
class	LinesideSignal	{	//	Lineside railway signal, 3 aspect colour light or 2 aspect semaphore
	uint32_t	commanded_state;
	uint32_t	flash_timer		= 0L;
	bool		flash_state;	//	Used in on/off toggle when 'occulting' (flashing)
	char	noid[2] = {'?', 0};
	const char * id = noid;
	SingleInput		In_A;	//	Two inputs to conform to the Moving Part Rule
	SingleInput		In_B;
	SingleOutput	Out_A;	//	Two outputs to define states 'Danger', 'Caution', 'Clear' and 'Black'
	SingleOutput	Out_B;
	const char * txt	()	const	{	return	(id);	}
  public:
//	LinkedListNode	node;	//	Initialises pointers to nullptr
	void	set_clear	();
	void	set_danger	();	//	default at power-up
	void	set_caution	();
	void	set_occulting	();	//	Do not EVER call this flashing! (Mustn't upset the gricers)
	void	set_black	();
	char *	report	(char *)	const	;
	uint32_t	tolog	()	const	{		return	(get_two_inbits() + (get_two_outbits() << 2) + (commanded_state << 4));	}	//	return 8 status bits

/*
 * LinesideSignal::LinesideSignal	(const char * signal_name,
 * 				INPIN ClearInputNum, INPIN DangerInputNum,
 * 				OUTPIN ClearOutputNum, OUTPIN DangerOutputNum)	:
 *
 */
	LinesideSignal	(const char * , INPIN, INPIN, OUTPIN, OUTPIN )	;

	void	set_to		(uint32_t);	//	set state to one of DANGER, CAUTION, CLEAR, BLACK, OCCULTING_Y
	void	read_update	();	//	Read state read back from input lines

	uint32_t	get_two_inbits	()	const	;	//	The state read back from the two input lines
	uint32_t	get_two_outbits	()	const	;	//	The state read back from the two output lines

	//	bool	is_semaphore();	//	Returns 'true' for semaphore detected, false otherwise (colour light assumed)
}	;	//	End of class	LinesideSignal	{


#endif



