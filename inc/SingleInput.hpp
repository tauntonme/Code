/*
 * SingleInput.hpp
 *
 *  Created on: Jan 20, 2024
 *      Author: jon34
 */
#include	"main.h"
#ifndef INC_SINGINPCLAS_HPP_
#define INC_SINGINPCLAS_HPP_

#define	DEFAULT_DEBOUNCE_VALUES	5	//	Switch readings will be the majority out of this number of consecutive reads (one read per ForeverLoop pass)
typedef	uint32_t	INPIN;
typedef	bool		POLARITY;


/**	class	SingleInput	{	This is a fundamental base unit reused in most objects
 *	Use : -	Choice of three
 *
 *	SingleInput	Fred	("InName", IO In PinNo);
 *
 *	SingleInput	Fred	("InName", IO In PinNo, input polarity true or false);
 *
 *	SingleInput	Fred	("InName", IO In PinNo, input polarity true or false, rise_function, fall_function);
 *
 *	input polarity caters for N.O or N.C. switch contacts.
 *	Polarity set either as parameter at instantiation, or
 *	explicitly set using 'set_polarity(bool p)'
 *
 *	SingleInput is the 'atomic' and only approved method for reading Inputs.
 *	Higher level objects using 'n' inputs contain 'n' SingleInput objects
 *
 *	bool	read_update()	returns latest captured input state
 *
 * 							Input readings are 'debounced' in read_update()
 */
class	SingleInput	{
	INPIN	input_ionum;	//	Which physical numbered input terminal on PCB the wire is screwed into
	bool	polarity 	= false;
	bool	present		= false;
	bool	previous	= false;
	const char	noid[2] = {'?', 0};
	const char * id = noid;
	bool (*rise_func)	() = nullptr;   //  points to function executed on detected input rise
	bool (*fall_func)	() = nullptr;   //  points to function executed on detected input fall
	const char * txt	()	const	{	return	(id);	}

  public:

	/**		SingleInput	(const char * my_id, INPIN);
	 * e.g.
	 * SingleInput	Fred	("My text id", 20);
	 * 		sets id to "My text id", binds input Inp20 to input item Fred
	 * 		polarity defaults to true
	 * 		function pointers default to nullptr
	 */
	SingleInput	(const char * my_id, INPIN);


	/**		SingleInput	(const char * my_id, INPIN, POLARITY);
	 * e.g.
	 * SingleInput	Fred	("My text id", 20);
	 * 		sets id to "My text id", binds input Inp20 to input item Fred
	 * 		polarity initialised by POLARITY
	 * 		function pointers default to nullptr
	 */
	SingleInput	(const char * my_id, INPIN, POLARITY);


	/**		SingleInput	(const char * my_id, INPIN, POLARITY, bool(*rise)(), bool(*fall)());
	 * e.g.
	 * SingleInput	Fred	("My text id", 20);
	 * 		sets id to "My text id", binds input Inp20 to input item Fred
	 * 		polarity initialised by POLARITY
	 * 		function pointers initialised as pointers to two functions,
	 * 		'rise' executed on detected input rising edge
	 * 		'fall' executed on detected input falling edge
	 */
	SingleInput	(const char * my_id, INPIN, POLARITY, bool(*rise)(), bool(*fall)());


	/**	bool	SingleInput::read_update	()	{
	 *
	 * Reads latest Input bit for this pin, 'debounced'.
	 * SingleInput is the 'atomic' low-level input reader.
	 * Higher order objects may contain multiple SingleInput entities.
	 * It is expected that all objects include a 'read_update' function, and that these
	 * higher order objects will call 'read_update's down through any 'tree' structure.
	 *
	 * Inputs are ONLY read through 'read_update', which shoud be called at 'forever loop' repeat rate.
	 *
	 * Edge detection triggers optional external function execution on rising or falling edge
	 *
	 * Returns latest Input bit for this pin, 'debounced'
	 */
	bool	read_update	()	;


	/**bool	SingleInput::is_active	()	const	;
	 *
	 * 		Returns level of input, true or false, as corrected for POLARITY
	 */
	bool	is_active	()	const	;	//	e.g. use if(button1.is_active()).	Returns true for active, false for inactive


	bool	is_rising_edge		()	const	;	//	Returns true when rising edge detected	i.e.	(present && !previous)


	bool	is_falling_edge		()	const	;	//	Returns true when falling edge detected	i.e.	(!present && previous)


	INPIN	get_pin_num	()	const	;


	void	set_polarity	(bool p)	;//	polarity = p;	}


	/**	char *	SingleInput::report	(char * dest )	const	{
	 *
	 * Assembles string of data about this input - pin name, number, level
	 * returns char * to dest for simplest interface to sprintf, pc.write etc
	 */
	char *	report	(char *)	const	;	//

}	;	//	Endof class	SingleInput	{




#endif


