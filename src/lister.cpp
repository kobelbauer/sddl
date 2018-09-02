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
/* File:           src/lister.cpp                                             */
/* Contents:       List surveillance input messages                           */
/* Author(s):      kb                                                         */
/* Last change:    2018-04-27                                                 */
/*----------------------------------------------------------------------------*/

#include "basics.h"
                   /* Basic system definitions */
#include "common.h"
                   /* Common declarations and definitions */
#include "progid.h"
                   /* Program identity and version definitions */

#include <math.h>
#include <stdarg.h>
#include <stddef.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/time.h>
#include <time.h>

                   /* Local data: */
                   /* ----------- */

static t_Bool initiated = FALSE;
                   /* Package initiated flag */

                   /* Local functions: */
                   /* ---------------- */

#if LISTER
static void excelize (char *str);
                   /* Adapt decimal point to Excel conventions */
#endif /* LISTER */
static void initiate (void);
                   /* Initiate package */

/*----------------------------------------------------------------------------*/
/* bad_frame       -- Dump bad frame                                          */
/*----------------------------------------------------------------------------*/

 void bad_frame
	(const char *text_ptr, t_Ui32 offset, t_Ui16 length, t_Byte *buffer)
{
	int i, j;

				   /* Check parameters: */
	Assert (text_ptr != NULL, "Invalid parameter");
	Assert (length > 0, "Invalid parameter");
	Assert (buffer != NULL, "Invalid parameter");

	fprintf (stderr, "-> ");
	if (*text_ptr)
	{
		fprintf (stderr, "%s", text_ptr);
	}
	else
	{
		fprintf (stderr, "Bad frame at");
	}
	fprintf (stderr, " offset " M_FMT_UI32 " (length=" M_FMT_UI16 "):\n",
                     offset, length);

	j = 0;
	for (i = 0; i < length; i ++)
	{
		if (j == 0)
		{
			fprintf (stderr, "   0x");
		}
		fprintf (stderr, " %02x", buffer[i]);

		++ j;
		if (j >= 16)
		{
			fprintf (stderr, "\n");

			j = 0;
		}
		else if ((j % 4) == 0)
		{
			fprintf (stderr, " ");
		}
	}
	if (j > 0)
	{
		fprintf (stderr, "\n");
	}

	return;
}

#if LISTER
/*----------------------------------------------------------------------------*/
/* excelize        -- Adapt decimal point to Excel conventions                */
/*----------------------------------------------------------------------------*/

 static
 void excelize (char *str)
{
	size_t len;
	size_t pos;

	if (str == NULL)
	{
		goto done;
	}

	len = strlen (str);
	if (len == 0)
	{
		goto done;
	}

	for (pos = 0; pos < len; pos ++)
	{
		if (str[pos] == '.')
		{
			str[pos] = ',';
		}
	}

	done:          /* We are done */
	return;
}
#endif /* LISTER */

/*----------------------------------------------------------------------------*/
/* initiate        -- Initiate package                                        */
/*----------------------------------------------------------------------------*/

 static
 void initiate (void)
{
	if (excel_output)
	{
		exc_file = fopen ("excel.out", "w");
		if (exc_file == NULL)
		{
			fprintf (stderr, "W> Cannot open output file for"
                             " Excel output\n");
		}
	}

	return;
}

/*----------------------------------------------------------------------------*/
/* lc_init         -- Initiate load counter                                   */
/*----------------------------------------------------------------------------*/

 void lc_init (t_Load_Counter *lc_ptr, t_Real period)
{
                   /* Check parameters: */
	Assert (lc_ptr != NULL, "Invalid parameter");
	Assert (period > 0.0, "Invalid parameter");

	lc_ptr->current_count = 0;
	lc_ptr->current_start_time = 0.0;
	lc_ptr->in_first_period = FALSE;
	lc_ptr->next_start_time = 0.0;
	lc_ptr->observation_period = period;
	lc_ptr->started = FALSE;

	return;
}

/*----------------------------------------------------------------------------*/
/* lc_updt          -- Update load counter                                    */
/*----------------------------------------------------------------------------*/

 void lc_updt (t_Load_Counter *lc_ptr, t_Secs current_tm, t_Ui16 count,
               t_Statistical_Item *stat_ptr)
{
                   /* Check parameters: */
	Assert (lc_ptr != NULL, "Invalid parameter");
	Assert (0.0 <= current_tm && current_tm < M_SECONDS_PER_DAY,
            "Invalid parameter");
	Assert (count > 0, "Invalid parameter");
	Assert (stat_ptr != NULL, "Invalid parameter");

                   /* If load counter is not yet started,
                      prepare it: */
	if (!lc_ptr->started)
	{
		t_Time ctm;
                   /* Current time; milliseconds */
		t_Time ptm;
                   /* Observation period; milliseconds */
		t_Time stm;
                   /* Start time; milliseconds */

		Assert (lc_ptr->observation_period > 0.0,
                "Invalid observation period");

		ptm = (t_Time) (1000.0 * lc_ptr->observation_period + 0.5);

		ctm = (t_Time) (1000.0 * current_tm + 0.5);

		stm = ctm - (ctm % ptm);

		lc_ptr->current_start_time = 0.001 * stm;
		lc_ptr->started = TRUE;

		lc_ptr->next_start_time =
			lc_ptr->current_start_time + lc_ptr->observation_period;

		if (ctm % ptm)
		{
			lc_ptr->in_first_period = TRUE;
		}
	}

                   /* Have we reached the end of the current
                      observation period ? */
	if (current_tm >= lc_ptr->next_start_time)
	{
		if (!lc_ptr->in_first_period)
		{
			stat_updt (stat_ptr, lc_ptr->current_count);
		}

		lc_ptr->current_count = 0;
		lc_ptr->in_first_period = FALSE;

		lc_ptr->current_start_time = lc_ptr->next_start_time;

		lc_ptr->next_start_time =
			lc_ptr->current_start_time + lc_ptr->observation_period;
		if (lc_ptr->next_start_time >= M_SECONDS_PER_DAY)
		{
			lc_ptr->next_start_time -= M_SECONDS_PER_DAY;
		}
	}

                   /* Increment current count: */
	lc_ptr->current_count += count;

	return;
}

#if LISTER
/*----------------------------------------------------------------------------*/
/* list_adsb       -- List an ADS-B report                                    */
/*----------------------------------------------------------------------------*/

 void list_adsb (t_Adsb *adsb_ptr)
{
	int ix;
	t_Bool new_sensor;
	t_Secs tm;
	t_Bool to_be_listed;
	char txt[257];

				   /* Check parameters: */
	Assert (adsb_ptr != NULL, "NULL pointer");

                   /* Check if the list file is available: */
	Assert (list_file != NULL, "No list file");

                   /* Check if package has been initiated: */
	if (!initiated)
	{
		initiate ();

		initiated = TRUE;
	}

                   /* Check against start offset: */
	if (start_offset_defined && input_offset < start_offset)
	{
		goto done;
	}

                   /* Extract relevant time: */
	tm = 0.0;
	if (adsb_ptr->detection_time.present)
	{
		tm = adsb_ptr->detection_time.value;
	}
	else if (adsb_ptr->frame_time.present)
	{
		tm = adsb_ptr->frame_time.value;
	}

                   /* Adapt by time bias, if any: */
	if (time_bias_defined)
	{
		tm += time_bias;
	}

                   /* Check for sensor number: */
	new_sensor = FALSE;
	if (!adsb_ptr->sensor_number.present)
	{
		if (adsb_ptr->data_source_identifier.present)
		{
			t_Ui16 dsi;
			t_Bool found;

			found = FALSE;
			dsi = adsb_ptr->data_source_identifier.value;
			for (ix = 0; ix < M_MAX_NUMBER_OF_SENSORS; ix ++)
			{
				if (sacsic_list[ix].orig_defined &&
                    sacsic_list[ix].orig == dsi)
				{
					found = TRUE;
					break;
				}
			}

			if (!found)
			{
				for (ix = 0; ix < M_MAX_NUMBER_OF_SENSORS; ix ++)
				{
					if (sacsic_list[ix].rsap_defined &&
                        sacsic_list[ix].rsap == dsi)
					{
						found = TRUE;
						break;
					}
				}
			}

			if (found)
			{
				adsb_ptr->sensor_number.present = TRUE;
				adsb_ptr->sensor_number.value = 1 + ix;
			}
		}
	}

	if (new_sensor &&
        adsb_ptr->data_source_identifier.present &&
        adsb_ptr->sensor_number.present)
	{
		list_text (3, "; new sensor %hu found: sacsic=0x%04x (%u/%u)",
                      adsb_ptr->sensor_number.value,
                      adsb_ptr->data_source_identifier.value,
                      hi_byte (adsb_ptr->data_source_identifier.value),
                      lo_byte (adsb_ptr->data_source_identifier.value));
		list_text (3, "\n");
	}

                   /* Decide whether this ADS-B report is to be listed: */

	to_be_listed = FALSE;
	if (1 <= list_level && list_level <= 3)
	{
		to_be_listed = TRUE;
	}
	if (adsb_ptr->data_format == e_data_format_asterix)
	{
		t_Byte cat;

		cat = adsb_ptr->asterix_category;

		if (cat_selection_defined)
		{
			if (!cat_selection[cat])
			{
				to_be_listed = FALSE;
			}
		}
	}

	adsb_ptr->has_been_listed = FALSE;
	adsb_ptr->to_be_listed = to_be_listed;

                   /* List ADS-B report information: */
	if (to_be_listed)
	{
		memset (txt, 0, 257);

		list_adsb (adsb_ptr, 256, txt);

		list_text (3, "%s", txt);
		list_text (3, "\n");
	}

	done:          /* We are done */
	return;
}
#endif /* LISTER */

