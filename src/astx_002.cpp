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
/* File:           src/astx_002.cpp                                           */
/* Contents:       Process ASTERIX category 002 data block                    */
/* Author(s):      kb                                                         */
/* Last change:    2018-04-27                                                 */
/*----------------------------------------------------------------------------*/

/* Reference document:
   -------------------

   Eurocontrol Standard Document for Surveillance Data Exchange.
   Part 2b: Transmission of Monoradar Service Messages.
   SUR.ET1.ST05.2000-STD-02b-01
   Edition: 1.0
   Edition Date: November 1997
   Status: Released Issue
*/

#include "basics.h"
                   /* Basic system definitions */
#include "common.h"
                   /* Common declarations and definitions */

#include <stddef.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

				   /* Local macros: */
				   /* ------------- */

#define M_MAX_FRN 14
				   /* Maximum field reference number */
#define M_MAX_FSPEC_LENGTH 2
				   /* Maximum fields specification length
					  for ASTERIX category 002 */

				   /* Local variables: */
				   /* ---------------- */

static t_Data_Item_Desc desc_i002_000;
                   /* Description of data item I002/000 */
static t_Data_Item_Desc desc_i002_010;
                   /* Description of data item I002/010 */
static t_Data_Item_Desc desc_i002_020;
                   /* Description of data item I002/020 */
static t_Data_Item_Desc desc_i002_030;
                   /* Description of data item I002/030 */
static t_Data_Item_Desc desc_i002_041;
                   /* Description of data item I002/041 */
static t_Data_Item_Desc desc_i002_050;
                   /* Description of data item I002/050 */
static t_Data_Item_Desc desc_i002_060;
                   /* Description of data item I002/060 */
static t_Data_Item_Desc desc_i002_070;
                   /* Description of data item I002/070 */
static t_Data_Item_Desc desc_i002_080;
                   /* Description of data item I002/080 */
static t_Data_Item_Desc desc_i002_090;
                   /* Description of data item I002/090 */
static t_Data_Item_Desc desc_i002_100;
                   /* Description of data item I002/100 */
static t_Data_Item_Desc desc_i002_rfs;
                   /* Description of RFS data item */
static t_Data_Item_Desc desc_i002_spf;
                   /* Description of SPF data item */
static t_Bool initiated = FALSE;
				   /* Package initiated flag */
static t_Data_Item_Desc *std_uap[M_MAX_FRN + 1];
				   /* Standard User Application Profile */

				   /* Local functions: */
				   /* ---------------- */

static void init_desc (void);
                   /* Initiate data item descriptions */
static void load_std_uap (void);
                   /* Load standard User Application Profile (UAP) */
static t_Retc proc_i002_000 (t_Ui16 length, t_Byte *buffer);
                   /* Process I002/000 data item */
static t_Retc proc_i002_010 (t_Ui16 length, t_Byte *buffer);
                   /* Process I002/010 data item */
static t_Retc proc_i002_020 (t_Ui16 length, t_Byte *buffer);
                   /* Process I002/020 data item */
static t_Retc proc_i002_030 (t_Ui16 length, t_Byte *buffer);
                   /* Process I002/030 data item */
static t_Retc proc_i002_041 (t_Ui16 length, t_Byte *buffer);
                   /* Process I002/041 data item */
static t_Retc proc_i002_050 (t_Ui16 length, t_Byte *buffer);
                   /* Process I002/050 data item */
static t_Retc proc_i002_060 (t_Ui16 length, t_Byte *buffer);
                   /* Process I002/060 data item */
static t_Retc proc_i002_070 (t_Ui16 length, t_Byte *buffer);
                   /* Process I002/070 data item */
static t_Retc proc_i002_080 (t_Ui16 length, t_Byte *buffer);
                   /* Process I002/080 data item */
static t_Retc proc_i002_090 (t_Ui16 length, t_Byte *buffer);
                   /* Process I002/090 data item */
static t_Retc proc_i002_100 (t_Ui16 length, t_Byte *buffer);
                   /* Process I002/100 data item */
static t_Retc proc_i002_rfs (t_Ui16 length, t_Byte *buffer, t_Ui16 *pos_ptr);
                   /* Process RFS indicator data item */
static t_Retc proc_i002_spf (t_Ui16 length, t_Byte *buffer, t_Ui16 *pos_ptr);
                   /* Process SPF indicator data item */

