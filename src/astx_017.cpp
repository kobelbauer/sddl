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
/* File:           src/astx_017.cpp                                           */
/* Contents:       Process ASTERIX category 017 data block                    */
/* Author(s):      kb                                                         */
/* Last change:    2018-04-27                                                 */
/*----------------------------------------------------------------------------*/

/* Reference document:
   -------------------

   POEMS Document for ASTERIX Category 017.
   Transmission of Mode S Surveillance Coordination Function
   Messages.
   SUR.ET2.ST03.3111-SPC-02-00
   Edition: 0.5
   Edition Date: February 1999
   Status: Proposed Issue
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

#define M_MAX_FRN 21
				   /* Maximum field reference number */
#define M_MAX_FSPEC_LENGTH 3
				   /* Maximum fields specification length
					  for ASTERIX category 017 */

				   /* Local variables: */
				   /* ---------------- */

static t_Data_Item_Desc desc_i017_000;
                   /* Description of data item I017/000 */
static t_Data_Item_Desc desc_i017_010;
                   /* Description of data item I017/010 */
static t_Data_Item_Desc desc_i017_012;
                   /* Description of data item I017/012 */
static t_Data_Item_Desc desc_i017_045;
                   /* Description of data item I017/045 */
static t_Data_Item_Desc desc_i017_050;
                   /* Description of data item I017/050 */
static t_Data_Item_Desc desc_i017_070;
                   /* Description of data item I017/070 */
static t_Data_Item_Desc desc_i017_140;
                   /* Description of data item I017/140 */
static t_Data_Item_Desc desc_i017_200;
                   /* Description of data item I017/200 */
static t_Data_Item_Desc desc_i017_210;
                   /* Description of data item I017/210 */
static t_Data_Item_Desc desc_i017_220;
                   /* Description of data item I017/220 */
static t_Data_Item_Desc desc_i017_221;
                   /* Description of data item I017/221 */
static t_Data_Item_Desc desc_i017_230;
                   /* Description of data item I017/230 */
static t_Data_Item_Desc desc_i017_240;
                   /* Description of data item I017/240 */
static t_Data_Item_Desc desc_i017_350;
                   /* Description of data item I017/350 */
static t_Data_Item_Desc desc_i017_360;
                   /* Description of data item I017/360 */
static t_Data_Item_Desc desc_i017_spf;
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
static t_Retc proc_i017_000 (t_Ui16 length, t_Byte *buffer);
                   /* Process I017/000 data item */
static t_Retc proc_i017_010 (t_Ui16 length, t_Byte *buffer);
                   /* Process I017/010 data item */
static t_Retc proc_i017_012 (t_Ui16 length, t_Byte *buffer);
                   /* Process I017/012 data item */
static t_Retc proc_i017_045 (t_Ui16 length, t_Byte *buffer);
                   /* Process I017/045 data item */
static t_Retc proc_i017_050 (t_Ui16 length, t_Byte *buffer);
                   /* Process I017/050 data item */
static t_Retc proc_i017_070 (t_Ui16 length, t_Byte *buffer);
                   /* Process I017/070 data item */
static t_Retc proc_i017_140 (t_Ui16 length, t_Byte *buffer);
                   /* Process I017/140 data item */
static t_Retc proc_i017_200 (t_Ui16 length, t_Byte *buffer);
                   /* Process I017/200 data item */
static t_Retc proc_i017_210 (t_Ui16 length, t_Byte *buffer);
                   /* Process I017/210 data item */
static t_Retc proc_i017_220 (t_Ui16 length, t_Byte *buffer);
                   /* Process I017/220 data item */
static t_Retc proc_i017_221 (t_Ui16 length, t_Byte *buffer);
                   /* Process I017/221 data item */
static t_Retc proc_i017_230 (t_Ui16 length, t_Byte *buffer);
                   /* Process I017/230 data item */
static t_Retc proc_i017_240 (t_Ui16 length, t_Byte *buffer);
                   /* Process I017/240 data item */
static t_Retc proc_i017_350 (t_Ui16 length, t_Byte *buffer);
                   /* Process I017/350 data item */
static t_Retc proc_i017_360 (t_Ui16 length, t_Byte *buffer);
                   /* Process I017/360 data item */
static t_Retc proc_i017_spf (t_Ui16 length, t_Byte *buffer, t_Ui16 *pos_ptr);
                   /* Process SPF indicator data item */