#if LISTER
/*----------------------------------------------------------------------------*/
/* list_adsb       -- List ADS-B report information                           */
/*----------------------------------------------------------------------------*/

 void list_adsb (t_Adsb *adsb_ptr, t_Ui16 length, char *buffer)
{
	t_Bool baro_alt_listed;
	t_Real fd_dt;
	t_Bool fd_dt_problem;
	t_Bool geom_alt_listed;
	char tmp[81];

				   /* Check parameters: */
	Assert (adsb_ptr != NULL, "Invalid parameter");
	Assert (length > 0, "Invalid parameter");
	Assert (buffer != NULL, "NULL pointer");

                   /* Check if package has been initiated: */
	if (!initiated)
	{
		initiate ();

		initiated = TRUE;
	}

                   /* Clear write buffer: */
	memset (buffer, 0, length);

                   /* List ADS-B report information: */

	fd_dt = 0.0;
	fd_dt_problem = FALSE;
	if (checking_fd &&
        adsb_ptr->detection_time.present &&
        adsb_ptr->frame_time.present)
	{
		fd_dt = time_diff (adsb_ptr->frame_time.value,
                           adsb_ptr->detection_time.value);
		if (fd_dt < -0.010 || fd_dt > 1.500)
		{
			fd_dt_problem = TRUE;
		}
	}

                   /* List frame time: */
	list_ftm (e_object_adsb, adsb_ptr, length, buffer);

                   /* Detection, estimated or frame time: */
	list_tod (e_object_adsb, adsb_ptr, length, buffer);

                   /* List sensor number: */
	list_sno (e_object_adsb, adsb_ptr, length, buffer);

                   /* List board/line number: */
	if (handle_multiple_lines)
	{
		if (adsb_ptr->line_number > 0)
		{
			memset (tmp, 0, 81);
			sprintf (tmp, " " M_FMT_LINE, adsb_ptr->line_number);
			strncat (buffer, tmp, length);
		}
		else
		{
			strncat (buffer, " ----", length);
		}
	}

                   /* List sensor data source identification: */
	list_dsi (e_object_adsb, adsb_ptr, length, buffer);

                   /* List antenna number: */
	list_ano (e_object_adsb, adsb_ptr, length, buffer);

                   /* List detection type: */
	list_mtp (e_object_adsb, adsb_ptr, length, buffer);

                   /* List generic number: */
	list_num (e_object_adsb, adsb_ptr, length, buffer);

                   /* List position: */
	list_pos (e_object_adsb, adsb_ptr, length, buffer);

                   /* List SSR mode S address: */
	list_msa (e_object_adsb, adsb_ptr, length, buffer);

                   /* List aircraft identification: */
	list_aid (e_object_adsb, adsb_ptr, length, buffer);

                   /* List SSR identification: */
	list_idt (e_object_adsb, adsb_ptr, length, buffer);

                   /* Flight level: */
	baro_alt_listed = FALSE;
	geom_alt_listed = FALSE;
	if (adsb_ptr->detection_type.reported_from_ads &&
        adsb_ptr->mode_c_height.present)
	{
		t_Si16 fl;

		strncat (buffer, " ", length);
		strncat (buffer, "C:", length);

		fl = adsb_ptr->mode_c_height.value_in_feet / 100;

		if (fl < -10 || fl > 1267)
		{
			strncat (buffer, " NVA", length);
			strncat (buffer, "   ", length);

			strncat (buffer, " ", length);
			strncat (buffer, "--", length);
		}
		else
		{
			memset (tmp, 0, 81);
			if (adsb_ptr->mode_c_height.in_25_feet)
			{
				sprintf (tmp, "%7.2f", 
                              0.01 * adsb_ptr->mode_c_height.value_in_feet);
			}
			else
			{
				sprintf (tmp, "%4hd   ", fl);
			}

			strncat (buffer, tmp, length);

			strncat (buffer, " ", length);
			strncat (buffer, "-", length);
			strncat (buffer, "-", length);
		}

		baro_alt_listed = TRUE;
	}
	else if (list_geometric_height &&
             adsb_ptr->detection_type.reported_from_ads &&
             adsb_ptr->geometric_altitude.present)
	{
		strncat (buffer, " ", length);
		strncat (buffer, "G:", length);

		memset (tmp, 0, 81);
		sprintf (tmp, "%7.2f", M_MTR2FL * adsb_ptr->geometric_altitude.value);
		strncat (buffer, tmp, length);

		strncat (buffer, "   ", length);

		geom_alt_listed = TRUE;
	}
	else
	{
		strncat (buffer, " ", length);
		strncat (buffer, "-:", length);
		strncat (buffer, "----", length);
		strncat (buffer, "   ", length);

		strncat (buffer, " ", length);
		strncat (buffer, "--", length);
	}

	strncat (buffer, " ;", length);

	if (adsb_ptr->detection_type.reported_from_ads &&
        adsb_ptr->mode_c_height.present &&
        !baro_alt_listed)
	{
		t_Si16 fl;

		strncat (buffer, " ", length);
		strncat (buffer, "C:", length);

		fl = adsb_ptr->mode_c_height.value_in_feet / 100;

		if (fl < -10 || fl > 1267)
		{
			strncat (buffer, "NVA", length);
		}
		else
		{
			memset (tmp, 0, 81);
			if (adsb_ptr->mode_c_height.in_25_feet)
			{
				sprintf (tmp, "%.2f", 
                              0.01 * adsb_ptr->mode_c_height.value_in_feet);
			}
			else
			{
				sprintf (tmp, "%hd", fl);
			}

			strncat (buffer, tmp, length);
		}
	}

	if (!geom_alt_listed &&
        list_geometric_height &&
        adsb_ptr->detection_type.reported_from_ads &&
        adsb_ptr->geometric_altitude.present)
	{
		strncat (buffer, " ", length);
		strncat (buffer, "G:", length);

		memset (tmp, 0, 81);
		sprintf (tmp, "%7.2f", M_MTR2FL * adsb_ptr->geometric_altitude.value);
		strncat (buffer, tmp, length);
	}

	if (adsb_ptr->acas_resolution_advisory_report.present)
	{
		strncat (buffer, " acas", length);
	}

	if (adsb_ptr->barometric_vertical_rate.present)
	{
		memset (tmp, 0, 81);
		sprintf (tmp, " bvr=%.1f",
                      adsb_ptr->barometric_vertical_rate.value);
		strncat (buffer, tmp, length);
	}

	if (adsb_ptr->mops_version.present &&
        adsb_ptr->mops_version.value_vns == 0)
	{
		if (adsb_ptr->mops_version.value_vn == 0)
		{
			strncat (buffer, " DO-260", length);
		}
		else if (adsb_ptr->mops_version.value_vn == 1)
		{
			strncat (buffer, " DO-260A", length);
		}
		else if (adsb_ptr->mops_version.value_vn == 2)
		{
			strncat (buffer, " DO-260B", length);
		}
	}

	if (adsb_ptr->detection_type.from_fixed_field_transponder == e_is_true)
	{
		strncat (buffer, " fft", length);
	}

	if (adsb_ptr->figure_of_merit.present)
	{
		memset (tmp, 0, 81);
		sprintf (tmp, " fom=%u", adsb_ptr->figure_of_merit.value);
		strncat (buffer, tmp, length);
	}

	if (fd_dt_problem)
	{
		memset (tmp, 0, 81);
		sprintf (tmp, " fd_dt=%.3f secs", fd_dt);
		strncat (buffer, tmp, length);
	}

	if (adsb_ptr->ground_bit_set)
	{
		strncat (buffer, " gnd", length);
	}

	if (adsb_ptr->quality_indicators.present)
	{
		memset (tmp, 0, 81);
		if (adsb_ptr->mops_version.present &&
            adsb_ptr->mops_version.value_vns == 0)
		{
			t_Byte nucp;

			nucp = adsb_ptr->quality_indicators.value_nucp_or_nic;

			if (adsb_ptr->mops_version.value_vn == 0)
			{
				sprintf (tmp, " nucp=%u", nucp);
			}
			else
			{
				sprintf (tmp, " nic=%u", nucp);
			}
			strncat (buffer, tmp, length);
		}
	}

	if (adsb_ptr->detection_type.simulated == e_is_true)
	{
		strncat (buffer, " sim", length);
	}

	if (adsb_ptr->special_position_indication == e_is_true)
	{
		strncat (buffer, " spi", length);
	}

	if (adsb_ptr->detection_type.test_target == e_is_true)
	{
		strncat (buffer, " tst", length);
	}

	return;
}
#endif /* LISTER */

/*----------------------------------------------------------------------------*/
/* list_bds_register -- List BDS register contents                            */
/*----------------------------------------------------------------------------*/

 void list_bds_register
	(t_Byte bds, t_Byte ds1, t_Byte ds2, t_Byte ds3,
     t_Byte ds4, t_Byte ds5, t_Byte ds6, t_Byte ds7)
{
	t_Bool to_be_listed;
                   /* Information to be listed */

                   /* Check if package has been initiated: */
	if (!initiated)
	{
		initiate ();

		initiated = TRUE;
	}

                   /* Decide whether list-out is wanted at all: */

	to_be_listed = FALSE;
	if (1 <= list_level && list_level <= 3)
	{
		to_be_listed = TRUE;
	}

	if (!to_be_listed)
	{
		goto done;
	}

	done:          /* We are done */
	return;
}

#if LISTER
/*----------------------------------------------------------------------------*/
/* list_buffer     -- List buffer contents                                    */
/*----------------------------------------------------------------------------*/

 void list_buffer (int level, const char *header, t_Ui16 length, t_Byte *buffer)
{
	t_Byte b;
	int i;
	int out;

				   /* Check parameters: */
	Assert (header != NULL, "Invalid parameter");
	Assert (length > 0, "Invalid parameter");
	Assert (buffer != NULL, "Invalid parameter");

				   /* Check if list file available: */
	Assert (list_file != NULL, "No list file");

                   /* Check if package has been initiated: */
	if (!initiated)
	{
		initiate ();

		initiated = TRUE;
	}

	out = 0;
	for (i = 0; i < length; i ++)
	{
		if (out == 0)
		{
			list_text (level, header);
			list_text (level, " 0x");
		}

		b = *(buffer + i);

		list_text (level, " " M_FMT_BYTE_HEX, b);

		++ out;
		if (out >= 16)
		{
			list_text (level, "\n");
			out = 0;
		}
		else if ((out % 4) == 0)
		{
			list_text (level, " ");
		}
	}

	if (out > 0)
	{
		list_text (level, "\n");
	}

	return;
}
#endif /* LISTER */

#if LISTER
/*----------------------------------------------------------------------------*/
/* list_frame      -- List input frame                                        */
/*----------------------------------------------------------------------------*/

 void list_frame (int level, int length, t_Byte *buffer)
{
	t_Byte b;
	int ix;
	int out;

				   /* Check parameters: */
	Assert (length > 0, "Invalid frame length");
	Assert (buffer != NULL, "NULL pointer");

				   /* Check if list file available: */
	Assert (list_file != NULL, "No list file");

                   /* Check if package has been initiated: */
	if (!initiated)
	{
		initiate ();

		initiated = TRUE;
	}

	out = 0;
	for (ix = 0; ix < length; ix ++)
	{
		if (out == 0)
		{
			list_text (level, "  0x");
		}

		b = *(buffer + ix);

		list_text (level, " %02x", b);

		++ out;
		if (out >= 16)
		{
			list_text (level, "\n");
			out = 0;
		}
		else if ((out % 4) == 0)
		{
			list_text (level, " ");
		}
	}

	if (out > 0)
	{
		list_text (level, "\n");
	}

	return;
}
#endif /* LISTER */

