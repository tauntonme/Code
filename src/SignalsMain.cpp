/*
 * SignalsMain.cpp
 *
 *  Created on: Dec 22, 2023
 *      Author: Jon Freeman
 *
 *      No .hpp file
 */
//#include 	"main.h"
#include	"Serial.hpp"
#include	"TimerClass.hpp"
#include	"LevelCrossing.hpp"
#include	"TurnoutPoints.hpp"
#include	"IOPinList.hpp"
//#include	<cstdio>
#include	<cstring>
//#include	<cstdbool>

extern	UartComChan pc;

/**	File	"SignalsMain.cpp"
 *
 * This is where all 'object' components of the signalling system are instantiated.
 *
 *
 */

/**
 * 		ClassLevelCrossing	Level_Crossing	(
 *
 * 		This is where the Level Crossing object is created
 * 		It has multiple parts, each of which is a C++ software 'object'. These are : -
 *
 * 		4	gates
 * 		2	gate locks
 * 		1	traffic light
 * 		1	alarm
 * 		2	actuator outputs for pneumatic gate movers
 * 		2	push buttons for requesting action.
 *
 * 		Objects require a number of setup parameters, these are coded into macros e.g. LC_ALARM etc.
 * 		These macros are in file "IOPinList.hpp",
 * 		where input and outputs are allocated to physical board terminals.
 *
 * 		It is easy to enter multiple use of pin numbers. Startup code should identify such errors.
 */
ClassLevelCrossing	Level_Crossing	(
		"West Buck Level Crossing",
		LEVEL_CROSSING_GATE1_INS,	//	gate1 input numbers, 'true' for switch normally open contacts
		LEVEL_CROSSING_GATE2_INS, 	//	likewise gate 2
		LEVEL_CROSSING_GATE3_INS, 	//	and gate 3
		LEVEL_CROSSING_GATE4_INS,	//	and gate 4
		LC_LOCK1_PINS, 	//	lock1
		LC_LOCK2_PINS,	//	lock2
		LC_TRAFFIC_LIGHT_INS_OUTS,	//	traffic light IO numbers
		LC_ALARM,				//		Out23,	true,				//	alarm
		LEVEL_CROSSING_ACTUATOR1,
		LEVEL_CROSSING_ACTUATOR2,
		PUSH_BUTTON1,
		PUSH_BUTTON2
		);

//	Create the set of 'LinesideSignal' objects, all wired up as specified in IOPinList.hpp
  LinesideSignal  	  Sig_Dn_Main_Distant				(SIG_DN_MAIN_DIST_PINLIST);
//  LinesideSignal	  Sig_Dn_Main_Outer_Home			(SIG_DN_MAIN_OUTER_HOME_PINLIST);
//  LinesideSignal	  Sig_Dn_Main_Home					(SIG_DN_MAIN_HOME_PINLIST);
//  LinesideSignal	  Sig_Dn_Main_To_Platform_Home		(SIG_DN_MAIN_PLATFORM_HOME_PINLIST);
//  LinesideSignal	  Sig_Disc_Dn_Main_Backing			(SIG_DISC_DN_MAIN_BACKING_PINLIST);
//  LinesideSignal	  Sig_Steaming_Bays_Exit			(SIG_STEAM_BAY_EXIT_PINLIST);
//  LinesideSignal	  Sig_Disc_Up_Main_Backing			(SIG_DISC_UP_MAIN_BACKING_PINLIST);
//  LinesideSignal	  Sig_Up_Loop_To_Carriage_Shed		(SIG_UP_LOOP_TO_SHED_PINLIST);
//  LinesideSignal	  Sig_Exit_From_Carriage_Shed		(SIG_EXIT_FROM_SHED_PINLIST);
//  LinesideSignal	  Sig_Up_Loop_To_Up_Main_Start		(SIG_UP_LOOP_UP_MAIN_START_PINLIST);
//  LinesideSignal	  Sig_Up_Loop_Home					(SIG_UP_LOOP_HOME_PINLIST);
  LinesideSignal	  Sig_Up_Main_Start					(SIG_UP_MAIN_START_PINLIST);
  LinesideSignal	  Sig_Up_Main_Home					(SIG_UP_MAIN_HOME_PINLIST);

