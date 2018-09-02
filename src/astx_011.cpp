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
/* File:           src/astx_011.cpp                                           */
/* Contents:       Process ASTERIX category 011 data block                    */
/* Author(s):      kb                                                         */
/* Last change:    2018-04-27                                                 */
/*----------------------------------------------------------------------------*/

/* Reference documents:
   --------------------

   Eurocontrol Standard Document for Surveillance Data Exchange.
   Part 8: Transmission of A-SMGCS Data.
   SUR.ET1.ST05.2000-STD-08-01

   Edition: 0.14
   Edition Date: October 2000
   Status: Working Draft

   Edition: 0.17
   Edition Date: December 2001
   Status: Working Draft
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

#define M_MAX_FRN 35
				   /* Maximum field reference number */
#define M_MAX_FSPEC_LENGTH 5
				   /* Maximum fields specification length
					  for ASTERIX category 011 */

                   /* Local texts: */
                   /* ------------ */

#if LISTER
static const char* emc_texts[24] =
                   /* Texts for emitter category in I011/380 */
{
	"light aircraft",
	"reserved",
	"medium aircraft",
	"reserved",
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
	"reserved",
	"reserved"
};
#endif /* LISTER */

				   /* Local variables: */
				   /* ---------------- */

static t_Data_Item_Desc desc_i011_000;
                   /* Description of data item I011/000 */
static t_Data_Item_Desc desc_i011_010;
                   /* Description of data item I011/010 */
static t_Data_Item_Desc desc_i011_015;
                   /* Description of data item I011/015 */
static t_Data_Item_Desc desc_i011_041;
                   /* Description of data item I011/041 */
static t_Data_Item_Desc desc_i011_042;
                   /* Description of data item I011/042 */
static t_Data_Item_Desc desc_i011_060;
                   /* Description of data item I011/060 */
static t_Data_Item_Desc desc_i011_090;
                   /* Description of data item I011/090 */
static t_Data_Item_Desc desc_i011_092;
                   /* Description of data item I011/092 */
static t_Data_Item_Desc desc_i011_093;
                   /* Description of data item I011/093 */
static t_Data_Item_Desc desc_i011_140;
                   /* Description of data item I011/140 */
static t_Data_Item_Desc desc_i011_161;
                   /* Description of data item I011/161 */
static t_Data_Item_Desc desc_i011_170;
                   /* Description of data item I011/170 */
static t_Data_Item_Desc desc_i011_202;
                   /* Description of data item I011/202 */
static t_Data_Item_Desc desc_i011_210;
                   /* Description of data item I011/210 */
static t_Data_Item_Desc desc_i011_215;
                   /* Description of data item I011/215 */
static t_Data_Item_Desc desc_i011_220;
                   /* Description of data item I011/220 */
static t_Data_Item_Desc desc_i011_245;
                   /* Description of data item I011/245 */
static t_Data_Item_Desc desc_i011_250;
                   /* Description of data item I011/250 */
static t_Data_Item_Desc desc_i011_270;
                   /* Description of data item I011/270 */
static t_Data_Item_Desc desc_i011_280;
                   /* Description of data item I011/280 */
static t_Data_Item_Desc desc_i011_282;
                   /* Description of data item I011/282 */
static t_Data_Item_Desc desc_i011_284;
                   /* Description of data item I011/284 */
static t_Data_Item_Desc desc_i011_286;
                   /* Description of data item I011/286 */
static t_Data_Item_Desc desc_i011_290;
                   /* Description of data item I011/290 */
static t_Data_Item_Desc desc_i011_300;
                   /* Description of data item I011/300 */
static t_Data_Item_Desc desc_i011_310;
                   /* Description of data item I011/310 */
static t_Data_Item_Desc desc_i011_380;
                   /* Description of data item I011/380 */
static t_Data_Item_Desc desc_i011_390;
                   /* Description of data item I011/390 */
static t_Data_Item_Desc desc_i011_430;
                   /* Description of data item I011/430 */
static t_Data_Item_Desc desc_i011_500;
                   /* Description of data item I011/500 */
static t_Data_Item_Desc desc_i011_600;
                   /* Description of data item I011/600 */
static t_Data_Item_Desc desc_i011_605;
                   /* Description of data item I011/605 */
static t_Data_Item_Desc desc_i011_610;
                   /* Description of data item I011/610 */
static t_Data_Item_Desc desc_i011_ref;
                   /* Description of REF data item */
static t_Data_Item_Desc desc_i011_spf;
                   /* Description of SPF data item */
static t_Bool initiated = FALSE;
				   /* Package initiated flag */
static t_Ui16 reference_vsn = 9;
                   /* Current version of reference document */
                   /* Implemented editions:
                     -1 ... 0.10 (June 1999)
                     -2 ... 0.11 (September 1999)
                     -3 ... 0.12 (February 2000)
                     -4 ... 0.13 (March 2000)
                      5 ... 0.14 (October 2000)
                      6 ... 0.14 (Sensis: Inn Valley modification)
                     -7 ... 0.15 (February 2001)
                     -8 ... 0.16 ()
                      9 ... 0.17 (December 2001)
                   */
static t_Data_Item_Desc *std_uap[M_MAX_FRN + 1];
				   /* Standard User Application Profile */

				   /* Local functions: */
				   /* ---------------- */

static void init_desc (void);
                   /* Initiate data item descriptions */
static void load_std_uap (void);
                   /* Load standard UAP */
static t_Retc proc_i011_000 (t_Ui16 length, t_Byte *buffer);
                   /* Process I011/000 data item */
static t_Retc proc_i011_010 (t_Ui16 length, t_Byte *buffer);
                   /* Process I011/010 data item */
static t_Retc proc_i011_015 (t_Ui16 length, t_Byte *buffer);
                   /* Process I011/015 data item */
static t_Retc proc_i011_041 (t_Ui16 length, t_Byte *buffer);
                   /* Process I011/041 data item */
static t_Retc proc_i011_042 (t_Ui16 length, t_Byte *buffer);
                   /* Process I011/042 data item */
static t_Retc proc_i011_060 (t_Ui16 length, t_Byte *buffer);
                   /* Process I011/060 data item */
static t_Retc proc_i011_090 (t_Ui16 length, t_Byte *buffer);
                   /* Process I011/090 data item */
static t_Retc proc_i011_092 (t_Ui16 length, t_Byte *buffer);
                   /* Process I011/092 data item */
static t_Retc proc_i011_093 (t_Ui16 length, t_Byte *buffer);
                   /* Process I011/093 data item */
static t_Retc proc_i011_140 (t_Ui16 length, t_Byte *buffer);
                   /* Process I011/140 data item */
static t_Retc proc_i011_161 (t_Ui16 length, t_Byte *buffer);
                   /* Process I011/161 data item */
static t_Retc proc_i011_170 (t_Ui16 length, t_Byte *buffer);
                   /* Process I011/170 data item */
static t_Retc proc_i011_202 (t_Ui16 length, t_Byte *buffer);
                   /* Process I011/202 data item */
static t_Retc proc_i011_210 (t_Ui16 length, t_Byte *buffer);
                   /* Process I011/210 data item */
static t_Retc proc_i011_215 (t_Ui16 length, t_Byte *buffer);
                   /* Process I011/215 data item */
static t_Retc proc_i011_220 (t_Ui16 length, t_Byte *buffer);
                   /* Process I011/220 data item */
static t_Retc proc_i011_245 (t_Ui16 length, t_Byte *buffer);
                   /* Process I011/245 data item */
static t_Retc proc_i011_250 (t_Ui16 length, t_Byte *buffer);
                   /* Process I011/250 data item */
static t_Retc proc_i011_270 (t_Ui16 length, t_Byte *buffer);
                   /* Process I011/270 data item */
static t_Retc proc_i011_280 (t_Ui16 length, t_Byte *buffer);
                   /* Process I011/280 data item */
static t_Retc proc_i011_282 (t_Ui16 length, t_Byte *buffer);
                   /* Process I011/282 data item */
static t_Retc proc_i011_284 (t_Ui16 length, t_Byte *buffer);
                   /* Process I011/284 data item */
static t_Retc proc_i011_286 (t_Ui16 length, t_Byte *buffer);
                   /* Process I011/286 data item */
static t_Retc proc_i011_290 (t_Ui16 length, t_Byte *buffer, t_Ui16 *pos_ptr);
                   /* Process I011/290 data item */
static t_Retc proc_i011_300 (t_Ui16 length, t_Byte *buffer);
                   /* Process I011/300 data item */
static t_Retc proc_i011_310 (t_Ui16 length, t_Byte *buffer);
                   /* Process I011/310 data item */
static t_Retc proc_i011_380 (t_Ui16 length, t_Byte *buffer, t_Ui16 *pos_ptr);
                   /* Process I011/380 data item */
static t_Retc proc_i011_390 (t_Ui16 length, t_Byte *buffer, t_Ui16 *pos_ptr);
                   /* Process I011/390 data item */
static t_Retc proc_i011_430 (t_Ui16 length, t_Byte *buffer);
                   /* Process I011/430 data item */
static t_Retc proc_i011_500 (t_Ui16 length, t_Byte *buffer, t_Ui16 *pos_ptr);
                   /* Process I011/500 data item */
static t_Retc proc_i011_600 (t_Ui16 length, t_Byte *buffer);
                   /* Process I011/600 data item */
static t_Retc proc_i011_605 (t_Ui16 length, t_Byte *buffer);
                   /* Process I011/605 data item */
static t_Retc proc_i011_610 (t_Ui16 length, t_Byte *buffer);
                   /* Process I011/610 data item */
static t_Retc proc_i011_ref (t_Ui16 length, t_Byte *buffer, t_Ui16 *pos_ptr);
                   /* Process REF indicator data item */
static t_Retc proc_i011_spf (t_Ui16 length, t_Byte *buffer, t_Ui16 *pos_ptr);
                   /* Process SPF indicator data item */

