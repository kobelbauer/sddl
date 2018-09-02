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
/* File:           src/astx_030.cpp                                           */
/* Contents:       Process ASTERIX category 030 data block                    */
/* Author(s):      kb                                                         */
/* Last change:    2018-04-27                                                 */
/*----------------------------------------------------------------------------*/

/* Reference document:
   -------------------

   Interface Specification: Application of ASTERIX to ARTAS.
   DED.3/SUR/ARTAS.ASTX.015
   Version: 2.8.1
   Edition Date: 26 Feb 1999
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

#define M_MAX_FRN 56
				   /* Maximum field reference number */
#define M_MAX_FSPEC_LENGTH 8
				   /* Maximum fields specification length
					  for ASTERIX category 030 */

				   /* Local variables: */
				   /* ---------------- */

static t_Data_Item_Desc desc_i030_010;
                   /* Description of data item I030/010 */
static t_Data_Item_Desc desc_i030_015;
                   /* Description of data item I030/015 */
static t_Data_Item_Desc desc_i030_020;
                   /* Description of data item I030/020 */
static t_Data_Item_Desc desc_i030_030;
                   /* Description of data item I030/030 */
static t_Data_Item_Desc desc_i030_035;
                   /* Description of data item I030/035 */
static t_Data_Item_Desc desc_i030_040;
                   /* Description of data item I030/040 */
static t_Data_Item_Desc desc_i030_050;
                   /* Description of data item I030/050 */
static t_Data_Item_Desc desc_i030_060;
                   /* Description of data item I030/060 */
static t_Data_Item_Desc desc_i030_070;
                   /* Description of data item I030/070 */
static t_Data_Item_Desc desc_i030_080;
                   /* Description of data item I030/080 */
static t_Data_Item_Desc desc_i030_090;
                   /* Description of data item I030/090 */
static t_Data_Item_Desc desc_i030_100;
                   /* Description of data item I030/100 */
static t_Data_Item_Desc desc_i030_110;
                   /* Description of data item I030/110 */
static t_Data_Item_Desc desc_i030_120;
                   /* Description of data item I030/120 */
static t_Data_Item_Desc desc_i030_130;
                   /* Description of data item I030/130 */
static t_Data_Item_Desc desc_i030_135;
                   /* Description of data item I030/135 */
static t_Data_Item_Desc desc_i030_140;
                   /* Description of data item I030/140 */
static t_Data_Item_Desc desc_i030_150;
                   /* Description of data item I030/150 */
static t_Data_Item_Desc desc_i030_160;
                   /* Description of data item I030/160 */
static t_Data_Item_Desc desc_i030_165;
                   /* Description of data item I030/165 */
static t_Data_Item_Desc desc_i030_170;
                   /* Description of data item I030/170 */
static t_Data_Item_Desc desc_i030_180;
                   /* Description of data item I030/180 */
static t_Data_Item_Desc desc_i030_181;
                   /* Description of data item I030/181 */
static t_Data_Item_Desc desc_i030_190;
                   /* Description of data item I030/190 */
static t_Data_Item_Desc desc_i030_191;
                   /* Description of data item I030/191 */
static t_Data_Item_Desc desc_i030_200;
                   /* Description of data item I030/200 */
static t_Data_Item_Desc desc_i030_210;
                   /* Description of data item I030/210 */
static t_Data_Item_Desc desc_i030_220;
                   /* Description of data item I030/220 */
static t_Data_Item_Desc desc_i030_230;
                   /* Description of data item I030/230 */
static t_Data_Item_Desc desc_i030_240;
                   /* Description of data item I030/240 */
static t_Data_Item_Desc desc_i030_250;
                   /* Description of data item I030/250 */
static t_Data_Item_Desc desc_i030_260;
                   /* Description of data item I030/260 */
static t_Data_Item_Desc desc_i030_270;
                   /* Description of data item I030/270 */
static t_Data_Item_Desc desc_i030_290;
                   /* Description of data item I030/290 */
static t_Data_Item_Desc desc_i030_340;
                   /* Description of data item I030/340 */
static t_Data_Item_Desc desc_i030_360;
                   /* Description of data item I030/360 */
static t_Data_Item_Desc desc_i030_370;
                   /* Description of data item I030/370 */
static t_Data_Item_Desc desc_i030_382;
                   /* Description of data item I030/382 */
static t_Data_Item_Desc desc_i030_384;
                   /* Description of data item I030/384 */
static t_Data_Item_Desc desc_i030_386;
                   /* Description of data item I030/386 */
static t_Data_Item_Desc desc_i030_390;
                   /* Description of data item I030/390 */
static t_Data_Item_Desc desc_i030_400;
                   /* Description of data item I030/400 */
static t_Data_Item_Desc desc_i030_410;
                   /* Description of data item I030/410 */
static t_Data_Item_Desc desc_i030_420;
                   /* Description of data item I030/420 */
static t_Data_Item_Desc desc_i030_430;
                   /* Description of data item I030/430 */
static t_Data_Item_Desc desc_i030_435;
                   /* Description of data item I030/435 */
static t_Data_Item_Desc desc_i030_440;
                   /* Description of data item I030/440 */
static t_Data_Item_Desc desc_i030_450;
                   /* Description of data item I030/450 */
static t_Data_Item_Desc desc_i030_460;
                   /* Description of data item I030/460 */
static t_Data_Item_Desc desc_i030_480;
                   /* Description of data item I030/480 */
static t_Data_Item_Desc desc_i030_490;
                   /* Description of data item I030/490 */
static t_Data_Item_Desc desc_i030_spf;
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
static t_Retc proc_i030_010 (t_Ui16 length, t_Byte *buffer);
                   /* Process I030/010 data item */
static t_Retc proc_i030_015 (t_Ui16 length, t_Byte *buffer);
                   /* Process I030/015 data item */
static t_Retc proc_i030_020 (t_Ui16 length, t_Byte *buffer);
                   /* Process I030/020 data item */
static t_Retc proc_i030_030 (t_Ui16 length, t_Byte *buffer);
                   /* Process I030/030 data item */
static t_Retc proc_i030_035 (t_Ui16 length, t_Byte *buffer);
                   /* Process I030/035 data item */
static t_Retc proc_i030_040 (t_Ui16 length, t_Byte *buffer);
                   /* Process I030/040 data item */
static t_Retc proc_i030_050 (t_Ui16 length, t_Byte *buffer, t_Ui16 *pos_ptr);
                   /* Process I030/050 data item */
static t_Retc proc_i030_060 (t_Ui16 length, t_Byte *buffer);
                   /* Process I030/060 data item */
static t_Retc proc_i030_070 (t_Ui16 length, t_Byte *buffer);
                   /* Process I030/070 data item */
static t_Retc proc_i030_080 (t_Ui16 length, t_Byte *buffer);
                   /* Process I030/080 data item */
static t_Retc proc_i030_090 (t_Ui16 length, t_Byte *buffer);
                   /* Process I030/090 data item */
static t_Retc proc_i030_100 (t_Ui16 length, t_Byte *buffer);
                   /* Process I030/100 data item */
static t_Retc proc_i030_110 (t_Ui16 length, t_Byte *buffer);
                   /* Process I030/110 data item */
static t_Retc proc_i030_120 (t_Ui16 length, t_Byte *buffer);
                   /* Process I030/120 data item */
static t_Retc proc_i030_130 (t_Ui16 length, t_Byte *buffer);
                   /* Process I030/130 data item */
static t_Retc proc_i030_135 (t_Ui16 length, t_Byte *buffer);
                   /* Process I030/135 data item */
static t_Retc proc_i030_140 (t_Ui16 length, t_Byte *buffer);
                   /* Process I030/140 data item */
static t_Retc proc_i030_150 (t_Ui16 length, t_Byte *buffer);
                   /* Process I030/150 data item */
static t_Retc proc_i030_160 (t_Ui16 length, t_Byte *buffer);
                   /* Process I030/160 data item */
static t_Retc proc_i030_165 (t_Ui16 length, t_Byte *buffer);
                   /* Process I030/165 data item */
static t_Retc proc_i030_170 (t_Ui16 length, t_Byte *buffer);
                   /* Process I030/170 data item */
static t_Retc proc_i030_180 (t_Ui16 length, t_Byte *buffer);
                   /* Process I030/180 data item */
static t_Retc proc_i030_181 (t_Ui16 length, t_Byte *buffer);
                   /* Process I030/181 data item */
static t_Retc proc_i030_190 (t_Ui16 length, t_Byte *buffer);
                   /* Process I030/190 data item */
static t_Retc proc_i030_191 (t_Ui16 length, t_Byte *buffer);
                   /* Process I030/191 data item */
static t_Retc proc_i030_200 (t_Ui16 length, t_Byte *buffer);
                   /* Process I030/200 data item */
static t_Retc proc_i030_210 (t_Ui16 length, t_Byte *buffer);
                   /* Process I030/210 data item */
static t_Retc proc_i030_220 (t_Ui16 length, t_Byte *buffer);
                   /* Process I030/220 data item */
static t_Retc proc_i030_230 (t_Ui16 length, t_Byte *buffer);
                   /* Process I030/230 data item */
static t_Retc proc_i030_240 (t_Ui16 length, t_Byte *buffer);
                   /* Process I030/240 data item */
static t_Retc proc_i030_250 (t_Ui16 length, t_Byte *buffer);
                   /* Process I030/250 data item */
static t_Retc proc_i030_260 (t_Ui16 length, t_Byte *buffer);
                   /* Process I030/260 data item */
static t_Retc proc_i030_270 (t_Ui16 length, t_Byte *buffer);
                   /* Process I030/270 data item */
static t_Retc proc_i030_290 (t_Ui16 length, t_Byte *buffer);
                   /* Process I030/290 data item */
static t_Retc proc_i030_340 (t_Ui16 length, t_Byte *buffer);
                   /* Process I030/340 data item */
static t_Retc proc_i030_360 (t_Ui16 length, t_Byte *buffer);
                   /* Process I030/360 data item */
static t_Retc proc_i030_370 (t_Ui16 length, t_Byte *buffer);
                   /* Process I030/370 data item */
static t_Retc proc_i030_382 (t_Ui16 length, t_Byte *buffer);
                   /* Process I030/382 data item */
static t_Retc proc_i030_384 (t_Ui16 length, t_Byte *buffer);
                   /* Process I030/384 data item */
static t_Retc proc_i030_386 (t_Ui16 length, t_Byte *buffer);
                   /* Process I030/386 data item */
static t_Retc proc_i030_390 (t_Ui16 length, t_Byte *buffer);
                   /* Process I030/390 data item */
static t_Retc proc_i030_400 (t_Ui16 length, t_Byte *buffer);
                   /* Process I030/400 data item */
static t_Retc proc_i030_410 (t_Ui16 length, t_Byte *buffer);
                   /* Process I030/410 data item */
static t_Retc proc_i030_420 (t_Ui16 length, t_Byte *buffer);
                   /* Process I030/420 data item */
static t_Retc proc_i030_430 (t_Ui16 length, t_Byte *buffer);
                   /* Process I030/430 data item */
static t_Retc proc_i030_435 (t_Ui16 length, t_Byte *buffer);
                   /* Process I030/435 data item */
static t_Retc proc_i030_440 (t_Ui16 length, t_Byte *buffer);
                   /* Process I030/440 data item */
static t_Retc proc_i030_450 (t_Ui16 length, t_Byte *buffer);
                   /* Process I030/450 data item */
static t_Retc proc_i030_460 (t_Ui16 length, t_Byte *buffer);
                   /* Process I030/460 data item */
static t_Retc proc_i030_480 (t_Ui16 length, t_Byte *buffer);
                   /* Process I030/480 data item */
static t_Retc proc_i030_490 (t_Ui16 length, t_Byte *buffer);
                   /* Process I030/490 data item */
static t_Retc proc_i030_spf (t_Ui16 length, t_Byte *buffer, t_Ui16 *pos_ptr);
                   /* Process SPF indicator data item */

