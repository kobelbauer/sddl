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
/* File:           src/astx_032.cpp                                           */
/* Contents:       Process ASTERIX category 032 data block                    */
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

#define M_MAX_FRN 21
				   /* Maximum field reference number */
#define M_MAX_FSPEC_LENGTH 3
				   /* Maximum fields specification length
					  for ASTERIX category 032 */

				   /* Local variables: */
				   /* ---------------- */

static t_Data_Item_Desc desc_i032_010;
                   /* Description of data item I032/010 */
static t_Data_Item_Desc desc_i032_015;
                   /* Description of data item I032/015 */
static t_Data_Item_Desc desc_i032_018;
                   /* Description of data item I032/018 */
static t_Data_Item_Desc desc_i032_020;
                   /* Description of data item I032/020 */
static t_Data_Item_Desc desc_i032_035;
                   /* Description of data item I032/035 */
static t_Data_Item_Desc desc_i032_040;
                   /* Description of data item I032/040 */
static t_Data_Item_Desc desc_i032_050;
                   /* Description of data item I032/050 */
static t_Data_Item_Desc desc_i032_060;
                   /* Description of data item I032/060 */
static t_Data_Item_Desc desc_i032_400;
                   /* Description of data item I032/400 */
static t_Data_Item_Desc desc_i032_410;
                   /* Description of data item I032/410 */
static t_Data_Item_Desc desc_i032_420;
                   /* Description of data item I032/420 */
static t_Data_Item_Desc desc_i032_430;
                   /* Description of data item I032/430 */
static t_Data_Item_Desc desc_i032_435;
                   /* Description of data item I032/435 */
static t_Data_Item_Desc desc_i032_440;
                   /* Description of data item I032/440 */
static t_Data_Item_Desc desc_i032_450;
                   /* Description of data item I032/450 */
static t_Data_Item_Desc desc_i032_460;
                   /* Description of data item I032/460 */
static t_Data_Item_Desc desc_i032_480;
                   /* Description of data item I032/480 */
static t_Data_Item_Desc desc_i032_490;
                   /* Description of data item I032/490 */
static t_Data_Item_Desc desc_i032_500;
                   /* Description of data item I032/500 */
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
static t_Retc proc_i032_010 (t_Ui16 length, t_Byte *buffer);
                   /* Process I032/010 data item */
static t_Retc proc_i032_015 (t_Ui16 length, t_Byte *buffer);
                   /* Process I032/015 data item */
static t_Retc proc_i032_018 (t_Ui16 length, t_Byte *buffer);
                   /* Process I032/018 data item */
static t_Retc proc_i032_020 (t_Ui16 length, t_Byte *buffer);
                   /* Process I032/020 data item */
static t_Retc proc_i032_035 (t_Ui16 length, t_Byte *buffer);
                   /* Process I032/035 data item */
static t_Retc proc_i032_040 (t_Ui16 length, t_Byte *buffer);
                   /* Process I032/040 data item */
static t_Retc proc_i032_050 (t_Ui16 length, t_Byte *buffer, t_Ui16 *pos_ptr);
                   /* Process I032/050 data item */
static t_Retc proc_i032_060 (t_Ui16 length, t_Byte *buffer);
                   /* Process I032/060 data item */
static t_Retc proc_i032_400 (t_Ui16 length, t_Byte *buffer);
                   /* Process I032/400 data item */
static t_Retc proc_i032_410 (t_Ui16 length, t_Byte *buffer);
                   /* Process I032/410 data item */
static t_Retc proc_i032_420 (t_Ui16 length, t_Byte *buffer);
                   /* Process I032/420 data item */
static t_Retc proc_i032_430 (t_Ui16 length, t_Byte *buffer);
                   /* Process I032/430 data item */
static t_Retc proc_i032_435 (t_Ui16 length, t_Byte *buffer);
                   /* Process I032/435 data item */
static t_Retc proc_i032_440 (t_Ui16 length, t_Byte *buffer);
                   /* Process I032/440 data item */
static t_Retc proc_i032_450 (t_Ui16 length, t_Byte *buffer);
                   /* Process I032/450 data item */
static t_Retc proc_i032_460 (t_Ui16 length, t_Byte *buffer);
                   /* Process I032/460 data item */