/*----------------------------------------------------------------------------*/
/* astx_017        -- Process ASTERIX category 017 data block                 */
/*----------------------------------------------------------------------------*/

 t_Retc astx_017 (t_Ui16 length, t_Byte *buffer)
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

					lrc = data_item (17, frn, std_uap[frn],
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
                   /* Description of data item I017/000: */
	desc_i017_000.category = 17;
	desc_i017_000.data_item = 0;
	desc_i017_000.item_type = e_fixed_length_data_item;
	desc_i017_000.fixed_length = 1;
	desc_i017_000.proc_fptr = proc_i017_000;
	desc_i017_000.read_fptr = NULL;

                   /* Description of data item I017/010: */
	desc_i017_010.category = 17;
	desc_i017_010.data_item = 10;
	desc_i017_010.item_type = e_fixed_length_data_item;
	desc_i017_010.fixed_length = 2;
	desc_i017_010.proc_fptr = proc_i017_010;
	desc_i017_010.read_fptr = NULL;

                   /* Description of data item I017/012: */
	desc_i017_012.category = 17;
	desc_i017_012.data_item = 12;
	desc_i017_012.item_type = e_fixed_length_data_item;
	desc_i017_012.fixed_length = 2;
	desc_i017_012.proc_fptr = proc_i017_012;
	desc_i017_012.read_fptr = NULL;

                   /* Description of data item I017/045: */
	desc_i017_045.category = 17;
	desc_i017_045.data_item = 45;
	desc_i017_045.item_type = e_fixed_length_data_item;
	desc_i017_045.fixed_length = 6;
	desc_i017_045.proc_fptr = proc_i017_045;
	desc_i017_045.read_fptr = NULL;

                   /* Description of data item I017/050: */
	desc_i017_050.category = 17;
	desc_i017_050.data_item = 50;
	desc_i017_050.item_type = e_fixed_length_data_item;
	desc_i017_050.fixed_length = 2;
	desc_i017_050.proc_fptr = proc_i017_050;
	desc_i017_050.read_fptr = NULL;

                   /* Description of data item I017/070: */
	desc_i017_070.category = 17;
	desc_i017_070.data_item = 70;
	desc_i017_070.item_type = e_fixed_length_data_item;
	desc_i017_070.fixed_length = 2;
	desc_i017_070.proc_fptr = proc_i017_070;
	desc_i017_070.read_fptr = NULL;

                   /* Description of data item I017/140: */
	desc_i017_140.category = 17;
	desc_i017_140.data_item = 140;
	desc_i017_140.item_type = e_fixed_length_data_item;
	desc_i017_140.fixed_length = 3;
	desc_i017_140.proc_fptr = proc_i017_140;
	desc_i017_140.read_fptr = NULL;

                   /* Description of data item I017/200: */
	desc_i017_200.category = 17;
	desc_i017_200.data_item = 200;
	desc_i017_200.item_type = e_fixed_length_data_item;
	desc_i017_200.fixed_length = 4;
	desc_i017_200.proc_fptr = proc_i017_200;
	desc_i017_200.read_fptr = NULL;

                   /* Description of data item I017/210: */
	desc_i017_210.category = 17;
	desc_i017_210.data_item = 210;
	desc_i017_210.item_type = e_repetitive_data_item;
	desc_i017_210.fixed_length = 3;
	desc_i017_210.proc_fptr = proc_i017_210;
	desc_i017_210.read_fptr = NULL;

                   /* Description of data item I017/220: */
	desc_i017_220.category = 17;
	desc_i017_220.data_item = 220;
	desc_i017_220.item_type = e_fixed_length_data_item;
	desc_i017_220.fixed_length = 3;
	desc_i017_220.proc_fptr = proc_i017_220;
	desc_i017_220.read_fptr = NULL;

                   /* Description of data item I017/221: */
	desc_i017_221.category = 17;
	desc_i017_221.data_item = 221;
	desc_i017_221.item_type = e_fixed_length_data_item;
	desc_i017_221.fixed_length = 2;
	desc_i017_221.proc_fptr = proc_i017_221;
	desc_i017_221.read_fptr = NULL;

                   /* Description of data item I017/230: */
	desc_i017_230.category = 17;
	desc_i017_230.data_item = 230;
	desc_i017_230.item_type = e_fixed_length_data_item;
	desc_i017_230.fixed_length = 1;
	desc_i017_230.proc_fptr = proc_i017_230;
	desc_i017_230.read_fptr = NULL;

                   /* Description of data item I017/240: */
	desc_i017_240.category = 17;
	desc_i017_240.data_item = 240;
	desc_i017_240.item_type = e_fixed_length_data_item;
	desc_i017_240.fixed_length = 1;
	desc_i017_240.proc_fptr = proc_i017_240;
	desc_i017_240.read_fptr = NULL;

                   /* Description of data item I017/350: */
	desc_i017_350.category = 17;
	desc_i017_350.data_item = 350;
	desc_i017_350.item_type = e_repetitive_data_item;
	desc_i017_350.fixed_length = 2;
	desc_i017_350.proc_fptr = proc_i017_350;
	desc_i017_350.read_fptr = NULL;

                   /* Description of data item I017/360: */
	desc_i017_360.category = 17;
	desc_i017_360.data_item = 360;
	desc_i017_360.item_type = e_fixed_length_data_item;
	desc_i017_360.fixed_length = 1;
	desc_i017_360.proc_fptr = proc_i017_360;
	desc_i017_360.read_fptr = NULL;

                   /* Description of SPF data item: */
	desc_i017_spf.category = 17;
	desc_i017_spf.data_item = M_SPF_INDICATOR;
	desc_i017_spf.item_type = e_immediate_data_item;
	desc_i017_spf.fixed_length = 0;
	desc_i017_spf.proc_fptr = NULL;
	desc_i017_spf.read_fptr = proc_i017_spf;

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

	std_uap[1] = &desc_i017_010;
	std_uap[2] = &desc_i017_012;
	std_uap[3] = &desc_i017_000;
	std_uap[4] = &desc_i017_350;
	std_uap[5] = &desc_i017_220;
	std_uap[6] = &desc_i017_221;
	std_uap[7] = &desc_i017_140;

	std_uap[8] = &desc_i017_045;
	std_uap[9] = &desc_i017_070;
	std_uap[10] = &desc_i017_050;
	std_uap[11] = &desc_i017_200;
	std_uap[12] = &desc_i017_230;
	std_uap[13] = &desc_i017_240;
	std_uap[14] = &desc_i017_210;

	std_uap[15] = &desc_i017_360;
	std_uap[21] = &desc_i017_spf;

	return;
}

