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
/* File:           src/astx_048.cpp                                           */
/* Contents:       Process ASTERIX category 048 data block                    */
/* Author(s):      kb                                                         */
/* Last change:    2018-04-27                                                 */
/*----------------------------------------------------------------------------*/

/* Reference document:
   -------------------

   Eurocontrol Standard Document for Surveillance Data Exchange
   Part 4: Transmission of Monoradar Target Reports
   SUR.ET1.ST05.2000-STD-04-01

   Edition: 1.14
   Edition Date: November 2000
   Status: Proposed Standard

   Edition: 1.15
   Edition Date: April 2007
   Status: Released Issue

   Edition: 1.16
   Edition Date: March 2009
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
                      for ASTERIX category 048 */

                   /* Local texts: */
                   /* ------------ */

#if LISTER
static const char* iff_texts[4] =
                   /* Texts for IFF information in I048/020 */
{
	"No mode 4 interrogation",
	"Friendly target",
	"Unknown target",
	"No reply"
};
static const char* type_texts[8] =
                   /* Texts for target type in I048/020 */
{
	"No detection",
	"Single PSR detection",
	"Single SSR detection",
	"SSR+PSR detection",
	"Single ModeS All-Call",
	"Single ModeS Roll-Call",
	"ModeS All-Call + PSR",
	"ModeS Roll-Call + PSR"
};
static const char* wec_texts[19] =
                   /* Texts for warning/error conditions
                      in I048/030 */
{
	"Not defined; never used",
	"Multipath reply (reflection)",
	"Reply due to sidelobe interrogation/reception",
	"Split plot",
	"Second time around reply",
	"Angel",
	"Slow moving target correlated with road infrastructure",
	"Fixed PSR plot",
	"Slow PSR target",
	"Low quality PSR plot",
	"Phantom SSR plot",
	"Non-matching mode 3/A code",
	"Mode C code / mode S altitude code abnormal value",
	"Target in clutter area",
	"Maximum Doppler response in zero filter",
	"Transponder anomaly detected",
	"Duplicated or illegal mode S aircraft address",
	"Mode S error correction applied",
	"Undecodable mode C code / mode S altitude code"
};
#endif /* LISTER */

                   /* Local variables: */
                   /* ---------------- */

static t_Data_Item_Desc desc_i048_010;
                   /* Description of data item I048/010 */
static t_Data_Item_Desc desc_i048_020;
                   /* Description of data item I048/020 */
static t_Data_Item_Desc desc_i048_030;
                   /* Description of data item I048/030 */
static t_Data_Item_Desc desc_i048_040;
                   /* Description of data item I048/040 */
static t_Data_Item_Desc desc_i048_042;
                   /* Description of data item I048/042 */
static t_Data_Item_Desc desc_i048_050;
                   /* Description of data item I048/050 */
static t_Data_Item_Desc desc_i048_055;
                   /* Description of data item I048/055 */
static t_Data_Item_Desc desc_i048_060;
                   /* Description of data item I048/060 */
static t_Data_Item_Desc desc_i048_065;
                   /* Description of data item I048/065 */
static t_Data_Item_Desc desc_i048_070;
                   /* Description of data item I048/070 */
static t_Data_Item_Desc desc_i048_080;
                   /* Description of data item I048/080 */
static t_Data_Item_Desc desc_i048_090;
                   /* Description of data item I048/090 */
static t_Data_Item_Desc desc_i048_100;
                   /* Description of data item I048/100 */
static t_Data_Item_Desc desc_i048_110;
                   /* Description of data item I048/110 */
static t_Data_Item_Desc desc_i048_120;
                   /* Description of data item I048/120 */
static t_Data_Item_Desc desc_i048_130;
                   /* Description of data item I048/130 */
static t_Data_Item_Desc desc_i048_140;
                   /* Description of data item I048/140 */
static t_Data_Item_Desc desc_i048_161;
                   /* Description of data item I048/161 */
static t_Data_Item_Desc desc_i048_170;
                   /* Description of data item I048/170 */
static t_Data_Item_Desc desc_i048_200;
                   /* Description of data item I048/200 */
static t_Data_Item_Desc desc_i048_210;
                   /* Description of data item I048/210 */
static t_Data_Item_Desc desc_i048_220;
                   /* Description of data item I048/220 */
static t_Data_Item_Desc desc_i048_230;
                   /* Description of data item I048/230 */
static t_Data_Item_Desc desc_i048_240;
                   /* Description of data item I048/240 */
static t_Data_Item_Desc desc_i048_250;
                   /* Description of data item I048/250 */
static t_Data_Item_Desc desc_i048_260;
                   /* Description of data item I048/260 */
static t_Data_Item_Desc desc_i048_ref;
                   /* Description of REF data item */
static t_Data_Item_Desc desc_i048_spf;
                   /* Description of SPF data item */
static t_Bool initiated = FALSE;
				   /* Package initiated flag */
static t_Ui16 reference_vsn = 2;
                   /* Current version of reference document */
                   /* Implemented editions:
                      1 ... 1.14 (November 2000)
                      2 ... 1.15 (April 2007)
                      3 ... 1.16 (March 2009)
                   */
static t_Data_Item_Desc *std_uap[M_MAX_FRN + 1];
                   /* Standard User Application Profile */

				   /* Local functions: */
				   /* ---------------- */

static void init_desc (void);
                   /* Initiate data item descriptions */
static void load_std_uap (void);
                   /* Load standard UAP */
static t_Retc proc_i048_010 (t_Ui16 length, t_Byte *buffer);
                   /* Process I048/010 data item */
static t_Retc proc_i048_020 (t_Ui16 length, t_Byte *buffer);
                   /* Process I048/020 data item */
static t_Retc proc_i048_030 (t_Ui16 length, t_Byte *buffer);
                   /* Process I048/030 data item */
static t_Retc proc_i048_040 (t_Ui16 length, t_Byte *buffer);
                   /* Process I048/040 data item */
static t_Retc proc_i048_042 (t_Ui16 length, t_Byte *buffer);
                   /* Process I048/042 data item */
static t_Retc proc_i048_050 (t_Ui16 length, t_Byte *buffer);
                   /* Process I048/050 data item */
static t_Retc proc_i048_055 (t_Ui16 length, t_Byte *buffer);
                   /* Process I048/055 data item */
static t_Retc proc_i048_060 (t_Ui16 length, t_Byte *buffer);
                   /* Process I048/060 data item */
static t_Retc proc_i048_065 (t_Ui16 length, t_Byte *buffer);
                   /* Process I048/065 data item */
static t_Retc proc_i048_070 (t_Ui16 length, t_Byte *buffer);
                   /* Process I048/070 data item */
static t_Retc proc_i048_080 (t_Ui16 length, t_Byte *buffer);
                   /* Process I048/080 data item */
static t_Retc proc_i048_090 (t_Ui16 length, t_Byte *buffer);
                   /* Process I048/090 data item */
static t_Retc proc_i048_100 (t_Ui16 length, t_Byte *buffer);
                   /* Process I048/100 data item */
static t_Retc proc_i048_110 (t_Ui16 length, t_Byte *buffer);
                   /* Process I048/110 data item */
static t_Retc proc_i048_120 (t_Ui16 length, t_Byte *buffer, t_Ui16 *pos_ptr);
                   /* Process I048/120 data item */
static t_Retc proc_i048_130 (t_Ui16 length, t_Byte *buffer, t_Ui16 *pos_ptr);
                   /* Process I048/130 data item */
static t_Retc proc_i048_140 (t_Ui16 length, t_Byte *buffer);
                   /* Process I048/140 data item */
static t_Retc proc_i048_161 (t_Ui16 length, t_Byte *buffer);
                   /* Process I048/161 data item */
static t_Retc proc_i048_170 (t_Ui16 length, t_Byte *buffer);
                   /* Process I048/170 data item */
static t_Retc proc_i048_200 (t_Ui16 length, t_Byte *buffer);
                   /* Process I048/200 data item */
static t_Retc proc_i048_210 (t_Ui16 length, t_Byte *buffer);
                   /* Process I048/210 data item */
static t_Retc proc_i048_220 (t_Ui16 length, t_Byte *buffer);
                   /* Process I048/220 data item */
static t_Retc proc_i048_230 (t_Ui16 length, t_Byte *buffer);
                   /* Process I048/230 data item */
static t_Retc proc_i048_240 (t_Ui16 length, t_Byte *buffer);
                   /* Process I048/240 data item */
static t_Retc proc_i048_250 (t_Ui16 length, t_Byte *buffer);
                   /* Process I048/250 data item */
static t_Retc proc_i048_260 (t_Ui16 length, t_Byte *buffer);
                   /* Process I048/260 data item */
static t_Retc proc_i048_ref (t_Ui16 length, t_Byte *buffer, t_Ui16 *pos_ptr);
                   /* Process REF indicator data item */
static t_Retc proc_i048_spf (t_Ui16 length, t_Byte *buffer, t_Ui16 *pos_ptr);
                   /* Process SPF indicator data item */