/*----------------------------------------------------------------------------*/
/* astx_002        -- Process ASTERIX category 002 data block                 */
/*----------------------------------------------------------------------------*/

 t_Retc astx_002 (t_Ui16 length, t_Byte *buffer)
{
	t_Byte b;      /* Auxiliary */
	t_Bool bit_set;
                   /* Some data item bit set */
	int frn;       /* Field reference number */
	t_Byte fspec_bits;
                   /* Byte within fields specification */
	t_Byte fspec_buffer[M_MAX_FSPEC_LENGTH];
				   /* Fields specification */
	t_Ui16 fspec_length;
				   /* Length of fields specification */
	int ix;        /* Auxiliary */
	t_Retc lrc;    /* Local return code */
	t_Byte msk;    /* Bit mask */
	t_Ui16 pos;    /* Position within data block */
	t_Retc ret;    /* Return code */

				   /* Preset the return code: */
	ret = RC_FAIL;

				   /* Check parameters: */
	Assert (length > 0, "Invalid parameter");
	Assert (buffer != NULL, "Invalid parameter");

				   /* Initiate package if not yet done: */
	if (!initiated)
	{
		init_desc ();
		load_std_uap ();

		initiated = TRUE;
	}

				   /* Preset position: */
	pos = 0;

				   /* Process along ... */
	while (pos < length)
	{
                   /* Increment number of records in this frame: */
		++ records_in_current_frame;

				   /* Clear fields specification: */
		memset (fspec_buffer, 0, M_MAX_FSPEC_LENGTH);
		fspec_length = 0;

				   /* Extract fields specification: */

				   /* Get first byte of fields specification: */
		b = buffer[pos];
		++ pos;

				   /* Enter into fields specification: */
		fspec_buffer[fspec_length] = b;
		++ fspec_length;

				   /* Check for field extension: */
		while (b & 0x01)
		{
				   /* Get next byte of fields specification: */
			b = buffer[pos];
			++ pos;

                   /* Check against buffer overrun: */
			if (fspec_length >= M_MAX_FSPEC_LENGTH)
			{
				error_msg ("Fields specification too long");
				goto done;
			}

				   /* Enter into fields specification: */
			fspec_buffer[fspec_length] = b;
			++ fspec_length;
		}

#if LISTER
				   /* List fields specification: */
		list_text (1, "; FSPEC: 0x");
		for (ix = 0; ix < fspec_length; ix ++)
		{
			list_text (1, " " M_FMT_BYTE_HEX, fspec_buffer[ix]);
		}
		list_text (1, "\n");

		list_text (2, "; Data Record:\n");
#endif /* LISTER */

                   /* Preset radar service information: */
		memset (&rsrv, 0, sizeof (rsrv));

                   /* Set data format: */
		rsrv.asterix_category = 2;
		rsrv.data_format = e_data_format_asterix;

				   /* Decode fields specification according to standard
					  User Application Profile: */

		bit_set = FALSE;
		frn = 1;
		for (ix = 0; ix < fspec_length; ix ++)
		{
			fspec_bits = fspec_buffer[ix];
			msk = 0x80;

			while (msk != 0x01)
			{
				if (fspec_bits & msk)
				{
					if (frn > M_MAX_FRN)
					{
						error_msg ("FRN too large");
						goto done;
					}

					bit_set = TRUE;

					lrc = data_item (2, frn, std_uap[frn],
                                     length, buffer, &pos);
					if (lrc != RC_OKAY)
					{
						error_msg ("Invalid data item");
						goto done;
					}
				}

				++ frn;
				msk >>= 1;
			}
		}

                   /* Check if any data item bit was set: */
        if (!bit_set)
        {
            error_msg ("Empty ASTERIX record");
            goto done;
        }

                   /* Add frame date, if available: */
        if (frame_date_present)
        {
            rsrv.frame_date.present = TRUE;
            rsrv.frame_date.value = frame_date;
        }

                   /* Add frame time, if available: */
        if (frame_time_present)
        {
            rsrv.frame_time.present = TRUE;
            rsrv.frame_time.value = frame_time;
        }

                   /* Add board/line number, if present: */
		if (current_line_number_defined)
		{
			rsrv.line_number = current_line_number;
		}

                   /* Process this radar service message: */
		lrc = process_rsrv (&rsrv);
		if (lrc != RC_OKAY && lrc != RC_SKIP)
		{
			ret = lrc;
			goto done;
		}
	}

				   /* Set the return code: */
	ret = RC_OKAY;

	done:          /* We are done */
	return ret;
}

