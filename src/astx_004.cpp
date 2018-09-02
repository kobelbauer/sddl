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
/* File:           src/astx_004.cpp                                           */
/* Contents:       Process ASTERIX category 004 data block                    */
/* Author(s):      kb                                                         */
/* Last change:    2018-04-27                                                 */
/*----------------------------------------------------------------------------*/

/* Reference document:
   -------------------

   Eurocontrol Standard Document for Surveillance Data Exchange.
   Part 17: Category 4
   Safety Net Messages
   SUR.ET1.ST05.2000-STD-17-02
   Edition: 1.2
   Edition Date: March 2007
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

#define M_MAX_FRN 21
				   /* Maximum field reference number */
#define M_MAX_FSPEC_LENGTH 3
				   /* Maximum fields specification length
					  for ASTERIX category 004 */

                   /* Local texts: */
                   /* ------------ */

#if LISTER
static const char *message_type_texts[16] =
{
	"Alive Message",
	"Route Adherence Monitor Longitudinal Deviation",
	"Route Adherence Monitor Heading Deviation",
	"Minimum Safe Altitude Warning",
	"Area Proximity Warning",
	"Clearance Level Adherence Monitor",
	"Short Term Conflict Alert",
	"Approach Funnel Deviation Alert",
	"RIMCAS Arrival / Landing Monitor (ALM)"
	"RIMCAS Arrival / Departure Wrong Runway Alert (WRA)",
	"RIMCAS Arrival / Departure Opposite Traffic Alert (OTA)",
	"RIMCAS Departure Monitor (RDM)",
	"RIMCAS Runway / Taxiway Crossing Monitor (RCM)",
	"RIMCAS Taxiway Separation Monitor (RSM)",
	"RIMCAS Unauthorized Taxiway Movement Monitor (UTMM)",
	"RIMCAS Stop Bar Overrun Alert (SBOA)",
	"End of Conflict (EOC)"
};
#endif /* LISTER */

				   /* Local variables: */
				   /* ---------------- */

static t_Data_Item_Desc desc_i004_000;
                   /* Description of data item I004/000 */
static t_Data_Item_Desc desc_i004_010;
                   /* Description of data item I004/010 */
static t_Data_Item_Desc desc_i004_015;
                   /* Description of data item I004/015 */
static t_Data_Item_Desc desc_i004_020;
                   /* Description of data item I004/020 */
static t_Data_Item_Desc desc_i004_030;
                   /* Description of data item I004/030 */
static t_Data_Item_Desc desc_i004_035;
                   /* Description of data item I004/035 */
static t_Data_Item_Desc desc_i004_040;
                   /* Description of data item I004/040 */
static t_Data_Item_Desc desc_i004_045;
                   /* Description of data item I004/045 */
static t_Data_Item_Desc desc_i004_060;
                   /* Description of data item I004/060 */
static t_Data_Item_Desc desc_i004_070;
                   /* Description of data item I004/070 */
static t_Data_Item_Desc desc_i004_074;
                   /* Description of data item I004/074 */
static t_Data_Item_Desc desc_i004_075;
                   /* Description of data item I004/075 */
static t_Data_Item_Desc desc_i004_076;
                   /* Description of data item I004/076 */
static t_Data_Item_Desc desc_i004_100;
                   /* Description of data item I004/100 */
static t_Data_Item_Desc desc_i004_110;
                   /* Description of data item I004/110 */
static t_Data_Item_Desc desc_i004_120;
                   /* Description of data item I004/120 */
static t_Data_Item_Desc desc_i004_170;
                   /* Description of data item I004/170 */
static t_Data_Item_Desc desc_i004_171;
                   /* Description of data item I004/171 */
static t_Data_Item_Desc desc_i004_ref;
                   /* Description of REF data item */
static t_Data_Item_Desc desc_i004_spf;
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
                   /* Load standard User Application Profile (UAP) */
static t_Retc proc_i004_000 (t_Ui16 length, t_Byte *buffer);
                   /* Process I004/000 data item */
static t_Retc proc_i004_010 (t_Ui16 length, t_Byte *buffer);
                   /* Process I004/010 data item */
static t_Retc proc_i004_015 (t_Ui16 length, t_Byte *buffer);
                   /* Process I004/015 data item */
static t_Retc proc_i004_020 (t_Ui16 length, t_Byte *buffer);
                   /* Process I004/020 data item */
static t_Retc proc_i004_030 (t_Ui16 length, t_Byte *buffer);
                   /* Process I004/030 data item */
static t_Retc proc_i004_035 (t_Ui16 length, t_Byte *buffer);
                   /* Process I004/035 data item */
static t_Retc proc_i004_040 (t_Ui16 length, t_Byte *buffer);
                   /* Process I004/040 data item */
static t_Retc proc_i004_045 (t_Ui16 length, t_Byte *buffer);
                   /* Process I004/045 data item */
static t_Retc proc_i004_060 (t_Ui16 length, t_Byte *buffer);
                   /* Process I004/060 data item */
static t_Retc proc_i004_070 (t_Ui16 length, t_Byte *buffer, t_Ui16 *pos_ptr);
                   /* Process I004/070 data item */
static t_Retc proc_i004_074 (t_Ui16 length, t_Byte *buffer);
                   /* Process I004/074 data item */
static t_Retc proc_i004_075 (t_Ui16 length, t_Byte *buffer);
                   /* Process I004/075 data item */
static t_Retc proc_i004_076 (t_Ui16 length, t_Byte *buffer);
                   /* Process I004/076 data item */
static t_Retc proc_i004_100 (t_Ui16 length, t_Byte *buffer, t_Ui16 *pos_ptr);
                   /* Process I004/100 data item */
static t_Retc proc_i004_110 (t_Ui16 length, t_Byte *buffer);
                   /* Process I004/110 data item */
static t_Retc proc_i004_120 (t_Ui16 length, t_Byte *buffer, t_Ui16 *pos_ptr);
                   /* Process I004/120 data item */
static t_Retc proc_i004_170 (t_Ui16 length, t_Byte *buffer, t_Ui16 *pos_ptr);
                   /* Process I004/170 data item */
static t_Retc proc_i004_171 (t_Ui16 length, t_Byte *buffer, t_Ui16 *pos_ptr);
                   /* Process I004/171 data item */
static t_Retc proc_i004_ref (t_Ui16 length, t_Byte *buffer, t_Ui16 *pos_ptr);
                   /* Process REF indicator data item */
static t_Retc proc_i004_spf (t_Ui16 length, t_Byte *buffer, t_Ui16 *pos_ptr);
                   /* Process SPF indicator data item */

/*----------------------------------------------------------------------------*/
/* astx_004        -- Process ASTERIX category 003 data block                 */
/*----------------------------------------------------------------------------*/

 t_Retc astx_004 (t_Ui16 length, t_Byte *buffer)
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
		list_text (1, "; FSPEC: =0x");
		for (ix = 0; ix < fspec_length; ix ++)
		{
			list_text (1, " " M_FMT_BYTE_HEX, fspec_buffer[ix]);
		}
		list_text (1, "\n");
		list_text (2, "; Data Record:\n");
