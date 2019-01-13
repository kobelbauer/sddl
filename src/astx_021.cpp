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
/* File:           src/astx_021.cpp                                           */
/* Contents:       Process ASTERIX category 021 data block                    */
/* Author(s):      kb                                                         */
/* Last change:    2018-04-27                                                 */
/*----------------------------------------------------------------------------*/

/* Reference documents:
   --------------------

   Eurocontrol Standard Document for Surveillance Data Exchange.
   Part 12: Transmission of ADS-B Messages.
   SUR.ET1.ST05.2000-STD-12-01

   Edition: 0.12
   Edition Date: February 2001
   Status: Working Draft

   Edition: 0.13
   Edition Date: June 2001
   Status: Working Draft

   Edition: 0.20
   Edition Date: December 2002
   Status: Working Draft

   Edition: 0.23
   Edition Date: November 2003
   Status: Working Draft

   Edition: 1.0P
   Edition Date: April 2008
   Status: Proposed Issue

   Edition: 1.4
   Edition Date: July 2009
   Status: Released Issue

   Edition: 2.1
   Edition Date: May 2011
   Status: Released Issue

   Edition: 2.4
   Edition Date: 15/06/2015
   Status: Released
*/

#include "basics.h"
                   /* Basic system definitions */
#include "common.h"
                   /* Common declarations and definitions */

#include <ctype.h>
#include <math.h>
#include <stddef.h>
#include <stdio.h>
#include <string.h>

                   /* Local macros: */
                   /* ------------- */

#define M_MAX_FRN 49
                   /* Maximum field reference number */
#define M_MAX_FSPEC_LENGTH 7
                   /* Maximum fields specification length
                      for ASTERIX category 021 */

                   /* Local texts: */
                   /* ------------ */

#if LISTER
static const char* arc_texts[4] =
                   /* Texts for altitude resolution in I021/040 */
{
	"25 ft",
	"100 ft",
	"unknown",
	"invalid"
};
static const char* atp_texts[8] =
                   /* Texts for aircraft address type in I021/040 */
{
	"24-Bit ICAO address",
	"Duplicate address",
	"Surface vehicle address",
	"Anonymous address",
	"Reserved for future use",
	"Reserved for future use",
	"Reserved for future use",
	"Reserved for future use"
};
static const char* cl_texts[4] =
                   /* Texts for confidence level in I021/040 */
{
	"Report valid",
	"Report suspect",
	"No information",
	"Reserved for future use"
};
static const char* emc_texts[24] =
                   /* Texts for emitter category in I021/020 */
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

static t_Data_Item_Desc desc_i021_008;
                   /* Description of data item I021/008 */
static t_Data_Item_Desc desc_i021_010;
                   /* Description of data item I021/010 */
static t_Data_Item_Desc desc_i021_015;
                   /* Description of data item I021/015 */
static t_Data_Item_Desc desc_i021_016;
                   /* Description of data item I021/016 */
static t_Data_Item_Desc desc_i021_020;
                   /* Description of data item I021/020 */
static t_Data_Item_Desc desc_i021_030;
                   /* Description of data item I021/030 */
static t_Data_Item_Desc desc_i021_032;
                   /* Description of data item I021/032 */
static t_Data_Item_Desc desc_i021_035;
                   /* Description of data item I021/035 */
static t_Data_Item_Desc desc_i021_040;
                   /* Description of data item I021/040 */
static t_Data_Item_Desc desc_i021_070;
                   /* Description of data item I021/070 */
static t_Data_Item_Desc desc_i021_071;
                   /* Description of data item I021/071 */
static t_Data_Item_Desc desc_i021_072;
                   /* Description of data item I021/072 */
static t_Data_Item_Desc desc_i021_073;
                   /* Description of data item I021/073 */
static t_Data_Item_Desc desc_i021_074;
                   /* Description of data item I021/074 */
static t_Data_Item_Desc desc_i021_075;
                   /* Description of data item I021/075 */
static t_Data_Item_Desc desc_i021_076;
                   /* Description of data item I021/076 */
static t_Data_Item_Desc desc_i021_077;
                   /* Description of data item I021/077 */
static t_Data_Item_Desc desc_i021_080;
                   /* Description of data item I021/080 */
static t_Data_Item_Desc desc_i021_090;
                   /* Description of data item I021/090 */
static t_Data_Item_Desc desc_i021_095;
                   /* Description of data item I021/095 */
static t_Data_Item_Desc desc_i021_100;
                   /* Description of data item I021/100 */
static t_Data_Item_Desc desc_i021_110;
                   /* Description of data item I021/110 */
static t_Data_Item_Desc desc_i021_130;
                   /* Description of data item I021/130 */
static t_Data_Item_Desc desc_i021_131;
                   /* Description of data item I021/131 */
static t_Data_Item_Desc desc_i021_132;
                   /* Description of data item I021/132 */
static t_Data_Item_Desc desc_i021_140;
                   /* Description of data item I021/140 */
static t_Data_Item_Desc desc_i021_145;
                   /* Description of data item I021/145 */
static t_Data_Item_Desc desc_i021_146;
                   /* Description of data item I021/146 */
static t_Data_Item_Desc desc_i021_147;
                   /* Description of data item I021/147 */
static t_Data_Item_Desc desc_i021_148;
                   /* Description of data item I021/148 */
static t_Data_Item_Desc desc_i021_150;
                   /* Description of data item I021/150 */
static t_Data_Item_Desc desc_i021_151;
                   /* Description of data item I021/151 */
static t_Data_Item_Desc desc_i021_152;
                   /* Description of data item I021/152 */
static t_Data_Item_Desc desc_i021_155;
                   /* Description of data item I021/155 */
static t_Data_Item_Desc desc_i021_157;
                   /* Description of data item I021/157 */
static t_Data_Item_Desc desc_i021_160;
                   /* Description of data item I021/160 */
static t_Data_Item_Desc desc_i021_161;
                   /* Description of data item I021/161 */
static t_Data_Item_Desc desc_i021_165;
                   /* Description of data item I021/165 */
static t_Data_Item_Desc desc_i021_170;
                   /* Description of data item I021/170 */
static t_Data_Item_Desc desc_i021_180;
                   /* Description of data item I021/180 */
static t_Data_Item_Desc desc_i021_190;
                   /* Description of data item I021/190 */
static t_Data_Item_Desc desc_i021_200;
                   /* Description of data item I021/200 */
static t_Data_Item_Desc desc_i021_210;
                   /* Description of data item I021/210 */
static t_Data_Item_Desc desc_i021_220;
                   /* Description of data item I021/220 */
static t_Data_Item_Desc desc_i021_230;
                   /* Description of data item I021/230 */
static t_Data_Item_Desc desc_i021_250;
                   /* Description of data item I021/250 */
static t_Data_Item_Desc desc_i021_260;
                   /* Description of data item I021/260 */
static t_Data_Item_Desc desc_i021_271;
                   /* Description of data item I021/271 */
static t_Data_Item_Desc desc_i021_295;
                   /* Description of data item I021/295 */
static t_Data_Item_Desc desc_i021_400;
                   /* Description of data item I021/400 */
static t_Data_Item_Desc desc_i021_ref;
                   /* Description of REF data item */
static t_Data_Item_Desc desc_i021_spf;
                   /* Description of SPF data item */
static t_Bool initiated = FALSE;
                   /* Package initiated flag */
static t_Ui16 reference_vsn = 21;
                   /* Current version of reference document */
                   /* Implemented editions:
                      1 ... 0.12 (February 2001)
                      2 ... 0.13 (June 2001)
                     -3 ... 0.14 (September 2001)
                     -4 ... 0.15 (November 2001)
                     -5 ... 0.16 (December 2001)
                     -6 ... 0.17 (January 2002)
                     -7 ... 0.18 (February 2002)
                     -8 ... 0.19 (October 2002)
                      9 ... 0.20 (December 2002)
                     -10 .. 0.21 (March 2003)
                     -11 .. 0.22 (August 2003)
                      12 .. 0.23 (November 2003)
                     -13 .. 0.27
                      14 .. 1.0P (April 2008)
                      15 .. 1.4  (July 2009)
                     -16 .. 1.5  (September 2010)
                     -17 .. 1.6  (October 2010)
                     -18 .. 1.7  (December 2010)
                     -19 .. 1.8  (January 2011)
                     -20 .. 2.0  (March 2011)
                      21 .. 2.1  (May 2011)
                      24 .. 2.4  (15 June 2015)
                   */
static t_Data_Item_Desc *std_uap[M_MAX_FRN + 1];
                   /* Standard User Application Profile */

                   /* Local functions: */
                   /* ---------------- */

static void init_desc (void);
                   /* Initiate data item descriptions */
static void load_std_uap (void);
                   /* Load standard User Application Profile (UAP) */
static t_Retc proc_i021_008 (t_Ui16 length, t_Byte *buffer);
                   /* Process I021/008 data item */
static t_Retc proc_i021_010 (t_Ui16 length, t_Byte *buffer);
                   /* Process I021/010 data item */
static t_Retc proc_i021_015 (t_Ui16 length, t_Byte *buffer);
                   /* Process I021/015 data item */
static t_Retc proc_i021_016 (t_Ui16 length, t_Byte *buffer);
                   /* Process I021/016 data item */
static t_Retc proc_i021_020 (t_Ui16 length, t_Byte *buffer);
                   /* Process I021/020 data item */
static t_Retc proc_i021_030 (t_Ui16 length, t_Byte *buffer);
                   /* Process I021/030 data item */
static t_Retc proc_i021_032 (t_Ui16 length, t_Byte *buffer);
                   /* Process I021/032 data item */
static t_Retc proc_i021_035 (t_Ui16 length, t_Byte *buffer);
                   /* Process I021/035 data item */
static t_Retc proc_i021_040 (t_Ui16 length, t_Byte *buffer);
                   /* Process I021/040 data item */
static t_Retc proc_i021_070 (t_Ui16 length, t_Byte *buffer);
                   /* Process I021/070 data item */
static t_Retc proc_i021_071 (t_Ui16 length, t_Byte *buffer);
                   /* Process I021/071 data item */
static t_Retc proc_i021_072 (t_Ui16 length, t_Byte *buffer);
                   /* Process I021/072 data item */
static t_Retc proc_i021_073 (t_Ui16 length, t_Byte *buffer);
                   /* Process I021/073 data item */
static t_Retc proc_i021_074 (t_Ui16 length, t_Byte *buffer);
                   /* Process I021/074 data item */
static t_Retc proc_i021_075 (t_Ui16 length, t_Byte *buffer);
                   /* Process I021/075 data item */
static t_Retc proc_i021_076 (t_Ui16 length, t_Byte *buffer);
                   /* Process I021/076 data item */
static t_Retc proc_i021_077 (t_Ui16 length, t_Byte *buffer);
                   /* Process I021/077 data item */
static t_Retc proc_i021_080 (t_Ui16 length, t_Byte *buffer);
                   /* Process I021/080 data item */
static t_Retc proc_i021_090 (t_Ui16 length, t_Byte *buffer);
                   /* Process I021/090 data item */
static t_Retc proc_i021_095 (t_Ui16 length, t_Byte *buffer);
                   /* Process I021/095 data item */
static t_Retc proc_i021_100 (t_Ui16 length, t_Byte *buffer);
                   /* Process I021/100 data item */
static t_Retc proc_i021_110 (t_Ui16 length, t_Byte *buffer);
                   /* Process I021/110 data item */
static t_Retc proc_i021_110 (t_Ui16 length, t_Byte *buffer, t_Ui16 *pos_ptr);
                   /* Process I021/110 data item */
static t_Retc proc_i021_130 (t_Ui16 length, t_Byte *buffer);
                   /* Process I021/130 data item */
static t_Retc proc_i021_131 (t_Ui16 length, t_Byte *buffer);
                   /* Process I021/131 data item */
static t_Retc proc_i021_132 (t_Ui16 length, t_Byte *buffer);
                   /* Process I021/132 data item */
static t_Retc proc_i021_140 (t_Ui16 length, t_Byte *buffer);
                   /* Process I021/140 data item */
static t_Retc proc_i021_145 (t_Ui16 length, t_Byte *buffer);
                   /* Process I021/145 data item */
static t_Retc proc_i021_146 (t_Ui16 length, t_Byte *buffer);
                   /* Process I021/146 data item */
static t_Retc proc_i021_147 (t_Ui16 length, t_Byte *buffer);
                   /* Process I021/147 data item */
static t_Retc proc_i021_148 (t_Ui16 length, t_Byte *buffer);
                   /* Process I021/148 data item */
static t_Retc proc_i021_150 (t_Ui16 length, t_Byte *buffer);
                   /* Process I021/150 data item */
static t_Retc proc_i021_151 (t_Ui16 length, t_Byte *buffer);
                   /* Process I021/151 data item */
static t_Retc proc_i021_152 (t_Ui16 length, t_Byte *buffer);
                   /* Process I021/152 data item */
static t_Retc proc_i021_155 (t_Ui16 length, t_Byte *buffer);
                   /* Process I021/155 data item */
static t_Retc proc_i021_157 (t_Ui16 length, t_Byte *buffer);
                   /* Process I021/157 data item */
static t_Retc proc_i021_160 (t_Ui16 length, t_Byte *buffer);
                   /* Process I021/160 data item */
static t_Retc proc_i021_161 (t_Ui16 length, t_Byte *buffer);
                   /* Process I021/161 data item */
static t_Retc proc_i021_165 (t_Ui16 length, t_Byte *buffer);
                   /* Process I021/165 data item */
static t_Retc proc_i021_170 (t_Ui16 length, t_Byte *buffer);
                   /* Process I021/170 data item */
static t_Retc proc_i021_180 (t_Ui16 length, t_Byte *buffer);
                   /* Process I021/180 data item */
static t_Retc proc_i021_190 (t_Ui16 length, t_Byte *buffer);
                   /* Process I021/190 data item */
static t_Retc proc_i021_200 (t_Ui16 length, t_Byte *buffer);
                   /* Process I021/200 data item */
static t_Retc proc_i021_210 (t_Ui16 length, t_Byte *buffer);
                   /* Process I021/210 data item */
static t_Retc proc_i021_220 (t_Ui16 length, t_Byte *buffer, t_Ui16 *pos_ptr);
                   /* Process I021/220 data item */
static t_Retc proc_i021_230 (t_Ui16 length, t_Byte *buffer);
                   /* Process I021/230 data item */
static t_Retc proc_i021_250 (t_Ui16 length, t_Byte *buffer);
                   /* Process I021/250 data item */
static t_Retc proc_i021_260 (t_Ui16 length, t_Byte *buffer);
                   /* Process I021/260 data item */
static t_Retc proc_i021_271 (t_Ui16 length, t_Byte *buffer, t_Ui16 *pos_ptr);
                   /* Process I021/271 data item */
static t_Retc proc_i021_295 (t_Ui16 length, t_Byte *buffer, t_Ui16 *pos_ptr);
                   /* Process I021/295 data item */
static t_Retc proc_i021_400 (t_Ui16 length, t_Byte *buffer);
                   /* Process I021/400 data item */
static t_Retc proc_i021_ref (t_Ui16 length, t_Byte *buffer, t_Ui16 *pos_ptr);
                   /* Process REF indicator data item */
static t_Retc proc_i021_spf (t_Ui16 length, t_Byte *buffer, t_Ui16 *pos_ptr);
                   /* Process SPF indicator data item */

