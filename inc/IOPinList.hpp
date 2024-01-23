/*
 * IOPinList.hpp
 *
 *  Created on: Jul 3, 2023
 *      Author: Jon Freeman  B. Eng. Hons
 *
 *      Here is where circuit board input and output connections get assigned
 *
 *      Input and Output polarity is selected using INV (inverted) or !INV (not inverted)
 *
 */
#ifndef INC_IOPINLIST_HPP_
#define INC_IOPINLIST_HPP_

enum	InputNumbers1	{Inp00, Inp01, Inp02, Inp03, Inp04, Inp05, Inp06, Inp07,
						 Inp08, Inp09, Inp10, Inp11, Inp12, Inp13, Inp14, Inp15,
						 Inp16, Inp17, Inp18, Inp19, Inp20, Inp21, Inp22, Inp23,
						 Inp24, Inp25, Inp26, Inp27, Inp28, Inp29, Inp30, Inp31,
						 Inp32, Inp33, Inp34, Inp35, Inp36, Inp37, Inp38, Inp39,
						 Inp40, Inp41, Inp42, Inp43, Inp44, Inp45, Inp46, Inp47,
						 Inp48, Inp49, Inp50, Inp51, Inp52, Inp53, Inp54, Inp55,
						 Inp56, Inp57, Inp58, Inp59, Inp60, Inp61, Inp62, Inp63,
						}	;	//Honda jazz vk17pyy

enum	InputNumbers2	{InpA00, InpA01, InpA02, InpA03, InpA04, InpA05, InpA06, InpA07,
						 InpA08, InpA09, InpA10, InpA11, InpA12, InpA13, InpA14, InpA15,
						 InpB00, InpB01, InpB02, InpB03, InpB04, InpB05, InpB06, InpB07,
						 InpB08, InpB09, InpB10, InpB11, InpB12, InpB13, InpB14, InpB15,
						 InpC00, InpC01, InpC02, InpC03, InpC04, InpC05, InpC06, InpC07,
						 InpC08, InpC09, InpC10, InpC11, Inpc12, Inpc13, Inpc14, Inpc15,
						 InpD00, InpD01, InpD02, InpD03, InpD04, InpD05, InpD06, InpD07,
						 InpD08, InpD09, InpD10, InpD11, InpD12, InpD13, InpD14, InpD15,
						}	;

enum	OutputNumbers1	{Out00 /*= 1000*/, Out01, Out02, Out03, Out04, Out05, Out06, Out07,
						 Out08, Out09, Out10, Out11, Out12, Out13, Out14, Out15,
						 Out16, Out17, Out18, Out19, Out20, Out21, Out22, Out23,
						 Out24, Out25, Out26, Out27, Out28, Out29, Out30, Out31,
						 Out32, Out33, Out34, Out35, Out36, Out37, Out38, Out39,
						 Out40, Out41, Out42, Out43, Out44, Out45, Out46, Out47,
						 Out48, Out49, Out50, Out51, Out52, Out53, Out54, Out55,
						 Out56, Out57, Out58, Out59, Out60, Out61, Out62, Out63,
						}	;

enum	OutputNumbers2	{OutA00, OutA01, OutA02, OutA03, OutA04, OutA05, OutA06, OutA07,
						 OutA08, OutA09, OutA10, OutA11, OutA12, OutA13, OutA14, OutA15,
						 OutB00, OutB01, OutB02, OutB03, OutB04, OutB05, OutB06, OutB07,
						 OutB08, OutB09, OutB10, OutB11, OutB12, OutB13, OutB14, OutB15,
						 OutC00, OutC01, OutC02, OutC03, OutC04, OutC05, OutC06, OutC07,
						 OutC08, OutC09, OutC10, OutC11, Outc12, Outc13, Outc14, Outc15,
						 OutD00, OutD01, OutD02, OutD03, OutD04, OutD05, OutD06, OutD07,
						 OutD08, OutD09, OutD10, OutD11, OutD12, OutD13, OutD14, OutD15,
						}	;

const	bool	INV		=	true;
const	bool	NONINV 	=	!INV;
const 	bool	INVERTED =	true;	//	used to select input polarity