#if LISTER
/*----------------------------------------------------------------------------*/
/* list_mlat       -- List multilateration report                             */
/*----------------------------------------------------------------------------*/

 void list_mlat (t_Mlat *mlat_ptr)
{
	int ix;
	t_Bool new_sensor;
	t_Secs tm;
	t_Bool to_be_listed;
	char txt[257];

				   /* Check parameters: */
	Assert (mlat_ptr != NULL, "Invalid parameter");

                   /* Check if the list file is available: */
	Assert (list_file != NULL, "No list file");

                   /* Check if package has been initiated: */
	if (!initiated)
	{
		initiate ();

		initiated = TRUE;
	}

                   /* Check against start offset: */
	if (start_offset_defined && input_offset < start_offset)
	{
		goto done;
	}

                   /* Extract relevant time: */
	tm = 0.0;
	if (mlat_ptr->detection_time.present)
	{
		tm = mlat_ptr->detection_time.value;
	}
	else if (mlat_ptr->frame_time.present)
	{
		tm = mlat_ptr->frame_time.value;
	}

                   /* Adapt by time bias, if any: */
	if (time_bias_defined)
	{
		tm += time_bias;
	}

                   /* Check for sensor number: */
	new_sensor = FALSE;
	if (!mlat_ptr->sensor_number.present)
	{
		if (mlat_ptr->data_source_identifier.present)
		{
			t_Ui16 dsi;
			t_Bool found;

			dsi = mlat_ptr->data_source_identifier.value;

			found = FALSE;
			for (ix = 0; ix < M_MAX_NUMBER_OF_SENSORS; ix ++)
			{
				if (sacsic_list[ix].orig_defined &&
                    sacsic_list[ix].orig == dsi)
				{
					found = TRUE;
					break;
				}
			}

			if (!found)
			{
				for (ix = 0; ix < M_MAX_NUMBER_OF_SENSORS; ix ++)
				{
					if (sacsic_list[ix].rsap_defined &&
                        sacsic_list[ix].rsap == dsi)
					{
						found = TRUE;
						break;
					}
				}
			}

			if (found)
			{
				mlat_ptr->sensor_number.present = TRUE;
				mlat_ptr->sensor_number.value = 1 + ix;
			}
		}
	}

	if (new_sensor &&
        mlat_ptr->data_source_identifier.present &&
        mlat_ptr->sensor_number.present)
	{
		list_text (3, "; new sensor %hu found: sacsic=0x%04x (%u/%u)",
                      mlat_ptr->sensor_number.value,
                      mlat_ptr->data_source_identifier.value,
                      hi_byte (mlat_ptr->data_source_identifier.value),
                      lo_byte (mlat_ptr->data_source_identifier.value));
		list_text (3, "\n");
	}

                   /* Decide whether multilateration report is
                      to be listed: */

	to_be_listed = FALSE;
	if (1 <= list_level && list_level <= 3)
	{
		to_be_listed = TRUE;
	}
	if (mlat_ptr->data_format == e_data_format_asterix)
	{
		t_Byte cat;

		cat = mlat_ptr->asterix_category;

		if (cat_selection_defined)
		{
			if (!cat_selection[cat])
			{
				to_be_listed = FALSE;
			}
		}
	}
	if (mlat_ptr->detection_type.present &&
        mlat_ptr->detection_type.reported_from_mlt != e_is_true)
	{
		to_be_listed = FALSE;
	}
	if (mlat_ptr->is_status_message)
	{
		to_be_listed = FALSE;
	}

	mlat_ptr->has_been_listed = FALSE;
	mlat_ptr->to_be_listed = to_be_listed;

                   /* List multilateration report: */
	if (to_be_listed)
	{
		memset (txt, 0, 257);

		list_mlat (mlat_ptr, 256, txt);

		fprintf (list_file, "%s", txt);

		fprintf (list_file, "\n");
	}

	done:
	return;
}
#endif /* LISTER */

#if LISTER
/*----------------------------------------------------------------------------*/
/* list_mlat       -- List multilateration report                             */
/*----------------------------------------------------------------------------*/

 void list_mlat (t_Mlat *mlat_ptr, t_Ui16 length, char *buffer)
{
	t_Real fd_dt;
	t_Bool fd_dt_problem;
	char tmp[81];

				   /* Check parameters: */
	Assert (mlat_ptr != NULL, "Invalid parameter");
	Assert (length > 0, "Invalid parameter");
	Assert (buffer != NULL, "Invalid parameter");

                   /* Check if package has been initiated: */
	if (!initiated)
	{
		initiate ();

		initiated = TRUE;
	}

                   /* Clear write buffer: */
	memset (buffer, 0, length);

                   /* List multilateration report information: */

	fd_dt = 0.0;
	fd_dt_problem = FALSE;
	if (checking_fd &&
        mlat_ptr->frame_time.present &&
        mlat_ptr->detection_time.present)
	{
		fd_dt = time_diff (mlat_ptr->frame_time.value,
                           mlat_ptr->detection_time.value);
		if (fd_dt < -0.010 || fd_dt > 2.000)
		{
			fd_dt_problem = TRUE;
		}
	}

                   /* List frame time: */
	list_ftm (e_object_mlat, mlat_ptr, length, buffer);

                   /* List detection or frame time: */
	list_tod (e_object_mlat, mlat_ptr, length, buffer);

                   /* List sensor number: */
	list_sno (e_object_mlat, mlat_ptr, length, buffer);

                   /* List board/line number: */
	if (handle_multiple_lines)
	{
		strncat (buffer, " ---", length);
	}

                   /* List sensor data source identification: */
	list_dsi (e_object_mlat, mlat_ptr, length, buffer);

                   /* List antenna number: */
	list_ano (e_object_mlat, mlat_ptr, length, buffer);

                   /* List message type: */
	list_mtp (e_object_mlat, mlat_ptr, length, buffer);

                   /* List sector, step or sweep number: */
	list_num (e_object_mlat, mlat_ptr, length, buffer);

                   /* List position: */
	list_pos (e_object_mlat, mlat_ptr, length, buffer);

                   /* List SSR mode S address: */
	list_msa (e_object_mlat, mlat_ptr, length, buffer);

                   /* List aircraft identification: */
	list_aid (e_object_mlat, mlat_ptr, length, buffer);

                   /* List SSR identification: */
	list_idt (e_object_mlat, mlat_ptr, length, buffer);

                   /* Flight level: */
	if (mlat_ptr->mode_c_height.present)
	{
		strncat (buffer, " ", length);
		strncat (buffer, "C:", length);

		if (mlat_ptr->mode_c_height.height_in_error == e_is_true)
		{
			strncat (buffer, " NVA", length);
			strncat (buffer, "   ", length);

			strncat (buffer, " ", length);
			strncat (buffer, "--", length);
		}
		else
		{
			t_Si16 fl;

			fl = mlat_ptr->mode_c_height.value_in_feet / 100;

			if (fl < -10 || fl > 1267)
			{
				strncat (buffer, " NVA", length);
				strncat (buffer, "   ", length);

				strncat (buffer, " ", length);
				strncat (buffer, "--", length);
			}
			else
			{
				memset (tmp, 0, 81);
				if (mlat_ptr->mode_c_height.in_25_feet == e_is_true)
				{
					sprintf (tmp, "%7.2f", 
                                  0.01 * mlat_ptr->mode_c_height.value_in_feet);
				}
				else
				{
					sprintf (tmp, "%4hd   ", fl);
				}

				strncat (buffer, tmp, length);

				strncat (buffer, " ", length);
				if (mlat_ptr->mode_c_height.garbled == e_is_true)
				{
					strncat (buffer, "g", length);
				}
				else
				{
					strncat (buffer, "-", length);
				}
				if (mlat_ptr->mode_c_height.invalid == e_is_true)
				{
					strncat (buffer, "i", length);
				}
				else
				{
					strncat (buffer, "-", length);
				}
			}
		}
	}
	else
	{
		strncat (buffer, " C:----    --", length);
	}

	strncat (buffer, " ;", length);

	if (list_geometric_height && mlat_ptr->geometric_altitude.present)
	{
		memset (tmp, 0, 81);
		sprintf (tmp, "%6.2f",
                      M_MTR2FL * mlat_ptr->geometric_altitude.value);

		strncat (buffer, " G:", length);
		strncat (buffer, tmp, length);
	}

	if (list_geometric_height && mlat_ptr->measured_height.present)
	{
		memset (tmp, 0, 81);
		sprintf (tmp, "%6.2f", M_MTR2FL * mlat_ptr->measured_height.value);

		strncat (buffer, " H:", length);
		strncat (buffer, tmp, length);
	}

	if (mlat_ptr->acas_resolution_advisory_report.present)
	{
		strncat (buffer, " acas", length);
	}

	if (mlat_ptr->fs_alert == e_is_true)
	{
		strncat (buffer, " alert", length);
	}

	if (fd_dt_problem)
	{
		memset (tmp, 0, 81);
		sprintf (tmp, " fd_dt=%.3f secs", fd_dt);
		strncat (buffer, tmp, length);
	}

	if (mlat_ptr->flight_status.present &&
        mlat_ptr->flight_status.value_arc == 1)
	{
		strncat (buffer, " arc", length);
	}

	if (mlat_ptr->corrupted_replies)
	{
		strncat (buffer, " crs", length);
	}

	if (mlat_ptr->plot_qualification.defined)
	{
		if (mlat_ptr->plot_qualification.parent_of_split)
		{
			strncat (buffer, " fpa(p-split)", length);
		}
		if (mlat_ptr->plot_qualification.split)
		{
			strncat (buffer, " fpa(split)", length);
		}
		if (mlat_ptr->plot_qualification.split_pair)
		{
			int qf;

			strncat (buffer, " fpa(split-pair", length);

			qf = mlat_ptr->plot_qualification.qualification_factor;

			memset (tmp, 0, 81);
			sprintf (tmp, ":%d", qf);
			strncat (buffer, tmp, length);

			strncat (buffer, ")", length);
		}
	}

	if (mlat_ptr->detection_type.from_fixed_field_transponder == e_is_true)
	{
		strncat (buffer, " fxd", length);
	}

	if (mlat_ptr->ground_bit_set)
	{
		strncat (buffer, " gnd", length);
	}

	if (list_ground_vector)
	{
		if (mlat_ptr->computed_velocity.present)
		{
			t_Real gsp, hdg;
			t_Real vx, vy;

			vx = mlat_ptr->computed_velocity.value_vx;
			vy = mlat_ptr->computed_velocity.value_vy;

			gsp = sqrt (vx * vx + vy * vy);
			hdg = azimuth (vx, vy);

			memset (tmp, 0, 81);

			sprintf (tmp, " gsp=%.2f [kts] hdg=%.3f [deg]",
                          M_MPS2KTS * gsp, hdg);
			strncat (buffer, tmp, length);
		}
		else if (mlat_ptr->ground_vector.present)
		{
			memset (tmp, 0, 81);

			sprintf (tmp, " gsp=%.2f [kts] hdg=%.3f [deg]",
                          M_MPS2KTS * mlat_ptr->ground_vector.value_gsp,
                          M_RAD2DEG * mlat_ptr->ground_vector.value_hdg);
			strncat (buffer, tmp, length);
		}
	}

	if (mlat_ptr->detection_type.simulated == e_is_true)
	{
		strncat (buffer, " sim", length);
	}

	if (mlat_ptr->detection_type.test_target == e_is_true)
	{
		strncat (buffer, " tst", length);
	}

	if (mlat_ptr->special_position_indication == e_is_true)
	{
		strncat (buffer, " spi", length);
	}

	if (mlat_ptr->wec_list.count > 0)
	{
		int j;

		strncat (buffer, " wec(", length);
		for (j = 0; j < mlat_ptr->wec_list.count; j ++)
		{
			if (j > 0)
			{
				strncat (buffer, ",", length);
			}

			memset (tmp, 0, 81);
			sprintf (tmp, "%u", mlat_ptr->wec_list.list[j]);
			strncat (buffer, tmp, length);
		}
		strncat (buffer, ")", length);
	}

	return;
}
#endif /* LISTER */

