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
/* File:           src/astx_010.cpp                                           */
/* Contents:       Process ASTERIX category 010 data block                    */
/* Author(s):      kb                                                         */
/* Last change:    2018-04-27                                                 */
/*----------------------------------------------------------------------------*/

/* Reference documents:
   --------------------

   Eurocontrol Standard Document for Surveillance Data Exchange.
   Part 7: Transmission of Monosensor Surface Movement Data.
   SUR.ET1.ST05.2000-STD-07-01

   Edition: 1.1
   Edition Date: March 2007
   Status: Released Issue
*/

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

#define M_MAX_FRN 28
				   /* Maximum field reference number */
#define M_MAX_FSPEC_LENGTH 4
				   /* Maximum fields specification length
					  for ASTERIX category 010 */

                   /* Local texts: */
                   /* ------------ */

#if LISTER
static const char *type_texts[8] =
                   /* Texts for target type in I010/020 */
{
	"SSR multilateration",
	"Mode S multilateration",
	"ADS-B",
	"PSR",
	"Magnetic Loop System",
	"HF multilateration",
	"Not defined",
	"Other types"
};
#endif /* LISTER */

				   /* Local variables: */
				   /* ---------------- */

static t_Data_Item_Desc desc_i010_000;
                   /* Description of data item I010/000 */
static t_Data_Item_Desc desc_i010_010;
                   /* Description of data item I010/010 */
static t_Data_Item_Desc desc_i010_020;
                   /* Description of data item I010/020 */
static t_Data_Item_Desc desc_i010_040;
                   /* Description of data item I010/040 */
static t_Data_Item_Desc desc_i010_041;
                   /* Description of data item I010/041 */
static t_Data_Item_Desc desc_i010_042;
                   /* Description of data item I010/042 */
static t_Data_Item_Desc desc_i010_060;
                   /* Description of data item I010/060 */
static t_Data_Item_Desc desc_i010_090;
                   /* Description of data item I010/090 */
static t_Data_Item_Desc desc_i010_091;
                   /* Description of data item I010/091 */
static t_Data_Item_Desc desc_i010_131;
                   /* Description of data item I010/131 */
static t_Data_Item_Desc desc_i010_140;
                   /* Description of data item I010/140 */
static t_Data_Item_Desc desc_i010_161;
                   /* Description of data item I010/161 */
static t_Data_Item_Desc desc_i010_170;
                   /* Description of data item I010/170 */
static t_Data_Item_Desc desc_i010_200;
                   /* Description of data item I010/200 */
static t_Data_Item_Desc desc_i010_202;
                   /* Description of data item I010/202 */
static t_Data_Item_Desc desc_i010_210;
                   /* Description of data item I010/210 */
static t_Data_Item_Desc desc_i010_220;
                   /* Description of data item I010/220 */
static t_Data_Item_Desc desc_i010_245;
                   /* Description of data item I010/245 */
static t_Data_Item_Desc desc_i010_250;
                   /* Description of data item I010/250 */
static t_Data_Item_Desc desc_i010_270;
                   /* Description of data item I010/270 */
static t_Data_Item_Desc desc_i010_280;
                   /* Description of data item I010/280 */
static t_Data_Item_Desc desc_i010_300;
                   /* Description of data item I010/300 */
static t_Data_Item_Desc desc_i010_310;
                   /* Description of data item I010/310 */
static t_Data_Item_Desc desc_i010_500;
                   /* Description of data item I010/500 */
static t_Data_Item_Desc desc_i010_550;
                   /* Description of data item I010/550 */
static t_Data_Item_Desc desc_i010_ref;
                   /* Description of REF data item */
static t_Data_Item_Desc desc_i010_spf;
                   /* Description of SPF data item */
static t_Bool initiated = FALSE;
				   /* Package initiated flag */
static t_Ui16 reference_vsn = 3;
                   /* Current version of reference document */
                   /* Implemented editions:
                      1 ... Sensis (April 5, 2002) for Heathrow MDS
                     -2 ... 
                      3 ... 1.1 (March 2007)
                   */
static t_Data_Item_Desc *std_uap[M_MAX_FRN + 1];
				   /* Standard User Application Profile */

				   /* Local functions: */
				   /* ---------------- */

static void init_desc (void);
                   /* Initiate data item descriptions */
static void load_std_uap (void);
                   /* Load standard User Application Profile (UAP) */
static t_Retc proc_i010_000 (t_Ui16 length, t_Byte *buffer);
                   /* Process I010/000 data item */
static t_Retc proc_i010_010 (t_Ui16 length, t_Byte *buffer);
                   /* Process I010/010 data item */
static t_Retc proc_i010_020 (t_Ui16 length, t_Byte *buffer);
                   /* Process I010/020 data item */
static t_Retc proc_i010_040 (t_Ui16 length, t_Byte *buffer);
                   /* Process I010/040 data item */
static t_Retc proc_i010_041 (t_Ui16 length, t_Byte *buffer);
                   /* Process I010/041 data item */
static t_Retc proc_i010_042 (t_Ui16 length, t_Byte *buffer);
                   /* Process I010/042 data item */
static t_Retc proc_i010_060 (t_Ui16 length, t_Byte *buffer);
                   /* Process I010/060 data item */
static t_Retc proc_i010_090 (t_Ui16 length, t_Byte *buffer);
                   /* Process I010/090 data item */
static t_Retc proc_i010_091 (t_Ui16 length, t_Byte *buffer);
                   /* Process I010/091 data item */
static t_Retc proc_i010_131 (t_Ui16 length, t_Byte *buffer);
                   /* Process I010/131 data item */
static t_Retc proc_i010_140 (t_Ui16 length, t_Byte *buffer);
                   /* Process I010/140 data item */
static t_Retc proc_i010_161 (t_Ui16 length, t_Byte *buffer);
                   /* Process I010/161 data item */
static t_Retc proc_i010_170 (t_Ui16 length, t_Byte *buffer);
                   /* Process I010/170 data item */
static t_Retc proc_i010_200 (t_Ui16 length, t_Byte *buffer);
                   /* Process I010/200 data item */
static t_Retc proc_i010_202 (t_Ui16 length, t_Byte *buffer);
                   /* Process I010/202 data item */
static t_Retc proc_i010_210 (t_Ui16 length, t_Byte *buffer);
                   /* Process I010/210 data item */
static t_Retc proc_i010_220 (t_Ui16 length, t_Byte *buffer);
                   /* Process I010/220 data item */
static t_Retc proc_i010_245 (t_Ui16 length, t_Byte *buffer);
                   /* Process I010/245 data item */
static t_Retc proc_i010_250 (t_Ui16 length, t_Byte *buffer);
                   /* Process I010/250 data item */
static t_Retc proc_i010_270 (t_Ui16 length, t_Byte *buffer);
                   /* Process I010/270 data item */
static t_Retc proc_i010_280 (t_Ui16 length, t_Byte *buffer);
                   /* Process I010/280 data item */
static t_Retc proc_i010_300 (t_Ui16 length, t_Byte *buffer);
                   /* Process I010/300 data item */
