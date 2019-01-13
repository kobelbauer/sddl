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
/* File:           src/astx_034.cpp                                           */
/* Contents:       Process ASTERIX category 034 data block                    */
/* Author(s):      kb                                                         */
/* Last change:    2018-04-27                                                 */
/*----------------------------------------------------------------------------*/

/* Reference document:
   -------------------

   Eurocontrol Standard Document for Surveillance Data Exchange.
   Part 2b: Transmission of Monoradar Service Messages.
   SUR.ET1.ST05.2000-STD-02b-01
   Edition: 1.27
   Edition Date: May 2007
   Status: Released Issue
*/

#include "basics.h"
                   /* Basic system definitions */
#include "common.h"
                   /* Common declarations and definitions */

#include <stddef.h>
#include <stdio.h>
#include <string.h>

				   /* Local macros: */
				   /* ------------- */

#define M_MAX_FRN 14
				   /* Maximum field reference number */
#define M_MAX_FSPEC_LENGTH 2
				   /* Maximum fields specification length
					  for ASTERIX category 034 */

				   /* Local variables: */
				   /* ---------------- */

static t_Data_Item_Desc desc_i034_000;
                   /* Description of data item I034/000 */
static t_Data_Item_Desc desc_i034_010;
                   /* Description of data item I034/010 */
static t_Data_Item_Desc desc_i034_020;
                   /* Description of data item I034/020 */
static t_Data_Item_Desc desc_i034_030;
                   /* Description of data item I034/030 */
static t_Data_Item_Desc desc_i034_041;
                   /* Description of data item I034/041 */
static t_Data_Item_Desc desc_i034_050;
                   /* Description of data item I034/050 */
static t_Data_Item_Desc desc_i034_060;
                   /* Description of data item I034/060 */
static t_Data_Item_Desc desc_i034_070;
                   /* Description of data item I034/070 */
static t_Data_Item_Desc desc_i034_090;
                   /* Description of data item I034/090 */
static t_Data_Item_Desc desc_i034_100;
                   /* Description of data item I034/100 */
static t_Data_Item_Desc desc_i034_110;
                   /* Description of data item I034/110 */
static t_Data_Item_Desc desc_i034_120;
                   /* Description of data item I034/120 */
static t_Data_Item_Desc desc_i034_ref;
                   /* Description of REF data item */
static t_Data_Item_Desc desc_i034_spf;
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
                   /* Load standard UAP */
static t_Retc proc_i034_000 (t_Ui16 length, t_Byte *buffer);
                   /* Process I034/000 data item */
static t_Retc proc_i034_010 (t_Ui16 length, t_Byte *buffer);
                   /* Process I034/010 data item */
static t_Retc proc_i034_020 (t_Ui16 length, t_Byte *buffer);
                   /* Process I034/020 data item */
static t_Retc proc_i034_030 (t_Ui16 length, t_Byte *buffer);
                   /* Process I034/030 data item */
static t_Retc proc_i034_041 (t_Ui16 length, t_Byte *buffer);
                   /* Process I034/041 data item */
static t_Retc proc_i034_050 (t_Ui16 length, t_Byte *buffer, t_Ui16 *pos_ptr);
                   /* Process I034/050 data item */
static t_Retc proc_i034_060 (t_Ui16 length, t_Byte *buffer, t_Ui16 *pos_ptr);
                   /* Process I034/060 data item */
static t_Retc proc_i034_070 (t_Ui16 length, t_Byte *buffer);
                   /* Process I034/070 data item */
static t_Retc proc_i034_090 (t_Ui16 length, t_Byte *buffer);
                   /* Process I034/090 data item */
static t_Retc proc_i034_100 (t_Ui16 length, t_Byte *buffer);
                   /* Process I034/100 data item */
static t_Retc proc_i034_110 (t_Ui16 length, t_Byte *buffer);
                   /* Process I034/110 data item */
static t_Retc proc_i034_120 (t_Ui16 length, t_Byte *buffer);
                   /* Process I034/120 data item */
static t_Retc proc_i034_ref (t_Ui16 length, t_Byte *buffer, t_Ui16 *pos_ptr);
                   /* Process REF indicator data item */
static t_Retc proc_i034_spf (t_Ui16 length, t_Byte *buffer, t_Ui16 *pos_ptr);
                   /* Process SPF indicator data item */

