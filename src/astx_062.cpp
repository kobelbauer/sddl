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
/* File:           src/astx_062.cpp                                           */
/* Contents:       Process ASTERIX category 062 data block                    */
/* Author(s):      kb                                                         */
/* Last change:    2018-07-29                                                 */
/*----------------------------------------------------------------------------*/

/* Reference document:
   -------------------

   Eurocontrol Standard Document for Surveillance Data Exchange
   Part 9: Category 062
   SDPS Track Messages
   SUR.ET1.ST05.2000-STD-09-01

   Edition: 1.3
   Edition Date: April 2005
   Status: Released Issue

   Edition: 1.8P
   Edition Date: August 2008
   Status: Proposed Issue
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

#define M_MAX_FRN 35
				   /* Maximum field reference number */
#define M_MAX_FSPEC_LENGTH 5
				   /* Maximum fields specification length
                      for ASTERIX category 062 */

				   /* Local variables: */
				   /* ---------------- */

static t_Data_Item_Desc desc_i062_010;
                   /* Description of data item I062/010 */
static t_Data_Item_Desc desc_i062_015;
                   /* Description of data item I062/015 */
static t_Data_Item_Desc desc_i062_040;
                   /* Description of data item I062/040 */
static t_Data_Item_Desc desc_i062_060;
                   /* Description of data item I062/060 */
static t_Data_Item_Desc desc_i062_070;
                   /* Description of data item I062/070 */
static t_Data_Item_Desc desc_i062_080;
                   /* Description of data item I062/080 */
static t_Data_Item_Desc desc_i062_100;
                   /* Description of data item I062/100 */
static t_Data_Item_Desc desc_i062_105;
                   /* Description of data item I062/105 */
static t_Data_Item_Desc desc_i062_110;
                   /* Description of data item I062/110 */
static t_Data_Item_Desc desc_i062_120;
                   /* Description of data item I062/120 */
static t_Data_Item_Desc desc_i062_130;
                   /* Description of data item I062/130 */
static t_Data_Item_Desc desc_i062_135;
                   /* Description of data item I062/135 */
static t_Data_Item_Desc desc_i062_136;
                   /* Description of data item I062/136 */
static t_Data_Item_Desc desc_i062_185;
                   /* Description of data item I062/185 */
static t_Data_Item_Desc desc_i062_200;
                   /* Description of data item I062/200 */
static t_Data_Item_Desc desc_i062_210;
                   /* Description of data item I062/210 */
static t_Data_Item_Desc desc_i062_220;
                   /* Description of data item I062/220 */
static t_Data_Item_Desc desc_i062_245;
                   /* Description of data item I062/245 */
static t_Data_Item_Desc desc_i062_270;
                   /* Description of data item I062/270 */
static t_Data_Item_Desc desc_i062_290;
                   /* Description of data item I062/290 */
static t_Data_Item_Desc desc_i062_295;
                   /* Description of data item I062/295 */
static t_Data_Item_Desc desc_i062_300;
                   /* Description of data item I062/300 */
static t_Data_Item_Desc desc_i062_340;
                   /* Description of data item I062/340 */
static t_Data_Item_Desc desc_i062_380;
                   /* Description of data item I062/380 */
static t_Data_Item_Desc desc_i062_390;
                   /* Description of data item I062/390 */
static t_Data_Item_Desc desc_i062_500;
                   /* Description of data item I062/500 */
static t_Data_Item_Desc desc_i062_510;
                   /* Description of data item I062/510 */
static t_Data_Item_Desc desc_i062_ref;
                   /* Description of REF data item */
static t_Data_Item_Desc desc_i062_spf;
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
static t_Retc proc_i062_010 (t_Ui16 length, t_Byte *buffer);
                   /* Process I062/010 data item */
static t_Retc proc_i062_015 (t_Ui16 length, t_Byte *buffer);
                   /* Process I062/015 data item */
static t_Retc proc_i062_040 (t_Ui16 length, t_Byte *buffer);
                   /* Process I062/040 data item */
static t_Retc proc_i062_060 (t_Ui16 length, t_Byte *buffer);
                   /* Process I062/060 data item */
static t_Retc proc_i062_070 (t_Ui16 length, t_Byte *buffer);
                   /* Process I062/070 data item */
static t_Retc proc_i062_080 (t_Ui16 length, t_Byte *buffer);
                   /* Process I062/080 data item */
static t_Retc proc_i062_100 (t_Ui16 length, t_Byte *buffer);
                   /* Process I062/100 data item */
static t_Retc proc_i062_105 (t_Ui16 length, t_Byte *buffer);
                   /* Process I062/105 data item */
static t_Retc proc_i062_110 (t_Ui16 length, t_Byte *buffer, t_Ui16 *pos_ptr);
                   /* Process I062/110 data item */
static t_Retc proc_i062_120 (t_Ui16 length, t_Byte *buffer);
                   /* Process I062/120 data item */
static t_Retc proc_i062_130 (t_Ui16 length, t_Byte *buffer);
                   /* Process I062/130 data item */
static t_Retc proc_i062_135 (t_Ui16 length, t_Byte *buffer);
                   /* Process I062/135 data item */
static t_Retc proc_i062_136 (t_Ui16 length, t_Byte *buffer);
                   /* Process I062/136 data item */
static t_Retc proc_i062_185 (t_Ui16 length, t_Byte *buffer);
                   /* Process I062/185 data item */
static t_Retc proc_i062_200 (t_Ui16 length, t_Byte *buffer);
                   /* Process I062/200 data item */
static t_Retc proc_i062_210 (t_Ui16 length, t_Byte *buffer);
                   /* Process I062/210 data item */
static t_Retc proc_i062_220 (t_Ui16 length, t_Byte *buffer);
                   /* Process I062/220 data item */
static t_Retc proc_i062_245 (t_Ui16 length, t_Byte *buffer);
                   /* Process I062/245 data item */
static t_Retc proc_i062_270 (t_Ui16 length, t_Byte *buffer);
                   /* Process I062/270 data item */
static t_Retc proc_i062_290 (t_Ui16 length, t_Byte *buffer, t_Ui16 *pos_ptr);
                   /* Process I062/290 data item */
static t_Retc proc_i062_295 (t_Ui16 length, t_Byte *buffer, t_Ui16 *pos_ptr);
                   /* Process I062/295 data item */
static t_Retc proc_i062_300 (t_Ui16 length, t_Byte *buffer);
                   /* Process I062/300 data item */
static t_Retc proc_i062_340 (t_Ui16 length, t_Byte *buffer, t_Ui16 *pos_ptr);
                   /* Process I062/340 data item */
static t_Retc proc_i062_380 (t_Ui16 length, t_Byte *buffer, t_Ui16 *pos_ptr);
                   /* Process I062/380 data item */
static t_Retc proc_i062_390 (t_Ui16 length, t_Byte *buffer, t_Ui16 *pos_ptr);
                   /* Process I062/390 data item */
static t_Retc proc_i062_500 (t_Ui16 length, t_Byte *buffer, t_Ui16 *pos_ptr);
                   /* Process I062/500 data item */
static t_Retc proc_i062_510 (t_Ui16 length, t_Byte *buffer, t_Ui16 *pos_ptr);
                   /* Process I062/510 data item */
static t_Retc proc_i062_ref (t_Ui16 length, t_Byte *buffer, t_Ui16 *pos_ptr);
                   /* Process RE indicator data item */
static t_Retc proc_i062_spf (t_Ui16 length, t_Byte *buffer, t_Ui16 *pos_ptr);
                   /* Process SP indicator data item */

/*----------------------------------------------------------------------------*/
/* astx_062        -- Process ASTERIX category 062 data block                 */
/*----------------------------------------------------------------------------*/

 t_Retc astx_062 (t_Ui16 length, t_Byte *buffer)
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

				   /* Check against max. length: */
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
		strk.asterix_category = 62;
		strk.data_format = e_data_format_asterix;

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

					lrc = data_item (62, frn, std_uap[frn],
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

                   /* Add frame time, if available: */
        if (frame_time_present)
        {
            strk.frame_time.present = TRUE;
            strk.frame_time.value = frame_time;
        }

                   /* Add board/line number, if present: */
		if (current_line_number_defined)
		{
			strk.line_number = current_line_number;
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
                   /* Description of data item I062/010: */
	desc_i062_010.category = 62;
	desc_i062_010.data_item = 10;
	desc_i062_010.item_type = e_fixed_length_data_item;
	desc_i062_010.fixed_length = 2;
	desc_i062_010.proc_fptr = proc_i062_010;
	desc_i062_010.read_fptr = NULL;

                   /* Description of data item I062/015: */
	desc_i062_015.category = 62;
	desc_i062_015.data_item = 15;
	desc_i062_015.item_type = e_fixed_length_data_item;
	desc_i062_015.fixed_length = 1;
	desc_i062_015.proc_fptr = proc_i062_015;
	desc_i062_015.read_fptr = NULL;

                   /* Description of data item I062/040: */
	desc_i062_040.category = 62;
	desc_i062_040.data_item = 40;
	desc_i062_040.item_type = e_fixed_length_data_item;
	desc_i062_040.fixed_length = 2;
	desc_i062_040.proc_fptr = proc_i062_040;
	desc_i062_040.read_fptr = NULL;

                   /* Description of data item I062/060: */
	desc_i062_060.category = 62;
	desc_i062_060.data_item = 60;
	desc_i062_060.item_type = e_fixed_length_data_item;
	desc_i062_060.fixed_length = 2;
	desc_i062_060.proc_fptr = proc_i062_060;
	desc_i062_060.read_fptr = NULL;

                   /* Description of data item I062/070: */
	desc_i062_070.category = 62;
	desc_i062_070.data_item = 70;
	desc_i062_070.item_type = e_fixed_length_data_item;
	desc_i062_070.fixed_length = 3;
	desc_i062_070.proc_fptr = proc_i062_070;
	desc_i062_070.read_fptr = NULL;

                   /* Description of data item I062/080: */
	desc_i062_080.category = 62;
	desc_i062_080.data_item = 80;
	desc_i062_080.item_type = e_variable_length_data_item;
	desc_i062_080.fixed_length = 0;
	desc_i062_080.proc_fptr = proc_i062_080;
	desc_i062_080.read_fptr = NULL;

                   /* Description of data item I062/100: */
	desc_i062_100.category = 62;
	desc_i062_100.data_item = 100;
	desc_i062_100.item_type = e_fixed_length_data_item;
	desc_i062_100.fixed_length = 6;
	desc_i062_100.proc_fptr = proc_i062_100;
	desc_i062_100.read_fptr = NULL;

                   /* Description of data item I062/105: */
	desc_i062_105.category = 62;
	desc_i062_105.data_item = 105;
	desc_i062_105.item_type = e_fixed_length_data_item;
	desc_i062_105.fixed_length = 8;
	desc_i062_105.proc_fptr = proc_i062_105;
	desc_i062_105.read_fptr = NULL;

                   /* Description of data item I062/110: */
	desc_i062_110.category = 62;
	desc_i062_110.data_item = 110;
	desc_i062_110.item_type = e_immediate_data_item;
	desc_i062_110.fixed_length = 0;
	desc_i062_110.proc_fptr = NULL;
	desc_i062_110.read_fptr = proc_i062_110;

                   /* Description of data item I062/120: */
	desc_i062_120.category = 62;
	desc_i062_120.data_item = 120;
	desc_i062_120.item_type = e_fixed_length_data_item;
	desc_i062_120.fixed_length = 2;
	desc_i062_120.proc_fptr = proc_i062_120;
	desc_i062_120.read_fptr = NULL;

                   /* Description of data item I062/130: */
	desc_i062_130.category = 62;
	desc_i062_130.data_item = 130;
	desc_i062_130.item_type = e_fixed_length_data_item;
	desc_i062_130.fixed_length = 2;
	desc_i062_130.proc_fptr = proc_i062_130;
	desc_i062_130.read_fptr = NULL;

                   /* Description of data item I062/135: */
	desc_i062_135.category = 62;
	desc_i062_135.data_item = 135;
	desc_i062_135.item_type = e_fixed_length_data_item;
	desc_i062_135.fixed_length = 2;
	desc_i062_135.proc_fptr = proc_i062_135;
	desc_i062_135.read_fptr = NULL;

                   /* Description of data item I062/136: */
	desc_i062_136.category = 62;
	desc_i062_136.data_item = 136;
	desc_i062_136.item_type = e_fixed_length_data_item;
	desc_i062_136.fixed_length = 2;
	desc_i062_136.proc_fptr = proc_i062_136;
	desc_i062_136.read_fptr = NULL;

                   /* Description of data item I062/185: */
	desc_i062_185.category = 62;
	desc_i062_185.data_item = 185;
	desc_i062_185.item_type = e_fixed_length_data_item;
	desc_i062_185.fixed_length = 4;
	desc_i062_185.proc_fptr = proc_i062_185;
	desc_i062_185.read_fptr = NULL;

                   /* Description of data item I062/200: */
	desc_i062_200.category = 62;
	desc_i062_200.data_item = 200;
	desc_i062_200.item_type = e_fixed_length_data_item;
	desc_i062_200.fixed_length = 1;
	desc_i062_200.proc_fptr = proc_i062_200;
	desc_i062_200.read_fptr = NULL;

                   /* Description of data item I062/210: */
	desc_i062_210.category = 62;
	desc_i062_210.data_item = 210;
	desc_i062_210.item_type = e_fixed_length_data_item;
	desc_i062_210.fixed_length = 2;
	desc_i062_210.proc_fptr = proc_i062_210;
	desc_i062_210.read_fptr = NULL;

                   /* Description of data item I062/220: */
	desc_i062_220.category = 62;
	desc_i062_220.data_item = 220;
	desc_i062_220.item_type = e_fixed_length_data_item;
	desc_i062_220.fixed_length = 2;
	desc_i062_220.proc_fptr = proc_i062_220;
	desc_i062_220.read_fptr = NULL;

                   /* Description of data item I062/245: */
	desc_i062_245.category = 62;
	desc_i062_245.data_item = 245;
	desc_i062_245.item_type = e_fixed_length_data_item;
	desc_i062_245.fixed_length = 7;
	desc_i062_245.proc_fptr = proc_i062_245;
	desc_i062_245.read_fptr = NULL;

                   /* Description of data item I062/270: */
	desc_i062_270.category = 62;
	desc_i062_270.data_item = 270;
	desc_i062_270.item_type = e_variable_length_data_item;
	desc_i062_270.fixed_length = 0;
	desc_i062_270.proc_fptr = proc_i062_270;
	desc_i062_270.read_fptr = NULL;

                   /* Description of data item I062/290: */
	desc_i062_290.category = 62;
	desc_i062_290.data_item = 290;
	desc_i062_290.item_type = e_immediate_data_item;
	desc_i062_290.fixed_length = 0;
	desc_i062_290.proc_fptr = NULL;
	desc_i062_290.read_fptr = proc_i062_290;

                   /* Description of data item I062/295: */
	desc_i062_295.category = 62;
	desc_i062_295.data_item = 295;
	desc_i062_295.item_type = e_immediate_data_item;
	desc_i062_295.fixed_length = 0;
	desc_i062_295.proc_fptr = NULL;
	desc_i062_295.read_fptr = proc_i062_295;

                   /* Description of data item I062/300: */
	desc_i062_300.category = 62;
	desc_i062_300.data_item = 300;
	desc_i062_300.item_type = e_fixed_length_data_item;
	desc_i062_300.fixed_length = 1;
	desc_i062_300.proc_fptr = proc_i062_300;
	desc_i062_300.read_fptr = NULL;

                   /* Description of data item I062/340: */
	desc_i062_340.category = 62;
	desc_i062_340.data_item = 340;
	desc_i062_340.item_type = e_immediate_data_item;
	desc_i062_340.fixed_length = 0;
	desc_i062_340.proc_fptr = NULL;
	desc_i062_340.read_fptr = proc_i062_340;

                   /* Description of data item I062/380: */
	desc_i062_380.category = 62;
	desc_i062_380.data_item = 380;
	desc_i062_380.item_type = e_immediate_data_item;
	desc_i062_380.fixed_length = 0;
	desc_i062_380.proc_fptr = NULL;
	desc_i062_380.read_fptr = proc_i062_380;

                   /* Description of data item I062/390: */
	desc_i062_390.category = 62;
	desc_i062_390.data_item = 390;
	desc_i062_390.item_type = e_immediate_data_item;
	desc_i062_390.fixed_length = 0;
	desc_i062_390.proc_fptr = NULL;
	desc_i062_390.read_fptr = proc_i062_390;

                   /* Description of data item I062/500: */
	desc_i062_500.category = 62;
	desc_i062_500.data_item = 500;
	desc_i062_500.item_type = e_immediate_data_item;
	desc_i062_500.fixed_length = 0;
	desc_i062_500.proc_fptr = NULL;
	desc_i062_500.read_fptr = proc_i062_500;

                   /* Description of data item I062/510: */
	desc_i062_510.category = 62;
	desc_i062_510.data_item = 510;
	desc_i062_510.item_type = e_immediate_data_item;
	desc_i062_510.fixed_length = 0;
	desc_i062_510.proc_fptr = NULL;
	desc_i062_510.read_fptr = proc_i062_510;

                   /* Description of REF data item: */
	desc_i062_ref.category = 62;
	desc_i062_ref.data_item = M_REF_INDICATOR;
	desc_i062_ref.item_type = e_immediate_data_item;
	desc_i062_ref.fixed_length = 0;
	desc_i062_ref.proc_fptr = NULL;
	desc_i062_ref.read_fptr = proc_i062_ref;

                   /* Description of SPF data item: */
	desc_i062_spf.category = 62;
	desc_i062_spf.data_item = M_SPF_INDICATOR;
	desc_i062_spf.item_type = e_immediate_data_item;
	desc_i062_spf.fixed_length = 0;
	desc_i062_spf.proc_fptr = NULL;
	desc_i062_spf.read_fptr = proc_i062_spf;

	return;
}

/*----------------------------------------------------------------------------*/
/* load_std_uap    -- Load standard UAP                                       */
/*----------------------------------------------------------------------------*/

 static
 void load_std_uap (void)
{
	int ix;         /* Auxiliary */

	for (ix = 0; ix <= M_MAX_FRN; ix ++)
	{
		std_uap[ix] = NULL;
	}

	std_uap[1] = &desc_i062_010;
	std_uap[3] = &desc_i062_015;
	std_uap[4] = &desc_i062_070;
	std_uap[5] = &desc_i062_105;
	std_uap[6] = &desc_i062_100;
	std_uap[7] = &desc_i062_185;

	std_uap[8] = &desc_i062_210;
	std_uap[9] = &desc_i062_060;
	std_uap[10] = &desc_i062_245;
	std_uap[11] = &desc_i062_380;
	std_uap[12] = &desc_i062_040;
	std_uap[13] = &desc_i062_080;
	std_uap[14] = &desc_i062_290;

	std_uap[15] = &desc_i062_200;
	std_uap[16] = &desc_i062_295;
	std_uap[17] = &desc_i062_136;
	std_uap[18] = &desc_i062_130;
	std_uap[19] = &desc_i062_135;
	std_uap[20] = &desc_i062_220;
	std_uap[21] = &desc_i062_390;

	std_uap[22] = &desc_i062_270;
	std_uap[23] = &desc_i062_300;
	std_uap[24] = &desc_i062_110;
	std_uap[25] = &desc_i062_120;
	std_uap[26] = &desc_i062_510;
	std_uap[27] = &desc_i062_500;
	std_uap[28] = &desc_i062_340;

	std_uap[34] = &desc_i062_ref;
	std_uap[35] = &desc_i062_spf;

	return;
}

/*----------------------------------------------------------------------------*/
/* proc_i062_010   -- Process I062/010 data item                              */
/*----------------------------------------------------------------------------*/

 static
 t_Retc proc_i062_010 (t_Ui16 length, t_Byte *buffer)
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
	strk.server_sacsic.present = TRUE;
	strk.server_sacsic.value = dsi;

				   /* Set the return code: */
	ret = RC_OKAY;

	return ret;
}

/*----------------------------------------------------------------------------*/
/* proc_i062_015   -- Process I062/015 data item                              */
/*----------------------------------------------------------------------------*/

 static
 t_Retc proc_i062_015 (t_Ui16 length, t_Byte *buffer)
{
	t_Byte df1;    /* Data field octet 1 */
	t_Retc ret;    /* Return code */

				   /* Preset the return code: */
	ret = RC_FAIL;

				   /* Check parameters: */
	Assert (length == 1, "Invalid parameter");
	Assert (buffer != NULL, "Invalid parameter");

				   /* Extract octets: */
	df1 = buffer[0];

#if LISTER
				   /* List data field: */
	list_text (2, ";  Service Identification:");
	list_text (2, " " M_FMT_BYTE, df1);
	list_text (2, "\n");
#endif /* LISTER */

				   /* Store this information: */
	strk.service_identification.present = TRUE;
	strk.service_identification.value = df1;

				   /* Set the return code: */
	ret = RC_OKAY;

	return ret;
}

/*----------------------------------------------------------------------------*/
/* proc_i062_040   -- Process I062/040 data item                              */
/*----------------------------------------------------------------------------*/

 static
 t_Retc proc_i062_040 (t_Ui16 length, t_Byte *buffer)
{
	t_Byte df1;    /* Data field octet 1 */
	t_Byte df2;    /* Data field octet 2 */
	t_Retc ret;    /* Return code */
	t_Ui16 stn;    /* System track number */
	t_Byte sttn;   /* System track numbering indicator */

				   /* Preset the return code: */
	ret = RC_FAIL;

				   /* Check parameters: */
	Assert (length == 2, "Invalid parameter");
	Assert (buffer != NULL, "Invalid parameter");

				   /* Extract octets: */
	df1 = buffer[0];
	df2 = buffer[1];

				   /* Extract information: */
	if (track_number_bits == 12)
	{
		sttn = (df1 >> 4) & 0x01;
		stn = make_ui16 (df1 & 0x0f, df2);
                   /* Please be aware that this is NOT as defined in the
                      ASTERIX standards document; but this was the factual
                      implementation of ARTAS for a long time
                   */
	}
	else
	{
                   /* System track number with 16 bits */

		sttn = 0;
		stn = make_ui16 (df1, df2);
	}

#if LISTER
				   /* List data field: */
	list_text (2, ";  Track Number:");
	if (track_number_bits == 12)
	{
		list_text (2, " " M_FMT_UI16 " (sttn=" M_FMT_BYTE ")", stn, sttn);
	}
	else
	{
		list_text (2, " " M_FMT_UI16, stn);
	}
	list_text (2, "\n");
#endif /* LISTER */

				   /* Store this information: */
	strk.track_number.present = TRUE;
	strk.track_number.value = stn;
	if (track_number_bits == 12)
	{
		strk.track_numbering_indicator_present = TRUE;
		strk.track_numbering_indicator = sttn;
	}

				   /* Set the return code: */
	ret = RC_OKAY;

	return ret;
}

