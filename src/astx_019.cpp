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
/* File:           src/astx_019.cpp                                           */
/* Contents:       Process ASTERIX category 019 data block                    */
/* Author(s):      kb                                                         */
/* Last change:    2018-04-27                                                 */
/*----------------------------------------------------------------------------*/

/* Reference document:
   -------------------

   EUROCONTROL Standard Document for Surveillance Data Exchange
   Part 18 : Category 019 - Multilateration System Status Messages
   SUR.ET1.ST05.2000-STD-18-02

   Edition: 1.1
   Edition date: March 2007
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

#define M_MAX_FRN 14
				   /* Maximum field reference number */
#define M_MAX_FSPEC_LENGTH 2
				   /* Maximum fields specification length
					  for ASTERIX category 019 */

				   /* Local variables: */
				   /* ---------------- */

static t_Data_Item_Desc desc_i019_000;
                   /* Description of data item I019/000 */
static t_Data_Item_Desc desc_i019_010;
                   /* Description of data item I019/010 */
static t_Data_Item_Desc desc_i019_140;
                   /* Description of data item I019/140 */
static t_Data_Item_Desc desc_i019_550;
                   /* Description of data item I019/550 */
static t_Data_Item_Desc desc_i019_551;
                   /* Description of data item I019/551 */
static t_Data_Item_Desc desc_i019_552;
                   /* Description of data item I019/552 */
static t_Data_Item_Desc desc_i019_553;
                   /* Description of data item I019/553 */
static t_Data_Item_Desc desc_i019_600;
                   /* Description of data item I019/600 */
static t_Data_Item_Desc desc_i019_610;
                   /* Description of data item I019/610 */
static t_Data_Item_Desc desc_i019_620;
                   /* Description of data item I019/620 */
static t_Data_Item_Desc desc_i019_ref;
                   /* Description of RE data item */
static t_Data_Item_Desc desc_i019_spf;
                   /* Description of SP data item */
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
static t_Retc proc_i019_000 (t_Ui16 length, t_Byte *buffer);
                   /* Process I019/000 data item */
static t_Retc proc_i019_010 (t_Ui16 length, t_Byte *buffer);
                   /* Process I019/010 data item */
static t_Retc proc_i019_140 (t_Ui16 length, t_Byte *buffer);
                   /* Process I019/140 data item */
static t_Retc proc_i019_550 (t_Ui16 length, t_Byte *buffer);
                   /* Process I019/550 data item */
static t_Retc proc_i019_551 (t_Ui16 length, t_Byte *buffer);
                   /* Process I019/551 data item */
static t_Retc proc_i019_552 (t_Ui16 length, t_Byte *buffer);
                   /* Process I019/552 data item */
static t_Retc proc_i019_553 (t_Ui16 length, t_Byte *buffer);
                   /* Process I019/553 data item */
static t_Retc proc_i019_600 (t_Ui16 length, t_Byte *buffer);
                   /* Process I019/600 data item */
static t_Retc proc_i019_610 (t_Ui16 length, t_Byte *buffer);
                   /* Process I019/610 data item */
static t_Retc proc_i019_620 (t_Ui16 length, t_Byte *buffer);
                   /* Process I019/620 data item */
static t_Retc proc_i019_ref (t_Ui16 length, t_Byte *buffer, t_Ui16 *pos_ptr);
                   /* Process RE indicator data item */
static t_Retc proc_i019_spf (t_Ui16 length, t_Byte *buffer, t_Ui16 *pos_ptr);
                   /* Process SP indicator data item */