/*----------------------------------------------------------------------------*/
/* astx_034        -- Process ASTERIX category 034 data block                 */
/*----------------------------------------------------------------------------*/

 t_Retc astx_034 (t_Ui16 length, t_Byte *buffer)
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

				   /* Enter into fields specification: */
			if (fspec_length >= M_MAX_FSPEC_LENGTH)
			{
				error_msg ("Fields specification too long");
				goto done;
			}
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
		rsrv.asterix_category = 34;
		rsrv.data_format = e_data_format_asterix;

				   /* Decode fields specification according to
					  standard User Application Profile: */

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

					lrc = data_item (34, frn, std_uap[frn],
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
                   /* Description of data item I034/000: */
	desc_i034_000.category = 34;
	desc_i034_000.data_item = 0;
	desc_i034_000.item_type = e_fixed_length_data_item;
	desc_i034_000.fixed_length = 1;
	desc_i034_000.proc_fptr = proc_i034_000;
	desc_i034_000.read_fptr = NULL;

                   /* Description of data item I034/010: */
	desc_i034_010.category = 34;
	desc_i034_010.data_item = 10;
	desc_i034_010.item_type = e_fixed_length_data_item;
	desc_i034_010.fixed_length = 2;
	desc_i034_010.proc_fptr = proc_i034_010;
	desc_i034_010.read_fptr = NULL;

                   /* Description of data item I034/020: */
	desc_i034_020.category = 34;
	desc_i034_020.data_item = 20;
	desc_i034_020.item_type = e_fixed_length_data_item;
	desc_i034_020.fixed_length = 1;
	desc_i034_020.proc_fptr = proc_i034_020;
	desc_i034_020.read_fptr = NULL;

                   /* Description of data item I034/030: */
	desc_i034_030.category = 34;
	desc_i034_030.data_item = 30;
	desc_i034_030.item_type = e_fixed_length_data_item;
	desc_i034_030.fixed_length = 3;
	desc_i034_030.proc_fptr = proc_i034_030;
	desc_i034_030.read_fptr = NULL;

                   /* Description of data item I034/041: */
	desc_i034_041.category = 34;
	desc_i034_041.data_item = 41;
	desc_i034_041.item_type = e_fixed_length_data_item;
	desc_i034_041.fixed_length = 2;
	desc_i034_041.proc_fptr = proc_i034_041;
	desc_i034_041.read_fptr = NULL;

                   /* Description of data item I034/050: */
	desc_i034_050.category = 34;
	desc_i034_050.data_item = 50;
	desc_i034_050.item_type = e_immediate_data_item;
	desc_i034_050.fixed_length = 0;
	desc_i034_050.proc_fptr = NULL;
	desc_i034_050.read_fptr = proc_i034_050;

                   /* Description of data item I034/060: */
	desc_i034_060.category = 34;
	desc_i034_060.data_item = 60;
	desc_i034_060.item_type = e_immediate_data_item;
	desc_i034_060.fixed_length = 0;
	desc_i034_060.proc_fptr = NULL;
	desc_i034_060.read_fptr = proc_i034_060;

                   /* Description of data item I034/070: */
	desc_i034_070.category = 34;
	desc_i034_070.data_item = 70;
	desc_i034_070.item_type = e_repetitive_data_item;
	desc_i034_070.fixed_length = 2;
	desc_i034_070.proc_fptr = proc_i034_070;
	desc_i034_070.read_fptr = NULL;

                   /* Description of data item I034/090: */
	desc_i034_090.category = 34;
	desc_i034_090.data_item = 90;
	desc_i034_090.item_type = e_fixed_length_data_item;
	desc_i034_090.fixed_length = 2;
	desc_i034_090.proc_fptr = proc_i034_090;
	desc_i034_090.read_fptr = NULL;

                   /* Description of data item I034/100: */
	desc_i034_100.category = 34;
	desc_i034_100.data_item = 100;
	desc_i034_100.item_type = e_fixed_length_data_item;
	desc_i034_100.fixed_length = 8;
	desc_i034_100.proc_fptr = proc_i034_100;
	desc_i034_100.read_fptr = NULL;

                   /* Description of data item I034/110: */
	desc_i034_110.category = 34;
	desc_i034_110.data_item = 110;
	desc_i034_110.item_type = e_fixed_length_data_item;
	desc_i034_110.fixed_length = 1;
	desc_i034_110.proc_fptr = proc_i034_110;
	desc_i034_110.read_fptr = NULL;

                   /* Description of data item I034/120: */
	desc_i034_120.category = 34;
	desc_i034_120.data_item = 120;
	desc_i034_120.item_type = e_fixed_length_data_item;
	desc_i034_120.fixed_length = 8;
	desc_i034_120.proc_fptr = proc_i034_120;
	desc_i034_120.read_fptr = NULL;

                   /* Description of REF data item: */
	desc_i034_ref.category = 34;
	desc_i034_ref.data_item = M_REF_INDICATOR;
	desc_i034_ref.item_type = e_immediate_data_item;
	desc_i034_ref.fixed_length = 0;
	desc_i034_ref.proc_fptr = NULL;
	desc_i034_ref.read_fptr = proc_i034_ref;

                   /* Description of SPF data item: */
	desc_i034_spf.category = 34;
	desc_i034_spf.data_item = M_SPF_INDICATOR;
	desc_i034_spf.item_type = e_immediate_data_item;
	desc_i034_spf.fixed_length = 0;
	desc_i034_spf.proc_fptr = NULL;
	desc_i034_spf.read_fptr = proc_i034_spf;

	return;
}

