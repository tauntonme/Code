/*
 * LinesideSignal.cpp
 *
 *  Created on: Jan 21, 2024
 *      Author: Jon Freeman  B. Eng. Hons
 */
#include	"LinesideSignal.hpp"
#include	<cstdio>	//	for sprintf
#include	<cstring>	//	for strcat
extern	uint32_t	millisecs;

LinesideSignal::LinesideSignal	(const char * signal_name, INPIN ClearInputNum, INPIN DangerInputNum,
				OUTPIN ClearOutputNum, OUTPIN DangerOutputNum)	:
	In_A		{"Inp_C", ClearInputNum, true},
	In_B		{"Inp_D", DangerInputNum, true},
	Out_A		{"Out_C", ClearOutputNum, true},
	Out_B		{"Out_D", DangerOutputNum, true}
		{	//	This is the "constructor"	//
			set_danger	()	;	//	also sets commanded_state
			if	(signal_name)
				id = signal_name;
		}	;

uint32_t	LinesideSignal::get_two_inbits	()	const	{		//	The state read back from input lines
	uint32_t	rv = 0;
	if	(In_A.is_active())	rv |= 1;
	if	(In_B.is_active())	rv |= 2;
	return	(rv)	;							//	Returns 0, 1, 2 or 3
}

uint32_t	LinesideSignal::get_two_outbits	()	const	{		//	The state read back from output buffers
	uint32_t	rv = 0;
	if	(Out_A.read())	rv |= 1;
	if	(Out_B.read())	rv |= 2;
	return	(rv)	;							//	Returns 0, 1, 2 or 3
}

void	LinesideSignal::set_danger	()	{
	Out_A.set_pin	(false);
	Out_B.set_pin	(true);
	commanded_state = SIGNAL_DANGER;
}

void	LinesideSignal::set_caution	()	{
	Out_A.set_pin	(true);
	Out_B.set_pin	(true);
	commanded_state = SIGNAL_CAUTION;
}

void	LinesideSignal::set_occulting	()	{	//	OCCULTING_YELLOW
	//	Requires yellow flash with on time > off time
	commanded_state = SIGNAL_OCCULTING_YELLOW;
}

void	LinesideSignal::set_clear	()	{
	Out_A.set_pin	(true);
	Out_B.set_pin	(false);
	commanded_state = SIGNAL_CLEAR;
}

void	LinesideSignal::set_black	()	{
	Out_A.set_pin	(false);
	Out_B.set_pin	(false);
	commanded_state = SIGNAL_BLACK;
}

void	LinesideSignal::set_to	(uint32_t state_to_set)	{	//	Affects two outputs only
	commanded_state = state_to_set;	//	record most recent setting
	switch	(state_to_set)	{
		case	SIGNAL_DANGER:
			set_danger	()	;
			break;
		case	SIGNAL_CAUTION:
			set_caution	()	;
			break;
		case	SIGNAL_OCCULTING_YELLOW:
			set_occulting	()	;
			break;
		case	SIGNAL_CLEAR:
			set_clear	()	;
			break;
		case	SIGNAL_BLACK:
			set_black	()	;
			break;
		default:
//			pc.write("Bad", 3);
			break;
	}
}


void	LinesideSignal::read_update	()	{	//	Affects two outputs only
	const	int	OCCULT_ON_MS	=	300;
	const	int	OCCULT_OFF_MS	=	100;

	In_A.read_update();	//	There are few if any reasons to read inputs back, except
	In_B.read_update();	//	to determine signal head type, head presence or absence.
	if	((commanded_state == SIGNAL_OCCULTING_YELLOW) && (flash_timer < millisecs))	{
		if	(flash_state)
			flash_timer = millisecs + OCCULT_ON_MS;
		else
			flash_timer = millisecs + OCCULT_OFF_MS;
		flash_state = !flash_state;
		Out_A.set_pin	(flash_state);
		Out_B.set_pin	(flash_state);
	}
}


char *	LinesideSignal::report	(char * dest)	const	{
	char	t[60];
	sprintf	(dest, "Signal is [%s], ", txt());
	strcat	(dest, In_A.report	(t));
	strcat	(dest, In_B.report	(t));
	strcat	(dest, Out_A.report	(t));
	strcat	(dest, Out_B.report	(t));
	strcat	(dest, "\r\n");
	strcat	(dest,	 (In_A.is_active() && !In_B.is_active())? "Clear" :
			(!In_A.is_active() && In_B.is_active())? "Danger" : "Wrong");
//		pc.write(dest, strlen(dest));
	return	(dest);
}	;