/*----------------------------------------------------------------------------*/
/* astx_021        -- Process ASTERIX category 021 data block                 */
/*----------------------------------------------------------------------------*/

 t_Retc astx_021 (t_Ui16 length, t_Byte *buffer)
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
		adsb.asterix_category = 21;
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

					lrc = data_item (21, frn, std_uap[frn],
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

                   /* Process this ADS-B report: */
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
                   /* Description of data item I021/008: */
	if (reference_vsn == 14 || reference_vsn == 15 ||
        reference_vsn == 21 || reference_vsn == 24)
	{
		desc_i021_008.category = 21;
		desc_i021_008.data_item = 8;
		desc_i021_008.item_type = e_fixed_length_data_item;
		desc_i021_008.fixed_length = 1;
		desc_i021_008.proc_fptr = proc_i021_008;
		desc_i021_008.read_fptr = NULL;
	}

                   /* Description of data item I021/010: */
	desc_i021_010.category = 21;
	desc_i021_010.data_item = 10;
	desc_i021_010.item_type = e_fixed_length_data_item;
	desc_i021_010.fixed_length = 2;
	desc_i021_010.proc_fptr = proc_i021_010;
	desc_i021_010.read_fptr = NULL;

                   /* Description of data item I021/015: */
	if (reference_vsn == 14 || reference_vsn == 15 ||
        reference_vsn == 21 || reference_vsn == 24)
	{
		desc_i021_015.category = 21;
		desc_i021_015.data_item = 15;
		desc_i021_015.item_type = e_fixed_length_data_item;
		desc_i021_015.fixed_length = 1;
		desc_i021_015.proc_fptr = proc_i021_015;
		desc_i021_015.read_fptr = NULL;
	}

                   /* Description of data item I021/016: */
	if (reference_vsn == 14 || reference_vsn == 15 ||
        reference_vsn == 21 || reference_vsn == 24)
	{
		desc_i021_016.category = 21;
		desc_i021_016.data_item = 16;
		desc_i021_016.item_type = e_fixed_length_data_item;
		desc_i021_016.fixed_length = 1;
		desc_i021_016.proc_fptr = proc_i021_016;
		desc_i021_016.read_fptr = NULL;
	}

                   /* Description of data item I021/020: */
	desc_i021_020.category = 21;
	desc_i021_020.data_item = 20;
	desc_i021_020.item_type = e_fixed_length_data_item;
	desc_i021_020.fixed_length = 1;
	desc_i021_020.proc_fptr = proc_i021_020;
	desc_i021_020.read_fptr = NULL;

                   /* Description of data item I021/030: */
	if (reference_vsn != 14 && reference_vsn != 15 &&
        reference_vsn != 21 && reference_vsn != 24)
	{
		desc_i021_030.category = 21;
		desc_i021_030.data_item = 30;
		desc_i021_030.item_type = e_fixed_length_data_item;
		desc_i021_030.fixed_length = 3;
		desc_i021_030.proc_fptr = proc_i021_030;
		desc_i021_030.read_fptr = NULL;
	}

                   /* Description of data item I021/032: */
	if (reference_vsn != 14 && reference_vsn != 15 &&
        reference_vsn != 21 && reference_vsn != 24)
	{
		desc_i021_032.category = 21;
		desc_i021_032.data_item = 32;
		desc_i021_032.item_type = e_fixed_length_data_item;
		desc_i021_032.fixed_length = 1;
		desc_i021_032.proc_fptr = proc_i021_032;
		desc_i021_032.read_fptr = NULL;
	}

                   /* Description of data item I021/035: */
	if (reference_vsn != 14 && reference_vsn != 15 &&
        reference_vsn != 21 && reference_vsn != 24)
	{
		desc_i021_035.category = 21;
		desc_i021_035.data_item = 35;
		desc_i021_035.item_type = e_fixed_length_data_item;
		desc_i021_035.fixed_length = 4;
		desc_i021_035.proc_fptr = proc_i021_035;
		desc_i021_035.read_fptr = NULL;
	}

                   /* Description of data item I021/040: */
	desc_i021_040.category = 21;
	desc_i021_040.data_item = 40;
	if (reference_vsn == 1 || reference_vsn == 2)
	{
		desc_i021_040.item_type = e_variable_length_data_item;
		desc_i021_040.fixed_length = 0;
	}
	else if (reference_vsn == 9 || reference_vsn == 12)
	{
		desc_i021_040.item_type = e_fixed_length_data_item;
		desc_i021_040.fixed_length = 2;
	}
	else if (reference_vsn == 14 || reference_vsn == 15 ||
             reference_vsn == 21 || reference_vsn == 24)
	{
		desc_i021_040.item_type = e_variable_length_data_item;
		desc_i021_040.fixed_length = 0;
	}
	else
	{
		Assert (FALSE, "Data item type of I021/040 undefined");
	}
	desc_i021_040.proc_fptr = proc_i021_040;
	desc_i021_040.read_fptr = NULL;

                   /* Description of data item I021/070: */
	if (reference_vsn == 14 || reference_vsn == 15 ||
        reference_vsn == 21 || reference_vsn == 24)
	{
		desc_i021_070.category = 21;
		desc_i021_070.data_item = 70;
		desc_i021_070.item_type = e_fixed_length_data_item;
		desc_i021_070.fixed_length = 2;
		desc_i021_070.proc_fptr = proc_i021_070;
		desc_i021_070.read_fptr = NULL;
	}

                   /* Description of data item I021/071: */
	if (reference_vsn == 14 || reference_vsn == 15 ||
        reference_vsn == 21 || reference_vsn == 24)
	{
		desc_i021_071.category = 21;
		desc_i021_071.data_item = 71;
		desc_i021_071.item_type = e_fixed_length_data_item;
		desc_i021_071.fixed_length = 3;
		desc_i021_071.proc_fptr = proc_i021_071;
		desc_i021_071.read_fptr = NULL;
	}

                   /* Description of data item I021/072: */
	if (reference_vsn == 14 || reference_vsn == 15 ||
        reference_vsn == 21 || reference_vsn == 24)
	{
		desc_i021_072.category = 21;
		desc_i021_072.data_item = 72;
		desc_i021_072.item_type = e_fixed_length_data_item;
		desc_i021_072.fixed_length = 3;
		desc_i021_072.proc_fptr = proc_i021_072;
		desc_i021_072.read_fptr = NULL;
	}

                   /* Description of data item I021/073: */
	if (reference_vsn == 14 || reference_vsn == 15 ||
        reference_vsn == 21 || reference_vsn == 24)
	{
		desc_i021_073.category = 21;
		desc_i021_073.data_item = 73;
		desc_i021_073.item_type = e_fixed_length_data_item;
		desc_i021_073.fixed_length = 3;
		desc_i021_073.proc_fptr = proc_i021_073;
		desc_i021_073.read_fptr = NULL;
	}

                   /* Description of data item I021/074: */
	if (reference_vsn == 14 || reference_vsn == 15 ||
        reference_vsn == 21 || reference_vsn == 24)
	{
		desc_i021_074.category = 21;
		desc_i021_074.data_item = 74;
		desc_i021_074.item_type = e_fixed_length_data_item;
		desc_i021_074.fixed_length = 4;
		desc_i021_074.proc_fptr = proc_i021_074;
		desc_i021_074.read_fptr = NULL;
	}

                   /* Description of data item I021/075: */
	if (reference_vsn == 14 || reference_vsn == 15 ||
        reference_vsn == 21 || reference_vsn == 24)
	{
		desc_i021_075.category = 21;
		desc_i021_075.data_item = 75;
		desc_i021_075.item_type = e_fixed_length_data_item;
		desc_i021_075.fixed_length = 3;
		desc_i021_075.proc_fptr = proc_i021_075;
		desc_i021_075.read_fptr = NULL;
	}

                   /* Description of data item I021/076: */
	if (reference_vsn == 14 || reference_vsn == 15 ||
        reference_vsn == 21 || reference_vsn == 24)
	{
		desc_i021_076.category = 21;
		desc_i021_076.data_item = 76;
		desc_i021_076.item_type = e_fixed_length_data_item;
		desc_i021_076.fixed_length = 4;
		desc_i021_076.proc_fptr = proc_i021_076;
		desc_i021_076.read_fptr = NULL;
	}

                   /* Description of data item I021/077: */
	if (reference_vsn == 14 || reference_vsn == 15 ||
        reference_vsn == 21 || reference_vsn == 24)
	{
		desc_i021_077.category = 21;
		desc_i021_077.data_item = 77;
		desc_i021_077.item_type = e_fixed_length_data_item;
		desc_i021_077.fixed_length = 3;
		desc_i021_077.proc_fptr = proc_i021_077;
		desc_i021_077.read_fptr = NULL;
	}

                   /* Description of data item I021/080: */
	desc_i021_080.category = 21;
	desc_i021_080.data_item = 80;
	desc_i021_080.item_type = e_fixed_length_data_item;
	desc_i021_080.fixed_length = 3;
	desc_i021_080.proc_fptr = proc_i021_080;
	desc_i021_080.read_fptr = NULL;

                   /* Description of data item I021/090: */
	if (reference_vsn == 1 || reference_vsn == 2 ||
        reference_vsn == 9 || reference_vsn == 12)
	{
		desc_i021_090.category = 21;
		desc_i021_090.data_item = 90;
		desc_i021_090.item_type = e_fixed_length_data_item;
		if (reference_vsn == 1 || reference_vsn == 2)
		{
			desc_i021_090.fixed_length = 1;
		}
		else if (reference_vsn == 9 || reference_vsn == 12)
		{
			desc_i021_090.fixed_length = 2;
		}
		else
		{
			Assert (FALSE, "Fixed length of I021/090 undefined");
		}
		desc_i021_090.proc_fptr = proc_i021_090;
		desc_i021_090.read_fptr = NULL;
	}
	else if (reference_vsn == 14 || reference_vsn == 15 ||
             reference_vsn == 21 || reference_vsn == 24)
	{
		desc_i021_090.category = 21;
		desc_i021_090.data_item = 90;
		desc_i021_090.item_type = e_variable_length_data_item;
		desc_i021_090.fixed_length = 0;
		desc_i021_090.proc_fptr = proc_i021_090;
		desc_i021_090.read_fptr = NULL;
	}

                   /* Description of data item I021/095: */
	if (reference_vsn != 14 && reference_vsn != 15 &&
        reference_vsn != 21 && reference_vsn != 24)
	{
		desc_i021_095.category = 21;
		desc_i021_095.data_item = 95;
		desc_i021_095.item_type = e_fixed_length_data_item;
		desc_i021_095.fixed_length = 1;
		desc_i021_095.proc_fptr = proc_i021_095;
		desc_i021_095.read_fptr = NULL;
	}

                   /* Description of data item I021/100: */
	if (reference_vsn != 14 && reference_vsn != 15 &&
        reference_vsn != 21 && reference_vsn != 24)
	{
		desc_i021_100.category = 21;
		desc_i021_100.data_item = 100;
		desc_i021_100.item_type = e_fixed_length_data_item;
		desc_i021_100.fixed_length = 1;
		desc_i021_100.proc_fptr = proc_i021_100;
		desc_i021_100.read_fptr = NULL;
	}

                   /* Description of data item I021/110: */
	desc_i021_110.category = 21;
	desc_i021_110.data_item = 110;
	if (reference_vsn == 1 || reference_vsn == 2)
	{
		desc_i021_110.item_type = e_repetitive_data_item;
		desc_i021_110.fixed_length = 11;
		desc_i021_110.proc_fptr = proc_i021_110;
		desc_i021_110.read_fptr = NULL;
	}
	else if (reference_vsn == 9 || reference_vsn == 12 ||
             reference_vsn == 14 || reference_vsn == 15 ||
             reference_vsn == 21 || reference_vsn == 24)
	{
		desc_i021_110.item_type = e_immediate_data_item;
		desc_i021_110.fixed_length = 0;
		desc_i021_110.proc_fptr = NULL;
		desc_i021_110.read_fptr = proc_i021_110;
	}
	else
	{
		Assert (FALSE, "Item type of I021/110 undefined");
	}

                   /* Description of data item I021/130: */
	desc_i021_130.category = 21;
	desc_i021_130.data_item = 130;
	desc_i021_130.item_type = e_fixed_length_data_item;
	desc_i021_130.fixed_length = 6;
	desc_i021_130.proc_fptr = proc_i021_130;
	desc_i021_130.read_fptr = NULL;

                   /* Description of data item I021/131: */
	if (reference_vsn == 14 || reference_vsn == 15 ||
        reference_vsn == 21 || reference_vsn == 24)
	{
		desc_i021_131.category = 21;
		desc_i021_131.data_item = 131;
		desc_i021_131.item_type = e_fixed_length_data_item;
		desc_i021_131.fixed_length = 8;
		desc_i021_131.proc_fptr = proc_i021_131;
		desc_i021_131.read_fptr = NULL;
	}

                   /* Description of data item I021/132: */
	if (reference_vsn == 14 || reference_vsn == 15 ||
        reference_vsn == 21 || reference_vsn == 24)
	{
		desc_i021_132.category = 21;
		desc_i021_132.data_item = 132;
		desc_i021_132.item_type = e_fixed_length_data_item;
		desc_i021_132.fixed_length = 1;
		desc_i021_132.proc_fptr = proc_i021_132;
		desc_i021_132.read_fptr = NULL;
	}

                   /* Description of data item I021/140: */
	desc_i021_140.category = 21;
	desc_i021_140.data_item = 140;
	desc_i021_140.item_type = e_fixed_length_data_item;
	desc_i021_140.fixed_length = 2;
	desc_i021_140.proc_fptr = proc_i021_140;
	desc_i021_140.read_fptr = NULL;

                   /* Description of data item I021/145: */
	desc_i021_145.category = 21;
	desc_i021_145.data_item = 145;
	desc_i021_145.item_type = e_fixed_length_data_item;
	desc_i021_145.fixed_length = 2;
	desc_i021_145.proc_fptr = proc_i021_145;
	desc_i021_145.read_fptr = NULL;

                   /* Description of data item I021/146: */
	desc_i021_146.category = 21;
	desc_i021_146.data_item = 146;
	desc_i021_146.item_type = e_fixed_length_data_item;
	desc_i021_146.fixed_length = 2;
	desc_i021_146.proc_fptr = proc_i021_146;
	desc_i021_146.read_fptr = NULL;

                   /* Description of data item I021/147: */
	if (reference_vsn != 14 && reference_vsn != 15 &&
        reference_vsn != 21 && reference_vsn != 24)
	{
		desc_i021_147.category = 21;
		desc_i021_147.data_item = 147;
		desc_i021_147.item_type = e_fixed_length_data_item;
		desc_i021_147.fixed_length = 2;
		desc_i021_147.proc_fptr = proc_i021_147;
		desc_i021_147.read_fptr = NULL;
	}

                   /* Description of data item I021/148: */
	desc_i021_148.category = 21;
	desc_i021_148.data_item = 148;
	desc_i021_148.item_type = e_fixed_length_data_item;
	desc_i021_148.fixed_length = 2;
	desc_i021_148.proc_fptr = proc_i021_148;
	desc_i021_148.read_fptr = NULL;

                   /* Description of data item I021/150: */
	desc_i021_150.category = 21;
	desc_i021_150.data_item = 150;
	desc_i021_150.item_type = e_fixed_length_data_item;
	if (reference_vsn == 1 || reference_vsn == 2)
	{
		desc_i021_150.fixed_length = 6;
	}
	else if (reference_vsn == 9 || reference_vsn == 12)
	{
		desc_i021_150.fixed_length = 2;
	}
	else if (reference_vsn == 14 || reference_vsn == 15 ||
             reference_vsn == 21 || reference_vsn == 24)
	{
		desc_i021_150.fixed_length = 2;
	}
	else
	{
		Assert (FALSE, "Fixed length for I021/150 undefined");
	}
	desc_i021_150.proc_fptr = proc_i021_150;
	desc_i021_150.read_fptr = NULL;

                   /* Description of data item I021/151: */
	desc_i021_151.category = 21;
	desc_i021_151.data_item = 151;
	desc_i021_151.item_type = e_fixed_length_data_item;
	desc_i021_151.fixed_length = 2;
	desc_i021_151.proc_fptr = proc_i021_151;
	desc_i021_151.read_fptr = NULL;

                   /* Description of data item I021/152: */
	desc_i021_152.category = 21;
	desc_i021_152.data_item = 152;
	desc_i021_152.item_type = e_fixed_length_data_item;
	desc_i021_152.fixed_length = 2;
	desc_i021_152.proc_fptr = proc_i021_152;
	desc_i021_152.read_fptr = NULL;

                   /* Description of data item I021/155: */
	desc_i021_155.category = 21;
	desc_i021_155.data_item = 155;
	desc_i021_155.item_type = e_fixed_length_data_item;
	desc_i021_155.fixed_length = 2;
	desc_i021_155.proc_fptr = proc_i021_155;
	desc_i021_155.read_fptr = NULL;

                   /* Description of data item I021/157: */
	desc_i021_157.category = 21;
	desc_i021_157.data_item = 157;
	desc_i021_157.item_type = e_fixed_length_data_item;
	desc_i021_157.fixed_length = 2;
	desc_i021_157.proc_fptr = proc_i021_157;
	desc_i021_157.read_fptr = NULL;

                   /* Description of data item I021/160: */
	desc_i021_160.category = 21;
	desc_i021_160.data_item = 160;
	desc_i021_160.item_type = e_fixed_length_data_item;
	if (reference_vsn == 1 || reference_vsn == 2)
	{
		desc_i021_160.fixed_length = 6;
	}
	else if (reference_vsn == 9 || reference_vsn == 12)
	{
		desc_i021_160.fixed_length = 4;
	}
	else if (reference_vsn == 14 || reference_vsn == 15 ||
             reference_vsn == 21 || reference_vsn == 24)
	{
		desc_i021_160.fixed_length = 4;
	}
	else
	{
		Assert (FALSE, "Fixed length of I021/160 undefined");
	}
	desc_i021_160.proc_fptr = proc_i021_160;
	desc_i021_160.read_fptr = NULL;

                   /* Description of data item I021/161: */
	if (reference_vsn == 14 || reference_vsn == 15 ||
        reference_vsn == 21 || reference_vsn == 24)
	{
		desc_i021_161.category = 21;
		desc_i021_161.data_item = 161;
		desc_i021_161.item_type = e_fixed_length_data_item;
		desc_i021_161.fixed_length = 2;
		desc_i021_161.proc_fptr = proc_i021_161;
		desc_i021_161.read_fptr = NULL;
	}

                   /* Description of data item I021/165: */
	desc_i021_165.category = 21;
	desc_i021_165.data_item = 165;
	if (reference_vsn != 14 && reference_vsn != 15 &&
        reference_vsn != 21 && reference_vsn != 24)
	{
		desc_i021_165.item_type = e_variable_length_data_item;
		desc_i021_165.fixed_length = 0;
	}
	else
	{
		desc_i021_165.item_type = e_fixed_length_data_item;
		desc_i021_165.fixed_length = 2;
	}
	desc_i021_165.proc_fptr = proc_i021_165;
	desc_i021_165.read_fptr = NULL;

                   /* Description of data item I021/170: */
	desc_i021_170.category = 21;
	desc_i021_170.data_item = 170;
	desc_i021_170.item_type = e_fixed_length_data_item;
	desc_i021_170.fixed_length = 6;
	desc_i021_170.proc_fptr = proc_i021_170;
	desc_i021_170.read_fptr = NULL;

                   /* Description of data item I021/180: */
	if (reference_vsn != 14 && reference_vsn != 15 &&
        reference_vsn != 21 && reference_vsn != 24)
	{
		desc_i021_180.category = 21;
		desc_i021_180.data_item = 180;
		desc_i021_180.item_type = e_fixed_length_data_item;
		desc_i021_180.fixed_length = 2;
		desc_i021_180.proc_fptr = proc_i021_180;
		desc_i021_180.read_fptr = NULL;
	}

                   /* Description of data item I021/190: */
	if (reference_vsn != 14 && reference_vsn != 15 &&
        reference_vsn != 21 && reference_vsn != 24)
	{
		desc_i021_190.category = 21;
		desc_i021_190.data_item = 190;
		desc_i021_190.item_type = e_fixed_length_data_item;
		desc_i021_190.fixed_length = 4;
		desc_i021_190.proc_fptr = proc_i021_190;
		desc_i021_190.read_fptr = NULL;
	}

                   /* Description of data item I021/200: */
	desc_i021_200.category = 21;
	desc_i021_200.data_item = 200;
	desc_i021_200.item_type = e_fixed_length_data_item;
	desc_i021_200.fixed_length = 1;
	desc_i021_200.proc_fptr = proc_i021_200;
	desc_i021_200.read_fptr = NULL;

                   /* Description of data item I021/210: */
	desc_i021_210.category = 21;
	desc_i021_210.data_item = 210;
	desc_i021_210.item_type = e_fixed_length_data_item;
	desc_i021_210.fixed_length = 1;
	desc_i021_210.proc_fptr = proc_i021_210;
	desc_i021_210.read_fptr = NULL;

                   /* Description of data item I021/220: */
	desc_i021_220.category = 21;
	desc_i021_220.data_item = 220;
	desc_i021_220.item_type = e_immediate_data_item;
	desc_i021_220.fixed_length = 0;
	desc_i021_220.proc_fptr = NULL;
	desc_i021_220.read_fptr = proc_i021_220;

                   /* Description of data item I021/230: */
	desc_i021_230.category = 21;
	desc_i021_230.data_item = 230;
	desc_i021_230.item_type = e_fixed_length_data_item;
	desc_i021_230.fixed_length = 2;
	desc_i021_230.proc_fptr = proc_i021_230;
	desc_i021_230.read_fptr = NULL;

                   /* Description of data item I021/250: */
	if (reference_vsn == 14 || reference_vsn == 15 ||
        reference_vsn == 21 || reference_vsn == 24)
	{
		desc_i021_250.category = 21;
		desc_i021_250.data_item = 250;
		desc_i021_250.item_type = e_repetitive_data_item;
		desc_i021_250.fixed_length = 8;
		desc_i021_250.proc_fptr = proc_i021_250;
		desc_i021_250.read_fptr = NULL;
	}

                   /* Description of data item I021/260: */
	if (reference_vsn == 14 || reference_vsn == 15 ||
        reference_vsn == 21 || reference_vsn == 24)
	{
		desc_i021_260.category = 21;
		desc_i021_260.data_item = 260;
		desc_i021_260.item_type = e_fixed_length_data_item;
		desc_i021_260.fixed_length = 7;
		desc_i021_260.proc_fptr = proc_i021_260;
		desc_i021_260.read_fptr = NULL;
	}

                   /* Description of data item I021/271: */
	if (reference_vsn == 14 || reference_vsn == 15 ||
        reference_vsn == 21 || reference_vsn == 24)
	{
		desc_i021_271.category = 21;
		desc_i021_271.data_item = 271;
		desc_i021_271.item_type = e_immediate_data_item;
		desc_i021_271.fixed_length = 0;
		desc_i021_271.proc_fptr = NULL;
		desc_i021_271.read_fptr = proc_i021_271;
	}

                   /* Description of data item I021/295: */
	if (reference_vsn == 14 || reference_vsn == 15 ||
        reference_vsn == 21 || reference_vsn == 24)
	{
		desc_i021_295.category = 21;
		desc_i021_295.data_item = 295;
		desc_i021_295.item_type = e_immediate_data_item;
		desc_i021_295.fixed_length = 0;
		desc_i021_295.proc_fptr = NULL;
		desc_i021_295.read_fptr = proc_i021_295;
	}

                   /* Description of data item I021/400: */
	if (reference_vsn == 14 || reference_vsn == 15 ||
        reference_vsn == 21 || reference_vsn == 24)
	{
		desc_i021_400.category = 21;
		desc_i021_400.data_item = 400;
		desc_i021_400.item_type = e_fixed_length_data_item;
		desc_i021_400.fixed_length = 1;
		desc_i021_400.proc_fptr = proc_i021_400;
		desc_i021_400.read_fptr = NULL;
	}

                   /* Description of data item I021/REF: */
	desc_i021_ref.category = 21;
	desc_i021_ref.data_item = M_REF_INDICATOR;
	desc_i021_ref.item_type = e_immediate_data_item;
	desc_i021_ref.fixed_length = 0;
	desc_i021_ref.proc_fptr = NULL;
	desc_i021_ref.read_fptr = proc_i021_ref;

                   /* Description of data item I021/SPF: */
	desc_i021_spf.category = 21;
	desc_i021_spf.data_item = M_SPF_INDICATOR;
	desc_i021_spf.item_type = e_immediate_data_item;
	desc_i021_spf.fixed_length = 0;
	desc_i021_spf.proc_fptr = NULL;
	desc_i021_spf.read_fptr = proc_i021_spf;

                   /* We are done: */
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

	if (reference_vsn == 1 || reference_vsn == 2)
	{
		std_uap[1] = &desc_i021_010;
		std_uap[2] = &desc_i021_020;
		std_uap[3] = &desc_i021_030;
		std_uap[4] = &desc_i021_035;
		std_uap[5] = &desc_i021_080;
		std_uap[6] = &desc_i021_090;
		std_uap[7] = &desc_i021_100;

		std_uap[8] = &desc_i021_110;
		std_uap[9] = &desc_i021_130;
		std_uap[10] = &desc_i021_140;
		std_uap[11] = &desc_i021_145;
		std_uap[12] = &desc_i021_150;
		std_uap[13] = &desc_i021_160;
		std_uap[14] = &desc_i021_165;

		std_uap[15] = &desc_i021_170;
		std_uap[16] = &desc_i021_180;
		std_uap[17] = &desc_i021_190;
		std_uap[18] = &desc_i021_095;
		std_uap[19] = &desc_i021_147;
		std_uap[20] = &desc_i021_200;

		if (reference_vsn == 2)
		{
			std_uap[21] = &desc_i021_032;
			std_uap[22] = &desc_i021_210;
			std_uap[23] = &desc_i021_040;
		}

		std_uap[27] = &desc_i021_ref;
		std_uap[28] = &desc_i021_spf;
	}
	else if (reference_vsn == 9 || reference_vsn == 12)
	{
		std_uap[1] = &desc_i021_010;
		std_uap[2] = &desc_i021_040;
		std_uap[3] = &desc_i021_030;
		std_uap[4] = &desc_i021_130;
		std_uap[5] = &desc_i021_080;
		std_uap[6] = &desc_i021_140;
		std_uap[7] = &desc_i021_090;

		std_uap[8] = &desc_i021_210;
		std_uap[9] = &desc_i021_230;
		std_uap[10] = &desc_i021_145;
		std_uap[11] = &desc_i021_150;
		std_uap[12] = &desc_i021_151;
		std_uap[13] = &desc_i021_152;
		std_uap[14] = &desc_i021_155;

		std_uap[15] = &desc_i021_157;
		std_uap[16] = &desc_i021_160;
		std_uap[17] = &desc_i021_165;
		std_uap[18] = &desc_i021_170;
		std_uap[19] = &desc_i021_095;
		std_uap[20] = &desc_i021_032;
		std_uap[21] = &desc_i021_200;

		std_uap[22] = &desc_i021_020;
		std_uap[23] = &desc_i021_220;
		std_uap[24] = &desc_i021_146;
		std_uap[25] = &desc_i021_148;
		std_uap[26] = &desc_i021_110;

		std_uap[34] = &desc_i021_ref;
		std_uap[35] = &desc_i021_spf;
	}
	else if (reference_vsn == 14 || reference_vsn == 15 ||
             reference_vsn == 21 || reference_vsn == 24)
	{
		std_uap[1] = &desc_i021_010;
		std_uap[2] = &desc_i021_040;
		std_uap[3] = &desc_i021_161;
		std_uap[4] = &desc_i021_015;
		std_uap[5] = &desc_i021_071;
		std_uap[6] = &desc_i021_130;
		std_uap[7] = &desc_i021_131;

		std_uap[8] = &desc_i021_072;
		std_uap[9] = &desc_i021_150;
		std_uap[10] = &desc_i021_151;
		std_uap[11] = &desc_i021_080;
		std_uap[12] = &desc_i021_073;
		std_uap[13] = &desc_i021_074;
		std_uap[14] = &desc_i021_075;

		std_uap[15] = &desc_i021_076;
		std_uap[16] = &desc_i021_140;
		std_uap[17] = &desc_i021_090;
		std_uap[18] = &desc_i021_210;
		std_uap[19] = &desc_i021_070;
		std_uap[20] = &desc_i021_230;
		std_uap[21] = &desc_i021_145;

		std_uap[22] = &desc_i021_152;
		std_uap[23] = &desc_i021_200;
		std_uap[24] = &desc_i021_155;
		std_uap[25] = &desc_i021_157;
		std_uap[26] = &desc_i021_160;
		std_uap[27] = &desc_i021_165;
		std_uap[28] = &desc_i021_077;

		std_uap[29] = &desc_i021_170;
		std_uap[30] = &desc_i021_020;
		std_uap[31] = &desc_i021_220;
		std_uap[32] = &desc_i021_146;
		std_uap[33] = &desc_i021_148;
		std_uap[34] = &desc_i021_110;
		std_uap[35] = &desc_i021_016;

		std_uap[36] = &desc_i021_008;
		std_uap[37] = &desc_i021_271;
		std_uap[38] = &desc_i021_132;
		std_uap[39] = &desc_i021_250;
		std_uap[40] = &desc_i021_260;
		std_uap[41] = &desc_i021_400;
		std_uap[42] = &desc_i021_295;

		std_uap[48] = &desc_i021_ref;
		std_uap[49] = &desc_i021_spf;
	}

                   /* We are done: */
	return;
}