#if LISTER
/*----------------------------------------------------------------------------*/
/* list_rsrv       -- List radar service information                          */
/*----------------------------------------------------------------------------*/

 void list_rsrv (t_Rsrv *rsrv_ptr)
{
	int ix;
	t_Bool new_sensor;
	t_Secs tm;
	t_Bool to_be_listed;
	char txt[257];

				   /* Check parameters: */
	Assert (rsrv_ptr != NULL, "NULL pointer");

                   /* Check if the list file is available: */
	Assert (list_file != NULL, "No list file");

                   /* Check if package has been initiated: */
	if (!initiated)
	{
		initiate ();

		initiated = TRUE;
	}

                   /* Check against start offset: */
	if (start_offset_defined && input_offset < start_offset)
	{
		goto done;
	}

                   /* Extract relevant time: */
	tm = 0.0;
	if (rsrv_ptr->message_time.present)
	{
		tm = rsrv_ptr->message_time.value;
	}
	else if (rsrv_ptr->frame_time.present)
	{
		tm = rsrv_ptr->frame_time.value;
	}

                   /* Adapt by time bias, if any: */
	if (time_bias_defined)
	{
		tm += time_bias;
	}

                   /* Check for sensor number: */
	new_sensor = FALSE;
	if (!rsrv_ptr->sensor_number.present)
	{
		if (rsrv_ptr->data_source_identifier.present)
		{
			t_Ui16 dsi;
			t_Bool found;

			dsi = rsrv_ptr->data_source_identifier.value;

			found = FALSE;
			for (ix = 0; ix < M_MAX_NUMBER_OF_SENSORS; ix ++)
			{
				if (sacsic_list[ix].orig_defined &&
                    sacsic_list[ix].orig == dsi)
				{
					found = TRUE;
					break;
				}
			}

			if (!found)
			{
				for (ix = 0; ix < M_MAX_NUMBER_OF_SENSORS; ix ++)
				{
					if (sacsic_list[ix].rsap_defined &&
                        sacsic_list[ix].rsap == dsi)
					{
						found = TRUE;
						break;
					}
				}
			}

			if (found)
			{
				rsrv_ptr->sensor_number.present = TRUE;
				rsrv_ptr->sensor_number.value = 1 + ix;
			}
		}
	}

	if (new_sensor &&
        rsrv_ptr->data_source_identifier.present &&
        rsrv_ptr->sensor_number.present)
	{
		list_text (3, "; new sensor %hu found: sacsic=0x%04x (%u/%u)",
                      rsrv_ptr->sensor_number.value,
                      rsrv_ptr->data_source_identifier.value,
                      hi_byte (rsrv_ptr->data_source_identifier.value),
                      lo_byte (rsrv_ptr->data_source_identifier.value));
		list_text (3, "\n");
	}

                   /* Decide whether radar service message is
                      to be listed: */

	to_be_listed = FALSE;
	if (1 <= list_level && list_level <= 3)
	{
		to_be_listed = TRUE;
	}
	if (rsrv_ptr->data_format == e_data_format_asterix)
	{
		t_Byte cat;

		cat = rsrv_ptr->asterix_category;

		if (cat_selection_defined)
		{
			if (!cat_selection[cat])
			{
				to_be_listed = FALSE;
			}
		}
	}

	rsrv_ptr->has_been_listed = FALSE;
	rsrv_ptr->to_be_listed = to_be_listed;

                   /* List radar service message: */
	if (to_be_listed)
	{
		memset (txt, 0, 257);

		list_rsrv (rsrv_ptr, 256, txt);

		fprintf (list_file, "%s", txt);

		fprintf (list_file, "\n");
	}

	done:
	return;
}
#endif /* LISTER */

#if LISTER
/*----------------------------------------------------------------------------*/
/* list_rsrv       -- List radar service information                          */
/*----------------------------------------------------------------------------*/

 void list_rsrv (t_Rsrv *rsrv_ptr, t_Ui16 length, char *buffer)
{
	t_Real fd_dt;
	t_Bool fd_dt_problem;
	char tmp[81];

				   /* Check parameters: */
	Assert (rsrv_ptr != NULL, "NULL pointer");
	Assert (length > 0, "Invalid parameter");
	Assert (buffer != NULL, "NULL pointer");

                   /* Check if package has been initiated: */
	if (!initiated)
	{
		initiate ();

		initiated = TRUE;
	}

                   /* Clear write buffer: */
	memset (buffer, 0, length);

                   /* List radar service information: */

	fd_dt = 0.0;
	fd_dt_problem = FALSE;
	if (checking_fd &&
        rsrv_ptr->frame_time.present &&
        rsrv_ptr->message_time.present)
	{
		fd_dt = time_diff (rsrv_ptr->frame_time.value,
                           rsrv_ptr->message_time.value);
		if (fd_dt < -0.010 || fd_dt > 1.000)
		{
			fd_dt_problem = TRUE;
		}
	}

                   /* List frame time: */
	list_ftm (e_object_rsrv, rsrv_ptr, length, buffer);

                   /* List ASTERIX category: */
	if (list_cat && rsrv_ptr->data_format == e_data_format_asterix)
	{
		memset (tmp, 0, 81);
		sprintf (tmp, " [A%03u]", rsrv_ptr->asterix_category);
		strncat (buffer, tmp, length);
	}

                   /* List message or frame time: */
	list_tod (e_object_rsrv, rsrv_ptr, length, buffer);

                   /* List sensor number: */
	list_sno (e_object_rsrv, rsrv_ptr, length, buffer);

                   /* List board/line number: */
	if (handle_multiple_lines)
	{
		if (rsrv_ptr->line_number > 0)
		{
			memset (tmp, 0, 81);
			sprintf (tmp, " " M_FMT_LINE, rsrv_ptr->line_number);
			strncat (buffer, tmp, length);
		}
		else
		{
			strncat (buffer, " ----", length);
		}
	}

                   /* List sensor data source identification: */
	list_dsi (e_object_rsrv, rsrv_ptr, length, buffer);

                   /* List antenna number: */
	list_ano (e_object_rsrv, rsrv_ptr, length, buffer);

                   /* List message type: */
	list_mtp (e_object_rsrv, rsrv_ptr, length, buffer);

                   /* List sector, step or sweep number: */
	list_num (e_object_rsrv, rsrv_ptr, length, buffer);

                   /* List position: */
	list_pos (e_object_rsrv, rsrv_ptr, length, buffer);

                   /* List SSR mode S address: */
	list_msa (e_object_rsrv, rsrv_ptr, length, buffer);

                   /* List aircraft identification: */
	list_aid (e_object_rsrv, rsrv_ptr, length, buffer);

                   /* List SSR identification: */
	list_idt (e_object_rsrv, rsrv_ptr, length, buffer);

                   /* Flight level (dummy): */
	strncat (buffer, "             ", length);

                   /* Supplementary information: */
	strncat (buffer, " ;", length);

	if (fd_dt_problem)
	{
		memset (tmp, 0, 81);
		sprintf (tmp, " fd_dt=%.3f secs", fd_dt);
		strncat (buffer, tmp, length);
	}

	if (rsrv_ptr->antenna_rotation_speed.present)
	{
		t_Real att;

		Assert (rsrv_ptr->antenna_rotation_speed.value != 0.0,
                "Dividing by zero");
		att = M_TWO_PI / rsrv_ptr->antenna_rotation_speed.value;

		memset (tmp, 0, 81);
		sprintf (tmp, "att=%.3f sec", att);

		strncat (buffer, " ", length);
		strncat (buffer, tmp, length);
	}

	if (rsrv_ptr->timejump_info.jump_occured)
	{
		memset (tmp, 0, 81);
		sprintf (tmp, "timejump(%hu)=%.3f secs",
                      rsrv_ptr->timejump_info.jump_type,
                      rsrv_ptr->timejump_info.value);
	}

	return;
}
#endif /* LISTER */

#if LISTER
/*----------------------------------------------------------------------------*/
/* list_rtgt       -- List radar target information                           */
/*----------------------------------------------------------------------------*/

 void list_rtgt (t_Rtgt *rtgt_ptr)
{
	int ix;
	t_Bool new_sensor;
	t_Secs tm;
	t_Bool to_be_listed;
	char txt[1000];

				   /* Check parameters: */
	Assert (rtgt_ptr != NULL, "Invalid parameter");

                   /* Check if the list file is available: */
	Assert (list_file != NULL, "No list file");

                   /* Check if package has been initiated: */
	if (!initiated)
	{
		initiate ();

		initiated = TRUE;
	}

                   /* Check against start offset: */
	if (start_offset_defined && input_offset < start_offset)
	{
		goto done;
	}

                   /* Extract relevant time: */
	tm = 0.0;
	if (rtgt_ptr->detection_time.present)
	{
		tm = rtgt_ptr->detection_time.value;
	}
	else if (rtgt_ptr->estimated_time.present)
	{
		tm = rtgt_ptr->estimated_time.value;
	}
	else if (rtgt_ptr->frame_time.present)
	{
		tm = rtgt_ptr->frame_time.value;
	}

                   /* Adapt by time bias, if any: */
	if (time_bias_defined)
	{
		tm += time_bias;
	}

                   /* Check for sensor number: */
	new_sensor = FALSE;
	if (!rtgt_ptr->sensor_number.present)
	{
		if (rtgt_ptr->data_source_identifier.present)
		{
			t_Ui16 dsi;
			t_Bool found;

			dsi = rtgt_ptr->data_source_identifier.value;

			found = FALSE;
			for (ix = 0; ix < M_MAX_NUMBER_OF_SENSORS; ix ++)
			{
				if (sacsic_list[ix].orig_defined &&
                    sacsic_list[ix].orig == dsi)
				{
					found = TRUE;
					break;
				}
			}

			if (!found)
			{
				for (ix = 0; ix < M_MAX_NUMBER_OF_SENSORS; ix ++)
				{
					if (sacsic_list[ix].rsap_defined &&
                        sacsic_list[ix].rsap == dsi)
					{
						found = TRUE;
						break;
					}
				}
			}

			if (found)
			{
				rtgt_ptr->sensor_number.present = TRUE;
				rtgt_ptr->sensor_number.value = 1 + ix;
			}
		}
	}

	if (new_sensor &&
        rtgt_ptr->data_source_identifier.present &&
        rtgt_ptr->sensor_number.present)
	{
		list_text (3, "; new sensor %hu found: sacsic=0x%04x (%u/%u)",
                      rtgt_ptr->sensor_number.value,
                      rtgt_ptr->data_source_identifier.value,
                      hi_byte (rtgt_ptr->data_source_identifier.value),
                      lo_byte (rtgt_ptr->data_source_identifier.value));
		list_text (3, "\n");
	}

                   /* Decide whether this radar target is
                      to be listed: */

	to_be_listed = FALSE;
	if (1 <= list_level && list_level <= 3)
	{
		to_be_listed = TRUE;
	}
	if (rtgt_ptr->data_format == e_data_format_asterix)
	{
		t_Byte cat;

		cat = rtgt_ptr->asterix_category;

		if (cat_selection_defined)
		{
			if (!cat_selection[cat])
			{
				to_be_listed = FALSE;
			}
		}
	}

	rtgt_ptr->has_been_listed = FALSE;
	rtgt_ptr->to_be_listed = to_be_listed;

                   /* List radar target report: */
	if (to_be_listed)
	{
		memset (txt, 0, 1000);

		list_rtgt (rtgt_ptr, 256, txt);

		list_text (3, "%s", txt);
		list_text (3, "\n");
	}

	done:
	return;
}
#endif /* LISTER */