#define	LEVEL_CROSSING_ACTUATOR1	"Actuator 1",	Out01, INV
#define	LEVEL_CROSSING_ACTUATOR2	"Actuator 2",	Out02, INV
#define	LEVEL_CROSSING_GATE1_INS	Inp01, !INV, Inp02, !INV	//	gates are components of LevelCrossing, therefore fixed names gate1 to gate4 suffuce
#define	LEVEL_CROSSING_GATE2_INS	Inp03, !INV, Inp04, !INV
#define	LEVEL_CROSSING_GATE3_INS	Inp05, !INV, Inp06, !INV
#define	LEVEL_CROSSING_GATE4_INS	Inp07, !INV, Inp08, !INV
#define	LC_TRAFFIC_LIGHT_INS_OUTS	"LC Traffic Light",	Inp09, Inp10, Out03, Out04		//	inputs 8,9, outputs 3,4
#define	LC_LOCK1_PINS				Inp11, !INV, Inp12, !INV, Out05, !INV
#define	LC_LOCK2_PINS				Inp13, !INV, Inp14, !INV, Out06, !INV
#define	LC_ALARM					"LC Alarm",	Out07
#define	PUSH_BUTTON1				"Level Crossing Push Button 1", Inp17, INV, Out08, INV, Out09, INV
#define	PUSH_BUTTON2				"Level Crossing Push Button 2", Inp18, INV, Out10, INV, Out11, INV



//  RailwaySignal

#define	SIG_DN_MAIN_DIST_PINLIST			"Sig Dn Main Dist",			Inp20, Inp21,	Out12, Out13

/*
#define	SIG_DN_MAIN_OUTER_HOME_PINLIST		"Sig Dn Main Outer Home",	Inp03, Inp04,	Out03, Out04
#define	SIG_DN_MAIN_HOME_PINLIST			"Sig Dn Main Home",			Inp05, Inp06,	Out05, Out06
#define	SIG_DN_MAIN_PLATFORM_HOME_PINLIST	"Sig Dn Main Plat Home",	Inp07, Inp08,	Out07, Out08
#define	SIG_DISC_DN_MAIN_BACKING_PINLIST	"Sig Disc Dn Main Backing",	Inp09, Inp10,	Out09, Out10
#define	SIG_STEAM_BAY_EXIT_PINLIST			"Sig Steam Bay Exit",		Inp11, Inp12,	Out11, Out12
#define	SIG_DISC_UP_MAIN_BACKING_PINLIST	"Sig Disc Up Main Backing",	Inp13, Inp14,	Out13, Out14
#define	SIG_UP_LOOP_TO_SHED_PINLIST			"Sig Up Loop To Shed",		Inp15, Inp16,	Out15, Out16
#define	SIG_EXIT_FROM_SHED_PINLIST			"Sig Exit From Shed",		Inp17, Inp18,	Out17, Out18
#define	SIG_UP_LOOP_UP_MAIN_START_PINLIST	"Sig Up Loop Up Main Start",Inp19, Inp20,	Out19, Out20
#define	SIG_UP_LOOP_HOME_PINLIST			"Sig Up Loop Home",			Inp21, Inp22,	Out21, Out22
*/
#define	SIG_UP_MAIN_START_PINLIST			"Sig Up Main Start",		Inp23, Inp24,	Out14, Out15

#define	SIG_UP_MAIN_HOME_PINLIST			"Sig Up Main Home",			Inp25, Inp26,	Out16, Out17

#define	POINTS_FROM_TURNTABLE_PINLIST		"Points 1", 			Inp27,	true,	Inp28,	true, Out18, true, Out19, true
#define	POINTS_DN_MAIN_TO_PLATFORM_PINLIST	"Points 2", 			Inp29,	true,	Inp30,	true, Out20, true, Out21, true
#define	XOVR_STEAMING_BAYS_PINLIST			"DualPointSteamBay", 	Inp31,	Inp32,true,true,Inp33,	Inp34,	true,true,	Out22, Out23, true,true
#define	XOVR_UP_LOOP_UP_MAIN_PINLIST		"DualPointUpLoopUp", 	Inp35,	Inp36,true,true,Inp37,	Inp38,	true,true,	Out24, Out25, true,true




#endif /* INC_IOPINLIST_HPP_ */