/*----------------------------------------------------------------------------*/
/* astx_030        -- Process ASTERIX category 030 data block                 */
/*----------------------------------------------------------------------------*/

 t_Retc astx_030 (t_Ui16 length, t_Byte *buffer)
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

                   /* Preset system track information: */
		memset (&strk, 0, sizeof (strk));

                   /* Set data format: */
		strk.asterix_category = 30;
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

					lrc = data_item (30, frn, std_uap[frn],
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
			rsrv.frame_date.present = TRUE;
			rsrv.frame_date.value = frame_date;
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
                   /* Description of data item I030/010: */
	desc_i030_010.category = 30;
	desc_i030_010.data_item = 10;
	desc_i030_010.item_type = e_fixed_length_data_item;
	desc_i030_010.fixed_length = 2;
	desc_i030_010.proc_fptr = proc_i030_010;
	desc_i030_010.read_fptr = NULL;

                   /* Description of data item I030/015: */
	desc_i030_015.category = 30;
	desc_i030_015.data_item = 15;
	desc_i030_015.item_type = e_fixed_length_data_item;
	desc_i030_015.fixed_length = 2;
	desc_i030_015.proc_fptr = proc_i030_015;
	desc_i030_015.read_fptr = NULL;

                   /* Description of data item I030/020: */
	desc_i030_020.category = 30;
	desc_i030_020.data_item = 20;
	desc_i030_020.item_type = e_fixed_length_data_item;
	desc_i030_020.fixed_length = 3;
	desc_i030_020.proc_fptr = proc_i030_020;
	desc_i030_020.read_fptr = NULL;

                   /* Description of data item I030/030: */
	desc_i030_030.category = 30;
	desc_i030_030.data_item = 30;
	desc_i030_030.item_type = e_variable_length_data_item;
	desc_i030_030.fixed_length = 0;
	desc_i030_030.proc_fptr = proc_i030_030;
	desc_i030_030.read_fptr = NULL;

                   /* Description of data item I030/035: */
	desc_i030_035.category = 30;
	desc_i030_035.data_item = 35;
	desc_i030_035.item_type = e_fixed_length_data_item;
	desc_i030_035.fixed_length = 1;
	desc_i030_035.proc_fptr = proc_i030_035;
	desc_i030_035.read_fptr = NULL;

                   /* Description of data item I030/040: */
	desc_i030_040.category = 30;
	desc_i030_040.data_item = 40;
	desc_i030_040.item_type = e_fixed_length_data_item;
	desc_i030_040.fixed_length = 2;
	desc_i030_040.proc_fptr = proc_i030_040;
	desc_i030_040.read_fptr = NULL;

                   /* Description of data item I030/050: */
	desc_i030_050.category = 30;
	desc_i030_050.data_item = 50;
	desc_i030_050.item_type = e_immediate_data_item;
	desc_i030_050.fixed_length = 0;
	desc_i030_050.proc_fptr = NULL;
	desc_i030_050.read_fptr = proc_i030_050;

                   /* Description of data item I030/060: */
	desc_i030_060.category = 30;
	desc_i030_060.data_item = 60;
	desc_i030_060.item_type = e_fixed_length_data_item;
	desc_i030_060.fixed_length = 2;
	desc_i030_060.proc_fptr = proc_i030_060;
	desc_i030_060.read_fptr = NULL;

                   /* Description of data item I030/070: */
	desc_i030_070.category = 30;
	desc_i030_070.data_item = 70;
	desc_i030_070.item_type = e_fixed_length_data_item;
	desc_i030_070.fixed_length = 3;
	desc_i030_070.proc_fptr = proc_i030_070;
	desc_i030_070.read_fptr = NULL;

                   /* Description of data item I030/080: */
	desc_i030_080.category = 30;
	desc_i030_080.data_item = 80;
	desc_i030_080.item_type = e_variable_length_data_item;
	desc_i030_080.fixed_length = 0;
	desc_i030_080.proc_fptr = proc_i030_080;
	desc_i030_080.read_fptr = NULL;

                   /* Description of data item I030/090: */
	desc_i030_090.category = 30;
	desc_i030_090.data_item = 90;
	desc_i030_090.item_type = e_fixed_length_data_item;
	desc_i030_090.fixed_length = 1;
	desc_i030_090.proc_fptr = proc_i030_090;
	desc_i030_090.read_fptr = NULL;

                   /* Description of data item I030/100: */
	desc_i030_100.category = 30;
	desc_i030_100.data_item = 100;
	desc_i030_100.item_type = e_fixed_length_data_item;
	desc_i030_100.fixed_length = 4;
	desc_i030_100.proc_fptr = proc_i030_100;
	desc_i030_100.read_fptr = NULL;

                   /* Description of data item I030/110: */
	desc_i030_110.category = 30;
	desc_i030_110.data_item = 110;
	desc_i030_110.item_type = e_fixed_length_data_item;
	desc_i030_110.fixed_length = 4;
	desc_i030_110.proc_fptr = proc_i030_110;
	desc_i030_110.read_fptr = NULL;

                   /* Description of data item I030/120: */
	desc_i030_120.category = 30;
	desc_i030_120.data_item = 120;
	desc_i030_120.item_type = e_fixed_length_data_item;
	desc_i030_120.fixed_length = 2;
	desc_i030_120.proc_fptr = proc_i030_120;
	desc_i030_120.read_fptr = NULL;

                   /* Description of data item I030/130: */
	desc_i030_130.category = 30;
	desc_i030_130.data_item = 130;
	desc_i030_130.item_type = e_fixed_length_data_item;
	desc_i030_130.fixed_length = 2;
	desc_i030_130.proc_fptr = proc_i030_130;
	desc_i030_130.read_fptr = NULL;

                   /* Description of data item I030/135: */
	desc_i030_135.category = 30;
	desc_i030_135.data_item = 135;
	desc_i030_135.item_type = e_fixed_length_data_item;
	desc_i030_135.fixed_length = 2;
	desc_i030_135.proc_fptr = proc_i030_135;
	desc_i030_135.read_fptr = NULL;

                   /* Description of data item I030/140: */
	desc_i030_140.category = 30;
	desc_i030_140.data_item = 140;
	desc_i030_140.item_type = e_fixed_length_data_item;
	desc_i030_140.fixed_length = 2;
	desc_i030_140.proc_fptr = proc_i030_140;
	desc_i030_140.read_fptr = NULL;

                   /* Description of data item I030/150: */
	desc_i030_150.category = 30;
	desc_i030_150.data_item = 150;
	desc_i030_150.item_type = e_fixed_length_data_item;
	desc_i030_150.fixed_length = 2;
	desc_i030_150.proc_fptr = proc_i030_150;
	desc_i030_150.read_fptr = NULL;

                   /* Description of data item I030/160: */
	desc_i030_160.category = 30;
	desc_i030_160.data_item = 160;
	desc_i030_160.item_type = e_fixed_length_data_item;
	desc_i030_160.fixed_length = 2;
	desc_i030_160.proc_fptr = proc_i030_160;
	desc_i030_160.read_fptr = NULL;

                   /* Description of data item I030/165: */
	desc_i030_165.category = 30;
	desc_i030_165.data_item = 165;
	desc_i030_165.item_type = e_fixed_length_data_item;
	desc_i030_165.fixed_length = 2;
	desc_i030_165.proc_fptr = proc_i030_165;
	desc_i030_165.read_fptr = NULL;

                   /* Description of data item I030/170: */
	desc_i030_170.category = 30;
	desc_i030_170.data_item = 170;
	desc_i030_170.item_type = e_fixed_length_data_item;
	desc_i030_170.fixed_length = 4;
	desc_i030_170.proc_fptr = proc_i030_170;
	desc_i030_170.read_fptr = NULL;

                   /* Description of data item I030/180: */
	desc_i030_180.category = 30;
	desc_i030_180.data_item = 180;
	desc_i030_180.item_type = e_fixed_length_data_item;
	desc_i030_180.fixed_length = 4;
	desc_i030_180.proc_fptr = proc_i030_180;
	desc_i030_180.read_fptr = NULL;

                   /* Description of data item I030/181: */
	desc_i030_181.category = 30;
	desc_i030_181.data_item = 181;
	desc_i030_181.item_type = e_fixed_length_data_item;
	desc_i030_181.fixed_length = 4;
	desc_i030_181.proc_fptr = proc_i030_181;
	desc_i030_181.read_fptr = NULL;

                   /* Description of data item I030/190: */
	desc_i030_190.category = 30;
	desc_i030_190.data_item = 190;
	desc_i030_190.item_type = e_fixed_length_data_item;
	desc_i030_190.fixed_length = 4;
	desc_i030_190.proc_fptr = proc_i030_190;
	desc_i030_190.read_fptr = NULL;

                   /* Description of data item I030/191: */
	desc_i030_191.category = 30;
	desc_i030_191.data_item = 191;
	desc_i030_191.item_type = e_fixed_length_data_item;
	desc_i030_191.fixed_length = 4;
	desc_i030_191.proc_fptr = proc_i030_191;
	desc_i030_191.read_fptr = NULL;

                   /* Description of data item I030/200: */
	desc_i030_200.category = 30;
	desc_i030_200.data_item = 200;
	desc_i030_200.item_type = e_fixed_length_data_item;
	desc_i030_200.fixed_length = 1;
	desc_i030_200.proc_fptr = proc_i030_200;
	desc_i030_200.read_fptr = NULL;

                   /* Description of data item I030/210: */
	desc_i030_210.category = 30;
	desc_i030_210.data_item = 210;
	desc_i030_210.item_type = e_fixed_length_data_item;
	desc_i030_210.fixed_length = 3;
	desc_i030_210.proc_fptr = proc_i030_210;
	desc_i030_210.read_fptr = NULL;

                   /* Description of data item I030/220: */
	desc_i030_220.category = 30;
	desc_i030_220.data_item = 220;
	desc_i030_220.item_type = e_fixed_length_data_item;
	desc_i030_220.fixed_length = 2;
	desc_i030_220.proc_fptr = proc_i030_220;
	desc_i030_220.read_fptr = NULL;

                   /* Description of data item I030/230: */
	desc_i030_230.category = 30;
	desc_i030_230.data_item = 230;
	desc_i030_230.item_type = e_fixed_length_data_item;
	desc_i030_230.fixed_length = 2;
	desc_i030_230.proc_fptr = proc_i030_230;
	desc_i030_230.read_fptr = NULL;

                   /* Description of data item I030/240: */
	desc_i030_240.category = 30;
	desc_i030_240.data_item = 240;
	desc_i030_240.item_type = e_fixed_length_data_item;
	desc_i030_240.fixed_length = 1;
	desc_i030_240.proc_fptr = proc_i030_240;
	desc_i030_240.read_fptr = NULL;

                   /* Description of data item I030/250: */
	desc_i030_250.category = 30;
	desc_i030_250.data_item = 250;
	desc_i030_250.item_type = e_fixed_length_data_item;
	desc_i030_250.fixed_length = 1;
	desc_i030_250.proc_fptr = proc_i030_250;
	desc_i030_250.read_fptr = NULL;

                   /* Description of data item I030/260: */
	desc_i030_260.category = 30;
	desc_i030_260.data_item = 260;
	desc_i030_260.item_type = e_fixed_length_data_item;
	desc_i030_260.fixed_length = 2;
	desc_i030_260.proc_fptr = proc_i030_260;
	desc_i030_260.read_fptr = NULL;

                   /* Description of data item I030/270: */
	desc_i030_270.category = 30;
	desc_i030_270.data_item = 270;
	desc_i030_270.item_type = e_fixed_length_data_item;
	desc_i030_270.fixed_length = 2;
	desc_i030_270.proc_fptr = proc_i030_270;
	desc_i030_270.read_fptr = NULL;

                   /* Description of data item I030/290: */
	desc_i030_290.category = 30;
	desc_i030_290.data_item = 290;
	desc_i030_290.item_type = e_fixed_length_data_item;
	desc_i030_290.fixed_length = 2;
	desc_i030_290.proc_fptr = proc_i030_290;
	desc_i030_290.read_fptr = NULL;

                   /* Description of data item I030/340: */
	desc_i030_340.category = 30;
	desc_i030_340.data_item = 340;
	desc_i030_340.item_type = e_fixed_length_data_item;
	desc_i030_340.fixed_length = 2;
	desc_i030_340.proc_fptr = proc_i030_340;
	desc_i030_340.read_fptr = NULL;

                   /* Description of data item I030/360: */
	desc_i030_360.category = 30;
	desc_i030_360.data_item = 360;
	desc_i030_360.item_type = e_fixed_length_data_item;
	desc_i030_360.fixed_length = 4;
	desc_i030_360.proc_fptr = proc_i030_360;
	desc_i030_360.read_fptr = NULL;

                   /* Description of data item I030/370: */
	desc_i030_370.category = 30;
	desc_i030_370.data_item = 370;
	desc_i030_370.item_type = e_fixed_length_data_item;
	desc_i030_370.fixed_length = 2;
	desc_i030_370.proc_fptr = proc_i030_370;
	desc_i030_370.read_fptr = NULL;

                   /* Description of data item I030/382: */
	desc_i030_382.category = 30;
	desc_i030_382.data_item = 382;
	desc_i030_382.item_type = e_fixed_length_data_item;
	desc_i030_382.fixed_length = 3;
	desc_i030_382.proc_fptr = proc_i030_382;
	desc_i030_382.read_fptr = NULL;

                   /* Description of data item I030/384: */
	desc_i030_384.category = 30;
	desc_i030_384.data_item = 384;
	desc_i030_384.item_type = e_fixed_length_data_item;
	desc_i030_384.fixed_length = 6;
	desc_i030_384.proc_fptr = proc_i030_384;
	desc_i030_384.read_fptr = NULL;

                   /* Description of data item I030/386: */
	desc_i030_386.category = 30;
	desc_i030_386.data_item = 386;
	desc_i030_386.item_type = e_fixed_length_data_item;
	desc_i030_386.fixed_length = 1;
	desc_i030_386.proc_fptr = proc_i030_386;
	desc_i030_386.read_fptr = NULL;

                   /* Description of data item I030/390: */
	desc_i030_390.category = 30;
	desc_i030_390.data_item = 390;
	desc_i030_390.item_type = e_fixed_length_data_item;
	desc_i030_390.fixed_length = 2;
	desc_i030_390.proc_fptr = proc_i030_390;
	desc_i030_390.read_fptr = NULL;

                   /* Description of data item I030/400: */
	desc_i030_400.category = 30;
	desc_i030_400.data_item = 400;
	desc_i030_400.item_type = e_fixed_length_data_item;
	desc_i030_400.fixed_length = 7;
	desc_i030_400.proc_fptr = proc_i030_400;
	desc_i030_400.read_fptr = NULL;

                   /* Description of data item I030/410: */
	desc_i030_410.category = 30;
	desc_i030_410.data_item = 410;
	desc_i030_410.item_type = e_fixed_length_data_item;
	desc_i030_410.fixed_length = 2;
	desc_i030_410.proc_fptr = proc_i030_410;
	desc_i030_410.read_fptr = NULL;

                   /* Description of data item I030/420: */
	desc_i030_420.category = 30;
	desc_i030_420.data_item = 420;
	desc_i030_420.item_type = e_fixed_length_data_item;
	desc_i030_420.fixed_length = 1;
	desc_i030_420.proc_fptr = proc_i030_420;
	desc_i030_420.read_fptr = NULL;

                   /* Description of data item I030/430: */
	desc_i030_430.category = 30;
	desc_i030_430.data_item = 430;
	desc_i030_430.item_type = e_fixed_length_data_item;
	desc_i030_430.fixed_length = 4;
	desc_i030_430.proc_fptr = proc_i030_430;
	desc_i030_430.read_fptr = NULL;

                   /* Description of data item I030/435: */
	desc_i030_435.category = 30;
	desc_i030_435.data_item = 435;
	desc_i030_435.item_type = e_fixed_length_data_item;
	desc_i030_435.fixed_length = 1;
	desc_i030_435.proc_fptr = proc_i030_435;
	desc_i030_435.read_fptr = NULL;

                   /* Description of data item I030/440: */
	desc_i030_440.category = 30;
	desc_i030_440.data_item = 440;
	desc_i030_440.item_type = e_fixed_length_data_item;
	desc_i030_440.fixed_length = 4;
	desc_i030_440.proc_fptr = proc_i030_440;
	desc_i030_440.read_fptr = NULL;

                   /* Description of data item I030/450: */
	desc_i030_450.category = 30;
	desc_i030_450.data_item = 450;
	desc_i030_450.item_type = e_fixed_length_data_item;
	desc_i030_450.fixed_length = 4;
	desc_i030_450.proc_fptr = proc_i030_450;
	desc_i030_450.read_fptr = NULL;

                   /* Description of data item I030/460: */
	desc_i030_460.category = 30;
	desc_i030_460.data_item = 460;
	desc_i030_460.item_type = e_repetitive_data_item;
	desc_i030_460.fixed_length = 2;
	desc_i030_460.proc_fptr = proc_i030_460;
	desc_i030_460.read_fptr = NULL;

                   /* Description of data item I030/480: */
	desc_i030_480.category = 30;
	desc_i030_480.data_item = 480;
	desc_i030_480.item_type = e_fixed_length_data_item;
	desc_i030_480.fixed_length = 2;
	desc_i030_480.proc_fptr = proc_i030_480;
	desc_i030_480.read_fptr = NULL;

                   /* Description of data item I030/490: */
	desc_i030_490.category = 30;
	desc_i030_490.data_item = 490;
	desc_i030_490.item_type = e_fixed_length_data_item;
	desc_i030_490.fixed_length = 2;
	desc_i030_490.proc_fptr = proc_i030_490;
	desc_i030_490.read_fptr = NULL;

                   /* Description of SPF data item: */
	desc_i030_spf.category = 30;
	desc_i030_spf.data_item = M_SPF_INDICATOR;
	desc_i030_spf.item_type = e_immediate_data_item;
	desc_i030_spf.fixed_length = 0;
	desc_i030_spf.proc_fptr = NULL;
	desc_i030_spf.read_fptr = proc_i030_spf;

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

	std_uap[1] = &desc_i030_010;
	std_uap[2] = &desc_i030_015;
	std_uap[3] = &desc_i030_030;
	std_uap[4] = &desc_i030_035;
	std_uap[5] = &desc_i030_040;
	std_uap[6] = &desc_i030_070;
	std_uap[7] = &desc_i030_170;

	std_uap[8] = &desc_i030_100;
	std_uap[9] = &desc_i030_180;
	std_uap[10] = &desc_i030_181;
	std_uap[11] = &desc_i030_060;
	std_uap[12] = &desc_i030_150;
	std_uap[13] = &desc_i030_130;
	std_uap[14] = &desc_i030_160;

	std_uap[15] = &desc_i030_080;
	std_uap[16] = &desc_i030_090;
	std_uap[17] = &desc_i030_200;
	std_uap[18] = &desc_i030_220;
	std_uap[19] = &desc_i030_240;
	std_uap[20] = &desc_i030_290;
	std_uap[21] = &desc_i030_260;

	std_uap[22] = &desc_i030_360;
	std_uap[23] = &desc_i030_140;
	std_uap[24] = &desc_i030_340;
	std_uap[25] = &desc_i030_spf;
	std_uap[26] = &desc_i030_390;
	std_uap[27] = &desc_i030_400;
	std_uap[28] = &desc_i030_410;

	std_uap[29] = &desc_i030_440;
	std_uap[30] = &desc_i030_450;
	std_uap[31] = &desc_i030_435;
	std_uap[32] = &desc_i030_430;
	std_uap[33] = &desc_i030_460;
	std_uap[34] = &desc_i030_480;
	std_uap[35] = &desc_i030_420;

	std_uap[36] = &desc_i030_490;
	std_uap[37] = &desc_i030_020;
	std_uap[38] = &desc_i030_382;
	std_uap[39] = &desc_i030_384;
	std_uap[40] = &desc_i030_386;
	std_uap[41] = &desc_i030_110;
	std_uap[42] = &desc_i030_190;

	std_uap[43] = &desc_i030_191;
	std_uap[44] = &desc_i030_135;
	std_uap[45] = &desc_i030_165;
	std_uap[46] = &desc_i030_230;
	std_uap[47] = &desc_i030_250;
	std_uap[48] = &desc_i030_210;
	std_uap[49] = &desc_i030_120;

	std_uap[50] = &desc_i030_050;
	std_uap[51] = &desc_i030_270;
	std_uap[52] = &desc_i030_370;

	return;
}