#if LISTER
/*----------------------------------------------------------------------------*/
/* list_rtgt       -- List radar target information                           */
/*----------------------------------------------------------------------------*/

 void list_rtgt (t_Rtgt *rtgt_ptr, t_Ui16 length, char *buffer)
{
	t_Real fd_dt;
	t_Bool fd_dt_problem;
	t_Bool h3d_reported;
	t_Bool ptod_problem;
	char tmp[81];

				   /* Check parameters: */
	Assert (rtgt_ptr != NULL, "Invalid parameter");
	Assert (length > 0, "Invalid parameter");
	Assert (buffer != NULL, "Invalid parameter");

                   /* Check if package has been initiated: */
	if (!initiated)
	{
		initiate ();

		initiated = TRUE;
	}

                   /* Clear write buffer: */
	memset (buffer, 0, length);

                   /* List radar target information: */

	fd_dt = 0.0;
	fd_dt_problem = FALSE;
	if (checking_fd &&
        rtgt_ptr->detection_time.present &&
        rtgt_ptr->frame_time.present)
	{
		fd_dt = time_diff (rtgt_ptr->frame_time.value,
                           rtgt_ptr->detection_time.value);
		if (fd_dt < -0.010 || fd_dt > 1.500)
		{
			fd_dt_problem = TRUE;
		}
	}

	ptod_problem = FALSE;
	if (checking_fd &&
        last_tod_available &&
        rtgt_ptr->detection_ptod.present &&
        !rtgt_ptr->detection_time.present)
	{
		ptod_problem = TRUE;
	}

                   /* List frame time: */
	list_ftm (e_object_rtgt, rtgt_ptr, length, buffer);

                   /* List ASTERIX category: */
	if (list_cat && rtgt_ptr->data_format == e_data_format_asterix)
	{
		memset (tmp, 0, 81);
		sprintf (tmp, " [A%03u]", rtgt_ptr->asterix_category);
		strncat (buffer, tmp, length);
	}

                   /* List detection, estimated or frame time: */
	list_tod (e_object_rtgt, rtgt_ptr, length, buffer);

                   /* List sensor number: */
	list_sno (e_object_rtgt, rtgt_ptr, length, buffer);

                   /* List board/line number: */
	if (handle_multiple_lines)
	{
		if (rtgt_ptr->line_number > 0)
		{
			memset (tmp, 0, 81);
			sprintf (tmp, " " M_FMT_LINE, rtgt_ptr->line_number);
			strncat (buffer, tmp, length);
		}
		else
		{
			strncat (buffer, " ----", length);
		}
	}

                   /* List sensor data source identification: */
	list_dsi (e_object_rtgt, rtgt_ptr, length, buffer);

                   /* List antenna number: */
	list_ano (e_object_rtgt, rtgt_ptr, length, buffer);

                   /* List detection type: */
	list_mtp (e_object_rtgt, rtgt_ptr, length, buffer);

                   /* List sweep angle or radar track number: */
	list_num (e_object_rtgt, rtgt_ptr, length, buffer);

                   /* List position: */
	list_pos (e_object_rtgt, rtgt_ptr, length, buffer);

                   /* List SSR mode S address: */
	list_msa (e_object_rtgt, rtgt_ptr, length, buffer);

                   /* List aircraft identification: */
	list_aid (e_object_rtgt, rtgt_ptr, length, buffer);

                   /* List SSR identification: */
	list_idt (e_object_rtgt, rtgt_ptr, length, buffer);

                   /* Flight level: */
	h3d_reported = FALSE;
	if ((rtgt_ptr->detection_type.reported_from_mlt == e_is_true ||
         rtgt_ptr->detection_type.reported_from_ssr == e_is_true) &&
        rtgt_ptr->mode_c_height.present)
	{
		strncat (buffer, " ", length);
		strncat (buffer, "C:", length);

		if (rtgt_ptr->mode_c_height.height_in_error == e_is_true)
		{
			strncat (buffer, " NVA", length);
			strncat (buffer, "   ", length);

			strncat (buffer, " ", length);
			strncat (buffer, "--", length);
		}
		else
		{
			t_Si16 fl;

			fl = rtgt_ptr->mode_c_height.value_in_feet / 100;

			if (fl < -10 || fl > 1267)
			{
				strncat (buffer, " NVA", length);
				strncat (buffer, "   ", length);

				strncat (buffer, " ", length);
				strncat (buffer, "--", length);
			}
			else
			{
				memset (tmp, 0, 81);
				if (rtgt_ptr->mode_c_height.in_25_feet == e_is_true)
				{
					sprintf (tmp, "%7.2f", 
                                  0.01 * rtgt_ptr->mode_c_height.value_in_feet);
				}
				else
				{
					sprintf (tmp, "%4hd   ", fl);
				}

				strncat (buffer, tmp, length);

				strncat (buffer, " ", length);
				if (rtgt_ptr->mode_c_height.garbled == e_is_true)
				{
					strncat (buffer, "g", length);
				}
				else
				{
					strncat (buffer, "-", length);
				}
				if (rtgt_ptr->mode_c_height.invalid == e_is_true)
				{
					strncat (buffer, "i", length);
				}
				else
				{
					strncat (buffer, "-", length);
				}
			}
		}
	}
	else if (rtgt_ptr->detection_type.reported_from_ssr == e_is_true &&
             rtgt_ptr->metric_height.present)
	{
		strncat (buffer, " ", length);
		if (rtgt_ptr->metric_height.is_relative)
		{
			strncat (buffer, "*", length);
		}
		else
		{
			strncat (buffer, "H", length);
		}
		strncat (buffer, ":", length);

		memset (tmp, 0, 81);
		sprintf (tmp, "%4hd", (t_Si16) (0.1 * rtgt_ptr->metric_height.value));
		strncat (buffer, tmp, length);
		strncat (buffer, "   ", length);

		strncat (buffer, " ", length);
		strncat (buffer, "--", length);
	}
#if nix_do
	else if (rtgt_ptr->height_3d_present)
	{
		strncat (buffer, " ", length);
		strncat (buffer, "D", length);
		strncat (buffer, ":", length);

		memset (tmp, 0, 81);
		sprintf (tmp, "%7.2f", 0.01 * rtgt_ptr->height_3d);
		strncat (buffer, tmp, length);

		strncat (buffer, " ", length);
		strncat (buffer, "-", length);
		if (rtgt_ptr->height_3d_valid)
		{
			strncat (buffer, "v", length);
		}
		else
		{
			strncat (buffer, "i", length);
		}

		h3d_reported = TRUE;
	}
#endif
	else if (rtgt_ptr->detection_type.reported_from_mlt == e_is_true ||
             rtgt_ptr->detection_type.reported_from_ssr == e_is_true)
	{
		strncat (buffer, " ", length);
		strncat (buffer, "-:", length);
		strncat (buffer, " ---", length);
		strncat (buffer, "   ", length);

		strncat (buffer, " ", length);
		strncat (buffer, "--", length);
	}
	else
	{
		strncat (buffer, " ", length);
		strncat (buffer, "  ", length);
		strncat (buffer, "    ", length);
		strncat (buffer, "   ", length);

		strncat (buffer, " ", length);
		strncat (buffer, "  ", length);
	}

	strncat (buffer, " ;", length);

	if (rtgt_ptr->acas_resolution_advisory_report.present)
	{
		strncat (buffer, " acas", length);
	}

	if (rtgt_ptr->fs_alert == e_is_true)
	{
		strncat (buffer, " alert", length);
	}

	if (rtgt_ptr->detection_type.is_radar_track == e_is_true &&
        rtgt_ptr->to_be_cancelled == e_is_true)
	{
		strncat (buffer, " cancel", length);
	}

	if (rtgt_ptr->civil_emergency == e_is_true)
	{
		strncat (buffer, " cem", length);
	}

	if (rtgt_ptr->detection_type.from_fixed_field_transponder == e_is_true)
	{
		strncat (buffer, " fxd", length);
	}

	if (rtgt_ptr->detection_type.simulated == e_is_true)
	{
		strncat (buffer, " sim", length);
	}

	if (rtgt_ptr->detection_type.test_target == e_is_true)
	{
		strncat (buffer, " tst", length);
	}

	if (rtgt_ptr->emergency_1 == e_is_true)
	{
		strncat (buffer, " em1", length);
	}
	if (rtgt_ptr->emergency_2 == e_is_true)
	{
		strncat (buffer, " em2", length);
	}
	if (rtgt_ptr->emergency_3 == e_is_true)
	{
		strncat (buffer, " em3", length);
	}

	if (rtgt_ptr->excessive_dt_fd && checking_fd)
	{
		strncat (buffer, " dt_fd", length);
	}
	if (rtgt_ptr->excessive_dt_fe && checking_fd)
	{
		strncat (buffer, " dt_fe", length);
	}

	if (fd_dt_problem)
	{
		memset (tmp, 0, 81);
		sprintf (tmp, " fd_dt=%.3f secs", fd_dt);
		strncat (buffer, tmp, length);
	}

	if (rtgt_ptr->fs_on_ground == e_is_true)
	{
		strncat (buffer, " gnd", length);
	}

	if (list_ground_vector && rtgt_ptr->ground_vector.present)
	{
		memset (tmp, 0, 81);

		sprintf (tmp, " gsp=%.2f [kts] hdg=%.3f [deg]",
                      M_MPS2KTS * rtgt_ptr->ground_vector.value_gsp,
                      M_RAD2DEG * rtgt_ptr->ground_vector.value_hdg);
		strncat (buffer, tmp, length);
	}

	if (rtgt_ptr->height_3d.present && !h3d_reported)
	{
		strncat (buffer, " ", length);
		strncat (buffer, "D", length);
		strncat (buffer, ":", length);

		memset (tmp, 0, 81);
		sprintf (tmp, "%.2f", 0.01 * rtgt_ptr->height_3d.value_in_feet);
		strncat (buffer, tmp, length);
	}

	if (rtgt_ptr->target_address.present &&
        rtgt_ptr->target_address.value == 0)
	{
		strncat (buffer, " inv_msa", length);
	}

	if (rtgt_ptr->data_source_identifier.present &&
        (rtgt_ptr->data_source_identifier.value == 0x3001 ||
         rtgt_ptr->data_source_identifier.value == 0x31a1) &&
         rtgt_ptr->measured_azm.present &&
         rtgt_ptr->measured_azm.value * M_RAD2DEG >= 248.0 &&
         rtgt_ptr->measured_azm.value * M_RAD2DEG <= 251.5 &&
         rtgt_ptr->measured_rng.present &&
         rtgt_ptr->measured_rng.value * M_MTR2NMI >= 24.10 &&
         rtgt_ptr->measured_rng.value * M_MTR2NMI <= 25.60)
	{
		strncat (buffer, " javbla", length);

		if (rtgt_ptr->mode_c_height.present &&
            rtgt_ptr->mode_c_height.height_in_error != e_is_true &&
            rtgt_ptr->mode_c_height.value_in_feet >= 600)
		{
			strncat (buffer, " look", length);
		}
	}

	if (rtgt_ptr->military_emergency == e_is_true)
	{
		strncat (buffer, " mil_em", length);
	}
	if (rtgt_ptr->military_ident == e_is_true)
	{
		strncat (buffer, " mil_id", length);
	}

	if (rtgt_ptr->mode_1_info.present)
	{
		strncat (buffer, " 1:", length);

		memset (tmp, 0, 81);
		sprintf (tmp, M_FMT_CODE, rtgt_ptr->mode_1_info.code);
		strncat (buffer, tmp, length);

		strncat (buffer, " ", length);
		strncat (buffer, "-", length);
		if (rtgt_ptr->mode_1_info.code_invalid == e_is_true)
		{
			strncat (buffer, "i", length);
		}
		else
		{
			strncat (buffer, "-", length);
		}
		strncat (buffer, "-", length);
	}

	if (rtgt_ptr->mode_2_info.present)
	{
		strncat (buffer, " 2:", length);

		memset (tmp, 0, 81);
		sprintf (tmp, M_FMT_CODE, rtgt_ptr->mode_2_info.code);
		strncat (buffer, tmp, length);

		strncat (buffer, " ", length);
		if (rtgt_ptr->mode_2_info.code_garbled == e_is_true)
		{
			strncat (buffer, "g", length);
		}
		else
		{
			strncat (buffer, "-", length);
		}
		if (rtgt_ptr->mode_2_info.code_invalid == e_is_true)
		{
			strncat (buffer, "i", length);
		}
		else
		{
			strncat (buffer, "-", length);
		}
		if (rtgt_ptr->mode_2_info.code_smoothed == e_is_true)
		{
			strncat (buffer, "l", length);
		}
		else
		{
			strncat (buffer, "-", length);
		}
	}

	if (rtgt_ptr->mode_4_info.present)
	{
		strncat (buffer, " m4=", length);

		memset (tmp, 0, 81);
		sprintf (tmp, M_FMT_UI16, rtgt_ptr->mode_4_info.value);
		strncat (buffer, tmp, length);

		if (rtgt_ptr->mode_4_info.invalid == e_is_true)
		{
			strncat (buffer, "i", length);
		}
	}

	if (rtgt_ptr->mode_c_code.present &&
        rtgt_ptr->mode_c_code.code_confidence_present)
	{
		strncat (buffer, " mC=", length);

		memset (tmp, 0, 81);
		sprintf (tmp, M_FMT_CODE, rtgt_ptr->mode_c_code.code & 0x0fff);
		strncat (buffer, tmp, length);

		strncat (buffer, ":", length);

		memset (tmp, 0, 81);
		sprintf (tmp, M_FMT_CODE, rtgt_ptr->mode_c_code.code_confidence & 0x0fff);
		strncat (buffer, tmp, length);

		strncat (buffer, " ", length);
		if (rtgt_ptr->mode_c_code.code_garbled == e_is_true)
		{
			strncat (buffer, "g", length);
		}
		else
		{
			strncat (buffer, "-", length);
		}
		if (rtgt_ptr->mode_c_code.code_invalid == e_is_true)
		{
			strncat (buffer, "i", length);
		}
		else
		{
			strncat (buffer, "-", length);
		}
	}

	if (rtgt_ptr->plot_qualification.defined)
	{
		if (rtgt_ptr->plot_qualification.parent_of_shadow)
		{
			strncat (buffer, " fpa(p-shadow)", length);
		}
		if (rtgt_ptr->plot_qualification.parent_of_split)
		{
			strncat (buffer, " fpa(p-split)", length);
		}
		if (rtgt_ptr->plot_qualification.shadow)
		{
			strncat (buffer, " fpa(shadow)", length);
		}
		if (rtgt_ptr->plot_qualification.split)
		{
			strncat (buffer, " fpa(split)", length);
		}
		if (rtgt_ptr->plot_qualification.split_pair)
		{
			int qf;

			strncat (buffer, " fpa(split-pair", length);

			qf = rtgt_ptr->plot_qualification.qualification_factor;

			memset (tmp, 0, 81);
			sprintf (tmp, ":%d", qf);
			strncat (buffer, tmp, length);

			strncat (buffer, ")", length);
		}
	}

	if (ptod_problem)
	{
		strncat (buffer, " ptod_problem", length);
	}

	if (rtgt_ptr->radial_speed.present)
	{
		memset (tmp, 0, 81);
		sprintf (tmp, " r_spd=%.2f", rtgt_ptr->radial_speed.value);
		strncat (buffer, tmp, length);
	}

	if (rtgt_ptr->received_power.present && list_received_power)
	{
		memset (tmp, 0, 81);
		sprintf (tmp, " pwr=" M_FMT_SI16, rtgt_ptr->received_power.value);
		strncat (buffer, tmp, length);
	}

	if (rtgt_ptr->special_position_indication == e_is_true)
	{
		strncat (buffer, " spi", length);
	}

	if (rtgt_ptr->wec_list.count > 0)
	{
		int j;

		strncat (buffer, " wec(", length);
		for (j = 0; j < rtgt_ptr->wec_list.count; j ++)
		{
			if (j > 0)
			{
				strncat (buffer, ",", length);
			}

			memset (tmp, 0, 81);
			sprintf (tmp, "%u", rtgt_ptr->wec_list.list[j]);
			strncat (buffer, tmp, length);
		}
		strncat (buffer, ")", length);
	}

	return;
}
#endif /* LISTER */