/*----------------------------------------------------------------------------*/
/* proc_i017_000   -- Process I017/000 data item                              */
/*----------------------------------------------------------------------------*/

 static
 t_Retc proc_i017_000 (t_Ui16 length, t_Byte *buffer)
{
#if LISTER
	t_Byte df1;    /* Data field octet 1 */
#endif /* LISTER */
	t_Retc ret;    /* Return code */
#if LISTER
	t_Byte type;   /* Message type */
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
	type = df1;

				   /* List data field: */
	list_text (2, ";  Message Type:");
	switch (type)
	{
	case 0:
		list_text (2, " Network information");
		break;
	case 10:
		list_text (2, " Track data");
		break;
	case 20:
		list_text (2, " Track data request");
		break;
	case 21:
		list_text (2, " Track data stop");
		break;
	case 22:
		list_text (2, " Cancel track data request");
		break;
	case 23:
		list_text (2, " Track data stop acknowledgement");
		break;
	case 30:
		list_text (2, " New Node / Change-over Initial or"
                      " intermediate message segment");
		break;
	case 31:
		list_text (2, " New Node / Change-over Final or"
                      " only message segment");
		break;
	case 32:
		list_text (2, " New Node / Change-over Initial or"
                      " intermediate message segment reply");
		break;
	case 33:
		list_text (2, " New Node / Change-over Final or"
                      " only message segment reply");
		break;
	case 110:
		list_text (2, " Move node to new cluster state");
		break;
	case 111:
		list_text (2, " Move node to new cluster state"
                      " acknowledgement");
		break;
	default:
		list_text (2, " unknown (" M_FMT_BYTE ")", type);
		break;
	}
	list_text (2, "\n");
#endif /* LISTER */

				   /* Set the return code: */
	ret = RC_OKAY;

	return ret;
}

