/*
 * CmdLine.cpp
 *
 *  Created on: Jun 30, 2023
 *      Author: Jon Freeman B Eng Hons
 *
 */
using namespace std;

#include	"CmdLine.hpp"
#include	"Serial.hpp"


extern const char * get_version	();
extern	uint32_t	can_errors;
extern	UartComChan	COM_PORT	;//, *Com_ptrs[];
//extern	UartComChan	ctrl;//, *Com_ptrs[];

extern	bool    fwistest (struct parameters & a)     ;
extern	void	sigs_buggery	()	;	//	In SignalsMain.cpp
extern	void	pin_lister	()	;	//	In RailwayHardware.cpp
extern	void	rtc_buggery	()	;
extern	bool	set_time	(struct parameters & a)	;
extern	bool	set_date	(struct parameters & a)	;

bool	st_cmd	 (struct parameters & a)     {
	return	(set_time	(a));
}


bool	sd_cmd	 (struct parameters & a)     {
	return	(set_date	(a));
}


bool	pl_cmd	 (struct parameters & a)     {
	pin_lister();
	return	(true);
}


bool	rtc_cmd	 (struct parameters & a)     {
	rtc_buggery();
	return	(true);
}

bool	sb_cmd	 (struct parameters & a)     {
	sigs_buggery();
	return	(true);
}

bool    null_cmd (struct parameters & a)     {
	const char t[] = "null command - does nothing useful!\r\n";
	COM_PORT.write(t, strlen(t));
    return	(true);
}

extern	void	ce_show	()	;

//bool	find_word_in_list	(const struct cli_menu_entry_set * list, char * word, int & position)	{
//extern	bool	test_fwil (struct parameters & a)     ;
extern	bool	edit_settings (struct parameters & a)     ;	//	Here from CLI

bool	ce_cmd (struct parameters & a)     {
	ce_show();
    return	(true);
}

/**
*   void    menucmd (struct parameters & a)
*
*   List available terminal commands to pc terminal. No sense in touch screen using this
*/
bool    menucmd (struct parameters & a)     {
    char    txt[240];
//    if  (a.respond) {
        int len = sprintf     (txt, "\r\n\n%s\r\nListing Commands:-\r\n", get_version());
        COM_PORT.write    (txt, len);
        int i = 0;
        while	(a.command_list[i].cmd_word)	{
            int len = sprintf     (txt, "[%s]\t\t%s\r\n", a.command_list[i].cmd_word, a.command_list[i].description);
            COM_PORT.write    (txt, len);
            i++;
        }	//	Endof while	()
        COM_PORT.write("End of List of Commands\r\n", 25);
//    }	//	Endof if (a.respond)
        return	(true);
}

/**
 * bool	set_output_bit	(uint32_t which_out, bool hiorlo)	{
 * Function in "main.c"
 * Number of output BYTES defined as 'IO_DAISY_CHAIN_BYTE_LEN' defined in "main.c", (probably 8)
 * Number of outputs = BYTES * 8 (numbered 0 to 63 when IO_DAISY_CHAIN_BYTE_LEN == 8)
 *
 * Function returns 'false' if 'which_output' contains invalid output number, returns 'true' otherwise
 * Using hiorlo 'true' turns output on, 'false' off.
 */
//extern	"C"	bool	set_output_bit	(uint32_t which_output, bool hiorlo)	;
extern	bool	set_output_bit	(uint32_t which_output, bool hiorlo)	;
//extern	"C"	bool	get_input_bit_debounced	(uint32_t which_input, uint32_t how_many)	;
extern	bool	get_input_bit_debounced	(uint32_t which_input, uint32_t how_many)	;

bool    i_cmd (struct parameters & a)	{	//	read an input
	char	t[55];
	int b = (int)a.flt[0];
	int	len = sprintf(t, "In%d = %c\r\n", b, get_input_bit_debounced(b,1) ? 'T' : 'F');
	COM_PORT.write(t, len);
    return	(true);
}

bool    seton_cmd (struct parameters & a)	{	//	set one or more outputs ON
	int	j;
	int	k = a.numof_floats;
	while	(k > 0)	{
		j = (int)a.flt[--k];
		set_output_bit	(j, true);
	}
    return	(true);
}