/*----------------------------------------------------------------------------*/
/* load_std_uap    -- Load standard UAP                                       */
/*----------------------------------------------------------------------------*/

 static
 void load_std_uap (void)
{
	int ix;

	for (ix = 0; ix <= M_MAX_FRN; ix ++)
	{
		std_uap[ix] = NULL;
	}

	std_uap[1] = &desc_i034_010;
	std_uap[2] = &desc_i034_000;
	std_uap[3] = &desc_i034_030;
	std_uap[4] = &desc_i034_020;
	std_uap[5] = &desc_i034_041;
	std_uap[6] = &desc_i034_050;
	std_uap[7] = &desc_i034_060;

	std_uap[8] = &desc_i034_070;
	std_uap[9] = &desc_i034_100;
	std_uap[10] = &desc_i034_110;
	std_uap[11] = &desc_i034_120;
	std_uap[12] = &desc_i034_090;
	std_uap[13] = &desc_i034_ref;
	std_uap[14] = &desc_i034_spf;

	return;
}

/*----------------------------------------------------------------------------*/
/* proc_i034_000   -- Process I034/000 data item                              */
/*----------------------------------------------------------------------------*/

 static
 t_Retc proc_i034_000 (t_Ui16 length, t_Byte *buffer)
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
	if (mtp == 1)
	{
		list_text (2, " North marker message");
	}
	else if (mtp == 2)
	{
		list_text (2, " Sector crossing message");
	}
	else if (mtp == 3)
	{
		list_text (2, " Geographical filtering message");
	}
	else if (mtp == 4)
	{
		list_text (2, " Jamming strobe message");
	}
	else
	{
		list_text (2, " unknown (" M_FMT_BYTE ")", mtp);
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
	/* tba ... geographical filtering message */
	else if (mtp == 4)
	{
		rsrv.rsrv_type = e_rsrv_jamming_strobe;
	}

				   /* Set the return code: */
	ret = RC_OKAY;

	return ret;
}

/*----------------------------------------------------------------------------*/
/* proc_i034_010   -- Process I034/010 data item                              */
/*----------------------------------------------------------------------------*/

 static
 t_Retc proc_i034_010 (t_Ui16 length, t_Byte *buffer)
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
/* proc_i034_020   -- Process I034/020 data item                              */
/*----------------------------------------------------------------------------*/

 static
 t_Retc proc_i034_020 (t_Ui16 length, t_Byte *buffer)
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
	list_text (2, " " M_FMT_BYTE " (%.3f degrees)", sno, azf);
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
/* proc_i034_030   -- Process I034/030 data item                              */
/*----------------------------------------------------------------------------*/

 static
 t_Retc proc_i034_030 (t_Ui16 length, t_Byte *buffer)
{
	t_Byte df1;    /* Data field octet 1 */
	t_Byte df2;    /* Data field octet 2 */
	t_Byte df3;    /* Data field octet 3 */
	t_Retc ret;    /* Return code */
	t_Ui32 tod;    /* Time of day; 1/128 seconds */
	t_Secs tod_in_secs;
                   /* Time of day; seconds */

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
	tod = make_ui32 (0x00, df1, df2, df3);

				   /* Compute time (of day): */
	tod_in_secs = (1.0 / 128.0) * tod;
	if (tod_in_secs >= M_SECONDS_PER_DAY)
	{
		tod_in_secs -= M_SECONDS_PER_DAY;
	}

                   /* Check time (of day): */
	Assert (0.0 <= tod_in_secs && tod_in_secs < M_SECONDS_PER_DAY,
            "Invalid time of day");

#if LISTER
				   /* List data field: */
	list_text (2, ";  Time of Day:");
	list_text (2, " " M_FMT_ATOD " (" M_FMT_UI32 "; %.6f secs; %s UTC)",
                  tod, tod, tod_in_secs, utc_text (tod_in_secs));
	list_text (2, "\n");
#endif /* LISTER */

                   /* Remember this information: */
	last_tod_available = TRUE;
	last_tod = tod;

				   /* Store this information: */
	rsrv.message_time.present = TRUE;
	rsrv.message_time.value = tod_in_secs;
	rsrv.time_of_day.present = TRUE;
	rsrv.time_of_day.value = tod;

				   /* Set the return code: */
	ret = RC_OKAY;

	return ret;
}

