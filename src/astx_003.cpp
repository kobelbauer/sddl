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
/* File:           src/astx_003.cpp                                           */
/* Contents:       Process ASTERIX category 003 data block                    */
/* Author(s):      kb                                                         */
/* Last change:    2018-04-27                                                 */
/*----------------------------------------------------------------------------*/

/* Reference document:
   -------------------

   User Interface Definition of the MADAP Track Server
   MAS-S/GD_0026_03
   April 1998

   Previous version:
   MAS-S/GD_0026_01
   August 1992
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

#define M_MAX_FRN 21
				   /* Maximum field reference number */
#define M_MAX_FSPEC_LENGTH 3
				   /* Maximum fields specification length
					  for ASTERIX category 003 */

				   /* Local variables: */
				   /* ---------------- */

static t_Data_Item_Desc desc_i003_010;
                   /* Description of data item I003/010 */
static t_Data_Item_Desc desc_i003_020;
                   /* Description of data item I003/020 */
static t_Data_Item_Desc desc_i003_040;
                   /* Description of data item I003/040 */
static t_Data_Item_Desc desc_i003_050;
                   /* Description of data item I003/050 */
static t_Data_Item_Desc desc_i003_070;
                   /* Description of data item I003/070 */
static t_Data_Item_Desc desc_i003_080;
                   /* Description of data item I003/080 */
static t_Data_Item_Desc desc_i003_090;
                   /* Description of data item I003/090 */
static t_Data_Item_Desc desc_i003_120;
                   /* Description of data item I003/120 */
static t_Data_Item_Desc desc_i003_130;
                   /* Description of data item I003/130 */
static t_Data_Item_Desc desc_i003_140;
                   /* Description of data item I003/140 */
static t_Data_Item_Desc desc_i003_150;
                   /* Description of data item I003/150 */
static t_Data_Item_Desc desc_i003_160;
                   /* Description of data item I003/160 */
static t_Data_Item_Desc desc_i003_170;
                   /* Description of data item I003/170 */
static t_Data_Item_Desc desc_i003_180;
                   /* Description of data item I003/180 */
static t_Data_Item_Desc desc_i003_rfs;
                   /* Description of RFS data item */
static t_Data_Item_Desc desc_i003_spf;
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
static t_Retc proc_i003_010 (t_Ui16 length, t_Byte *buffer);
                   /* Process I003/010 data item */
static t_Retc proc_i003_020 (t_Ui16 length, t_Byte *buffer);
                   /* Process I003/020 data item */
static t_Retc proc_i003_040 (t_Ui16 length, t_Byte *buffer);
                   /* Process I003/040 data item */
static t_Retc proc_i003_050 (t_Ui16 length, t_Byte *buffer);
                   /* Process I003/050 data item */
static t_Retc proc_i003_070 (t_Ui16 length, t_Byte *buffer);
                   /* Process I003/070 data item */
static t_Retc proc_i003_080 (t_Ui16 length, t_Byte *buffer);
                   /* Process I003/080 data item */
static t_Retc proc_i003_090 (t_Ui16 length, t_Byte *buffer);
                   /* Process I003/090 data item */
static t_Retc proc_i003_120 (t_Ui16 length, t_Byte *buffer);
                   /* Process I003/120 data item */
static t_Retc proc_i003_130 (t_Ui16 length, t_Byte *buffer);
                   /* Process I003/130 data item */
static t_Retc proc_i003_140 (t_Ui16 length, t_Byte *buffer);
                   /* Process I003/140 data item */
static t_Retc proc_i003_150 (t_Ui16 length, t_Byte *buffer);
                   /* Process I003/150 data item */
static t_Retc proc_i003_160 (t_Ui16 length, t_Byte *buffer);
                   /* Process I003/160 data item */
static t_Retc proc_i003_170 (t_Ui16 length, t_Byte *buffer);
                   /* Process I003/170 data item */
static t_Retc proc_i003_180 (t_Ui16 length, t_Byte *buffer);
                   /* Process I003/180 data item */
static t_Retc proc_i003_rfs (t_Ui16 length, t_Byte *buffer, t_Ui16 *pos_ptr);
                   /* Process RFS indicator data item */
static t_Retc proc_i003_spf (t_Ui16 length, t_Byte *buffer, t_Ui16 *pos_ptr);
                   /* Process SPF indicator data item */