/*----------------------------------------------------------------------------*/
/* proc_i062_060   -- Process I062/060 data item                              */
/*----------------------------------------------------------------------------*/

 static
 t_Retc proc_i062_060 (t_Ui16 length, t_Byte *buffer)
{
	t_Byte df1;    /* Data field octet 1 */
	t_Byte df2;    /* Data field octet 2 */
	t_Ui16 m3c;    /* Track mode 3/A code */
	t_Byte m3c_changed;
                   /* Track mode 3/A code changed */
	t_Byte m3c_garbled;
                   /* Track mode 3/A code garbled */
	t_Byte m3c_invalid;
                   /* Track mode 3/A code invalid */
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
	m3c_invalid = (df1 >> 7) & 0x01;
	m3c_garbled = (df1 >> 6) & 0x01;
	m3c_changed = (df1 >> 5) & 0x01;

#if LISTER
				   /* List data field: */
	list_text (2, ";  Track Mode 3/A Code:");
	list_text (2, " i=" M_FMT_BYTE ";", m3c_invalid);
	list_text (2, " g=" M_FMT_BYTE ";", m3c_garbled);
	list_text (2, " c=" M_FMT_BYTE ";", m3c_changed);
	list_text (2, " code=" M_FMT_CODE, m3c);
	list_text (2, "\n");
#endif /* LISTER */

				   /* Store this information: */
	strk.mode_3a_info.code = m3c;
	strk.mode_3a_info.code_changed = m3c_changed;
	strk.mode_3a_info.code_garbled = m3c_garbled;
	strk.mode_3a_info.code_invalid = m3c_invalid;
	strk.mode_3a_info.present = TRUE;

				   /* Set the return code: */
	ret = RC_OKAY;

	return ret;
}

/*----------------------------------------------------------------------------*/
/* proc_i062_070   -- Process I062/070 data item                              */
/*----------------------------------------------------------------------------*/

 static
 t_Retc proc_i062_070 (t_Ui16 length, t_Byte *buffer)
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

#if LISTER
				   /* List data field: */
	list_text (2, ";  Time of Track Information:");
	list_text (2, " " M_FMT_ATOD " (" M_FMT_UI32 "; %s UTC)",
                  tod, tod, utc_text (tod_in_secs));
	list_text (2, "\n");
#endif /* LISTER */

				   /* Store this information: */
	strk.time_of_last_update.present = TRUE;
	strk.time_of_last_update.value = tod_in_secs;

				   /* Set the return code: */
	ret = RC_OKAY;

	return ret;
}

/*----------------------------------------------------------------------------*/
/* proc_i062_080   -- Process I062/080 data item                              */
/*----------------------------------------------------------------------------*/

 static
 t_Retc proc_i062_080 (t_Ui16 length, t_Byte *buffer)
{
	t_Byte dfo;    /* Data field octet */
	int ix;        /* Auxiliary */
	t_Retc ret;    /* Return code */
	t_Byte src;    /* Source of calculated track altitude */

				   /* Preset the return code: */
	ret = RC_FAIL;

				   /* Check parameters: */
	Assert (length > 0, "Invalid parameter");
	Assert (buffer != NULL, "Invalid parameter");

#if LISTER
				   /* List data field: */
	list_text (2, ";  Track Status:\n");
	for (ix = 0; ix < length; ix ++)
	{
		dfo = buffer[ix];

		if (ix == 0 && dfo != 0x01)
		{
			list_text (2, ";  ");
			if (dfo & 0x80)
			{
				list_text (2, " MON");
			}
			else
			{
				list_text (2, " MUL");
			}
			if (dfo & 0x40)
			{
				list_text (2, " SPI");
			}
			list_text (2, " MRH=" M_FMT_BYTE, (dfo >> 5) & 0x01);

			src = (dfo >> 2) & 0x07;
			list_text (2, " SRC=" M_FMT_BYTE, src);
			switch (src)
			{
			case 0:
				list_text (2, " (no source)");
				break;
			case 1:
				list_text (2, " (GNSS)");
				break;
			case 2:
				list_text (2, " (3D radar)");
				break;
			case 3:
				list_text (2, " (triangulation)");
				break;
			case 4:
				list_text (2, " (height from coverage)");
				break;
			case 5:
				list_text (2, " (speed look-up table)");
				break;
			case 6:
				list_text (2, " (default height)");
				break;
			case 7:
				list_text (2, " (multilateration)");
				break;
			}

			if (dfo & 0x02)
			{
				list_text (2, " TNT");
			}
			else
			{
				list_text (2, " CNF");
			}
			list_text (2, "\n");
		}
		else if (ix == 1 && dfo != 0x01)
		{
			list_text (2, ";  ");
			if (dfo & 0x80)
			{
				list_text (2, " SIM");
			}
			if (dfo & 0x40)
			{
				list_text (2, " TSE");
			}
			if (dfo & 0x20)
			{
				list_text (2, " TSB");
			}
			if (dfo & 0x10)
			{
				list_text (2, " FPC");
			}
			if (dfo & 0x08)
			{
				list_text (2, " AFF");
			}
			if (dfo & 0x04)
			{
				list_text (2, " STP");
			}
			list_text (2, " KOS=" M_FMT_BYTE, (dfo >> 1) & 0x01);
			list_text (2, "\n");
		}
		else if (ix == 2 && dfo != 0x01)
		{
			list_text (2, ";  ");
			if (dfo & 0x80)
			{
				list_text (2, " AMA");
			}
			if (dfo & 0x60)
			{
				list_text (2, " MD4=" M_FMT_BYTE, (dfo >> 5) & 0x03);
			}
			if (dfo & 0x10)
			{
				list_text (2, " ME");
			}
			if (dfo & 0x08)
			{
				list_text (2, " MI");
			}
			if (dfo & 0x06)
			{
				list_text (2, " MD5=" M_FMT_BYTE, (dfo >> 1) & 0x03);
			}
			list_text (2, "\n");
		}
		else if (ix == 3 && dfo != 0x01)
		{
			list_text (2, ";  ");
			if (dfo & 0x80)
			{
				list_text (2, " CST");
			}
			if ((dfo & 0x40) == 0x00)
			{
				list_text (2, " PSR");
			}
			if ((dfo & 0x20) == 0x00)
			{
				list_text (2, " SSR");
			}
			if ((dfo & 0x10) == 0x00)
			{
				list_text (2, " MDS");
			}
			if ((dfo & 0x08) == 0x00)
			{
				list_text (2, " ADS");
			}
			if (dfo & 0x04)
			{
				list_text (2, " SUC");
			}
			if (dfo & 0x02)
			{
				list_text (2, " AAC");
			}
			list_text (2, "\n");
		}
	}
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
			strk.multi_sensor_track = M_TRES ((dfo & 0x80) == 0x00);

			strk.special_position_indication = M_TRES (dfo & 0x40);

			strk.most_reliable_height_present = TRUE;
			strk.most_reliable_height = (t_Byte) ((dfo >> 5) & 0x01);
			src = (dfo >> 2) & 0x07;
			switch (src)
			{
			case 0:
				strk.calculated_track_altitude.source = 4;
				break;
			case 1:
				strk.calculated_track_altitude.source = 5;
				break;
			case 2:
				strk.calculated_track_altitude.source = 6;
				break;
			case 3:
				strk.calculated_track_altitude.source = 7;
				break;
			case 4:
				strk.calculated_track_altitude.source = 8;
				break;
			case 5:
				strk.calculated_track_altitude.source = 9;
				break;
			case 6:
				strk.calculated_track_altitude.source = 10;
				break;
			case 7:
				strk.calculated_track_altitude.source = 11;
				break;
			}

			strk.tentative_track = M_TRES (dfo & 0x02);
		}
		else if (ix == 1)
		{
			strk.simulated = M_TRES (dfo & 0x80);

			strk.track_terminated = M_TRES (dfo & 0x40);

			strk.track_created = M_TRES (dfo & 0x20);

			strk.flight_plan_correlated = M_TRES (dfo & 0x10);

			strk.adsb_blunder_detected = M_TRES (dfo & 0x08);

			strk.slave_track_promotion = M_TRES (dfo & 0x04);

			strk.background_service = M_TRES (dfo & 0x02);
		}
		else if (ix == 2)
		{
			strk.amalgamated_track = M_TRES (dfo & 0x80);

			if (dfo & 0x60)
			{
				strk.mode_4_info_present = TRUE;
				strk.mode_4_info = (dfo >> 5) & 0x03;
			}

			strk.military_emergency = M_TRES (dfo & 0x10);

			strk.military_ident = M_TRES (dfo & 0x08);

			if (dfo & 0x06)
			{
				strk.mode_5_info.present = TRUE;
				strk.mode_5_info.value = (dfo >> 1) & 0x03;
			}
		}
		else if (ix == 3)
		{
			strk.coasted_track = M_TRES (dfo & 0x80);

			if ((dfo & 0x40) == 0x00)
			{
				strk.observed_by_psr = e_is_true;
			}
			if ((dfo & 0x20) == 0x00)
			{
				strk.observed_by_ssr = e_is_true;
			}
			if ((dfo & 0x10) == 0x00)
			{
				strk.observed_by_mds = e_is_true;
			}
			if ((dfo & 0x08) == 0x00)
			{
				strk.observed_by_ads = e_is_true;
			}

			strk.special_used_code = M_TRES (dfo & 0x04);

			strk.assigned_code_conflict = M_TRES (dfo & 0x02);
		}
	}

				   /* Set the return code: */
	ret = RC_OKAY;

	return ret;
}

/*----------------------------------------------------------------------------*/
/* proc_i062_100   -- Process I062/100 data item                              */
/*----------------------------------------------------------------------------*/

 static
 t_Retc proc_i062_100 (t_Ui16 length, t_Byte *buffer)
{
	t_Byte df1;    /* Data field octet 1 */
	t_Byte df2;    /* Data field octet 2 */
	t_Byte df3;    /* Data field octet 3 */
	t_Byte df4;    /* Data field octet 4 */
	t_Byte df5;    /* Data field octet 5 */
	t_Byte df6;    /* Data field octet 6 */
	t_Retc ret;    /* Return code */
	t_Si32 x;      /* x coordinate; 0.5 metres */
	t_Si32 y;      /* y coordinate; 0.5 metres */

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
	if (df1 & 0x80)
	{
		x = make_si32 (0xff, df1, df2, df3);
	}
	else
	{
		x = make_si32 (0x00, df1, df2, df3);
	}
	if (df4 & 0x80)
	{
		y = make_si32 (0xff, df4, df5, df6);
	}
	else
	{
		y = make_si32 (0x00, df4, df5, df6);
	}

#if LISTER
				   /* List data field: */
	list_text (2, ";  Calculated Track Position (Cartesian):");
	list_text (2, " x=" M_FMT_SI32 " (%.3f nmi)", x, (0.5 * M_MTR2NMI) * x);
	list_text (2, "; y=" M_FMT_SI32 " (%.3f nmi)", y, (0.5 * M_MTR2NMI) * y);
	list_text (2, "\n");
#endif /* LISTER */

				   /* Store this information: */
	strk.calculated_position.present = TRUE;
	strk.calculated_position.value_x = 0.5 * x;
	strk.calculated_position.value_y = 0.5 * y;

				   /* Set the return code: */
	ret = RC_OKAY;

	return ret;
}

/*----------------------------------------------------------------------------*/
/* proc_i062_105   -- Process I062/105 data item                              */
/*----------------------------------------------------------------------------*/

 static
 t_Retc proc_i062_105 (t_Ui16 length, t_Byte *buffer)
{
	t_Byte df1;    /* Data field octet 1 */
	t_Byte df2;    /* Data field octet 2 */
	t_Byte df3;    /* Data field octet 3 */
	t_Byte df4;    /* Data field octet 4 */
	t_Byte df5;    /* Data field octet 5 */
	t_Byte df6;    /* Data field octet 6 */
	t_Byte df7;    /* Data field octet 7 */
	t_Byte df8;    /* Data field octet 8 */
	t_Si32 lat;    /* Latitude; 180/2**25 degrees */
	t_Real latf;   /* Latitude; degrees */
	t_Si32 lon;    /* Longitude; 180/2**25 degrees */
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
	lat = make_si32 (df1, df2, df3, df4);
	lon = make_si32 (df5, df6, df7, df8);

                   /* Convert: */
	latf = (180.0 / M_TWO_POWER_25) * lat;
	lonf = (180.0 / M_TWO_POWER_25) * lon;

#if LISTER
				   /* List data field: */
	list_text (2, ";  Calculated Position in WGS-84 Co-ordinates:\n");
	list_text (2, ";   lat=" M_FMT_SI32 " (%s)", lat, lat_text (latf));
	list_text (2, "; lon=" M_FMT_SI32 " (%s) ", lon, lon_text (lonf));
	list_text (2, "\n");
#endif /* LISTER */

				   /* Store this information: */
	strk.calculated_wgs84_position.present = TRUE;
	strk.calculated_wgs84_position.value_latitude = M_DEG2RAD * latf;
	strk.calculated_wgs84_position.value_longitude = M_DEG2RAD * lonf;
                   /* Values in radians */

				   /* Set the return code: */
	ret = RC_OKAY;

	return ret;
}

/*----------------------------------------------------------------------------*/
/* proc_i062_110   -- Process I062/110 data item                              */
/*----------------------------------------------------------------------------*/

 static
 t_Retc proc_i062_110 (t_Ui16 length, t_Byte *buffer, t_Ui16 *pos_ptr)
{
	t_Byte df1;    /* Data field octet 1 */
	t_Byte df2;    /* Data field octet 2 */
	t_Byte df3;    /* Data field octet 3 */
	t_Byte df4;    /* Data field octet 4 */
	t_Byte df5;    /* Data field octet 5 */
	t_Byte df6;    /* Data field octet 6 */
	t_Byte df7;    /* Data field octet 7 */
	t_Ui16 em1c;   /* Extended mode 1 code */
	t_Si16 ga;     /* GNSS-derived altitude; 25 feet */
	int ix;        /* Auxiliary */
#if LISTER
	int j;         /* Auxiliary */
#endif /* LISTER */
	t_Si32 lat;    /* Latitude; 180/2**23 degrees */
	t_Byte len;    /* Length of data field */
	t_Si32 lon;    /* Longitude; 180/2**23 degrees */
	t_Byte mis;    /* Mission code */
	t_Byte nat;    /* National origin */
	t_Ui16 pin;    /* PIN code */
	t_Ui16 pos;    /* Position within buffer */
	t_Byte res;    /* Resolution of GNSS-derived altitude */
	t_Retc ret;    /* Return code */
	t_Byte sf1_present;
                   /* Subfield 1 present (SUM) */
	t_Byte sf2_present;
                   /* Subfield 2 present (PMN) */
	t_Byte sf3_present;
                   /* Subfield 3 present (POS) */
	t_Byte sf4_present;
                   /* Subfield 4 present (GA) */
	t_Byte sf5_present;
                   /* Subfield 5 present (EM1) */
	t_Byte sf6_present;
                   /* Subfield 6 present (TOS) */
	t_Byte sf7_present;
                   /* Subfield 7 present (XP) */
	t_Byte sum;    /* Mode 5 summary */
	t_Byte tos;    /* Time offset for position and altitude */
	t_Byte xp;     /* X pulses */

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
	df1 = buffer[pos];

                   /* Evaluate presence of subfields: */
	sf1_present = (df1 >> 7) & 0x01;
	sf2_present = (df1 >> 6) & 0x01;
	sf3_present = (df1 >> 5) & 0x01;
	sf4_present = (df1 >> 4) & 0x01;
	sf5_present = (df1 >> 3) & 0x01;
	sf6_present = (df1 >> 2) & 0x01;
	sf7_present = (df1 >> 1) & 0x01;

                   /* Determine expected length of data field: */
	len = 1 +
          ord (sf1_present) +
          4 * ord (sf2_present) +
          6 * ord (sf3_present) +
          2 * ord (sf4_present) +
          2 * ord (sf5_present) +
          ord (sf6_present) +
          ord (sf7_present);

                   /* Check against buffer length: */
	if (pos + len > length)
	{
		error_msg ("Invalid buffer length (I062/110)");
		goto done;
	}

#if LISTER
                   /* List raw data field: */
	list_text (1, ";  I062/110: 0x");
	j = 0;
	for (ix = 0; ix < len; ix ++)
	{
		list_text (1, " " M_FMT_BYTE_HEX, buffer[pos + ix]);
		++ j;

		if (j >= 16)
		{
			list_text (1, "\n");
			if (1 + ix < len)
			{
				list_text (1, ";          + 0x");
			}

			j = 0;
		}
		else if ((j % 4) == 0)
		{
			list_text (1, " ");
		}
	}
	if (j > 0)
	{
		list_text (1, "\n");
	}
#endif /* LISTER */

                   /* Extract secondary subfields: */
	ix = 1;
	if (sf1_present)
	{
		sum = buffer[pos + ix];
		++ ix;
	}
	if (sf2_present)
	{
		df2 = buffer[pos + ix];
		df3 = buffer[pos + ix + 1];
		df4 = buffer[pos + ix + 2];
		df5 = buffer[pos + ix + 3];
		ix += 4;

		pin = make_ui16 (df2 & 0x3f, df3);
		nat = df4 & 0x1f;
		mis = df5 & 0x3f;
	}
	if (sf3_present)
	{
		df2 = buffer[pos + ix];
		df3 = buffer[pos + ix + 1];
		df4 = buffer[pos + ix + 2];
		df5 = buffer[pos + ix + 3];
		df6 = buffer[pos + ix + 4];
		df7 = buffer[pos + ix + 5];
		ix += 6;

		if (df2 & 0x80)
		{
			lat = make_si32 (0xff, df2, df3, df4);
		}
		else
		{
			lat = make_si32 (0x00, df2, df3, df4);
		}
		if (df5 & 0x80)
		{
			lon = make_si32 (0xff, df5, df6, df7);
		}
		else
		{
			lon = make_si32 (0x00, df5, df6, df7);
		}
	}
	if (sf4_present)
	{
		df2 = buffer[pos + ix];
		df3 = buffer[pos + ix + 1];
		ix += 2;

		res = (df2 >> 6) & 0x01;
		ga = make_si16 (df2 & 0x3f, df3);
		if (df2 & 0x20)
		{
			ga |= 0xc000;
		}
	}
	if (sf5_present)
	{
		df2 = buffer[pos + ix];
		df3 = buffer[pos + ix + 1];
		ix += 2;

		em1c = make_ui16 (df2 & 0x0f, df3);
	}
	if (sf6_present)
	{
		tos = buffer[pos + ix];
		++ ix;
	}
	if (sf7_present)
	{
		xp = buffer[pos + ix];
		++ ix;
	}
	Assert (ix == len, "Invalid structure decoding");

#if LISTER
                   /* List data field: */
	list_text (2, ";  Mode 5 Data reports & Extended Mode 1 Code:");
	list_text (2, "\n");
	if (sf1_present)
	{
		list_text (2, ";   Mode 5 Summary:");
		list_text (2, " M5=" M_FMT_BYTE ";", (sum >> 7) & 0x01);
		list_text (2, " ID=" M_FMT_BYTE ";", (sum >> 6) & 0x01);
		list_text (2, " DA=" M_FMT_BYTE ";", (sum >> 5) & 0x01);
		list_text (2, " M1=" M_FMT_BYTE ";", (sum >> 4) & 0x01);
		list_text (2, " M2=" M_FMT_BYTE ";", (sum >> 3) & 0x01);
		list_text (2, " M3=" M_FMT_BYTE ";", (sum >> 2) & 0x01);
		list_text (2, " MC=" M_FMT_BYTE ";", (sum >> 1) & 0x01);
		list_text (2, " X=" M_FMT_BYTE, sum & 0x01);
		list_text (2, "\n");
	}
	if (sf2_present)
	{
		list_text (2, ";   Mode 5 PIN / National Origin / Mission Code:");
		list_text (2, " PIN=" M_FMT_UI16 ";", pin);
		list_text (2, " NAT=" M_FMT_BYTE ";", nat);
		list_text (2, " MIS=" M_FMT_BYTE, mis);
		list_text (2, "\n");
	}
	if (sf3_present)
	{
		list_text (2, ";   Mode 5 Reported Position:");
		list_text (2, " lat=" M_FMT_SI32 " (%.8f deg);",
                      lat, (180.0 / M_TWO_POWER_23) * lat);
		list_text (2, " lon=" M_FMT_SI32 " (%.8f deg)",
                      lon, (180.0 / M_TWO_POWER_23) * lon);
		list_text (2, "\n");
	}
	if (sf4_present)
	{
		list_text (2, ";   Mode 5 GNSS-derived Altitude:");
		list_text (2, " res=" M_FMT_BYTE ";", res);
		list_text (2, " ga=" M_FMT_SI16 " (%.2f FL)", ga, 0.25 * ga);
		list_text (2, "\n");
	}
	if (sf5_present)
	{
		list_text (2, ";   Extended Mode 1 Code"
                      " in Octal Representation:");
		list_text (2, " " M_FMT_CODE, em1c & 0x0fff);
		list_text (2, "\n");
	}
	if (sf6_present)
	{
		list_text (2, ";   Time Offset for POS and GA:");
		list_text (2, " " M_FMT_BYTE " (1/128 secs)", tos);
		list_text (2, "\n");
	}
	if (sf7_present)
	{
		list_text (2, ";   X Pulse Presence:");
		list_text (2, " X5=" M_FMT_BYTE ";", (xp >> 4) & 0x01);
		list_text (2, " XC=" M_FMT_BYTE ";", (xp >> 3) & 0x01);
		list_text (2, " X3=" M_FMT_BYTE ";", (xp >> 2) & 0x01);
		list_text (2, " X2=" M_FMT_BYTE ";", (xp >> 1) & 0x01);
		list_text (2, " X1=" M_FMT_BYTE, xp & 0x01);
		list_text (2, "\n");
	}
#endif /* LISTER */

                   /* Store this information: */
	if (sf1_present)
	{
		strk.mode_5_data.present = TRUE;
		strk.mode_5_data.sum_present = TRUE;
		strk.mode_5_data.value_sum = sum & 0xfe;
	}
	if (sf2_present)
	{
		strk.mode_5_data.present = TRUE;
		strk.mode_5_data.pnm_present = TRUE;
		strk.mode_5_data.value_mis = mis;
		strk.mode_5_data.value_nat = nat;
		strk.mode_5_data.value_pin = pin;
	}
	if (sf3_present)
	{
		strk.mode_5_data.present = TRUE;
		strk.mode_5_data.pos_present = TRUE;
		strk.mode_5_data.value_lat = (180.0 / M_TWO_POWER_23) * lat;
		strk.mode_5_data.value_lon = (180.0 / M_TWO_POWER_23) * lon;
	}
	if (sf4_present)
	{
		strk.mode_5_data.present = TRUE;
		strk.mode_5_data.ga_present = TRUE;
		strk.mode_5_data.value_ga = ga;
		strk.mode_5_data.value_res = res;
	}
	if (sf5_present)
	{
		strk.mode_5_data.present = TRUE;
		strk.mode_5_data.em1_present = TRUE;
		strk.mode_5_data.value_em1 = em1c;
	}
	if (sf6_present)
	{
		strk.mode_5_data.present = TRUE;
		strk.mode_5_data.tos_present = TRUE;
		strk.mode_5_data.value_tos = tos;
	}
	if (sf7_present)
	{
		strk.mode_5_data.present = TRUE;
		strk.mode_5_data.value_xp = xp;
		strk.mode_5_data.xp_present = TRUE;
	}

                   /* Set position: */
	*pos_ptr = pos + len;

                   /* Set the return code: */
	ret = RC_OKAY;

	done:          /* We are done */
	return ret;
}