/*----------------------------------------------------------------------------*/
/* astx_048        -- Process ASTERIX category 048 data block                 */
/*----------------------------------------------------------------------------*/

 t_Retc astx_048 (t_Ui16 length, t_Byte *buffer)
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

                   /* Preset radar target report: */
		memset (&rtgt, 0, sizeof (rtgt));

                   /* Set data format: */
		rtgt.asterix_category = 48;
		rtgt.data_format = e_data_format_asterix;

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

					lrc = data_item (48, frn, std_uap[frn],
                                     length, buffer, &pos);
					if (lrc != RC_OKAY)
					{
						error_msg ("Invalid data item.");
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
                   /* Description of data item I048/010: */
	desc_i048_010.category = 48;
	desc_i048_010.data_item = 10;
	desc_i048_010.item_type = e_fixed_length_data_item;
	desc_i048_010.fixed_length = 2;
	desc_i048_010.proc_fptr = proc_i048_010;
	desc_i048_010.read_fptr = NULL;

                   /* Description of data item I048/020: */
	desc_i048_020.category = 48;
	desc_i048_020.data_item = 20;
	desc_i048_020.item_type = e_variable_length_data_item;
	desc_i048_020.fixed_length = 0;
	desc_i048_020.proc_fptr = proc_i048_020;
	desc_i048_020.read_fptr = NULL;

                   /* Description of data item I048/030: */
	desc_i048_030.category = 48;
	desc_i048_030.data_item = 30;
	desc_i048_030.item_type = e_variable_length_data_item;
	desc_i048_030.fixed_length = 0;
	desc_i048_030.proc_fptr = proc_i048_030;
	desc_i048_030.read_fptr = NULL;

                   /* Description of data item I048/040: */
	desc_i048_040.category = 48;
	desc_i048_040.data_item = 40;
	desc_i048_040.item_type = e_fixed_length_data_item;
	desc_i048_040.fixed_length = 4;
	desc_i048_040.proc_fptr = proc_i048_040;
	desc_i048_040.read_fptr = NULL;

                   /* Description of data item I048/042: */
	desc_i048_042.category = 48;
	desc_i048_042.data_item = 42;
	desc_i048_042.item_type = e_fixed_length_data_item;
	desc_i048_042.fixed_length = 4;
	desc_i048_042.proc_fptr = proc_i048_042;
	desc_i048_042.read_fptr = NULL;

                   /* Description of data item I048/050: */
	desc_i048_050.category = 48;
	desc_i048_050.data_item = 50;
	desc_i048_050.item_type = e_fixed_length_data_item;
	desc_i048_050.fixed_length = 2;
	desc_i048_050.proc_fptr = proc_i048_050;
	desc_i048_050.read_fptr = NULL;

                   /* Description of data item I048/055: */
	desc_i048_055.category = 48;
	desc_i048_055.data_item = 55;
	desc_i048_055.item_type = e_fixed_length_data_item;
	desc_i048_055.fixed_length = 1;
	desc_i048_055.proc_fptr = proc_i048_055;
	desc_i048_055.read_fptr = NULL;

                   /* Description of data item I048/060: */
	desc_i048_060.category = 48;
	desc_i048_060.data_item = 60;
	desc_i048_060.item_type = e_fixed_length_data_item;
	desc_i048_060.fixed_length = 2;
	desc_i048_060.proc_fptr = proc_i048_060;
	desc_i048_060.read_fptr = NULL;

                   /* Description of data item I048/065: */
	desc_i048_065.category = 48;
	desc_i048_065.data_item = 65;
	desc_i048_065.item_type = e_fixed_length_data_item;
	desc_i048_065.fixed_length = 1;
	desc_i048_065.proc_fptr = proc_i048_065;
	desc_i048_065.read_fptr = NULL;

                   /* Description of data item I048/070: */
	desc_i048_070.category = 48;
	desc_i048_070.data_item = 70;
	desc_i048_070.item_type = e_fixed_length_data_item;
	desc_i048_070.fixed_length = 2;
	desc_i048_070.proc_fptr = proc_i048_070;
	desc_i048_070.read_fptr = NULL;

                   /* Description of data item I048/080: */
	desc_i048_080.category = 48;
	desc_i048_080.data_item = 80;
	desc_i048_080.item_type = e_fixed_length_data_item;
	desc_i048_080.fixed_length = 2;
	desc_i048_080.proc_fptr = proc_i048_080;
	desc_i048_080.read_fptr = NULL;

                   /* Description of data item I048/090: */
	desc_i048_090.category = 48;
	desc_i048_090.data_item = 90;
	desc_i048_090.item_type = e_fixed_length_data_item;
	desc_i048_090.fixed_length = 2;
	desc_i048_090.proc_fptr = proc_i048_090;
	desc_i048_090.read_fptr = NULL;

                   /* Description of data item I048/100: */
	desc_i048_100.category = 48;
	desc_i048_100.data_item = 100;
	desc_i048_100.item_type = e_fixed_length_data_item;
	desc_i048_100.fixed_length = 4;
	desc_i048_100.proc_fptr = proc_i048_100;
	desc_i048_100.read_fptr = NULL;

                   /* Description of data item I048/110: */
	desc_i048_110.category = 48;
	desc_i048_110.data_item = 110;
	desc_i048_110.item_type = e_fixed_length_data_item;
	desc_i048_110.fixed_length = 2;
	desc_i048_110.proc_fptr = proc_i048_110;
	desc_i048_110.read_fptr = NULL;

                   /* Description of data item I048/120: */
	desc_i048_120.category = 48;
	desc_i048_120.data_item = 120;
	desc_i048_120.item_type = e_immediate_data_item;
	desc_i048_120.fixed_length = 0;
	desc_i048_120.proc_fptr = NULL;
	desc_i048_120.read_fptr = proc_i048_120;

                   /* Description of data item I048/130: */
	desc_i048_130.category = 48;
	desc_i048_130.data_item = 130;
	desc_i048_130.item_type = e_immediate_data_item;
	desc_i048_130.fixed_length = 0;
	desc_i048_130.proc_fptr = NULL;
	desc_i048_130.read_fptr = proc_i048_130;

                   /* Description of data item I048/140: */
	desc_i048_140.category = 48;
	desc_i048_140.data_item = 140;
	desc_i048_140.item_type = e_fixed_length_data_item;
	desc_i048_140.fixed_length = 3;
	desc_i048_140.proc_fptr = proc_i048_140;
	desc_i048_140.read_fptr = NULL;

                   /* Description of data item I048/161: */
	desc_i048_161.category = 48;
	desc_i048_161.data_item = 161;
	desc_i048_161.item_type = e_fixed_length_data_item;
	desc_i048_161.fixed_length = 2;
	desc_i048_161.proc_fptr = proc_i048_161;
	desc_i048_161.read_fptr = NULL;

                   /* Description of data item I048/170: */
	desc_i048_170.category = 48;
	desc_i048_170.data_item = 170;
	desc_i048_170.item_type = e_variable_length_data_item;
	desc_i048_170.fixed_length = 0;
	desc_i048_170.proc_fptr = proc_i048_170;
	desc_i048_170.read_fptr = NULL;

                   /* Description of data item I048/200: */
	desc_i048_200.category = 48;
	desc_i048_200.data_item = 200;
	desc_i048_200.item_type = e_fixed_length_data_item;
	desc_i048_200.fixed_length = 4;
	desc_i048_200.proc_fptr = proc_i048_200;
	desc_i048_200.read_fptr = NULL;

                   /* Description of data item I048/210: */
	desc_i048_210.category = 48;
	desc_i048_210.data_item = 210;
	desc_i048_210.item_type = e_fixed_length_data_item;
	desc_i048_210.fixed_length = 4;
	desc_i048_210.proc_fptr = proc_i048_210;
	desc_i048_210.read_fptr = NULL;

                   /* Description of data item I048/220: */
	desc_i048_220.category = 48;
	desc_i048_220.data_item = 220;
	desc_i048_220.item_type = e_fixed_length_data_item;
	desc_i048_220.fixed_length = 3;
	desc_i048_220.proc_fptr = proc_i048_220;
	desc_i048_220.read_fptr = NULL;

                   /* Description of data item I048/230: */
	desc_i048_230.category = 48;
	desc_i048_230.data_item = 230;
	desc_i048_230.item_type = e_fixed_length_data_item;
	desc_i048_230.fixed_length = 2;
	desc_i048_230.proc_fptr = proc_i048_230;
	desc_i048_230.read_fptr = NULL;

                   /* Description of data item I048/240: */
	desc_i048_240.category = 48;
	desc_i048_240.data_item = 240;
	desc_i048_240.item_type = e_fixed_length_data_item;
	desc_i048_240.fixed_length = 6;
	desc_i048_240.proc_fptr = proc_i048_240;
	desc_i048_240.read_fptr = NULL;

                   /* Description of data item I048/250: */
	desc_i048_250.category = 48;
	desc_i048_250.data_item = 250;
	desc_i048_250.item_type = e_repetitive_data_item;
	desc_i048_250.fixed_length = 8;
	desc_i048_250.proc_fptr = proc_i048_250;
	desc_i048_250.read_fptr = NULL;

                   /* Description of data item I048/260: */
	desc_i048_260.category = 48;
	desc_i048_260.data_item = 260;
	desc_i048_260.item_type = e_fixed_length_data_item;
	desc_i048_260.fixed_length = 7;
	desc_i048_260.proc_fptr = proc_i048_260;
	desc_i048_260.read_fptr = NULL;

                   /* Description of data item I048/REF: */
	desc_i048_ref.category = 48;
	desc_i048_ref.data_item = M_REF_INDICATOR;
	desc_i048_ref.item_type = e_immediate_data_item;
	desc_i048_ref.fixed_length = 0;
	desc_i048_ref.proc_fptr = NULL;
	desc_i048_ref.read_fptr = proc_i048_ref;

                   /* Description of data item I048/SPF: */
	desc_i048_spf.category = 48;
	desc_i048_spf.data_item = M_SPF_INDICATOR;
	desc_i048_spf.item_type = e_immediate_data_item;
	desc_i048_spf.fixed_length = 0;
	desc_i048_spf.proc_fptr = NULL;
	desc_i048_spf.read_fptr = proc_i048_spf;

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

                   /* This is the track UAP: */

	std_uap[1] = &desc_i048_010;
    std_uap[2] = &desc_i048_140;
	std_uap[3] = &desc_i048_020;
	std_uap[4] = &desc_i048_040;
	std_uap[5] = &desc_i048_070;
	std_uap[6] = &desc_i048_090;
	std_uap[7] = &desc_i048_130;

	std_uap[8] = &desc_i048_220;
	std_uap[9] = &desc_i048_240;
	std_uap[10] = &desc_i048_250;
	std_uap[11] = &desc_i048_161;
	std_uap[12] = &desc_i048_042;
	std_uap[13] = &desc_i048_200;
	std_uap[14] = &desc_i048_170;

	std_uap[15] = &desc_i048_210;
	std_uap[16] = &desc_i048_030;
	std_uap[17] = &desc_i048_080;
	std_uap[18] = &desc_i048_100;
	std_uap[19] = &desc_i048_110;
	std_uap[20] = &desc_i048_120;
	std_uap[21] = &desc_i048_230;

	std_uap[22] = &desc_i048_260;
	std_uap[23] = &desc_i048_055;
	std_uap[24] = &desc_i048_050;
	std_uap[25] = &desc_i048_065;
	std_uap[26] = &desc_i048_060;
	std_uap[27] = &desc_i048_spf;
	std_uap[28] = &desc_i048_ref;

	return;
}

