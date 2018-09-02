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
/* File:           src/astx_001.cpp                                           */
/* Contents:       Process ASTERIX category 001 data block                    */
/* Author(s):      kb                                                         */
/* Last change:    2018-04-27                                                 */
/*----------------------------------------------------------------------------*/

/* Reference document:
   -------------------

   Eurocontrol Standard Document for Radar Data Exchange
   Part 2a: Transmission of Monoradar Data Target Reports
   SUR.ET1.ST05.2000-STD-02a-01

   Edition: 1.1
   Edition Date: August 2002
   Status: Released Issue
*/

#include "basics.h"
                   /* Basic system definitions */
#include "common.h"
                   /* Common declarations and definitions */

#include <math.h>
#include <stddef.h>
#include <stdio.h>
#include <string.h>

				   /* Local macros: */
				   /* ------------- */

#define M_MAX_FRN 28
				   /* Maximum field reference number */
#define M_MAX_FSPEC_LENGTH 4
				   /* Maximum fields specification length
					  for ASTERIX category 001 */

				   /* Local variables: */
				   /* ---------------- */

static t_Data_Item_Desc desc_i001_010;
                   /* Description of data item I001/010 */
static t_Data_Item_Desc desc_i001_020;
                   /* Description of data item I001/020 */
static t_Data_Item_Desc desc_i001_030;
                   /* Description of data item I001/030 */
static t_Data_Item_Desc desc_i001_040;
                   /* Description of data item I001/040 */
static t_Data_Item_Desc desc_i001_042;
                   /* Description of data item I001/042 */
static t_Data_Item_Desc desc_i001_050;
                   /* Description of data item I001/050 */
static t_Data_Item_Desc desc_i001_060;
                   /* Description of data item I001/060 */
static t_Data_Item_Desc desc_i001_070;
                   /* Description of data item I001/070 */
static t_Data_Item_Desc desc_i001_080;
                   /* Description of data item I001/080 */
static t_Data_Item_Desc desc_i001_090;
                   /* Description of data item I001/090 */
static t_Data_Item_Desc desc_i001_100;
                   /* Description of data item I001/100 */
static t_Data_Item_Desc desc_i001_120;
                   /* Description of data item I001/120 */
static t_Data_Item_Desc desc_i001_130;
                   /* Description of data item I001/130 */
static t_Data_Item_Desc desc_i001_131;
                   /* Description of data item I001/131 */
static t_Data_Item_Desc desc_i001_141;
                   /* Description of data item I001/141 */
static t_Data_Item_Desc desc_i001_150;
                   /* Description of data item I001/150 */
static t_Data_Item_Desc desc_i001_161;
                   /* Description of data item I001/161 */
static t_Data_Item_Desc desc_i001_170;
                   /* Description of data item I001/170 */
static t_Data_Item_Desc desc_i001_200;
                   /* Description of data item I001/200 */
static t_Data_Item_Desc desc_i001_210;
                   /* Description of data item I001/210 */
static t_Data_Item_Desc desc_i001_rfs;
                   /* Description of RFS data item */
static t_Data_Item_Desc desc_i001_spf;
                   /* Description of SPF data item */
static t_Bool initiated = FALSE;
				   /* Package initiated flag */
static t_Bool is_a_rtrk;
                   /* Radar track flag */
static t_Data_Item_Desc *std_uap[M_MAX_FRN + 1];
				   /* Standard User Application Profile */

				   /* Local functions: */
				   /* ---------------- */

static void init_desc (void);
                   /* Initiate data item descriptions */
static void load_std_uap (void);
                   /* Load standard User Application Profile (UAP) */
static void load_trk_uap (void);
                   /* Load standard radar track UAP */
static t_Retc proc_i001_010 (t_Ui16 length, t_Byte *buffer);
                   /* Process I001/010 data item */
static t_Retc proc_i001_020 (t_Ui16 length, t_Byte *buffer);
                   /* Process I001/020 data item */
static t_Retc proc_i001_030 (t_Ui16 length, t_Byte *buffer);
                   /* Process I001/030 data item */
static t_Retc proc_i001_040 (t_Ui16 length, t_Byte *buffer);
                   /* Process I001/040 data item */
static t_Retc proc_i001_042 (t_Ui16 length, t_Byte *buffer);
                   /* Process I001/042 data item */
static t_Retc proc_i001_050 (t_Ui16 length, t_Byte *buffer);
                   /* Process I001/050 data item */
static t_Retc proc_i001_060 (t_Ui16 length, t_Byte *buffer);
                   /* Process I001/060 data item */
static t_Retc proc_i001_070 (t_Ui16 length, t_Byte *buffer);
                   /* Process I001/070 data item */
static t_Retc proc_i001_080 (t_Ui16 length, t_Byte *buffer);
                   /* Process I001/080 data item */
static t_Retc proc_i001_090 (t_Ui16 length, t_Byte *buffer);
                   /* Process I001/090 data item */
static t_Retc proc_i001_100 (t_Ui16 length, t_Byte *buffer);
                   /* Process I001/100 data item */
static t_Retc proc_i001_120 (t_Ui16 length, t_Byte *buffer);
                   /* Process I001/120 data item */
static t_Retc proc_i001_130 (t_Ui16 length, t_Byte *buffer);
                   /* Process I001/130 data item */
static t_Retc proc_i001_131 (t_Ui16 length, t_Byte *buffer);
                   /* Process I001/131 data item */
static t_Retc proc_i001_141 (t_Ui16 length, t_Byte *buffer);
                   /* Process I001/141 data item */
static t_Retc proc_i001_150 (t_Ui16 length, t_Byte *buffer);
                   /* Process I001/150 data item */
static t_Retc proc_i001_161 (t_Ui16 length, t_Byte *buffer);
                   /* Process I001/161 data item */
static t_Retc proc_i001_170 (t_Ui16 length, t_Byte *buffer);
                   /* Process I001/170 data item */
static t_Retc proc_i001_200 (t_Ui16 length, t_Byte *buffer);
                   /* Process I001/200 data item */
static t_Retc proc_i001_210 (t_Ui16 length, t_Byte *buffer);
                   /* Process I001/210 data item */
