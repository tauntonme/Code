/*
 * SingleInput.cpp
 *
 *  Created on: Jan 21, 2024
 *      Author: Jon Freeman  B. Eng. Hons
 *
 *      See SingleInput.hpp for documentation
 */

#include	"SingleInput.hpp"
//#include	"Serial.hpp"
#include	<cstdio>	//	sprintf
//extern	UartComChan pc;
extern	bool	get_input_bit_debounced	(INPIN which_in, uint32_t how_many)	;	//	from Shift_Reg_SPI.cpp
extern	void	inpincounter	(INPIN, const char *)	;	//	Keeps record of what inputs used and how many times !


SingleInput::SingleInput	(const char * myident, INPIN n) : input_ionum{n}	{	//	Constructor without polarity
	if	(myident)
		id = myident;
	inpincounter	(input_ionum, id);	//	Record fact that pin 'inpin_ionum' has been used
}


SingleInput::SingleInput	(const char * myident, INPIN n, POLARITY p) : input_ionum{n}, polarity{p}	{	//	Constructor
	if	(myident)
		id = myident;
	inpincounter	(input_ionum, id);	//	Record fact that pin 'inpin_ionum' has been used
}


SingleInput::SingleInput	(const char * myident, INPIN n, POLARITY p, bool(*rise)(), bool(*fall)()
		) : input_ionum{n}, polarity{p}, rise_func{rise}, fall_func{fall}	{	//	Constructor
	if	(myident)
		id = myident;
	inpincounter	(input_ionum, id);	//	Record fact that pin 'inpin_ionum' has been used
}


bool	SingleInput::read_update	()	{	//	Copies present into previous, updates present with latest read of input pin
	previous = present;		//	Save for possible edge detection
	present = ((get_input_bit_debounced	(input_ionum ,DEFAULT_DEBOUNCE_VALUES)) ^ polarity);
	if	(present != previous)	{	//	input has changed
		if	(present)	{	//	is rising edge
			if	(rise_func != nullptr)
				rise_func	();
		}
		else	{	//	is falling edge
			if	(fall_func != nullptr)
				fall_func	();
		}
	}
	return	(present);
}


bool	SingleInput::is_active	()	const	{	//	e.g. use if(button1.is_active()).	Returns true for active, false for inactive
	return	(present)	;	//	Returns latest debounced input read
}

bool	SingleInput::is_rising_edge		()	const	{	//	Returns true when rising edge detected	i.e.	(present && !previous)
	return	(present && !previous)	;	//	Found __--
}

bool	SingleInput::is_falling_edge		()	const	{	//	Returns true when falling edge detected	i.e.	(!present && previous)
	return	(!present && previous)	;	//	Found --__
}

INPIN	SingleInput::get_pin_num	()	const	{
	return	(input_ionum)	;
}	//

void	SingleInput::set_polarity	(bool p)	{	polarity = p;	}


char *	SingleInput::report	(char * dest )	const	{
	sprintf	(dest, "Input is [%s],\tpin [Inp%02ld], Level[%c]\r\n", txt(), get_pin_num(), is_active() ? 'T':'F');
	return	(dest);
}