/*----------------------------------------------------------------------------*/
/* init_desc       -- Initiate data item descriptions                         */
/*----------------------------------------------------------------------------*/

 static
 void init_desc (void)
{
                   /* Description of data item I002/000: */
    desc_i002_000.category = 2;
    desc_i002_000.data_item = 0;
    desc_i002_000.item_type = e_fixed_length_data_item;
    desc_i002_000.fixed_length = 1;
    desc_i002_000.proc_fptr = proc_i002_000;
    desc_i002_000.read_fptr = NULL;

                   /* Description of data item I002/010: */
    desc_i002_010.category = 2;
    desc_i002_010.data_item = 10;
    desc_i002_010.item_type = e_fixed_length_data_item;
    desc_i002_010.fixed_length = 2;
    desc_i002_010.proc_fptr = proc_i002_010;
    desc_i002_010.read_fptr = NULL;

                   /* Description of data item I002/020: */
    desc_i002_020.category = 2;
    desc_i002_020.data_item = 20;
    desc_i002_020.item_type = e_fixed_length_data_item;
    desc_i002_020.fixed_length = 1;
    desc_i002_020.proc_fptr = proc_i002_020;
    desc_i002_020.read_fptr = NULL;

                   /* Description of data item I002/030: */
    desc_i002_030.category = 2;
    desc_i002_030.data_item = 30;
    desc_i002_030.item_type = e_fixed_length_data_item;
    desc_i002_030.fixed_length = 3;
    desc_i002_030.proc_fptr = proc_i002_030;
    desc_i002_030.read_fptr = NULL;

                   /* Description of data item I002/041: */
    desc_i002_041.category = 2;
    desc_i002_041.data_item = 41;
    desc_i002_041.item_type = e_fixed_length_data_item;
    desc_i002_041.fixed_length = 2;
    desc_i002_041.proc_fptr = proc_i002_041;
    desc_i002_041.read_fptr = NULL;

                   /* Description of data item I002/050: */
    desc_i002_050.category = 2;
    desc_i002_050.data_item = 50;
    desc_i002_050.item_type = e_variable_length_data_item;
    desc_i002_050.fixed_length = 0;
    desc_i002_050.proc_fptr = proc_i002_050;
    desc_i002_050.read_fptr = NULL;

                   /* Description of data item I002/060: */
    desc_i002_060.category = 2;
    desc_i002_060.data_item = 60;
    desc_i002_060.item_type = e_variable_length_data_item;
    desc_i002_060.fixed_length = 0;
    desc_i002_060.proc_fptr = proc_i002_060;
    desc_i002_060.read_fptr = NULL;

                   /* Description of data item I002/070: */
    desc_i002_070.category = 2;
    desc_i002_070.data_item = 70;
    desc_i002_070.item_type = e_repetitive_data_item;
    desc_i002_070.fixed_length = 2;
    desc_i002_070.proc_fptr = proc_i002_070;
    desc_i002_070.read_fptr = NULL;

                   /* Description of data item I002/080: */
    desc_i002_080.category = 2;
    desc_i002_080.data_item = 80;
    desc_i002_080.item_type = e_variable_length_data_item;
    desc_i002_080.fixed_length = 0;
    desc_i002_080.proc_fptr = proc_i002_080;
    desc_i002_080.read_fptr = NULL;

                   /* Description of data item I002/090: */
    desc_i002_090.category = 2;
    desc_i002_090.data_item = 90;
    desc_i002_090.item_type = e_fixed_length_data_item;
    desc_i002_090.fixed_length = 2;
    desc_i002_090.proc_fptr = proc_i002_090;
    desc_i002_090.read_fptr = NULL;

                   /* Description of data item I002/100: */
    desc_i002_100.category = 2;
    desc_i002_100.data_item = 100;
    desc_i002_100.item_type = e_fixed_length_data_item;
    desc_i002_100.fixed_length = 8;
    desc_i002_100.proc_fptr = proc_i002_100;
    desc_i002_100.read_fptr = NULL;

                   /* Description of RFS data item: */
    desc_i002_rfs.category = 2;
    desc_i002_rfs.data_item = M_RFS_INDICATOR;
    desc_i002_rfs.item_type = e_immediate_data_item;
    desc_i002_rfs.fixed_length = 0;
    desc_i002_rfs.proc_fptr = NULL;
    desc_i002_rfs.read_fptr = proc_i002_rfs;

                   /* Description of SPF data item: */
    desc_i002_spf.category = 2;
    desc_i002_spf.data_item = M_SPF_INDICATOR;
    desc_i002_spf.item_type = e_immediate_data_item;
    desc_i002_spf.fixed_length = 0;
    desc_i002_spf.proc_fptr = NULL;
    desc_i002_spf.read_fptr = proc_i002_spf;

	return;
}

/*----------------------------------------------------------------------------*/
/* load_std_uap    -- Load standard User Application Profile (UAP)            */
/*----------------------------------------------------------------------------*/

 static
 void load_std_uap (void)
{
	int ix;

	for (ix = 0; ix <= M_MAX_FRN; ix ++)
	{
		std_uap[ix] = NULL;
	}

	std_uap[1] = &desc_i002_010;
	std_uap[2] = &desc_i002_000;
	std_uap[3] = &desc_i002_020;
	std_uap[4] = &desc_i002_030;
	std_uap[5] = &desc_i002_041;
	std_uap[6] = &desc_i002_050;
	std_uap[7] = &desc_i002_060;

	std_uap[8] = &desc_i002_070;
	std_uap[9] = &desc_i002_100;
	std_uap[10] = &desc_i002_090;
	std_uap[11] = &desc_i002_080;
	std_uap[13] = &desc_i002_spf;
	std_uap[14] = &desc_i002_rfs;

	return;
}

