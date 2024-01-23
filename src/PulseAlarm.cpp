/*
 * PulseAlarm.cpp
 *
 *  Created on: Jan 21, 2024
 *      Author: Jon Freeman  B. Eng. Hons
 */
#include	"PulseAlarm.hpp"
#include	<cstdio>	//	for sprintf
#include	<cstring>	//	for strcat


void	PulseAlarm::read_update	()	{
	uint32_t	current_time = millisecs;
	if	(alarm_on && (current_time > alarm_timer))	{
		if	(OutPin.read())	{
			alarm_timer = current_time + space;
			OutPin.set_pin	(false);
		}
		else	{
			alarm_timer = current_time + mark;
			OutPin.set_pin	(true);
		}
	}
}




