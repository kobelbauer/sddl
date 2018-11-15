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
/* File:           src/astx_221.cpp                                           */
/* Contents:       Process ASTERIX category 221 data block                    */
/* Author(s):      kb                                                         */
/* Last change:    2018-04-27                                                 */
/*----------------------------------------------------------------------------*/

/* Is equal to ASTERIX category 021 ed. 0.23 */

#include "basics.h"
                   /* Basic system definitions */
#include "common.h"
                   /* Common declarations and definitions */

#include <ctype.h>
#include <stddef.h>
#include <stdio.h>
#include <string.h>

				   /* Local macros: */
				   /* ------------- */

#define M_MAX_FRN 49
				   /* Maximum field reference number */
#define M_MAX_FSPEC_LENGTH 7
				   /* Maximum fields specification length
					  for ASTERIX category 221 */

                   /* Local texts: */
                   /* ------------ */

#if LISTER
static const char* emc_texts[24] =
                   /* Texts for emitter category in I221/020 */
{
	"light aircraft",
	"small aircraft",
	"medium aircraft",
	"high vortex large",
	"heavy aircraft",
	"highly manoeuvrable, high speed",
	"reserved",
	"reserved",
	"reserved",
	"rotocraft",
	"glider/sailplane",
	"lighter-than-air",
	"unmanned aerial vehicle",
	"space/transatmospheric vehicle",
	"ultralight/handglider/paraglider",
	"parachutist/skydiver",
	"reserved",
	"reserved",
	"reserved",
	"surface emergency vehicle",
	"surface service vehicle",
	"fixed ground or tethered obstruction",
	"cluster obstacle",
	"line obstacle"
};
#endif /* LISTER */

				   /* Local variables: */
				   /* ---------------- */

static t_Data_Item_Desc desc_i221_010;
                   /* Description of data item I221/010 */
static t_Data_Item_Desc desc_i221_020;
                   /* Description of data item I221/020 */
static t_Data_Item_Desc desc_i221_030;
                   /* Description of data item I221/030 */
static t_Data_Item_Desc desc_i221_032;
                   /* Description of data item I221/032 */
static t_Data_Item_Desc desc_i221_035;
                   /* Description of data item I221/035 */
static t_Data_Item_Desc desc_i221_040;
                   /* Description of data item I221/040 */
static t_Data_Item_Desc desc_i221_080;
                   /* Description of data item I221/080 */
static t_Data_Item_Desc desc_i221_090;
                   /* Description of data item I221/090 */
static t_Data_Item_Desc desc_i221_095;
                   /* Description of data item I221/095 */
static t_Data_Item_Desc desc_i221_100;
                   /* Description of data item I221/100 */
static t_Data_Item_Desc desc_i221_110;
                   /* Description of data item I221/110 */
static t_Data_Item_Desc desc_i221_130;
                   /* Description of data item I221/130 */
static t_Data_Item_Desc desc_i221_140;
                   /* Description of data item I221/140 */
static t_Data_Item_Desc desc_i221_145;
                   /* Description of data item I221/145 */
static t_Data_Item_Desc desc_i221_146;
                   /* Description of data item I221/146 */
static t_Data_Item_Desc desc_i221_147;
                   /* Description of data item I221/147 */
static t_Data_Item_Desc desc_i221_148;
                   /* Description of data item I221/148 */
static t_Data_Item_Desc desc_i221_150;
                   /* Description of data item I221/150 */
static t_Data_Item_Desc desc_i221_151;
                   /* Description of data item I221/151 */
static t_Data_Item_Desc desc_i221_152;
                   /* Description of data item I221/152 */
static t_Data_Item_Desc desc_i221_155;
                   /* Description of data item I221/155 */
static t_Data_Item_Desc desc_i221_157;
                   /* Description of data item I221/157 */
static t_Data_Item_Desc desc_i221_160;
                   /* Description of data item I221/160 */
static t_Data_Item_Desc desc_i221_165;
                   /* Description of data item I221/165 */
static t_Data_Item_Desc desc_i221_170;
                   /* Description of data item I221/170 */
static t_Data_Item_Desc desc_i221_180;
                   /* Description of data item I221/180 */
static t_Data_Item_Desc desc_i221_190;
                   /* Description of data item I221/190 */
static t_Data_Item_Desc desc_i221_200;
                   /* Description of data item I221/200 */
static t_Data_Item_Desc desc_i221_210;
                   /* Description of data item I221/210 */
static t_Data_Item_Desc desc_i221_220;
                   /* Description of data item I221/220 */
static t_Data_Item_Desc desc_i221_230;
                   /* Description of data item I221/230 */
static t_Data_Item_Desc desc_i221_ref;
                   /* Description of REF data item */
static t_Data_Item_Desc desc_i221_spf;
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
static t_Retc proc_i221_010 (t_Ui16 length, t_Byte *buffer);
                   /* Process I221/010 data item */
static t_Retc proc_i221_020 (t_Ui16 length, t_Byte *buffer);
                   /* Process I221/020 data item */
static t_Retc proc_i221_030 (t_Ui16 length, t_Byte *buffer);
                   /* Process I221/030 data item */
static t_Retc proc_i221_032 (t_Ui16 length, t_Byte *buffer);
                   /* Process I221/032 data item */
static t_Retc proc_i221_035 (t_Ui16 length, t_Byte *buffer);
                   /* Process I221/035 data item */
static t_Retc proc_i221_040 (t_Ui16 length, t_Byte *buffer);
                   /* Process I221/040 data item */
static t_Retc proc_i221_080 (t_Ui16 length, t_Byte *buffer);
                   /* Process I221/080 data item */
static t_Retc proc_i221_090 (t_Ui16 length, t_Byte *buffer);
                   /* Process I221/090 data item */
static t_Retc proc_i221_095 (t_Ui16 length, t_Byte *buffer);
                   /* Process I221/095 data item */
static t_Retc proc_i221_100 (t_Ui16 length, t_Byte *buffer);
                   /* Process I221/100 data item */
static t_Retc proc_i221_110 (t_Ui16 length, t_Byte *buffer, t_Ui16 *pos_ptr);
                   /* Process I221/110 data item */
static t_Retc proc_i221_130 (t_Ui16 length, t_Byte *buffer);
                   /* Process I221/130 data item */
static t_Retc proc_i221_140 (t_Ui16 length, t_Byte *buffer);
                   /* Process I221/140 data item */
static t_Retc proc_i221_145 (t_Ui16 length, t_Byte *buffer);
                   /* Process I221/145 data item */
static t_Retc proc_i221_146 (t_Ui16 length, t_Byte *buffer);
                   /* Process I221/146 data item */
static t_Retc proc_i221_147 (t_Ui16 length, t_Byte *buffer);
                   /* Process I221/147 data item */
static t_Retc proc_i221_148 (t_Ui16 length, t_Byte *buffer);
                   /* Process I221/148 data item */
static t_Retc proc_i221_150 (t_Ui16 length, t_Byte *buffer);
                   /* Process I221/150 data item */
static t_Retc proc_i221_151 (t_Ui16 length, t_Byte *buffer);
                   /* Process I221/151 data item */
static t_Retc proc_i221_152 (t_Ui16 length, t_Byte *buffer);
                   /* Process I221/152 data item */
static t_Retc proc_i221_155 (t_Ui16 length, t_Byte *buffer);
                   /* Process I221/155 data item */
static t_Retc proc_i221_157 (t_Ui16 length, t_Byte *buffer);
                   /* Process I221/157 data item */
static t_Retc proc_i221_160 (t_Ui16 length, t_Byte *buffer);
                   /* Process I221/160 data item */
static t_Retc proc_i221_165 (t_Ui16 length, t_Byte *buffer);
                   /* Process I221/165 data item */
static t_Retc proc_i221_170 (t_Ui16 length, t_Byte *buffer);
                   /* Process I221/170 data item */
static t_Retc proc_i221_180 (t_Ui16 length, t_Byte *buffer);
                   /* Process I221/180 data item */
static t_Retc proc_i221_190 (t_Ui16 length, t_Byte *buffer);
                   /* Process I221/190 data item */
static t_Retc proc_i221_200 (t_Ui16 length, t_Byte *buffer);
                   /* Process I221/200 data item */
static t_Retc proc_i221_210 (t_Ui16 length, t_Byte *buffer);
                   /* Process I221/210 data item */
static t_Retc proc_i221_220 (t_Ui16 length, t_Byte *buffer, t_Ui16 *pos_ptr);
                   /* Process I221/220 data item */
static t_Retc proc_i221_230 (t_Ui16 length, t_Byte *buffer);
                   /* Process I221/230 data item */