/*----------------------------------------------------------------------------*/
/* proc_i002_000   -- Process I002/000 data item                              */
/*----------------------------------------------------------------------------*/

 static
 t_Retc proc_i002_000 (t_Ui16 length, t_Byte *buffer)
{
	t_Byte df1;    /* Data field octet 1 */
	t_Byte mtp;    /* Message type */
	t_Retc ret;    /* Return code */

				   /* Preset the return code: */
	ret = RC_FAIL;

				   /* Check parameters: */
	Assert (length == 1, "Invalid parameter");
	Assert (buffer != NULL, "Invalid parameter");

				   /* Extract octets: */
	df1 = buffer[0];

				   /* Extract information: */
	mtp = df1;

#if LISTER
				   /* List data field: */
	list_text (2, ";  Message Type:");
	list_text (2, " mtp=" M_FMT_BYTE, mtp);
	if (mtp == 1)
	{
		list_text (2, " (North marker message)");
	}
	else if (mtp == 2)
	{
		list_text (2, " (Sector crossing message)");
	}
	else if (mtp == 3)
	{
		list_text (2, " (South marker message)");
	}
	else if (mtp == 8)
	{
		list_text (2, " (Activation of blind zone filtering)");
	}
	else if (mtp == 9)
	{
		list_text (2, " (Stop of blind zone filtering)");
	}
	list_text (2, "\n");
#endif /* LISTER */

				   /* Store this information: */
	rsrv.rsrv_type = e_rsrv_undefined;
	if (mtp == 1)
	{
		rsrv.rsrv_type = e_rsrv_north_marker;

		rsrv.antenna_azimuth.present = TRUE;
		rsrv.antenna_azimuth.value = 0.0;
	}
	else if (mtp == 2)
	{
		rsrv.rsrv_type = e_rsrv_sector_crossing;
	}
	else if (mtp == 3)
	{
		rsrv.rsrv_type = e_rsrv_south_marker;

		rsrv.antenna_azimuth.present = TRUE;
		rsrv.antenna_azimuth.value = M_PI;
	}
	else if (mtp == 8)
	{
		rsrv.rsrv_type = e_rsrv_filtering_zone;
		rsrv.activate_filtering_zone = TRUE;
	}
	else if (mtp == 9)
	{
		rsrv.rsrv_type = e_rsrv_filtering_zone;
		rsrv.deactivate_filtering_zone = TRUE;
	}

				   /* Set the return code: */
	ret = RC_OKAY;

	return ret;
}

/*----------------------------------------------------------------------------*/
/* proc_i002_010   -- Process I002/010 data item                              */
/*----------------------------------------------------------------------------*/

 static
 t_Retc proc_i002_010 (t_Ui16 length, t_Byte *buffer)
{
	t_Byte df1;    /* Data field octet 1 */
	t_Byte df2;    /* Data field octet 2 */
	t_Ui16 dsi;    /* Data source identifier */
	t_Retc ret;    /* Return code */
#if LISTER
	t_Byte sac;    /* Source area code */
	t_Byte sic;    /* Source identification code */
#endif /* LISTER */

				   /* Preset the return code: */
	ret = RC_FAIL;

				   /* Check parameters: */
	Assert (length == 2, "Invalid parameter");
	Assert (buffer != NULL, "Invalid parameter");

				   /* Extract octets: */
	df1 = buffer[0];
	df2 = buffer[1];

				   /* Extract information: */
#if LISTER
	sac = df1;
	sic = df2;
#endif /* LISTER */
	dsi = make_ui16 (df1, df2);

#if LISTER
				   /* List data field: */
	list_text (2, ";  Data Source Identifier:");
	list_text (2, " 0x" M_FMT_UI16_HEX, dsi);
	list_text (2, " (SAC=" M_FMT_BYTE "; SIC=" M_FMT_BYTE ")", sac, sic);
	list_text (2, "\n");
#endif /* LISTER */

                   /* Remember this information: */
	last_sacsic_available = TRUE;
	last_sacsic = dsi;
                   /* This is a feature specific to ASTERIX category 001
                      where radar target reports without SAC/SIC will
                      inherit this information from a preceding radar
                      service message with SAC/SIC
                   */

				   /* Store this information: */
	rsrv.data_source_identifier.present = TRUE;
	rsrv.data_source_identifier.value = dsi;
    rsrv.data_source_identifier.sac = df1;
    rsrv.data_source_identifier.sic = df2;


				   /* Set the return code: */
	ret = RC_OKAY;

	return ret;
}

/*----------------------------------------------------------------------------*/
/* proc_i002_020   -- Process I002/020 data item                              */
/*----------------------------------------------------------------------------*/

 static
 t_Retc proc_i002_020 (t_Ui16 length, t_Byte *buffer)
{
	t_Real azf;    /* Sector angle; degrees */
	t_Byte df1;    /* Data field octet 1 */
	t_Retc ret;    /* Return code */
	t_Byte sno;    /* Sector angle; 360/256 degrees */

				   /* Preset the return code: */
	ret = RC_FAIL;

				   /* Check parameters: */
	Assert (length == 1, "Invalid parameter");
	Assert (buffer != NULL, "Invalid parameter");

				   /* Extract octets: */
	df1 = buffer[0];

				   /* Extract information: */
	sno = df1;

                   /* Compute sector angle (in degrees): */
	azf = (360.0 / 256.0) * sno;

#if LISTER
				   /* List data field: */
	list_text (2, ";  Sector Angle:");
	list_text (2, " " M_FMT_BYTE " (%.3f deg)", sno, azf);
	list_text (2, "\n");
#endif /* LISTER */

				   /* Store this information: */
	rsrv.antenna_azimuth.present = TRUE;
	rsrv.antenna_azimuth.value = M_DEG2RAD * azf;
                   /* In radians */

				   /* Set the return code: */
	ret = RC_OKAY;

	return ret;
}