static t_Retc proc_i010_310 (t_Ui16 length, t_Byte *buffer);
                   /* Process I010/310 data item */
static t_Retc proc_i010_500 (t_Ui16 length, t_Byte *buffer);
                   /* Process I010/500 data item */
static t_Retc proc_i010_550 (t_Ui16 length, t_Byte *buffer);
                   /* Process I010/550 data item */
static t_Retc proc_i010_ref (t_Ui16 length, t_Byte *buffer, t_Ui16 *pos_ptr);
                   /* Process REF indicator data item */
static t_Retc proc_i010_spf (t_Ui16 length, t_Byte *buffer, t_Ui16 *pos_ptr);
                   /* Process SPF indicator data item */

/*----------------------------------------------------------------------------*/
/* astx_010        -- Process ASTERIX category 010 data block                 */
/*----------------------------------------------------------------------------*/

 t_Retc astx_010 (t_Ui16 length, t_Byte *buffer)
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

                   /* Preset multilateration report information: */
		memset (&mlat, 0, sizeof (mlat));

                   /* Preset detection type: */
		mlat.detection_type.present = TRUE;
		mlat.detection_type.reported_from_mlt = e_is_true;

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

					lrc = data_item (10, frn, std_uap[frn],
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
            mlat.frame_date.present = TRUE;
            mlat.frame_date.value = frame_date;
        }

                   /* Add frame time, if available: */
		if (frame_time_present)
		{
			mlat.frame_time.present = TRUE;
			mlat.frame_time.value = frame_time;
		}

                   /* Process this multilateration report: */
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
                   /* Description of data item I010/000: */
	desc_i010_000.category = 10;
	desc_i010_000.data_item = 0;
	desc_i010_000.item_type = e_fixed_length_data_item;
	desc_i010_000.fixed_length = 1;
	desc_i010_000.proc_fptr = proc_i010_000;
	desc_i010_000.read_fptr = NULL;

                   /* Description of data item I010/010: */
	desc_i010_010.category = 10;
	desc_i010_010.data_item = 10;
	desc_i010_010.item_type = e_fixed_length_data_item;
	desc_i010_010.fixed_length = 2;
	desc_i010_010.proc_fptr = proc_i010_010;
	desc_i010_010.read_fptr = NULL;

                   /* Description of data item I010/020: */
	desc_i010_020.category = 10;
	desc_i010_020.data_item = 20;
	desc_i010_020.item_type = e_variable_length_data_item;
	desc_i010_020.fixed_length = 0;
	desc_i010_020.proc_fptr = proc_i010_020;
	desc_i010_020.read_fptr = NULL;

                   /* Description of data item I010/040: */
	desc_i010_040.category = 10;
	desc_i010_040.data_item = 40;
	desc_i010_040.item_type = e_fixed_length_data_item;
	desc_i010_040.fixed_length = 4;
	desc_i010_040.proc_fptr = proc_i010_040;
	desc_i010_040.read_fptr = NULL;

                   /* Description of data item I010/041: */
	desc_i010_041.category = 10;
	desc_i010_041.data_item = 41;
	desc_i010_041.item_type = e_fixed_length_data_item;
	desc_i010_041.fixed_length = 8;
	desc_i010_041.proc_fptr = proc_i010_041;
	desc_i010_041.read_fptr = NULL;

                   /* Description of data item I010/042: */
	desc_i010_042.category = 10;
	desc_i010_042.data_item = 42;
	desc_i010_042.item_type = e_fixed_length_data_item;
	desc_i010_042.fixed_length = 4;
	desc_i010_042.proc_fptr = proc_i010_042;
	desc_i010_042.read_fptr = NULL;

                   /* Description of data item I010/060: */
	desc_i010_060.category = 10;
	desc_i010_060.data_item = 60;
	desc_i010_060.item_type = e_fixed_length_data_item;
	desc_i010_060.fixed_length = 2;
	desc_i010_060.proc_fptr = proc_i010_060;
	desc_i010_060.read_fptr = NULL;

                   /* Description of data item I010/090: */
	desc_i010_090.category = 10;
	desc_i010_090.data_item = 90;
	desc_i010_090.item_type = e_fixed_length_data_item;
	desc_i010_090.fixed_length = 2;
	desc_i010_090.proc_fptr = proc_i010_090;
	desc_i010_090.read_fptr = NULL;

                   /* Description of data item I010/091: */
	desc_i010_091.category = 10;
	desc_i010_091.data_item = 91;
	desc_i010_091.item_type = e_fixed_length_data_item;
	desc_i010_091.fixed_length = 2;
	desc_i010_091.proc_fptr = proc_i010_091;
	desc_i010_091.read_fptr = NULL;

                   /* Description of data item I010/131: */
	desc_i010_131.category = 10;
	desc_i010_131.data_item = 131;
	desc_i010_131.item_type = e_fixed_length_data_item;
	desc_i010_131.fixed_length = 1;
	desc_i010_131.proc_fptr = proc_i010_131;
	desc_i010_131.read_fptr = NULL;

                   /* Description of data item I010/140: */
	desc_i010_140.category = 10;
	desc_i010_140.data_item = 140;
	desc_i010_140.item_type = e_fixed_length_data_item;
	desc_i010_140.fixed_length = 3;
	desc_i010_140.proc_fptr = proc_i010_140;
	desc_i010_140.read_fptr = NULL;

                   /* Description of data item I010/161: */
	desc_i010_161.category = 10;
	desc_i010_161.data_item = 161;
	desc_i010_161.item_type = e_fixed_length_data_item;
	desc_i010_161.fixed_length = 2;
	desc_i010_161.proc_fptr = proc_i010_161;
	desc_i010_161.read_fptr = NULL;

                   /* Description of data item I010/170: */
	desc_i010_170.category = 10;
	desc_i010_170.data_item = 170;
	desc_i010_170.item_type = e_variable_length_data_item;
	desc_i010_170.fixed_length = 0;
	desc_i010_170.proc_fptr = proc_i010_170;
	desc_i010_170.read_fptr = NULL;

                   /* Description of data item I010/200: */
	desc_i010_200.category = 10;
	desc_i010_200.data_item = 200;
	desc_i010_200.item_type = e_fixed_length_data_item;
	desc_i010_200.fixed_length = 4;
	desc_i010_200.proc_fptr = proc_i010_200;
	desc_i010_200.read_fptr = NULL;

                   /* Description of data item I010/202: */
	desc_i010_202.category = 10;
	desc_i010_202.data_item = 202;
	desc_i010_202.item_type = e_fixed_length_data_item;
	if (reference_vsn == 1)
	{
		desc_i010_202.fixed_length = 3;
	}
	else if (reference_vsn == 3)
	{
		desc_i010_202.fixed_length = 4;
	}
	else
	{
		Assert (FALSE, "Unexpected reference_vsn");
	}
	desc_i010_202.proc_fptr = proc_i010_202;
	desc_i010_202.read_fptr = NULL;

                   /* Description of data item I010/210: */
	desc_i010_210.category = 10;
	desc_i010_210.data_item = 210;
	desc_i010_210.item_type = e_fixed_length_data_item;
	desc_i010_210.fixed_length = 2;
	desc_i010_210.proc_fptr = proc_i010_210;
	desc_i010_210.read_fptr = NULL;

                   /* Description of data item I010/220: */
	desc_i010_220.category = 10;
	desc_i010_220.data_item = 220;
	desc_i010_220.item_type = e_fixed_length_data_item;
	desc_i010_220.fixed_length = 3;
	desc_i010_220.proc_fptr = proc_i010_220;
	desc_i010_220.read_fptr = NULL;

                   /* Description of data item I010/245: */
	desc_i010_245.category = 10;
	desc_i010_245.data_item = 245;
	desc_i010_245.item_type = e_fixed_length_data_item;
	desc_i010_245.fixed_length = 7;
	desc_i010_245.proc_fptr = proc_i010_245;
	desc_i010_245.read_fptr = NULL;

                   /* Description of data item I010/250: */
	desc_i010_250.category = 10;
	desc_i010_250.data_item = 250;
	desc_i010_250.item_type = e_repetitive_data_item;
	desc_i010_250.fixed_length = 8;
	desc_i010_250.proc_fptr = proc_i010_250;
	desc_i010_250.read_fptr = NULL;

                   /* Description of data item I010/270: */
	desc_i010_270.category = 10;
	desc_i010_270.data_item = 270;
	desc_i010_270.item_type = e_variable_length_data_item;
	desc_i010_270.fixed_length = 0;
	desc_i010_270.proc_fptr = proc_i010_270;
	desc_i010_270.read_fptr = NULL;

                   /* Description of data item I010/280: */
	desc_i010_280.category = 10;
	desc_i010_280.data_item = 280;
	desc_i010_280.item_type = e_repetitive_data_item;
	desc_i010_280.fixed_length = 2;
	desc_i010_280.proc_fptr = proc_i010_280;
	desc_i010_280.read_fptr = NULL;

                   /* Description of data item I010/300: */
	desc_i010_300.category = 10;
	desc_i010_300.data_item = 300;
	desc_i010_300.item_type = e_fixed_length_data_item;
	desc_i010_300.fixed_length = 1;
	desc_i010_300.proc_fptr = proc_i010_300;
	desc_i010_300.read_fptr = NULL;

                   /* Description of data item I010/310: */
	desc_i010_310.category = 10;
	desc_i010_310.data_item = 310;
	desc_i010_310.item_type = e_fixed_length_data_item;
	desc_i010_310.fixed_length = 1;
	desc_i010_310.proc_fptr = proc_i010_310;
	desc_i010_310.read_fptr = NULL;

                   /* Description of data item I010/500: */
	desc_i010_500.category = 10;
	desc_i010_500.data_item = 500;
	desc_i010_500.item_type = e_fixed_length_data_item;
	desc_i010_500.fixed_length = 4;
	desc_i010_500.proc_fptr = proc_i010_500;
	desc_i010_500.read_fptr = NULL;

                   /* Description of data item I010/550: */
	desc_i010_550.category = 10;
	desc_i010_550.data_item = 550;
	desc_i010_550.item_type = e_fixed_length_data_item;
	desc_i010_550.fixed_length = 1;
	desc_i010_550.proc_fptr = proc_i010_550;
	desc_i010_550.read_fptr = NULL;

                   /* Description of data item I010/REF: */
	desc_i010_ref.category = 10;
	desc_i010_ref.data_item = M_REF_INDICATOR;
	desc_i010_ref.item_type = e_immediate_data_item;
	desc_i010_ref.fixed_length = 0;
	desc_i010_ref.proc_fptr = NULL;
	desc_i010_ref.read_fptr = proc_i010_ref;

                   /* Description of data item I010/SPF: */
	desc_i010_spf.category = 10;
	desc_i010_spf.data_item = M_SPF_INDICATOR;
	desc_i010_spf.item_type = e_immediate_data_item;
	desc_i010_spf.fixed_length = 0;
	desc_i010_spf.proc_fptr = NULL;
	desc_i010_spf.read_fptr = proc_i010_spf;

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

	std_uap[1] = &desc_i010_010;
	std_uap[2] = &desc_i010_000;
	std_uap[3] = &desc_i010_020;
	std_uap[4] = &desc_i010_140;
	std_uap[5] = &desc_i010_041;
	if (reference_vsn == 3)
	{
		std_uap[6] = &desc_i010_040;
	}
	std_uap[7] = &desc_i010_042;

	if (reference_vsn == 3)
	{
		std_uap[8] = &desc_i010_200;
	}
	std_uap[9] = &desc_i010_202;
	std_uap[10] = &desc_i010_161;
	std_uap[11] = &desc_i010_170;
	std_uap[12] = &desc_i010_060;
	std_uap[13] = &desc_i010_220;
	std_uap[14] = &desc_i010_245;

	if (reference_vsn == 3)
	{
		std_uap[15] = &desc_i010_250;
		std_uap[16] = &desc_i010_300;
	}
	std_uap[17] = &desc_i010_090;
	std_uap[18] = &desc_i010_091;
	if (reference_vsn == 3)
	{
		std_uap[19] = &desc_i010_270;
	}
	std_uap[20] = &desc_i010_550;
	if (reference_vsn == 3)
	{
		std_uap[21] = &desc_i010_310;
	}

	std_uap[22] = &desc_i010_500;
	if (reference_vsn == 3)
	{
		std_uap[23] = &desc_i010_280;
		std_uap[24] = &desc_i010_131;
		std_uap[25] = &desc_i010_210;
	}
	std_uap[27] = &desc_i010_spf;
	std_uap[28] = &desc_i010_ref;

	return;
}