/*----------------------------------------------------------------------------*/
/* proc_i021_008   -- Process I021/008 data item                              */
/*----------------------------------------------------------------------------*/

 static
 t_Retc proc_i021_008 (t_Ui16 length, t_Byte *buffer)
{
	t_Byte arv;    /* Air-referenced velocity report capability */
	t_Byte cdti;   /* Cockpit display of traffic information */
	t_Byte df1;    /* Data field octet 1 */
	t_Byte ra;     /* TCAS resolution advisory active */
	t_Retc ret;    /* Return code */
	t_Byte sa;     /* Single antenna */
	t_Byte tc;     /* Target change report capability */
	t_Byte tcas;   /* TCAS system status */
	t_Byte ts;     /* Target state report capability */

                   /* Preset the return code: */
	ret = RC_FAIL;

                   /* Check parameters: */
	Assert (length == 1, "Invalid parameter");
	Assert (buffer != NULL, "Invalid parameter");

                   /* Check reference version: */
	Assert (reference_vsn == 14 || reference_vsn == 15 ||
            reference_vsn == 21 || reference_vsn == 24,
            "Data item not defined");

                   /* Extract octets: */
	df1 = buffer[0];

                   /* Extract information: */
	ra = (df1 >> 7) & 0x01;
	tc = (df1 >> 5) & 0x03;
	ts = (df1 >> 4) & 0x01;
	arv = (df1 >> 3) & 0x01;
	cdti = (df1 >> 2) & 0x01;
	tcas = (df1 >> 1) & 0x01;
	sa = 0;
	if (reference_vsn == 21 || reference_vsn == 24)
	{
		sa = df1 & 0x01;
	}

#if LISTER
                   /* List data field: */
	list_text (2, ";  Aircraft Operational Status:");
	list_text (2, " ra=" M_FMT_BYTE ";", ra);
	list_text (2, " tc=" M_FMT_BYTE ";", tc);
	list_text (2, " ts=" M_FMT_BYTE ";", ts);
	list_text (2, " arv=" M_FMT_BYTE ";", arv);
	list_text (2, " cdti=" M_FMT_BYTE ";", cdti);
	if (reference_vsn == 14)
	{
		list_text (2, " tcas=" M_FMT_BYTE, tcas);
	}
	else if (reference_vsn == 15 ||
             reference_vsn == 21 || reference_vsn == 24)
	{
		list_text (2, " not_tcas=" M_FMT_BYTE, tcas);
	}
	if (reference_vsn == 21 || reference_vsn == 24)
	{
		list_text (2, " sa=" M_FMT_BYTE ";", sa);
	}
	list_text (2, "\n");
#endif /* LISTER */

                   /* Store this information: */
	adsb.aircraft_operational_status.present = TRUE;
	adsb.aircraft_operational_status.value_arv = arv;
	adsb.aircraft_operational_status.value_cdti = cdti;
	adsb.aircraft_operational_status.value_ra = ra;
	adsb.aircraft_operational_status.value_sa = 0;
	if (reference_vsn == 21 || reference_vsn == 24)
	{
		adsb.aircraft_operational_status.value_sa = sa;
	}
	adsb.aircraft_operational_status.value_tc = tc;
	adsb.aircraft_operational_status.value_tcas = tcas;
	adsb.aircraft_operational_status.value_ts = ts;

                   /* Set the return code: */
	ret = RC_OKAY;

                   /* We are done: */
	return ret;
}

/*----------------------------------------------------------------------------*/
/* proc_i021_010   -- Process I021/010 data item                              */
/*----------------------------------------------------------------------------*/

 static
 t_Retc proc_i021_010 (t_Ui16 length, t_Byte *buffer)
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

                   /* We are done: */
	return ret;
}

/*----------------------------------------------------------------------------*/
/* proc_i021_015   -- Process I021/015 data item                              */
/*----------------------------------------------------------------------------*/

 static
 t_Retc proc_i021_015 (t_Ui16 length, t_Byte *buffer)
{
	t_Byte df1;    /* Data field octet 1 */
	t_Retc ret;    /* Return code */

                   /* Preset the return code: */
	ret = RC_FAIL;

                   /* Check parameters: */
	Assert (length == 1, "Invalid parameter");
	Assert (buffer != NULL, "Invalid parameter");

                   /* Check reference version: */
	Assert (reference_vsn == 14 || reference_vsn == 15 ||
            reference_vsn == 21 || reference_vsn == 24,
            "Data item not defined");

				   /* Extract octets: */
	df1 = buffer[0];

#if LISTER
				   /* List data field: */
	list_text (2, ";  Service Identification:");
	list_text (2, " " M_FMT_BYTE, df1);
	list_text (2, "\n");
#endif /* LISTER */

                   /* Store this information: */
	adsb.service_identification.present = TRUE;
	adsb.service_identification.value = df1;

                   /* Set the return code: */
	ret = RC_OKAY;

                   /* We are done: */
	return ret;
}

/*----------------------------------------------------------------------------*/
/* proc_i021_016   -- Process I021/016 data item                              */
/*----------------------------------------------------------------------------*/

 static
 t_Retc proc_i021_016 (t_Ui16 length, t_Byte *buffer)
{
	t_Byte df1;    /* Data field octet 1 */
	t_Retc ret;    /* Return code */

                   /* Preset the return code: */
	ret = RC_FAIL;

                   /* Check parameters: */
	Assert (length == 1, "Invalid parameter");
	Assert (buffer != NULL, "Invalid parameter");

                   /* Check reference version: */
	Assert (reference_vsn == 14 || reference_vsn == 15 ||
            reference_vsn == 21 || reference_vsn == 24,
            "Data item not defined");

                   /* Extract octets: */
	df1 = buffer[0];

#if LISTER
                   /* List data field: */
	list_text (2, ";  Service Management:");
	list_text (2, " rp=" M_FMT_BYTE, df1);
	if (df1 == 0)
	{
		list_text (2, " (data driven mode)");
	}
	else
	{
		list_text (2, " (%.1f sec)", 0.5 * df1);
	}
	list_text (2, "\n");
#endif /* LISTER */

                   /* Store this information: */
	adsb.service_management.present = TRUE;
	adsb.service_management.value_rp = 0.5 * df1;

				   /* Set the return code: */
	ret = RC_OKAY;

                   /* We are done: */
	return ret;
}

/*----------------------------------------------------------------------------*/
/* proc_i021_020   -- Process I021/020 data item                              */
/*----------------------------------------------------------------------------*/

 static
 t_Retc proc_i021_020 (t_Ui16 length, t_Byte *buffer)
{
	t_Byte df1;    /* Data field octet 1 */
	t_Byte emc;    /* Emitter category */
	t_Retc ret;    /* Return code */

				   /* Preset the return code: */
	ret = RC_FAIL;

				   /* Check parameters: */
	Assert (length == 1, "Invalid parameter");
	Assert (buffer != NULL, "Invalid parameter");

				   /* Extract octets: */
	df1 = buffer[0];

				   /* Extract information: */
	emc = df1;

#if LISTER
				   /* List data field: */
	list_text (2, ";  Emitter Category:");
	list_text (2, " " M_FMT_BYTE, emc);
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
	adsb.emitter_category.present = TRUE;
	adsb.emitter_category.value = emc;

                   /* Set the return code: */
	ret = RC_OKAY;

                   /* We are done: */
	return ret;
}

/*----------------------------------------------------------------------------*/
/* proc_i021_030   -- Process I021/030 data item                              */
/*----------------------------------------------------------------------------*/

 static
 t_Retc proc_i021_030 (t_Ui16 length, t_Byte *buffer)
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

                   /* Check reference version: */
	Assert (reference_vsn != 14 && reference_vsn != 15 &&
            reference_vsn != 21 && reference_vsn != 24,
            "Data item not defined");

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

                   /* We are done: */
	return ret;
}

/*----------------------------------------------------------------------------*/
/* proc_i021_032   -- Process I021/032 data item                              */
/*----------------------------------------------------------------------------*/

 static
 t_Retc proc_i021_032 (t_Ui16 length, t_Byte *buffer)
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

                   /* Check reference version: */
	Assert (reference_vsn != 14 && reference_vsn != 15 &&
            reference_vsn != 21 && reference_vsn != 24,
            "Data item not defined");

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

                   /* We are done: */
	return ret;
}

/*----------------------------------------------------------------------------*/
/* proc_i021_035   -- Process I021/035 data item                              */
/*----------------------------------------------------------------------------*/

 static
 t_Retc proc_i021_035 (t_Ui16 length, t_Byte *buffer)
{
	t_Byte df1;    /* Data field octet 1 */
	t_Byte df2;    /* Data field octet 2 */
	t_Byte df3;    /* Data field octet 3 */
	t_Byte df4;    /* Data field octet 4 */
	t_Retc ret;    /* Return code */
	t_Ui32 tor;    /* Time of receipt; 2**-15 seconds */

                   /* Preset the return code: */
	ret = RC_FAIL;

                   /* Check parameters: */
	Assert (length == 4, "Invalid parameter");
	Assert (buffer != NULL, "Invalid parameter");

                   /* Check reference version: */
	Assert (reference_vsn != 14 && reference_vsn != 15 &&
            reference_vsn != 21 && reference_vsn != 24,
            "Data item not defined");

                   /* Extract octets: */
	df1 = buffer[0];
	df2 = buffer[1];
	df3 = buffer[2];
	df4 = buffer[3];

                   /* Extract information: */
	tor = make_ui32 (df1, df2, df3, df4);

#if LISTER
                   /* List data field: */
	list_text (2, ";  Time of Receipt:");
	list_text (2, " 0x" M_FMT_UI32_HEX " (" M_FMT_UI32 ")", tor, tor);
	list_text (2, "\n");
#endif /* LISTER */

                   /* Store this information: */
	adsb.time_of_receipt.present = TRUE;
	adsb.time_of_receipt.value = (1.0 / 32768.0) * tor;

                   /* Set the return code: */
	ret = RC_OKAY;

                   /* We are done: */
	return ret;
}

/*----------------------------------------------------------------------------*/
/* proc_i021_040   -- Process I021/040 data item                              */
/*----------------------------------------------------------------------------*/

 static
 t_Retc proc_i021_040 (t_Ui16 length, t_Byte *buffer)
{
	t_Byte df1;    /* Data field octet 1 */
	t_Byte df2;    /* Data field octet 2 */
	int ix;        /* Auxiliary */
	t_Retc ret;    /* Return code */

                   /* Preset the return code: */
	ret = RC_FAIL;

                   /* Check parameters: */
	if (reference_vsn == 1 || reference_vsn == 2)
	{
		Assert (length > 0, "Invalid parameter");
	}
	else if (reference_vsn == 9 || reference_vsn == 12)
	{
		Assert (length == 2, "Invalid parameter");
	}
	else if (reference_vsn == 14 || reference_vsn == 15 ||
             reference_vsn == 21 || reference_vsn == 24)
	{
		Assert (length > 0, "Invalid parameter");
	}
	Assert (buffer != NULL, "Invalid parameter");

                   /* Extract information: */
	if (reference_vsn == 9 || reference_vsn == 12)
	{
		df1 = buffer[0];
		df2 = buffer[1];
	}

#if LISTER
                   /* List data field: */
	list_text (2, ";  Target Report Descriptor:\n");
	if (reference_vsn == 1 || reference_vsn == 2)
	{
		for (ix = 0; ix < length; ix ++)
		{
			df1 = buffer[ix];

			if (ix == 0)
			{
				t_Byte typ;

				list_text (2, ";   ");

				typ = (df1 >> 5) & 0x07;
				switch (typ)
				{
				case 0:
					list_text (2, "SSR multilateration");
					break;
				case 1:
					list_text (2, "Mode S multilateration");
					break;
				case 2:
					list_text (2, "ADS-B");
					break;
				case 3:
					list_text (2, "PSR");
					break;
				case 4:
					list_text (2, "Magnetic Loop System");
					break;
				case 5:
					list_text (2, "HF multilateration");
					break;
				case 6:
					list_text (2, "Not defined");
					break;
				case 7:
					list_text (2, "Other types");
					break;
				}

				if (df1 & 0x10)
				{
					list_text (2, " DCR");
				}

				if (df1 & 0x08)
				{
					list_text (2, " CH1");
				}
				else
				{
					list_text (2, " CH2");
				}

				if (df1 & 0x04)
				{
					list_text (2, " GBS");
				}

				if (df1 & 0x02)
				{
					list_text (2, " CRT");
				}

				list_text (2, "\n");
			}
			else if (ix == 1)
			{
				t_Byte lop;
				t_Byte tot;

				list_text (2, ";  ");

				if (df1 & 0x80)
				{
					list_text (2, " SIM");
				}

				if (df1 & 0x40)
				{
					list_text (2, " TST");
				}

				if (df1 & 0x20)
				{
					list_text (2, " RAB");
				}

				lop = (df1 >> 3) & 0x03;
				list_text (2, " LOP=" M_FMT_BYTE, lop);

				tot = (df1 >> 1) & 0x03;
				list_text (2, " TOT=" M_FMT_BYTE, tot);

				list_text (2, "\n");
			}
			else if (ix == 2)
			{
				list_text (2, ";  ");

				if (df1 & 0x80)
				{
					list_text (2, " SPI");
				}

				if (df1 & 0x40)
				{
					list_text (2, " ARC=100 ft");
				}
				else
				{
					list_text (2, " ARC=25 ft");
				}

				/* tba... */
				list_text (2, "\n");
			}
		}
	}
	else if (reference_vsn == 9 || reference_vsn == 12)
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
	else if (reference_vsn == 14 || reference_vsn == 15 ||
             reference_vsn == 21)
	{
		for (ix = 0; ix < length; ix ++)
		{
			df1 = buffer[ix];

			if (ix == 0)
			{
				t_Byte arc;
				t_Byte atp;
				t_Byte rab;
				t_Byte rc;

				list_text (2, ";   ");

				atp = (df1 >> 5) & 0x07;
				list_text (2, "atp=" M_FMT_BYTE, atp);
				Assert (atp <= 7, "Domain violation");
				list_text (2, " (%s)", atp_texts[atp]);
				list_text (2, "; ");

				arc = (df1 >> 3) & 0x03;
				list_text (2, "arc=" M_FMT_BYTE, arc);
				Assert (arc <= 3, "Domain violation");
				list_text (2, " (%s)", arc_texts[arc]);
				list_text (2, "; ", arc);

				rc = (df1 >> 2) & 0x01;
				list_text (2, "rc=" M_FMT_BYTE "; ", rc);

				rab = (df1 >> 1) & 0x01;
				list_text (2, "rab=" M_FMT_BYTE, rab);

				list_text (2, "\n");
			}
			else if (ix == 1)
			{
				t_Byte cl;
				t_Byte dcr;
				t_Byte gbs;
				t_Byte saa;
				t_Byte sim;
				t_Byte tst;

				list_text (2, ";   ");

				dcr = (df1 >> 7) & 0x01;
				list_text (2, "dcr=" M_FMT_BYTE "; ", dcr);

				gbs = (df1 >> 6) & 0x01;
				list_text (2, "gbs=" M_FMT_BYTE "; ", gbs);

				sim = (df1 >> 5) & 0x01;
				list_text (2, "sim=" M_FMT_BYTE "; ", sim);

				tst = (df1 >> 4) & 0x01;
				list_text (2, "tst=" M_FMT_BYTE "; ", tst);

				saa = (df1 >> 3) & 0x01;
				list_text (2, "saa=" M_FMT_BYTE "; ", saa);

				cl = (df1 >> 1) & 0x03;
				list_text (2, "cl=" M_FMT_BYTE, cl);
				Assert (cl <= 3, "Domain violation");
				list_text (2, " (%s)", cl_texts[cl]);

				list_text (2, "\n");
			}
			else if (ix == 2)
			{
				t_Byte cpr;
				t_Byte ec5;
				t_Byte ec6;
				t_Byte ec7;
				t_Byte ipc;
				t_Byte ldpj;
				t_Byte nogo;
				t_Byte rcf;

				list_text (2, ";   ");

				if (reference_vsn != 21)
				{
					ec7 = (df1 >> 7) & 0x01;
					list_text (2, "ec7=" M_FMT_BYTE "; ", ec7);

					ec6 = (df1 >> 6) & 0x01;
					list_text (2, "ec6=" M_FMT_BYTE "; ", ec6);

					ec5 = (df1 >> 5) & 0x01;
					list_text (2, "ec5=" M_FMT_BYTE "; ", ec5);
				}
				else
				{
					ipc = (df1 >> 5) & 0x01;
					list_text (2, "ipc=" M_FMT_BYTE "; ", ipc);
				}

				nogo = (df1 >> 4) & 0x01;
				list_text (2, "nogo=" M_FMT_BYTE "; ", nogo);

				cpr = (df1 >> 3) & 0x01;
				list_text (2, "cpr=" M_FMT_BYTE "; ", cpr);

				ldpj = (df1 >> 2) & 0x01;
				list_text (2, "ldpj=" M_FMT_BYTE "; ", ldpj);

				rcf = (df1 >> 1) & 0x01;
				list_text (2, "rcf=" M_FMT_BYTE, rcf);

				list_text (2, "\n");
			}
		}
	}
	else if (reference_vsn == 24)
	{
		for (ix = 0; ix < length; ix ++)
		{
			df1 = buffer[ix];

			if (ix == 0)
			{
				t_Byte arc;
				t_Byte atp;
				t_Byte rab;
				t_Byte rc;

				list_text (2, ";   ");

				atp = (df1 >> 5) & 0x07;
				list_text (2, "atp=" M_FMT_BYTE, atp);
				Assert (atp <= 7, "Domain violation");
				list_text (2, " (%s)", atp_texts[atp]);
				list_text (2, "; ");

				arc = (df1 >> 3) & 0x03;
				list_text (2, "arc=" M_FMT_BYTE, arc);
				Assert (arc <= 3, "Domain violation");
				list_text (2, " (%s)", arc_texts[arc]);
				list_text (2, "; ", arc);

				rc = (df1 >> 2) & 0x01;
				list_text (2, "rc=" M_FMT_BYTE "; ", rc);

				rab = (df1 >> 1) & 0x01;
				list_text (2, "rab=" M_FMT_BYTE, rab);

				list_text (2, "\n");
			}
			else if (ix == 1)
			{
				t_Byte cl;
				t_Byte dcr;
				t_Byte gbs;
				t_Byte saa;
				t_Byte sim;
				t_Byte tst;

				list_text (2, ";   ");

				dcr = (df1 >> 7) & 0x01;
				list_text (2, "dcr=" M_FMT_BYTE "; ", dcr);

				gbs = (df1 >> 6) & 0x01;
				list_text (2, "gbs=" M_FMT_BYTE "; ", gbs);

				sim = (df1 >> 5) & 0x01;
				list_text (2, "sim=" M_FMT_BYTE "; ", sim);

				tst = (df1 >> 4) & 0x01;
				list_text (2, "tst=" M_FMT_BYTE "; ", tst);

				saa = (df1 >> 3) & 0x01;
				list_text (2, "saa=" M_FMT_BYTE "; ", saa);

				cl = (df1 >> 1) & 0x03;
				list_text (2, "cl=" M_FMT_BYTE, cl);
				Assert (cl <= 3, "Domain violation");
				list_text (2, " (%s)", cl_texts[cl]);

				list_text (2, "\n");
			}
			else if (ix == 2)
			{
				t_Byte cpr;
				t_Byte ipc;
				t_Byte ldpj;
				t_Byte llc;
				t_Byte nogo;
				t_Byte rcf;

				list_text (2, ";   ");

				llc = (df1 >> 6) & 0x01;
				list_text (2, "llc=" M_FMT_BYTE "; ", llc);

				ipc = (df1 >> 5) & 0x01;
				list_text (2, "ipc=" M_FMT_BYTE "; ", ipc);

				nogo = (df1 >> 4) & 0x01;
				list_text (2, "nogo=" M_FMT_BYTE "; ", nogo);

				cpr = (df1 >> 3) & 0x01;
				list_text (2, "cpr=" M_FMT_BYTE "; ", cpr);

				ldpj = (df1 >> 2) & 0x01;
				list_text (2, "ldpj=" M_FMT_BYTE "; ", ldpj);

				rcf = (df1 >> 1) & 0x01;
				list_text (2, "rcf=" M_FMT_BYTE, rcf);

				list_text (2, "\n");
			}
		}
	}
#endif /* LISTER */

                   /* Store this information: */
	if (reference_vsn == 1 || reference_vsn == 2)
	{
		/* tba ... */
	}
	else if (reference_vsn == 9 || reference_vsn == 12)
	{
		t_Byte arc;
		t_Byte atp;

		adsb.differential_correction = M_TRES (df1 & 0x80);

		if (df1 & 0x40)
		{
			adsb.ground_bit_set = TRUE;
		}

		adsb.detection_type.simulated = M_TRES (df1 & 0x20);

		adsb.detection_type.test_target = M_TRES (df1 & 0x10);

		adsb.detection_type.from_fixed_field_transponder =
			M_TRES (df1 & 0x08);

		adsb.selected_altitude_available = M_TRES (df1 & 0x04);

		adsb.special_position_indication = M_TRES (df1 & 0x02);

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

		arc = (df2 >> 3) & 0x03;

		adsb.altitude_reporting_capability.present = TRUE;
		if (arc == 0)
		{
			adsb.altitude_reporting_capability.value = 2;
		}
		else if (arc == 1)
		{
			adsb.altitude_reporting_capability.value = 0;
		}
		else if (arc == 2)
		{
			adsb.altitude_reporting_capability.value = 1;
		}
	}
	else if (reference_vsn == 14 || reference_vsn == 15 ||
             reference_vsn == 21)
	{
		for (ix = 0; ix < length; ix ++)
		{
			df1 = buffer[ix];

			if (ix == 0)
			{
				t_Byte arc;
				t_Byte atp;
				t_Byte rab;
				t_Byte rc;

				atp = (df1 >> 5) & 0x07;
				arc = (df1 >> 3) & 0x03;
				rc = (df1 >> 2) & 0x01;
				rab = (df1 >> 1) & 0x01;
		
				adsb.target_address_type.present = TRUE;
				switch (atp)
				{
				case 0:
					adsb.target_address_type.value =
						e_target_address_icao_24_bit;
					break;
				case 1:
					adsb.target_address_type.value =
						e_target_address_non_unique;
					break;
				case 2:
					adsb.target_address_type.value =
						e_target_address_surface_vehicle;
					break;
				case 3:
					adsb.target_address_type.value =
						e_target_address_anonymous;
					break;
				default:
					adsb.target_address_type.value =
						e_target_address_undefined;
					break;
				}

				adsb.altitude_reporting_capability.present = TRUE;
				adsb.altitude_reporting_capability.value = arc;

				adsb.range_check = M_TRES (rc);

				adsb.detection_type.from_fixed_field_transponder =
					M_TRES (rab);
			}
			else if (ix == 1)
			{
				t_Byte cl;
				t_Byte dcr;
				t_Byte gbs;
				t_Byte saa;
				t_Byte sim;
				t_Byte tst;

				dcr = (df1 >> 7) & 0x01;
				gbs = (df1 >> 6) & 0x01;
				sim = (df1 >> 5) & 0x01;
				tst = (df1 >> 4) & 0x01;
				saa = (df1 >> 3) & 0x01;
				cl = (df1 >> 1) & 0x03;

				adsb.differential_correction = M_TRES (dcr);

				if (gbs)
				{
					adsb.ground_bit_set = TRUE;
				}

				adsb.detection_type.simulated = M_TRES (sim);

				adsb.detection_type.test_target = M_TRES (tst);

				adsb.selected_altitude_available = M_TRES (saa);

				adsb.confidence_level.present = TRUE;
				adsb.confidence_level.value = cl;
			}
			else if (ix == 2)
			{
				t_Byte cpr;
#if CCWARN
				t_Byte ec5;
				t_Byte ec6;
				t_Byte ec7;
				t_Byte ipc;
#endif /* CCWARN */
				t_Byte ldpj;
				t_Byte nogo;
				t_Byte rcf;

#if CCWARN
				ec7 = ec6 = ec5 = ipc = 0;
				if (reference_vsn != 21)
				{
					ec7 = (df1 >> 7) & 0x01;
					ec6 = (df1 >> 6) & 0x01;
					ec5 = (df1 >> 5) & 0x01;
				}
				else
				{
					ipc = (df1 >> 5) & 0x01;
				}
#endif /* CCWARN */
				nogo = (df1 >> 4) & 0x01;
				cpr = (df1 >> 3) & 0x01;
				ldpj = (df1 >> 2) & 0x01;
				rcf = (df1 >> 1) & 0x01;

				/* tba ... ec7 */
				/* tba ... ec6 */
				/* tba ... ec5 */

				/* tba ... ipc */

				adsb.nogo_bit_status = M_TRES (nogo);

				adsb.compact_position_reporting = M_TRES (cpr);

				adsb.local_decoding_position_jump = M_TRES (ldpj);

				adsb.range_check_failed = M_TRES (rcf);
			}
		}
	}
	else if (reference_vsn == 24)
	{
		for (ix = 0; ix < length; ix ++)
		{
			df1 = buffer[ix];

			if (ix == 0)
			{
				t_Byte arc;
				t_Byte atp;
				t_Byte rab;
				t_Byte rc;

				atp = (df1 >> 5) & 0x07;
				arc = (df1 >> 3) & 0x03;
				rc = (df1 >> 2) & 0x01;
				rab = (df1 >> 1) & 0x01;
		
				adsb.target_address_type.present = TRUE;
				switch (atp)
				{
				case 0:
					adsb.target_address_type.value =
						e_target_address_icao_24_bit;
					break;
				case 1:
					adsb.target_address_type.value =
						e_target_address_non_unique;
					break;
				case 2:
					adsb.target_address_type.value =
						e_target_address_surface_vehicle;
					break;
				case 3:
					adsb.target_address_type.value =
						e_target_address_anonymous;
					break;
				default:
					adsb.target_address_type.value =
						e_target_address_undefined;
					break;
				}

				adsb.altitude_reporting_capability.present = TRUE;
				adsb.altitude_reporting_capability.value = arc;

				adsb.range_check = M_TRES (rc);

				adsb.detection_type.from_fixed_field_transponder =
					M_TRES (rab);
			}
			else if (ix == 1)
			{
				t_Byte cl;
				t_Byte dcr;
				t_Byte gbs;
				t_Byte saa;
				t_Byte sim;
				t_Byte tst;

				dcr = (df1 >> 7) & 0x01;
				gbs = (df1 >> 6) & 0x01;
				sim = (df1 >> 5) & 0x01;
				tst = (df1 >> 4) & 0x01;
				saa = (df1 >> 3) & 0x01;
				cl = (df1 >> 1) & 0x03;

				adsb.differential_correction = M_TRES (dcr);

				if (gbs)
				{
					adsb.ground_bit_set = TRUE;
				}

				adsb.detection_type.simulated = M_TRES (sim);

				adsb.detection_type.test_target = M_TRES (tst);

				adsb.selected_altitude_available = M_TRES (saa);

				adsb.confidence_level.present = TRUE;
				adsb.confidence_level.value = cl;
			}
			else if (ix == 2)
			{
				t_Byte cpr;
				t_Byte ipc;
				t_Byte ldpj;
				t_Byte llc;
				t_Byte nogo;
				t_Byte rcf;

				llc = (df1 >> 6) & 0x01;
				ipc = (df1 >> 5) & 0x01;
				nogo = (df1 >> 4) & 0x01;
				cpr = (df1 >> 3) & 0x01;
				ldpj = (df1 >> 2) & 0x01;
				rcf = (df1 >> 1) & 0x01;

				adsb.list_lookup_check_failed = M_TRES (llc);

				adsb.independent_position_check_failed = M_TRES (ipc);

				adsb.nogo_bit_status = M_TRES (nogo);

				adsb.compact_position_reporting = M_TRES (cpr);

				adsb.local_decoding_position_jump = M_TRES (ldpj);

				adsb.range_check_failed = M_TRES (rcf);
			}
		}
	}


                   /* Set the return code: */
	ret = RC_OKAY;

                   /* We are done: */
	return ret;
}

