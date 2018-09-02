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
/* File:           src/list_pos.cpp                                           */
/* Contents:       List some position                                         */
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

                   /* Local functions: */
                   /* ---------------- */

#if LISTER
static void list_lat (t_Real latitude, t_Ui16 length, char *buffer);
                   /* List geodetical latitude */
static void list_lon (t_Real longitude, t_Ui16 length, char *buffer);
                   /* List geodetical longitude */
#endif /* LISTER */

#if LISTER
/*----------------------------------------------------------------------------*/
/* list_lat        -- List geodetical latitude                                */
/*----------------------------------------------------------------------------*/

 static
 void list_lat (t_Real latitude, t_Ui16 length, char *buffer)
{
	int deg;
	t_Real f;
	int min;
	int msec;
	int sec;
	int ten_msecs;
	char tmp[81];

	if (latitude >= 0.0)
	{
		f = +latitude;
	}
	else
	{
		f = -latitude;
	}

	f = M_RAD2DEG * f;

	deg = (int) f;
	f = 60.0 * (f - (t_Real) deg);
	min = (int) f;
	f = 60.0 * (f - (t_Real) min);
	sec = (int) f;
	f = 1000.0 * (f - (t_Real) sec) + 0.5;
	msec = (int) f;

	if (msec >= 1000)
	{
		msec = 0;
		++ sec;
		if (sec >= 60)
		{
			sec = 0;
			++ min;

			if (min >= 60)
			{
				min = 0;
				++ deg;
			}
		}
	}

	ten_msecs = (int) (0.1 * msec);

	memset (tmp, 0, 81);
	sprintf (tmp, "%02d:%02d:%02d.%02d", deg, min, sec, ten_msecs);
	strncat (buffer, tmp, length);

	if (latitude >= 0.0)
	{
		strncat (buffer, "N", length);
	}
	else
	{
		strncat (buffer, "S", length);
	}

	return;
}
#endif /* LISTER */

#if LISTER
/*----------------------------------------------------------------------------*/
/* list_lon        -- List geodetical longitude                               */
/*----------------------------------------------------------------------------*/

 static
 void list_lon (t_Real longitude, t_Ui16 length, char *buffer)
{
	int deg;
	t_Real f;
	int min;
	int msec;
	int sec;
	int ten_msecs;
	char tmp[81];

	if (longitude >= 0.0)
	{
		f = +longitude;
	}
	else
	{
		f = -longitude;
	}

	f = M_RAD2DEG * f;

	deg = (int) f;
	deg = (int) f;
	f = 60.0 * (f - (t_Real) deg);
	min = (int) f;
	f = 60.0 * (f - (t_Real) min);
	sec = (int) f;
	f = 1000.0 * (f - (t_Real) sec) + 0.5;
	msec = (int) f;

	if (msec >= 1000)
	{
		msec = 0;
		++ sec;
		if (sec >= 60)
		{
			sec = 0;
			++ min;

			if (min >= 60)
			{
				min = 0;
				++ deg;
			}
		}
	}

	ten_msecs = (int) (0.1 * msec);

	memset (tmp, 0, 81);
	sprintf (tmp, "%03d:%02d:%02d.%02d", deg, min, sec, ten_msecs);
	strncat (buffer, tmp, length);

	if (longitude >= 0.0)
	{
		strncat (buffer, "E", length);
	}
	else
	{
		strncat (buffer, "W", length);
	}

	return;
}
#endif /* LISTER */