static t_Retc proc_i221_ref (t_Ui16 length, t_Byte *buffer, t_Ui16 *pos_ptr);
                   /* Process REF indicator data item */
static t_Retc proc_i221_spf (t_Ui16 length, t_Byte *buffer, t_Ui16 *pos_ptr);
                   /* Process SPF indicator data item */

/*----------------------------------------------------------------------------*/
/* astx_221        -- Process ASTERIX category 221 data block                 */
/*----------------------------------------------------------------------------*/

 t_Retc astx_221 (t_Ui16 length, t_Byte *buffer)
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

                   /* Preset ADS-B report information: */
		memset (&adsb, 0, sizeof (adsb));

                   /* Set data format: */
		adsb.asterix_category = 221;
		adsb.data_format = e_data_format_asterix;

                   /* Preset detection type: */
		adsb.detection_type.present = TRUE;
		adsb.detection_type.reported_from_ads = e_is_true;

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

					lrc = data_item (221, frn, std_uap[frn],
                                     length, buffer, &pos);
					if (lrc  != RC_OKAY)
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
			adsb.frame_date.present = TRUE;
			adsb.frame_date.value = frame_date;
		}

                   /* Add frame time, if available: */
		if (frame_time_present)
		{
			adsb.frame_time.present = TRUE;
			adsb.frame_time.value = frame_time;
		}

                   /* Add board/line number, if present: */
		if (current_line_number_defined)
		{
			adsb.line_number = current_line_number;
		}

                   /* Evaluate this ADS-B report: */
		lrc = process_adsb (&adsb);
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
                   /* Description of data item I221/010: */
	desc_i221_010.category = 221;
	desc_i221_010.data_item = 10;
	desc_i221_010.item_type = e_fixed_length_data_item;
	desc_i221_010.fixed_length = 2;
	desc_i221_010.proc_fptr = proc_i221_010;
	desc_i221_010.read_fptr = NULL;

                   /* Description of data item I221/020: */
	desc_i221_020.category = 221;
	desc_i221_020.data_item = 20;
	desc_i221_020.item_type = e_fixed_length_data_item;
	desc_i221_020.fixed_length = 1;
	desc_i221_020.proc_fptr = proc_i221_020;
	desc_i221_020.read_fptr = NULL;

                   /* Description of data item I221/030: */
	desc_i221_030.category = 221;
	desc_i221_030.data_item = 30;
	desc_i221_030.item_type = e_fixed_length_data_item;
	desc_i221_030.fixed_length = 3;
	desc_i221_030.proc_fptr = proc_i221_030;
	desc_i221_030.read_fptr = NULL;

                   /* Description of data item I221/032: */
	desc_i221_032.category = 221;
	desc_i221_032.data_item = 32;
	desc_i221_032.item_type = e_fixed_length_data_item;
	desc_i221_032.fixed_length = 1;
	desc_i221_032.proc_fptr = proc_i221_032;
	desc_i221_032.read_fptr = NULL;

                   /* Description of data item I221/035: */
	desc_i221_035.category = 221;
	desc_i221_035.data_item = 35;
	desc_i221_035.item_type = e_fixed_length_data_item;
	desc_i221_035.fixed_length = 4;
	desc_i221_035.proc_fptr = proc_i221_035;
	desc_i221_035.read_fptr = NULL;

                   /* Description of data item I221/040: */
	desc_i221_040.category = 221;
	desc_i221_040.data_item = 40;
	desc_i221_040.item_type = e_fixed_length_data_item;
	desc_i221_040.fixed_length = 2;
	desc_i221_040.proc_fptr = proc_i221_040;
	desc_i221_040.read_fptr = NULL;

                   /* Description of data item I221/080: */
	desc_i221_080.category = 221;
	desc_i221_080.data_item = 80;
	desc_i221_080.item_type = e_fixed_length_data_item;
	desc_i221_080.fixed_length = 3;
	desc_i221_080.proc_fptr = proc_i221_080;
	desc_i221_080.read_fptr = NULL;

                   /* Description of data item I221/090: */
	desc_i221_090.category = 221;
	desc_i221_090.data_item = 90;
	desc_i221_090.item_type = e_fixed_length_data_item;
	desc_i221_090.fixed_length = 2;
	desc_i221_090.proc_fptr = proc_i221_090;
	desc_i221_090.read_fptr = NULL;

                   /* Description of data item I221/095: */
	desc_i221_095.category = 221;
	desc_i221_095.data_item = 95;
	desc_i221_095.item_type = e_fixed_length_data_item;
	desc_i221_095.fixed_length = 1;
	desc_i221_095.proc_fptr = proc_i221_095;
	desc_i221_095.read_fptr = NULL;

                   /* Description of data item I221/100: */
	desc_i221_100.category = 221;
	desc_i221_100.data_item = 100;
	desc_i221_100.item_type = e_fixed_length_data_item;
	desc_i221_100.fixed_length = 1;
	desc_i221_100.proc_fptr = proc_i221_100;
	desc_i221_100.read_fptr = NULL;

                   /* Description of data item I221/110: */
	desc_i221_110.category = 221;
	desc_i221_110.data_item = 110;
	desc_i221_110.item_type = e_immediate_data_item;
	desc_i221_110.fixed_length = 0;
	desc_i221_110.proc_fptr = NULL;
	desc_i221_110.read_fptr = proc_i221_110;

                   /* Description of data item I221/130: */
	desc_i221_130.category = 221;
	desc_i221_130.data_item = 130;
	desc_i221_130.item_type = e_fixed_length_data_item;
	desc_i221_130.fixed_length = 6;
	desc_i221_130.proc_fptr = proc_i221_130;
	desc_i221_130.read_fptr = NULL;

                   /* Description of data item I221/140: */
	desc_i221_140.category = 221;
	desc_i221_140.data_item = 140;
	desc_i221_140.item_type = e_fixed_length_data_item;
	desc_i221_140.fixed_length = 2;
	desc_i221_140.proc_fptr = proc_i221_140;
	desc_i221_140.read_fptr = NULL;

                   /* Description of data item I221/145: */
	desc_i221_145.category = 221;
	desc_i221_145.data_item = 145;
	desc_i221_145.item_type = e_fixed_length_data_item;
	desc_i221_145.fixed_length = 2;
	desc_i221_145.proc_fptr = proc_i221_145;
	desc_i221_145.read_fptr = NULL;

                   /* Description of data item I221/146: */
	desc_i221_146.category = 221;
	desc_i221_146.data_item = 146;
	desc_i221_146.item_type = e_fixed_length_data_item;
	desc_i221_146.fixed_length = 2;
	desc_i221_146.proc_fptr = proc_i221_146;
	desc_i221_146.read_fptr = NULL;

                   /* Description of data item I221/147: */
	desc_i221_147.category = 221;
	desc_i221_147.data_item = 147;
	desc_i221_147.item_type = e_fixed_length_data_item;
	desc_i221_147.fixed_length = 2;
	desc_i221_147.proc_fptr = proc_i221_147;
	desc_i221_147.read_fptr = NULL;

                   /* Description of data item I221/148: */
	desc_i221_148.category = 221;
	desc_i221_148.data_item = 148;
	desc_i221_148.item_type = e_fixed_length_data_item;
	desc_i221_148.fixed_length = 2;
	desc_i221_148.proc_fptr = proc_i221_148;
	desc_i221_148.read_fptr = NULL;

                   /* Description of data item I221/150: */
	desc_i221_150.category = 221;
	desc_i221_150.data_item = 150;
	desc_i221_150.item_type = e_fixed_length_data_item;
	desc_i221_150.fixed_length = 2;
	desc_i221_150.proc_fptr = proc_i221_150;
	desc_i221_150.read_fptr = NULL;

                   /* Description of data item I221/151: */
	desc_i221_151.category = 221;
	desc_i221_151.data_item = 151;
	desc_i221_151.item_type = e_fixed_length_data_item;
	desc_i221_151.fixed_length = 2;
	desc_i221_151.proc_fptr = proc_i221_151;
	desc_i221_151.read_fptr = NULL;

                   /* Description of data item I221/152: */
	desc_i221_152.category = 221;
	desc_i221_152.data_item = 152;
	desc_i221_152.item_type = e_fixed_length_data_item;
	desc_i221_152.fixed_length = 2;
	desc_i221_152.proc_fptr = proc_i221_152;
	desc_i221_152.read_fptr = NULL;

                   /* Description of data item I221/155: */
	desc_i221_155.category = 221;
	desc_i221_155.data_item = 155;
	desc_i221_155.item_type = e_fixed_length_data_item;
	desc_i221_155.fixed_length = 2;
	desc_i221_155.proc_fptr = proc_i221_155;
	desc_i221_155.read_fptr = NULL;

                   /* Description of data item I221/157: */
	desc_i221_157.category = 221;
	desc_i221_157.data_item = 157;
	desc_i221_157.item_type = e_fixed_length_data_item;
	desc_i221_157.fixed_length = 2;
	desc_i221_157.proc_fptr = proc_i221_157;
	desc_i221_157.read_fptr = NULL;

                   /* Description of data item I221/160: */
	desc_i221_160.category = 221;
	desc_i221_160.data_item = 160;
	desc_i221_160.item_type = e_fixed_length_data_item;
	desc_i221_160.fixed_length = 4;
	desc_i221_160.proc_fptr = proc_i221_160;
	desc_i221_160.read_fptr = NULL;

                   /* Description of data item I221/165: */
	desc_i221_165.category = 221;
	desc_i221_165.data_item = 165;
	desc_i221_165.item_type = e_variable_length_data_item;
	desc_i221_165.fixed_length = 0;
	desc_i221_165.proc_fptr = proc_i221_165;
	desc_i221_165.read_fptr = NULL;

                   /* Description of data item I221/170: */
	desc_i221_170.category = 221;
	desc_i221_170.data_item = 170;
	desc_i221_170.item_type = e_fixed_length_data_item;
	desc_i221_170.fixed_length = 6;
	desc_i221_170.proc_fptr = proc_i221_170;
	desc_i221_170.read_fptr = NULL;

                   /* Description of data item I221/180: */
	desc_i221_180.category = 221;
	desc_i221_180.data_item = 180;
	desc_i221_180.item_type = e_fixed_length_data_item;
	desc_i221_180.fixed_length = 2;
	desc_i221_180.proc_fptr = proc_i221_180;
	desc_i221_180.read_fptr = NULL;

                   /* Description of data item I221/190: */
	desc_i221_190.category = 221;
	desc_i221_190.data_item = 190;
	desc_i221_190.item_type = e_fixed_length_data_item;
	desc_i221_190.fixed_length = 4;
	desc_i221_190.proc_fptr = proc_i221_190;
	desc_i221_190.read_fptr = NULL;

                   /* Description of data item I221/200: */
	desc_i221_200.category = 221;
	desc_i221_200.data_item = 200;
	desc_i221_200.item_type = e_fixed_length_data_item;
	desc_i221_200.fixed_length = 1;
	desc_i221_200.proc_fptr = proc_i221_200;
	desc_i221_200.read_fptr = NULL;

                   /* Description of data item I221/210: */
	desc_i221_210.category = 221;
	desc_i221_210.data_item = 210;
	desc_i221_210.item_type = e_fixed_length_data_item;
	desc_i221_210.fixed_length = 1;
	desc_i221_210.proc_fptr = proc_i221_210;
	desc_i221_210.read_fptr = NULL;

                   /* Description of data item I221/220: */
	desc_i221_220.category = 221;
	desc_i221_220.data_item = 220;
	desc_i221_220.item_type = e_immediate_data_item;
	desc_i221_220.fixed_length = 0;
	desc_i221_220.proc_fptr = NULL;
	desc_i221_220.read_fptr = proc_i221_220;

                   /* Description of data item I221/230: */
	desc_i221_230.category = 221;
	desc_i221_230.data_item = 230;
	desc_i221_230.item_type = e_fixed_length_data_item;
	desc_i221_230.fixed_length = 2;
	desc_i221_230.proc_fptr = proc_i221_230;
	desc_i221_230.read_fptr = NULL;

                   /* Description of data item I221/REF: */
	desc_i221_ref.category = 221;
	desc_i221_ref.data_item = M_REF_INDICATOR;
	desc_i221_ref.item_type = e_immediate_data_item;
	desc_i221_ref.fixed_length = 0;
	desc_i221_ref.proc_fptr = NULL;
	desc_i221_ref.read_fptr = proc_i221_ref;

                   /* Description of data item I221/SPF: */
	desc_i221_spf.category = 221;
	desc_i221_spf.data_item = M_SPF_INDICATOR;
	desc_i221_spf.item_type = e_immediate_data_item;
	desc_i221_spf.fixed_length = 0;
	desc_i221_spf.proc_fptr = NULL;
	desc_i221_spf.read_fptr = proc_i221_spf;

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

	std_uap[1] = &desc_i221_010;
	std_uap[2] = &desc_i221_040;
	std_uap[3] = &desc_i221_030;
	std_uap[4] = &desc_i221_130;
	std_uap[5] = &desc_i221_080;
	std_uap[6] = &desc_i221_140;
	std_uap[7] = &desc_i221_090;

	std_uap[8] = &desc_i221_210;
	std_uap[9] = &desc_i221_230;
	std_uap[10] = &desc_i221_145;
	std_uap[11] = &desc_i221_150;
	std_uap[12] = &desc_i221_151;
	std_uap[13] = &desc_i221_152;
	std_uap[14] = &desc_i221_155;

	std_uap[15] = &desc_i221_157;
	std_uap[16] = &desc_i221_160;
	std_uap[17] = &desc_i221_165;
	std_uap[18] = &desc_i221_170;
	std_uap[19] = &desc_i221_095;
	std_uap[20] = &desc_i221_032;
	std_uap[21] = &desc_i221_200;

	std_uap[22] = &desc_i221_020;
	std_uap[23] = &desc_i221_220;
	std_uap[24] = &desc_i221_146;
	std_uap[25] = &desc_i221_148;
	std_uap[26] = &desc_i221_110;

	std_uap[34] = &desc_i221_ref;
	std_uap[35] = &desc_i221_spf;

	return;
}

