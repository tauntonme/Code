/*
 * TurnoutPoints.hpp
 *
 *  Created on: Jan 20, 2024
 *      Author: jon34
 */
#include	"main.h"
#ifndef INC_TURNOUTPOINTSCLAS_HPP_
#define INC_TURNOUTPOINTSCLAS_HPP_

#include	"SingleInput.hpp"
#include	"SingleOutput.hpp"

enum	PointsStates	{UNPOWERED, NORMAAL, OMKEREN, FAULTY}	;	//	Position of points, levers and whatever. Either of UNPOWERED or FAULTY considered 'wrong'


/**	class	Points	{	//	Points - a.k.a. Turnouts
 *
 *	Two inputs conforming to the Moving Parts Rule.
 *	Two outputs for actuators.
 */
class	Points	{	//	Points - a.k.a. Turnouts
	uint32_t		set_state;
	SingleInput		In_A;
	SingleInput		In_B;
	SingleOutput	Out_A;
	SingleOutput	Out_B;
	char	noid[2] = {'?', 0};
	const char * id = noid;
	const char * txt	()	const	{	return	(id);	}
  public:

	Points	(const char * points_name,
			 INPIN inA, POLARITY polAin, INPIN inB, POLARITY polBin,
			 OUTPIN outA, POLARITY polAout, OUTPIN outB, POLARITY polBout):
		In_A		{"Sw_A", inA,  polAin},		//	2 inputs from points travel limit switches
		In_B		{"Sw_B", inB,  polBin},		//	2 inputs from points travel limit switches
		Out_A		{"PointsOut_A", outA, polAout},		//	2 outputs for points actuators , 1 out, 1 back
		Out_B		{"PointsOut_B", outB, polBout}		//	2 outputs for points actuators , 1 out, 1 back
		{	//	Constructor start
			/*	*/
			set_state = UNPOWERED;	//
			Out_A.set_pin	(false);
			Out_B.set_pin	(false);
			if	(points_name)
				id = points_name;
		};	//	Constructor end

		void	set_to		(int);	//	set state to one of NORMAAL, NIET_NORMAAL, UNPOWERED

		void	read_update	()	{	//	Read state read back from input lines
			In_A.read_update();
			In_B.read_update();
		}


		uint32_t	get_two_inbits	()	const	{	;	//	The state read back from input lines
			uint32_t	rv = 0;
			if	(In_A.is_active())	rv |= 1;
			if	(In_B.is_active())	rv |= 2;
			return	(rv)	;							//	Returns 0, 1, 2 or 3
		}

		uint32_t	get_two_outbits	()	const	{	;	//	The state read back from output buffers
			uint32_t	rv = 0;
			if	(Out_A.read())	rv |= 1;
			if	(Out_B.read())	rv |= 2;
			return	(rv)	;							//	Returns 0, 1, 2 or 3
		}

		bool	wrong		()	const	;	//	returns true while readback state does not match set state
		bool	normal		()	const	;
		bool	reverse		()	const	;

		char *	report	(char *)	const	;
		uint32_t	tolog	()	const	{		return	(get_two_inbits() + (get_two_outbits() << 2));	}
}	;	//	End of class	Points	{	//	Points - a.k.a. Turnouts


class	DualPoints	{	//	Uses only 1 pair of outputs as point pair work together
	Points	Points1;
	Points	Points2;
	char	noid[2] = {'?', 0};
	const char * id = noid;
	const char * txt	()	const	{	return	(id);	}
  public:
	DualPoints	(
			const char * DualPointsName,
			INPIN in1A, INPIN in1B, POLARITY pol1Ain, POLARITY pol1Bin, INPIN in2A, INPIN in2B, POLARITY pol2Ain, POLARITY pol2Bin,
			OUTPIN outA, OUTPIN outB, POLARITY polAout, POLARITY polBout )
				:
				Points1	{DualPointsName, in1A, pol1Ain, in1B, pol1Bin, outA, polAout, outB, polBout},
				Points2	{DualPointsName, in2A, pol2Ain, in2B, pol2Bin, outA, polAout, outB, polBout}
		{	//	consructor start
			if	(DualPointsName)
				id = DualPointsName;
		}	//	constructor end
		void	set_to		(int);	//	set state to one of NORMAAL, NIET_NORMAAL, UNPOWERED
		void	read_update	();		//	Read state read back from input lines
		bool	wrong		()	const	;	//	returns true while readback state does not match set state
		bool	normal		()	const	;
		bool	reverse		()	const	;
		char *	report	(char *)	const	;	//	{	Points1.report();	Points2.report();	}
		uint32_t	tolog	()	const	{	return	(Points1.tolog() + (Points2.tolog() << 4));	}
}	;	//	End of class	DualPoints	{


#endif