static t_Retc proc_i032_480 (t_Ui16 length, t_Byte *buffer);
                   /* Process I032/480 data item */
static t_Retc proc_i032_490 (t_Ui16 length, t_Byte *buffer);
                   /* Process I032/490 data item */
static t_Retc proc_i032_500 (t_Ui16 length, t_Byte *buffer, t_Ui16 *pos_ptr);
                   /* Process I032/500 data item */

/*----------------------------------------------------------------------------*/
/* astx_032        -- Process ASTERIX category 032 data block                 */
/*----------------------------------------------------------------------------*/

 t_Retc astx_032 (t_Ui16 length, t_Byte *buffer)
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

					lrc = data_item (32, frn, std_uap[frn],
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
			error_msg ("Empty ASTERIX record.");
			goto done;
		}

                   /* Add frame date, if available: */
		if (frame_date_present)
		{
			strk.frame_date.present = TRUE;
			strk.frame_date.value = frame_date;
		}

                   /* Add frame time, if available: */
		if (frame_time_present)
		{
			strk.frame_time.present = TRUE;
			strk.frame_time.value = frame_time;
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
                   /* Description of data item I032/010: */
	desc_i032_010.category = 32;
	desc_i032_010.data_item = 10;
	desc_i032_010.item_type = e_fixed_length_data_item;
	desc_i032_010.fixed_length = 2;
	desc_i032_010.proc_fptr = proc_i032_010;
	desc_i032_010.read_fptr = NULL;

                   /* Description of data item I032/015: */
	desc_i032_015.category = 32;
	desc_i032_015.data_item = 15;
	desc_i032_015.item_type = e_fixed_length_data_item;
	desc_i032_015.fixed_length = 2;
	desc_i032_015.proc_fptr = proc_i032_015;
	desc_i032_015.read_fptr = NULL;

                   /* Description of data item I032/018: */
	desc_i032_018.category = 32;
	desc_i032_018.data_item = 18;
	desc_i032_018.item_type = e_fixed_length_data_item;
	desc_i032_018.fixed_length = 2;
	desc_i032_018.proc_fptr = proc_i032_018;
	desc_i032_018.read_fptr = NULL;

                   /* Description of data item I032/020: */
	desc_i032_020.category = 32;
	desc_i032_020.data_item = 20;
	desc_i032_020.item_type = e_fixed_length_data_item;
	desc_i032_020.fixed_length = 3;
	desc_i032_020.proc_fptr = proc_i032_020;
	desc_i032_020.read_fptr = NULL;

                   /* Description of data item I032/035: */
	desc_i032_035.category = 32;
	desc_i032_035.data_item = 35;
	desc_i032_035.item_type = e_fixed_length_data_item;
	desc_i032_035.fixed_length = 1;
	desc_i032_035.proc_fptr = proc_i032_035;
	desc_i032_035.read_fptr = NULL;

                   /* Description of data item I032/040: */
	desc_i032_040.category = 32;
	desc_i032_040.data_item = 40;
	desc_i032_040.item_type = e_fixed_length_data_item;
	desc_i032_040.fixed_length = 2;
	desc_i032_040.proc_fptr = proc_i032_040;
	desc_i032_040.read_fptr = NULL;

                   /* Description of data item I032/050: */
	desc_i032_050.category = 32;
	desc_i032_050.data_item = 50;
	desc_i032_050.item_type = e_immediate_data_item;
	desc_i032_050.fixed_length = 0;
	desc_i032_050.proc_fptr = NULL;
	desc_i032_050.read_fptr = proc_i032_050;

                   /* Description of data item I032/060: */
	desc_i032_060.category = 32;
	desc_i032_060.data_item = 60;
	desc_i032_060.item_type = e_fixed_length_data_item;
	desc_i032_060.fixed_length = 2;
	desc_i032_060.proc_fptr = proc_i032_060;
	desc_i032_060.read_fptr = NULL;

                   /* Description of data item I032/400: */
	desc_i032_400.category = 32;
	desc_i032_400.data_item = 400;
	desc_i032_400.item_type = e_fixed_length_data_item;
	desc_i032_400.fixed_length = 7;
	desc_i032_400.proc_fptr = proc_i032_400;
	desc_i032_400.read_fptr = NULL;

                   /* Description of data item I032/410: */
	desc_i032_410.category = 32;
	desc_i032_410.data_item = 410;
	desc_i032_410.item_type = e_fixed_length_data_item;
	desc_i032_410.fixed_length = 2;
	desc_i032_410.proc_fptr = proc_i032_410;
	desc_i032_410.read_fptr = NULL;

                   /* Description of data item I032/420: */
	desc_i032_420.category = 32;
	desc_i032_420.data_item = 420;
	desc_i032_420.item_type = e_fixed_length_data_item;
	desc_i032_420.fixed_length = 1;
	desc_i032_420.proc_fptr = proc_i032_420;
	desc_i032_420.read_fptr = NULL;

                   /* Description of data item I032/430: */
	desc_i032_430.category = 32;
	desc_i032_430.data_item = 430;
	desc_i032_430.item_type = e_fixed_length_data_item;
	desc_i032_430.fixed_length = 4;
	desc_i032_430.proc_fptr = proc_i032_430;
	desc_i032_430.read_fptr = NULL;

                   /* Description of data item I032/435: */
	desc_i032_435.category = 32;
	desc_i032_435.data_item = 435;
	desc_i032_435.item_type = e_fixed_length_data_item;
	desc_i032_435.fixed_length = 1;
	desc_i032_435.proc_fptr = proc_i032_435;
	desc_i032_435.read_fptr = NULL;

                   /* Description of data item I032/440: */
	desc_i032_440.category = 32;
	desc_i032_440.data_item = 440;
	desc_i032_440.item_type = e_fixed_length_data_item;
	desc_i032_440.fixed_length = 4;
	desc_i032_440.proc_fptr = proc_i032_440;
	desc_i032_440.read_fptr = NULL;

                   /* Description of data item I032/450: */
	desc_i032_450.category = 32;
	desc_i032_450.data_item = 450;
	desc_i032_450.item_type = e_fixed_length_data_item;
	desc_i032_450.fixed_length = 4;
	desc_i032_450.proc_fptr = proc_i032_450;
	desc_i032_450.read_fptr = NULL;

                   /* Description of data item I032/460: */
	desc_i032_460.category = 32;
	desc_i032_460.data_item = 460;
	desc_i032_460.item_type = e_repetitive_data_item;
	desc_i032_460.fixed_length = 2;
	desc_i032_460.proc_fptr = proc_i032_460;
	desc_i032_460.read_fptr = NULL;

                   /* Description of data item I032/480: */
	desc_i032_480.category = 32;
	desc_i032_480.data_item = 480;
	desc_i032_480.item_type = e_fixed_length_data_item;
	desc_i032_480.fixed_length = 2;
	desc_i032_480.proc_fptr = proc_i032_480;
	desc_i032_480.read_fptr = NULL;

                   /* Description of data item I032/490: */
	desc_i032_490.category = 32;
	desc_i032_490.data_item = 490;
	desc_i032_490.item_type = e_fixed_length_data_item;
	desc_i032_490.fixed_length = 2;
	desc_i032_490.proc_fptr = proc_i032_490;
	desc_i032_490.read_fptr = NULL;

                   /* Description of data item I032/500: */
	desc_i032_500.category = 32;
	desc_i032_500.data_item = 500;
	desc_i032_500.item_type = e_immediate_data_item;
	desc_i032_500.fixed_length = 0;
	desc_i032_500.proc_fptr = NULL;
	desc_i032_500.read_fptr = proc_i032_500;

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

	std_uap[1] = &desc_i032_010;
	std_uap[2] = &desc_i032_015;
	std_uap[3] = &desc_i032_018;
	std_uap[4] = &desc_i032_035;
	std_uap[5] = &desc_i032_020;
	std_uap[6] = &desc_i032_040;
	std_uap[7] = &desc_i032_050;

	std_uap[8] = &desc_i032_060;
	std_uap[9] = &desc_i032_400;
	std_uap[10] = &desc_i032_410;
	std_uap[11] = &desc_i032_420;
	std_uap[12] = &desc_i032_440;
	std_uap[13] = &desc_i032_450;
	std_uap[14] = &desc_i032_480;

	std_uap[15] = &desc_i032_490;
	std_uap[16] = &desc_i032_430;
	std_uap[17] = &desc_i032_435;
	std_uap[18] = &desc_i032_460;
	std_uap[19] = &desc_i032_500;

	return;
}

