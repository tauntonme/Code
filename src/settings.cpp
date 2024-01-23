/*
 * settings.cpp
 *  Created on: Jun 13, 2023
 *      Author: Jon Freeman B Eng Hons
 */
/*	Non-volatile "settings" variables stored in external EEPROM 24LC64
 * 	One 'int32_t' saved in EEPROM per 'setting'
 *
 * 	"settings.cpp / .hpp" evolved from Electronic Motor Control projects,
 * 	it is expected new users will use supplied "settings" code for guidance only.
 *
 *	Reworked Dec 2023
 *	Settings now brought into line with Command Line Interpreter
 *	us numbers replaced by text, e.g. "us defaults".
 *	In use, initial handler from CLI extracts word following "us", e.g. "defaults".
 *	This word is searched in 'set_list'
 *	If match found at position 'n', then function at nth position in set_list is executed, as for CLI.
 *	This should prove easier to maintain as well as being more user intuitive.
 */

#include 	"main.h"

#include	"parameters.hpp"
#include	"Serial.hpp"	//	settings only alterable from Command Line
#include	"CmdLine.hpp"	//	these include <cstdio>, <cstring>, <cstdbool> etc
#include	<cctype>
#include	<cstring>
#include	<cstdio>

#define	LM75_ADDR	0x90	//	Temperature sensor (optional). List other i2c devices here
#define	LC64_ADDR	0xa0	//	EEPROM memory
#define	EEPROM_PAGE_SIZE	32	//	Can write to EEPROM only this many bytes per write (page write)
#define	SETTINGS_BASE_ADDRESS	1024	//	To fit around any other uses you may have for EEPROM

extern	UartComChan	pc		;	//	UARTs
extern	I2C_HandleTypeDef hi2c1;	//	I2C

//	Prototypes needed here for visibility within 'set_list'
extern	bool    null_cmd (struct parameters & a)	;	//	Does nothing
bool    set_defaults_cmd (struct parameters & a)	;	//	Resets all factory setting defaults
bool	set_one_wrapper_cmd (struct parameters & a)	;	//	Sets one setting to value if in valid range
bool	board_cmd	(struct parameters & a)	;

bool	find_word_in_string	(const char * s_str, const char * targ_wrd, uint32_t & posn)	;


#define	OPTION_COUNT	8	//	varies with number of settings, compile fails if too small

/*	struct cli_menu_entry_set, and struct parameters, defined in CmdLine.hpp
struct cli_menu_entry_set  {	//  Commands tabulated as list of these structures as seen below
  const char * cmd_word;	//  points to command text e.g. "command_name"
  const char * description;	//  very brief explanation or clue as to purpose of function
  bool (*func)(struct parameters &);   //  points to function code for this menu choice
  const	int32_t	min;		//	setting 'min' and 'max' used to prevent setting outside limits
  const	int32_t	max;
  const	int32_t	de_fault;	//	used in 'settings', all settings put to factory defaults with 'us defaults' cmd
  const	float	mpy = 1.0;	//	Multiplier stored to, e.g. convert A-D reading to '32.45 Volts'
}  ;
*/

class	i2eeprom_settings	{	//	No need to be visible outside this file
	struct  cli_menu_entry_set const  set_list[OPTION_COUNT] = {    // Can not form pointers to member functions.
		{"0",     	"Lists all user settings, alters none", null_cmd}, //	Examples of use follow
		{"board",	"Board ID ascii '1' to '9'", board_cmd,	'1', '9', '0'}, //  BOARD_ID    defaults to '0' before eerom setup for first time 12
		{"defaults","Reset settings to factory defaults", set_defaults_cmd},     //	restore factory defaults to all settings
		{"three",	"Low Volts Cutoff Ramp (bottom end) V times 10", set_one_wrapper_cmd,	100, 650, 220, 10.0},    //
		{"four",	"Low Volts Cutoff Ramp Range, V times 10", set_one_wrapper_cmd,	1, 100, 20, 10.0}, //
		{"five",	"User Setting number five", set_one_wrapper_cmd, 	10, 60, 10, 3.142},   //
		{nullptr},	//	June 2023 new end of list delimiter. No need to pass sizeof
	}	;
	int32_t 	values[OPTION_COUNT + 2] = {0};  //  These get loaded with stored values read from EEPROM
	uint32_t	set_list_position = 0;
	bool		eeprom_status = false;	//	Set true only in 'load' if eeprom found good
	bool	write_eeprom	(uint32_t address, const char *buffer, uint32_t size)	const	;
	bool	fcwi_set_list	(const char * wrd, uint32_t & position)	const;	//	Used in 'edit' and 'read1'
  public:
	i2eeprom_settings	()	{	//	Constructor
	}	;
    bool	read1 (const char *, int32_t &, float & mult)	const ;  //  Read one setup int32 and one float mpy value
	bool	set_one	(struct parameters & );	//	This is only way to write a setting value
	bool	save	()	const	;	//	Write settings to EEPROM
	bool	load	()	;	//	Load settings from EEPROM
	bool	edit	(struct parameters & )	;	//	edit one setting value if found
	bool	set_defaults	()	;				//	All settings to factory defaults
	void	list	()	const	;						//	Show set_list contents
}	;