static t_Retc proc_i001_rfs (t_Ui16 length, t_Byte *buffer, t_Ui16 *pos_ptr);
                   /* Process RFS indicator data item */
static t_Retc proc_i001_spf (t_Ui16 length, t_Byte *buffer, t_Ui16 *pos_ptr);
                   /* Process SPF indicator data item */

/*----------------------------------------------------------------------------*/
/* astx_001        -- Process ASTERIX category 001 data block                 */
/*----------------------------------------------------------------------------*/

 t_Retc astx_001 (t_Ui16 length, t_Byte *buffer)
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

                   /* Preset last SAC/SIC: */
	last_sacsic = 0x0000;
	last_sacsic_available = FALSE;

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

                   /* Preset radar target report: */
		memset (&rtgt, 0, sizeof (rtgt));

                   /* Set data format: */
		rtgt.asterix_category = 1;
		rtgt.data_format = e_data_format_asterix;

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

					lrc = data_item (1, frn, std_uap[frn],
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
			rtgt.frame_date.present = TRUE;
			rtgt.frame_date.value = frame_date;
		}

                   /* Add frame time, if available: */
		if (frame_time_present)
		{
			rtgt.frame_time.present = TRUE;
			rtgt.frame_time.value = frame_time;
		}

                   /* Add board/line number, if present: */
		if (current_line_number_defined)
		{
			rtgt.line_number = current_line_number;
		}

                   /* If radar target doesn't hold a SAC/SIC, but a
                      last SAC/SIC is available, copy this SAC/SIC
                      to the radar target: */
		if (!rtgt.data_source_identifier.present && last_sacsic_available)
		{
			rtgt.data_source_identifier.present = TRUE;
			rtgt.data_source_identifier.supplemented = TRUE;
			rtgt.data_source_identifier.value = last_sacsic;
		}

                   /* Process this radar target report: */
		lrc = process_rtgt (&rtgt);
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
                   /* Description of data item I001/010: */
	desc_i001_010.category = 1;
	desc_i001_010.data_item = 10;
	desc_i001_010.item_type = e_fixed_length_data_item;
	desc_i001_010.fixed_length = 2;
	desc_i001_010.proc_fptr = proc_i001_010;
	desc_i001_010.read_fptr = NULL;

                   /* Description of data item I001/020: */
	desc_i001_020.category = 1;
	desc_i001_020.data_item = 20;
	desc_i001_020.item_type = e_variable_length_data_item;
	desc_i001_020.fixed_length = 0;
	desc_i001_020.proc_fptr = proc_i001_020;
	desc_i001_020.read_fptr = NULL;

                   /* Description of data item I001/030: */
	desc_i001_030.category = 1;
	desc_i001_030.data_item = 30;
	desc_i001_030.item_type = e_variable_length_data_item;
	desc_i001_030.fixed_length = 0;
	desc_i001_030.proc_fptr = proc_i001_030;
	desc_i001_030.read_fptr = NULL;

                   /* Description of data item I001/040: */
	desc_i001_040.category = 1;
	desc_i001_040.data_item = 40;
	desc_i001_040.item_type = e_fixed_length_data_item;
	desc_i001_040.fixed_length = 4;
	desc_i001_040.proc_fptr = proc_i001_040;
	desc_i001_040.read_fptr = NULL;

                   /* Description of data item I001/042: */
	desc_i001_042.category = 1;
	desc_i001_042.data_item = 42;
	desc_i001_042.item_type = e_fixed_length_data_item;
	desc_i001_042.fixed_length = 4;
	desc_i001_042.proc_fptr = proc_i001_042;
	desc_i001_042.read_fptr = NULL;

                   /* Description of data item I001/050: */
	desc_i001_050.category = 1;
	desc_i001_050.data_item = 50;
	desc_i001_050.item_type = e_fixed_length_data_item;
	desc_i001_050.fixed_length = 2;
	desc_i001_050.proc_fptr = proc_i001_050;
	desc_i001_050.read_fptr = NULL;

                   /* Description of data item I001/060: */
	desc_i001_060.category = 1;
	desc_i001_060.data_item = 60;
	desc_i001_060.item_type = e_fixed_length_data_item;
	desc_i001_060.fixed_length = 2;
	desc_i001_060.proc_fptr = proc_i001_060;
	desc_i001_060.read_fptr = NULL;

                   /* Description of data item I001/070: */
	desc_i001_070.category = 1;
	desc_i001_070.data_item = 70;
	desc_i001_070.item_type = e_fixed_length_data_item;
	desc_i001_070.fixed_length = 2;
	desc_i001_070.proc_fptr = proc_i001_070;
	desc_i001_070.read_fptr = NULL;

                   /* Description of data item I001/080: */
	desc_i001_080.category = 1;
	desc_i001_080.data_item = 80;
	desc_i001_080.item_type = e_fixed_length_data_item;
	desc_i001_080.fixed_length = 2;
	desc_i001_080.proc_fptr = proc_i001_080;
	desc_i001_080.read_fptr = NULL;

                   /* Description of data item I001/090: */
	desc_i001_090.category = 1;
	desc_i001_090.data_item = 90;
	desc_i001_090.item_type = e_fixed_length_data_item;
	desc_i001_090.fixed_length = 2;
	desc_i001_090.proc_fptr = proc_i001_090;
	desc_i001_090.read_fptr = NULL;

                   /* Description of data item I001/100: */
	desc_i001_100.category = 1;
	desc_i001_100.data_item = 100;
	desc_i001_100.item_type = e_fixed_length_data_item;
	desc_i001_100.fixed_length = 4;
	desc_i001_100.proc_fptr = proc_i001_100;
	desc_i001_100.read_fptr = NULL;

                   /* Description of data item I001/120: */
	desc_i001_120.category = 1;
	desc_i001_120.data_item = 120;
	desc_i001_120.item_type = e_fixed_length_data_item;
	desc_i001_120.fixed_length = 1;
	desc_i001_120.proc_fptr = proc_i001_120;
	desc_i001_120.read_fptr = NULL;

                   /* Description of data item I001/130: */
	desc_i001_130.category = 1;
	desc_i001_130.data_item = 130;
	desc_i001_130.item_type = e_variable_length_data_item;
	desc_i001_130.fixed_length = 0;
	desc_i001_130.proc_fptr = proc_i001_130;
	desc_i001_130.read_fptr = NULL;

                   /* Description of data item I001/131: */
	desc_i001_131.category = 1;
	desc_i001_131.data_item = 131;
	desc_i001_131.item_type = e_fixed_length_data_item;
	desc_i001_131.fixed_length = 1;
	desc_i001_131.proc_fptr = proc_i001_131;
	desc_i001_131.read_fptr = NULL;

                   /* Description of data item I001/141: */
	desc_i001_141.category = 1;
	desc_i001_141.data_item = 141;
	desc_i001_141.item_type = e_fixed_length_data_item;
	desc_i001_141.fixed_length = 2;
	desc_i001_141.proc_fptr = proc_i001_141;
	desc_i001_141.read_fptr = NULL;

                   /* Description of data item I001/150: */
	desc_i001_150.category = 1;
	desc_i001_150.data_item = 150;
	desc_i001_150.item_type = e_fixed_length_data_item;
	desc_i001_150.fixed_length = 1;
	desc_i001_150.proc_fptr = proc_i001_150;
	desc_i001_150.read_fptr = NULL;

                   /* Description of data item I001/161: */
	desc_i001_161.category = 1;
	desc_i001_161.data_item = 161;
	desc_i001_161.item_type = e_fixed_length_data_item;
	desc_i001_161.fixed_length = 2;
	desc_i001_161.proc_fptr = proc_i001_161;
	desc_i001_161.read_fptr = NULL;

                   /* Description of data item I001/170: */
	desc_i001_170.category = 1;
	desc_i001_170.data_item = 170;
	desc_i001_170.item_type = e_variable_length_data_item;
	desc_i001_170.fixed_length = 0;
	desc_i001_170.proc_fptr = proc_i001_170;
	desc_i001_170.read_fptr = NULL;

                   /* Description of data item I001/200: */
	desc_i001_200.category = 1;
	desc_i001_200.data_item = 200;
	desc_i001_200.item_type = e_fixed_length_data_item;
	desc_i001_200.fixed_length = 4;
	desc_i001_200.proc_fptr = proc_i001_200;
	desc_i001_200.read_fptr = NULL;

                   /* Description of data item I001/210: */
	desc_i001_210.category = 1;
	desc_i001_210.data_item = 210;
	desc_i001_210.item_type = e_variable_length_data_item;
	desc_i001_210.fixed_length = 0;
	desc_i001_210.proc_fptr = proc_i001_210;
	desc_i001_210.read_fptr = NULL;

                   /* Description of RFS data item: */
	desc_i001_rfs.category = 1;
	desc_i001_rfs.data_item = M_RFS_INDICATOR;
	desc_i001_rfs.item_type = e_immediate_data_item;
	desc_i001_rfs.fixed_length = 0;
	desc_i001_rfs.proc_fptr = NULL;
	desc_i001_rfs.read_fptr = proc_i001_rfs;

                   /* Description of SPF data item: */
	desc_i001_spf.category = 1;
	desc_i001_spf.data_item = M_SPF_INDICATOR;
	desc_i001_spf.item_type = e_immediate_data_item;
	desc_i001_spf.fixed_length = 0;
	desc_i001_spf.proc_fptr = NULL;
	desc_i001_spf.read_fptr = proc_i001_spf;

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

                   /* Load the standard plot UAP: */

	std_uap[1] = &desc_i001_010;
	std_uap[2] = &desc_i001_020;
	std_uap[3] = &desc_i001_040;
	std_uap[4] = &desc_i001_070;
	std_uap[5] = &desc_i001_090;
	std_uap[6] = &desc_i001_130;
	std_uap[7] = &desc_i001_141;

	std_uap[8] = &desc_i001_050;
	std_uap[9] = &desc_i001_120;
	std_uap[10] = &desc_i001_131;
	std_uap[11] = &desc_i001_080;
	std_uap[12] = &desc_i001_100;
	std_uap[13] = &desc_i001_060;
	std_uap[14] = &desc_i001_030;

	std_uap[15] = &desc_i001_150;
	std_uap[20] = &desc_i001_spf;
	std_uap[21] = &desc_i001_rfs;

	return;
}