/*----------------------------------------------------------------------------*/
/* proc_i062_120   -- Process I062/120 data item                              */
/*----------------------------------------------------------------------------*/

 static
 t_Retc proc_i062_120 (t_Ui16 length, t_Byte *buffer)
{
	t_Byte df1;    /* Data field octet 1 */
	t_Byte df2;    /* Data field octet 2 */
	t_Ui16 m2c;    /* Track mode 2 code */
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
	list_text (2, ";  Track Mode 2 Code:");
	list_text (2, " " M_FMT_CODE, m2c);
	list_text (2, "\n");
#endif /* LISTER */

				   /* Store this information: */
	strk.mode_2_info.code = m2c;
	strk.mode_2_info.present = TRUE;

				   /* Set the return code: */
	ret = RC_OKAY;

	return ret;
}

/*----------------------------------------------------------------------------*/
/* proc_i062_130   -- Process I062/130 data item                              */
/*----------------------------------------------------------------------------*/

 static
 t_Retc proc_i062_130 (t_Ui16 length, t_Byte *buffer)
{
	t_Si16 alt;    /* Altitude; 6.25 feet */
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
	alt = make_si16 (df1, df2);

#if LISTER
				   /* List data field: */
	list_text (2, ";  Calculated Track Geometric Altitude:");
	list_text (2, " " M_FMT_SI16 " (%.2f FL)", alt, 0.0625 * alt);
	list_text (2, "\n");
#endif /* LISTER */

				   /* Store this information: */
	strk.calculated_track_altitude.present = TRUE;
	strk.calculated_track_altitude.value = (6.25 * M_FT2MTR) * alt;
                   /* In metres */

				   /* Set the return code: */
	ret = RC_OKAY;

	return ret;
}

/*----------------------------------------------------------------------------*/
/* proc_i062_135   -- Process I062/135 data item                              */
/*----------------------------------------------------------------------------*/

 static
 t_Retc proc_i062_135 (t_Ui16 length, t_Byte *buffer)
{
	t_Si16 alt;    /* Altitude; 25 feet */
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
	alt = make_si16 (df1 & 0x3f, df2);
	if (df1 & 0x40)
	{
		alt |= 0xc000;
	}

#if LISTER
				   /* List data field: */
	list_text (2, ";  Calculated Track Barometric Altitude:");
	list_text (2, " qnh=" M_FMT_BYTE ";", (df1 >> 7) & 0x01);
	list_text (2, " alt=" M_FMT_SI16 " (%.2f FL)", alt, 0.25 * alt);
	list_text (2, "\n");
#endif /* LISTER */

				   /* Store this information: */
	strk.calculated_track_flight_level.present = TRUE;
	strk.calculated_track_flight_level.value = 25.0 * M_FT2MTR * alt;
	strk.calculated_track_flight_level.value_in_feet = 25 * alt;

				   /* Set the return code: */
	ret = RC_OKAY;

	return ret;
}

/*----------------------------------------------------------------------------*/
/* proc_i062_136   -- Process I062/136 data item                              */
/*----------------------------------------------------------------------------*/

 static
 t_Retc proc_i062_136 (t_Ui16 length, t_Byte *buffer)
{
	t_Si16 alt;    /* Altitude; 25 feet */
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
	alt = make_si16 (df1, df2);

#if LISTER
				   /* List data field: */
	list_text (2, ";  Measured Flight Level:");
	list_text (2, " " M_FMT_SI16 " (%.2f FL)", alt, 0.25 * alt);
	list_text (2, "\n");
#endif /* LISTER */

				   /* Store this information: */
	strk.measured_track_mode_c_height.present = TRUE;
	strk.measured_track_mode_c_height.value = 25.0 * M_FT2MTR * alt;
	strk.measured_track_mode_c_height.value_in_feet = 25 * alt;

				   /* Set the return code: */
	ret = RC_OKAY;

	return ret;
}

/*----------------------------------------------------------------------------*/
/* proc_i062_185   -- Process I062/185 data item                              */
/*----------------------------------------------------------------------------*/

 static
 t_Retc proc_i062_185 (t_Ui16 length, t_Byte *buffer)
{
	t_Byte df1;    /* Data field octet 1 */
	t_Byte df2;    /* Data field octet 2 */
	t_Byte df3;    /* Data field octet 3 */
	t_Byte df4;    /* Data field octet 4 */
	t_Retc ret;    /* Return code */
	t_Si16 vx;     /* x velocity component; 0.25 metres/second */
	t_Si16 vy;     /* y velocity component; 0.25 metres/second */

				   /* Preset the return code: */
	ret = RC_FAIL;

				   /* Check parameters: */
	Assert (length == 4, "Invalid length");
	Assert (buffer != NULL, "NULL pointer");

				   /* Extract octets: */
	df1 = buffer[0];
	df2 = buffer[1];
	df3 = buffer[2];
	df4 = buffer[3];

				   /* Extract information: */
	vx = make_si16 (df1, df2);
	vy = make_si16 (df3, df4);

#if LISTER
				   /* List data field: */
	list_text (2, ";  Calculated Track Velocity (Cartesian):");
	list_text (2, " vx=" M_FMT_SI16 " (%.3f kts);",
                  vx, (0.25 / M_KTS2MPS) * vx);
	list_text (2, " vy=" M_FMT_SI16 " (%.3f kts)",
                  vy, (0.25 / M_KTS2MPS) * vy);
	list_text (2, "\n");
#endif /* LISTER */

				   /* Store this information: */
	strk.calculated_cartesian_velocity.present = TRUE;
	strk.calculated_cartesian_velocity.value_vx = 0.25 * vx;
	strk.calculated_cartesian_velocity.value_vy = 0.25 * vy;
                   /* In metres/second */

				   /* Set the return code: */
	ret = RC_OKAY;

	return ret;
}

/*----------------------------------------------------------------------------*/
/* proc_i062_200   -- Process I062/200 data item                              */
/*----------------------------------------------------------------------------*/

 static
 t_Retc proc_i062_200 (t_Ui16 length, t_Byte *buffer)
{
	t_Byte df1;    /* Data field octet 1 */
	t_Byte longi;  /* Longitudinal acceleration */
	t_Retc ret;    /* Return code */
	t_Byte trans;  /* Transversal acceleration */
	t_Byte vert;   /* Vertical attitude */

				   /* Preset the return code: */
	ret = RC_FAIL;

				   /* Check parameters: */
	Assert (length == 1, "Invalid parameter");
	Assert (buffer != NULL, "Invalid parameter");

				   /* Extract octets: */
	df1 = buffer[0];

				   /* Extract information: */
	trans = (df1 >> 6) & 0x03;
	longi = (df1 >> 4) & 0x03;
	vert = (df1 >> 2) & 0x03;

#if LISTER
				   /* List data field: */
	list_text (2, ";  Mode of Movement:");
	list_text (2, " trans=" M_FMT_BYTE, trans);
	switch (trans)
	{
	case 0:
		list_text (2, " (constant course)");
		break;
	case 1:
		list_text (2, " (right turn)");
		break;
	case 2:
		list_text (2, " (left turn)");
		break;
	case 3:
		list_text (2, " (undetermined)");
		break;
	}
	list_text (2, "; long=" M_FMT_BYTE, longi);
	switch (longi)
	{
	case 0:
		list_text (2, " (constant groundspeed)");
		break;
	case 1:
		list_text (2, " (increasing groundspeed)");
		break;
	case 2:
		list_text (2, " (decreasing groundspeed)");
		break;
	case 3:
		list_text (2, " (undetermined)");
		break;
	}
	list_text (2, "; vert=" M_FMT_BYTE, vert);
	switch (vert)
	{
	case 0:
		list_text (2, " (level)");
		break;
	case 1:
		list_text (2, " (climb)");
		break;
	case 2:
		list_text (2, " (descent)");
		break;
	case 3:
		list_text (2, " (undetermined)");
		break;
	}
	list_text (2, "\n");
#endif /* LISTER */

				   /* Store this information: */
	strk.mode_of_flight.present = TRUE;
	strk.mode_of_flight.value_transversal = trans;
	strk.mode_of_flight.value_longitudinal = longi;
	strk.mode_of_flight.value_vertical = vert;

				   /* Set the return code: */
	ret = RC_OKAY;

	return ret;
}

/*----------------------------------------------------------------------------*/
/* proc_i062_210   -- Process I062/210 data item                              */
/*----------------------------------------------------------------------------*/

 static
 t_Retc proc_i062_210 (t_Ui16 length, t_Byte *buffer)
{
	t_Si16 ax;     /* x acceleration component; 0.25 metres/second**2 */
	t_Si16 ay;     /* y acceleration component; 0.25 metres/second**2 */
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
	if (df1 & 0x80)
	{
		ax = make_si16 (0xff, df1);
	}
	else
	{
		ax = make_si16 (0x00, df1);
	}
	if (df2 & 0x80)
	{
		ay = make_si16 (0xff, df2);
	}
	else
	{
		ay = make_si16 (0x00, df2);
	}

#if LISTER
				   /* List data field: */
	list_text (2, ";  Calculated Acceleration (Cartesian):");
	list_text (2, " ax=%.2f m/s**2;", 0.25 * ax);
	list_text (2, " ay=%.2f m/s**2", 0.25 * ay);
	list_text (2, "\n");
#endif /* LISTER */

				   /* Store this information: */
	strk.calculated_acceleration.present = TRUE;
	strk.calculated_acceleration.value_ax = 0.25 * ax;
	strk.calculated_acceleration.value_ay = 0.25 * ay;

				   /* Set the return code: */
	ret = RC_OKAY;

	return ret;
}

/*----------------------------------------------------------------------------*/
/* proc_i062_220   -- Process I062/220 data item                              */
/*----------------------------------------------------------------------------*/

 static
 t_Retc proc_i062_220 (t_Ui16 length, t_Byte *buffer)
{
	t_Byte df1;    /* Data field octet 1 */
	t_Byte df2;    /* Data field octet 2 */
	t_Si16 rcd;    /* Calculated rate of climb/descent; 6.25 feet/minute */
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
	rcd = make_si16 (df1, df2);

#if LISTER
				   /* List data field: */
	list_text (2, ";  Calculated Rate of Climb/Descent:");
	list_text (2, " " M_FMT_SI16 " (%.2f ft/min)", rcd, 6.25 * rcd);
	list_text (2, "\n");
#endif /* LISTER */

				   /* Store this information: */
	strk.calculated_rate_of_climb_descent.present = TRUE;
	strk.calculated_rate_of_climb_descent.value =
		(6.25 * M_FT2MTR / 60.0) * rcd;
                   /* In metres/second */

				   /* Set the return code: */
	ret = RC_OKAY;

	return ret;
}

/*----------------------------------------------------------------------------*/
/* proc_i062_245   -- Process I062/245 data item                              */
/*----------------------------------------------------------------------------*/

 static
 t_Retc proc_i062_245 (t_Ui16 length, t_Byte *buffer)
{
	t_Byte c;      /* 6 bits character */
	t_Byte df1;    /* Data field octet 1 */
	t_Byte df2;    /* Data field octet 2 */
	t_Byte df3;    /* Data field octet 3 */
	t_Byte df4;    /* Data field octet 4 */
	t_Byte df5;    /* Data field octet 5 */
	t_Byte df6;    /* Data field octet 6 */
	t_Byte df7;    /* Data field octet 7 */
	char idt[M_AIRCRAFT_IDENTIFICATION_LENGTH + 1];
                   /* Target identification */
	t_Retc ret;    /* Return code */
	t_Byte sti;    /* Source of target identification */

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
	sti = (df1 >> 6) & 0x03;
	memset (idt, 0, 9);
    c = (df2 >> 2) & 0x3f;
    idt[0] = expand_c (c);
    c = (df2 & 0x03) << 4;
    c |= (df3 >> 4) & 0x0f;
    idt[1] = expand_c (c);
    c = (df3 & 0x0f) << 2;
    c |= (df4 >> 6) & 0x03;
    idt[2] = expand_c (c);
    c = df4 & 0x03f;
    idt[3] = expand_c (c);
    c = (df5 >> 2) & 0x3f;
    idt[4] = expand_c (c);
    c = (df5 & 0x03) << 4;
    c |= (df6 >> 4) & 0x0f;
    idt[5] = expand_c (c);
    c = (df6 & 0x0f) << 2;
    c |= (df7 >> 6) & 0x03;
    idt[6] = expand_c (c);
    c = df7 & 0x3f;
    idt[7] = expand_c (c);

#if LISTER
				   /* List data field: */
	list_text (2, ";  Target Identification:");
	list_text (2, " sti=" M_FMT_BYTE ";", sti);
	list_text (2, " idt='%s'", idt);
	list_text (2, "\n");
#endif /* LISTER */

				   /* Store this information: */
	strk.aircraft_identification.present = TRUE;
	strk.aircraft_identification.sti_present = TRUE;
	strncpy (strk.aircraft_identification.value_idt, idt,
             M_AIRCRAFT_IDENTIFICATION_LENGTH);
	strk.aircraft_identification.value_sti = sti;

				   /* Set the return code: */
	ret = RC_OKAY;

	return ret;
}

/*----------------------------------------------------------------------------*/
/* proc_i062_270   -- Process I062/270 data item                              */
/*----------------------------------------------------------------------------*/

 static
 t_Retc proc_i062_270 (t_Ui16 length, t_Byte *buffer)
{
	t_Byte len;    /* Target length; metres */
	t_Byte orientation;
                   /* Orientation of target; degrees */
	t_Retc ret;    /* Return code */
	t_Byte width;  /* Target width; metres */

				   /* Preset the return code: */
	ret = RC_FAIL;

				   /* Check parameters: */
	Assert (length > 0, "Invalid parameter");
	Assert (length <= 3, "Invalid parameter");
	Assert (buffer != NULL, "Invalid parameter");

                   /* Preset local data: */
	len = 0;
	orientation = 0;
	width = 0;

#if LISTER
				   /* List data field: */
	list_text (2, ";  Target Size & Orientation:\n");
	if (length >= 1)
	{
		len = (buffer[0] >> 1) & 0x7f;

		list_text (2, ";   Length=");
		list_text (2, M_FMT_BYTE " mtr", len);
		list_text (2, "\n");
	}
	if (length >= 2)
	{
		orientation = (buffer[1] >> 1) & 0x7f;

		list_text (2, ";   Orientation=");
		list_text (2, M_FMT_BYTE " (%.3f deg)",
                      orientation, (360.0 / 128.0) * orientation);
		list_text (2, "\n");
	}
	if (length >= 3)
	{
		width = (buffer[2] >> 1) & 0x7f;

		list_text (2, ";   Width=");
		list_text (2, M_FMT_BYTE " mtr", width);
		list_text (2, "\n");
	}
#endif /* LISTER */

				   /* Store this information: */
	if (length >= 1)
	{
		strk.target_size.length_present = TRUE;
		strk.target_size.present = TRUE;
		strk.target_size.value_length = len;
	}
	if (length >= 2)
	{
		strk.target_size.orientation_present = TRUE;
		strk.target_size.present = TRUE;
		strk.target_size.value_orientation = (360.0 / 128.0) * orientation;
	}
	if (length >= 3)
	{
		strk.target_size.present = TRUE;
		strk.target_size.value_width = width;
		strk.target_size.width_present = TRUE;
	}

				   /* Set the return code: */
	ret = RC_OKAY;

	return ret;
}

/*----------------------------------------------------------------------------*/
/* proc_i062_290   -- Process I062/290 data item                              */
/*----------------------------------------------------------------------------*/

 static
 t_Retc proc_i062_290 (t_Ui16 length, t_Byte *buffer, t_Ui16 *pos_ptr)
{
	t_Ui16 ads;    /* Age of the last ADS-C report used to update
                      the track; 1/4 seconds */
	t_Byte df1;    /* Data field octet 1 */
	t_Byte df2;    /* Data field octet 2 */
	t_Byte df3;    /* Data field octet 3 */
	t_Byte df4;    /* Data field octet 4 */
	t_Byte es;     /* Age of the last 1090 extended squitter ADS-B
                      report used to update the track; 1/4 seconds */
	int ix;        /* Auxiliary */
#if LISTER
	int j;         /* Auxiliary */
#endif /* LISTER */
	t_Byte len;    /* Length of data field */
	t_Byte lop;    /* Age of the last magnetic loop detection;
                      1/4 seconds */
	t_Byte mds;    /* Age of the last mode S detection used to
                      update the track; 1/4 seconds */
	t_Byte mlt;    /* Age of the last MLT detection; 1/4 seconds */
	t_Ui16 pos;    /* Position within buffer */
	t_Byte psr;    /* Age of the last primary detection used to
                      update the track; 1/4 seconds */
	t_Retc ret;    /* Return code */
	t_Byte sf1_present;
                   /* Subfield 1 present (TRK) */
	t_Byte sf2_present;
                   /* Subfield 2 present (PSR) */
	t_Byte sf3_present;
                   /* Subfield 3 present (SSR) */
	t_Byte sf4_present;
                   /* Subfield 4 present (MDS) */
	t_Byte sf5_present;
                   /* Subfield 5 present (ADS) */
	t_Byte sf6_present;
                   /* Subfield 6 present (ES) */
	t_Byte sf7_present;
                   /* Subfield 7 present (VDL) */
	t_Byte sf8_present;
                   /* Subfield 8 present (UAT) */
	t_Byte sf9_present;
                   /* Subfield 9 present (LOP) */
	t_Byte sf10_present;
                   /* Subfield 10 present (MLT) */
	t_Byte ssr;    /* Age of the last secondary detection used
                      to update the track; 1/4 seconds */
	t_Byte trk;    /* Actual track age since first occurence;
                      1/4 seconds */
	t_Byte uat;    /* Age of the last UAT ADS-B report used to
                      update the track; 1/4 seconds */
	t_Byte vdl;    /* Age of the last VDL mode 4 ADS-B report
                      used to update the track; 1/4 seconds */

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

                   /* Preset presence of subfields: */
	sf1_present = FALSE;
	sf2_present = FALSE;
	sf3_present = FALSE;
	sf4_present = FALSE;
	sf5_present = FALSE;
	sf6_present = FALSE;
	sf7_present = FALSE;
	sf8_present = FALSE;
	sf9_present = FALSE;
	sf10_present = FALSE;

                   /* Extract octet: */
	df1 = buffer[pos];

                   /* Evaluate presence of subfields: */
	sf1_present = (df1 >> 7) & 0x01;
	sf2_present = (df1 >> 6) & 0x01;
	sf3_present = (df1 >> 5) & 0x01;
	sf4_present = (df1 >> 4) & 0x01;
	sf5_present = (df1 >> 3) & 0x01;
	sf6_present = (df1 >> 2) & 0x01;
	sf7_present = (df1 >> 1) & 0x01;

                   /* Check for second octet: */
	if (df1 & 0x01)
	{
		if (pos + 1 >= length)
		{
			error_msg ("Invalid buffer length (I062/290)");
			goto done;
		}

		df2 = buffer[pos + 1];

		sf8_present = (df2 >> 7) & 0x01;
		sf9_present = (df2 >> 6) & 0x01;
		sf10_present = (df2 >> 5) & 0x01;
	}

                   /* Determine expected length of data field: */
	len = 1 +
          1 * ord (df1 & 0x01) +
          1 * ord (sf1_present) +
          1 * ord (sf2_present) +
          1 * ord (sf3_present) +
          1 * ord (sf4_present) +
          2 * ord (sf5_present) +
          1 * ord (sf6_present) +
          1 * ord (sf7_present) +
          1 * ord (sf8_present) +
          1 * ord (sf9_present) +
          1 * ord (sf10_present);

                   /* Check against buffer length: */
	if (pos + len > length)
	{
		error_msg ("Invalid buffer length (I062/290)");
		goto done;
	}

#if LISTER
                   /* List raw data field: */
	list_text (1, ";  I062/290: 0x");
	j = 0;
	for (ix = 0; ix < len; ix ++)
	{
		list_text (1, " " M_FMT_BYTE_HEX, buffer[pos + ix]);
		++ j;

		if (j >= 16)
		{
			list_text (1, "\n");
			if (1 + ix < len)
			{
				list_text (1, ";          + 0x");
			}

			j = 0;
		}
		else if ((j % 4) == 0)
		{
			list_text (1, " ");
		}
	}
	if (j > 0)
	{
		list_text (1, "\n");
	}
#endif /* LISTER */

                   /* Extract secondary subfields: */
	ix = 1;
	if (df1 & 0x01)
	{
		ix = 2;
	}
	if (sf1_present)
	{
		trk = buffer[pos + ix];
		++ ix;
	}
	if (sf2_present)
	{
		psr = buffer[pos + ix];
		++ ix;
	}
	if (sf3_present)
	{
		ssr = buffer[pos + ix];
		++ ix;
	}
	if (sf4_present)
	{
		mds = buffer[pos + ix];
		++ ix;
	}
	if (sf5_present)
	{
		df3 = buffer[pos + ix];
		df4 = buffer[pos + ix + 1];
		ix += 2;

		ads = make_ui16 (df3, df4);
	}
	if (sf6_present)
	{
		es = buffer[pos + ix];
		++ ix;
	}
	if (sf7_present)
	{
		vdl = buffer[pos + ix];
		++ ix;
	}
	if (sf8_present)
	{
		uat = buffer[pos + ix];
		++ ix;
	}
	if (sf9_present)
	{
		lop = buffer[pos + ix];
		++ ix;
	}
	if (sf10_present)
	{
		mlt = buffer[pos + ix];
		++ ix;
	}
	Assert (ix == len, "Invalid structure decoding");

#if LISTER
                   /* List data field: */
	list_text (2, ";  System Track Update Ages:");
	list_text (2, "\n");
	if (sf1_present)
	{
		list_text (2, ";   Track age:");
		list_text (2, " " M_FMT_BYTE " (%.2f sec)", trk, 0.25 * trk);
		list_text (2, "\n");
	}
	if (sf2_present)
	{
		list_text (2, ";   PSR age:");
		list_text (2, " " M_FMT_BYTE " (%.2f sec)", psr, 0.25 * psr);
		list_text (2, "\n");
	}
	if (sf3_present)
	{
		list_text (2, ";   SSR age:");
		list_text (2, " " M_FMT_BYTE " (%.2f sec)", ssr, 0.25 * ssr);
		list_text (2, "\n");
	}
	if (sf4_present)
	{
		list_text (2, ";   MDS age:");
		list_text (2, " " M_FMT_BYTE " (%.2f sec)", mds, 0.25 * mds);
		list_text (2, "\n");
	}
	if (sf5_present)
	{
		list_text (2, ";   ADS-C age:");
		list_text (2, " " M_FMT_UI16 " (%.2f sec)", ads, 0.25 * ads);
		list_text (2, "\n");
	}
	if (sf6_present)
	{
		list_text (2, ";   ES age:");
		list_text (2, " " M_FMT_BYTE " (%.2f sec)", es, 0.25 * es);
		list_text (2, "\n");
	}
	if (sf7_present)
	{
		list_text (2, ";   VDL age:");
		list_text (2, " " M_FMT_BYTE " (%.2f sec)", vdl, 0.25 * vdl);
		list_text (2, "\n");
	}
	if (sf8_present)
	{
		list_text (2, ";   UAT age:");
		list_text (2, " " M_FMT_BYTE " (%.2f sec)", uat, 0.25 * uat);
		list_text (2, "\n");
	}
	if (sf9_present)
	{
		list_text (2, ";   LOP age:");
		list_text (2, " " M_FMT_BYTE " (%.2f sec)", lop, 0.25 * lop);
		list_text (2, "\n");
	}
	if (sf10_present)
	{
		list_text (2, ";   MLT age:");
		list_text (2, " " M_FMT_BYTE " (%.2f sec)", mlt, 0.25 * mlt);
		list_text (2, "\n");
	}
#endif /* LISTER */

                   /* Store this information: */
	if (sf1_present)
	{
		strk.update_ages.present = TRUE;
		strk.update_ages.trk_present = TRUE;
		strk.update_ages.value_trk = 0.25 * trk;
	}
	if (sf2_present)
	{
		strk.update_ages.present = TRUE;
		strk.update_ages.psr_present = TRUE;
		strk.update_ages.value_psr = 0.25 * psr;
	}
	if (sf3_present)
	{
		strk.update_ages.present = TRUE;
		strk.update_ages.ssr_present = TRUE;
		strk.update_ages.value_ssr = 0.25 * ssr;
	}
	if (sf4_present)
	{
		strk.update_ages.present = TRUE;
		strk.update_ages.mds_present = TRUE;
		strk.update_ages.value_mds = 0.25 * mds;
	}
	if (sf5_present)
	{
		strk.update_ages.present = TRUE;
		strk.update_ages.ads_present = TRUE;
		strk.update_ages.value_ads = 0.25 * ads;
	}
	if (sf6_present)
	{
		strk.update_ages.present = TRUE;
		strk.update_ages.es_present = TRUE;
		strk.update_ages.value_es = 0.25 * es;
	}
	if (sf7_present)
	{
		strk.update_ages.present = TRUE;
		strk.update_ages.value_vdl = 0.25 * vdl;
		strk.update_ages.vdl_present = TRUE;
	}
	if (sf8_present)
	{
		strk.update_ages.present = TRUE;
		strk.update_ages.uat_present = TRUE;
		strk.update_ages.value_uat = 0.25 * uat;
	}
	if (sf9_present)
	{
		strk.update_ages.present = TRUE;
		strk.update_ages.lop_present = TRUE;
		strk.update_ages.value_lop = 0.25 * lop;
	}
	if (sf10_present)
	{
		strk.update_ages.present = TRUE;
		strk.update_ages.mlt_present = TRUE;
		strk.update_ages.value_mlt = 0.25 * mlt;
	}

                   /* Set position: */
	*pos_ptr = pos + len;

                   /* Set the return code: */
	ret = RC_OKAY;

	done:          /* We are done */
	return ret;
}