/*----------------------------------------------------------------------------*/
/* proc_i010_000   -- Process I010/000 data item                              */
/*----------------------------------------------------------------------------*/

 static
 t_Retc proc_i010_000 (t_Ui16 length, t_Byte *buffer)
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
	list_text (2, " " M_FMT_BYTE, mtp);
	switch (mtp)
	{
	case 1:
		list_text (2, " (Target Report)");
		break;
	case 2:
		list_text (2, " (Start of Update Cycle)");
		break;
	case 3:
		list_text (2, " (Periodic Status Message)");
		break;
	case 4:
		list_text (2, " (Event-triggered Status Message)");
		break;
	default:
		break;
	}
	list_text (2, "\n");
#endif /* LISTER */

                   /* Store this information: */
	if (mtp == 1)
	{
		mlat.detection_type.present = TRUE;
		mlat.detection_type.reported_from_mlt = e_is_true;
	}

	/* tba ... */

				   /* Set the return code: */
	ret = RC_OKAY;

	return ret;
}

/*----------------------------------------------------------------------------*/
/* proc_i010_010   -- Process I010/010 data item                              */
/*----------------------------------------------------------------------------*/

 static
 t_Retc proc_i010_010 (t_Ui16 length, t_Byte *buffer)
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
/* proc_i010_020   -- Process I010/020 data item                              */
/*----------------------------------------------------------------------------*/

 static
 t_Retc proc_i010_020 (t_Ui16 length, t_Byte *buffer)
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
				list_text (2, " DCR");
			}

			if (dfo & 0x08)
			{
				list_text (2, " CHN-2");
			}
			else
			{
				list_text (2, " CHN-1");
			}

			if (dfo & 0x04)
			{
				list_text (2, " GBS");
			}

			if (dfo & 0x02)
			{
				list_text (2, " CRT");
			}
		}
		else if (ix == 1)
		{
			t_Byte lop;
			t_Byte tot;

			if (dfo & 0x80)
			{
				list_text (2, " SIM");
			}

			if (dfo & 0x40)
			{
				list_text (2, " TST");
			}

			if (dfo & 0x20)
			{
				list_text (2, " RAB");
			}

			lop = (dfo >> 3) & 0x03;
			list_text (2, " LOP=%u", lop);

			tot = (dfo >> 1) & 0x03;
			list_text (2, " TOT=%u", tot);
		}
		else if (ix == 2)
		{
			if (dfo & 0x80)
			{
				list_text (2, " SPI");
			}
		}
	}
	list_text (2, "\n");
