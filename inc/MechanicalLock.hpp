/*
 * MechanicalLock.hpp
 *
 *  Created on: Jan 20, 2024
 *      Author: jon34
 */
#include	"main.h"
#ifndef INC_MECHLOCKCLAS_HPP_
#define INC_MECHLOCKCLAS_HPP_

#include	"SingleInput.hpp"
#include	"SingleOutput.hpp"

/**	class	MechLock	{	//	Mechanical Lock.
 *
 */
class	MechLock	{	//	Mechanical Lock. Two position sensing switches or sensors, one output to energise.
	char	noid[2] = {'?', 0};
	const char * id = noid;
	const char * txt	()	const	{	return	(id);	}
	SingleOutput	Out;
	SingleInput	In_A;
	SingleInput	In_B;
  public:

	MechLock	(const char * lock_name,
					INPIN A_input_ionum, POLARITY A_polarity,
					INPIN B_input_ionum, POLARITY B_polarity,
					OUTPIN output_ionum, POLARITY Opolarity )	;


	uint32_t	get_two_inbits	()	const	{	;	//	The state read back from input lines
		uint32_t	rv = 0;
		if	(In_A.is_active())	rv |= 1;
		if	(In_B.is_active())	rv |= 2;
		return	(rv)	;							//	Returns 0, 1, 2 or 3
	}


	bool	is_locked		()	const	;


	bool	is_unlocked	()	const	;


	void	set_pin			(bool output_state)	{	//	value 'output_state ^ polarity' is output
		Out.set_pin	(output_state);
	}


	void	read_update	()	{	//	Calls read_update for inputs A and B
		In_A.read_update()	;					//	the Moving Part Rule
		In_B.read_update()	;
	}


	char *	report	(char *)	const	;
};		//	End of class	MechLock	{	//	Mechanical Lock.


#endif