/*----------------------------------------------------------------------------*/
/* astx_011        -- Process ASTERIX category 011 data block                 */
/*----------------------------------------------------------------------------*/

 t_Retc astx_011 (t_Ui16 length, t_Byte *buffer)
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

					lrc = data_item (11, frn, std_uap[frn],
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
                   /* Description of data item I011/000: */
	desc_i011_000.category = 11;
	desc_i011_000.data_item = 0;
	desc_i011_000.item_type = e_fixed_length_data_item;
	desc_i011_000.fixed_length = 1;
	desc_i011_000.proc_fptr = proc_i011_000;
	desc_i011_000.read_fptr = NULL;

                   /* Description of data item I011/010: */
	desc_i011_010.category = 11;
	desc_i011_010.data_item = 10;
	desc_i011_010.item_type = e_fixed_length_data_item;
	desc_i011_010.fixed_length = 2;
	desc_i011_010.proc_fptr = proc_i011_010;
	desc_i011_010.read_fptr = NULL;

                   /* Description of data item I011/015: */
	desc_i011_015.category = 11;
	desc_i011_015.data_item = 15;
	desc_i011_015.item_type = e_fixed_length_data_item;
	desc_i011_015.fixed_length = 1;
	desc_i011_015.proc_fptr = proc_i011_015;
	desc_i011_015.read_fptr = NULL;

                   /* Description of data item I011/041: */
	desc_i011_041.category = 11;
	desc_i011_041.data_item = 41;
	desc_i011_041.item_type = e_fixed_length_data_item;
	desc_i011_041.fixed_length = 8;
	desc_i011_041.proc_fptr = proc_i011_041;
	desc_i011_041.read_fptr = NULL;

                   /* Description of data item I011/042: */
	desc_i011_042.category = 11;
	desc_i011_042.data_item = 42;
	desc_i011_042.item_type = e_fixed_length_data_item;
	desc_i011_042.fixed_length = 4;
	if (reference_vsn == 6)
	{
		desc_i011_042.fixed_length = 8;
	}
	desc_i011_042.proc_fptr = proc_i011_042;
	desc_i011_042.read_fptr = NULL;

                   /* Description of data item I011/060: */
	desc_i011_060.category = 11;
	desc_i011_060.data_item = 60;
	desc_i011_060.item_type = e_fixed_length_data_item;
	desc_i011_060.fixed_length = 2;
	desc_i011_060.proc_fptr = proc_i011_060;
	desc_i011_060.read_fptr = NULL;

                   /* Description of data item I011/090: */
	desc_i011_090.category = 11;
	desc_i011_090.data_item = 90;
	desc_i011_090.item_type = e_fixed_length_data_item;
	desc_i011_090.fixed_length = 2;
	desc_i011_090.proc_fptr = proc_i011_090;
	desc_i011_090.read_fptr = NULL;

                   /* Description of data item I011/092: */
	desc_i011_092.category = 11;
	desc_i011_092.data_item = 92;
	desc_i011_092.item_type = e_fixed_length_data_item;
	desc_i011_092.fixed_length = 2;
	desc_i011_092.proc_fptr = proc_i011_092;
	desc_i011_092.read_fptr = NULL;

                   /* Description of data item I011/093: */
	desc_i011_093.category = 11;
	desc_i011_093.data_item = 93;
	desc_i011_093.item_type = e_fixed_length_data_item;
	desc_i011_093.fixed_length = 2;
	desc_i011_093.proc_fptr = proc_i011_093;
	desc_i011_093.read_fptr = NULL;

                   /* Description of data item I011/140: */
	desc_i011_140.category = 11;
	desc_i011_140.data_item = 140;
	desc_i011_140.item_type = e_fixed_length_data_item;
	desc_i011_140.fixed_length = 3;
	desc_i011_140.proc_fptr = proc_i011_140;
	desc_i011_140.read_fptr = NULL;

                   /* Description of data item I011/161: */
	desc_i011_161.category = 11;
	desc_i011_161.data_item = 161;
	desc_i011_161.item_type = e_fixed_length_data_item;
	desc_i011_161.fixed_length = 2;
	desc_i011_161.proc_fptr = proc_i011_161;
	desc_i011_161.read_fptr = NULL;

                   /* Description of data item I011/170: */
	desc_i011_170.category = 11;
	desc_i011_170.data_item = 170;
	desc_i011_170.item_type = e_variable_length_data_item;
	desc_i011_170.fixed_length = 0;
	desc_i011_170.proc_fptr = proc_i011_170;
	desc_i011_170.read_fptr = NULL;

                   /* Description of data item I011/202: */
	desc_i011_202.category = 11;
	desc_i011_202.data_item = 202;
	desc_i011_202.item_type = e_fixed_length_data_item;
	if (reference_vsn == 5 || reference_vsn == 6)
	{
		desc_i011_202.fixed_length = 3;
	}
	else if (reference_vsn == 9)
	{
		desc_i011_202.fixed_length = 4;
	}
	else
	{
		Assert (FALSE, "Fixed length of I011/202 undefined");
	}
	desc_i011_202.proc_fptr = proc_i011_202;
	desc_i011_202.read_fptr = NULL;

                   /* Description of data item I011/210: */
	desc_i011_210.category = 11;
	desc_i011_210.data_item = 210;
	desc_i011_210.item_type = e_fixed_length_data_item;
	desc_i011_210.fixed_length = 2;
	desc_i011_210.proc_fptr = proc_i011_210;
	desc_i011_210.read_fptr = NULL;

                   /* Description of data item I011/215: */
	desc_i011_215.category = 11;
	desc_i011_215.data_item = 215;
	desc_i011_215.item_type = e_fixed_length_data_item;
	desc_i011_215.fixed_length = 2;
	desc_i011_215.proc_fptr = proc_i011_215;
	desc_i011_215.read_fptr = NULL;

                   /* Description of data item I011/220: */
	if (reference_vsn == 5 || reference_vsn == 6)
	{
		desc_i011_220.category = 11;
		desc_i011_220.data_item = 220;
		desc_i011_220.item_type = e_fixed_length_data_item;
		desc_i011_220.fixed_length = 3;
		desc_i011_220.proc_fptr = proc_i011_220;
		desc_i011_220.read_fptr = NULL;
	}

                   /* Description of data item I011/245: */
	desc_i011_245.category = 11;
	desc_i011_245.data_item = 245;
	desc_i011_245.item_type = e_fixed_length_data_item;
	desc_i011_245.fixed_length = 7;
	desc_i011_245.proc_fptr = proc_i011_245;
	desc_i011_245.read_fptr = NULL;

                   /* Description of data item I011/250: */
	if (reference_vsn == 5 || reference_vsn == 6)
	{
		desc_i011_250.category = 11;
		desc_i011_250.data_item = 250;
		desc_i011_250.item_type = e_repetitive_data_item;
		desc_i011_250.fixed_length = 8;
		desc_i011_250.proc_fptr = proc_i011_250;
		desc_i011_250.read_fptr = NULL;
	}

                   /* Description of data item I011/270: */
	desc_i011_270.category = 11;
	desc_i011_270.data_item = 270;
	desc_i011_270.item_type = e_variable_length_data_item;
	desc_i011_270.fixed_length = 0;
	desc_i011_270.proc_fptr = proc_i011_270;
	desc_i011_270.read_fptr = NULL;

                   /* Description of data item I011/280: */
	if (reference_vsn == 5 || reference_vsn == 6)
	{
		desc_i011_280.category = 11;
		desc_i011_280.data_item = 280;
		desc_i011_280.item_type = e_fixed_length_data_item;
		desc_i011_280.fixed_length = 4;
		desc_i011_280.proc_fptr = proc_i011_280;
		desc_i011_280.read_fptr = NULL;
	}

                   /* Description of data item I011/282: */
	if (reference_vsn == 5 || reference_vsn == 6)
	{
		desc_i011_282.category = 11;
		desc_i011_282.data_item = 282;
		desc_i011_282.item_type = e_fixed_length_data_item;
		desc_i011_282.fixed_length = 1;
		desc_i011_282.proc_fptr = proc_i011_282;
		desc_i011_282.read_fptr = NULL;
	}

                   /* Description of data item I011/284: */
	if (reference_vsn == 5 || reference_vsn == 6)
	{
		desc_i011_284.category = 11;
		desc_i011_284.data_item = 284;
		desc_i011_284.item_type = e_fixed_length_data_item;
		desc_i011_284.fixed_length = 1;
		desc_i011_284.proc_fptr = proc_i011_284;
		desc_i011_284.read_fptr = NULL;
	}

                   /* Description of data item I011/286: */
	if (reference_vsn == 5 || reference_vsn == 6)
	{
		desc_i011_286.category = 11;
		desc_i011_286.data_item = 286;
		desc_i011_286.item_type = e_fixed_length_data_item;
		desc_i011_286.fixed_length = 7;
		desc_i011_286.proc_fptr = proc_i011_286;
		desc_i011_286.read_fptr = NULL;
	}

                   /* Description of data item I011/290: */
	desc_i011_290.category = 11;
	desc_i011_290.data_item = 290;
	desc_i011_290.item_type = e_immediate_data_item;
	desc_i011_290.fixed_length = 0;
	desc_i011_290.proc_fptr = NULL;
	desc_i011_290.read_fptr = proc_i011_290;

                   /* Description of data item I011/300: */
	desc_i011_300.category = 11;
	desc_i011_300.data_item = 300;
	desc_i011_300.item_type = e_fixed_length_data_item;
	desc_i011_300.fixed_length = 1;
	desc_i011_300.proc_fptr = proc_i011_300;
	desc_i011_300.read_fptr = NULL;

                   /* Description of data item I011/310: */
	desc_i011_310.category = 11;
	desc_i011_310.data_item = 310;
	desc_i011_310.item_type = e_fixed_length_data_item;
	desc_i011_310.fixed_length = 1;
	desc_i011_310.proc_fptr = proc_i011_310;
	desc_i011_310.read_fptr = NULL;

                   /* Description of data item I011/380: */
	if (reference_vsn == 9)
	{
		desc_i011_380.category = 11;
		desc_i011_380.data_item = 380;
		desc_i011_380.item_type = e_immediate_data_item;
		desc_i011_380.fixed_length = 0;
		desc_i011_380.proc_fptr = NULL;
		desc_i011_380.read_fptr = proc_i011_380;
	}

                   /* Description of data item I011/390: */
	desc_i011_390.category = 11;
	desc_i011_390.data_item = 390;
	desc_i011_390.item_type = e_immediate_data_item;
	desc_i011_390.fixed_length = 0;
	desc_i011_390.proc_fptr = NULL;
	desc_i011_390.read_fptr = proc_i011_390;

                   /* Description of data item I011/430: */
	desc_i011_430.category = 11;
	desc_i011_430.data_item = 430;
	desc_i011_430.item_type = e_fixed_length_data_item;
	desc_i011_430.fixed_length = 1;
	desc_i011_430.proc_fptr = proc_i011_430;
	desc_i011_430.read_fptr = NULL;

                   /* Description of data item I011/500: */
	desc_i011_500.category = 11;
	desc_i011_500.data_item = 500;
	desc_i011_500.item_type = e_immediate_data_item;
	desc_i011_500.fixed_length = 0;
	desc_i011_500.proc_fptr = NULL;
	desc_i011_500.read_fptr = proc_i011_500;

                   /* Description of data item I011/600: */
	if (reference_vsn == 9)
	{
		desc_i011_600.category = 11;
		desc_i011_600.data_item = 600;
		desc_i011_600.item_type = e_fixed_length_data_item;
		desc_i011_600.fixed_length = 3;
		desc_i011_600.proc_fptr = proc_i011_600;
		desc_i011_600.read_fptr = NULL;
	}

                   /* Description of data item I011/605: */
	if (reference_vsn == 9)
	{
		desc_i011_605.category = 11;
		desc_i011_605.data_item = 605;
		desc_i011_605.item_type = e_repetitive_data_item;
		desc_i011_605.fixed_length = 2;
		desc_i011_605.proc_fptr = proc_i011_605;
		desc_i011_605.read_fptr = NULL;
	}

                   /* Description of data item I011/610: */
	if (reference_vsn == 9)
	{
		desc_i011_610.category = 11;
		desc_i011_610.data_item = 610;
		desc_i011_610.item_type = e_repetitive_data_item;
		desc_i011_610.fixed_length = 2;
		desc_i011_610.proc_fptr = proc_i011_610;
		desc_i011_610.read_fptr = NULL;
	}

                   /* Description of data item I011/REF: */
	desc_i011_ref.category = 11;
	desc_i011_ref.data_item = M_REF_INDICATOR;
	desc_i011_ref.item_type = e_immediate_data_item;
	desc_i011_ref.fixed_length = 0;
	desc_i011_ref.proc_fptr = NULL;
	desc_i011_ref.read_fptr = proc_i011_ref;

                   /* Description of data item I011/SPF: */
	desc_i011_spf.category = 11;
	desc_i011_spf.data_item = M_SPF_INDICATOR;
	desc_i011_spf.item_type = e_immediate_data_item;
	desc_i011_spf.fixed_length = 0;
	desc_i011_spf.proc_fptr = NULL;
	desc_i011_spf.read_fptr = proc_i011_spf;

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

	if (reference_vsn == 5 || reference_vsn == 6)
	{
		std_uap[1] = &desc_i011_010;
		std_uap[2] = &desc_i011_000;
		std_uap[3] = &desc_i011_015;
		std_uap[4] = &desc_i011_140;
		std_uap[5] = &desc_i011_041;
		std_uap[6] = &desc_i011_042;
		std_uap[7] = &desc_i011_202;

		std_uap[8] = &desc_i011_090;
		std_uap[9] = &desc_i011_210;
		std_uap[10] = &desc_i011_215;
		std_uap[11] = &desc_i011_161;
		std_uap[12] = &desc_i011_170;
		std_uap[13] = &desc_i011_060;
		std_uap[14] = &desc_i011_220;

		std_uap[15] = &desc_i011_300;
		std_uap[16] = &desc_i011_093;
		std_uap[17] = &desc_i011_092;
		std_uap[18] = &desc_i011_250;
		std_uap[19] = &desc_i011_390;
		std_uap[20] = &desc_i011_245;
		std_uap[21] = &desc_i011_270;

		std_uap[22] = &desc_i011_500;
		std_uap[23] = &desc_i011_430;
		std_uap[24] = &desc_i011_310;
		std_uap[31] = &desc_i011_290;

		std_uap[34] = &desc_i011_spf;
		std_uap[35] = &desc_i011_ref;
	}

	else if (reference_vsn == 9)
	{
		std_uap[1] = &desc_i011_010;
		std_uap[2] = &desc_i011_000;
		std_uap[3] = &desc_i011_015;
		std_uap[4] = &desc_i011_140;
		std_uap[5] = &desc_i011_041;
		std_uap[6] = &desc_i011_042;
		std_uap[7] = &desc_i011_202;

		std_uap[8] = &desc_i011_210;
		std_uap[9] = &desc_i011_060;
		std_uap[10] = &desc_i011_245;
		std_uap[11] = &desc_i011_380;
		std_uap[12] = &desc_i011_161;
		std_uap[13] = &desc_i011_170;
		std_uap[14] = &desc_i011_290;

		std_uap[15] = &desc_i011_430;
		std_uap[16] = &desc_i011_090;
		std_uap[17] = &desc_i011_093;
		std_uap[18] = &desc_i011_092;
		std_uap[19] = &desc_i011_215;
		std_uap[20] = &desc_i011_270;
		std_uap[21] = &desc_i011_390;

		std_uap[22] = &desc_i011_300;
		std_uap[23] = &desc_i011_310;
		std_uap[24] = &desc_i011_500;
		std_uap[25] = &desc_i011_600;
		std_uap[26] = &desc_i011_605;
		std_uap[27] = &desc_i011_610;
		std_uap[28] = &desc_i011_spf;

		std_uap[29] = &desc_i011_ref;
	}

	return;
}