/*----------------------------------------------------------------------------*/
/* proc_i017_010   -- Process I017/010 data item                              */
/*----------------------------------------------------------------------------*/

 static
 t_Retc proc_i017_010 (t_Ui16 length, t_Byte *buffer)
{
#if LISTER
	t_Byte df1;    /* Data field octet 1 */
	t_Byte df2;    /* Data field octet 2 */
	t_Ui16 dsi;    /* Data source identifier */
#endif /* LISTER */
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

#if LISTER
				   /* Extract octets: */
	df1 = buffer[0];
	df2 = buffer[1];

				   /* Extract information: */
	sac = df1;
	sic = df2;
	dsi = make_ui16 (df1, df2);
 
                   /* List data field: */
	list_text (2, ";  Data Source Identifier:");
	list_text (2, " 0x" M_FMT_UI16_HEX, dsi);
	list_text (2, " (SAC=" M_FMT_BYTE "; SIC=" M_FMT_BYTE ")", sac, sic);
	list_text (2, "\n");
#endif /* LISTER */

				   /* Set the return code: */
	ret = RC_OKAY;

	return ret;
}

/*----------------------------------------------------------------------------*/
/* proc_i017_012   -- Process I017/012 data item                              */
/*----------------------------------------------------------------------------*/

 static
 t_Retc proc_i017_012 (t_Ui16 length, t_Byte *buffer)
{
#if LISTER
	t_Ui16 ddi;    /* Data destination identifier */
	t_Byte df1;    /* Data field octet 1 */
	t_Byte df2;    /* Data field octet 2 */
#endif /* LISTER */
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

#if LISTER
				   /* Extract octets: */
	df1 = buffer[0];
	df2 = buffer[1];

				   /* Extract information: */
	sac = df1;
	sic = df2;
	ddi = make_ui16 (df1, df2);
 
                   /* List data field: */
	list_text (2, ";  Data Destination Identifier:");
	list_text (2, " 0x" M_FMT_UI16_HEX, ddi);
	list_text (2, " (SAC=" M_FMT_BYTE "; SIC=" M_FMT_BYTE ")", sac, sic);
	list_text (2, "\n");
#endif /* LISTER */

				   /* Set the return code: */
	ret = RC_OKAY;

	return ret;
}

/*----------------------------------------------------------------------------*/
/* proc_i017_045   -- Process I017/045 data item                              */
/*----------------------------------------------------------------------------*/

 static
 t_Retc proc_i017_045 (t_Ui16 length, t_Byte *buffer)
{
#if LISTER
	t_Byte df1;    /* Data field octet 1 */
	t_Byte df2;    /* Data field octet 2 */
	t_Byte df3;    /* Data field octet 3 */
	t_Byte df4;    /* Data field octet 4 */
	t_Byte df5;    /* Data field octet 5 */
	t_Byte df6;    /* Data field octet 6 */
	t_Si32 lat;    /* Latitude; 180/2**23 degrees */
	t_Si32 lon;    /* Longitude; 180/2**23 degrees */
#endif /* LISTER */
	t_Retc ret;    /* Return code */

				   /* Preset the return code: */
	ret = RC_FAIL;

				   /* Check parameters: */
	Assert (length == 6, "Invalid parameter");
	Assert (buffer != NULL, "Invalid parameter");

#if LISTER
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

				   /* List data field: */
	list_text (2, ";  Calculated Position (WGS-84):");
	list_text (2, " lat=%10.6f; lon=%11.6f deg",
                  (180.0 / M_TWO_POWER_23) * lat,
                  (180.0 / M_TWO_POWER_23) * lon);
	list_text (2, "\n");
#endif /* LISTER */

				   /* Set the return code: */
	ret = RC_OKAY;

	return ret;
}

/*----------------------------------------------------------------------------*/
/* proc_i017_050   -- Process I017/050 data item                              */
/*----------------------------------------------------------------------------*/

 static
 t_Retc proc_i017_050 (t_Ui16 length, t_Byte *buffer)
{
#if LISTER
	t_Si16 alt;    /* Altitude; 25 feet */
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
	alt = make_si16 (df1 & 0x3f, df2);

				   /* List data field: */
	list_text (2, ";  Flight Level:");
	list_text (2, " v=" M_FMT_BYTE ";", (df1 >> 7) & 0x01);
	list_text (2, " g=" M_FMT_BYTE ";", (df1 >> 6) & 0x01);
	list_text (2, " alt=" M_FMT_SI16 " (FL=%.2f)", alt, 0.25 * alt);
	list_text (2, "\n");
#endif /* LISTER */

				   /* Set the return code: */
	ret = RC_OKAY;

	return ret;
}