/*----------------------------------------------------------------------------*/
/* astx_003        -- Process ASTERIX category 003 data block                 */
/*----------------------------------------------------------------------------*/

 t_Retc astx_003 (t_Ui16 length, t_Byte *buffer)
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

                   /* Preset system track information: */
		memset (&strk, 0, sizeof (strk));

                   /* Set data format: */
		strk.asterix_category = 3;
		strk.data_format = e_data_format_asterix;

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

					lrc = data_item (3, frn, std_uap[frn],
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

			ret = RC_SKIP;
            goto done;
        }

                   /* Add frame date, if available: */
		if (frame_date_present)
		{
			strk.frame_date.present = TRUE;
			strk.frame_date.value = frame_date;
		}

                   /* Add frame time, if available: */
		if (frame_time_present)
		{
			strk.frame_time.present = TRUE;
			strk.frame_time.value = frame_time;
		}

                   /* Process this system track message: */
		lrc = process_strk (&strk);
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
                   /* Description of data item I003/010: */
    desc_i003_010.category = 3;
    desc_i003_010.data_item = 10;
    desc_i003_010.item_type = e_fixed_length_data_item;
    desc_i003_010.fixed_length = 2;
    desc_i003_010.proc_fptr = proc_i003_010;
    desc_i003_010.read_fptr = NULL;

                   /* Description of data item I003/020: */
    desc_i003_020.category = 3;
    desc_i003_020.data_item = 20;
    desc_i003_020.item_type = e_fixed_length_data_item;
    desc_i003_020.fixed_length = 4;
    desc_i003_020.proc_fptr = proc_i003_020;
    desc_i003_020.read_fptr = NULL;

                   /* Description of data item I003/040: */
    desc_i003_040.category = 3;
    desc_i003_040.data_item = 40;
    desc_i003_040.item_type = e_fixed_length_data_item;
    desc_i003_040.fixed_length = 2;
    desc_i003_040.proc_fptr = proc_i003_040;
    desc_i003_040.read_fptr = NULL;

                   /* Description of data item I003/050: */
    desc_i003_050.category = 3;
    desc_i003_050.data_item = 50;
    desc_i003_050.item_type = e_fixed_length_data_item;
    desc_i003_050.fixed_length = 2;
    desc_i003_050.proc_fptr = proc_i003_050;
    desc_i003_050.read_fptr = NULL;

                   /* Description of data item I003/070: */
    desc_i003_070.category = 3;
    desc_i003_070.data_item = 70;
    desc_i003_070.item_type = e_fixed_length_data_item;
    desc_i003_070.fixed_length = 2;
    desc_i003_070.proc_fptr = proc_i003_070;
    desc_i003_070.read_fptr = NULL;

                   /* Description of data item I003/080: */
    desc_i003_080.category = 3;
    desc_i003_080.data_item = 80;
    desc_i003_080.item_type = e_variable_length_data_item;
    desc_i003_080.fixed_length = 0;
    desc_i003_080.proc_fptr = proc_i003_080;
    desc_i003_080.read_fptr = NULL;

                   /* Description of data item I003/090: */
    desc_i003_090.category = 3;
    desc_i003_090.data_item = 90;
    desc_i003_090.item_type = e_fixed_length_data_item;
    desc_i003_090.fixed_length = 1;
    desc_i003_090.proc_fptr = proc_i003_090;
    desc_i003_090.read_fptr = NULL;

                   /* Description of data item I003/120: */
    desc_i003_120.category = 3;
    desc_i003_120.data_item = 120;
    desc_i003_120.item_type = e_fixed_length_data_item;
    desc_i003_120.fixed_length = 4;
    desc_i003_120.proc_fptr = proc_i003_120;
    desc_i003_120.read_fptr = NULL;

                   /* Description of data item I003/130: */
    desc_i003_130.category = 3;
    desc_i003_130.data_item = 130;
    desc_i003_130.item_type = e_fixed_length_data_item;
    desc_i003_130.fixed_length = 1;
    desc_i003_130.proc_fptr = proc_i003_130;
    desc_i003_130.read_fptr = NULL;

                   /* Description of data item I003/140: */
    desc_i003_140.category = 3;
    desc_i003_140.data_item = 140;
    desc_i003_140.item_type = e_fixed_length_data_item;
    desc_i003_140.fixed_length = 2;
    desc_i003_140.proc_fptr = proc_i003_140;
    desc_i003_140.read_fptr = NULL;

                   /* Description of data item I003/150: */
    desc_i003_150.category = 3;
    desc_i003_150.data_item = 150;
    desc_i003_150.item_type = e_variable_length_data_item;
    desc_i003_150.fixed_length = 0;
    desc_i003_150.proc_fptr = proc_i003_150;
    desc_i003_150.read_fptr = NULL;

                   /* Description of data item I003/160: */
    desc_i003_160.category = 3;
    desc_i003_160.data_item = 160;
    desc_i003_160.item_type = e_fixed_length_data_item;
    desc_i003_160.fixed_length = 7;
    desc_i003_160.proc_fptr = proc_i003_160;
    desc_i003_160.read_fptr = NULL;

                   /* Description of data item I003/170: */
    desc_i003_170.category = 3;
    desc_i003_170.data_item = 170;
    desc_i003_170.item_type = e_fixed_length_data_item;
    desc_i003_170.fixed_length = 1;
    desc_i003_170.proc_fptr = proc_i003_170;
    desc_i003_170.read_fptr = NULL;

                   /* Description of data item I003/180: */
    desc_i003_180.category = 3;
    desc_i003_180.data_item = 180;
    desc_i003_180.item_type = e_fixed_length_data_item;
    desc_i003_180.fixed_length = 2;
    desc_i003_180.proc_fptr = proc_i003_180;
    desc_i003_180.read_fptr = NULL;

                   /* Description of RFS data item: */
    desc_i003_spf.category = 3;
    desc_i003_spf.data_item = M_RFS_INDICATOR;
    desc_i003_spf.item_type = e_immediate_data_item;
    desc_i003_spf.fixed_length = 0;
    desc_i003_spf.proc_fptr = NULL;
    desc_i003_spf.read_fptr = proc_i003_rfs;

                   /* Description of SPF data item: */
    desc_i003_spf.category = 3;
    desc_i003_spf.data_item = M_SPF_INDICATOR;
    desc_i003_spf.item_type = e_immediate_data_item;
    desc_i003_spf.fixed_length = 0;
    desc_i003_spf.proc_fptr = NULL;
    desc_i003_spf.read_fptr = proc_i003_spf;

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

	std_uap[1] = &desc_i003_010;
	std_uap[2] = &desc_i003_070;
	std_uap[3] = &desc_i003_020;
	std_uap[4] = &desc_i003_120;
	std_uap[5] = &desc_i003_050;
	std_uap[6] = &desc_i003_080;
	std_uap[7] = &desc_i003_150;

	std_uap[8] = &desc_i003_140;
	std_uap[9] = &desc_i003_130;
	std_uap[10] = &desc_i003_160;
	std_uap[11] = &desc_i003_040;
	std_uap[12] = &desc_i003_170;
	std_uap[13] = &desc_i003_180;
	std_uap[14] = &desc_i003_090;

	std_uap[20] = &desc_i003_spf;
	std_uap[21] = &desc_i003_rfs;

	return;
}