#endif /* LISTER */

                   /* Store this information: */
	for (ix = 0; ix < length; ix ++)
	{
		dfo = buffer[ix];

		if (ix == 0)
		{
			t_Byte typ;

			typ = (dfo >> 5) & 0x07;

			if (reference_vsn == 1)
			{
				if (typ == 1)
				{
					mlat.detected_by_mds = e_is_true;
				}

				if (typ == 2)
				{
					mlat.detected_by_adsb = e_is_true;
				}
			}

			if (typ == 3)
			{
				mlat.detection_type.reported_from_psr = e_is_true;
			}

			if (dfo & 0x04)
			{
				mlat.ground_bit_set = TRUE;
			}

			/* tba ... */
		}
		else if (ix == 1)
		{
			mlat.detection_type.simulated = M_TRES (dfo & 0x80);

			mlat.detection_type.test_target = M_TRES (dfo & 0x40);

			mlat.detection_type.from_fixed_field_transponder =
				M_TRES (dfo & 0x20);

			/* tba ... */
		}
		else if (ix == 2)
		{
			mlat.special_position_indication = M_TRES (dfo & 0x80);
		}
	}

                   /* Set the return code: */
	ret = RC_OKAY;

	return ret;
}

/*----------------------------------------------------------------------------*/
/* proc_i010_040   -- Process I010/040 data item                              */
/*----------------------------------------------------------------------------*/

 static
 t_Retc proc_i010_040 (t_Ui16 length, t_Byte *buffer)
{
#if LISTER
	t_Byte df1;    /* Data field octet 1 */
	t_Byte df2;    /* Data field octet 2 */
	t_Byte df3;    /* Data field octet 3 */
	t_Byte df4;    /* Data field octet 4 */
#endif /* LISTER */
	t_Retc ret;    /* Return code */
#if LISTER
	t_Ui16 rho;    /* Slant range; metres */
	t_Ui16 theta;  /* Azimuth; 360/2**16 degrees */
#endif /* LISTER */

				   /* Preset the return code: */
	ret = RC_FAIL;

				   /* Check parameters: */
	Assert (length == 4, "Invalid parameter");
	Assert (buffer != NULL, "Invalid parameter");

                   /* Check reference version: */
	Assert (reference_vsn == 3, "Data item not defined");

#if LISTER
				   /* Extract octets: */
	df1 = buffer[0];
	df2 = buffer[1];
	df3 = buffer[2];
	df4 = buffer[3];

				   /* Extract information: */
	rho = make_ui16 (df1, df2);
	theta = make_ui16 (df3, df4);

				   /* List data field: */
	list_text (2, ";  Measured Position:");
	list_text (2, " srg=" M_FMT_UI16 " (%.3f nmi);",
                  rho, M_MTR2NMI * rho);
	list_text (2, " azm=" M_FMT_UI16 " (%.3f deg)",
                  theta, (360.0 / 65536.0) * theta);
	list_text (2, "\n");
#endif /* LISTER */

				   /* Store this information: */
	/* tba ... */

				   /* Set the return code: */
	ret = RC_OKAY;

	return ret;
}

/*----------------------------------------------------------------------------*/
/* proc_i010_041   -- Process I010/041 data item                              */
/*----------------------------------------------------------------------------*/

 static
 t_Retc proc_i010_041 (t_Ui16 length, t_Byte *buffer)
{
	t_Byte df1;    /* Data field octet 1 */
	t_Byte df2;    /* Data field octet 2 */
	t_Byte df3;    /* Data field octet 3 */
	t_Byte df4;    /* Data field octet 4 */
	t_Byte df5;    /* Data field octet 5 */
	t_Byte df6;    /* Data field octet 6 */
	t_Byte df7;    /* Data field octet 7 */
	t_Byte df8;    /* Data field octet 8 */
	t_Si32 lat;    /* Latitude; 180/2**31 degrees */
	t_Real latf;   /* Latitude; degrees */
	t_Si32 lon;    /* Longitude; 180/2**31 degrees */
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

                   /* Convert to radians: */
	latf = 180.0 / M_TWO_POWER_31 * lat;
	lonf = 180.0 / M_TWO_POWER_31 * lon;

#if LISTER
				   /* List data field: */
	list_text (2, ";  Position in WGS-84 Co-ordinates:\n");
	list_text (2, ";   lat=" M_FMT_SI32 " (%s);", lat, lat_text (latf));
	list_text (2, " lon=" M_FMT_SI32 " (%s)", lon, lon_text (lonf));
	list_text (2, "\n");
#endif /* LISTER */

                   /* Store this information: */
	mlat.wgs84_position.present = TRUE;
	mlat.wgs84_position.value_lat = M_DEG2RAD * latf;
	mlat.wgs84_position.value_lon = M_DEG2RAD * lonf;
                   /* Values are in radians */

				   /* Set the return code: */
	ret = RC_OKAY;

	return ret;
}

/*----------------------------------------------------------------------------*/
/* proc_i010_042   -- Process I010/042 data item                              */
/*----------------------------------------------------------------------------*/

 static
 t_Retc proc_i010_042 (t_Ui16 length, t_Byte *buffer)
{
	t_Byte df1;    /* Data field octet 1 */
	t_Byte df2;    /* Data field octet 2 */
	t_Byte df3;    /* Data field octet 3 */
	t_Byte df4;    /* Data field octet 4 */
	t_Retc ret;    /* Return code */
	t_Si16 x;      /* x component; metres */
	t_Si16 y;      /* y component; metres */

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
	if (reference_vsn == 1)
	{
		list_text (2, ";  Position in Cartesian Coordinates:");
	}
	else if (reference_vsn == 3)
	{
		list_text (2, ";  Calculated Position:");
	}
	else
	{
		Assert (FALSE, "Unexpected reference_vsn");
	}
	list_text (2, " x=" M_FMT_SI16 " mtr;", x);
	list_text (2, " y=" M_FMT_SI16 " mtr", y);
	list_text (2, "\n");
#endif /* LISTER */

				   /* Store this information: */
	mlat.computed_position.present = TRUE;
	mlat.computed_position.value_x = x;
	mlat.computed_position.value_y = y;

				   /* Set the return code: */
	ret = RC_OKAY;

	return ret;
}