/*----------------------------------------------------------------------------*/
/* load_trk_uap    -- Load standard radar track UAP                           */
/*----------------------------------------------------------------------------*/

 static
 void load_trk_uap (void)
{
	int ix;

	for (ix = 0; ix <= M_MAX_FRN; ix ++)
	{
		std_uap[ix] = NULL;
	}

                   /* Load the standard (radar) track UAP: */

	std_uap[1] = &desc_i001_010;
	std_uap[2] = &desc_i001_020;
	std_uap[3] = &desc_i001_161;
	std_uap[4] = &desc_i001_040;
	std_uap[5] = &desc_i001_042;
	std_uap[6] = &desc_i001_200;
	std_uap[7] = &desc_i001_070;

	std_uap[8] = &desc_i001_090;
	std_uap[9] = &desc_i001_141;
	std_uap[10] = &desc_i001_130;
	std_uap[11] = &desc_i001_131;
	std_uap[12] = &desc_i001_120;
	std_uap[13] = &desc_i001_170;
	std_uap[14] = &desc_i001_210;

	std_uap[15] = &desc_i001_050;
	std_uap[16] = &desc_i001_080;
	std_uap[17] = &desc_i001_100;
	std_uap[18] = &desc_i001_060;
	std_uap[19] = &desc_i001_030;
	std_uap[20] = &desc_i001_spf;
	std_uap[21] = &desc_i001_rfs;

	std_uap[22] = &desc_i001_150;

	return;
}

/*----------------------------------------------------------------------------*/
/* proc_i001_010   -- Process I001/010 data item                              */
/*----------------------------------------------------------------------------*/

 static
 t_Retc proc_i001_010 (t_Ui16 length, t_Byte *buffer)
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
                   /* This is a feature specific for ASTERIX category 001
                      where radar target reports without SAC/SIC will
                      inherit this information from a preceding radar
                      service message or radar target report with SAC/SIC
                   */

				   /* Store this information: */
	rtgt.data_source_identifier.present = TRUE;
	rtgt.data_source_identifier.value = dsi;

				   /* Set the return code: */
	ret = RC_OKAY;

	return ret;
}