/*----------------------------------------------------------------------------*/
/* proc_i021_070   -- Process I021/070 data item                              */
/*----------------------------------------------------------------------------*/

 static
 t_Retc proc_i021_070 (t_Ui16 length, t_Byte *buffer)
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

                   /* Check reference version: */
	Assert (reference_vsn == 14 || reference_vsn == 15 ||
            reference_vsn == 21 || reference_vsn == 24,
            "Data item not defined");

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
	adsb.mode_3a_info.code = m3a;
	adsb.mode_3a_info.present = TRUE;

                   /* Set the return code: */
	ret = RC_OKAY;

                   /* We are done: */
	return ret;
}

/*----------------------------------------------------------------------------*/
/* proc_i021_071   -- Process I021/071 data item                              */
/*----------------------------------------------------------------------------*/

 static
 t_Retc proc_i021_071 (t_Ui16 length, t_Byte *buffer)
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

                   /* Check reference version: */
	Assert (reference_vsn == 14 || reference_vsn == 15 ||
            reference_vsn == 21 || reference_vsn == 24,
            "Data item not defined");

                   /* Extract octets: */
	df1 = buffer[0];
	df2 = buffer[1];
	df3 = buffer[2];

                   /* Extract information: */
	tod = make_ui32 (0x00, df1, df2, df3);

                   /* Convert ot seconds: */
	tod_in_secs = (1.0 / 128.0) * tod;

#if LISTER
                   /* List data field: */
	list_text (2, ";  Time of Applicability for Position:");
	list_text (2, " " M_FMT_ATOD " (" M_FMT_UI32 "; %s UTC)",
                  tod, tod, utc_text (tod_in_secs));
	list_text (2, "\n");
#endif /* LISTER */

                   /* Store this information: */
	adsb.toa_position.present = TRUE;
	adsb.toa_position.value = tod_in_secs;

                   /* Set the return code: */
	ret = RC_OKAY;

                   /* We are done: */
	return ret;
}

/*----------------------------------------------------------------------------*/
/* proc_i021_072   -- Process I021/072 data item                              */
/*----------------------------------------------------------------------------*/

 static
 t_Retc proc_i021_072 (t_Ui16 length, t_Byte *buffer)
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

                   /* Check reference version: */
	Assert (reference_vsn == 14 || reference_vsn == 15 ||
            reference_vsn == 21 || reference_vsn == 24,
            "Data item not defined");

                   /* Extract octets: */
	df1 = buffer[0];
	df2 = buffer[1];
	df3 = buffer[2];

                   /* Extract information: */
	tod = make_ui32 (0x00, df1, df2, df3);

                   /* Convert ot seconds: */
	tod_in_secs = (1.0 / 128.0) * tod;

#if LISTER
                   /* List data field: */
	list_text (2, ";  Time of Applicability for Velocity:");
	list_text (2, " " M_FMT_ATOD " (" M_FMT_UI32 "; %s UTC)",
                  tod, tod, utc_text (tod_in_secs));
	list_text (2, "\n");
#endif /* LISTER */

                   /* Store this information: */
	adsb.toa_velocity.present = TRUE;
	adsb.toa_velocity.value = tod_in_secs;

                   /* Set the return code: */
	ret = RC_OKAY;

                   /* We are done: */
	return ret;
}

/*----------------------------------------------------------------------------*/
/* proc_i021_073   -- Process I021/073 data item                              */
/*----------------------------------------------------------------------------*/

 static
 t_Retc proc_i021_073 (t_Ui16 length, t_Byte *buffer)
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

                   /* Check reference version: */
	Assert (reference_vsn == 14 || reference_vsn == 15 ||
            reference_vsn == 21 || reference_vsn == 24,
            "Data item not defined");

                   /* Extract octets: */
	df1 = buffer[0];
	df2 = buffer[1];
	df3 = buffer[2];

                   /* Extract information: */
	tod = make_ui32 (0x00, df1, df2, df3);

                   /* Convert ot seconds: */
	tod_in_secs = (1.0 / 128.0) * tod;

#if LISTER
                   /* List data field: */
	list_text (2, ";  Time of Message Reception for Position:");
	list_text (2, " " M_FMT_ATOD " (" M_FMT_UI32 "; %s UTC)",
                  tod, tod, utc_text (tod_in_secs));
	list_text (2, "\n");
#endif /* LISTER */

                   /* Store this information: */
	adsb.tor_position.present = TRUE;
	adsb.tor_position.value = tod_in_secs;

                   /* Set the return code: */
	ret = RC_OKAY;

                   /* We are done: */
	return ret;
}

/*----------------------------------------------------------------------------*/
/* proc_i021_074   -- Process I021/074 data item                              */
/*----------------------------------------------------------------------------*/

 static
 t_Retc proc_i021_074 (t_Ui16 length, t_Byte *buffer)
{
#if LISTER
	t_Byte df1;    /* Data field octet 1 */
	t_Byte df2;    /* Data field octet 2 */
	t_Byte df3;    /* Data field octet 3 */
	t_Byte df4;    /* Data field octet 4 */
	t_Real fpt;    /* Fractional part of time; seconds */
	t_Byte fsi;    /* Full second indication */
#endif /* LISTER */
	t_Retc ret;    /* Return code */
#if LISTER
	t_Ui32 u32;    /* Auxiliary */
#endif /* LISTER */

                   /* Preset the return code: */
	ret = RC_FAIL;

                   /* Check parameters: */
	Assert (length == 4, "Invalid parameter");
	Assert (buffer != NULL, "Invalid parameter");

                   /* Check reference version: */
	Assert (reference_vsn == 14 || reference_vsn == 15 ||
            reference_vsn == 21 || reference_vsn == 24,
            "Data item not defined");

#if LISTER
                   /* Extract octets: */
	df1 = buffer[0];
	df2 = buffer[1];
	df3 = buffer[2];
	df4 = buffer[3];

                   /* Extract information: */
	fsi = (df1 >> 6) & 0x03;
	u32 = make_ui32 (df1 & 0x3f, df2, df3, df4);
	fpt = (1.0 / M_TWO_POWER_30) * u32;

                   /* List data field: */
	list_text (2, ";  Time of Message Reception for Position - "
                  "High Precision: ");
	list_text (2, "fsi=" M_FMT_BYTE "; ", fsi);
	list_text (2, "fpt=%.9f (sec)", fpt);
	list_text (2, "\n");
#endif /* LISTER */

                   /* Store this information: */
	/* tba ... */

                   /* Set the return code: */
	ret = RC_OKAY;

                   /* We are done: */
	return ret;
}

/*----------------------------------------------------------------------------*/
/* proc_i021_075   -- Process I021/075 data item                              */
/*----------------------------------------------------------------------------*/

 static
 t_Retc proc_i021_075 (t_Ui16 length, t_Byte *buffer)
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

                   /* Check reference version: */
	Assert (reference_vsn == 14 || reference_vsn == 15 ||
            reference_vsn == 21 || reference_vsn == 24,
            "Data item not defined");

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
	list_text (2, ";  Time of Message Reception for Velocity:");
	list_text (2, " " M_FMT_ATOD " (" M_FMT_UI32 "; %s UTC)",
                  tod, tod, utc_text (tod_in_secs));
	list_text (2, "\n");
#endif /* LISTER */

                   /* Store this information: */
	adsb.tor_velocity.present = TRUE;
	adsb.tor_velocity.value = tod_in_secs;

                   /* Set the return code: */
	ret = RC_OKAY;

                   /* We are done: */
	return ret;
}

/*----------------------------------------------------------------------------*/
/* proc_i021_076   -- Process I021/076 data item                              */
/*----------------------------------------------------------------------------*/

 static
 t_Retc proc_i021_076 (t_Ui16 length, t_Byte *buffer)
{
#if LISTER
	t_Byte df1;    /* Data field octet 1 */
	t_Byte df2;    /* Data field octet 2 */
	t_Byte df3;    /* Data field octet 3 */
	t_Byte df4;    /* Data field octet 4 */
	t_Real fpt;    /* Fractional part of time; seconds */
	t_Byte fsi;    /* Full second indication */
#endif /* LISTER */
	t_Retc ret;    /* Return code */
#if LISTER
	t_Ui32 u32;    /* Auxiliary */
#endif /* LISTER */

                   /* Preset the return code: */
	ret = RC_FAIL;

                   /* Check parameters: */
	Assert (length == 4, "Invalid parameter");
	Assert (buffer != NULL, "Invalid parameter");

                   /* Check reference version: */
	Assert (reference_vsn == 14 || reference_vsn == 15 ||
            reference_vsn == 21 || reference_vsn == 24,
            "Data item not defined");

#if LISTER
                   /* Extract octets: */
	df1 = buffer[0];
	df2 = buffer[1];
	df3 = buffer[2];
	df4 = buffer[3];

                   /* Extract information: */
	fsi = (df1 >> 6) & 0x03;
	u32 = make_ui32 (df1 & 0x3f, df2, df3, df4);
	fpt = (1.0 / M_TWO_POWER_30) * u32;

                   /* List data field: */
	list_text (2, ";  Time of Message Reception for Velocity - "
                  "High Precision:");
	list_text (2, "fsi=" M_FMT_BYTE "; ", fsi);
	list_text (2, "fpt=%.9f (sec)", fpt);
	list_text (2, "\n");
#endif /* LISTER */

                   /* Store this information: */
	/* tba ... */

                   /* Set the return code: */
	ret = RC_OKAY;

                   /* We are done: */
	return ret;
}

/*----------------------------------------------------------------------------*/
/* proc_i021_077   -- Process I021/077 data item                              */
/*----------------------------------------------------------------------------*/

 static
 t_Retc proc_i021_077 (t_Ui16 length, t_Byte *buffer)
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

                   /* Check reference version: */
	Assert (reference_vsn == 14 || reference_vsn == 15 ||
            reference_vsn == 21 || reference_vsn == 24,
            "Data item not defined");

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
	if (reference_vsn == 14)
	{
		list_text (2, ";  Time of Message Transmission:");
	}
	else if (reference_vsn == 15 ||
             reference_vsn == 21 || reference_vsn == 24)
	{
		list_text (2, ";  Time of ASTERIX Report Transmission:");
	}
	list_text (2, " " M_FMT_ATOD " (" M_FMT_UI32 "; %s UTC)",
                  tod, tod, utc_text (tod_in_secs));
	list_text (2, "\n");
#endif /* LISTER */

                   /* Store this information: */
	adsb.time_of_report.present = TRUE;
	adsb.time_of_report.value = tod_in_secs;

                   /* Set the return code: */
	ret = RC_OKAY;

                   /* We are done: */
	return ret;
}

/*----------------------------------------------------------------------------*/
/* proc_i021_080   -- Process I021/080 data item                              */
/*----------------------------------------------------------------------------*/

 static
 t_Retc proc_i021_080 (t_Ui16 length, t_Byte *buffer)
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
	if (reference_vsn != 14 && reference_vsn != 15 &&
        reference_vsn != 21 && reference_vsn != 24)
	{
		list_text (2, ";  Aircraft Address:");
	}
	else
	{
		list_text (2, ";  Target Address:");
	}
	list_text (2, " " M_FMT_ADDR " (" M_FMT_UI32 ")", ta, ta);
	list_text (2, "\n");
#endif /* LISTER */

                   /* Store this information: */
	adsb.target_address.present = TRUE;
	adsb.target_address.value = ta;

                   /* Set the return code: */
	ret = RC_OKAY;

                   /* We are done: */
	return ret;
}

