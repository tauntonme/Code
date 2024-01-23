/*
 * MechaniclaLock.cpp
 *
 *  Created on: Jan 21, 2024
 *      Author: Jon Freeman  B. Eng. Hons
 */
#include	"MechanicalLock.hpp"
#include	<cstdio>	//	for sprintf
#include	<cstring>	//	for strcat

MechLock::MechLock	(const char * lock_name,
					INPIN A_input_ionum, POLARITY A_polarity,
					INPIN B_input_ionum, POLARITY B_polarity,
					OUTPIN output_ionum, POLARITY Opolarity
			)	:
				Out{"Out", output_ionum, Opolarity},	//	"Out", "InpA" and "InpB" component names fixed.
				In_A{"InpA", A_input_ionum, A_polarity,},
				In_B{"InpB", B_input_ionum, B_polarity,}
			{	if(lock_name)	id = lock_name;
		}	//	Nothing else to initialise in constructor


bool	MechLock::is_locked		() const	{
	return	(In_A.is_active() && !In_B.is_active());
}


bool	MechLock::is_unlocked	()	const	{
	return	(!In_A.is_active() && In_B.is_active());
}


char *	MechLock::report	(char * dest)	const	{
	char	t[200] = {0};
	sprintf	(dest, "ElectroMech Lock is [%s],\r\n", txt());
	strcat	(dest, In_A.report	(t));
	strcat	(dest, In_B.report	(t));
	strcat	(dest, Out.report	(t));
//	strcat	(dest,	 normal() ? "Normaal" : reverse() ? "Reverse" : "Wrong");
	strcat	(dest,	 is_locked() ? "Locked" : is_unlocked() ? "Unlocked" : "Wrong");
			//is_unlocked()? "Unlocked" : "Wrong");
	strcat	(dest, "\r\n");
	//	pc.write(dest, strlen(dest));
	return	(dest);
}