/*----------------------------------------------------------------------------*/
/* proc_i003_010   -- Process I003/010 data item                              */
/*----------------------------------------------------------------------------*/

 static
 t_Retc proc_i003_010 (t_Ui16 length, t_Byte *buffer)
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
	strk.measured_information.present = TRUE;
	strk.measured_information.sid_present = TRUE;
	strk.measured_information.value_sid = dsi;

				   /* Set the return code: */
	ret = RC_OKAY;

	return ret;
}

/*----------------------------------------------------------------------------*/
/* proc_i003_020   -- Process I003/020 data item                              */
/*----------------------------------------------------------------------------*/

 static
 t_Retc proc_i003_020 (t_Ui16 length, t_Byte *buffer)
{
	t_Byte df1;    /* Data field octet 1 */
	t_Byte df2;    /* Data field octet 2 */
	t_Byte df3;    /* Data field octet 3 */
	t_Byte df4;    /* Data field octet 4 */
	t_Retc ret;    /* Return code */
	t_Si16 x;      /* x component; 1/64 nautical miles */
	t_Si16 y;      /* y component; 1/64 nautical miles */

				   /* Preset the return code: */
	ret = RC_FAIL;

				   /* Check parameters: */
	Assert (length == 4, "Invalid parameter");
	Assert (buffer != NULL, "Invalid parameter");

				   /* Extract octets: */
	df1 = buffer[0];
	df2 = buffer[1];
	df3 = buffer[2];
	df4 = buffer[3];

				   /* Extract information: */
	x = make_si16 (df1, df2);
	y = make_si16 (df3, df4);

#if LISTER
				   /* List data field: */
	list_text (2, ";  Calculated Position:");
	list_text (2, " x=" M_FMT_SI16 " (%.3f nmi);", x, (1.0 / 64.0) * x);
	list_text (2, " y=" M_FMT_SI16 " (%.3f nmi)", y, (1.0 / 64.0) * y);
	list_text (2, "\n");
#endif /* LISTER */

				   /* Store this information: */
	strk.calculated_position.present = TRUE;
	strk.calculated_position.value_x = (M_NMI2MTR / 64.0) * x;
	strk.calculated_position.value_y = (M_NMI2MTR / 64.0) * y;

				   /* Set the return code: */
	ret = RC_OKAY;

	return ret;
}