/*----------------------------------------------------------------------------*/
/* proc_i002_030   -- Process I002/030 data item                              */
/*----------------------------------------------------------------------------*/

 static
 t_Retc proc_i002_030 (t_Ui16 length, t_Byte *buffer)
{
	t_Byte df1;    /* Data field octet 1 */
	t_Byte df2;    /* Data field octet 2 */
	t_Byte df3;    /* Data field octet 3 */
	t_Retc ret;    /* Return code */
	t_Secs tm;     /* Time (of day); seconds */
	t_Ui32 tod;    /* Time of day; 1/128 seconds */

				   /* Preset the return code: */
	ret = RC_FAIL;

				   /* Check parameters: */
	Assert (length == 3, "Invalid parameter");
	Assert (buffer != NULL, "Invalid parameter");

				   /* Extract octets: */
	df1 = buffer[0];
	df2 = buffer[1];
	df3 = buffer[2];

				   /* Extract information: */
	tod = make_ui32 (0, df1, df2, df3);

				   /* Compute time (of day): */
	tm = (1.0 / 128.0) * tod;
	if (tm >= M_SECONDS_PER_DAY)
	{
		tm -= M_SECONDS_PER_DAY;
	}

                   /* Check time (of day): */
	Assert (0.0 <= tm && tm < M_SECONDS_PER_DAY, "Invalid time");

#if LISTER
				   /* List data field: */
	list_text (2, ";  Time of Day:");
	list_text (2, " " M_FMT_ATOD " (" M_FMT_UI32 "; %.6f secs; %s UTC)",
                  tod, tod, tm, utc_text (tm));
	list_text (2, "\n");
#endif /* LISTER */

                   /* Remember this information: */
	last_tod_available = TRUE;
	last_tod = tod;

				   /* Store this information: */
	rsrv.message_time.present = TRUE;
	rsrv.message_time.value = tm;
	rsrv.time_of_day.present = TRUE;
	rsrv.time_of_day.value = tod;

				   /* Set the return code: */
	ret = RC_OKAY;

	return ret;
}

/*----------------------------------------------------------------------------*/
/* proc_i002_041   -- Process I002/041 data item                              */
/*----------------------------------------------------------------------------*/

 static
 t_Retc proc_i002_041 (t_Ui16 length, t_Byte *buffer)
{
	t_Ui16 ars;    /* Antenna rotation speed; 1/128 seconds */
	t_Byte df1;    /* Data field octet 1 */
	t_Byte df2;    /* Data field octet 2 */
	t_Retc ret;    /* Return code */

				   /* Preset the return code: */
	ret = RC_FAIL;

				   /* Check parameters: */
	Assert (length == 2, "Invalid parameter");
	Assert (buffer != NULL, "Invalid parameter");

				   /* Extract octets: */
	df1 = buffer[0];
	df2 = buffer[1];

				   /* Extract information: */
	ars = make_ui16 (df1, df2);

#if LISTER
				   /* List data field: */
	list_text (2, ";  Antenna Rotation Speed:");
	list_text (2, " " M_FMT_UI16 " (%.3f seconds)", ars, (1.0 / 128.0) * ars);
	list_text (2, "\n");
#endif /* LISTER */

				   /* Store this information: */
	if (ars > 0)
	{
		rsrv.antenna_rotation_speed.present = TRUE;
		rsrv.antenna_rotation_speed.value = (M_TWO_PI * 128.0) / ars;
                   /* In radians/second */
	}

				   /* Set the return code: */
	ret = RC_OKAY;

	return ret;
}

/*----------------------------------------------------------------------------*/
/* proc_i002_050   -- Process I002/050 data item                              */
/*----------------------------------------------------------------------------*/

 static
 t_Retc proc_i002_050 (t_Ui16 length, t_Byte *buffer)
{
#if LISTER
	int ix;        /* Auxiliary */
#endif /* LISTER */
	t_Retc ret;    /* Return code */

				   /* Preset the return code: */
	ret = RC_FAIL;

				   /* Check parameters: */
	Assert (length > 0, "Invalid parameter");
	Assert (buffer != NULL, "Invalid parameter");

#if LISTER
				   /* List data field: */
	list_text (2, ";  Station Configuration Status: 0x");
	for (ix = 0; ix < length; ix ++)
	{
		list_text (2, " " M_FMT_BYTE_HEX, buffer[ix] & ~0x01);
	}
	list_text (2, "\n");
#endif /* LISTER */

				   /* Store this information: */
	if (length == 1)
	{
		rsrv.station_configuration_status.buffer =
			(t_Ui16) buffer[0];
		rsrv.station_configuration_status.data_ptr = NULL;
	}
	else if (length == 2)
	{
		rsrv.station_configuration_status.buffer =
			make_ui16 (buffer[0], buffer[1]);
		rsrv.station_configuration_status.data_ptr = NULL;
	}
	else
	{
		t_Pntr ptr;

		ptr = malloc (length);
		Assert (ptr != NULL, "Not enough memory");

		memcpy (ptr, buffer, length);

		rsrv.station_configuration_status.buffer = 0;
		rsrv.station_configuration_status.data_ptr = (t_Byte *) ptr;
	}
	rsrv.station_configuration_status.length = length;
	rsrv.station_configuration_status.present = TRUE;

				   /* Set the return code: */
	ret = RC_OKAY;

	return ret;
}