/*----------------------------------------------------------------------------*/
/* proc_i032_010   -- Process I032/010 data item                              */
/*----------------------------------------------------------------------------*/

 static
 t_Retc proc_i032_010 (t_Ui16 length, t_Byte *buffer)
{
	t_Byte df1;    /* Data field octet 1 */
	t_Byte df2;    /* Data field octet 2 */
	t_Ui16 dsi;    /* Served identification tag */
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
/* proc_i032_015   -- Process I032/015 data item                              */
/*----------------------------------------------------------------------------*/

 static
 t_Retc proc_i032_015 (t_Ui16 length, t_Byte *buffer)
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
/* proc_i032_018   -- Process I032/018 data item                              */
/*----------------------------------------------------------------------------*/

 static
 t_Retc proc_i032_018 (t_Ui16 length, t_Byte *buffer)
{
#if LISTER
	t_Byte df1;    /* Data field octet 1 */
	t_Byte df2;    /* Data field octet 2 */
	t_Ui16 dsi;    /* Data source identification tag */
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
	list_text (2, ";  Data Source Identification Tag:");
	list_text (2, " 0x" M_FMT_UI16_HEX, dsi);
	list_text (2, " (SAC=" M_FMT_BYTE "; SIC=" M_FMT_BYTE ")", sac, sic);
	list_text (2, "\n");
#endif /* LISTER */

				   /* Store this information: */
	/* tba ... */

				   /* Set the return code: */
	ret = RC_OKAY;

	return ret;
}

/*----------------------------------------------------------------------------*/
/* proc_i032_020   -- Process I032/020 data field                             */
/*----------------------------------------------------------------------------*/

 static
 t_Retc proc_i032_020 (t_Ui16 length, t_Byte *buffer)
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
/* proc_i032_035   -- Process I032/035 data item                              */
/*----------------------------------------------------------------------------*/

 static
 t_Retc proc_i032_035 (t_Ui16 length, t_Byte *buffer)
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
/* proc_i032_040   -- Process I032/040 data item                              */
/*----------------------------------------------------------------------------*/

 static
 t_Retc proc_i032_040 (t_Ui16 length, t_Byte *buffer)
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
/* proc_i032_050   -- Process I032/050 data item                              */
/*----------------------------------------------------------------------------*/

 static
 t_Retc proc_i032_050 (t_Ui16 length, t_Byte *buffer, t_Ui16 *pos_ptr)
{
	t_Ui16 base_pos;
                   /* Base position within buffer */
#if CCWARN
	t_Byte df1;    /* Data field octet 1 */
	t_Byte df2;    /* Data field octet 2 */
#endif /* CCWARN */
	t_Byte df3;    /* Data field octet 3 */
#if LISTER
	int ix;        /* Auxiliary */
	int j;         /* Auxiliary */
#endif /* LISTER */
	int num;       /* Number of local track numbers */
	t_Ui16 pos;    /* Position within buffer */
	t_Retc ret;    /* Return code */

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
#if CCWARN
	df1 = buffer[pos];
	df2 = buffer[pos + 1];
#endif /* CCWARN */
	df3 = buffer[pos + 2];

	pos += 3;

	num = 1;
	while (df3 & 0x01)
	{
#if CCWARN
		df1 = buffer[pos];
		df2 = buffer[pos + 1];
#endif /* CCWARN */
		df3 = buffer[pos + 2];

		pos += 3;

		++ num;
	}

#if LISTER
				   /* List raw data field: */
	list_text (1, ";  I032/050: 0x");
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
	/* tba ... */
	list_text (2, "\n");
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
/* proc_i032_060   -- Process I032/060 data item                              */
/*----------------------------------------------------------------------------*/

 static
 t_Retc proc_i032_060 (t_Ui16 length, t_Byte *buffer)
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
	list_text (2, " code=" M_FMT_CODE, m3c);
	list_text (2, "\n");
#endif /* LISTER */

				   /* Store this information: */
	strk.mode_3a_info.code = m3c;
	strk.mode_3a_info.present = TRUE;

				   /* Set the return code: */
	ret = RC_OKAY;

	return ret;
}

/*----------------------------------------------------------------------------*/
/* proc_i032_400   -- Process I032/400 data item                              */
/*----------------------------------------------------------------------------*/

 static
 t_Retc proc_i032_400 (t_Ui16 length, t_Byte *buffer)
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
/* proc_i032_410   -- Process I032/410 data item                              */
/*----------------------------------------------------------------------------*/

 static
 t_Retc proc_i032_410 (t_Ui16 length, t_Byte *buffer)
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
/* proc_i032_420   -- Process I032/420 data item                              */
/*----------------------------------------------------------------------------*/

 static
 t_Retc proc_i032_420 (t_Ui16 length, t_Byte *buffer)
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
/* proc_i032_430   -- Process I032/430 data item                              */
/*----------------------------------------------------------------------------*/

 static
 t_Retc proc_i032_430 (t_Ui16 length, t_Byte *buffer)
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
/* proc_i032_435   -- Process I032/435 data item                              */
/*----------------------------------------------------------------------------*/

 static
 t_Retc proc_i032_435 (t_Ui16 length, t_Byte *buffer)
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
/* proc_i032_440   -- Process I032/440 data item                              */
/*----------------------------------------------------------------------------*/

 static
 t_Retc proc_i032_440 (t_Ui16 length, t_Byte *buffer)
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
/* proc_i032_450   -- Process I032/450 data item                              */
/*----------------------------------------------------------------------------*/

 static
 t_Retc proc_i032_450 (t_Ui16 length, t_Byte *buffer)
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
/* proc_i032_460   -- Process I032/460 data item                              */
/*----------------------------------------------------------------------------*/

 static
 t_Retc proc_i032_460 (t_Ui16 length, t_Byte *buffer)
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
/* proc_i032_480   -- Process I032/480 data item                              */
/*----------------------------------------------------------------------------*/

 static
 t_Retc proc_i032_480 (t_Ui16 length, t_Byte *buffer)
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
/* proc_i032_490   -- Process I032/490 data item                              */
/*----------------------------------------------------------------------------*/

 static
 t_Retc proc_i032_490 (t_Ui16 length, t_Byte *buffer)
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
/* proc_i032_500   -- Process I032/500 data item                              */
/*----------------------------------------------------------------------------*/

 static
 t_Retc proc_i032_500 (t_Ui16 length, t_Byte *buffer, t_Ui16 *pos_ptr)
{
#if LISTER
	char ast[7];   /* Aircraft stand */
#endif /* LISTER */
	t_Ui16 base_pos;
                   /* Base position within buffer */
	t_Byte df1;    /* Data field octet 1 */
	t_Byte df2;    /* Data field octet 2 */
	int inx;       /* Index within buffer */
#if LISTER
	t_Byte ifi[4]; /* IFPS flight id */
	t_Byte ifi_typ;
	t_Ui32 ifi_nbr;
	int ix;        /* Auxiliary */
	int j;         /* Auxiliary */
#endif /* LISTER */
	t_Ui16 len;    /* Data item length */
	t_Ui16 pos;    /* Position within buffer */
#if LISTER
	char rds[4];   /* Runway designation */
#endif /* LISTER */
	t_Retc ret;    /* Return code */
#if LISTER
	t_Bool rvp;    /* RVSM & flight priority */
#endif /* LISTER */
	t_Bool sf1_present;
                   /* Sub-field #1 (IFI) present */
	t_Bool sf2_present;
                   /* Sub-field #2 (RVP) present */
	t_Bool sf3_present;
                   /* Sub-field #3 (RDS) present */
	t_Bool sf4_present;
                   /* Sub-field #4 (TOD) present */
	t_Bool sf5_present;
                   /* Sub-field #5 (AST) present */
	t_Bool sf6_present;
                   /* Sub-field #6 (STS) present */
	t_Bool sf7_present;
                   /* Sub-field #7 (STD) present */
	t_Bool sf8_present;
                   /* Sub-field #8 (STA) present */
#if LISTER
	char sta[8];   /* Standard instrument arrival */
	char std[8];   /* Standard instrument departure */
	t_Byte sts;    /* Stand status */
#endif /* LISTER */
	t_Byte tod_rep;
                   /* Repetition factor for TOD */
#if LISTER
	t_Ui16 tod_pos;
                   /* Position within buffer - start of TOD content */
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
	if (pos >= length)
	{
		error_msg ("Invalid buffer length(1)");
		goto done;
	}

                   /* Preset sub-field presence indicators: */
	sf1_present = FALSE;
	sf2_present = FALSE;
	sf3_present = FALSE;
	sf4_present = FALSE;
	sf5_present = FALSE;
	sf6_present = FALSE;
	sf7_present = FALSE;
	sf8_present = FALSE;

				   /* Extract octets: */
	df1 = buffer[pos];
	if (df1 & 0x01)
	{
		if (pos + 1 >= length)
		{
			error_msg ("Invalid buffer length(1a)");
			goto done;
		}

		df2 = buffer[pos + 1];
	}

                   /* Determine presence of sub-fields: */
	if (df1 & 0x80)
	{
		sf1_present = TRUE;
	}
	if (df1 & 0x40)
	{
		sf2_present = TRUE;
	}
	if (df1 & 0x20)
	{
		sf3_present = TRUE;
	}
	if (df1 & 0x10)
	{
		sf4_present = TRUE;
	}
	if (df1 & 0x08)
	{
		sf5_present = TRUE;
	}
	if (df1 & 0x04)
	{
		sf6_present = TRUE;
	}
	if (df1 & 0x02)
	{
		sf7_present = TRUE;
	}
	if (df1 & 0x01)
	{
		if (df2 & 0x80)
		{
			sf8_present = TRUE;
		}
	}

                   /* Be aware that sub-field #4 has dynamic length,
                      therefore we need to READ the subsequent data octets
                      to determine the data item length */

                   /* Determine data item length: */
	inx = 1 +
          ord (df1 & 0x01);
	if (sf1_present)
	{
		if (pos + inx + 3 >= length)
		{
			error_msg ("Invalid buffer length (sf1)");
			goto done;
		}

#if LISTER
		ifi[0] = buffer[pos + inx];
		ifi[1] = buffer[pos + inx + 1];
		ifi[2] = buffer[pos + inx + 2];
		ifi[3] = buffer[pos + inx + 3];

		ifi_typ = (ifi[0] >> 6) & 0x03;
		ifi_nbr = make_ui32 (0x00, ifi[1], ifi[2], ifi[3]);
#endif /* LISTER */

		inx += 4;
	}
	if (sf2_present)
	{
		if (pos + inx >= length)
		{
			error_msg ("Invalid buffer length (sf2)");
			goto done;
		}

#if LISTER
		rvp = buffer[pos + inx];
#endif /* LISTER */

		inx += 1;
	}
	if (sf3_present)
	{
		if (pos + inx + 2 >= length)
		{
			error_msg ("Invalid buffer length (sf3)");
			goto done;
		}

#if LISTER
		rds[0] = buffer[pos + inx];
		rds[1] = buffer[pos + inx + 1];
		rds[2] = buffer[pos + inx + 2];
		rds[3] = '\0';
#endif /* LISTER */

		inx += 3;
	}
	if (sf4_present)
	{
		if (pos + inx >= length)
		{
			error_msg ("Invalid buffer length (sf4)");
			goto done;
		}

		tod_rep = buffer[pos + inx];

		if (tod_rep == 0)
		{
			error_msg ("Invalid repetition factor");
			goto done;
		}

		if (pos + inx + 1 + tod_rep * 4 > length)
		{
			error_msg ("Invalid buffer length (sf4)");
			goto done;
		}

#if LISTER
		tod_pos = pos + inx + 1;
#endif /* LISTER */

		inx += (1 + tod_rep * 4);
	}
	if (sf5_present)
	{
		if (pos + inx + 5 >= length)
		{
			error_msg ("Invalid buffer length (sf5)");
			goto done;
		}

#if LISTER
		ast[0] = buffer[pos + inx];
		ast[1] = buffer[pos + inx + 1];
		ast[2] = buffer[pos + inx + 2];
		ast[3] = buffer[pos + inx + 3];
		ast[4] = buffer[pos + inx + 4];
		ast[5] = buffer[pos + inx + 5];
		ast[6] = '\0';
#endif /* LISTER */

		inx += 6;
	}
	if (sf6_present)
	{
		if (pos + inx >= length)
		{
			error_msg ("Invalid buffer length (sf6)");
			goto done;
		}

#if LISTER
		sts = buffer[pos + inx];
#endif /* LISTER */

		inx += 1;
	}
	if (sf7_present)
	{
		if (pos + inx + 6 >= length)
		{
			error_msg ("Invalid buffer length (sf7)");
			goto done;
		}

#if LISTER
		std[0] = buffer[pos + inx];
		std[1] = buffer[pos + inx + 1];
		std[2] = buffer[pos + inx + 2];
		std[3] = buffer[pos + inx + 3];
		std[4] = buffer[pos + inx + 4];
		std[5] = buffer[pos + inx + 5];
		std[6] = buffer[pos + inx + 6];
		std[7] = '\0';
#endif /* LISTER */

		inx += 7;
	}
	if (sf8_present)
	{
		if (pos + inx + 6 >= length)
		{
			error_msg ("Invalid buffer length (sf8)");
			goto done;
		}

#if LISTER
		sta[0] = buffer[pos + inx];
		sta[1] = buffer[pos + inx + 1];
		sta[2] = buffer[pos + inx + 2];
		sta[3] = buffer[pos + inx + 3];
		sta[4] = buffer[pos + inx + 4];
		sta[5] = buffer[pos + inx + 5];
		sta[6] = buffer[pos + inx + 6];
		sta[7] = '\0';
#endif /* LISTER */

		inx += 7;
	}

	len = inx;

#if LISTER
				   /* List raw data field: */
	list_text (1, ";  I032/500: 0x");
	j = 0;
	for (ix = 0; ix < len; ix ++)
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

#if LISTER
				   /* List data field: */
	list_text (2, ";  Supplementary Flight Data:");
	list_text (2, "\n");
	if (sf1_present)
	{
		list_text (2, ";   IFPS Flight ID: typ=" M_FMT_BYTE "; nbr=" M_FMT_UI32,
                      ifi_typ, ifi_nbr);
		list_text (2, "\n");
	}
	if (sf2_present)
	{
		list_text (2, ";   RVSM & Flight Priority: rvsm=" M_FMT_BYTE
                      "; hpr=" M_FMT_BYTE,
                      (rvp >> 1) & 0x03, rvp & 0x01);
		list_text (2, "\n");
	}
	if (sf3_present)
	{
		list_text (2, ";   Runway Designation: [%s]", rds);
		list_text (2, "\n");
	}
	if (sf4_present)
	{
		t_Byte tod[4];

		list_text (2, ";   Time of Departure/Arrival:");
		list_text (2, "\n");

		inx = tod_pos;
		for (ix = 0; ix < tod_rep; ix ++)
		{
			tod[0] = buffer[inx];
			tod[1] = buffer[inx + 1];
			tod[2] = buffer[inx + 2];
			tod[3] = buffer[inx + 3];

			inx += 4;

			list_text (2, ";   typ=" M_FMT_BYTE, (tod[0] >> 3) & 0x1f);
			list_text (2, "; day=" M_FMT_BYTE, (tod[0] >> 1) & 0x03);
			list_text (2, "; hor=" M_FMT_BYTE, tod[1] & 0x1f);
			list_text (2, "; min=" M_FMT_BYTE, tod[2] & 0x3f);
			list_text (2, "; sec=" M_FMT_BYTE, tod[3] & 0x3f);
			list_text (2, "\n");
		}
	}
	if (sf5_present)
	{
		list_text (2, ";   Aircraft Stand: [%s]", ast);
		list_text (2, "\n");
	}
	if (sf6_present)
	{
		list_text (2, ";   Stand Status: emp=" M_FMT_BYTE "; avl=" M_FMT_BYTE,
                      (sts >> 6) & 0x03, (sts >> 4) & 0x03);
		list_text (2, "\n");
	}
	if (sf7_present)
	{
		list_text (2, ";   Standard Instrument Departure: [%s]", std);
		list_text (2, "\n");
	}
	if (sf8_present)
	{
		list_text (2, ";   Standard Instrument Arrival: [%s]", sta);
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
/* end-of-file */