/*----------------------------------------------------------------------------*/
/* proc_i048_010   -- Process I048/010 data item                              */
/*----------------------------------------------------------------------------*/

 static
 t_Retc proc_i048_010 (t_Ui16 length, t_Byte *buffer)
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
	rtgt.data_source_identifier.present = TRUE;
	rtgt.data_source_identifier.value = dsi;

				   /* Set the return code: */
	ret = RC_OKAY;

	return ret;
}

/*----------------------------------------------------------------------------*/
/* proc_i048_020   -- Process I048/020 data item                              */
/*----------------------------------------------------------------------------*/

 static
 t_Retc proc_i048_020 (t_Ui16 length, t_Byte *buffer)
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
	list_text (2, ";  Target Report Descriptor:");
	for (ix = 0; ix < length; ix ++)
	{
		dfo = buffer[ix];

		if (ix == 0)
		{
			t_Byte typ;

			typ = (dfo >> 5) & 0x07;
			list_text (2, " TYP='%s'", type_texts[typ]);

			if (dfo & 0x10)
			{
				list_text (2, " SIM");
			}
			else
			{
				list_text (2, " ACT");
			}

			if (dfo & 0x08)
			{
				list_text (2, " RDP-2");
			}
			else
			{
				list_text (2, " RDP-1");
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
			t_Byte iff;

			if (dfo & 0x80)
			{
				list_text (2, " TST");
			}

			if (dfo & 0x10)
			{
				list_text (2, " ME");
			}

			if (dfo & 0x08)
			{
				list_text (2, " MI");
			}

			iff = (dfo >> 1) & 0x03;
			if (iff)
			{
				list_text (2, " IFF=" M_FMT_BYTE " (%s)",
                              iff, iff_texts[iff]);
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
			t_Byte typ;

			typ = (dfo >> 5) & 0x07;
			if (typ == 1)
			{
				rtgt.detection_type.reported_from_psr = e_is_true;
			}
			else if (typ == 2)
			{
				rtgt.detection_type.reported_from_ssr = e_is_true;
			}
			else if (typ == 3)
			{
				rtgt.detection_type.reported_from_psr = e_is_true;
				rtgt.detection_type.reported_from_ssr = e_is_true;
			}
			else if (typ == 4)
			{
				rtgt.detection_type.reported_from_ssr = e_is_true;
				rtgt.detection_type.reported_from_mds = e_is_true;
				rtgt.detection_type.mode_s_all_call = e_is_true;
			}
			else if (typ == 5)
			{
				rtgt.detection_type.reported_from_ssr = e_is_true;
				rtgt.detection_type.reported_from_mds = e_is_true;
				rtgt.detection_type.mode_s_roll_call = e_is_true;
			}
			else if (typ == 6)
			{
				rtgt.detection_type.reported_from_psr = e_is_true;
				rtgt.detection_type.reported_from_ssr = e_is_true;
				rtgt.detection_type.reported_from_mds = e_is_true;
				rtgt.detection_type.mode_s_all_call = e_is_true;
			}
			else if (typ == 7)
			{
				rtgt.detection_type.reported_from_psr = e_is_true;
				rtgt.detection_type.reported_from_ssr = e_is_true;
				rtgt.detection_type.reported_from_mds = e_is_true;
				rtgt.detection_type.mode_s_roll_call = e_is_true;
			}

			rtgt.detection_type.simulated = M_TRES (dfo & 0x10);

			if (dfo & 0x08)
			{
				rtgt.from_rdp_chain_2 = TRUE;
			}
			else
			{
				rtgt.from_rdp_chain_1 = TRUE;
			}

			rtgt.special_position_indication = M_TRES (dfo & 0x04);

			rtgt.detection_type.from_fixed_field_transponder =
				M_TRES (dfo & 0x02);
		}
		else if (ix == 1)
		{
			t_Byte iff;

			rtgt.detection_type.test_target = M_TRES (dfo & 0x80);

			rtgt.military_emergency = M_TRES (dfo & 0x10);

			rtgt.military_ident = M_TRES (dfo & 0x08);

			iff = (dfo >> 1) & 0x03;
			if (iff)
			{
				rtgt.mode_4_info.present = TRUE;
				rtgt.mode_4_info.value = iff;
			}
		}
	}

				   /* Set the return code: */
	ret = RC_OKAY;

	return ret;
}

/*----------------------------------------------------------------------------*/
/* proc_i048_030   -- Process I048/030 data item                              */
/*----------------------------------------------------------------------------*/

 static
 t_Retc proc_i048_030 (t_Ui16 length, t_Byte *buffer)
{
	t_Byte dfo;    /* Data field octet */
	int ix;        /* Auxiliary */
	t_Ui16 inx;    /* Index into list of warning/error conditions */
	t_Retc ret;    /* Return code */
	t_Byte wec;    /* Warning/error condition */


				   /* Preset the return code: */
	ret = RC_FAIL;

				   /* Check parameters: */
	Assert (length > 0, "Invalid parameter");
	Assert (buffer != NULL, "Invalid parameter");

#if LISTER
				   /* List data field: */
	list_text (2, ";  Warning/Error Conditions:\n");
	for (ix = 0; ix < length; ix ++)
	{
		dfo = buffer[ix];

		wec = (dfo >> 1) & 0x7f;

		list_text (2, ";   " M_FMT_BYTE, wec);
		if (1 <= wec && wec <= 18)
		{
			list_text (2, " (%s)", wec_texts[wec]);
		}
		list_text (2, "\n");
	}
#endif /* LISTER */

				   /* Store this information: */
	inx = 0;
	for (ix = 0; ix < length; ix ++)
	{
		dfo = buffer[ix];

		wec = (dfo >> 1) & 0x7f;

		if (wec == 100)
		{
			rtgt.detection_type.is_raw_plot = e_is_true;
                   /* This is an EADS MSSR 2000 I extension */
		}
		else
		{
			Assert (inx < M_MAX_NUMBER_OF_WEC,
                    "Too many warning/error conditions");

			rtgt.wec_list.list[inx] = wec;
			++ inx;
		}
	}
	rtgt.wec_list.count =
		(inx < M_MAX_NUMBER_OF_WEC ? inx : M_MAX_NUMBER_OF_WEC);
	rtgt.wec_list.present = TRUE;

				   /* Set the return code: */
	ret = RC_OKAY;

	return ret;
}

/*----------------------------------------------------------------------------*/
/* proc_i048_040   -- Process I048/040 data item                              */
/*----------------------------------------------------------------------------*/

 static
 t_Retc proc_i048_040 (t_Ui16 length, t_Byte *buffer)
{
	t_Byte df1;    /* Data field octet 1 */
	t_Byte df2;    /* Data field octet 2 */
	t_Byte df3;    /* Data field octet 3 */
	t_Byte df4;    /* Data field octet 4 */
	t_Retc ret;    /* Return code */
	t_Ui16 rho;    /* Slant range; 1/256 nautical miles */
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
	list_text (2, " srg=" M_FMT_UI16 " (%.3f nmi);",
                  rho, (1.0 / 256.0) * rho);
	list_text (2, " azm=" M_FMT_UI16 " (%.3f deg)",
                  theta, (360.0 / 65536.0) * theta);
	list_text (2, "\n");
#endif /* LISTER */

				   /* Store this information: */
	rtgt.measured_azm.present = TRUE;
	rtgt.measured_azm.value = (M_TWO_PI / 65536.0) * theta;
                   /* In radians */
	rtgt.measured_rng.present = TRUE;
	rtgt.measured_rng.value = (M_NMI2MTR / 256.0) * rho;
                   /* In metres */

				   /* Set the return code: */
	ret = RC_OKAY;

	return ret;
}

/*----------------------------------------------------------------------------*/
/* proc_i048_042   -- Process I048/042 data item                              */
/*----------------------------------------------------------------------------*/

 static
 t_Retc proc_i048_042 (t_Ui16 length, t_Byte *buffer)
{
	t_Byte df1;    /* Data field octet 1 */
	t_Byte df2;    /* Data field octet 2 */
	t_Byte df3;    /* Data field octet 3 */
	t_Byte df4;    /* Data field octet 4 */
	t_Retc ret;    /* Return code */
	t_Si16 x;      /* x component; 1/128 nautical miles */
	t_Si16 y;      /* y component; 1/128 nautical miles */

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
	list_text (2, " x=" M_FMT_SI16 " (%.3f nmi); ", x, (1.0 / 128.0) * x);
	list_text (2, " y=" M_FMT_SI16 " (%.3f nmi)", y, (1.0 / 128.0) * y);
	list_text (2, "\n");
#endif /* LISTER */

				   /* Store this information: */
	rtgt.computed_position.present = TRUE;
	rtgt.computed_position.value_x = (M_NMI2MTR / 128.0) * x;
	rtgt.computed_position.value_y = (M_NMI2MTR / 128.0) * y;
                   /* In metres */

				   /* Set the return code: */
	ret = RC_OKAY;

	return ret;
}

/*----------------------------------------------------------------------------*/
/* proc_i048_050   -- Process I048/050 data item                              */
/*----------------------------------------------------------------------------*/

 static
 t_Retc proc_i048_050 (t_Ui16 length, t_Byte *buffer)
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
/* proc_i048_055   -- Process I048/055 data item                              */
/*----------------------------------------------------------------------------*/

 static
 t_Retc proc_i048_055 (t_Ui16 length, t_Byte *buffer)
{
	t_Byte df1;    /* Data field octet 1 */
	t_Byte m1c;    /* Mode 1 code */
	t_Ui16 m1c_expanded;
                   /* Expanded mode 1 code */
	t_Retc ret;    /* Return code */
	t_Ui16 u16;    /* Auxiliary */

				   /* Preset the return code: */
	ret = RC_FAIL;

				   /* Check parameters: */
	Assert (length == 1, "Invalid parameter");
	Assert (buffer != NULL, "Invalid parameter");

				   /* Extract octets: */
	df1 = buffer[0];

				   /* Extract information: */
	m1c = df1 & 0x1f;

                   /* Expand to usual sequence of data pulses: */
	u16 = (t_Ui16) ((m1c >> 2) & 0x07);
	m1c_expanded = (u16 << 9);
	u16 = (t_Ui16) (m1c & 0x03);
	m1c_expanded |= (u16 << 6);

#if LISTER
				   /* List data field: */
	list_text (2, ";  Mode 1 Code:");
	list_text (2, " v=" M_FMT_BYTE ";", (df1 >> 7) & 0x01);
	list_text (2, " g=" M_FMT_BYTE ";", (df1 >> 6) & 0x01);
	list_text (2, " l=" M_FMT_BYTE ";", (df1 >> 5) & 0x01);
	list_text (2, " code=0%02o (" M_FMT_CODE ")", m1c, m1c_expanded);
	list_text (2, "\n");
#endif /* LISTER */

				   /* Store this information: */
	rtgt.mode_1_info.code = m1c_expanded;
	rtgt.mode_1_info.code_garbled = M_TRES (df1 & 0x40);
	rtgt.mode_1_info.code_invalid = M_TRES (df1 & 0x80);
	rtgt.mode_1_info.code_smoothed = M_TRES (df1 & 0x20);
	rtgt.mode_1_info.present = TRUE;

				   /* Set the return code: */
	ret = RC_OKAY;

	return ret;
}

/*----------------------------------------------------------------------------*/
/* proc_i048_060   -- Process I048/060 data item                              */
/*----------------------------------------------------------------------------*/

 static
 t_Retc proc_i048_060 (t_Ui16 length, t_Byte *buffer)
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
/* proc_i048_065   -- Process I048/065 data item                              */
/*----------------------------------------------------------------------------*/

 static
 t_Retc proc_i048_065 (t_Ui16 length, t_Byte *buffer)
{
	t_Byte df1;    /* Data field octet 1 */
	t_Byte m1cc;   /* Mode 1 code confidence */
	t_Ui16 m1cc_expanded;
                   /* Expanded mode 1 code confidence */
	t_Retc ret;    /* Return code */
	t_Ui16 u16;    /* Auxiliary */

				   /* Preset the return code: */
	ret = RC_FAIL;

				   /* Check parameters: */
	Assert (length == 1, "Invalid parameter");
	Assert (buffer != NULL, "Invalid parameter");

				   /* Extract octets: */
	df1 = buffer[0];

				   /* Extract information: */
	m1cc = df1 & 0x1f;

                   /* Expand to usual sequence of data pulses: */
	u16 = (t_Ui16) ((m1cc >> 2) & 0x07);
	m1cc_expanded = (u16 << 9);
	u16 = (t_Ui16) (m1cc & 0x03);
	m1cc_expanded |= (u16 << 6);

#if LISTER
				   /* List data field: */
	list_text (2, ";  Mode 1 Code Confidence:");
	list_text (2, " 0%02o (" M_FMT_CODE ")", m1cc, m1cc_expanded);
	list_text (2, "\n");
#endif /* LISTER */

				   /* Store this information: */
	if (rtgt.mode_1_info.present)
	{
		rtgt.mode_1_info.code_confidence = m1cc_expanded;
                   /* Not inverted, i. e. 0=high confidence */
		rtgt.mode_1_info.code_confidence_present = TRUE;
	}

				   /* Set the return code: */
	ret = RC_OKAY;

	return ret;
}

/*----------------------------------------------------------------------------*/
/* proc_i048_070   -- Process I048/070 data item                              */
/*----------------------------------------------------------------------------*/

 static
 t_Retc proc_i048_070 (t_Ui16 length, t_Byte *buffer)
{
	t_Byte df1;    /* Data field octet 1 */
	t_Byte df2;    /* Data field octet 2 */
	t_Ui16 m3c;    /* Mode 3/A code */
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
	list_text (2, " v=" M_FMT_BYTE ";", (df1 >> 7) & 0x01);
	list_text (2, " g=" M_FMT_BYTE ";", (df1 >> 6) & 0x01);
	list_text (2, " l=" M_FMT_BYTE ";", (df1 >> 5) & 0x01);
	list_text (2, " code=" M_FMT_CODE, m3c);
	list_text (2, "\n");
#endif /* LISTER */

				   /* Store this information: */
	rtgt.mode_3a_info.code = m3c;
	rtgt.mode_3a_info.code_garbled = M_TRES (df1 & 0x40);
	rtgt.mode_3a_info.code_invalid = M_TRES (df1 & 0x80);
	rtgt.mode_3a_info.code_smoothed = M_TRES (df1 & 0x20);
	rtgt.mode_3a_info.present = TRUE;

				   /* Set the return code: */
	ret = RC_OKAY;

	return ret;
}

/*----------------------------------------------------------------------------*/
/* proc_i048_080   -- Process I048/080 data item                              */
/*----------------------------------------------------------------------------*/

 static
 t_Retc proc_i048_080 (t_Ui16 length, t_Byte *buffer)
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
/* proc_i048_090   -- Process I048/090 data item                              */
/*----------------------------------------------------------------------------*/

 static
 t_Retc proc_i048_090 (t_Ui16 length, t_Byte *buffer)
{
	t_Byte df1;    /* Data field octet 1 */
	t_Byte df2;    /* Data field octet 2 */
	t_Si16 mch;    /* Mode C height; 25 feet */
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
	mch = make_si16 (df1 & 0x1f, df2);
	if (df1 & 0x20)
	{
		mch |= 0xe000;
                   /* Negative height encoded in two's complement */
	}

#if LISTER
				   /* List data field: */
	list_text (2, ";  Flight Level:");
	list_text (2, " v=" M_FMT_BYTE ";", (df1 >> 7) & 0x01);
	list_text (2, " g=" M_FMT_BYTE ";", (df1 >> 6) & 0x01);
	list_text (2, " code=" M_FMT_SI16 " (%.2f FL)", mch, 0.25 * mch);
	list_text (2, "\n");
#endif /* LISTER */

				   /* Store this information: */
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

				   /* Set the return code: */
	ret = RC_OKAY;

	return ret;
}

/*----------------------------------------------------------------------------*/
/* proc_i048_100   -- Process I048/100 data item                              */
/*----------------------------------------------------------------------------*/

 static
 t_Retc proc_i048_100 (t_Ui16 length, t_Byte *buffer)
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
	rtgt.mode_c_code.code_garbled = M_TRES (df1 & 0x40);
	rtgt.mode_c_code.code_invalid = M_TRES (df1 & 0x80);
	rtgt.mode_c_code.code_confidence = mccc;
                   /* Not inverted, i. e. 0=high confidence */
	rtgt.mode_c_code.code_confidence_present = TRUE;
	rtgt.mode_c_code.present = TRUE;

				   /* Set the return code: */
	ret = RC_OKAY;

	return ret;
}