/*----------------------------------------------------------------------------*/
/* astx_019        -- Process ASTERIX category 019 data block                 */
/*----------------------------------------------------------------------------*/

 t_Retc astx_019 (t_Ui16 length, t_Byte *buffer)
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
		mlat.asterix_category = 19;
		mlat.data_format = e_data_format_asterix;

                   /* This report is a status message: */
		mlat.is_status_message = TRUE;

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

					lrc = data_item (19, frn, std_uap[frn],
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

                   /* Process multilateration status report: */
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
                   /* Description of data item I019/000: */
    desc_i019_000.category = 19;
    desc_i019_000.data_item = 0;
    desc_i019_000.item_type = e_fixed_length_data_item;
    desc_i019_000.fixed_length = 1;
    desc_i019_000.proc_fptr = proc_i019_000;
    desc_i019_000.read_fptr = NULL;

                   /* Description of data item I019/010: */
    desc_i019_010.category = 19;
    desc_i019_010.data_item = 10;
    desc_i019_010.item_type = e_fixed_length_data_item;
    desc_i019_010.fixed_length = 2;
    desc_i019_010.proc_fptr = proc_i019_010;
    desc_i019_010.read_fptr = NULL;

                   /* Description of data item I019/140: */
    desc_i019_140.category = 19;
    desc_i019_140.data_item = 140;
    desc_i019_140.item_type = e_fixed_length_data_item;
    desc_i019_140.fixed_length = 3;
    desc_i019_140.proc_fptr = proc_i019_140;
    desc_i019_140.read_fptr = NULL;

                   /* Description of data item I019/550: */
    desc_i019_550.category = 19;
    desc_i019_550.data_item = 550;
    desc_i019_550.item_type = e_fixed_length_data_item;
    desc_i019_550.fixed_length = 1;
    desc_i019_550.proc_fptr = proc_i019_550;
    desc_i019_550.read_fptr = NULL;

                   /* Description of data item I019/551: */
    desc_i019_551.category = 19;
    desc_i019_551.data_item = 551;
    desc_i019_551.item_type = e_fixed_length_data_item;
    desc_i019_551.fixed_length = 1;
    desc_i019_551.proc_fptr = proc_i019_551;
    desc_i019_551.read_fptr = NULL;

                   /* Description of data item I019/552: */
    desc_i019_552.category = 19;
    desc_i019_552.data_item = 552;
    desc_i019_552.item_type = e_repetitive_data_item;
    desc_i019_552.fixed_length = 2;
    desc_i019_552.proc_fptr = proc_i019_552;
    desc_i019_552.read_fptr = NULL;

                   /* Description of data item I019/553: */
    desc_i019_553.category = 19;
    desc_i019_553.data_item = 553;
    desc_i019_553.item_type = e_variable_length_data_item;
    desc_i019_553.fixed_length = 0;
    desc_i019_553.proc_fptr = proc_i019_553;
    desc_i019_553.read_fptr = NULL;

                   /* Description of data item I019/600: */
    desc_i019_600.category = 19;
    desc_i019_600.data_item = 600;
    desc_i019_600.item_type = e_fixed_length_data_item;
    desc_i019_600.fixed_length = 8;
    desc_i019_600.proc_fptr = proc_i019_600;
    desc_i019_600.read_fptr = NULL;

                   /* Description of data item I019/610: */
    desc_i019_610.category = 19;
    desc_i019_610.data_item = 610;
    desc_i019_610.item_type = e_fixed_length_data_item;
    desc_i019_610.fixed_length = 2;
    desc_i019_610.proc_fptr = proc_i019_610;
    desc_i019_610.read_fptr = NULL;

                   /* Description of data item I019/620: */
    desc_i019_620.category = 19;
    desc_i019_620.data_item = 620;
    desc_i019_620.item_type = e_fixed_length_data_item;
    desc_i019_620.fixed_length = 1;
    desc_i019_620.proc_fptr = proc_i019_620;
    desc_i019_620.read_fptr = NULL;

                   /* Description of RE data item: */
    desc_i019_ref.category = 19;
    desc_i019_ref.data_item = M_REF_INDICATOR;
    desc_i019_ref.item_type = e_immediate_data_item;
    desc_i019_ref.fixed_length = 0;
    desc_i019_ref.proc_fptr = NULL;
    desc_i019_ref.read_fptr = proc_i019_ref;

                   /* Description of SP data item: */
    desc_i019_spf.category = 19;
    desc_i019_spf.data_item = M_SPF_INDICATOR;
    desc_i019_spf.item_type = e_immediate_data_item;
    desc_i019_spf.fixed_length = 0;
    desc_i019_spf.proc_fptr = NULL;
    desc_i019_spf.read_fptr = proc_i019_spf;

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

	std_uap[1] = &desc_i019_010;
	std_uap[2] = &desc_i019_000;
	std_uap[3] = &desc_i019_140;
	std_uap[4] = &desc_i019_550;
	std_uap[5] = &desc_i019_551;
	std_uap[6] = &desc_i019_552;
	std_uap[7] = &desc_i019_553;

	std_uap[8] = &desc_i019_600;
	std_uap[9] = &desc_i019_610;
	std_uap[10] = &desc_i019_620;
	std_uap[13] = &desc_i019_ref;
	std_uap[14] = &desc_i019_spf;

	return;
}