#endif /* LISTER */

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

					lrc = data_item (4, frn, std_uap[frn],
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
                   /* Description of data item I004/000: */
    desc_i004_000.category = 4;
    desc_i004_000.data_item = 0;
    desc_i004_000.item_type = e_fixed_length_data_item;
    desc_i004_000.fixed_length = 1;
    desc_i004_000.proc_fptr = proc_i004_000;
    desc_i004_000.read_fptr = NULL;

                   /* Description of data item I004/010: */
    desc_i004_010.category = 4;
    desc_i004_010.data_item = 10;
    desc_i004_010.item_type = e_fixed_length_data_item;
    desc_i004_010.fixed_length = 2;
    desc_i004_010.proc_fptr = proc_i004_010;
    desc_i004_010.read_fptr = NULL;

                   /* Description of data item I004/015: */
    desc_i004_015.category = 4;
    desc_i004_015.data_item = 15;
    desc_i004_015.item_type = e_repetitive_data_item;
    desc_i004_015.fixed_length = 2;
    desc_i004_015.proc_fptr = proc_i004_015;
    desc_i004_015.read_fptr = NULL;

                   /* Description of data item I004/020: */
    desc_i004_020.category = 4;
    desc_i004_020.data_item = 20;
    desc_i004_020.item_type = e_fixed_length_data_item;
    desc_i004_020.fixed_length = 3;
    desc_i004_020.proc_fptr = proc_i004_020;
    desc_i004_020.read_fptr = NULL;

                   /* Description of data item I004/030: */
    desc_i004_030.category = 4;
    desc_i004_030.data_item = 30;
    desc_i004_030.item_type = e_fixed_length_data_item;
    desc_i004_030.fixed_length = 2;
    desc_i004_030.proc_fptr = proc_i004_030;
    desc_i004_030.read_fptr = NULL;

                   /* Description of data item I004/035: */
    desc_i004_035.category = 4;
    desc_i004_035.data_item = 35;
    desc_i004_035.item_type = e_fixed_length_data_item;
    desc_i004_035.fixed_length = 2;
    desc_i004_035.proc_fptr = proc_i004_035;
    desc_i004_035.read_fptr = NULL;

                   /* Description of data item I004/040: */
    desc_i004_040.category = 4;
    desc_i004_040.data_item = 40;
    desc_i004_040.item_type = e_fixed_length_data_item;
    desc_i004_040.fixed_length = 2;
    desc_i004_040.proc_fptr = proc_i004_040;
    desc_i004_040.read_fptr = NULL;

                   /* Description of data item I004/045: */
    desc_i004_045.category = 4;
    desc_i004_045.data_item = 45;
    desc_i004_045.item_type = e_fixed_length_data_item;
    desc_i004_045.fixed_length = 1;
    desc_i004_045.proc_fptr = proc_i004_045;
    desc_i004_045.read_fptr = NULL;

                   /* Description of data item I004/060: */
    desc_i004_060.category = 4;
    desc_i004_060.data_item = 60;
    desc_i004_060.item_type = e_variable_length_data_item;
    desc_i004_060.fixed_length = 0;
    desc_i004_060.proc_fptr = proc_i004_060;
    desc_i004_060.read_fptr = NULL;

                   /* Description of data item I004/070: */
    desc_i004_070.category = 4;
    desc_i004_070.data_item = 70;
    desc_i004_070.item_type = e_immediate_data_item;
    desc_i004_070.fixed_length = 0;
    desc_i004_070.proc_fptr = NULL;
    desc_i004_070.read_fptr = proc_i004_070;

                   /* Description of data item I004/074: */
    desc_i004_074.category = 4;
    desc_i004_074.data_item = 74;
    desc_i004_074.item_type = e_fixed_length_data_item;
    desc_i004_074.fixed_length = 2;
    desc_i004_074.proc_fptr = proc_i004_074;
    desc_i004_074.read_fptr = NULL;

                   /* Description of data item I004/075: */
    desc_i004_075.category = 4;
    desc_i004_075.data_item = 75;
    desc_i004_075.item_type = e_fixed_length_data_item;
    desc_i004_075.fixed_length = 3;
    desc_i004_075.proc_fptr = proc_i004_075;
    desc_i004_075.read_fptr = NULL;

                   /* Description of data item I004/076: */
    desc_i004_076.category = 4;
    desc_i004_076.data_item = 76;
    desc_i004_076.item_type = e_fixed_length_data_item;
    desc_i004_076.fixed_length = 2;
    desc_i004_076.proc_fptr = proc_i004_076;
    desc_i004_076.read_fptr = NULL;

                   /* Description of data item I004/100: */
    desc_i004_100.category = 4;
    desc_i004_100.data_item = 100;
    desc_i004_100.item_type = e_immediate_data_item;
    desc_i004_100.fixed_length = 0;
    desc_i004_100.proc_fptr = NULL;
    desc_i004_100.read_fptr = proc_i004_100;

                   /* Description of data item I004/110: */
    desc_i004_110.category = 4;
    desc_i004_110.data_item = 110;
    desc_i004_110.item_type = e_repetitive_data_item;
    desc_i004_110.fixed_length = 2;
    desc_i004_110.proc_fptr = proc_i004_110;
    desc_i004_110.read_fptr = NULL;

                   /* Description of data item I004/120: */
    desc_i004_120.category = 4;
    desc_i004_120.data_item = 120;
    desc_i004_120.item_type = e_immediate_data_item;
    desc_i004_120.fixed_length = 0;
    desc_i004_120.proc_fptr = NULL;
    desc_i004_120.read_fptr = proc_i004_120;

                   /* Description of data item I004/170: */
    desc_i004_170.category = 4;
    desc_i004_170.data_item = 170;
    desc_i004_170.item_type = e_immediate_data_item;
    desc_i004_170.fixed_length = 0;
    desc_i004_170.proc_fptr = NULL;
    desc_i004_170.read_fptr = proc_i004_170;

                   /* Description of data item I004/171: */
    desc_i004_171.category = 4;
    desc_i004_171.data_item = 171;
    desc_i004_171.item_type = e_immediate_data_item;
    desc_i004_171.fixed_length = 0;
    desc_i004_171.proc_fptr = NULL;
    desc_i004_171.read_fptr = proc_i004_171;

                   /* Description of RFS data item: */
    desc_i004_spf.category = 4;
    desc_i004_spf.data_item = M_REF_INDICATOR;
    desc_i004_spf.item_type = e_immediate_data_item;
    desc_i004_spf.fixed_length = 0;
    desc_i004_spf.proc_fptr = NULL;
    desc_i004_spf.read_fptr = proc_i004_ref;

                   /* Description of SPF data item: */
    desc_i004_spf.category = 4;
    desc_i004_spf.data_item = M_SPF_INDICATOR;
    desc_i004_spf.item_type = e_immediate_data_item;
    desc_i004_spf.fixed_length = 0;
    desc_i004_spf.proc_fptr = NULL;
    desc_i004_spf.read_fptr = proc_i004_spf;

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

	std_uap[1] = &desc_i004_010;
	std_uap[2] = &desc_i004_000;
	std_uap[3] = &desc_i004_015;
	std_uap[4] = &desc_i004_020;
	std_uap[5] = &desc_i004_040;
	std_uap[6] = &desc_i004_045;
	std_uap[7] = &desc_i004_060;

	std_uap[8] = &desc_i004_030;
	std_uap[9] = &desc_i004_170;
	std_uap[10] = &desc_i004_120;
	std_uap[11] = &desc_i004_070;
	std_uap[12] = &desc_i004_076;
	std_uap[13] = &desc_i004_074;
	std_uap[14] = &desc_i004_075;

	std_uap[15] = &desc_i004_100;
	std_uap[16] = &desc_i004_035;
	std_uap[17] = &desc_i004_171;
	std_uap[18] = &desc_i004_110;
	std_uap[20] = &desc_i004_ref;
	std_uap[21] = &desc_i004_spf;

	return;
}

/*----------------------------------------------------------------------------*/
/* proc_i004_000   -- Process I004/000 data item                              */
/*----------------------------------------------------------------------------*/

 static
 t_Retc proc_i004_000 (t_Ui16 length, t_Byte *buffer)
{
#if LISTER
	t_Byte df1;    /* Data field octet 1 */
	t_Byte mtp;    /* Message type */
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
	mtp = df1;

				   /* List data field: */
	list_text (2, ";  Message Type:");
	list_text (2, " " M_FMT_BYTE, mtp);
	if (1 <= mtp && mtp <= 17)
	{
		list_text (2, " (%s)", message_type_texts[mtp - 1]);
	}
	else if (mtp == 99)
	{
		list_text (2, " (AIW)");
	}
	list_text (2, "\n");
#endif /* LISTER */

				   /* Set the return code: */
	ret = RC_OKAY;

	return ret;
}

/*----------------------------------------------------------------------------*/
/* proc_i004_010   -- Process I004/010 data item                              */
/*----------------------------------------------------------------------------*/

 static
 t_Retc proc_i004_010 (t_Ui16 length, t_Byte *buffer)
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
/* proc_i004_015   -- Process I004/015 data item                              */
/*----------------------------------------------------------------------------*/

 static
 t_Retc proc_i004_015 (t_Ui16 length, t_Byte *buffer)
{
#if LISTER
	t_Byte df1;    /* Data field octet 1 */
	t_Byte df2;    /* Data field octet 2 */
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
	if (rep == 0)
	{
		ret = RC_SKIP;
		goto done;
	}

				   /* Check length (again): */
	Assert (length == 1 + rep * 2, "Invalid length");

#if LISTER
				   /* List data field: */
	list_text (2, ";  SDPS Identifier:\n");
	for (ix = 0; ix < rep; ix ++)
	{
		df1 = buffer[1 + 2 * ix];
		df2 = buffer[1 + 2 * ix + 1];

		list_text (2, ";   SAC=" M_FMT_BYTE "; SIC=" M_FMT_BYTE, df1, df2);
		list_text (2, "\n");
	}
#endif /* LISTER */

				   /* Set the return code: */
	ret = RC_OKAY;

	done:          /* We are done */
	return ret;
}

/*----------------------------------------------------------------------------*/
/* proc_i004_020   -- Process I004/020 data item                              */
/*----------------------------------------------------------------------------*/

 static
 t_Retc proc_i004_020 (t_Ui16 length, t_Byte *buffer)
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

                   /* Convert to milliseconds: */
	tm = (1.0 / 128.0) * tod;

				   /* List data field: */
	list_text (2, ";  Time of Day:");
	list_text (2, " " M_FMT_ATOD " (" M_FMT_UI32 "; %.6f secs; %s UTC)",
                  tod, tod, tm, utc_text (tm));
	list_text (2, "\n");
#endif /* LISTER */

				   /* Set the return code: */
	ret = RC_OKAY;

	return ret;
}