/*----------------------------------------------------------------------------*/
/* proc_i017_070   -- Process I017/070 data item                              */
/*----------------------------------------------------------------------------*/

 static
 t_Retc proc_i017_070 (t_Ui16 length, t_Byte *buffer)
{
#if LISTER
	t_Byte df1;    /* Data field octet 1 */
	t_Byte df2;    /* Data field octet 2 */
	t_Ui16 m3c;    /* Mode 3/A code */
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
	m3c = make_ui16 (df1 & 0x0f, df2);

				   /* List data field: */
	list_text (2, ";  Mode 3/A Code:");
	list_text (2, " v=" M_FMT_BYTE ";", (df1 >> 7) & 0x01);
	list_text (2, " g=" M_FMT_BYTE ";", (df1 >> 6) & 0x01);
	list_text (2, " l=" M_FMT_BYTE ";", (df1 >> 5) & 0x01);
	list_text (2, " code=" M_FMT_CODE, m3c);
	list_text (2, "\n");
#endif /* LISTER */

				   /* Set the return code: */
	ret = RC_OKAY;

	return ret;
}

/*----------------------------------------------------------------------------*/
/* proc_i017_140   -- Process I017/140 data item                              */
/*----------------------------------------------------------------------------*/

 static
 t_Retc proc_i017_140 (t_Ui16 length, t_Byte *buffer)
{
#if LISTER
	t_Byte df1;    /* Data field octet 1 */
	t_Byte df2;    /* Data field octet 2 */
	t_Byte df3;    /* Data field octet 3 */
#endif /* LISTER */
	t_Retc ret;    /* Return code */
#if LISTER
	t_Secs tm;     /* Time of day; seconds */
	t_Ui32 tod;    /* Time of day; 1/128 seconds */
#endif /* LISTER */

				   /* Preset the return code: */
	ret = RC_FAIL;

				   /* Check parameters: */
	Assert (length == 3, "Invalid parameter");
	Assert (buffer != NULL, "Invalid parameter");

#if LISTER
				   /* Extract octets: */
	df1 = buffer[0];
	df2 = buffer[1];
	df3 = buffer[2];

				   /* Extract information: */
	tod = make_ui32 (0x00, df1, df2, df3);

				   /* Compute time of day: */
	tm = (1.0 / 128.0) * tod;

				   /* List data field: */
	list_text (2, ";  Time of Day:");
	list_text (2, " " M_FMT_ATOD " (" M_FMT_UI32 "; %s UTC)",
                  tod, tod, utc_text (tm));
	list_text (2, "\n");
#endif /* LISTER */

				   /* Set the return code: */
	ret = RC_OKAY;

	return ret;
}

/*----------------------------------------------------------------------------*/
/* proc_i017_200   -- Process I017/200 data item                              */
/*----------------------------------------------------------------------------*/

 static
 t_Retc proc_i017_200 (t_Ui16 length, t_Byte *buffer)
{
#if LISTER
	t_Byte df1;    /* Data field octet 1 */
	t_Byte df2;    /* Data field octet 2 */
	t_Byte df3;    /* Data field octet 3 */
	t_Byte df4;    /* Data field octet 4 */
	t_Ui16 hdg;    /* Claculated heading; 360/2**16 degrees */
#endif /* LISTER */
	t_Retc ret;    /* Return code */
#if LISTER
	t_Ui16 spd;    /* Calculated groundspeed; 2**-14 nmi/sec */
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
	spd = make_ui16 (df1, df2);
	hdg = make_ui16 (df3, df4);

				   /* List data field: */
	list_text (2, ";  Track Velocity:");
	list_text (2, " spd=" M_FMT_UI16 " (%.3f kts)",
                  spd, (3600.0 / 16384.0) * spd);
	list_text (2, " hdg=" M_FMT_UI16 " (%.3f deg)",
                  hdg, (360.0 / 65536.0) * hdg);
	list_text (2, "\n");
#endif /* LISTER */

				   /* Set the return code: */
	ret = RC_OKAY;

	return ret;
}