/*----------------------------------------------------------------------------*/
/* proc_i002_060   -- Process I002/060 data item                              */
/*----------------------------------------------------------------------------*/

 static
 t_Retc proc_i002_060 (t_Ui16 length, t_Byte *buffer)
{
#if LISTER
	int ix;        /* Auxiliary */
#endif /* LISTER */
	t_Retc ret;    /* Return code */

				   /* Preset the return code: */
	ret = RC_FAIL;

				   /* Check parameters: */
	Assert (length > 0, "Invalid parameter");
	Assert (buffer != NULL, "Invalid parameter");

#if LISTER
				   /* List data field: */
	list_text (2, ";  Station Processing Mode: 0x");
	for (ix = 0; ix < length; ix ++)
	{
		list_text (2, " " M_FMT_BYTE_HEX, buffer[ix] & ~0x01);
	}
	list_text (2, "\n");
#endif /* LISTER */

				   /* Store this information: */
	if (length == 1)
	{
		rsrv.station_processing_mode.buffer =
			(t_Ui16) buffer[0];
		rsrv.station_processing_mode.data_ptr = NULL;
	}
	else if (length == 2)
	{
		rsrv.station_processing_mode.buffer =
			make_ui16 (buffer[0], buffer[1]);
		rsrv.station_processing_mode.data_ptr = NULL;
	}
	else
	{
		t_Pntr ptr;

		ptr = malloc (length);
		Assert (ptr != NULL, "Not enough memory");

		memcpy (ptr, buffer, length);

		rsrv.station_processing_mode.buffer = 0;
		rsrv.station_processing_mode.data_ptr = (t_Byte *) ptr;
	}
	rsrv.station_processing_mode.length = length;
	rsrv.station_processing_mode.present = TRUE;

				   /* Set the return code: */
	ret = RC_OKAY;

	return ret;
}

/*----------------------------------------------------------------------------*/
/* proc_i002_070   -- Process I002/070 data item                              */
/*----------------------------------------------------------------------------*/

 static
 t_Retc proc_i002_070 (t_Ui16 length, t_Byte *buffer)
{
#if LISTER
	t_Ui16 a;      /* Aerial identification */
	t_Ui16 counter;
                   /* Counter value */
	t_Byte df1;    /* Data field octet 1 */
	t_Byte df2;    /* Data field octet 2 */
	int ix;        /* Auxiliary */
	t_Ui16 ident;  /* Plot category identification code */
#endif /* LISTER */
	t_Byte rep;    /* Repetition factor */
	t_Retc ret;    /* Return code */
#if LISTER
	t_Ui16 u;      /* Auxiliary */
#endif /* LISTER */

				   /* Preset the return code: */
	ret = RC_FAIL;

				   /* Check parameters: */
	Assert (length > 0, "Invalid parameter");
	Assert (buffer != NULL, "Invalid parameter");

				   /* Get repetition factor: */
	rep = buffer[0];

                   /* Check length (again): */
	Assert (length == 1 + 2 * rep, "Invalid length");

#if LISTER
				   /* List data field: */
	list_text (2, ";  Plot Count Values:\n");
	for (ix = 0; ix < rep; ix ++)
	{
		df1 = buffer[1 + 2 * ix];
		df2 = buffer[1 + 2 * ix + 1];

		u = make_ui16 (df1, df2);

		list_text (2, ";   Count %d:", 1 + ix);

		a = (u >> 15) & 0x01;
		ident = (u >> 10) & 0x001f;
		counter = u & 0x03ff;

		list_text (2, " a=" M_FMT_UI16, a);
		list_text (2, " ident=" M_FMT_UI16, ident);
		if (ident == 1)
		{
			list_text (2, " (PSR)");
		}
		else if (ident == 2)
		{
			list_text (2, " (SSR)");
		}
		else if (ident == 3)
		{
			list_text (2, " (CMB)");
		}
		list_text (2, ";");
		list_text (2, " counter=" M_FMT_UI16, counter);
		list_text (2, "\n");
	}
#endif /* LISTER */

				   /* Store this information: */
	/* tba ... */

				   /* Set the return code: */
	ret = RC_OKAY;

	return ret;
}

