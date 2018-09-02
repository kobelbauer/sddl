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
/* File:           src/astx_020.cpp                                           */
/* Contents:       Process ASTERIX category 020 data block                    */
/* Author(s):      kb                                                         */
/* Last change:    2018-04-27                                                 */
/*----------------------------------------------------------------------------*/

/* Reference document:
   -------------------

   EUROCONTROL Standard Document for Surveillance Data Exchange
   Part 14 : Category 020 - Multilateration Target Reports
   SUR.ET1.ST05.2000-STD-14-02

   Edition: 1.0
   Edition date: November 2005
   Status: Proposed Issue

   Edition: 1.2
   Edition date: April 2007
   Status: Released Issue

   Edition: 1.5
   Edition date: April 2008
   Status: Released Issue

   EUROCONTROL Standard Document for Surveillance Data Exchange
   Part 14 / Category 020 - Appendix A
   Coding Rules for "Reserved Expansion Field"
   SUR.ET1.ST05.2000-STD-14-02

   Edition: 1.2
   Edition date: April 2010
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

#define M_MAX_FRN 28
				   /* Maximum field reference number */
#define M_MAX_FSPEC_LENGTH 4
				   /* Maximum fields specification length
					  for ASTERIX category 020 */

#define SSC_HACK 1
                   /* Special fix for SSC software bug */

                   /* Local constant texts: */
                   /* --------------------- */

#if LISTER
static const char* vfi_texts[17] =
                   /* Texts for vehicle fleet identification in I020/300 */
{
	"unknown",
	"ATC equipment maintenance",
	"airport maintenance",
	"fire",
	"bird scarer",
	"snow plough",
	"runway sweeper",
	"emergency",
	"police",
	"bus",
	"tug (push/tow)",
	"grass cutter",
	"fuel",
	"baggage",
	"catering",
	"aircraft maintenance",
	"flyco (follow me)"
};
static const char *wec_texts[19] =
                   /* Texts for warning/error conditions in I020/030 */
{
	"Not defined; never used",
	"Multipath reply (reflection)",
	"",
	"Split plot",
	"",
	"",
	"",
	"",
	"",
	"",
	"Phantom SSR plot",
	"Non-matching mode 3/A code",
	"Mode C code / mode S altitude code abnormal value",
	"",
	"",
	"Transponder anomaly detected",
	"Duplicated or illegal mode S aircraft address",
	"Mode S error correction applied",
	"Undecodable mode C code / mode S altitude code"
};
#endif /* LISTER */

                   /* Local data types: */
                   /* ----------------- */

typedef struct
{
	t_Byte age;
                   /* BDS register age; 0.1 seconds */
	t_Byte bds_register;
                   /* BDS register */
	t_Bool defined;
                   /* BDS register age defined */
} t_BDS_Register_Age;

                   /* Local variables: */
                   /* ---------------- */

static t_Data_Item_Desc desc_i020_010;
                   /* Description of data item I020/010 */
static t_Data_Item_Desc desc_i020_020;
                   /* Description of data item I020/020 */
static t_Data_Item_Desc desc_i020_030;
                   /* Description of data item I020/030 */
static t_Data_Item_Desc desc_i020_041;
                   /* Description of data item I020/041 */
static t_Data_Item_Desc desc_i020_042;
                   /* Description of data item I020/042 */
static t_Data_Item_Desc desc_i020_050;
                   /* Description of data item I020/050 */
static t_Data_Item_Desc desc_i020_055;
                   /* Description of data item I020/055 */
static t_Data_Item_Desc desc_i020_070;
                   /* Description of data item I020/070 */
static t_Data_Item_Desc desc_i020_090;
                   /* Description of data item I020/090 */
static t_Data_Item_Desc desc_i020_100;
                   /* Description of data item I020/100 */
static t_Data_Item_Desc desc_i020_105;
                   /* Description of data item I020/105 */
static t_Data_Item_Desc desc_i020_110;
                   /* Description of data item I020/110 */
static t_Data_Item_Desc desc_i020_140;
                   /* Description of data item I020/140 */
static t_Data_Item_Desc desc_i020_161;
                   /* Description of data item I020/161 */
static t_Data_Item_Desc desc_i020_170;
                   /* Description of data item I020/170 */
static t_Data_Item_Desc desc_i020_202;
                   /* Description of data item I020/202 */
static t_Data_Item_Desc desc_i020_210;
                   /* Description of data item I020/210 */
static t_Data_Item_Desc desc_i020_220;
                   /* Description of data item I020/220 */
static t_Data_Item_Desc desc_i020_230;
                   /* Description of data item I020/230 */
static t_Data_Item_Desc desc_i020_245;
                   /* Description of data item I020/245 */
static t_Data_Item_Desc desc_i020_250;
                   /* Description of data item I020/250 */
static t_Data_Item_Desc desc_i020_260;
                   /* Description of data item I020/260 */
static t_Data_Item_Desc desc_i020_300;
                   /* Description of data item I020/300 */
static t_Data_Item_Desc desc_i020_310;
                   /* Description of data item I020/310 */
static t_Data_Item_Desc desc_i020_400;
                   /* Description of data item I020/400 */
static t_Data_Item_Desc desc_i020_500;
                   /* Description of data item I020/500 */
static t_Data_Item_Desc desc_i020_ref;
                   /* Description of RE data item */
static t_Data_Item_Desc desc_i020_spf;
                   /* Description of SP data item */
static t_Bool initiated = FALSE;
				   /* Package initiated flag */
static t_Ui16 reference_vsn = 6;
                   /* Current version of reference document */
                   /* Implemented editions:
                      1 ... 1.0 (November 2005)
                     -2 ... 1.1 (April 2006)
                      3 ... 1.2 (April 2007)
                     -4 ... 1.3
                     -5 ... 1.4
                      6 ... 1.5 (April 2008)
                   */
static t_Data_Item_Desc *std_uap[M_MAX_FRN + 1];
				   /* Standard User Application Profile */

				   /* Local functions: */
				   /* ---------------- */

static void init_desc (void);
                   /* Initiate data item descriptions */
static void load_std_uap (void);
                   /* Load standard User Application Profile (UAP) */
static t_Retc proc_i020_010 (t_Ui16 length, t_Byte *buffer);
                   /* Process I020/010 data item */
static t_Retc proc_i020_020 (t_Ui16 length, t_Byte *buffer);
                   /* Process I020/020 data item */
static t_Retc proc_i020_030 (t_Ui16 length, t_Byte *buffer);
                   /* Process I020/030 data item */
static t_Retc proc_i020_041 (t_Ui16 length, t_Byte *buffer);
                   /* Process I020/041 data item */
static t_Retc proc_i020_042 (t_Ui16 length, t_Byte *buffer);
                   /* Process I020/042 data item */
static t_Retc proc_i020_050 (t_Ui16 length, t_Byte *buffer);
                   /* Process I020/050 data item */
static t_Retc proc_i020_055 (t_Ui16 length, t_Byte *buffer);
                   /* Process I020/055 data item */
static t_Retc proc_i020_070 (t_Ui16 length, t_Byte *buffer);
                   /* Process I020/070 data item */
static t_Retc proc_i020_090 (t_Ui16 length, t_Byte *buffer);
                   /* Process I020/090 data item */
static t_Retc proc_i020_100 (t_Ui16 length, t_Byte *buffer);
                   /* Process I020/100 data item */
static t_Retc proc_i020_105 (t_Ui16 length, t_Byte *buffer);
                   /* Process I020/105 data item */
static t_Retc proc_i020_110 (t_Ui16 length, t_Byte *buffer);
                   /* Process I020/110 data item */
static t_Retc proc_i020_140 (t_Ui16 length, t_Byte *buffer);
                   /* Process I020/140 data item */
static t_Retc proc_i020_161 (t_Ui16 length, t_Byte *buffer);
                   /* Process I020/161 data item */
static t_Retc proc_i020_170 (t_Ui16 length, t_Byte *buffer);
                   /* Process I020/170 data item */
static t_Retc proc_i020_202 (t_Ui16 length, t_Byte *buffer);
                   /* Process I020/202 data item */
static t_Retc proc_i020_210 (t_Ui16 length, t_Byte *buffer);
                   /* Process I020/210 data item */
static t_Retc proc_i020_220 (t_Ui16 length, t_Byte *buffer);
                   /* Process I020/220 data item */
static t_Retc proc_i020_230 (t_Ui16 length, t_Byte *buffer);
                   /* Process I020/230 data item */
static t_Retc proc_i020_245 (t_Ui16 length, t_Byte *buffer);
                   /* Process I020/245 data item */
static t_Retc proc_i020_250 (t_Ui16 length, t_Byte *buffer);
                   /* Process I020/250 data item */
static t_Retc proc_i020_260 (t_Ui16 length, t_Byte *buffer);
                   /* Process I020/260 data item */
static t_Retc proc_i020_300 (t_Ui16 length, t_Byte *buffer);
                   /* Process I020/300 data item */
static t_Retc proc_i020_310 (t_Ui16 length, t_Byte *buffer);
                   /* Process I020/310 data item */
static t_Retc proc_i020_400 (t_Ui16 length, t_Byte *buffer);
                   /* Process I020/400 data item */
static t_Retc proc_i020_500 (t_Ui16 length, t_Byte *buffer, t_Ui16 *pos_ptr);
                   /* Process I020/500 data item */
static t_Retc proc_i020_ref (t_Ui16 length, t_Byte *buffer, t_Ui16 *pos_ptr);
                   /* Process RE indicator data item */
static t_Retc proc_i020_spf (t_Ui16 length, t_Byte *buffer, t_Ui16 *pos_ptr);
                   /* Process SP indicator data item */