/*----------------------------------------------------------------------------*/
/* proc_i062_295   -- Process I062/295 data item                              */
/*----------------------------------------------------------------------------*/

 static
 t_Retc proc_i062_295 (t_Ui16 length, t_Byte *buffer, t_Ui16 *pos_ptr)
{
	t_Byte acs;    /* Age of the DAP "ACAS Resolution Advisory
                      Report"; 1/4 seconds */
	t_Byte bps;    /* Age of the DAP "Barometric Pressure Setting";
                      1/4 seconds */
	t_Byte bvr;    /* Age of the DAP "Barometric Vertical Rate";
                      1/4 seconds */
	t_Byte com;    /* Age of the DAP "Communications/ACAS Capability
                      and Flight Status"; 1/4 seconds */
	t_Byte df1;    /* Data field octet 1 */
	t_Byte df2;    /* Data field octet 2 */
	t_Byte df3;    /* Data field octet 3 */
	t_Byte df4;    /* Data field octet 4 */
	t_Byte df5;    /* Data field octet 5 */
	t_Byte emc;    /* Age of the DAP "Emitter Category"; 1/4 seconds */
	t_Byte fss;    /* Age of the DAP "Final State Selected Altitude";
                      1/4 seconds */
	t_Byte gal;    /* Age of the DAP "Geometric Altitude";
                      1/4 seconds */
	t_Byte gsp;    /* Age of the DAP "Ground Speed"; 1/4 seconds */
	t_Byte gvr;    /* Age of the DAP "Geometrical Vertical Rate";
                      1/4 seconds */
	t_Byte iar;    /* Age of the DAP "Indicated Airspeed";
                      1/4 seconds */
	t_Byte ias;    /* Age of the DAP "Indicated Airspeed / Mach
                      Number"; 1/4 seconds */
	int ix;        /* Auxiliary */
#if LISTER
	int j;         /* Auxiliary */
#endif /* LISTER */
	t_Ui16 len;    /* Length of data field */
	t_Byte mac;    /* Age of the DAP "Mach Number"; 1/4 seconds */
	t_Byte mb;     /* Age of the DAP "Mode S MB Data";
                      1/4 seconds */
	t_Byte md1;    /* Age of the last valid and credible mode 1 code
                      used to update the track; 1/4 seconds */
	t_Byte md2;    /* Age of the last valid and credible mode 2 code
                      used to update the track; 1/4 seconds */
	t_Byte md4;    /* Age of the last valid and credible mode 4 code
                      used to update the track; 1/4 seconds */
	t_Byte md5;    /* Age of the last valid and credible mode 5 code
                      used to update the track; 1/4 seconds */
	t_Byte mda;    /* Age of the last valid and credible mode 3/A code
                      used to update the track; 1/4 seconds */
	t_Byte met;    /* Age of the DAP "Meteorological Data";
                      1/4 seconds */
	t_Byte mfl;    /* Age of the last valid and credible mode C code
                      or barometric altitude from ADS-B used to update
                      the track; 1/4 seconds */
	t_Byte mhg;    /* Age of the DAP "Magnetic Heading"; 1/4 seconds */
	t_Ui16 pos;    /* Position within buffer */
#if LISTER
	t_Byte psa;    /* Age of the DAP "Position"; 1/4 seconds */
#endif /* LISTER */
	t_Byte pun;    /* Age of the DAP "Position Uncertainty";
                      1/4 seconds */
	t_Byte ran;    /* Age of the DAP "Roll Angle"; 1/4 seconds */
	t_Retc ret;    /* Return code */
	t_Byte sab;    /* Age of the DAP "Status Reported by ADS-B";
                      1/4 seconds */
	t_Byte sal;    /* Age of the DAP "Selected Altitude";
                      1/4 seconds */
	t_Byte sf1_present;
                   /* Subfield 1 present (MFL) */
	t_Byte sf2_present;
                   /* Subfield 2 present (MD1) */
	t_Byte sf3_present;
                   /* Subfield 3 present (MD2) */
	t_Byte sf4_present;
                   /* Subfield 4 present (MDA) */
	t_Byte sf5_present;
                   /* Subfield 5 present (MD4) */
	t_Byte sf6_present;
                   /* Subfield 6 present (MD5) */
	t_Byte sf7_present;
                   /* Subfield 7 present (MHG) */
	t_Byte sf8_present;
                   /* Subfield 8 present (IAS) */
	t_Byte sf9_present;
                   /* Subfield 9 present (TAS) */
	t_Byte sf10_present;
                   /* Subfield 10 present (SAL) */
	t_Byte sf11_present;
                   /* Subfield 11 present (FSS) */
	t_Byte sf12_present;
                   /* Subfield 12 present (TID) */
	t_Byte sf13_present;
                   /* Subfield 13 present (COM) */
	t_Byte sf14_present;
                   /* Subfield 14 present (SAB) */
	t_Byte sf15_present;
                   /* Subfield 15 present (ACS) */
	t_Byte sf16_present;
                   /* Subfield 16 present (BVR) */
	t_Byte sf17_present;
                   /* Subfield 17 present (GVR) */
	t_Byte sf18_present;
                   /* Subfield 18 present (RAN) */
	t_Byte sf19_present;
                   /* Subfield 19 present (TAR) */
	t_Byte sf20_present;
                   /* Subfield 20 present (TAN) */
	t_Byte sf21_present;
                   /* Subfield 21 present (GSP) */
	t_Byte sf22_present;
                   /* Subfield 22 present (VUN) */
	t_Byte sf23_present;
                   /* Subfield 23 present (MET) */
	t_Byte sf24_present;
                   /* Subfield 24 present (EMC) */
	t_Byte sf25_present;
                   /* Subfield 25 present (POS) */
	t_Byte sf26_present;
                   /* Subfield 26 present (GAL) */
	t_Byte sf27_present;
                   /* Subfield 27 present (PUN) */
	t_Byte sf28_present;
                   /* Subfield 28 present (MB) */
	t_Byte sf29_present;
                   /* Subfield 29 present (IAR) */
	t_Byte sf30_present;
                   /* Subfield 30 present (MAC) */
	t_Byte sf31_present;
                   /* Subfield 31 present (BPS) */
	t_Byte tan;    /* Age of the DAP "Track Angle"; 1/4 seconds */
	t_Byte tar;    /* Age of the DAP "Track Angle Rate";
                      1/4 seconds */
	t_Byte tas;    /* Age of the DAP "True Airspeed"; 1/4 seconds */
	t_Byte tid;    /* Age of the DAP "Trajectory Intent";
                      1/4 seconds */
	t_Byte vun;    /* Age of the DAP "Velocity Uncertainty";
                      1/4 seconds */

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
	df1 = buffer[pos];

                   /* Evaluate presence of subfields: */
	sf1_present = (df1 >> 7) & 0x01;
	sf2_present = (df1 >> 6) & 0x01;
	sf3_present = (df1 >> 5) & 0x01;
	sf4_present = (df1 >> 4) & 0x01;
	sf5_present = (df1 >> 3) & 0x01;
	sf6_present = (df1 >> 2) & 0x01;
	sf7_present = (df1 >> 1) & 0x01;

                   /* Preset other data fields: */
	df2 = 0x00;
	df3 = 0x00;
	df4 = 0x00;
	df5 = 0x00;

                   /* Preset presence of other subfields: */
	sf8_present = FALSE;
	sf9_present = FALSE;
	sf10_present = FALSE;
	sf11_present = FALSE;
	sf12_present = FALSE;
	sf13_present = FALSE;
	sf14_present = FALSE;
	sf15_present = FALSE;
	sf16_present = FALSE;
	sf17_present = FALSE;
	sf18_present = FALSE;
	sf19_present = FALSE;
	sf20_present = FALSE;
	sf21_present = FALSE;
	sf22_present = FALSE;
	sf23_present = FALSE;
	sf24_present = FALSE;
	sf25_present = FALSE;
	sf26_present = FALSE;
	sf27_present = FALSE;
	sf28_present = FALSE;
	sf29_present = FALSE;
	sf30_present = FALSE;
	sf31_present = FALSE;

                   /* Check for second octet: */
	if (df1 & 0x01)
	{
		if (pos + 1 >= length)
		{
			error_msg ("Invalid buffer length (I062/295)");
			goto done;
		}

		df2 = buffer[pos + 1];

		sf8_present = (df2 >> 7) & 0x01;
		sf9_present = (df2 >> 6) & 0x01;
		sf10_present = (df2 >> 5) & 0x01;
		sf11_present = (df2 >> 4) & 0x01;
		sf12_present = (df2 >> 3) & 0x01;
		sf13_present = (df2 >> 2) & 0x01;
		sf14_present = (df2 >> 1) & 0x01;
	}

                   /* Check for third octet: */
	if (df2 & 0x01)
	{
		if (pos + 2 >= length)
		{
			error_msg ("Invalid buffer length (I062/295)");
			goto done;
		}

		df3 = buffer[pos + 2];

		sf15_present = (df3 >> 7) & 0x01;
		sf16_present = (df3 >> 6) & 0x01;
		sf17_present = (df3 >> 5) & 0x01;
		sf18_present = (df3 >> 4) & 0x01;
		sf19_present = (df3 >> 3) & 0x01;
		sf20_present = (df3 >> 2) & 0x01;
		sf21_present = (df3 >> 1) & 0x01;
	}

                   /* Check for fourth octet: */
	if (df3 & 0x01)
	{
		if (pos + 3 >= length)
		{
			error_msg ("Invalid buffer length (I062/295)");
			goto done;
		}

		df4 = buffer[pos + 3];

		sf22_present = (df4 >> 7) & 0x01;
		sf23_present = (df4 >> 6) & 0x01;
		sf24_present = (df4 >> 5) & 0x01;
		sf25_present = (df4 >> 4) & 0x01;
		sf26_present = (df4 >> 3) & 0x01;
		sf27_present = (df4 >> 2) & 0x01;
		sf28_present = (df4 >> 1) & 0x01;
	}

                   /* Check for fifth octet: */
	if (df4 & 0x01)
	{
		if (pos + 4 >= length)
		{
			error_msg ("Invalid buffer length (I062/295)");
			goto done;
		}

		df5 = buffer[pos + 4];

		sf29_present = (df5 >> 7) & 0x01;
		sf30_present = (df5 >> 6) & 0x01;
		sf31_present = (df5 >> 5) & 0x01;
	}

                   /* Fifth octet must not have the FX bit set: */
	Assert ((df5 & 0x01) == 0, "Unexpected FX bit");

                   /* Determine expected length of data field: */
	len = 1 +
          ord (df1 & 0x01) +
          ord (df2 & 0x01) +
          ord (df3 & 0x01) +
          ord (df4 & 0x01) +
          ord (sf1_present) +
          ord (sf2_present) +
          ord (sf3_present) +
          ord (sf4_present) +
          ord (sf5_present) +
          ord (sf6_present) +
          ord (sf7_present) +
          ord (sf8_present) +
          ord (sf9_present) +
          ord (sf10_present) +
          ord (sf11_present) +
          ord (sf12_present) +
          ord (sf13_present) +
          ord (sf14_present) +
          ord (sf15_present) +
          ord (sf16_present) +
          ord (sf17_present) +
          ord (sf18_present) +
          ord (sf19_present) +
          ord (sf20_present) +
          ord (sf21_present) +
          ord (sf22_present) +
          ord (sf23_present) +
          ord (sf24_present) +
          ord (sf25_present) +
          ord (sf26_present) +
          ord (sf27_present) +
          ord (sf28_present) +
          ord (sf29_present) +
          ord (sf30_present) +
          ord (sf31_present);

                   /* Check against buffer length: */
	if (pos + len > length)
	{
		error_msg ("Invalid buffer length (I062/295)");
		goto done;
	}

#if LISTER
                   /* List raw data field: */
	list_text (1, ";  I062/295: 0x");
	j = 0;
	for (ix = 0; ix < len; ix ++)
	{
		list_text (1, " " M_FMT_BYTE_HEX, buffer[pos + ix]);
		++ j;

		if (j >= 16)
		{
			list_text (1, "\n");
			if (1 + ix < len)
			{
				list_text (1, ";          + 0x");
			}

			j = 0;
		}
		else if ((j % 4) == 0)
		{
			list_text (1, " ");
		}
	}
	if (j > 0)
	{
		list_text (1, "\n");
	}
#endif /* LISTER */

                   /* Extract secondary subfields: */
	ix = 1 +
         ord (df1 & 0x01) +
         ord (df2 & 0x01) +
         ord (df3 & 0x01) +
         ord (df4 & 0x01);
	if (sf1_present)
	{
		mfl = buffer[pos + ix];
		++ ix;
	}
	if (sf2_present)
	{
		md1 = buffer[pos + ix];
		++ ix;
	}
	if (sf3_present)
	{
		md2 = buffer[pos + ix];
		++ ix;
	}
	if (sf4_present)
	{
		mda = buffer[pos + ix];
		++ ix;
	}
	if (sf5_present)
	{
		md4 = buffer[pos + ix];
		++ ix;
	}
	if (sf6_present)
	{
		md5 = buffer[pos + ix];
		++ ix;
	}
	if (sf7_present)
	{
		mhg = buffer[pos + ix];
		++ ix;
	}
	if (sf8_present)
	{
		ias = buffer[pos + ix];
		++ ix;
	}
	if (sf9_present)
	{
		tas = buffer[pos + ix];
		++ ix;
	}
	if (sf10_present)
	{
		sal = buffer[pos + ix];
		++ ix;
	}
	if (sf11_present)
	{
		fss = buffer[pos + ix];
		++ ix;
	}
	if (sf12_present)
	{
		tid = buffer[pos + ix];
		++ ix;
	}
	if (sf13_present)
	{
		com = buffer[pos + ix];
		++ ix;
	}
	if (sf14_present)
	{
		sab = buffer[pos + ix];
		++ ix;
	}
	if (sf15_present)
	{
		acs = buffer[pos + ix];
		++ ix;
	}
	if (sf16_present)
	{
		bvr = buffer[pos + ix];
		++ ix;
	}
	if (sf17_present)
	{
		gvr = buffer[pos + ix];
		++ ix;
	}
	if (sf18_present)
	{
		ran = buffer[pos + ix];
		++ ix;
	}
	if (sf19_present)
	{
		tar = buffer[pos + ix];
		++ ix;
	}
	if (sf20_present)
	{
		tan = buffer[pos + ix];
		++ ix;
	}
	if (sf21_present)
	{
		gsp = buffer[pos + ix];
		++ ix;
	}
	if (sf22_present)
	{
		vun = buffer[pos + ix];
		++ ix;
	}
	if (sf23_present)
	{
		met = buffer[pos + ix];
		++ ix;
	}
	if (sf24_present)
	{
		emc = buffer[pos + ix];
		++ ix;
	}
	if (sf25_present)
	{
#if LISTER
		psa = buffer[pos + ix];
#endif /* LISTER */
		++ ix;
	}
	if (sf26_present)
	{
		gal = buffer[pos + ix];
		++ ix;
	}
	if (sf27_present)
	{
		pun = buffer[pos + ix];
		++ ix;
	}
	if (sf28_present)
	{
		mb = buffer[pos + ix];
		++ ix;
	}
	if (sf29_present)
	{
		iar = buffer[pos + ix];
		++ ix;
	}
	if (sf30_present)
	{
		mac = buffer[pos + ix];
		++ ix;
	}
	if (sf31_present)
	{
		bps = buffer[pos + ix];
		++ ix;
	}
	Assert (ix == len, "Invalid structure decoding");

#if LISTER
                   /* List data field: */
	list_text (2, ";  Track Data Ages:");
	list_text (2, "\n");
	if (sf1_present)
	{
		list_text (2, ";   Measured flight level age:");
		list_text (2, " " M_FMT_BYTE " (%.2f sec)", mfl, 0.25 * mfl);
		list_text (2, "\n");
	}
	if (sf2_present)
	{
		list_text (2, ";   Mode 1 age:");
		list_text (2, " " M_FMT_BYTE " (%.2f sec)", md1, 0.25 * md1);
		list_text (2, "\n");
	}
	if (sf3_present)
	{
		list_text (2, ";   Mode 2 age:");
		list_text (2, " " M_FMT_BYTE " (%.2f sec)", md2, 0.25 * md2);
		list_text (2, "\n");
	}
	if (sf4_present)
	{
		list_text (2, ";   Mode 3/A age:");
		list_text (2, " " M_FMT_BYTE " (%.2f sec)", mda, 0.25 * mda);
		list_text (2, "\n");
	}
	if (sf5_present)
	{
		list_text (2, ";   Mode 4 age:");
		list_text (2, " " M_FMT_UI16 " (%.2f sec)", md4, 0.25 * md4);
		list_text (2, "\n");
	}
	if (sf6_present)
	{
		list_text (2, ";   Mode 5 age:");
		list_text (2, " " M_FMT_BYTE " (%.2f sec)", md5, 0.25 * md5);
		list_text (2, "\n");
	}
	if (sf7_present)
	{
		list_text (2, ";   Magnetic heading age:");
		list_text (2, " " M_FMT_BYTE " (%.2f sec)", mhg, 0.25 * mhg);
		list_text (2, "\n");
	}
	if (sf8_present)
	{
		list_text (2, ";   Indicated airspeed / Mach number age:");
		list_text (2, " " M_FMT_BYTE " (%.2f sec)", ias, 0.25 * ias);
		list_text (2, "\n");
	}
	if (sf9_present)
	{
		list_text (2, ";   True airspeed age:");
		list_text (2, " " M_FMT_BYTE " (%.2f sec)", tas, 0.25 * tas);
		list_text (2, "\n");
	}
	if (sf10_present)
	{
		list_text (2, ";   Selected altitude age:");
		list_text (2, " " M_FMT_BYTE " (%.2f sec)", sal, 0.25 * sal);
		list_text (2, "\n");
	}
	if (sf11_present)
	{
		list_text (2, ";   Final state selected altitude age:");
		list_text (2, " " M_FMT_BYTE " (%.2f sec)", fss, 0.25 * fss);
		list_text (2, "\n");
	}
	if (sf12_present)
	{
		list_text (2, ";   Trajectory intent age:");
		list_text (2, " " M_FMT_BYTE " (%.2f sec)", tid, 0.25 * tid);
		list_text (2, "\n");
	}
	if (sf13_present)
	{
		list_text (2, ";   Communication/ACAS capability and "
                      "flight status age:");
		list_text (2, " " M_FMT_BYTE " (%.2f sec)", com, 0.25 * com);
		list_text (2, "\n");
	}
	if (sf14_present)
	{
		list_text (2, ";   Status reported by ADS-B age:");
		list_text (2, " " M_FMT_BYTE " (%.2f sec)", sab, 0.25 * sab);
		list_text (2, "\n");
	}
	if (sf15_present)
	{
		list_text (2, ";   ACAS resolution advisory report age:");
		list_text (2, " " M_FMT_BYTE " (%.2f sec)", acs, 0.25 * acs);
		list_text (2, "\n");
	}
	if (sf16_present)
	{
		list_text (2, ";   Barometric vertical rate age:");
		list_text (2, " " M_FMT_BYTE " (%.2f sec)", bvr, 0.25 * bvr);
		list_text (2, "\n");
	}
	if (sf17_present)
	{
		list_text (2, ";   Geometrical vertical rate age:");
		list_text (2, " " M_FMT_BYTE " (%.2f sec)", gvr, 0.25 * gvr);
		list_text (2, "\n");
	}
	if (sf18_present)
	{
		list_text (2, ";   Roll angle age:");
		list_text (2, " " M_FMT_BYTE " (%.2f sec)", ran, 0.25 * ran);
		list_text (2, "\n");
	}
	if (sf19_present)
	{
		list_text (2, ";   Track angle rate age:");
		list_text (2, " " M_FMT_BYTE " (%.2f sec)", tar, 0.25 * tar);
		list_text (2, "\n");
	}
	if (sf20_present)
	{
		list_text (2, ";   Track angle age:");
		list_text (2, " " M_FMT_BYTE " (%.2f sec)", tan, 0.25 * tan);
		list_text (2, "\n");
	}
	if (sf21_present)
	{
		list_text (2, ";   Ground speed age:");
		list_text (2, " " M_FMT_BYTE " (%.2f sec)", gsp, 0.25 * gsp);
		list_text (2, "\n");
	}
	if (sf22_present)
	{
		list_text (2, ";   Velocity uncertainty age:");
		list_text (2, " " M_FMT_BYTE " (%.2f sec)", vun, 0.25 * vun);
		list_text (2, "\n");
	}
	if (sf23_present)
	{
		list_text (2, ";   Meteorological data age:");
		list_text (2, " " M_FMT_BYTE " (%.2f sec)", met, 0.25 * met);
		list_text (2, "\n");
	}
	if (sf24_present)
	{
		list_text (2, ";   Emitter category age:");
		list_text (2, " " M_FMT_BYTE " (%.2f sec)", emc, 0.25 * emc);
		list_text (2, "\n");
	}
	if (sf25_present)
	{
		list_text (2, ";   Position age:");
		list_text (2, " " M_FMT_BYTE " (%.2f sec)", psa, 0.25 * psa);
		list_text (2, "\n");
	}
	if (sf26_present)
	{
		list_text (2, ";   Geometric altitude age:");
		list_text (2, " " M_FMT_BYTE " (%.2f sec)", gal, 0.25 * gal);
		list_text (2, "\n");
	}
	if (sf27_present)
	{
		list_text (2, ";   Position uncertainty age:");
		list_text (2, " " M_FMT_BYTE " (%.2f sec)", pun, 0.25 * pun);
		list_text (2, "\n");
	}
	if (sf28_present)
	{
		list_text (2, ";   Mode S MB data age:");
		list_text (2, " " M_FMT_BYTE " (%.2f sec)", mb, 0.25 * mb);
		list_text (2, "\n");
	}
	if (sf29_present)
	{
		list_text (2, ";   Indicated airspeed data age:");
		list_text (2, " " M_FMT_BYTE " (%.2f sec)", iar, 0.25 * iar);
		list_text (2, "\n");
	}
	if (sf30_present)
	{
		list_text (2, ";   Mach number data age:");
		list_text (2, " " M_FMT_BYTE " (%.2f sec)", mac, 0.25 * mac);
		list_text (2, "\n");
	}
	if (sf31_present)
	{
		list_text (2, ";   Barometric pressure setting age:");
		list_text (2, " " M_FMT_BYTE " (%.2f sec)", bps, 0.25 * bps);
		list_text (2, "\n");
	}
#endif /* LISTER */

                   /* Store this information: */
	if (sf1_present)
	{
		strk.data_ages.present = TRUE;
		strk.data_ages.mfl_present = TRUE;
		strk.data_ages.value_mfl = 0.25 * mfl;
	}
	if (sf2_present)
	{
		strk.data_ages.present = TRUE;
		strk.data_ages.md1_present = TRUE;
		strk.data_ages.value_md1 = 0.25 * md1;
	}
	if (sf3_present)
	{
		strk.data_ages.present = TRUE;
		strk.data_ages.md2_present = TRUE;
		strk.data_ages.value_md2 = 0.25 * md2;
	}
	if (sf4_present)
	{
		strk.data_ages.present = TRUE;
		strk.data_ages.mda_present = TRUE;
		strk.data_ages.value_mda = 0.25 * mda;
	}
	if (sf5_present)
	{
		strk.data_ages.present = TRUE;
		strk.data_ages.md4_present = TRUE;
		strk.data_ages.value_md4 = 0.25 * md4;
	}
	if (sf6_present)
	{
		strk.data_ages.present = TRUE;
		strk.data_ages.md5_present = TRUE;
		strk.data_ages.value_md5 = 0.25 * md5;
	}
	if (sf7_present)
	{
		strk.data_ages.present = TRUE;
		strk.data_ages.mhg_present = TRUE;
		strk.data_ages.value_mhg = 0.25 * mhg;
	}
	if (sf8_present)
	{
		strk.data_ages.present = TRUE;
		strk.data_ages.ias_present = TRUE;
		strk.data_ages.value_ias = 0.25 * ias;
	}
	if (sf9_present)
	{
		strk.data_ages.present = TRUE;
		strk.data_ages.tas_present = TRUE;
		strk.data_ages.value_tas = 0.25 * tas;
	}
	if (sf10_present)
	{
		strk.data_ages.present = TRUE;
		strk.data_ages.sal_present = TRUE;
		strk.data_ages.value_sal = 0.25 * sal;
	}
	if (sf11_present)
	{
		strk.data_ages.present = TRUE;
		strk.data_ages.fss_present = TRUE;
		strk.data_ages.value_fss = 0.25 * fss;
	}
	if (sf12_present)
	{
		strk.data_ages.present = TRUE;
		strk.data_ages.tid_present = TRUE;
		strk.data_ages.value_tid = 0.25 * tid;
	}
	if (sf13_present)
	{
		strk.data_ages.present = TRUE;
		strk.data_ages.com_present = TRUE;
		strk.data_ages.value_com = 0.25 * com;
	}
	if (sf14_present)
	{
		strk.data_ages.present = TRUE;
		strk.data_ages.sab_present = TRUE;
		strk.data_ages.value_sab = 0.25 * sab;
	}
	if (sf15_present)
	{
		strk.data_ages.present = TRUE;
		strk.data_ages.acs_present = TRUE;
		strk.data_ages.value_acs = 0.25 * acs;
	}
	if (sf16_present)
	{
		strk.data_ages.present = TRUE;
		strk.data_ages.bvr_present = TRUE;
		strk.data_ages.value_bvr = 0.25 * bvr;
	}
	if (sf17_present)
	{
		strk.data_ages.present = TRUE;
		strk.data_ages.gvr_present = TRUE;
		strk.data_ages.value_gvr = 0.25 * gvr;
	}
	if (sf18_present)
	{
		strk.data_ages.present = TRUE;
		strk.data_ages.ran_present = TRUE;
		strk.data_ages.value_ran = 0.25 * ran;
	}
	if (sf19_present)
	{
		strk.data_ages.present = TRUE;
		strk.data_ages.tar_present = TRUE;
		strk.data_ages.value_tar = 0.25 * tar;
	}
	if (sf20_present)
	{
		strk.data_ages.present = TRUE;
		strk.data_ages.tan_present = TRUE;
		strk.data_ages.value_tan = 0.25 * tan;
	}
	if (sf21_present)
	{
		strk.data_ages.present = TRUE;
		strk.data_ages.gsp_present = TRUE;
		strk.data_ages.value_gsp = 0.25 * gsp;
	}
	if (sf22_present)
	{
		strk.data_ages.present = TRUE;
		strk.data_ages.vun_present = TRUE;
		strk.data_ages.value_vun = 0.25 * vun;
	}
	if (sf23_present)
	{
		strk.data_ages.present = TRUE;
		strk.data_ages.met_present = TRUE;
		strk.data_ages.value_met = 0.25 * met;
	}
	if (sf24_present)
	{
		strk.data_ages.present = TRUE;
		strk.data_ages.emc_present = TRUE;
		strk.data_ages.value_emc = 0.25 * emc;
	}
	if (sf25_present)
	{
		strk.data_ages.present = TRUE;
		strk.data_ages.pos_present = TRUE;
		strk.data_ages.value_pos = 0.25 * pos;
	}
	if (sf26_present)
	{
		strk.data_ages.present = TRUE;
		strk.data_ages.gal_present = TRUE;
		strk.data_ages.value_gal = 0.25 * gal;
	}
	if (sf27_present)
	{
		strk.data_ages.present = TRUE;
		strk.data_ages.pun_present = TRUE;
		strk.data_ages.value_pun = 0.25 * pun;
	}
	if (sf28_present)
	{
		strk.data_ages.present = TRUE;
		strk.data_ages.mb_present = TRUE;
		strk.data_ages.value_mb = 0.25 * mb;
	}
	if (sf29_present)
	{
		strk.data_ages.present = TRUE;
		strk.data_ages.iar_present = TRUE;
		strk.data_ages.value_iar = 0.25 * iar;
	}
	if (sf30_present)
	{
		strk.data_ages.present = TRUE;
		strk.data_ages.mac_present = TRUE;
		strk.data_ages.value_mac = 0.25 * mac;
	}
	if (sf31_present)
	{
		strk.data_ages.present = TRUE;
		strk.data_ages.bps_present = TRUE;
		strk.data_ages.value_bps = 0.25 * bps;
	}

                   /* Set position: */
	*pos_ptr = pos + len;

                   /* Set the return code: */
	ret = RC_OKAY;

	done:          /* We are done */
	return ret;
}

