/*
 * SingleOutput.hpp
 *
 *  Created on: Jan 20, 2024
 *      Author: Jon Freeman  B. Eng. Hons
 *
 *      SingleOutput files also contain pincounters and pin use logs
 */
#include	"main.h"
#ifndef INC_SINGOUTPCLAS_HPP_
#define INC_SINGOUTPCLAS_HPP_

typedef	uint32_t	OUTPIN;
typedef	bool		POLARITY;


/**	class	SingleOutput	{
 *	Use : -
 *	SingleOutput	Fred	("Name of Output", Board IO Output pin number, Output polarity true or false);
 *
 *	Output polarity, true or false, selects output sense.
 *	Polarity set either as parameter at instantiation, or
 *	explicitly set using 'set_polarity(bool p)'
 *
 *	SingleOutput is the 'atomic' and only approved method for setting Outputs.
 *	Higher level objects using 'n' outputs contain 'n' SingleOutput objects
 */
class	SingleOutput	{	//	Normal use is to drive output pin. Also usable as 1 bit register with invalid output_ionum
	OUTPIN		output_ionum;
	POLARITY	polarity = true;	//	Outputs normally 'active on', allows for choice of 'active off'.
	char	noid[2] = {'?', 0};
	const char * id = noid;
	const char * txt	()	const	{	return	(id);	}
  public:

	SingleOutput	(const char * myid, OUTPIN which_output)	;

	SingleOutput	(const char * myid, OUTPIN which_output, POLARITY p)	;

	bool	set_pin			(bool output_state)	;	//	value 'output_state ^ polarity' is output

	bool	read		() const	;

	OUTPIN	get_pin_num	() const	;

	void	set_polarity	(bool p)	;

	char *	report	(char *)	const	;

	//	void	read_update	()	;			//	There is nothing to update here
}	;	//	Endof class	SingleOutput	{


#endif