/*----------------------------------------------------------------------------*/
/* proc_i001_020   -- Process I001/020 data item                              */
/*----------------------------------------------------------------------------*/

 static
 t_Retc proc_i001_020 (t_Ui16 length, t_Byte *buffer)
{
	t_Byte dfo;    /* Data field octet */
	int ix;        /* Auxiliary */
	t_Retc ret;    /* Return code */

				   /* Preset the return code: */
	ret = RC_FAIL;

				   /* Check parameters: */
	Assert (length > 0, "Invalid parameter");
	Assert (buffer != NULL, "Invalid parameter");

                   /* Load respective User Application Profile: */
	dfo = buffer[0];
	if ((dfo & 0x80) == 0)
	{
		is_a_rtrk = FALSE;
		load_std_uap ();
	}
	else
	{
		is_a_rtrk = TRUE;
		load_trk_uap ();
	}

                   /* This is a specific feature of ASTERIX category 001 where
                      a bit in one particular data item decides on the User
                      Application Profile (UAP) to be used. This only works
                      because plot UAP and track UAP start with the same (two)
                      data items until they diverge
                   */

#if LISTER
				   /* List data field: */
	list_text (2, ";  Target Report Descriptor:");
	for (ix = 0; ix < length; ix ++)
	{
		dfo = buffer[ix];

		if (ix == 0)
		{
			t_Byte rdt;

			if (dfo & 0x80)
			{
				list_text (2, " TRK");
			}
			else
			{
				list_text (2, " PLT");
			}

			if (dfo & 0x40)
			{
				list_text (2, " SIM");
			}
			else
			{
				list_text (2, " ACT");
			}

			rdt = (dfo >> 4) & 0x03;
			if (rdt == 0)
			{
				list_text (2, " ---");
			}
			else if (rdt == 1)
			{
				list_text (2, " PRI");
			}
			else if (rdt == 2)
			{
				list_text (2, " SEC");
			}
			else
			{
				list_text (2, " CMB");
			}

			if (dfo & 0x08)
			{
				list_text (2, " A2");
			}
			else
			{
				list_text (2, " A1");
			}

			if (dfo & 0x04)
			{
				list_text (2, " SPI");
			}

			if (dfo & 0x02)
			{
				list_text (2, " FXD");
			}
		}
		else if (ix == 1)
		{
			t_Byte emg;

			if (dfo & 0x80)
			{
				list_text (2, " TST");
			}

			emg = (dfo >> 5) & 0x03;
			if (emg == 1)
			{
				list_text (2, " EM1");
			}
			else if (emg == 2)
			{
				list_text (2, " EM2");
			}
			else if (emg == 3)
			{
				list_text (2, " EM3");
			}

			if (dfo & 0x10)
			{
				list_text (2, " ME");
			}

			if (dfo & 0x08)
			{
				list_text (2, " MI");
			}
		}
	}
	list_text (2, "\n");
#endif /* LISTER */

				   /* Evaluate and store this information: */
	rtgt.detection_type.present = TRUE;
	for (ix = 0; ix < length; ix ++)
	{
    	dfo = buffer[ix];

		if (ix == 0)
		{
			rtgt.detection_type.is_radar_track = M_TRES (dfo & 0x80);

			rtgt.detection_type.simulated = M_TRES (dfo & 0x40);

			rtgt.detection_type.reported_from_ssr = M_TRES (dfo & 0x20);
			rtgt.detection_type.reported_from_psr = M_TRES (dfo & 0x10);

			rtgt.antenna_number.present = TRUE;
			if (dfo & 0x08)
			{
				rtgt.antenna_number.value = 2;
			}
			else
			{
				rtgt.antenna_number.value = 1;
			}

			rtgt.special_position_indication = M_TRES (dfo & 0x04);

			rtgt.detection_type.from_fixed_field_transponder =
				M_TRES (dfo & 0x02);
		}
		else if (ix == 1)
		{
			t_Byte emg;

			rtgt.detection_type.test_target = M_TRES (dfo & 0x80);

			emg = (dfo >> 5) & 0x03;
			rtgt.emergency_1 = M_TRES (emg == 1);
			rtgt.emergency_2 = M_TRES (emg == 2);
			rtgt.emergency_3 = M_TRES (emg == 3);

			rtgt.military_emergency = M_TRES (dfo & 0x10);

			rtgt.military_ident = M_TRES (dfo & 0x08);
		}
	}

				   /* Set the return code: */
	ret = RC_OKAY;

	return ret;
}

/*----------------------------------------------------------------------------*/
/* proc_i001_030   -- Process I001/030 data item                              */
/*----------------------------------------------------------------------------*/

 static
 t_Retc proc_i001_030 (t_Ui16 length, t_Byte *buffer)
{
	t_Ui16 cnt;    /* Count */
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
	list_text (2, ";  Warning/Error Conditions:");
	for (ix = 0; ix < length; ix ++)
	{
		dfo = buffer[ix];
		list_text (2, " " M_FMT_BYTE_HEX, dfo & ~0x01);
	}
	list_text (2, "\n");
#endif /* LISTER */

				   /* Store this information: */
	cnt = 0;
	for (ix = 0; ix < length; ix ++)
	{
		t_Byte wec;

		dfo = buffer[ix];
		wec = (dfo >> 1) & 0x7f;

		Assert (cnt < M_MAX_NUMBER_OF_WEC,
                "Too many warning/error conditions");

		rtgt.wec_list.list[cnt] = wec;
		++ cnt;
	}
	rtgt.wec_list.count =
		(cnt < M_MAX_NUMBER_OF_WEC ? cnt : M_MAX_NUMBER_OF_WEC);
	rtgt.wec_list.present = TRUE;

				   /* Set the return code: */
	ret = RC_OKAY;

	return ret;
}