/*----------------------------------------------------------------------------*/
/* proc_i030_010   -- Process I030/010 data item                              */
/*----------------------------------------------------------------------------*/

 static
 t_Retc proc_i030_010 (t_Ui16 length, t_Byte *buffer)
{
	t_Byte df1;    /* Data field octet 1 */
	t_Byte df2;    /* Data field octet 2 */
	t_Ui16 dsi;    /* Server identification tag */
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
	list_text (2, ";  Server Identification Tag:");
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
/* proc_i030_015   -- Process I030/015 data item                              */
/*----------------------------------------------------------------------------*/

 static
 t_Retc proc_i030_015 (t_Ui16 length, t_Byte *buffer)
{
	t_Byte df1;    /* Data field octet 1 */
	t_Byte df2;    /* Data field octet 2 */
	t_Retc ret;    /* Return code */
	t_Ui16 usr;    /* User number */

				   /* Preset the return code: */
	ret = RC_FAIL;

				   /* Check parameters: */
	Assert (length == 2, "Invalid parameter");
	Assert (buffer != NULL, "Invalid parameter");

				   /* Extract octets: */
	df1 = buffer[0];
	df2 = buffer[1];

				   /* Extract information: */
	usr = make_ui16 (df1, df2);

#if LISTER
				   /* List data field: */
	list_text (2, ";  User Number:");
	list_text (2, " " M_FMT_UI16, usr);
	list_text (2, "\n");
#endif /* LISTER */

				   /* Store this information: */
	strk.user_number_present = TRUE;
	strk.user_number = usr;

				   /* Set the return code: */
	ret = RC_OKAY;

	return ret;
}

/*----------------------------------------------------------------------------*/
/* proc_i030_020   -- Process I030/020 data item                              */
/*----------------------------------------------------------------------------*/

 static
 t_Retc proc_i030_020 (t_Ui16 length, t_Byte *buffer)
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
	list_text (2, ";  Time of Message:");
	list_text (2, " " M_FMT_ATOD " (" M_FMT_UI32 "; %s UTC)",
                  tod, tod, utc_text (tod_in_secs));
	list_text (2, "\n");
#endif /* LISTER */

				   /* Store this information: */
	strk.time_of_message.present = TRUE;
	strk.time_of_message.value = tod_in_secs;

				   /* Set the return code: */
	ret = RC_OKAY;

	return ret;
}

/*----------------------------------------------------------------------------*/
/* proc_i030_030   -- Process I030/030 data item                              */
/*----------------------------------------------------------------------------*/

 static
 t_Retc proc_i030_030 (t_Ui16 length, t_Byte *buffer)
{
	t_Byte df1;    /* Data field octet 1 */
	int ix;        /* Auxiliary */
	t_Retc ret;    /* Return code */
	t_Byte si;     /* Service identification */

				   /* Preset the return code: */
	ret = RC_FAIL;

				   /* Check parameters: */
	Assert (length > 0, "Invalid parameter");
	Assert (buffer != NULL, "Invalid parameter");

#if LISTER
				   /* List data field: */
	list_text (2, ";  Service Identification:");
	for (ix = 0; ix < length; ix ++)
	{
		df1 = buffer[ix];

		if (ix == 0)
		{
			if (df1 & 0x04)
			{
				list_text (2, " BS");
			}

			if (df1 & 0x02)
			{
				list_text (2, " C1");
			}
		}
		else if (ix == 1)
		{
			if (df1 & 0x80)
			{
				list_text (2, " C2");
			}

			if (df1 & 0x40)
			{
				list_text (2, " C3");
			}

			if (df1 & 0x20)
			{
				list_text (2, " C4");
			}

			if (df1 & 0x10)
			{
				list_text (2, " C5");
			}
		}
	}
	list_text (2, "\n");
#endif /* LISTER */

                   /* Extract the information: */
	si = 0x00;
	for (ix = 0; ix < length; ix ++)
	{
		df1 = buffer[ix];

		if (ix == 0)
		{
			si |= ((df1 & 0x06) << 3);
		}
		else if (ix == 1)
		{
			si |= ((df1 >> 4) & 0x0f);
		}
	}

				   /* Store this information: */
	strk.service_identification.present = TRUE;
	strk.service_identification.value = si;

				   /* Set the return code: */
	ret = RC_OKAY;

	return ret;
}

/*----------------------------------------------------------------------------*/
/* proc_i030_035   -- Process I030/035 data item                              */
/*----------------------------------------------------------------------------*/

 static
 t_Retc proc_i030_035 (t_Ui16 length, t_Byte *buffer)
{
#if LISTER
	t_Byte df1;    /* Data field octet 1 */
	t_Byte family; /* Family */
	t_Byte nature; /* Nature */
#endif /* LISTER */
	t_Retc ret;    /* Return code */

				   /* Preset the return code: */
	ret = RC_FAIL;

				   /* Check parameters: */
	Assert (length == 1, "Invalid parameter");
	Assert (buffer != NULL, "Invalid parameter");

#if LISTER
				   /* Extract octets: */
	df1 = buffer[0];

				   /* Extract information: */
	family = (df1 >> 4) & 0x0f;
	nature = df1 & 0x0f;

				   /* List data field: */
	list_text (2, ";  Type of Message:");
	list_text (2, " family=" M_FMT_BYTE ";", family);
	list_text (2, " nature=" M_FMT_BYTE, nature);
	list_text (2, "\n");
#endif /* LISTER */

				   /* Store this information: */
	/* tba ... */

				   /* Set the return code: */
	ret = RC_OKAY;

	return ret;
}

/*----------------------------------------------------------------------------*/
/* proc_i030_040   -- Process I030/040 data item                              */
/*----------------------------------------------------------------------------*/

 static
 t_Retc proc_i030_040 (t_Ui16 length, t_Byte *buffer)
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
	sttn = (df1 >> 4) & 0x01;
	stn = make_ui16 (df1 & 0x0f, df2);

#if LISTER
				   /* List data field: */
	list_text (2, ";  Track Number:");
	list_text (2, " sttn=" M_FMT_BYTE ";", sttn);
	list_text (2, " stn=" M_FMT_UI16, stn);
	list_text (2, "\n");
#endif /* LISTER */

				   /* Store this information: */
	strk.track_number.present = TRUE;
	strk.track_number.value = stn;
	strk.track_numbering_indicator_present = TRUE;
	strk.track_numbering_indicator = sttn;

				   /* Set the return code: */
	ret = RC_OKAY;

	return ret;
}