/*----------------------------------------------------------------------------*/
/* proc_i034_041   -- Process I034/041 data item                              */
/*----------------------------------------------------------------------------*/

 static
 t_Retc proc_i034_041 (t_Ui16 length, t_Byte *buffer)
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
	list_text (2, " " M_FMT_BYTE " (%.3f seconds)",
                  ars, (1.0 / 128.0) * ars);
	list_text (2, "\n");
#endif /* LISTER */

				   /* Store this information: */
	if (ars > 0)
	{
		rsrv.antenna_rotation_speed.present = TRUE;
		rsrv.antenna_rotation_speed.value = (128.0 * M_TWO_PI) / ars;
                   /* In radians/second */
	}

				   /* Set the return code: */
	ret = RC_OKAY;

	return ret;
}

/*----------------------------------------------------------------------------*/
/* proc_i034_050   -- Process I034/050 data item                              */
/*----------------------------------------------------------------------------*/

 static
 t_Retc proc_i034_050 (t_Ui16 length, t_Byte *buffer, t_Ui16 *pos_ptr)
{
	int cnt;       /* Count for secondary subfield octets */
#if LISTER
	t_Byte com;    /* Common part */
#endif /* LISTER */
	t_Byte df1;    /* Data field octet 1 */
#if LISTER
	int ix;        /* Auxiliary */
	t_Ui16 mds;    /* Specific status for mode S sensor */
#endif /* LISTER */
	t_Ui16 pos;    /* Position within buffer */
#if LISTER
	t_Byte psr;    /* Specific status for PSR sensor */
#endif /* LISTER */
	t_Retc ret;    /* Return code */
	t_Byte sf1_present;
                   /* Subfield #1 (Common part) present */
	t_Byte sf4_present;
                   /* Subfield #4 (Status for PSR sensor) present */
	t_Byte sf5_present;
                   /* Subfield #5 (Status for SSR sensor) present */
	t_Byte sf6_present;
                   /* Subfield #6 (Status for mode S sensor) present */
#if LISTER
	t_Byte ssr;    /* Specific status for SSR sensor */
#endif /* LISTER */

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

				   /* Extract first octet: */
	df1 = buffer[pos ++];

                   /* Evaluate presence of subfields: */
	sf1_present = (df1 >> 7) & 0x01;
	sf4_present = (df1 >> 4) & 0x01;
	sf5_present = (df1 >> 3) & 0x01;
	sf6_present = (df1 >> 2) & 0x01;

                   /* Calculate number of octets in secondary
                      subfields: */
	cnt = ord (sf1_present) +
          ord (sf4_present) +
          ord (sf5_present) +
          2 * ord (sf6_present);

				   /* Check against buffer length: */
	if (pos + cnt > length)
	{
		error_msg ("Invalid buffer length");
		goto done;
	}

#if LISTER
				   /* List raw data field: */
	list_text (1, ";  I034/050: 0x %02x", df1);
	for (ix = 0; ix < cnt; ix ++)
	{
		list_text (1, " " M_FMT_BYTE_HEX, buffer[pos + ix]);
	}
	list_text (1, "\n");
#endif /* LISTER */

                   /* Preset information: */
#if LISTER
	com = 0;
	mds = 0;
	psr = 0;
	ssr = 0;
#endif /* LISTER */

                   /* Extract information: */
	if (sf1_present)
	{
#if LISTER
		com = buffer[pos];
#endif /* LISTER */

		++ pos;
	}
	if (sf4_present)
	{
#if LISTER
		psr = buffer[pos];
#endif /* LISTER */

		++ pos;
	}
	if (sf5_present)
	{
#if LISTER
		ssr = buffer[pos];
#endif /* LISTER */

		++ pos;
	}
	if (sf6_present)
	{
#if LISTER
		t_Byte df2, df3;

		df2 = buffer[pos];
		df3 = buffer[pos + 1];

		mds = make_ui16 (df2, df3);
#endif /* LISTER */

		pos += 2;
	}

#if LISTER
				   /* List data field: */
	list_text (2, ";  System Configuration and Status:\n");
	if (sf1_present)
	{
		list_text (2, ";   Common Part:");
		list_text (2, " NOGO=" M_FMT_BYTE ";", (com >> 7) & 0x01);
		list_text (2, " RDPC=" M_FMT_BYTE ";", (com >> 6) & 0x01);
		list_text (2, " RDPR=" M_FMT_BYTE ";", (com >> 5) & 0x01);
		list_text (2, " OVL_RDP=" M_FMT_BYTE ";", (com >> 4) & 0x01);
		list_text (2, " OVL_XMT=" M_FMT_BYTE ";", (com >> 3) & 0x01);
		list_text (2, " MSC=" M_FMT_BYTE ";", (com >> 2) & 0x01);
		list_text (2, " TSV=" M_FMT_BYTE, (com >> 1) & 0x01);
		list_text (2, "\n");
	}
	if (sf4_present)
	{
		list_text (2, ";   PSR Sensor:");
		list_text (2, " ANT=" M_FMT_BYTE ";", (psr >> 7) & 0x01);
		list_text (2, " CH-A/B=" M_FMT_BYTE ";", (psr >> 5) & 0x03);
		list_text (2, " OVL=" M_FMT_BYTE ";", (psr >> 4) & 0x01);
		list_text (2, " MSC=" M_FMT_BYTE, (psr >> 3) & 0x01);
		list_text (2, "\n");
	}
	if (sf5_present)
	{
		list_text (2, ";   SSR Sensor:");
		list_text (2, " ANT=" M_FMT_BYTE ";", (ssr >> 7) & 0x01);
		list_text (2, " CH-A/B=" M_FMT_BYTE ";", (ssr >> 5) & 0x03);
		list_text (2, " OVL=" M_FMT_BYTE ";", (ssr >> 4) & 0x01);
		list_text (2, " MSC=" M_FMT_BYTE, (ssr >> 3) & 0x01);
		list_text (2, "\n");
	}
	if (sf6_present)
	{
		list_text (2, ";   Mode S Sensor:");
		list_text (2, " ANT=" M_FMT_UI16 ";", (mds >> 15) & 0x01);
		list_text (2, " CH-A/B=" M_FMT_UI16 ";", (mds >> 13) & 0x03);
		list_text (2, " OVL_SUR=" M_FMT_UI16 ";", (mds >> 12) & 0x01);
		list_text (2, " MSC=" M_FMT_UI16 ";", (mds >> 11) & 0x01);
		list_text (2, " SCF=" M_FMT_UI16 ";", (mds >> 10) & 0x01);
		list_text (2, " DLF=" M_FMT_UI16 ";", (mds >> 9) & 0x01);
		list_text (2, " OVL_SCF=" M_FMT_UI16 ";", (mds >> 8) & 0x01);
		list_text (2, " OVL_DLF=" M_FMT_UI16, (mds >> 7) & 0x01);
		list_text (2, "\n");
	}
#endif /* LISTER */

				   /* Store this information: */
	/* tba ... */

				   /* Set position: */
	*pos_ptr = pos;

				   /* Set the return code: */
	ret = RC_OKAY;

	done:          /* We are done */
	return ret;
}