/*----------------------------------------------------------------------------*/
/* proc_i003_040   -- Process I003/040 data item                              */
/*----------------------------------------------------------------------------*/

 static
 t_Retc proc_i003_040 (t_Ui16 length, t_Byte *buffer)
{
	t_Byte df1;    /* Data field octet 1 */
	t_Byte df2;    /* Data field octet 2 */
	t_Ui16 m3c;    /* SSR mode 3/A code */
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
	m3c = make_ui16 (df1 & 0x0f, df2);

#if LISTER
				   /* List data field: */
	list_text (2, ";  Mode 3/A Code:");
	list_text (2, " " M_FMT_CODE, m3c);
	list_text (2, "\n");
#endif /* LISTER */

				   /* Store this information: */
	strk.mode_3a_info.code = m3c;
	strk.mode_3a_info.code_changed = (df1 >> 5) & 0x01;
	strk.mode_3a_info.code_garbled = (df1 >> 6) & 0x01;
	strk.mode_3a_info.code_invalid = (df1 >> 7) & 0x01;
	strk.mode_3a_info.present = TRUE;

				   /* Set the return code: */
	ret = RC_OKAY;

	return ret;
}

/*----------------------------------------------------------------------------*/
/* proc_i003_050   -- Process I003/050 data item                              */
/*----------------------------------------------------------------------------*/

 static
 t_Retc proc_i003_050 (t_Ui16 length, t_Byte *buffer)
{
	t_Si16 afl;    /* Actual flight level; 25 feet */
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
	afl = make_si16 (df1, df2);

#if LISTER
				   /* List data field: */
	list_text (2, ";  Actual Flight Level:");
	list_text (2, " " M_FMT_SI16 " (%.2f FL)", afl, 0.25 * afl);
	list_text (2, "\n");
#endif /* LISTER */

				   /* Store this information: */
	strk.measured_track_mode_c_height.present = TRUE;
	strk.measured_track_mode_c_height.value = 25.0 * M_FT2MTR * afl;
	strk.measured_track_mode_c_height.value_in_feet = 25 * afl;

				   /* Set the return code: */
	ret = RC_OKAY;

	return ret;
}

/*----------------------------------------------------------------------------*/
/* proc_i003_070   -- Process I003/070 data item                              */
/*----------------------------------------------------------------------------*/

 static
 t_Retc proc_i003_070 (t_Ui16 length, t_Byte *buffer)
{
	t_Byte df1;    /* Data field octet 1 */
	t_Byte df2;    /* Data field octet 2 */
	t_Retc ret;    /* Return code */
#if LISTER
	t_Ui16 tno;    /* Track number */
#endif /* LISTER */

				   /* Preset the return code: */
	ret = RC_FAIL;

				   /* Check parameters: */
	Assert (length == 2, "Invalid parameter");
	Assert (buffer != NULL, "Invalid parameter");

				   /* Extract octets: */
	df1 = buffer[0];
	df2 = buffer[1];

#if LISTER
				   /* Extract information: */
	tno = make_ui16 (df1, df2);

				   /* List data field: */
	list_text (2, ";  Track Number:");
	list_text (2, " " M_FMT_UI16, tno);
	list_text (2, "\n");
#endif /* LISTER */

				   /* Store this information: */
	strk.track_number.present = TRUE;
	strk.track_number.value = make_ui16 (df1 & 0x0f, df2);

				   /* Set the return code: */
	ret = RC_OKAY;

	return ret;
}

