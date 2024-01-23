/*
 * TurnoutPoints.cpp
 *
 *  Created on: Jan 21, 2024
 *      Author: Jon Freeman  B. Eng. Hons
 */
#include	"TurnoutPoints.hpp"
#include	<cstdio>	//	for sprintf
#include	<cstring>	//	for strcat


void	Points::set_to	(int state_to_set)	{	//	Affects two outputs only
//	set_state = state_to_set;
	switch	(state_to_set)	{
		case	UNPOWERED:
			Out_A.set_pin	(false);	//	set_output_bit	(ClearOutputNum, false);
			Out_B.set_pin	(false);	//	set_output_bit	(DangerOutputNum, true);
			break;
		case	NORMAAL:	//	Dutch spelling prevents potential keyword conflict with 'NORMAL'
			set_state = state_to_set;
			Out_A.set_pin	(true);	//	set_output_bit	(ClearOutputNum, true);
			Out_B.set_pin	(false);	//	set_output_bit	(DangerOutputNum, true);
			break;
		case	OMKEREN:	//	Dutch for 'reverse', avoids potential keyword conflict with 'REVERSE'
			set_state = state_to_set;
			Out_A.set_pin	(false);	//	set_output_bit	(ClearOutputNum, true);
			Out_B.set_pin	(true);	//	set_output_bit	(DangerOutputNum, false);
			break;
//		case	FAULTY:
//			outs.A.clr();	//	set_output_bit	(ClearOutputNum, false);
//			outs.B.clr();	//	set_output_bit	(DangerOutputNum, false);
//			break;
		default:
//			pc.write("Bad", 3);
			break;
	}
}


bool	Points::wrong	()	const	{	//	returns true while readback state does not match set state
	if	((set_state != NORMAAL) && (set_state != OMKEREN))
		return	(true);	//	Points have not been set
//	returned	()	;	//	updates returned_state
	return	(!(get_two_inbits() == set_state))	;
}


bool	Points::normal	()	const	{
	return	(!wrong() && In_A.is_active() && !In_B.is_active());
}


bool	Points::reverse	()	const	{
	return	(!wrong() && !In_A.is_active() && In_B.is_active());
}


bool	DualPoints::wrong	()	const	{	//	returns true while readback state does not match set state
	return	(Points1.wrong() || Points2.wrong());
}


bool	DualPoints::normal	()	const	{	//	returns true while readback state does not match set state
	return	(Points1.normal() && Points2.normal());
}


bool	DualPoints::reverse	()	const	{	//	returns true while readback state does not match set state
	return	(Points1.reverse() && Points2.reverse());
}


char *	Points::report	(char * dest)	const	{
	char	t[200];
	sprintf	(dest, "Points is [%s],\r\n", txt());
	strcat	(dest, In_A.report	(t));
	strcat	(dest, In_B.report	(t));
	strcat	(dest, Out_A.report	(t));
	strcat	(dest, Out_B.report	(t));
	strcat	(dest,	 normal() ? "Normaal" : reverse() ? "Reverse" : "Wrong");
	return	(dest);
}


char *	DualPoints::report(char * dest)	const	{
	char	t[200];
	sprintf	(dest, "Points Pair is [%s],\r\n", txt());
	strcat	(dest, Points1.report(t));
	strcat	(dest, Points2.report(t));
	return	(dest);
}


void	DualPoints::set_to	(int settostate)	{
	Points1.set_to(settostate);	//	Uses only 1 pair of outputs as point pair work together
//	Points2.set_to(settostate);
}


void	DualPoints::read_update()	{
	Points1.read_update();
	Points2.read_update();
}