/*----------------------------------------------------------------------------*/
/* proc_i062_300   -- Process I062/300 data item                              */
/*----------------------------------------------------------------------------*/

 static
 t_Retc proc_i062_300 (t_Ui16 length, t_Byte *buffer)
{
	t_Byte df1;    /* Data field octet 1 */
	t_Retc ret;    /* Return code */

				   /* Preset the return code: */
	ret = RC_FAIL;

				   /* Check parameters: */
	Assert (length == 1, "Invalid parameter");
	Assert (buffer != NULL, "Invalid parameter");

				   /* Extract octets: */
	df1 = buffer[0];

#if LISTER
				   /* List data field: */
	list_text (2, ";  Vehicle Fleet Identification:");
	list_text (2, " " M_FMT_BYTE, df1);
	switch (df1)
	{
	case 0:
		list_text (2, " (Unknown)");
		break;
	case 1:
		list_text (2, " (ATC equipment maintenance)");
		break;
	case 2:
		list_text (2, " (Airport maintenance)");
		break;
	case 3:
		list_text (2, " (Fire)");
		break;
	case 4:
		list_text (2, " (Bird scarer)");
		break;
	case 5:
		list_text (2, " (Snow plough)");
		break;
	case 6:
		list_text (2, " (Runway sweeper)");
		break;
	case 7:
		list_text (2, " (Emergency)");
		break;
	case 8:
		list_text (2, " (Police)");
		break;
	case 9:
		list_text (2, " (Bus)");
		break;
	case 10:
		list_text (2, " (Tug (push/tow))");
		break;
	case 11:
		list_text (2, " (Grass cutter)");
		break;
	case 12:
		list_text (2, " (Fuel)");
		break;
	case 13:
		list_text (2, " (Baggage)");
		break;
	case 14:
		list_text (2, " (Catering)");
		break;
	case 15:
		list_text (2, " (Aircraft maintenance)");
		break;
	case 16:
		list_text (2, " (Flyco (follow me))");
		break;
	default:
		break;
	}
	list_text (2, "\n");
#endif /* LISTER */

				   /* Store this information: */
	strk.vehicle_fleet_identification.present = TRUE;
	strk.vehicle_fleet_identification.value = df1;

				   /* Set the return code: */
	ret = RC_OKAY;

	return ret;
}

/*----------------------------------------------------------------------------*/
/* proc_i062_340   -- Process I062/340 data item                              */
/*----------------------------------------------------------------------------*/

 static
 t_Retc proc_i062_340 (t_Ui16 length, t_Byte *buffer, t_Ui16 *pos_ptr)
{
	t_Byte df1;    /* Data field octet 1 */
	t_Byte df2;    /* Data field octet 2 */
	t_Byte df3;    /* Data field octet 3 */
	t_Byte df4;    /* Data field octet 4 */
	t_Byte df5;    /* Data field octet 5 */
	t_Si16 hgt;    /* Measured 3-D height; 25 feet */
	int ix;        /* Auxiliary */
#if LISTER
	int j;         /* Auxiliary */
#endif /* LISTER */
	t_Ui16 len;    /* Length of data field */
	t_Ui16 lma;    /* Last measured mode 3/A information */
	t_Ui16 lma_code;
                   /* Last measured mode 3/A code */
	t_Ui16 lmc;    /* Last measured mode C information */
	t_Si16 lmc_code;
                   /* Last measured mode C code; 25 feet */
	t_Ui16 pos;    /* Position within buffer */
	t_Retc ret;    /* Return code */
	t_Ui16 rho;    /* Measured distance; 1/256 nautical miles */
	t_Byte rtp;    /* Report type */
	t_Byte sf1_present;
                   /* Subfield 1 present (SID) */
	t_Byte sf2_present;
                   /* Subfield 2 present (POS) */
	t_Byte sf3_present;
                   /* Subfield 3 present (HEI) */
	t_Byte sf4_present;
                   /* Subfield 4 present (MDC) */
	t_Byte sf5_present;
                   /* Subfield 5 present (MDA) */
	t_Byte sf6_present;
                   /* Subfield 6 present (TYP) */
	t_Ui16 sid;    /* Sensor identification */
	t_Ui16 theta;  /* Measured azimuth; 360/2**16 degrees */

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
	df1 = buffer[pos];

                   /* Evaluate presence of subfields: */
	sf1_present = (df1 >> 7) & 0x01;
	sf2_present = (df1 >> 6) & 0x01;
	sf3_present = (df1 >> 5) & 0x01;
	sf4_present = (df1 >> 4) & 0x01;
	sf5_present = (df1 >> 3) & 0x01;
	sf6_present = (df1 >> 2) & 0x01;

                   /* First octet must not have the FX bit set: */
	if (df1 & 0x01)
	{
		error_msg ("Unexpected FX bit (I062/340)");
		goto done;
	}

                   /* Determine expected length of data field: */
	len = 1 +
          2 * ord (sf1_present) +
          4 * ord (sf2_present) +
          2 * ord (sf3_present) +
          2 * ord (sf4_present) +
          2 * ord (sf5_present) +
          ord (sf6_present);

                   /* Check against buffer length: */
	if (pos + len > length)
	{
		error_msg ("Invalid buffer length (I062/340)");
		goto done;
	}

#if LISTER
                   /* List raw data field: */
	list_text (1, ";  I062/340: 0x");
	j = 0;
	for (ix = 0; ix < len; ix ++)
	{
		list_text (1, " " M_FMT_BYTE_HEX, buffer[pos + ix]);
		++ j;

		if (j >= 16)
		{
			list_text (1, "\n");
			if (1 + ix < len)
			{
				list_text (1, ";          + 0x");
			}

			j = 0;
		}
		else if ((j % 4) == 0)
		{
			list_text (1, " ");
		}
	}
	if (j > 0)
	{
		list_text (1, "\n");
	}
#endif /* LISTER */

                   /* Extract secondary subfields: */
	ix = 1;
	if (sf1_present)
	{
		df2 = buffer[pos + ix];
		df3 = buffer[pos + ix + 1];
		ix += 2;

		sid = make_ui16 (df2, df3);
	}
	if (sf2_present)
	{
		df2 = buffer[pos + ix];
		df3 = buffer[pos + ix + 1];
		df4 = buffer[pos + ix + 2];
		df5 = buffer[pos + ix + 3];
		ix += 4;

		rho = make_ui16 (df2, df3);
		theta = make_ui16 (df4, df5);
	}
	if (sf3_present)
	{
		df2 = buffer[pos + ix];
		df3 = buffer[pos + ix + 1];
		ix += 2;

		hgt = make_si16 (df2, df3);
	}
	if (sf4_present)
	{
		df2 = buffer[pos + ix];
		df3 = buffer[pos + ix + 1];
		ix += 2;

		lmc = make_ui16 (df2, df3);
		lmc_code = lmc & 0x3fff;
		if (lmc & 0x2000)
		{
			lmc_code |= 0xc000;
		}
	}
	if (sf5_present)
	{
		df2 = buffer[pos + ix];
		df3 = buffer[pos + ix + 1];
		ix += 2;

		lma = make_ui16 (df2, df3);
		lma_code = lma & 0x0fff;
	}
	if (sf6_present)
	{
		rtp = buffer[pos + ix];
		++ ix;
	}
	Assert (ix == len, "Invalid structure decoding");

#if LISTER
                   /* List data field: */
	list_text (2, ";  Measured Information:");
	list_text (2, "\n");
	if (sf1_present)
	{
		list_text (2, ";   Sensor identification:");
		list_text (2, " SAC=" M_FMT_BYTE "; SIC=" M_FMT_BYTE,
                      (sid >> 8) & 0x00ff, sid & 0x00ff);
		list_text (2, "\n");
	}
	if (sf2_present)
	{
		list_text (2, ";   Measured position:");
		list_text (2, " rho=" M_FMT_UI16 " (%.3f nmi);",
                      rho, (1.0 / 256.0) * rho);
		list_text (2, " theta=" M_FMT_UI16 " (%.3f deg)",
                      theta, (360.0 / 65536.0) * theta);
		list_text (2, "\n");
	}
	if (sf3_present)
	{
		list_text (2, ";   Measured 3-D height:");
		list_text (2, " " M_FMT_SI16 " (%.2f FL)", hgt, 0.25 * hgt);
		list_text (2, "\n");
	}
	if (sf4_present)
	{
		list_text (2, ";   Last measured mode C code:");
		list_text (2, " v=" M_FMT_UI16 ";", (lmc >> 15) & 0x0001);
		list_text (2, " g=" M_FMT_UI16 ";", (lmc >> 14) & 0x0001);
		list_text (2, " code=" M_FMT_SI16 " (%.2f FL)",
                      lmc_code, 0.25 * lmc_code);
		list_text (2, "\n");
	}
	if (sf5_present)
	{
		list_text (2, ";   Last measured mode 3/A code:");
		list_text (2, " v=" M_FMT_UI16 ";", (lma >> 15) & 0x0001);
		list_text (2, " g=" M_FMT_UI16 ";", (lma >> 14) & 0x0001);
		list_text (2, " l=" M_FMT_UI16 ";", (lma >> 13) & 0x0001);
		list_text (2, " code=" M_FMT_CODE, lma_code);
		list_text (2, "\n");
	}
	if (sf6_present)
	{
		t_Byte typ;

		typ = (rtp >> 5) & 0x07;

		list_text (2, ";   Report type:");
		list_text (2, " typ=" M_FMT_BYTE, typ);
		switch (typ)
		{
		case 0:
			list_text (2, " (no detection)");
			break;
		case 1:
			list_text (2, " (single PSR detection)");
			break;
		case 2:
			list_text (2, " (single SSR detection)");
			break;
		case 3:
			list_text (2, " (SSR + PSR detection)");
			break;
		case 4:
			list_text (2, " (single mode S all-call)");
			break;
		case 5:
			list_text (2, " (single mode S roll-call)");
			break;
		case 6:
			list_text (2, " (mode S all-call + PSR)");
			break;
		case 7:
			list_text (2, " (mode S roll-call + PSR)");
			break;
		}
		list_text (2, "; sim=" M_FMT_BYTE, (rtp >> 4) & 0x01);
		list_text (2, "; rab=" M_FMT_BYTE, (rtp >> 3) & 0x01);
		list_text (2, "; tst=" M_FMT_BYTE, (rtp >> 2) & 0x01);
		list_text (2, "\n");
	}
#endif /* LISTER */

                   /* Store this information: */
	if (sf1_present)
	{
		strk.measured_information.present = TRUE;
		strk.measured_information.sid_present = TRUE;
		strk.measured_information.value_sid = sid;
	}
	if (sf2_present)
	{
		strk.measured_information.present = TRUE;
		strk.measured_information.pos_present = TRUE;
		strk.measured_information.value_rho = (M_NMI2MTR / 256.0) * rho;
		strk.measured_information.value_theta = (M_TWO_PI / 65536.0) * theta;
	}
	if (sf3_present)
	{
		strk.measured_information.present = TRUE;
		strk.measured_information.h3d_present = TRUE;
		strk.measured_information.value_h3d = hgt;
	}
	if (sf4_present)
	{
		strk.measured_information.present = TRUE;
		strk.measured_information.mdc_present = TRUE;
		strk.measured_information.value_mdc = lmc_code;
		strk.measured_information.value_mdc_garbled =
			((lmc >> 14) & 0x0001);
		strk.measured_information.value_mdc_invalid =
			((lmc >> 15) & 0x0001);
	}
	if (sf5_present)
	{
		strk.measured_information.present = TRUE;
		strk.measured_information.mda_present = TRUE;
		strk.measured_information.value_mda = lma_code;
		strk.measured_information.value_mda_garbled =
			((lma >> 14) & 0x0001);
		strk.measured_information.value_mda_invalid =
			((lma >> 15) & 0x0001);
		strk.measured_information.value_mda_smoothed =
			((lma >> 13) & 0x0001);
	}
	if (sf6_present)
	{
		t_Byte typ;

		strk.measured_information.present = TRUE;
		strk.measured_information.typ_present = TRUE;

		typ = (rtp >> 5) & 0x07;
		switch (typ)
		{
		case 0:
			break;
		case 1:
			strk.measured_information.value_typ_psr = TRUE;
			break;
		case 2:
			strk.measured_information.value_typ_ssr = TRUE;
			break;
		case 3:
			strk.measured_information.value_typ_psr = TRUE;
			strk.measured_information.value_typ_ssr = TRUE;
			break;
		case 4:
			strk.measured_information.value_typ_mds = TRUE;
			strk.measured_information.value_typ_mds_all_call = TRUE;
			break;
		case 5:
			strk.measured_information.value_typ_mds = TRUE;
			strk.measured_information.value_typ_mds_roll_call = TRUE;
			break;
		case 6:
			strk.measured_information.value_typ_mds = TRUE;
			strk.measured_information.value_typ_mds_all_call = TRUE;
			strk.measured_information.value_typ_psr = TRUE;
			break;
		case 7:
			strk.measured_information.value_typ_mds = TRUE;
			strk.measured_information.value_typ_mds_roll_call = TRUE;
			strk.measured_information.value_typ_psr = TRUE;
			break;
		}

		strk.measured_information.value_typ_sim = (typ >> 4) & 0x01;
		strk.measured_information.value_typ_rab = (typ >> 3) & 0x01;
		strk.measured_information.value_typ_tst = (typ >> 2) & 0x01;
	}

                   /* Set position: */
	*pos_ptr = pos + len;

                   /* Set the return code: */
	ret = RC_OKAY;

	done:          /* We are done */
	return ret;
}