/*----------------------------------------------------------------------------*/
/* proc_i021_090   -- Process I021/090 data item                              */
/*----------------------------------------------------------------------------*/

 static
 t_Retc proc_i021_090 (t_Ui16 length, t_Byte *buffer)
{
#if LISTER
	t_Byte ac;     /* ACAS status */
	t_Byte dc;     /* Differential correction status */
#endif /* LISTER */
	t_Byte df1;    /* Data field octet 1 */
	t_Byte df2;    /* Data field octet 2 */
	t_Byte df3;    /* Data field octet 3 */
	t_Byte df4;    /* Data field octet 4 */
	t_Byte fom;    /* Figure of merit */
	t_Byte gva;    /* Geometric altitude accuracy */
	t_Byte mn;     /* Multiple navigational aids status */
	t_Byte nacp;   /* NACp */
	t_Byte nic_baro;
                   /* NICbaro */
	t_Byte nucp;   /* NUCp or NIC */
	t_Byte nucr;   /* NUCr or NAVc */
	t_Byte pa;     /* Position accuracy */
	t_Byte pic;    /* Position integrity category */
	t_Retc ret;    /* Return code */
	t_Byte sda;    /* Horizontal position system design assurance level */
	t_Byte sil;    /* SIL */
	t_Byte sil2;   /* SIL supplement */

                   /* Preset the return code: */
	ret = RC_FAIL;

                   /* Check parameters: */
	if (reference_vsn == 1 || reference_vsn == 2)
	{
		Assert (length == 1, "Invalid parameter");
	}
	else if (reference_vsn == 9 || reference_vsn == 12)
	{
		Assert (length == 2, "Invalid parameter");
	}
	else if (reference_vsn == 14 || reference_vsn == 15)
	{
		Assert (1 <= length && length <= 2, "Invalid parameter");
	}
	else if (reference_vsn == 21 || reference_vsn == 24)
	{
		Assert (1 <= length && length <= 4, "Invalid parameter");
	}
	Assert (buffer != NULL, "Invalid parameter");

                   /* Preset local data: */
#if LISTER
	ac = dc = 0;
#endif /* LISTER */
	fom = gva = mn = 0;
	nacp = nic_baro = nucp = nucr = pa = pic = sda = sil = sil2 = 0;

                   /* Extract octets: */
	df1 = buffer[0];
	df2 = df3 = df4 = 0;
	if (reference_vsn == 9 || reference_vsn == 12)
	{
		df2 = buffer[1];
	}
	else if (reference_vsn == 14 && length == 2)
	{
		df2 = buffer[1];
	}
	else if (reference_vsn == 15 && length == 2)
	{
		df2 = buffer[1];
	}
	else if (reference_vsn == 21 || reference_vsn == 24)
	{
		if (length == 2)
		{
			df2 = buffer[1];
		}
		else if (length == 3)
		{
			df2 = buffer[1];
			df3 = buffer[2];
		}
		else if (length == 4)
		{
			df2 = buffer[1];
			df3 = buffer[2];
			df4 = buffer[3];
		}
	}

                   /* Extract information: */
	if (reference_vsn == 1 || reference_vsn == 2)
	{
		fom = df1;
#if LISTER
		ac = (fom >> 7) & 0x01;
#endif /* LISTER */
		mn = (fom >> 6) & 0x01;
#if LISTER
		dc = (fom >> 5) & 0x01;
#endif /* LISTER */
		pa = (fom >> 1) & 0x0f;
	}
	else if (reference_vsn == 9 || reference_vsn == 12)
	{
#if LISTER
		ac = (df1 >> 6) & 0x03;
#endif /* LISTER */
		mn = (df1 >> 4) & 0x03;
#if LISTER
		dc = (df1 >> 2) & 0x03;
#endif /* LISTER */
		pa = df2 & 0x0f;
	}
	else if (reference_vsn == 14 || reference_vsn == 15)
	{
		nucr = (df1 >> 5) & 0x07;
		nucp = (df1 >> 1) & 0x0f;

		if (length >= 2)
		{
			nic_baro = (df2 >> 7) & 0x01;
			sil = (df2 >> 5) & 0x03;
			nacp = (df2 >> 1) & 0x0f;
		}
	}
	else if (reference_vsn == 21 || reference_vsn == 24)
	{
		nucr = (df1 >> 5) & 0x07;
		nucp = (df1 >> 1) & 0x0f;

		if (length >= 2)
		{
			nic_baro = (df2 >> 7) & 0x01;
			sil = (df2 >> 5) & 0x03;
			nacp = (df2 >> 1) & 0x0f;
		}

		if (length >= 3)
		{
			sil2 = (df3 >> 5) & 0x01;
			sda = (df3 >> 3) & 0x03;
			gva = (df3 >> 1) & 0x03;
		}

		if (length >= 4)
		{
			pic = (df4 >> 4) & 0x0f;
		}
	}

#if LISTER
                   /* List data field: */
	if (reference_vsn == 1 || reference_vsn == 2 ||
        reference_vsn == 9 || reference_vsn == 12)
	{
		list_text (2, ";  Figure of Merit:");
		list_text (2, " ac=" M_FMT_BYTE ";", ac);
		list_text (2, " mn=" M_FMT_BYTE ";", mn);
		list_text (2, " dc=" M_FMT_BYTE ";", dc);
		list_text (2, " pa=" M_FMT_BYTE, pa);
		list_text (2, "\n");
	}
	else if (reference_vsn == 14 || reference_vsn == 15)
	{
		list_text (2, ";  Quality Indicators:");
		list_text (2, " NUCr_or_NACv=" M_FMT_BYTE ";", nucr);
		list_text (2, " NUCp_or_NIC=" M_FMT_BYTE, nucp);
		if (length >= 2)
		{
			list_text (2, ";");
			list_text (2, " NICbaro=" M_FMT_BYTE ";", nic_baro);
			list_text (2, " SIL=" M_FMT_BYTE ";", sil);
			list_text (2, " NACp=" M_FMT_BYTE, nacp);
		}
		list_text (2, "\n");
	}
	else if (reference_vsn == 21 || reference_vsn == 24)
	{
		list_text (2, ";  Quality Indicators:");
		list_text (2, "\n");

		list_text (2, ";   NUCr_or_NACv=" M_FMT_BYTE ";", nucr);
		list_text (2, " NUCp_or_NIC=" M_FMT_BYTE, nucp);
		list_text (2, "\n");

		if (length >= 2)
		{
			list_text (2, ";  ");
			list_text (2, " NICbaro=" M_FMT_BYTE ";", nic_baro);
			list_text (2, " SIL=" M_FMT_BYTE ";", sil);
			list_text (2, " NACp=" M_FMT_BYTE, nacp);
			list_text (2, "\n");
		}

		if (length >= 3)
		{
			list_text (2, ";  ");
			list_text (2, " SILsupp=" M_FMT_BYTE ";", sil2);
			list_text (2, " SDA=" M_FMT_BYTE ";", sda);
			list_text (2, " GVA=" M_FMT_BYTE, gva);
			list_text (2, "\n");
		}

		if (length >= 4)
		{
			list_text (2, ";  ");
			list_text (2, " PIC=" M_FMT_BYTE, pic);
			list_text (2, "\n");
		}
	}
#endif /* LISTER */

                   /* Store this information: */
	if (reference_vsn != 14 && reference_vsn != 15 &&
        reference_vsn != 21 && reference_vsn != 24)
	{
		if (pa > 0)
		{
			adsb.figure_of_merit.present = TRUE;
			adsb.figure_of_merit.value = pa;
		}
	}
	else
	{
		adsb.quality_indicators.present = TRUE;
		adsb.quality_indicators.value_nucr_or_nacv = nucr;
		adsb.quality_indicators.value_nucp_or_nic = nucp;

		if (length >= 2)
		{
			adsb.quality_indicators.nic_baro_present = TRUE;
			adsb.quality_indicators.value_nic_baro = nic_baro;

			adsb.quality_indicators.sil_present = TRUE;
			adsb.quality_indicators.value_sil = sil;

			adsb.quality_indicators.nacp_present = TRUE;
			adsb.quality_indicators.value_nacp = nacp;
		}

		/* tba ... */
	}

                   /* Set the return code: */
	ret = RC_OKAY;

                   /* We are done: */
	return ret;
}

/*----------------------------------------------------------------------------*/
/* proc_i021_095   -- Process I021/095 data item                              */
/*----------------------------------------------------------------------------*/

 static
 t_Retc proc_i021_095 (t_Ui16 length, t_Byte *buffer)
{
	t_Byte df1;    /* Data field octet 1 */
	t_Retc ret;    /* Return code */
	t_Byte va;     /* Velocity accuracy */

                   /* Preset the return code: */
	ret = RC_FAIL;

                   /* Check parameters: */
	Assert (length == 1, "Invalid parameter");
	Assert (buffer != NULL, "Invalid parameter");

                   /* Check reference version: */
	Assert (reference_vsn != 14 && reference_vsn != 15 &&
            reference_vsn != 21 && reference_vsn != 24,
            "Data item not defined");

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

                   /* We are done: */
	return ret;
}

/*----------------------------------------------------------------------------*/
/* proc_i021_100   -- Process I021/100 data item                              */
/*----------------------------------------------------------------------------*/

 static
 t_Retc proc_i021_100 (t_Ui16 length, t_Byte *buffer)
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

                   /* Check reference version: */
	Assert (reference_vsn != 14 && reference_vsn != 15 &&
            reference_vsn != 21 && reference_vsn != 24,
            "Data item not defined");

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

                   /* We are done: */
	return ret;
}

/*----------------------------------------------------------------------------*/
/* proc_i021_110   -- Process I021/110 data item                              */
/*----------------------------------------------------------------------------*/

 static
 t_Retc proc_i021_110 (t_Ui16 length, t_Byte *buffer)
{
	t_Byte df1;    /* Data field octet 1 */
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
	Assert (reference_vsn == 1 || reference_vsn == 2,
            "Wrong function call for processing I021/110");

                   /* Extract repetition factor: */
	df1 = buffer[0];
	rep = df1;

                   /* Check against buffer length: */
	Assert (1 + rep * 11 == length, "Invalid buffer length (I021/110)");

#if LISTER
                   /* List data field: */
	list_text (2, ";  Projected Profile:");
	list_text (2, "\n");
	for (ix = 0; ix < rep; ix ++)
	{
		t_Si16 alt;
                   /* Altitude; 10 feet */
		t_Byte df2;
                   /* Data field octet 2 */
		t_Byte df3;
                   /* Data field octet 3 */
		t_Byte df4;
                   /* Data field octet 4 */
		t_Byte df5;
                   /* Data field octet 5 */
		t_Byte df6;
                   /* Data field octet 6 */
		t_Byte df7;
                   /* Data field octet 7 */
		t_Byte df8;
                   /* Data field octet 8 */
		t_Byte df9;
                   /* Data field octet 9 */
		t_Byte df10;
                   /* Data field octet 10 */
		t_Byte df11;
                   /* Data field octet 11 */
		t_Ui32 lat;
                   /* Latitude; 180/2**23 degrees */
		t_Ui32 lon;
                   /* Longitude; 180/2**23 degrees */
		t_Byte nc; /* TCP compliance */
		t_Byte tca;
                   /* TCP number available */
		t_Byte tcp;
                   /* TCP number */
		t_Byte tta;
                   /* TTG available */
		t_Ui16 ttg;
                   /* Time to go; 0.125 minutes */

		df1 = buffer[1 + 11 * ix];
		df2 = buffer[1 + 11 * ix + 1];
		df3 = buffer[1 + 11 * ix + 2];
		df4 = buffer[1 + 11 * ix + 3];
		df5 = buffer[1 + 11 * ix + 4];
		df6 = buffer[1 + 11 * ix + 5];
		df7 = buffer[1 + 11 * ix + 6];
		df8 = buffer[1 + 11 * ix + 7];
		df9 = buffer[1 + 11 * ix + 8];
		df10 = buffer[1 + 11 * ix + 9];
		df11 = buffer[1 + 11 * ix + 10];

		alt = make_ui16 (df1, df2);
		lat = make_ui32 (0x00, df3, df4, df5);
		lon = make_ui32 (0x00, df6, df7, df8);

		tta = (df9 >> 7) & 0x01;
		ttg = make_ui16 (df9 & 0x7f, df10);

		tca = (df11 >> 7) & 0x01;
		nc = (df11 >> 6) & 0x01;
		tcp = df11 & 0x3f;

		list_text (2, ";   %d:", 1 + ix);
		list_text (2, " alt=" M_FMT_UI16 " ft;", 10 * alt);
		list_text (2, " lat=%10.6f;", (180.0 / M_TWO_POWER_23) * lat);
		list_text (2, " lon=%11.6f;", (180.0 / M_TWO_POWER_23) * lon);
		if (tta)
		{
			list_text (2, " ttg=%.3f min;", 0.125 * ttg);
		}
		if (tca)
		{
			list_text (2, " tcp=" M_FMT_BYTE "; nc=" M_FMT_BYTE, tcp, nc);
		}
		list_text (2, "\n");
	}
#endif /* LISTER */

                   /* Store this information: */
	/* tba ... */

                   /* Set the return code: */
	ret = RC_OKAY;

                   /* We are done: */
	return ret;
}

/*----------------------------------------------------------------------------*/
/* proc_i021_110   -- Process I021/110 data item                              */
/*----------------------------------------------------------------------------*/

 static
 t_Retc proc_i021_110 (t_Ui16 length, t_Byte *buffer, t_Ui16 *pos_ptr)
{
	t_Byte df1;    /* Data field octet 1 */
	t_Byte len;    /* Length of data field */
	int inx;       /* Auxiliary */
#if LISTER
	int ix;        /* Auxiliary */
	int j;         /* Auxiliary */
#endif /* LISTER */
	t_Ui16 pos;    /* Position within buffer */
	t_Byte rep;    /* Repetition factor */
	t_Retc ret;    /* Return code */
	t_Byte sf1_present;
                   /* Subfield 1 present (TIS) */
	t_Byte sf2_present;
                   /* Subfield 2 present (TID) */
#if LISTER
	t_Byte tid_buffer[15];
                   /* Buffer for trajectory intent data */
#endif /* LISTER */
	t_Byte tis;    /* Trajectory intent status */

                   /* Preset the return code: */
	ret = RC_FAIL;

                   /* Check parameters: */
	Assert (length > 0, "Invalid parameter");
	Assert (buffer != NULL, "Invalid parameter");
	Assert (pos_ptr != NULL, "Invalid parameter");

                   /* Check reference version: */
	Assert (reference_vsn == 9 || reference_vsn == 12 ||
            reference_vsn == 14 || reference_vsn == 15 ||
            reference_vsn == 21 || reference_vsn == 24,
            "Wrong function call for processing I021/110");

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
			error_msg ("Invalid buffer length (I021/110)");
			goto done;
		}

		rep = buffer[pos + len];

		len += (1 + rep * 15);
	}

                   /* Check against buffer length: */
	if (pos + len > length)
	{
		error_msg ("Invalid buffer length (I021/110)");
		goto done;
	}

#if LISTER
                   /* List raw data field: */
	list_text (1, ";  I021/110: 0x");
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
		Assert (rep > 0, "Unexpected REP value");

		inx += (1 + rep * 15);
	}
	Assert (inx == len, "Invalid structure decoding");

#if LISTER
                   /* List data field: */
	list_text (2, ";  Trajectory Intent:");
	list_text (2, " tis=" M_FMT_BYTE ";", sf1_present);
	list_text (2, " tid=" M_FMT_BYTE, sf2_present);
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
		list_text (2, ";   Trajectory intent data:");
		list_text (2, "\n");

		for (ix = 0; ix < rep; ix ++)
		{
			t_Si16 alt;
			t_Si32 lat;
			t_Real latf;
			t_Si32 lon;
			t_Real lonf;
			t_Byte nc;
			t_Byte point_type;
			t_Byte tca;
			t_Byte tcp;
			t_Byte td;
			t_Byte toa;
			t_Ui32 tov;
			t_Byte tra;
			t_Ui16 ttr;

			memset (tid_buffer, 0, 15);
			memcpy (tid_buffer, &(buffer[pos + 1 + ix * 15]), 15);

			tca = (tid_buffer[0] >> 7) & 0x01;
			nc = (tid_buffer[0] >> 6) & 0x01;
			tcp = tid_buffer[0] & 0x3f;

			alt = make_si16 (tid_buffer[1], tid_buffer[2]);

			lat = make_si32 (0x00, tid_buffer[3], tid_buffer[4], tid_buffer[5]);
			if (tid_buffer[3] & 0x80)
			{
				lat |= 0xff000000;
			}

			latf = (180.0 / M_TWO_POWER_23) * lat;

			lon = make_si32 (0x00, tid_buffer[6], tid_buffer[7], tid_buffer[8]);
			if (tid_buffer[6] & 0x80)
			{
				lon |= 0xff000000;
			}

			lonf = (180.0 / M_TWO_POWER_23) * lon;

			point_type = (tid_buffer[9] >> 4) & 0x0f;
			td = (tid_buffer[9] >> 2) & 0x03;
			tra = (tid_buffer[9] >> 1) & 0x01;
			toa = tid_buffer[9] & 0x01;

			tov = make_ui32 (0x00, tid_buffer[10],
                             tid_buffer[11], tid_buffer[12]);

			ttr = make_ui16 (tid_buffer[13], tid_buffer[14]);

			list_text (2, ";    Trajectory intent point #%d:", 1 + ix);
			list_text (2, "\n");

			list_text (2, ";    ");
			list_text (2, " tca=" M_FMT_BYTE ";", tca);
			list_text (2, " nc=" M_FMT_BYTE ";", nc);
			list_text (2, " tcp=" M_FMT_BYTE, tcp);
			list_text (2, "\n");

			list_text (2, ";    ");
			list_text (2, " alt=" M_FMT_SI16 " (10 ft)", alt);
			list_text (2, "\n");

			list_text (2, ";    ");
			list_text (2, " lat=" M_FMT_SI32 " (%s)", lat, lat_text (latf));
			list_text (2, "\n");

			list_text (2, ";    ");
			list_text (2, " lon=" M_FMT_SI32 " (%s)", lon, lon_text (lonf));
			list_text (2, "\n");

			list_text (2, ";    ");
			list_text (2, " point_type=" M_FMT_BYTE ";", point_type);
			list_text (2, " td=" M_FMT_BYTE ";", td);
			list_text (2, " tra=" M_FMT_BYTE ";", tra);
			list_text (2, " toa=" M_FMT_BYTE, toa);
			list_text (2, "\n");

			list_text (2, ";    ");
			list_text (2, " tov=" M_FMT_UI32 " (sec)", tov);
			list_text (2, "\n");

			list_text (2, ";    ");
			list_text (2, " ttr=" M_FMT_UI16 " (0.01 NM)", ttr);
			list_text (2, "\n");
		}
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
/* proc_i021_130   -- Process I021/130 data item                              */
/*----------------------------------------------------------------------------*/

 static
 t_Retc proc_i021_130 (t_Ui16 length, t_Byte *buffer)
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
                   /* Beware: May list out-of-range values */
	list_text (2, ";  Position in WGS-84 Co-ordinates:");
	list_text (2, "\n");
	list_text (2, ";   lat=" M_FMT_SI32 " (%s)", lat, lat_text (latf));
	list_text (2, "; lon=" M_FMT_SI32 " (%s)", lon, lon_text (lonf));
	list_text (2, "\n");
#endif /* LISTER */

                   /* Store this information: */
	if (fabs (latf) <= 90.0 && fabs (lonf) <= 180.0)
	{
		adsb.wgs84_position.high_precision = FALSE;
		adsb.wgs84_position.present = TRUE;
		adsb.wgs84_position.value_lat = M_DEG2RAD * latf;
		adsb.wgs84_position.value_lon = M_DEG2RAD * lonf;
                   /* Values are in radians */
	}

                   /* Set the return code: */
	ret = RC_OKAY;

                   /* We are done: */
	return ret;
}

/*----------------------------------------------------------------------------*/
/* proc_i021_131   -- Process I021/131 data item                              */
/*----------------------------------------------------------------------------*/

 static
 t_Retc proc_i021_131 (t_Ui16 length, t_Byte *buffer)
{
	t_Byte df1;    /* Data field octet 1 */
	t_Byte df2;    /* Data field octet 2 */
	t_Byte df3;    /* Data field octet 3 */
	t_Byte df4;    /* Data field octet 4 */
	t_Byte df5;    /* Data field octet 5 */
	t_Byte df6;    /* Data field octet 6 */
	t_Byte df7;    /* Data field octet 7 */
	t_Byte df8;    /* Data field octet 8 */
	t_Si32 lat;    /* Latitude; 180/2**30 degrees */
	t_Real latf;   /* Latitude; degrees */
	t_Si32 lon;    /* Longitude; 180/2**30 degrees */
	t_Real lonf;   /* Longitude; degrees */
	t_Retc ret;    /* Return code */

                   /* Preset the return code: */
	ret = RC_FAIL;

                   /* Check parameters: */
	Assert (length == 8, "Invalid parameter");
	Assert (buffer != NULL, "Invalid parameter");

                   /* Check reference version: */
	Assert (reference_vsn == 14 || reference_vsn == 15 ||
            reference_vsn == 21 || reference_vsn == 24,
            "Data item not defined");

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
	latf = (180.0 / M_TWO_POWER_30) * lat;
	lonf = (180.0 / M_TWO_POWER_30) * lon;

#if LISTER
                   /* List data field: */
                   /* Beware: May list out-of-range values */
	list_text (2, ";  High-Resolution Position in WGS-84 Co-ordinates:");
	list_text (2, "\n");
	list_text (2, ";   lat=" M_FMT_SI32 " (%s)", lat, lat_text (latf));
	list_text (2, "; lon=" M_FMT_SI32 " (%s)", lon, lon_text (lonf));
	list_text (2, "\n");
#endif /* LISTER */

                   /* Store this information: */
	if (fabs (latf) <= 90.0 && fabs (lonf) <= 180.0)
	{
		adsb.wgs84_position.high_precision = TRUE;
		adsb.wgs84_position.present = TRUE;
		adsb.wgs84_position.value_lat = M_DEG2RAD * latf;
		adsb.wgs84_position.value_lon = M_DEG2RAD * lonf;
                   /* Values are in radians */
	}

                   /* Set the return code: */
	ret = RC_OKAY;

                   /* We are done: */
	return ret;
}

/*----------------------------------------------------------------------------*/
/* proc_i021_132   -- Process I021/132 data item                              */
/*----------------------------------------------------------------------------*/

 static
 t_Retc proc_i021_132 (t_Ui16 length, t_Byte *buffer)
{
	t_Byte df1;    /* Data field octet 1 */
	t_Si08 mam;    /* Message amplitude; dBm */
	t_Retc ret;    /* Return code */

                   /* Preset the return code: */
	ret = RC_FAIL;

                   /* Check parameters: */
	Assert (length == 1, "Invalid parameter");
	Assert (buffer != NULL, "Invalid parameter");

                   /* Check reference version: */
	Assert (reference_vsn == 14 || reference_vsn == 15 ||
            reference_vsn == 21 || reference_vsn == 24,
            "Data item not defined");

                   /* Extract octets: */
	df1 = buffer[0];

                   /* Extract information: */
	mam = (t_Si08) df1;

#if LISTER
                   /* List data field: */
	list_text (2, ";  Message Amplitude:");
	list_text (2, " mam=" M_FMT_SI08 " (dBm)", mam);
	list_text (2, "\n");
#endif /* LISTER */

                   /* Store this information: */
	adsb.message_amplitude.present = TRUE;
	adsb.message_amplitude.value = (t_Si16) mam;

                   /* Set the return code: */
	ret = RC_OKAY;

                   /* We are done: */
	return ret;
}

/*----------------------------------------------------------------------------*/
/* proc_i021_140   -- Process I021/140 data item                              */
/*----------------------------------------------------------------------------*/

 static
 t_Retc proc_i021_140 (t_Ui16 length, t_Byte *buffer)
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
	if (reference_vsn != 14 && reference_vsn != 15 &&
        reference_vsn != 21 && reference_vsn != 24)
	{
		list_text (2, ";  Altitude:");
	}
	else
	{
		list_text (2, ";  Geometric Height:");
	}
	list_text (2, " " M_FMT_SI16 " (%.2f ft)", alt, 6.25 * alt);
	list_text (2, "\n");
#endif /* LISTER */

                   /* Store this information: */
	adsb.geometric_altitude.present = TRUE;
	adsb.geometric_altitude.value = 6.25 * M_FT2MTR * alt;

                   /* Set the return code: */
	ret = RC_OKAY;

                   /* We are done: */
	return ret;
}

/*----------------------------------------------------------------------------*/
/* proc_i021_145   -- Process I021/145 data item                              */
/*----------------------------------------------------------------------------*/

 static
 t_Retc proc_i021_145 (t_Ui16 length, t_Byte *buffer)
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
                   /* Value in feet */

                   /* Set the return code: */
	ret = RC_OKAY;

                   /* We are done: */
	return ret;
}