/*----------------------------------------------------------------------------*/
/* proc_i004_030   -- Process I004/030 data item                              */
/*----------------------------------------------------------------------------*/

 static
 t_Retc proc_i004_030 (t_Ui16 length, t_Byte *buffer)
{
#if LISTER
	t_Byte df1;    /* Data field octet 1 */
	t_Byte df2;    /* Data field octet 2 */
#endif /* LISTER */
	t_Retc ret;    /* Return code */
#if LISTER
	t_Ui16 tn1;    /* Track number 1 */
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
	tn1 = make_ui16 (df1, df2);

				   /* List data field: */
	list_text (2, ";  Track Number 1:");
	list_text (2, " " M_FMT_UI16, tn1);
	list_text (2, "\n");
#endif /* LISTER */

				   /* Set the return code: */
	ret = RC_OKAY;

	return ret;
}

/*----------------------------------------------------------------------------*/
/* proc_i004_035   -- Process I004/035 data item                              */
/*----------------------------------------------------------------------------*/

 static
 t_Retc proc_i004_035 (t_Ui16 length, t_Byte *buffer)
{
#if LISTER
	t_Byte df1;    /* Data field octet 1 */
	t_Byte df2;    /* Data field octet 2 */
#endif /* LISTER */
	t_Retc ret;    /* Return code */
#if LISTER
	t_Ui16 tn2;    /* Track number 2 */
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
	tn2 = make_ui16 (df1, df2);

				   /* List data field: */
	list_text (2, ";  Track Number 2:");
	list_text (2, " " M_FMT_UI16, tn2);
	list_text (2, "\n");
#endif /* LISTER */

				   /* Set the return code: */
	ret = RC_OKAY;

	return ret;
}

/*----------------------------------------------------------------------------*/
/* proc_i004_040   -- Process I004/040 data item                              */
/*----------------------------------------------------------------------------*/

 static
 t_Retc proc_i004_040 (t_Ui16 length, t_Byte *buffer)
{
#if LISTER
	t_Byte df1;    /* Data field octet 1 */
	t_Byte df2;    /* Data field octet 2 */
	t_Ui16 num;    /* Alert identifier */
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
	num = make_ui16 (df1, df2);

				   /* List data field: */
	list_text (2, ";  Alert Identifier:");
	list_text (2, " " M_FMT_UI16, num);
	list_text (2, "\n");
#endif /* LISTER */

				   /* Set the return code: */
	ret = RC_OKAY;

	return ret;
}

/*----------------------------------------------------------------------------*/
/* proc_i004_045   -- Process I004/045 data item                              */
/*----------------------------------------------------------------------------*/

 static
 t_Retc proc_i004_045 (t_Ui16 length, t_Byte *buffer)
{
#if LISTER
	t_Byte df1;    /* Data field octet 1 */
#endif /* LISTER */
	t_Retc ret;    /* Return code */
#if LISTER
	t_Byte sta;    /* Alert status */
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
	sta = (t_Byte) ((df1 >> 1) & 0x07);

				   /* List data field: */
	list_text (2, ";  Alert Status:");
	list_text (2, " " M_FMT_BYTE, sta);
	list_text (2, "\n");
#endif /* LISTER */

				   /* Set the return code: */
	ret = RC_OKAY;

	return ret;
}

/*----------------------------------------------------------------------------*/
/* proc_i004_060   -- Process I004/060 data item                              */
/*----------------------------------------------------------------------------*/

 static
 t_Retc proc_i004_060 (t_Ui16 length, t_Byte *buffer)
{
#if LISTER
	t_Byte dfo;    /* Data field octet */
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
	list_text (2, ";  Safety Net Function Status:");
	list_text (2, "\n");
	for (ix = 0; ix < length; ix ++)
	{
		dfo = buffer[ix];

		list_text (2, ";   ix=%d: 0x" M_FMT_BYTE_HEX "\n", 1 + ix, dfo);

		if (ix == 0)
		{
			list_text (2, ";   RAMLD=" M_FMT_BYTE, ((dfo >> 6) & 0x01));
			list_text (2, "; RAMHD=" M_FMT_BYTE, ((dfo >> 5) & 0x01));
			list_text (2, "; MSAW=" M_FMT_BYTE, ((dfo >> 4) & 0x01));
			list_text (2, "; APW=" M_FMT_BYTE, ((dfo >> 3) & 0x01));
			list_text (2, "; CLAM=" M_FMT_BYTE, ((dfo >> 2) & 0x01));
			list_text (2, "; STCA=" M_FMT_BYTE, ((dfo >> 1) & 0x01));
			list_text (2, "\n");
		}
		else if (ix == 1)
		{
			list_text (2, ";   AFDA=" M_FMT_BYTE, ((dfo >> 7) & 0x01));
			list_text (2, "; RIMCA=" M_FMT_BYTE, ((dfo >> 6) & 0x01));
			list_text (2, "\n");
		}
	}
#endif /* LISTER */

				   /* Set the return code: */
	ret = RC_OKAY;

	return ret;
}

