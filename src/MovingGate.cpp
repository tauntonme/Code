/*
 * MovingGate.cpp
 *
 *  Created on: Jan 21, 2024
 *      Author: Jon Freeman  B. Eng. Hons
 */
#include	"MovingGate.hpp"
#include	<cstdio>	//	for sprintf
#include	<cstring>	//	for strcat

MovingGate::MovingGate	(
			const char * gate_name,
			INPIN A_input_ionum, POLARITY A_polarity,
			INPIN B_input_ionum, POLARITY B_polarity	)
			:
		In_A{"InpA", A_input_ionum, A_polarity},	//	"InpA" and "InpB" component names fixed.
		In_B{"InpB", B_input_ionum, B_polarity}
			{	if(gate_name)	id = gate_name;
}	//	Nothing else to initialise in constructor


void	MovingGate::read_update	()	{	//	Calls read_update for inputs A and B
	In_A.read_update()	;					//	the Moving Part Rule
	In_B.read_update()	;
}


uint32_t	MovingGate::get_two_inbits	()	const	{	;	//	The state read back from input lines
	uint32_t	rv = 0;
	if	(In_A.is_active())	rv |= 1;
	if	(In_B.is_active())	rv |= 2;
	return	(rv)	;							//	Returns 0, 1, 2 or 3
}


bool	MovingGate::is_open		()	const	{
	return	(In_A.is_active() && !In_B.is_active());
}


bool	MovingGate::is_closed	()	const	{
	return	(!In_A.is_active() && In_B.is_active());
}


char *	MovingGate::report	(char * dest)	const	{
	char	t[100];
	sprintf	(dest, "Gate is [%s],\r\n", txt());
	strcat	(dest, In_A.report	(t));
	strcat	(dest, In_B.report	(t));
	strcat	(dest,	 is_open() ? "Open" : is_closed() ? "Closed" : "Wrong");
//	strcat	(dest,	 (In_A.is_active() && !In_B.is_active())? "Rail" :
//					 (!In_A.is_active() && In_B.is_active())? "Road" : "Wrong");
	strcat	(dest, "\r\n");
	//	pc.write(dest, strlen(dest));
	return	(dest);
}