i2eeprom_settings	j_settings		;	//	Create one i2eeprom_settings object named 'j_settings'

bool	get_settings	()	{	//	called as part of system init from 'Signal_sys_Setup' in file CmdLine.cpp
	return	(j_settings.load());	//	true on success
}


bool    set_defaults_cmd (struct parameters & a)     {
	return	(j_settings.set_defaults());
}


bool	edit_settings_cmd (struct parameters & a)     {	//	Here from CLI having found "us "
	return	(j_settings.edit	(a));
}


bool	board_cmd	(struct parameters & a)	{
	a.flt[0] += 48.0;	//	Turn 0 to 9 into '0' to '9'
	return	(j_settings.set_one	(a));
}

/*
template	<typename T>	//	This is untested
bool	in_range	(T value, T position)	{
	return	((value >= set_list[position].min) && (value <= set_list[position].max));
}
*/


/**	bool	i2eeprom_settings::fcwi_set_list	(const char * word, uint32_t & position)	{
 * 	fcwi_set_list	find_command_word_in_set_list
 *
 *	CLI and 'settings' menu data now has unified structure, being a list of
 *	 "struct cli_menu_entry_set" structures.
 *	 First entry of each struct is "cmd_word", the '?', 'menu' or whatever text input by user to invoke action.
 *
 *	Returns false if word not found in list, int position unaffected
 *	Returns true if match is found, int position set to position in list
 */
//#define	FCWIVERBOSE
bool	i2eeprom_settings::fcwi_set_list	(const char * wrd, uint32_t & position)	const	{//Used in 'edit' and 'read1'
	int i = 0;
	uint32_t	junk;	//	not interested in how far along string found. Need position in list, not line
#ifdef	FCWIVERBOSE
	char	q[100];
	int	len = sprintf	(q, "Serching for [%s] in list, len %d\r\n", wrd, strlen(wrd));
	pc.write(q, len);
#endif
	while	(set_list[i].cmd_word)	{
//		bool	find_word_in_string	(const char * s_str, const char * targ_wrd, uint32_t & posn)	{
#ifdef	FCWIVERBOSE
		len = sprintf	(q, "list entry %d = [%s], len %d\r\n", i, set_list[i].cmd_word, strlen(set_list[i].cmd_word));
		pc.write(q, len);
#endif
		if	(find_word_in_string(set_list[i].cmd_word, wrd, junk))	{
//		if	(strncmp	(wrd, set_list[i].cmd_word, strlen	(wrd)) == 0)	{
#ifdef	FCWIVERBOSE
			len = sprintf	(q, "Match found for %s at position %d\r\n", wrd, i);
			pc.write	(q, len);
#endif
			position = i;
			return	(true);
		}
		i++;
	}
#ifdef	FCWIVERBOSE
	len = sprintf	(q, "\r\nMatch not found, End of list\r\n");
	pc.write(q, len);
#endif
	return	(false);
}


void	i2eeprom_settings::list	()	const	{
	uint32_t	i = 0;
	int32_t		val;
	float	mult;
	char	t[100];
	uint32_t	len = sprintf	(t, "\tEEPROM Settings, option count = %d\r\n", OPTION_COUNT);
	pc.write	(t, len);
	while	(set_list[i].cmd_word != nullptr)	{
		if	(j_settings.read1	(set_list[i].cmd_word, val, mult))	{	//	loads val and mult
			len = sprintf	(t, "%s [%ld] %s, min %ld, max %ld, def %ld, fmpy %.2f\r\n",
				set_list[i].cmd_word,
				val,	//j_settings.read1	(uscl[i].cmd_word, i),
				set_list[i].description,
				set_list[i].min, set_list[i].max, set_list[i].de_fault,
				set_list[i].mpy );
			pc.write	(t, len);
			i++;
		}	//	else error in read1
	}
	pc.write	("\tEnd of EEPROM Settings\r\n", 25);
}