/*----------------------------------------------------------------------------*/
/* proc_i002_080   -- Process I002/080 data item                              */
/*----------------------------------------------------------------------------*/

 static
 t_Retc proc_i002_080 (t_Ui16 length, t_Byte *buffer)
{
#if LISTER
	int ix;        /* Auxiliary */
#endif /* LISTER */
	t_Retc ret;    /* Return code */

				   /* Preset the return code: */
	ret = RC_FAIL;

				   /* Check parameters: */
	Assert (length > 0, "Invalid parameter");
	Assert (buffer != NULL, "Invalid parameter");

#if LISTER
				   /* List data field: */
	list_text (2, ";  Warning/Error Conditions: 0x");
	for (ix = 0; ix < length; ix ++)
	{
		list_text (2, " " M_FMT_BYTE_HEX, buffer[ix] & ~0x01);
	}
	list_text (2, "\n");
#endif /* LISTER */

				   /* Store this information: */
	if (length == 1)
	{
		rsrv.warning_error_conditions.buffer =
			(t_Ui16) buffer[0];
		rsrv.warning_error_conditions.data_ptr = NULL;
	}
	else if (length == 2)
	{
		rsrv.warning_error_conditions.buffer =
			make_ui16 (buffer[0], buffer[1]);
		rsrv.warning_error_conditions.data_ptr = NULL;
	}
	else
	{
		t_Pntr ptr;

		ptr = malloc (length);
		Assert (ptr != NULL, "Not enough memory");

		memcpy (ptr, buffer, length);

		rsrv.warning_error_conditions.buffer = 0;
		rsrv.warning_error_conditions.data_ptr = (t_Byte *) ptr;
	}
	rsrv.warning_error_conditions.length = length;
	rsrv.warning_error_conditions.present = TRUE;

				   /* Set the return code: */
	ret = RC_OKAY;

	return ret;
}

/*----------------------------------------------------------------------------*/
/* proc_i002_090   -- Process I002/090 data item                              */
/*----------------------------------------------------------------------------*/

 static
 t_Retc proc_i002_090 (t_Ui16 length, t_Byte *buffer)
{
	t_Si16 aze;    /* Azimuth error; 360/2**14 degrees */
	t_Byte df1;    /* Data field octet 1 */
	t_Byte df2;    /* Data field octet 2 */
	t_Retc ret;    /* Return code */
	t_Si16 rge;    /* Range error; 1/128 nautical mile */

				   /* Preset the return code: */
	ret = RC_FAIL;

				   /* Check parameters: */
	Assert (length == 2, "Invalid parameter");
	Assert (buffer != NULL, "Invalid parameter");

				   /* Extract octets: */
	df1 = buffer[0];
	df2 = buffer[1];

				   /* Extract information: */
	rge = (t_Si16) (df1 & 0x7f);
	if (df1 & 0x80)
	{
		rge = -rge;
	}
	aze = (t_Si16) (df2 & 0x7f);
	if (df2 & 0x80)
	{
		aze = -aze;
	}

#if LISTER
				   /* List data field: */
	list_text (2, ";  Collimation Error:");
	list_text (2, " rge=" M_FMT_SI16 " (%.3f nmi);",
                  rge, (1.0 / 128.0) * rge);
	list_text (2, " aze=" M_FMT_SI16 " (%.3f deg)",
                  aze, (360.0 / 16384.0) * aze);
	list_text (2, "\n");
#endif /* LISTER */

				   /* Store this information: */
	rsrv.collimation_error.present = TRUE;
	rsrv.collimation_error.value_azm = (M_TWO_PI / 16384.0) * aze;
	rsrv.collimation_error.value_rng = (M_NMI2MTR / 128.0) * rge;

				   /* Set the return code: */
	ret = RC_OKAY;

	return ret;
}

/*----------------------------------------------------------------------------*/
/* proc_i002_100   -- Process I002/100 data item                              */
/*----------------------------------------------------------------------------*/

 static
 t_Retc proc_i002_100 (t_Ui16 length, t_Byte *buffer)
{
	t_Byte df1;    /* Data field octet 1 */
	t_Byte df2;    /* Data field octet 2 */
	t_Byte df3;    /* Data field octet 3 */
	t_Byte df4;    /* Data field octet 4 */
	t_Byte df5;    /* Data field octet 5 */
	t_Byte df6;    /* Data field octet 6 */
	t_Byte df7;    /* Data field octet 7 */
	t_Byte df8;    /* Data field octet 8 */
	t_Retc ret;    /* Return code */
	t_Ui16 rho_end;
                   /* End of range; 1/128 nautical miles */
	t_Ui16 rho_start;
				   /* Start of range; 1/128 nautical miles */
	t_Ui16 theta_end;
				   /* End of azimuth; 360/2**16 degrees */
	t_Ui16 theta_start;
				   /* Start of azimuth; 360/2**16 degrees */

				   /* Preset the return code: */
	ret = RC_FAIL;

				   /* Check parameters: */
	Assert (length == 8, "Invalid parameter");
	Assert (buffer != NULL, "Invalid parameter");

				   /* Extract octets: */
	df1 = buffer[0];
	df2 = buffer[1];
	df3 = buffer[2];
	df4 = buffer[3];
	df5 = buffer[4];
	df6 = buffer[5];
	df7 = buffer[6];
	df8 = buffer[7];

				   /* Extract information: */
	rho_start = make_ui16 (df1, df2);
	rho_end = make_ui16 (df3, df4);
	theta_start = make_ui16 (df5, df6);
	theta_end = make_ui16 (df7, df8);

#if LISTER
				   /* List data field: */
	list_text (2, ";  Dynamic Window:");
	list_text (2, " rho=" M_FMT_UI16 " ... " M_FMT_UI16
                  "; theta=" M_FMT_UI16 " ... " M_FMT_UI16,
                  rho_start, rho_end, theta_start, theta_end);
	list_text (2, "\n");
#endif /* LISTER */

				   /* Store this information: */
	rsrv.polar_window.present = TRUE;
	rsrv.polar_window.value_azm_start = (M_TWO_PI / 65536.0) * theta_start;
	rsrv.polar_window.value_azm_stop = (M_TWO_PI / 65536.0) * theta_end;
	rsrv.polar_window.value_rng_start = (M_NMI2MTR / 128.0) * rho_start;
	rsrv.polar_window.value_rng_stop = (M_NMI2MTR / 128.0) * rho_end;

				   /* Set the return code: */
	ret = RC_OKAY;

	return ret;
}