/*----------------------------------------------------------------------------*/
/* proc_i017_210   -- Process I017/210 data item                              */
/*----------------------------------------------------------------------------*/

 static
 t_Retc proc_i017_210 (t_Ui16 length, t_Byte *buffer)
{
#if LISTER
	t_Ui32 aa;     /* Aircraft address */
#endif /* LISTER */
	t_Byte df1;    /* Data field octet 1 */
#if LISTER
	t_Byte df2;    /* Data field octet 2 */
	t_Byte df3;    /* Data field octet 3 */
	int ix;        /* Auxiliary */
#endif /* LISTER */
	t_Byte rep;    /* Repetition factor */
	t_Retc ret;    /* Return code */

				   /* Preset the return code: */
	ret = RC_FAIL;

				   /* Check parameters: */
	Assert (length > 0, "Invalid parameter");
	Assert (buffer != NULL, "Invalid parameter");

				   /* Extract first octet: */
	df1 = buffer[0];

                   /* This is the repetition factor: */
	rep = df1;
                   /* May legally be zero */

				   /* Check against buffer length: */
	Assert (length == 1 + rep * 3, "Invalid length.");

#if LISTER
				   /* List data field: */
	list_text (2, ";  Mode S Address List:");
	if (rep == 0)
	{
		list_text (2, " <none>");
	}
	else
	{
		for (ix = 0; ix < rep; ix ++)
		{
			df1 = buffer[1 + ix * 3];
			df2 = buffer[1 + ix * 3 + 1];
			df3 = buffer[1 + ix * 3 + 2];

			aa = make_ui32 (0x00, df1, df2, df3);

			list_text (2, " " M_FMT_ADDR, aa);
		}
	}
	list_text (2, "\n");
#endif /* LISTER */

				   /* Set the return code: */
	ret = RC_OKAY;

	return ret;
}

/*----------------------------------------------------------------------------*/
/* proc_i017_220   -- Process I017/220 data item                              */
/*----------------------------------------------------------------------------*/

 static
 t_Retc proc_i017_220 (t_Ui16 length, t_Byte *buffer)
{
#if LISTER
	t_Ui32 aa;     /* Aircraft address */
	t_Byte df1;    /* Data field octet 1 */
	t_Byte df2;    /* Data field octet 2 */
	t_Byte df3;    /* Data field octet 3 */
#endif /* LISTER */
	t_Retc ret;    /* Return code */

				   /* Preset the return code: */
	ret = RC_FAIL;

				   /* Check parameters: */
	Assert (length == 3, "Invalid parameter");
	Assert (buffer != NULL, "Invalid parameter");

#if LISTER
				   /* Extract octets: */
	df1 = buffer[0];
	df2 = buffer[1];
	df3 = buffer[2];

				   /* Extract information: */
	aa = make_ui32 (0x00, df1, df2, df3);

				   /* List data field: */
	list_text (2, ";  Aircraft Address:");
	list_text (2, " " M_FMT_ADDR " (" M_FMT_UI32 ")", aa, aa);
	list_text (2, "\n");
#endif /* LISTER */

				   /* Set the return code: */
	ret = RC_OKAY;

	return ret;
}

/*----------------------------------------------------------------------------*/
/* proc_i017_221   -- Process I017/221 data item                              */
/*----------------------------------------------------------------------------*/

 static
 t_Retc proc_i017_221 (t_Ui16 length, t_Byte *buffer)
{
#if LISTER
	t_Byte df1;    /* Data field octet 1 */
	t_Byte df2;    /* Data field octet 2 */
	t_Ui16 drn;    /* Duplicate address reference number */
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
	drn = make_ui16 (df1, df2);

				   /* List data field: */
	list_text (2, ";  Duplicate Address Reference Number:");
	list_text (2, " " M_FMT_UI16, drn);
	list_text (2, "\n");
#endif /* LISTER */

				   /* Set the return code: */
	ret = RC_OKAY;

	return ret;
}

/*----------------------------------------------------------------------------*/
/* proc_i017_230   -- Process I017/230 data item                              */
/*----------------------------------------------------------------------------*/

 static
 t_Retc proc_i017_230 (t_Ui16 length, t_Byte *buffer)
{
#if LISTER
	t_Byte ca;     /* Transponder capability */
	t_Byte df1;    /* Data field octet 1 */
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
	ca = (df1 >> 5) & 0x07;

				   /* List data field: */
	list_text (2, ";  Transponder Capability:");
	list_text (2, " " M_FMT_BYTE, ca);
	list_text (2, "\n");
#endif /* LISTER */

				   /* Set the return code: */
	ret = RC_OKAY;

	return ret;
}