#if LISTER
/*----------------------------------------------------------------------------*/
/* list_ssta       -- List sensor status information                          */
/*----------------------------------------------------------------------------*/

 void list_ssta (t_Ssta *ssta_ptr)
{
	static t_Secs earliest_time = 0.0;
	static t_Bool earliest_time_defined = FALSE;
	t_Bool large_value;
	t_Secs tm;
	char tmp[81];

				   /* Check parameters: */
	Assert (ssta_ptr != NULL, "NULL pointer");

                   /* Check if the list file is available: */
	Assert (list_file != NULL, "No list file");

                   /* Check if package has been initiated: */
	if (!initiated)
	{
		initiate ();

		initiated = TRUE;
	}

	if (!ssta_ptr->connection_status_present)
	{
		goto done;
	}
	if (!ssta_ptr->reporting_time_present)
	{
		goto done;
	}
	if (!ssta_ptr->sensor_id_present)
	{
		goto done;
	}

	if (list_ssta_info_for_excel && ssta_ptr->connection_status != 0)
	{
		goto done;
	}

	fprintf (list_file, "ssta:");

	if (list_frame_time)
	{
		tm = ssta_ptr->reporting_time;
		memset (tmp, 0, 81);
		strncat (tmp, utc_text (tm), 80);
		fprintf (list_file, " %s", tmp);
	}

	tm = ssta_ptr->reporting_time;
	if (earliest_time_defined)
	{
		tm = time_diff (tm, earliest_time);
	}
	else
	{
		earliest_time = tm;
		earliest_time_defined = TRUE;

		tm = 0.0;
	}

	memset (tmp, 0, 81);
	sprintf (tmp, "%09.3f", tm);
	if (list_ssta_info_for_excel)
	{
		tmp[8] = '\0';
		excelize (tmp);
	}
	fprintf (list_file, " %s", tmp);

	fprintf (list_file, "  0x%04x", ssta_ptr->sensor_id);

	if (!list_ssta_info_for_excel)
	{
		t_Byte stat;

		stat = ssta_ptr->connection_status;

		fprintf (list_file, " status=%u", stat);
		fprintf (list_file, " (");
		if (stat == 0)
		{
			fprintf (list_file, "operational   ");
		}
		else if (stat == 1)
		{
			fprintf (list_file, "degraded      ");
		}
		else if (stat == 2)
		{
			fprintf (list_file, "initialization");
		}
		else if (stat == 3)
		{
			fprintf (list_file, "not connected ");
		}
		else
		{
			fprintf (list_file, "              ");
		}
		fprintf (list_file, ")");
	}

	fprintf (list_file, " ");

	memset (tmp, 0, 81);
	if (ssta_ptr->psr_azm_bias_present)
	{
		sprintf (tmp, "%6.3f", ssta_ptr->psr_azm_bias);
	}
	else
	{
		strncat (tmp, "---.---", 80);
	}
	if (list_ssta_info_for_excel)
	{
		excelize (tmp);
	}
	fprintf (list_file, " %s", tmp);

	memset (tmp, 0, 81);
	if (ssta_ptr->psr_rng_bias_present)
	{
		sprintf (tmp, "%7.1f", ssta_ptr->psr_rng_bias);
	}
	else
	{
		strncat (tmp, "-----.-", 80);
	}
	if (list_ssta_info_for_excel)
	{
		excelize (tmp);
	}
	fprintf (list_file, " %s", tmp);

	memset (tmp, 0, 81);
	if (ssta_ptr->psr_rng_gain_present)
	{
		sprintf (tmp, "%9.6f", ssta_ptr->psr_rng_gain);
	}
	else
	{
		strncat (tmp, "--.------", 80);
	}
	if (list_ssta_info_for_excel)
	{
		excelize (tmp);
	}
	fprintf (list_file, " %s", tmp);

	fprintf (list_file, "  ");

	memset (tmp, 0, 81);
	if (ssta_ptr->ssr_azm_bias_present)
	{
		sprintf (tmp, "%6.3f", ssta_ptr->ssr_azm_bias);
	}
	else
	{
		strncat (tmp, "---.---", 80);
	}
	if (list_ssta_info_for_excel)
	{
		excelize (tmp);
	}
	fprintf (list_file, " %s", tmp);

	memset (tmp, 0, 81);
	if (ssta_ptr->ssr_rng_bias_present)
	{
		sprintf (tmp, "%7.1f", ssta_ptr->ssr_rng_bias);
	}
	else
	{
		strncat (tmp, "-----.-", 80);
	}
	if (list_ssta_info_for_excel)
	{
		excelize (tmp);
	}
	fprintf (list_file, " %s", tmp);

	memset (tmp, 0, 81);
	if (ssta_ptr->ssr_rng_gain_present)
	{
		sprintf (tmp, "%9.6f", ssta_ptr->ssr_rng_gain);
	}
	else
	{
		strncat (tmp, "--.------", 80);
	}
	if (list_ssta_info_for_excel)
	{
		excelize (tmp);
	}
	fprintf (list_file, " %s", tmp);

	memset (tmp, 0, 81);
	if (ssta_ptr->time_stamping_bias_present)
	{
		sprintf (tmp, "%9.6f", ssta_ptr->time_stamping_bias);
	}
	else
	{
		strncat (tmp, "--.------", 80);
	}
	if (list_ssta_info_for_excel)
	{
		excelize (tmp);
	}
	fprintf (list_file, " %s", tmp);

	large_value = FALSE;
	if (ssta_ptr->psr_azm_bias_present &&
        fabs (ssta_ptr->psr_azm_bias) >= 0.3)
	{
		large_value = TRUE;
	}
	if (ssta_ptr->psr_rng_bias_present &&
        fabs (ssta_ptr->psr_rng_bias) >= 250.0)
	{
		large_value = TRUE;
	}
	if (ssta_ptr->psr_rng_gain_present &&
        fabs (ssta_ptr->psr_rng_gain) >= 0.05)
	{
		large_value = TRUE;
	}
	if (ssta_ptr->ssr_azm_bias_present &&
        fabs (ssta_ptr->ssr_azm_bias) >= 0.3)
	{
		large_value = TRUE;
	}
	if (ssta_ptr->ssr_rng_bias_present &&
        fabs (ssta_ptr->ssr_rng_bias) >= 250.0)
	{
		large_value = TRUE;
	}
	if (ssta_ptr->ssr_rng_gain_present &&
        fabs (ssta_ptr->ssr_rng_gain) >= 0.001)
	{
		large_value = TRUE;
	}
	if (ssta_ptr->time_stamping_bias_present &&
        fabs (ssta_ptr->time_stamping_bias) >= 0.1)
	{
		large_value = TRUE;
	}
	/* tba ... */
	if (large_value)
	{
		fprintf (list_file, " ; large");
	}

	fprintf (list_file, "\n");
		
	done:          /* We are done */
	return;
}
#endif /* LISTER */