#define	SET1VERBOSE
bool	i2eeprom_settings::set_one	(struct parameters & a)	{	//	????search for any 2nd word in user input command
	int32_t	new_val = (int32_t)a.flt[0];	//	no, this has been done in edit, this func called from edit
#ifdef	SET1VERBOSE
	char	t[100];
	int	len = sprintf	(t, "At set_one_wrapper_cmd [%s], %.2f, %.2f, %.2f\r\n", a.second_word, a.flt[0], a.flt[1], a.flt[2]);
	pc.write	(t, len);
#endif
	if	(a.numof_floats != 1)	{
#ifdef	SET1VERBOSE
		len = sprintf	(t, "In settings set_one, error, found %ld parameters\r\n", a.numof_floats);
		pc.write	(t, len);
#endif
		return	(false);
	}
	if	(new_val == values[set_list_position])
		return	(true);	//	setting unchanged, nothing to do
	if	((new_val >= set_list[set_list_position].min) && (new_val <= set_list[set_list_position].max))	{
		values[set_list_position] = new_val;
		j_settings.save	();	//	Write modified settings to EEPROM
#ifdef	SET1VERBOSE
		len = sprintf	(t, "Setting New Value for {%s}. %ld\r\n", a.second_word, new_val);
		pc.write	(t, len);
#endif
		return	(true);
	}
#ifdef	SET1VERBOSE
	len = sprintf	(t, "Failed to set setting %ld, to %ld, min %ld, max %ld\r\n",
			set_list_position, new_val, set_list[set_list_position].min, set_list[set_list_position].max);
	pc.write	(t, len);
#endif
	return	(false);
}

bool    set_one_wrapper_cmd (struct parameters & a)     {	//	Called via edit, a.second_word found in edit
	return	(j_settings.set_one	(a));
}



/**	bool	find_word_in_string	(const char * s_str, const char * targ_wrd, uint32_t & posn)	;
 *
 * Searches search string 's_str' for first occurrence of target string "targ_wrd"
 * Returns 'true' if target found, with 'posn' set to start position within search string
 * Returns 'false' if target not found.
 *
 * Checks for non-alpha leading and trailing characters to prevent finding e.g. "and" in "wander"
 *
 */
#define	FWISVERBOSE

bool	find_word_in_string	(const char * s_str, const char * targ_wrd, uint32_t & posn)	{
	uint32_t	s_len = strlen	(s_str);
	uint32_t	w_len = strlen	(targ_wrd);
	uint32_t	max_poss_positions;
#ifdef	FWISVERBOSE
	char	t[100];
	uint32_t	len;
	len = sprintf	(t, "Searching [%s] for [%s], ", s_str, targ_wrd);
	pc.write	(t, len);
#endif
if	(w_len <= s_len)	{	//	Target word can not be longer than search string, return false if it is
		max_poss_positions = s_len - w_len;
		for	(uint32_t i = 0; i <= max_poss_positions; i++)	{//	'i' is search start position in s_str
			if	((strncmp	(s_str + i, targ_wrd, w_len) == 0)	//	Found, (but may have found "and" in "wander")
				&&	((i == 0) || !(isalpha(s_str[i - 1])))		//	and any char before is not alpha
				&&	(			 !(isalpha(s_str[i + w_len]))) )	{//	and any char after is not alpha
					posn = i;
#ifdef	FWISVERBOSE
					len = sprintf	(t, "True at position %ld\r\n", i);
					pc.write	(t, len);
#endif
					return	(true);
			}	//	endof if	((strncmp	(s_str + i, targ_wrd, w_len) == 0) && .. && .. )
		}	//		endof for	(uint32_t i = 0; i < max_poss_positions; i++)
	}	//			endof if	(w_len <= s_len)
#ifdef	FWISVERBOSE
	pc.write	("False\r\n", 7);
#endif
	return	(false);
}