/*----------------------------------------------------------------------------*/
/* astx_020        -- Process ASTERIX category 020 data block                 */
/*----------------------------------------------------------------------------*/

 t_Retc astx_020 (t_Ui16 length, t_Byte *buffer)
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

                   /* Preset multilateration target report: */
		memset (&mlat, 0, sizeof (mlat));

                   /* Set data format: */
		mlat.asterix_category = 20;
		mlat.data_format = e_data_format_asterix;

                   /* This report is not a status message: */
		mlat.is_status_message = FALSE;

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

					lrc = data_item (20, frn, std_uap[frn],
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
			mlat.frame_date.present = TRUE;
			mlat.frame_date.value = frame_date;
		}

                   /* Add frame time, if available: */
		if (frame_time_present)
		{
			mlat.frame_time.present = TRUE;
			mlat.frame_time.value = frame_time;
		}

                   /* Add board/line number, if present: */
		if (current_line_number_defined)
		{
			mlat.line_number = current_line_number;
		}

                   /* Process multilateration target report: */
		lrc = process_mlat (&mlat);
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
                   /* Description of data item I020/010: */
    desc_i020_010.category = 20;
    desc_i020_010.data_item = 10;
    desc_i020_010.item_type = e_fixed_length_data_item;
    desc_i020_010.fixed_length = 2;
    desc_i020_010.proc_fptr = proc_i020_010;
    desc_i020_010.read_fptr = NULL;

                   /* Description of data item I020/020: */
    desc_i020_020.category = 20;
    desc_i020_020.data_item = 20;
    desc_i020_020.item_type = e_variable_length_data_item;
    desc_i020_020.fixed_length = 0;
    desc_i020_020.proc_fptr = proc_i020_020;
    desc_i020_020.read_fptr = NULL;

                   /* Description of data item I020/030: */
    desc_i020_030.category = 20;
    desc_i020_030.data_item = 30;
    desc_i020_030.item_type = e_variable_length_data_item;
    desc_i020_030.fixed_length = 0;
    desc_i020_030.proc_fptr = proc_i020_030;
    desc_i020_030.read_fptr = NULL;

                   /* Description of data item I020/041: */
    desc_i020_041.category = 20;
    desc_i020_041.data_item = 41;
    desc_i020_041.item_type = e_fixed_length_data_item;
    desc_i020_041.fixed_length = 8;
    desc_i020_041.proc_fptr = proc_i020_041;
    desc_i020_041.read_fptr = NULL;

                   /* Description of data item I020/042: */
    desc_i020_042.category = 20;
    desc_i020_042.data_item = 42;
    desc_i020_042.item_type = e_fixed_length_data_item;
    desc_i020_042.fixed_length = 6;
    desc_i020_042.proc_fptr = proc_i020_042;
    desc_i020_042.read_fptr = NULL;

                   /* Description of data item I020/050: */
    desc_i020_050.category = 20;
    desc_i020_050.data_item = 50;
    desc_i020_050.item_type = e_fixed_length_data_item;
    desc_i020_050.fixed_length = 2;
    desc_i020_050.proc_fptr = proc_i020_050;
    desc_i020_050.read_fptr = NULL;

                   /* Description of data item I020/055: */
    desc_i020_055.category = 20;
    desc_i020_055.data_item = 55;
    desc_i020_055.item_type = e_fixed_length_data_item;
    desc_i020_055.fixed_length = 1;
    desc_i020_055.proc_fptr = proc_i020_055;
    desc_i020_055.read_fptr = NULL;

                   /* Description of data item I020/070: */
    desc_i020_070.category = 20;
    desc_i020_070.data_item = 70;
    desc_i020_070.item_type = e_fixed_length_data_item;
    desc_i020_070.fixed_length = 2;
    desc_i020_070.proc_fptr = proc_i020_070;
    desc_i020_070.read_fptr = NULL;

                   /* Description of data item I020/090: */
    desc_i020_090.category = 20;
    desc_i020_090.data_item = 90;
    desc_i020_090.item_type = e_fixed_length_data_item;
    desc_i020_090.fixed_length = 2;
    desc_i020_090.proc_fptr = proc_i020_090;
    desc_i020_090.read_fptr = NULL;

                   /* Description of data item I020/100: */
    desc_i020_100.category = 20;
    desc_i020_100.data_item = 100;
    desc_i020_100.item_type = e_fixed_length_data_item;
    desc_i020_100.fixed_length = 4;
    desc_i020_100.proc_fptr = proc_i020_100;
    desc_i020_100.read_fptr = NULL;

                   /* Description of data item I020/105: */
    desc_i020_105.category = 20;
    desc_i020_105.data_item = 105;
    desc_i020_105.item_type = e_fixed_length_data_item;
    desc_i020_105.fixed_length = 2;
    desc_i020_105.proc_fptr = proc_i020_105;
    desc_i020_105.read_fptr = NULL;

                   /* Description of data item I020/110: */
    desc_i020_110.category = 20;
    desc_i020_110.data_item = 110;
    desc_i020_110.item_type = e_fixed_length_data_item;
    desc_i020_110.fixed_length = 2;
    desc_i020_110.proc_fptr = proc_i020_110;
    desc_i020_110.read_fptr = NULL;

                   /* Description of data item I020/140: */
    desc_i020_140.category = 20;
    desc_i020_140.data_item = 140;
    desc_i020_140.item_type = e_fixed_length_data_item;
    desc_i020_140.fixed_length = 3;
    desc_i020_140.proc_fptr = proc_i020_140;
    desc_i020_140.read_fptr = NULL;

                   /* Description of data item I020/161: */
    desc_i020_161.category = 20;
    desc_i020_161.data_item = 161;
    desc_i020_161.item_type = e_fixed_length_data_item;
    desc_i020_161.fixed_length = 2;
    desc_i020_161.proc_fptr = proc_i020_161;
    desc_i020_161.read_fptr = NULL;

                   /* Description of data item I020/170: */
    desc_i020_170.category = 20;
    desc_i020_170.data_item = 170;
    desc_i020_170.item_type = e_variable_length_data_item;
    desc_i020_170.fixed_length = 0;
    desc_i020_170.proc_fptr = proc_i020_170;
    desc_i020_170.read_fptr = NULL;

                   /* Description of data item I020/202: */
    desc_i020_202.category = 20;
    desc_i020_202.data_item = 202;
    desc_i020_202.item_type = e_fixed_length_data_item;
    desc_i020_202.fixed_length = 4;
    desc_i020_202.proc_fptr = proc_i020_202;
    desc_i020_202.read_fptr = NULL;

                   /* Description of data item I020/210: */
    desc_i020_210.category = 20;
    desc_i020_210.data_item = 210;
    desc_i020_210.item_type = e_fixed_length_data_item;
    desc_i020_210.fixed_length = 2;
    desc_i020_210.proc_fptr = proc_i020_210;
    desc_i020_210.read_fptr = NULL;

                   /* Description of data item I020/220: */
    desc_i020_220.category = 20;
    desc_i020_220.data_item = 220;
    desc_i020_220.item_type = e_fixed_length_data_item;
    desc_i020_220.fixed_length = 3;
    desc_i020_220.proc_fptr = proc_i020_220;
    desc_i020_220.read_fptr = NULL;

                   /* Description of data item I020/230: */
    desc_i020_230.category = 20;
    desc_i020_230.data_item = 230;
    desc_i020_230.item_type = e_fixed_length_data_item;
    desc_i020_230.fixed_length = 2;
    desc_i020_230.proc_fptr = proc_i020_230;
    desc_i020_230.read_fptr = NULL;

                   /* Description of data item I020/245: */
    desc_i020_245.category = 20;
    desc_i020_245.data_item = 245;
    desc_i020_245.item_type = e_fixed_length_data_item;
    desc_i020_245.fixed_length = 7;
    desc_i020_245.proc_fptr = proc_i020_245;
    desc_i020_245.read_fptr = NULL;

                   /* Description of data item I020/250: */
    desc_i020_250.category = 20;
    desc_i020_250.data_item = 250;
    desc_i020_250.item_type = e_repetitive_data_item;
    desc_i020_250.fixed_length = 8;
    desc_i020_250.proc_fptr = proc_i020_250;
    desc_i020_250.read_fptr = NULL;

                   /* Description of data item I020/260: */
    desc_i020_260.category = 20;
    desc_i020_260.data_item = 260;
    desc_i020_260.item_type = e_fixed_length_data_item;
    desc_i020_260.fixed_length = 7;
    desc_i020_260.proc_fptr = proc_i020_260;
    desc_i020_260.read_fptr = NULL;

                   /* Description of data item I020/300: */
    desc_i020_300.category = 20;
    desc_i020_300.data_item = 300;
    desc_i020_300.item_type = e_fixed_length_data_item;
    desc_i020_300.fixed_length = 1;
    desc_i020_300.proc_fptr = proc_i020_300;
    desc_i020_300.read_fptr = NULL;

                   /* Description of data item I020/310: */
    desc_i020_310.category = 20;
    desc_i020_310.data_item = 310;
    desc_i020_310.item_type = e_fixed_length_data_item;
    desc_i020_310.fixed_length = 1;
    desc_i020_310.proc_fptr = proc_i020_310;
    desc_i020_310.read_fptr = NULL;

                   /* Description of data item I020/400: */
    desc_i020_400.category = 20;
    desc_i020_400.data_item = 400;
    desc_i020_400.item_type = e_repetitive_data_item;
    desc_i020_400.fixed_length = 1;
    desc_i020_400.proc_fptr = proc_i020_400;
    desc_i020_400.read_fptr = NULL;

                   /* Description of data item I020/500: */
    desc_i020_500.category = 20;
    desc_i020_500.data_item = 500;
    desc_i020_500.item_type = e_immediate_data_item;
    desc_i020_500.fixed_length = 0;
    desc_i020_500.proc_fptr = NULL;
    desc_i020_500.read_fptr = proc_i020_500;

                   /* Description of RE data item: */
    desc_i020_ref.category = 20;
    desc_i020_ref.data_item = M_REF_INDICATOR;
    desc_i020_ref.item_type = e_immediate_data_item;
    desc_i020_ref.fixed_length = 0;
    desc_i020_ref.proc_fptr = NULL;
    desc_i020_ref.read_fptr = proc_i020_ref;

                   /* Description of SP data item: */
    desc_i020_spf.category = 20;
    desc_i020_spf.data_item = M_SPF_INDICATOR;
    desc_i020_spf.item_type = e_immediate_data_item;
    desc_i020_spf.fixed_length = 0;
    desc_i020_spf.proc_fptr = NULL;
    desc_i020_spf.read_fptr = proc_i020_spf;

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

	std_uap[1] = &desc_i020_010;
	std_uap[2] = &desc_i020_020;
	std_uap[3] = &desc_i020_140;
	std_uap[4] = &desc_i020_041;
	std_uap[5] = &desc_i020_042;
	std_uap[6] = &desc_i020_161;
	std_uap[7] = &desc_i020_170;

	std_uap[8] = &desc_i020_070;
	std_uap[9] = &desc_i020_202;
	std_uap[10] = &desc_i020_090;
	std_uap[11] = &desc_i020_100;
	std_uap[12] = &desc_i020_220;
	std_uap[13] = &desc_i020_245;
	std_uap[14] = &desc_i020_110;

	std_uap[15] = &desc_i020_105;
	std_uap[16] = &desc_i020_210;
	std_uap[17] = &desc_i020_300;
	std_uap[18] = &desc_i020_310;
	std_uap[19] = &desc_i020_500;
	std_uap[20] = &desc_i020_400;
	std_uap[21] = &desc_i020_250;

	std_uap[22] = &desc_i020_230;
	std_uap[23] = &desc_i020_260;
	std_uap[24] = &desc_i020_030;
	std_uap[25] = &desc_i020_055;
	std_uap[26] = &desc_i020_050;
	std_uap[27] = &desc_i020_ref;
	std_uap[28] = &desc_i020_spf;

	return;
}

/*----------------------------------------------------------------------------*/
/* proc_i020_010   -- Process I020/010 data item                              */
/*----------------------------------------------------------------------------*/

 static
 t_Retc proc_i020_010 (t_Ui16 length, t_Byte *buffer)
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
	mlat.data_source_identifier.present = TRUE;
	mlat.data_source_identifier.value = dsi;

				   /* Set the return code: */
	ret = RC_OKAY;

	return ret;
}

/*----------------------------------------------------------------------------*/
/* proc_i020_020   -- Process I020/020 data item                              */
/*----------------------------------------------------------------------------*/

 static
 t_Retc proc_i020_020 (t_Ui16 length, t_Byte *buffer)
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
			t_Ui16 cnt;
			t_Byte typ;

			if (reference_vsn == 3)
			{
				typ = (dfo >> 2) & 0x3f;
			}
			else if (reference_vsn == 6)
			{
				typ = (dfo >> 1) & 0x7f;
			}
			else
			{
				Assert (FALSE, "Unexpected reference version");
			}
			list_text (2, " TYP=" M_FMT_BYTE, typ);
			cnt = 0;
			if (dfo & 0x80)
			{
				if (cnt == 0)
				{
					list_text (2, " (");
				}
				else
				{
					list_text (2, ", ");
				}
				list_text (2, "SSR");
				++ cnt;
			}
			if (dfo & 0x40)
			{
				if (cnt == 0)
				{
					list_text (2, " (");
				}
				else
				{
					list_text (2, ", ");
				}
				list_text (2, "MS");
				++ cnt;
			}
			if (dfo & 0x20)
			{
				if (cnt == 0)
				{
					list_text (2, " (");
				}
				else
				{
					list_text (2, ", ");
				}
				list_text (2, "HF");
				++ cnt;
			}
			if (dfo & 0x10)
			{
				if (cnt == 0)
				{
					list_text (2, " (");
				}
				else
				{
					list_text (2, ", ");
				}
				list_text (2, "VDL4");
				++ cnt;
			}
			if (dfo & 0x08)
			{
				if (cnt == 0)
				{
					list_text (2, " (");
				}
				else
				{
					list_text (2, ", ");
				}
				list_text (2, "UAT");
				++ cnt;
			}
			if (dfo & 0x04)
			{
				if (cnt == 0)
				{
					list_text (2, " (");
				}
				else
				{
					list_text (2, ", ");
				}
				list_text (2, "DME");
				++ cnt;
			}
			if (reference_vsn == 6)
			{
				if (dfo & 0x02)
				{
					if (cnt == 0)
					{
						list_text (2, " (");
					}
					else
					{
						list_text (2, ", ");
					}
					list_text (2, "OT");
					++ cnt;
				}
			}
			if (cnt > 0)
			{
				list_text (2, ")");
			}
		}
		else if (ix == 1)
		{
			list_text (2, ";");
			if (dfo & 0x80)
			{
				list_text (2, " RAB");
			}

			if (dfo & 0x40)
			{
				list_text (2, " SPI");
			}

			if (dfo & 0x20)
			{
				list_text (2, " CHN=2");
			}
			else
			{
				list_text (2, " CHN=1");
			}

			if (dfo & 0x10)
			{
				list_text (2, " GBS");
			}

			if (dfo & 0x08)
			{
				list_text (2, " CRT");
			}

			if (dfo & 0x04)
			{
				list_text (2, " SIM");
			}

			if (dfo & 0x02)
			{
				list_text (2, " TST");
			}
		}
	}
	list_text (2, "\n");
