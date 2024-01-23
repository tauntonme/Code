/*
 * PushButton.hpp
 *
 *  Created on: Jan 20, 2024
 *      Author: Jon Freeman  B. Eng. Hons
 */
#include	"main.h"
#ifndef INC_PUSHBUTTPCLAS_HPP_
#define INC_PUSHBUTTPCLAS_HPP_

#include	"SingleInput.hpp"
#include	"SingleOutput.hpp"

/**	class	PushButton	{
 * 	Use : -
 *	PushButton	Fred	("Name of Push Button",
 *						 Board IO Input pin number, input polarity true or false,
 *						 Out_A Board IO Output pin number, output polarity,
 *						 Out_B Board IO Output pin number, output polarity );
 *
 * Reads one switch contact input, selectable polarity.
 * Two outputs provided to e.g. illuminate "Wait" / "Go" indicators
 */
class	PushButton	{
	char	noid[2] = {'?', 0};
	const char * id = noid;
	SingleInput		In_A;	//	Reads state of push switch
	SingleOutput	Out_A;	//	Two outputs for red and green lamps, alarm or whatever.
	SingleOutput	Out_B;
	bool	latest_in 	= false;
	bool	previous_in = false;
//	void (*press_func)	() = nullptr;   //  points to function executed on detected input rise
//	void (*release_func)() = nullptr;   //  points to function executed on detected input fall
	const char * txt	()	const	{	return	(id);	}
  public:

	PushButton	(	const char * button_name,
					INPIN A_input_ionum, POLARITY A_polarity,
					OUTPIN outnum1, POLARITY outpolarity1,
					OUTPIN outnum2, POLARITY outpolarity2 )	;

	PushButton	(	const char * button_name,	//	Pass function pointers down to SingleInput
					INPIN A_input_ionum, POLARITY A_polarity, bool(*press)(), bool(*release)(),
					OUTPIN outnum1, POLARITY outpolarity1,
					OUTPIN outnum2, POLARITY outpolarity2 )	;


	void	setA	(bool new_state)	{
		Out_A.set_pin	(new_state);
	}

	void	setB	(bool new_state)	{
		Out_B.set_pin	(new_state);
	}

	bool	read_update	()	;	//	Calls read_update for input A

	bool	active		()	const	{	//	e.g. use if(button1.active()).	Returns true for active, false for inactive
		return	(In_A.is_active())	;
	}

	bool	is_rising_edge		()	const	{	//	Returns true when rising edge detected	i.e.	(present && !previous)
		return	(In_A.is_rising_edge	())	;	//	Found __--
	}

	bool	is_falling_edge		()	const	{	//	Returns true when falling edge detected	i.e.	(!present && previous)
		return	(In_A.is_falling_edge	())	;	//	Found --__
	}

	char *	report	(char *)	const	;
}	;	//	End of class	PushButton	{


#endif



