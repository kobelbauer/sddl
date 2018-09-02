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
/* File:           src/list_ano.cpp                                           */
/* Contents:       List some antenna number                                   */
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
/* list_ano        -- List antenna number                                     */
/*----------------------------------------------------------------------------*/

 void list_ano
	(t_Object_Type object, t_Pntr pntr, t_Ui16 length, char *buffer)
{
	t_Bool ano_present;
	t_Ui16 ano_value;
	t_Rsrv *rsrv_ptr;
	t_Rtgt *rtgt_ptr;
	char tmp[81];

                   /* Check parameters: */
	Assert (e_no_object < object && object < e_object_sentinel,
            "Invalid parameter");
	Assert (pntr != NULL, "Invalid parameter");
	Assert (length > 0, "Invalid parameter");
	Assert (buffer != NULL, "Invalid parameter");

                   /* Preset antenna number: */
	ano_present = FALSE;
	ano_value = 0;

                   /* Extract antenna number: */
	switch (object)
	{
	case e_object_adsb:
                   /* ADS-B report */

		/* holds no antenna number */
		break;

	case e_object_mlat:
                   /* Multilateration report */

		/* holds no antenna number */
		break;

	case e_object_rsrv:
                   /* Radar service message */

		rsrv_ptr = (t_Rsrv *) pntr;
		if (rsrv_ptr->rsrv_type != e_rsrv_supervisory)
		{
			ano_present = TRUE;
			ano_value = 1;
		}
		break;

	case e_object_rtgt:
                   /* Radar target report */

		rtgt_ptr = (t_Rtgt *) pntr;
		if (rtgt_ptr->antenna_number.present)
		{
			ano_present = TRUE;
			ano_value = rtgt_ptr->antenna_number.value;
		}
		break;

	case e_object_scv6:
                   /* SASS-C V6 listbox line */

		/* holds no antenna number */
		break;

	case e_object_strk:
                   /* System track message */

		/* holds no antenna number */
		break;

	default:
		Assert (FALSE, "Unexpected object type");
		break;
	}

                   /* List antenna number: */

	memset (tmp, 0, 81);

	if (ano_present)
	{
		sprintf (tmp, "A" M_FMT_UI16, ano_value);
	}
	else
	{
		strncat (tmp, "A-", 80);
	}

	strncat (buffer, " ", length);
	strncat (buffer, tmp, length);

	return;
}
#endif /* LISTER */
/* end-of-file */