/*----------------------------------------------------------------------------*/
/* proc_i019_000   -- Process I019/000 data item                              */
/*----------------------------------------------------------------------------*/

 static
 t_Retc proc_i019_000 (t_Ui16 length, t_Byte *buffer)
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
	list_text (2, ";  Message Type: ");
	list_text (2, "mtp=" M_FMT_BYTE, mtp);
	if (mtp == 1)
	{
		list_text (2, " (Start of update cycle)");
	}
	else if (mtp == 2)
	{
		list_text (2, " (Periodic status message)");
	}
	else if (mtp == 3)
	{
		list_text (2, " (Event-triggered status message)");
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
/* proc_i019_010   -- Process I019/010 data item                              */
/*----------------------------------------------------------------------------*/

 static
 t_Retc proc_i019_010 (t_Ui16 length, t_Byte *buffer)
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
    mlat.data_source_identifier.sac = df1;
    mlat.data_source_identifier.sic = df2;

				   /* Set the return code: */
	ret = RC_OKAY;

	return ret;
}

/*----------------------------------------------------------------------------*/
/* proc_i019_140   -- Process I019/140 data item                              */
/*----------------------------------------------------------------------------*/

 static
 t_Retc proc_i019_140 (t_Ui16 length, t_Byte *buffer)
{
#if LISTER
	t_Secs atm;    /* Absolute time; seconds */
	t_Byte df1;    /* Data field octet 1 */
	t_Byte df2;    /* Data field octet 2 */
	t_Byte df3;    /* Data field octet 3 */
#endif /* LISTER */
	t_Retc ret;    /* Return code */
#if LISTER
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
	tod = make_ui32 (0, df1, df2, df3);

				   /* Compute time (of day): */
	atm = (1.0 / 128.0) * tod;

				   /* List data field: */
	list_text (2, ";  Time of Day:");
	list_text (2, " " M_FMT_ATOD " (" M_FMT_UI32 "; %s UTC)",
                  tod, tod, utc_text (atm));
	list_text (2, "\n");
#endif /* LISTER */

				   /* Store this information: */
	/* tba ... */

				   /* Set the return code: */
	ret = RC_OKAY;

	return ret;
}

/*----------------------------------------------------------------------------*/
/* proc_i019_550   -- Process I019/550 data item                              */
/*----------------------------------------------------------------------------*/

 static
 t_Retc proc_i019_550 (t_Ui16 length, t_Byte *buffer)
{
#if LISTER
	t_Byte df1;    /* Data field octet 1 */
	t_Byte nogo;   /* Operational release status of the system */
	t_Byte ovl;    /* Overload indicator */
#endif /* LISTER */
	t_Retc ret;    /* Return code */
#if LISTER
	t_Byte tsv;    /* Time source validity */
	t_Byte ttf;    /* Test target failure */
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
	nogo = (df1 >> 6) & 0x03;
	ovl = (df1 >> 5) & 0x01;
	tsv = (df1 >> 4) & 0x01;
	ttf = (df1 >> 3) & 0x01;

				   /* List data field: */
	list_text (2, ";  System Status:");
	list_text (2, " nogo=%u", nogo);
	if (nogo == 0)
	{
		list_text (2, " (operational)");
	}
	else if (nogo == 1)
	{
		list_text (2, " (degraded)");
	}
	else if (nogo == 2)
	{
		list_text (2, " (nogo)");
	}
	else if (nogo == 3)
	{
		list_text (2, " (undefined)");
	}
	list_text (2, ";");
	list_text (2, " ovl=" M_FMT_BYTE ";", ovl);
	list_text (2, " tsv=" M_FMT_BYTE ";", tsv);
	list_text (2, " ttf=" M_FMT_BYTE, ttf);
	list_text (2, "\n");
#endif /* LISTER */

				   /* Store this information: */
	/* tba ... */

				   /* Set the return code: */
	ret = RC_OKAY;

	return ret;
}