/*----------------------------------------------------------------------------*/
/* proc_i062_380   -- Process I062/380 data item                              */
/*----------------------------------------------------------------------------*/

 static
 t_Retc proc_i062_380 (t_Ui16 length, t_Byte *buffer, t_Ui16 *pos_ptr)
{
	t_Byte acs[7]; /* ACAS resolution advisory report */
	t_Ui32 adr;    /* Target address */
#if LISTER
	t_Ui16 bps;    /* Barometric pressure setting */
#endif /* LISTER */
	t_Si16 bvr;    /* Barometric vertical rate; 6.25 feet/minute */
	t_Byte c;      /* Auxiliary */
	t_Ui16 com;    /* Communications/ACAS capability and flight
                      status */
	t_Byte df1;    /* Data field octet 1 */
	t_Byte df2;    /* Data field octet 2 */
	t_Byte df3;    /* Data field octet 3 */
	t_Byte df4;    /* Data field octet 4 */
	t_Byte df5;    /* Data field octet 5 */
	t_Byte df6;    /* Data field octet 6 */
	t_Byte df7;    /* Data field octet 7 */
	t_Byte df8;    /* Data field octet 8 */
	t_Byte df9;    /* Data field octet 9 */
	t_Byte df10;   /* Data field octet 10 */
#if LISTER
	t_Byte emc;    /* Emitter category */
#endif /* LISTER */
	t_Ui16 fss;    /* Final state selected altitude */
	t_Si16 gal;    /* Geometric altitude; 6.25 feet */
#if LISTER
	t_Si16 gsp;    /* Ground speed; 2**-14 nautical miles/second */
#endif /* LISTER */
	t_Si16 gvr;    /* Geometric vertical rate; 6.25 feet/minute */
#if LISTER
	t_Ui16 iar;    /* Indicated airspeed; knots */
#endif /* LISTER */
	t_Ui16 ias;    /* Indicated airspeed / Mach number */
	char id[M_AIRCRAFT_IDENTIFICATION_LENGTH + 1];
                   /* Target identification */
	int ix;        /* Auxiliary */
#if LISTER
	int j;         /* Auxiliary */
#endif /* LISTER */
	t_Byte len;    /* Length of data field */
#if LISTER
	t_Ui16 mac;    /* Mach number; 0.008 Mach */
#endif /* LISTER */
	int mb_cnt;    /* Count for MB data (BDS registers) */
	t_Byte mb_data[M_MAX_BDS_REGISTERS][1 + M_BDS_REGISTER_LENGTH];
                   /* Buffer for MB data */
	t_Byte mb_rep; /* Repetition factor for DAP "Mode S MB Data" */
#if CCWARN
	t_Byte met[8]; /* MET data */
#endif /* CCWARN */
	t_Ui16 mhg;    /* Magnetic heading; 360/2**16 degrees */
	t_Ui16 pos;    /* Position within buffer */
#if LISTER
	t_Si32 pos_lat;
                   /* Position - latitude; 180/2**23 degrees */
	t_Si32 pos_lon;
                   /* Position - longitude; 180/2**23 degrees */
#endif /* LISTER */
	t_Byte pun;    /* Position uncertainty */
#if LISTER
	t_Si16 ran;    /* Roll angle; 0.01 degrees */
#endif /* LISTER */
	t_Retc ret;    /* Return code */
#if LISTER
	t_Ui16 sab;    /* Status reported by ADS-B */
#endif /* LISTER */
	t_Ui16 sal;    /* Selected altitude */
	t_Byte sf1_present;
                   /* Subfield 1 present (ADR) */
	t_Byte sf2_present;
                   /* Subfield 2 present (ID) */
	t_Byte sf3_present;
                   /* Subfield 3 present (MHG) */
	t_Byte sf4_present;
                   /* Subfield 4 present (IAS) */
	t_Byte sf5_present;
                   /* Subfield 5 present (TAS) */
	t_Byte sf6_present;
                   /* Subfield 6 present (SAL) */
	t_Byte sf7_present;
                   /* Subfield 7 present (FSS) */
	t_Byte sf8_present;
                   /* Subfield 8 present (TIS) */
	t_Byte sf9_present;
                   /* Subfield 9 present (TID) */
	t_Byte sf10_present;
                   /* Subfield 10 present (COM) */
	t_Byte sf11_present;
                   /* Subfield 11 present (SAB) */
	t_Byte sf12_present;
                   /* Subfield 12 present (ACS) */
	t_Byte sf13_present;
                   /* Subfield 13 present (BVR) */
	t_Byte sf14_present;
                   /* Subfield 14 present (GVR) */
	t_Byte sf15_present;
                   /* Subfield 15 present (RAN) */
	t_Byte sf16_present;
                   /* Subfield 16 present (TAR) */
	t_Byte sf17_present;
                   /* Subfield 17 present (TAN) */
	t_Byte sf18_present;
                   /* Subfield 18 present (GSP) */
	t_Byte sf19_present;
                   /* Subfield 19 present (VUN) */
	t_Byte sf20_present;
                   /* Subfield 20 present (MET) */
	t_Byte sf21_present;
                   /* Subfield 21 present (EMC) */
	t_Byte sf22_present;
                   /* Subfield 22 present (POS) */
	t_Byte sf23_present;
                   /* Subfield 23 present (GAL) */
	t_Byte sf24_present;
                   /* Subfield 24 present (PUN) */
	t_Byte sf25_present;
                   /* Subfield 25 present (MB) */
	t_Byte sf26_present;
                   /* Subfield 26 present (IAR) */
	t_Byte sf27_present;
                   /* Subfield 27 present (MAC) */
	t_Byte sf28_present;
                   /* Subfield 28 present (BPS) */
#if LISTER
	t_Ui16 tan;    /* Track angle; 360/2**16 degrees */
	t_Ui16 tar;    /* Track angle rate */
#endif /* LISTER */
	t_Ui16 tas;    /* True airspeed; knots */
	t_Byte tid_rep;
                   /* Repetition factor for DAP "Trajectory Intent
                      Data" */
#if LISTER
	t_Byte tis;    /* Trajectory intent status */
	t_Byte vun;    /* Velocity uncertainty */
#endif /* LISTER */

				   /* Preset the return code: */
	ret = RC_FAIL;

				   /* Check parameters: */
	Assert (length > 0, "Invalid parameter");
	Assert (buffer != NULL, "Invalid parameter");
	Assert (pos_ptr != NULL, "Invalid parameter");

                   /* Preset local data: */
	mb_cnt = 0;
	mb_rep = 0;
	tid_rep = 0;
				   /* Set position within buffer: */
	pos = *pos_ptr;

				   /* Check position: */
	Assert (pos < length, "Invalid position");

                   /* Extract octet: */
	df1 = buffer[pos];

                   /* Evaluate presence of subfields: */
	sf1_present = (df1 >> 7) & 0x01;
	sf2_present = (df1 >> 6) & 0x01;
	sf3_present = (df1 >> 5) & 0x01;
	sf4_present = (df1 >> 4) & 0x01;
	sf5_present = (df1 >> 3) & 0x01;
	sf6_present = (df1 >> 2) & 0x01;
	sf7_present = (df1 >> 1) & 0x01;

                   /* Preset other data fields: */
	df2 = 0x00;
	df3 = 0x00;
	df4 = 0x00;

                   /* Preset presence of other subfields: */
	sf8_present = FALSE;
	sf9_present = FALSE;
	sf10_present = FALSE;
	sf11_present = FALSE;
	sf12_present = FALSE;
	sf13_present = FALSE;
	sf14_present = FALSE;
	sf15_present = FALSE;
	sf16_present = FALSE;
	sf17_present = FALSE;
	sf18_present = FALSE;
	sf19_present = FALSE;
	sf20_present = FALSE;
	sf21_present = FALSE;
	sf22_present = FALSE;
	sf23_present = FALSE;
	sf24_present = FALSE;
	sf25_present = FALSE;
	sf26_present = FALSE;
	sf27_present = FALSE;
	sf28_present = FALSE;

                   /* Check for second octet: */
	if (df1 & 0x01)
	{
		if (pos + 1 >= length)
		{
			error_msg ("Invalid buffer length (I062/380)");
			goto done;
		}

		df2 = buffer[pos + 1];

		sf8_present = (df2 >> 7) & 0x01;
		sf9_present = (df2 >> 6) & 0x01;
		sf10_present = (df2 >> 5) & 0x01;
		sf11_present = (df2 >> 4) & 0x01;
		sf12_present = (df2 >> 3) & 0x01;
		sf13_present = (df2 >> 2) & 0x01;
		sf14_present = (df2 >> 1) & 0x01;
	}

                   /* Check for third octet: */
	if (df2 & 0x01)
	{
		if (pos + 2 >= length)
		{
			error_msg ("Invalid buffer length (I062/380)");
			goto done;
		}

		df3 = buffer[pos + 2];

		sf15_present = (df3 >> 7) & 0x01;
		sf16_present = (df3 >> 6) & 0x01;
		sf17_present = (df3 >> 5) & 0x01;
		sf18_present = (df3 >> 4) & 0x01;
		sf19_present = (df3 >> 3) & 0x01;
		sf20_present = (df3 >> 2) & 0x01;
		sf21_present = (df3 >> 1) & 0x01;
	}

                   /* Check for fourth octet: */
	if (df3 & 0x01)
	{
		if (pos + 3 >= length)
		{
			error_msg ("Invalid buffer length (I062/380)");
			goto done;
		}

		df4 = buffer[pos + 3];

		sf22_present = (df4 >> 7) & 0x01;
		sf23_present = (df4 >> 6) & 0x01;
		sf24_present = (df4 >> 5) & 0x01;
		sf25_present = (df4 >> 4) & 0x01;
		sf26_present = (df4 >> 3) & 0x01;
		sf27_present = (df4 >> 2) & 0x01;
		sf28_present = (df4 >> 1) & 0x01;
	}

                   /* Fourth octet must not have the FX bit set: */
	Assert ((df4 & 0x01) == 0, "Unexpected FX bit");

                   /* Determine expected length of data field: */
	len = 1 +
          ord (df1 & 0x01) +
          ord (df2 & 0x01) +
          ord (df3 & 0x01) +
          3 * ord (sf1_present) +
          6 * ord (sf2_present) +
          2 * ord (sf3_present) +
          2 * ord (sf4_present) +
          2 * ord (sf5_present) +
          2 * ord (sf6_present) +
          2 * ord (sf7_present) +
          ord (sf8_present);
	if (sf9_present)
	{
		if (pos + len >= length)
		{
			error_msg ("Invalid buffer length (I062/380)");
			goto done;
		}

		tid_rep = buffer[pos + len];

		len += (1 + tid_rep * 15);
	}
	len = len +
          2 * ord (sf10_present) +
          2 * ord (sf11_present) +
          7 * ord (sf12_present) +
          2 * ord (sf13_present) +
          2 * ord (sf14_present) +
          2 * ord (sf15_present) +
          2 * ord (sf16_present) +
          2 * ord (sf17_present) +
          2 * ord (sf18_present) +
          ord (sf19_present) +
          8 * ord (sf20_present) +
          ord (sf21_present) +
          6 * ord (sf22_present) +
          2 * ord (sf23_present) +
          ord (sf24_present);
	if (sf25_present)
	{
		if (pos + len >= length)
		{
			error_msg ("Invalid buffer length (I062/380)");
			goto done;
		}

		mb_rep = buffer[pos + len];

		len += (1 + mb_rep * 8);
	}
	len = len +
          2 * ord (sf26_present) +
          2 * ord (sf27_present) +
          2 * ord (sf28_present);

                   /* Check against buffer length: */
	if (pos + len > length)
	{
		error_msg ("Invalid buffer length (I062/380)");
		goto done;
	}

#if LISTER
                   /* List raw data field: */
	list_text (1, ";  I062/380: 0x");
	j = 0;
	for (ix = 0; ix < len; ix ++)
	{
		list_text (1, " " M_FMT_BYTE_HEX, buffer[pos + ix]);
		++ j;

		if (j >= 16)
		{
			list_text (1, "\n");
			if (ix + 1 < len)
			{
				list_text (1, ";          + 0x");
			}

			j = 0;
		}
		else if ((j % 4) == 0)
		{
			list_text (1, " ");
		}
	}
	if (j > 0)
	{
		list_text (1, "\n");
	}
#endif /* LISTER */

                   /* Extract secondary subfields: */
	ix = 1 +
         ord (df1 & 0x01) +
         ord (df2 & 0x01) +
         ord (df3 & 0x01);
	if (sf1_present)
	{
		df5 = buffer[pos + ix];
		df6 = buffer[pos + ix + 1];
		df7 = buffer[pos + ix + 2];
		ix += 3;

		adr = make_ui32 (0x00, df5, df6, df7);
	}
	if (sf2_present)
	{
		df5 = buffer[pos + ix];
		df6 = buffer[pos + ix + 1];
		df7 = buffer[pos + ix + 2];
		df8 = buffer[pos + ix + 3];
		df9 = buffer[pos + ix + 4];
		df10 = buffer[pos + ix + 5];
		ix += 6;

		memset (id, 0, 9);

		c = (df5 >> 2) & 0x3f;
		id[0] = expand_c (c);
		c = (df5 & 0x03) << 4;
		c |= (df6 >> 4) & 0x0f;
		id[1] = expand_c (c);
		c = (df6 & 0x0f) << 2;
		c |= (df7 >> 6) & 0x03;
		id[2] = expand_c (c);
		c = df7 & 0x03f;
		id[3] = expand_c (c);
		c = (df8 >> 2) & 0x3f;
		id[4] = expand_c (c);
		c = (df8 & 0x03) << 4;
		c |= (df9 >> 4) & 0x0f;
		id[5] = expand_c (c);
		c = (df9 & 0x0f) << 2;
		c |= (df10 >> 6) & 0x03;
		id[6] = expand_c (c);
		c = df10 & 0x3f;
		id[7] = expand_c (c);
	}
	if (sf3_present)
	{
		df5 = buffer[pos + ix];
		df6 = buffer[pos + ix + 1];
		ix += 2;

		mhg = make_ui16 (df5, df6);
	}
	if (sf4_present)
	{
		df5 = buffer[pos + ix];
		df6 = buffer[pos + ix + 1];
		ix += 2;

		ias = make_ui16 (df5, df6);
	}
	if (sf5_present)
	{
		df5 = buffer[pos + ix];
		df6 = buffer[pos + ix + 1];
		ix += 2;

		tas = make_ui16 (df5, df6);
	}
	if (sf6_present)
	{
		df5 = buffer[pos + ix];
		df6 = buffer[pos + ix + 1];
		ix += 2;

		sal = make_ui16 (df5, df6);
	}
	if (sf7_present)
	{
		df5 = buffer[pos + ix];
		df6 = buffer[pos + ix + 1];
		ix += 2;

		fss = make_ui16 (df5, df6);
	}
	if (sf8_present)
	{
#if LISTER
		tis = buffer[pos + ix];
#endif /* LISTER */

		++ ix;
	}
	if (sf9_present)
	{
		Assert (tid_rep == buffer[pos + ix],
                "Invalid structure decoding");

		/* tba ... */

		ix += (1 + tid_rep * 15); 
	}
	if (sf10_present)
	{
		df5 = buffer[pos + ix];
		df6 = buffer[pos + ix + 1];
		ix += 2;

		com = make_ui16 (df5, df6);
	}
	if (sf11_present)
	{
		df5 = buffer[pos + ix];
		df6 = buffer[pos + ix + 1];
		ix += 2;

#if LISTER
		sab = make_ui16 (df5, df6);
#endif /* LISTER */
	}
	if (sf12_present)
	{
		acs[0] = buffer[pos + ix];
		acs[1] = buffer[pos + ix + 1];
		acs[2] = buffer[pos + ix + 2];
		acs[3] = buffer[pos + ix + 3];
		acs[4] = buffer[pos + ix + 4];
		acs[5] = buffer[pos + ix + 5];
		acs[6] = buffer[pos + ix + 6];
		ix += 7;
	}
	if (sf13_present)
	{
		df5 = buffer[pos + ix];
		df6 = buffer[pos + ix + 1];
		ix += 2;

		bvr = make_si16 (df5, df6);
	}
	if (sf14_present)
	{
		df5 = buffer[pos + ix];
		df6 = buffer[pos + ix + 1];
		ix += 2;

		gvr = make_si16 (df5, df6);
	}
	if (sf15_present)
	{
		df5 = buffer[pos + ix];
		df6 = buffer[pos + ix + 1];
		ix += 2;

#if LISTER
		ran = make_si16 (df5, df6);
#endif /* LISTER */
	}
	if (sf16_present)
	{
		df5 = buffer[pos + ix];
		df6 = buffer[pos + ix + 1];
		ix += 2;

#if LISTER
		tar = make_ui16 (df5, df6);
#endif /* LISTER */
	}
	if (sf17_present)
	{
		df5 = buffer[pos + ix];
		df6 = buffer[pos + ix + 1];
		ix += 2;

#if LISTER
		tan = make_ui16 (df5, df6);
#endif /* LISTER */
	}
	if (sf18_present)
	{
		df5 = buffer[pos + ix];
		df6 = buffer[pos + ix + 1];
		ix += 2;

#if LISTER
		gsp = make_si16 (df5, df6);
#endif /* LISTER */
	}
	if (sf19_present)
	{
#if LISTER
		vun = buffer[pos + ix];
#endif /* LISTER */

		++ ix;
	}
	if (sf20_present)
	{
#if CCWARN
		met[0] = buffer[pos + ix];
		met[1] = buffer[pos + ix + 1];
		met[2] = buffer[pos + ix + 2];
		met[3] = buffer[pos + ix + 3];
		met[4] = buffer[pos + ix + 4];
		met[5] = buffer[pos + ix + 5];
		met[6] = buffer[pos + ix + 6];
		met[7] = buffer[pos + ix + 7];
#endif /* CCWARN */

		ix += 8;
	}
	if (sf21_present)
	{
#if LISTER
		emc = buffer[pos + ix];
#endif /* LISTER */

		++ ix;
	}
	if (sf22_present)
	{
		df5 = buffer[pos + ix];
		df6 = buffer[pos + ix + 1];
		df7 = buffer[pos + ix + 2];
		df8 = buffer[pos + ix + 3];
		df9 = buffer[pos + ix + 4];
		df10 = buffer[pos + ix + 5];
		ix += 6;

#if LISTER
		if (df5 & 0x80)
		{
			pos_lat = make_si32 (0xff, df5, df6, df7);
		}
		else
		{
			pos_lat = make_si32 (0x00, df5, df6, df7);
		}
		if (df8 & 0x80)
		{
			pos_lon = make_si32 (0xff, df8, df9, df10);
		}
		else
		{
			pos_lon = make_si32 (0x00, df8, df9, df10);
		}
#endif /* LISTER */
	}
	if (sf23_present)
	{
		df5 = buffer[pos + ix];
		df6 = buffer[pos + ix + 1];
		ix += 2;

		gal = make_si16 (df5, df6);
	}
	if (sf24_present)
	{
		pun = buffer[pos + ix];
		++ ix;
	}
	if (sf25_present)
	{
		t_Byte buf[8];
		int k;

		Assert (mb_rep == buffer[pos + ix],
                "Invalid structure decoding");

		Assert (mb_rep <= M_MAX_BDS_REGISTERS, "Too many BDS registers");

		mb_cnt = 0;
		memset (mb_data, 0, M_MAX_BDS_REGISTERS * (1 + M_BDS_REGISTER_LENGTH));

		memset (buf, 0, 8);
		for (k = 0; k < mb_rep; k ++)
		{
			memcpy (buf, buffer + pos + ix + 1 + k * 8, 8);
			if (k < M_MAX_BDS_REGISTERS)
			{
				++ mb_cnt;
				memcpy (mb_data[k], buf, 8);
			}
		}

		ix += (1 + mb_rep * 8); 
	}
	if (sf26_present)
	{
		df5 = buffer[pos + ix];
		df6 = buffer[pos + ix + 1];
		ix += 2;

#if LISTER
		iar = make_ui16 (df5, df6);
#endif /* LISTER */
	}
	if (sf27_present)
	{
		df5 = buffer[pos + ix];
		df6 = buffer[pos + ix + 1];
		ix += 2;

#if LISTER
		mac = make_ui16 (df5, df6);
#endif /* LISTER */
	}
	if (sf28_present)
	{
		df5 = buffer[pos + ix];
		df6 = buffer[pos + ix + 1];
		ix += 2;

#if LISTER
		bps = make_ui16 (df5, df6);
#endif /* LISTER */
	}
	Assert (ix == len, "Invalid structure decoding");

#if LISTER
                   /* List data field: */
	list_text (2, ";  Aircraft Derived Data:");
	list_text (2, "\n");
	if (sf1_present)
	{
		list_text (2, ";   Target address:");
		list_text (2, " " M_FMT_ADDR, adr);
		list_text (2, "\n");
	}
	if (sf2_present)
	{
		list_text (2, ";   Target identification:");
		list_text (2, " " M_FMT_STRG, id);
		list_text (2, "\n");
	}
	if (sf3_present)
	{
		list_text (2, ";   Magnetic heading:");
		list_text (2, " " M_FMT_UI16 " (%.3f deg)",
                      mhg, (360.0 / 65536.0) * mhg);
		list_text (2, "\n");
	}
	if (sf4_present)
	{
		t_Ui16 im;
		t_Ui16 spd;

		im = (ias >> 15) & 0x0001;
		spd = ias & 0x7fff;

		list_text (2, ";   Indicated airspeed / Mach number:");
		list_text (2, " im=" M_FMT_UI16 ";", im);
		if (im == 1)
		{
			list_text (2, " air_speed(Mach)=" M_FMT_UI16 " (%.3f Mach)",
                          spd, 0.001 * spd);
		}
		else
		{
			list_text (2, " air_speed(IAS)=" M_FMT_UI16 " (%.3f kts)",
                          spd, (3600.0 / 16384.0) * spd);
		}
		list_text (2, "\n");
	}
	if (sf5_present)
	{
		list_text (2, ";   True airspeed:");
		list_text (2, " " M_FMT_UI16 " (kts)", tas);
		list_text (2, "\n");
	}
	if (sf6_present)
	{
		t_Si16 alt;

		alt = sal & 0x1fff;
		if (sal & 0x1000)
		{
			alt |= 0xe000;
		}

		list_text (2, ";   Selected altitude:");
		list_text (2, " sas=" M_FMT_UI16 ";", (sal >> 15) & 0x0001);
		list_text (2, " src=" M_FMT_UI16 ";", (sal >> 13) & 0x0003);
		list_text (2, " alt=" M_FMT_UI16 " (%.2f FL)", alt, 0.25 * alt);
		list_text (2, "\n");
	}
	if (sf7_present)
	{
		t_Si16 alt;

		alt = fss & 0x1fff;
		if (fss & 0x1000)
		{
			alt |= 0xe000;
		}

		list_text (2, ";   Final state selected altitude:");
		list_text (2, " mv=" M_FMT_UI16 ";", (fss >> 15) & 0x0001);
		list_text (2, " ah=" M_FMT_UI16 ";", (fss >> 14) & 0x0001);
		list_text (2, " am=" M_FMT_UI16 ";", (fss >> 13) & 0x0001);
		list_text (2, " alt=" M_FMT_SI16 "(%.2f FL)", alt, 0.25 * alt);
		list_text (2, "\n");
	}
	if (sf8_present)
	{
		list_text (2, ";   Trajectory intent status:");
		list_text (2, " nav=" M_FMT_BYTE ";", (tis >> 7) & 0x01);
		list_text (2, " nvb=" M_FMT_BYTE "", (tis >> 6) & 0x01);
		list_text (2, "\n");
	}
	if (sf9_present)
	{
		list_text (2, ";   Trajectory intent data:");
		/* tba ... */
		list_text (2, "\n");
	}
	if (sf10_present)
	{
		t_Ui16 stat;

		stat = (com >> 10) & 0x0007;

		list_text (2, ";   Communications/ACAS capability and "
                      "flight status:");
		list_text (2, "\n");
		list_text (2, ";    com=" M_FMT_UI16 ";", (com >> 13) & 0x0007);
		list_text (2, " stat=" M_FMT_UI16 "", stat);
		switch (stat)
		{
		case 0:
			list_text (2, " (no alert, no SPI, aircraft airborne)");
			break;
		case 1:
			list_text (2, " (no alert, no SPI, aircraft on ground)");
			break;
		case 2:
			list_text (2, " (alert, no SPI, aircraft airborne)");
			break;
		case 3:
			list_text (2, " (alert, no SPI, aircraft on ground)");
			break;
		case 4:
			list_text (2, " (alert, SPI)");
			break;
		case 5:
			list_text (2, " (no alert, SPI)");
			break;
		default:
			break;
		}
		list_text (2, ";");
		list_text (2, " ssc=" M_FMT_UI16 ";", (com >> 7) & 0x0001);
		list_text (2, " arc=" M_FMT_UI16 ";", (com >> 6) & 0x0001);
		list_text (2, " aic=" M_FMT_UI16 ";", (com >> 5) & 0x0001);
		list_text (2, " b1a=" M_FMT_UI16 ";", (com >> 4) & 0x0001);
		list_text (2, " b1b=" M_FMT_UI16 "", com & 0x000f);
		list_text (2, "\n");
	}
	if (sf11_present)
	{
		list_text (2, ";   Status reported by ADS-B:");
		list_text (2, " ac=" M_FMT_UI16 ";", (sab >> 14) & 0x0003);
		list_text (2, " mn=" M_FMT_UI16 ";", (sab >> 12) & 0x0003);
		list_text (2, " dc=" M_FMT_UI16 ";", (sab >> 10) & 0x0003);
		list_text (2, " gbs=" M_FMT_UI16 ";", (sab >> 9) & 0x0001);
		list_text (2, " stat=" M_FMT_UI16 "", sab & 0x000f);
		list_text (2, "\n");
	}
	if (sf12_present)
	{
		list_text (2, ";   ACAS resolution advisory report:");
		list_text (2, " 0x " M_FMT_BYTE_HEX " " M_FMT_BYTE_HEX " " M_FMT_BYTE_HEX
                      " " M_FMT_BYTE_HEX "  " M_FMT_BYTE_HEX " " M_FMT_BYTE_HEX
                      " " M_FMT_BYTE_HEX,
                      acs[0], acs[1], acs[2], acs[3],
                      acs[4], acs[5], acs[6]);
		list_text (2, "\n");
	}
	if (sf13_present)
	{
		list_text (2, ";   Barometric vertical rate:");
		list_text (2, " " M_FMT_SI16 " (%.2f ft/min)", bvr, 6.25 * bvr);
		list_text (2, "\n");
	}
	if (sf14_present)
	{
		list_text (2, ";   Geometric vertical rate:");
		list_text (2, " " M_FMT_SI16 " (%.2f ft/min)", gvr, 6.25 * gvr);
		list_text (2, "\n");
	}
	if (sf15_present)
	{
		list_text (2, ";   Roll angle:");
		list_text (2, " " M_FMT_SI16 " (%.3f deg)", ran, 0.01 * ran);
		list_text (2, "\n");
	}
	if (sf16_present)
	{
		t_Si16 rate_of_turn;

		rate_of_turn = (t_Si16) ((tar >> 1) & 0x007f);
		if (tar & 0x0080)
		{
			rate_of_turn |= 0xff80;
		}

		list_text (2, ";   Track angle rate:");
		list_text (2, " ti=" M_FMT_UI16 ";", (tar >> 14) & 0x0003);
		list_text (2, " rate_of_turn=" M_FMT_SI16 " (%.2f deg/sec)",
                      rate_of_turn, 0.25 * rate_of_turn);
		list_text (2, "\n");
	}
	if (sf17_present)
	{
		list_text (2, ";   Track angle:");
		list_text (2, " " M_FMT_UI16 " (%.3f deg)",
                      tan, (360.0 / 65536.0) * tan);
		list_text (2, "\n");
	}
	if (sf18_present)
	{
		list_text (2, ";   Ground speed:");
		list_text (2, " " M_FMT_SI16 " (%.2f kts)",
                      gsp, (3600.0 / 16384.0) * gsp);
		list_text (2, "\n");
	}
	if (sf19_present)
	{
		list_text (2, ";   Velocity uncertainty:");
		list_text (2, " " M_FMT_BYTE, vun);
		list_text (2, "\n");
	}
	if (sf20_present)
	{
		list_text (2, ";   Met data:");
		/* tba ... */
		list_text (2, "\n");
	}
	if (sf21_present)
	{
		list_text (2, ";   Emitter category:");
		list_text (2, " " M_FMT_BYTE, emc);
		switch (emc)
		{
		case 1:
			list_text (2, " (light aircraft <= 7000 kg)");
			break;
		case 2:
			list_text (2, " (reserved)");
			break;
		case 3:
			list_text (2, " (7000 kg <= medium aircraft < 136000 kg)");
			break;
		case 4:
			list_text (2, " (reserved)");
			break;
		case 5:
			list_text (2, " (136000 kg <= heavy aircraft)");
			break;
		case 6:
			list_text (2, " (highly manoeuverable and high speed)");
			break;
		case 7:
		case 8:
		case 9:
			list_text (2, " (reserved)");
			break;
		case 10:
			list_text (2, " (rotocraft)");
			break;
		case 11:
			list_text (2, " (glider / sailplane)");
			break;
		case 12:
			list_text (2, " (lighter-than-air)");
			break;
		case 13:
			list_text (2, " (unmanned aerial vehicle)");
			break;
		case 14:
			list_text (2, " (space / transatmospheric vehicle)");
			break;
		case 15:
			list_text (2, " (ultralight / handglider / paraglider)");
			break;
		case 16:
			list_text (2, " (parachutist / skydiver)");
			break;
		case 17:
		case 18:
		case 19:
			list_text (2, " (reserved)");
			break;
		case 20:
			list_text (2, " (surface emergency vehicle)");
			break;
		case 21:
			list_text (2, " (surface service vehicle)");
			break;
		case 22:
			list_text (2, " (fixed ground or tethered obstruction)");
			break;
		case 23:
		case 24:
			list_text (2, " (reserved)");
			break;
		default:
			break;
		}
		list_text (2, "\n");
	}
	if (sf22_present)
	{
		list_text (2, ";   Position:\n");
		list_text (2, ";    lat=" M_FMT_UI32 " (%s)",
                      pos_lat, lat_text ((180.0 / M_TWO_POWER_23) * pos_lat));
		list_text (2, "; lon=" M_FMT_UI32 " (%s) ",
                      pos_lon, lon_text ((180.0 / M_TWO_POWER_23) * pos_lon));
		list_text (2, "\n");
	}
	if (sf23_present)
	{
		list_text (2, ";   Geometric altitude:");
		list_text (2, " " M_FMT_SI16 " (%.2f FL)", gal, 0.0625 * gal);
		list_text (2, "\n");
	}
	if (sf24_present)
	{
		list_text (2, ";   Position uncertainty:");
		list_text (2, " " M_FMT_BYTE, pun & 0x0f);
		list_text (2, "\n");
	}
	if (sf25_present)
	{
		int k;

		list_text (2, ";   Mode S MB data:");
		list_text (2, "\n");

		for (k = 0; k < mb_cnt; k ++)
		{
			t_Byte bdsreg;
			int kx;

			list_text (2, ";    BDS ");
			bdsreg = mb_data[k][0];
			list_text (2, M_FMT_BYTE "," M_FMT_BYTE ": 0x",
                          (bdsreg >> 4) & 0x0f, bdsreg & 0xf);
			for (kx = 0; kx < M_BDS_REGISTER_LENGTH; kx ++)
			{
				list_text (2, " " M_FMT_BYTE_HEX, mb_data[k][1 + kx]);
			}
			list_text (2, "\n");
		}
	}
	if (sf26_present)
	{
		list_text (2, ";   Indicated air speed:");
		list_text (2, " " M_FMT_UI16 " (kts)", iar);
		list_text (2, "\n");
	}
	if (sf27_present)
	{
		list_text (2, ";   Mach number:");
		list_text (2, " " M_FMT_UI16 " (%.3f Mach)", mac, 0.008 * mac);
		list_text (2, "\n");
	}
	if (sf28_present)
	{
		list_text (2, ";   Barometric pressure setting:");
		list_text (2, " " M_FMT_UI16 "", bps & 0x0fff);
		list_text (2, "\n");
	}
#endif /* LISTER */

                   /* Store this information: */
	if (sf1_present)
	{
		strk.aircraft_address.present = TRUE;
		strk.aircraft_address.value = adr;
	}
	if (sf2_present)
	{
		strk.aircraft_identification.present = TRUE;
		strncpy (strk.aircraft_identification.value_idt, id,
                 M_AIRCRAFT_IDENTIFICATION_LENGTH);
	}
	if (sf3_present)
	{
		strk.magnetic_heading.present = TRUE;
		strk.magnetic_heading.value = (M_TWO_PI / 65536.0) * mhg;
	}
	if (sf4_present)
	{
		t_Ui16 im;
		t_Ui16 spd;

		im = (ias >> 15) & 0x0001;
		spd = ias & 0x7fff;

		if (im == 0)
		{
			strk.indicated_airspeed.present = TRUE;
			strk.indicated_airspeed.value = (M_NMI2MTR / 16384.0) * spd;
		}
		else if (im == 1)
		{
			strk.mach_number.present = TRUE;
			strk.mach_number.value = 0.001 * spd;
		}
	}
	if (sf5_present)
	{
		strk.true_airspeed.present = TRUE;
		strk.true_airspeed.value = M_KTS2MPS * tas;
	}
	if (sf6_present)
	{
		t_Si16 alt;

		alt = sal & 0x1fff;
		if (sal & 0x1000)
		{
			alt |= 0xe000;
		}

		strk.selected_altitude.present = TRUE;
		strk.selected_altitude.value = 25 * (t_Si32) alt;
                   /* In feet */
		strk.selected_altitude.value_sas = (t_Byte) ((sal >> 15) & 0x0001);
		strk.selected_altitude.value_src = (t_Byte) ((sal >> 13) & 0x0003);
	}
	if (sf7_present)
	{
		t_Si16 alt;

		alt = fss & 0x1fff;
		if (fss & 0x1000)
		{
			alt |= 0xe000;
		}

		strk.final_state_selected_altitude.present = TRUE;
		strk.final_state_selected_altitude.value = 25 * M_FT2MTR * alt;
                   /* In metres */
		strk.final_state_selected_altitude.value_ah =
			(t_Byte) ((fss >> 14) & 0x0001);
		strk.final_state_selected_altitude.value_am =
			(t_Byte) ((fss >> 13) & 0x0001);
		strk.final_state_selected_altitude.value_in_feet = 25 * (t_Si32) alt;
                   /* In feet */
		strk.final_state_selected_altitude.value_mv =
			(t_Byte) ((fss >> 15) & 0x0001);
	}
	/* tba ... sf8_present */
	/* tba ... sf9_present */
	if (sf10_present)
	{
		strk.communications_capability.present = TRUE;
		strk.communications_capability.value_com =
			(t_Byte) ((com >> 13) & 0x0007);

		strk.flight_status.present = TRUE;
		strk.flight_status.value_aic = (t_Byte) ((com >> 5) & 0x0001);
		strk.flight_status.value_arc = (t_Byte) ((com >> 6) & 0x0001);
		strk.flight_status.value_b1a = (t_Byte) ((com >> 4) & 0x0001);
		strk.flight_status.value_b1b = (t_Byte) (com & 0x000f);
		strk.flight_status.value_mssc = (t_Byte) ((com >> 7) & 0x0001);
		strk.flight_status.value_status = (t_Byte) ((com >> 10) & 0x0007);
	}
	/* tba ... sf11_present */
	if (sf12_present)
	{
		strk.acas_resolution_advisory_report.present = TRUE;
		memcpy (strk.acas_resolution_advisory_report.value, acs,
                M_ACAS_RESOLUTION_ADVISORY_LENGTH);
	}
	if (sf13_present)
	{
		strk.barometric_vertical_rate.present = TRUE;
		strk.barometric_vertical_rate.value = (6.25 * M_FPM2MPS) * bvr;
	}
	if (sf14_present)
	{
		strk.geometric_vertical_rate.present = TRUE;
		strk.geometric_vertical_rate.value = (6.25 * M_FPM2MPS) * gvr;
	}
	/* tba ... sf15_present */
	/* tba ... sf16_present */
	/* tba ... sf17_present */
	/* tba ... sf18_present */
	/* tba ... sf19_present */
	/* tba ... sf20_present */
	/* tba ... sf21_present */
	/* tba ... sf22_present */
	if (sf23_present)
	{
		strk.geometric_altitude.present = TRUE;
		strk.geometric_altitude.value = (6.25 * M_FT2MTR) * gal;
	}
	if (sf24_present)
	{
		strk.position_uncertainty_present = TRUE;
		strk.position_uncertainty = pun & 0x0f;
	}
	if (sf25_present)
	{
		int k;

		strk.mode_s_mb_data.count = mb_cnt;
		strk.mode_s_mb_data.present = TRUE;
		for (k = 0; k < mb_cnt; k ++)
		{
			strk.mode_s_mb_data.value[k].number = mb_data[k][0];
			strk.mode_s_mb_data.value[k].present = TRUE;
			memcpy (&(strk.mode_s_mb_data.value[k].value),
                    &(mb_data[k][1]),
                    M_BDS_REGISTER_LENGTH);
		}
	}
	/* tba ... sf26_present */
	/* tba ... sf27_present */
	/* tba ... sf28_present */

                   /* Set position: */
	*pos_ptr = pos + len;

                   /* Set the return code: */
	ret = RC_OKAY;

	done:          /* We are done */
	return ret;
}