/*----------------------------------------------------------------------------*/
/* proc_i011_000   -- Process I011/000 data item                              */
/*----------------------------------------------------------------------------*/

 static
 t_Retc proc_i011_000 (t_Ui16 length, t_Byte *buffer)
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
/* proc_i011_010   -- Process I011/010 data item                              */
/*----------------------------------------------------------------------------*/

 static
 t_Retc proc_i011_010 (t_Ui16 length, t_Byte *buffer)
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
/* proc_i011_015   -- Process I011/015 data item                              */
/*----------------------------------------------------------------------------*/

 static
 t_Retc proc_i011_015 (t_Ui16 length, t_Byte *buffer)
{
#if LISTER
	t_Byte df1;    /* Data field octet 1 */
#endif /* LISTER */
	t_Retc ret;    /* Return code */
#if LISTER
	t_Byte srv;    /* Service identification */
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
	srv = df1;

				   /* List data field: */
	list_text (2, ";  Service Identification:");
	list_text (2, " " M_FMT_BYTE, srv);
	list_text (2, "\n");
#endif /* LISTER */

                   /* Store this information: */
	/* tba ... */

				   /* Set the return code: */
	ret = RC_OKAY;

	return ret;
}

/*----------------------------------------------------------------------------*/
/* proc_i011_041   -- Process I011/041 data item                              */
/*----------------------------------------------------------------------------*/

 static
 t_Retc proc_i011_041 (t_Ui16 length, t_Byte *buffer)
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
	t_Si32 lon;    /* Longitude; 180/2**31 degrees */
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

#if LISTER
				   /* List data field: */
	list_text (2, ";  Position in WGS-84 Co-ordinates:");
	list_text (2, " lat=%.8f deg", (180.0 / M_TWO_POWER_31) * lat);
	list_text (2, "; lon=%.8f deg", (180.0 / M_TWO_POWER_31) * lon);
	list_text (2, "\n");
#endif /* LISTER */

                   /* Store this information: */
	mlat.wgs84_position.present = TRUE;
	mlat.wgs84_position.value_lat = (M_PI / M_TWO_POWER_31) * lat;
	mlat.wgs84_position.value_lon = (M_PI / M_TWO_POWER_31) * lon;
                   /* Values in radians */

				   /* Set the return code: */
	ret = RC_OKAY;

	return ret;
}

/*----------------------------------------------------------------------------*/
/* proc_i011_042   -- Process I011/042 data item                              */
/*----------------------------------------------------------------------------*/

 static
 t_Retc proc_i011_042 (t_Ui16 length, t_Byte *buffer)
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
	t_Si16 x;      /* x component; metres */
	t_Si32 xs;     /* x component (Sensis); metres */
	t_Si16 y;      /* y component; metres */
	t_Si32 ys;     /* y component (Sensis); metres */

				   /* Preset the return code: */
	ret = RC_FAIL;

				   /* Check parameters: */
	if (reference_vsn == 6)
	{
		Assert (length == 8, "Invalid parameter");
	}
	else
	{
		Assert (length == 4, "Invalid parameter");
	}
	Assert (buffer != NULL, "Invalid parameter");

				   /* Extract octets: */
	df1 = buffer[0];
	df2 = buffer[1];
	df3 = buffer[2];
	df4 = buffer[3];
	if (reference_vsn == 6)
	{
		df5 = buffer[4];
		df6 = buffer[5];
		df7 = buffer[6];
		df8 = buffer[7];
	}

				   /* Extract information: */
	if (reference_vsn != 6)
	{
		x = make_si16 (df1, df2);
		y = make_si16 (df3, df4);
	}
	else
	{
		xs = make_si32 (df1, df2, df3, df4);
		ys = make_si32 (df5, df6, df7, df8);
	}

#if LISTER
				   /* List data field: */
	list_text (2, ";  Calculated Position: ");
	if (reference_vsn != 6)
	{
		list_text (2, "x=" M_FMT_SI16 " mtr", x);
		list_text (2, "; y=" M_FMT_SI16 " mtr", y);
	}
	else
	{
		list_text (2, "x=" M_FMT_SI32 " mtr", xs);
		list_text (2, "; y=" M_FMT_SI32 " mtr", ys);
	}
	list_text (2, "\n");
#endif /* LISTER */

				   /* Store this information: */
	mlat.computed_position.present = TRUE;
	if (reference_vsn != 6)
	{
		mlat.computed_position.value_x = (1.0 / M_NMI2MTR) * x;
		mlat.computed_position.value_y = (1.0 / M_NMI2MTR) * y;
	}
	else
	{
		mlat.computed_position.value_x = (1.0 / M_NMI2MTR) * xs;
		mlat.computed_position.value_y = (1.0 / M_NMI2MTR) * ys;
	}

				   /* Set the return code: */
	ret = RC_OKAY;

	return ret;
}

/*----------------------------------------------------------------------------*/
/* proc_i011_060   -- Process I011/060 data item                              */
/*----------------------------------------------------------------------------*/

 static
 t_Retc proc_i011_060 (t_Ui16 length, t_Byte *buffer)
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
	list_text (2, " " M_FMT_CODE, m3a);
	list_text (2, "\n");
#endif /* LISTER */

                   /* Store this information: */
	mlat.mode_3a_info.code = m3a;
	mlat.mode_3a_info.present = TRUE;

				   /* Set the return code: */
	ret = RC_OKAY;

	return ret;
}

/*----------------------------------------------------------------------------*/
/* proc_i011_090   -- Process I011/090 data item                              */
/*----------------------------------------------------------------------------*/

 static
 t_Retc proc_i011_090 (t_Ui16 length, t_Byte *buffer)
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
	mfl = make_si16 (df1, df2);

#if LISTER
				   /* List data field: */
	list_text (2, ";  Measured Flight Level: ");
	list_text (2, "%.2f FL", 0.25 * mfl);
	list_text (2, "\n");
#endif /* LISTER */

                   /* Store this information: */
	mlat.mode_c_height.in_25_feet = e_is_true;
	mlat.mode_c_height.present = TRUE;
	mlat.mode_c_height.value = (25.0 * M_FT2MTR) * mfl;
	mlat.mode_c_height.value_in_feet = 25 * (t_Si32) mfl;
                   /* Value in feet */

				   /* Set the return code: */
	ret = RC_OKAY;

	return ret;
}

/*----------------------------------------------------------------------------*/
/* proc_i011_092   -- Process I011/092 data item                              */
/*----------------------------------------------------------------------------*/

 static
 t_Retc proc_i011_092 (t_Ui16 length, t_Byte *buffer)
{
	t_Byte df1;    /* Data field octet 1 */
	t_Byte df2;    /* Data field octet 2 */
	t_Si16 ga;     /* Calculated track geometric altitude; 6.25 ft */
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

#if LISTER
				   /* List data field: */
	if (reference_vsn == 5 || reference_vsn == 6)
	{
		list_text (2, ";  Height:");
	}
	else if (reference_vsn == 9)
	{
		list_text (2, ";  Calculated Track Geometric Altitude:");
	}
	list_text (2, " %.2f FL", 0.0625 * ga);
	list_text (2, "\n");
#endif /* LISTER */

                   /* Store this information: */
	mlat.geometric_altitude.present = TRUE;
	mlat.geometric_altitude.value = (6.25 * M_FT2MTR) * ga;

				   /* Set the return code: */
	ret = RC_OKAY;

	return ret;
}