/*----------------------------------------------------------------------------*/
/* proc_i003_080   -- Process I003/080 data item                              */
/*----------------------------------------------------------------------------*/

 static
 t_Retc proc_i003_080 (t_Ui16 length, t_Byte *buffer)
{
	t_Byte dfo;    /* Data field octet */
	int ix;        /* Auxiliary */
	t_Retc ret;    /* Return code */

				   /* Preset the return code: */
	ret = RC_FAIL;

				   /* Check parameters: */
	Assert (length > 0, "Invalid parameter");
	Assert (buffer != NULL, "Invalid parameter");

#if LISTER
				   /* List data field: */
	list_text (2, ";  Track Status:");
	for (ix = 0; ix < length; ix ++)
	{
		dfo = buffer[ix];
		if (ix == 0)
		{
			t_Byte typ;

			if (dfo & 0x80)
			{
				list_text (2, " LIV");
			}
			else
			{
				list_text (2, " SIM");
			}

			if (dfo & 0x40)
			{
				list_text (2, " CNF");
			}
			else
			{
				list_text (2, " TNT");
			}

			if (dfo & 0x20)
			{
				list_text (2, " MNV");
			}

			if (dfo & 0x010)
			{
				list_text (2, " MDA");
			}

			typ = (dfo >> 2) & 0x03;
			if (typ == 1)
			{
				list_text (2, " PUD");
			}
			else if (typ == 2)
			{
				list_text (2, " SUD");
			}
			else if (typ == 3)
			{
				list_text (2, " P+S");
			}

			if (dfo & 0x02)
			{
				list_text (2, " COR");
			}
		}
		else if (ix == 1)
		{
			if (dfo & 0x80)
			{
				list_text (2, " CON");
			}

			if (dfo & 0x40)
			{
				list_text (2, " DIV");
			}

			if (dfo & 0x20)
			{
				list_text (2, " GHO");
			}

			if (dfo & 0x10)
			{
				list_text (2, " TRE");
			}

			if (dfo & 0x08)
			{
				list_text (2, " SPI");
			}

			if (dfo & 0x06)
			{
				t_Byte emg;

				emg = (dfo >> 1) & 0x03;
				list_text (2, " EM" M_FMT_BYTE, emg);
			}
		}
	}
	/* tba ... */
	list_text (2, "\n");
#endif /* LISTER */

				   /* Store this information: */
	strk.adsb_blunder_detected = e_is_undefined;
	strk.amalgamated_track = e_is_undefined;
	strk.assigned_code_conflict = e_is_undefined;
	strk.background_service = e_is_undefined;
	strk.coasted_track = e_is_undefined;
	strk.flight_plan_correlated = e_is_undefined;
	strk.formation_flight = e_is_undefined;
	strk.ghost_track = e_is_undefined;
	strk.manoeuvring_track = e_is_undefined;
	strk.military_emergency = e_is_undefined;
	strk.military_ident = e_is_undefined;
	strk.multi_sensor_track = e_is_undefined;
	strk.observed_by_ads = e_is_undefined;
	strk.observed_by_mds = e_is_undefined;
	strk.observed_by_psr = e_is_undefined;
	strk.observed_by_ssr = e_is_undefined;
	strk.simulated = e_is_undefined;
	strk.slave_track_promotion = e_is_undefined;
	strk.special_position_indication = e_is_undefined;
	strk.special_used_code = e_is_undefined;
	strk.tentative_track = e_is_undefined;
	strk.test_target = e_is_undefined;
	strk.track_created = e_is_undefined;
	strk.track_status_present = TRUE;
	strk.track_terminated = e_is_undefined;
	strk.track_with_aircraft_derived_data = e_is_undefined;
	strk.transponder_delay_correction = e_is_undefined;

	for (ix = 0; ix < length; ix ++)
	{
		dfo = buffer[ix];

		if (ix == 0)
		{
			t_Byte typ;

			strk.simulated = M_TRES ((dfo & 0x80) == 0);

			strk.tentative_track = M_TRES ((dfo & 0x40) == 0);

			strk.manoeuvring_track = M_TRES (dfo & 0x20);

			typ = (dfo >> 2) & 0x03;
			if (typ == 1)
			{
				strk.observed_by_psr = e_is_true;
				strk.observed_by_ssr = e_is_false;
			}
			else if (typ == 2)
			{
				strk.observed_by_psr = e_is_false;
				strk.observed_by_ssr = e_is_true;
			}
			else if (typ == 3)
			{
				strk.observed_by_psr = e_is_true;
				strk.observed_by_ssr = e_is_true;
			}
		}
		else if (ix == 1)
		{
			strk.ghost_track = M_TRES (dfo & 0x20);

			strk.track_terminated = M_TRES (dfo & 0x10);

			strk.special_position_indication = M_TRES (dfo & 0x08);
		}
	}

				   /* Set the return code: */
	ret = RC_OKAY;

	return ret;
}