#endif /* LISTER */

				   /* Evaluate and store this information: */
	for (ix = 0; ix < length; ix ++)
	{
    	dfo = buffer[ix];

		if (ix == 0)
		{
			mlat.detection_type.reported_from_mlt = e_is_true;
			mlat.detection_type.present = TRUE;

			mlat.detected_by_ssr = M_TRES (dfo & 0x80);
			mlat.detected_by_mds = M_TRES (dfo & 0x40);
			mlat.detected_by_hf  = M_TRES (dfo & 0x20);
			mlat.detected_by_vdl = M_TRES (dfo & 0x10);
			mlat.detected_by_uat = M_TRES (dfo & 0x08);
			mlat.detected_by_dme = M_TRES (dfo & 0x04);

			if (reference_vsn == 6)
			{
				mlat.detected_by_ot = M_TRES (dfo & 0x02);
			}
		}
		else if (ix == 1)
		{
			mlat.detection_type.from_fixed_field_transponder =
				M_TRES (dfo & 0x80);

			mlat.special_position_indication = M_TRES (dfo & 0x04);

			/* tba ... 0x20 (chain number) */

			if (dfo & 0x10)
			{
				mlat.ground_bit_set = TRUE;
			}

			if (dfo & 0x08)
			{
				mlat.corrupted_replies = TRUE;
			}

			mlat.detection_type.simulated = M_TRES (dfo & 0x04);

			mlat.detection_type.test_target = M_TRES (dfo & 0x02);
		}
	}

				   /* Set the return code: */
	ret = RC_OKAY;

	return ret;
}

/*----------------------------------------------------------------------------*/
/* proc_i020_030   -- Process I020/030 data item                              */
/*----------------------------------------------------------------------------*/

 static
 t_Retc proc_i020_030 (t_Ui16 length, t_Byte *buffer)
{
	t_Byte dfo;    /* Data field octet */
	int inx;       /* Auxiliary */
	int ix;        /* Auxiliary */
	t_Retc ret;    /* Return code */
	t_Byte wec;    /* Warning/error condition */

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

		wec = (dfo >> 1) & 0x1f;

		list_text (2, " " M_FMT_BYTE, wec);
		if (1 <= wec && wec <= 18)
		{
			list_text (2, " (%s)", wec_texts[wec]);
		}
	}
	list_text (2, "\n");
#endif /* LISTER */

				   /* Store this information: */
	inx = 0;
	for (ix = 0; ix < length; ix ++)
	{
		dfo = buffer[ix];

		wec = (dfo >> 1) & 0x7f;

		Assert (inx < M_MAX_NUMBER_OF_WEC,
                "Too many warning/error conditions");

		mlat.wec_list.list[inx] = wec;
		++ inx;
	}
	mlat.wec_list.count =
		(inx < M_MAX_NUMBER_OF_WEC ? inx : M_MAX_NUMBER_OF_WEC);
	mlat.wec_list.present = TRUE;

				   /* Set the return code: */
	ret = RC_OKAY;

	return ret;
}

/*----------------------------------------------------------------------------*/
/* proc_i020_041   -- Process I020/041 data item                              */
/*----------------------------------------------------------------------------*/

 static
 t_Retc proc_i020_041 (t_Ui16 length, t_Byte *buffer)
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
	if (reference_vsn == 1)
	{
		list_text (2, ";  Calculated Position in WGS-84 Co-ordinates:\n");
	}
	else if (reference_vsn == 3 || reference_vsn == 6)
	{
		list_text (2, ";  Position in WGS-84 Coordinates:\n");
	}
	list_text (2, ";   lat=" M_FMT_SI32 " (%s);", lat, lat_text (latf));
	list_text (2, " lon=" M_FMT_SI32 " (%s) ", lon, lon_text (lonf));
	list_text (2, "\n");
#endif /* LISTER */

				   /* Store this information: */
	mlat.wgs84_position.present = TRUE;
	mlat.wgs84_position.value_lat = M_DEG2RAD * latf;
	mlat.wgs84_position.value_lon = M_DEG2RAD * lonf;
                   /* Values in radians */

				   /* Set the return code: */
	ret = RC_OKAY;

	return ret;
}

/*----------------------------------------------------------------------------*/
/* proc_i020_042   -- Process I020/042 data item                              */
/*----------------------------------------------------------------------------*/

 static
 t_Retc proc_i020_042 (t_Ui16 length, t_Byte *buffer)
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
	list_text (2, ";  Position in Cartesian coordinates:\n");
	list_text (2, ";   x=" M_FMT_SI32 " (%.1f mtr =%.3f nmi);",
                  x, 0.5 * x, (0.5 / M_NMI2MTR) * x);
	list_text (2, " y=" M_FMT_SI32 " (%.1f mtr =%.3f nmi)",
                  y, 0.5 * y, (0.5 / M_NMI2MTR) * y);
	list_text (2, "\n");
#endif /* LISTER */

				   /* Store this information: */
	mlat.computed_position.present = TRUE;
	mlat.computed_position.value_x = 0.5 * x;
	mlat.computed_position.value_y = 0.5 * y;

				   /* Set the return code: */
	ret = RC_OKAY;

	return ret;
}

/*----------------------------------------------------------------------------*/
/* proc_i020_050   -- Process I020/050 data item                              */
/*----------------------------------------------------------------------------*/

 static
 t_Retc proc_i020_050 (t_Ui16 length, t_Byte *buffer)
{
	t_Byte df1;    /* Data field octet 1 */
	t_Byte df2;    /* Data field octet 2 */
	t_Ui16 m2c;    /* Mode 2 code information */
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
	m2c = make_ui16 (df1, df2);

#if LISTER
				   /* List data field: */
	list_text (2, ";  Mode 2 Code:");
	list_text (2, " v=" M_FMT_BYTE ";", (m2c >> 15) & 0x01);
	list_text (2, " g=" M_FMT_BYTE ";", (m2c >> 14) & 0x01);
	list_text (2, " l=" M_FMT_BYTE ";", (m2c >> 13) & 0x01);
	list_text (2, " code=" M_FMT_CODE, m2c & 0x0fff);
	list_text (2, "\n");
#endif /* LISTER */

				   /* Store this information: */
	mlat.mode_2_info.code = m2c & 0x0fff;
	mlat.mode_2_info.code_garbled = M_TRES ((m2c >> 14) & 0x01);
	mlat.mode_2_info.code_invalid = M_TRES ((m2c >> 15) & 0x01);
	mlat.mode_2_info.code_smoothed = M_TRES ((m2c >> 13) & 0x01);
	mlat.mode_2_info.present = TRUE;

				   /* Set the return code: */
	ret = RC_OKAY;

	return ret;
}

/*----------------------------------------------------------------------------*/
/* proc_i020_055   -- Process I020/055 data item                              */
/*----------------------------------------------------------------------------*/

 static
 t_Retc proc_i020_055 (t_Ui16 length, t_Byte *buffer)
{
	t_Byte df1;    /* Data field octet 1 */
	t_Ui16 m1c;    /* Mode 1 code information */
	t_Retc ret;    /* Return code */

				   /* Preset the return code: */
	ret = RC_FAIL;

				   /* Check parameters: */
	Assert (length == 1, "Invalid parameter");
	Assert (buffer != NULL, "Invalid parameter");

				   /* Extract octets: */
	df1 = buffer[0];

				   /* Extract information: */
	m1c = 0x0000;
	m1c |= (t_Ui16) ((df1 & 0xe0) << 8);
	m1c |= (t_Ui16) ((df1 & 0x1c) << 7);
	m1c |= (t_Ui16) ((df1 & 0x03) << 6);
                   /* Establishing usual sequence of data pulses */
                   /* From V G L A4 A2 A1 B2 B1 to
                      V G L A4 A2 A1 B4 B2 B1 C4 C2 C1 D4 D2 D1
                   */

#if LISTER
				   /* List data field: */
	list_text (2, ";  Mode 1 Code:");
	list_text (2, " v=" M_FMT_BYTE ";", (m1c >> 15) & 0x01);
	list_text (2, " g=" M_FMT_BYTE ";", (m1c >> 14) & 0x01);
	list_text (2, " l=" M_FMT_BYTE ";", (m1c >> 13) & 0x01);
	list_text (2, " code=" M_FMT_CODE, m1c & 0x0fff);
	list_text (2, "\n");
#endif /* LISTER */

				   /* Store this information: */
	mlat.mode_1_info.code = m1c & 0x0fff;
	mlat.mode_1_info.code_garbled = M_TRES ((m1c >> 14) & 0x01);
	mlat.mode_1_info.code_invalid = M_TRES ((m1c >> 15) & 0x01);
	mlat.mode_1_info.code_smoothed = M_TRES ((m1c >> 13) & 0x01);
	mlat.mode_1_info.present = TRUE;

				   /* Set the return codee: */
	ret = RC_OKAY;

	return ret;
}

/*----------------------------------------------------------------------------*/
/* proc_i020_070   -- Process I020/070 data item                              */
/*----------------------------------------------------------------------------*/

 static
 t_Retc proc_i020_070 (t_Ui16 length, t_Byte *buffer)
{
	t_Byte df1;    /* Data field octet 1 */
	t_Byte df2;    /* Data field octet 2 */
	t_Ui16 m3a;    /* Mode 3/A code information */
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
	m3a = make_ui16 (df1, df2);

#if LISTER
				   /* List data field: */
	list_text (2, ";  Mode 3/A Code:");
	list_text (2, " v=" M_FMT_BYTE ";", (m3a >> 15) & 0x01);
	list_text (2, " g=" M_FMT_BYTE ";", (m3a >> 14) & 0x01);
	list_text (2, " l=" M_FMT_BYTE ";", (m3a >> 13) & 0x01);
	list_text (2, " code=" M_FMT_CODE, m3a & 0x0fff);
	list_text (2, "\n");
#endif /* LISTER */

				   /* Store this information: */
	mlat.mode_3a_info.code = m3a & 0x0fff;
	mlat.mode_3a_info.code_garbled = M_TRES ((m3a >> 14) & 0x01);
	mlat.mode_3a_info.code_invalid = M_TRES ((m3a >> 15) & 0x01);
	mlat.mode_3a_info.code_smoothed = M_TRES ((m3a >> 13) & 0x01);
	mlat.mode_3a_info.present = TRUE;

				   /* Set the return codee: */
	ret = RC_OKAY;

	return ret;
}

/*----------------------------------------------------------------------------*/
/* proc_i020_090   -- Process I020/090 data item                              */
/*----------------------------------------------------------------------------*/

 static
 t_Retc proc_i020_090 (t_Ui16 length, t_Byte *buffer)
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
                   /* Negative values in two's complement */
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
	mlat.mode_c_height.garbled = M_TRES (df1 & 0x40);
	mlat.mode_c_height.height_in_error = e_is_false;
	mlat.mode_c_height.in_25_feet = M_TRES (mch & 0x03);
	mlat.mode_c_height.invalid = M_TRES (df1 & 0x80);
	mlat.mode_c_height.present = TRUE;
	mlat.mode_c_height.value = 25.0 * M_FT2MTR * mch;
	mlat.mode_c_height.value_in_feet = 25 * (t_Si32) mch;
                   /* Value in feet */

				   /* Set the return code: */
	ret = RC_OKAY;

	return ret;
}