/*----------------------------------------------------------------------------*/
/* proc_i048_110   -- Process I048/110 data item                              */
/*----------------------------------------------------------------------------*/

 static
 t_Retc proc_i048_110 (t_Ui16 length, t_Byte *buffer)
{
	t_Byte df1;    /* Data field octet 1 */
	t_Byte df2;    /* Data field octet 2 */
	t_Si16 hgt;    /* 3D height; 25 feet */
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
	hgt = make_si16 (df1 & 0x1f, df2);
	if (df1 & 0x20)
	{
		hgt |= 0xe000;
                   /* Negative height encoded in two's complement */
	}

#if LISTER
				   /* List data field: */
	list_text (2, ";  3D Height:");
	list_text (2, " " M_FMT_SI16 " (%.2f FL)", hgt, 0.25 * hgt);
	list_text (2, "\n");
#endif /* LISTER */

				   /* Store this information: */
	rtgt.height_3d.present = TRUE;
	rtgt.height_3d.valid = e_is_true;
	rtgt.height_3d.value = 25.0 * M_FT2MTR * hgt;
	rtgt.height_3d.value_in_feet = 25 * (t_Si32) hgt;

				   /* Set the return code: */
	ret = RC_OKAY;

	return ret;
}

/*----------------------------------------------------------------------------*/
/* proc_i048_120   -- Process I048/120 data item                              */
/*----------------------------------------------------------------------------*/

 static
 t_Retc proc_i048_120 (t_Ui16 length, t_Byte *buffer, t_Ui16 *pos_ptr)
{
	t_Ui16 amb;    /* Ambiguity range; metres/second */
	t_Si16 cal;    /* Calculated Doppler speed; metres/second */
	t_Bool dbt;    /* Calculated Doppler speed doubtful indication */
	t_Byte df1;    /* Data field octet 1 */
	t_Byte df2;    /* Data field octet 2 */
	t_Byte df3;    /* Data field octet 3 */
	t_Byte df4;    /* Data field octet 4 */
	t_Byte df5;    /* Data field octet 5 */
	t_Byte df6;    /* Data field octet 6 */
	t_Byte df7;    /* Data field octet 7 */
	t_Si16 dop;    /* Raw Doppler speed; metres/second */
	t_Ui16 frq;    /* Transmitter frequency; MHz */
	int ix;        /* Auxiliary */
	t_Ui16 pos;    /* Position within buffer */
	t_Byte rep;    /* Repetition factor */
	t_Retc ret;    /* Return code */
	t_Byte sf1_present;
                   /* Subfield #1 (Calculated Doppler speed) present */
	t_Byte sf2_present;
                   /* Subfield #2 (Raw Doppler speed) present */

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

				   /* Extract octet: */
	df1 = buffer[pos ++];

                   /* Evaluate presence of subfields: */
	sf1_present = (df1 >> 7) & 0x01;
	sf2_present = (df1 >> 6) & 0x01;

                   /* Check presence of subfields: */
	if (sf1_present && sf2_present)
	{
		error_msg ("Both subfields present in I048/120");
		goto done;
	}

                   /* Handle subfield #1, if present: */
	if (sf1_present)
	{
				   /* Check against buffer length: */
		if (pos + 1 >= length)
		{
			error_msg ("Invalid buffer length (I048/120)");
			goto done;
		}

                   /* Extract octets: */
		df2 = buffer[pos ++];
		df3 = buffer[pos ++];
	}

                   /* Handle subfield #2, if present: */
	if (sf2_present)
	{
                   /* Get repetition factor: */
		rep = buffer[pos ++];

                   /* Check repetition factor: */
		if (rep != 1)
		{
			error_msg ("Unexpected repetition factor (I048/120)");
			goto done;
		}

                   /* Check against buffer length: */
		if (pos + rep * 6 > length)
		{
			error_msg ("Invalid buffer length (I048/120)");
			goto done;
		}

                   /* Extract octets: */
		for (ix = 0; ix < rep; ix ++)
		{
			df2 = buffer[pos ++];
			df3 = buffer[pos ++];
			df4 = buffer[pos ++];
			df5 = buffer[pos ++];
			df6 = buffer[pos ++];
			df7 = buffer[pos ++];
		}
	}

#if LISTER
				   /* List raw data field: */
	list_text (1, ";  I048/120: 0x " M_FMT_BYTE_HEX, df1);
	if (sf1_present)
	{
		list_text (1, " " M_FMT_BYTE_HEX " " M_FMT_BYTE_HEX, df2, df3);
	}
	else if (sf2_present)
	{
		list_text (1, " " M_FMT_BYTE_HEX " " M_FMT_BYTE_HEX " " M_FMT_BYTE_HEX
                      "  " M_FMT_BYTE_HEX " " M_FMT_BYTE_HEX " " M_FMT_BYTE_HEX
                      " " M_FMT_BYTE_HEX,
                      rep, df2, df3, df4, df5, df6, df7);
	}
	list_text (1, "\n");
#endif /* LISTER */

				   /* Extract information: */
	if (sf1_present)
	{
		dbt = FALSE;
		if (df1 & 0x80)
		{
			dbt = TRUE;
		}
		cal = make_si16 (df2 & 0x01, df3);
		if (df2 & 0x02)
		{
			cal |= 0xfe00;
		}
	}
	else if (sf2_present)
	{
		dop = make_si16 (df2, df3);
		amb = make_ui16 (df4, df5);
		frq = make_ui16 (df6, df7);
	}

#if LISTER
				   /* List data field: */
	list_text (2, ";  Radial Doppler Speed:");
	if (sf1_present)
	{
		list_text (2, " d=" M_FMT_BYTE "; cal=" M_FMT_SI16 " m/s",
                      dbt, cal);
	}
	else if (sf2_present)
	{
		list_text (2, " dop=" M_FMT_SI16 " m/s; amb=" M_FMT_UI16
                      " m/s; frq=" M_FMT_UI16 " MHz",
                      dop, amb, frq);
	}
	list_text (2, "\n");
#endif /* LISTER */

				   /* Store this information: */
	if (sf1_present)
	{
		rtgt.calculated_doppler_speed.is_doubtful = M_TRES (dbt);
		rtgt.calculated_doppler_speed.present = TRUE;
		rtgt.calculated_doppler_speed.value = cal;
	}
	else if (sf2_present)
	{
		rtgt.raw_doppler_speed.ambiguity_range = amb;
		rtgt.raw_doppler_speed.present = TRUE;
		rtgt.raw_doppler_speed.value = dop;
		rtgt.raw_doppler_speed.transmitter_frequency = frq;
	}

				   /* Set position: */
	*pos_ptr = pos;

				   /* Set the return code: */
	ret = RC_OKAY;

	done:          /* We are done */
	return ret;
}