/*----------------------------------------------------------------------------*/
/* proc_i001_040   -- Process I001/040 data item                              */
/*----------------------------------------------------------------------------*/

 static
 t_Retc proc_i001_040 (t_Ui16 length, t_Byte *buffer)
{
	t_Byte df1;    /* Data field octet 1 */
	t_Byte df2;    /* Data field octet 2 */
	t_Byte df3;    /* Data field octet 3 */
	t_Byte df4;    /* Data field octet 4 */
	t_Retc ret;    /* Return code */
	t_Ui16 rho;    /* Slant range; 1/128 nautical miles */
	t_Ui16 theta;  /* Azimuth; 360/2**16 degrees */

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
	rho = make_ui16 (df1, df2);
	theta = make_ui16 (df3, df4);

#if LISTER
				   /* List data field: */
	list_text (2, ";  Measured Position:");
	list_text (2, " rng=" M_FMT_UI16 " (%.3f nmi);",
                  rho, (1.0 / 128.0) * rho);
	list_text (2, " azm=" M_FMT_UI16 " (%.3f deg)",
                  theta, (360.0 / 65536.0) * theta);
	list_text (2, "\n");
#endif /* LISTER */

				   /* Store this information: */
	rtgt.measured_azm.present = TRUE;
	rtgt.measured_azm.value = (M_TWO_PI / 65536.0) * theta;
	rtgt.measured_rng.present = TRUE;
	rtgt.measured_rng.value = (M_NMI2MTR / 128.0) * rho;

				   /* Set the return code: */
	ret = RC_OKAY;

	return ret;
}

/*----------------------------------------------------------------------------*/
/* proc_i001_042   -- Process I001/042 data item                              */
/*----------------------------------------------------------------------------*/

 static
 t_Retc proc_i001_042 (t_Ui16 length, t_Byte *buffer)
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
	rtgt.computed_position.present = TRUE;
	rtgt.computed_position.value_x = (M_NMI2MTR / 64.0) * x;
	rtgt.computed_position.value_y = (M_NMI2MTR / 64.0) * y;

				   /* Set the return code: */
	ret = RC_OKAY;

	return ret;
}

/*----------------------------------------------------------------------------*/
/* proc_i001_050   -- Process I001/050 data item                              */
/*----------------------------------------------------------------------------*/

 static
 t_Retc proc_i001_050 (t_Ui16 length, t_Byte *buffer)
{
	t_Byte df1;    /* Data field octet 1 */
	t_Byte df2;    /* Data field octet 2 */
	t_Ui16 m2c;    /* Mode 2 code */
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
	m2c = make_ui16 (df1 & 0x0f, df2);

#if LISTER
				   /* List data field: */
	list_text (2, ";  Mode 2 Code:");
	list_text (2, " v=" M_FMT_BYTE ";", (df1 >> 7) & 0x01);
	list_text (2, " g=" M_FMT_BYTE ";", (df1 >> 6) & 0x01);
	list_text (2, " l=" M_FMT_BYTE ";", (df1 >> 5) & 0x01);
	list_text (2, " code=" M_FMT_CODE, m2c);
	list_text (2, "\n");
#endif /* LISTER */

				   /* Store this information: */
                   /* We do NOT store the information if
                      - the mode 2 code is zero and is marked as not valid
                   */
	if (m2c != 0 || (df1 & 0x80) == 0x00)
	{
		rtgt.mode_2_info.code = m2c;
		rtgt.mode_2_info.code_garbled = M_TRES (df1 & 0x40);
		rtgt.mode_2_info.code_invalid = M_TRES (df1 & 0x80);
		rtgt.mode_2_info.code_smoothed = M_TRES (df1 & 0x20);
		rtgt.mode_2_info.present = TRUE;
	}

				   /* Set the return code: */
	ret = RC_OKAY;

	return ret;
}

/*----------------------------------------------------------------------------*/
/* proc_i001_060   -- Process I001/060 data item                              */
/*----------------------------------------------------------------------------*/

 static
 t_Retc proc_i001_060 (t_Ui16 length, t_Byte *buffer)
{
	t_Byte df1;    /* Data field octet 1 */
	t_Byte df2;    /* Data field octet 2 */
	t_Ui16 m2cc;   /* Mode 2 code confidence */
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
	m2cc = make_ui16 (df1 & 0x0f, df2);

#if LISTER
				   /* List data field: */
	list_text (2, ";  Mode 2 Code Confidence:");
	list_text (2, " m2cc=" M_FMT_CODE, m2cc);
	list_text (2, "\n");
#endif /* LISTER */

				   /* Store this information: */
	if (rtgt.mode_2_info.present)
	{
		rtgt.mode_2_info.code_confidence = m2cc;
                   /* Not inverted, i. e. 0=high confidence */
		rtgt.mode_2_info.code_confidence_present = TRUE;
	}

				   /* Set the return code: */
	ret = RC_OKAY;

	return ret;
}

/*----------------------------------------------------------------------------*/
/* proc_i001_070   -- Process I001/070 data item                              */
/*----------------------------------------------------------------------------*/

 static
 t_Retc proc_i001_070 (t_Ui16 length, t_Byte *buffer)
{
	t_Byte df1;    /* Data field octet 1 */
	t_Byte df2;    /* Data field octet 2 */
	t_Ui16 m3c;    /* Mode 3/A code */
	t_Retc ret;    /* Return code */
	t_Bool store_it;
                   /* Store this information */

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
	list_text (2, " v=" M_FMT_BYTE ";", (df1 >> 7) & 0x01);
	list_text (2, " g=" M_FMT_BYTE ";", (df1 >> 6) & 0x01);
	list_text (2, " l=" M_FMT_BYTE ";", (df1 >> 5) & 0x01);
	list_text (2, " code=" M_FMT_CODE, m3c);
	list_text (2, "\n");
#endif /* LISTER */

                   /* Determine whether to store this information: */
                   /* We do NOT store the information if
                      - the mode 3/A code is zero and is not marked as valid
                   */
	store_it = TRUE;
	if (m3c == 0 && (df1 & 0x80) != 0x00)
	{
		store_it = FALSE;
	}

				   /* Store this information: */
	if (store_it)
	{
		rtgt.mode_3a_info.code = m3c;
		rtgt.mode_3a_info.code_garbled = M_TRES (df1 & 0x40);
		rtgt.mode_3a_info.code_invalid = M_TRES (df1 & 0x80);
		rtgt.mode_3a_info.code_smoothed = M_TRES (df1 & 0x20);
		rtgt.mode_3a_info.present = TRUE;
	}

				   /* Set the return code: */
	ret = RC_OKAY;

	return ret;
}

