/*
 * SingleOutput.cpp
 *
 *  Created on: Jan 21, 2024
 *      Author: Jon Freeman  B. Eng. Hons
 *
 *      See SingleOutput.hpp for documentation
 *      SingleOutput files also contain pincounters and pin use logs
 */

#include	"SingleOutput.hpp"
#include	"Serial.hpp"
#include	<cstdio>	//	sprintf
extern	UartComChan pc;

extern	bool	set_output_bit	(OUTPIN which_output, bool hiorlo)	;	// in Shift_Reg_SPI.cpp
extern	bool	get_outreg_1bit	(OUTPIN bitnum)	;

extern	void	outpincounter	(OUTPIN, const char *);	//	Useful for checking not used same pins more than once

SingleOutput::SingleOutput	(const char * myid, OUTPIN which_output)
		 : output_ionum{which_output}	{	//	Constructor without polarity
	if	(myid)
		id = myid;
	outpincounter	(output_ionum, id);	//	Record fact that pin 'inpin_ionum' has been used
}

SingleOutput::SingleOutput	(const char * myid, OUTPIN which_output, POLARITY p)
		 : output_ionum{which_output}, polarity{p}	{	//	Constructor including polarity
	if	(myid)
		id = myid;
	outpincounter	(output_ionum, id);	//	Record fact that pin 'inpin_ionum' has been used
}


bool	SingleOutput::set_pin			(bool output_state)	{	//	value 'output_state ^ polarity' is output
	//	set_output_bit returns true when bit has been changed
	return	(set_output_bit	(output_ionum, (output_state ^ polarity)));
}


bool	SingleOutput::read	() const	{
	return	(get_outreg_1bit(output_ionum))	;//	return last value output
}


OUTPIN	SingleOutput::get_pin_num	() const	{
	return	(output_ionum);
}


void	SingleOutput::set_polarity	(bool p)	{
	polarity = p;
}


char *	SingleOutput::report	(char * dest)	const	{
	sprintf	(dest, "Output is [%s],\tpin [Out%02ld], Level[%c]\r\n", txt(), get_pin_num(), read() ? 'T':'F');
	return	(dest);
}


struct	config_err_log	{		//	To help identify multiple use of same pin problems
	const char * t[4] = {"Spare\0", nullptr, nullptr, nullptr};	//	can form list of conflicting items
	uint32_t	pin		= 0L;
	uint32_t	count	= 0L;
}	;

config_err_log	IP_pinlog[66]	;
config_err_log	OP_pinlog[66]	;


bool	pincounter	(uint32_t pin, const char * id, config_err_log & logadd, const char * inout_txt)	{	//	Keeps record of what inputs used and how many times !
	int	len;
	char	t[100];
	if	((pin < 0) || (pin > 63))	{
		return	(false);
	}
	if	(logadd.count == 0)	{	//	Pin is free to use
		logadd.t[0] = id;
		logadd.count++;
	}
	else	{	//	Pin is not free to use
		len = sprintf	(t, "Error! Multiple use of %sPIN %ld, [%s], [%s]\r\n", inout_txt, pin, logadd.t[0], id);
		pc.write	(t, len);
	}
	return	(true);
}


/**	void	inpincounter	(INPIN pin, const char * id)	{
 * Only call is from class SingleInput constructor, as all inputs are accessed through 'SingleInput' objects.
 */
#define	INPIN	uint32_t
void	inpincounter	(INPIN pin, const char * id)	{	//	Keeps record of what inputs used and how many times !
	bool	result = pincounter	(pin, id, IP_pinlog[pin], "IN");
	if	(!(result))
		pc.write	("IN Pincntr BAD\r\n", 16);
}


/**	void	outpincounter	(OUTPIN pin, const char * id)	{
 * Only call is from class SingleOutput constructor, as all outputs are accessed through 'SingleOutput' objects.
 */
void	outpincounter	(OUTPIN pin, const char * id)	{	//	Useful for checking not used same output pins more than once
	bool	result = pincounter	(pin, id, OP_pinlog[pin], "OUT");
	if	(!(result))
		pc.write	("OUT Pincntr BAD\r\n", 17);
}


void	pin_lister	()	{
	int		len;
	char	t[100];
	pc.write	("Listing Inputs 00 to 31\r\n", 16);
	for	(int i = 0; i < 32; i++)	{
		len = sprintf	(t, "In%2d [%s]\r\n", i, IP_pinlog[i].t[0]);
		pc.write	(t, len);
	}
	pc.write	("\r\nListing Outputs 00 to 31\r\n", 19);
	for	(int i = 0; i < 32; i++)	{
		len = sprintf	(t, "Out%2d [%s]\r\n", i, OP_pinlog[i].t[0]);
		pc.write	(t, len);
	}
}