/*----------------------------------------------------------------------------*/
/* proc_i062_390   -- Process I062/390 data item                              */
/*----------------------------------------------------------------------------*/

 static
 t_Retc proc_i062_390 (t_Ui16 length, t_Byte *buffer, t_Ui16 *pos_ptr)
{
	char ast[7];   /* Aircraft stand */
#if LISTER
	t_Si16 cfl;    /* Current cleared flight level; 1/4 FL */
#endif /* LISTER */
	char csn[8];   /* Callsign */
#if LISTER
	t_Ui16 ctl;    /* Current control position */
#endif /* LISTER */
	char dep[5];   /* Departure airport */
	t_Byte df1;    /* Data field octet 1 */
	t_Byte df2;    /* Data field octet 2 */
	t_Byte df3;    /* Data field octet 3 */
	t_Byte df4;    /* Data field octet 4 */
	t_Byte df5;    /* Data field octet 5 */
	t_Byte df6;    /* Data field octet 6 */
	t_Byte df7;    /* Data field octet 7 */
	char dst[5];   /* Destination airport */
#if LISTER
	t_Byte fct;    /* Flight category */
	t_Byte ifi[4]; /* IFPS flight ID */
	t_Ui32 ifi_nbr;
                   /* Plan number */
	t_Byte ifi_typ;
                   /* Type of plan number */
#endif /* LISTER */
	t_Ui16 ix;     /* Auxiliary */
	int j;         /* Auxiliary */
	t_Byte len;    /* Length of data field */
	char pec[8];   /* Pre-emergency callsign */
#if LISTER
	t_Ui16 pem;    /* Pre-emergency mode 3/A */
#endif /* LISTER */
	t_Ui16 pos;    /* Position within buffer */
	char rds[4];   /* Runway designation */
	t_Retc ret;    /* Return code */
	t_Byte sf1_present;
                   /* Subfield 1 present (TAG) */
	t_Byte sf2_present;
                   /* Subfield 2 present (CSN) */
	t_Byte sf3_present;
                   /* Subfield 3 present (IFI) */
	t_Byte sf4_present;
                   /* Subfield 4 present (FCT) */
	t_Byte sf5_present;
                   /* Subfield 5 present (TAC) */
	t_Byte sf6_present;
                   /* Subfield 6 present (WTC) */
	t_Byte sf7_present;
                   /* Subfield 7 present (DEP) */
	t_Byte sf8_present;
                   /* Subfield 8 present (DST) */
	t_Byte sf9_present;
                   /* Subfield 9 present (RDS) */
	t_Byte sf10_present;
                   /* Subfield 10 present (CFL) */
	t_Byte sf11_present;
                   /* Subfield 11 present (CTL) */
	t_Byte sf12_present;
                   /* Subfield 12 present (TOD) */
	t_Byte sf13_present;
                   /* Subfield 13 present (AST) */
	t_Byte sf14_present;
                   /* Subfield 14 present (STS) */
	t_Byte sf15_present;
                   /* Subfield 15 present (STD) */
	t_Byte sf16_present;
                   /* Subfield 16 present (STA) */
	t_Byte sf17_present;
                   /* Subfield 17 present (PEM) */
	t_Byte sf18_present;
                   /* Subfield 18 present (PEC) */
	char sta[8];   /* Standard instrument arrival */
	char std[8];   /* Standard instrument departure */
#if LISTER
	t_Byte sts;    /* Stand status */
#endif /* LISTER */
	char tac[5];   /* Type of aircraft */
#if LISTER
	t_Ui16 tag;    /* FPPS identification tag */
#endif /* LISTER */
	t_Ui32 tod[32];
                   /* Buffer for "Time of Departure/Arrival" */
#if LISTER
	t_Byte tod_avs;
                   /* Seconds available in "Time of Departure/Arrival" */
	t_Byte tod_day;
                   /* Day indication in "Time of Departure/Arrival" */
	t_Byte tod_hor;
                   /* Hours in "Time of Departure/Arrival" */
	t_Byte tod_min;
                   /* Minutes in "Time of Departure/Arrival" */
#endif /* LISTER */
	t_Byte tod_rep;
                   /* Repetition factor for "Time of Departure/Arrival" */
#if LISTER
	t_Byte tod_sec;
                   /* Seconds in "Time of Departure/Arrival" */
	t_Byte tod_typ;
                   /* Type of "Time of Departure/Arrival" */
	t_Byte wtc;    /* Wake turbulence category */
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

                   /* Extract octet: */
	df1 = buffer[pos];

                   /* Evaluate presence of subfields: */
	sf1_present = (df1 >> 7) & 0x01;
	sf2_present = (df1 >> 6) & 0x01;
	sf3_present = (df1 >> 5) & 0x01;
	sf4_present = (df1 >> 4) & 0x01;
	sf5_present = (df1 >> 3) & 0x01;
	sf6_present = (df1 >> 2) & 0x01;
	sf7_present = (df1 >> 1) & 0x01;

                   /* Preset other data fields: */
	df2 = 0x00;
	df3 = 0x00;

                   /* Preset presence of other subfields: */
	sf8_present = FALSE;
	sf9_present = FALSE;
	sf10_present = FALSE;
	sf11_present = FALSE;
	sf12_present = FALSE;
	sf13_present = FALSE;
	sf14_present = FALSE;
	sf15_present = FALSE;
	sf16_present = FALSE;
	sf17_present = FALSE;
	sf18_present = FALSE;

                   /* Check for second octet: */
	if (df1 & 0x01)
	{
		if (pos + 1 >= length)
		{
			error_msg ("Invalid buffer length (I062/390)");
			goto done;
		}

		df2 = buffer[pos + 1];

		sf8_present = (df2 >> 7) & 0x01;
		sf9_present = (df2 >> 6) & 0x01;
		sf10_present = (df2 >> 5) & 0x01;
		sf11_present = (df2 >> 4) & 0x01;
		sf12_present = (df2 >> 3) & 0x01;
		sf13_present = (df2 >> 2) & 0x01;
		sf14_present = (df2 >> 1) & 0x01;
	}

                   /* Check for third octet: */
	if (df2 & 0x01)
	{
		if (pos + 2 >= length)
		{
			error_msg ("Invalid buffer length (I062/390)");
			goto done;
		}

		df3 = buffer[pos + 2];

		sf15_present = (df3 >> 7) & 0x01;
		sf16_present = (df3 >> 6) & 0x01;
		sf17_present = (df3 >> 5) & 0x01;
		sf18_present = (df3 >> 4) & 0x01;
	}

                   /* Third octet must not have the FX bit set: */
	Assert ((df3 & 0x01) == 0, "Unexpected FX bit");

                   /* Determine expected length of data field: */
	len = 1 +
          ord (df1 & 0x01) +
          ord (df2 & 0x01) +
          2 * ord (sf1_present) +
          7 * ord (sf2_present) +
          4 * ord (sf3_present) +
          ord (sf4_present) +
          4 * ord (sf5_present) +
          ord (sf6_present) +
          4 * ord (sf7_present) +
          4 * ord (sf8_present) +
          3 * ord (sf9_present) +
          2 * ord (sf10_present) +
          2 * ord (sf11_present);
	if (sf12_present)
	{
		if (pos + len > length)
		{
			error_msg ("Invalid buffer length (I062/390)");
			goto done;
		}

		tod_rep = buffer[pos + len];

		len += (1 + tod_rep * 4);
	}
	len = len +
          6 * ord (sf13_present) +
          ord (sf14_present) +
          7 * ord (sf15_present) +
          7 * ord (sf16_present) +
          2 * ord (sf17_present) +
          7 * ord (sf18_present);

                   /* Check against buffer length: */
	if (pos + len > length)
	{
		error_msg ("Invalid buffer length (I062/390)");
		goto done;
	}

#if LISTER
                   /* List raw data field: */
	list_text (1, ";  I062/390: 0x");
	j = 0;
	for (ix = 0; ix < len; ix ++)
	{
		list_text (1, " " M_FMT_BYTE_HEX, buffer[pos + ix]);
		++ j;

		if (j >= 16)
		{
			list_text (1, "\n");
			if (1 + ix < len)
			{
				list_text (1, ";          + 0x");
			}

			j = 0;
		}
		else if ((j % 4) == 0)
		{
			list_text (1, " ");
		}
	}
	if (j > 0)
	{
		list_text (1, "\n");
	}
#endif /* LISTER */

                   /* Extract secondary subfields: */
	ix = 1 +
         ord (df1 & 0x01) +
         ord (df2 & 0x01) +
         ord (df3 & 0x01);

	if (sf1_present)
	{
		df4 = buffer[pos + ix];
		df5 = buffer[pos + ix + 1];
		ix += 2;

#if LISTER
		tag = make_ui16 (df4, df5);
#endif /* LISTER */
	}
	if (sf2_present)
	{
		memset (csn, 0, 8);

		csn[0] = (char) buffer[pos + ix];
		csn[1] = (char) buffer[pos + ix + 1];
		csn[2] = (char) buffer[pos + ix + 2];
		csn[3] = (char) buffer[pos + ix + 3];
		csn[4] = (char) buffer[pos + ix + 4];
		csn[5] = (char) buffer[pos + ix + 5];
		csn[6] = (char) buffer[pos + ix + 6];
		ix += 7;
	}
	if (sf3_present)
	{
#if LISTER
		ifi[0] = buffer[pos + ix];
		ifi[1] = buffer[pos + ix + 1];
		ifi[2] = buffer[pos + ix + 2];
		ifi[3] = buffer[pos + ix + 3];
#endif /* LISTER */

		ix += 4;

#if LISTER
		ifi_typ = (ifi[0] >> 6) & 0x03;
		ifi_nbr = make_ui32 (ifi[0] & 0x07, ifi[1], ifi[2], ifi[3]);
#endif /* LISTER */
	}
	if (sf4_present)
	{
#if LISTER
		fct = buffer[pos + ix];
#endif /* LISTER */

		++ ix;
	}
	if (sf5_present)
	{
		memset (tac, 0, 5);

		tac[0] = buffer[pos + ix];
		tac[1] = buffer[pos + ix + 1];
		tac[2] = buffer[pos + ix + 2];
		tac[3] = buffer[pos + ix + 3];
		ix += 4;
	}
	if (sf6_present)
	{
#if LISTER
		wtc = buffer[pos + ix];
#endif /* LISTER */

		++ ix;
	}
	if (sf7_present)
	{
		memset (dep, 0, 5);

		dep[0] = (char) buffer[pos + ix];
		dep[1] = (char) buffer[pos + ix + 1];
		dep[2] = (char) buffer[pos + ix + 2];
		dep[3] = (char) buffer[pos + ix + 3];
		ix += 4;
	}
	if (sf8_present)
	{
		memset (dst, 0, 5);

		dst[0] = (char) buffer[pos + ix];
		dst[1] = (char) buffer[pos + ix + 1];
		dst[2] = (char) buffer[pos + ix + 2];
		dst[3] = (char) buffer[pos + ix + 3];
		ix += 4;
	}
	if (sf9_present)
	{
		memset (rds, 0, 4);

		rds[0] = buffer[pos + ix];
		rds[1] = buffer[pos + ix + 1];
		rds[2] = buffer[pos + ix + 2];
		ix += 3;
	}
	if (sf10_present)
	{
		df4 = buffer[pos + ix];
		df5 = buffer[pos + ix + 1];
		ix += 2;

#if LISTER
		cfl = make_si16 (df4, df5);
#endif /* LISTER */
	}
	if (sf11_present)
	{
		df4 = buffer[pos + ix];
		df5 = buffer[pos + ix + 1];
		ix += 2;

#if LISTER
		ctl = make_ui16 (df4, df5);
#endif /* LISTER */
	}
	if (sf12_present)
	{
		tod_rep = buffer[pos + ix];

		memset (tod, 0, 32 * sizeof (t_Ui32));

		for (j = 0; j < tod_rep; j ++)
		{
			df4 = buffer[pos + ix + 1 + 4 * j];
			df5 = buffer[pos + ix + 1 + 4 * j + 1];
			df6 = buffer[pos + ix + 1 + 4 * j + 2];
			df7 = buffer[pos + ix + 1 + 4 * j + 3];

			if (j < 32)
			{
				tod[j] = make_ui32 (df4, df5, df6, df7);
			}
		}

		ix += (1 + tod_rep * 4);
	}
	if (sf13_present)
	{
		memset (ast, 0, 7);

		ast[0] = buffer[pos + ix];
		ast[1] = buffer[pos + ix + 1];
		ast[2] = buffer[pos + ix + 2];
		ast[3] = buffer[pos + ix + 3];
		ast[4] = buffer[pos + ix + 4];
		ast[5] = buffer[pos + ix + 5];
		ix += 6;
	}
	if (sf14_present)
	{
#if LISTER
		sts = buffer[pos + ix];
#endif /* LISTER */

		++ ix;
	}
	if (sf15_present)
	{
		memset (std, 0, 8);

		std[0] = buffer[pos + ix];
		std[1] = buffer[pos + ix + 1];
		std[2] = buffer[pos + ix + 2];
		std[3] = buffer[pos + ix + 3];
		std[4] = buffer[pos + ix + 4];
		std[5] = buffer[pos + ix + 5];
		std[6] = buffer[pos + ix + 6];
		ix += 7;
	}
	if (sf16_present)
	{
		memset (sta, 0, 8);

		sta[0] = buffer[pos + ix];
		sta[1] = buffer[pos + ix + 1];
		sta[2] = buffer[pos + ix + 2];
		sta[3] = buffer[pos + ix + 3];
		sta[4] = buffer[pos + ix + 4];
		sta[5] = buffer[pos + ix + 5];
		sta[6] = buffer[pos + ix + 6];
		ix += 7;
	}
	if (sf17_present)
	{
		df4 = buffer[pos + ix];
		df5 = buffer[pos + ix + 1];
		ix += 2;

#if LISTER
		pem = make_ui16 (df4, df5);
#endif /* LISTER */
	}
	if (sf18_present)
	{
		memset (pec, 0, 8);

		pec[0] = (char) buffer[pos + ix];
		pec[1] = (char) buffer[pos + ix + 1];
		pec[2] = (char) buffer[pos + ix + 2];
		pec[3] = (char) buffer[pos + ix + 3];
		pec[4] = (char) buffer[pos + ix + 4];
		pec[5] = (char) buffer[pos + ix + 5];
		pec[6] = (char) buffer[pos + ix + 6];
		ix += 7;
	}
	Assert (ix == len, "Invalid structure decoding");

#if LISTER
                   /* List data field: */
	list_text (2, ";  Flight Plan Related Data:");
	list_text (2, "\n");
	if (sf1_present)
	{
		list_text (2, ";   FPPS identification tag:");
		list_text (2, " 0x" M_FMT_UI16_HEX, tag);
		list_text (2, " (SAC=" M_FMT_BYTE "; SIC=" M_FMT_BYTE ")",
                      hi_byte (tag), lo_byte (tag));
		list_text (2, "\n");
	}
	if (sf2_present)
	{
		list_text (2, ";   Callsign:");
		list_text (2, " [%s]", csn);
		list_text (2, "\n");
	}
	if (sf3_present)
	{
		list_text (2, ";   IFPS flight ID:");
		list_text (2, " typ=" M_FMT_BYTE, ifi_typ);
		list_text (2, "; nbr=" M_FMT_UI32, ifi_nbr);
		list_text (2, "\n");
	}
	if (sf4_present)
	{
		list_text (2, ";   Flight category:");
		list_text (2, " GAT/OAT=" M_FMT_BYTE, (fct >> 6) & 0x03);
		list_text (2, "; FR=" M_FMT_BYTE, (fct >> 4) & 0x03);
		list_text (2, "; RVSM=" M_FMT_BYTE, (fct >> 2) & 0x03);
		list_text (2, "; HPR=" M_FMT_BYTE, (fct >> 1) & 0x01);
		list_text (2, "\n");
	}
	if (sf5_present)
	{
		list_text (2, ";   Type of aircraft:");
		list_text (2, " [%s]", tac);
		list_text (2, "\n");
	}
	if (sf6_present)
	{
		list_text (2, ";   Wake turbulence category:");
		list_text (2, " %c", wtc);
		list_text (2, "\n");
	}
	if (sf7_present)
	{
		list_text (2, ";   Departure airport:");
		list_text (2, " [%s]", dep);
		list_text (2, "\n");
	}
	if (sf8_present)
	{
		list_text (2, ";   Destination airport:");
		list_text (2, " [%s]", dst);
		list_text (2, "\n");
	}
	if (sf9_present)
	{
		list_text (2, ";   Runway designation:");
		list_text (2, " [%s]", rds);
		list_text (2, "\n");
	}
	if (sf10_present)
	{
		list_text (2, ";   Current cleared flight level:");
		list_text (2, " " M_FMT_SI16 " (1/4 FL)", cfl);
		list_text (2, "\n");
	}
	if (sf11_present)
	{
		list_text (2, ";   Current control position:");
		list_text (2, " centre=" M_FMT_BYTE, hi_byte (ctl));
		list_text (2, "; position=" M_FMT_BYTE, lo_byte (ctl));
		list_text (2, "\n");
	}
	if (sf12_present)
	{
		list_text (2, ";   Time of Departure/Arrival:");
		list_text (2, "\n");

		if (tod_rep > 32)
		{
			tod_rep = 32;
		}

		for (j = 0; j < tod_rep; j ++)
		{
			t_Byte b;

			b = (t_Byte) ((tod[j] >> 24) & 0x000000ff);

			tod_typ = (b >> 3) & 0x1f;
			tod_day = (b >> 1) & 0x03;

			b = (t_Byte) ((tod[j] >> 16) & 0x000000ff);

			tod_hor = b & 0x1f;

			b = (t_Byte) ((tod[j] >> 8) & 0x000000ff);

			tod_min = b & 0x3f;

			b = (t_Byte) (tod[j] & 0x000000ff);

			tod_avs = (b >> 7) & 0x01;
			tod_sec = b & 0x3f;

			list_text (2, ";    typ=" M_FMT_BYTE, tod_typ);
			list_text (2, ": day=" M_FMT_BYTE, tod_day);
			list_text (2, "; tm=%02u:%02u", tod_hor, tod_min);
			if (tod_avs)
			{
				list_text (2, ":%02u", tod_sec);
			}
			list_text (2, "\n");
		}
	}
	if (sf13_present)
	{
		list_text (2, ";   Aircraft stand:");
		list_text (2, " [%s]", ast);
		list_text (2, "\n");
	}
	if (sf14_present)
	{
		list_text (2, ";   Stand status:");
		list_text (2, " emp=" M_FMT_BYTE, (sts >> 6) & 0x03);
		list_text (2, "; avl=" M_FMT_BYTE, (sts >> 4) & 0x03);
		list_text (2, "\n");
	}
	if (sf15_present)
	{
		list_text (2, ";   Standard instrument departure:");
		list_text (2, " [%s]", std);
		list_text (2, "\n");
	}
	if (sf16_present)
	{
		list_text (2, ";   Standard instrument arrival:");
		list_text (2, " [%s]", sta);
		list_text (2, "\n");
	}
	if (sf17_present)
	{
		list_text (2, ";   Pre-emergency mode 3/A:");
		list_text (2, " va=" M_FMT_UI16 "; code=" M_FMT_CODE,
                      (pem >> 12) & 0x0001, pem & 0x0fff);
		list_text (2, "\n");
	}
	if (sf18_present)
	{
		list_text (2, ";   Pre-emergency callsign:");
		list_text (2, " [%s]", pec);
		list_text (2, "\n");
	}
#endif /* LISTER */

                   /* Store this information: */
	/* tba ... */
	if (sf2_present)
	{
		strk.callsign.present = TRUE;
		strncpy (strk.callsign.value, csn, 7);
	}
	/* tba ... */

                   /* Set position: */
	*pos_ptr = pos + len;

                   /* Set the return code: */
	ret = RC_OKAY;

	done:          /* We are done */
	return ret;
}