/*----------------------------------------------------------------------------*/
/* proc_i021_146   -- Process I021/146 data item                              */
/*----------------------------------------------------------------------------*/

 static
 t_Retc proc_i021_146 (t_Ui16 length, t_Byte *buffer)
{
	t_Si16 alt;    /* Altitude; 25 feet */
	t_Byte df1;    /* Data field octet 1 */
	t_Byte df2;    /* Data field octet 2 */
	t_Retc ret;    /* Return code */
	t_Byte sas;    /* Source available */
	t_Byte src;    /* Source */

                   /* Preset the return code: */
	ret = RC_FAIL;

                   /* Check parameters: */
	Assert (length == 2, "Invalid parameter");
	Assert (buffer != NULL, "Invalid parameter");

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

#if LISTER
                   /* List data field: */
	if (reference_vsn != 21 && reference_vsn != 24)
	{
		list_text (2, ";  Intermediate State Selected Altitude:");
	}
	else
	{
		list_text (2, ";  Selected Altitude:");
	}
	list_text (2, " sas=" M_FMT_BYTE ";", sas);
	list_text (2, " src=" M_FMT_BYTE ";", src);
	list_text (2, " alt=" M_FMT_SI16 " (%.2f FL)", alt, 0.25 * alt);
	list_text (2, "\n");
#endif /* LISTER */

                   /* Store this information: */
	adsb.intermediate_selected_altitude.present = TRUE;
	adsb.intermediate_selected_altitude.value = 25.0 * M_FT2MTR * alt;
	adsb.intermediate_selected_altitude.value_in_feet = 25 * (t_Si32) alt;
	adsb.intermediate_selected_altitude.value_sas = sas;
	adsb.intermediate_selected_altitude.value_src = src;

                   /* Set the return code: */
	ret = RC_OKAY;

                   /* We are done: */
	return ret;
}

/*----------------------------------------------------------------------------*/
/* proc_i021_147   -- Process I021/147 data item                              */
/*----------------------------------------------------------------------------*/

 static
 t_Retc proc_i021_147 (t_Ui16 length, t_Byte *buffer)
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

                   /* Check reference version: */
	Assert (reference_vsn != 14 && reference_vsn != 15 &&
            reference_vsn != 21 && reference_vsn != 24,
            "Data item not defined");

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

                   /* We are done: */
	return ret;
}

/*----------------------------------------------------------------------------*/
/* proc_i021_148   -- Process I021/148 data item                              */
/*----------------------------------------------------------------------------*/

 static
 t_Retc proc_i021_148 (t_Ui16 length, t_Byte *buffer)
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
	list_text (2, " am=" M_FMT_BYTE ";", (df1 >> 5) & 0x01);
	list_text (2, " alt=" M_FMT_SI16 " (%.2f FL)", alt, 0.25 * alt);
	list_text (2, "\n");
#endif /* LISTER */

                   /* Store this information: */
	adsb.final_selected_altitude.present = TRUE;
	adsb.final_selected_altitude.value = 25 * M_FT2MTR * alt;
                   /* In metres */
	adsb.final_selected_altitude.value_ah = (df1 >> 6) & 0x01;
	adsb.final_selected_altitude.value_am = (df1 >> 5) & 0x01;
	adsb.final_selected_altitude.value_in_feet = 25 * (t_Si32) alt;
	adsb.final_selected_altitude.value_mv = (df1 >> 7) & 0x01;

                   /* Set the return code: */
	ret = RC_OKAY;

                   /* We are done: */
	return ret;
}

/*----------------------------------------------------------------------------*/
/* proc_i021_150   -- Process I021/150 data item                              */
/*----------------------------------------------------------------------------*/

 static
 t_Retc proc_i021_150 (t_Ui16 length, t_Byte *buffer)
{
	t_Byte df1;    /* Data field octet 1 */
	t_Byte df2;    /* Data field octet 2 */
#if LISTER
	t_Byte df3;    /* Data field octet 3 */
	t_Byte df4;    /* Data field octet 4 */
	t_Byte df5;    /* Data field octet 5 */
	t_Byte df6;    /* Data field octet 6 */
	t_Ui16 hdg;    /* Heading; 360/2**16 degrees */
#endif /* LISTER */
	t_Byte im;     /* Air speed in IAS or in Mach */
	t_Retc ret;    /* Return code */
	t_Ui16 spd;    /* Air speed */
#if LISTER
	t_Si16 vr;     /* Vertical rate; 6.25 feet/minute */
#endif /* LISTER */

                   /* Preset the return code: */
	ret = RC_FAIL;

                   /* Check parameters: */
	if (reference_vsn == 1 || reference_vsn == 2)
	{
		Assert (length == 6, "Invalid parameter");
	}
	else if (reference_vsn == 9 || reference_vsn == 12 ||
             reference_vsn == 14 || reference_vsn == 15 ||
             reference_vsn == 21 || reference_vsn == 24)
	{
		Assert (length == 2, "Invalid parameter");
	}
	Assert (buffer != NULL, "Invalid parameter");

                   /* Extract octets: */
	if (reference_vsn == 1 || reference_vsn == 2)
	{
		df1 = buffer[0];
		df2 = buffer[1];
#if LISTER
		df3 = buffer[2];
		df4 = buffer[3];
		df5 = buffer[4];
		df6 = buffer[5];
#endif /* LISTER */
	}
	else if (reference_vsn == 9 || reference_vsn == 12 ||
             reference_vsn == 14 || reference_vsn == 15 ||
             reference_vsn == 21 || reference_vsn == 24)
	{
		df1 = buffer[0];
		df2 = buffer[1];
	}

                   /* Extract information: */
	if (reference_vsn == 1 || reference_vsn == 2)
	{
		im = (df1 >> 7) & 0x01;
		spd = make_ui16 (df1 & 0x7f, df2);
#if LISTER
		hdg = make_ui16 (df3, df4);
		vr = make_ui16 (df5, df6);
#endif /* LISTER */
	}
	else if (reference_vsn == 9 || reference_vsn == 12 ||
             reference_vsn == 14 || reference_vsn == 15 ||
             reference_vsn == 21 || reference_vsn == 24)
	{
		im = (df1 >> 7) & 0x01;
		spd = make_ui16 (df1 & 0x7f, df2);
	}

#if LISTER
                   /* List data field: */
	if (reference_vsn == 1 || reference_vsn == 2)
	{
		list_text (2, ";  Air Vector:");
		list_text (2, " spd=");
		if (im == 0)
		{
			list_text (2, "%.3f kts;", 3600.0 * spd / 16384.0);
		}
		else
		{
			list_text (2, "%.3f Mach;", 0.001 * spd);
		}
		list_text (2, " hdg=%.3f deg;", 360.0 * hdg / 65536.0);
		list_text (2, " vr=%.2f ft/min", 6.25 * vr);
		list_text (2, "\n");
	}
	else if (reference_vsn == 9 || reference_vsn == 12 ||
             reference_vsn == 14 || reference_vsn == 15 ||
             reference_vsn == 21 || reference_vsn == 24)
	{
		list_text (2, ";  Air Speed:");
		list_text (2, " im=" M_FMT_BYTE ";", im);
		list_text (2, " spd=");
		if (im == 0)
		{
			list_text (2, "%.3f (kts)", (3600.0 / 16384.0) * spd);
		}
		else
		{
			list_text (2, "%.3f (Mach)", 0.001 * spd);
		}
		list_text (2, "\n");
	}
#endif /* LISTER */

                   /* Store this information: */
	/* tba ... */
	if (reference_vsn == 15 ||
        reference_vsn == 21 || reference_vsn == 24)
	{
		adsb.air_speed.present = TRUE;
		if (im == 0)
		{
			adsb.air_speed.value = 3600.0 / 16384.0 * spd;
			adsb.air_speed.value_im = 0;
		}
		else
		{
			adsb.air_speed.value = 0.001 * spd;
			adsb.air_speed.value_im = 1;
		}
	}

                   /* Set the return code: */
	ret = RC_OKAY;

                   /* We are done: */
	return ret;
}

/*----------------------------------------------------------------------------*/
/* proc_i021_151   -- Process I021/151 data item                              */
/*----------------------------------------------------------------------------*/

 static
 t_Retc proc_i021_151 (t_Ui16 length, t_Byte *buffer)
{
	t_Byte df1;    /* Data field octet 1 */
	t_Byte df2;    /* Data field octet 2 */
	t_Byte re;     /* Range exceeded indicator */
	t_Retc ret;    /* Return code */
	t_Ui16 spd;    /* True air speed; knots */

                   /* Preset the return code: */
	ret = RC_FAIL;

                   /* Check parameters: */
	Assert (length == 2, "Invalid parameter");
	Assert (buffer != NULL, "Invalid parameter");

                   /* Extract octets: */
	df1 = buffer[0];
	df2 = buffer[1];

                   /* Extract information: */
	if (reference_vsn != 14 && reference_vsn != 15 &&
        reference_vsn != 21 && reference_vsn != 24)
	{
		spd = make_ui16 (df1, df2);
	}
	else
	{
		re = (df1 >> 7) & 0x01;
		spd = make_ui16 (df1 & 0x7f, df2);
	}

#if LISTER
                   /* List data field: */
	if (reference_vsn != 14 && reference_vsn != 15 &&
        reference_vsn != 21 && reference_vsn != 24)
	{
		list_text (2, ";  True Air Speed:");
		list_text (2, " spd=" M_FMT_UI16 " kts", spd);
		list_text (2, "\n");
	}
	else
	{
		list_text (2, ";  True Airspeed:");
		list_text (2, " re=" M_FMT_BYTE ";", re);
		list_text (2, " spd=" M_FMT_UI16 " kts", spd);
		list_text (2, "\n");
	}
#endif /* LISTER */

                   /* Store this information: */
	/* tba ... */
	if (reference_vsn == 14 || reference_vsn == 15 ||
        reference_vsn == 21 || reference_vsn == 24)
	{
		adsb.true_airspeed.present = TRUE;
		adsb.true_airspeed.range_exceeded = M_TRES (re);
		adsb.true_airspeed.value = M_KTS2MPS * spd;
                   /* In metres/second */
	}

                   /* Set the return code: */
	ret = RC_OKAY;

                   /* We are done: */
	return ret;
}

/*----------------------------------------------------------------------------*/
/* proc_i021_152   -- Process I021/152 data item                              */
/*----------------------------------------------------------------------------*/

 static
 t_Retc proc_i021_152 (t_Ui16 length, t_Byte *buffer)
{
	t_Byte df1;    /* Data field octet 1 */
	t_Byte df2;    /* Data field octet 2 */
	t_Ui16 hdg;    /* Magnetic heading; 360/2**16 degrees */
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
	hdg = make_ui16 (df1, df2);

#if LISTER
                   /* List data field: */
	list_text (2, ";  Magnetic Heading:");
	list_text (2, " " M_FMT_UI16 " (%.3f deg)", hdg, (360.0 / 65536.0) * hdg);
	list_text (2, "\n");
#endif /* LISTER */

                   /* Store this information: */
	adsb.magnetic_heading.present = TRUE;
	adsb.magnetic_heading.value = M_TWO_PI / 65536.0 * hdg;
                   /* In radians */

                   /* Set the return code: */
	ret = RC_OKAY;

                   /* We are done: */
	return ret;
}

/*----------------------------------------------------------------------------*/
/* proc_i021_155   -- Process I021/155 data item                              */
/*----------------------------------------------------------------------------*/

 static
 t_Retc proc_i021_155 (t_Ui16 length, t_Byte *buffer)
{
	t_Si16 bvr;    /* Barometric vertical rate; 6.25 feet/minute */
	t_Byte df1;    /* Data field octet 1 */
	t_Byte df2;    /* Data field octet 2 */
	t_Byte re;     /* Range exceeded indicator */
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
	if (reference_vsn != 14 && reference_vsn != 15 &&
        reference_vsn != 21 && reference_vsn != 24)
	{
		bvr = make_si16 (df1, df2);
	}
	else
	{
		re = (df1 >> 7) & 0x01;
		if (df1 & 0x40)
		{
			bvr = make_si16 (df1 & 0x7f, df2) | 0x8000;
		}
		else
		{
			bvr = make_si16 (df1 & 0x7f, df2);
		}
	}

#if LISTER
                   /* List data field: */
	list_text (2, ";  Barometric Vertical Rate:");
	if (reference_vsn == 14 || reference_vsn == 15 ||
        reference_vsn == 21 || reference_vsn == 24)
	{
		list_text (2, " re=" M_FMT_BYTE ";", re);
	}
	list_text (2, " bvr=" M_FMT_SI16 " (%.2f ft/min)", bvr, 6.25 * bvr);
	list_text (2, "\n");
#endif /* LISTER */

                   /* Store this information: */
	adsb.barometric_vertical_rate.present = TRUE;
	if (reference_vsn == 14 || reference_vsn == 15 ||
        reference_vsn == 21 || reference_vsn == 24)
	{
		adsb.barometric_vertical_rate.range_exceeded = M_TRES (re);
	}
	adsb.barometric_vertical_rate.value = 6.25 * (M_FT2MTR / 60.0) * bvr;
                   /* In metres/second */

                   /* Set the return code: */
	ret = RC_OKAY;

                   /* We are done: */
	return ret;
}

/*----------------------------------------------------------------------------*/
/* proc_i021_157   -- Process I021/157 data item                              */
/*----------------------------------------------------------------------------*/

 static
 t_Retc proc_i021_157 (t_Ui16 length, t_Byte *buffer)
{
	t_Byte df1;    /* Data field octet 1 */
	t_Byte df2;    /* Data field octet 2 */
	t_Si16 gvr;    /* Geometric vertical rate; 6.25 feet/minute */
	t_Byte re;     /* Range exceeded indicator */
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
	if (reference_vsn != 14 && reference_vsn != 15 &&
        reference_vsn != 21 && reference_vsn != 24)
	{
		gvr = make_si16 (df1, df2);
	}
	else
	{
		re = (df1 >> 7) & 0x01;
		if (df1 & 0x40)
		{
			gvr = make_si16 (df1 & 0x7f, df2) | 0x8000;
		}
		else
		{
			gvr = make_si16 (df1 & 0x7f, df2);
		}
	}

#if LISTER
                   /* List data field: */
	list_text (2, ";  Geometric Vertical Rate:");
	if (reference_vsn == 14 || reference_vsn == 15 ||
        reference_vsn == 21 || reference_vsn == 24)
	{
		list_text (2, " re=" M_FMT_BYTE ";", re);
	}
	list_text (2, " gvr=" M_FMT_SI16 " (%.2f ft/min)", gvr, 6.25 * gvr);
	list_text (2, "\n");
#endif /* LISTER */

                   /* Store this information: */
	adsb.geometric_vertical_rate.present = TRUE;
	if (reference_vsn == 14 || reference_vsn == 15 ||
        reference_vsn == 21 || reference_vsn == 24)
	{
		adsb.geometric_vertical_rate.range_exceeded = M_TRES (re);
	}
	adsb.geometric_vertical_rate.value = 6.25 * (M_FT2MTR / 60.0) * gvr;
                   /* In metres/second */

                   /* Set the return code: */
	ret = RC_OKAY;

                   /* We are done: */
	return ret;
}

/*----------------------------------------------------------------------------*/
/* proc_i021_160   -- Process I021/160 data item                              */
/*----------------------------------------------------------------------------*/

 static
 t_Retc proc_i021_160 (t_Ui16 length, t_Byte *buffer)
{
	t_Byte df1;    /* Data field octet 1 */
	t_Byte df2;    /* Data field octet 2 */
	t_Byte df3;    /* Data field octet 3 */
	t_Byte df4;    /* Data field octet 4 */
#if LISTER
	t_Byte df5;    /* Data field octet 5 */
	t_Byte df6;    /* Data field octet 6 */
#endif /* LISTER */
	t_Byte re;     /* Range exceeded indicator */
	t_Retc ret;    /* Return code */
	t_Si16 spd;    /* Ground speed; 2**-14 nautical miles/second */
	t_Ui16 ta;     /* Track angle; 360/2**16 degrees */
#if LISTER
	t_Si16 vr;     /* Vertical rate; 6.25 ft/min */
#endif /* LISTER */

                   /* Preset the return code: */
	ret = RC_FAIL;

                   /* Check parameters: */
	if (reference_vsn == 1 || reference_vsn == 2)
	{
		Assert (length == 6, "Invalid parameter");
	}
	else if (reference_vsn == 9 || reference_vsn == 12 ||
             reference_vsn == 14 || reference_vsn == 15 ||
             reference_vsn == 21 || reference_vsn == 24)
	{
		Assert (length == 4, "Invalid parameter");
	}
	Assert (buffer != NULL, "Invalid parameter");

                   /* Extract octets: */
	if (reference_vsn == 1 || reference_vsn == 2)
	{
		df1 = buffer[0];
		df2 = buffer[1];
		df3 = buffer[2];
		df4 = buffer[3];
#if LISTER
		df5 = buffer[4];
		df6 = buffer[5];
#endif /* LISTER */
	}
	else if (reference_vsn == 9 || reference_vsn == 12 ||
             reference_vsn == 14 || reference_vsn == 15 ||
             reference_vsn == 21 || reference_vsn == 24)
	{
		df1 = buffer[0];
		df2 = buffer[1];
		df3 = buffer[2];
		df4 = buffer[3];
	}

                   /* Extract information: */
	if (reference_vsn == 1 || reference_vsn == 2)
	{
		spd = make_si16 (df1, df2);
		ta = make_ui16 (df3, df4);
#if LISTER
		vr = make_ui16 (df5, df6);
#endif /* LISTER */
	}
	else if (reference_vsn == 9 || reference_vsn == 12)
	{
		spd = make_si16 (df1, df2);
		ta = make_ui16 (df3, df4);
	}
	else if (reference_vsn == 14 || reference_vsn == 15 ||
             reference_vsn == 21 || reference_vsn == 24)
	{
		re = (df1 >> 7) & 0x01;
		spd = make_si16 (df1 & 0x7f, df2);
		ta = make_ui16 (df3, df4);
	}

#if LISTER
                   /* List data field: */
	list_text (2, ";  Ground Vector:");
	if (reference_vsn == 14 || reference_vsn == 15 ||
        reference_vsn == 21 || reference_vsn == 24)
	{
		list_text (2, " re=" M_FMT_BYTE ";", re);
	}
	list_text (2, " spd=" M_FMT_SI16 " (%.3f kts);",
                  spd, (3600.0 / 16384.0) * spd);
	list_text (2, " ta=" M_FMT_UI16 " (%.3f deg)",
                  ta, (360.0 / 65536.0) * ta);
	if (reference_vsn == 1 || reference_vsn == 2)
	{
		list_text (2, ";");
		list_text (2, " vr=" M_FMT_SI16 " (%.2f ft/min)", vr, 6.25 * vr);
	}
	list_text (2, "\n");
#endif /* LISTER */

                   /* Store this information: */
	adsb.ground_vector.present = TRUE;
	if (reference_vsn == 14 || reference_vsn == 15 ||
        reference_vsn == 21 || reference_vsn == 24)
	{
		adsb.ground_vector.range_exceeded = M_TRES (re);
	}
	adsb.ground_vector.value_gsp = (M_NMI2MTR / 16384.0) * spd;
	adsb.ground_vector.value_hdg = (M_TWO_PI / 65536.0) * ta; 
                   /* Value is in radians */
	/* tba ... */

                   /* Set the return code: */
	ret = RC_OKAY;

                   /* We are done: */
	return ret;
}

/*----------------------------------------------------------------------------*/
/* proc_i021_161   -- Process I021/161 data item                              */
/*----------------------------------------------------------------------------*/

 static
 t_Retc proc_i021_161 (t_Ui16 length, t_Byte *buffer)
{
	t_Byte df1;    /* Data field octet 1 */
	t_Byte df2;    /* Data field octet 2 */
	t_Retc ret;    /* Return code */
	t_Ui16 tn;     /* Track number */

                   /* Preset the return code: */
	ret = RC_FAIL;

                   /* Check parameters: */
	Assert (length == 2, "Invalid parameter");
	Assert (buffer != NULL, "Invalid parameter");

                   /* Check reference version: */
	Assert (reference_vsn == 14 || reference_vsn == 15 ||
            reference_vsn == 21 || reference_vsn == 24,
            "Data item not defined");

                   /* Extract octets: */
	df1 = buffer[0];
	df2 = buffer[1];

                   /* Extract information: */
	tn = make_ui16 (df1 & 0x0f, df2);

#if LISTER
                   /* List data field: */
	if (reference_vsn == 14)
	{
		list_text (2, ";  Track ID:");
	}
	else if (reference_vsn == 15 ||
             reference_vsn == 21 || reference_vsn == 24)
	{
		list_text (2, ";  Track Number:");
	}
	list_text (2, " tn=" M_FMT_UI16, tn);
	list_text (2, "\n");
#endif /* LISTER */

                   /* Store this information: */
	adsb.track_number.present = TRUE;
	adsb.track_number.value = tn;

                   /* Set the return code: */
	ret = RC_OKAY;

                   /* We are done: */
	return ret;
}

/*----------------------------------------------------------------------------*/
/* proc_i021_165   -- Process I021/165 data item                              */
/*----------------------------------------------------------------------------*/

 static
 t_Retc proc_i021_165 (t_Ui16 length, t_Byte *buffer)
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
	if (reference_vsn != 14 && reference_vsn != 15 &&
        reference_vsn != 21 && reference_vsn != 24)
	{
		Assert (length >= 1, "Invalid parameter");
	}
	else
	{
		Assert (length == 2, "Invalid parameter");
	}
	Assert (buffer != NULL, "Invalid parameter");

                   /* Extract octets: */
	if (reference_vsn != 14 && reference_vsn != 15 &&
        reference_vsn != 21 && reference_vsn != 24)
	{
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
	}
	else
	{
		df1 = buffer[0];
		df2 = buffer[1];
	}

                   /* Extract information: */
	if (reference_vsn != 14 && reference_vsn != 15 &&
        reference_vsn != 21 && reference_vsn != 24)
	{
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
	}
	else
	{
		rate = make_si16 (df1 & 0x03, df2);
		if (df1 & 0x02)
		{
			rate |= 0xfc00;
		}
                   /* Two's complement */
	}