/*----------------------------------------------------------------------------*/
/* proc_i001_080   -- Process I001/080 data item                              */
/*----------------------------------------------------------------------------*/

 static
 t_Retc proc_i001_080 (t_Ui16 length, t_Byte *buffer)
{
	t_Byte df1;    /* Data field octet 1 */
	t_Byte df2;    /* Data field octet 2 */
	t_Ui16 m3cc;   /* Mode 3/A code confidence */
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
	m3cc = make_ui16 (df1 & 0x0f, df2);

#if LISTER
				   /* List data field: */
	list_text (2, ";  Mode 3/A Code Confidence:");
	list_text (2, " " M_FMT_CODE, m3cc);
	list_text (2, "\n");
#endif /* LISTER */

				   /* Store this information: */
	if (rtgt.mode_3a_info.present)
	{
		rtgt.mode_3a_info.code_confidence = m3cc;
                   /* Not inverted, i. e. 0=high confidence */
		rtgt.mode_3a_info.code_confidence_present = TRUE;
	}

				   /* Set the return code: */
	ret = RC_OKAY;

	return ret;
}

/*----------------------------------------------------------------------------*/
/* proc_i001_090   -- Process I001/090 data item                              */
/*----------------------------------------------------------------------------*/

 static
 t_Retc proc_i001_090 (t_Ui16 length, t_Byte *buffer)
{
	t_Byte df1;    /* Data field octet 1 */
	t_Byte df2;    /* Data field octet 2 */
	t_Si16 mch;    /* Mode C height; 25 feet */
	t_Retc ret;    /* Return code */
	t_Bool store_it;
                   /* Store this information */

				   /* Preset the return code: */
	ret = RC_FAIL;

				   /* Check parameters: */
	Assert (length == 2, "Invalid parameter");
	Assert (buffer != NULL, "Invalid parameter");

				   /* Extract octets: */
	df1 = buffer[0];
	df2 = buffer[1];

				   /* Extract information: */
	mch = make_si16 (df1 & 0x1f, df2);
	if (df1 & 0x20)
	{
		mch |= 0xe000;
                   /* Two's complement */
	}

#if LISTER
				   /* List data field: */
	list_text (2, ";  Mode C Code:");
	list_text (2, " v=" M_FMT_BYTE ";", (df1 >> 7) & 0x01);
	list_text (2, " g=" M_FMT_BYTE ";", (df1 >> 6) & 0x01);
	list_text (2, " code=" M_FMT_SI16 " (%.2f FL)", mch, 0.25 * mch);
	list_text (2, "\n");
#endif /* LISTER */

                   /* Determine whether to store this information: */
                   /* We do NOT store the information if
                      - the mode C code height is zero and is not marked as valid
                   */
	store_it = TRUE;
	if (mch == 0 && (df1 & 0x80) != 0x00)
	{
		store_it = FALSE;
	}

				   /* Store this information: */
	if (store_it)
	{
		rtgt.mode_c_height.garbled = M_TRES (df1 & 0x40);
		rtgt.mode_c_height.height_in_error = e_is_false;
		if (mch % 4)
		{
			rtgt.mode_c_height.in_25_feet = e_is_true;
		}
		rtgt.mode_c_height.invalid = M_TRES (df1 & 0x80);
		rtgt.mode_c_height.present = TRUE;
		rtgt.mode_c_height.value = 25.0 * M_FT2MTR * mch;
		rtgt.mode_c_height.value_in_feet = 25 * (t_Si32) mch;
	}

				   /* Set the return code: */
	ret = RC_OKAY;

	return ret;
}

/*----------------------------------------------------------------------------*/
/* proc_i001_100   -- Process I001/100 data item                              */
/*----------------------------------------------------------------------------*/

 static
 t_Retc proc_i001_100 (t_Ui16 length, t_Byte *buffer)
{
	t_Byte df1;    /* Data field octet 1 */
	t_Byte df2;    /* Data field octet 2 */
	t_Byte df3;    /* Data field octet 3 */
	t_Byte df4;    /* Data field octet 4 */
	t_Ui16 mcc;    /* Mode C code (in Gray notation) */
	t_Ui16 mccc;   /* Mode C code confidence */
	t_Retc ret;    /* Return code */

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
	mcc = make_ui16 (df1 & 0x0f, df2);
	mccc = make_ui16 (df3 & 0x0f, df4);

#if LISTER
				   /* List data field: */
	list_text (2, ";  Mode C (Gray) Code:");
	list_text (2, " v=" M_FMT_BYTE ";", (df1 >> 7) & 0x01);
	list_text (2, " g=" M_FMT_BYTE ";", (df1 >> 6) & 0x01);
	list_text (2, " code=" M_FMT_CODE ";", mcc);
	list_text (2, " conf=" M_FMT_CODE, mccc);
	list_text (2, "\n");
#endif /* LISTER */

				   /* Store this information: */
	rtgt.mode_c_code.code = mcc;
	rtgt.mode_c_code.code_confidence = mccc;
	rtgt.mode_c_code.code_confidence_present = TRUE;
	rtgt.mode_c_code.code_garbled = M_TRES (df1 & 0x40);
	rtgt.mode_c_code.code_invalid = M_TRES (df1 & 0x80);
	rtgt.mode_c_code.present = TRUE;

				   /* Set the return code: */
	ret = RC_OKAY;

	return ret;
}

/*----------------------------------------------------------------------------*/
/* proc_i001_120   -- Process I001/120 data item                              */
/*----------------------------------------------------------------------------*/

 static
 t_Retc proc_i001_120 (t_Ui16 length, t_Byte *buffer)
{
	t_Byte df1;    /* Data field octet 1 */
	t_Retc ret;    /* Return code */
	t_Si16 spd;    /* Radial speed; 1/256 nmi/second */

				   /* Preset the return code: */
	ret = RC_FAIL;

				   /* Check parameters: */
	Assert (length == 1, "Invalid parameter");
	Assert (buffer != NULL, "Invalid parameter");

				   /* Extract octet: */
	df1 = buffer[0];

				   /* Extract information: */
	spd = (t_Si16) (df1 & 0x7f);
	if (df1 & 0x80)
	{
		spd |= 0xff80;
                   /* If value is negative, sign bit is set; value in
                      two's complement */
	}

#if LISTER
				   /* List data field: */
	list_text (2, ";  Measured Radial Doppler Speed:");
	list_text (2, " " M_FMT_SI16 " (%.2f kts)", spd, (3600.0 / 256.0) * spd);
	list_text (2, "\n");
#endif /* LISTER */

				   /* Store this information: */
	rtgt.radial_speed.present = TRUE;
	rtgt.radial_speed.value = (M_NMI2MTR / 256.0) * spd;
                   /* Conversion factor is
                      (3600.0 / 256.0) * (M_NMI2MTR / 3600.0)
                   */

				   /* Set the return code: */
	ret = RC_OKAY;

	return ret;
}

