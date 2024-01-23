/*
 * ForeverLoop.cpp
 *
 *  Created on: Jun 6, 2023
 *      Author: Jon Freeman B Eng Hons
 */
#include 	"main.h"

#include	"Serial.hpp"
#include	"CmdLine.hpp"

using namespace std;

extern	UartComChan	pc;		//, * Com_ptrs[];
extern	bool	can_flag;
extern	void	ce_show	()	;
extern	void	check_commands	()	;	//	Looks after all command line input
extern	void	can_try	()	;	//	Test CAN bus

uint32_t	millisecs 			= 0L;
int32_t		forever_loop_timer 	= 0L;
int32_t		slow_loop_timer 	= 0L;
volatile	bool	timer_1ms_flag;


/*void	showme	()	{
	char	txt[255];
	int	len;
	pc.write	("Lev X data\r\n", 12);
	len = sprintf	(txt, "gate1 %d %c, %d %c,\tgate2 %d %c, %d %c,\tgate3 %d %c, %d %c,\tgate4 %d %c, %d %c\r\n",
			Level_Crossing.gate1.A.get_pin_num(), Level_Crossing.gate1.A.active() ? 'T' : 'F', Level_Crossing.gate1.B.get_pin_num(), Level_Crossing.gate1.B.active() ? 'T' : 'F',
			Level_Crossing.gate2.A.get_pin_num(), Level_Crossing.gate2.A.active() ? 'T' : 'F', Level_Crossing.gate2.B.get_pin_num(), Level_Crossing.gate2.B.active() ? 'T' : 'F',
			Level_Crossing.gate3.A.get_pin_num(), Level_Crossing.gate3.A.active() ? 'T' : 'F', Level_Crossing.gate3.B.get_pin_num(), Level_Crossing.gate3.B.active() ? 'T' : 'F',
			Level_Crossing.gate4.A.get_pin_num(), Level_Crossing.gate4.A.active() ? 'T' : 'F', Level_Crossing.gate4.B.get_pin_num(), Level_Crossing.gate4.B.active() ? 'T' : 'F'
			);
	pc.write(txt, len);
	len = sprintf(txt, "alarm %d\r\n", Level_Crossing.alarm.get_pin_num());
	pc.write(txt, len);
}*/
//Sig_Dn_Main_Distant.set_occulting();

#define	FOREVER_LOOP_REPEAT_MS	20
#define	SLOW_LOOP_REPEAT_MS	500
extern	void	send_queued_uart_messages	()	;
extern	bool	spi_inputs_updated	()	;

int32_t	sigs_time;

void	do_even_halfsec_stuff	()	{
	can_try();
}

void	do_odd_halfsec_stuff	()	{
//char	t[100];
//	int len = sprintf	(t, "Sigs_time us = %d\r\n", sigs_time);
//	pc.write	(t, len);
}

void	do_fastest_stuff	()	{
	check_commands	()	;			//	Read and execute any commands rec'd from pc
}

extern	void	signals_engine	()	;

extern	TIM_HandleTypeDef htim6;


void	do_forever_loop_rate_stuff	()	{
	if	(can_flag)	{
		can_flag = false;
//		ce_show	();
	}
	int32_t	old_sigs_time =	__HAL_TIM_GET_COUNTER(&htim6);
//	int	pld_sigs_time =	(&htim6)->Instance->CNT;	//	same as '__HAL_TIM_GET_COUNTER(&htim6)' above


		signals_engine	();


	sigs_time =	__HAL_TIM_GET_COUNTER(&htim6) - old_sigs_time;	//	time consumed by signals_engine in us
	if	(sigs_time < 0)
		sigs_time += 1000;
}

void	one_ms_stuff	()	{
	send_queued_uart_messages	();
	if	(spi_inputs_updated())	{	//	Returns true after update, false when updates not yet available
		do_forever_loop_rate_stuff	()	;	//	As soon as latest inputs have been updated
	}
}

void	do_slow_loop_rate_stuff	()	{
	static	bool	oddeven;
	HAL_GPIO_TogglePin(LD3_GPIO_Port, LD3_Pin);	//	as good a place as any to toggle green Nucleo led
	oddeven = !oddeven;
	if	(oddeven)
		do_even_halfsec_stuff	();
	else
		do_odd_halfsec_stuff	();
}


extern	void	start_spi_TxRx	();	//	Call this to initiate multi-byte SPI transfer

extern "C" void	ForeverLoop	()	{	// Jumps to here from 'main.c'
	while	(true)	{				//	Always forever loop top
		do_fastest_stuff	();		//	While waiting for timer to sync slower stuff
		if	(timer_1ms_flag)	{
			timer_1ms_flag = false;
			one_ms_stuff	();		//	Do all the once per millisec stuff
			if	(forever_loop_timer >= FOREVER_LOOP_REPEAT_MS)	{
				forever_loop_timer -= FOREVER_LOOP_REPEAT_MS;
//no				do_forever_loop_rate_stuff	();
				//	start spi In/Out update sequence all under DMA control.
				start_spi_TxRx	();		//	SPI comms all happens under DMA control with no code interventions
				//	When spi shift reg read/write ends, DMA raises 'complete' interrupt
				//	It is this interrupt that initiates 'do_forever_loop_rate_stuff	()'
				//	By so doing, io data is used at its freshest possible.
				if	(slow_loop_timer > SLOW_LOOP_REPEAT_MS)	{
					slow_loop_timer -= SLOW_LOOP_REPEAT_MS;
					do_slow_loop_rate_stuff	();					//	once per half second
				}	//	endof if	(slow_loop_timer > SLOW_LOOP_REPEAT_MS)
			}		//	endof if	(forever_loop_timer >= FOREVER_LOOP_REPEAT_MS)
		}			//	endof if	(timer_1ms_flag)
	}				//	endof while	(true)
}					//	endof void	ForeverLoop(). Never gets to here, function never returns.

extern	TIM_HandleTypeDef htim6;	//	TIM6 counts 16 bit micro secs, interrupts once per milli sec
//	Callback: timer has reset
void	HAL_TIM_PeriodElapsedCallback(TIM_HandleTypeDef *htim)
{	//	check for timer 6
	if	(htim == &htim6)	{
		timer_1ms_flag = true;
		millisecs++;		//	Global count of milli seconds since power-up
		forever_loop_timer++;
		slow_loop_timer++;
	}
}