/*----------------------------------------------------------------------------*/
/* proc_i003_090   -- Process I003/090 data item                              */
/*----------------------------------------------------------------------------*/

 static
 t_Retc proc_i003_090 (t_Ui16 length, t_Byte *buffer)
{
#if LISTER
	t_Byte df1;    /* Data field octet 1 */
	t_Byte fr;     /* Flight rules */
#endif /* LISTER */
	t_Retc ret;    /* Return code */
#if LISTER
	t_Byte sp;     /* Special conditions */
#endif /* LISTER */

				   /* Preset the return code: */
	ret = RC_FAIL;

				   /* Check parameters: */
	Assert (length == 1, "Invalid parameter");
	Assert (buffer != NULL, "Invalid parameter");

#if LISTER
                   /* Extract octet: */
	df1 = buffer[0];

				   /* List data field: */
	list_text (2, ";  Track Category:");
	if (df1 & 0x80)
	{
		list_text (2, " OAT");
	}
	if (df1 & 0x40)
	{
		list_text (2, " GAT");
	}
	fr = (df1 >> 4) & 0x03;
	list_text (2, "; FR=" M_FMT_BYTE, fr);
	sp = (df1 >> 1) & 0x07;
	list_text (2, "; SP=" M_FMT_BYTE, sp);
	list_text (2, "\n");
#endif /* LISTER */

				   /* Store this information: */
	/* tba ... */

				   /* Set the return code: */
	ret = RC_OKAY;

	return ret;
}

/*----------------------------------------------------------------------------*/
/* proc_i003_120   -- Process I003/120 data item                              */
/*----------------------------------------------------------------------------*/

 static
 t_Retc proc_i003_120 (t_Ui16 length, t_Byte *buffer)
{
	t_Byte df1;    /* Data field octet 1 */
	t_Byte df2;    /* Data field octet 2 */
	t_Byte df3;    /* Data field octet 3 */
	t_Byte df4;    /* Data field octet 4 */
	t_Ui16 hdg;    /* Calculated heading; 360/2**16 degrees */
	t_Retc ret;    /* Return code */
	t_Ui16 spd;    /* Calculated groundspeed; 2**-14 nautical miles/second */

				   /* Preset the return code: */
	ret = RC_FAIL;

				   /* Check parameters: */
	Assert (length == 4, "Invalid parameter");
	Assert (buffer != NULL, "Invalid parameter");

				   /* Extract octets: */
	df1 = buffer[0];
	df2 = buffer[1];
	df3 = buffer[2];
	df4 = buffer[3];

				   /* Extract information: */
	spd = make_ui16 (df1, df2);
	hdg = make_ui16 (df3, df4);

#if LISTER
				   /* List data field: */
	list_text (2, ";  Calculated Track Velocity:");
	list_text (2, " spd=" M_FMT_UI16 " (%.3f kts);",
                  spd, (3600.0 / 16384.0) * spd);
	list_text (2, " hdg=" M_FMT_UI16 " (%.3f deg)",
                  hdg, (360.0 / 65536.0) * hdg);
	list_text (2, "\n");
#endif /* LISTER */

				   /* Store this information: */
	strk.ground_vector.present = TRUE;
	strk.ground_vector.value_gsp = (3600.0 * M_NMI2MTR / 16384.0) * spd;
	strk.ground_vector.value_hdg = (M_TWO_PI / 65536.0) * hdg;

				   /* Set the return code: */
	ret = RC_OKAY;

	return ret;
}

