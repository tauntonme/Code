/*
 * PulseAlarm.hpp
 *
 *  Created on: Jan 20, 2024
 *      Author: jon34
 */
#include	"main.h"
#ifndef INC_PULSEALARMCLAS_HPP_
#define INC_PULSEALARMCLAS_HPP_

#include	"SingleOutput.hpp"
extern	uint32_t	millisecs	;

/**	class	PulseAlarm	{
 * Use e.g. : -
 *
 * PulseAlarm	MyAlarm	("MyAlarmName", Output pin number);
 *
 * Polarity setting removed as not useful. Swap mark/space to suit
 *
 */
class	PulseAlarm	{
	SingleOutput	OutPin	;
	uint32_t	mark	= 250	;
	uint32_t	space	= 250	;
	uint32_t	alarm_timer	= 0L;
	bool	alarm_on	= false	;
	char	noid[2] = {'?', 0};
	const char * id = noid;
public:

//	PulseAlarm	(const char * alarm_name, OUTPIN a, POLARITY b) : OutPin{alarm_name, a, b}	{
	PulseAlarm	(const char * alarm_name, OUTPIN a) : OutPin{alarm_name, a}	{
		OutPin.set_pin	(false);
		if	(alarm_name)	id = alarm_name;
	}

	void	mark_space	(int mark_ms, int space_ms)	{	//	set mark and space times, each in millisecs
		mark = mark_ms;
		space = space_ms;
	}

	void	start_alarm	()	{									//	Start alarm
		alarm_on = true;
		alarm_timer = millisecs + mark;
		OutPin.set_pin	(true);
	}

	void	clr	()	{									//	Stop alarm
		alarm_on = false;
		OutPin.set_pin	(false);
	}

	void	read_update	()	;							//	Call at forever loop repetition rate
	OUTPIN	get_pin_num	()	const	{
		return	(OutPin.get_pin_num())	;
	}
}	;	//	End of class	PulseAlarm	{




#endif



