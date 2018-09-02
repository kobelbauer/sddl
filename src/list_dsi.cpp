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
/* File:           src/list_dsi.cpp                                           */
/* Contents:       List some data source identifier                           */
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
/* list_dsi        -- List data source identifier                             */
/*----------------------------------------------------------------------------*/

 void list_dsi
	(t_Object_Type object, t_Pntr pntr, t_Ui16 length, char *buffer)
{
	t_Adsb *adsb_ptr;
	t_Bool dsi_present;
	t_Ui16 dsi_value;
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
	if (!list_dsis)
	{
		goto done;
	}

                   /* Preset data source identifier: */
	dsi_present = FALSE;
	dsi_value = 0;

                   /* Extract data source identifier: */
	switch (object)
	{
	case e_object_adsb:
                   /* ADS-B report */

		adsb_ptr = (t_Adsb *) pntr;
		if (adsb_ptr->data_source_identifier.present)
		{
			dsi_present = TRUE;
			dsi_value = adsb_ptr->data_source_identifier.value;
		}
		break;

	case e_object_mlat:
                   /* Multilateration report */

		mlat_ptr = (t_Mlat *) pntr;
		if (mlat_ptr->data_source_identifier.present)
		{
			dsi_present = TRUE;
			dsi_value = mlat_ptr->data_source_identifier.value;
		}
		break;

	case e_object_rsrv:
                   /* Radar service message */

		rsrv_ptr = (t_Rsrv *) pntr;
		if (rsrv_ptr->data_source_identifier.present)
		{
			dsi_present = TRUE;
			dsi_value = rsrv_ptr->data_source_identifier.value;
		}
		break;

	case e_object_rtgt:
                   /* Radar target report */

		rtgt_ptr = (t_Rtgt *) pntr;
		if (rtgt_ptr->data_source_identifier.present)
		{
			dsi_present = TRUE;
			dsi_value = rtgt_ptr->data_source_identifier.value;
		}
		break;

	case e_object_step:
                   /* System picture step */

		step_ptr = (t_Step *) pntr;
		if (step_ptr->data_source_identifier.present)
		{
			dsi_present = TRUE;
			dsi_value = step_ptr->data_source_identifier.value;
		}
		break;

	case e_object_strk:
                   /* System track message */

		strk_ptr = (t_Strk *) pntr;
		if (list_last_updating_sensor)
		{
			if (strk_ptr->measured_information.sid_present)
			{
				dsi_present = TRUE;
				dsi_value = strk_ptr->measured_information.value_sid;
			}
			else if (strk_ptr->last_updating_sensor.present)
			{
				dsi_present = TRUE;
				dsi_value = strk_ptr->last_updating_sensor.value;
			}
		}
		else
		{
			if (strk_ptr->server_sacsic.present)
			{
				dsi_present = TRUE;
				dsi_value = strk_ptr->server_sacsic.value;
			}
		}
		break;

	default:
		Assert (FALSE, "Unexpected object type");
		break;
	}

                   /* List data source identifier: */

	memset (tmp, 0, 81);

	if (dsi_present)
	{
		sprintf (tmp, "0x" M_FMT_UI16_HEX, dsi_value);
	}

	strncat (buffer, " ", length);
	strncat (buffer, tmp, length);

	done:          /* We are done */
	return;
}
#endif /* LISTER */
/* end-of-file */