/*----------------------------------------------------------------------------*/
/* proc_i001_130   -- Process I001/130 data item                              */
/*----------------------------------------------------------------------------*/

 static
 t_Retc proc_i001_130 (t_Ui16 length, t_Byte *buffer)
{
#if LISTER
	t_Byte df1;    /* Data field octet 1 */
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
	list_text (2, ";  Radar Plot Characteristics: 0x");
	for (ix = 0; ix < length; ix ++)
	{
		df1 = buffer[ix];
		list_text (2, " " M_FMT_BYTE_HEX, df1);
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
/* proc_i001_131   -- Process I001/131 data item                              */
/*----------------------------------------------------------------------------*/

 static
 t_Retc proc_i001_131 (t_Ui16 length, t_Byte *buffer)
{
	t_Byte df1;    /* Data field octet 1 */
	t_Si16 pwr;    /* Received power; dBm */
	t_Retc ret;    /* Return code */

				   /* Preset the return code: */
	ret = RC_FAIL;

				   /* Check parameters: */
	Assert (length == 1, "Invalid parameter");
	Assert (buffer != NULL, "Invalid parameter");

				   /* Extract octet: */
	df1 = buffer[0];

				   /* Extract information: */
	pwr = (t_Si16) (df1 & 0x7f);
	if (df1 & 0x80)
	{
		pwr |= 0xff80;
                   /* Two's complement */
	}
                   /* This encoding is compliant with the ASTERIX
                      standard
                   */

#if LISTER
				   /* List data field: */
	list_text (2, ";  Received Power:");
	list_text (2, " power=" M_FMT_SI16 " dBm", pwr);
	list_text (2, "\n");
#endif /* LISTER */

				   /* Store this information: */
	rtgt.received_power.present = TRUE;
	rtgt.received_power.value = pwr;

				   /* Set the return code: */
	ret = RC_OKAY;

	return ret;
}

/*----------------------------------------------------------------------------*/
/* proc_i001_141   -- Process I001/141 data item                              */
/*----------------------------------------------------------------------------*/

 static
 t_Retc proc_i001_141 (t_Ui16 length, t_Byte *buffer)
{
	t_Byte df1;    /* Data field octet 1 */
	t_Byte df2;    /* Data field octet 2 */
	t_Retc lrc;    /* Local return code */
	t_Retc ret;    /* Return code */
	t_Secs tm;     /* Time (of day); seconds */
	t_Ui32 tod;    /* (Full) time of day; 1/128 seconds */
	t_Ui16 ttod;   /* Truncated time of day; 1/128 seconds */

				   /* Preset the return code: */
	ret = RC_FAIL;

				   /* Check parameters: */
	Assert (length == 2, "Invalid parameter");
	Assert (buffer != NULL, "Invalid parameter");

				   /* Extract octets: */
	df1 = buffer[0];
	df2 = buffer[1];

				   /* Extract information: */
	ttod = make_ui16 (df1, df2);

                   /* Try to fill up this truncated time of day: */
	lrc = fillup_tod (ttod, &tod);
	if (lrc == RC_OKAY)
	{
		tm = (1.0 / 128.0) * tod;
	}

#if LISTER
				   /* List data field: */
	list_text (2, ";  Truncated Time of Day:");
	list_text (2, " 0x" M_FMT_UI16_HEX, ttod);
	list_text (2, "\n");
	if (lrc == RC_OKAY)
	{
		list_text (2, ";  Time of Day:");
		list_text (2, " " M_FMT_ATOD " (" M_FMT_UI32 "; %.6f secs; %s UTC)",
                      tod, tod, tm, utc_text (tm));
		list_text (2, "\n");
	}
#endif /* LISTER */

				   /* Store this information: */
	rtgt.detection_ptod.present = TRUE;
	rtgt.detection_ptod.value = ttod;
	if (lrc == RC_OKAY)
	{
		rtgt.detection_time.present = TRUE;
		rtgt.detection_time.value = tm;
	}

				   /* Set the return code: */
	ret = RC_OKAY;

	return ret;
}

/*----------------------------------------------------------------------------*/
/* proc_i001_150   -- Process I001/150 data item                              */
/*----------------------------------------------------------------------------*/

 static
 t_Retc proc_i001_150 (t_Ui16 length, t_Byte *buffer)
{
	t_Byte df1;    /* Data field octet 1 */
	t_Retc ret;    /* Return code */

				   /* Preset the return code: */
	ret = RC_FAIL;

				   /* Check parameters: */
	Assert (length == 1, "Invalid parameter");
	Assert (buffer != NULL, "Invalid parameter");

				   /* Extract octet: */
	df1 = buffer[0];

#if LISTER
				   /* List data field: */
	list_text (2, ";  X pulse:");
	list_text (2, " xa=" M_FMT_BYTE ";", (df1 >> 7) & 0x01);
	list_text (2, " xc=" M_FMT_BYTE ";", (df1 >> 5) & 0x01);
	list_text (2, " x2=" M_FMT_BYTE, (df1 >> 2) & 0x01);
	list_text (2, "\n");
#endif /* LISTER */

				   /* Store this information: */
	rtgt.x_pulses.present = TRUE;
	rtgt.x_pulses.value_m2 = M_TRES ((df1 >> 2) & 0x01);
	rtgt.x_pulses.value_m3a = M_TRES ((df1 >> 7) & 0x01);
	rtgt.x_pulses.value_mc = M_TRES ((df1 >> 5) & 0x01);

				   /* Set the return code: */
	ret = RC_OKAY;

	return ret;
}

/*----------------------------------------------------------------------------*/
/* proc_i001_161   -- Process I001/161 data item                              */
/*----------------------------------------------------------------------------*/

 static
 t_Retc proc_i001_161 (t_Ui16 length, t_Byte *buffer)
{
	t_Byte df1;    /* Data field octet 1 */
	t_Byte df2;    /* Data field octet 2 */
	t_Ui16 num;    /* Track/Plot number */
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
	num = make_ui16 (df1, df2);

#if LISTER
				   /* List data field: */
	list_text (2, ";  Track/Plot Number:");
	list_text (2, " num=" M_FMT_UI16, num);
	list_text (2, "\n");
#endif /* LISTER */

				   /* Store this information: */
	rtgt.track_number.present = TRUE;
	rtgt.track_number.value = num;

				   /* Set the return code: */
	ret = RC_OKAY;

	return ret;
}

/*----------------------------------------------------------------------------*/
/* proc_i001_170   -- Process I001/170 data item                              */
/*----------------------------------------------------------------------------*/

 static
 t_Retc proc_i001_170 (t_Ui16 length, t_Byte *buffer)
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
			if (dfo & 0x80)
			{
				list_text (2, " INI");
			}
			else
			{
				list_text (2, " CNF");
			}

			if (dfo & 0x40)
			{
				list_text (2, " SRT");
			}
			else
			{
				list_text (2, " PRT");
			}

			if (dfo & 0x20)
			{
				list_text (2, " MAN");
			}

			if (dfo & 0x10)
			{
				list_text (2, " DOU");
			}

			if (dfo & 0x02)
			{
				list_text (2, " GHO");
			}
		}
		else if (ix == 1)
		{
			if (dfo & 0x80)
			{
				list_text (2, " TRE");
			}
		}
	}
	list_text (2, "\n");