/*----------------------------------------------------------------------------*/
/* proc_i020_100   -- Process I020/100 data item                              */
/*----------------------------------------------------------------------------*/

 static
 t_Retc proc_i020_100 (t_Ui16 length, t_Byte *buffer)
{
	t_Byte df1;    /* Data field octet 1 */
	t_Byte df2;    /* Data field octet 2 */
	t_Byte df3;    /* Data field octet 3 */
	t_Byte df4;    /* Data field octet 4 */
	t_Retc ret;    /* Return code */
	t_Ui16 u1;     /* Auxiliary */
	t_Ui16 u2;     /* Auxiliary */

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
	u1 = make_ui16 (df1, df2);
	u2 = make_ui16 (df3, df4);

#if LISTER
				   /* List data field: */
	list_text (2, ";  Mode C Code:");
	list_text (2, " v=" M_FMT_BYTE ";", (df1 >> 7) & 0x01);
	list_text (2, " g=" M_FMT_BYTE ";", (df1 >> 6) & 0x01);
	list_text (2, " code=" M_FMT_CODE ";", u1 & 0x0fff);
	list_text (2, " conf=" M_FMT_CODE, u2 & 0x0fff);
	list_text (2, "\n");
#endif /* LISTER */

				   /* Store this information: */
	mlat.mode_c_info.code = u1 & 0x0fff;
	mlat.mode_c_info.code_confidence = u2 & 0x0fff;
	mlat.mode_c_info.code_garbled = (df1 >> 6) & 0x01;
	mlat.mode_c_info.code_invalid = (df1 >> 7) & 0x01;
	mlat.mode_c_info.present = TRUE;

				   /* Set the return code: */
	ret = RC_OKAY;

	return ret;
}

/*----------------------------------------------------------------------------*/
/* proc_i020_105   -- Process I020/105 data item                              */
/*----------------------------------------------------------------------------*/

 static
 t_Retc proc_i020_105 (t_Ui16 length, t_Byte *buffer)
{
	t_Byte df1;    /* Data field octet 1 */
	t_Byte df2;    /* Data field octet 2 */
	t_Si16 ga;     /* Geometric altitude; 6.25 feet */
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
	ga = make_si16 (df1, df2);
                   /* Negative values in two's complement */

#if LISTER
				   /* List data field: */
	if (reference_vsn == 1 || reference_vsn == 3)
	{
		list_text (2, ";  Geometric Altitude (WGS-84):");
	}
	else if (reference_vsn == 6)
	{
		list_text (2, ";  Geometric Height (WGS-84):");
	}
	else
	{
		Assert (FALSE, "Unexpected reference version");
	}
	list_text (2, " " M_FMT_SI16 " (%.2f feet)", ga, 6.25 * ga);
	list_text (2, "\n");
#endif /* LISTER */

				   /* Store this information: */
	mlat.geometric_altitude.present = TRUE;
	mlat.geometric_altitude.value = 6.25 * M_FT2MTR * ga;

				   /* Set the return code: */
	ret = RC_OKAY;

	return ret;
}

/*----------------------------------------------------------------------------*/
/* proc_i020_110   -- Process I020/110 data item                              */
/*----------------------------------------------------------------------------*/

 static
 t_Retc proc_i020_110 (t_Ui16 length, t_Byte *buffer)
{
	t_Byte df1;    /* Data field octet 1 */
	t_Byte df2;    /* Data field octet 2 */
	t_Si16 hgt;    /* Measured height; 6.25 feet */
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
                   /* Negative values in two's complement */

#if LISTER
				   /* List data field: */
	list_text (2, ";  Measured Height:");
	list_text (2, " " M_FMT_SI16 " (%.2f feet)", hgt, 6.25 * hgt);
	list_text (2, "\n");
#endif /* LISTER */

				   /* Store this information: */
	mlat.measured_height.present = TRUE;
	mlat.measured_height.value = 6.25 * M_FT2MTR * hgt;

				   /* Set the return code: */
	ret = RC_OKAY;

	return ret;
}

/*----------------------------------------------------------------------------*/
/* proc_i020_140   -- Process I020/140 data item                              */
/*----------------------------------------------------------------------------*/

 static
 t_Retc proc_i020_140 (t_Ui16 length, t_Byte *buffer)
{
	t_Secs atm;    /* Absolute time; seconds */
	t_Byte df1;    /* Data field octet 1 */
	t_Byte df2;    /* Data field octet 2 */
	t_Byte df3;    /* Data field octet 3 */
	t_Retc ret;    /* Return code */
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
	atm = (1.0 / 128.0) * tod;

#if LISTER
				   /* List data field: */
	list_text (2, ";  Time of Day:");
	list_text (2, " " M_FMT_ATOD " (" M_FMT_UI32 "; %s UTC)",
                  tod, tod, utc_text (atm));
	list_text (2, "\n");
#endif /* LISTER */

				   /* Store this information: */
	mlat.detection_time.present = TRUE;
	mlat.detection_time.value = atm;

				   /* Set the return code: */
	ret = RC_OKAY;

	return ret;
}

/*----------------------------------------------------------------------------*/
/* proc_i020_161   -- Process I020/161 data item                              */
/*----------------------------------------------------------------------------*/

 static
 t_Retc proc_i020_161 (t_Ui16 length, t_Byte *buffer)
{
	t_Byte df1;    /* Data field octet 1 */
	t_Byte df2;    /* Data field octet 2 */
	t_Retc ret;    /* Return code */
	t_Ui16 tno;    /* Track number */

				   /* Preset the return code: */
	ret = RC_FAIL;

				   /* Check parameters: */
	Assert (length == 2, "Invalid parameter");
	Assert (buffer != NULL, "Invalid parameter");

				   /* Extract octets: */
	df1 = buffer[0];
	df2 = buffer[1];

				   /* Extract information: */
	if (reference_vsn == 1)
	{
		tno = make_ui16 (df1, df2);
	}
	else if (reference_vsn == 3 || reference_vsn == 6)
	{
		tno = make_ui16 (df1 & 0x0f, df2);
	}
	else
	{
		Assert (FALSE, "Unexpected reference version");
	}

#if LISTER
				   /* List data field: */
	list_text (2, ";  Track Number:");
	list_text (2, " " M_FMT_UI16, tno);
	list_text (2, "\n");
#endif /* LISTER */

				   /* Store this information: */
	mlat.track_number.present = TRUE;
	mlat.track_number.value = tno;

				   /* Set the return code: */
	ret = RC_OKAY;

	return ret;
}

/*----------------------------------------------------------------------------*/
/* proc_i020_170   -- Process I020/170 data item                              */
/*----------------------------------------------------------------------------*/

 static
 t_Retc proc_i020_170 (t_Ui16 length, t_Byte *buffer)
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
			if ((dfo & 0x80) == 0)
			{
				list_text (2, " CNF");
			}

			if (dfo & 0x40)
			{
				list_text (2, " TRE");
			}

			if (dfo & 0x20)
			{
				list_text (2, " CST");
			}

			list_text (2, " CDM=%u", (dfo >> 3) & 0x03);

			if (dfo & 0x04)
			{
				list_text (2, " MAH");
			}

			if (dfo & 0x02)
			{
				list_text (2, " STH");
			} 
		}
		else if (ix == 1)
		{
			if (dfo & 0x80)
			{
				list_text (2, " GHO");
			}
		}
	}
	list_text (2, "\n");
#endif /* LISTER */

				   /* Evaluate and store this information: */
	mlat.reported_track_status.present = TRUE;
	for (ix = 0; ix < length; ix ++)
	{
    	dfo = buffer[ix];

		if (ix == 0)
		{
			mlat.reported_track_status.confirmed = M_TRES ((dfo & 0x80) == 0);
			mlat.reported_track_status.tre = M_TRES (dfo & 0x40);
			mlat.reported_track_status.coasted = M_TRES (dfo & 0x20);

			mlat.reported_track_status.attitude = (dfo >> 3) & 0x03;

			mlat.reported_track_status.horizontal_manoeuvre =
				M_TRES (dfo & 0x04);
			mlat.reported_track_status.smoothed = M_TRES (dfo & 0x02);
		}
		else if (ix == 1)
		{
			mlat.reported_track_status.ghost = M_TRES (dfo & 0x80);
		}
	}

				   /* Set the return code: */
	ret = RC_OKAY;

	return ret;
}

/*----------------------------------------------------------------------------*/
/* proc_i020_202   -- Process I020/202 data item                              */
/*----------------------------------------------------------------------------*/

 static
 t_Retc proc_i020_202 (t_Ui16 length, t_Byte *buffer)
{
	t_Byte df1;    /* Data field octet 1 */
	t_Byte df2;    /* Data field octet 2 */
	t_Byte df3;    /* Data field octet 3 */
	t_Byte df4;    /* Data field octet 4 */
	t_Retc ret;    /* Return code */
	t_Si16 vx;     /* x component of velocity; 0.25 metres/second */
	t_Si16 vy;     /* y component of velocity; 0.25 metres/second */

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
	list_text (2, " vx=" M_FMT_SI16 " (%.2f m/s);", vx, 0.25 * vx);
	list_text (2, " vy=" M_FMT_SI16 " (%.2f m/s)", vy, 0.25 * vy);
	list_text (2, "\n");
#endif /* LISTER */

				   /* Store this information: */
	mlat.computed_velocity.present = TRUE;
	mlat.computed_velocity.value_vx = 0.25 * vx;
	mlat.computed_velocity.value_vy = 0.25 * vy;

				   /* Set the return code: */
	ret = RC_OKAY;

	return ret;
}

/*----------------------------------------------------------------------------*/
/* proc_i020_210   -- Process I020/210 data item                              */
/*----------------------------------------------------------------------------*/

 static
 t_Retc proc_i020_210 (t_Ui16 length, t_Byte *buffer)
{
	t_Si16 ax;     /* x component of acceleration; 0.25 metres/second**2 */
	t_Si16 ay;     /* y component of acceleration; 0.25 metres/second**2 */
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
	ax = (t_Si16) (df1 & 0x7f);
	if (df1 & 0x80)
	{
		ax |= 0xff80;
	}
	ay = (t_Si16) (df2 & 0x7f);
	if (df2 & 0x80)
	{
		ay |= 0xff80;
	}

#if LISTER
				   /* List data field: */
	list_text (2, ";  Calculated Acceleration:");
	list_text (2, " ax=" M_FMT_SI16 " (%.2f m/s**2);", ax, 0.25 * ax);
	list_text (2, " ay=" M_FMT_SI16 " (%.2f m/s**2)", ay, 0.25 * ay);
	list_text (2, "\n");
#endif /* LISTER */

				   /* Store this information: */
	mlat.computed_acceleration.present = TRUE;
	mlat.computed_acceleration.value_ax = 0.25 * ax;
	mlat.computed_acceleration.value_ay = 0.25 * ay;

				   /* Set the return code: */
	ret = RC_OKAY;

	return ret;
}

/*----------------------------------------------------------------------------*/
/* proc_i020_220   -- Process I020/220 data item                              */
/*----------------------------------------------------------------------------*/

 static
 t_Retc proc_i020_220 (t_Ui16 length, t_Byte *buffer)
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
	list_text (2, ";  Target Address:");
	list_text (2, " " M_FMT_ADDR " (" M_FMT_UI32 ")", ta, ta);
	list_text (2, "\n");
#endif /* LISTER */

                   /* Store this information: */
	mlat.target_address.present = TRUE;
	mlat.target_address.value = ta;

				   /* Set the return code: */
	ret = RC_OKAY;

	return ret;
}