#if LISTER
/*----------------------------------------------------------------------------*/
/* list_step       -- List system picture step                                */
/*----------------------------------------------------------------------------*/

 void list_step (t_Step *step_ptr)
{
	char tmp[81];
	char txt[257];

				   /* Check parameters: */
	Assert (step_ptr != NULL, "Invalid parameter");

                   /* Check if the list file is available: */
	Assert (list_file != NULL, "No list file");

                   /* Check if package has been initiated: */
	if (!initiated)
	{
		initiate ();

		initiated = TRUE;
	}

                   /* Check against service_identification filter: */
	if (service_identification_defined)
	{
		if (step_ptr->service_identification.present)
		{
			if (step_ptr->service_identification.value !=
                service_identification)
			{
				goto done;
			}
		}
	}

	memset (txt, 0, 257);

                   /* List frame time: */
	list_ftm (e_object_step, step_ptr, 256, txt);

                   /* List last update, message or frame time: */
	list_tod (e_object_step, step_ptr, 256, txt);

                   /* List sensor number: */
	strncat (txt, " --", 256);

                   /* List sensor name: */
	strncat (txt, " ----", 256);

                   /* List board/line number: */
	if (handle_multiple_lines)
	{
		strncat (txt, " ---", 256);
	}

                   /* List sensor data source identification: */
	list_dsi (e_object_step, step_ptr, 256, txt);

                   /* List batch number: */
	if (step_ptr->batch_number.present)
	{
		memset (tmp, 0, 81);
		sprintf (tmp, " batch=%2u", step_ptr->batch_number.value);
		strncat (txt, tmp, 256);
	}

	if (list_service_id && step_ptr->service_identification.present)
	{
		memset (tmp, 0, 81);
		sprintf (tmp, " service=" M_FMT_BYTE,
                      step_ptr->service_identification.value);
		strncat (txt, tmp, 256);
	}

	list_text (3, "%s", txt);
	list_text (3, "\n");

	done:          /* We are done */
	return;
}
#endif /* LISTER */

#if LISTER
/*----------------------------------------------------------------------------*/
/* list_strk       -- List system track information                           */
/*----------------------------------------------------------------------------*/

 void list_strk (t_Strk *strk_ptr)
{
	t_Secs tm;     /* Time (of day); seconds */
	t_Bool to_be_listed;
	static char txt[1024];

				   /* Check parameters: */
	Assert (strk_ptr != NULL, "Invalid parameter");

                   /* Check if the list file is available: */
	Assert (list_file != NULL, "No list file");

                   /* Check if package has been initiated: */
	if (!initiated)
	{
		initiate ();

		initiated = TRUE;
	}

                   /* Check against filter for service_identification: */
	if (service_identification_defined)
	{
		if (strk_ptr->service_identification.present)
		{
			if (strk_ptr->service_identification.value !=
                service_identification)
			{
				goto done;
			}
		}
	}

                   /* Check against start offset: */
	if (start_offset_defined && input_offset < start_offset)
	{
		goto done;
	}

                   /* Extract time: */
	tm = 0.0;
	if (strk_ptr->time_of_last_update.present)
	{
		tm = strk_ptr->time_of_last_update.value;
	}
	else if (strk_ptr->frame_time.present)
	{
		tm = strk_ptr->frame_time.value;
	}

                   /* Adapt by time bias, if any: */
	if (time_bias_defined)
	{
		tm += time_bias;
	}

	if (excel_output && exc_file != NULL)
	{
		char *p;
		t_Secs tm;
		char tmp[21];
		char *txt;

		if (strk_ptr->time_of_last_update.present)
		{
			tm = strk_ptr->time_of_last_update.value;
			if (time_bias_defined)
			{
				tm += time_bias;
			}

			txt = utc_text (tm);
			memset (tmp, 0, 21);
			strcat (tmp, txt);

			p = tmp;
			while (*p)
			{
				if (*p == '.')
				{
					*p = '\0';
					break;
				}
				++ p;
			}

			fprintf (exc_file, "%s", tmp);
		}
		else
		{
			fprintf (exc_file, "--:--:--");
		}

		if (strk_ptr->calculated_rate_of_climb_descent.present)
		{
			t_Real rocd;

			rocd = (60.0 / M_FT2MTR) *
                   strk_ptr->calculated_rate_of_climb_descent.value;

			memset (tmp, 0, 21);
			sprintf (tmp, "%8.2f", rocd);

			p = tmp;
			while (*p)
			{
				if (*p == '.')
				{
					*p = ',';
				}
				++ p;
			}
			
			fprintf (exc_file, "\t%s", tmp);
		}
		else
		{
			fprintf (exc_file, "\t");
		}

		if (strk_ptr->track_number.present)
		{
			fprintf (exc_file, "\t#%04hu", strk_ptr->track_number.value);
		}
		else
		{
			fprintf (exc_file, "\t#----");
		}

		fprintf (exc_file, "\n");
	}

                   /* Determine whether this system track is 
                      to be listed: */

	to_be_listed = FALSE;
	if (1 <= list_level && list_level <= 3)
	{
		to_be_listed = TRUE;
	}
	if (strk_ptr->data_format == e_data_format_asterix)
	{
		t_Byte cat;

		cat = strk_ptr->asterix_category;

		if (cat_selection_defined)
		{
			if (!cat_selection[cat])
			{
				to_be_listed = FALSE;
			}
		}
	}
	if (list_only_background_service)
	{
		if (strk_ptr->track_status_present &&
            strk_ptr->background_service == e_is_false)
		{
			to_be_listed = FALSE;
		}
	}
	if (strk_ptr->data_format == e_data_format_asterix &&
        strk_ptr->asterix_category == 62 &&
        strk_ptr->track_status_present &&
        strk_ptr->background_service == e_is_false)
	{
		to_be_listed = list_only_complementary_service;
	}
	if (list_wgs84)
	{
		to_be_listed = FALSE;
		if (strk_ptr->calculated_wgs84_position.present)
		{
			t_Real glat;
			t_Real glon;

			glat = strk_ptr->calculated_wgs84_position.value_latitude;
			glon = strk_ptr->calculated_wgs84_position.value_longitude;

			glat *= M_RAD2DEG;
			glon *= M_RAD2DEG;

			if (46.0 <= glat && glat <= 50.0 &&
                9.0 <= glon && glon <= 18.0)
			{
				to_be_listed = TRUE;
			}
		}
	}

	strk_ptr->has_been_listed = FALSE;
	strk_ptr->to_be_listed = to_be_listed;

                   /* List system track information: */

	if (to_be_listed)
	{
		memset (txt, 0, 1024);

		list_strk (strk_ptr, 1022, txt);

		list_text (3, "%s", txt);
		list_text (3, "\n");
	}

	done:          /* We are done */
	return;
}
#endif /* LISTER */