/*----------------------------------------------------------------------------*/
/* proc_i017_240   -- Process I017/240 data item                              */
/*----------------------------------------------------------------------------*/

 static
 t_Retc proc_i017_240 (t_Ui16 length, t_Byte *buffer)
{
#if LISTER
	t_Byte cst;    /* Track coasted */
	t_Byte df1;    /* Data field octet 1 */
	t_Byte flt;    /* Flight level tracking */
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
	cst = (df1 >> 7) & 0x01;
	flt = (df1 >> 6) & 0x01;

				   /* List data field: */
	list_text (2, ";  Track Status:");
	list_text (2, " cst=" M_FMT_BYTE ";", cst);
	list_text (2, " flt=" M_FMT_BYTE, flt);
	list_text (2, "\n");
#endif /* LISTER */

				   /* Set the return code: */
	ret = RC_OKAY;

	return ret;
}

/*----------------------------------------------------------------------------*/
/* proc_i017_350   -- Process I017/350 data item                              */
/*----------------------------------------------------------------------------*/

 static
 t_Retc proc_i017_350 (t_Ui16 length, t_Byte *buffer)
{
	t_Byte df1;    /* Data field octet 1 */
#if LISTER
	t_Byte df2;    /* Data field octet 2 */
	int ix;        /* Auxiliary */
#endif /* LISTER */
	t_Byte rep;    /* Repetition factor */
	t_Retc ret;    /* Return code */
#if LISTER
	t_Byte sac;    /* Source area code */
	t_Byte sic;    /* Source identification code */
#endif /* LISTER */

				   /* Preset the return code: */
	ret = RC_FAIL;

				   /* Check parameters: */
	Assert (length > 0, "Invalid parameter");
	Assert (buffer != NULL, "Invalid parameter");

				   /* Extract first octet: */
	df1 = buffer[0];

                   /* This is the repetition factor: */
	rep = df1;

				   /* Check against buffer length: */
	Assert (length == 1 + rep * 2, "Invalid length");

#if LISTER
				   /* List data field: */
	list_text (2, ";  Cluster Station/Node List:");
	list_text (2, "\n");
	for (ix = 0; ix < rep; ix ++)
	{
		df1 = buffer[1 + ix * 2];
		df2 = buffer[1 + ix * 2 + 1];

		sac = df1;
		sic = df2;

		list_text (2, ";   %d:", 1 + ix);
		list_text (2, " SAC=" M_FMT_BYTE "; SIC=" M_FMT_BYTE, sac, sic);
		list_text (2, "\n");
	}
#endif /* LISTER */

				   /* Set the return code: */
	ret = RC_OKAY;

	return ret;
}

/*----------------------------------------------------------------------------*/
/* proc_i017_360   -- Process I017/360 data item                              */
/*----------------------------------------------------------------------------*/

 static
 t_Retc proc_i017_360 (t_Ui16 length, t_Byte *buffer)
{
#if LISTER
	t_Byte df1;    /* Data field octet 1 */
#endif /* LISTER */
	t_Retc ret;    /* Return code */
#if LISTER
	t_Byte state;  /* Cluster controller command state */
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
	state = df1;

				   /* List data field: */
	list_text (2, ";  Cluster Controller Command State:");
	list_text (2, " " M_FMT_BYTE, state);
	list_text (2, "\n");
#endif /* LISTER */

				   /* Set the return code: */
	ret = RC_OKAY;

	return ret;
}

/*----------------------------------------------------------------------------*/
/* proc_i017_spf   -- Process SPF indicator data item                         */
/*----------------------------------------------------------------------------*/

 static
 t_Retc proc_i017_spf (t_Ui16 length, t_Byte *buffer, t_Ui16 *pos_ptr)
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
	list_buffer (1, ";  I017/SPF:", len, buffer + pos);
#endif /* LISTER */

				   /* Set position: */
	*pos_ptr = pos + len;

				   /* Set the return code: */
	ret = RC_OKAY;

	done:          /* We are done */
	return ret;
}
/* end-of-file */