/*----------------------------------------------------------------------------*/
/* proc_i020_230   -- Process I020/230 data item                              */
/*----------------------------------------------------------------------------*/

 static
 t_Retc proc_i020_230 (t_Ui16 length, t_Byte *buffer)
{
	t_Byte aic;    /* Aircraft identification capability */
	t_Byte arc;    /* Altitude reporting capability */
	t_Byte b1a;    /* BDS 1,0 bit 16 */
	t_Byte b1b;    /* BDS 1,0 bits 37 to 40 */
	t_Byte com;    /* Communications capability */
	t_Byte df1;    /* Data field octet 1 */
	t_Byte df2;    /* Data field octet 2 */
	t_Byte fs;     /* Flight status */
	t_Byte mssc;   /* Mode S specific service capability */
	t_Retc ret;    /* Return code */
	t_Byte stat;   /* Flight status */

				   /* Preset the return code: */
	ret = RC_FAIL;

				   /* Check parameters: */
	Assert (length == 2, "Invalid parameter");
	Assert (buffer != NULL, "Invalid parameter");

				   /* Extract octets: */
	df1 = buffer[0];
	df2 = buffer[1];

				   /* Extract information: */
	com = (df1 >> 5) & 0x07;
	stat = (df1 >> 2) & 0x07;
	mssc = (df2 >> 7) & 0x01;
	arc = (df2 >> 6) & 0x01;
	aic = (df2 >> 5) & 0x01;
	b1a = (df2 >> 4) & 0x01;
	b1b = df2 & 0x0f;

#if LISTER
				   /* List data field: */
	list_text (2, ";  Communications/ACAS Capability and Flight Status:\n");
	list_text (2, ";   COM=" M_FMT_BYTE ";", com);
	list_text (2, " STAT=" M_FMT_BYTE ";", stat);
	list_text (2, " ARC=" M_FMT_BYTE ";", arc);
	list_text (2, " AIC=" M_FMT_BYTE ";", aic);
	list_text (2, " B1A=" M_FMT_BYTE ";", b1a);
	list_text (2, " B1B=" M_FMT_BYTE "", b1b);
	list_text (2, "\n");
#endif /* LISTER */

				   /* Store this information: */
	mlat.communications_capability.present = TRUE;
	mlat.communications_capability.value_com = com;
	mlat.flight_status.present = TRUE;
	mlat.flight_status.value_aic = aic;
	mlat.flight_status.value_arc = arc;
	mlat.flight_status.value_b1a = b1a;
	mlat.flight_status.value_b1b = b1b;
	mlat.flight_status.value_mssc = mssc;
	mlat.flight_status.value_status = stat;

                   /* Decode flight status: */
	mlat.fs_airborne = e_is_undefined;
	mlat.fs_alert = e_is_undefined;
	mlat.fs_on_ground = e_is_undefined;
	mlat.fs_spi = e_is_undefined;
	fs = stat;
	if (fs == 0)
	{
		mlat.fs_airborne = e_is_true;
		mlat.fs_alert = e_is_false;
		mlat.fs_spi = e_is_false;
	}
	else if (fs == 1)
	{
		mlat.fs_alert = e_is_false;
		mlat.fs_on_ground = e_is_true;
		mlat.fs_spi = e_is_false;
	}
	else if (fs == 2)
	{
		mlat.fs_airborne = e_is_true;
		mlat.fs_alert = e_is_true;
		mlat.fs_spi = e_is_false;
	}
	else if (fs == 3)
	{
		mlat.fs_alert = e_is_true;
		mlat.fs_on_ground = e_is_true;
		mlat.fs_spi = e_is_false;
	}
	else if (fs == 4)
	{
		mlat.fs_alert = e_is_true;
		mlat.fs_spi = e_is_true;
	}
	else if (fs == 5)
	{
		mlat.fs_alert = e_is_false;
		mlat.fs_spi = e_is_true;
	}

				   /* Set the return code: */
	ret = RC_OKAY;

	return ret;
}

/*----------------------------------------------------------------------------*/
/* proc_i020_245   -- Process I020/245 data item                              */
/*----------------------------------------------------------------------------*/

 static
 t_Retc proc_i020_245 (t_Ui16 length, t_Byte *buffer)
{
	t_Byte c6;     /* Character in 6-bit encoding */
	t_Byte df1;    /* Data field octet 1 */
	t_Byte df2;    /* Data field octet 2 */
	t_Byte df3;    /* Data field octet 3 */
	t_Byte df4;    /* Data field octet 4 */
	t_Byte df5;    /* Data field octet 5 */
	t_Byte df6;    /* Data field octet 6 */
	t_Byte df7;    /* Data field octet 7 */
	t_Retc ret;    /* Return code */
	t_Byte sti;    /* Source of target identification */
	char txt[9];   /* Text buffer for target identification */
                   /* If a flight plan has been submitted to the
                      responsible ATC/ATM authorities, this ID is
                      equal to the callsign as entered into the
                      flight plan, or else it is equal to the
                      tail number of the aircraft */

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
	memset (txt, 0, 9);
	c6 = (df2 >> 2) & 0x3f;
	txt[0] = expand_c (c6);
	c6 = ((df2 & 0x03) << 4) | ((df3 >> 4) & 0x0f);
	txt[1] = expand_c (c6);
    c6 = ((df3 & 0x0f) << 2) | ((df4 >> 6) & 0x03);
	txt[2] = expand_c (c6);
	c6 = df4 & 0x3f;
	txt[3] = expand_c (c6);
	c6 = (df5 >> 2) & 0x3f;
	txt[4] = expand_c (c6);
	c6 = ((df5 & 0x03) << 4) | ((df6 >> 4) & 0x0f);
	txt[5] = expand_c (c6);
	c6 = ((df6 & 0x0f) << 2) | ((df7 >> 6) & 0x03);
	txt[6] = expand_c (c6);
	c6 = df7 & 0x3f;
	txt[7] = expand_c (c6);

#if LISTER
				   /* List data field: */
	list_text (2, ";  Target Identification:");
	list_text (2, " sti=" M_FMT_BYTE ";", sti);
	list_text (2, " idt=[%s]", txt);
	list_text (2, "\n");
#endif /* LISTER */

                   /* Store this information: */
	mlat.target_identification.present = TRUE;
	strncpy (mlat.target_identification.value_idt, txt,
             M_AIRCRAFT_IDENTIFICATION_LENGTH);
	mlat.target_identification.value_sti = sti;

				   /* Set the return code: */
	ret = RC_OKAY;

	return ret;
}

