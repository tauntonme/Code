/*
 * PushButton.cpp
 *
 *  Created on: Jan 21, 2024
 *      Author: Jon Freeman  B. Eng. Hons
 */
#include	"PushButton.hpp"
#include	<cstdio>	//	for sprintf
#include	<cstring>	//	for strcat

PushButton::PushButton	(	const char * button_name,
					INPIN A_input_ionum, POLARITY A_polarity,
					OUTPIN outnum1, POLARITY outpolarity1,
					OUTPIN outnum2, POLARITY outpolarity2 )
					:	In_A{"In", A_input_ionum, A_polarity},	//	"In", "OutA" and "OutB" component names fixed.
						Out_A{"OutA", outnum1, outpolarity1},
						Out_B{"OutB", outnum2, outpolarity2}
	{	if(button_name)	id = button_name;	}	//	Constructor, nothing left to do


PushButton::PushButton	(	const char * button_name,	//	Pass function pointers down to SingleInput
					INPIN A_input_ionum, POLARITY A_polarity, bool(*press)(), bool(*release)(),
					OUTPIN outnum1, POLARITY outpolarity1,
					OUTPIN outnum2, POLARITY outpolarity2 )
					:	In_A{"In", A_input_ionum, A_polarity, press, release},	//	"In", "OutA" and "OutB" component names fixed.
						Out_A{"OutA", outnum1, outpolarity1},
						Out_B{"OutB", outnum2, outpolarity2}
	{
		if(button_name)	id = button_name;
//		press_func	= press;
//		release_func= release;
	}	//	Constructor, nothing left to do

bool	PushButton::read_update	()	{	//	Calls read_update for input A
		previous_in = latest_in;
		latest_in = In_A.read_update()	;					//
		return	(latest_in);
	}


char *	PushButton::report	(char * dest)	const	{
	char	t[200];
	sprintf	(dest, "Push Button is [%s],\r\n", txt());
	strcat	(dest, In_A.report	(t));
	strcat	(dest, Out_A.report	(t));
	strcat	(dest, Out_B.report	(t));
	strcat	(dest, "\r\n");
	return	(dest);
}	;




