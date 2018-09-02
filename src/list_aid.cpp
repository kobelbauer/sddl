/*
 * This file is part of SDDL.
 *
 * SDDL is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * SDDL is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.

 * You should have received a copy of the GNU General Public License
 * along with SDDL.  If not, see <http://www.gnu.org/licenses/>.
 */

/*----------------------------------------------------------------------------*/
/* Project:        Surveillance Data Decoder and Lister                       */
/* File:           src/list_aid.cpp                                           */
/* Contents:       List some aircraft identification                          */
/* Author(s):      kb                                                         */
/* Last change:    2018-04-27                                                 */
/*----------------------------------------------------------------------------*/

#include "basics.h"
                   /* Basic system definitions */
#include "common.h"
                   /* Common declarations and definitions */

#include <stddef.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#if LISTER
/*----------------------------------------------------------------------------*/
/* list_aid        -- List aircraft identification (callsign or tail number)  */
/*----------------------------------------------------------------------------*/

 void list_aid
	(t_Object_Type object, t_Pntr pntr, t_Ui16 length, char *buffer)
{
	t_Adsb *adsb_ptr;
	t_Mlat *mlat_ptr;
	t_Rtgt *rtgt_ptr;
	t_Strk *strk_ptr;
	char tmp[81];

                   /* Check parameters: */
	Assert (e_no_object < object && object < e_object_sentinel,
            "Invalid parameter");
	Assert (pntr != NULL, "Invalid parameter");
	Assert (length > 0, "Invalid parameter");
	Assert (buffer != NULL, "Invalid parameter");

	if (list_no_mode_s)
	{
		goto done;
	}

	switch (object)
	{
	case e_object_adsb:
                   /* ADS-B report */

		adsb_ptr = (t_Adsb *) pntr;

		memset (tmp, 0, 81);
		if (adsb_ptr->target_identification.present)
		{
			sprintf (tmp, " [%s]", adsb_ptr->target_identification.value_idt);
		}
		else
		{
			strncat (tmp, "           ", 80);
		}
		strncat (buffer, tmp, length);
		break;

	case e_object_mlat:
                   /* Multilateration report */

		mlat_ptr = (t_Mlat *) pntr;

		memset (tmp, 0, 81);
		if (mlat_ptr->target_identification.present)
		{
			sprintf (tmp, " [%s]", mlat_ptr->target_identification.value_idt);
		}
		else
		{
			strncat (tmp, "           ", 80);
		}
		strncat (buffer, tmp, length);
		break;

	case e_object_rsrv:
                   /* Radar service message */

		strncat (buffer, "           ", length);
		break;

	case e_object_rtgt:
                   /* Radar target report */

		rtgt_ptr = (t_Rtgt *) pntr;

		memset (tmp, 0, 81);
		if (rtgt_ptr->aircraft_identification.present)
		{
			sprintf (tmp, " [%s]", rtgt_ptr->aircraft_identification.value_idt);
		}
		else
		{
			strncat (tmp, "           ", 80);
		}
		strncat (buffer, tmp, length);
		break;

	case e_object_strk:
                   /* System track message */

		strk_ptr = (t_Strk *) pntr;

		memset (tmp, 0, 81);
		if (strk_ptr->callsign.present &&
            strk_ptr->callsign.value[0] != '\0' &&
            strk_ptr->callsign.value[0] != ' ')
		{
			if (strk_ptr->aircraft_identification.present)
			{
				if (strncmp (strk_ptr->callsign.value,
                             strk_ptr->aircraft_identification.value_idt,
                             strlen (strk_ptr->callsign.value)) == 0)
				{
					sprintf (tmp, " =%-8s=", strk_ptr->callsign.value);
				}
				else
				{
					sprintf (tmp, " #%-8s#", strk_ptr->callsign.value);
				}
			}
			else
			{
				sprintf (tmp, " <%-8s>", strk_ptr->callsign.value);
			}
		}
		else if (strk_ptr->aircraft_identification.present)
		{
			sprintf (tmp, " [%s]", strk_ptr->aircraft_identification.value_idt);
		}
		else
		{
			strncat (tmp, "           ", 80);
		}
		strncat (buffer, tmp, length);

		break;

	default:
		Assert (FALSE, "Unexpected object type");
		break;
	}

	done:          /* We are done */
	return;
}
#endif /* LISTER */
/* end-of-file */
