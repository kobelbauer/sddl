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
/* File:           src/list_idt.cpp                                           */
/* Contents:       List some SSR identification                               */
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
/* list_idt        -- List SSR identification                                 */
/*----------------------------------------------------------------------------*/

 void list_idt
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

	switch (object)
	{
	case e_object_adsb:
                   /* ADS-B report */

		adsb_ptr = (t_Adsb *) pntr;
		if (adsb_ptr->mode_3a_info.present)
		{
			strncat (buffer, " ", length);
			strncat (buffer, "A:", length);

			memset (tmp, 0, 81);
			sprintf (tmp, M_FMT_CODE, adsb_ptr->mode_3a_info.code);
			strncat (buffer, tmp, length);

			strncat (buffer, " ", length);
			strncat (buffer, "-", length);
			if (adsb_ptr->mode_3a_info.code_invalid == e_is_true)
			{
				strncat (buffer, "i", length);
			}
			else
			{
				strncat (buffer, "-", length);
			}
			strncat (buffer, "-", length);
		}
		else
		{
			strncat (buffer, " ", length);
			strncat (buffer, "  ", length);
			strncat (buffer, "     ", length);

			strncat (buffer, " ", length);
			strncat (buffer, "   ", length);
		}
		break;

	case e_object_mlat:
                   /* Multilateration report */

		mlat_ptr = (t_Mlat *) pntr;

		if (mlat_ptr->mode_3a_info.present)
		{
			strncat (buffer, " ", length);
			strncat (buffer, "A:", length);

			memset (tmp, 0, 81);
			sprintf (tmp, M_FMT_CODE, mlat_ptr->mode_3a_info.code);
			strncat (buffer, tmp, length);

			strncat (buffer, " ", length);
			if (mlat_ptr->mode_3a_info.code_garbled == e_is_true)
			{
				strncat (buffer, "g", length);
			}
			else
			{
				strncat (buffer, "-", length);
			}
			if (mlat_ptr->mode_3a_info.code_invalid == e_is_true) 
			{
				strncat (buffer, "i", length);
			}
			else
			{
				strncat (buffer, "-", length);
			}
			if (mlat_ptr->mode_3a_info.code_smoothed == e_is_true)
			{
				strncat (buffer, "l", length);
			}
			else
			{
				strncat (buffer, "-", length);
			}
		}
		else
		{
			strncat (buffer, " ", length);
			strncat (buffer, "-:", length);
			strncat (buffer, "-----", length);

			strncat (buffer, " ", length);
			strncat (buffer, "---", length);
		}
		break;

	case e_object_rsrv:
                   /* Radar service message */

		strncat (buffer, "            ", length);
		break;

	case e_object_rtgt:
                   /* Radar target report */

		rtgt_ptr = (t_Rtgt *) pntr;
		if (rtgt_ptr->mode_3a_info.present)
		{
			strncat (buffer, " ", length);
			strncat (buffer, "A:", length);

			memset (tmp, 0, 81);
			sprintf (tmp, M_FMT_CODE, rtgt_ptr->mode_3a_info.code);
			strncat (buffer, tmp, length);

			strncat (buffer, " ", length);
			if (rtgt_ptr->mode_3a_info.code_garbled == e_is_true)
			{
				strncat (buffer, "g", length);
			}
			else
			{
				strncat (buffer, "-", length);
			}
			if (rtgt_ptr->mode_3a_info.code_invalid == e_is_true) 
			{
				strncat (buffer, "i", length);
			}
			else
			{
				strncat (buffer, "-", length);
			}
			if (rtgt_ptr->mode_3a_info.code_smoothed == e_is_true)
			{
				strncat (buffer, "l", length);
			}
			else
			{
				strncat (buffer, "-", length);
			}
		}
		else if (rtgt_ptr->detection_type.reported_from_ads == e_is_true ||
                 rtgt_ptr->detection_type.reported_from_mlt == e_is_true ||
                 rtgt_ptr->detection_type.reported_from_ssr == e_is_true)
		{
			strncat (buffer, " ", length);
			strncat (buffer, "-:", length);
			strncat (buffer, "-----", length);

			strncat (buffer, " ", length);
			strncat (buffer, "---", length);
		}
		else
		{
			strncat (buffer, " ", length);
			strncat (buffer, "  ", length);
			strncat (buffer, "     ", length);

			strncat (buffer, " ", length);
			strncat (buffer, "   ", length);
		}
		break;

	case e_object_strk:
                   /* System track message */

		strk_ptr = (t_Strk *) pntr;

		memset (tmp, 0, 81);
		if (strk_ptr->mode_3a_info.present)
		{
			sprintf (tmp, "A:" M_FMT_CODE, strk_ptr->mode_3a_info.code);
			if (strk_ptr->mode_3a_info.code_garbled)
			{
				strncat (tmp, " g", 80);
			}
			else
			{
				strncat (tmp, " -", 80);
			}
			if (strk_ptr->mode_3a_info.code_invalid) 
			{
				strncat (tmp, "i", 80);
			}
			else
			{
				strncat (tmp, "-", 80);
			}
			if (strk_ptr->mode_3a_info.code_changed)
			{
				strncat (tmp, "c", 80);
			}
			else
			{
				strncat (tmp, "-", 80);
			}
		}
		else if (strk_ptr->observed_by_ssr)
		{
			strncat (tmp, "-:----- ---", 80);
		}
		else
		{
			strncat (tmp, "           ", 80);
		}

		strncat (buffer, " ", length);
		strncat (buffer, tmp, length);

		break;

	default:
		Assert (FALSE, "Unexpected object type");
		break;
	}

	return;
}
#endif /* LISTER */
/* end-of-file */