#endif /* LISTER */

				   /* Store this information: */
	rtgt.reported_track_status.present = TRUE;
	for (ix = 0; ix < length; ix ++)
	{
		dfo = buffer[ix];

		if (ix == 0)
		{
			rtgt.reported_track_status.confirmed = M_TRES ((dfo & 0x80) == 0);
			rtgt.reported_track_status.doubtful_association =
				M_TRES (dfo & 0x10);
			rtgt.reported_track_status.ghost = M_TRES (dfo & 0x02);
			rtgt.reported_track_status.horizontal_manoeuvre =
				M_TRES (dfo & 0x20);
			rtgt.reported_track_status.primary_track =
				M_TRES ((dfo & 0x40) == 0);
			rtgt.reported_track_status.secondary_track = M_TRES (dfo & 0x40);
		}
		else if (ix == 1)
		{
			rtgt.reported_track_status.tre = M_TRES (dfo & 0x80);
			if (dfo & 0x80)
			{
				rtgt.to_be_cancelled = e_is_true;
			}
		}
	}

				   /* Set the return code: */
	ret = RC_OKAY;

	return ret;
}

/*----------------------------------------------------------------------------*/
/* proc_i001_200   -- Process I001/200 data item                              */
/*----------------------------------------------------------------------------*/

 static
 t_Retc proc_i001_200 (t_Ui16 length, t_Byte *buffer)
{
	t_Byte df1;    /* Data field octet 1 */
	t_Byte df2;    /* Data field octet 2 */
	t_Byte df3;    /* Data field octet 3 */
	t_Byte df4;    /* Data field octet 4 */
	t_Ui16 hdg;    /* Calculated heading; 360/2**16 degrees */
	t_Retc ret;    /* Return code */
	t_Ui16 spd;    /* Calculated ground speed; 2**-14 nmi/second */

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
	spd = make_ui16 (df1 & 0x7f, df2);
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
	rtgt.ground_vector.present = TRUE;
	rtgt.ground_vector.value_gsp = (M_NMI2MTR / 16384.0) * spd;
	rtgt.ground_vector.value_hdg = (M_TWO_PI / 65536.0) * hdg;

				   /* Set the return code: */
	ret = RC_OKAY;

	return ret;
}

/*----------------------------------------------------------------------------*/
/* proc_i001_210   -- Process I001/210 data item                              */
/*----------------------------------------------------------------------------*/

 static
 t_Retc proc_i001_210 (t_Ui16 length, t_Byte *buffer)
{
#if LISTER
	t_Byte df1;    /* Data field octet 1 */
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
	list_text (2, ";  Track Quality:");
	for (ix = 0; ix < length; ix ++)
	{
		df1 = buffer[ix];
		list_text (2, " " M_FMT_BYTE, df1);
	}
	list_text (2, "\n");
#endif /* LISTER */

				   /* Store this information: */
	rtgt.reported_track_quality.length = length;
	rtgt.reported_track_quality.present = TRUE;
	if (length == 1)
	{
		rtgt.reported_track_quality.value = (t_Ui16) buffer[0];
	}
	else
	{
		rtgt.reported_track_quality.value = make_ui16 (buffer[0], buffer[1]);
	}

				   /* Set the return code: */
	ret = RC_OKAY;

	return ret;
}

/*----------------------------------------------------------------------------*/
/* proc_i001_rfs   -- Process RFS indicator data item                         */
/*----------------------------------------------------------------------------*/

 static
 t_Retc proc_i001_rfs (t_Ui16 length, t_Byte *buffer, t_Ui16 *pos_ptr)
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
	list_text (1, ";  I001/RFS: 0x " M_FMT_BYTE_HEX " ...\n", n);
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
		list_text (1, ";  I001/RFS: frn=" M_FMT_BYTE "\n", frn);
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

		lrc = data_item (1, frn, std_uap[frn],
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
/* proc_i001_spf   -- Process SPF indicator data item                         */
/*----------------------------------------------------------------------------*/

 static
 t_Retc proc_i001_spf (t_Ui16 length, t_Byte *buffer, t_Ui16 *pos_ptr)
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
	list_buffer (1, ";  I001/SPF:", len, buffer + pos);
#endif /* LISTER */

				   /* Set position: */
	*pos_ptr = pos + len;

				   /* Set the return code: */
	ret = RC_OKAY;

	done:          /* We are done */
	return ret;
}
/* end-of-file */
