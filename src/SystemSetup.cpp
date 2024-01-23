/*
 * SystemSetup.cpp
 *
 *  Created on: Jan 14, 2024
 *      Author: jon34
 */
using namespace std;

#include	<cstdio>
#include	<cstdbool>
#include	<cstring>

#include	"Serial.hpp"

//#define	COM_PORT	pc	//	Name of com port setup in serial
extern	UartComChan	COM_PORT	;//, *Com_ptrs[];
const	char	version_str[] = "West Buck Sigs Controller v1," __DATE__;
const 	char * 	get_version	()	{	return	(version_str);	}
extern	bool	get_settings	();
extern	void	setup_alarms	()	;
//extern	uint32_t	find_daisy_length	()	;
bool	eeprom_valid_flag = false;

extern	"C"	void	Signal_sys_Setup	()	{	//	Called from main immediately before start of forever loop
	COM_PORT.write("\r\n\n\n\n", 5);
	COM_PORT.write(version_str, strlen(version_str));
	COM_PORT.write("\r\n\n", 3);
	eeprom_valid_flag = get_settings	();
	if	(eeprom_valid_flag)	{	//	Read 24LC64 eeprom for any config data
		COM_PORT.write("Loaded eeprom settings ok\r\n", 27);
	}
	else	{
		COM_PORT.write("eeprom settings load FAILED\r\n", 29);
	}
//	find_daisy_length	();	//	does not work
	setup_alarms	()	;
}