/*----------------------------------------------------------------------------*/
/* proc_i030_050   -- Process I030/050 data item                              */
/*----------------------------------------------------------------------------*/

 static
 t_Retc proc_i030_050 (t_Ui16 length, t_Byte *buffer, t_Ui16 *pos_ptr)
{
	t_Ui16 base_pos;
                   /* Base position within buffer */
#if LISTER
	t_Byte df1;    /* Data field octet 1 */
	t_Byte df2;    /* Data field octet 2 */
#endif /* LISTER */
	t_Byte df3;    /* Data field octet 3 */
#if LISTER
	int ix;        /* Auxiliary */
	int j;         /* Auxiliary */
#endif /* LISTER */
	int num;       /* Number of local track numbers */
	t_Ui16 pos;    /* Position within buffer */
	t_Retc ret;    /* Return code */
#if LISTER
	t_Ui16 stnr;   /* System track number */
	t_Ui16 unit;   /* ARTAS unit identification */
#endif /* LISTER */

				   /* Preset the return code: */
	ret = RC_FAIL;

				   /* Check parameters: */
	Assert (length > 0, "Invalid parameter");
	Assert (buffer != NULL, "Invalid parameter");
	Assert (pos_ptr != NULL, "Invalid parameter");

				   /* Set position within buffer: */
	base_pos = *pos_ptr;
	pos = base_pos;

				   /* Check position: */
	Assert (pos < length, "Invalid position");

				   /* Check against buffer length: */
	if (pos + 3 > length)
	{
		error_msg ("Invalid buffer length");
		goto done;
	}

				   /* Extract octets: */
#if LISTER
	df1 = buffer[pos];
	df2 = buffer[pos + 1];
#endif /* LISTER */
	df3 = buffer[pos + 2];

	pos += 3;

	num = 1;
	while (df3 & 0x01)
	{
#if LISTER
		df1 = buffer[pos];
		df2 = buffer[pos + 1];
#endif /* LISTER */
		df3 = buffer[pos + 2];

		pos += 3;

		++ num;
	}

#if LISTER
				   /* List raw data field: */
	list_text (1, ";  I030/050: 0x");
	j = 0;
	for (ix = 0; ix < num * 3; ix ++)
	{
		list_text (1, " " M_FMT_BYTE_HEX, buffer[base_pos + ix]);
		++ j;

		if (j >= 16)
		{
			list_text (1, "\n");
			list_text (1, ";          + 0x");

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

				   /* Extract information: */
	/* tba ... */

#if LISTER
				   /* List data field: */
	list_text (2, ";  ARTAS Track Number:");
	if (num == 1)
	{
		df1 = buffer[base_pos];
		df2 = buffer[base_pos + 1];
		df3 = buffer[base_pos + 2];

		unit = (t_Ui16) df1;
		unit <<= 3;
		unit |= (t_Ui16) ((df2 >> 5) & 0x07);

		stnr = (t_Ui16) (df2 & 0x1f);
		stnr <<= 7;
		stnr |= (t_Ui16) ((df3 >> 1) & 0x7f);

		list_text (2, " unit=" M_FMT_UI16 ";", unit);
		list_text (2, " stnr=" M_FMT_UI16, stnr);
		list_text (2, "\n");
	}
	else
	{
		list_text (2, "\n");

		for (ix = 0; ix < num; ix ++)
		{
			df1 = buffer[base_pos + 3 * ix];
			df2 = buffer[base_pos + 3 * ix + 1];
			df3 = buffer[base_pos + 3 * ix + 2];

			unit = (t_Ui16) df1;
			unit <<= 3;
			unit |= (t_Ui16) ((df2 >> 5) & 0x07);

			stnr = (t_Ui16) (df2 & 0x1f);
			stnr <<= 7;
			stnr |= (t_Ui16) ((df3 >> 1) & 0x7f);

			list_text (2, ";   ");
			if (num == 1)
			{
				list_text (2, "Master Track Number:");
			}
			else
			{
				list_text (2, "Slave Track Number:");
			}
			list_text (2, " unit=" M_FMT_UI16 ";", unit);
			list_text (2, " stnr=" M_FMT_UI16, stnr);
			list_text (2, "\n");
		}
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
/* proc_i030_060   -- Process I030/060 data item                              */
/*----------------------------------------------------------------------------*/

 static
 t_Retc proc_i030_060 (t_Ui16 length, t_Byte *buffer)
{
	t_Byte df1;    /* Data field octet 1 */
	t_Byte df2;    /* Data field octet 2 */
	t_Ui16 m3c;    /* Track mode 3/A code */
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
	list_text (2, ";  Track Mode 3/A:");
	list_text (2, " v=" M_FMT_BYTE ";", (df1 >> 7) & 0x01);
	list_text (2, " g=" M_FMT_BYTE ";", (df1 >> 6) & 0x01);
	list_text (2, " c=" M_FMT_BYTE ";", (df1 >> 5) & 0x01);
	list_text (2, " code=" M_FMT_CODE, m3c);
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
/* proc_i030_070   -- Process I030/070 data item                              */
/*----------------------------------------------------------------------------*/

 static
 t_Retc proc_i030_070 (t_Ui16 length, t_Byte *buffer)
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
	list_text (2, ";  Time of Last Update:");
	list_text (2, " " M_FMT_ATOD " (" M_FMT_UI32 "; %s UTC);",
                  tod, tod, utc_text (tod_in_secs));
	list_text (2, " tod=%12.6f", tod_in_secs);
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
/* proc_i030_080   -- Process I030/080 data item                              */
/*----------------------------------------------------------------------------*/

 static
 t_Retc proc_i030_080 (t_Ui16 length, t_Byte *buffer)
{
	t_Byte df1;    /* Data field octet 1 */
	int ix;        /* Auxiliary */
	t_Retc ret;    /* Return code */
	t_Byte type;   /* Type of track */

				   /* Preset the return code: */
	ret = RC_FAIL;

				   /* Check parameters: */
	Assert (length > 0, "Invalid parameter");
	Assert (buffer != NULL, "Invalid parameter");

#if LISTER
				   /* List data field: */
	list_text (2, ";  ARTAS Track Status:\n");
	for (ix = 0; ix < length; ix ++)
	{
		df1 = buffer[ix];
			
		if (ix == 0)
		{
			list_text (2, ";  ");

			if (df1 & 0x80)
			{
				list_text (2, " SIM");
			}
			else
			{
				list_text (2, " LIV");
			}

			if (df1 & 0x40)
			{
				list_text (2, " TNT");
			}
			else
			{
				list_text (2, " CNF");
			}

			if (df1 & 0x20)
			{
				list_text (2, " ADD");
			}

			if (df1 & 0x10)
			{
				list_text (2, " CST");
			}

			type = (df1 >> 1) & 0x07;
			list_text (2, " TYPE=%u (", type);
			switch (type)
			{
			case 0:
				list_text (2, "both PR and SSR multiradar track");
				break;
			case 1:
				list_text (2, "PR only multiradar track");
				break;
			case 2:
				list_text (2, "SSR only multiradar track");
				break;
			case 3:
				list_text (2, "not used");
				break;
			case 4:
				list_text (2, "combined monoradar track");
				break;
			case 5:
				list_text (2, "PR monoradar track");
				break;
			case 6:
				list_text (2, "SSR monoradar track");
				break;
			case 7:
				list_text (2, "not used");
				break;
			}
			list_text (2, ")");
			list_text (2, "\n");
		}
		else if (ix == 1)
		{
			t_Byte cor;
			t_Byte slr;

			list_text (2, ";  ");

			if (df1 & 0x80)
			{
				list_text (2, " TRM");
			}

			if (df1 & 0x40)
			{
				list_text (2, " CRE");
			}

			slr = (df1 >> 4) & 0x03;
			list_text (2, " SLR=" M_FMT_BYTE, slr);

			cor = (df1 >> 1) & 0x07;
			list_text (2, " COR=" M_FMT_BYTE, cor);
			list_text (2, "\n");
		}
		else if (ix == 2)
		{
			list_text (2, ";  ");

			if (df1 & 0x20)
			{
				list_text (2, " FOR");
			}

			if (df1 & 0x10)
			{
				list_text (2, " AMA");
			}

			if (df1 & 0x08)
			{
				list_text (2, " SPI");
			}

			if (df1 & 0x04)
			{
				list_text (2, " ME");
			}

			if (df1 & 0x02)
			{
				list_text (2, " TDC");
			}

			list_text (2, "\n");
		}
		else if (ix == 3)
		{
			list_text (2, ";  ");

			if (df1 & 0x80)
			{
				list_text (2, " SF=1 (1/32 nmi)");
			}
			else
			{
				list_text (2, " SF=0 (1/64 nmi)");
			}

			list_text (2, "\n");
		}
	}
#endif /* LISTER */

				   /* Store this information: */
	strk.adsb_blunder_detected = e_is_undefined;
	strk.amalgamated_track = e_is_undefined;
	strk.assigned_code_conflict = e_is_undefined;
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
		df1 = buffer[ix] & ~0x01;

		if (ix == 0)
		{
			strk.simulated = M_TRES (df1 & 0x80);

			strk.tentative_track = M_TRES (df1 & 0x40);

			strk.track_with_aircraft_derived_data = M_TRES (df1 & 0x20);

			strk.coasted_track = M_TRES (df1 & 0x01);

			type = (df1 >> 1) & 0x07;
			if (type == 0)
			{
				strk.multi_sensor_track = e_is_true;
				strk.observed_by_psr = e_is_true;
				strk.observed_by_ssr = e_is_true;
			}
			else if (type == 1)
			{
				strk.multi_sensor_track = e_is_true;
				strk.observed_by_psr = e_is_true;
				strk.observed_by_ssr = e_is_false;
			}
			else if (type == 2)
			{
				strk.multi_sensor_track = e_is_true;
				strk.observed_by_psr = e_is_false;
				strk.observed_by_ssr = e_is_true;
			}
			else if (type == 4)
			{
				strk.observed_by_psr = e_is_true;
				strk.observed_by_ssr = e_is_true;
			}
			else if (type == 5)
			{
				strk.observed_by_psr = e_is_true;
				strk.observed_by_ssr = e_is_false;
			}
			else if (type == 6)
			{
				strk.observed_by_psr = e_is_false;
				strk.observed_by_ssr = e_is_true;
			}
		}
		else if (ix == 1)
		{
			t_Byte slr;

			strk.track_terminated = M_TRES (df1 & 0x80);

			strk.track_created = M_TRES (df1 & 0x40);

			slr = (df1 >> 4) & 0x03;
			if (slr == 0)
			{
				strk.most_reliable_height = 0;
				strk.most_reliable_height_present = TRUE;
			}
			else if (slr == 1 || slr == 2)
			{
				strk.most_reliable_height = 1;
				strk.most_reliable_height_present = TRUE;
			}
		}
		else if (ix == 2)
		{
			strk.formation_flight = M_TRES (df1 & 0x20);

			strk.amalgamated_track = M_TRES (df1 & 0x10);

			strk.special_position_indication = M_TRES (df1 & 0x08);

			strk.military_emergency = M_TRES (df1 & 0x04);

			strk.transponder_delay_correction = M_TRES (df1 & 0x02);
		}
		else if (ix == 3)
		{
			strk.track_position_coding_precision = (df1 >> 7) & 0x01;
		}
	}

				   /* Set the return code: */
	ret = RC_OKAY;

	return ret;
}

/*----------------------------------------------------------------------------*/
/* proc_i030_090   -- Process I030/090 data item                              */
/*----------------------------------------------------------------------------*/

 static
 t_Retc proc_i030_090 (t_Ui16 length, t_Byte *buffer)
{
	t_Byte atq;    /* ARTAS track quality */
	t_Byte df1;    /* Data field octet 1 */
	t_Retc ret;    /* Return code */

				   /* Preset the return code: */
	ret = RC_FAIL;

				   /* Check parameters: */
	Assert (length == 1, "Invalid parameter");
	Assert (buffer != NULL, "Invalid parameter");

				   /* Extract octets: */
	df1 = buffer[0];

				   /* Extract information: */
	atq = df1 & 0x07;

#if LISTER
				   /* List data field: */
	list_text (2, ";  ARTAS Track Quality:");
	list_text (2, " " M_FMT_BYTE, atq);
	list_text (2, "\n");
#endif /* LISTER */

				   /* Store this information: */
	strk.track_quality_present = TRUE;
	strk.track_quality = atq;

				   /* Set the return code: */
	ret = RC_OKAY;

	return ret;
}

/*----------------------------------------------------------------------------*/
/* proc_i030_100   -- Process I030/100 data item                              */
/*----------------------------------------------------------------------------*/

 static
 t_Retc proc_i030_100 (t_Ui16 length, t_Byte *buffer)
{
	t_Byte df1;    /* Data field octet 1 */
	t_Byte df2;    /* Data field octet 2 */
	t_Byte df3;    /* Data field octet 3 */
	t_Byte df4;    /* Data field octet 4 */
	t_Retc ret;    /* Return code */
	t_Si16 x;      /* x component of Cartesian position */
	t_Si16 y;      /* y component of Cartesian position */

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
	list_text (2, ";  Calculated Track Position:");
	list_text (2, " x=" M_FMT_SI16 " (%.3f or %.3f nmi);",
                  x, (1.0 / 64.0) * x, (1.0 / 32.0) * x);
	list_text (2, " y=" M_FMT_SI16 " (%.3f or %.3f nmi)",
                  y, (1.0 / 64.0) * y, (1.0 / 32.0) * y);
	list_text (2, "\n");
#endif /* LISTER */

				   /* Store this information: */
	strk.calculated_position.present = TRUE;
	strk.calculated_position.value_x = (M_NMI2MTR / 64.0) * x;
	strk.calculated_position.value_y = (M_NMI2MTR / 64.0) * y;
                   /* Beware: Using the default LSB; may be adapted later */

				   /* Set the return code: */
	ret = RC_OKAY;

	return ret;
}

/*----------------------------------------------------------------------------*/
/* proc_i030_110   -- Process I030/110 data item                              */
/*----------------------------------------------------------------------------*/

 static
 t_Retc proc_i030_110 (t_Ui16 length, t_Byte *buffer)
{
	t_Byte df1;    /* Data field octet 1 */
	t_Byte df2;    /* Data field octet 2 */
	t_Byte df3;    /* Data field octet 3 */
	t_Byte df4;    /* Data field octet 4 */
	t_Retc ret;    /* Return code */
	t_Si16 x_acc;  /* Accuracy for x component of Cartesian position */
	t_Si16 y_acc;  /* Accuracy for y component of Cartesian position */

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
	x_acc = make_si16 (df1, df2);
	y_acc = make_si16 (df3, df4);

#if LISTER
				   /* List data field: */
	list_text (2, ";  Estimated Accuracy of Track Position:");
	list_text (2, " x=" M_FMT_SI16 " (%.3f or %.3f nmi)",
                  x_acc, (1.0 / 64.0) * x_acc, (1.0 / 32.0) * x_acc);
	list_text (2, " y=" M_FMT_SI16 " (%.3f or %.3f nmi)",
                  y_acc, (1.0 / 64.0) * y_acc, (1.0 / 32.0) * y_acc);
	list_text (2, "\n");
#endif /* LISTER */

				   /* Store this information: */
	strk.calculated_position_accuracy_present = TRUE;
	strk.calculated_x_position_accuracy = (1.0 / 64.0) * x_acc;
	strk.calculated_y_position_accuracy = (1.0 / 64.0) * y_acc;
                   /* Beware: Using the default LSB; may be adapted later */

				   /* Set the return code: */
	ret = RC_OKAY;

	return ret;
}

/*----------------------------------------------------------------------------*/
/* proc_i030_120   -- Process I030/120 data item                              */
/*----------------------------------------------------------------------------*/

 static
 t_Retc proc_i030_120 (t_Ui16 length, t_Byte *buffer)
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
	list_text (2, ";  Track Mode 2:");
	list_text (2, " v=" M_FMT_BYTE ";", (df1 >> 7) & 0x01);
	list_text (2, " g=" M_FMT_BYTE ";", (df1 >> 6) & 0x01);
	list_text (2, " c=" M_FMT_BYTE ";", (df1 >> 5) & 0x01);
	list_text (2, " code=" M_FMT_CODE, m2c);
	list_text (2, "\n");
#endif /* LISTER */

				   /* Store this information: */
	strk.mode_2_info.code = m2c;
	strk.mode_2_info.code_changed = (df1 >> 5) & 0x01;
	strk.mode_2_info.code_garbled = (df1 >> 6) & 0x01;
	strk.mode_2_info.code_invalid = (df1 >> 7) & 0x01;
	strk.mode_2_info.present = TRUE;

				   /* Set the return code: */
	ret = RC_OKAY;

	return ret;
}

/*----------------------------------------------------------------------------*/
/* proc_i030_130   -- Process I030/130 data item                              */
/*----------------------------------------------------------------------------*/

 static
 t_Retc proc_i030_130 (t_Ui16 length, t_Byte *buffer)
{
	t_Si16 alt;    /* Calculated track altitude; 25 feet */
	t_Byte alt_src;
                   /* Source indication */
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
	alt_src = (df1 >> 6) & 0x03;
	alt = make_si16 (df1 & 0x3f, df2);
	if (df1 & 0x20)
	{
		alt |= 0xc000;
	}

#if LISTER
				   /* List data field: */
	list_text (2, ";  Calculated Track Altitude:");
	list_text (2, " src=" M_FMT_BYTE " (", alt_src);
	if (alt_src == 0)
	{
		list_text (2, "3D height");
	}
	else if (alt_src == 1)
	{
		list_text (2, "triangulated height");
	}
	else if (alt_src == 2)
	{
		list_text (2, "height derived from coverage");
	}
	else if (alt_src == 3)
	{
		list_text (2, "assumed height");
	}
	list_text (2, ");");
	list_text (2, " value=" M_FMT_SI16 " (%.2f FL)", alt, 0.25 * alt);
	list_text (2, "\n");
#endif /* LISTER */

				   /* Store this information: */
	strk.calculated_track_altitude.present = TRUE;
	strk.calculated_track_altitude.source = alt_src;
	strk.calculated_track_altitude.value = (25.0 * M_FT2MTR) * alt;

				   /* Set the return code: */
	ret = RC_OKAY;

	return ret;
}

/*----------------------------------------------------------------------------*/
/* proc_i030_135   -- Process I030/135 data item                              */
/*----------------------------------------------------------------------------*/

 static
 t_Retc proc_i030_135 (t_Ui16 length, t_Byte *buffer)
{
	t_Ui16 alt_acc;
                   /* Estimated accuracy of track altitude; 25 feet */
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
	alt_acc = make_ui16 (df1, df2);

#if LISTER
				   /* List data field: */
	list_text (2, ";  Estimated Accuracy of Track Altitude:");
	list_text (2, " value=" M_FMT_UI16 " (%.2f FL)", alt_acc, 0.25 * alt_acc);
	list_text (2, "\n");
#endif /* LISTER */

				   /* Store this information: */
	strk.calculated_track_altitude_accuracy_present = TRUE;
	strk.calculated_track_altitude_accuracy =
		(25.0 * M_FT2MTR) * alt_acc;

				   /* Set the return code: */
	ret = RC_OKAY;

	return ret;
}

/*----------------------------------------------------------------------------*/
/* proc_i030_140   -- Process I030/140 data item                              */
/*----------------------------------------------------------------------------*/

 static
 t_Retc proc_i030_140 (t_Ui16 length, t_Byte *buffer)
{
	t_Byte df1;    /* Data field octet 1 */
	t_Byte df2;    /* Data field octet 2 */
	t_Si16 mch;    /* Last measured mode C height; 25 feet */
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
	mch = make_si16 (df1 & 0x3f, df2);
	if (df1 & 0x20)
	{
		mch |= 0xc000;
	}

#if LISTER
				   /* List data field: */
	list_text (2, ";  Last Measured Mode C:");
	list_text (2, " v=" M_FMT_BYTE ";", (df1 >> 7) & 0x01);
	list_text (2, " g=" M_FMT_BYTE ";", (df1 >> 6) & 0x01);
	list_text (2, " value=" M_FMT_SI16 " (%.2f FL)", mch, 0.25 * mch);
	list_text (2, "\n");
#endif /* LISTER */

				   /* Store this information: */
	strk.measured_information.present = TRUE;
	strk.measured_information.mdc_present = TRUE;
	strk.measured_information.value_mdc = mch;
	strk.measured_information.value_mdc_garbled = (df1 >> 6) & 0x01;
	strk.measured_information.value_mdc_invalid = (df1 >> 7) & 0x01;

				   /* Set the return code: */
	ret = RC_OKAY;

	return ret;
}

/*----------------------------------------------------------------------------*/
/* proc_i030_150   -- Process I030/150 data item                              */
/*----------------------------------------------------------------------------*/

 static
 t_Retc proc_i030_150 (t_Ui16 length, t_Byte *buffer)
{
	t_Byte df1;    /* Data field octet 1 */
	t_Byte df2;    /* Data field octet 2 */
	t_Si16 mch;    /* Measured track mode C height; 25 feet */
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
	mch = make_si16 (df1 & 0x3f, df2);
	if (df1 & 0x20)
	{
		mch |= 0xc000;
	}

#if LISTER
				   /* List data field: */
	list_text (2, ";  Measured Track Mode C:");
	list_text (2, " value=" M_FMT_SI16 " (%.2f FL)", mch, 0.25 * mch);
	list_text (2, "\n");
#endif /* LISTER */

				   /* Store this information: */
	strk.measured_track_mode_c_height.present = TRUE;
	strk.measured_track_mode_c_height.value = 25.0 * M_FT2MTR * mch;
	strk.measured_track_mode_c_height.value_in_feet = 25 * mch;

				   /* Set the return code: */
	ret = RC_OKAY;

	return ret;
}

/*----------------------------------------------------------------------------*/
/* proc_i030_160   -- Process I030/160 data item                              */
/*----------------------------------------------------------------------------*/

 static
 t_Retc proc_i030_160 (t_Ui16 length, t_Byte *buffer)
{
	t_Si16 cfl;    /* Calculated track flight level; 25 feet */
	t_Byte df1;    /* Data field octet 1 */
	t_Byte df2;    /* Data field octet 2 */
#if LISTER
	t_Byte qnc;    /* QNH correction */
#endif /* LISTER */
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
#if LISTER
	qnc = (df1 >> 6) & 0x01;
#endif /* LISTER */
	cfl = make_si16 (df1 & 0x3f, df2);
	if (df1 & 0x20)
	{
		cfl |= 0xc000;
	}

#if LISTER
				   /* List data field: */
	list_text (2, ";  Calculated Track Flight Level:");
	list_text (2, " qnc=" M_FMT_BYTE ";", qnc);
	list_text (2, " value=" M_FMT_SI16 " (%.2f FL)", cfl, 0.25 * cfl);
	list_text (2, "\n");
#endif /* LISTER */

				   /* Store this information: */
	strk.calculated_track_flight_level.present = TRUE;
	strk.calculated_track_flight_level.value = 25.0 * M_FT2MTR * cfl;
	strk.calculated_track_flight_level.value_in_feet = 25 * cfl;

				   /* Set the return code: */
	ret = RC_OKAY;

	return ret;
}

/*----------------------------------------------------------------------------*/
/* proc_i030_165   -- Process I030/165 data item                              */
/*----------------------------------------------------------------------------*/

 static
 t_Retc proc_i030_165 (t_Ui16 length, t_Byte *buffer)
{
	t_Ui16 alt_acc;
                   /* Estimated accuracy of calculated track flight level;
                      25 feet */
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
	alt_acc = make_ui16 (df1, df2);

#if LISTER
				   /* List data field: */
	list_text (2, ";  Estimated Accuracy of Calculated "
                  "Track Flight Level:");
	list_text (2, " value=" M_FMT_UI16 " (%.2f FL)",
                  alt_acc, 0.25 * alt_acc);
	list_text (2, "\n");
#endif /* LISTER */

				   /* Store this information: */
	strk.calculated_track_flight_level_accuracy_present = TRUE;
	strk.calculated_track_flight_level_accuracy = alt_acc;

				   /* Set the return code: */
	ret = RC_OKAY;

	return ret;
}

/*----------------------------------------------------------------------------*/
/* proc_i030_170   -- Process I030/170 data item                              */
/*----------------------------------------------------------------------------*/

 static
 t_Retc proc_i030_170 (t_Ui16 length, t_Byte *buffer)
{
	t_Byte amode;  /* Age of the last detection of the mode 3/A
                      contained in the track */
	t_Byte cmode;  /* Age of the last valid and credible mode C
                      contained in the track */
	t_Byte df1;    /* Data field octet 1 */
	t_Byte df2;    /* Data field octet 2 */
	t_Byte df3;    /* Data field octet 3 */
	t_Byte df4;    /* Data field octet 4 */
	t_Byte psr;    /* Age of the last primary plot/local track used
                      to update the track */
	t_Retc ret;    /* Return code */
	t_Byte ssr;    /* Age of the last secondary plot/local track used
                      to update the track */

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
	psr = df1;
	ssr = df2;
	amode = df3;
	cmode = df4;

#if LISTER
				   /* List data field: */
	list_text (2, ";  Track Ages:");
	list_text (2, " PSR=" M_FMT_BYTE " (%.2f sec);", psr, 0.25 * psr);
	list_text (2, " SSR=" M_FMT_BYTE " (%.2f sec)", ssr, 0.25 * ssr);
	list_text (2, "\n");
	list_text (2, ";   ");
	list_text (2, " amode=" M_FMT_BYTE " (%.2f sec);", amode, 0.25 * amode);
	list_text (2, " cmode=" M_FMT_BYTE " (%.2f sec)", cmode, 0.25 * cmode);
	list_text (2, "\n");
#endif /* LISTER */

				   /* Store this information: */
	strk.track_psr_age_present = TRUE;
	strk.track_psr_age = 0.25 * psr;
	strk.track_ssr_age_present = TRUE;
	strk.track_ssr_age = 0.25 * ssr;
	strk.track_mode_3a_age_present = TRUE;
	strk.track_mode_3a_age = 0.25 * amode;
	strk.data_ages.present = TRUE;
	strk.data_ages.mfl_present = TRUE;
	strk.data_ages.value_mfl = 0.25 * cmode;

				   /* Set the return code: */
	ret = RC_OKAY;

	return ret;
}

/*----------------------------------------------------------------------------*/
/* proc_i030_180   -- Process I030/180 data item                              */
/*----------------------------------------------------------------------------*/

 static
 t_Retc proc_i030_180 (t_Ui16 length, t_Byte *buffer)
{
	t_Byte df1;    /* Data field octet 1 */
	t_Byte df2;    /* Data field octet 2 */
	t_Byte df3;    /* Data field octet 3 */
	t_Byte df4;    /* Data field octet 4 */
	t_Ui16 hdg;    /* Heading; 360/2**16 degrees */
	t_Retc ret;    /* Return code */
	t_Ui16 spd;    /* Speed; 2**-14 nmi/second */

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
	strk.calculated_polar_velocity_present = TRUE;
	strk.calculated_track_speed = (M_NMI2MTR / 16384.0) * spd;
                   /* In metres/second */
	strk.calculated_track_heading = (M_TWO_PI / 65536.0) * hdg;
                   /* In radians */

				   /* Set the return code: */
	ret = RC_OKAY;

	return ret;
}

/*----------------------------------------------------------------------------*/
/* proc_i030_181   -- Process I030/181 data item                              */
/*----------------------------------------------------------------------------*/

 static
 t_Retc proc_i030_181 (t_Ui16 length, t_Byte *buffer)
{
	t_Byte df1;    /* Data field octet 1 */
	t_Byte df2;    /* Data field octet 2 */
	t_Byte df3;    /* Data field octet 3 */
	t_Byte df4;    /* Data field octet 4 */
	t_Retc ret;    /* Return code */
	t_Si16 vx;     /* x component of Cartesian velocity; 2**-14 nmi/second */
	t_Si16 vy;     /* y component of Cartesian velocity; 2**-14 nmi/second */

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
	vx = make_si16 (df1, df2);
	vy = make_si16 (df3, df4);

#if LISTER
				   /* List data field: */
	list_text (2, ";  Calculated Track Velocity:");
	list_text (2, " vx=" M_FMT_SI16 " (%.3f kts);",
                  vx, (3600.0 / 16384.0) * vx);
	list_text (2, " vy=" M_FMT_SI16 " (%.3f kts)",
                  vy, (3600.0 / 16384.0) * vy);
	list_text (2, "\n");
#endif /* LISTER */

				   /* Store this information: */
	strk.calculated_cartesian_velocity.present = TRUE;
	strk.calculated_cartesian_velocity.value_vx = (M_NMI2MTR / 16384.0) * vx;
	strk.calculated_cartesian_velocity.value_vy = (M_NMI2MTR / 16384.0) * vy;
                   /* In metres/second */

				   /* Set the return code: */
	ret = RC_OKAY;

	return ret;
}

/*----------------------------------------------------------------------------*/
/* proc_i030_190   -- Process I030/190 data item                              */
/*----------------------------------------------------------------------------*/

 static
 t_Retc proc_i030_190 (t_Ui16 length, t_Byte *buffer)
{
	t_Byte df1;    /* Data field octet 1 */
	t_Byte df2;    /* Data field octet 2 */
	t_Byte df3;    /* Data field octet 3 */
	t_Byte df4;    /* Data field octet 4 */
	t_Ui16 hdg_acc;
                   /* Heading accuracy; 360/2**16 degrees */
	t_Retc ret;    /* Return code */
	t_Ui16 spd_acc;
                   /* Speed accuracy; 2**-14 nmi/second */

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
	spd_acc = make_ui16 (df1, df2);
	hdg_acc = make_ui16 (df3, df4);

#if LISTER
				   /* List data field: */
	list_text (2, ";  Estimated Accuracy of Track Velocity:");
	list_text (2, " spd_acc=" M_FMT_UI16 " (%.3f kts);", 
                  spd_acc, (3600.0 / 16384.0) * spd_acc);
	list_text (2, " hdg_acc=" M_FMT_UI16 " (%.3f deg)",
                  hdg_acc, (360.0 / 65536.0) * hdg_acc);
	list_text (2, "\n");
#endif /* LISTER */

				   /* Store this information: */
	strk.calculated_polar_velocity_accuracy_present = TRUE;
	strk.calculated_track_speed_accuracy =
		(3600.0 / 16384.0) * spd_acc;
	strk.calculated_track_heading_accuracy =
		(360.0 / 65536.0) * hdg_acc;

				   /* Set the return code: */
	ret = RC_OKAY;

	return ret;
}

/*----------------------------------------------------------------------------*/
/* proc_i030_191   -- Process I030/191 data item                              */
/*----------------------------------------------------------------------------*/

 static
 t_Retc proc_i030_191 (t_Ui16 length, t_Byte *buffer)
{
	t_Byte df1;    /* Data field octet 1 */
	t_Byte df2;    /* Data field octet 2 */
	t_Byte df3;    /* Data field octet 3 */
	t_Byte df4;    /* Data field octet 4 */
	t_Retc ret;    /* Return code */
	t_Si16 vx_acc; /* x component accuracy of Cartesian velocity;
                      2**-14 nmi/second */
	t_Si16 vy_acc; /* y component accuracy of Cartesian velocity;
                      2**-14 nmi/second */

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
	vx_acc = make_si16 (df1, df2);
	vy_acc = make_si16 (df3, df4);

#if LISTER
				   /* List data field: */
	list_text (2, ";  Estimated Accuracy of Track Velocity:");
	list_text (2, " vx=" M_FMT_SI16 " (%.3f kts);",
                  vx_acc, (3600.0 / 16384.0) * vx_acc);
	list_text (2, " vy=" M_FMT_SI16 " (%.3f kts)",
                  vy_acc, (3600.0 / 16384.0) * vy_acc);
	list_text (2, "\n");
#endif /* LISTER */

				   /* Store this information: */
	strk.calculated_cartesian_velocity_accuracy_present = TRUE;
	strk.calculated_x_velocity_accuracy =
		(3600.0 / 16384.0) * vx_acc;
	strk.calculated_y_velocity_accuracy =
		(3600.0 / 16384.0) * vy_acc;

				   /* Set the return code: */
	ret = RC_OKAY;

	return ret;
}

/*----------------------------------------------------------------------------*/
/* proc_i030_200   -- Process I030/200 data item                              */
/*----------------------------------------------------------------------------*/

 static
 t_Retc proc_i030_200 (t_Ui16 length, t_Byte *buffer)
{
	t_Byte df1;    /* Data field octet 1 */
	t_Byte longi;  /* Longitudinal acceleration */
	t_Retc ret;    /* Return code */
	t_Byte trans;  /* Transversal acceleration */
	t_Byte verti;  /* Vertical attitude */

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
	verti = (df1 >> 2) & 0x03;

#if LISTER
				   /* List data field: */
	list_text (2, ";  Mode of Flight:");
	list_text (2, " trans=" M_FMT_BYTE " ", trans);
	if (trans == 0)
	{
		list_text (2, "(constant course) ");
	}
	else if (trans == 1)
	{
		list_text (2, "(right turn) ");
	}
	else if (trans == 2)
	{
		list_text (2, "(left turn) ");
	}
	else
	{
		list_text (2, "(undetermined) ");
	}
	list_text (2, ";");
	list_text (2, " longi=" M_FMT_BYTE " ", longi);
	if (longi == 0)
	{
		list_text (2, "(constant groundspeed) ");
	}
	else if (longi == 1)
	{
		list_text (2, "(increasing groundspeed) ");
	}
	else if (longi == 2)
	{
		list_text (2, "(decreasing groundspeed) ");
	}
	else
	{
		list_text (2, "(undetermined) ");
	}
	list_text (2, ";");
	list_text (2, " verti=" M_FMT_BYTE " ", verti);
	if (verti == 0)
	{
		list_text (2, "(level flight)");
	}
	else if (verti == 1)
	{
		list_text (2, "(climb)");
	}
	else if (verti == 2)
	{
		list_text (2, "(descent)");
	}
	else
	{
		list_text (2, "(undetermined)");
	}
	list_text (2, "\n");
#endif /* LISTER */

				   /* Store this information: */
	strk.mode_of_flight.present = TRUE;
	strk.mode_of_flight.value_longitudinal = longi;
	strk.mode_of_flight.value_transversal = trans;
	strk.mode_of_flight.value_vertical = verti;

				   /* Set the return code: */
	ret = RC_OKAY;

	return ret;
}

/*----------------------------------------------------------------------------*/
/* proc_i030_210   -- Process I030/210 data item                              */
/*----------------------------------------------------------------------------*/

 static
 t_Retc proc_i030_210 (t_Ui16 length, t_Byte *buffer)
{
	t_Byte df1;    /* Data field octet 1 */
	t_Byte df2;    /* Data field octet 2 */
	t_Byte df3;    /* Data field octet 3 */
	t_Byte lcprob; /* Probability for longitudinal acceleration
                      mode of flight component */
	t_Retc ret;    /* Return code */
	t_Byte tcprob; /* Probability for transversal acceleration
                      mode of flight component */
	t_Byte vcprob; /* Probability for vertical acceleration
                      mode of flight component */

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
	tcprob = df1;
	lcprob = df2;
	vcprob = df3;

#if LISTER
				   /* List data field: */
	list_text (2, ";  Mode of Flight Probabilities:");
	list_text (2, " trans=%.1f;", 0.5 * tcprob);
	list_text (2, " longi=%.1f;", 0.5 * lcprob);
	list_text (2, " verti=%.1f", 0.5 * vcprob);
	list_text (2, "\n");
#endif /* LISTER */

				   /* Store this information: */
	strk.mode_of_flight_probabilities_present = TRUE;
	strk.mof_transversal_probability = tcprob;
	strk.mof_longitudinal_probability = lcprob;
	strk.mof_vertical_probability = vcprob;

				   /* Set the return code: */
	ret = RC_OKAY;

	return ret;
}

/*----------------------------------------------------------------------------*/
/* proc_i030_220   -- Process I030/220 data item                              */
/*----------------------------------------------------------------------------*/

 static
 t_Retc proc_i030_220 (t_Ui16 length, t_Byte *buffer)
{
	t_Byte df1;    /* Data field octet 1 */
	t_Byte df2;    /* Data field octet 2 */
	t_Si16 rcd;    /* Calculated rate of climb/descent; 2**-10 FL/second */
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
	list_text (2, " crocd=" M_FMT_SI16 " (%.2f ft/min)", 
                  rcd, (60.0 * 100.0 / 1024.0) * rcd);
	list_text (2, "\n");
#endif /* LISTER */

				   /* Store this information: */
	strk.calculated_rate_of_climb_descent.present = TRUE;
	strk.calculated_rate_of_climb_descent.value =
		(100.0 * M_FT2MTR / 1024.0) * rcd;
                   /* In metres/second */

				   /* Set the return code: */
	ret = RC_OKAY;

	return ret;
}

/*----------------------------------------------------------------------------*/
/* proc_i030_230   -- Process I030/230 data item                              */
/*----------------------------------------------------------------------------*/

 static
 t_Retc proc_i030_230 (t_Ui16 length, t_Byte *buffer)
{
	t_Byte df1;    /* Data field octet 1 */
	t_Byte df2;    /* Data field octet 2 */
	t_Ui16 rcd_acc;
                   /* Estimated accuracy of rate of climb/descent; 
                      2**-10 FL/second */
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
	rcd_acc = make_ui16 (df1, df2);

#if LISTER
				   /* List data field: */
	list_text (2, ";  Estimated Accuracy of Rate of "
                  "Climb/Descent:");
	list_text (2, " crocd_acc=" M_FMT_UI16 " (%.2f ft/min)", 
                  rcd_acc, 
                  (60.0 * 100.0 / 1024.0) * rcd_acc);
	list_text (2, "\n");
#endif /* LISTER */

				   /* Store this information: */
	strk.calculated_rate_of_climb_descent_accuracy_present = TRUE;
	strk.calculated_rate_of_climb_descent_accuracy = 
		(60.0 * 100.0 / 1024.0) * rcd_acc;

				   /* Set the return code: */
	ret = RC_OKAY;

	return ret;
}

/*----------------------------------------------------------------------------*/
/* proc_i030_240   -- Process I030/240 data item                              */
/*----------------------------------------------------------------------------*/

 static
 t_Retc proc_i030_240 (t_Ui16 length, t_Byte *buffer)
{
	t_Si16 crt;    /* Calculated rate of turn; 2**-2 deg/second */
	t_Byte df1;    /* Data field octet 1 */
	t_Retc ret;    /* Return code */

				   /* Preset the return code: */
	ret = RC_FAIL;

				   /* Check parameters: */
	Assert (length == 1, "Invalid parameter");
	Assert (buffer != NULL, "Invalid parameter");

				   /* Extract octets: */
	df1 = buffer[0];

				   /* Extract information: */
	if (df1 & 0x80)
	{
		crt = make_si16 (0xff, df1);
	}
	else
	{
		crt = make_si16 (0x00, df1);
	}

#if LISTER
				   /* List data field: */
	list_text (2, ";  Calculated Rate of Turn:");
	list_text (2, " " M_FMT_SI16 " (%.2f deg/sec)", crt, 0.25 * crt);
	list_text (2, "\n");
#endif /* LISTER */

				   /* Store this information: */
	strk.calculated_rate_of_turn_present = TRUE;
	strk.calculated_rate_of_turn = 0.25 * crt;

				   /* Set the return code: */
	ret = RC_OKAY;

	return ret;
}

/*----------------------------------------------------------------------------*/
/* proc_i030_250   -- Process I030/250 data item                              */
/*----------------------------------------------------------------------------*/

 static
 t_Retc proc_i030_250 (t_Ui16 length, t_Byte *buffer)
{
	t_Ui16 crt_acc;
                   /* Estimated accuracy of rate of turn; 0.25 deg/second */
	t_Byte df1;    /* Data field octet 1 */
	t_Retc ret;    /* Return code */

				   /* Preset the return code: */
	ret = RC_FAIL;

				   /* Check parameters: */
	Assert (length == 1, "Invalid parameter");
	Assert (buffer != NULL, "Invalid parameter");

				   /* Extract octets: */
	df1 = buffer[0];

				   /* Extract information: */
	crt_acc = make_ui16 (0x00, df1);

#if LISTER
				   /* List data field: */
	list_text (2, ";  Estimated Accuracy of Rate of Turn:");
	list_text (2, " " M_FMT_UI16 " (%.2f deg/sec)",
                  crt_acc, 0.25 * crt_acc);
	list_text (2, "\n");
#endif /* LISTER */

				   /* Store this information: */
	strk.calculated_rate_of_turn_accuracy_present = TRUE;
	strk.calculated_rate_of_turn_accuracy = 0.25 * crt_acc;

				   /* Set the return code: */
	ret = RC_OKAY;

	return ret;
}

/*----------------------------------------------------------------------------*/
/* proc_i030_260   -- Process I030/260 data item                              */
/*----------------------------------------------------------------------------*/

 static
 t_Retc proc_i030_260 (t_Ui16 length, t_Byte *buffer)
{
	t_Byte df1;    /* Data field octet 1 */
	t_Byte df2;    /* Data field octet 2 */
	t_Ui16 dsi;    /* Radar identification tag */
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
	list_text (2, ";  Radar Identification Tag:");
	list_text (2, " 0x" M_FMT_UI16_HEX, dsi);
	list_text (2, " (SAC=" M_FMT_BYTE "; SIC=" M_FMT_BYTE ")", sac, sic);
	list_text (2, "\n");
#endif /* LISTER */

				   /* Store this information: */
	strk.last_updating_sensor.present = TRUE;
	strk.last_updating_sensor.value = dsi;

				   /* Set the return code: */
	ret = RC_OKAY;

	return ret;
}

/*----------------------------------------------------------------------------*/
/* proc_i030_270   -- Process I030/270 data item                              */
/*----------------------------------------------------------------------------*/

 static
 t_Retc proc_i030_270 (t_Ui16 length, t_Byte *buffer)
{
	t_Byte df1;    /* Data field octet 1 */
	t_Byte df2;    /* Data field octet 2 */
	t_Retc ret;    /* Return code */
	t_Ui16 tno;    /* Local track number */

				   /* Preset the return code: */
	ret = RC_FAIL;

				   /* Check parameters: */
	Assert (length == 2, "Invalid parameter");
	Assert (buffer != NULL, "Invalid parameter");

				   /* Extract octets: */
	df1 = buffer[0];
	df2 = buffer[1];

				   /* Extract information: */
	tno = make_ui16 (df1, df2);

#if LISTER
				   /* List data field: */
	list_text (2, ";  Local Track Number:");
	list_text (2, " " M_FMT_UI16, tno);
	list_text (2, "\n");
#endif /* LISTER */

				   /* Store this information: */
	strk.local_track_number.present = TRUE;
	strk.local_track_number.value = tno;

				   /* Set the return code: */
	ret = RC_OKAY;

	return ret;
}

/*----------------------------------------------------------------------------*/
/* proc_i030_290   -- Process I030/290 data item                              */
/*----------------------------------------------------------------------------*/

 static
 t_Retc proc_i030_290 (t_Ui16 length, t_Byte *buffer)
{
	t_Byte amode;  /* Age of last mode 3/A; 1/4 seconds */
	t_Byte cmode;  /* Age of last mode C; 1/4 seconds */
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
	amode = df1;
	cmode = df2;

#if LISTER
				   /* List data field: */
	list_text (2, ";  Plot Ages:");
	list_text (2, " amode=" M_FMT_BYTE " (%.2f sec);", amode, 0.25 * amode);
	list_text (2, " cmode=" M_FMT_BYTE " (%.2f sec)", cmode, 0.25 * cmode);
	list_text (2, "\n");
#endif /* LISTER */

				   /* Store this information: */
	strk.plot_mode_3a_age_present = TRUE;
	strk.plot_mode_3a_age = 0.25 * amode;
	strk.plot_mode_c_age_present = TRUE;
	strk.plot_mode_c_age = 0.25 * cmode;

				   /* Set the return code: */
	ret = RC_OKAY;

	return ret;
}

/*----------------------------------------------------------------------------*/
/* proc_i030_340   -- Process I030/340 data item                              */
/*----------------------------------------------------------------------------*/

 static
 t_Retc proc_i030_340 (t_Ui16 length, t_Byte *buffer)
{
	t_Byte df1;    /* Data field octet 1 */
	t_Byte df2;    /* Data field octet 2 */
	t_Ui16 m3c;    /* Last measured mode 3/A code */
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
	m3c = make_si16 (df1 & 0x0f, df2);

#if LISTER
				   /* List data field: */
	list_text (2, ";  Last Measured Mode 3/A:");
	list_text (2, " v=" M_FMT_BYTE ";", (df1 >> 7) & 0x01);
	list_text (2, " g=" M_FMT_BYTE ";", (df1 >> 6) & 0x01);
	list_text (2, " l=" M_FMT_BYTE ";", (df1 >> 5) & 0x01);
	list_text (2, " code=" M_FMT_CODE, m3c);
	list_text (2, "\n");
#endif /* LISTER */

				   /* Store this information: */
	strk.measured_information.present = TRUE;
	strk.measured_information.mda_present = TRUE;
	strk.measured_information.value_mda = m3c;
	strk.measured_information.value_mda_garbled = (df1 >> 6) & 0x01;
	strk.measured_information.value_mda_invalid = (df1 >> 7) & 0x01;
	strk.measured_information.value_mda_smoothed = (df1 >> 5) & 0x01;

				   /* Set the return code: */
	ret = RC_OKAY;

	return ret;
}

/*----------------------------------------------------------------------------*/
/* proc_i030_360   -- Process I030/360 data item                              */
/*----------------------------------------------------------------------------*/

 static
 t_Retc proc_i030_360 (t_Ui16 length, t_Byte *buffer)
{
	t_Byte df1;    /* Data field octet 1 */
	t_Byte df2;    /* Data field octet 2 */
	t_Byte df3;    /* Data field octet 3 */
	t_Byte df4;    /* Data field octet 4 */
	t_Retc ret;    /* Return code */
	t_Ui16 rho;    /* Measured distance; 1/128 nmi */
	t_Ui16 theta;  /* Measured azimuth; 360/2**16 degrees */

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
	list_text (2, " rho=" M_FMT_UI16 " (%.3f nmi);",
                  rho, (1.0 / 128.0) * rho);
	list_text (2, " theta=" M_FMT_UI16 " (%.3f deg)",
                  theta, (360.0 / 65536.0) * theta);
	list_text (2, "\n");
#endif /* LISTER */

				   /* Store this information: */
	strk.measured_information.present = TRUE;
	strk.measured_information.pos_present = TRUE;
	strk.measured_information.value_rho = (M_NMI2MTR / 128.0) * rho;
	strk.measured_information.value_theta = (M_TWO_PI / 65536.0) * theta;

				   /* Set the return code: */
	ret = RC_OKAY;

	return ret;
}

/*----------------------------------------------------------------------------*/
/* proc_i030_370   -- Process I030/370 data item                              */
/*----------------------------------------------------------------------------*/

 static
 t_Retc proc_i030_370 (t_Ui16 length, t_Byte *buffer)
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
	hgt = make_si16 (df1, df2);

#if LISTER
				   /* List data field: */
	list_text (2, ";  Measured 3D Height:");
	list_text (2, " " M_FMT_SI16 " (%.2f FL)", hgt, 0.25 * hgt);
	list_text (2, "\n");
#endif /* LISTER */

				   /* Store this information: */
	strk.measured_information.present = TRUE;
	strk.measured_information.h3d_present = TRUE;
	strk.measured_information.value_h3d = hgt;

				   /* Set the return code: */
	ret = RC_OKAY;

	return ret;
}

/*----------------------------------------------------------------------------*/
/* proc_i030_382   -- Process I030/382 data item                              */
/*----------------------------------------------------------------------------*/

 static
 t_Retc proc_i030_382 (t_Ui16 length, t_Byte *buffer)
{
	t_Byte df1;    /* Data field octet 1 */
	t_Byte df2;    /* Data field octet 2 */
	t_Byte df3;    /* Data field octet 3 */
	t_Ui32 msa;    /* Mode S address */
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
	msa = make_ui32 (0, df1, df2, df3);

#if LISTER
				   /* List data field: */
	list_text (2, ";  Aircraft Address:");
	list_text (2, " " M_FMT_ADDR " (" M_FMT_UI32 ")", msa, msa);
	list_text (2, "\n");
#endif /* LISTER */

				   /* Store this information: */
	strk.aircraft_address.present = TRUE;
	strk.aircraft_address.value = msa;

				   /* Set the return code: */
	ret = RC_OKAY;

	return ret;
}

/*----------------------------------------------------------------------------*/
/* proc_i030_384   -- Process I030/384 data item                              */
/*----------------------------------------------------------------------------*/

 static
 t_Retc proc_i030_384 (t_Ui16 length, t_Byte *buffer)
{
	t_Byte c;      /* 6 bits character */
	t_Byte df1;    /* Data field octet 1 */
	t_Byte df2;    /* Data field octet 2 */
	t_Byte df3;    /* Data field octet 3 */
	t_Byte df4;    /* Data field octet 4 */
	t_Byte df5;    /* Data field octet 5 */
	t_Byte df6;    /* Data field octet 6 */
	char idt[M_AIRCRAFT_IDENTIFICATION_LENGTH + 1];
                   /* Aircraft identification */
	t_Retc ret;    /* Return code */

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
	memset (idt, 0, 9);
	c = (df1 >> 2) & 0x3f;
	idt[0] = expand_c (c);
	c = (df1 & 0x03) << 4;
	c |= (df2 >> 4) & 0x0f;
	idt[1] = expand_c (c);
	c = (df2 & 0x0f) << 2;
	c |= (df3 >> 6) & 0x03;
	idt[2] = expand_c (c);
	c = df3 & 0x03f;
	idt[3] = expand_c (c);
	c = (df4 >> 2) & 0x3f;
	idt[4] = expand_c (c);
	c = (df4 & 0x03) << 4;
	c |= (df5 >> 4) & 0x0f;
	idt[5] = expand_c (c);
	c = (df5 & 0x0f) << 2;
	c |= (df6 >> 6) & 0x03;
	idt[6] = expand_c (c);
	c = df6 & 0x3f;
	idt[7] = expand_c (c);

#if LISTER
				   /* List data field: */
	list_text (2, ";  Aircraft Identification:");
	list_text (2, " '%s'", idt);
	list_text (2, "\n");
#endif /* LISTER */

				   /* Store this information: */
	strk.aircraft_identification.present = TRUE;
	strncpy (strk.aircraft_identification.value_idt, idt,
             M_AIRCRAFT_IDENTIFICATION_LENGTH);

				   /* Set the return code: */
	ret = RC_OKAY;

	return ret;
}

/*----------------------------------------------------------------------------*/
/* proc_i030_386   -- Process I030/386 data item                              */
/*----------------------------------------------------------------------------*/

 static
 t_Retc proc_i030_386 (t_Ui16 length, t_Byte *buffer)
{
	t_Byte cc;     /* Communications capability */
	t_Byte df1;    /* Data field octet 1 */
	t_Byte fs;     /* Flight status */
	t_Retc ret;    /* Return code */

				   /* Preset the return code: */
	ret = RC_FAIL;

				   /* Check parameters: */
	Assert (length == 1, "Invalid parameter");
	Assert (buffer != NULL, "Invalid parameter");

				   /* Extract octets: */
	df1 = buffer[0];

				   /* Extract information: */
	cc = (df1 >> 5) & 0x07;
	fs = (df1 >> 2) & 0x07;

#if LISTER
				   /* List data field: */
	list_text (2, ";  Communications Capability and Flight Status:");
	list_text (2, " cc=" M_FMT_BYTE ";", cc);
	list_text (2, " fs=" M_FMT_BYTE, fs);
	list_text (2, "\n");
#endif /* LISTER */

				   /* Store this information: */
	strk.communications_capability.present = TRUE;
	strk.communications_capability.value_com = cc;
	strk.flight_status.present = TRUE;
	strk.flight_status.value_status = fs;

				   /* Set the return code: */
	ret = RC_OKAY;

	return ret;
}

/*----------------------------------------------------------------------------*/
/* proc_i030_390   -- Process I030/390 data item                              */
/*----------------------------------------------------------------------------*/

 static
 t_Retc proc_i030_390 (t_Ui16 length, t_Byte *buffer)
{
	t_Byte df1;    /* Data field octet 1 */
	t_Byte df2;    /* Data field octet 2 */
	t_Ui16 dsi;    /* FPPS identification tag */
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
	list_text (2, ";  FPPS Identification Tag:");
	list_text (2, " 0x" M_FMT_UI16_HEX, dsi);
	list_text (2, " (SAC=" M_FMT_BYTE "; SIC=" M_FMT_BYTE ")", sac, sic);
	list_text (2, "\n");
#endif /* LISTER */

				   /* Store this information: */
	strk.fpps_sacsic_present = TRUE;
	strk.fpps_sacsic = dsi;

				   /* Set the return code: */
	ret = RC_OKAY;

	return ret;
}

/*----------------------------------------------------------------------------*/
/* proc_i030_400   -- Process I030/400 data item                              */
/*----------------------------------------------------------------------------*/

 static
 t_Retc proc_i030_400 (t_Ui16 length, t_Byte *buffer)
{
	char cs[7 + 1];
                   /* Callsign */
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

				   /* Extract information: */
	memset (cs, 0, 8);
	cs[0] = df1;
	cs[1] = df2;
	cs[2] = df3;
	cs[3] = df4;
	cs[4] = df5;
	cs[5] = df6;
	cs[6] = df7;

#if LISTER
				   /* List data field: */
	list_text (2, ";  Callsign:");
	list_text (2, " '%s'", cs);
	list_text (2, "\n");
#endif /* LISTER */

				   /* Store this information: */
	strk.callsign.present = TRUE;
	strncpy (strk.callsign.value, cs, 7);

				   /* Set the return code: */
	ret = RC_OKAY;

	return ret;
}

/*----------------------------------------------------------------------------*/
/* proc_i030_410   -- Process I030/410 data item                              */
/*----------------------------------------------------------------------------*/

 static
 t_Retc proc_i030_410 (t_Ui16 length, t_Byte *buffer)
{
	t_Byte df1;    /* Data field octet 1 */
	t_Byte df2;    /* Data field octet 2 */
	t_Ui16 pln;    /* PLN number */
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
	pln = make_ui16 (df1, df2);

#if LISTER
				   /* List data field: */
	list_text (2, ";  PLN Number:");
	list_text (2, " " M_FMT_UI16, pln);
	list_text (2, "\n");
#endif /* LISTER */

				   /* Store this information: */
	strk.pln_number_present = TRUE;
	strk.pln_number = pln;

				   /* Set the return code: */
	ret = RC_OKAY;

	return ret;
}

/*----------------------------------------------------------------------------*/
/* proc_i030_420   -- Process I030/420 data item                              */
/*----------------------------------------------------------------------------*/

 static
 t_Retc proc_i030_420 (t_Ui16 length, t_Byte *buffer)
{
	t_Byte df1;    /* Data field octet 1 */
	t_Byte fc;     /* Flight category */
	t_Retc ret;    /* Return code */

				   /* Preset the return code: */
	ret = RC_FAIL;

				   /* Check parameters: */
	Assert (length == 1, "Invalid parameter");
	Assert (buffer != NULL, "Invalid parameter");

				   /* Extract octets: */
	df1 = buffer[0];

				   /* Extract information: */
	fc = df1;

#if LISTER
				   /* List data field: */
	list_text (2, ";  Flight Category:");
	list_text (2, " " M_FMT_BYTE " (0x" M_FMT_BYTE_HEX ")", fc, fc);
	list_text (2, "\n");
#endif /* LISTER */

				   /* Store this information: */
	strk.flight_category_present = TRUE;
	strk.flight_category = fc;

				   /* Set the return code: */
	ret = RC_OKAY;

	return ret;
}

/*----------------------------------------------------------------------------*/
/* proc_i030_430   -- Process I030/430 data item                              */
/*----------------------------------------------------------------------------*/

 static
 t_Retc proc_i030_430 (t_Ui16 length, t_Byte *buffer)
{
	t_Byte df1;    /* Data field octet 1 */
	t_Byte df2;    /* Data field octet 2 */
	t_Byte df3;    /* Data field octet 3 */
	t_Byte df4;    /* Data field octet 4 */
	t_Retc ret;    /* Return code */
	char toa[4 + 1];
                   /* Type of aircraft */

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
	memset (toa, 0, 5);
	toa[0] = df1;
	toa[1] = df2;
	toa[2] = df3;
	toa[3] = df4;

#if LISTER
				   /* List data field: */
	list_text (2, ";  Type of Aircraft:");
	list_text (2, " '%s'", toa);
	list_text (2, "\n");
#endif /* LISTER */

				   /* Store this information: */
	strk.type_of_aircraft_present = TRUE;
	strncpy (strk.type_of_aircraft, toa, 4);

				   /* Set the return code: */
	ret = RC_OKAY;

	return ret;
}

/*----------------------------------------------------------------------------*/
/* proc_i030_435   -- Process I030/435 data item                              */
/*----------------------------------------------------------------------------*/

 static
 t_Retc proc_i030_435 (t_Ui16 length, t_Byte *buffer)
{
	t_Byte df1;    /* Data field octet 1 */
	t_Retc ret;    /* Return code */
	char wtc;      /* Wake turbulence category */

				   /* Preset the return code: */
	ret = RC_FAIL;

				   /* Check parameters: */
	Assert (length == 1, "Invalid parameter");
	Assert (buffer != NULL, "Invalid parameter");

				   /* Extract octets: */
	df1 = buffer[0];

				   /* Extract information: */
	wtc = df1;

#if LISTER
				   /* List data field: */
	list_text (2, ";  Category of Turbulence:");
	list_text (2, " '%c'", wtc);
	list_text (2, "\n");
#endif /* LISTER */

				   /* Store this information: */
	strk.category_of_turbulence_present = TRUE;
	strk.category_of_turbulence = wtc;

				   /* Set the return code: */
	ret = RC_OKAY;

	return ret;
}

/*----------------------------------------------------------------------------*/
/* proc_i030_440   -- Process I030/440 data item                              */
/*----------------------------------------------------------------------------*/

 static
 t_Retc proc_i030_440 (t_Ui16 length, t_Byte *buffer)
{
	char adep[4 + 1];
                   /* Departure airport */
	t_Byte df1;    /* Data field octet 1 */
	t_Byte df2;    /* Data field octet 2 */
	t_Byte df3;    /* Data field octet 3 */
	t_Byte df4;    /* Data field octet 4 */
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
	memset (adep, 0, 5);
	adep[0] = df1;
	adep[1] = df2;
	adep[2] = df3;
	adep[3] = df4;

#if LISTER
				   /* List data field: */
	list_text (2, ";  Departure Airport:");
	list_text (2, " '%s'", adep);
	list_text (2, "\n");
#endif /* LISTER */

				   /* Store this information: */
	strk.departure_airport_present = TRUE;
	strncpy (strk.departure_airport, adep, 4);

				   /* Set the return code: */
	ret = RC_OKAY;

	return ret;
}

/*----------------------------------------------------------------------------*/
/* proc_i030_450   -- Process I030/450 data item                              */
/*----------------------------------------------------------------------------*/

 static
 t_Retc proc_i030_450 (t_Ui16 length, t_Byte *buffer)
{
	char ades[4 + 1];
                   /* Destination airport */
	t_Byte df1;    /* Data field octet 1 */
	t_Byte df2;    /* Data field octet 2 */
	t_Byte df3;    /* Data field octet 3 */
	t_Byte df4;    /* Data field octet 4 */
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
	memset (ades, 0, 5);
	ades[0] = df1;
	ades[1] = df2;
	ades[2] = df3;
	ades[3] = df4;

#if LISTER
				   /* List data field: */
	list_text (2, ";  Destination Airport:");
	list_text (2, " '%s'", ades);
	list_text (2, "\n");
#endif /* LISTER */

				   /* Store this information: */
	strk.destination_airport_present = TRUE;
	strncpy (strk.destination_airport, ades, 4);

				   /* Set the return code: */
	ret = RC_OKAY;

	return ret;
}

/*----------------------------------------------------------------------------*/
/* proc_i030_460   -- Process I030/460 data item                              */
/*----------------------------------------------------------------------------*/

 static
 t_Retc proc_i030_460 (t_Ui16 length, t_Byte *buffer)
{
#if LISTER
	t_Byte df1, df2;
                   /* Data field octets */
	t_Ui16 inx;    /* Index within buffer */
	int ix;        /* Auxiliary */
#endif /* LISTER */
	t_Byte rep;    /* Repetition factor */
	t_Retc ret;    /* Return code */
#if LISTER
	t_Ui16 ssr;    /* SSR code */
#endif /* LISTER */

				   /* Preset the return code: */
	ret = RC_FAIL;

				   /* Check parameters: */
	Assert (length > 0, "Invalid parameter");
	Assert (buffer != NULL, "Invalid parameter");

                   /* Extract the repetition factor: */
	rep = buffer[0];

                   /* Check data item length (again): */
	Assert (length == 1 + 2 * rep, "Invalid length");

				   /* Extract information: */
	/* tba ... */

#if LISTER
				   /* List data field: */
	list_text (2, ";  Allocated SSR Codes:");

	inx = 1;
	for (ix = 0; ix < rep; ix ++)
	{
		df1 = buffer[inx];
		df2 = buffer[inx + 1];

		inx += 2;

		ssr = make_ui16 (df1 & 0x0f, df2);

		list_text (2, " " M_FMT_CODE, ssr);
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
/* proc_i030_480   -- Process I030/480 data item                              */
/*----------------------------------------------------------------------------*/

 static
 t_Retc proc_i030_480 (t_Ui16 length, t_Byte *buffer)
{
	t_Si16 cfl;    /* Current cleared flight level; 25 feet */
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
	cfl = make_si16 (df1, df2);

#if LISTER
				   /* List data field: */
	list_text (2, ";  Current Cleared Flight Level:");
	list_text (2, " " M_FMT_SI16 " (%.2f FL)", cfl, 0.25 * cfl);
	list_text (2, "\n");
#endif /* LISTER */

				   /* Store this information: */
	strk.cleared_flight_level_present = TRUE;
	strk.cleared_flight_level = cfl;

				   /* Set the return code: */
	ret = RC_OKAY;

	return ret;
}

/*----------------------------------------------------------------------------*/
/* proc_i030_490   -- Process I030/490 data item                              */
/*----------------------------------------------------------------------------*/

 static
 t_Retc proc_i030_490 (t_Ui16 length, t_Byte *buffer)
{
#if LISTER
	t_Byte centre; /* Centre identification code */
#endif /* LISTER */
	t_Byte df1;    /* Data field octet 1 */
	t_Byte df2;    /* Data field octet 2 */
#if LISTER
	t_Byte position;
                   /* Position identification code */
#endif /* LISTER */
	t_Retc ret;    /* Return code */

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
	centre = df1;
	position = df2;

				   /* List data field: */
	list_text (2, ";  Current Control Position:");
	list_text (2, " centre=" M_FMT_BYTE ";", centre);
	list_text (2, " position=" M_FMT_BYTE, position);
	list_text (2, "\n");
#endif /* LISTER */

				   /* Store this information: */
	strk.control_position_present = TRUE;
	strk.control_position = make_ui16 (df1, df2);

				   /* Set the return code: */
	ret = RC_OKAY;

	return ret;
}

/*----------------------------------------------------------------------------*/
/* proc_i030_spf   -- Process SPF indicator data item                         */
/*----------------------------------------------------------------------------*/

 static
 t_Retc proc_i030_spf (t_Ui16 length, t_Byte *buffer, t_Ui16 *pos_ptr)
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
	if (pos >= length)
	{
		error_msg ("ASTERIX frame too short");
		goto done;
	}
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
	list_buffer (1, ";  I030/SPF:", len, buffer + pos);
#endif /* LISTER */

				   /* Set position: */
	*pos_ptr = pos + len;

				   /* Set the return code: */
	ret = RC_OKAY;

	done:          /* We are done */
	return ret;
}
/* end-of-file */