/*----------------------------------------------------------------------------*/
/* proc_i019_551   -- Process I019/551 data item                              */
/*----------------------------------------------------------------------------*/

 static
 t_Retc proc_i019_551 (t_Ui16 length, t_Byte *buffer)
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

#if LISTER
				   /* Extract octets: */
	df1 = buffer[0];

				   /* Extract information: */
	/* tba ... */

				   /* List data field: */
	list_text (2, ";  Tracking Processor Detailed Status:");
	list_text (2, " 0x" M_FMT_BYTE_HEX, df1);
	list_text (2, "\n");
#endif /* LISTER */

				   /* Store this information: */
	/* tba ... */

				   /* Set the return code: */
	ret = RC_OKAY;

	return ret;
}

/*----------------------------------------------------------------------------*/
/* proc_i019_552   -- Process I019/552 data item                              */
/*----------------------------------------------------------------------------*/

 static
 t_Retc proc_i019_552 (t_Ui16 length, t_Byte *buffer)
{
#if LISTER
	t_Byte df1;    /* Data field octet 1 */
	t_Byte df2;    /* Data field octet 2 */
	int ix;        /* Auxiliary */
#endif /* LISTER */
	t_Byte rep;    /* Repetition factor */
	t_Retc ret;    /* Return code */
#if LISTER
	t_Byte rs_id;  /* Remote sensor identification */
	t_Byte rs_type;
                   /* Remote sensor type */
	t_Byte status; /* Status */
#endif /* LISTER */

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
	Assert (length == 1 + rep * 2, "Invalid length");

#if LISTER
				   /* List data field: */
	list_text (2, ";  Remote Sensor Detailed Status:\n");
	for (ix = 0; ix < rep; ix ++)
	{
		df1 = buffer[1 + 2 * ix];
		df2 = buffer[1 + 2 * ix + 1];

		rs_id = df1;
		rs_type = (df2 >> 4) & 0x0f;
		status = df2 & 0x0f;

		list_text (2, ";  ");
		list_text (2, " id=" M_FMT_BYTE ";", rs_id);
		list_text (2, " type=" M_FMT_BYTE ";", rs_type);
		list_text (2, " status=" M_FMT_BYTE, status);
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
/* proc_i019_553   -- Process I019/553 data item                              */
/*----------------------------------------------------------------------------*/

 static
 t_Retc proc_i019_553 (t_Ui16 length, t_Byte *buffer)
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
	list_text (2, ";  Reference Transponder Detailed Status:\n");
	for (ix = 0; ix < length; ix ++)
	{
		dfo = buffer[ix];

		list_text (2, ";   0x" M_FMT_BYTE_HEX "\n", dfo);
	}
#endif /* LISTER */

                   /* Store this information: */
	/* tba ... */

				   /* Set the return code: */
	ret = RC_OKAY;

	return ret;
}

/*----------------------------------------------------------------------------*/
/* proc_i019_600   -- Process I019/600 data item                              */
/*----------------------------------------------------------------------------*/

 static
 t_Retc proc_i019_600 (t_Ui16 length, t_Byte *buffer)
{
#if LISTER
	t_Byte df1;    /* Data field octet 1 */
	t_Byte df2;    /* Data field octet 2 */
	t_Byte df3;    /* Data field octet 3 */
	t_Byte df4;    /* Data field octet 4 */
	t_Byte df5;    /* Data field octet 5 */
	t_Byte df6;    /* Data field octet 6 */
	t_Byte df7;    /* Data field octet 7 */
	t_Byte df8;    /* Data field octet 8 */
	t_Si32 lat;    /* Latitude in WGS-84; 180/2**30 degrees */
	t_Real latf;   /* Latitude; degrees */
	t_Si32 lon;    /* Longitude in WGS-84; 180/2**30 degrees */
	t_Real lonf;   /* Longitude; degrees */
#endif /* LISTER */
	t_Retc ret;    /* Return code */

				   /* Preset the return code: */
	ret = RC_FAIL;

				   /* Check parameters: */
	Assert (length == 8, "Invalid parameter");
	Assert (buffer != NULL, "Invalid parameter");

#if LISTER
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

				   /* List data field: */
	list_text (2, ";  Position of the MLT System Reference Point:\n");
	list_text (2, ";   Latitude: " M_FMT_SI32 " (%s)\n",
                  lat, lat_text (latf));
	list_text (2, ";   Longitude: " M_FMT_SI32 " (%s)\n",
                  lon, lon_text (lonf));
#endif /* LISTER */

				   /* Store this information: */
	/* tba ... */

				   /* Set the return code: */
	ret = RC_OKAY;

	return ret;
}

/*----------------------------------------------------------------------------*/
/* proc_i019_610   -- Process I019/610 data item                              */
/*----------------------------------------------------------------------------*/

 static
 t_Retc proc_i019_610 (t_Ui16 length, t_Byte *buffer)
{
#if LISTER
	t_Byte df1;    /* Data field octet 1 */
	t_Byte df2;    /* Data field octet 2 */
	t_Si16 hgt;    /* WGS-84 height; 0.25 metres */
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
	hgt = make_si16 (df1, df2);

				   /* List data field: */
	list_text (2, ";  Height of the MLT System Reference Point:");
	list_text (2, " %.2f mtr", 0.25 * hgt);
	list_text (2, "\n");
#endif /* LISTER */

				   /* Store this information: */
	/* tba ... */

				   /* Set the return code: */
	ret = RC_OKAY;

	return ret;
}

/*----------------------------------------------------------------------------*/
/* proc_i019_620   -- Process I019/620 data item                              */
/*----------------------------------------------------------------------------*/

 static
 t_Retc proc_i019_620 (t_Ui16 length, t_Byte *buffer)
{
#if LISTER
	t_Byte df1;    /* Data field octet 1 */
#endif /* LISTER */
	t_Retc ret;    /* Return code */
#if LISTER
	t_Si16 und;    /* WGS-84 undulation; metres */
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
	if (df1 & 0x80)
	{
		und = make_si16 (0xff, df1);
	}
	else
	{
		und = make_si16 (0x00, df1);
	}

				   /* List data field: */
	list_text (2, ";  WGS-84 Undulation:");
	list_text (2, " " M_FMT_SI16 " mtr", und);
	list_text (2, "\n");
#endif /* LISTER */

				   /* Store this information: */
	/* tba ... */

				   /* Set the return code: */
	ret = RC_OKAY;

	return ret;
}

/*----------------------------------------------------------------------------*/
/* proc_i019_ref   -- Process RE indicator data item                          */
/*----------------------------------------------------------------------------*/

 static
 t_Retc proc_i019_ref (t_Ui16 length, t_Byte *buffer, t_Ui16 *pos_ptr)
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
		error_msg ("Invalid buffer length (I019/REF)");
		goto done;
	}

#if LISTER
				   /* List raw data field: */
	list_buffer (1, ";  I019/RE :", len, buffer + pos);
#endif /* LISTER */

				   /* Set position: */
	*pos_ptr = pos + len;

				   /* Set the return code: */
	ret = RC_OKAY;

	done:          /* We are done */
	return ret;
}

/*----------------------------------------------------------------------------*/
/* proc_i019_spf   -- Process SP indicator data field                         */
/*----------------------------------------------------------------------------*/

 static
 t_Retc proc_i019_spf (t_Ui16 length, t_Byte *buffer, t_Ui16 *pos_ptr)
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
		error_msg ("Invalid buffer length (I019/SP)");
		goto done;
	}

#if LISTER
				   /* List raw data field: */
	list_buffer (1, ";  I019/SP :", len, buffer + pos);
#endif /* LISTER */

				   /* Set position: */
	*pos_ptr = pos + len;

				   /* Set the return code: */
	ret = RC_OKAY;

	done:          /* We are done */
	return ret;
}
/* end-of-file */