bool    clroff_cmd (struct parameters & a)	{	//	set one or more outputs OFF
	int	j;
	int	k = a.numof_floats;
	while	(k > 0)	{
		j = (int)a.flt[--k];
		set_output_bit	(j, false);
	}
    return	(true);
}

bool    vi_cmd (struct parameters & a)	{	//
	COM_PORT.write	("Nothing for vi command to do!\r\n", 31);
    return	(true);
}

extern	bool	edit_settings_cmd	(struct parameters & a);

  /**
  struct  cli_menu_entry_set      const loco_command_list[] = {
  List of commands accepted from external pc through non-opto isolated com port 115200, 8,n,1
  */
  struct  cli_menu_entry_set       const pc_command_list[] = {
      {"?", "Lists available commands", menucmd},
      {"vi", "Fifth, do nothing very much at all really", vi_cmd},
      {"set", "set one or  more output on", seton_cmd},
      {"clr", "clr one or more output off", clroff_cmd},
	  {"i", "read an input", i_cmd},
	  {"ce", "can errors", ce_cmd},
	  {"fwistest", "user settings", fwistest},
	  {"sb", "signals buggery", sb_cmd},
	  {"rtc", "real time clock buggery", rtc_cmd},
	  {"st", "real time clock Time", st_cmd},
	  {"sd", "real time clock Date", sd_cmd},
	  {"pl", "pins lister", pl_cmd},
	  {"us", "user settings", edit_settings_cmd},
      {"nu", "do nothing", null_cmd},
      {nullptr},	//	June 2023 new end of list delimiter. No need to pass sizeof
  }   ;

  CommandLineHandler	command_line_handler	(pc_command_list);	//	Nice and clean

void	check_commands	()	{	//	Called from ForeverLoop
/**
 * bool	UartComChan::test_for_message	()	{
 *
 * Called from ForeverLoop at repetition rate
 * Returns true when "\r\n" terminated command line has been assembled in lin_inbuff
 */
	char * buff_start = COM_PORT.test_for_message();
	if	(buff_start)
		command_line_handler.CommandExec(buff_start);
}


bool	CommandLineHandler::CommandExec(const char * inbuff)	{
	uint32_t	list_pos = 0;
	uint32_t 	cmd_wrd_len;
	par.command_line = inbuff;	//	copy for use by any functions called
	const char * pEnd;
	bool	got_numerics;
	bool	found_cmd = false;
	for	(int j = 0; j < MAX_CLI_PARAMS; j++)	//	zero all float parameter variables
		par.flt[j] = 0.0;

	while	(!found_cmd && pcmdlist[list_pos].cmd_word)	{
		cmd_wrd_len = strlen(pcmdlist[list_pos].cmd_word);
		if	((strncmp(inbuff, pcmdlist[list_pos].cmd_word, cmd_wrd_len) == 0) && (!isalpha(inbuff[cmd_wrd_len])))	{	//	Don't find 'fre' in 'fred'
			found_cmd = true;
			par.numof_floats = 0;
			pEnd = inbuff + cmd_wrd_len - 1;
			got_numerics = false;
			while	(!got_numerics && *(++pEnd))//	Test for digits present in command line
				got_numerics = isdigit(*pEnd);	//	if digits found, have 1 or more numeric parameters to read

			if	(got_numerics)	{
				while   (*pEnd && (par.numof_floats < MAX_CLI_PARAMS))  {          //  Assemble all numerics as doubles
					par.flt[par.numof_floats++] = strtof    ((const char *)pEnd, (char **)&pEnd);
					while   (*pEnd && !isdigit(*pEnd) && ('.' != *pEnd) && ('-' != *pEnd) && ('+' != *pEnd))  {   //  Can
						pEnd++;
					}   //
					if  (((*pEnd == '-') || (*pEnd == '+')) && (!isdigit(*(pEnd+1))) && ('.' !=*(pEnd+1)))
						pEnd = inbuff + strlen(inbuff);   //  fixed by aborting remainder of line
				}		//	Endof while	(*pEnd ...
			}			//	Endof if	(got_numerics)

			par.function_returned = pcmdlist[list_pos].func(par);	//	Execute code for this command

		}
		else	//	Command word not matched at list position 'cnt'
			list_pos++;
	}			//	End of while	(!found_cmd ...
	return	(true);
}