/*----------------------------------------------------------------------------*/
/* proc_i020_250   -- Process I020/250 data item                              */
/*----------------------------------------------------------------------------*/

 static
 t_Retc proc_i020_250 (t_Ui16 length, t_Byte *buffer)
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
	Assert (rep > 0, "Invalid repetition factor");

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

		list_text (2, ";   BDS " M_FMT_BYTE "," M_FMT_BYTE " data=0x "
                      M_FMT_BYTE_HEX " " M_FMT_BYTE_HEX " " M_FMT_BYTE_HEX " "
                      M_FMT_BYTE_HEX " " M_FMT_BYTE_HEX " " M_FMT_BYTE_HEX " "
                      M_FMT_BYTE_HEX "\n",
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

#if LISTER
		bds1 = (df8 >> 4) & 0x0f;
		bds2 = df8 & 0x0f;
#endif /* LISTER */

		if (ix < M_MAX_BDS_REGISTERS)
		{
			mlat.bds_registers[ix].number = df8;
			mlat.bds_registers[ix].present = TRUE;
			mlat.bds_registers[ix].value[0] = df1;
			mlat.bds_registers[ix].value[1] = df2;
			mlat.bds_registers[ix].value[2] = df3;
			mlat.bds_registers[ix].value[3] = df4;
			mlat.bds_registers[ix].value[4] = df5;
			mlat.bds_registers[ix].value[5] = df6;
			mlat.bds_registers[ix].value[6] = df7;
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
						mlat.selected_altitude.present = TRUE;
						mlat.selected_altitude.source = 1;
						mlat.selected_altitude.value =
							(t_Si32) (16 * mcp_fcu_sa);
					}
					else if (fms_sa_status)
					{
						mlat.selected_altitude.present = TRUE;
						mlat.selected_altitude.source = 1;
						mlat.selected_altitude.value = (t_Si32) (16 * fms_sa);
					}
				}
				else if (tas == 2)
				{
					if (mcp_fcu_sa_status)
					{
						mlat.selected_altitude.present = TRUE;
						mlat.selected_altitude.source = 2;
						mlat.selected_altitude.value =
							(t_Si32) (16 * mcp_fcu_sa);
					}
				}
				else if (tas == 3)
				{
					if (fms_sa_status)
					{
						mlat.selected_altitude.present = TRUE;
						mlat.selected_altitude.source = 3;
						mlat.selected_altitude.value = (t_Si32) (16 * fms_sa);
					}
				}
			}
			else
			{
				if (mcp_fcu_sa_status)
				{
					mlat.selected_altitude.present = TRUE;
					mlat.selected_altitude.source = 2;
					mlat.selected_altitude.value = (t_Si32) (16 * mcp_fcu_sa);
				}
				else if (fms_sa_status)
				{
					mlat.selected_altitude.present = TRUE;
					mlat.selected_altitude.source = 3;
					mlat.selected_altitude.value = (t_Si32) (16 * fms_sa);
				}
			}

			if (mlat.selected_altitude.present)
			{
				mlat.selected_altitude.value_alt_hold = e_is_false;
				mlat.selected_altitude.value_approach = e_is_false;
				mlat.selected_altitude.value_vnav = e_is_false;

                if ((df6 & 0x01) == 0x01)
				{
					if (df7 & 0x80)
					{
						mlat.selected_altitude.value_vnav = e_is_true;
					}
					if (df7 & 0x40)
					{
						mlat.selected_altitude.value_alt_hold = e_is_true;
					}
					if (df7 & 0x20)
					{
						mlat.selected_altitude.value_approach = e_is_true;
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

	return ret;
}

/*----------------------------------------------------------------------------*/
/* proc_i020_260   -- Process I020/260 data item                              */
/*----------------------------------------------------------------------------*/

 static
 t_Retc proc_i020_260 (t_Ui16 length, t_Byte *buffer)
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
	mlat.acas_resolution_advisory_report.present = TRUE;
	mlat.acas_resolution_advisory_report.value[0] = df1;
	mlat.acas_resolution_advisory_report.value[1] = df2;
	mlat.acas_resolution_advisory_report.value[2] = df3;
	mlat.acas_resolution_advisory_report.value[3] = df4;
	mlat.acas_resolution_advisory_report.value[4] = df5;
	mlat.acas_resolution_advisory_report.value[5] = df6;
	mlat.acas_resolution_advisory_report.value[6] = df7;

				   /* Set the return code: */
	ret = RC_OKAY;

	return ret;
}

/*----------------------------------------------------------------------------*/
/* proc_i020_300   -- Process I020/300 data item                              */
/*----------------------------------------------------------------------------*/

 static
 t_Retc proc_i020_300 (t_Ui16 length, t_Byte *buffer)
{
	t_Byte df1;    /* Data field octet 1 */
	t_Retc ret;    /* Return code */
	t_Byte vfi;    /* Vehicle fleet identification */

				   /* Preset the return code: */
	ret = RC_FAIL;

				   /* Check parameters: */
	Assert (length == 1, "Invalid parameter");
	Assert (buffer != NULL, "Invalid parameter");

				   /* Extract octets: */
	df1 = buffer[0];

				   /* Extract information: */
	vfi = df1;

#if LISTER
				   /* List data field: */
	list_text (2, ";  Vehicle Fleet Identification:");
	list_text (2, " " M_FMT_BYTE, vfi);
	if (vfi <= 16)
	{
		list_text (2, " (%s)", vfi_texts[vfi]);
	}
	list_text (2, "\n");
#endif /* LISTER */

				   /* Store this information: */
	mlat.vehicle_fleet_identification.present = TRUE;
	mlat.vehicle_fleet_identification.value = vfi;

				   /* Set the return code: */
	ret = RC_OKAY;

	return ret;
}

/*----------------------------------------------------------------------------*/
/* proc_i020_310   -- Process I020/310 data item                              */
/*----------------------------------------------------------------------------*/

 static
 t_Retc proc_i020_310 (t_Ui16 length, t_Byte *buffer)
{
#if LISTER
	t_Byte df1;    /* Data field octet 1 */
	t_Byte msg;    /* Pre-programmed message */
#endif /* LISTER */
	t_Retc ret;    /* Return code */
#if LISTER
	t_Byte trb;    /* Trouble indication */
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
	trb = (df1 >> 7) & 0x01;
	msg = df1 & 0x7f;

				   /* List data field: */
	list_text (2, ";  Pre-programmed Message: ");
	list_text (2, " trb=" M_FMT_BYTE ";", trb);
	list_text (2, " msg=" M_FMT_BYTE, msg);
	switch (msg)
	{
	case 1:
		list_text (2, " (towing aircraft)");
		break;
	case 2:
		list_text (2, " (\"follow me\" operation)");
		break;
	case 3:
		list_text (2, " (runway check)");
		break;
	case 4:
		list_text (2, " (emergency operation)");
		break;
	case 5:
		list_text (2, " (work in progress)");
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
/* proc_i020_400   -- Process I020/400 data item                              */
/*----------------------------------------------------------------------------*/

 static
 t_Retc proc_i020_400 (t_Ui16 length, t_Byte *buffer)
{
#if LISTER
	t_Byte df1;    /* Data field octet 1 */
	int ix;        /* Auxiliary */
#endif /* LISTER */
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
	Assert (rep > 0, "Invalid repetition factor");

				   /* Check length (again): */
	Assert (length == 1 + rep, "Invalid length");

#if LISTER
				   /* List data field: */
	list_text (2, ";  Contributing Receivers:");
	list_text (2, " 0x");
	for (ix = 0; ix < rep; ix ++)
	{
		df1 = buffer[1 + ix];

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
/* proc_i020_500   -- Process I020/500 data item                              */
/*----------------------------------------------------------------------------*/

 static
 t_Retc proc_i020_500 (t_Ui16 length, t_Byte *buffer, t_Ui16 *pos_ptr)
{
	t_Byte df1;    /* Data field octet 1 */
	t_Byte df2;    /* Data field octet 2 */
	t_Byte df3;    /* Data field octet 3 */
	t_Byte df4;    /* Data field octet 4 */
	t_Byte df5;    /* Data field octet 5 */
	t_Byte df6;    /* Data field octet 6 */
	t_Byte df7;    /* Data field octet 7 */
	t_Byte dop_x;  /* DOP along x axis; 0.25 */
	t_Si16 dop_xy; /* DOP-xy; 0.25 */
	t_Byte dop_y;  /* DOP along y axis; 0.25 */
	int inx;       /* Auxiliary */
#if LISTER
	int ix;        /* Auxiliary */
	int j;         /* Auxiliary */
#endif /* LISTER */
	t_Ui16 len;    /* Length of data field */
	t_Ui16 pos;    /* Position within buffer */
	t_Retc ret;    /* Return code */
	t_Byte sf1_present;
                   /* Subfield #1 (DOP) present */
	t_Byte sf2_present;
                   /* Subfield #2 (SDP) present */
	t_Byte sf3_present;
                   /* Subfield #3 (SDH) present */
	t_Ui16 sigma_ga;
                   /* Standard deviation of geometric altitude; 0.5 metres */
	t_Byte sigma_x;
                   /* Standard deviation for x component; 0.25 metres */
	t_Si16 sigma_xy;
                   /* Correlation coefficient; 0.25 */
	t_Byte sigma_y;
                   /* Standard deviation for y component; 0.25 metres */

				   /* Preset the return code: */
	ret = RC_FAIL;

				   /* Check parameters: */
	Assert (length > 0, "Invalid parameter");
	Assert (buffer != NULL, "Invalid parameter");
	Assert (pos_ptr != NULL, "Invalid parameter");

                   /* Check reference version: */
	Assert (reference_vsn == 1 || reference_vsn == 3 ||
            reference_vsn == 6,
            "Unexpected reference version");

				   /* Set position within buffer: */
	pos = *pos_ptr;

				   /* Check position: */
	Assert (pos < length, "Invalid position");

                   /* Extract octet: */
	df1 = buffer[pos];

                   /* Evaluate presence of subfields: */
	sf1_present = (df1 >> 7) & 0x01;
	sf2_present = (df1 >> 6) & 0x01;
	if (reference_vsn == 3 || reference_vsn == 6)
	{
		sf3_present = (df1 >> 5) & 0x01;
	}
	else
	{
		sf3_present = FALSE;
	}

                   /* Determine expected length of data field: */
	if (reference_vsn == 1)
	{
		len = 1 +
              4 * ord (sf1_present) +
              4 * ord (sf2_present);
	}
	else if (reference_vsn == 3 || reference_vsn == 6)
	{
		len = 1 +
              6 * ord (sf1_present) +
              6 * ord (sf2_present) +
              2 * ord (sf3_present);
	}

                   /* Check against buffer length: */
	if (pos + len > length)
	{
		error_msg ("Invalid buffer length (I020/500)");
		goto done;
	}

#if LISTER
                   /* List raw data field: */
	list_text (1, ";  I020/500: 0x");
	j = 0;
	for (ix = 0; ix < len; ix ++)
	{
		list_text (1, " " M_FMT_BYTE_HEX, buffer[pos + ix]);
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

                   /* Extract secondary subfields: */
	inx = 1;
	if (sf1_present)
	{
		if (reference_vsn == 1)
		{
			df2 = buffer[pos + inx];
			df3 = buffer[pos + inx + 1];
			df4 = buffer[pos + inx + 2];
			df5 = buffer[pos + inx + 3];
			inx += 4;

			dop_x = df2;
			dop_y = df3;
			dop_xy = make_si16 (df4, df5);
		}
		else if (reference_vsn == 3 ||
                 reference_vsn == 6)
		{
			df2 = buffer[pos + inx];
			df3 = buffer[pos + inx + 1];
			df4 = buffer[pos + inx + 2];
			df5 = buffer[pos + inx + 3];
			df6 = buffer[pos + inx + 4];
			df7 = buffer[pos + inx + 5];
			inx += 6;

			dop_x = make_ui16 (df2, df3);
			dop_y = make_ui16 (df4, df5);
			dop_xy = make_si16 (df6, df7);
		}
	}
	if (sf2_present)
	{
		if (reference_vsn == 1)
		{
			df2 = buffer[pos + inx];
			df3 = buffer[pos + inx + 1];
			df4 = buffer[pos + inx + 2];
			df5 = buffer[pos + inx + 3];
			inx += 4;

			sigma_x = df2;
			sigma_y = df3;
			sigma_xy = make_si16 (df4, df5);
		}
		else if (reference_vsn == 3 ||
                 reference_vsn == 6)
		{
			df2 = buffer[pos + inx];
			df3 = buffer[pos + inx + 1];
			df4 = buffer[pos + inx + 2];
			df5 = buffer[pos + inx + 3];
			df6 = buffer[pos + inx + 4];
			df7 = buffer[pos + inx + 5];
			inx += 6;

			sigma_x = make_ui16 (df2, df3);
			sigma_y = make_ui16 (df4, df5);
			sigma_xy = make_si16 (df6, df7);
		}
	}
	if (sf3_present)
	{
		df2 = buffer[pos + inx];
		df3 = buffer[pos + inx + 1];
		inx += 2;

		sigma_ga = make_ui16 (df2, df3);
	}
	Assert (inx == len, "Invalid structure decoding");

#if LISTER
                   /* List data field: */
	list_text (2, ";  Position Accuracy:");
	list_text (2, "\n");
	if (sf1_present)
	{
		if (reference_vsn == 1)
		{
			list_text (2, ";   GDOP of Position:");
		}
		else if (reference_vsn == 3 || reference_vsn == 6)
		{
			list_text (2, ";   DOP of Position:");
		}
		list_text (2, " x=" M_FMT_BYTE " (%.2f);", dop_x, 0.25 * dop_x);
		list_text (2, " y=" M_FMT_BYTE " (%.2f);", dop_y, 0.25 * dop_y);
		list_text (2, " xy=" M_FMT_SI16 " (%.2f)", dop_xy, 0.25 * dop_xy);
		list_text (2, "\n");
	}
	if (sf2_present)
	{
		list_text (2, ";   Standard Deviation of Position:");
		list_text (2, " x=" M_FMT_BYTE " (%.2f mtr);", sigma_x, 0.25 * sigma_x);
		list_text (2, " y=" M_FMT_BYTE " (%.2f mtr);", sigma_y, 0.25 * sigma_y);
		list_text (2, " xy=" M_FMT_SI16 " (%.2f)", sigma_xy, 0.25 * sigma_xy);
		list_text (2, "\n");
	}
	if (sf3_present)
	{
		if (reference_vsn == 3)
		{
			list_text (2, ";   Standard Deviation of Geometric Altitude:");
		}
		else if (reference_vsn == 6)
		{
			list_text (2, ";   Standard Deviation of Geometric Height:");
		}
		else
		{
			Assert (FALSE, "Unexpected reference version");
		}
		list_text (2, " " M_FMT_UI16 " (%.2f mtr)", sigma_ga, 0.5 * sigma_ga);
		list_text (2, "\n");
	}
#endif /* LISTER */

                   /* Store this information: */
	if (sf1_present)
	{
		mlat.dop_of_position.present = TRUE;
		mlat.dop_of_position.value_x = 0.25 * dop_x;
		mlat.dop_of_position.value_xy = 0.25 * dop_xy;
		mlat.dop_of_position.value_y = 0.25 * dop_y;
	}
	if (sf2_present)
	{
		mlat.computed_position_stdev.present = TRUE;
		mlat.computed_position_stdev.value_x = 0.25 * sigma_x;
		mlat.computed_position_stdev.value_xy = 0.25 * sigma_xy;
		mlat.computed_position_stdev.value_y = 0.25 * sigma_y;
	}
	if (sf3_present)
	{
		mlat.geometric_altitude_stdev.present = TRUE;
		mlat.geometric_altitude_stdev.value = 0.5 * sigma_ga;
	}

                   /* Set position: */
	*pos_ptr = pos + len;

                   /* Set the return code: */
	ret = RC_OKAY;

	done:          /* We are done */
	return ret;
}

/*----------------------------------------------------------------------------*/
/* proc_i020_ref   -- Process RE indicator data item                          */
/*----------------------------------------------------------------------------*/

 static
 t_Retc proc_i020_ref (t_Ui16 length, t_Byte *buffer, t_Ui16 *pos_ptr)
{
#if LISTER
	t_Byte age_aic;
                   /* Aircraft identification capability age; 0.1 seconds */
	t_Byte age_ara;
                   /* ACAS resolution advisory age; 0.1 seconds */
	t_Byte age_arc;
                   /* Altitude reporting capability age; 0.1 seconds */
	t_Byte age_fl; /* Flight level age; 0.1 seconds */
	t_Byte age_fs; /* Flight status age; 0.1 seconds */
	t_Byte age_gh; /* Geometric / Measured height age; 0.1 seconds */
	t_Byte age_m1; /* Mode 1 code age; 0.1 seconds */
	t_Byte age_m2; /* Mode 2 code age; 0.1 seconds */
	t_Byte age_m3a;
                   /* Mode 3/A code age; 0.1 seconds */
	t_Byte age_mc; /* Mode C code age; 0.1 seconds */
	t_BDS_Register_Age age_mbd[M_MAX_BDS_REGISTERS];
                   /* Mode S MB age(s) */
	t_Byte age_msg;
                   /* Pre-programmed message age; 0.1 seconds */
	t_Byte age_mss;
                   /* Mode S specific capabilities age; 0.1 seconds */
	t_Byte age_spi;
                   /* Special position indication age; 0.1 seconds */
	t_Byte age_ta; /* Target address age; 0.1 seconds */
	t_Byte age_ti; /* Target identification age; 0.1 seconds */
	t_Byte age_vi; /* Vehicle fleet identification age; 0.1 seconds */
	t_Si16 cov_wgs;
                   /* Lat/Long covariance component; 180/2**25 degrees */
#endif /* LISTER */
	t_Si16 cov_xy; /* Covariance component of standard deviation
                      of position; 0.25 metres */
	t_Byte da;     /* Data ages present */
	t_Byte da_sf1_present;
                   /* Data ages sub-field #1 (SPI) present */
	t_Byte da_sf2_present;
                   /* Data ages sub-field #2 (TI) present */
	t_Byte da_sf3_present;
                   /* Data ages sub-field #3 (MBD) present */
	t_Byte da_sf4_present;
                   /* Data ages sub-field #4 (M3A) present */
	t_Byte da_sf5_present;
                   /* Data ages sub-field #5 (FL) present */
	t_Byte da_sf6_present;
                   /* Data ages sub-field #6 (FS) present */
	t_Byte da_sf7_present;
                   /* Data ages sub-field #7 (GH) present */
	t_Byte da_sf8_present;
                   /* Data ages sub-field #8 (TA) present */
	t_Byte da_sf9_present;
                   /* Data ages sub-field #9 (MC) present */
	t_Byte da_sf10_present;
                   /* Data ages sub-field #10 (MSS) present */
	t_Byte da_sf11_present;
                   /* Data ages sub-field #11 (ARC) present */
	t_Byte da_sf12_present;
                   /* Data ages sub-field #12 (AIC) present */
	t_Byte da_sf13_present;
                   /* Data ages sub-field #13 (M2) present */
	t_Byte da_sf14_present;
                   /* Data ages sub-field #14 (M1) present */
	t_Byte da_sf15_present;
                   /* Data ages sub-field #15 (ARA) present */
	t_Byte da_sf16_present;
                   /* Data ages sub-field #16 (VI) present */
	t_Byte da_sf17_present;
                   /* Data ages sub-field #17 (MSG) present */
	t_Byte df2;    /* Data field octet 2 */
	t_Byte df3;    /* Data field octet 3 */
	t_Byte df4;    /* Data field octet 4 */
	t_Byte df5;    /* Data field octet 5 */
	t_Byte df6;    /* Data field octet 6 */
	t_Byte df7;    /* Data field octet 7 */
	t_Ui16 dop_x;  /* DOP along x axis; 0.25 */
	t_Si16 dop_xy; /* DOP covariance component; 0.25 */
	t_Ui16 dop_y;  /* DOP along y axis; 0.25 */
	t_Ui16 expected_len;
                   /* Expected length */
#if LISTER
	t_Ui16 gsp;    /* Ground speed; 2**-14 NM/s */
	t_Byte gssd;   /* Standard deviation of ground speed; 2**-14 NM/s */
#endif /* LISTER */
	t_Byte gva;    /* Ground velocity accuracy present */
	t_Byte gvv;    /* Ground velocity vector present */
	t_Ui16 inx;    /* Index into buffer */
#if LISTER
	int ix;        /* Auxiliary */
	int j;         /* Auxiliary */
#endif /* LISTER */
	t_Byte len;    /* Length of data field */
	t_Byte pa;     /* Position accuracy present */
	t_Bool pa_error;
                   /* Indication for encoding error for PA */
	t_Ui16 pos;    /* Position within buffer */
#if LISTER
	t_Byte re_gsp; /* Range exceeded in ground speed */
#endif /* LISTER */
	t_Retc ret;    /* Return code */
	t_Ui16 sdc_x;  /* x component of standard deviation of position
                      of the target expressed in Cartesian coordinates;
                      0.25 metres */
	t_Ui16 sdc_y;  /* y component of standard deviation of position
                      of the target expressed in Cartesian coordinates;
                      0.25 metres */
	t_Ui16 sdh;    /* Standard deviation of geometric height; feet */
#if LISTER
	t_Ui16 sdw_lat;
                   /* Latitude component of standard deviation of position
                      of the target expressed in WGS-84;
                      180/2**25 degrees */
	t_Ui16 sdw_lon;
                   /* Longitude component of standard deviation of position
                      of the target expressed in WGS-84;
                      180/2**25 degrees */
#endif /* LISTER */
	t_Byte sf1_present;
                   /* Sub-field #1 (DOP) present */
	t_Byte sf2_present;
                   /* Sub-field #2 (SDC) present */
	t_Byte sf3_present;
                   /* Sub-field #3 (SDH) present */
	t_Byte sf4_present;
                   /* Sub-field #4 (SDW) present */
#if LISTER
	t_Ui16 ta;     /* Track angle; 360/2**16 degrees */
	t_Byte tasd;   /* Standard deviation of track angle; 360/2**16 degrees */
	t_Ui32 tr_tms; /* Timestamp of transmission of report; 1/128 seconds */
#endif /* LISTER */
	t_Byte trt;    /* Time of report transmission present */

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
		error_msg ("Invalid buffer length (I020/RE)");
		goto done;
	}

#if LISTER
				   /* List raw data field: */
	list_text (1, ";  I020/RE:  0x");
	j = 0;
	for (ix = 0; ix < len; ix ++)
	{
		list_text (1, " " M_FMT_BYTE_HEX, buffer[pos + ix]);
		++ j;

		if (j >= 16)
		{
			list_text (1, "\n");
			list_text (1, ";            0x");

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

	if (reference_vsn != 6)
	{
		goto term;
	}

                   /* Preset local data: */
	da = 0;
	gva = 0;
	gvv = 0;
	pa = 0;
	pa_error = FALSE;
	trt = 0;

                   /* Get and store information: */
	Assert (len >= 2, "Invalid length of I020/RE field");

	pa = (buffer[pos + 1] >> 7) & 0x01;
	gvv = (buffer[pos + 1] >> 6) & 0x01;
	gva = (buffer[pos + 1] >> 5) & 0x01;
	trt = (buffer[pos + 1] >> 4) & 0x01;
	da = (buffer[pos + 1] >> 3) & 0x01;

	Assert (len >= 3, "Invalid length of I020/RE field");

#if SSC_HACK
                   /* Special fix for SSC software bug: */
	if (len == 15 &&
        buffer[pos + 1] == 0x00)
	{
		pa = 1;
		pa_error = TRUE;

		printf ("-> Fixed I020/RE encoding bug (assuming PA to be set) ...\n");
	}
#endif /* SSC_HACK */

	sf1_present = 0;
	sf2_present = 0;
	sf3_present = 0;
	sf4_present = 0;

	da_sf1_present = 0;
	da_sf2_present = 0;
	da_sf3_present = 0;
	da_sf4_present = 0;
	da_sf5_present = 0;
	da_sf6_present = 0;
	da_sf7_present = 0;
	da_sf8_present = 0;
	da_sf9_present = 0;
	da_sf10_present = 0;
	da_sf11_present = 0;
	da_sf12_present = 0;
	da_sf13_present = 0;
	da_sf14_present = 0;
	da_sf15_present = 0;
	da_sf16_present = 0;
	da_sf17_present = 0;

	if (pa)
	{
		sf1_present = (buffer[pos + 2] >> 7) & 0x01;
		sf2_present = (buffer[pos + 2] >> 6) & 0x01;
		sf3_present = (buffer[pos + 2] >> 5) & 0x01;
		sf4_present = (buffer[pos + 2] >> 4) & 0x01;
	}

	expected_len = 3 +
                   6 * ord (sf1_present) +
                   6 * ord (sf2_present) +
                   2 * ord (sf3_present) +
                   6 * ord (sf4_present);

	if (expected_len > len)
	{
		error_msg ("Invalid buffer length (I020/RE)");
		goto done;
	}

	inx = 3;

	if (sf1_present)
	{
		df2 = buffer[pos + inx];
		df3 = buffer[pos + inx + 1];
		df4 = buffer[pos + inx + 2];
		df5 = buffer[pos + inx + 3];
		df6 = buffer[pos + inx + 4];
		df7 = buffer[pos + inx + 5];
		inx += 6;

		dop_x = make_ui16 (df2, df3);
		dop_y = make_ui16 (df4, df5);
		dop_xy = make_si16 (df6, df7);
	}
	if (sf2_present)
	{
		df2 = buffer[pos + inx];
		df3 = buffer[pos + inx + 1];
		df4 = buffer[pos + inx + 2];
		df5 = buffer[pos + inx + 3];
		df6 = buffer[pos + inx + 4];
		df7 = buffer[pos + inx + 5];
		inx += 6;

		sdc_x = make_ui16 (df2, df3);
		sdc_y = make_ui16 (df4, df5);
		cov_xy = make_si16 (df6, df7);
	}
	if (sf3_present)
	{
		df2 = buffer[pos + inx];
		df3 = buffer[pos + inx + 1];
		inx += 2;

		sdh = make_ui16 (df2, df3);
	}
	if (sf4_present)
	{
		df2 = buffer[pos + inx];
		df3 = buffer[pos + inx + 1];
		df4 = buffer[pos + inx + 2];
		df5 = buffer[pos + inx + 3];
		df6 = buffer[pos + inx + 4];
		df7 = buffer[pos + inx + 5];
		inx += 6;

#if LISTER
		sdw_lat = make_ui16 (df2, df3);
		sdw_lon = make_ui16 (df4, df5);
		cov_wgs = make_si16 (df6, df7);
#endif /* LISTER */
	}

	if (gvv)
	{
		df2 = buffer[pos + inx];
		df3 = buffer[pos + inx + 1];
		df4 = buffer[pos + inx + 2];
		df5 = buffer[pos + inx + 3];
		inx += 4;

#if LISTER
		re_gsp = (df2 >> 7) & 0x01;
		gsp = make_ui16 (df2 & 0x7f, df3);
		ta = make_ui16 (df3, df4);
#endif /* LISTER */
	}

	if (gva)
	{
		df2 = buffer[pos + inx];
		df3 = buffer[pos + inx + 1];
		inx += 2;

#if LISTER
		gssd = df2;
		tasd = df3;
#endif /* LISTER */
	}

	if (trt)
	{
		df2 = buffer[pos + inx];
		df3 = buffer[pos + inx + 1];
		df4 = buffer[pos + inx + 2];
		inx += 3;

#if LISTER
		tr_tms = make_ui32 (0x00, df2, df3, df4);
#endif /* LISTER */
	}

	if (da)
	{
		t_Byte da_len;

		da_len = 0;

		df2 = buffer[pos + inx];
		++ da_len;
		++ inx;

		da_sf1_present = (df2 >> 7) & 0x01;
		da_sf2_present = (df2 >> 6) & 0x01;
		da_sf3_present = (df2 >> 5) & 0x01;
		da_sf4_present = (df2 >> 4) & 0x01;
		da_sf5_present = (df2 >> 3) & 0x01;
		da_sf6_present = (df2 >> 2) & 0x01;
		da_sf7_present = (df2 >> 1) & 0x01;

		if (df2 & 0x01)
		{
			df2 = buffer[pos + inx];
			++ da_len;
			++ inx;

			da_sf8_present = (df2 >> 7) & 0x01;
			da_sf9_present = (df2 >> 6) & 0x01;
			da_sf10_present = (df2 >> 5) & 0x01;
			da_sf11_present = (df2 >> 4) & 0x01;
			da_sf12_present = (df2 >> 3) & 0x01;
			da_sf13_present = (df2 >> 2) & 0x01;
			da_sf14_present = (df2 >> 1) & 0x01;
		}

		if (df2 & 0x01)
		{
			df2 = buffer[pos + inx];
			++ da_len;
			++ inx;

			da_sf15_present = (df2 >> 7) & 0x01;
			da_sf16_present = (df2 >> 6) & 0x01;
			da_sf17_present = (df2 >> 5) & 0x01;
		}

		if (da_sf1_present)
		{
			df2 = buffer[pos + inx];
			++ inx;

#if LISTER
			age_spi = df2;
#endif /* LISTER */
		}

		if (da_sf2_present)
		{
			df2 = buffer[pos + inx];
			++ inx;

#if LISTER
			age_ti = df2;
#endif /* LISTER */
		}

		if (da_sf3_present)
		{
			t_Byte ix;
			t_Byte rep;

#if LISTER
			for (ix = 0; ix < M_MAX_BDS_REGISTERS; ix ++)
			{
				age_mbd[ix].age = 0;
				age_mbd[ix].bds_register = 0;
				age_mbd[ix].defined = FALSE;
			}
#endif /* LISTER */

			rep = buffer[pos + inx];
			++ inx;

			Assert (1 <= rep && rep <= M_MAX_BDS_REGISTERS,
                    "Invalid repetition factor");

			for (ix = 0; ix < rep; ix ++)
			{
				df2 = buffer[pos + inx];
				df3 = buffer[pos + inx + 1];
				inx += 2;

#if LISTER
				age_mbd[ix].age = df3;
				age_mbd[ix].bds_register = df2;
				age_mbd[ix].defined = TRUE;
#endif /* LISTER */
			}
		}

		if (da_sf4_present)
		{
			df2 = buffer[pos + inx];
			++ inx;

#if LISTER
			age_m3a = df2;
#endif /* LISTER */
		}

		if (da_sf5_present)
		{
			df2 = buffer[pos + inx];
			++ inx;

#if LISTER
			age_fl = df2;
#endif /* LISTER */
		}

		if (da_sf6_present)
		{
			df2 = buffer[pos + inx];
			++ inx;

#if LISTER
			age_fs = df2;
#endif /* LISTER */
		}

		if (da_sf7_present)
		{
			df2 = buffer[pos + inx];
			++ inx;

#if LISTER
			age_gh = df2;
#endif /* LISTER */
		}

		if (da_sf8_present)
		{
			df2 = buffer[pos + inx];
			++ inx;

#if LISTER
			age_ta = df2;
#endif /* LISTER */
		}

		if (da_sf9_present)
		{
			df2 = buffer[pos + inx];
			++ inx;

#if LISTER
			age_mc = df2;
#endif /* LISTER */
		}

		if (da_sf10_present)
		{
			df2 = buffer[pos + inx];
			++ inx;

#if LISTER
			age_mss = df2;
#endif /* LISTER */
		}

		if (da_sf11_present)
		{
			df2 = buffer[pos + inx];
			++ inx;

#if LISTER
			age_arc = df2;
#endif /* LISTER */
		}

		if (da_sf12_present)
		{
			df2 = buffer[pos + inx];
			++ inx;

#if LISTER
			age_aic = df2;
#endif /* LISTER */
		}

		if (da_sf13_present)
		{
			df2 = buffer[pos + inx];
			++ inx;

#if LISTER
			age_m2 = df2;
#endif /* LISTER */
		}

		if (da_sf14_present)
		{
			df2 = buffer[pos + inx];
			++ inx;

#if LISTER
			age_m1 = df2;
#endif /* LISTER */
		}

		if (da_sf15_present)
		{
			df2 = buffer[pos + inx];
			++ inx;

#if LISTER
			age_ara = df2;
#endif /* LISTER */
		}

		if (da_sf16_present)
		{
			df2 = buffer[pos + inx];
			++ inx;

#if LISTER
			age_vi = df2;
#endif /* LISTER */
		}

		if (da_sf17_present)
		{
			df2 = buffer[pos + inx];
			++ inx;

#if LISTER
			age_msg = df2;
#endif /* LISTER */
		}
	}

                   /* Check validity of structure encoding: */
	if (inx != len)
	{
		error_msg ("Invalid structure encoding of I020/RE");
		goto done;
	}

#if LISTER
                   /* List data field: */
	if (pa)
	{
		list_text (2, ";  Position Accuracy:");
		list_text (2, "\n");
		if (sf1_present)
		{
			list_text (2, ";   DOP of Position:");
			list_text (2, " x=" M_FMT_BYTE " (%.2f);", dop_x, 0.25 * dop_x);
			list_text (2, " y=" M_FMT_BYTE " (%.2f);", dop_y, 0.25 * dop_y);
			list_text (2, " xy=" M_FMT_SI16 " (%.2f)", dop_xy, 0.25 * dop_xy);
			list_text (2, "\n");
		}
		if (sf2_present)
		{
			list_text (2, ";   Standard Deviation of Position:");
			list_text (2, " x=" M_FMT_UI16 " (%.2f mtr);", sdc_x, 0.25 * sdc_x);
			list_text (2, " y=" M_FMT_UI16 " (%.2f mtr);", sdc_y, 0.25 * sdc_y);
			list_text (2, " xy=" M_FMT_SI16 " (%.2f mtr)", cov_xy, 0.25 * cov_xy);
			list_text (2, "\n");
		}
		if (sf3_present)
		{
			list_text (2, ";   Standard Deviation of Geometric Height:");
			list_text (2, " " M_FMT_UI16 " (%.2f mtr)", sdh, M_FT2MTR * sdh);
			list_text (2, "\n");
		}
		if (sf4_present)
		{
			list_text (2, ";   Standard Deviation of Position:");
			list_text (2, " lat=" M_FMT_UI16 " (%.6f deg);",
                          sdw_lat, (180.0 / M_TWO_POWER_25) * sdw_lat);
			list_text (2, " lon=" M_FMT_UI16 " (%.6f deg);",
                          sdw_lon, (180.0 / M_TWO_POWER_25) * sdw_lon);
			list_text (2, " cov=" M_FMT_SI16 " (%.6f deg)",
                          cov_wgs, (180.0 / M_TWO_POWER_25) * cov_wgs);
			list_text (2, "\n");
		}
	}

	if (gvv)
	{
		list_text (2, ";  Ground Velocity Vector:");
		list_text (2, "\n");

		list_text (2, ";   re_gsp=" M_FMT_BYTE, re_gsp);
		list_text (2, "; gsp=" M_FMT_UI16 " (%.2f kts)",
                      gsp, (3600.0 / 16384.0) * gsp);
		list_text (2, "; ta=" M_FMT_UI16 " (%.3f deg)",
                      ta, (360.0 / 65536.0) * ta);
		list_text (2, "\n");
	}

	if (gva)
	{
		list_text (2, ";  Ground Velocity Accuracy:");
		list_text (2, "\n");

		list_text (2, ";   gssd=" M_FMT_BYTE " (%.2f kts)",
                      gssd, (3600.0 / 16384.0) * gssd);
		list_text (2, "; tasd=" M_FMT_BYTE " (%.3f deg)",
                      tasd, (360.0 / 65536.0) * tasd);
		list_text (2, "\n");
	}

	if (trt)
	{
		list_text (2, ";  Time of ASTERIX Report Transmission:");
		list_text (2, " " M_FMT_ATOD " (" M_FMT_UI32 "; %s UTC)",
                      tr_tms, tr_tms, utc_text ((1.0 / 128.0) * tr_tms));
		list_text (2, "\n");
	}

	if (da)
	{
		list_text (2, ";  Data Ages:");
		list_text (2, "\n");

		if (da_sf1_present)
		{
			list_text (2, ";   Special Position Indication age: %.1f sec",
                          0.1 * age_spi);
			list_text (2, "\n");
		}

		if (da_sf2_present)
		{
			list_text (2, ";   Target Identification age: %.1f sec",
                          0.1 * age_ti);
			list_text (2, "\n");
		}

		if (da_sf3_present)
		{
			list_text (2, ";   Mode S MB age:");
			list_text (2, "\n");

			for (ix = 0; ix < M_MAX_BDS_REGISTERS; ix ++)
			{
				if (age_mbd[ix].defined)
				{
					list_text (2, ";    BDS " M_FMT_BYTE "," M_FMT_BYTE " age:"
                                  " %.1f sec",
                                  (age_mbd[ix].bds_register >> 4) & 0x0f,
                                  age_mbd[ix].bds_register & 0x0f,
                                  0.1 * age_mbd[ix].age);
					list_text (2, "\n");
				}
			}
		}

		if (da_sf4_present)
		{
			list_text (2, ";   Mode 3/A Code age: %.1f sec", 0.1 * age_m3a);
			list_text (2, "\n");
		}

		if (da_sf5_present)
		{
			list_text (2, ";   Flight Level age: %.1f sec", 0.1 * age_fl);
			list_text (2, "\n");
		}

		if (da_sf6_present)
		{
			list_text (2, ";   Flight Status age: %.1f sec", 0.1 * age_fs);
			list_text (2, "\n");
		}

		if (da_sf7_present)
		{
			list_text (2, ";   Geometric / Measured Height age: %.1f sec",
                          0.1 * age_gh);
			list_text (2, "\n");
		}

		if (da_sf8_present)
		{
			list_text (2, ";   Target Address age: %.1f sec", 0.1 * age_ta);
			list_text (2, "\n");
		}

		if (da_sf9_present)
		{
			list_text (2, ";   Mode C Code age: %.1f sec", 0.1 * age_mc);
			list_text (2, "\n");
		}

		if (da_sf10_present)
		{
			list_text (2, ";   Mode S Specific Service Capability age: %.1f sec",
                          0.1 * age_mss);
			list_text (2, "\n");
		}

		if (da_sf11_present)
		{
			list_text (2, ";   Altitude Reporting Capability age: %.1f sec",
                          0.1 * age_arc);
			list_text (2, "\n");
		}

		if (da_sf12_present)
		{
			list_text (2, ";   Aircraft Identification Reporting Capability age:"
                          " %.1f sec",
                          0.1 * age_aic);
			list_text (2, "\n");
		}

		if (da_sf13_present)
		{
			list_text (2, ";   Mode 2 Code age: %.1f sec", 0.1 * age_m2);
			list_text (2, "\n");
		}

		if (da_sf14_present)
		{
			list_text (2, ";   Mode 1 Code age: %.1f sec", 0.1 * age_m1);
			list_text (2, "\n");
		}

		if (da_sf15_present)
		{
			list_text (2, ";   ACAS Resolution Advisory age: %.1f sec",
                          0.1 * age_ara);
			list_text (2, "\n");
		}

		if (da_sf16_present)
		{
			list_text (2, ";   Vehicle Fleet Identification age: %.1f sec",
                          0.1 * age_vi);
			list_text (2, "\n");
		}

		if (da_sf17_present)
		{
			list_text (2, ";   Pre-programmed Message age: %.1f sec",
                          0.1 * age_msg);
			list_text (2, "\n");
		}
	}
#endif /* LISTER */

                   /* Store this information: */
	if (pa && !pa_error)
	{
		if (sf1_present)
		{
			mlat.dop_of_position.present = TRUE;
			mlat.dop_of_position.value_x = 0.25 * dop_x;
			mlat.dop_of_position.value_xy = 0.25 * dop_xy;
			mlat.dop_of_position.value_y = 0.25 * dop_y;
		}
		if (sf2_present)
		{
			mlat.computed_position_stdev.present = TRUE;
			mlat.computed_position_stdev.value_x = 0.25 * sdc_x;
			mlat.computed_position_stdev.value_xy = 0.25 * cov_xy;
			mlat.computed_position_stdev.value_y = 0.25 * sdc_y;
		}
		if (sf3_present)
		{
			mlat.geometric_altitude_stdev.present = TRUE;
			mlat.geometric_altitude_stdev.value = M_FT2MTR * sdh;
		}
		/* tba ... sdw_lat, sdw_lon, cov_wgs */
	}
	if (gvv)
	{
		/* tba ... re_gsp, gsp, ta */
	}
	if (gva)
	{
		/* tba ... gssd, tasd */
	}
	if (trt)
	{
		/* tba ... tr_tms */
	}
	if (da)
	{
		/* tba ... */
	}

	term:

				   /* Set position: */
	*pos_ptr = pos + len;

				   /* Set the return code: */
	ret = RC_OKAY;

	done:          /* We are done */
	return ret;
}

/*----------------------------------------------------------------------------*/
/* proc_i020_spf   -- Process SP indicator data item                          */
/*----------------------------------------------------------------------------*/

 static
 t_Retc proc_i020_spf (t_Ui16 length, t_Byte *buffer, t_Ui16 *pos_ptr)
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
		error_msg ("Invalid buffer length (I020/SP)");
		goto done;
	}

#if LISTER
				   /* List raw data field: */
	list_buffer (1, ";  I020/SP :", len, buffer + pos);
#endif /* LISTER */

				   /* Set position: */
	*pos_ptr = pos + len;

				   /* Set the return code: */
	ret = RC_OKAY;

	done:          /* We are done */
	return ret;
}

/*----------------------------------------------------------------------------*/
/* set_vsn020      -- Set ASTERIX category 020 reference version              */
/*----------------------------------------------------------------------------*/

 t_Retc set_vsn020 (char *vsn_text)
{
	t_Retc ret;    /* Return code */

                   /* Preset the return code: */
	ret = RC_FAIL;

                   /* Check parameter(s): */
	Assert (vsn_text != NULL, "Invalid parameter");

                   /* Check against implemented reference versions: */
	if (strcmp (vsn_text, "1.0") == 0)
	{
		reference_vsn = 1;
		ret = RC_OKAY;
	}
	else if (strcmp (vsn_text, "1.1") == 0)
	{
		reference_vsn = 2;
		ret = RC_OKAY;
	}
	else if (strcmp (vsn_text, "1.2") == 0)
	{
		reference_vsn = 3;
		ret = RC_OKAY;
	}
	else if (strcmp (vsn_text, "1.3") == 0)
	{
		reference_vsn = 4;
		ret = RC_OKAY;
	}
	else if (strcmp (vsn_text, "1.4") == 0)
	{
		reference_vsn = 5;
		ret = RC_OKAY;
	}
	else if (strcmp (vsn_text, "1.5") == 0)
	{
		reference_vsn = 6;
		ret = RC_OKAY;
	}

	return ret;
}
/* end-of-file */