/*----------------------------------------------------------------------------*/
/* proc_i011_093   -- Process I011/093 data item                              */
/*----------------------------------------------------------------------------*/

 static
 t_Retc proc_i011_093 (t_Ui16 length, t_Byte *buffer)
{
#if LISTER
	t_Si16 ba;     /* Calculated track barometric altitude; 1/4 FL */
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
	if (reference_vsn == 5 || reference_vsn == 6)
	{
		ba = make_si16 (df1 & 0x3f, df2);
	}
	else
	{
		ba = make_si16 (df1, df2);
	}

				   /* List data field: */
	if (reference_vsn == 5 || reference_vsn == 6)
	{
		list_text (2, ";  Calculated Flight Level:");
	}
	else if (reference_vsn == 9)
	{
		list_text (2, ";  Calculated Track Barometric Altitude:");
	}
	list_text (2, " %.2f FL", 0.25 * ba);
	list_text (2, "\n");
#endif /* LISTER */

                   /* Store this information: */
	/* tba ... */

				   /* Set the return code: */
	ret = RC_OKAY;

	return ret;
}

/*----------------------------------------------------------------------------*/
/* proc_i011_140   -- Process I011/140 data item                              */
/*----------------------------------------------------------------------------*/

 static
 t_Retc proc_i011_140 (t_Ui16 length, t_Byte *buffer)
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
	list_text (2, ";  Time of Track Information:");
	list_text (2, " " M_FMT_ATOD " (" M_FMT_UI32 "; %s UTC)",
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
/* proc_i011_161   -- Process I011/161 data item                              */
/*----------------------------------------------------------------------------*/

 static
 t_Retc proc_i011_161 (t_Ui16 length, t_Byte *buffer)
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
	tno = make_ui16 (df1 & 0x7f, df2);

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
/* proc_i011_170   -- Process I011/170 data item                              */
/*----------------------------------------------------------------------------*/

 static
 t_Retc proc_i011_170 (t_Ui16 length, t_Byte *buffer)
{
	t_Byte df1;    /* Data field octet 1 */
	int ix;        /* Auxiliary */
	t_Retc ret;    /* Return code */

				   /* Preset the return code: */
	ret = RC_FAIL;

				   /* Check parameters: */
	Assert (length > 0, "Invalid parameter");
	Assert (buffer != NULL, "Invalid parameter");

#if LISTER
				   /* List data field: */
	list_text (2, ";  Track Status:\n");
	if (reference_vsn == 5 || reference_vsn == 6 || reference_vsn == 9)
	{
		for (ix = 0; ix < length; ix ++)
		{
			df1 = buffer[ix];

			if (ix == 1)
			{
				t_Byte src;

				list_text (2, ";   ");

				if (df1 & 0x80)
				{
					list_text (2, "MON");
				}

				if (df1 & 0x40)
				{
					list_text (2, " GBS");
				}

				if (df1 & 0x20)
				{
					list_text (2, " MRH=GA");
				}
				else
				{
					list_text (2, " MRH=BA");
				}

				src = (df1 >> 2) & 0x07;
				list_text (2, " SRC=%u", src);
				switch (src)
				{
				case 0:
					list_text (2, " (no source)");
					break;
				case 1:
					list_text (2, " (GPS)");
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
					if (reference_vsn == 5 || reference_vsn == 6)
					{
						list_text (2, " (other or multiple)");
					}
					else if (reference_vsn == 9)
					{
						list_text (2, " (multilateration)");
					}
					break;
				}

				if (df1 & 0x02)
				{
					list_text (2, " TNT");
				}
				list_text (2, "\n");
			}
			else if (ix == 2)
			{
				t_Byte m4i;

				list_text (2, ";  ");

				if (df1 & 0x80)
				{
					list_text (2, " SIM");
				}

				if (df1 & 0x40)
				{
					list_text (2, " TSE");
				}

				if (df1 & 0x20)
				{
					list_text (2, " TSB");
				}

				m4i = (df1 >> 3) & 0x03;
				list_text (2, " M4I=%u", m4i);

				if (df1 & 0x04)
				{
					list_text (2, " ME");
				}

				if (df1 & 0x02)
				{
					list_text (2, " MI");
				}

				list_text (2, "\n");
			}
			else if (ix == 3)
			{
				list_text (2, ";  ");
				/* tba ... */
				list_text (2, "\n");
			}
			else if (ix == 4)
			{
				list_text (2, ";  ");
				/* tba ... */
				list_text (2, "\n");
			}
		}
	}
#endif /* LISTER */

                   /* Store this information: */
	if (reference_vsn == 5 || reference_vsn == 6 || reference_vsn == 9)
	{
		for (ix = 0; ix < length; ix ++)
		{
			df1 = buffer[ix];

			if (ix == 1)
			{
#if CCWARN
				t_Byte src;
#endif

				if (df1 & 0x40)
				{
					mlat.ground_bit_set = TRUE;
				}

#if CCWARN
				src = (df1 >> 2) & 0x07;
#endif
			}
			else if (ix == 2)
			{
#if CCWARN
				t_Byte m4i;
#endif

				mlat.detection_type.simulated = M_TRES (df1 & 0x80);

#if CCWARN
				m4i = (df1 >> 3) & 0x03;
#endif
			}
			else if (ix == 3)
			{
				/* tba ... */
			}
			else if (ix == 4)
			{
				/* tba ... */
			}
		}
	}

				   /* Set the return code: */
	ret = RC_OKAY;

	return ret;
}

/*----------------------------------------------------------------------------*/
/* proc_i011_202   -- Process I011/202 data item                              */
/*----------------------------------------------------------------------------*/

 static
 t_Retc proc_i011_202 (t_Ui16 length, t_Byte *buffer)
{
	t_Byte df1;    /* Data field octet 1 */
	t_Byte df2;    /* Data field octet 2 */
	t_Byte df3;    /* Data field octet 3 */
	t_Byte df4;    /* Data field octet 4 */
	t_Retc ret;    /* Return code */
	t_Si16 vx;     /* x velocity component; m/s or 0.25 m/s */
	t_Si16 vy;     /* y velocity component; m/s or 0.25 m/s */
	t_Byte tmp;    /* Auxiliary */

				   /* Preset the return code: */
	ret = RC_FAIL;

				   /* Check parameters: */
	if (reference_vsn == 5 || reference_vsn == 6)
	{
		Assert (length == 3, "Invalid parameter");
	}
	else if (reference_vsn == 9)
	{
		Assert (length == 4, "Invalid parameter");
	}
	Assert (buffer != NULL, "Invalid parameter");

				   /* Extract octets: */
	if (reference_vsn == 5 || reference_vsn == 6)
	{
		df1 = buffer[0];
		df2 = buffer[1];
		df3 = buffer[2];
	}
	else if (reference_vsn == 9)
	{
		df1 = buffer[0];
		df2 = buffer[1];
		df3 = buffer[2];
		df4 = buffer[3];
	}

				   /* Extract information: */
	if (reference_vsn == 5 || reference_vsn == 6)
	{
		tmp = ((df2 & 0xf0) >> 4);
		vx = (((t_Si16) df1) << 4) | tmp;
		if (df1 & 0x80)
		{
			vx |= 0xf000;
		}
		vy = make_si16 (df2 & 0x0f, df3);
		if (df2 & 0x08)
		{
			vy |= 0xf000;
		}
	}
	else if (reference_vsn == 9)
	{
		vx = make_si16 (df1, df2);
		vy = make_si16 (df3, df4);
	}

#if LISTER
				   /* List data field: */
	list_text (2, ";  Calculated Track Velocity in Cartesian"
                  " Co-ordinates:");
	if (reference_vsn == 5 || reference_vsn == 6)
	{
		list_text (2, " vx=%hd m/s; vy=%hd m/s", vx, vy);
	}
	else if (reference_vsn == 9)
	{
		list_text (2, " vx=%.2f m/s; vy=%.2f m/s", 0.25 * vx, 0.25 * vy);
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
/* proc_i011_210   -- Process I011/210 data item                              */
/*----------------------------------------------------------------------------*/

 static
 t_Retc proc_i011_210 (t_Ui16 length, t_Byte *buffer)
{
#if CCWARN
	t_Byte df1;    /* Data field octet 1 */
	t_Byte df2;    /* Data field octet 2 */
#endif /* CCWARN */
	t_Retc ret;    /* Return code */

				   /* Preset the return code: */
	ret = RC_FAIL;

				   /* Check parameters: */
	Assert (length == 2, "Invalid parameter");
	Assert (buffer != NULL, "Invalid parameter");

#if LISTER
				   /* Extract octets: */
#if CCWARN
	df1 = buffer[0];
	df2 = buffer[1];
#endif /* CCWARN */

				   /* Extract information: */
	/* tba ... */

				   /* List data field: */
	list_text (2, ";  Calculated Acceleration: ");
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
/* proc_i011_215   -- Process I011/215 data item                              */
/*----------------------------------------------------------------------------*/

 static
 t_Retc proc_i011_215 (t_Ui16 length, t_Byte *buffer)
{
#if CCWARN
	t_Byte df1;    /* Data field octet 1 */
	t_Byte df2;    /* Data field octet 2 */
#endif /* CCWARN */
	t_Retc ret;    /* Return code */
#if CCWARN
	t_Si16 rocd;   /* Calculated rate of climb/descent; 6.25 ft/min */
#endif /* CCWARN */

				   /* Preset the return code: */
	ret = RC_FAIL;

				   /* Check parameters: */
	Assert (length == 2, "Invalid parameter");
	Assert (buffer != NULL, "Invalid parameter");

#if LISTER
				   /* Extract octets: */
#if CCWARN
	df1 = buffer[0];
	df2 = buffer[1];
#endif /* CCWARN */

				   /* Extract information: */
#if CCWARN
	rocd = make_si16 (df1, df2);
#endif /* CCWARN */

				   /* List data field: */
	list_text (2, ";  Calculated Rate of Climb/Descent:");
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
/* proc_i011_220   -- Process I011/220 data item                              */
/*----------------------------------------------------------------------------*/

 static
 t_Retc proc_i011_220 (t_Ui16 length, t_Byte *buffer)
{
	t_Ui32 aa;     /* Aircraft address */
	t_Byte df1;    /* Data field octet 1 */
	t_Byte df2;    /* Data field octet 2 */
	t_Byte df3;    /* Data field octet 3 */
	t_Retc ret;    /* Return code */

				   /* Preset the return code: */
	ret = RC_FAIL;

				   /* Check parameters: */
	Assert (reference_vsn == 5 || reference_vsn == 6,
            "Wrong function call for processing I011/220");
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
/* proc_i011_245   -- Process I011/245 data item                              */
/*----------------------------------------------------------------------------*/

 static
 t_Retc proc_i011_245 (t_Ui16 length, t_Byte *buffer)
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
/* proc_i011_250   -- Process I011/250 data item                              */
/*----------------------------------------------------------------------------*/

 static
 t_Retc proc_i011_250 (t_Ui16 length, t_Byte *buffer)
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
	Assert (reference_vsn == 5 || reference_vsn == 6,
            "Wrong function call for processing I011/250");
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

		list_text (2, ";   BDS1=" M_FMT_BYTE " BDS2=" M_FMT_BYTE " data=0x"
                      " " M_FMT_BYTE_HEX " " M_FMT_BYTE_HEX " " M_FMT_BYTE_HEX
                      " " M_FMT_BYTE_HEX " " M_FMT_BYTE_HEX " " M_FMT_BYTE_HEX
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
/* proc_i011_270   -- Process I011/270 data item                              */
/*----------------------------------------------------------------------------*/

 static
 t_Retc proc_i011_270 (t_Ui16 length, t_Byte *buffer)
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
	if (reference_vsn == 5 || reference_vsn == 6)
	{
		list_text (2, ";  Target Size:\n");
		for (ix = 0; ix < length; ix ++)
		{
			df1 = (buffer[ix] >> 1) & 0x7f;

			if (ix == 1)
			{
				list_text (2, ";  size=" M_FMT_BYTE "\n", df1);
			}
			else if (ix == 2)
			{
				list_text (2, ";  orientation=" M_FMT_BYTE "\n", df1);
			}
		}
	}
	else if (reference_vsn == 9)
	{
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
	}
#endif /* LISTER */

                   /* Store this information: */
	/* tba ... */

				   /* Set the return code: */
	ret = RC_OKAY;

	return ret;
}

/*----------------------------------------------------------------------------*/
/* proc_i011_280   -- Process I011/280 data item                              */
/*----------------------------------------------------------------------------*/

 static
 t_Retc proc_i011_280 (t_Ui16 length, t_Byte *buffer)
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
	Assert (reference_vsn == 5 || reference_vsn == 6,
            "Wrong function call for processing I011/280");
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
	/* tba ... */

				   /* Set the return code: */
	ret = RC_OKAY;

	return ret;
}

/*----------------------------------------------------------------------------*/
/* proc_i011_282   -- Process I011/282 data item                              */
/*----------------------------------------------------------------------------*/

 static
 t_Retc proc_i011_282 (t_Ui16 length, t_Byte *buffer)
{
#if LISTER
	t_Byte df1;    /* Data field octet 1 */
#endif /* LISTER */
	t_Retc ret;    /* Return code */
#if LISTER
	char wtc;      /* Wake turbulence category */
#endif /* LISTER */

				   /* Preset the return code: */
	ret = RC_FAIL;

				   /* Check parameters: */
	Assert (reference_vsn == 5 || reference_vsn == 6,
            "Wrong function call for processing I011/282");
	Assert (length == 1, "Invalid parameter");
	Assert (buffer != NULL, "Invalid parameter");

#if LISTER
				   /* Extract octets: */
	df1 = buffer[0];

				   /* Extract information: */
	wtc = df1;

				   /* List data field: */
	list_text (2, ";  Category of Turbulence:");
	list_text (2, " '%c'", wtc);
	list_text (2, "\n");
#endif /* LISTER */

				   /* Store this information: */
	/* tba ... */

				   /* Set the return code: */
	ret = RC_OKAY;

	return ret;
}

/*----------------------------------------------------------------------------*/
/* proc_i011_284   -- Process I011/284 data item                              */
/*----------------------------------------------------------------------------*/

 static
 t_Retc proc_i011_284 (t_Ui16 length, t_Byte *buffer)
{
#if LISTER
	t_Byte df1;    /* Data field octet 1 */
#endif /* LISTER */
	t_Retc ret;    /* Return code */
#if LISTER
	t_Byte wc;     /* Weight category */
#endif /* LISTER */

				   /* Preset the return code: */
	ret = RC_FAIL;

				   /* Check parameters: */
	Assert (reference_vsn == 5 || reference_vsn == 6,
            "Wrong function call for processing I011/284");
	Assert (length == 1, "Invalid parameter");
	Assert (buffer != NULL, "Invalid parameter");

#if LISTER
				   /* Extract octets: */
	df1 = buffer[0];

				   /* Extract information: */
	wc = df1;

				   /* List data field: */
	list_text (2, ";  Weight Category:");
	list_text (2, " " M_FMT_BYTE " (%c)", wc, wc);
	list_text (2, "\n");
#endif /* LISTER */

				   /* Store this information: */
	/* tba ... */

				   /* Set the return code: */
	ret = RC_OKAY;

	return ret;
}

/*----------------------------------------------------------------------------*/
/* proc_i011_286   -- Process I011/286 data item                              */
/*----------------------------------------------------------------------------*/

 static
 t_Retc proc_i011_286 (t_Ui16 length, t_Byte *buffer)
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
                   /* Registration */

				   /* Preset the return code: */
	ret = RC_FAIL;

				   /* Check parameters: */
	Assert (reference_vsn == 5 || reference_vsn == 6,
            "Wrong function call for processing I011/286");
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
	list_text (2, ";  Aircraft Registration:");
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
/* proc_i011_290   -- Process I011/290 data item                              */
/*----------------------------------------------------------------------------*/

 static
 t_Retc proc_i011_290 (t_Ui16 length, t_Byte *buffer, t_Ui16 *pos_ptr)
{
#if LISTER
	t_Byte adb;    /* Age of the last ADS-B report used to update
                      the track; 1/4 seconds */
	t_Ui16 ads;    /* Age of the last ADS report used to update
                      the track; 1/4 seconds */
#endif /* LISTER */
	t_Byte df1;    /* Data field octet 1 */
	t_Byte df2;    /* Data field octet 2 */
#if LISTER
	t_Byte df3;    /* Data field octet 3 */
	t_Byte df4;    /* Data field octet 4 */
#endif /* LISTER */
	int inx;       /* Auxiliary */
#if LISTER
	int ix;        /* Auxiliary */
	int j;         /* Auxiliary */
#endif /* LISTER */
	t_Byte len;    /* Length of data field */
#if LISTER
	t_Byte lop;    /* Age of the last magnetic loop detection;
                      1/4 seconds */
	t_Byte md1;    /* Age of the last valid mode 1 report used to
                      update the track; 1/4 seconds */
	t_Byte md2;    /* Age of the last valid mode 2 report used to
                      update the track; 1/4 seconds */
	t_Byte mda;    /* Age of the last valid mode A report used to
                      update the track; 1/4 seconds */
	t_Byte mds;    /* Age of the last mode S report used to
                      update the track; 1/4 seconds */
	t_Byte mfl;    /* Age of the last valid and credible mode C used to
                      update the track; 1/4 seconds */
	t_Byte mul;    /* Age of the last multilateration detection;
                      1/4 seconds */
#endif /* LISTER */
	t_Ui16 pos;    /* Position within buffer */
#if LISTER
	t_Byte psr;    /* Age of the last primary report used to
                      update the track; 1/4 seconds */
#endif /* LISTER */
	t_Retc ret;    /* Return code */
	t_Byte sf1_present;
                   /* Subfield 1 present (PSR) */
	t_Byte sf2_present;
                   /* Subfield 2 present (SSR) */
	t_Byte sf3_present;
                   /* Subfield 3 present (MDA) */
	t_Byte sf4_present;
                   /* Subfield 4 present (MFL) */
	t_Byte sf5_present;
                   /* Subfield 5 present (MDS) */
	t_Byte sf6_present;
                   /* Subfield 6 present (ADS) */
	t_Byte sf7_present;
                   /* Subfield 7 present (ADB) */
	t_Byte sf8_present;
                   /* Subfield 8 present (MD1) */
	t_Byte sf9_present;
                   /* Subfield 9 present (MD2) */
	t_Byte sf10_present;
                   /* Subfield 10 present (LOP) */
	t_Byte sf11_present;
                   /* Subfield 11 present (TRK) */
	t_Byte sf12_present;
                   /* Subfield 12 present (MUL) */
#if LISTER
	t_Byte ssr;    /* Age of the last secondary report used
                      to update the track; 1/4 seconds */
	t_Byte trk;    /* Actual track age since first occurrence;
                      1/4 seconds */
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

                   /* Preset presence of other subfields: */
	sf8_present = FALSE;
	sf9_present = FALSE;
	sf10_present = FALSE;
	sf11_present = FALSE;
	sf12_present = FALSE;

                   /* Check for second octet: */
	if (df1 & 0x01)
	{
		if (pos + 1 >= length)
		{
			error_msg ("Invalid buffer length (I011/290)");
			goto done;
		}

		df2 = buffer[pos + 1];

		sf8_present = (df2 >> 7) & 0x01;
		sf9_present = (df2 >> 6) & 0x01;
		sf10_present = (df2 >> 5) & 0x01;
		if (reference_vsn == 9)
		{
			sf11_present = (df2 >> 4) & 0x01;
			sf12_present = (df2 >> 3) & 0x01;
		}
	}
	else
	{
		error_msg ("Primary subfield too short (I011/290)");
		goto done;
	}

                   /* Determine expected length of data field: */
	len = 1 +
          ord (df1 & 0x01) +
          ord (sf1_present) +
          ord (sf2_present) +
          ord (sf3_present) +
          ord (sf4_present) +
          ord (sf5_present) +
          2 * ord (sf6_present) +
          ord (sf7_present) +
          ord (sf8_present) +
          ord (sf9_present) +
          ord (sf10_present) +
          ord (sf11_present) +
          ord (sf12_present);

                   /* Check against buffer length: */
	if (pos + len > length)
	{
		error_msg ("Invalid buffer length (I011/290)");
		goto done;
	}

#if LISTER
                   /* List raw data field: */
	list_text (1, ";  I011/290: 0x");
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
	inx = 1 + ord (df1 & 0x01);
	if (sf1_present)
	{
#if LISTER
		psr = buffer[pos + inx];
#endif /* LISTER */
		++ inx;
	}
	if (sf2_present)
	{
#if LISTER
		ssr = buffer[pos + inx];
#endif /* LISTER */
		++ inx;
	}
	if (sf3_present)
	{
#if LISTER
		mda = buffer[pos + inx];
#endif /* LISTER */
		++ inx;
	}
	if (sf4_present)
	{
#if LISTER
		mfl = buffer[pos + inx];
#endif /* LISTER */
		++ inx;
	}
	if (sf5_present)
	{
#if LISTER
		mds = buffer[pos + inx];
#endif /* LISTER */
		++ inx;
	}
	if (sf6_present)
	{
#if LISTER
		df3 = buffer[pos + inx];
		df4 = buffer[pos + inx + 1];
#endif /* LISTER */
		inx += 2;

#if LISTER
		ads = make_ui16 (df3, df4);
#endif /* LISTER */
	}
	if (sf7_present)
	{
#if LISTER
		adb = buffer[pos + inx];
#endif /* LISTER */
		++ inx;
	}
	if (sf8_present)
	{
#if LISTER
		md1 = buffer[pos + inx];
#endif /* LISTER */
		++ inx;
	}
	if (sf9_present)
	{
#if LISTER
		md2 = buffer[pos + inx];
#endif /* LISTER */
		++ inx;
	}
	if (sf10_present)
	{
#if LISTER
		lop = buffer[pos + inx];
#endif /* LISTER */
		++ inx;
	}
	if (sf11_present)
	{
#if LISTER
		trk = buffer[pos + inx];
#endif /* LISTER */
		++ inx;
	}
	if (sf12_present)
	{
#if LISTER
		mul = buffer[pos + inx];
#endif /* LISTER */
		++ inx;
	}
	Assert (inx == len, "Invalid structure decoding");

#if LISTER
                   /* List data field: */
	list_text (2, ";  System Track Update Ages:");
	list_text (2, "\n");
	if (sf1_present)
	{
		list_text (2, ";   PSR age:");
		list_text (2, " " M_FMT_BYTE " (%.2f sec)", psr, 0.25 * psr);
		list_text (2, "\n");
	}
	if (sf2_present)
	{
		list_text (2, ";   SSR age:");
		list_text (2, " " M_FMT_BYTE " (%.2f sec)", ssr, 0.25 * ssr);
		list_text (2, "\n");
	}
	if (sf3_present)
	{
		list_text (2, ";   Mode A age:");
		list_text (2, " " M_FMT_BYTE " (%.2f sec)", mda, 0.25 * mda);
		list_text (2, "\n");
	}
	if (sf4_present)
	{
		list_text (2, ";   Measured Flight Level age:");
		list_text (2, " " M_FMT_BYTE " (%.2f sec)", mfl, 0.25 * mfl);
		list_text (2, "\n");
	}
	if (sf5_present)
	{
		list_text (2, ";   Mode S age:");
		list_text (2, " " M_FMT_UI16 "u (%.2f sec)", mds, 0.25 * mds);
		list_text (2, "\n");
	}
	if (sf6_present)
	{
		list_text (2, ";   ADS age:");
		list_text (2, " " M_FMT_BYTE " (%.2f sec)", ads, 0.25 * ads);
		list_text (2, "\n");
	}
	if (sf7_present)
	{
		list_text (2, ";   ADS-B age:");
		list_text (2, " " M_FMT_BYTE " (%.2f sec)", adb, 0.25 * adb);
		list_text (2, "\n");
	}
	if (sf8_present)
	{
		list_text (2, ";   Mode 1 age:");
		list_text (2, " " M_FMT_BYTE " (%.2f sec)", md1, 0.25 * md1);
		list_text (2, "\n");
	}
	if (sf9_present)
	{
		list_text (2, ";   Mode 2 age:");
		list_text (2, " " M_FMT_BYTE " (%.2f sec)", md2, 0.25 * md2);
		list_text (2, "\n");
	}
	if (sf10_present)
	{
		list_text (2, ";   Loop age:");
		list_text (2, " " M_FMT_BYTE " (%.2f sec)", lop, 0.25 * lop);
		list_text (2, "\n");
	}
	if (sf11_present)
	{
		list_text (2, ";   Track age:");
		list_text (2, " " M_FMT_BYTE " (%.2f sec)", trk, 0.25 * trk);
		list_text (2, "\n");
	}
	if (sf12_present)
	{
		list_text (2, ";   Multilateration age:");
		list_text (2, " " M_FMT_BYTE " (%.2f sec)", mul, 0.25 * mul);
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
/* proc_i011_300   -- Process I011/300 data item                              */
/*----------------------------------------------------------------------------*/

 static
 t_Retc proc_i011_300 (t_Ui16 length, t_Byte *buffer)
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
/* proc_i011_310   -- Process I011/310 data item                              */
/*----------------------------------------------------------------------------*/

 static
 t_Retc proc_i011_310 (t_Ui16 length, t_Byte *buffer)
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
/* proc_i011_380   -- Process I011/380 data item                              */
/*----------------------------------------------------------------------------*/

 static
 t_Retc proc_i011_380 (t_Ui16 length, t_Byte *buffer, t_Ui16 *pos_ptr)
{
	t_Ui32 adr;    /* Aircraft address */
#if CCWARN
	t_Byte atc;    /* Available technologies */
#endif /* CCWARN */
	t_Byte df1;    /* Data field octet 1 */
	t_Byte df2;    /* Data field octet 2 */
	t_Byte df3;    /* Data field octet 3 */
	t_Byte df4;    /* Data field octet 4 */
	t_Byte df5;    /* Data field octet 5 */
#if CCWARN
	t_Byte df6;    /* Data field octet 6 */
#endif /* CCWARN */
#if LISTER
	t_Byte emc;    /* Emitter category */
#endif /* LISTER */
	int inx;       /* Auxiliary */
#if LISTER
	int ix;        /* Auxiliary */
	int j;         /* Auxiliary */
#endif /* LISTER */
	t_Byte len;    /* Length of data field */
	t_Byte mb_rep; /* Repetition factor for "Mode S MB Data" */
	t_Ui16 pos;    /* Position within buffer */
	t_Retc ret;    /* Return code */
	t_Byte sf1_present;
                   /* Subfield 1 present (MB) */
	t_Byte sf2_present;
                   /* Subfield 2 present (ADR) */
	t_Byte sf3_present;
                   /* Subfield 3 present */
	t_Byte sf4_present;
                   /* Subfield 4 present (COM) */
	t_Byte sf5_present;
                   /* Subfield 5 present */
	t_Byte sf6_present;
                   /* Subfield 6 present */
	t_Byte sf7_present;
                   /* Subfield 7 present */
	t_Byte sf8_present;
                   /* Subfield 8 present (ACT) */
	t_Byte sf9_present;
                   /* Subfield 9 present (EMC) */
	t_Byte sf10_present;
                   /* Subfield 10 present */
	t_Byte sf11_present;
                   /* Subfield 11 present (ATC) */

				   /* Preset the return code: */
	ret = RC_FAIL;

				   /* Check parameters: */
	Assert (reference_vsn == 9,
            "Wrong function call for processing I011/380");
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

                   /* Check for second octet: */
	if (df1 & 0x01)
	{
		if (pos + 1 >= length)
		{
			error_msg ("Invalid buffer length (I011/380)");
			goto done;
		}

		df2 = buffer[pos + 1];

		sf8_present = (df2 >> 7) & 0x01;
		sf9_present = (df2 >> 6) & 0x01;
		sf10_present = (df2 >> 5) & 0x01;
		sf11_present = (df2 >> 4) & 0x01;
	}
	else
	{
		error_msg ("Primary subfield too short (I011/380)");

		sf8_present = FALSE;
		sf9_present = FALSE;
		sf10_present = FALSE;
		sf11_present = FALSE;
	}

                   /* Check subfields that should not be present: */
	if (sf3_present)
	{
		error_msg ("Subfield #3 present (I011/380)");
		goto done;
	}
	if (sf5_present)
	{
		error_msg ("Subfield #5 present (I011/380)");
		goto done;
	}
	if (sf6_present)
	{
		error_msg ("Subfield #6 present (I011/380)");
		goto done;
	}
	if (sf7_present)
	{
		error_msg ("Subfield #7 present (I011/380)");
		goto done;
	}
	if (sf10_present)
	{
		error_msg ("Subfield #10 present (I011/380)");
		goto done;
	}

                   /* Determine expected length of data field: */
	len = 1 + ord (df1 & 0x01);
	if (sf1_present)
	{
		if (pos + len >= length)
		{
			error_msg ("Invalid buffer length (I011/380)");
			goto done;
		}

		mb_rep = buffer[pos + len];

		len += (1 + mb_rep * 8);
	}
	len = len +
          3 * ord (sf2_present) +
          3 * ord (sf4_present) +
          4 * ord (sf8_present) +
          ord (sf9_present) +
          ord (sf11_present);

                   /* Check against buffer length: */
	if (pos + len > length)
	{
		error_msg ("Invalid buffer length (I011/380)");
		goto done;
	}

#if LISTER
                   /* List raw data field: */
	list_text (1, ";  I011/380: 0x");
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
	inx = 1 + ord (df1 & 0x01);
	if (sf1_present)
	{
		Assert (mb_rep == buffer[pos + inx],
                "Invalid structure decoding");

		/* tba ... */

		inx += (1 + mb_rep * 8); 
	}
	if (sf2_present)
	{
		df3 = buffer[pos + inx];
		df4 = buffer[pos + inx + 1];
		df5 = buffer[pos + inx + 2];
		inx += 3;

		adr = make_ui32 (0x00, df3, df4, df5);
	}
	if (sf4_present)
	{
		df3 = buffer[pos + inx];
		df4 = buffer[pos + inx + 1];
		df5 = buffer[pos + inx + 2];
		inx += 3;

		/* tba ... */
	}
	if (sf8_present)
	{
		df3 = buffer[pos + inx];
		df4 = buffer[pos + inx + 1];
		df5 = buffer[pos + inx + 2];
#if CCWARN
		df6 = buffer[pos + inx + 3];
#endif /* CCWARN */
		inx += 4;

		/* tba ... */
	}
	if (sf9_present)
	{
#if LISTER
		emc = buffer[pos + inx];
#endif /* LISTER */
		++ inx;
	}
	if (sf11_present)
	{
#if CCWARN
		atc = buffer[pos + inx];
#endif /* CCWARN */
		++ inx;
	}
	Assert (inx == len, "Invalid structure decoding");

#if LISTER
                   /* List data field: */
	list_text (2, ";  Mode S / ADS-B Related Data:");
	list_text (2, "\n");
	if (sf1_present)
	{
		list_text (2, ";   Mode S MB data:");
		/* tba ... */
		list_text (2, "\n");
	}
	if (sf2_present)
	{
		list_text (2, ";   Aircraft address:");
		list_text (2, " " M_FMT_ADDR, adr);
		list_text (2, "\n");
	}
	if (sf4_present)
	{
		list_text (2, ";   Communications/ACAS capability and"
                      " flight status:");
		/* tba ... */
		list_text (2, "\n");
	}
	if (sf8_present)
	{
		list_text (2, ";   Aircraft derived data type:");
		/* tba ... */
		list_text (2, "\n");
	}
	if (sf9_present)
	{
		list_text (2, ";   Emitter category:");
		list_text (2, " " M_FMT_BYTE, emc);
		if (1 <= emc && emc <= 24)
		{
			list_text (2, " (%s)", emc_texts[emc - 1]);
		}
		list_text (2, "\n");
	}
	if (sf11_present)
	{
		list_text (2, ";   Available technologies:");
		/* tba ... */
		list_text (2, "\n");
	}
#endif /* LISTER */

                   /* Store this information: */
	/* tba ... */
	if (sf2_present)
	{
		mlat.target_address.present = TRUE;
		mlat.target_address.value = adr;
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
/* proc_i011_390   -- Process I011/390 data item                              */
/*----------------------------------------------------------------------------*/

 static
 t_Retc proc_i011_390 (t_Ui16 length, t_Byte *buffer, t_Ui16 *pos_ptr)
{
	t_Byte df1;    /* Data field octet 1 */
	t_Byte df2;    /* Data field octet 2 */
#if CCWARN
	t_Byte df3;    /* Data field octet 3 */
	t_Byte df4;    /* Data field octet 4 */
	t_Byte df5;    /* Data field octet 5 */
	t_Byte df6;    /* Data field octet 6 */
	t_Byte df7;    /* Data field octet 7 */
	t_Byte df8;    /* Data field octet 8 */
	t_Byte df9;    /* Data field octet 9 */
#endif /* CCWARN */
	int inx;       /* Auxiliary */
#if LISTER
	int ix;        /* Auxiliary */
	int j;         /* Auxiliary */
#endif /* LISTER */
	t_Byte len;    /* Length of data field */
	t_Ui16 pos;    /* Position within buffer */
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
#if CCWARN
	t_Byte sts;    /* Stand status */
#endif /* CCWARN */
	t_Byte tod_rep;
                   /* Repetation factor for "Time of Departure" */
#if LISTER
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

                   /* Preset presence of other subfields: */
	sf8_present = FALSE;
	sf9_present = FALSE;
	sf10_present = FALSE;
	sf11_present = FALSE;
	sf12_present = FALSE;
	sf13_present = FALSE;
	sf14_present = FALSE;

                   /* Check for second octet: */
	if (df1 & 0x01)
	{
		if (pos + 1 >= length)
		{
			error_msg ("Invalid buffer length (I011/390)");
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
	else
	{
		error_msg ("Primary subfield too short (I011/390)");
		goto done;
	}

                   /* Second octet must not have the FX bit set: */
	Assert ((df2 & 0x01) == 0, "Unexpected FX bit");

                   /* Determine expected length of data field: */
	len = 1;
	if (df1 & 0x01)
	{
		len += 1;
	}
	if (sf1_present)
	{
		len += 2;
	}
	if (sf2_present)
	{
		len += 7;
	}
	if (sf3_present)
	{
		len += 4;
	}
	if (sf4_present)
	{
		len += 1;
	}
	if (sf5_present)
	{
		len += 4;
	}
	if (sf6_present)
	{
		len += 1;
	}
	if (sf7_present)
	{
		len += 4;
	}
	if (sf8_present)
	{
		len += 4;
	}
	if (sf9_present)
	{
		len += 3;
	}
	if (sf10_present)
	{
		len += 2;
	}
	if (sf11_present)
	{
		len += 2;
	}
	if (sf12_present)
	{
		if (pos + len >= length)
		{
			error_msg ("Invalid buffer length (I011/390)");
			goto done;
		}

		tod_rep = buffer[pos + len];

		len += (1 + tod_rep * 4);
	}
	if (sf13_present)
	{
		len += 6;
	}
	if (sf14_present)
	{
		len += 1;
	}

                   /* Check against buffer length: */
	if (pos + len >= length)
	{
		error_msg ("Invalid buffer length (I011/390)");
		goto done;
	}

#if LISTER
                   /* List raw data field: */
	list_text (1, ";  I011/390: 0x");
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
	inx = 1 + ord (df1 & 0x01);
	if (sf1_present)
	{
#if CCWARN
		df3 = buffer[pos + inx];
		df4 = buffer[pos + inx + 1];
#endif /* CCWARN */
		inx += 2;

		/* tba ... */
	}
	if (sf2_present)
	{
#if CCWARN
		df3 = buffer[pos + inx];
		df4 = buffer[pos + inx + 1];
		df5 = buffer[pos + inx + 2];
		df6 = buffer[pos + inx + 3];
		df7 = buffer[pos + inx + 4];
		df8 = buffer[pos + inx + 5];
		df9 = buffer[pos + inx + 6];
#endif /* CCWARN */
		inx += 7;

		/* tba ... */
	}
	if (sf3_present)
	{
#if CCWARN
		df3 = buffer[pos + inx];
		df4 = buffer[pos + inx + 1];
		df5 = buffer[pos + inx + 2];
		df6 = buffer[pos + inx + 3];
#endif /* CCWARN */
		inx += 4;

		/* tba ... */
	}
	if (sf4_present)
	{
#if CCWARN
		df3 = buffer[pos + inx];
#endif /* CCWARN */
		++ inx;

		/* tba ... */
	}
	if (sf5_present)
	{
#if CCWARN
		df3 = buffer[pos + inx];
		df4 = buffer[pos + inx + 1];
		df5 = buffer[pos + inx + 2];
		df6 = buffer[pos + inx + 3];
#endif /* CCWARN */
		inx += 4;

		/* tba ... */
	}
	if (sf6_present)
	{
#if LISTER
		wtc = buffer[pos + inx];
#endif /* LISTER */
		++ inx;
	}
	if (sf7_present)
	{
#if CCWARN
		df3 = buffer[pos + inx];
		df4 = buffer[pos + inx + 1];
		df5 = buffer[pos + inx + 2];
		df6 = buffer[pos + inx + 3];
#endif /* CCWARN */
		inx += 4;

		/* tba ... */
	}
	if (sf8_present)
	{
#if CCWARN
		df3 = buffer[pos + inx];
		df4 = buffer[pos + inx + 1];
		df5 = buffer[pos + inx + 2];
		df6 = buffer[pos + inx + 3];
#endif /* CCWARN */
		inx += 4;

		/* tba ... */
	}
	if (sf9_present)
	{
#if CCWARN
		df3 = buffer[pos + inx];
		df4 = buffer[pos + inx + 1];
		df5 = buffer[pos + inx + 2];
#endif /* CCWARN */
		inx += 3;

		/* tba ... */
	}
	if (sf10_present)
	{
#if CCWARN
		df3 = buffer[pos + inx];
		df4 = buffer[pos + inx + 1];
#endif /* CCWARN */
		inx += 2;

		/* tba ... */
	}
	if (sf11_present)
	{
#if CCWARN
		df3 = buffer[pos + inx];
		df4 = buffer[pos + inx + 1];
#endif /* CCWARN */
		inx += 2;

		/* tba ... */
	}
	if (sf12_present)
	{
		Assert (tod_rep == buffer[pos + inx],
                "Invalid structure decoding");

		/* tba ... */

		inx += (1 + tod_rep * 4); 
	}
	if (sf13_present)
	{
#if CCWARN
		df3 = buffer[pos + inx];
		df4 = buffer[pos + inx + 1];
		df5 = buffer[pos + inx + 2];
		df6 = buffer[pos + inx + 3];
		df7 = buffer[pos + inx + 4];
		df8 = buffer[pos + inx + 5];
#endif /* CCWARN */
		inx += 6;

		/* tba ... */
	}
	if (sf14_present)
	{
#if CCWARN
		sts = buffer[pos + inx];
#endif /* CCWARN */
		++ inx;
	}
	Assert (inx == len, "Invalid structure decoding");

#if LISTER
                   /* List data field: */
	list_text (2, ";  Flight Plan Related Data:");
	list_text (2, "\n");
	if (sf1_present)
	{
		list_text (2, ";   FPPS identification tag:");
		/* tba ... */
		list_text (2, "\n");
	}
	if (sf2_present)
	{
		list_text (2, ";   Callsign:");
		/* tba ... */
		list_text (2, "\n");
	}
	if (sf3_present)
	{
		list_text (2, ";   IFPS flight id:");
		/* tba ... */
		list_text (2, "\n");
	}
	if (sf4_present)
	{
		list_text (2, ";   Flight category:");
		/* tba ... */
		list_text (2, "\n");
	}
	if (sf5_present)
	{
		list_text (2, ";   Type of aircraft:");
		/* tba ... */
		list_text (2, "\n");
	}
	if (sf6_present)
	{
		list_text (2, ";   Wake turbulence category:");
		list_text (2, " " M_FMT_BYTE " (%c)", wtc, wtc);
		list_text (2, "\n");
	}
	if (sf7_present)
	{
		list_text (2, ";   Departure airport:");
		/* tba ... */
		list_text (2, "\n");
	}
	if (sf8_present)
	{
		list_text (2, ";   Destination airport:");
		/* tba ... */
		list_text (2, "\n");
	}
	if (sf9_present)
	{
		list_text (2, ";   Runway designation:");
		/* tba ... */
		list_text (2, "\n");
	}
	if (sf10_present)
	{
		list_text (2, ";   Current cleared flight level:");
		/* tba ... */
		list_text (2, "\n");
	}
	if (sf11_present)
	{
		list_text (2, ";   Current control position:");
		/* tba ... */
		list_text (2, "\n");
	}
	if (sf12_present)
	{
		list_text (2, ";   Time of departure:");
		/* tba ... */
		list_text (2, "\n");
	}
	if (sf13_present)
	{
		list_text (2, ";   Aircraft stand:");
		/* tba ... */
		list_text (2, "\n");
	}
	if (sf14_present)
	{
		list_text (2, ";   Stand status:");
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
/* proc_i011_430   -- Process I011/430 data item                              */
/*----------------------------------------------------------------------------*/

 static
 t_Retc proc_i011_430 (t_Ui16 length, t_Byte *buffer)
{
#if CCWARN
	t_Byte df1;    /* Data field octet 1 */
#endif
	t_Retc ret;    /* Return code */

				   /* Preset the return code: */
	ret = RC_FAIL;

				   /* Check parameters: */
	Assert (length == 1, "Invalid parameter");
	Assert (buffer != NULL, "Invalid parameter");

                   /* Extract octets: */
#if CCWARN
	df1 = buffer[0];
#endif

#if LISTER
				   /* List data field: */
	if (reference_vsn == 5 || reference_vsn == 6)
	{
		list_text (2, ";  Flight Status:");
	}
	else if (reference_vsn == 9)
	{
		list_text (2, ";  Phase of Flight:");
	}
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
/* proc_i011_500   -- Process I011/500 data item                              */
/*----------------------------------------------------------------------------*/

 static
 t_Retc proc_i011_500 (t_Ui16 length, t_Byte *buffer, t_Ui16 *pos_ptr)
{
#if LISTER
	t_Byte aac_x;  /* Estimated accuracy of acceleration (Cartesian)
                      - x component; 0.01 metres/second**2 */
	t_Byte aac_y;  /* Estimated accuracy of acceleration (Cartesian)
                      - y component; 0.01 metres/second**2 */
	t_Byte apc_x;  /* Estimated accuracy of track position
                      (Cartesian) - x component; 0.25 metres */
	t_Byte apc_y;  /* Estimated accuracy of track position
                      (Cartesian) - y component; 0.25 metres */
	t_Ui16 apw_lat;
                   /* Estimated accuracy of track position
                      (WGS-84) - latitude component;
                      180/2**31 degrees */
	t_Ui16 apw_lon;
                   /* Estimated accuracy of track position
                      (WGS-84) - longitude component;
                      180/2**31 degrees */
	t_Byte arc;    /* Estimated accuracy of rate of climb/descent;
                      0.1 metres/second */
	t_Ui16 ath;    /* Estimated accuracy of height; 0.5 metres */
	t_Byte avc_x;  /* Estimated accuracy of calculated velocity
                      (Cartesian) - x component; 0.1 metres/second */
	t_Byte avc_y;  /* Estimated accuracy of calculated velocity
                      (Cartesian) - y component; 0.1 metres/second */
#endif /* LISTER */
	t_Byte df1;    /* Data field octet 1 */
#if LISTER
	t_Byte df2;    /* Data field octet 2 */
	t_Byte df3;    /* Data field octet 3 */
	t_Byte df4;    /* Data field octet 4 */
	t_Byte df5;    /* Data field octet 5 */
#endif /* LISTER */
	int inx;       /* Auxiliary */
#if LISTER
	int ix;        /* Auxiliary */
	int j;         /* Auxiliary */
#endif /* LISTER */
	t_Ui16 len;    /* Length of data field */
	t_Ui16 pos;    /* Position within buffer */
	t_Retc ret;    /* Return code */
	t_Byte sf1_present;
                   /* Subfield 1 present (APC) */
	t_Byte sf2_present;
                   /* Subfield 2 present (APW) */
	t_Byte sf3_present;
                   /* Subfield 3 present (ATH) */
	t_Byte sf4_present;
                   /* Subfield 4 present (AVC) */
	t_Byte sf5_present;
                   /* Subfield 5 present (ARC) */
	t_Byte sf6_present;
                   /* Subfield 6 present (AAC) */

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
	Assert ((df1 & 0x01) == 0, "Unexpected FX bit");

                   /* Determine expected length of data field: */
	len = 1;
	if (sf1_present)
	{
		len += 2;
	}
	if (sf2_present)
	{
		len += 4;
	}
	if (sf3_present)
	{
		len += 2;
	}
	if (sf4_present)
	{
		len += 2;
	}
	if (sf5_present)
	{
		len += 1;
	}
	if (sf6_present)
	{
		len += 2;
	}

                   /* Check against buffer length: */
	if (pos + len > length)
	{
		error_msg ("Invalid buffer length (I011/500)");
		goto done;
	}

#if LISTER
                   /* List raw data field: */
	list_text (1, ";  I011/500: 0x");
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
#if LISTER
		df2 = buffer[pos + inx];
		df3 = buffer[pos + inx + 1];
#endif /* LISTER */
		inx += 2;

#if LISTER
		apc_x = df2;
		apc_y = df3;
#endif /* LISTER */
	}
	if (sf2_present)
	{
#if LISTER
		df2 = buffer[pos + inx];
		df3 = buffer[pos + inx + 1];
		df4 = buffer[pos + inx + 2];
		df5 = buffer[pos + inx + 3];
#endif /* LISTER */
		inx += 4;

#if LISTER
		apw_lat = make_ui16 (df2, df3);
		apw_lon = make_ui16 (df4, df5);
#endif /* LISTER */
	}
	if (sf3_present)
	{
#if LISTER
		df2 = buffer[pos + inx];
		df3 = buffer[pos + inx + 1];
#endif /* LISTER */
		inx += 2;

#if LISTER
		ath = make_ui16 (df2, df3);
#endif /* LISTER */
	}
	if (sf4_present)
	{
#if LISTER
		df2 = buffer[pos + inx];
		df3 = buffer[pos + inx + 1];
#endif /* LISTER */
		inx += 2;

#if LISTER
		avc_x = df2;
		avc_y = df3;
#endif /* LISTER */
	}
	if (sf5_present)
	{
#if LISTER
		arc = buffer[pos + inx];
#endif /* LISTER */
		++ inx;
	}
	if (sf6_present)
	{
#if LISTER
		df2 = buffer[pos + inx];
		df3 = buffer[pos + inx + 1];
#endif /* LISTER */
		inx += 2;

#if LISTER
		aac_x = df2;
		aac_y = df3;
#endif /* LISTER */
	}
	Assert (inx == len, "Invalid structure decoding");

#if LISTER
                   /* List data field: */
	list_text (2, ";  Estimated Accuracies:");
	list_text (2, "\n");
	if (sf1_present)
	{
		list_text (2, ";   Estimated accuracy of track position "
                      "(Cartesian):");
		list_text (2, " x=%u (%.2f mtr);", apc_x, 0.25 * apc_x);
		list_text (2, " y=%u (%.2f mtr)", apc_y, 0.25 * apc_y);
		list_text (2, "\n");
	}
	if (sf2_present)
	{
		list_text (2, ";   Estimated accuracy of track position "
                      "(WGS-84):");
		list_text (2, " lat=%hu (%.8f deg); ",
                      apw_lat, (180.0 / M_TWO_POWER_31) * apw_lat);
		list_text (2, " lon=%hu (%.8f deg)",
                      apw_lon, (180.0 / M_TWO_POWER_31) * apw_lon);
		list_text (2, "\n");
	}
	if (sf3_present)
	{
		list_text (2, ";   Estimated accuracy of height:");
		list_text (2, " %u (%.1f mtr)", ath, 0.5 * ath);
		list_text (2, "\n");
	}
	if (sf4_present)
	{
		list_text (2, ";   Estimated accuracy of track velocity "
                      "(Cartesian):");
		list_text (2, " x=%u (%.1f m/s);", avc_x, 0.1 * avc_x);
		list_text (2, " y=%u (%.1f m/s)", avc_y, 0.1 * avc_y);
		list_text (2, "\n");
	}
	if (sf5_present)
	{
		list_text (2, ";   Estimated accuracy of rate of climb"
                      "/descent:");
		list_text (2, " %u (%.1f mtr/sec)", arc, 0.1 * arc);
		list_text (2, "\n");
	}
	if (sf6_present)
	{
		list_text (2, ";   Estimated accuracy of acceleration "
                      "(Cartesian):");
		list_text (2, " x=%u (%.2f m/s**2);", aac_x, 0.01 * aac_x);
		list_text (2, " y=%u (%.2f m/s**2)", aac_y, 0.01 * aac_y);
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
/* proc_i011_600   -- Process I011/600 data item                              */
/*----------------------------------------------------------------------------*/

 static
 t_Retc proc_i011_600 (t_Ui16 length, t_Byte *buffer)
{
#if CCWARN
	t_Byte df1;    /* Data field octet 1 */
	t_Byte df2;    /* Data field octet 2 */
	t_Byte df3;    /* Data field octet 3 */
#endif
	t_Retc ret;    /* Return code */

				   /* Preset the return code: */
	ret = RC_FAIL;

				   /* Check parameters: */
	Assert (reference_vsn == 9,
            "Wrong function call for processing I011/600");
	Assert (length == 3, "Invalid parameter");
	Assert (buffer != NULL, "Invalid parameter");

                   /* Extract octets: */
#if CCWARN
	df1 = buffer[0];
	df2 = buffer[1];
	df3 = buffer[2];
#endif

#if LISTER
				   /* List data field: */
	list_text (2, ";  Alert Messages:");
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
/* proc_i011_605   -- Process I011/605 data item                              */
/*----------------------------------------------------------------------------*/

 static
 t_Retc proc_i011_605 (t_Ui16 length, t_Byte *buffer)
{
#if LISTER
	t_Byte df1;    /* Data field octet 1 */
	t_Byte df2;    /* Data field octet 2 */
	int ix;        /* Auxiliary */
#endif /* LISTER */
	t_Byte rep;    /* Repetition factor */
	t_Retc ret;    /* Return code */
#if LISTER
	t_Ui16 tno;    /* Fusion track number */
#endif /* LISTER */

				   /* Preset the return code: */
	ret = RC_FAIL;

				   /* Check parameters: */
	Assert (reference_vsn == 9,
            "Wrong function call for processing I011/605");
	Assert (length > 0, "Invalid parameter");
	Assert (buffer != NULL, "Invalid parameter");

				   /* Get repetition factor: */
	rep = buffer[0];

                   /* Check repetition factor: */
	Assert (rep > 0, "Invalid repetition factor");

				   /* Check length (again): */
	Assert (length == 1 + rep * 2, "Invalid length");

#if LISTER
				   /* List data field: */
	list_text (2, ";  Tracks in Alert:\n");
	for (ix = 0; ix < rep; ix ++)
	{
		df1 = buffer[1 + 2 * ix];
		df2 = buffer[1 + 2 * ix + 1];

		tno = make_ui16 (df1 & 0x0f, df2);

		list_text (2, ";   tno=" M_FMT_UI16 "\n", tno);
	}
#endif /* LISTER */

                   /* Store this information: */
	/* tba ... */

                   /* Set the return code: */
	ret = RC_OKAY;

	return ret;
}

/*----------------------------------------------------------------------------*/
/* proc_i011_610   -- Process I011/610 data item                              */
/*----------------------------------------------------------------------------*/

 static
 t_Retc proc_i011_610 (t_Ui16 length, t_Byte *buffer)
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
	Assert (reference_vsn == 9,
            "Wrong function call for processing I011/610");
	Assert (length > 0, "Invalid parameter");
	Assert (buffer != NULL, "Invalid parameter");

				   /* Get repetition factor: */
	rep = buffer[0];

                   /* Check repetition factor: */
	Assert (rep > 0, "Invalid repetition factor");

				   /* Check length (again): */
	Assert (length == 1 + rep * 2, "Invalid length");

#if LISTER
				   /* List data field: */
	list_text (2, ";  Holdbar Status:");
	for (ix = 0; ix < rep; ix ++)
	{
#if CCWARN
		df1 = buffer[1 + 2 * ix];
		df2 = buffer[1 + 2 * ix + 1];
#endif /* CCWARN */

		/* tba ... */
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
/* proc_i011_ref   -- Process REF indicator data item                         */
/*----------------------------------------------------------------------------*/

 static
 t_Retc proc_i011_ref (t_Ui16 length, t_Byte *buffer, t_Ui16 *pos_ptr)
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
		error_msg ("Invalid buffer length (I011/REF)");
		goto done;
	}

#if LISTER
				   /* List raw data field: */
	list_buffer (1, ";  I011/REF:", len, buffer + pos);
#endif /* LISTER */

				   /* Set position: */
	*pos_ptr = pos + len;

                   /* Set the return code: */
	ret = RC_OKAY;

	done:          /* We are done */
	return ret;
}

/*----------------------------------------------------------------------------*/
/* proc_i011_spf   -- Process SPF indicator data item                         */
/*----------------------------------------------------------------------------*/

 static
 t_Retc proc_i011_spf (t_Ui16 length, t_Byte *buffer, t_Ui16 *pos_ptr)
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
		error_msg ("Invalid buffer length (I011/SPF)");
		goto done;
	}

#if LISTER
				   /* List raw data field: */
	list_buffer (1, ";  I011/SPF:", len, buffer + pos);
#endif /* LISTER */

				   /* Set position: */
	*pos_ptr = pos + len;

				   /* Set the return code: */
	ret = RC_OKAY;

	done:          /* We are done */
	return ret;
}

/*----------------------------------------------------------------------------*/
/* set_vsn011      -- Set ASTERIX category 011 reference version              */
/*----------------------------------------------------------------------------*/

 t_Retc set_vsn011 (char *vsn_text)
{
	t_Retc ret;    /* Return code */

                   /* Preset the return code: */
	ret = RC_FAIL;

                   /* Check parameter(s): */
	Assert (vsn_text != NULL, "Invalid parameter");

                   /* Check against implemented reference versions: */
	if (strcmp (vsn_text, "0.10") == 0)
	{
		reference_vsn = 1;
		ret = RC_OKAY;
	}
	else if (strcmp (vsn_text, "0.11") == 0)
	{
		reference_vsn = 2;
		ret = RC_OKAY;
	}
	else if (strcmp (vsn_text, "0.12") == 0)
	{
		reference_vsn = 3;
		ret = RC_OKAY;
	}
	else if (strcmp (vsn_text, "0.13") == 0)
	{
		reference_vsn = 4;
		ret = RC_OKAY;
	}
	else if (strcmp (vsn_text, "0.14") == 0)
	{
		reference_vsn = 5;
		ret = RC_OKAY;
	}
	else if (strcmp (vsn_text, "0.14i") == 0 ||
             strcmp (vsn_text, "0.14Sensis") == 0)
	{
		reference_vsn = 6;
		ret = RC_OKAY;
	}
	else if (strcmp (vsn_text, "0.15") == 0)
	{
		reference_vsn = 7;
		ret = RC_OKAY;
	}
	else if (strcmp (vsn_text, "0.16") == 0)
	{
		reference_vsn = 8;
		ret = RC_OKAY;
	}
	else if (strcmp (vsn_text, "0.17") == 0)
	{
		reference_vsn = 9;
		ret = RC_OKAY;
	}

	return ret;
}
/* end-of-file */
