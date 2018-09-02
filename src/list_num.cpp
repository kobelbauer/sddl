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
/* File:           src/list_num.cpp                                           */
/* Contents:       List some generic number                                   */
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
/* list_num        -- List generic number                                     */
/*----------------------------------------------------------------------------*/

 void list_num
	(t_Object_Type object, t_Pntr pntr, t_Ui16 length, char *buffer)
{
	t_Mlat *mlat_ptr;
	t_Rsrv *rsrv_ptr;
	t_Rtgt *rtgt_ptr;
	t_Strk *strk_ptr;
	char tmp[81];

                   /* Check parameters: */
	Assert (e_no_object < object && object < e_object_sentinel,
            "Invalid parameter");
	Assert (pntr != NULL, "Invalid parameter");
	Assert (length > 0, "Invalid parameter");
	Assert (buffer != NULL, "Invalid parameter");

	memset (tmp, 0, 81);

	switch (object)
	{
	case e_object_adsb:
                   /* ADS-B report */

		strncat (tmp, "-----", 80);
		break;

	case e_object_mlat:
                   /* Multilateration target report */

		mlat_ptr = (t_Mlat *) pntr;

		if (mlat_ptr->track_number.present)
		{
			sprintf (tmp, "#%04hu", mlat_ptr->track_number.value);
		}
		else
		{
			strncat (tmp, "-----", 80);
		}
		break;

	case e_object_rsrv:
                   /* Radar service message */

		rsrv_ptr = (t_Rsrv *) pntr;

		if (rsrv_ptr->sector_number.present)
		{
			sprintf (tmp, "  %3hu", rsrv_ptr->sector_number.value);
		}
		else if (rsrv_ptr->step_number.present)
		{
			sprintf (tmp, "  %3hu", rsrv_ptr->step_number.value);
		}
		else
		{
			strncat (tmp, "-----", 80);
		}
		break;

	case e_object_rtgt:
                   /* Radar target report */

		rtgt_ptr = (t_Rtgt *) pntr;

		if (rtgt_ptr->track_number.present)
		{
			sprintf (tmp, "#%04hu", rtgt_ptr->track_number.value);
		}
		else
		{
			strncat (tmp, "-----", 80);
		}
		break;

	case e_object_strk:
                   /* System track message */

		strk_ptr = (t_Strk *) pntr;

		if (strk_ptr->track_number.present)
		{
			sprintf (tmp, "#%04hu", strk_ptr->track_number.value);
		}
		else
		{
			strncat (tmp, "-----", 80);
		}
		break;

	default:
		Assert (FALSE, "Unexpected object type");
		break;
	}

	if (tmp[0] != '\0')
	{
		strncat (buffer, " ", length);
		strncat (buffer, tmp, length);
	}

	return;
}
#endif /* LISTER */
/* end-of-file */