/*----------------------------------------------------------------------------*/
/* proc_i002_rfs   -- Process RFS indicator data item                         */
/*----------------------------------------------------------------------------*/

 static
 t_Retc proc_i002_rfs (t_Ui16 length, t_Byte *buffer, t_Ui16 *pos_ptr)
{
	t_Byte frn;    /* Field reference number */
	int ix;        /* Auxiliary */
	t_Retc lrc;    /* Local return code */
	t_Byte n;      /* Number of data fields */
	t_Ui16 pos;    /* Position within buffer */
	t_Retc ret;    /* Return code */

				   /* Preset the return code: */
	ret = RC_FAIL;

				   /* Check parameters: */
	Assert (length > 0, "Invalid parameter");
	Assert (buffer != NULL, "Invalid parameter");
	Assert (pos_ptr != NULL, "Invalid parameter");

				   /* Set position within buffer: */
	pos = *pos_ptr;

				   /* Check position: */
	Assert (pos < length, "Invalid position");

				   /* Get number of data fields: */
	n = buffer[pos ++];

				   /* Check number of data fields: */
	if (n == 0)
	{
		error_msg ("Invalid number of data fields");
		goto done;
	}

#if LISTER
				   /* List raw data field: */
	list_text (1, ";  I002/RFS: 0x " M_FMT_BYTE_HEX " ...\n", n);
#endif /* LISTER */

				   /* Process data fields: */
	for (ix = 0; ix < n; ix ++)
	{
				   /* Check against buffer length: */
		if (pos >= length)
		{
			error_msg ("Invalid buffer length");
			goto done;
		}

				   /* Get field reference number: */
		frn = buffer[pos ++];

#if LISTER
				   /* List raw data field: */
		list_text (1, ";  I002/RFS: frn=" M_FMT_BYTE "\n", frn);
#endif /* LISTER */

				   /* Check field reference number: */
		if (frn <= 0)
		{
			error_msg ("Invalid FRN");
			goto done;
		}
		else if (frn > M_MAX_FRN)
		{
			error_msg ("FRN too large");
			goto done;
		}

		lrc = data_item (2, frn, std_uap[frn],
                         length, buffer, &pos);
		if (lrc != RC_OKAY)
		{
			error_msg ("Invalid data item");
			goto done;
		}
	}

				   /* Set position: */
	*pos_ptr = pos;

				   /* Set the return code: */
	ret = RC_OKAY;

	done:          /* We are done */
	return ret;
}

/*----------------------------------------------------------------------------*/
/* proc_i002_spf   -- Process SPF indicator data item                         */
/*----------------------------------------------------------------------------*/

 static
 t_Retc proc_i002_spf (t_Ui16 length, t_Byte *buffer, t_Ui16 *pos_ptr)
{
	t_Byte len;    /* Length of data field */
	t_Ui16 pos;    /* Position within buffer */
	t_Retc ret;    /* Return code */

				   /* Preset the return code: */
	ret = RC_FAIL;

				   /* Check parameters: */
	Assert (length > 0, "Invalid parameter");
	Assert (buffer != NULL, "Invalid parameter");
	Assert (pos_ptr != NULL, "Invalid parameter");

				   /* Set position within buffer: */
	pos = *pos_ptr;

				   /* Check position: */
	Assert (pos < length, "Invalid position");

				   /* Get length of data field: */
	len = buffer[pos];

				   /* Check length of data field: */
	if (len < 1)
	{
		error_msg ("Invalid length of data field");
		goto done;
	}

				   /* Check against buffer length: */
	if (pos + len > length)
	{
		error_msg ("Invalid buffer length");
		goto done;
	}

#if LISTER
				   /* List raw data field: */
	list_buffer (1, ";  I002/SPF:", len, buffer + pos);
#endif /* LISTER */

				   /* Set position: */
	*pos_ptr = pos + len;

				   /* Set the return code: */
	ret = RC_OKAY;

	done:          /* We are done */
	return ret;
}
/* end-of-file */