bool	fwistest	(struct parameters & a)	{
	uint32_t	pis;	//	position in string
	find_word_in_string	("wander", "and", pis);
	find_word_in_string	("wo ander", "and", pis);
	find_word_in_string	("wand er", "and", pis);
	find_word_in_string	("wo and er", "and", pis);
	find_word_in_string	("first second word", "first", pis);
	find_word_in_string	("first second word", "second", pis);
	find_word_in_string	("first second word", "word", pis);
	find_word_in_string	("first second word", "second word", pis);
	find_word_in_string	("first second word", "first second ", pis);
	find_word_in_string	("freda", "freda", pis);
	find_word_in_string	("freda", "zfredaz", pis);
	find_word_in_string	("zfredaz", "freda", pis);
	pc.write	("\r\n", 2);
	return	(true);
}


//	bool	i2eeprom_settings::edit    (struct parameters & a) {	//	Redirected Here from CLI having found "us "
//	Returns true on success, false if any 'second word' on command line not found or not recognised
//
//	Extract any word following "us " or other command word
//	Test for word in list
//	Update setting if apt
bool	i2eeprom_settings::edit    (struct parameters & a) {	//	Redirected Here from CLI having found "us "
	uint32_t i = 0;
	const char * p = a.command_line;	//	start of user input
	//	Read any second word entered on the command line
	a.second_word[0] = 0;
	if	(!(eeprom_status))	{
		pc.write	("Can't set, EEPROM bad\r\n", 23);
		return	(false);
	}
	while	(isalpha(*p))	{	p++;	}	//	read over "cmdwrd "	//	User setting commands begin "us "
	while	(isspace(*p))	{	p++;	}	//	read over " "
	while	(isalpha(*p) && *p && (i < MAX_2ND_WORD_LEN))	{	//	assemble word, to max length
		a.second_word[i++] = *p++;
		a.second_word[i] = 0;
	}				//	Have extracted any second word [fred] from "anycommand fred 4" etc user command
	if	(i == 0)	{	//	length of second word found
		return	(false);
	}	//	To pass here a second word was found, written to a.second_word[]

//	if	(!find_2nd_word(a))	//	Copies any second word, if found, into a.second_word
//		return	(false);
	if	(fcwi_set_list	(a.second_word, set_list_position)) {	//	Sets 'set_list_position' if match found

		a.function_returned = set_list[set_list_position].func(a)	;	//	Execute function, returns bool

		pc.write	("Back from settings function\r\n", 29);
		return	(true);
	}
	return	(false);
}



bool	i2eeprom_settings::set_defaults    () {         //  Put default settings into EEPROM and local buffer
	bool	rv;
    for (unsigned int i = 0; i < OPTION_COUNT; i++)  {
        values[i] = set_list[i].de_fault;       //  Load defaults and 'Save Settings'
    }
    rv =  save    ();
    if	(rv)
    	pc.write	("save GOOD\r\n", 11);
    else
    	pc.write	("save BAD!", 11);
    return	(rv);
}


bool	i2eeprom_settings::read1  (const char * which, int32_t & tmp, float & mult)	const	{	//  Read one setup char value from private buffer 'settings'
	uint32_t position_in_list;
	if	(fcwi_set_list	(which, position_in_list))	{
		tmp = values[position_in_list];
		mult = set_list[position_in_list].mpy;
		return	(true);
	}
	return	(false);
}


bool	read_setting	(const char * which, int32_t & result, float & mult)	{
	return	(j_settings.read1(which, result, mult));
}

/*
KEEP THIS FOR REFERENCE
bool	i2eeprom_settings::read_eeprom	(uint32_t address, const char *buffer, uint32_t size)	{
	HAL_StatusTypeDef	ret;	//	Used to test return results of HAL functions
	uint8_t	addr[4];
	addr[0] = address >> 8;
	addr[1] = address & 0xff;
	ret = HAL_I2C_Master_Transmit	(&hi2c1, LC64_ADDR, addr, 2, 10);	//	write 2 byte address
	if	(ret != HAL_OK)
		return	(false);
	HAL_Delay(1);
  	ret = HAL_I2C_Master_Receive	(&hi2c1, LC64_ADDR, (uint8_t*)buffer, size, 100);
	if	(ret != HAL_OK)
		return	false;
	return	(true);
}*/