#if LISTER
/*----------------------------------------------------------------------------*/
/* list_pos        -- List position                                           */
/*----------------------------------------------------------------------------*/

 void list_pos
	(t_Object_Type object, t_Pntr pntr, t_Ui16 length, char *buffer)
{
	t_Adsb *adsb_ptr;
	t_Mlat *mlat_ptr;
	t_Rsrv *rsrv_ptr;
	t_Rtgt *rtgt_ptr;
	t_Strk *strk_ptr;
	char tmp[81];
	t_Real x;
	t_Real y;

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

		adsb_ptr = (t_Adsb *) pntr;

                   /* Coordinates indication: */
		strncat (buffer, " ", length);
		if (adsb_ptr->detection_type.reported_from_ads &&
            adsb_ptr->wgs84_position.present)
		{
			strncat (buffer, "LL", length);
		}
		else
		{
			strncat (buffer, "--", length);
		}
		strncat (buffer, " ", length);

                   /* Geodetical position: */
		if (adsb_ptr->detection_type.reported_from_ads &&
            adsb_ptr->wgs84_position.present)
		{
			strncat (buffer, " ", length);

			list_lat (adsb_ptr->wgs84_position.value_lat, length, buffer);

			strncat (buffer, " ", length);
			strncat (buffer, " ", length);

			list_lon (adsb_ptr->wgs84_position.value_lon, length, buffer);
		}
		else
		{
			strncat (buffer, " ", length);
			strncat (buffer, "--:--:--.-- ", length);

			strncat (buffer, " ", length);
			strncat (buffer, " ", length);
			strncat (buffer, "---:--:--.-- ", length);
		}
		break;

	case e_object_mlat:
                   /* Multilateration report */

		mlat_ptr = (t_Mlat *) pntr;

                   /* Coordinates indication: */
		strncat (buffer, " ", length);
		if (list_wgs84 && mlat_ptr->wgs84_position.present)
		{
			strncat (buffer, "LL", length);
		}
		else if (mlat_ptr->computed_position.present)
		{
			strncat (buffer, "XY", length);
		}
		else if (mlat_ptr->wgs84_position.present)
		{
			strncat (buffer, "LL", length);
		}
		else
		{
			strncat (buffer, "--", length);
		}
		strncat (buffer, " ", length);

		if (list_wgs84 && mlat_ptr->wgs84_position.present)
		{
			strncat (buffer, " ", length);

			list_lat (mlat_ptr->wgs84_position.value_lat, length, buffer);

			strncat (buffer, " ", length);
			strncat (buffer, " ", length);

			list_lon (mlat_ptr->wgs84_position.value_lon, length, buffer);
		}
		else if (mlat_ptr->computed_position.present)
		{
			memset (tmp, 0, 81);
			sprintf (tmp, "%8.3f",
                          M_MTR2NMI * mlat_ptr->computed_position.value_x);

			strncat (buffer, tmp, length);

			memset (tmp, 0, 81);
			sprintf (tmp, "%8.3f",
                          M_MTR2NMI * mlat_ptr->computed_position.value_y);

			strncat (buffer, " ", length);
			strncat (buffer, tmp, length);
		}
		else if (mlat_ptr->wgs84_position.present)
		{
			strncat (buffer, " ", length);

			list_lat (mlat_ptr->wgs84_position.value_lat, length, buffer);

			strncat (buffer, " ", length);
			strncat (buffer, " ", length);

			list_lon (mlat_ptr->wgs84_position.value_lon, length, buffer);
		}
		else
		{
			strncat (buffer, "----.---", length);

			strncat (buffer, " ", length);
			strncat (buffer, "----.---", length);
		}
		break;

	case e_object_rsrv:
                   /* Radar service message */

		rsrv_ptr = (t_Rsrv *) pntr;

                   /* Coordinates indication: */
		strncat (buffer, " ", length);
		if (rsrv_ptr->antenna_azimuth.present)
		{
			strncat (buffer, "A", length);
		}
		else
		{
			strncat (buffer, "-", length);
		}
		if (rsrv_ptr->slant_range.present)
		{
			strncat (buffer, "R", length);
		}
		else
		{
			strncat (buffer, "-", length);
		}
		strncat (buffer, " ", length);

                   /* Antenna azimuth (in degrees): */
		memset (tmp, 0, 81);
		if (rsrv_ptr->antenna_azimuth.present)
		{
			sprintf (tmp, M_FMT_AZIM_DEG,
                          M_RAD2DEG * rsrv_ptr->antenna_azimuth.value);
		}
		else
		{
			strncat (tmp, "---.---", 80);
		}

		strncat (buffer, " ", length);
		strncat (buffer, tmp, length);

                   /* Slant range (in nautical miles): */
		memset (tmp, 0, 81);
		if (rsrv_ptr->slant_range.present)
		{
			sprintf (tmp, M_FMT_RNGE_NMI,
                          M_MTR2NMI * rsrv_ptr->slant_range.value);
		}
		else
		{
			strncat (tmp, "       ", 80);
		}

		strncat (buffer, " ", length);
		strncat (buffer, " ", length);
		strncat (buffer, tmp, length);

		break;

	case e_object_rtgt:
                   /* Radar target report */

		rtgt_ptr = (t_Rtgt *) pntr;

                   /* Coordinates indication: */
		strncat (buffer, " ", length);
		if (rtgt_ptr->measured_azm.present &&
            rtgt_ptr->measured_rng.present)
		{
			strncat (buffer, "AR", length);
		}
		else if (rtgt_ptr->detection_type.is_radar_track &&
                 rtgt_ptr->to_be_cancelled)
		{
			strncat (buffer, "  ", length);
		}
		else if (rtgt_ptr->computed_position.present)
		{
			strncat (buffer, "XY", length);
		}
		else if (rtgt_ptr->measured_azm.present)
		{
			strncat (buffer, "A-", length);
		}
		else if (rtgt_ptr->measured_rng.present)
		{
			strncat (buffer, "-R", length);
		}
		else
		{
			strncat (buffer, "--", length);
		}

                   /* Azimuth (in degrees) and
                      slant range (in nautical miles): */
		if (rtgt_ptr->measured_azm.present &&
            rtgt_ptr->measured_rng.present)
		{
			memset (tmp, 0, 81);
			sprintf (tmp, M_FMT_AZIM_DEG,
                          M_RAD2DEG * rtgt_ptr->measured_azm.value);

			strncat (buffer, " ", length);
			strncat (buffer, " ", length);
			strncat (buffer, tmp, length);

			memset (tmp, 0, 81);
			sprintf (tmp, M_FMT_RNGE_NMI,
                          M_MTR2NMI * rtgt_ptr->measured_rng.value);

			strncat (buffer, " ", length);
			strncat (buffer, " ", length);
			strncat (buffer, tmp, length);
		}
		else if (rtgt_ptr->detection_type.is_radar_track &&
                 rtgt_ptr->to_be_cancelled)
		{
			strncat (buffer, " ", length);
			strncat (buffer, " ", length);
			strncat (buffer, "       ", length);

			strncat (buffer, " ", length);
			strncat (buffer, " ", length);
			strncat (buffer, "       ", length);
		}
		else if (rtgt_ptr->computed_position.present)
		{
			memset (tmp, 0, 81);
			sprintf (tmp, "%8.3f",
                          M_MTR2NMI * rtgt_ptr->computed_position.value_x);

			strncat (buffer, " ", length);
			strncat (buffer, tmp, length);

			memset (tmp, 0, 81);
			sprintf (tmp, "%8.3f", 
                          M_MTR2NMI * rtgt_ptr->computed_position.value_y);

			strncat (buffer, " ", length);
			strncat (buffer, tmp, length);
		}
		else if (rtgt_ptr->measured_azm.present)
		{
			memset (tmp, 0, 81);
			sprintf (tmp, M_FMT_AZIM_DEG,
                          M_RAD2DEG * rtgt_ptr->measured_azm.value);

			strncat (buffer, " ", length);
			strncat (buffer, " ", length);
			strncat (buffer, tmp, length);

			strncat (buffer, " ", length);
			strncat (buffer, " ", length);
			strncat (buffer, "---.---", length);
		}
		else if (rtgt_ptr->measured_rng.present)
		{
			strncat (buffer, " ", length);
			strncat (buffer, " ", length);
			strncat (buffer, "---.---", length);

			memset (tmp, 0, 81);
			sprintf (tmp, M_FMT_RNGE_NMI,
                          M_MTR2NMI * rtgt_ptr->measured_rng.value);

			strncat (buffer, " ", length);
			strncat (buffer, " ", length);
			strncat (buffer, tmp, length);
		}
		else
		{
			strncat (buffer, " ", length);
			strncat (buffer, " ", length);
			strncat (buffer, "---.---", length);

			strncat (buffer, " ", length);
			strncat (buffer, " ", length);
			strncat (buffer, "---.---", length);
		}

		break;

	case e_object_strk:
                   /* System track message */

		strk_ptr = (t_Strk *) pntr;

                   /* Coordinates indication: */
		strncat (buffer, " ", length);
		if (list_wgs84 && strk_ptr->calculated_wgs84_position.present)
		{
			strncat (buffer, "LL", length);
		}
		else if (strk_ptr->calculated_position.present)
		{
			strncat (buffer, "UV", length);
		}
		else if (strk_ptr->calculated_wgs84_position.present)
		{
			strncat (buffer, "LL", length);
		}
		else
		{
			strncat (buffer, "--", length);
		}
		strncat (buffer, " ", length);

		if (list_wgs84 && strk_ptr->calculated_wgs84_position.present)
		{
			strncat (buffer, " ", length);

			list_lat (strk_ptr->calculated_wgs84_position.value_latitude,
                      length, buffer);

			strncat (buffer, " ", length);

			list_lon (strk_ptr->calculated_wgs84_position.value_longitude,
                      length, buffer);
		}
		else if (strk_ptr->calculated_position.present)
		{
			x = M_MTR2NMI * strk_ptr->calculated_position.value_x;
			y = M_MTR2NMI * strk_ptr->calculated_position.value_y;

			if (strk_ptr->track_position_coding_precision)
			{
				x *= 2.0;
				y *= 2.0;
			}

			memset (tmp, 0, 81);
			sprintf (tmp, "%8.3f %8.3f", x, y);

			strncat (buffer, tmp, length);
		}
		else if (strk_ptr->calculated_wgs84_position.present)
		{
			list_lat (strk_ptr->calculated_wgs84_position.value_latitude,
                      length, buffer);

			strncat (buffer, " ", length);

			list_lon (strk_ptr->calculated_wgs84_position.value_longitude,
                      length, buffer);
		}
		else if (list_wgs84)
		{
			memset (tmp, 0, 81);

			strncat (tmp, "                         ", 80);

			strncat (buffer, tmp, length);
		}
		else
		{
			memset (tmp, 0, 81);

			strncat (tmp, "                 ", 80);

			strncat (buffer, tmp, length);
		}
		break;

	default:
		Assert (FALSE, "Unexpected object type");
		break;
	}

	return;
}
#endif /* LISTER */
/* end-of-file */