/*----------------------------------------------------------------------------*/
/* proc_i003_130   -- Process I003/130 data item                              */
/*----------------------------------------------------------------------------*/

 static
 t_Retc proc_i003_130 (t_Ui16 length, t_Byte *buffer)
{
#if CCWARN
	t_Byte df1;    /* Data field octet 1 */
#endif /* CCWARN */
	t_Retc ret;    /* Return code */

				   /* Preset the return code: */
	ret = RC_FAIL;

				   /* Check parameters: */
	Assert (length == 1, "Invalid parameter");
	Assert (buffer != NULL, "Invalid parameter");

#if LISTER
				   /* Extract octets: */
#if CCWARN
	df1 = buffer[0];
#endif /* CCWARN */

				   /* Extract information: */
	/* tba ... */

				   /* List data field: */
	list_text (2, ";  Attitude/Intention Indicator:");
	/* tba ... */
	list_text (2, "\n");
#endif /* LISTER */

				   /* Store this information: */
	/* tba ... */

				   /* Set the return code: */
	ret = RC_OKAY;

	return ret;
}

/*----------------------------------------------------------------------------*/
/* proc_i003_140   -- Process I003/140 data item                              */
/*----------------------------------------------------------------------------*/

 static
 t_Retc proc_i003_140 (t_Ui16 length, t_Byte *buffer)
{
	t_Byte df1;    /* Data field octet 1 */
	t_Byte df2;    /* Data field octet 2 */
	t_Si16 rate;   /* Calculated rate of climb/descent; 2**-10 FL/second */
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
	rate = make_si16 (df1, df2);

#if LISTER
				   /* List data field: */
	list_text (2, ";  Calculated Rate of Climb/Descent:");
	list_text (2, " " M_FMT_SI16 " (%.2f ft/min)",
                  rate, (6000.0 / 1024.0) * rate);
	list_text (2, "\n");
#endif /* LISTER */

				   /* Store this information: */
	strk.calculated_rate_of_climb_descent.present = TRUE;
	strk.calculated_rate_of_climb_descent.value = (M_FL2MTR / 1024.0) * rate;
                   /* In metres/second */

				   /* Set the return code: */
	ret = RC_OKAY;

	return ret;
}