/*----------------------------------------------------------------------------*/
/* proc_i062_500   -- Process I062/500 data item                              */
/*----------------------------------------------------------------------------*/

 static
 t_Retc proc_i062_500 (t_Ui16 length, t_Byte *buffer, t_Ui16 *pos_ptr)
{
#if LISTER
	t_Byte aa_x;   /* Estimated accuracy of acceleration (Cartesian)
                      - x component; 0.25 metres/second**2 */
	t_Byte aa_y;   /* Estimated accuracy of acceleration (Cartesian)
                      - y component; 0.25 metres/second**2 */
#endif /* LISTER */
	t_Byte aba;    /* Estimated accuracy of calculated track
                      barometric altitude; 25 feet */
	t_Byte aga;    /* Estimated accuracy of calculated track
                      geometric altitude; 6.25 feet */
#if LISTER
	t_Ui16 apc_x;  /* Estimated accuracy of track position
                      (Cartesian) - x component; 0.5 metres */
	t_Ui16 apc_y;  /* Estimated accuracy of track position
                      (Cartesian) - y component; 0.5 metres */
	t_Ui16 apw_lat;
                   /* Estimated accuracy of track position
                      (WGS-84) - latitude component;
                      180/2**25 degrees */
	t_Ui16 apw_lon;
                   /* Estimated accuracy of track position
                      (WGS-84) - longitude component;
                      180/2**25 degrees */
#endif /* LISTER */
	t_Byte arc;    /* Estimated accuracy of rate of climb/descent;
                      6.25 feet/minute */
#if LISTER
	t_Ui16 atv_x;  /* Estimated accuracy of track velocity
                      (Cartesian) - x component; 0.25 metres/second */
	t_Ui16 atv_y;  /* Estimated accuracy of track velocity
                      (Cartesian) - y component; 0.25 metres/second */
	t_Si16 cov;    /* XY covariance component; 0.5 metres */
#endif /* LISTER */
	t_Byte df1;    /* Data field octet 1 */
	t_Byte df2;    /* Data field octet 2 */
#if LISTER
	t_Byte df3;    /* Data field octet 3 */
	t_Byte df4;    /* Data field octet 4 */
	t_Byte df5;    /* Data field octet 5 */
	t_Byte df6;    /* Data field octet 6 */
#endif /* LISTER */
	int ix;        /* Auxiliary */
#if LISTER
	int j;         /* Auxiliary */
#endif /* LISTER */
	t_Ui16 len;    /* Length of data field */
	t_Ui16 pos;    /* Position within buffer */
	t_Retc ret;    /* Return code */
	t_Byte sf1_present;
                   /* Subfield 1 present (APC) */
	t_Byte sf2_present;
                   /* Subfield 2 present (COV) */
	t_Byte sf3_present;
                   /* Subfield 3 present (APW) */
	t_Byte sf4_present;
                   /* Subfield 4 present (AGA) */
	t_Byte sf5_present;
                   /* Subfield 5 present (ABA) */
	t_Byte sf6_present;
                   /* Subfield 6 present (ATV) */
	t_Byte sf7_present;
                   /* Subfield 7 present (AA) */
	t_Byte sf8_present;
                   /* Subfield 8 present (ARC) */

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
	df1 = buffer[pos];

                   /* Evaluate presence of subfields: */
	sf1_present = (df1 >> 7) & 0x01;
	sf2_present = (df1 >> 6) & 0x01;
	sf3_present = (df1 >> 5) & 0x01;
	sf4_present = (df1 >> 4) & 0x01;
	sf5_present = (df1 >> 3) & 0x01;
	sf6_present = (df1 >> 2) & 0x01;
	sf7_present = (df1 >> 1) & 0x01;

                   /* Preset other data fields: */
	df2 = 0x00;
	sf8_present = FALSE;

                   /* Check for second octet: */
	if (df1 & 0x01)
	{
		if (pos + 1 >= length)
		{
			error_msg ("Invalid buffer length (I062/500)");
			goto done;
		}

		df2 = buffer[pos + 1];

		sf8_present = (df2 >> 7) & 0x01;
	}

                   /* Second octet must not have the FX bit set: */
	Assert ((df2 & 0x01) == 0, "Unexpected FX bit");

                   /* Determine expected length of data field: */
	len = 1 +
          ord (df1 & 0x01) +
          4 * ord (sf1_present) +
          2 * ord (sf2_present) +
          4 * ord (sf3_present) +
          ord (sf4_present) +
          ord (sf5_present) +
          2 * ord (sf6_present) +
          2 * ord (sf7_present) +
          ord (sf8_present);

                   /* Check against buffer length: */
	if (pos + len > length)
	{
		error_msg ("Invalid buffer length (I062/500).");
		goto done;
	}

#if LISTER
                   /* List raw data field: */
	list_text (1, ";  I062/500: 0x");
	j = 0;
	for (ix = 0; ix < len; ix ++)
	{
		list_text (1, " " M_FMT_BYTE_HEX, buffer[pos + ix]);
		++ j;

		if (j >= 16)
		{
			list_text (1, "\n");
			if (1 + ix < len)
			{
				list_text (1, ";          + 0x");
			}

			j = 0;
		}
		else if ((j % 4) == 0)
		{
			list_text (1, " ");
		}
	}
	if (j > 0)
	{
		list_text (1, "\n");
	}
#endif /* LISTER */

                   /* Extract secondary subfields: */
	ix = 1 +
         ord (df1 & 0x01);
	if (sf1_present)
	{
#if LISTER
		df3 = buffer[pos + ix];
		df4 = buffer[pos + ix + 1];
		df5 = buffer[pos + ix + 2];
		df6 = buffer[pos + ix + 3];
#endif /* LISTER */
		ix += 4;

#if LISTER
		apc_x = make_ui16 (df3, df4);
		apc_y = make_ui16 (df5, df6);
#endif /* LISTER */
	}
	if (sf2_present)
	{
#if LISTER
		df3 = buffer[pos + ix];
		df4 = buffer[pos + ix + 1];
#endif /* LISTER */
		ix += 2;

#if LISTER
		cov = make_si16 (df3, df4);
#endif /* LISTER */
	}
	if (sf3_present)
	{
#if LISTER
		df3 = buffer[pos + ix];
		df4 = buffer[pos + ix + 1];
		df5 = buffer[pos + ix + 2];
		df6 = buffer[pos + ix + 3];
#endif /* LISTER */
		ix += 4;

#if LISTER
		apw_lat = make_ui16 (df3, df4);
		apw_lon = make_ui16 (df5, df6);
#endif /* LISTER */
	}
	if (sf4_present)
	{
		aga = buffer[pos + ix];
		++ ix;
	}
	if (sf5_present)
	{
		aba = buffer[pos + ix];
		++ ix;
	}
	if (sf6_present)
	{
#if LISTER
		df3 = buffer[pos + ix];
		df4 = buffer[pos + ix + 1];
#endif /* LISTER */
		ix += 2;

#if LISTER
		atv_x = df3;
		atv_y = df4;
#endif /* LISTER */
	}
	if (sf7_present)
	{
#if LISTER
		df3 = buffer[pos + ix];
		df4 = buffer[pos + ix + 1];
#endif /* LISTER */
		ix += 2;

#if LISTER
		aa_x = df3;
		aa_y = df4;
#endif /* LISTER */
	}
	if (sf8_present)
	{
		arc = buffer[pos + ix];
		++ ix;
	}
	Assert (ix == len, "Invalid structure decoding");

#if LISTER
                   /* List data field: */
	list_text (2, ";  Estimated Accuracies:");
	list_text (2, "\n");
	if (sf1_present)
	{
		list_text (2, ";   Estimated accuracy of track position"
                      " (Cartesian):");
		list_text (2, " x=" M_FMT_UI16 " (%.1f mtr)", apc_x, 0.5 * apc_x);
		list_text (2, "; y=" M_FMT_UI16 " (%.1f mtr)", apc_y, 0.5 * apc_y);
		list_text (2, "\n");
	}
	if (sf2_present)
	{
		list_text (2, ";   XY covariance component:");
		list_text (2, " " M_FMT_SI16 " (%.1f mtr)", cov, 0.5 * cov);
		list_text (2, "\n");
	}
	if (sf3_present)
	{
		list_text (2, ";   Estimated accuracy of track position"
                      " (WGS-84):");
		list_text (2, " lat=" M_FMT_UI16 " (%.7f deg)",
                      apw_lat, (180.0 / M_TWO_POWER_25) * apw_lat);
		list_text (2, "; lon=" M_FMT_UI16 " (%.7f deg)",
                      apw_lon, (180.0 / M_TWO_POWER_25) * apw_lon);
		list_text (2, "\n");
	}
	if (sf4_present)
	{
		list_text (2, ";   Estimated accuracy of calculated track"
                      " geometric altitude:");
		list_text (2, " " M_FMT_BYTE " (%.2f FL)", aga, 0.0625 * aga);
		list_text (2, "\n");
	}
	if (sf5_present)
	{
		list_text (2, ";   Estimated accuracy of calculated track"
                      " barometric altitude:");
		list_text (2, " " M_FMT_BYTE " (%.2f FL)", aba, 0.25 * aba);
		list_text (2, "\n");
	}
	if (sf6_present)
	{
		list_text (2, ";   Estimated accuracy of track velocity"
                      " (Cartesian):");
		list_text (2, " x=" M_FMT_UI16 " (%.2f m/s)", atv_x, 0.25 * atv_x);
		list_text (2, "; y=" M_FMT_UI16 " (%.2f m/s)", atv_y, 0.25 * atv_y);
		list_text (2, "\n");
	}
	if (sf7_present)
	{
		list_text (2, ";   Estimated accuracy of acceleration"
                      " (Cartesian):");
		list_text (2, " x=" M_FMT_UI16 " (%.2f m/s**2)", aa_x, 0.25 * aa_x);
		list_text (2, "; y=" M_FMT_UI16 " (%.2f m/s**2)", aa_y, 0.25 * aa_y);
		list_text (2, "\n");
	}
	if (sf8_present)
	{
		list_text (2, ";   Estimated accuracy of rate of climb"
                      "/descent:");
		list_text (2, " " M_FMT_BYTE " (%.2f ft/min)", arc, 6.25 * arc);
		list_text (2, "\n");
	}
#endif /* LISTER */

                   /* Store this information: */
	/* tba ... */
	if (sf4_present)
	{
		strk.calculated_track_altitude_accuracy = (6.25 * 0.3048) * aga;
		strk.calculated_track_altitude_accuracy_present = TRUE;
	}
	if (sf5_present)
	{
		strk.calculated_track_flight_level_accuracy = aba;
		strk.calculated_track_flight_level_accuracy_present = TRUE;
	}
	/* tba ... */
	if (sf8_present)
	{
		strk.calculated_rate_of_climb_descent_accuracy = 6.25 * arc;
		strk.calculated_rate_of_climb_descent_accuracy_present = TRUE;
	}

                   /* Set position: */
	*pos_ptr = pos + len;

                   /* Set the return code: */
	ret = RC_OKAY;

	done:          /* We are done */
	return ret;
}

/*----------------------------------------------------------------------------*/
/* proc_i062_510   -- Process I062/510 data item                              */
/*----------------------------------------------------------------------------*/

 static
 t_Retc proc_i062_510 (t_Ui16 length, t_Byte *buffer, t_Ui16 *pos_ptr)
{
#if LISTER
	t_Byte df1;    /* Data field octet 1 */
	t_Byte df2;    /* Data field octet 2 */
#endif /* LISTER */
	t_Byte df3;    /* Data field octet 3 */
	int ix;        /* Auxiliary */
	t_Ui16 len;    /* Length of data field */
	t_Ui16 pos;    /* Position within buffer */
	t_Retc ret;    /* Return code */
#if LISTER
	t_Ui16 stn;    /* System track number */
	t_Byte sui;    /* System unit identification */
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

                   /* Check against buffer length: */
	if (pos + 2 >= length)
	{
		error_msg ("Invalid buffer length (I062/510)");
		goto done;
	}

                   /* Extract the first three octets: */
#if LISTER
	df1 = buffer[pos];
	df2 = buffer[pos + 1];
#endif /* LISTER */
	df3 = buffer[pos + 2];

                   /* Determine expected length of data field: */
	ix = 3;
	len = 3;
	while (df3 & 0x01)
	{
                   /* Check against buffer length: */
		if (pos + ix + 2 >= length)
		{
			error_msg ("Invalid buffer length (I062/510)");
			goto done;
		}

                   /* Extract next three octets: */
#if LISTER
		df1 = buffer[pos + ix];
		df2 = buffer[pos + ix + 1];
#endif /* LISTER */
		df3 = buffer[pos + ix + 2];

                   /* Increment index and expected length: */
		ix += 3;
		len += 3;
	}

                   /* Check against buffer length: */
	if (pos + len > length)
	{
		error_msg ("Invalid buffer length (I062/510)");
		goto done;
	}

#if LISTER
                   /* List raw data field: */
	list_buffer (1, ";  I062/510:", len, buffer + pos);

                   /* List data field: */
	list_text (2, ";  Composed Track Number:");
	list_text (2, "\n");
	ix = 0;
	while (ix < len)
	{
		df1 = buffer[pos + ix];
		df2 = buffer[pos + ix + 1];
		df3 = buffer[pos + ix + 2];

		sui = df1;
		stn = make_ui16 (df2, df3);

		stn >>= 1;

		if (ix == 0)
		{
			list_text (2, ";   Master track number:");
		}
		else
		{
			list_text (2, ";   Slave track number:");
		}
		list_text (2, " sui=" M_FMT_BYTE ";", sui);
		list_text (2, " stn=" M_FMT_UI16, stn);
		list_text (2, "\n");

		ix += 3;
	}
#endif /* LISTER */

                   /* Store this information: */
	/* tba ... */

                   /* Adapt position: */
	*pos_ptr = pos + len;

                   /* Set the return code: */
	ret = RC_OKAY;

	done:          /* We are done */
	return ret;
}

/*----------------------------------------------------------------------------*/
/* proc_i062_ref   -- Process RE indicator data item                          */
/*----------------------------------------------------------------------------*/

 static
 t_Retc proc_i062_ref (t_Ui16 length, t_Byte *buffer, t_Ui16 *pos_ptr)
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
		error_msg ("Invalid buffer length (I062/REF)");
		goto done;
	}

#if LISTER
				   /* List raw data field: */
	list_buffer (1, ";  I062/REF:", len, buffer + pos);
#endif /* LISTER */

				   /* Set position: */
	*pos_ptr = pos + len;

                   /* Set the return code: */
	ret = RC_OKAY;

	done:          /* We are done */
	return ret;
}

/*----------------------------------------------------------------------------*/
/* proc_i062_spf   -- Process SP indicator data item                          */
/*----------------------------------------------------------------------------*/

 static
 t_Retc proc_i062_spf (t_Ui16 length, t_Byte *buffer, t_Ui16 *pos_ptr)
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
		error_msg ("Invalid buffer length (I062/SPF)");
		goto done;
	}

#if LISTER
				   /* List raw data field: */
	list_buffer (1, ";  I062/SPF:", len, buffer + pos);
#endif /* LISTER */

				   /* Set position: */
	*pos_ptr = pos + len;

				   /* Set the return code: */
	ret = RC_OKAY;

	done:          /* We are done */
	return ret;
}
/* end-of-file */