#if LISTER
                   /* List data field: */
	if (reference_vsn != 14 && reference_vsn != 15 &&
        reference_vsn != 21 && reference_vsn != 24)
	{
		list_text (2, ";  Rate of Turn:");
		list_text (2, " ti=" M_FMT_BYTE, ti);
		if (reference_vsn == 1 || reference_vsn == 2)
		{
			if (ti == 0)
			{
				list_text (2, " (left turn)");
			}
			else if (ti == 1)
			{
				list_text (2, " (right turn)");
			}
			else if (ti == 2)
			{
				list_text (2, " (straight)");
			}
		}
		else if (reference_vsn == 9 || reference_vsn == 12)
		{
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
		}
		if (df1 & 0x01)
		{
			list_text (2, ";");
			list_text (2, " rate=%.2f deg/sec", 0.25 * rate);
		}
		list_text (2, "\n");
	}
	else
	{
		list_text (2, ";  Track Angle Rate:");
		list_text (2, " rate=%.2f deg/sec", (1.0 / 32.0) * rate);
		list_text (2, "\n");
	}
#endif /* LISTER */

                   /* Remember this information: */
	/* tba ... */
	if (reference_vsn == 14 || reference_vsn == 15 ||
        reference_vsn == 21 || reference_vsn == 24)
	{
		adsb.track_angle_rate.present = TRUE;
		adsb.track_angle_rate.value = (1.0 / 32.0) * (M_PI / 180.0) * rate;
                   /* In radians/second */
	}

                   /* Set the return code: */
	ret = RC_OKAY;

                   /* We are done: */
	return ret;
}

/*----------------------------------------------------------------------------*/
/* proc_i021_170   -- Process I021/170 data item                              */
/*----------------------------------------------------------------------------*/

 static
 t_Retc proc_i021_170 (t_Ui16 length, t_Byte *buffer)
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

                   /* We are done: */
	return ret;
}

/*----------------------------------------------------------------------------*/
/* proc_i021_180   -- Process I021/180 data item                              */
/*----------------------------------------------------------------------------*/

 static
 t_Retc proc_i021_180 (t_Ui16 length, t_Byte *buffer)
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

                   /* Check reference version: */
	Assert (reference_vsn != 14 && reference_vsn != 15 &&
            reference_vsn != 21 && reference_vsn != 24,
            "Data item not defined");

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

                   /* We are done: */
	return ret;
}

/*----------------------------------------------------------------------------*/
/* proc_i021_190   -- Process I021/190 data item                              */
/*----------------------------------------------------------------------------*/

 static
 t_Retc proc_i021_190 (t_Ui16 length, t_Byte *buffer)
{
	t_Byte df1;    /* Data field octet 1 */
	t_Byte df2;    /* Data field octet 2 */
	t_Byte df3;    /* Data field octet 3 */
	t_Byte df4;    /* Data field octet 4 */
	t_Retc ret;    /* Return code */
	char txt[5];   /* Text buffer for aircraft type */

                   /* Preset the return code: */
	ret = RC_FAIL;

                   /* Check parameters: */
	Assert (length == 4, "Invalid parameter");
	Assert (buffer != NULL, "Invalid parameter");

                   /* Check reference version: */
	Assert (reference_vsn != 14 && reference_vsn != 15 &&
            reference_vsn != 21 && reference_vsn != 24,
            "Data item not defined");

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

#if LISTER
                   /* List data field: */
	list_text (2, ";  Aircraft Type:");
	list_text (2, " %s", txt);
	list_text (2, "\n");
#endif /* LISTER */

                   /* Store this information: */
	/* tba ... */

                   /* Set the return code: */
	ret = RC_OKAY;

                   /* We are done: */
	return ret;
}

/*----------------------------------------------------------------------------*/
/* proc_i021_200   -- Process I021/200 data item                              */
/*----------------------------------------------------------------------------*/

 static
 t_Retc proc_i021_200 (t_Ui16 length, t_Byte *buffer)
{
	t_Byte df1;    /* Data field octet 1 */
#if LISTER
	t_Byte icf;    /* Intent change flag */
	t_Byte lnav;   /* LNAV mode */
	int out;       /* Auxiliary */
#endif /* LISTER */
	t_Byte ps;     /* Priority status */
	t_Retc ret;    /* Return code */
	t_Byte ss;     /* Surveillance status */
	t_Byte sta;    /* Target status */

                   /* Preset the return code: */
	ret = RC_FAIL;

                   /* Check parameters: */
	Assert (length == 1, "Invalid parameter");
	Assert (buffer != NULL, "Invalid parameter");

                   /* Extract octets: */
	df1 = buffer[0];

                   /* Preset local data: */
#if LISTER
	icf = lnav = 0;
#endif /* LISTER */
	ps = ss = sta = 0;

                   /* Extract information: */
	if (reference_vsn != 14 && reference_vsn != 15 &&
        reference_vsn != 21 && reference_vsn != 24)
	{
		sta = df1;
	}
	else
	{
#if LISTER
		icf = lnav = 0;
		if (reference_vsn == 21)
		{
			icf = (df1 >> 7) & 0x01;
			lnav = (df1 >> 6) & 0x01;
		}
#endif /* LISTER */
		ps = (df1 >> 2) & 0x07;
		ss = df1 & 0x03;
	}

#if LISTER
                   /* List data field: */
	list_text (2, ";  Target Status:");
	if (reference_vsn == 1 || reference_vsn == 2)
	{
		out = 0;
		if (sta & 0x01)
		{
			list_text (2, " No emergency");
			++ out;
		}
		if (sta & 0x02)
		{
			if (out > 0)
			{
				list_text (2, ";");
			}
			list_text (2, " General emergency");
			++ out;
		}
		if (sta & 0x04)
		{
			if (out > 0)
			{
				list_text (2, ";");
			}
			list_text (2, " Lifeguard/medical");
			++ out;
		}
		if (sta & 0x08)
		{
			if (out > 0)
			{
				list_text (2, ";");
			}
			list_text (2, " Minimum fuel");
			++ out;
		}
		if (sta & 0x10)
		{
			if (out > 0)
			{
				list_text (2, ";");
			}
			list_text (2, " No communications");
			++ out;
		}
		if (sta & 0x20)
		{
			if (out > 0)
			{
				list_text (2, ";");
			}
			list_text (2, " Unlawful interference");
			++ out;
		}
	}
	else if (reference_vsn == 9 || reference_vsn == 12)
	{
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
	}
	else if (reference_vsn == 14 || reference_vsn == 15 ||
             reference_vsn == 21 || reference_vsn == 24)
	{
		if (reference_vsn == 21 || reference_vsn == 24)
		{
			list_text (2, " icf=" M_FMT_BYTE ";", icf);
			list_text (2, " lnav=" M_FMT_BYTE ";", lnav);
		}
		list_text (2, " ps=" M_FMT_BYTE ";", ps);
		list_text (2, " ss=" M_FMT_BYTE, ss);
	}
	list_text (2, "\n");
#endif /* LISTER */

                   /* Store this information: */
	/* tba ... */
	if (reference_vsn == 14 || reference_vsn == 15 ||
        reference_vsn == 21 || reference_vsn == 24)
	{
		adsb.target_status.present = TRUE;
		/*adsb.target_status.value_icf = icf;*/
		/*adsb.target_status.value_lnav = lnav;*/
		adsb.target_status.value_ps = ps;
		adsb.target_status.value_ss = ss;
	}

                   /* Set the return code: */
	ret = RC_OKAY;

                   /* We are done: */
	return ret;
}

/*----------------------------------------------------------------------------*/
/* proc_i021_210   -- Process I021/210 data item                              */
/*----------------------------------------------------------------------------*/

 static
 t_Retc proc_i021_210 (t_Ui16 length, t_Byte *buffer)
{
	t_Byte df1;    /* Data field octet 1 */
	t_Byte lti;    /* Link technology indicator */
	t_Byte ltt;    /* Link technology type */
#if LISTER
	int out;       /* Auxiliary */
#endif /* LISTER */
	t_Retc ret;    /* Return code */
	t_Byte vn;     /* Version number */
	t_Byte vns;    /* Version not supported */

                   /* Preset the return code: */
	ret = RC_FAIL;

                   /* Check parameters: */
	Assert (length == 1, "Invalid parameter");
	Assert (buffer != NULL, "Invalid parameter");

                   /* Extract octets: */
	df1 = buffer[0];

                   /* Preset local data: */
	lti = ltt = vn = vns = 0;

                   /* Extract information: */
	if (reference_vsn != 14 && reference_vsn != 15 &&
        reference_vsn != 21 && reference_vsn != 24)
	{
		lti = df1;
	}
	else
	{
		vns = (df1 >> 6) & 0x01;
		vn = (df1 >> 3) & 0x07;
		ltt = df1 & 0x07;
	}

#if LISTER
                   /* List data field: */
	if (reference_vsn != 14 && reference_vsn != 15 &&
        reference_vsn != 21 && reference_vsn != 24)
	{
		list_text (2, ";  Link Technology Indicator:");
		out = 0;
		if (reference_vsn == 12)
		{
			if (lti & 0x10)
			{
				list_text (2, " CDTI");
				++ out;
			}
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
	}
	else
	{
		list_text (2, ";  MOPS Version:");
		list_text (2, " vns=" M_FMT_BYTE ";", vns);
		list_text (2, " vn=" M_FMT_BYTE, vn);
		if (vn == 0)
		{
			list_text (2, " (DO-260)");
		}
		else if (vn == 1)
		{
			list_text (2, " (DO-260A)");
		}
		else if (vn == 2)
		{
			list_text (2, " (DO-260B)");
		}
		list_text (2, ";");
		list_text (2, " ltt=" M_FMT_BYTE, ltt);
		switch (ltt)
		{
		case 0:
			list_text (2, " (other)");
			break;
		case 1:
			list_text (2, " (UAT)");
			break;
		case 2:
			list_text (2, " (1090 ES)");
			break;
		case 3:
			list_text (2, " (VDL 4)");
			break;
		default:
			break;
		}
	}
	list_text (2, "\n");
#endif /* LISTER */

                   /* Store this information: */
	if (reference_vsn != 14 && reference_vsn != 15 &&
        reference_vsn != 21 && reference_vsn != 24)
	{
		adsb.link_technology.present = TRUE;
		adsb.link_technology.value = lti;
	}
	else
	{
		adsb.mops_version.present = TRUE;
		adsb.mops_version.value_ltt = ltt;
		adsb.mops_version.value_vn = vn;
		adsb.mops_version.value_vns = vns;
	}

                   /* Set the return code: */
	ret = RC_OKAY;

                   /* We are done: */
	return ret;
}

/*----------------------------------------------------------------------------*/
/* proc_i021_220   -- Process I021/220 data item                              */
/*----------------------------------------------------------------------------*/

 static
 t_Retc proc_i021_220 (t_Ui16 length, t_Byte *buffer, t_Ui16 *pos_ptr)
{
	t_Byte df1;    /* Data field octet 1 */
	t_Byte df2;    /* Data field octet 2 */
	t_Byte df3;    /* Data field octet 3 */
	t_Byte len;    /* Length of data field */
	int inx;       /* Auxiliary */
#if LISTER
	int ix;        /* Auxiliary */
	int j;         /* Auxiliary */
#endif /* LISTER */
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
	t_Si16 tmp;    /* Temperature; 0.25 degrees Celsius */
	t_Byte trb;    /* Turbulence */
	t_Ui16 wd;     /* Wind direction; degrees */
	t_Ui16 ws;     /* Wind speed; knots */

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
		error_msg ("Invalid buffer length (I021/220)");
		goto done;
	}

#if LISTER
                   /* List raw data field: */
	list_text (1, ";  I021/220: 0x");
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

                   /* Extract secondary subfields: */
	inx = 1;
	if (sf1_present)
	{
		df2 = buffer[pos + inx];
		df3 = buffer[pos + inx + 1];

		ws = make_ui16 (df2, df3);
		inx += 2;
	}
	if (sf2_present)
	{
		df2 = buffer[pos + inx];
		df3 = buffer[pos + inx + 1];

		wd = make_ui16 (df2, df3);
		inx += 2;
	}
	if (sf3_present)
	{
		df2 = buffer[pos + inx];
		df3 = buffer[pos + inx + 1];

		tmp = make_si16 (df2, df3);
		inx += 2;
	}
	if (sf4_present)
	{
		trb = buffer[pos + inx];
		++ inx;
	}
	Assert (inx == len, "Invalid structure decoding");

#if LISTER
                   /* List data field: */
	list_text (2, ";  Met Information:");
	list_text (2, "\n");
	if (sf1_present)
	{
		list_text (2, ";   Wind speed:");
		list_text (2, " " M_FMT_UI16 " kts", ws);
		list_text (2, "\n");
	}
	if (sf2_present)
	{
		list_text (2, ";   Wind direction:");
		list_text (2, " " M_FMT_UI16 " deg", wd);
		list_text (2, "\n");
	}
	if (sf3_present)
	{
		list_text (2, ";   Temperature:");
		list_text (2, " " M_FMT_SI16 " (%.2f °C)", tmp, 0.25 * tmp);
		list_text (2, "\n");
	}
	if (sf4_present)
	{
		list_text (2, ";   Turbulence:");
		list_text (2, " " M_FMT_BYTE, trb);
		list_text (2, "\n");
	}
#endif /* LISTER */

                   /* Store this information: */
	adsb.met_information.present = TRUE;
	adsb.met_information.present_ws = sf1_present;
	if (sf1_present)
	{
		adsb.met_information.value_ws = M_KTS2MPS * ws;
	}
	adsb.met_information.present_wd = sf2_present;
	if (sf2_present)
	{
		adsb.met_information.value_wd = M_DEG2RAD * wd;
	}
	adsb.met_information.present_tmp = sf3_present;
	if (sf3_present)
	{
		adsb.met_information.value_tmp = 0.25 * tmp;
	}
	adsb.met_information.present_trb = sf4_present;
	if (sf4_present)
	{
		adsb.met_information.value_trb = trb;
	}

                   /* Set position: */
	*pos_ptr = pos + len;

                   /* Set the return code: */
	ret = RC_OKAY;

	done:          /* We are done */
	return ret;
}

/*----------------------------------------------------------------------------*/
/* proc_i021_230   -- Process I021/230 data item                              */
/*----------------------------------------------------------------------------*/

 static
 t_Retc proc_i021_230 (t_Ui16 length, t_Byte *buffer)
{
	t_Byte df1;    /* Data field octet 1 */
	t_Byte df2;    /* Data field octet 2 */
	t_Si16 ra;     /* Roll angle; 0.01 degrees */
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
	ra = make_si16 (df1, df2);

#if LISTER
                   /* List data field: */
	list_text (2, ";  Roll Angle:");
	list_text (2, " " M_FMT_SI16 " (%.2f deg)", ra, 0.01 * ra);
	list_text (2, "\n");
#endif /* LISTER */

                   /* Store this information: */
	adsb.roll_angle.present = TRUE;
	adsb.roll_angle.value = 0.01 * M_DEG2RAD * ra;
                   /* In radians */

                   /* Set the return code: */
	ret = RC_OKAY;

                   /* We are done: */
	return ret;
}

/*----------------------------------------------------------------------------*/
/* proc_i021_250   -- Process I021/250 data item                              */
/*----------------------------------------------------------------------------*/

 static
 t_Retc proc_i021_250 (t_Ui16 length, t_Byte *buffer)
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

                   /* Check reference version: */
	Assert (reference_vsn == 14 || reference_vsn == 15 ||
            reference_vsn == 21 || reference_vsn == 24,
            "Data item not defined");

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

		if (ix < M_MAX_BDS_REGISTERS)
		{
			adsb.bds_registers[ix].number = df8;
			adsb.bds_registers[ix].present = TRUE;
			adsb.bds_registers[ix].value[0] = df1;
			adsb.bds_registers[ix].value[1] = df2;
			adsb.bds_registers[ix].value[2] = df3;
			adsb.bds_registers[ix].value[3] = df4;
			adsb.bds_registers[ix].value[4] = df5;
			adsb.bds_registers[ix].value[5] = df6;
			adsb.bds_registers[ix].value[6] = df7;
		}
	}

	done:          /* Set the return code: */
	ret = RC_OKAY;

                   /* We are done: */
	return ret;
}

/*----------------------------------------------------------------------------*/
/* proc_i021_260   -- Process I021/260 data item                              */
/*----------------------------------------------------------------------------*/

 static
 t_Retc proc_i021_260 (t_Ui16 length, t_Byte *buffer)
{
#if LISTER
	t_Ui16 ara;    /* Active resolution advisories */
#endif /* LISTER */
	t_Byte bds;    /* BDS code */
	t_Byte df1;    /* Data field octet 1 */
	t_Byte df2;    /* Data field octet 2 */
	t_Byte df3;    /* Data field octet 3 */
	t_Byte df4;    /* Data field octet 4 */
	t_Byte df5;    /* Data field octet 5 */
	t_Byte df6;    /* Data field octet 6 */
	t_Byte df7;    /* Data field octet 7 */
#if LISTER
	t_Byte mte;    /* Multiple threat encounter */
	t_Byte rat;    /* RA terminated */
#endif /* LISTER */
	t_Retc ret;    /* Return code */
#if LISTER
	t_Byte rr;     /* RACs record */
	t_Ui32 tid;    /* Threat identity data */
	t_Byte tti;    /* Threat type indicator */
#endif /* LISTER */

                   /* Preset the return code: */
	ret = RC_FAIL;

                   /* Check parameters: */
	Assert (length == 7, "Invalid parameter");
	Assert (buffer != NULL, "Invalid parameter");

                   /* Check reference version: */
	Assert (reference_vsn == 14 || reference_vsn == 15 ||
            reference_vsn == 21 || reference_vsn == 24,
            "Data item not defined");

                   /* Extract octets: */
	df1 = buffer[0];
	df2 = buffer[1];
	df3 = buffer[2];
	df4 = buffer[3];
	df5 = buffer[4];
	df6 = buffer[5];
	df7 = buffer[6];

                   /* Extract information: */
	bds = df1;
#if LISTER
	ara = (((t_Ui16) df2) << 6) | (((t_Ui16) df3 >> 2) & 0x3f);
	rr = ((df3 & 0x03) << 2) | ((df4 >> 6) & 0x03); 
	rat = (df4 >> 5) & 0x01;
	mte = (df4 >> 4) & 0x01;
	tti = (df4 >> 2) & 0x03;
	tid = make_ui32 (df4 & 0x03, df5, df6, df7);
#endif /* LISTER */

#if LISTER
                   /* List data field: */
	list_text (2, ";  ACAS Resolution Advisory Report:");
	list_text (2, "\n");

	list_text (2, ";  ");
	list_text (2, " bds=0x" M_FMT_BYTE_HEX ";", bds);
	list_text (2, " ara=" M_FMT_UI16 ";", ara);
	list_text (2, " rr=" M_FMT_BYTE ";", rr);
	list_text (2, " rat=" M_FMT_BYTE ";", rat);
	list_text (2, " mte=" M_FMT_BYTE ";", mte);
	list_text (2, " tti=" M_FMT_BYTE ";", tti);
	list_text (2, " tid=" M_FMT_UI32 " (0x" M_FMT_UI32_HEX ")", tid, tid);
	list_text (2, "\n");
#endif /* LISTER */

                   /* Store this information: */
	if (bds == 0x30)
	{
		adsb.acas_resolution_advisory_report.present = TRUE;
		adsb.acas_resolution_advisory_report.value[0] = df1;
		adsb.acas_resolution_advisory_report.value[1] = df2;
		adsb.acas_resolution_advisory_report.value[2] = df3;
		adsb.acas_resolution_advisory_report.value[3] = df4;
		adsb.acas_resolution_advisory_report.value[4] = df5;
		adsb.acas_resolution_advisory_report.value[5] = df6;
		adsb.acas_resolution_advisory_report.value[6] = df7;
	}

                   /* Set the return code: */
	ret = RC_OKAY;

                   /* We are done: */
	return ret;
}

/*----------------------------------------------------------------------------*/
/* proc_i021_271   -- Process I021/271 data item                              */
/*----------------------------------------------------------------------------*/

 static
 t_Retc proc_i021_271 (t_Ui16 length, t_Byte *buffer, t_Ui16 *pos_ptr)
{
#if LISTER
	t_Byte b2_low; /* Class B2 transmit power less than 70 Watts */
	t_Byte cdti;   /* Cockpit display of traffic information (surface) */
#endif /* LISTER */
	t_Byte df1;    /* Data field octet 1 */
#if LISTER
	t_Byte df2;    /* Data field octet 2 */
	t_Byte ident;  /* Setting of "IDENT" switch */
	int ix;        /* Auxiliary */
	int j;         /* Auxiliary */
	t_Byte law;    /* Length and width */
#endif /* LISTER */
	t_Byte len;    /* Length of data field */
#if LISTER
	t_Byte poa;    /* Position offset applied */
#endif /* LISTER */
	t_Ui16 pos;    /* Position within buffer */
#if LISTER
	t_Byte ras;    /* Receiving ATC services */
#endif /* LISTER */
	t_Retc ret;    /* Return code */

                   /* Preset the return code: */
	ret = RC_FAIL;

                   /* Check parameters: */
	Assert (length > 0, "Invalid parameter");
	Assert (buffer != NULL, "Invalid parameter");
	Assert (pos_ptr != NULL, "Invalid parameter");

                   /* Check reference version: */
	Assert (reference_vsn == 14 || reference_vsn == 15 ||
            reference_vsn == 21 || reference_vsn == 24,
            "Data item not defined");

                   /* Set position within buffer: */
	pos = *pos_ptr;

                   /* Check position: */
	Assert (pos < length, "Invalid position");

                   /* Extract octets: */
	df1 = buffer[pos];
#if LISTER
	df2 = 0;
#endif /* LISTER */
	len = 1;
	if (df1 & 0x01)
	{
		if (pos + 1 >= length)
		{
			error_msg ("Invalid buffer length (I021/271)");
			goto done;
		}

#if LISTER
		df2 = buffer[pos + 1];
#endif /* LISTER */
		len = 2;
	}

#if LISTER
                   /* Extract information: */
	poa = (df1 >> 5) & 0x01;
	cdti = (df1 >> 4) & 0x01;
	b2_low = (df1 >> 3) & 0x01;
	ras = (df1 >> 2) & 0x01;
	ident = (df1 >> 1) & 0x01;
	if (len == 2)
	{
		if (reference_vsn == 24)
		{
			law = (df2 >> 4) & 0x0f;
		}
		else
		{
			law = df2 & 0x0f;
		}
	}

                   /* List raw data field: */
	list_text (1, ";  I021/271: 0x");
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

#if LISTER
                   /* List data field: */
	list_text (2, ";  Surface Capabilities and Characteristics:");
	list_text (2, " poa=" M_FMT_BYTE ";", poa);
	list_text (2, " cdti/s=" M_FMT_BYTE ";", cdti);
	list_text (2, " b2_low=" M_FMT_BYTE ";", b2_low);
	list_text (2, " ras=" M_FMT_BYTE ";", ras);
	list_text (2, " ident=" M_FMT_BYTE, ident);
	if (len == 2)
	{
		list_text (2, "; l+w=" M_FMT_BYTE, law);
	}
	list_text (2, "\n");
#endif /* LISTER */

                   /* Store this information: */
	/* tba ... */

                   /* Set position: */
	*pos_ptr = pos + len;

                   /* Set the return code: */
	ret = RC_OKAY;

	done:          /* We are done: */
	return ret;
}