/*----------------------------------------------------------------------------*/
/* proc_i221_010   -- Process I221/010 data item                              */
/*----------------------------------------------------------------------------*/

 static
 t_Retc proc_i221_010 (t_Ui16 length, t_Byte *buffer)
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
	adsb.data_source_identifier.present = TRUE;
	adsb.data_source_identifier.value = dsi;
    adsb.data_source_identifier.sac = df1;
    adsb.data_source_identifier.sic = df2;

				   /* Set the return code: */
	ret = RC_OKAY;

	return ret;
}

/*----------------------------------------------------------------------------*/
/* proc_i221_020   -- Process I221/020 data item                              */
/*----------------------------------------------------------------------------*/

 static
 t_Retc proc_i221_020 (t_Ui16 length, t_Byte *buffer)
{
#if LISTER
	t_Byte df1;    /* Data field octet 1 */
	t_Byte emc;    /* Emitter category */
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
	emc = df1;

				   /* List data field: */
	list_text (2, ";  Emitter Category: " M_FMT_BYTE, emc);
	if (1 <= emc && emc <= 24)
	{
		list_text (2, " (%s)", emc_texts[emc - 1]);
	}
	else
	{
		list_text (2, " (unexpected)");
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
/* proc_i221_030   -- Process I221/030 data item                              */
/*----------------------------------------------------------------------------*/

 static
 t_Retc proc_i221_030 (t_Ui16 length, t_Byte *buffer)
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

				   /* Convert to seconds: */
	tod_in_secs = (1.0 / 128.0) * tod;

#if LISTER
				   /* List data field: */
	list_text (2, ";  Time of Day:");
	list_text (2, " " M_FMT_ATOD " (" M_FMT_UI32 "; %s UTC)",
                  tod, tod, utc_text (tod_in_secs));
	list_text (2, "\n");
#endif /* LISTER */

                   /* Remember this information: */
	last_tod_available = TRUE;
	last_tod = tod;

                   /* Store this information: */
	adsb.detection_time.present = TRUE;
	adsb.detection_time.value = tod_in_secs;

				   /* Set the return code: */
	ret = RC_OKAY;

	return ret;
}

/*----------------------------------------------------------------------------*/
/* proc_i221_032   -- Process I221/032 data item                              */
/*----------------------------------------------------------------------------*/

 static
 t_Retc proc_i221_032 (t_Ui16 length, t_Byte *buffer)
{
#if LISTER
	t_Byte df1;    /* Data field octet 1 */
#endif /* LISTER */
	t_Retc ret;    /* Return code */
#if LISTER
	t_Byte tod_acc;
                   /* Time of day accuracy; 1/256 seconds */
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
	tod_acc = df1;

				   /* List data field: */
	list_text (2, ";  Time of Day Accuracy:");
	list_text (2, " " M_FMT_BYTE " (%.3f seconds)",
                  tod_acc, (1.0 / 256.0) * tod_acc);
	list_text (2, "\n");
#endif /* LISTER */

                   /* Store this information: */
	/* tba ... */

				   /* Set the return code: */
	ret = RC_OKAY;

	return ret;
}

/*----------------------------------------------------------------------------*/
/* proc_i221_035   -- Process I221/035 data item                              */
/*----------------------------------------------------------------------------*/

 static
 t_Retc proc_i221_035 (t_Ui16 length, t_Byte *buffer)
{
#if LISTER
	t_Byte df1;    /* Data field octet 1 */
	t_Byte df2;    /* Data field octet 2 */
	t_Byte df3;    /* Data field octet 3 */
	t_Byte df4;    /* Data field octet 4 */
#endif /* LISTER */
	t_Retc ret;    /* Return code */
#if LISTER
	t_Ui32 tor;    /* Time of receipt; 2**-15 seconds */
#endif /* LISTER */

				   /* Preset the return code: */
	ret = RC_FAIL;

				   /* Check parameters: */
	Assert (length == 4, "Invalid parameter");
	Assert (buffer != NULL, "Invalid parameter");

#if LISTER
				   /* Extract octets: */
	df1 = buffer[0];
	df2 = buffer[1];
	df3 = buffer[2];
	df4 = buffer[3];

				   /* Extract information: */
	tor = make_ui32 (df1, df2, df3, df4);

				   /* List data field: */
	list_text (2, ";  Time of Receipt:");
	list_text (2, " 0x" M_FMT_UI32_HEX " (" M_FMT_UI32 ")", tor, tor);
	list_text (2, "\n");
#endif /* LISTER */

                   /* Store this information: */
	/* tba ... */

				   /* Set the return code: */
	ret = RC_OKAY;

	return ret;
}

/*----------------------------------------------------------------------------*/
/* proc_i221_040   -- Process I221/040 data item                              */
/*----------------------------------------------------------------------------*/

 static
 t_Retc proc_i221_040 (t_Ui16 length, t_Byte *buffer)
{
	t_Byte df1;    /* Data field octet 1 */
	t_Byte df2;    /* Data field octet 2 */
	t_Retc ret;    /* Return code */

				   /* Preset the return code: */
	ret = RC_FAIL;

				   /* Check parameters: */
	Assert (length == 2, "Invalid parameter");
	Assert (buffer != NULL, "Invalid parameter");

                   /* Extract information: */
	df1 = buffer[0];
	df2 = buffer[1];

#if LISTER
				   /* List data field: */
	list_text (2, ";  Target Report Descriptor:\n");
	{
		t_Byte arc;
		t_Byte atp;

		list_text (2, ";  ");

		if (df1 & 0x80)
		{
			list_text (2, " DCR");
		}

		if (df1 & 0x40)
		{
			list_text (2, " GBS");
		}

		if (df1 & 0x20)
		{
			list_text (2, " SIM");
		}

		if (df1 & 0x10)
		{
			list_text (2, " TST");
		}

		if (df1 & 0x08)
		{
			list_text (2, " RAB");
		}

		if (df1 & 0x04)
		{
			list_text (2, " SAA");
		}

		if (df1 & 0x02)
		{
			list_text (2, " SPI");
		}

		list_text (2, "\n");

		list_text (2, ";  ");

		atp = (df2 >> 5) & 0x07;
		list_text (2, " ATP=" M_FMT_BYTE, atp);
		switch (atp)
		{
		case 0:
			list_text (2, " (non unique address)");
			break;
		case 1:
			list_text (2, " (24-bit ICAO address)");
			break;
		case 2:
			list_text (2, " (surface vehicle address)");
			break;
		case 3:
			list_text (2, " (anonymous address)");
			break;
		case 4:
		case 5:
		case 6:
		case 7:
			list_text (2, " (reserved for future use)");
			break;
		}

		arc = (df2 >> 3) & 0x03;
		list_text (2, " ARC=" M_FMT_BYTE, arc);
		if (arc == 0)
		{
			list_text (2, " (unknown)");
		}
		else if (arc == 1)
		{
			list_text (2, " (25 ft)");
		}
		else if (arc == 2)
		{
			list_text (2, " (100 ft)");
		}

		list_text (2, "\n");
	}
#endif /* LISTER */

                   /* Store this information: */
	{
		t_Byte atp;

		/* tba ... */

		if (df1 & 0x40)
		{
			adsb.ground_bit_set = TRUE;
		}

		adsb.detection_type.simulated = M_TRES (df1 & 0x20);

		adsb.detection_type.test_target = M_TRES (df1 & 0x10);

		adsb.detection_type.from_fixed_field_transponder =
			M_TRES (df1 & 0x08);

		adsb.special_position_indication =
			M_TRES (df1 & 0x02);

		/* tba ... */

		atp = (df2 >> 5) & 0x07;
		adsb.target_address_type.present = TRUE;
		switch (atp)
		{
		case 0:
			adsb.target_address_type.value = e_target_address_non_unique;
			break;
		case 1:
			adsb.target_address_type.value = e_target_address_icao_24_bit;
			break;
		case 2:
			adsb.target_address_type.value = e_target_address_surface_vehicle;
			break;
		case 3:
			adsb.target_address_type.value = e_target_address_anonymous;
			break;
		default:
			adsb.target_address_type.value = e_target_address_undefined;
			break;
		}

		/* tba ... */
	}


				   /* Set the return code: */
	ret = RC_OKAY;

	return ret;
}

/*----------------------------------------------------------------------------*/
/* proc_i221_080   -- Process I221/080 data item                              */
/*----------------------------------------------------------------------------*/

 static
 t_Retc proc_i221_080 (t_Ui16 length, t_Byte *buffer)
{
	t_Byte df1;    /* Data field octet 1 */
	t_Byte df2;    /* Data field octet 2 */
	t_Byte df3;    /* Data field octet 3 */
	t_Retc ret;    /* Return code */
	t_Ui32 ta;     /* Target address */

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
	ta = make_ui32 (0x00, df1, df2, df3);

#if LISTER
				   /* List data field: */
	list_text (2, ";  Aircraft Address:");
	list_text (2, " " M_FMT_ADDR " (" M_FMT_UI32 ")", ta, ta);
	list_text (2, "\n");
#endif /* LISTER */

                   /* Store this information: */
	adsb.target_address.present = TRUE;
	adsb.target_address.value = ta;

				   /* Set the return code: */
	ret = RC_OKAY;

	return ret;
}

/*----------------------------------------------------------------------------*/
/* proc_i221_090   -- Process I221/090 data item                              */
/*----------------------------------------------------------------------------*/

 static
 t_Retc proc_i221_090 (t_Ui16 length, t_Byte *buffer)
{
#if LISTER
	t_Byte ac;     /* ACAS status */
	t_Byte dc;     /* Differential correction status */
	t_Byte df1;    /* Data field octet 1 */
#endif /* LISTER */
	t_Byte df2;    /* Data field octet 2 */
#if LISTER
	t_Byte mn;     /* Multiple navigational aids status */
#endif /* LISTER */
	t_Byte pa;     /* Position accuracy */
	t_Retc ret;    /* Return code */

				   /* Preset the return code: */
	ret = RC_FAIL;

				   /* Check parameters: */
	Assert (length == 2, "Invalid parameter");
	Assert (buffer != NULL, "Invalid parameter");

				   /* Extract octets: */
#if LISTER
	df1 = buffer[0];
#endif /* LISTER */
	df2 = buffer[1];

				   /* Extract information: */
#if LISTER
	ac = (df1 >> 6) & 0x03;
	mn = (df1 >> 4) & 0x03;
	dc = (df1 >> 2) & 0x03;
#endif /* LISTER */
	pa = df2 & 0x0f;

#if LISTER
				   /* List data field: */
	list_text (2, ";  Figure of Merit:");
	list_text (2, " ac=" M_FMT_BYTE ";", ac);
	list_text (2, " mn=" M_FMT_BYTE ";", mn);
	list_text (2, " dc=" M_FMT_BYTE ";", dc);
	list_text (2, " pa=" M_FMT_BYTE, pa);
	list_text (2, "\n");
#endif /* LISTER */

                   /* Store this information: */
	if (pa > 0)
	{
		adsb.figure_of_merit.present = TRUE;
		adsb.figure_of_merit.value = pa;
	}
	/* tba ... */

				   /* Set the return code: */
	ret = RC_OKAY;

	return ret;
}

/*----------------------------------------------------------------------------*/
/* proc_i221_095   -- Process I221/095 data item                              */
/*----------------------------------------------------------------------------*/

 static
 t_Retc proc_i221_095 (t_Ui16 length, t_Byte *buffer)
{
	t_Byte df1;    /* Data field octet 1 */
	t_Retc ret;    /* Return code */
	t_Byte va;     /* Velocity accuracy */

				   /* Preset the return code: */
	ret = RC_FAIL;

				   /* Check parameters: */
	Assert (length == 1, "Invalid parameter");
	Assert (buffer != NULL, "Invalid parameter");

				   /* Extract octets: */
	df1 = buffer[0];

				   /* Extract information: */
	va = df1;

#if LISTER
				   /* List data field: */
	list_text (2, ";  Velocity Accuracy:");
	list_text (2, " " M_FMT_BYTE, va);
	list_text (2, "\n");
#endif /* LISTER */

                   /* Store this information: */
	if (va > 0)
	{
		adsb.velocity_accuracy.present = TRUE;
		adsb.velocity_accuracy.value = va;
	}

				   /* Set the return code: */
	ret = RC_OKAY;

	return ret;
}

/*----------------------------------------------------------------------------*/
/* proc_i221_100   -- Process I221/100 data item                              */
/*----------------------------------------------------------------------------*/

 static
 t_Retc proc_i221_100 (t_Ui16 length, t_Byte *buffer)
{
#if LISTER
	t_Byte df1;    /* Data field octet 1 */
	t_Byte err;    /* Event report reason */
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
	err = df1;

				   /* List data field: */
	list_text (2, ";  Event Report Reason:");
	list_text (2, " " M_FMT_BYTE, err);
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
/* proc_i221_110   -- Process I221/110 data item                              */
/*----------------------------------------------------------------------------*/

 static
 t_Retc proc_i221_110 (t_Ui16 length, t_Byte *buffer, t_Ui16 *pos_ptr)
{
	t_Byte df1;    /* Data field octet 1 */
	t_Byte len;    /* Length of data field */
	int inx;       /* Auxiliary */
	t_Ui16 pos;    /* Position within buffer */
	t_Byte rep;    /* Repetition factor */
	t_Retc ret;    /* Return code */
	t_Byte sf1_present;
                   /* Subfield 1 present (TIS) */
	t_Byte sf2_present;
                   /* Subfield 2 present (TID) */
	t_Byte tis;    /* Trajectory intent status */

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

                   /* Determine expected length of data field: */
	len = 1;
	if (sf1_present)
	{
		len += 1;
	}
	if (sf2_present)
	{
		if (pos + len >= length)
		{
			error_msg ("Invalid buffer length (I221/110)");
			goto done;
		}

		rep = buffer[pos + len];

		len += (1 + rep * 15);
	}

                   /* Check against buffer length: */
	if (pos + len > length)
	{
		error_msg ("Invalid buffer length (I221/110)");
		goto done;
	}

#if LISTER
                   /* List raw data field: */
	list_buffer (1, ";  I221/110:", len, buffer + pos);
#endif /* LISTER */

                   /* Extract secondary subfields: */
	inx = 1;
	if (sf1_present)
	{
		tis = buffer[pos + inx];
		++ inx;

		Assert ((tis & 0x01) == 0x00, "Unexpected FX bit");
	}
	if (sf2_present)
	{
		/* tba ... */

		inx += (1 + rep * 15);
	}
	Assert (inx == len, "Invalid structure decoding");

#if LISTER
                   /* List data field: */
	list_text (2, ";  Trajectory Intent:");
	list_text (2, "\n");
	if (sf1_present)
	{
		list_text (2, ";   Trajectory intent status:");
		list_text (2, " nav=" M_FMT_BYTE ";", (tis >> 7) & 0x01);
		list_text (2, " nvb=" M_FMT_BYTE, (tis >> 6) & 0x01);
		list_text (2, "\n");
	}
	if (sf2_present)
	{
		list_text (2, ";   Trajectory intent data: ");
		/* tba ... */
		list_text (2, "\n");
	}
#endif /* LISTER */

                   /* Store this information: */
	/* tba ... */

                   /* Set position: */
	*pos_ptr = pos + len;

                   /* Set the return code: */
	ret = RC_OKAY;

	done:          /* We are done */
	return ret;
}

/*----------------------------------------------------------------------------*/
/* proc_i221_130   -- Process I221/130 data item                              */
/*----------------------------------------------------------------------------*/

 static
 t_Retc proc_i221_130 (t_Ui16 length, t_Byte *buffer)
{
	t_Byte df1;    /* Data field octet 1 */
	t_Byte df2;    /* Data field octet 2 */
	t_Byte df3;    /* Data field octet 3 */
	t_Byte df4;    /* Data field octet 4 */
	t_Byte df5;    /* Data field octet 5 */
	t_Byte df6;    /* Data field octet 6 */
	t_Si32 lat;    /* Latitude; 180/2**23 degrees */
	t_Real latf;   /* Latitude; degrees */
	t_Si32 lon;    /* Longitude; 180/2**23 degrees */
	t_Real lonf;   /* Longitude; degrees */
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
	if (df1 & 0x80)
	{
		lat = make_si32 (0xff, df1, df2, df3);
	}
	else
	{
		lat = make_si32 (0x00, df1, df2, df3);
	}
	if (df4 & 0x80)
	{
		lon = make_si32 (0xff, df4, df5, df6);
	}
	else
	{
		lon = make_si32 (0x00, df4, df5, df6);
	}

                   /* Convert: */
	latf = (180.0 / M_TWO_POWER_23) * lat;
	lonf = (180.0 / M_TWO_POWER_23) * lon;

#if LISTER
				   /* List data field: */
	list_text (2, ";  Position in WGS-84 Co-ordinates:");
	list_text (2, "\n");
	list_text (2, ";   lat=" M_FMT_SI32 " (%s);", lat, lat_text (latf));
	list_text (2, " lon=" M_FMT_SI32 " (%s)", lon, lon_text (lonf));
	list_text (2, "\n");
#endif /* LISTER */

                   /* Store this information: */
	adsb.wgs84_position.present = TRUE;
	adsb.wgs84_position.value_lat = M_DEG2RAD * latf;
	adsb.wgs84_position.value_lon = M_DEG2RAD * lonf;
                   /* Values in radians */

				   /* Set the return code: */
	ret = RC_OKAY;

	return ret;
}

/*----------------------------------------------------------------------------*/
/* proc_i221_140   -- Process I221/140 data item                              */
/*----------------------------------------------------------------------------*/

 static
 t_Retc proc_i221_140 (t_Ui16 length, t_Byte *buffer)
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
	list_text (2, ";  Altitude:");
	list_text (2, " " M_FMT_SI16 " (%.2f ft)", alt, 6.25 * alt);
	list_text (2, "\n");
#endif /* LISTER */

                   /* Store this information: */
	adsb.geometric_altitude.present = TRUE;
	adsb.geometric_altitude.value = 6.25 * M_FT2MTR * alt;

				   /* Set the return code: */
	ret = RC_OKAY;

	return ret;
}

/*----------------------------------------------------------------------------*/
/* proc_i221_145   -- Process I221/145 data item                              */
/*----------------------------------------------------------------------------*/

 static
 t_Retc proc_i221_145 (t_Ui16 length, t_Byte *buffer)
{
	t_Byte df1;    /* Data field octet 1 */
	t_Byte df2;    /* Data field octet 2 */
	t_Si16 fl;     /* Flight level; 25 feet */
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
	fl = make_si16 (df1, df2);

#if LISTER
				   /* List data field: */
	list_text (2, ";  Flight Level:");
	list_text (2, " " M_FMT_SI16 " (%.2f FL)", fl, 0.25 * fl);
	list_text (2, "\n");
#endif /* LISTER */

                   /* Store this information: */
	adsb.mode_c_height.in_25_feet = e_is_true;
	adsb.mode_c_height.present = TRUE;
	adsb.mode_c_height.value = 25.0 * M_FT2MTR * fl;
	adsb.mode_c_height.value_in_feet = 25 * (t_Si32) fl;

				   /* Set the return code: */
	ret = RC_OKAY;

	return ret;
}

/*----------------------------------------------------------------------------*/
/* proc_i221_146   -- Process I221/146 data item                              */
/*----------------------------------------------------------------------------*/

 static
 t_Retc proc_i221_146 (t_Ui16 length, t_Byte *buffer)
{
#if LISTER
	t_Si16 alt;    /* Altitude; 25 feet */
	t_Byte df1;    /* Data field octet 1 */
	t_Byte df2;    /* Data field octet 2 */
#endif /* LISTER */
	t_Retc ret;    /* Return code */
#if LISTER
	t_Byte sas;    /* Source available */
	t_Byte src;    /* Source */
#endif /* LISTER */

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
	sas = (df1 >> 7) & 0x01;
	src = (df1 >> 5) & 0x03;
	alt = make_si16 (df1 & 0x1f, df2);
	if (df1 & 0x10)
	{
		alt |= 0xe000;
	}

				   /* List data field: */
	list_text (2, ";  Intermediate State Selected Altitude:");
	list_text (2, " sas=" M_FMT_BYTE ";", sas);
	list_text (2, " src=" M_FMT_BYTE ";", src);
	list_text (2, " alt=" M_FMT_SI16 " (%.2f FL)", alt, 0.25 * alt);
	list_text (2, "\n");
#endif /* LISTER */

                   /* Store this information: */
	/* tba ... */

				   /* Set the return code: */
	ret = RC_OKAY;

	return ret;
}

/*----------------------------------------------------------------------------*/
/* proc_i221_147   -- Process I221/147 data item                              */
/*----------------------------------------------------------------------------*/

 static
 t_Retc proc_i221_147 (t_Ui16 length, t_Byte *buffer)
{
#if LISTER
	t_Byte df1;    /* Data field octet 1 */
	t_Byte df2;    /* Data field octet 2 */
	t_Si16 fl;     /* Selected flight level; 25 feet */
#endif /* LISTER */
	t_Retc ret;    /* Return code */
#if LISTER
	t_Byte v;      /* Valid selected flight level */
#endif /* LISTER */

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
	v = (df1 >> 7) & 0x01;
	fl = make_si16 (df1 & 0x7f, df2);

				   /* List data field: */
	list_text (2, ";  Selected Flight Level:");
	list_text (2, " v=" M_FMT_BYTE ";", v);
	list_text (2, " fl=" M_FMT_SI16 " (%.2f ft)", fl, 25.0 * fl);
	list_text (2, "\n");
#endif /* LISTER */

                   /* Store this information: */
	/* tba ... */

				   /* Set the return code: */
	ret = RC_OKAY;

	return ret;
}

/*----------------------------------------------------------------------------*/
/* proc_i221_148   -- Process I221/148 data item                              */
/*----------------------------------------------------------------------------*/

 static
 t_Retc proc_i221_148 (t_Ui16 length, t_Byte *buffer)
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
	alt = make_si16 (df1 & 0x1f, df2);
	if (df1 & 0x10)
	{
		alt |= 0xe000;
	}

#if LISTER
				   /* List data field: */
	list_text (2, ";  Final State Selected Altitude:");
	list_text (2, " mv=" M_FMT_BYTE ";", (df1 >> 7) & 0x01);
	list_text (2, " ah=" M_FMT_BYTE ";", (df1 >> 6) & 0x01);
	list_text (2, " am=" M_FMT_BYTE ";", (df1 >> 5) &0x01);
	list_text (2, " alt=" M_FMT_SI16 " (%.2f FL)", alt, 0.25 * alt);
	list_text (2, "\n");
#endif /* LISTER */

                   /* Store this information: */
	/* tba ... */

				   /* Set the return code: */
	ret = RC_OKAY;

	return ret;
}