//	Create the set of 'Points', or 'Turnout' objects, all wired up as specified in IOPinList.hpp
  Points	Turnout_From_Turntable				(POINTS_FROM_TURNTABLE_PINLIST);
	//	Above Manually op, but include in case signalling system needs to know setting
  Points	Turnout_Dn_Main_To_Platform			(POINTS_DN_MAIN_TO_PLATFORM_PINLIST);
  DualPoints	Xover_To_Steaming_Bays			(XOVR_STEAMING_BAYS_PINLIST);
  DualPoints	Xover_Up_Loop_To_Up_Main		(XOVR_UP_LOOP_UP_MAIN_PINLIST);

//	PulseAlarm	Al1	("Alarm Name1", Out07, true);
//	PulseAlarm	Al2	("Alarm Name2", Out08, true);
	PulseAlarm	Al1	("Alarm Name1", Out07);
	PulseAlarm	Al2	("Alarm Name2", Out08);

	/**	All objects created above.
	 *
	 *
	 * 	Signalling system 'signals_engine' works by using objects as below.
	 */

void	sigs_buggery	()	{	//	A test / debug function from command line "sb"
	char	t[2250] = {0};
	Sig_Up_Main_Home.set_occulting();
	Sig_Up_Main_Home.report(t);
	pc.write	(t, strlen(t));
	Level_Crossing.report(t);
	pc.write	(t, strlen(t));
}


bool	tim1start	()	;	//	Pointers to these functions passed to TimerClass
bool	tim2start	()	;	//	Executed when Timer2 started
bool	tim1end		()	;	//	Executed when Timer1 times out
bool	tim2end		()	;

TimerClass	Timer1	("Timer1", tim1start, tim1end);
TimerClass	Timer2	("Timer2", tim2start, tim2end);


/**		void	signals_engine	()	{
 *
 * This is the core workings of the signalling system.
 * The 'signals_engine()' function is called many times per second from the "forever loop" code.
 * This might be 30, 50, 100 times per second, repeat rate defined elsewhere.
 *
 * The first part of 'signals_engine' calls the '.read_update()' function
 * for each and every object created above.
 * This updates reading of all system inputs, with switch debounce.
 */