/*----------------------------------------------------------------------------*/
/* proc_i021_295   -- Process I021/295 data item                              */
/*----------------------------------------------------------------------------*/

 static
 t_Retc proc_i021_295 (t_Ui16 length, t_Byte *buffer, t_Ui16 *pos_ptr)
{
#if LISTER
	t_Byte aos;    /* Aircraft operational status age; 0.1 seconds */
	t_Byte ara;    /* ACAS resolution advisory report age; 0.1 seconds */
	t_Byte as;     /* Air speed age; 0.1 seconds */
	t_Byte bvr;    /* Barometric vertical rate age; 0.1 seconds */
#endif /* LISTER */
	t_Byte df1;    /* Data field octet 1 */
	t_Byte df2;    /* Data field octet 2 */
	t_Byte df3;    /* Data field octet 3 */
	t_Byte df4;    /* Data field octet 4 */
#if LISTER
	t_Byte fl;     /* Flight level age; 0.1 seconds */
	t_Byte fsa;    /* Final state selected altitude age; 0.1 seconds */
	t_Byte gh;     /* Geometroc height age; 0.1 seconds */
	t_Byte gv;     /* Ground vector age; 0.1 seconds */
	t_Byte gvr;    /* Geometric vertical rate age; 0.1 seconds */
#endif /* LISTER */
	int inx;       /* Auxiliary */
#if LISTER
	t_Byte isa;    /* Intermediate state selected altitude age; 0.1 seconds */
	int ix;        /* Auxiliary */
	int j;         /* Auxiliary */
#endif /* LISTER */
	t_Byte len;    /* Length of data field */
#if LISTER
	t_Byte m3a;    /* Mode 3/A code age; 0.1 seconds */
	t_Byte mam;    /* Message amplitude age; 0.1 seconds */
	t_Byte met;    /* Met information age; 0.1 seconds */
	t_Byte mh;     /* Magnetic heading age; 0.1 seconds */
#endif /* LISTER */
	t_Ui16 pos;    /* Position within buffer */
#if LISTER
	t_Byte qi;     /* Quality indicators age; 0.1 seconds */
#endif /* LISTER */
	t_Retc ret;    /* Return code */
#if LISTER
	t_Byte roa;    /* Roll angle age; 0.1 seconds */
	t_Byte scc;    /* Surface capabilities and characteristics age;
                      0.1 seconds */
#endif /* LISTER */
	t_Byte sf1_present;
                   /* Subfield #1 (Aircraft operational status age) present */
	t_Byte sf2_present;
                   /* Subfield #2 (Target report descriptor age) present */
	t_Byte sf3_present;
                   /* Subfield #3 (Mode 3/A code age) present */
	t_Byte sf4_present;
                   /* Subfield #4 (Quality indicators age) present */
	t_Byte sf5_present;
                   /* Subfield #5 (Trajectory intent age) present */
	t_Byte sf6_present;
                   /* Subfield #6 (Message amplitude age) present */
	t_Byte sf7_present;
                   /* Subfield #7 (Geometric height age) present */
	t_Byte sf8_present;
                   /* Subfield #8 (Flight level age) present */
	t_Byte sf9_present;
                   /* Subfield #9 (Intermediate state selected altitude age)
                      present */
	t_Byte sf10_present;
                   /* Subfield #10 (Final state selected altitude age)
                      present */
	t_Byte sf11_present;
                   /* Subfield #11 (Air speed age) present */
	t_Byte sf12_present;
                   /* Subfield #12 (True air speed age) present */
	t_Byte sf13_present;
                   /* Subfield #13 (Magnetic heading age) present */
	t_Byte sf14_present;
                   /* Subfield #14 (Barometric vertical rate age) present */
	t_Byte sf15_present;
                   /* Subfield #15 (Geometric vertical rate age) present */
	t_Byte sf16_present;
                   /* Subfield #16 (Ground vector age) present */
	t_Byte sf17_present;
                   /* Subfield #17 (Track angle rate age) present */
	t_Byte sf18_present;
                   /* Subfield #18 (Target identification age) present */
	t_Byte sf19_present;
                   /* Subfield #19 (Target status age) present */
	t_Byte sf20_present;
                   /* Subfield #20 (Met information age) present */
	t_Byte sf21_present;
                   /* Subfield #21 (Roll angle age) present */
	t_Byte sf22_present;
                   /* Subfield #22 (ACAS resolution advisory age) present */
	t_Byte sf23_present;
                   /* Subfield #23 (Surface capabilities and characteristics
                      age) present */
#if LISTER
	t_Byte tar;    /* Track angle rate age; 0.1 seconds */
	t_Byte tas;    /* True air speed age; 0.1 seconds */
	t_Byte ti;     /* Trajectory intent age; 0.1 seconds */
	t_Byte tid;    /* Target identification age; 0.1 seconds */
	t_Byte trd;    /* Target report descriptor age; 0.1 seconds */
	t_Byte ts;     /* Target status age; 0.1 seconds */
#endif /* LISTER */

                   /* Preset the return code: */
	ret = RC_FAIL;

                   /* Check parameters: */
	Assert (length > 0, "Invalid parameter");
	Assert (buffer != NULL, "Invalid parameter");
	Assert (pos_ptr != NULL, "Invalid parameter");

                   /* Check reference version: */
	Assert (reference_vsn == 14 || reference_vsn == 15 ||
            reference_vsn == 21 || reference_vsn == 24,
            "Data item not defined");

                   /* Set position within buffer: */
	pos = *pos_ptr;

                   /* Check position: */
	Assert (pos < length, "Invalid position");

                   /* Preset all subfield present flags: */
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

                   /* Get first octet: */
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

                   /* Check for second octet: */
	if (df1 & 0x01)
	{
		if (pos + 1 >= length)
		{
			error_msg ("Invalid buffer length (I021/295)");
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
			error_msg ("Invalid buffer length (I021/295)");
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
			error_msg ("Invalid buffer length (I021/295)");
			goto done;
		}

		df4 = buffer[pos + 3];

		sf22_present = (df4 >> 7) & 0x01;
		sf23_present = (df4 >> 6) & 0x01;
	}

                   /* Fourth octet must not have the FX bit set: */
	Assert ((df4 & 0x01) == 0, "Unexpected FX bit");

                   /* Determine expected length of data field: */
	len = 1 +
          ord (df1 & 0x01) +
          ord (df2 & 0x01) +
          ord (df3 & 0x01) +
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
          ord (sf23_present);

                   /* Check against buffer length: */
	if (pos + len > length)
	{
		error_msg ("Invalid buffer length (I021/295)");
		goto done;
	}

#if LISTER
                   /* List raw data field: */
	list_text (1, ";  I021/295: 0x");
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
	inx = 1 +
          ord (df1 & 0x01) +
          ord (df2 & 0x01) +
          ord (df3 & 0x01);
	if (sf1_present)
	{
#if LISTER
		aos = buffer[pos + inx];
#endif /* LISTER */

		++ inx;
	}
	if (sf2_present)
	{
#if LISTER
		trd = buffer[pos + inx];
#endif /* LISTER */

		++ inx;
	}
	if (sf3_present)
	{
#if LISTER
		m3a = buffer[pos + inx];
#endif /* LISTER */

		++ inx;
	}
	if (sf4_present)
	{
#if LISTER
		qi = buffer[pos + inx];
#endif /* LISTER */

		++ inx;
	}
	if (sf5_present)
	{
#if LISTER
		ti = buffer[pos + inx];
#endif /* LISTER */

		++ inx;
	}
	if (sf6_present)
	{
#if LISTER
		mam = buffer[pos + inx];
#endif /* LISTER */

		++ inx;
	}
	if (sf7_present)
	{
#if LISTER
		gh = buffer[pos + inx];
#endif /* LISTER */

		++ inx;
	}
	if (sf8_present)
	{
#if LISTER
		fl = buffer[pos + inx];
#endif /* LISTER */

		++ inx;
	}
	if (sf9_present)
	{
#if LISTER
		isa = buffer[pos + inx];
#endif /* LISTER */

		++ inx;
	}
	if (sf10_present)
	{
#if LISTER
		fsa = buffer[pos + inx];
#endif /* LISTER */

		++ inx;
	}
	if (sf11_present)
	{
#if LISTER
		as = buffer[pos + inx];
#endif /* LISTER */

		++ inx;
	}
	if (sf12_present)
	{
#if LISTER
		tas = buffer[pos + inx];
#endif /* LISTER */

		++ inx;
	}
	if (sf13_present)
	{
#if LISTER
		mh = buffer[pos + inx];
#endif /* LISTER */

		++ inx;
	}
	if (sf14_present)
	{
#if LISTER
		bvr = buffer[pos + inx];
#endif /* LISTER */

		++ inx;
	}
	if (sf15_present)
	{
#if LISTER
		gvr = buffer[pos + inx];
#endif /* LISTER */

		++ inx;
	}
	if (sf16_present)
	{
#if LISTER
		gv = buffer[pos + inx];
#endif /* LISTER */

		++ inx;
	}
	if (sf17_present)
	{
#if LISTER
		tar = buffer[pos + inx];
#endif /* LISTER */

		++ inx;
	}
	if (sf18_present)
	{
#if LISTER
		tid = buffer[pos + inx];
#endif /* LISTER */

		++ inx;
	}
	if (sf19_present)
	{
#if LISTER
		ts = buffer[pos + inx];
#endif /* LISTER */

		++ inx;
	}
	if (sf20_present)
	{
#if LISTER
		met = buffer[pos + inx];
#endif /* LISTER */

		++ inx;
	}
	if (sf21_present)
	{
#if LISTER
		roa = buffer[pos + inx];
#endif /* LISTER */

		++ inx;
	}
	if (sf22_present)
	{
#if LISTER
		ara = buffer[pos + inx];
#endif /* LISTER */

		++ inx;
	}
	if (sf23_present)
	{
#if LISTER
		scc = buffer[pos + inx];
#endif /* LISTER */

		++ inx;
	}
	Assert (inx == len, "Invalid structure decoding");

#if LISTER
                   /* List data field: */
	list_text (2, ";  Data Ages:");
	list_text (2, "\n");
	if (sf1_present)
	{
		list_text (2, ";   Aircraft operational status age:");
		list_text (2, " " M_FMT_BYTE " (%.2f sec)", aos, 0.1 * aos);
		list_text (2, "\n");
	}
	if (sf2_present)
	{
		list_text (2, ";   Target report descriptor age:");
		list_text (2, " " M_FMT_BYTE " (%.2f sec)", trd, 0.1 * trd);
		list_text (2, "\n");
	}
	if (sf3_present)
	{
		list_text (2, ";   Mode 3/A code age:");
		list_text (2, " " M_FMT_BYTE " (%.2f sec)", m3a, 0.1 * m3a);
		list_text (2, "\n");
	}
	if (sf4_present)
	{
		list_text (2, ";   Quality indicators age:");
		list_text (2, " " M_FMT_BYTE " (%.2f sec)", qi, 0.1 * qi);
		list_text (2, "\n");
	}
	if (sf5_present)
	{
		list_text (2, ";   Trajectory intent age:");
		list_text (2, " " M_FMT_BYTE " (%.2f sec)", ti, 0.1 * ti);
		list_text (2, "\n");
	}
	if (sf6_present)
	{
		list_text (2, ";   Message amplitude age:");
		list_text (2, " " M_FMT_BYTE " (%.2f sec)", mam, 0.1 * mam);
		list_text (2, "\n");
	}
	if (sf7_present)
	{
		list_text (2, ";   Geometric height age:");
		list_text (2, " " M_FMT_BYTE " (%.2f sec)", gh, 0.1 * gh);
		list_text (2, "\n");
	}
	if (sf8_present)
	{
		list_text (2, ";   Flight level age:");
		list_text (2, " " M_FMT_BYTE " (%.2f sec)", fl, 0.1 * fl);
		list_text (2, "\n");
	}
	if (sf9_present)
	{
		list_text (2, ";   Intermediate state selected altitude age:");
		list_text (2, " " M_FMT_BYTE " (%.2f sec)", isa, 0.1 * isa);
		list_text (2, "\n");
	}
	if (sf10_present)
	{
		list_text (2, ";   Final state selected altitude age:");
		list_text (2, " " M_FMT_BYTE " (%.2f sec)", fsa, 0.1 * fsa);
		list_text (2, "\n");
	}
	if (sf11_present)
	{
		list_text (2, ";   Air speed age:");
		list_text (2, " " M_FMT_BYTE " (%.2f sec)", as, 0.1 * as);
		list_text (2, "\n");
	}
	if (sf12_present)
	{
		list_text (2, ";   True air speed age:");
		list_text (2, " " M_FMT_BYTE " (%.2f sec)", tas, 0.1 * tas);
		list_text (2, "\n");
	}
	if (sf13_present)
	{
		list_text (2, ";   Magnetic heading age:");
		list_text (2, " " M_FMT_BYTE " (%.2f sec)", mh, 0.1 * mh);
		list_text (2, "\n");
	}
	if (sf14_present)
	{
		list_text (2, ";   Barometric vertical rate age:");
		list_text (2, " " M_FMT_BYTE " (%.2f sec)", bvr, 0.1 * bvr);
		list_text (2, "\n");
	}
	if (sf15_present)
	{
		list_text (2, ";   Geometric vertical rate age:");
		list_text (2, " " M_FMT_BYTE " (%.2f sec)", gvr, 0.1 * gvr);
		list_text (2, "\n");
	}
	if (sf16_present)
	{
		list_text (2, ";   Ground vector age:");
		list_text (2, " " M_FMT_BYTE " (%.2f sec)", gv, 0.1 * gv);
		list_text (2, "\n");
	}
	if (sf17_present)
	{
		list_text (2, ";   Track angle rate age:");
		list_text (2, " " M_FMT_BYTE " (%.2f sec)", tar, 0.1 * tar);
		list_text (2, "\n");
	}
	if (sf18_present)
	{
		list_text (2, ";   Track identification age:");
		list_text (2, " " M_FMT_BYTE " (%.2f sec)", tid, 0.1 * tid);
		list_text (2, "\n");
	}
	if (sf19_present)
	{
		list_text (2, ";   Target status age:");
		list_text (2, " " M_FMT_BYTE " (%.2f sec)", ts, 0.1 * ts);
		list_text (2, "\n");
	}
	if (sf20_present)
	{
		list_text (2, ";   Met information age:");
		list_text (2, " " M_FMT_BYTE " (%.2f sec)", met, 0.1 * met);
		list_text (2, "\n");
	}
	if (sf21_present)
	{
		list_text (2, ";   Roll angle age:");
		list_text (2, " " M_FMT_BYTE " (%.2f sec)", roa, 0.1 * roa);
		list_text (2, "\n");
	}
	if (sf22_present)
	{
		list_text (2, ";   ACAS resolution advisory report age:");
		list_text (2, " " M_FMT_BYTE " (%.2f sec)", ara, 0.1 * ara);
		list_text (2, "\n");
	}
	if (sf23_present)
	{
		list_text (2, ";   Surface capabilities and characteristics age:");
		list_text (2, " " M_FMT_BYTE " (%.2f sec)", scc, 0.1 * scc);
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
/* proc_i021_400   -- Process I021/400 data item                              */
/*----------------------------------------------------------------------------*/

 static
 t_Retc proc_i021_400 (t_Ui16 length, t_Byte *buffer)
{
#if LISTER
	t_Byte df1;    /* Data field octet 1 */
#endif /* LISTER */
	t_Retc ret;    /* Return code */

                   /* Preset the return code: */
	ret = RC_FAIL;

                   /* Check parameters: */
	Assert (length == 1, "Invalid parameter");
	Assert (buffer != NULL, "Invalid parameter");

                   /* Check reference version: */
	Assert (reference_vsn == 14 || reference_vsn == 15 ||
            reference_vsn == 21 || reference_vsn == 24,
            "Data item not defined");

#if LISTER
                   /* Extract octets: */
	df1 = buffer[0];

                   /* List data field: */
	list_text (2, ";  Receiver ID:");
	list_text (2, " " M_FMT_BYTE, df1);
	list_text (2, "\n");
#endif /* LISTER */

                   /* Store this information: */
	/* tba ... */

                   /* Set the return code: */
	ret = RC_OKAY;

                   /* We are done: */
	return ret;
}

/*----------------------------------------------------------------------------*/
/* proc_i021_ref   -- Process REF indicator data item                         */
/*----------------------------------------------------------------------------*/

 static
 t_Retc proc_i021_ref (t_Ui16 length, t_Byte *buffer, t_Ui16 *pos_ptr)
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
		error_msg ("Invalid buffer length (I021/RE)");
		goto done;
	}

#if LISTER
                   /* List raw data field: */
	list_buffer (1, ";  I021/RE :", len, buffer + pos);
#endif /* LISTER */

                   /* Set position: */
	*pos_ptr = pos + len;

                   /* Set the return code: */
	ret = RC_OKAY;

	done:          /* We are done */
	return ret;
}

/*----------------------------------------------------------------------------*/
/* proc_i021_spf   -- Process SPF indicator data item                         */
/*----------------------------------------------------------------------------*/

 static
 t_Retc proc_i021_spf (t_Ui16 length, t_Byte *buffer, t_Ui16 *pos_ptr)
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
		error_msg ("Invalid buffer length (I021/SP)");
		goto done;
	}

#if LISTER
                   /* List raw data field: */
	list_buffer (1, ";  I021/SP :", len, buffer + pos);
#endif /* LISTER */

                   /* Set position: */
	*pos_ptr = pos + len;

                   /* Set the return code: */
	ret = RC_OKAY;

	done:          /* We are done */
	return ret;
}

/*----------------------------------------------------------------------------*/
/* set_vsn021      -- Set ASTERIX category 021 reference version              */
/*----------------------------------------------------------------------------*/

 t_Retc set_vsn021 (char *vsn_text)
{
	t_Retc ret;    /* Return code */

                   /* Preset the return code: */
	ret = RC_FAIL;

                   /* Check parameters: */
	Assert (vsn_text != NULL, "Invalid parameter");

                   /* Check against implemented reference versions: */
	if (strcmp (vsn_text, "0.12") == 0)
	{
		reference_vsn = 1;
		ret = RC_OKAY;
	}
	else if (strcmp (vsn_text, "0.13") == 0)
	{
		reference_vsn = 2;
		ret = RC_OKAY;
	}
	else if (strcmp (vsn_text, "0.14") == 0)
	{
		reference_vsn = 3;
		ret = RC_OKAY;
	}
	else if (strcmp (vsn_text, "0.15") == 0)
	{
		reference_vsn = 4;
		ret = RC_OKAY;
	}
	else if (strcmp (vsn_text, "0.16") == 0)
	{
		reference_vsn = 5;
		ret = RC_OKAY;
	}
	else if (strcmp (vsn_text, "0.17") == 0)
	{
		reference_vsn = 6;
		ret = RC_OKAY;
	}
	else if (strcmp (vsn_text, "0.18") == 0)
	{
		reference_vsn = 7;
		ret = RC_OKAY;
	}
	else if (strcmp (vsn_text, "0.19") == 0)
	{
		reference_vsn = 8;
		ret = RC_OKAY;
	}
	else if (strcmp (vsn_text, "0.20") == 0)
	{
		reference_vsn = 9;
		ret = RC_OKAY;
	}
	else if (strcmp (vsn_text, "0.21") == 0)
	{
		reference_vsn = 10;
		ret = RC_OKAY;
	}
	else if (strcmp (vsn_text, "0.22") == 0)
	{
		reference_vsn = 11;
		ret = RC_OKAY;
	}
	else if (strcmp (vsn_text, "0.23") == 0)
	{
		reference_vsn = 12;
		ret = RC_OKAY;
	}
	else if (strcmp (vsn_text, "0.27") == 0)
	{
		reference_vsn = 13;
		ret = RC_OKAY;
	}
	else if (strcmp (vsn_text, "1.0P") == 0)
	{
		reference_vsn = 14;
		ret = RC_OKAY;
	}
	else if (strcmp (vsn_text, "1.4") == 0)
	{
		reference_vsn = 15;
		ret = RC_OKAY;
	}
	else if (strcmp (vsn_text, "2.1") == 0)
	{
		reference_vsn = 21;
		ret = RC_OKAY;
	}
	else if (strcmp (vsn_text, "2.4") == 0)
	{
		reference_vsn = 24;
		ret = RC_OKAY;
	}

                   /* We are done: */
	return ret;
}
/* end-of-file */