/*----------------------------------------------------------------------------*/
/* proc_i010_060   -- Process I010/060 data item                              */
/*----------------------------------------------------------------------------*/

 static
 t_Retc proc_i010_060 (t_Ui16 length, t_Byte *buffer)
{
	t_Byte df1;    /* Data field octet 1 */
	t_Byte df2;    /* Data field octet 2 */
	t_Ui16 m3a;    /* Mode 3/A code */
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
	m3a = make_ui16 (df1 & 0x0f, df2);

#if LISTER
				   /* List data field: */
	list_text (2, ";  Mode 3/A Code:");
	list_text (2, " v=" M_FMT_BYTE ";", (df1 >> 7) & 0x01);
	list_text (2, " g=" M_FMT_BYTE ";", (df1 >> 6) & 0x01);
	list_text (2, " l=" M_FMT_BYTE ";", (df1 >> 5) & 0x01);
	list_text (2, " code=" M_FMT_CODE, m3a);
	list_text (2, "\n");
#endif /* LISTER */

                   /* Store this information: */
	mlat.mode_3a_info.code = m3a;
	mlat.mode_3a_info.code_garbled = M_TRES (df1 & 0x40);
	mlat.mode_3a_info.code_invalid = M_TRES (df1 & 0x80);
	mlat.mode_3a_info.code_smoothed = M_TRES (df1 & 0x20);
	mlat.mode_3a_info.present = TRUE;

				   /* Set the return code: */
	ret = RC_OKAY;

	return ret;
}

/*----------------------------------------------------------------------------*/
/* proc_i010_090   -- Process I010/090 data item                              */
/*----------------------------------------------------------------------------*/

 static
 t_Retc proc_i010_090 (t_Ui16 length, t_Byte *buffer)
{
	t_Byte df1;    /* Data field octet 1 */
	t_Byte df2;    /* Data field octet 2 */
	t_Si16 mfl;    /* Measured flight level; 1/4 FL */
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
	mfl = make_si16 (df1 & 0x1f, df2);
	if (df1 & 0x20)
	{
		mfl |= 0xc000;
                   /* Two's complement */
	}

#if LISTER
				   /* List data field: */
	list_text (2, ";  Flight Level:");
	list_text (2, " v=" M_FMT_BYTE ";", (df1 >> 7) & 0x01);
	list_text (2, " g=" M_FMT_BYTE ";", (df1 >> 6) & 0x01);
	list_text (2, " fl=%.2f FL", 0.25 * mfl);
	list_text (2, "\n");
#endif /* LISTER */

                   /* Store this information: */
	mlat.mode_c_height.garbled = M_TRES (df1 & 0x40);
	mlat.mode_c_height.height_in_error = e_is_false;
	mlat.mode_c_height.in_25_feet = e_is_true;
	mlat.mode_c_height.invalid = M_TRES (df1 & 0x80);
	mlat.mode_c_height.present = TRUE;
	mlat.mode_c_height.value = (25.0 * M_FT2MTR) * mfl;
	mlat.mode_c_height.value_in_feet = 25 * (t_Si32) mfl;
                   /* Value in feet */

				   /* Set the return code: */
	ret = RC_OKAY;

	return ret;
}

/*----------------------------------------------------------------------------*/
/* proc_i010_091   -- Process I010/091 data item                              */
/*----------------------------------------------------------------------------*/

 static
 t_Retc proc_i010_091 (t_Ui16 length, t_Byte *buffer)
{
	t_Byte df1;    /* Data field octet 1 */
	t_Byte df2;    /* Data field octet 2 */
	t_Si16 mh;     /* Measured height; 6.25 ft */
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
	mh = make_si16 (df1, df2);

#if LISTER
				   /* List data field: */
	list_text (2, ";  Measured Height:");
	list_text (2, " %.2f FL", 0.0625 * mh);
	list_text (2, "\n");
#endif /* LISTER */

                   /* Store this information: */
	mlat.measured_height.present = TRUE;
	mlat.measured_height.value = (6.25 * M_FT2MTR) * mh;

				   /* Set the return code: */
	ret = RC_OKAY;

	return ret;
}

/*----------------------------------------------------------------------------*/
/* proc_i010_131   -- Process I010/131 data item                              */
/*----------------------------------------------------------------------------*/

 static
 t_Retc proc_i010_131 (t_Ui16 length, t_Byte *buffer)
{
#if LISTER
	t_Byte df1;    /* Data field octet 1 */
	t_Byte pam;    /* Primary plot amplitude */
#endif /* LISTER */
	t_Retc ret;    /* Return code */

				   /* Preset the return code: */
	ret = RC_FAIL;

				   /* Check parameters: */
	Assert (length == 1, "Invalid parameter");
	Assert (buffer != NULL, "Invalid parameter");

                   /* Check reference version: */
	Assert (reference_vsn == 3, "Data item not defined");

#if LISTER
				   /* Extract octets: */
	df1 = buffer[0];

				   /* Extract information: */
	pam = df1;

				   /* List data field: */
	list_text (2, ";  Amplitude of Primary Plot:");
	list_text (2, " pam=" M_FMT_BYTE, pam);
	list_text (2, "\n");
#endif /* LISTER */

                   /* Store this information: */
	/* tba ... */

				   /* Set the return code: */
	ret = RC_OKAY;

	return ret;
}

/*----------------------------------------------------------------------------*/
/* proc_i010_140   -- Process I010/140 data item                              */
/*----------------------------------------------------------------------------*/

 static
 t_Retc proc_i010_140 (t_Ui16 length, t_Byte *buffer)
{
	t_Byte df1;    /* Data field octet 1 */
	t_Byte df2;    /* Data field octet 2 */
	t_Byte df3;    /* Data field octet 3 */
	t_Retc ret;    /* Return code */
	t_Secs tm;     /* Absolute time; seconds */
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
	tod = make_ui32 (0x00, df1, df2, df3);

				   /* Compute absolute time: */
	tm = (1.0 / 128.0) * tod;

#if LISTER
				   /* List data field: */
	list_text (2, ";  Time of Day:");
	list_text (2, " tod=" M_FMT_ATOD " (" M_FMT_UI32 "; %s UTC)",
                  tod, tod, utc_text (tm));
	list_text (2, "\n");
#endif /* LISTER */

                   /* Remember this information: */
	last_tod_available = TRUE;
	last_tod = tod;

                   /* Store this information: */
	mlat.detection_time.present = TRUE;
	mlat.detection_time.value = tm;

				   /* Set the return code: */
	ret = RC_OKAY;

	return ret;
}

/*----------------------------------------------------------------------------*/
/* proc_i010_161   -- Process I010/161 data item                              */
/*----------------------------------------------------------------------------*/

 static
 t_Retc proc_i010_161 (t_Ui16 length, t_Byte *buffer)
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
	tno = make_ui16 (df1 & 0x0f, df2);

