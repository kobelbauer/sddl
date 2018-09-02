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
/* File:           src/list_sno.cpp                                           */
/* Contents:       List some sensor number                                    */
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
/* list_sno        -- List sensor number                                      */
/*----------------------------------------------------------------------------*/

 void list_sno
	(t_Object_Type object, t_Pntr pntr, t_Ui16 length, char *buffer)
{
	t_Adsb *adsb_ptr;
	t_Mlat *mlat_ptr;
	t_Rsrv *rsrv_ptr;
	t_Rtgt *rtgt_ptr;
	t_Bool sno_present;
	t_Ui16 sno_value;
	t_Strk *strk_ptr;
	char tmp[81];

                   /* Check parameters: */
	Assert (e_no_object < object && object < e_object_sentinel,
            "Invalid parameter");
	Assert (pntr != NULL, "Invalid parameter");
	Assert (length > 0, "Invalid parameter");
	Assert (buffer != NULL, "Invalid parameter");

                   /* Check whether information should be listed: */
	if (!list_sensor_number)
	{
		goto done;
	}

                   /* Preset sensor number: */
	sno_present = FALSE;
	sno_value = 0;

                   /* Extract sensor number: */
	switch (object)
	{
	case e_object_adsb:
                   /* ADS-B report */

		adsb_ptr = (t_Adsb *) pntr;
		if (adsb_ptr->sensor_number.present)
		{
			sno_present = TRUE;
			sno_value = adsb_ptr->sensor_number.value;
		}
		break;

	case e_object_mlat:
                   /* Multilateration report */

		mlat_ptr = (t_Mlat *) pntr;
		if (mlat_ptr->sensor_number.present)
		{
			sno_present = TRUE;
			sno_value = mlat_ptr->sensor_number.value;
		}
		break;

	case e_object_rsrv:
                   /* Radar service message */

		rsrv_ptr = (t_Rsrv *) pntr;
		if (rsrv_ptr->sensor_number.present)
		{
			sno_present = TRUE;
			sno_value = rsrv_ptr->sensor_number.value;
		}
		break;

	case e_object_rtgt:
                   /* Radar target report */

		rtgt_ptr = (t_Rtgt *) pntr;
		if (rtgt_ptr->sensor_number.present)
		{
			sno_present = TRUE;
			sno_value = rtgt_ptr->sensor_number.value;
		}
		break;

	case e_object_strk:
                   /* System track message */

		strk_ptr = (t_Strk *) pntr;
		if (strk_ptr->sensor_number.present)
		{
			sno_present = TRUE;
			sno_value = strk_ptr->sensor_number.value;
		}
		break;

	default:
		Assert (FALSE, "Unexpected object type");
		break;
	}

                   /* List sensor number: */

	memset (tmp, 0, 81);

	if (sno_present)
	{
		sprintf (tmp, "%2hu", sno_value);
	}
	else
	{
		strncat (tmp, "--", 80);
	}

	strncat (buffer, " ", length);
	strncat (buffer, tmp, length);

	done:          /* We are done */
	return;
}
#endif /* LISTER */
/* end-of-file */