/*----------------------------------------------------------------------------*/
/* proc_i034_060   -- Process I034/060 data item                              */
/*----------------------------------------------------------------------------*/

 static
 t_Retc proc_i034_060 (t_Ui16 length, t_Byte *buffer, t_Ui16 *pos_ptr)
{
	int cnt;       /* Count for secondary subfield octets */
#if LISTER
	t_Byte com;    /* Common part */
#endif /* LISTER */
	t_Byte df1;    /* Data field octet 1 */
#if LISTER
	t_Byte mds;    /* Specific processing mode information for
                      mode S sensor */
#endif /* LISTER */
	t_Ui16 pos;    /* Position within buffer */
#if LISTER
	t_Byte psr;    /* Specific processing mode information for
                      PSR sensor */
#endif /* LISTER */
	t_Retc ret;    /* Return code */
	t_Byte sf1_present;
                   /* Subfield #1 (Common part) present */
	t_Byte sf4_present;
                   /* Subfield #4 (Mode information for 
                      PSR sensor) present */
	t_Byte sf5_present;
                   /* Subfield #5 (Mode information for 
                      SSR sensor) present */
	t_Byte sf6_present;
                   /* Subfield #6 (Mode information for
                      mode S sensor) present */
#if LISTER
	t_Byte ssr;    /* Specific processing mode information for
                      SSR sensor */
#endif /* LISTER */

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

				   /* Extract first octet: */
	df1 = buffer[pos ++];

                   /* Evaluate presence of subfields: */
	sf1_present = (df1 >> 7) & 0x01;
	sf4_present = (df1 >> 4) & 0x01;
	sf5_present = (df1 >> 3) & 0x01;
	sf6_present = (df1 >> 2) & 0x01;

                   /* Calculate number of octets in secondary
                      subfields: */
	cnt = ord (sf1_present) +
          ord (sf4_present) +
          ord (sf5_present) +
          ord (sf6_present);

				   /* Check against buffer length: */
	if (pos + cnt > length)
	{
		error_msg ("Invalid buffer length");
		goto done;
	}

#if LISTER
				   /* List raw data field: */
	list_buffer (1, ";  I034/060:", cnt + 1, buffer + pos - 1);
#endif /* LISTER */

                   /* Preset information: */
#if LISTER
	com = 0;
	mds = 0;
	psr = 0;
	ssr = 0;
#endif /* LISTER */

                   /* Extract information: */
	if (sf1_present)
	{
#if LISTER
		com = buffer[pos];
#endif /* LISTER */

		++ pos;
	}
	if (sf4_present)
	{
#if LISTER
		psr = buffer[pos];
#endif /* LISTER */

		++ pos;
	}
	if (sf5_present)
	{
#if LISTER
		ssr = buffer[pos];
#endif /* LISTER */

		++ pos;
	}
	if (sf6_present)
	{
#if LISTER
		mds = buffer[pos];
#endif /* LISTER */

		++ pos;
	}

#if LISTER
				   /* List data field: */
	list_text (2, ";  System Processing Mode:\n");
	if (sf1_present)
	{
		list_text (2, ";   Common Part:");
		list_text (2, " RED_RDP=" M_FMT_BYTE ";", (com >> 4) & 0x07);
		list_text (2, " RED_XMT=" M_FMT_BYTE, (com >> 1) & 0x07);
		list_text (2, "\n");
	}
	if (sf4_present)
	{
		list_text (2, ";   PSR Sensor:");
		list_text (2, " POL=" M_FMT_BYTE ";", (psr >> 7) & 0x01);
		list_text (2, " RED_RAD=" M_FMT_BYTE ";", (psr >> 4) & 0x07);
		list_text (2, " STC=" M_FMT_BYTE, (psr >> 2) & 0x03);
		list_text (2, "\n");
	}
	if (sf5_present)
	{
		list_text (2, ";   SSR Sensor:");
		list_text (2, " RED_RAD=" M_FMT_BYTE, (ssr >> 5) & 0x07);
		list_text (2, "\n");
	}
	if (sf6_present)
	{
		list_text (2, ";   Mode S Sensor:");
		list_text (2, " RED_RAD=" M_FMT_BYTE ";", (mds >> 5) & 0x07);
		list_text (2, " CLU=" M_FMT_BYTE, (mds >> 4) & 0x01);
		list_text (2, "\n");
	}
#endif /* LISTER */

				   /* Store this information: */
	/* tba ... */

				   /* Set position: */
	*pos_ptr = pos;

				   /* Set the return code: */
	ret = RC_OKAY;

	done:          /* We are done */
	return ret;
}