bool	i2eeprom_settings::write_eeprom	(uint32_t address, const char *buffer, uint32_t size)	const	{
		HAL_StatusTypeDef	ret;	//	Used to test return results of HAL functions
		uint8_t	values[40];
		// Check the address and size fit onto the chip.
		if	((address + size) >= 8192)
			return	(false);			//	Fail - Attempt to write something too big to fit
	    const char *page = buffer;
	    uint32_t 	left = size;
	    // While we have some more data to write.
	    while (left != 0) {
	        // Calculate the number of bytes we can write in the current page.
	        // If the address is not page aligned then write enough to page
	        // align it.
	        uint32_t toWrite;
	        if ((address % EEPROM_PAGE_SIZE) != 0) {
	            toWrite = (((address / EEPROM_PAGE_SIZE) + 1) * EEPROM_PAGE_SIZE) - address;
	            if (toWrite > size) {
	                toWrite = size;
	            }
	        } else {
	            if (left <= EEPROM_PAGE_SIZE) {
	                toWrite = left;
	            } else {
	                toWrite = EEPROM_PAGE_SIZE;
	            }
	        }
	        //printf("Writing [%.*s] at %d size %d\n\r", toWrite, page, address, toWrite);
	        // Start the page write with the address in one write call.
	        values[0] = /*(char)*/(address >> 8);	//	these are uint8_t
	        values[1] = /*(char)*/(address & 0x0FF);

	        for (uint32_t count = 0; count != toWrite; ++count)
	        	values[count + 2] = *page++;
	        ret = HAL_I2C_Master_Transmit	(&hi2c1, LC64_ADDR, values, toWrite + 2, 100);	//	write 2 byte address followed by n data
	        if	(ret != HAL_OK)
	        	return	(false);

	        HAL_Delay(5);			//        waitForWrite();
	        left -= toWrite;        // Update the counters with the amount we've just written
	        address += toWrite;
	    }
		return	(true);
}



bool	i2eeprom_settings::load	()	{
	uint8_t		tmp[4] = {0};
	eeprom_status = true;
	tmp[0] = (SETTINGS_BASE_ADDRESS >> 8);
	tmp[1] = (SETTINGS_BASE_ADDRESS & 0xff);
	HAL_StatusTypeDef	ret = HAL_I2C_Master_Transmit	(&hi2c1, LC64_ADDR, tmp, 2, 10);	//	write 2 byte address
	if	(ret != HAL_OK)
		eeprom_status = (false);
	HAL_Delay(1);
	ret = HAL_I2C_Master_Receive	(&hi2c1, LC64_ADDR, (uint8_t*)values, OPTION_COUNT * 4, 100);
	if	(ret != HAL_OK)	{
		eeprom_status = (false);
	}
	return	(eeprom_status);
}


bool	i2eeprom_settings::save	()	const	{
	char * buff = (char *)values;
	return	(write_eeprom	(SETTINGS_BASE_ADDRESS, buff, OPTION_COUNT * 4));	//	4 = sizeof(int32_t)
}

/*	Useful only if LM75 temperature sensor connected
bool	read_LM75_temperature	(float & temperature)	{	//	Can not detect sensor. Check by temperature reported for sanity
	HAL_StatusTypeDef	ret;	//	Used to test return results of HAL functions
	uint8_t	i2c_tx_buff[10];
	uint8_t	i2c_rx_buff[10];
	  i2c_tx_buff[0] = i2c_tx_buff[1] = 0;	//	pointer to temperature data
	  ret = HAL_I2C_Master_Transmit	(&hi2c1, LM75_ADDR, i2c_tx_buff, 1, 10);
	  if	(ret != HAL_OK)
		  return	(false);
	  int16_t tmp = 0;	//	i2c send addr to LM75 worked
	  ret = HAL_I2C_Master_Receive	(&hi2c1, LM75_ADDR, i2c_rx_buff, 2, 10);
	  if	(ret != HAL_OK)
		  return	(false);
	  tmp = ((int16_t)i2c_rx_buff[0] << 3) | (i2c_rx_buff[1] >> 5);	//	read two temperature bytes
	  if(tmp > 0x3ff)	{	//	temperature is below 0
		  tmp |= 0xf800;	//	sign extend
	  }
	  temperature = ((float) tmp) / 8.0;	//
	return	(true);
}*/