/*----------------------------------------------------------------------------*/
/* proc_i048_130   -- Process I048/130 data item                              */
/*----------------------------------------------------------------------------*/

 static
 t_Retc proc_i048_130 (t_Ui16 length, t_Byte *buffer, t_Ui16 *pos_ptr)
{
	t_Si16 apd;    /* Difference in azimuth between PSR and SSR plot;
                      360/2**14 degrees */
	t_Byte b;      /* Auxiliary */
	int cnt;       /* Count for secondary subfield octets */
	t_Byte df1;    /* Data field octet 1 */
#if LISTER
	int ix;        /* Auxiliary */
#endif /* LISTER */
	t_Si16 pam;    /* Amplitude of primary plot; dBm */
	t_Ui16 pos;    /* Position within buffer */
	t_Byte prl;    /* Primary plot runlength; 360/2**13 degrees */
	t_Retc ret;    /* Return code */
	t_Si16 rpd;    /* Difference in range between PSR and SSR plot;
                      1/256 nautical miles */
	t_Si16 sam;    /* Amplitude of (M)SSR reply; dBm */
	t_Byte sf1_present;
                   /* Subfield #1 (SSR plot runlength) present */
	t_Byte sf2_present;
                   /* Subfield #2 (Number of received replies
                      for (M)SSR) present */
	t_Byte sf3_present;
                   /* Subfield #3 (Amplitude of received replies
                      for (M)SSR) present */
	t_Byte sf4_present;
                   /* Subfield #4 (PSR plot runlength) present */
	t_Byte sf5_present;
                   /* Subfield #5 (PSR amplitude) present */
	t_Byte sf6_present;
                   /* Subfield #6 (Difference in range between
                      PSR and SSR plot) present */
	t_Byte sf7_present;
                   /* Subfield #7 (Difference in azimuth between
                      PSR and SSR plot) present */
	t_Byte srl;    /* SSR plot runlength; 360/2**13 degrees */
	t_Byte srr;    /* Number of received replies for M(SSR) */

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
	sf2_present = (df1 >> 6) & 0x01;
	sf3_present = (df1 >> 5) & 0x01;
	sf4_present = (df1 >> 4) & 0x01;
	sf5_present = (df1 >> 3) & 0x01;
	sf6_present = (df1 >> 2) & 0x01;
	sf7_present = (df1 >> 1) & 0x01;

                   /* Calculate number of octets in secondary subfields: */
	cnt = ord (sf1_present) +
          ord (sf2_present) +
          ord (sf3_present) +
          ord (sf4_present) +
          ord (sf5_present) +
          ord (sf6_present) +
          ord (sf7_present);

				   /* Check against buffer length: */
	if (pos + cnt > length)
	{
		error_msg ("Invalid buffer length (I048/130)");
		goto done;
	}

#if LISTER
				   /* List raw data field: */
	list_text (1, ";  I048/130: 0x " M_FMT_BYTE_HEX, df1);
	for (ix = 0; ix < cnt; ix ++)
	{
		list_text (1, " " M_FMT_BYTE_HEX, buffer[pos + ix]);
	}
	list_text (1, "\n");
#endif /* LISTER */

                   /* Extract secondary subfields: */
	if (sf1_present)
	{
		srl = buffer[pos ++];
	}
	if (sf2_present)
	{
		srr = buffer[pos ++];
	}
	if (sf3_present)
	{
		b = buffer[pos ++];

		sam = (t_Si16) (b & 0x7f);
		if (b & 0x80)
		{
			sam |= 0xff80;
		}
	}
	if (sf4_present)
	{
		prl = buffer[pos ++];
	}
	if (sf5_present)
	{
		b = buffer[pos ++];

		pam = (t_Si16) (b & 0x7f);
		if (b & 0x80)
		{
			pam |= 0xff80;
		}
	}
	if (sf6_present)
	{
		b = buffer[pos ++];

		rpd = (t_Si16) (b & 0x7f);
		if (b & 0x80)
		{
			rpd |= 0xff80;
		}
	}
	if (sf7_present)
	{
		b = buffer[pos ++];

		apd = (t_Si16) (b & 0x7f);
		if (b & 0x80)
		{
			apd |= 0xff80;
		}
	}

#if LISTER
				   /* List data field: */
	list_text (2, ";  Radar Plot Characteristics:\n");
	if (sf1_present)
	{
		list_text (2, ";   SSR plot runlength: " M_FMT_BYTE " (%.3f deg)\n",
                      srl, (360.0 / 8192.0) * srl);
	}
	if (sf2_present)
	{
		list_text (2, ";   Number of received replies for (M)SSR: "
                      M_FMT_BYTE "\n", srr);
	}
	if (sf3_present)
	{
		list_text (2, ";   Amplitude of (M)SSR reply: " M_FMT_SI16
                      " dBm\n", sam);
	}
	if (sf4_present)
	{
		list_text (2, ";   Primary plot runlength: " M_FMT_BYTE " (%.3f deg)\n",
                      prl, (360.0 / 8192.0) * prl);
	}
	if (sf5_present)
	{
		list_text (2, ";   Amplitude of primary plot:"
                      " " M_FMT_SI16 " dBm\n", pam);
	}
	if (sf6_present)
	{
		list_text (2, ";   Difference in range between PSR and SSR plot:"
                      " " M_FMT_SI16 " (%.3f nmi)\n", rpd, (1.0 / 256.0) * rpd);
	}
	if (sf7_present)
	{
		list_text (2, ";   Difference in azimuth between PSR and SSR plot:"
                      " " M_FMT_SI16 " (%.3f deg)\n",
                      apd, (360.0 / 16384.0) * apd);
	}
#endif /* LISTER */

				   /* Store this information: */
	if (sf1_present)
	{
		rtgt.ssr_runlength.present = TRUE;
		rtgt.ssr_runlength.value = (360.0 / 8192.0) * srl;
	}
	if (sf2_present)
	{
		rtgt.ssr_replies.present = TRUE;
		rtgt.ssr_replies.value = srr;
	}
	if (sf3_present)
	{
		rtgt.ssr_amplitude.present = TRUE;
		rtgt.ssr_amplitude.value = sam;
	}
	if (sf4_present)
	{
		rtgt.psr_runlength.present = TRUE;
		rtgt.psr_runlength.value = (360.0 / 8192.0) * prl;
	}
	if (sf5_present)
	{
		rtgt.psr_amplitude.present = TRUE;
		rtgt.psr_amplitude.value = pam;
	}
	if (sf6_present)
	{
		rtgt.range_difference.present = TRUE;
		rtgt.range_difference.value = (M_NMI2MTR / 256.0) * rpd;
	}
	if (sf7_present)
	{
		rtgt.azimuth_difference.present = TRUE;
		rtgt.azimuth_difference.value = (M_TWO_PI / 16384.0) * apd;
	}

				   /* Set position: */
	*pos_ptr = pos;

				   /* Set the return code: */
	ret = RC_OKAY;

	done:          /* We are done */
	return ret;
}