void	signals_engine	()	{	//	Called from forever loop at loop repeat rate (maybe 30Hz, or 50, or 100 etc)
	//	Every object has a 'read_update' function.  These do any input switch debouncing etc

	Timer1.read_update();	//	Timer has no physical IO pin. Requires regular read_update.
	Timer2.read_update();
	Level_Crossing	.read_update	();
	Al1				.read_update	();	//	PulseAlarm has one 'SingleOutput'
	Al2				.read_update	();

	Sig_Dn_Main_Distant			.read_update()	;
//	Sig_Dn_Main_Outer_Home		.read_update()	;
//	Sig_Dn_Main_Home			.read_update()	;
//	Sig_Dn_Main_To_Platform_Home.read_update()	;
//	Sig_Disc_Dn_Main_Backing	.read_update()	;
//	Sig_Steaming_Bays_Exit		.read_update()	;
//	Sig_Disc_Up_Main_Backing	.read_update()	;
//	Sig_Up_Loop_To_Carriage_Shed.read_update()	;
//	Sig_Exit_From_Carriage_Shed	.read_update()	;
//	Sig_Up_Loop_To_Up_Main_Start.read_update()	;
//	Sig_Up_Loop_Home			.read_update()	;
	Sig_Up_Main_Start			.read_update()	;

	Sig_Up_Main_Home			.read_update()	;

	Turnout_From_Turntable		.read_update()	;
	Turnout_Dn_Main_To_Platform	.read_update()	;
	Xover_To_Steaming_Bays		.read_update()	;
	Xover_Up_Loop_To_Up_Main	.read_update()	;

/**
 * 	All input readings now up to the millisecond
 *
 * 	Can now proceed to working with any updated system inputs and modify any outputs accordingly.
 */
	/*	if	(false)
		Sig_Dn_Main_Distant.set_clear();
	else
		Sig_Dn_Main_Distant.set_danger();
	*/


/*
	if	(false)
		Sig_Dn_Main_Outer_Home.set_clear();
	else
		Sig_Dn_Main_Outer_Home.set_danger();


	if	(Turnout_Dn_Main_To_Platform.reverse() && Level_Crossing.safe_for_rail() && Xover_To_Steaming_Bays.normal())
		Sig_Dn_Main_Home.set_clear();
	else
		Sig_Dn_Main_Home.set_danger();


	if	(Turnout_Dn_Main_To_Platform.reverse() && Level_Crossing.safe_for_rail() && Xover_To_Steaming_Bays.normal())
		Sig_Dn_Main_To_Platform_Home.set_clear();
	else
		Sig_Dn_Main_To_Platform_Home.set_danger();
*/
	/*

	if	(Level_Crossing.safe_for_rail())
		Sig_Disc_Dn_Main_Backing.set_clear();
	else
		Sig_Disc_Dn_Main_Backing.set_danger();


	if	(Xover_To_Steaming_Bays.reverse() && Level_Crossing.safe_for_rail())
		Sig_Steaming_Bays_Exit.set_clear();
	else
		Sig_Steaming_Bays_Exit.set_danger();

	if	(!Xover_Up_Loop_To_Up_Main.wrong())	//	Detect No.11B switchblades in either position
		Sig_Disc_Up_Main_Backing.set_clear();
	else
		Sig_Disc_Up_Main_Backing.set_danger();

	if	(false)
		Sig_Up_Loop_To_Carriage_Shed.set_clear();
	else
		Sig_Up_Loop_To_Carriage_Shed.set_danger();

	if	(false)
		Sig_Exit_From_Carriage_Shed.set_clear();
	else
		Sig_Exit_From_Carriage_Shed.set_danger();


	if	(false)
		Sig_Up_Loop_To_Up_Main_Start.set_clear();
	else
		Sig_Up_Loop_To_Up_Main_Start.set_danger();


	if	(Level_Crossing.safe_for_rail())
		Sig_Up_Loop_Home.set_clear();
	else
		Sig_Up_Loop_Home.set_danger();


	if	(false)
		Sig_Up_Main_Start.set_clear();
	else
		Sig_Up_Main_Start.set_danger();


	if	(Level_Crossing.safe_for_rail())
		Sig_Up_Main_Home.set_clear();
	else
		Sig_Up_Main_Home.set_danger();
*/

}


/**	TimerClass	Timer	("timer name", startfunc, endfunc);
 *
 *
 * 	TimerClass	Timer1	("Timer1", tim1start_function, tim1end_function);
 * 	TimerClass	Timer2	("Timer2", tim2start_function, tim2end_function);
 *
 * 	Test routine using two timers running around a loop starting the other on timeout
 * 	Remember regular calls to read_update() are needed to keep timers working
 *
 *	Arguably the start_function is not particularly useful but is provided for symmetry.
 */


bool	tim1start	()	{	//	might be a problem here with empty functions
	millisecs++;			//	anything to prevent empty function
//	pc.write	("t1 ", 3);
	return	(true);
}

bool	tim2start	()	{
	millisecs--;
//	pc.write	("t2 ", 3);
	return	(true);
}

bool	tim1end	()	{
	Timer2.start_timer();
	return	(true);
}

bool	tim2end	()	{
	Timer1.start_timer();
	return	(true);
}

void	setup_alarms	()
	{
	Timer1.set_period_ms(750);
	Timer2.set_period_ms(1250);
	Timer1.start_timer();
	Al2.mark_space(100,500);
	Level_Crossing.alarm.mark_space(950, 50);
	Level_Crossing.alarm.start_alarm	();					//	start alarm
	Al1.start_alarm	();
	Al2.start_alarm	();
	Sig_Dn_Main_Distant.set_occulting();
}