#if LISTER
				   /* List data field: */
	list_text (2, ";  Track Number:");
	list_text (2, " tno=" M_FMT_UI16, tno);
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
/* proc_i010_170   -- Process I010/170 data item                              */
/*----------------------------------------------------------------------------*/

 static
 t_Retc proc_i010_170 (t_Ui16 length, t_Byte *buffer)
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
	list_text (2, ";  Track Status:\n");
	for (ix = 0; ix < length; ix ++)
	{
		t_Byte cst;

		df1 = buffer[ix];

		if (ix == 0)
		{
			list_text (2, ";   ");

			if (df1 & 0x80)
			{
				list_text (2, "TNT");
			}
			else
			{
				list_text (2, "CON");
			}

			if (df1 & 0x40)
			{
				list_text (2, " TRE");
			}

			cst = (df1 >> 4) & 0x03;
			list_text (2, " CST=" M_FMT_BYTE, cst);

			if (df1 & 0x08)
			{
				list_text (2, " MAH");
			}

			list_text (2, " TCC=" M_FMT_BYTE, (df1 >> 2) & 0x01);

			list_text (2, " STH=" M_FMT_BYTE, (df1 >> 1) & 0x01);
		}
		else if (ix == 1)
		{
			t_Byte dou;
			t_Byte mrs;
			t_Byte tom;

			list_text (2, ";  ");

			tom = (df1 >> 6) & 0x03;
			list_text (2, " TOM=" M_FMT_BYTE, tom);

			dou = (df1 >> 3) & 0x07;
			list_text (2, " DOU=" M_FMT_BYTE, dou);

			mrs = (df1 >> 1) & 0x03;
			list_text (2, " mrs=" M_FMT_BYTE, mrs);
		}
		else if (ix == 2)
		{
			if (df1 & 0x80)
			{
				list_text (2, " GHO");
			}
			list_text (2, "\n");
		}
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
/* proc_i010_200   -- Process I010/200 data item                              */
/*----------------------------------------------------------------------------*/

 static
 t_Retc proc_i010_200 (t_Ui16 length, t_Byte *buffer)
{
	t_Byte df1;    /* Data field octet 1 */
	t_Byte df2;    /* Data field octet 2 */
	t_Byte df3;    /* Data field octet 3 */
	t_Byte df4;    /* Data field octet 4 */
	t_Ui16 hdg;    /* Track angle; 360/2**16 degrees */
	t_Retc ret;    /* Return code */
	t_Ui16 spd;    /* Ground speed; 2**-14 nmi/second */

				   /* Preset the return code: */
	ret = RC_FAIL;

				   /* Check parameters: */
	Assert (length == 4, "Invalid parameter");
	Assert (buffer != NULL, "Invalid parameter");

                   /* Check reference version: */
	Assert (reference_vsn == 3, "Data item not defined");

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
	mlat.ground_vector.present = TRUE;
	mlat.ground_vector.value_gsp = (M_NMI2MTR / 16384.0) * spd;
	mlat.ground_vector.value_hdg = (M_TWO_PI / 65536.0) * hdg;
                   /* Value is in radians */

				   /* Set the return code: */
	ret = RC_OKAY;

	return ret;
}

/*----------------------------------------------------------------------------*/
/* proc_i010_202   -- Process I010/202 data item                              */
/*----------------------------------------------------------------------------*/

 static
 t_Retc proc_i010_202 (t_Ui16 length, t_Byte *buffer)
{
	t_Byte df1;    /* Data field octet 1 */
	t_Byte df2;    /* Data field octet 2 */
	t_Byte df3;    /* Data field octet 3 */
	t_Byte df4;    /* Data field octet 4 */
	t_Retc ret;    /* Return code */
	t_Si16 vx;     /* x velocity component; m/s or 0.25 m/s */
	t_Real vxf;    /* x velocity component; m/s */
	t_Si16 vy;     /* y velocity component; m/s or 0.25 m/s */
	t_Real vyf;    /* y velocity component; m/s */

				   /* Preset the return code: */
	ret = RC_FAIL;

				   /* Check parameters: */
	if (reference_vsn == 1)
	{
		Assert (length == 3, "Invalid parameter");
	}
	else if (reference_vsn == 3)
	{
		Assert (length == 4, "Invalid parameter");
	}
	else
	{
		Assert (FALSE, "Unexpected reference_vsn");
	}
	Assert (buffer != NULL, "Invalid parameter");

				   /* Extract octets: */
	df1 = buffer[0];
	df2 = buffer[1];
	df3 = buffer[2];
	if (reference_vsn == 3)
	{
		df4 = buffer[3];
	}

				   /* Extract information: */
	vx = 0;
	vy = 0;
	if (reference_vsn == 1)
	{
		vx = df1;
		vx <<= 4;
		vx |= ((df2 >> 4) & 0x0f);

		if (df1 & 0x80)
		{
			vx |= 0xf000;
		}

		vy = (df2 & 0x0f);
		vy <<= 8;
		vy |= df3;

		if (df2 & 0x08)
		{
			vy |= 0xf000;
		}
	}
	else if (reference_vsn == 3)
	{
		vx = make_si16 (df1, df2);
		vy = make_si16 (df3, df4);
	}

                   /* Extract velocity components: */
	vxf = 0.0;
	vyf = 0.0;
	if (reference_vsn == 1)
	{
		vxf = (t_Real) vx;
		vyf = (t_Real) vy;
	}
	else if (reference_vsn == 3)
	{
		vxf = 0.25 * (t_Real) vx;
		vyf = 0.25 * (t_Real) vy;
	}

#if LISTER
				   /* List data field: */
	list_text (2, ";  Calculated Track Velocity in Cartesian"
                  " Co-ordinates:");
	if (reference_vsn == 1 || reference_vsn == 3)
	{
		list_text (2, " vx=%.2f m/s; vy=%.2f m/s", vxf, vyf);
	}
	list_text (2, "\n");
#endif /* LISTER */

                   /* Store this information: */
	mlat.computed_velocity.present = TRUE;
	mlat.computed_velocity.value_vx = vxf;
	mlat.computed_velocity.value_vy = vyf;

				   /* Set the return code: */
	ret = RC_OKAY;

	return ret;
}

/*----------------------------------------------------------------------------*/
/* proc_i010_210   -- Process I010/210 data item                              */
/*----------------------------------------------------------------------------*/

 static
 t_Retc proc_i010_210 (t_Ui16 length, t_Byte *buffer)
{
#if LISTER
	t_Si16 ax;     /* x acceleration; 0.25 metres/second**2 */
	t_Si16 ay;     /* y acceleration; 0.25 metres/second**2 */
	t_Byte df1;    /* Data field octet 1 */
	t_Byte df2;    /* Data field octet 2 */
#endif /* LISTER */
	t_Retc ret;    /* Return code */

				   /* Preset the return code: */
	ret = RC_FAIL;

				   /* Check parameters: */
	Assert (length == 2, "Invalid parameter");
	Assert (buffer != NULL, "Invalid parameter");

                   /* Check reference version: */
	Assert (reference_vsn == 3, "Data item not defined");

#if LISTER
				   /* Extract octets: */
	df1 = buffer[0];
	df2 = buffer[1];

				   /* Extract information: */
	if (df1 & 0x80)
	{
		ax = (t_Si16) (0xff00 | df1);
	}
	else
	{
		ax = (t_Si16) (0x00ff & df1);
	}
	if (df2 & 0x80)
	{
		ay = (t_Si16) (0xff00 | df2);
	}
	else
	{
		ay = (t_Si16) (0x00ff & df2);
	}

				   /* List data field: */
	list_text (2, ";  Calculated Acceleration:");
	list_text (2, " ax=%.2f [m/s**2];", 0.25 * ax);
	list_text (2, " ay=%.2f [m/s**2]", 0.25 * ay);
	list_text (2, "\n");
#endif /* LISTER */

                   /* Store this information: */
	/* tba ... */

				   /* Set the return code: */
	ret = RC_OKAY;

	return ret;
}

/*----------------------------------------------------------------------------*/
/* proc_i010_220   -- Process I010/220 data item                              */
/*----------------------------------------------------------------------------*/

 static
 t_Retc proc_i010_220 (t_Ui16 length, t_Byte *buffer)
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
	mlat.target_address.present = TRUE;
	mlat.target_address.value = aa;

				   /* Set the return code: */
	ret = RC_OKAY;

	return ret;
}