/*----------------------------------------------------------------------------*/
/* proc_i034_070   -- Process I034/070 data item                              */
/*----------------------------------------------------------------------------*/

 static
 t_Retc proc_i034_070 (t_Ui16 length, t_Byte *buffer)
{
#if LISTER
	t_Ui16 counter;
                   /* Counter value */
	t_Byte df1;    /* Data field octet 1 */
	t_Byte df2;    /* Data field octet 2 */
	int ix;        /* Auxiliary */
#endif /* LISTER */
	t_Byte rep;    /* Repetition factor */
	t_Retc ret;    /* Return code */
#if LISTER
	t_Ui16 typ;    /* Type of message counter */
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
	list_text (2, ";  Message Count Values:\n");
	for (ix = 0; ix < rep; ix ++)
	{
		df1 = buffer[1 + 2 * ix];
		df2 = buffer[1 + 2 * ix + 1];

		u = make_ui16 (df1, df2);

		list_text (2, ";   Count %d:", 1 + ix);

		typ = (u >> 11) & 0x001f;
		counter = u & 0x07ff;

		list_text (2, " type=" M_FMT_UI16 ";", typ);
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
/* proc_i034_090   -- Process I034/090 data item                              */
/*----------------------------------------------------------------------------*/

 static
 t_Retc proc_i034_090 (t_Ui16 length, t_Byte *buffer)
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
                   /* Negative values encoded in two's complement */
	rge = (t_Si16) (df1 & 0x7f);
	if (df1 & 0x80)
	{
		rge |= 0xff80;
	}
	aze = (t_Si16) (df2 & 0x7f);
	if (df2 & 0x80)
	{
		aze |= 0xff80;
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
/* proc_i034_100   -- Process I034/100 data item                              */
/*----------------------------------------------------------------------------*/

 static
 t_Retc proc_i034_100 (t_Ui16 length, t_Byte *buffer)
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
                   /* End of range; 1/256 nautical miles */
	t_Ui16 rho_start;
				   /* Start of range; 1/256 nautical miles */
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
	list_text (2, ";  Generic Polar Window:");
	list_text (2, " rho=" M_FMT_UI16 " ... " M_FMT_UI16 ";",
                  rho_start, rho_end);
	list_text (2, " theta=" M_FMT_UI16 " ... " M_FMT_UI16,
                  theta_start, theta_end);
	list_text (2, "\n");
#endif /* LISTER */

				   /* Store this information: */
	rsrv.polar_window.present = TRUE;
	rsrv.polar_window.value_azm_start = (M_TWO_PI / 65536.0) * theta_start;
	rsrv.polar_window.value_azm_stop = (M_TWO_PI / 65536.0) * theta_end;
	rsrv.polar_window.value_rng_start = (M_NMI2MTR / 256.0) * rho_start;
	rsrv.polar_window.value_rng_stop = (M_NMI2MTR / 256.0) * rho_end;

				   /* Set the return code: */
	ret = RC_OKAY;

	return ret;
}

/*----------------------------------------------------------------------------*/
/* proc_i034_110   -- Process I034/110 data item                              */
/*----------------------------------------------------------------------------*/

 static
 t_Retc proc_i034_110 (t_Ui16 length, t_Byte *buffer)
{
#if LISTER
	t_Byte df1;    /* Data field octet 1 */
#endif /* LISTER */
	t_Retc ret;    /* Return code */
#if LISTER
	t_Byte typ;    /* Filter type */
#endif /* LISTER */

				   /* Preset the return code: */
	ret = RC_FAIL;

				   /* Check parameters: */
	Assert (length == 1, "Invalid parameter");
	Assert (buffer != NULL, "Invalid parameter");

#if LISTER
				   /* Extract octets: */
	df1 = buffer[0];

				   /* Extract information: */
	typ = df1;

				   /* List data field: */
	list_text (2, ";  Data Filter:");
	if (typ == 0)
	{
		list_text (2, " Invalid value");
	}
	else if (typ == 1)
	{
		list_text (2, " Filter for Weather data");
	}
	else if (typ == 2)
	{
		list_text (2, " Filter for Jamming Strobe");
	}
	else if (typ == 3)
	{
		list_text (2, " Filter for PSR data");
	}
	else if (typ == 4)
	{
		list_text (2, " Filter for SSR/Mode S data");
	}
	else if (typ == 5)
	{
		list_text (2, " Filter for SSR/Mode S + PSR data");
	}
	else if (typ == 6)
	{
		list_text (2, " Enhanced Surveillance data");
	}
	else if (typ == 7)
	{
		list_text (2, " Filter for PSR + Enhanced Surveillance data");
	}
	else if (typ == 8)
	{
		list_text (2, " Filter for PSR + Enhanced Surveillance +"
                      " SSR/Mode S data not in area of Prime Interest");
	}
	else if (typ == 9)
	{
		list_text (2, " Filter for PSR + Enhanced Surveillance +"
                      " all SSR/Mode S data");
	}
	else
	{
		list_text (2, " unknown (%u)", typ);
	}
	list_text (2, "\n");
#endif /* LISTER */

				   /* Store this information: */
	/* tba ... */

				   /* Set the return code: */
	ret = RC_OKAY;

	return ret;
}

/*----------------------------------------------------------------------------*/
/* proc_i034_120   -- Process I034/120 data item                              */
/*----------------------------------------------------------------------------*/

 static
 t_Retc proc_i034_120 (t_Ui16 length, t_Byte *buffer)
{
	t_Byte df1;    /* Data field octet 1 */
	t_Byte df2;    /* Data field octet 2 */
	t_Byte df3;    /* Data field octet 3 */
	t_Byte df4;    /* Data field octet 4 */
	t_Byte df5;    /* Data field octet 5 */
	t_Byte df6;    /* Data field octet 6 */
	t_Byte df7;    /* Data field octet 7 */
	t_Byte df8;    /* Data field octet 8 */
	t_Si16 hgt;    /* Height in WGS-84; metres */
	t_Si32 lat;    /* Latitude in WGS-84; 180/2**23 degrees */
	t_Real latf;   /* Latitude; degrees */
	t_Si32 lon;    /* Longitude in WGS-84; 180/2**23 degrees */
	t_Real lonf;   /* Longitude; degrees */
	t_Retc ret;    /* Return code */

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
	hgt = make_si16 (df1, df2);
	lat = make_ui32 (0x00, df3, df4, df5);
	if (df3 & 0x80)
	{
		lat |= 0xff000000;
	}
	lon = make_ui32 (0x00, df6, df7, df8);
	if (df6 & 0x80)
	{
		lon |= 0xff000000;
	}

                   /* Convert: */
	latf = (180.0 / M_TWO_POWER_23) * lat;
	lonf = (180.0 / M_TWO_POWER_23) * lon;

#if LISTER
				   /* List data field: */
	list_text (2, ";  3D Position of Data Source:\n");
	list_text (2, ";   Height: " M_FMT_SI16 " mtr\n", hgt);
	list_text (2, ";   Latitude: " M_FMT_SI32 " (%s)\n",
                  lat, lat_text (latf));
	list_text (2, ";   Longitude: " M_FMT_SI32 " (%s)\n",
                  lon, lon_text (lonf));
#endif /* LISTER */

				   /* Store this information: */
	rsrv.data_source_position.present = TRUE;
	rsrv.data_source_position.value_altitude = (t_Real) hgt;
	rsrv.data_source_position.value_latitude = M_DEG2RAD * latf;
	rsrv.data_source_position.value_longitude = M_DEG2RAD * lonf;

				   /* Set the return code: */
	ret = RC_OKAY;

	return ret;
}

/*----------------------------------------------------------------------------*/
/* proc_i034_ref   -- Process REF indicator data item                         */
/*----------------------------------------------------------------------------*/

 static
 t_Retc proc_i034_ref (t_Ui16 length, t_Byte *buffer, t_Ui16 *pos_ptr)
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
	list_text (1, ";  I034/REF: 0x " M_FMT_BYTE_HEX " ...\n", n);
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
		list_text (1, ";  I034/REF: frn=" M_FMT_BYTE "\n", frn);
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

		lrc = data_item (34, frn, std_uap[frn],
                         length, buffer, &pos);
		if (lrc != RC_OKAY)
		{
			error_msg ("Invalid data field");
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
/* proc_i034_spf   -- Process SPF indicator data item                         */
/*----------------------------------------------------------------------------*/

 static
 t_Retc proc_i034_spf (t_Ui16 length, t_Byte *buffer, t_Ui16 *pos_ptr)
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
	list_buffer (1, ";  I034/SPF:", len, buffer + pos);
#endif /* LISTER */

				   /* Set position: */
	*pos_ptr = pos + len;

				   /* Set the return code: */
	ret = RC_OKAY;

	done:          /* We are done */
	return ret;
}
/* end-of-file */
