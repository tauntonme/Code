/*
 * LevelCrossing.cpp
 *
 *  Created on: Jan 21, 2024
 *      Author: Jon Freeman  B. Eng. Hons
 */
#include	"LevelCrossing.hpp"
#include	<cstdio>	//	for sprintf
#include	<cstring>	//	for strcat



bool	ClassLevelCrossing::set	(uint32_t new_state)	{	//	new_state will be in range 0 to 3
	if	(/*(new_state < 0) ||*/ (new_state > 3L))
		return	(false)	;
	actuator_A.set_pin	((new_state & 1) != 0L);
	actuator_B.set_pin	((new_state & 2) != 0L);
	return	(true)	;
}


bool	ClassLevelCrossing::gates_are_locked	()	const	{
	return	((lock1.get_two_inbits() == 1L) && (lock2.get_two_inbits() == 1L));
}


bool	ClassLevelCrossing::safe_for_road	()	const	{	//	As active, Bs inactive
	return	((gates_are_locked())
			&& (gate1.get_two_inbits() == 1L) && (gate2.get_two_inbits() == 1L)
			&& (gate3.get_two_inbits() == 1L) && (gate4.get_two_inbits() == 1L));
}


bool	ClassLevelCrossing::safe_for_rail	()	const	{
	return	((gates_are_locked())
			&& (gate1.get_two_inbits() == 2L) && (gate2.get_two_inbits() == 2L)
			&& (gate3.get_two_inbits() == 2L) && (gate4.get_two_inbits() == 2L));
}


void	ClassLevelCrossing::read_update()	{	//	Reads and debounces all inputs once per loop pass here.
	gate1.read_update	()	;					//	Debouncing uses processor time,
	gate2.read_update	()	;					//	therfore quicker to debounce once and reuse result
	gate3.read_update	()	;
	gate4.read_update	()	;
	lock1.read_update	()	;
	lock2.read_update	()	;
	alarm.read_update	();
	push_butt_1.read_update();
	push_butt_2.read_update();
	trafficlight.read_update();
	switch	(state)	{			//	This is where any gate action is initiated
	case	0:					//	Reset state, item positions unknown
		break;
	default:
		break;
	}
}


uint32_t	ClassLevelCrossing::tolog()	const	{
	uint32_t	rv = 0L;
	rv	|=	gate1.get_two_inbits();
	rv	|=	(gate2.get_two_inbits() << 2);
	rv	|=	(gate3.get_two_inbits() << 4);
	rv	|=	(gate4.get_two_inbits() << 6);
	rv	|= 	(lock1.get_two_inbits() << 8);
	rv	|= 	(lock1.get_two_inbits() << 10);
	if	(push_butt_1.active())	rv |= 0x1000;	//	Read push button input, ignore outputs
	if	(push_butt_2.active())	rv |= 0x2000;
	if	(actuator_A.read())		rv |= 0x4000;	//	Actuator output
	if	(actuator_B.read())		rv |= 0x8000;
	return	(rv);
}


char *	ClassLevelCrossing::report(char * dest)	const	{
	char	t[200];
	int len;
	sprintf	(dest, "Level Crossing is [%s]\r\n", txt());
	strcat	(dest, gate1.report(t));
	strcat	(dest, gate2.report(t));
	strcat	(dest, gate3.report(t));
	strcat	(dest, gate4.report(t));
	strcat	(dest, lock1.report(t));
	strcat	(dest, lock2.report(t));
	strcat	(dest, push_butt_1.report(t));
	strcat	(dest, push_butt_2.report(t));
	strcat	(dest, actuator_A.report(t));
	strcat	(dest, actuator_B.report(t));
	strcat	(dest, trafficlight.report(t));
//	bool	gates_are_locked();	//	Returns 'true' if gates are locked !
//	bool	safe_for_road	();	//	Returns 'true' if gates are locked AND all 4 gates safe for road
//	bool	safe_for_rail	();	//	Returns 'true' if gates are locked AND all 4 gates safe for rail
//	len = sprintf	(t, "gates_are_locked ?  [%c]\r\n", gates_are_locked() ? 'T': fred ? 'F':'G');
//	pc.write	(dest, strlen(dest));
	len = sprintf	(t, "gates_are_locked ?  [%c]\r\n", gates_are_locked() ? 'T': 'F');
	pc.write	(t, len);
	len = sprintf	(t, "safe_for_road    ?  [%c]\r\n", safe_for_road() ? 'T':'F');
	pc.write	(t, len);
	len = sprintf	(t, "safe_for_rail    ?  [%c]\r\n", safe_for_rail() ? 'T':'F');
	pc.write	(t, len);
	return	(dest);
}





