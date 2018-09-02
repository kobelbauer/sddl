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
/* File:           src/list_mtp.cpp                                           */
/* Contents:       List some message or detection type                        */
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
/* list_mtp        -- List message or detection type                          */
/*----------------------------------------------------------------------------*/

 void list_mtp
	(t_Object_Type object, t_Pntr pntr, t_Ui16 length, char *buffer)
{
	t_Adsb *adsb_ptr;
	t_Ui16 cnt;
	t_Mlat *mlat_ptr;
	t_Bool mtp_present;
	char mtp_value[6];
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

                   /* Preset message type: */
	mtp_present = FALSE;
	memset (mtp_value, 0, 6);

                   /* Extract message type: */
	switch (object)
	{
	case e_object_adsb:
                   /* ADS-B report */

		adsb_ptr = (t_Adsb *) pntr;
		Assert (adsb_ptr->detection_type.reported_from_ads == e_is_true,
                "Unexpected detection type");
		mtp_present = TRUE;
		strncat (mtp_value, "P:ADS", 5);
		break;

	case e_object_mlat:
                   /* Multilateration report */

		mlat_ptr = (t_Mlat *) pntr;

		mtp_present = TRUE;
		mtp_value[0] = 'M';
		mtp_value[1] = ':';

		cnt = 0;
		if (mlat_ptr->detection_type.present)
		{
			Assert (mlat_ptr->detection_type.reported_from_mlt == e_is_true,
                    "Unexpected detection type");
		}
		if (mlat_ptr->detected_by_adsb == e_is_true)
		{
			++ cnt;
		}
		if (mlat_ptr->detected_by_dme == e_is_true)
		{
			++ cnt;
		}
		if (mlat_ptr->detected_by_hf == e_is_true)
		{
			++ cnt;
		}
		if (mlat_ptr->detected_by_mds == e_is_true)
		{
			++ cnt;
		}
		if (mlat_ptr->detected_by_ot == e_is_true)
		{
			++ cnt;
		}
		if (mlat_ptr->detected_by_ssr == e_is_true)
		{
			++ cnt;
		}
		if (mlat_ptr->detected_by_uat == e_is_true)
		{
			++ cnt;
		}
		if (mlat_ptr->detected_by_vdl == e_is_true)
		{
			++ cnt;
		}

		if (cnt == 2 &&
            mlat_ptr->detected_by_mds == e_is_true &&
            mlat_ptr->detected_by_ssr == e_is_true)
		{
			strncat (mtp_value, "MSS", 5);
		}
		else if (cnt > 1)
		{
			strncat (mtp_value, "RPT", 5);
		}
		else if (cnt == 1)
		{
			if (mlat_ptr->detected_by_adsb == e_is_true)
			{
				strncat (mtp_value, "ADS", 5);
			}
			else if (mlat_ptr->detected_by_dme == e_is_true)
			{
				strncat (mtp_value, "DME", 5);
			}
			else if (mlat_ptr->detected_by_hf == e_is_true)
			{
				strncat (mtp_value, "HF ", 5);
			}
			else if (mlat_ptr->detected_by_mds == e_is_true)
			{
				strncat (mtp_value, "MDS", 5);
			}
			else if (mlat_ptr->detected_by_ot == e_is_true)
			{
				strncat (mtp_value, "OT ", 5);
			}
			else if (mlat_ptr->detected_by_ssr == e_is_true)
			{
				strncat (mtp_value, "SSR", 5);
			}
			else if (mlat_ptr->detected_by_uat == e_is_true)
			{
				strncat (mtp_value, "UAT", 5);
			}
			else if (mlat_ptr->detected_by_vdl == e_is_true)
			{
				strncat (mtp_value, "VDL", 5);
			}
		}
		else if (mlat_ptr->detection_type.reported_from_psr == e_is_true)
		{
			strncat (mtp_value, "PSR", 5);
		}
		else
		{
			strncat (mtp_value, "---", 5);
		}
		break;

	case e_object_rsrv:
                   /* Radar service message */

		rsrv_ptr = (t_Rsrv *) pntr;

		mtp_present = TRUE;
		mtp_value[0] = 'S';
		mtp_value[1] = ':';

		if (rsrv_ptr->rsrv_type == e_rsrv_north_marker)
		{
			strncat (mtp_value, "NMK", 5);
		}
		else if (rsrv_ptr->rsrv_type == e_rsrv_south_marker)
		{
			strncat (mtp_value, "SMK", 5);
		}
		else if (rsrv_ptr->rsrv_type == e_rsrv_sector_crossing)
		{
			strncat (mtp_value, "SCT", 5);
		}
		else if (rsrv_ptr->rsrv_type == e_rsrv_supervisory)
		{
			strncat (mtp_value, "SVM", 5);
		}
		else if (rsrv_ptr->rsrv_type == e_rsrv_jamming_strobe)
		{
			strncat (mtp_value, "ECM", 5);
		}
		else
		{
			strncat (mtp_value, "???", 5);
		}
		break;

	case e_object_rtgt:
                   /* Radar target report */

		rtgt_ptr = (t_Rtgt *) pntr;

		mtp_present = TRUE;
		if (rtgt_ptr->detection_type.is_radar_track == e_is_true)
		{
			mtp_value[0] = 'L';
		}
		else
		{
			mtp_value[0] = 'P';
		}
		mtp_value[1] = ':';

		if (rtgt_ptr->detection_type.reported_from_mlt == e_is_true)
		{
			strncat (mtp_value, "MLT", 5);
		}
		else if (rtgt_ptr->detection_type.reported_from_psr == e_is_true)
		{
			if (rtgt_ptr->detection_type.reported_from_mds == e_is_true)
			{
				if (rtgt_ptr->detection_type.mode_s_all_call == e_is_true)
				{
					strncat (mtp_value, "MCA", 5);
				}
				else if (rtgt_ptr->detection_type.mode_s_roll_call == e_is_true)
				{
					strncat (mtp_value, "MCR", 5);
				}
				else
				{
					strncat (mtp_value, "MC-", 5);
				}
			}
			else if (rtgt_ptr->detection_type.reported_from_ssr == e_is_true)
			{
				strncat (mtp_value, "CMB", 5);
			}
			else
			{
				strncat (mtp_value, "PSR", 5);
			}
		}
		else if (rtgt_ptr->detection_type.reported_from_ssr == e_is_true)
		{
			if (rtgt_ptr->detection_type.reported_from_mds == e_is_true)
			{
				if (rtgt_ptr->detection_type.mode_s_all_call == e_is_true)
				{
					strncat (mtp_value, "MSA", 5);
				}
				else if (rtgt_ptr->detection_type.mode_s_roll_call == e_is_true)
				{
					strncat (mtp_value, "MSR", 5);
				}
				else
				{
					strncat (mtp_value, "MS-", 5);
				}
			}
			else
			{
				strncat (mtp_value, "SSR", 5);
			}
		}
		else if (rtgt_ptr->detection_type.is_radar_track == e_is_true)
		{
			strncat (mtp_value, "CST", 5);
		}
		else
		{
			strncat (mtp_value, "cst", 5);
		}
		break;

	case e_object_strk:
                   /* System track message */

		strk_ptr = (t_Strk *) pntr;

		mtp_present = TRUE;
		if (strk_ptr->from_reference_trajectory)
		{
			strncat (mtp_value, "T:REF", 5);
		}
		else
		{
			mtp_value[0] = 'T';
			mtp_value[1] = ':';
			mtp_value[2] = 'U';
			if (strk_ptr->observed_by_psr == e_is_true &&
                strk_ptr->observed_by_ssr == e_is_true)
			{
				mtp_value[3] = 'C';
			}
			else if (strk_ptr->observed_by_psr == e_is_true)
			{
				mtp_value[3] = 'P';
			}
			else if (strk_ptr->observed_by_ssr == e_is_true ||
                     strk_ptr->observed_by_mds == e_is_true)
			{
				mtp_value[3] = 'S';
			}
			else if (strk_ptr->observed_by_ads == e_is_true)
			{
				mtp_value[3] = 'A';
			}
			else
			{
				mtp_value[3] = 'X';
			}
			if (strk_ptr->multi_sensor_track == e_is_true)
			{
				mtp_value[4] = '+';
			}
			else if (strk_ptr->observed_by_psr == e_is_true ||
                     strk_ptr->observed_by_ssr == e_is_true ||
                     strk_ptr->observed_by_mds == e_is_true)
			{
				mtp_value[4] = 'R';
			}
			else
			{
				mtp_value[4] = 'X';
			}
		}
		break;

	default:
		Assert (FALSE, "Unexpected object type");
		break;
	}

                   /* List message type: */

	memset (tmp, 0, 81);

	if (mtp_present)
	{
		strncat (tmp, mtp_value, 80);
	}
	else
	{
		strncat (tmp, "-:???", 80);
	}

	strncat (buffer, " ", length);
	strncat (buffer, tmp, length);

	return;
}
#endif /* LISTER */
/* end-of-file */