/*----------------------------------------------------------------------------*/
/* proc_i048_140   -- Process I048/140 data item                              */
/*----------------------------------------------------------------------------*/

 static
 t_Retc proc_i048_140 (t_Ui16 length, t_Byte *buffer)
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

                   /* Convert to milliseconds: */
	tod_in_secs = (1.0 / 128.0) * tod;

#if LISTER
				   /* List data field: */
	list_text (2, ";  Time of Day:");
	list_text (2, " " M_FMT_ATOD " (" M_FMT_UI32 "; %.6f secs; %s UTC)",
                  tod, tod, tod_in_secs, utc_text (tod_in_secs));
	list_text (2, "\n");
#endif /* LISTER */

                   /* Store this information: */
	rtgt.detection_time.present = TRUE;
	rtgt.detection_time.value = tod_in_secs;

				   /* Set the return code: */
	ret = RC_OKAY;

	return ret;
}

/*----------------------------------------------------------------------------*/
/* proc_i048_161   -- Process I048/161 data item                              */
/*----------------------------------------------------------------------------*/

 static
 t_Retc proc_i048_161 (t_Ui16 length, t_Byte *buffer)
{
	t_Byte df1;    /* Data field octet 1 */
	t_Byte df2;    /* Data field octet 2 */
	t_Ui16 num;    /* Track number */
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
	num = make_ui16 (df1 & 0x0f, df2);

#if LISTER
				   /* List data field: */
	list_text (2, ";  Track Number:");
	list_text (2, " num=" M_FMT_UI16, num);
	list_text (2, "\n");
#endif /* LISTER */

				   /* Store this information: */
	rtgt.detection_type.is_radar_track = e_is_true;
	rtgt.track_number.present = TRUE;
	rtgt.track_number.value = num;

				   /* Set the return code: */
	ret = RC_OKAY;

	return ret;
}

/*----------------------------------------------------------------------------*/
/* proc_i048_170   -- Process I048/170 data item                              */
/*----------------------------------------------------------------------------*/

 static
 t_Retc proc_i048_170 (t_Ui16 length, t_Byte *buffer)
{
	t_Byte dfo;    /* Data field octet */
	int ix;        /* Auxiliary */
	t_Retc ret;    /* Return code */

				   /* Preset the return code: */
	ret = RC_FAIL;

#if LISTER
				   /* Check parameters: */
	Assert (length > 0, "Invalid parameter");
	Assert (buffer != NULL, "Invalid parameter");

				   /* List data field: */
	list_text (2, ";  Track Status:");
	for (ix = 0; ix < length; ix ++)
	{
		dfo = buffer[ix];
		if (ix == 0)
		{
			t_Byte cdm;
			t_Byte rad;

			if (dfo & 0x80)
			{
				list_text (2, " INI");
			}
			else
			{
				list_text (2, " CNF");
			}

			rad = (dfo >> 5) & 0x03;
			if (rad == 0)
			{
				list_text (2, " CRT");
			}
			else if (rad == 1)
			{
				list_text (2, " PRT");
			}
			else if (rad == 2)
			{
				list_text (2, " SRT");
			}

			if (dfo & 0x10)
			{
				list_text (2, " DOU");
			}

			if (dfo & 0x08)
			{
				list_text (2, " MAH");
			}

			cdm = (dfo >> 1) & 0x03;
			if (cdm == 0)
			{
				list_text (2, " LVL");
			}
			else if (cdm == 1)
			{
				list_text (2, " CLB");
			}
			else if (cdm == 2)
			{
				list_text (2, " DSC");
			}
		}
		else if (ix == 1)
		{
			if (dfo & 0x80)
			{
				list_text (2, " TRE");
			}

			if (dfo & 0x40)
			{
				list_text (2, " GHO");
			}

			if (dfo & 0x20)
			{
				list_text (2, " SUP");
			}

			if (dfo & 0x10)
			{
				list_text (2, " TCC=0");
			}
			else
			{
				list_text (2, " TCC=1");
			}
		}
	}
	list_text (2, "\n");
#endif /* LISTER */

				   /* Store this information: */
	rtgt.detection_type.is_radar_track = e_is_true;
	for (ix = 0; ix < length; ix ++)
	{
		dfo = buffer[ix];

		if (ix == 0)
		{
			/* tba ... */
		}
		else if (ix == 1)
		{
			if (dfo & 0x80)
			{
				rtgt.to_be_cancelled = e_is_true;
			}

			/* tba ... */
		}
	}

				   /* Set the return code: */
	ret = RC_OKAY;

	return ret;
}

/*----------------------------------------------------------------------------*/
/* proc_i048_200   -- Process I048/200 data item                              */
/*----------------------------------------------------------------------------*/

 static
 t_Retc proc_i048_200 (t_Ui16 length, t_Byte *buffer)
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
	rtgt.detection_type.is_radar_track = e_is_true;
	rtgt.ground_vector.present = true;
	rtgt.ground_vector.value_gsp = (M_NMI2MTR / 16384.0) * spd;
                   /* In metres/second */
	rtgt.ground_vector.value_hdg = (M_TWO_PI / 65536.0) * hdg;
                   /* In radians */

				   /* Set the return code: */
	ret = RC_OKAY;

	return ret;
}

/*----------------------------------------------------------------------------*/
/* proc_i048_210   -- Process I048/210 data item                              */
/*----------------------------------------------------------------------------*/

 static
 t_Retc proc_i048_210 (t_Ui16 length, t_Byte *buffer)
{
#if CCWARN
	t_Byte df1;    /* Data field octet 1 */
	t_Byte df2;    /* Data field octet 2 */
	t_Byte df3;    /* Data field octet 3 */
	t_Byte df4;    /* Data field octet 4 */
#endif
	t_Retc ret;    /* Return code */

				   /* Preset the return code: */
	ret = RC_FAIL;

				   /* Check parameters: */
	Assert (length == 4, "Invalid parameter");
	Assert (buffer != NULL, "Invalid parameter");

				   /* Extract octets: */
#if CCWARN
	df1 = buffer[0];
	df2 = buffer[1];
	df3 = buffer[2];
	df4 = buffer[3];
#endif

#if LISTER
				   /* List data field: */
	list_text (2, ";  Track Quality:");
	/* tba ... */
	list_text (2, "\n");
#endif /* LISTER */

				   /* Store this information: */
	rtgt.detection_type.is_radar_track = e_is_true;
	/* tba ... */

				   /* Set the return code: */
	ret = RC_OKAY;

	return ret;
}

/*----------------------------------------------------------------------------*/
/* proc_i048_220   -- Process I048/220 data item                              */
/*----------------------------------------------------------------------------*/

 static
 t_Retc proc_i048_220 (t_Ui16 length, t_Byte *buffer)
{
	t_Ui32 aa;     /* Aircraft address */
	t_Byte df1;    /* Data field octet 1 */
	t_Byte df2;    /* Data field octet 2 */
	t_Byte df3;    /* Data field octet 3 */
	t_Retc ret;    /* Return code */

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
	aa = make_ui32 (0x00, df1, df2, df3);

#if LISTER
				   /* List data field: */
	list_text (2, ";  Aircraft Address:");
	list_text (2, " " M_FMT_ADDR " (" M_FMT_UI32 ")", aa, aa);
	list_text (2, "\n");
#endif /* LISTER */

				   /* Store this information: */
	rtgt.target_address.present = TRUE;
	rtgt.target_address.value = aa;

				   /* Set the return code: */
	ret = RC_OKAY;

	return ret;
}

