/*
 * MovingGate.hpp
 *
 *  Created on: Jan 20, 2024
 *      Author: Jon Freeman  B. Eng. Hons
 */
#include	"main.h"
#ifndef INC_MOVINGGATECLAS_HPP_
#define INC_MOVINGGATECLAS_HPP_

#include	"SingleInput.hpp"

class	MovingGate	{
	char	noid[2] = {'?', 0};
	const char * id = noid;
	const char * txt	()	const	{	return	(id);	}
	SingleInput	In_A;
	SingleInput	In_B;
  public:

	MovingGate	(
			const char * gate_name,
			INPIN A_input_ionum, POLARITY A_polarity,
			INPIN B_input_ionum, POLARITY B_polarity	)	;


	void	read_update	()	;	//	Calls read_update for inputs A and B

	bool	is_open		()	const	;

	bool	is_closed	()	const	;

	uint32_t	get_two_inbits	()	const	;	//	The state read back from input lines

	char *	report	(char *)	const	;
};


#endif