/*----------------------------------------------------------------------------*/
/* proc_i221_150   -- Process I221/150 data item                              */
/*----------------------------------------------------------------------------*/

 static
 t_Retc proc_i221_150 (t_Ui16 length, t_Byte *buffer)
{
#if LISTER
	t_Byte df1;    /* Data field octet 1 */
	t_Byte df2;    /* Data field octet 2 */
	t_Byte im;     /* Air speed in IAS or in Mach */
#endif /* LISTER */
	t_Retc ret;    /* Return code */
#if LISTER
	t_Ui16 spd;    /* Air speed */
#endif /* LISTER */

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
	im = (df1 >> 7) & 0x01;
	spd = make_ui16 (df1 & 0x7f, df2);

				   /* List data field: */
	list_text (2, ";  Air Speed:");
	list_text (2, " im=" M_FMT_BYTE ";", im);
	list_text (2, " spd=");
	if (im == 0)
	{
		list_text (2, "%.3f kts", (3600.0 / 16384.0) * spd);
	}
	else
	{
		list_text (2, "%.3f Mach", 0.001 * spd);
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
/* proc_i221_151   -- Process I221/151 data item                              */
/*----------------------------------------------------------------------------*/

 static
 t_Retc proc_i221_151 (t_Ui16 length, t_Byte *buffer)
{
#if LISTER
	t_Byte df1;    /* Data field octet 1 */
	t_Byte df2;    /* Data field octet 2 */
#endif /* LISTER */
	t_Retc ret;    /* Return code */
#if LISTER
	t_Ui16 spd;    /* True air speed; knots */
#endif /* LISTER */

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
	spd = make_ui16 (df1, df2);

				   /* List data field: */
	list_text (2, ";  True Air Speed:");
	list_text (2, " spd=" M_FMT_UI16 " kts", spd);
	list_text (2, "\n");
#endif /* LISTER */

                   /* Store this information: */
	/* tba ... */

				   /* Set the return code: */
	ret = RC_OKAY;

	return ret;
}

/*----------------------------------------------------------------------------*/
/* proc_i221_152   -- Process I221/152 data item                              */
/*----------------------------------------------------------------------------*/

 static
 t_Retc proc_i221_152 (t_Ui16 length, t_Byte *buffer)
{
#if LISTER
	t_Byte df1;    /* Data field octet 1 */
	t_Byte df2;    /* Data field octet 2 */
	t_Ui16 hdg;    /* Magnetic heading; 360/2**16 degrees */
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
	hdg = make_ui16 (df1, df2);

				   /* List data field: */
	list_text (2, ";  Magnetic Heading:");
	list_text (2, " " M_FMT_UI16 " (%.3f deg)", hdg, (360.0 / 65536.0) * hdg);
	list_text (2, "\n");
#endif /* LISTER */

                   /* Store this information: */
	/* tba ... */

				   /* Set the return code: */
	ret = RC_OKAY;

	return ret;
}

/*----------------------------------------------------------------------------*/
/* proc_i221_155   -- Process I221/155 data item                              */
/*----------------------------------------------------------------------------*/

 static
 t_Retc proc_i221_155 (t_Ui16 length, t_Byte *buffer)
{
	t_Si16 bvr;    /* Barometric vertical rate; 6.25 feet/minute */
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
	bvr = make_si16 (df1, df2);

#if LISTER
				   /* List data field: */
	list_text (2, ";  Barometric Vertical Rate:");
	list_text (2, " bvr=" M_FMT_SI16 " (%.2f ft/min)", bvr, 6.25 * bvr);
	list_text (2, "\n");
#endif /* LISTER */

                   /* Store this information: */
	adsb.barometric_vertical_rate.present = TRUE;
	adsb.barometric_vertical_rate.value = 6.25 * bvr;

				   /* Set the return code: */
	ret = RC_OKAY;

	return ret;
}

/*----------------------------------------------------------------------------*/
/* proc_i221_157   -- Process I221/157 data item                              */
/*----------------------------------------------------------------------------*/

 static
 t_Retc proc_i221_157 (t_Ui16 length, t_Byte *buffer)
{
	t_Byte df1;    /* Data field octet 1 */
	t_Byte df2;    /* Data field octet 2 */
	t_Si16 gvr;    /* Geometric vertical rate; 6.25 feet/minute */
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
	gvr = make_si16 (df1, df2);

#if LISTER
				   /* List data field: */
	list_text (2, ";  Geometric Vertical Rate:");
	list_text (2, " gvr=" M_FMT_SI16 " (%.2f ft/min)", gvr, 6.25 * gvr);
	list_text (2, "\n");
#endif /* LISTER */

                   /* Store this information: */
	adsb.geometric_vertical_rate.present = TRUE;
	adsb.geometric_vertical_rate.value = 6.25 * gvr;

				   /* Set the return code: */
	ret = RC_OKAY;

	return ret;
}

/*----------------------------------------------------------------------------*/
/* proc_i221_160   -- Process I221/160 data item                              */
/*----------------------------------------------------------------------------*/

 static
 t_Retc proc_i221_160 (t_Ui16 length, t_Byte *buffer)
{
	t_Byte df1;    /* Data field octet 1 */
	t_Byte df2;    /* Data field octet 2 */
	t_Byte df3;    /* Data field octet 3 */
	t_Byte df4;    /* Data field octet 4 */
	t_Retc ret;    /* Return code */
	t_Si16 spd;    /* Ground speed; 2**-14 nautical miles/second */
	t_Ui16 ta;     /* Track angle; 360/2**16 degrees */

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
	spd = make_si16 (df1, df2);
	ta = make_ui16 (df3, df4);

#if LISTER
				   /* List data field: */
	list_text (2, ";  Ground Vector:");
	list_text (2, " spd=" M_FMT_SI16 " (%.3f kts);",
                  spd, (3600.0 / 16384.0) * spd);
	list_text (2, " ta=" M_FMT_UI16 " (%.3f deg)",
                  ta, (360.0 / 65536.0) * ta);
	list_text (2, "\n");
#endif /* LISTER */

                   /* Store this information: */
	adsb.ground_vector.present = TRUE;
	adsb.ground_vector.value_gsp = (M_NMI2MTR / 16384.0) * spd;
	adsb.ground_vector.value_hdg = (M_TWO_PI / 65536.0) * ta; 

				   /* Set the return code: */
	ret = RC_OKAY;

	return ret;
}

/*----------------------------------------------------------------------------*/
/* proc_i221_165   -- Process I221/165 data item                              */
/*----------------------------------------------------------------------------*/

 static
 t_Retc proc_i221_165 (t_Ui16 length, t_Byte *buffer)
{
	t_Byte df1;    /* Data field octet 1 */
	t_Byte df2;    /* Data field octet 2 */
	t_Si16 rate;   /* Rate of turn; 0.25 degrees/second */
	t_Retc ret;    /* Return code */
#if LISTER
	t_Byte ti;     /* Turn indicator */
#endif /* LISTER */

				   /* Preset the return code: */
	ret = RC_FAIL;

				   /* Check parameters: */
	Assert (length >= 1, "Invalid parameter");
	Assert (buffer != NULL, "Invalid parameter");

				   /* Extract octets: */
	df1 = buffer[0];

                   /* There may be a second octet: */
	if (df1 & 0x01)
	{
		Assert (length == 2, "Invalid buffer length");

		df2 = buffer[1];
	}
	else
	{
		Assert (length == 1, "Invalid buffer length");
	}

				   /* Extract information: */
#if LISTER
	ti = (df1 >> 6) & 0x03;
#endif /* LISTER */
	if (df1 & 0x01)
	{
		rate = (df2 >> 1) & 0x3f;
		if (df2 & 0x80)
		{
			rate |= 0xffc0;
		}
                   /* Two's complement */
	}

#if LISTER
				   /* List data field: */
	list_text (2, ";  Rate of Turn:");
	list_text (2, " ti=" M_FMT_BYTE, ti);
	if (ti == 0)
	{
		list_text (2, " (not available)");
	}
	else if (ti == 1)
	{
		list_text (2, " (left)");
	}
	else if (ti == 2)
	{
		list_text (2, " (right)");
	}
	else if (ti == 3)
	{
		list_text (2, " (straight)");
	}
	if (df1 & 0x01)
	{
		list_text (2, "; rate=%.2f deg/sec", 0.25 * rate);
	}
	list_text (2, "\n");
#endif /* LISTER */

                   /* Remember this information: */
	/* tba ... */

				   /* Set the return code: */
	ret = RC_OKAY;

	return ret;
}

/*----------------------------------------------------------------------------*/
/* proc_i221_170   -- Process I221/170 data item                              */
/*----------------------------------------------------------------------------*/

 static
 t_Retc proc_i221_170 (t_Ui16 length, t_Byte *buffer)
{
	t_Byte c6;     /* Character in 6-bit encoding */
	t_Byte df1;    /* Data field octet 1 */
	t_Byte df2;    /* Data field octet 2 */
	t_Byte df3;    /* Data field octet 3 */
	t_Byte df4;    /* Data field octet 4 */
	t_Byte df5;    /* Data field octet 5 */
	t_Byte df6;    /* Data field octet 6 */
	t_Retc ret;    /* Return code */
	char txt[9];   /* Text buffer for target identification */
                   /* If a flight plan has been submitted to the
                      responsible ATC/ATM authorities, this ID is
                      equal to the callsign as entered into the
                      flight plan, or else it is equal to the
                      tail number of the aircraft */

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
	memset (txt, 0, 9);
	c6 = (df1 >> 2) & 0x3f;
	txt[0] = expand_c (c6);
	c6 = ((df1 & 0x03) << 4) | ((df2 >> 4) & 0x0f);
	txt[1] = expand_c (c6);
    c6 = ((df2 & 0x0f) << 2) | ((df3 >> 6) & 0x03);
	txt[2] = expand_c (c6);
	c6 = df3 & 0x3f;
	txt[3] = expand_c (c6);
	c6 = (df4 >> 2) & 0x3f;
	txt[4] = expand_c (c6);
	c6 = ((df4 & 0x03) << 4) | ((df5 >> 4) & 0x0f);
	txt[5] = expand_c (c6);
	c6 = ((df5 & 0x0f) << 2) | ((df6 >> 6) & 0x03);
	txt[6] = expand_c (c6);
	c6 = df6 & 0x3f;
	txt[7] = expand_c (c6);

#if LISTER
				   /* List data field: */
	list_text (2, ";  Target Identification:");
	list_text (2, " idt=[%s]", txt);
	list_text (2, "\n");
#endif /* LISTER */

                   /* Store this information: */
	adsb.target_identification.present = TRUE;
	strncpy (adsb.target_identification.value_idt, txt,
             M_AIRCRAFT_IDENTIFICATION_LENGTH);

				   /* Set the return code: */
	ret = RC_OKAY;

	return ret;
}

/*----------------------------------------------------------------------------*/
/* proc_i221_180   -- Process I221/180 data item                              */
/*----------------------------------------------------------------------------*/

 static
 t_Retc proc_i221_180 (t_Ui16 length, t_Byte *buffer)
{
	t_Byte df1;    /* Data field octet 1 */
	t_Byte df2;    /* Data field octet 2 */
	t_Ui16 m3c;    /* Mode 3/A code */
	t_Byte m3cv;   /* Code validated flag */
                   /* 0=code validated, 1=code not validated */
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
	m3cv = (df1 >> 7) & 0x01;
	m3c = make_ui16 (df1 & 0x0f, df2);

#if LISTER
				   /* List data field: */
	list_text (2, ";  Mode 3/A Code:");
	list_text (2, " v=" M_FMT_BYTE ";", m3cv);
	list_text (2, " code=" M_FMT_CODE, m3c);
	list_text (2, "\n");
#endif /* LISTER */

                   /* Store this information: */
	adsb.mode_3a_info.present = TRUE;
	adsb.mode_3a_info.code = m3c;
	adsb.mode_3a_info.code_invalid = M_TRES (m3cv);

				   /* Set the return code: */
	ret = RC_OKAY;

	return ret;
}

/*----------------------------------------------------------------------------*/
/* proc_i221_190   -- Process I221/190 data item                              */
/*----------------------------------------------------------------------------*/

 static
 t_Retc proc_i221_190 (t_Ui16 length, t_Byte *buffer)
{
#if LISTER
	t_Byte df1;    /* Data field octet 1 */
	t_Byte df2;    /* Data field octet 2 */
	t_Byte df3;    /* Data field octet 3 */
	t_Byte df4;    /* Data field octet 4 */
#endif /* LISTER */
	t_Retc ret;    /* Return code */
#if LISTER
	char txt[5];   /* Text buffer for aircraft type */
#endif /* LISTER */

				   /* Preset the return code: */
	ret = RC_FAIL;

				   /* Check parameters: */
	Assert (length == 4, "Invalid parameter");
	Assert (buffer != NULL, "Invalid parameter");

#if LISTER
				   /* Extract octets: */
	df1 = buffer[0];
	df2 = buffer[1];
	df3 = buffer[2];
	df4 = buffer[3];

				   /* Extract information: */
	memset (txt, 0, 5);
	if (isprint (df1))
	{
		txt[0] = df1;
	}
	else
	{
		txt[0] = '?';
	}
	if (isprint (df2))
	{
		txt[1] = df2;
	}
	else
	{
		txt[1] = '?';
	}
	if (isprint (df3))
	{
		txt[2] = df3;
	}
	else
	{
		txt[2] = '?';
	}
	if (isprint (df4))
	{
		txt[3] = df4;
	}
	else
	{
		txt[3] = '?';
	}

				   /* List data field: */
	list_text (2, ";  Aircraft Type:");
	list_text (2, " %s", txt);
	list_text (2, "\n");
#endif /* LISTER */

                   /* Store this information: */
	/* tba ... */

				   /* Set the return code: */
	ret = RC_OKAY;

	return ret;
}

/*----------------------------------------------------------------------------*/
/* proc_i221_200   -- Process I221/200 data item                              */
/*----------------------------------------------------------------------------*/

 static
 t_Retc proc_i221_200 (t_Ui16 length, t_Byte *buffer)
{
#if LISTER
	t_Byte df1;    /* Data field octet 1 */
#endif /* LISTER */
	t_Retc ret;    /* Return code */
#if LISTER
	t_Byte sta;    /* Target status */
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
	sta = df1;

				   /* List data field: */
	list_text (2, ";  Target Status:");
	switch (sta)
	{
	case 0:
		list_text (2, " no emergency/not reported");
		break;
	case 1:
		list_text (2, " general emergency");
		break;
	case 2:
		list_text (2, " lifeguard/medical");
		break;
	case 3:
		list_text (2, " minimum fuel");
		break;
	case 4:
		list_text (2, " no communications");
		break;
	case 5:
		list_text (2, " unlawful interference");
		break;
	default:
		break;
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
/* proc_i221_210   -- Process I221/210 data item                              */
/*----------------------------------------------------------------------------*/

 static
 t_Retc proc_i221_210 (t_Ui16 length, t_Byte *buffer)
{
	t_Byte df1;    /* Data field octet 1 */
	t_Byte lti;    /* Link technology indicator */
#if LISTER
	int out;       /* Auxiliary */
#endif /* LISTER */
	t_Retc ret;    /* Return code */

				   /* Preset the return code: */
	ret = RC_FAIL;

				   /* Check parameters: */
	Assert (length == 1, "Invalid parameter");
	Assert (buffer != NULL, "Invalid parameter");

				   /* Extract octets: */
	df1 = buffer[0];

				   /* Extract information: */
	lti = df1;

#if LISTER
				   /* List data field: */
	list_text (2, ";  Link Technology Indicator:");
	out = 0;
	if (lti & 0x10)
	{
		list_text (2, " CDTI");
		++ out;
	}
	if (lti & 0x08)
	{
		if (out > 0)
		{
			list_text (2, ";");
		}
		list_text (2, " mode S extended squitter");
		++ out;
	}
	if (lti & 0x04)
	{
		if (out > 0)
		{
			list_text (2, ";");
		}
		list_text (2, " UAT");
		++ out;
	}
	if (lti & 0x02)
	{
		if (out > 0)
		{
			list_text (2, ";");
		}
		list_text (2, " VDL mode 4");
		++ out;
	}
	if (lti & 0x01)
	{
		if (out > 0)
		{
			list_text (2, ";");
		}
		list_text (2, " other technology");
		++ out;
	}
	list_text (2, "\n");
#endif /* LISTER */

                   /* Store this information: */
	adsb.link_technology.present = TRUE;
	adsb.link_technology.value = lti;

				   /* Set the return code: */
	ret = RC_OKAY;

	return ret;
}

/*----------------------------------------------------------------------------*/
/* proc_i221_220   -- Process I221/220 data item                              */
/*----------------------------------------------------------------------------*/

 static
 t_Retc proc_i221_220 (t_Ui16 length, t_Byte *buffer, t_Ui16 *pos_ptr)
{
	t_Byte df1;    /* Data field octet 1 */
#if LISTER
	t_Byte df2;    /* Data field octet 2 */
	t_Byte df3;    /* Data field octet 3 */
#endif /* LISTER */
	t_Byte len;    /* Length of data field */
	int inx;       /* Auxiliary */
	t_Ui16 pos;    /* Position within buffer */
	t_Retc ret;    /* Return code */
	t_Byte sf1_present;
                   /* Subfield 1 present (WS) */
	t_Byte sf2_present;
                   /* Subfield 2 present (WD) */
	t_Byte sf3_present;
                   /* Subfield 3 present (TMP) */
	t_Byte sf4_present;
                   /* Subfield 4 present (TRB) */
#if LISTER
	t_Si16 tmp;    /* Temperature; 0.25 degrees Celsius */
	t_Byte trb;    /* Turbulence */
	t_Ui16 wd;     /* Wind direction; degrees */
	t_Ui16 ws;     /* Wind speed; knots */
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

                   /* Determine expected length of data field: */
	len = 1 +
          2 * ord (sf1_present) +
          2 * ord (sf2_present) +
          2 * ord (sf3_present) +
          ord (sf4_present);

                   /* Check against buffer length: */
	if (pos + len > length)
	{
		error_msg ("Invalid buffer length (I221/220)");
		goto done;
	}

#if LISTER
                   /* List raw data field: */
	list_buffer (1, ";  I221/220:", len, buffer + pos);
#endif /* LISTER */

                   /* Extract secondary subfields: */
	inx = 1;
	if (sf1_present)
	{
#if LISTER
		df2 = buffer[pos + inx];
		df3 = buffer[pos + inx + 1];

		ws = make_ui16 (df2, df3);
#endif /* LISTER */

		inx += 2;
	}
	if (sf2_present)
	{
#if LISTER
		df2 = buffer[pos + inx];
		df3 = buffer[pos + inx + 1];

		wd = make_ui16 (df2, df3);
#endif /* LISTER */

		inx += 2;
	}
	if (sf3_present)
	{
#if LISTER
		df2 = buffer[pos + inx];
		df3 = buffer[pos + inx + 1];

		tmp = make_si16 (df2, df3);
#endif /* LISTER */

		inx += 2;
	}
	if (sf4_present)
	{
#if LISTER
		trb = buffer[pos + inx];
#endif /* LISTER */

		++ inx;
	}
	Assert (inx == len, "Invalid structure decoding");

#if LISTER
                   /* List data field: */
	list_text (2, ";  Met Information:");
	list_text (2, "\n");
	if (sf1_present)
	{
		list_text (2, ";   Wind speed: ");
		list_text (2, M_FMT_UI16 " kts", ws);
		list_text (2, "\n");
	}
	if (sf2_present)
	{
		list_text (2, ";   Wind direction: ");
		list_text (2, M_FMT_UI16 " deg", wd);
		list_text (2, "\n");
	}
	if (sf3_present)
	{
		list_text (2, ";   Temperature: ");
		list_text (2, M_FMT_SI16 " (%.2f °C)", tmp, 0.25 * tmp);
		list_text (2, "\n");
	}
	if (sf4_present)
	{
		list_text (2, ";   Turbulence: ");
		list_text (2, M_FMT_BYTE, trb);
		list_text (2, "\n");
	}
#endif /* LISTER */

                   /* Store this information: */
	/* tba ... */

                   /* Set position: */
	*pos_ptr = pos + len;

                   /* Set the return code: */
	ret = RC_OKAY;

	done:          /* We are done */
	return ret;
}

/*----------------------------------------------------------------------------*/
/* proc_i221_230   -- Process I221/230 data item                              */
/*----------------------------------------------------------------------------*/

 static
 t_Retc proc_i221_230 (t_Ui16 length, t_Byte *buffer)
{
#if LISTER
	t_Byte df1;    /* Data field octet 1 */
	t_Byte df2;    /* Data field octet 2 */
	t_Si16 ra;     /* Roll angle; 0.01 degrees */
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
	ra = make_si16 (df1, df2);

				   /* List data field: */
	list_text (2, ";  Roll Angle:");
	list_text (2, " " M_FMT_SI16 " (%.2f deg)", ra, 0.01 * ra);
	list_text (2, "\n");
#endif /* LISTER */

                   /* Store this information: */
	/* tba ... */

				   /* Set the return code: */
	ret = RC_OKAY;

	return ret;
}

/*----------------------------------------------------------------------------*/
/* proc_i221_ref   -- Process REF indicator data item                         */
/*----------------------------------------------------------------------------*/

 static
 t_Retc proc_i221_ref (t_Ui16 length, t_Byte *buffer, t_Ui16 *pos_ptr)
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
		error_msg ("Invalid buffer length (I221/RE)");
		goto done;
	}

#if LISTER
				   /* List raw data field: */
	list_buffer (1, ";  I221/RE :", len, buffer + pos);
#endif /* LISTER */

				   /* Set position: */
	*pos_ptr = pos + len;

                   /* Set the return code: */
	ret = RC_OKAY;

	done:          /* We are done */
	return ret;
}

/*----------------------------------------------------------------------------*/
/* proc_i221_spf   -- Process SPF indicator data item                         */
/*----------------------------------------------------------------------------*/

 static
 t_Retc proc_i221_spf (t_Ui16 length, t_Byte *buffer, t_Ui16 *pos_ptr)
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
		error_msg ("Invalid buffer length (I221/SP)");
		goto done;
	}

#if LISTER
				   /* List raw data field: */
	list_buffer (1, ";  I221/SP :", len, buffer + pos);
#endif /* LISTER */

				   /* Set position: */
	*pos_ptr = pos + len;

				   /* Set the return code: */
	ret = RC_OKAY;

	done:          /* We are done */
	return ret;
}
/* end-of-file */