/*----------------------------------------------------------------------------*/
/* proc_i048_230   -- Process I048/230 data item                              */
/*----------------------------------------------------------------------------*/

 static
 t_Retc proc_i048_230 (t_Ui16 length, t_Byte *buffer)
{
	t_Byte aic;    /* Aircraft identification capability */
	t_Byte arc;    /* Altitude reporting capability */
	t_Byte b1a;    /* BDS 1,0 bit 16 */
	t_Byte b1b;    /* BDS 1,0 bits 37/40 */
	t_Byte cc;     /* Communications capability */
	t_Byte df1;    /* Data field octet 1 */
	t_Byte df2;    /* Data field octet 2 */
	t_Byte fs;     /* Flight status */
	t_Byte mssc;   /* Mode S specific service capability */
	t_Retc ret;    /* Return code */
	t_Byte si;     /* SI/II transponder capability */

				   /* Preset the return code: */
	ret = RC_FAIL;

				   /* Check parameters: */
	Assert (length == 2, "Invalid parameter");
	Assert (buffer != NULL, "Invalid parameter");

				   /* Extract octets: */
	df1 = buffer[0];
	df2 = buffer[1];

                   /* Extract information: */
	cc = (df1 >> 5) & 0x07;
	fs = (df1 >> 2) & 0x07;
	if (reference_vsn == 3)
	{
		si = (df1 & 0x01);
	}
	mssc = (df2 >> 7) & 0x01;
	arc = (df2 >> 6) & 0x01;
	aic = (df2 >> 5) & 0x01;
	b1a = (df2 >> 4) & 0x01;
	b1b = df2 & 0x0f;

#if LISTER
				   /* List data field: */
	list_text (2, ";  Communications Capability:");
	list_text (2, " CC=" M_FMT_BYTE ";", cc);
	list_text (2, " FS=" M_FMT_BYTE, fs);
	if (fs == 0)
	{
		list_text (2, " (airborne)");
	}
	else if (fs == 1)
	{
		list_text (2, " (on ground)");
	}
	else if (fs == 2)
	{
		list_text (2, " (airborne, alert)");
	}
	else if (fs == 3)
	{
		list_text (2, " (on ground, alert)");
	}
	else if (fs == 4)
	{
		list_text (2, " (alert, spi)");
	}
	else if (fs == 5)
	{
		list_text (2, " (spi)");
	}
	list_text (2, ";");
	if (reference_vsn == 3)
	{
		list_text (2, " SI=" M_FMT_BYTE ";", si);
	}
	if (mssc)
	{
		list_text (2, " MSSC;");
	}
	if (arc)
	{
		list_text (2, " ARC=25ft;");
	}
	else
	{
		list_text (2, " ARC=100ft;");
	}
	if (aic)
	{
		list_text (2, " AIC;");
	}
	list_text (2, " B1A=" M_FMT_BYTE ";", b1a);
	list_text (2, " B1B=" M_FMT_BYTE "", b1b);
	list_text (2, "\n");
#endif /* LISTER */

                   /* Decode flight status: */
	rtgt.fs_airborne = e_is_undefined;
	rtgt.fs_alert = e_is_undefined;
	rtgt.fs_on_ground = e_is_undefined;
	rtgt.fs_spi = e_is_undefined;
	if (fs == 0)
	{
		rtgt.fs_airborne = e_is_true;
		rtgt.fs_alert = e_is_false;
		rtgt.fs_spi = e_is_false;
	}
	else if (fs == 1)
	{
		rtgt.fs_alert = e_is_false;
		rtgt.fs_on_ground = e_is_true;
		rtgt.fs_spi = e_is_false;
	}
	else if (fs == 2)
	{
		rtgt.fs_airborne = e_is_true;
		rtgt.fs_alert = e_is_true;
		rtgt.fs_spi = e_is_false;
	}
	else if (fs == 3)
	{
		rtgt.fs_alert = e_is_true;
		rtgt.fs_on_ground = e_is_true;
		rtgt.fs_spi = e_is_false;
	}
	else if (fs == 4)
	{
		rtgt.fs_alert = e_is_true;
		rtgt.fs_spi = e_is_true;
	}
	else if (fs == 5)
	{
		rtgt.fs_alert = e_is_false;
		rtgt.fs_spi = e_is_true;
	}

                   /* Adapt the SSR mode C height, if present: */
	if (arc == 1)
	{
		if (rtgt.mode_c_height.present)
		{
			rtgt.mode_c_height.in_25_feet = e_is_true;
		}
	}
	else
	{
		if (rtgt.mode_c_height.present)
		{
			rtgt.mode_c_height.in_25_feet = e_is_false;
		}
	}

				   /* Store this information: */
	rtgt.mode_s_flags.present = TRUE;
	rtgt.mode_s_flags.value_aic = aic;
	rtgt.mode_s_flags.value_arc = arc;
	rtgt.mode_s_flags.value_b1a = b1a;
	rtgt.mode_s_flags.value_b1b = b1b;
	rtgt.mode_s_flags.value_cc = cc;
	rtgt.mode_s_flags.value_fs = fs;
	rtgt.mode_s_flags.value_mssc = mssc;
	rtgt.mode_s_flags.value_si = e_is_undefined;
	if (reference_vsn == 3)
	{
		rtgt.mode_s_flags.value_si = M_TRES (si == 0);
	}

				   /* Set the return code: */
	ret = RC_OKAY;

	return ret;
}

/*----------------------------------------------------------------------------*/
/* proc_i048_240   -- Process I048/240 data item                              */
/*----------------------------------------------------------------------------*/

 static
 t_Retc proc_i048_240 (t_Ui16 length, t_Byte *buffer)
{
	t_Byte c1;     /* Character 1 */
	t_Byte c2;     /* Character 2 */
	t_Byte c3;     /* Character 3 */
	t_Byte c4;     /* Character 4 */
	t_Byte c5;     /* Character 5 */
	t_Byte c6;     /* Character 6 */
	t_Byte c7;     /* Character 7 */
	t_Byte c8;     /* Character 8 */
	t_Byte df1;    /* Data field octet 1 */
	t_Byte df2;    /* Data field octet 2 */
	t_Byte df3;    /* Data field octet 3 */
	t_Byte df4;    /* Data field octet 4 */
	t_Byte df5;    /* Data field octet 5 */
	t_Byte df6;    /* Data field octet 6 */
	t_Retc ret;    /* Return code */
	char txt[9];   /* Text buffer */

				   /* Preset the return code: */
	ret = RC_FAIL;

				   /* Check parameters: */
	Assert (length == 6, "Invalid parameter");
	Assert (buffer != NULL, "Invalid parameter");

				   /* Extract octets: */
	df1 = buffer[0];
	df2 = buffer[1];
	df3 = buffer[2];
	df4 = buffer[3];
	df5 = buffer[4];
	df6 = buffer[5];

                   /* Extract information: */
	c1 = (df1 >> 2) & 0x3f;
	c2 = df1 & 0x03;
	c2 <<= 4;
	c2 |= (df2 >> 4) & 0x0f;
	c3 = df2 & 0x0f;
	c3 <<= 2;
	c3 |= (df3 >> 6) & 0x03;
	c4 = df3 & 0x3f;
	c5 = (df4 >> 2) & 0x3f;
	c6 = df4 & 0x03;
	c6 <<= 4;
	c6 |= (df5 >> 4) & 0x0f;
	c7 = df5 & 0x0f;
	c7 <<= 2;
	c7 |= (df6 >> 6) & 0x03;
	c8 = df6 & 0x3f;

                   /* Convert to conventional text: */
	memset (txt, 0, 9);
	txt[0] = expand_c (c1);
	txt[1] = expand_c (c2);
	txt[2] = expand_c (c3);
	txt[3] = expand_c (c4);
	txt[4] = expand_c (c5);
	txt[5] = expand_c (c6);
	txt[6] = expand_c (c7);
	txt[7] = expand_c (c8);

#if LISTER
				   /* List data field: */
	list_text (2, ";  Aircraft Identification: \"%s\"\n", txt);
#endif /* LISTER */

				   /* Store this information: */
	rtgt.aircraft_identification.present = TRUE;
	strncpy (rtgt.aircraft_identification.value_idt, txt,
             M_AIRCRAFT_IDENTIFICATION_LENGTH);

				   /* Set the return code: */
	ret = RC_OKAY;

	return ret;
}