#if LISTER
/*----------------------------------------------------------------------------*/
/* list_strk       -- List system track information                           */
/*----------------------------------------------------------------------------*/

 void list_strk (t_Strk *strk_ptr, t_Ui16 length, char *buffer)
{
	t_Bool baro_alt_listed;
	t_Bool geom_alt_listed;
	t_Real gv_hdg;
	t_Bool gv_present;
	t_Real gv_spd;
	int hgt_src;
	t_Bool large_hgt_diff;
	char tmp[81];

				   /* Check parameters: */
	Assert (strk_ptr != NULL, "NULL pointer");
	Assert (length > 0, "Invalid parameter");
	Assert (buffer != NULL, "NULL pointer");

                   /* Check if package has been initiated: */
	if (!initiated)
	{
		initiate ();

		initiated = TRUE;
	}

                   /* Clear write buffer: */
	memset (buffer, 0, length);

                   /* List system track information: */

                   /* List frame time: */
	list_ftm (e_object_strk, strk_ptr, length, buffer);

                   /* List last update, message or frame time: */
	list_tod (e_object_strk, strk_ptr, length, buffer);

                   /* List sensor number: */
	list_sno (e_object_strk, strk_ptr, length, buffer);

                   /* List board/line number: */
	if (handle_multiple_lines)
	{
		strncat (buffer, " ---", length);
	}

                   /* List sensor data source identification: */
	list_dsi (e_object_strk, strk_ptr, length, buffer);

                   /* List antenna number: */
	list_ano (e_object_strk, strk_ptr, length, buffer);

                   /* List detection type: */
	list_mtp (e_object_strk, strk_ptr, length, buffer);

                   /* List system track number: */
	list_num (e_object_strk, strk_ptr, length, buffer);

                   /* List position: */
	list_pos (e_object_strk, strk_ptr, length, buffer);

                   /* List SSR mode S address (ICAO technical address): */
	list_msa (e_object_strk, strk_ptr, length, buffer);

                   /* List aircraft identification: */
	list_aid (e_object_strk, strk_ptr, length, buffer);

                   /* List SSR identification code: */
	list_idt (e_object_strk, strk_ptr, length, buffer);

                   /* Height (altitude): */
	baro_alt_listed = FALSE;
	geom_alt_listed = FALSE;
	hgt_src = 0;
	if (strk_ptr->most_reliable_height_present)
	{
		if (strk_ptr->most_reliable_height == 0)
		{
			if (strk_ptr->measured_track_mode_c_height.present)
			{
				hgt_src = 2;
			}
		}
		else
		{
			if (strk_ptr->calculated_track_altitude.present)
			{
				hgt_src = 3;
			}
		}
	}
	else if (strk_ptr->measured_track_mode_c_height.present)
	{
		hgt_src = 2;
	}
	else if (strk_ptr->calculated_track_altitude.present)
	{
		hgt_src = 3;
	}
	else if (strk_ptr->measured_information.mdc_present)
	{
		hgt_src = 1;
	}

	if (hgt_src == 0)
	{
		if (strk_ptr->measured_track_mode_c_height.present)
		{
			hgt_src = 2;
		}
	}

	memset (tmp, 0, 81);
	if (hgt_src == 2)
	{
		t_Real fl;

		if (strk_ptr->measured_track_mode_c_height.present)
		{
			fl = 0.01 * strk_ptr->measured_track_mode_c_height.value_in_feet;

			if (strk_ptr->measured_track_mode_c_height.value_in_feet % 100)
			{
				sprintf (tmp, "C:%7.2f --", fl);
			}
			else
			{
				sprintf (tmp, "C:%4d    --", rounds (fl));
			}
		}
		else
		{
			strncat (tmp, "C: ---    --", 80);
		}

		baro_alt_listed = TRUE;
	}
	else if (hgt_src == 3)
	{
		t_Real fl;

		if (strk_ptr->calculated_track_altitude.present)
		{
			fl = (1.0 / 30.48) * strk_ptr->calculated_track_altitude.value;

			if (fl < -12.0 || fl > 1267.0)
			{
				strncat (tmp, "G: NVA    --", 80);
			}
			else
			{
				sprintf (tmp, "G:%7.2f --", fl);
			}
		}
		else
		{
			strncat (tmp, "G:----.-- --", 80);
		}

		geom_alt_listed = TRUE;
	}
	else if (hgt_src == 1)
	{
		t_Si16 fl;

		fl = strk_ptr->measured_information.value_mdc / 4;

		if (fl < -12 || fl > 1267)
		{
			strncat (tmp, "c: NVA --", 80);
		}
		else
		{
			sprintf (tmp, "c:%4hd", fl);
			if (strk_ptr->measured_information.value_mdc_garbled)
			{
				strncat (tmp, " g", 80);
			}
			else
			{
				strncat (tmp, " -", 80);
			}
			if (strk_ptr->measured_information.value_mdc_invalid)
			{
				strncat (tmp, "i", 80);
			}
			else
			{
				strncat (tmp, "-", 80);
			}
		}
	}
	else if (strk_ptr->observed_by_ssr)
	{
		strncat (tmp, "-: ---    --", 80);
	}
	else
	{
		strncat (tmp, "            ", 80);
	}

	strncat (buffer, " ", length);
	strncat (buffer, tmp, length);

	large_hgt_diff = FALSE;
	if (strk_ptr->calculated_track_flight_level.present &&
        strk_ptr->measured_track_mode_c_height.present)
	{
		t_Real dhgt;

		dhgt = 0.25 * strk_ptr->calculated_track_flight_level.value -
               0.25 * strk_ptr->measured_track_mode_c_height.value;

		if (fabs (dhgt) > 5.0)
		{
			large_hgt_diff = TRUE;
		}
	}

	gv_hdg = 0.0;
	gv_present = FALSE;
	gv_spd = 0.0;

	if (strk_ptr->calculated_polar_velocity_present)
	{
		gv_present = TRUE;

		gv_spd = strk_ptr->calculated_track_speed;
		gv_hdg = strk_ptr->calculated_track_heading;
	}
	else if (strk_ptr->calculated_cartesian_velocity.present)
	{
		t_Real vx, vy;

		vx = strk_ptr->calculated_cartesian_velocity.value_vx;
		vy = strk_ptr->calculated_cartesian_velocity.value_vy;

		gv_present = TRUE;

		gv_spd = distance (vx, vy);
		gv_hdg = M_DEG2RAD * azimuth (vx, vy);
	}
	else if (strk_ptr->ground_vector.present)
	{
		gv_present = TRUE;

		gv_spd = strk_ptr->ground_vector.value_gsp;
		gv_hdg = strk_ptr->ground_vector.value_hdg;
	}

	strncat (buffer, " ;", length);

	if (strk_ptr->acas_resolution_advisory_report.present)
	{
		strncat (buffer, " acas", length);
	}

	if (strk_ptr->track_terminated == e_is_true)
	{
		strncat (buffer, " cancel", length);
	}

	if (strk_ptr->track_created == e_is_true)
	{
		strncat (buffer, " create", length);
	}

	if (strk_ptr->mode_2_info.present)
	{
		sprintf (tmp, " 2:0%04o", strk_ptr->mode_2_info.code);
		if (strk_ptr->mode_2_info.code_garbled)
		{
			strncat (tmp, "g", 80);
		}
		else
		{
			strncat (tmp, "-", 80);
		}
		if (strk_ptr->mode_2_info.code_invalid) 
		{
			strncat (tmp, "i", 80);
		}
		else
		{
			strncat (tmp, "-", 80);
		}
		if (strk_ptr->mode_2_info.code_changed)
		{
			strncat (tmp, "c", 80);
		}
		else
		{
			strncat (tmp, "-", 80);
		}
	}

	if (strk_ptr->possible_split_pair == e_is_true)
	{
		int qf;

		strncat (buffer, " fta(split-pair", length);

		qf = strk_ptr->split_qualification_factor;

		memset (tmp, 0, 81);
		sprintf (tmp, ":%d", qf);
		strncat (buffer, tmp, length);

		strncat (buffer, ")", length);
	}
	if (strk_ptr->possible_split_parent == e_is_true)
	{
		strncat (buffer, " fta(p-split)", length);
	}
	if (strk_ptr->possible_split_track == e_is_true)
	{
		strncat (buffer, " fta(split)", length);
	}

	if (list_ground_vector && gv_present)
	{
		memset (tmp, 0, 81);

		sprintf (tmp, " gsp=%.2f [kts] hdg=%.3f [deg]",
                      M_MPS2KTS * gv_spd, M_RAD2DEG * gv_hdg);
		strncat (buffer, tmp, length);
	}

	if (strk_ptr->assigned_code_conflict == e_is_true)
	{
		strncat (buffer, " aac", length);
	}

	if (strk_ptr->adsb_blunder_detected == e_is_true)
	{
		strncat (buffer, " blunder", length);
	}

	if (!baro_alt_listed && strk_ptr->measured_track_mode_c_height.present)
	{
		t_Real fl;

		memset (tmp, 0, 81);

		fl = 0.01 * strk_ptr->measured_track_mode_c_height.value_in_feet;

		if (strk_ptr->measured_track_mode_c_height.value_in_feet % 100)
		{
			sprintf (tmp, " C:%.2f", fl);
		}
		else
		{
			sprintf (tmp, " C:%d", rounds (fl));
		}

		strncat (buffer, tmp, length);
	}

	if (strk_ptr->flight_plan_correlated == e_is_true)
	{
		strncat (buffer, " cor", length);
	}

	if (list_only_complementary_service &&
        strk_ptr->data_format == e_data_format_asterix &&
        strk_ptr->asterix_category == 62 &&
        strk_ptr->track_status_present &&
        strk_ptr->background_service == e_is_false)
	{
		strncat (buffer, " cy", length);
	}

	if (!geom_alt_listed && strk_ptr->calculated_track_altitude.present)
	{
		t_Real fl;

		memset (tmp, 0, 81);

		if (strk_ptr->calculated_track_altitude.present)
		{
			fl = (1.0 / 30.48) * strk_ptr->calculated_track_altitude.value;

			if (fl < -12.0 || fl > 1267.0)
			{
				strncat (tmp, " G:NVA", 80);
			}
			else
			{
				sprintf (tmp, " G:%.2f", fl);
			}
		}

		strncat (buffer, tmp, length);
	}

	if (list_large_height_difference && large_hgt_diff)
	{
		t_Real cfl;
		t_Real tmc;

		cfl = M_FT2MTR * strk_ptr->calculated_track_flight_level.value_in_feet;
		tmc = M_FT2MTR * strk_ptr->measured_track_mode_c_height.value_in_feet;

		strncat (buffer, " large_hgt_diff", length);
		strncat (buffer, "(cfl=", length);
		memset (tmp, 0, 81);
		sprintf (tmp, "%.2f", cfl);
		strncat (buffer, tmp, length);
		strncat (buffer, "; tmc=", length);
		memset (tmp, 0, 81);
		sprintf (tmp, "%.2f", tmc);
		strncat (buffer, tmp, length);
		strncat (buffer, " FL)", length);
	}

	if (strk_ptr->manoeuvring_track == e_is_true)
	{
		strncat (buffer, " mnv", length);
	}

	if (strk_ptr->military_emergency == e_is_true)
	{
		strncat (buffer, " mem", length);
	}

	if (strk_ptr->miss_update && strk_ptr->track_terminated != e_is_true)
	{
		strncat (buffer, " miss-upd", length);
	}

	if (list_mof && strk_ptr->mode_of_flight.present)
	{
		t_Byte b;

		strncat (buffer, " mof=", length);
		b = strk_ptr->mode_of_flight.value_longitudinal;
		if (b == 0)
		{
			strncat (buffer, "cs", length);
		}
		else if (b == 1)
		{
			strncat (buffer, "is", length);
		}
		else if (b == 2)
		{
			strncat (buffer, "ds", length);
		}
		else
		{
			strncat (buffer, "??", length);
		}
		strncat (buffer, "+", length);
		b = strk_ptr->mode_of_flight.value_transversal;
		if (b == 0)
		{
			strncat (buffer, "cc", length);
		}
		else if (b == 1)
		{
			strncat (buffer, "rt", length);
		}
		else if (b == 2)
		{
			strncat (buffer, "lt", length);
		}
		else
		{
			strncat (buffer, "??", length);
		}
		strncat (buffer, "+", length);
		b = strk_ptr->mode_of_flight.value_vertical;
		if (b == 0)
		{
			strncat (buffer, "lf", length);
		}
		else if (b == 1)
		{
			strncat (buffer, "cl", length);
		}
		else if (b == 2)
		{
			strncat (buffer, "de", length);
		}
		else
		{
			strncat (buffer, "??", length);
		}
	}

	if (strk_ptr->ghost_track == e_is_true)
	{
		strncat (buffer, " rfl", length);
	}

	if (list_service_id && strk_ptr->service_identification.present)
	{
		memset (tmp, 0, 81);
		sprintf (tmp, " service=" M_FMT_BYTE,
                      strk_ptr->service_identification.value);
		strncat (buffer, tmp, length);
	}

	if (strk_ptr->simulated == e_is_true)
	{
		strncat (buffer, " sim", length);
	}

	if (strk_ptr->special_position_indication == e_is_true)
	{
		strncat (buffer, " spi", length);
	}

	if (strk_ptr->tentative_track == e_is_true)
	{
		strncat (buffer, " tnt", length);
	}

	return;
}
#endif /* LISTER */

#if LISTER
/*----------------------------------------------------------------------------*/
/* list_text       -- List some text                                          */
/*----------------------------------------------------------------------------*/

 void list_text (int level, const char *format, ...)
{
	va_list ptr;   /* Pointer to variable arguments */

                   /* Check parameters: */
	Assert (format != NULL, "Invalid parameter");

                   /* Check against start offset: */
	if (start_offset_defined && input_offset < start_offset)
	{
		goto done;
	}

                   /* Check against current list level: */
	if (level != -1 && !list_all_levels && level != list_level)
	{
		goto done;
	}

                   /* Check for list file: */
	if (list_file == NULL)
	{
		goto done;
	}

                   /* Check format string: */
	if (*format == '\0')
	{
		goto done;
	}

                   /* Write-out text to the list file: */
	va_start (ptr, format);
	vfprintf (list_file, format, ptr);
	va_end (ptr);

	done:          /* We are done */
	return;
}
#endif /* LISTER */

#if LISTER
/*----------------------------------------------------------------------------*/
/* term_lister     -- Terminate the lister                                    */
/*----------------------------------------------------------------------------*/

 void term_lister (void)
{
	if (excel_output)
	{
		if (exc_file != NULL)
		{
			fclose (exc_file);
			exc_file = NULL;
		}
	}

	return;
}
#endif /* LISTER */
/* end-of-file */