/*----------------------------------------------------------------------------*/
/* proc_i010_245   -- Process I010/245 data item                              */
/*----------------------------------------------------------------------------*/

 static
 t_Retc proc_i010_245 (t_Ui16 length, t_Byte *buffer)
{
	t_Byte c1;     /* Character 1 */
	t_Byte c2;     /* Character 2 */
	t_Byte c3;     /* Character 3 */
	t_Byte c4;     /* Character 4 */
	t_Byte c5;     /* Character 5 */
	t_Byte c6;     /* Character 6 */
	t_Byte c7;     /* Character 7 */
	t_Byte c8;     /* Character 8 */
#if CCWARN
	t_Byte df1;    /* Data field octet 1 */
#endif /* CCWARN */
	t_Byte df2;    /* Data field octet 2 */
	t_Byte df3;    /* Data field octet 3 */
	t_Byte df4;    /* Data field octet 4 */
	t_Byte df5;    /* Data field octet 5 */
	t_Byte df6;    /* Data field octet 6 */
	t_Byte df7;    /* Data field octet 7 */
	t_Retc ret;    /* Return code */
#if CCWARN
	t_Byte sti;    /* Source of target identification */
#endif /* CCWARN */
	char txt[M_AIRCRAFT_IDENTIFICATION_LENGTH + 1];
                   /* Text buffer */

				   /* Preset the return code: */
	ret = RC_FAIL;

				   /* Check parameters: */
	Assert (length == 7, "Invalid parameter");
	Assert (buffer != NULL, "Invalid parameter");

				   /* Extract octets: */
#if CCWARN
	df1 = buffer[0];
#endif /* CCWARN */
	df2 = buffer[1];
	df3 = buffer[2];
	df4 = buffer[3];
	df5 = buffer[4];
	df6 = buffer[5];
	df7 = buffer[6];

				   /* Extract information: */
#if CCWARN
	sti = (df1 >> 6) & 0x03;
#endif /* CCWARN */
	c1 = (df2 >> 2) & 0x3f;
	c2 = df2 & 0x03;
	c2 <<= 4;
	c2 |= (df3 >> 4) & 0x0f;
	c3 = df3 & 0x0f;
	c3 <<= 2;
	c3 |= (df4 >> 6) & 0x03;
	c4 = df4 & 0x3f;
	c5 = (df5 >> 2) & 0x3f;
	c6 = df5 & 0x03;
	c6 <<= 4;
	c6 |= (df6 >> 4) & 0x0f;
	c7 = df6 & 0x0f;
	c7 <<= 2;
	c7 |= (df7 >> 6) & 0x03;
	c8 = df7 & 0x3f;

                   /* Convert to conventional text: */
	memset (txt, 0, M_AIRCRAFT_IDENTIFICATION_LENGTH + 1);
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
	list_text (2, ";  Target Identification: \"%s\"", txt);
	list_text (2, "\n");
#endif /* LISTER */

                   /* Store this information: */
	mlat.target_identification.present = TRUE;
	strncpy (mlat.target_identification.value_idt,
             txt, M_AIRCRAFT_IDENTIFICATION_LENGTH);

				   /* Set the return code: */
	ret = RC_OKAY;

	return ret;
}

/*----------------------------------------------------------------------------*/
/* proc_i010_250   -- Process I010/250 data item                              */
/*----------------------------------------------------------------------------*/

 static
 t_Retc proc_i010_250 (t_Ui16 length, t_Byte *buffer)
{
#if LISTER
	t_Byte bds1;   /* BDS 1 */
	t_Byte bds2;   /* BDS 2 */
	t_Byte df1;    /* Data field octet 1 */
	t_Byte df2;    /* Data field octet 2 */
	t_Byte df3;    /* Data field octet 3 */
	t_Byte df4;    /* Data field octet 4 */
	t_Byte df5;    /* Data field octet 5 */
	t_Byte df6;    /* Data field octet 6 */
	t_Byte df7;    /* Data field octet 7 */
	t_Byte df8;    /* Data field octet 8 */
	int ix;        /* Auxiliary */
#endif /* LISTER */
	t_Byte rep;    /* Repetition factor */
	t_Retc ret;    /* Return code */

				   /* Preset the return code: */
	ret = RC_FAIL;

				   /* Check parameters: */
	Assert (length > 0, "Invalid parameter");
	Assert (buffer != NULL, "Invalid parameter");

                   /* Check reference version: */
	Assert (reference_vsn == 3, "Data item not defined");

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

		list_text (2, ";   BDS1=" M_FMT_BYTE ", BDS2=" M_FMT_BYTE "; data=0x"
                      " " M_FMT_BYTE_HEX " " M_FMT_BYTE_HEX
                      " " M_FMT_BYTE_HEX " " M_FMT_BYTE_HEX
                      " " M_FMT_BYTE_HEX " " M_FMT_BYTE_HEX
                      " " M_FMT_BYTE_HEX,
                      bds1, bds2,
                      df1, df2, df3, df4, df5, df6, df7);
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
/* proc_i010_270   -- Process I010/270 data item                              */
/*----------------------------------------------------------------------------*/

 static
 t_Retc proc_i010_270 (t_Ui16 length, t_Byte *buffer)
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

                   /* Check reference version: */
	Assert (reference_vsn == 3, "Data item not defined");

#if LISTER
				   /* List data field: */
	list_text (2, ";  Target Size and Orientation:\n");
	for (ix = 0; ix < length; ix ++)
	{
		df1 = (buffer[ix] >> 1) & 0x7f;

		if (ix == 1)
		{
			list_text (2, ";  length=" M_FMT_BYTE "\n", df1);
		}
		else if (ix == 2)
		{
			list_text (2, ";  orientation=" M_FMT_BYTE "\n", df1);
		}
		else if (ix == 3)
		{
			list_text (2, ";  width=" M_FMT_BYTE "\n", df1);
		}
	}
#endif /* LISTER */

                   /* Store this information: */
	/* tba ... */

				   /* Set the return code: */
	ret = RC_OKAY;

	return ret;
}