/*----------------------------------------------------------------------------*/
/* proc_i004_070   -- Process I004/070 data item                              */
/*----------------------------------------------------------------------------*/

 static
 t_Retc proc_i004_070 (t_Ui16 length, t_Byte *buffer, t_Ui16 *pos_ptr)
{
#if LISTER
	t_Ui32 chs;    /* Current horizontal separation; 0.5 metres */
	t_Ui16 cvs;    /* Current vertical separation; 25 feet */
#endif /* LISTER */
	t_Byte df1;    /* Data field octet 1 */
	t_Ui16 inx;    /* Index into buffer */
#if LISTER
	int ix;        /* Auxiliary */
	int j;         /* Auxiliary */
#endif /* LISTER */
	t_Byte len;    /* Length of data field */
#if LISTER
	t_Ui16 mhs;    /* Estimated minimum horizontal separation; 0.5 metres */
	t_Ui16 mvs;    /* Estimated minimum vertical separation; 25 feet */
#endif /* LISTER */
	t_Ui16 pos;    /* Position within buffer */
	t_Retc ret;    /* Return code */
	t_Bool sf01_present;
                   /* Subfield #1 present */
	t_Bool sf02_present;
                   /* Subfield #2 present */
	t_Bool sf03_present;
                   /* Subfield #3 present */
	t_Bool sf04_present;
                   /* Subfield #4 present */
	t_Bool sf05_present;
                   /* Subfield #5 present */
	t_Bool sf06_present;
                   /* Subfield #6 present */
#if LISTER
	t_Ui32 tc;     /* Time to conflict; 1/128 seconds */
	t_Ui32 tca;    /* Time to closest approach; 1/128 seconds */
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

                   /* Preset local data: */
	sf01_present = FALSE;
	sf02_present = FALSE;
	sf03_present = FALSE;
	sf04_present = FALSE;
	sf05_present = FALSE;
	sf06_present = FALSE;

                   /* Get primary subfield: */
	df1 = buffer[pos];
	inx = 1;

	sf01_present = ((df1 & 0x80) != 0x00);
	sf02_present = ((df1 & 0x40) != 0x00);
	sf03_present = ((df1 & 0x20) != 0x00);
	sf04_present = ((df1 & 0x10) != 0x00);
	sf05_present = ((df1 & 0x08) != 0x00);
	sf06_present = ((df1 & 0x04) != 0x00);

				   /* Get length of data field: */
	len = inx;
	if (sf01_present)
	{
		len += 3;

		Assert (pos + len <= length, "Read buffer overrun");

#if LISTER
		tc = make_ui32 (0x00,
                        buffer[pos + inx],
                        buffer[pos + inx + 1],
                        buffer[pos + inx + 2]);
#endif /* LISTER */

		inx += 3;
	}
	if (sf02_present)
	{
		len += 3;

		Assert (pos + len <= length, "Read buffer overrun");

#if LISTER
		tca = make_ui32 (0x00,
                         buffer[pos + inx],
                         buffer[pos + inx + 1],
                         buffer[pos + inx + 2]);
#endif /* LISTER */

		inx += 3;
	}
	if (sf03_present)
	{
		len += 3;

		Assert (pos + len <= length, "Read buffer overrun");

#if LISTER
		chs = make_ui32 (0x00,
                         buffer[pos + inx],
                         buffer[pos + inx + 1],
                         buffer[pos + inx + 2]);
#endif /* LISTER */

		inx += 3;
	}
	if (sf04_present)
	{
		len += 2;

		Assert (pos + len <= length, "Read buffer overrun");

#if LISTER
		mhs = make_ui16 (buffer[pos + inx],
                         buffer[pos + inx + 1]);
#endif /* LISTER */

		inx += 2;
	}
	if (sf05_present)
	{
		len += 2;

		Assert (pos + len <= length, "Read buffer overrun");

#if LISTER
		cvs = make_ui16 (buffer[pos + inx],
                         buffer[pos + inx + 1]);
#endif /* LISTER */

		inx += 2;
	}
	if (sf06_present)
	{
		len += 2;

		Assert (pos + len <= length, "Read buffer overrun");

#if LISTER
		mvs = make_ui16 (buffer[pos + inx],
                         buffer[pos + inx + 1]);
#endif /* LISTER */

		inx += 2;
	}
	Assert (inx == len, "Illegal structure decoding");

				   /* Check against buffer length: */
	if (pos + len > length)
	{
		error_msg ("Invalid buffer length (I004/070)");
		goto done;
	}

#if LISTER
				   /* List raw data field: */
	list_text (1, ";  I004/070: =0x");
	j = 0;
	for (ix = 0; ix < len; ix ++)
	{
		list_text (1, " " M_FMT_BYTE_HEX, buffer[pos + ix]);
		++ j;

		if (j >= 16)
		{
			list_text (1, "\n");
			list_text (1, ";            +0x");

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

				   /* List data field: */
	list_text (2, ";  Conflict Timing and Separation:\n");
	if (sf01_present)
	{
		list_text (2, ";   Time to Conflict: " M_FMT_UI32 " (%.3f sec)\n",
                      tc, (1.0 / 128.0) * tc);
	}
	if (sf02_present)
	{
		list_text (2, ";   Time to Closest Approach: " M_FMT_UI32 " (%.3f sec)\n",
                      tca, (1.0 / 128.0) * tca);
	}
	if (sf03_present)
	{
		list_text (2, ";   Current Horizontal Separation: " M_FMT_UI32 " (%.1f mtr)\n",
                      chs, 0.5 * chs);
	}
	if (sf04_present)
	{
		list_text (2, ";   Estimated Minimum Horizontal Separation: " M_FMT_UI16
                      " (%.1f mtr)\n",
                      mhs, 0.5 * mhs);
	}
	if (sf05_present)
	{
		list_text (2, ";   Current Vertical Separation: " M_FMT_UI16 " (%.2f FL)\n",
                      cvs, 0.25 * cvs);
	}
	if (sf06_present)
	{
		list_text (2, ";   Estimated Minimum Vertical Separation: " M_FMT_UI16
                      " (%.2f FL)\n",
                      mvs, 0.25 * mvs);
	}
#endif /* LISTER */

				   /* Set position: */
	*pos_ptr = pos + len;

				   /* Set the return code: */
	ret = RC_OKAY;

	done:          /* We are done */
	return ret;
}

/*----------------------------------------------------------------------------*/
/* proc_i004_074   -- Process I004/074 data item                              */
/*----------------------------------------------------------------------------*/

 static
 t_Retc proc_i004_074 (t_Ui16 length, t_Byte *buffer)
{
#if LISTER
	t_Ui16 dev;    /* Longitudinal deviation; 32 mtr */
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
	dev = make_ui16 (df1, df2);

				   /* List data field: */
	list_text (2, ";  Longitudinal Deviation:");
	list_text (2, " " M_FMT_UI16 " (%.0f mtr)", dev, 32.0 * dev);
	list_text (2, "\n");
#endif /* LISTER */

				   /* Set the return code: */
	ret = RC_OKAY;

	return ret;
}

/*----------------------------------------------------------------------------*/
/* proc_i004_075   -- Process I004/075 data item                              */
/*----------------------------------------------------------------------------*/

 static
 t_Retc proc_i004_075 (t_Ui16 length, t_Byte *buffer)
{
#if LISTER
	t_Si32 dev;    /* Transversal distance deviation; 0.5 mtr */
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
	if (df1 & 0x80)
	{
		dev = make_si32 (0xff, df1, df2, df3);
	}
	else
	{
		dev = make_si32 (0x00, df1, df2, df3);
	}

				   /* List data field: */
	list_text (2, ";  Transversal Distance Deviation:");
	list_text (2, " " M_FMT_SI32 " (%.1f mtr)", dev, 0.5 * dev);
	list_text (2, "\n");
#endif /* LISTER */

				   /* Set the return code: */
	ret = RC_OKAY;

	return ret;
}

/*----------------------------------------------------------------------------*/
/* proc_i004_076   -- Process I004/076 data item                              */
/*----------------------------------------------------------------------------*/

 static
 t_Retc proc_i004_076 (t_Ui16 length, t_Byte *buffer)
{
#if LISTER
	t_Si16 dev;    /* Vertical deviation; 25 feet */
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
	dev = make_si16 (df1, df2);

				   /* List data field: */
	list_text (2, ";  Vertical Deviation:");
	list_text (2, " " M_FMT_SI16 " (%.2f FL)", dev, 0.25 * dev);
	list_text (2, "\n");
#endif /* LISTER */

				   /* Set the return code: */
	ret = RC_OKAY;

	return ret;
}

/*----------------------------------------------------------------------------*/
/* proc_i004_100   -- Process I004/100 data item                              */
/*----------------------------------------------------------------------------*/

 static
 t_Retc proc_i004_100 (t_Ui16 length, t_Byte *buffer, t_Ui16 *pos_ptr)
{
	char an[7];    /* Area name */
	char can[8];   /* Crossing area name */
	t_Byte df1;    /* Data field octet 1 */
	char g[8];     /* Gate designator */
	t_Ui16 inx;    /* Index into buffer */
#if LISTER
	int ix;        /* Auxiliary */
	int j;         /* Auxiliary */
#endif /* LISTER */
	t_Byte len;    /* Length of data field */
	t_Ui16 pos;    /* Position within buffer */
	t_Retc ret;    /* Return code */
	char rt1[8];   /* Runway/taxiway designator 1 */
	char rt2[8];   /* Runway/taxiway designator 2 */
	char sb[8];    /* Stop bar designator */
	t_Bool sf01_present;
                   /* Subfield #1 present */
	t_Bool sf02_present;
                   /* Subfield #2 present */
	t_Bool sf03_present;
                   /* Subfield #3 present */
	t_Bool sf04_present;
                   /* Subfield #4 present */
	t_Bool sf05_present;
                   /* Subfield #5 present */
	t_Bool sf06_present;
                   /* Subfield #6 present */

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

                   /* Preset local data: */
	sf01_present = FALSE;
	sf02_present = FALSE;
	sf03_present = FALSE;
	sf04_present = FALSE;
	sf05_present = FALSE;
	sf06_present = FALSE;

                   /* Get primary subfield: */
	df1 = buffer[pos];
	inx = 1;

	sf01_present = ((df1 & 0x80) != 0x00);
	sf02_present = ((df1 & 0x40) != 0x00);
	sf03_present = ((df1 & 0x20) != 0x00);
	sf04_present = ((df1 & 0x10) != 0x00);
	sf05_present = ((df1 & 0x08) != 0x00);
	sf06_present = ((df1 & 0x04) != 0x00);

				   /* Get length of data field: */
	len = inx;
	if (sf01_present)
	{
		len += 6;

		Assert (pos + len <= length, "Read buffer overrun");

		memset (an, 0, 7);
		an[0] = buffer[pos + inx];
		an[1] = buffer[pos + inx + 1];
		an[2] = buffer[pos + inx + 2];
		an[3] = buffer[pos + inx + 3];
		an[4] = buffer[pos + inx + 4];
		an[5] = buffer[pos + inx + 5];

		inx += 6;
	}
	if (sf02_present)
	{
		len += 7;

		Assert (pos + len <= length, "Read buffer overrun");

		memset (can, 0, 8);
		can[0] = buffer[pos + inx];
		can[1] = buffer[pos + inx + 1];
		can[2] = buffer[pos + inx + 2];
		can[3] = buffer[pos + inx + 3];
		can[4] = buffer[pos + inx + 4];
		can[5] = buffer[pos + inx + 5];
		can[6] = buffer[pos + inx + 6];

		inx += 7;
	}
	if (sf03_present)
	{
		len += 7;

		Assert (pos + len <= length, "Read buffer overrun");

		memset (rt1, 0, 8);
		rt1[0] = buffer[pos + inx];
		rt1[1] = buffer[pos + inx + 1];
		rt1[2] = buffer[pos + inx + 2];
		rt1[3] = buffer[pos + inx + 3];
		rt1[4] = buffer[pos + inx + 4];
		rt1[5] = buffer[pos + inx + 5];
		rt1[6] = buffer[pos + inx + 6];

		inx += 7;
	}
	if (sf04_present)
	{
		len += 7;

		Assert (pos + len <= length, "Read buffer overrun");

		memset (rt2, 0, 8);
		rt2[0] = buffer[pos + inx];
		rt2[1] = buffer[pos + inx + 1];
		rt2[2] = buffer[pos + inx + 2];
		rt2[3] = buffer[pos + inx + 3];
		rt2[4] = buffer[pos + inx + 4];
		rt2[5] = buffer[pos + inx + 5];
		rt2[6] = buffer[pos + inx + 6];

		inx += 7;
	}
	if (sf05_present)
	{
		len += 7;

		Assert (pos + len <= length, "Read buffer overrun");

		memset (sb, 0, 8);
		sb[0] = buffer[pos + inx];
		sb[1] = buffer[pos + inx + 1];
		sb[2] = buffer[pos + inx + 2];
		sb[3] = buffer[pos + inx + 3];
		sb[4] = buffer[pos + inx + 4];
		sb[5] = buffer[pos + inx + 5];
		sb[6] = buffer[pos + inx + 6];

		inx += 7;
	}
	if (sf06_present)
	{
		len += 7;

		Assert (pos + len <= length, "Read buffer overrun");

		memset (g, 0, 8);
		g[0] = buffer[pos + inx];
		g[1] = buffer[pos + inx + 1];
		g[2] = buffer[pos + inx + 2];
		g[3] = buffer[pos + inx + 3];
		g[4] = buffer[pos + inx + 4];
		g[5] = buffer[pos + inx + 5];
		g[6] = buffer[pos + inx + 6];

		inx += 7;
	}
	Assert (inx == len, "Illegal structure decoding");

				   /* Check against buffer length: */
	if (pos + len > length)
	{
		error_msg ("Invalid buffer length (I004/100)");
		goto done;
	}

#if LISTER
				   /* List raw data field: */
	list_text (1, ";  I004/100: =0x");
	j = 0;
	for (ix = 0; ix < len; ix ++)
	{
		list_text (1, " " M_FMT_BYTE_HEX, buffer[pos + ix]);
		++ j;

		if (j >= 16)
		{
			list_text (1, "\n");
			list_text (1, ";            +0x");

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

				   /* List data field: */
	list_text (2, ";  Area Definition:\n");
	if (sf01_present)
	{
		list_text (2, ";   Area Name: [%s]\n", an);
	}
	if (sf02_present)
	{
		list_text (2, ";   Crossing Area Name: [%s]\n", can);
	}
	if (sf03_present)
	{
		list_text (2, ";   Runway/Taxiway Designator 1: [%s]\n", rt1);
	}
	if (sf04_present)
	{
		list_text (2, ";   Runway/Taxiway Designator 2: [%s]\n", rt2);
	}
	if (sf05_present)
	{
		list_text (2, ";   Stop Bar Designator: [%s]\n", sb);
	}
	if (sf06_present)
	{
		list_text (2, ";   Gate Designator: [%s]\n", g);
	}
#endif /* LISTER */

				   /* Set position: */
	*pos_ptr = pos + len;

				   /* Set the return code: */
	ret = RC_OKAY;

	done:          /* We are done */
	return ret;
}

/*----------------------------------------------------------------------------*/
/* proc_i004_110   -- Process I004/110 data item                              */
/*----------------------------------------------------------------------------*/

 static
 t_Retc proc_i004_110 (t_Ui16 length, t_Byte *buffer)
{
#if LISTER
	t_Byte df1;    /* Data field octet 1 */
	t_Byte df2;    /* Data field octet 2 */
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
	if (rep == 0)
	{
		ret = RC_SKIP;
		goto done;
	}

				   /* Check length (again): */
	Assert (length == 1 + rep * 2, "Invalid length");

#if LISTER
				   /* List data field: */
	list_text (2, ";  FDPS Sector Control Identification:\n");
	for (ix = 0; ix < rep; ix ++)
	{
		df1 = buffer[1 + 2 * ix];
		df2 = buffer[1 + 2 * ix + 1];

		list_text (2, ";   centre=" M_FMT_BYTE "; position=" M_FMT_BYTE "\n",
                      df1, df2);
	}
#endif /* LISTER */

				   /* Set the return code: */
	ret = RC_OKAY;

	done:          /* We are done */
	return ret;
}

/*----------------------------------------------------------------------------*/
/* proc_i004_120   -- Process I004/120 data item                              */
/*----------------------------------------------------------------------------*/

 static
 t_Retc proc_i004_120 (t_Ui16 length, t_Byte *buffer, t_Ui16 *pos_ptr)
{
#if LISTER
	t_Byte cc;     /* Conflict classification */
	t_Ui32 cd;     /* Conflict duration; 1/128 seconds */
	t_Ui16 cn;     /* Conflict nature */
	t_Byte cp;     /* Conflict probability; 0.5 per cent */
#endif /* LISTER */
	t_Byte df1;    /* Data field octet 1 */
	t_Ui16 inx;    /* Index into buffer */
#if LISTER
	int ix;        /* Auxiliary */
	int j;         /* Auxiliary */
#endif /* LISTER */
	t_Byte len;    /* Length of data field */
	t_Ui16 pos;    /* Position within buffer */
	t_Retc ret;    /* Return code */
	t_Bool sf01_present;
                   /* Subfield #1 present */
	t_Bool sf02_present;
                   /* Subfield #2 present */
	t_Bool sf03_present;
                   /* Subfield #3 present */
	t_Bool sf04_present;
                   /* Subfield #4 present */

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

                   /* Preset local data: */
	sf01_present = FALSE;
	sf02_present = FALSE;
	sf03_present = FALSE;
	sf04_present = FALSE;

                   /* Get primary subfield: */
	df1 = buffer[pos];
	inx = 1;

	sf01_present = ((df1 & 0x80) != 0x00);
	sf02_present = ((df1 & 0x40) != 0x00);
	sf03_present = ((df1 & 0x20) != 0x00);
	sf04_present = ((df1 & 0x10) != 0x00);

				   /* Get length of data field: */
	len = inx;
	if (sf01_present)
	{
		t_Byte df;

		len += 1;

		Assert (pos + len <= length, "Read buffer overrun");

		df = buffer[pos + inx];
		if (df & 0x01)
		{
			len += 1;

			Assert (pos + len <= length, "Read buffer overrun");

#if LISTER
			cn = make_ui16 (buffer[pos + inx],
                            buffer[pos + inx + 1]);
#endif /* LISTER */

			inx += 1;
		}
		else
		{
#if LISTER
			cn = make_ui16 (buffer[pos + inx],
                            0x00);
#endif /* LISTER */

			inx += 1;
		}
	}
	if (sf02_present)
	{
		len += 1;

		Assert (pos + len <= length, "Read buffer overrun");

#if LISTER
		cc = buffer[pos + inx];
#endif /* LISTER */

		inx += 1;
	}
	if (sf03_present)
	{
		len += 1;

		Assert (pos + len <= length, "Read buffer overrun");

#if LISTER
		cp = buffer[pos + inx];
#endif /* LISTER */

		inx += 1;
	}
	if (sf04_present)
	{
		len += 3;

		Assert (pos + len <= length, "Read buffer overrun");

#if LISTER
		cd = make_ui32 (0x00,
                        buffer[pos + inx],
                        buffer[pos + inx + 1],
                        buffer[pos + inx + 2]);
#endif /* LISTER */

		inx += 3;
	}
	Assert (inx == len, "Illegal structure decoding");

				   /* Check against buffer length: */
	if (pos + len > length)
	{
		error_msg ("Invalid buffer length (I004/120)");
		goto done;
	}

#if LISTER
				   /* List raw data field: */
	list_text (1, ";  I004/120: =0x");
	j = 0;
	for (ix = 0; ix < len; ix ++)
	{
		list_text (1, " " M_FMT_BYTE_HEX, buffer[pos + ix]);
		++ j;

		if (j >= 16)
		{
			list_text (1, "\n");
			list_text (1, ";            +0x");

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

				   /* List data field: */
	list_text (2, ";  Conflict Characteristics:\n");
	if (sf01_present)
	{
		list_text (2, ";   Conflict Nature:\n");
		list_text (2, ";    cn=0x" M_FMT_UI16_HEX "\n", cn);
	}
	if (sf02_present)
	{
		list_text (2, ";   Conflict Classification:\n");
		list_text (2, ";    cc=0x" M_FMT_BYTE_HEX "\n", cc);
	}
	if (sf03_present)
	{
		list_text (2, ";   Conflict Probability:\n");
		list_text (2, ";    cp=" M_FMT_BYTE " (%.2f per cent)\n",
                      cp, 0.5 * cp);
	}
	if (sf04_present)
	{
		list_text (2, ";   Conflict Duration:\n");
		list_text (2, ";    cd=" M_FMT_UI32 " (%.3f seconds)\n",
                      cd, (1.0 / 128.0) * cd);
	}
#endif /* LISTER */

				   /* Set position: */
	*pos_ptr = pos + len;

				   /* Set the return code: */
	ret = RC_OKAY;

	done:          /* We are done */
	return ret;
}

/*----------------------------------------------------------------------------*/
/* proc_i004_170   -- Process I004/170 data item                              */
/*----------------------------------------------------------------------------*/

 static
 t_Retc proc_i004_170 (t_Ui16 length, t_Byte *buffer, t_Ui16 *pos_ptr)
{
#if LISTER
	t_Byte ac1[2]; /* Aircraft characteristics aircraft 1 */
	char ai1[8];   /* Aircraft identifier 1 */
	t_Si16 cf1;    /* Cleared flight level aircraft 1; 25 feet */
	t_Si32 cpc_x;  /* Predicted conflict position aircraft 1 in Cartesian
                      coordinates; x position; 0.5 mtr */
	t_Si32 cpc_y;  /* Predicted conflict position aircraft 1 in Cartesian
                      coordinates; y position; 0.5 mtr */
	t_Si16 cpc_z;  /* Predicted conflict position aircraft 1 in Cartesian
                      coordinates; z position; 25 feet */
	t_Si16 cpw_alt;
                   /* Predicted conflict position aircraft 1; WGS-84 altitude;
                      25 feet */
	t_Si32 cpw_lat;
                   /* Predicted conflict position aircraft 1; WGS-84 latitude;
                      180/2**25 degrees */
	t_Si32 cpw_lon;
                   /* Predicted conflict position aircraft 1; WGS-84 longitude;
                      180/2**25 degrees */
#endif /* LISTER */
	t_Byte df1;    /* Data field octet 1 */
	t_Byte df2;    /* Data field octet 2 */
#if LISTER
	t_Ui16 dt1;    /* Distance to threshold aircraft 1; 0.5 mtr */
	t_Ui32 fp1;    /* Flight plan number aircraft 1 */
#endif /* LISTER */
	t_Ui16 inx;    /* Index into buffer */
#if LISTER
	int ix;        /* Auxiliary */
	int j;         /* Auxiliary */
#endif /* LISTER */
	t_Byte len;    /* Length of data field */
#if LISTER
	t_Ui16 m31;    /* Mode 3/A code aircraft 1 */
	t_Byte ms1[6]; /* Mode S identifier aircraft 1 */
#endif /* LISTER */
	t_Ui16 pos;    /* Position within buffer */
	t_Retc ret;    /* Return code */
	t_Bool sf01_present;
                   /* Subfield #1 present */
	t_Bool sf02_present;
                   /* Subfield #2 present */
	t_Bool sf03_present;
                   /* Subfield #3 present */
	t_Bool sf04_present;
                   /* Subfield #4 present */
	t_Bool sf05_present;
                   /* Subfield #5 present */
	t_Bool sf06_present;
                   /* Subfield #6 present */
	t_Bool sf07_present;
                   /* Subfield #7 present */
	t_Bool sf08_present;
                   /* Subfield #8 present */
	t_Bool sf09_present;
                   /* Subfield #9 present */
	t_Bool sf10_present;
                   /* Subfield #10 present */
#if LISTER
	t_Ui32 tt1;    /* Time to threshold aircraft 1; 1/128 seconds */
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

                   /* Preset local data: */
	sf01_present = FALSE;
	sf02_present = FALSE;
	sf03_present = FALSE;
	sf04_present = FALSE;
	sf05_present = FALSE;
	sf06_present = FALSE;
	sf07_present = FALSE;
	sf08_present = FALSE;
	sf09_present = FALSE;
	sf10_present = FALSE;

                   /* Get primary subfield: */
	df1 = buffer[pos];
	inx = 1;

	sf01_present = ((df1 & 0x80) != 0x00);
	sf02_present = ((df1 & 0x40) != 0x00);
	sf03_present = ((df1 & 0x20) != 0x00);
	sf04_present = ((df1 & 0x10) != 0x00);
	sf05_present = ((df1 & 0x08) != 0x00);
	sf06_present = ((df1 & 0x04) != 0x00);
	sf07_present = ((df1 & 0x02) != 0x00);
	
	if (df1 & 0x01)
	{
		Assert (pos + 1 < length, "Read buffer overrun");

		df2 = buffer[pos + 1];
		inx = 2;

		sf08_present = ((df2 & 0x80) != 0x00);
		sf09_present = ((df2 & 0x40) != 0x00);
		sf10_present = ((df2 & 0x20) != 0x00);

		if ((df2 & 0x01) != 0x00)
		{
			error_msg ("Bad encoding in I004/170");
			goto done;
		}
	}

				   /* Get length of data field: */
	len = inx;
	if (sf01_present)
	{
		len += 7;

		Assert (pos + len <= length, "Read buffer overrun");

#if LISTER
		memset (ai1, 0, 8);

		ai1[0] = buffer[pos + inx];
		ai1[1] = buffer[pos + inx + 1];
		ai1[2] = buffer[pos + inx + 2];
		ai1[3] = buffer[pos + inx + 3];
		ai1[4] = buffer[pos + inx + 4];
		ai1[5] = buffer[pos + inx + 5];
		ai1[6] = buffer[pos + inx + 6];
#endif /* LISTER */

		inx += 7;
	}
	if (sf02_present)
	{
		len += 2;

		Assert (pos + len <= length, "Read buffer overrun");

#if LISTER
		m31 = make_ui16 (buffer[pos + inx],
                         buffer[pos + inx + 1]);
#endif /* LISTER */

		inx += 2;
	}
	if (sf03_present)
	{
		len += 10;

		Assert (pos + len <= length, "Read buffer overrun");

#if LISTER
		cpw_lat = make_si32 (buffer[pos + inx],
                             buffer[pos + inx + 1],
                             buffer[pos + inx + 2],
                             buffer[pos + inx + 3]);

		cpw_lon = make_si32 (buffer[pos + inx + 4],
                             buffer[pos + inx + 5],
                             buffer[pos + inx + 6],
                             buffer[pos + inx + 7]);

		cpw_alt = make_si16 (buffer[pos + inx + 8],
                             buffer[pos + inx + 9]);
#endif /* LISTER */

		inx += 10;
	}
	if (sf04_present)
	{
		len += 8;

		Assert (pos + len <= length, "Read buffer overrun");

#if LISTER
		if ((buffer[pos + inx] & 0x80) == 0)
		{
			cpc_x = make_si32 (0x00,
                               buffer[pos + inx],
                               buffer[pos + inx + 1],
                               buffer[pos + inx + 2]);
		}
		else
		{
			cpc_x = make_si32 (0xff,
                               buffer[pos + inx],
                               buffer[pos + inx + 1],
                               buffer[pos + inx + 2]);
		}

		if ((buffer[pos + inx + 3] & 0x80) == 0)
		{
			cpc_y = make_si32 (0x00,
                               buffer[pos + inx + 3],
                               buffer[pos + inx + 4],
                               buffer[pos + inx + 5]);
		}
		else
		{
			cpc_y = make_si32 (0xff,
                               buffer[pos + inx + 3],
                               buffer[pos + inx + 4],
                               buffer[pos + inx + 5]);
		}

		cpc_z = make_si16 (buffer[pos + inx + 6],
                           buffer[pos + inx + 7]);
#endif /* LISTER */

		inx += 8;
	}
	if (sf05_present)
	{
		len += 3;

		Assert (pos + len <= length, "Read buffer overrun");

#if LISTER
		tt1 = make_ui32 (0x00,
                         buffer[pos + inx],
                         buffer[pos + inx + 1],
                         buffer[pos + inx + 2]);
#endif /* LISTER */

		inx += 3;
	}
	if (sf06_present)
	{
		len += 2;

		Assert (pos + len <= length, "Read buffer overrun");

#if LISTER
		dt1 = make_ui16 (buffer[pos + inx],
                         buffer[pos + inx + 1]);
#endif /* LISTER */

		inx += 2;
	}
	if (sf07_present)
	{
		t_Byte df;

		len += 1;

		Assert (pos + len <= length, "Read buffer overrun");

		df = buffer[pos + inx];
		if (df & 0x01)
		{
			len += 1;

			Assert (pos + len <= length, "Read buffer overrun");

#if LISTER
			ac1[0] = buffer[pos + inx];
			ac1[1] = buffer[pos + inx + 1];
#endif /* LISTER */

			inx += 2;
		}
		else
		{
#if LISTER
			ac1[0] = buffer[pos + inx];
			ac1[1] = 0;
#endif /* LISTER */

			inx += 1;
		}
	}
	if (sf08_present)
	{
		len += 6;

		Assert (pos + len <= length, "Read buffer overrun");

#if LISTER
		ms1[0] = buffer[pos + inx];
		ms1[1] = buffer[pos + inx + 1];
		ms1[2] = buffer[pos + inx + 2];
		ms1[3] = buffer[pos + inx + 3];
		ms1[4] = buffer[pos + inx + 4];
		ms1[5] = buffer[pos + inx + 5];
#endif /* LISTER */

		inx += 6;
	}
	if (sf09_present)
	{
		len += 4;

		Assert (pos + len <= length, "Read buffer overrun");

#if LISTER
		fp1 = make_ui32 (buffer[pos + inx],
                         buffer[pos + inx + 1],
                         buffer[pos + inx + 2],
                         buffer[pos + inx + 3]);
#endif /* LISTER */

		inx += 4;
	}
	if (sf10_present)
	{
		len += 2;

		Assert (pos + len <= length, "Read buffer overrun");

#if LISTER
		cf1 = make_si16 (buffer[pos + inx],
                         buffer[pos + inx + 1]);
#endif /* LISTER */

		inx += 2;
	}
	Assert (inx == len, "Illegal structure decoding");

				   /* Check against buffer length: */
	if (pos + len > length)
	{
		error_msg ("Invalid buffer length (I004/170)");
		goto done;
	}

#if LISTER
				   /* List raw data field: */
	list_text (1, ";  I004/170: =0x");
	j = 0;
	for (ix = 0; ix < len; ix ++)
	{
		list_text (1, " " M_FMT_BYTE_HEX, buffer[pos + ix]);
		++ j;

		if (j >= 16)
		{
			list_text (1, "\n");
			list_text (1, ";            +0x");

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

				   /* List data field: */
	list_text (2, ";  Aircraft Identification and Characteristics 1:\n");
	if (sf01_present)
	{
		list_text (2, ";   Aircraft Identifier 1: [%s]\n", ai1);
	}
	if (sf02_present)
	{
		list_text (2, ";   Mode 3/A Code Aircraft 1: " M_FMT_CODE "\n",
                      m31 & 0x0fff);
	}
	if (sf03_present)
	{
		list_text (2, ";   Predicted Conflict Position Aircraft 1 (WGS-84):\n");
		list_text (2, ";    lat=" M_FMT_SI32 " (%s)\n",
                      cpw_lat, lat_text ((180.0 / M_TWO_POWER_25) * cpw_lat));
		list_text (2, ";    lon=" M_FMT_SI32 " (%s)\n",
                      cpw_lon, lon_text ((180.0 / M_TWO_POWER_25) * cpw_lon));
		list_text (2, ";    alt=" M_FMT_SI16 " (%.2f FL)\n",
                      cpw_alt, 0.25 * cpw_alt);
	}
	if (sf04_present)
	{
		list_text (2, ";   Predicted Conflict Position Aircraft 1 (Cartesian):\n");
		list_text (2, ";    x=" M_FMT_SI32 " (%.1f mtr)\n",
                      cpc_x, 0.5 * cpc_x);
		list_text (2, ";    y=" M_FMT_SI32 " (%.1f mtr)\n",
                      cpc_y, 0.5 * cpc_y);
		list_text (2, ";    z=" M_FMT_SI16 " (%.2f FL)\n",
                      cpc_z, 0.25 * cpc_z);
	}
	if (sf05_present)
	{
		list_text (2, ";   Time to Threshold Aircraft 1:\n");
		list_text (2, ";    tt1=" M_FMT_UI32 " (%.3f sec)\n",
                      tt1, (1.0 / 128.0) * tt1);
	}
	if (sf06_present)
	{
		list_text (2, ";   Distance to Threshold Aircraft 1:\n");
		list_text (2, ";    dt1=" M_FMT_UI16 " (%.1f mtr)\n",
                      dt1, 0.5 * dt1);
	}
	if (sf07_present)
	{
		list_text (2, ";   Aircraft Characteristics Aircraft 1:\n");
		if (ac1[0] & 0x01)
		{
			list_text (2, ";    ac1=0x" M_FMT_BYTE_HEX "+" M_FMT_BYTE_HEX "\n",
                          ac1[0], ac1[1]);
		}
		else
		{
			list_text (2, ";    ac1=0x" M_FMT_BYTE_HEX "\n", ac1[0]);
		}
	}
	if (sf08_present)
	{
		list_text (2, ";   Mode S Identifier Aircraft 1:\n");
		list_text (2, ";    ms1=0x" M_FMT_BYTE_HEX " " M_FMT_BYTE_HEX
                      " " M_FMT_BYTE_HEX " " M_FMT_BYTE_HEX
                      " " M_FMT_BYTE_HEX " " M_FMT_BYTE_HEX "\n",
                      ms1[0], ms1[1], ms1[2], ms1[3], ms1[4], ms1[5]);
	}
	if (sf09_present)
	{
		list_text (2, ";   Flight Plan Number Aircraft 1:\n");
		list_text (2, ";    fp1=" M_FMT_UI32 "\n", fp1);
	}
	if (sf10_present)
	{
		list_text (2, ";   Cleared Flight Level Aircraft 1:\n");
		list_text (2, ";    cf1=" M_FMT_SI16 " (%.2f FL)\n",
                      cf1, 0.25 * cf1);
	}
#endif /* LISTER */

				   /* Set position: */
	*pos_ptr = pos + len;

				   /* Set the return code: */
	ret = RC_OKAY;

	done:          /* We are done */
	return ret;
}

/*----------------------------------------------------------------------------*/
/* proc_i004_171   -- Process I004/171 data item                              */
/*----------------------------------------------------------------------------*/

 static
 t_Retc proc_i004_171 (t_Ui16 length, t_Byte *buffer, t_Ui16 *pos_ptr)
{
#if LISTER
	t_Byte ac2[2]; /* Aircraft characteristics aircraft 2 */
	char ai2[8];   /* Aircraft identifier 2 */
	t_Si16 cf2;    /* Cleared flight level aircraft 2; 25 feet */
	t_Si32 cpc_x;  /* Predicted conflict position aircraft 2 in Cartesian
                      coordinates; x position; 0.5 mtr */
	t_Si32 cpc_y;  /* Predicted conflict position aircraft 2 in Cartesian
                      coordinates; y position; 0.5 mtr */
	t_Si16 cpc_z;  /* Predicted conflict position aircraft 2 in Cartesian
                      coordinates; z position; 25 feet */
	t_Si16 cpw_alt;
                   /* Predicted conflict position aircraft 2; WGS-84 altitude;
                      25 feet */
	t_Si32 cpw_lat;
                   /* Predicted conflict position aircraft 2; WGS-84 latitude;
                      180/2**25 degrees */
	t_Si32 cpw_lon;
                   /* Predicted conflict position aircraft 2; WGS-84 longitude;
                      180/2**25 degrees */
#endif /* LISTER */
	t_Byte df1;    /* Data field octet 1 */
	t_Byte df2;    /* Data field octet 2 */
#if LISTER
	t_Ui16 dt2;    /* Distance to threshold aircraft 2; 0.5 mtr */
	t_Ui32 fp2;    /* Flight plan number aircraft 2 */
#endif /* LISTER */
	t_Ui16 inx;    /* Index into buffer */
#if LISTER
	int ix;        /* Auxiliary */
	int j;         /* Auxiliary */
#endif /* LISTER */
	t_Byte len;    /* Length of data field */
#if LISTER
	t_Ui16 m32;    /* Mode 3/A code aircraft 2 */
	t_Byte ms2[6]; /* Mode S identifier aircraft 2 */
#endif /* LISTER */
	t_Ui16 pos;    /* Position within buffer */
	t_Retc ret;    /* Return code */
	t_Bool sf01_present;
                   /* Subfield #1 present */
	t_Bool sf02_present;
                   /* Subfield #2 present */
	t_Bool sf03_present;
                   /* Subfield #3 present */
	t_Bool sf04_present;
                   /* Subfield #4 present */
	t_Bool sf05_present;
                   /* Subfield #5 present */
	t_Bool sf06_present;
                   /* Subfield #6 present */
	t_Bool sf07_present;
                   /* Subfield #7 present */
	t_Bool sf08_present;
                   /* Subfield #8 present */
	t_Bool sf09_present;
                   /* Subfield #9 present */
	t_Bool sf10_present;
                   /* Subfield #10 present */
#if LISTER
	t_Ui32 tt2;    /* Time to threshold aircraft 2; 1/128 seconds */
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

                   /* Preset local data: */
	sf01_present = FALSE;
	sf02_present = FALSE;
	sf03_present = FALSE;
	sf04_present = FALSE;
	sf05_present = FALSE;
	sf06_present = FALSE;
	sf07_present = FALSE;
	sf08_present = FALSE;
	sf09_present = FALSE;
	sf10_present = FALSE;

                   /* Get primary subfield: */
	df1 = buffer[pos];
	inx = 1;

	sf01_present = ((df1 & 0x80) != 0x00);
	sf02_present = ((df1 & 0x40) != 0x00);
	sf03_present = ((df1 & 0x20) != 0x00);
	sf04_present = ((df1 & 0x10) != 0x00);
	sf05_present = ((df1 & 0x08) != 0x00);
	sf06_present = ((df1 & 0x04) != 0x00);
	sf07_present = ((df1 & 0x02) != 0x00);
	
	if (df1 & 0x01)
	{
		Assert (pos + 1 < length, "Read buffer overrun");

		df2 = buffer[pos + 1];
		inx = 2;

		sf08_present = ((df2 & 0x80) != 0x00);
		sf09_present = ((df2 & 0x40) != 0x00);
		sf10_present = ((df2 & 0x20) != 0x00);

		if ((df2 & 0x01) != 0x00)
		{
			error_msg ("Bad encoding in I004/171");
			goto done;
		}
	}

				   /* Get length of data field: */
	len = inx;
	if (sf01_present)
	{
		len += 7;

		Assert (pos + len <= length, "Read buffer overrun");

#if LISTER
		memset (ai2, 0, 8);

		ai2[0] = buffer[pos + inx];
		ai2[1] = buffer[pos + inx + 1];
		ai2[2] = buffer[pos + inx + 2];
		ai2[3] = buffer[pos + inx + 3];
		ai2[4] = buffer[pos + inx + 4];
		ai2[5] = buffer[pos + inx + 5];
		ai2[6] = buffer[pos + inx + 6];
#endif /* LISTER */

		inx += 7;
	}
	if (sf02_present)
	{
		len += 2;

		Assert (pos + len <= length, "Read buffer overrun");

#if LISTER
		m32 = make_ui16 (buffer[pos + inx],
                         buffer[pos + inx + 1]);
#endif /* LISTER */

		inx += 2;
	}
	if (sf03_present)
	{
		len += 10;

		Assert (pos + len <= length, "Read buffer overrun");

#if LISTER
		cpw_lat = make_si32 (buffer[pos + inx],
                             buffer[pos + inx + 1],
                             buffer[pos + inx + 2],
                             buffer[pos + inx + 3]);

		cpw_lon = make_si32 (buffer[pos + inx + 4],
                             buffer[pos + inx + 5],
                             buffer[pos + inx + 6],
                             buffer[pos + inx + 7]);

		cpw_alt = make_si16 (buffer[pos + inx + 8],
                             buffer[pos + inx + 9]);
#endif /* LISTER */

		inx += 10;
	}
	if (sf04_present)
	{
		len += 8;

		Assert (pos + len <= length, "Read buffer overrun");

#if LISTER
		if ((buffer[pos + inx] & 0x80) == 0)
		{
			cpc_x = make_si32 (0x00,
                               buffer[pos + inx],
                               buffer[pos + inx + 1],
                               buffer[pos + inx + 2]);
		}
		else
		{
			cpc_x = make_si32 (0xff,
                               buffer[pos + inx],
                               buffer[pos + inx + 1],
                               buffer[pos + inx + 2]);
		}

		if ((buffer[pos + inx + 3] & 0x80) == 0)
		{
			cpc_y = make_si32 (0x00,
                               buffer[pos + inx + 3],
                               buffer[pos + inx + 4],
                               buffer[pos + inx + 5]);
		}
		else
		{
			cpc_y = make_si32 (0xff,
                               buffer[pos + inx + 3],
                               buffer[pos + inx + 4],
                               buffer[pos + inx + 5]);
		}

		cpc_z = make_si16 (buffer[pos + inx + 6],
                           buffer[pos + inx + 7]);
#endif /* LISTER */

		inx += 8;
	}
	if (sf05_present)
	{
		len += 3;

		Assert (pos + len <= length, "Read buffer overrun");

#if LISTER
		tt2 = make_ui32 (0x00,
                         buffer[pos + inx],
                         buffer[pos + inx + 1],
                         buffer[pos + inx + 2]);
#endif /* LISTER */

		inx += 3;
	}
	if (sf06_present)
	{
		len += 2;

		Assert (pos + len <= length, "Read buffer overrun");

#if LISTER
		dt2 = make_ui16 (buffer[pos + inx],
                         buffer[pos + inx + 1]);
#endif /* LISTER */

		inx += 2;
	}
	if (sf07_present)
	{
		t_Byte df;

		len += 1;

		Assert (pos + len <= length, "Read buffer overrun");

		df = buffer[pos + inx];
		if (df & 0x01)
		{
			len += 1;

			Assert (pos + len <= length, "Read buffer overrun");

#if LISTER
			ac2[0] = buffer[pos + inx];
			ac2[1] = buffer[pos + inx + 1];
#endif /* LISTER */

			inx += 2;
		}
		else
		{
#if LISTER
			ac2[0] = buffer[pos + inx];
			ac2[1] = 0;
#endif /* LISTER */

			inx += 1;
		}
	}
	if (sf08_present)
	{
		len += 6;

		Assert (pos + len <= length, "Read buffer overrun");

#if LISTER
		ms2[0] = buffer[pos + inx];
		ms2[1] = buffer[pos + inx + 1];
		ms2[2] = buffer[pos + inx + 2];
		ms2[3] = buffer[pos + inx + 3];
		ms2[4] = buffer[pos + inx + 4];
		ms2[5] = buffer[pos + inx + 5];
#endif /* LISTER */

		inx += 6;
	}
	if (sf09_present)
	{
		len += 4;

		Assert (pos + len <= length, "Read buffer overrun");

#if LISTER
		fp2 = make_ui32 (buffer[pos + inx],
                         buffer[pos + inx + 1],
                         buffer[pos + inx + 2],
                         buffer[pos + inx + 3]);
#endif /* LISTER */

		inx += 4;
	}
	if (sf10_present)
	{
		len += 2;

		Assert (pos + len <= length, "Read buffer overrun");

#if LISTER
		cf2 = make_si16 (buffer[pos + inx],
                         buffer[pos + inx + 1]);
#endif /* LISTER */

		inx += 2;
	}
	Assert (inx == len, "Illegal structure decoding");

				   /* Check against buffer length: */
	if (pos + len > length)
	{
		error_msg ("Invalid buffer length (I004/171)");
		goto done;
	}

#if LISTER
				   /* List raw data field: */
	list_text (1, ";  I004/171: =0x");
	j = 0;
	for (ix = 0; ix < len; ix ++)
	{
		list_text (1, " " M_FMT_BYTE_HEX, buffer[pos + ix]);
		++ j;

		if (j >= 16)
		{
			list_text (1, "\n");
			list_text (1, ";            +0x");

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

				   /* List data field: */
	list_text (2, ";  Aircraft Identification and Characteristics 2:\n");
	if (sf01_present)
	{
		list_text (2, ";   Aircraft Identifier 2: [%s]\n", ai2);
	}
	if (sf02_present)
	{
		list_text (2, ";   Mode 3/A Code Aircraft 2: " M_FMT_CODE "\n",
                      m32 & 0x0fff);
	}
	if (sf03_present)
	{
		list_text (2, ";   Predicted Conflict Position Aircraft 2 (WGS-84):\n");
		list_text (2, ";    lat=" M_FMT_SI32 " (%s)\n",
                      cpw_lat, lat_text ((180.0 / M_TWO_POWER_25) * cpw_lat));
		list_text (2, ";    lon=" M_FMT_SI32 " (%s)\n",
                      cpw_lon, lon_text ((180.0 / M_TWO_POWER_25) * cpw_lon));
		list_text (2, ";    alt=" M_FMT_SI16 " (%.2f FL)\n",
                      cpw_alt, 0.25 * cpw_alt);
	}
	if (sf04_present)
	{
		list_text (2, ";   Predicted Conflict Position Aircraft 2 (Cartesian):\n");
		list_text (2, ";    x=" M_FMT_SI32 " (%.1f mtr)\n",
                      cpc_x, 0.5 * cpc_x);
		list_text (2, ";    y=" M_FMT_SI32 " (%.1f mtr)\n",
                      cpc_y, 0.5 * cpc_y);
		list_text (2, ";    z=" M_FMT_SI16 " (%.2f FL)\n",
                      cpc_z, 0.25 * cpc_z);
	}
	if (sf05_present)
	{
		list_text (2, ";   Time to Threshold Aircraft 2:\n");
		list_text (2, ";    tt2=" M_FMT_UI32 " (%.3f sec)\n",
                      tt2, (1.0 / 128.0) * tt2);
	}
	if (sf06_present)
	{
		list_text (2, ";   Distance to Threshold Aircraft 2:\n");
		list_text (2, ";    dt2=" M_FMT_UI16 " (%.1f mtr)\n",
                      dt2, 0.5 * dt2);
	}
	if (sf07_present)
	{
		list_text (2, ";   Aircraft Characteristics Aircraft 2:\n");
		if (ac2[0] & 0x01)
		{
			list_text (2, ";    ac2=0x" M_FMT_BYTE_HEX "+" M_FMT_BYTE_HEX "\n",
                          ac2[0], ac2[1]);
		}
		else
		{
			list_text (2, ";    ac2=0x" M_FMT_BYTE_HEX "\n", ac2[0]);
		}
	}
	if (sf08_present)
	{
		list_text (2, ";   Mode S Identifier Aircraft 2:\n");
		list_text (2, ";    ms2=0x" M_FMT_BYTE_HEX " " M_FMT_BYTE_HEX
                      " " M_FMT_BYTE_HEX " " M_FMT_BYTE_HEX
                      " " M_FMT_BYTE_HEX " " M_FMT_BYTE_HEX "\n",
                      ms2[0], ms2[1], ms2[2], ms2[3], ms2[4], ms2[5]);
	}
	if (sf09_present)
	{
		list_text (2, ";   Flight Plan Number Aircraft 2:\n");
		list_text (2, ";    fp2=" M_FMT_UI32 "\n", fp2);
	}
	if (sf10_present)
	{
		list_text (2, ";   Cleared Flight Level Aircraft 2:\n");
		list_text (2, ";    cf2=" M_FMT_SI16 " (%.2f FL)\n",
                      cf2, 0.25 * cf2);
	}
#endif /* LISTER */

				   /* Set position: */
	*pos_ptr = pos + len;

				   /* Set the return code: */
	ret = RC_OKAY;

	done:          /* We are done */
	return ret;
}

/*----------------------------------------------------------------------------*/
/* proc_i004_ref   -- Process REF indicator data item                         */
/*----------------------------------------------------------------------------*/

 static
 t_Retc proc_i004_ref (t_Ui16 length, t_Byte *buffer, t_Ui16 *pos_ptr)
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
		error_msg ("Invalid buffer length (I004/REF)");
		goto done;
	}

#if LISTER
				   /* List raw data field: */
	list_buffer (1, ";  I004/REF:", len, buffer + pos);
#endif /* LISTER */

				   /* Set position: */
	*pos_ptr = pos + len;

				   /* Set the return code: */
	ret = RC_OKAY;

	done:          /* We are done */
	return ret;
}

/*----------------------------------------------------------------------------*/
/* proc_i004_spf   -- Process SPF indicator data item                         */
/*----------------------------------------------------------------------------*/

 static
 t_Retc proc_i004_spf (t_Ui16 length, t_Byte *buffer, t_Ui16 *pos_ptr)
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
	list_buffer (1, ";  I004/SPF:", len, buffer + pos);
#endif /* LISTER */

				   /* Set position: */
	*pos_ptr = pos + len;

				   /* Set the return code: */
	ret = RC_OKAY;

	done:          /* We are done */
	return ret;
}
/* end-of-file */
