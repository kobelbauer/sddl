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
/* File:           src/list_ftm.cpp                                           */
/* Contents:       List some frame time (of day)                              */
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
/* list_ftm        -- List frame time (of day) in UTC format                  */
/*----------------------------------------------------------------------------*/

 void list_ftm
	(t_Object_Type object, t_Pntr pntr, t_Ui16 length, char *buffer)
{
	t_Adsb *adsb_ptr;
	t_Bool ftm_present;
	t_Secs ftm_value;
	t_Mlat *mlat_ptr;
	t_Rsrv *rsrv_ptr;
	t_Rtgt *rtgt_ptr;
	t_Step *step_ptr;
	t_Strk *strk_ptr;
	char tmp[81];

                   /* Check parameters: */
	Assert (e_no_object < object && object < e_object_sentinel,
            "Invalid parameter");
	Assert (pntr != NULL, "Invalid parameter");
	Assert (length > 0, "Invalid parameter");
	Assert (buffer != NULL, "Invalid parameter");

                   /* Check whether information should be listed: */
	if (!list_frame_time)
	{
		goto done;
	}

                   /* Preset frame time (of day): */
	ftm_present = FALSE;
	ftm_value = 0.0;

                   /* Extract frame time (of day): */
	switch (object)
	{
	case e_object_adsb:
                   /* ADS-B report */

		adsb_ptr = (t_Adsb *) pntr;
		if (adsb_ptr->frame_time.present)
		{
			ftm_present = TRUE;
			ftm_value = adsb_ptr->frame_time.value;
		}
		break;

	case e_object_mlat:
                   /* Multilateration report */

		mlat_ptr = (t_Mlat *) pntr;
		if (mlat_ptr->frame_time.present)
		{
			ftm_present = TRUE;
			ftm_value = mlat_ptr->frame_time.value;
		}
		break;

	case e_object_rsrv:
                   /* Radar service message */

		rsrv_ptr = (t_Rsrv *) pntr;
		if (rsrv_ptr->frame_time.present)
		{
			ftm_present = TRUE;
			ftm_value = rsrv_ptr->frame_time.value;
		}
		break;

	case e_object_rtgt:
                   /* Radar target report */

		rtgt_ptr = (t_Rtgt *) pntr;
		if (rtgt_ptr->frame_time.present)
		{
			ftm_present = TRUE;
			ftm_value = rtgt_ptr->frame_time.value;
		}
		break;

	case e_object_scv6:
                   /* SASS-C V6 listbox line */

		break;

	case e_object_step:
                   /* System picture step */

		step_ptr = (t_Step *) pntr;
		if (step_ptr->frame_time.present)
		{
			ftm_present = TRUE;
			ftm_value = step_ptr->frame_time.value;
		}
		break;

	case e_object_strk:
                   /* System track report */

		strk_ptr = (t_Strk *) pntr;
		if (strk_ptr->frame_time.present)
		{
			ftm_present = TRUE;
			ftm_value = strk_ptr->frame_time.value;
		}
		break;

	default:
		Assert (FALSE, "Unexpected object type");
		break;
	}

                   /* List frame time (of day): */

	memset (tmp, 0, 81);

	if (ftm_present)
	{
		if (time_bias_defined)
		{
			ftm_value += time_bias;
		}

		sprintf (tmp, "%s", utc_text (ftm_value));
	}
	else
	{
		strncat (tmp, "--:--:--.---", 80);
	}

	strncat (buffer, " [", length);
	strncat (buffer, tmp, length);
	strncat (buffer, "]", length);

	done:          /* We are done */
	return;
}
#endif /* LISTER */
/* end-of-file */