/*----------------------------------------------------------------------------*/
/* proc_i010_280   -- Process I010/280 data item                              */
/*----------------------------------------------------------------------------*/

 static
 t_Retc proc_i010_280 (t_Ui16 length, t_Byte *buffer)
{
#if CCWARN
	t_Byte df1;    /* Data field octet 1 */
	t_Byte df2;    /* Data field octet 2 */
#endif /* CCWARN */
#if LISTER
	int ix;        /* Auxiliary */
#endif /* LISTER */
	t_Byte rep;    /* Repetition factor */
	t_Retc ret;    /* Return code */

				   /* Preset the return code: */
	ret = RC_FAIL;

				   /* Check parameters: */
	Assert (length > 0, "Invalid parameter");
	Assert (buffer != NULL, "Invalid parameter");

                   /* Check reference version: */
	Assert (reference_vsn == 3, "Data item not defined");

				   /* Get repetition factor: */
	rep = buffer[0];

                   /* Check repetition factor: */
	Assert (rep > 0, "Invalid repetition factor");

				   /* Check length (again): */
	Assert (length == 1 + rep * 2, "Invalid length");

#if LISTER
				   /* List data field: */
	list_text (2, ";  Presence:\n");
	for (ix = 0; ix < rep; ix ++)
	{
#if CCWARN
		df1 = buffer[1 + 2 * ix];
		df2 = buffer[1 + 2 * ix + 1];
#endif /* CCWARN */

		/* tba ... */
	}
#endif /* LISTER */

				   /* Store this information: */
	/* tba ... */

				   /* Set the return code: */
	ret = RC_OKAY;

	return ret;
}

/*----------------------------------------------------------------------------*/
/* proc_i010_300   -- Process I010/300 data item                              */
/*----------------------------------------------------------------------------*/

 static
 t_Retc proc_i010_300 (t_Ui16 length, t_Byte *buffer)
{
#if LISTER
	t_Byte df1;    /* Data field octet 1 */
#endif /* LISTER */
	t_Retc ret;    /* Return code */
#if LISTER
	t_Byte vfi;    /* Vehicle fleet identification */
#endif /* LISTER */

				   /* Preset the return code: */
	ret = RC_FAIL;

				   /* Check parameters: */
	Assert (length == 1, "Invalid parameter");
	Assert (buffer != NULL, "Invalid parameter");

                   /* Check reference version: */
	Assert (reference_vsn == 3, "Data item not defined");

#if LISTER
				   /* Extract octets: */
	df1 = buffer[0];

				   /* Extract information: */
	vfi = df1;

				   /* List data field: */
	list_text (2, ";  Vehicle Fleet Identification: " M_FMT_BYTE, vfi);
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
/* proc_i010_310   -- Process I010/310 data item                              */
/*----------------------------------------------------------------------------*/

 static
 t_Retc proc_i010_310 (t_Ui16 length, t_Byte *buffer)
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

                   /* Check reference version: */
	Assert (reference_vsn == 3, "Data item not defined");

#if LISTER
                   /* Extract octets: */
#if CCWARN
	df1 = buffer[0];
#endif /* CCWARN */

				   /* List data field: */
	list_text (2, ";  Pre-programmed Message:");
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
/* proc_i010_500   -- Process I010/500 data item                              */
/*----------------------------------------------------------------------------*/

 static
 t_Retc proc_i010_500 (t_Ui16 length, t_Byte *buffer)
{
#if LISTER
	t_Byte df1;    /* Data field octet 1 */
	t_Byte df2;    /* Data field octet 2 */
	t_Byte df3;    /* Data field octet 3 */
	t_Byte df4;    /* Data field octet 4 */
#endif /* LISTER */
	t_Retc ret;    /* Return code */
#if LISTER
	t_Ui16 sx;     /* sigma-x; 0.25 metres */
	t_Si16 sxy;    /* sigma-xy; 0.25 metres**2 */
	t_Ui16 sy;     /* sigma-y; 0.25 metres */
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
	sx = (t_Ui16) df1;
	sy = (t_Ui16) df2;
	sxy = make_si16 (df3, df4);

				   /* List data field: */
	list_text (2, ";  Standard Deviation in Position:");
	list_text (2, " sx=%.2f mtr;", 0.25 * sx);
	list_text (2, " sy=%.2f mtr;", 0.25 * sy);
	list_text (2, " sxy=%.2f mtr**2", 0.25 * sxy);
	list_text (2, "\n");
#endif /* LISTER */

				   /* Store this information: */
	/* tba ... */

				   /* Set the return code: */
	ret = RC_OKAY;

	return ret;
}

/*----------------------------------------------------------------------------*/
/* proc_i010_550   -- Process I010/550 data item                              */
/*----------------------------------------------------------------------------*/

 static
 t_Retc proc_i010_550 (t_Ui16 length, t_Byte *buffer)
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

				   /* List data field: */
	list_text (2, ";  System Status:");
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
/* proc_i010_ref   -- Process REF indicator data item                         */
/*----------------------------------------------------------------------------*/

 static
 t_Retc proc_i010_ref (t_Ui16 length, t_Byte *buffer, t_Ui16 *pos_ptr)
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
		error_msg ("Invalid buffer length (I010/REF)");
		goto done;
	}

#if LISTER
				   /* List raw data field: */
	list_buffer (1, ";  I010/REF:", len, buffer + pos);
#endif /* LISTER */

				   /* Set position: */
	*pos_ptr = pos + len;

                   /* Set the return code: */
	ret = RC_OKAY;

	done:          /* We are done */
	return ret;
}

/*----------------------------------------------------------------------------*/
/* proc_i010_spf   -- Process SPF indicator data item                         */
/*----------------------------------------------------------------------------*/

 static
 t_Retc proc_i010_spf (t_Ui16 length, t_Byte *buffer, t_Ui16 *pos_ptr)
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
		error_msg ("Invalid buffer length (I010/SPF)");
		goto done;
	}

#if LISTER
				   /* List raw data field: */
	list_buffer (1, ";  I010/SPF:", len, buffer + pos);
#endif /* LISTER */

				   /* Set position: */
	*pos_ptr = pos + len;

				   /* Set the return code: */
	ret = RC_OKAY;

	done:          /* We are done */
	return ret;
}

/*----------------------------------------------------------------------------*/
/* set_vsn010      -- Set ASTERIX category 010 reference version              */
/*----------------------------------------------------------------------------*/

 t_Retc set_vsn010 (char *vsn_text)
{
	t_Retc ret;    /* Return code */

                   /* Preset the return code: */
	ret = RC_FAIL;

                   /* Check parameter(s): */
	Assert (vsn_text != NULL, "NULL pointer");

                   /* Check against implemented reference versions: */
	if (strcmp (vsn_text, "0.24Sensis") == 0 ||
        strcmp (vsn_text, "0.24s") == 0)
	{
		reference_vsn = 1;
		ret = RC_OKAY;
	}
	else if (strcmp (vsn_text, "1.1") == 0)
	{
		reference_vsn = 3;
		ret = RC_OKAY;
	}

	return ret;
}
/* end-of-file */
