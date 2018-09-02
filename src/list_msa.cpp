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
/* File:           src/list_msa.cpp                                           */
/* Contents:       List some mode S address                                   */
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
/* list_msa        -- List SSR mode S address                                 */
/*----------------------------------------------------------------------------*/

 void list_msa
	(t_Object_Type object, t_Pntr pntr, t_Ui16 length, char *buffer)
{
	t_Adsb *adsb_ptr;
	t_Mlat *mlat_ptr;
	t_Bool msa_present;
	t_Ui32 msa_value;
	t_Rtgt *rtgt_ptr;
	t_Strk *strk_ptr;
	char tmp[81];

                   /* Check parameters: */
	Assert (e_no_object < object && object < e_object_sentinel,
            "Invalid parameter");
	Assert (pntr != NULL, "Invalid parameter");
	Assert (length > 0, "Invalid parameter");
	Assert (buffer != NULL, "Invalid parameter");

                   /* Check whether information should be listed: */
	if (!list_mode_s_address)
	{
		goto done;
	}
	if (list_no_mode_s)
	{
		goto done;
	}

                   /* Preset SSR mode S address: */
	msa_present = FALSE;
	msa_value = 0;

                   /* Extract SSR mode S address: */
	switch (object)
	{
	case e_object_adsb:
                   /* ADS-B report */

		adsb_ptr = (t_Adsb *) pntr;
		if (adsb_ptr->target_address.present)
		{
			msa_present = TRUE;
			msa_value = adsb_ptr->target_address.value;
		}
		break;

	case e_object_mlat:
                   /* Multilateration report */

		mlat_ptr = (t_Mlat *) pntr;
		if (mlat_ptr->target_address.present)
		{
			msa_present = TRUE;
			msa_value = mlat_ptr->target_address.value;
		}
		break;

	case e_object_rsrv:
                   /* Radar service message */

		/* holds no SSR mode S address */
		break;

	case e_object_rtgt:
                   /* Radar target report */

		rtgt_ptr = (t_Rtgt *) pntr;
		if (rtgt_ptr->target_address.present)
		{
			msa_present = TRUE;
			msa_value = rtgt_ptr->target_address.value;
		}
		break;

	case e_object_strk:
                   /* System track message */

		strk_ptr = (t_Strk *) pntr;
		if (strk_ptr->aircraft_address.present)
		{
			msa_present = TRUE;
			msa_value = strk_ptr->aircraft_address.value;
		}
		break;

	default:
		Assert (FALSE, "Unexpected object type");
		break;
	}

                   /* List SSR mode S address: */

	memset (tmp, 0, 81);

	if (msa_present)
	{
		sprintf (tmp, "S:" M_FMT_ADDR, msa_value);
	}
	else
	{
		strncat (tmp, "          ", 80);
	}

	strncat (buffer, " ", length);
	strncat (buffer, tmp, length);

	done:          /* We are done */
	return;
}
#endif /* LISTER */
/* end-of-file */
