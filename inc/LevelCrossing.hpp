/*
 * LevelCrossing.hpp
 *
 *  Created on: Jan 20, 2024
 *      Author: jon34
 */
#include	"main.h"
#ifndef INC_LEVELCROSSINGCLAS_HPP_
#define INC_LEVELCROSSINGCLAS_HPP_

#include	"Serial.hpp"
#include	"SingleInput.hpp"
#include	"SingleOutput.hpp"
#include	"PushButton.hpp"
#include	"MechanicalLock.hpp"
#include	"MovingGate.hpp"
#include	"LinesideSignal.hpp"
#include	"PulseAlarm.hpp"
extern	UartComChan pc;

/**
 * class	ClassLevelCrossing	{
 *
 * As defined here, a Level Crossing has 4 gates, 2 locks, 2 push buttons, 1 alarm and 1 traffic light
 * 2 actuator outputs.
 *
 * Note West Buckland level crossing object instantiated in 'SignalsMain.cpp', using pin list data in 'IOPinList.hpp'
 */
class	ClassLevelCrossing	{
	//    	state = 0L;	//	Reset state, we don't know the position of any parts yet.
 		uint32_t	state	= 0L	;		//	state of the level crossing (not used to 9th June 2023 J.F.)
// 		uint32_t	timer1	= 0L	;
//		uint32_t	timer2	= 0L	;
// 		uint32_t	timer3	= 0L	;
 		char	noid[2] = {'?', 0};
 		const char * id = noid;
 		const char * txt	()	const	{	return	(id);	}
 		MovingGate		gate1	;	//	Level crossing has 4 gates, MovingParts
 		MovingGate	 	gate2	;
 		MovingGate	 	gate3	;
 		MovingGate		gate4	;
		MechLock		lock1	;
		MechLock		lock2	;				//	2 gate locks, MovingParts
		LinesideSignal	trafficlight;			//	LinesideSignal used here as road traffic lights
 		SingleOutput	actuator_A;		//	pneumatics
 		SingleOutput	actuator_B;
 		PushButton		push_butt_1;	//	one input from switch, two outputs for indicator lights or whatever
 		PushButton		push_butt_2;
public:
 		PulseAlarm		alarm;			//

    ClassLevelCrossing (	//
    		//LEVEL_CROSSING_GATE1_INS
    		const char * level_crossing_name,
			INPIN ain1, 	POLARITY pa1, 	INPIN bin1, POLARITY pb1,	//	gate1 info,	gate is 'MovingPart' with 2 position detectors.
			INPIN ain2, 	POLARITY pa2, 	INPIN bin2, POLARITY pb2, //	gate2 info,	bool pa1 etc are polarity selectors for N.O. or N.C. switch types
			INPIN ain3, 	POLARITY pa3, 	INPIN bin3, POLARITY pb3, //	gate3 info
			INPIN ain4, 	POLARITY pa4, 	INPIN bin4, POLARITY pb4, //	gate4 info
			INPIN locka1, 	POLARITY lockap1, INPIN lockb1, 	POLARITY lockbp1, OUTPIN lock1opin, POLARITY lock1opol,	//	lock1 info
			INPIN locka2, 	POLARITY lockap2, INPIN lockb2, 	POLARITY lockbp2, OUTPIN lock2opin, POLARITY lock2opol,	//	lock2 info
			const char * traf_light_txt,	INPIN tli1,		INPIN	tli2,	OUTPIN	tlo1,	OUTPIN	tlo2,	//	traffic light I/O numbers
			const char * alarmtxt,	OUTPIN	alarm_pin,						//	alarm
			const char * acto1txt,	OUTPIN	act1opin, POLARITY acto1pol,
			const char * acto2txt,	OUTPIN	act2opin, POLARITY acto2pol,
			const char * but1name, INPIN pbut1in, POLARITY pb1pol1, OUTPIN but1opin1, POLARITY but1opol1, OUTPIN but1opin2, POLARITY but1opol2,
			const char * but2name, INPIN pbut2in, POLARITY pb2pol1, OUTPIN but2opin1, POLARITY but2opol1, OUTPIN but2opin2, POLARITY but2opol2
    ) :
 	        gate1	{"Gate1", ain1, pa1, bin1, pb1},	//	two input port numbers, two boolean polarity selectors
 	        gate2	{"Gate2", ain2, pa2, bin2, pb2},	//	Jan 2024 - added const char * helpful text
 	        gate3	{"Gate3", ain3, pa3, bin3, pb3},
			gate4	{"Gate4", ain4, pa4, bin4, pb4},

			lock1	{"Lock1", 	locka1, lockap1,		//	Lock 1 Input A
								lockb1, lockbp1,		//	Lock 1 Input B
								lock1opin, lock1opol},	//	Lock 1 actuator output
    		lock2	{"Lock2", 	locka2, lockap2, 	//	Lock 2 Input A
								lockb2, lockbp2,		//	Lock 2 Input B
								lock2opin, lock2opol},	//	Lock 2 actuator output
			trafficlight	{traf_light_txt, tli1, tli2, tlo1, tlo2},	//	two inputs, two outputs
			actuator_A		{acto1txt, act1opin, acto1pol},
			actuator_B		{acto2txt, act2opin, acto2pol},
			push_butt_1		{but1name, pbut1in, pb1pol1, but1opin1, but1opol1, but1opin2, but1opol2},
			push_butt_2		{but2name, pbut2in, pb2pol1, but2opin1, but2opol1, but2opin2, but2opol2},
			alarm			{alarmtxt, alarm_pin}

			{
		if	(level_crossing_name)
			id = level_crossing_name;
    	alarm.clr();
    }
    //	public member functions for LevelCrossing
	bool	set				(uint32_t);	//	energise actuators
	void	read_update		();	//	Updates input info in all included MovingParts
	bool	gates_are_locked()	const;	//	Returns 'true' if gates are locked !
	bool	safe_for_road	()	const;	//	Returns 'true' if gates are locked AND all 4 gates safe for road
	bool	safe_for_rail	()	const;	//	Returns 'true' if gates are locked AND all 4 gates safe for rail
	char *	report	(char *)	const	;
	uint32_t	tolog	()	const	;
}	;


#endif