/*----------------------------------------------------------------------------*/
/* proc_i048_250   -- Process I048/250 data item                              */
/*----------------------------------------------------------------------------*/

 static
 t_Retc proc_i048_250 (t_Ui16 length, t_Byte *buffer)
{
#if LISTER
	t_Byte bds1;   /* BDS 1 */
	t_Byte bds2;   /* BDS 2 */
#endif /* LISTER */
	t_Byte df1;    /* Data field octet 1 */
	t_Byte df2;    /* Data field octet 2 */
	t_Byte df3;    /* Data field octet 3 */
	t_Byte df4;    /* Data field octet 4 */
	t_Byte df5;    /* Data field octet 5 */
	t_Byte df6;    /* Data field octet 6 */
	t_Byte df7;    /* Data field octet 7 */
	t_Byte df8;    /* Data field octet 8 */
	int ix;        /* Auxiliary */
	t_Byte rep;    /* Repetition factor */
	t_Retc ret;    /* Return code */

				   /* Preset the return code: */
	ret = RC_FAIL;

				   /* Check parameters: */
	Assert (length > 0, "Invalid parameter");
	Assert (buffer != NULL, "Invalid parameter");

				   /* Get repetition factor: */
	rep = buffer[0];

                   /* Check repetition factor: */
	if (rep == 0)
	{
		ret = RC_SKIP;
		goto done;
	}

				   /* Check length (again): */
	Assert (length == 1 + rep * 8, "Invalid length");

#if LISTER
				   /* List data field: */
	list_text (2, ";  Mode S MB Data:\n");
	for (ix = 0; ix < rep; ix ++)
	{
		df1 = buffer[1 + 8 * ix];
		df2 = buffer[1 + 8 * ix + 1];
		df3 = buffer[1 + 8 * ix + 2];
		df4 = buffer[1 + 8 * ix + 3];
		df5 = buffer[1 + 8 * ix + 4];
		df6 = buffer[1 + 8 * ix + 5];
		df7 = buffer[1 + 8 * ix + 6];
		df8 = buffer[1 + 8 * ix + 7];

		bds1 = (df8 >> 4) & 0x0f;
		bds2 = df8 & 0x0f;

		list_text (2, ";   BDS " M_FMT_BYTE "," M_FMT_BYTE " data=0x"
                      " " M_FMT_BYTE_HEX " " M_FMT_BYTE_HEX
                      " " M_FMT_BYTE_HEX " " M_FMT_BYTE_HEX
                      " " M_FMT_BYTE_HEX " " M_FMT_BYTE_HEX
                      " " M_FMT_BYTE_HEX "\n",
                      bds1, bds2,
                      df1, df2, df3, df4, df5, df6, df7);
	}
#endif /* LISTER */

				   /* Store this information: */
	for (ix = 0; ix < rep; ix ++)
	{
		df1 = buffer[1 + 8 * ix];
		df2 = buffer[1 + 8 * ix + 1];
		df3 = buffer[1 + 8 * ix + 2];
		df4 = buffer[1 + 8 * ix + 3];
		df5 = buffer[1 + 8 * ix + 4];
		df6 = buffer[1 + 8 * ix + 5];
		df7 = buffer[1 + 8 * ix + 6];
		df8 = buffer[1 + 8 * ix + 7];

		if (ix < M_MAX_BDS_REGISTERS)
		{
			rtgt.bds_registers[ix].number = df8;
			rtgt.bds_registers[ix].present = TRUE;
			rtgt.bds_registers[ix].value[0] = df1;
			rtgt.bds_registers[ix].value[1] = df2;
			rtgt.bds_registers[ix].value[2] = df3;
			rtgt.bds_registers[ix].value[3] = df4;
			rtgt.bds_registers[ix].value[4] = df5;
			rtgt.bds_registers[ix].value[5] = df6;
			rtgt.bds_registers[ix].value[6] = df7;
		}

		if (df8 == 0x40)
		{
			t_Ui16 fms_sa;
                   /* FMS selected altitude; 16 feet */
			t_Byte fms_sa_status;
                   /* Status for FMS selected altitude */
			t_Ui16 mcp_fcu_sa;
                   /* MCP/FCU selected altitude; 16 feet */
			t_Byte mcp_fcu_sa_status;
                   /* Status for MCP/FCU selected altitude */
			t_Byte tas;
                   /* Target altitude source */
			t_Byte tas_status;
                   /* Status of target altitude source */

			mcp_fcu_sa_status = (df1 >> 7) & 0x01;

			mcp_fcu_sa = 0;
			mcp_fcu_sa |= (t_Ui16) ((df1 & 0x7f) << 5);
			mcp_fcu_sa |= (t_Ui16) ((df2 & 0xf8) >> 3);

			fms_sa_status = (df2 >> 2) & 0x01;

			fms_sa = 0;
			fms_sa |= (t_Ui16) ((df2 & 0xc0) << 10);
			fms_sa |= (t_Ui16) (df3 << 2);
			fms_sa |= (t_Ui16) ((df4 & 0xc0) >> 6);

			tas_status = (df7 >> 2) & 0x01;

			tas = df7 & 0x03;

			if (tas_status && tas != 0)
			{
				if (tas == 1)
				{
					if (mcp_fcu_sa_status)
					{
						rtgt.selected_altitude.present = TRUE;
						rtgt.selected_altitude.source = 1;
						rtgt.selected_altitude.value =
							(t_Si32) (16 * mcp_fcu_sa);
					}
					else if (fms_sa_status)
					{
						rtgt.selected_altitude.present = TRUE;
						rtgt.selected_altitude.source = 1;
						rtgt.selected_altitude.value = (t_Si32) (16 * fms_sa);
					}
				}
				else if (tas == 2)
				{
					if (mcp_fcu_sa_status)
					{
						rtgt.selected_altitude.present = TRUE;
						rtgt.selected_altitude.source = 2;
						rtgt.selected_altitude.value =
							(t_Si32) (16 * mcp_fcu_sa);
					}
				}
				else if (tas == 3)
				{
					if (fms_sa_status)
					{
						rtgt.selected_altitude.present = TRUE;
						rtgt.selected_altitude.source = 3;
						rtgt.selected_altitude.value = (t_Si32) (16 * fms_sa);
					}
				}
			}
			else
			{
				if (mcp_fcu_sa_status)
				{
					rtgt.selected_altitude.present = TRUE;
					rtgt.selected_altitude.source = 2;
					rtgt.selected_altitude.value = (t_Si32) (16 * mcp_fcu_sa);
				}
				else if (fms_sa_status)
				{
					rtgt.selected_altitude.present = TRUE;
					rtgt.selected_altitude.source = 3;
					rtgt.selected_altitude.value = (t_Si32) (16 * fms_sa);
				}
			}

			if (rtgt.selected_altitude.present)
			{
				rtgt.selected_altitude.value_alt_hold = e_is_false;
				rtgt.selected_altitude.value_approach = e_is_false;
				rtgt.selected_altitude.value_vnav = e_is_false;

                if ((df6 & 0x01) == 0x01)
				{
					if (df7 & 0x80)
					{
						rtgt.selected_altitude.value_vnav = e_is_true;
					}
					if (df7 & 0x40)
					{
						rtgt.selected_altitude.value_alt_hold = e_is_true;
					}
					if (df7 & 0x20)
					{
						rtgt.selected_altitude.value_approach = e_is_true;
					}
				}
			}

#if LISTER
			if (mcp_fcu_sa_status || fms_sa_status)
			{
				t_Ui16 cnt;

				list_text (1, ";           ");

				cnt = 0;
				if (mcp_fcu_sa_status)
				{
					list_text (1, "mcp_fcu_selected_altitude=");
					list_text (1, "%.0f[ft]", 16.0 * mcp_fcu_sa);

					++ cnt;
				}

				if (fms_sa_status)
				{
					if (cnt > 0)
					{
						list_text (1, " ");
					}
					list_text (1, "fms_selected_altitude=");
					list_text (1, "%.0f[ft]", 16.0 * fms_sa);

					++ cnt;
				}

				if (tas_status)
				{
					if (cnt > 0)
					{
						list_text (1, " ");
					}
					list_text (1, "tas=" M_FMT_BYTE, tas);

					++ cnt;
				}

				list_text (1, "\n");
			}
#endif /* LISTER */
		}
	}

				   /* Set the return code: */
	ret = RC_OKAY;

	done:          /* We are done */
	return ret;
}

/*----------------------------------------------------------------------------*/
/* proc_i048_260   -- Process I048/260 data item                              */
/*----------------------------------------------------------------------------*/

 static
 t_Retc proc_i048_260 (t_Ui16 length, t_Byte *buffer)
{
	t_Byte df1;    /* Data field octet 1 */
	t_Byte df2;    /* Data field octet 2 */
	t_Byte df3;    /* Data field octet 3 */
	t_Byte df4;    /* Data field octet 4 */
	t_Byte df5;    /* Data field octet 5 */
	t_Byte df6;    /* Data field octet 6 */
	t_Byte df7;    /* Data field octet 7 */
	t_Retc ret;    /* Return code */

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

#if LISTER
				   /* List data field: */
	list_text (2, ";  ACAS Resolution Advisory Report: 0x");
	list_text (2, " " M_FMT_BYTE_HEX " " M_FMT_BYTE_HEX
                  " " M_FMT_BYTE_HEX " " M_FMT_BYTE_HEX
                  " " M_FMT_BYTE_HEX " " M_FMT_BYTE_HEX
                  " " M_FMT_BYTE_HEX,
                  df1, df2, df3, df4, df5, df6, df7);
	list_text (2, "\n");
#endif /* LISTER */

				   /* Store this information: */
	rtgt.acas_resolution_advisory_report.present = TRUE;
	rtgt.acas_resolution_advisory_report.value[0] = df1;
	rtgt.acas_resolution_advisory_report.value[1] = df2;
	rtgt.acas_resolution_advisory_report.value[2] = df3;
	rtgt.acas_resolution_advisory_report.value[3] = df4;
	rtgt.acas_resolution_advisory_report.value[4] = df5;
	rtgt.acas_resolution_advisory_report.value[5] = df6;
	rtgt.acas_resolution_advisory_report.value[6] = df7;

				   /* Set the return code: */
	ret = RC_OKAY;

	return ret;
}

/*----------------------------------------------------------------------------*/
/* proc_i048_ref   -- Process REF indicator data item                         */
/*----------------------------------------------------------------------------*/

 static
 t_Retc proc_i048_ref (t_Ui16 length, t_Byte *buffer, t_Ui16 *pos_ptr)
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
		error_msg ("Invalid buffer length (I048/REF)");
		goto done;
	}

#if LISTER
				   /* List raw data field: */
	list_buffer (1, ";  I048/REF:", len, buffer + pos);
#endif /* LISTER */

				   /* Set position: */
	*pos_ptr = pos + len;

				   /* Set the return code: */
	ret = RC_OKAY;

	done:          /* We are done */
	return ret;
}

/*----------------------------------------------------------------------------*/
/* proc_i048_spf   -- Process SPF indicator data item                         */
/*----------------------------------------------------------------------------*/

 static
 t_Retc proc_i048_spf (t_Ui16 length, t_Byte *buffer, t_Ui16 *pos_ptr)
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
		error_msg ("Invalid buffer length (I048/SPF)");
		goto done;
	}

#if LISTER
				   /* List raw data field: */
	list_buffer (1, ";  I048/SPF:", len, buffer + pos);
#endif /* LISTER */

				   /* Set position: */
	*pos_ptr = pos + len;

				   /* Set the return code: */
	ret = RC_OKAY;

	done:          /* We are done */
	return ret;
}

/*----------------------------------------------------------------------------*/
/* set_vsn048      -- Set ASTERIX category 048 reference version              */
/*----------------------------------------------------------------------------*/

 t_Retc set_vsn048 (char *vsn_text)
{
	t_Retc ret;    /* Return code */

                   /* Preset the return code: */
	ret = RC_FAIL;

                   /* Check parameters: */
	Assert (vsn_text != NULL, "Invalid parameter");

                   /* Check against implemented reference versions: */
	if (strcmp (vsn_text, "1.14") == 0)
	{
		reference_vsn = 1;
		ret = RC_OKAY;
	}
	else if (strcmp (vsn_text, "1.15") == 0)
	{
		reference_vsn = 2;
		ret = RC_OKAY;
	}
	else if (strcmp (vsn_text, "1.16") == 0)
	{
		reference_vsn = 3;
		ret = RC_OKAY;
	}

	return ret;
}
/* end-of-file */