/*----------------------------------------------------------------------------*/
/* proc_i003_150   -- Process I003/150 data item                              */
/*----------------------------------------------------------------------------*/

 static
 t_Retc proc_i003_150 (t_Ui16 length, t_Byte *buffer)
{
	t_Retc ret;    /* Return code */

				   /* Preset the return code: */
	ret = RC_FAIL;

				   /* Check parameters: */
	Assert (length > 0, "Invalid parameter");
	Assert (buffer != NULL, "Invalid parameter");

#if LISTER
				   /* List data field: */
	list_text (2, ";  Track Quality:");
	if (length == 1)
	{
		t_Byte cv;
		t_Byte q;

		cv = (buffer[0] >> 6) & 0x03;
		q = (buffer[0] >> 1) & 0x0f;

		list_text (2, " cv=" M_FMT_BYTE, cv);
		if (cv == 0)
		{
			list_text (2, " (not applicable)");
		}
		else if (cv == 1)
		{
			list_text (2, " (track updated by 1 radar)");
		}
		else if (cv == 2)
		{
			list_text (2, " (track updated by 2 radars)");
		}
		else if (cv == 3)
		{
			list_text (2, " (track updated by at least 3 radars)");
		}

		list_text (2, "; q=" M_FMT_UI16, q);
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
/* proc_i003_160   -- Process I003/160 data item                              */
/*----------------------------------------------------------------------------*/

 static
 t_Retc proc_i003_160 (t_Ui16 length, t_Byte *buffer)
{
	t_Byte df1;    /* Data field octet 1 */
	t_Byte df2;    /* Data field octet 2 */
	t_Byte df3;    /* Data field octet 3 */
	t_Byte df4;    /* Data field octet 4 */
	t_Byte df5;    /* Data field octet 5 */
	t_Byte df6;    /* Data field octet 6 */
	t_Byte df7;    /* Data field octet 7 */
	t_Retc ret;    /* Return code */
	char txt[7 + 1];
                   /* Callsign */

				   /* Preset the return code: */
	ret = RC_FAIL;

				   /* Check parameters: */
	Assert (length == 7, "Invalid parameter");
	Assert (buffer != NULL, "Invalid parameter");

				   /* Extract octets: */
	df1 = buffer[0];
	df2 = buffer[1];
	df3 = buffer[2];
	df4 = buffer[3];
	df5 = buffer[4];
	df6 = buffer[5];
	df7 = buffer[6];

				   /* Extract information: */
	memset (txt, 0, 8);
	txt[0] = df1;
	txt[1] = df2;
	txt[2] = df3;
	txt[3] = df4;
	txt[4] = df5;
	txt[5] = df6;
	txt[6] = df7;

#if LISTER
				   /* List data field: */
	list_text (2, ";  Callsign:");
	list_text (2, " '%s'", txt);
	list_text (2, "\n");
#endif /* LISTER */

				   /* Store this information: */
	/* tba ... */

				   /* Set the return code: */
	ret = RC_OKAY;

	return ret;
}

/*----------------------------------------------------------------------------*/
/* proc_i003_170   -- Process I003/170 data item                              */
/*----------------------------------------------------------------------------*/

 static
 t_Retc proc_i003_170 (t_Ui16 length, t_Byte *buffer)
{
#if CCWARN
	t_Byte df1;    /* Data field octet 1 */
#endif /* CCWARN */
	t_Retc ret;    /* Return code */

				   /* Preset the return code: */
	ret = RC_FAIL;

				   /* Check parameters: */
	Assert (length == 1, "Invalid parameter");
	Assert (buffer != NULL, "Invalid parameter");

#if LISTER
				   /* Extract octets: */
#if CCWARN
	df1 = buffer[0];
#endif /* CCWARN */

				   /* Extract information: */
	/* tba ... */

				   /* List data field: */
	list_text (2, ";  Current Control Position:");
	/* tba ... */
	list_text (2, "\n");
#endif /* LISTER */

				   /* Store this information: */
	/* tba ... */

				   /* Set the return code: */
	ret = RC_OKAY;

	return ret;
}

/*----------------------------------------------------------------------------*/
/* proc_i003_180   -- Process I003/180 data item                              */
/*----------------------------------------------------------------------------*/

 static
 t_Retc proc_i003_180 (t_Ui16 length, t_Byte *buffer)
{
#if LISTER
	t_Si16 cfl;    /* Current cleared flight level; 25 feet */
	t_Byte df1;    /* Data field octet 1 */
	t_Byte df2;    /* Data field octet 2 */
#endif /* LISTER */
	t_Retc ret;    /* Return code */

				   /* Preset the return code: */
	ret = RC_FAIL;

				   /* Check parameters: */
	Assert (length == 2, "Invalid parameter");
	Assert (buffer != NULL, "Invalid parameter");

#if LISTER
				   /* Extract octets: */
	df1 = buffer[0];
	df2 = buffer[1];

				   /* Extract information: */
	cfl = make_si16 (df1, df2);

				   /* List data field: */
	list_text (2, ";  Cleared Flight Level:");
	list_text (2, " " M_FMT_SI16 " (%.2f FL)", cfl, 0.25 * cfl);
	list_text (2, "\n");
#endif /* LISTER */

				   /* Store this information: */
	/* tba ... */

				   /* Set the return code: */
	ret = RC_OKAY;

	return ret;
}

/*----------------------------------------------------------------------------*/
/* proc_i003_rfs   -- Process RFS indicator data item                         */
/*----------------------------------------------------------------------------*/

 static
 t_Retc proc_i003_rfs (t_Ui16 length, t_Byte *buffer, t_Ui16 *pos_ptr)
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
	list_text (1, ";  I003/RFS: 0x " M_FMT_BYTE_HEX " ...\n", n);
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
		list_text (1, ";  I003/RFS: frn=" M_FMT_BYTE "\n", frn);
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

		lrc = data_item (3, frn, std_uap[frn], length, buffer, &pos);
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
/* proc_i003_spf   -- Process SPF indicator data item                         */
/*----------------------------------------------------------------------------*/

 static
 t_Retc proc_i003_spf (t_Ui16 length, t_Byte *buffer, t_Ui16 *pos_ptr)
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
	list_buffer (1, ";  I003/SPF:", len, buffer + pos);
#endif /* LISTER */

				   /* Set position: */
	*pos_ptr = pos + len;

				   /* Set the return code: */
	ret = RC_OKAY;

	done:          /* We are done */
	return ret;
}
/* end-of-file */
