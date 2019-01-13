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
/* File:           src/astx_000.cpp                                           */
/* Contents:       Process ASTERIX category 000 data block                    */
/* Author(s):      kb                                                         */
/* Last change:    2018-04-27                                                 */
/*----------------------------------------------------------------------------*/

/* Reference document:
   -------------------

   User Interface Definition of the MADAP Track Server
   MAS-S/GD_0026_03
   April 1998
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
					  for ASTERIX category 000 */

				   /* Local variables: */
				   /* ---------------- */

static t_Data_Item_Desc desc_i000_010;
                   /* Description of data item I000/010 */
static t_Data_Item_Desc desc_i000_020;
                   /* Description of data item I000/020 */
static t_Data_Item_Desc desc_i000_030;
                   /* Description of data item I000/030 */
static t_Data_Item_Desc desc_i000_040;
                   /* Description of data item I000/040 */
static t_Data_Item_Desc desc_i000_050;
                   /* Description of data item I000/050 */
static t_Data_Item_Desc desc_i000_spf;
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
static t_Retc proc_i000_010 (t_Ui16 length, t_Byte *buffer);
                   /* Process I000/010 data item */
static t_Retc proc_i000_020 (t_Ui16 length, t_Byte *buffer);
                   /* Process I000/020 data item */
static t_Retc proc_i000_030 (t_Ui16 length, t_Byte *buffer);
                   /* Process I000/030 data item */
static t_Retc proc_i000_040 (t_Ui16 length, t_Byte *buffer);
                   /* Process I000/040 data item */
static t_Retc proc_i000_050 (t_Ui16 length, t_Byte *buffer);
                   /* Process I000/050 data item */
static t_Retc proc_i000_spf (t_Ui16 length, t_Byte *buffer, t_Ui16 *pos_ptr);
                   /* Process SPF indicator data item */

/*----------------------------------------------------------------------------*/
/* astx_000        -- Process ASTERIX category 000 data block                 */
/*----------------------------------------------------------------------------*/

 t_Retc astx_000 (t_Ui16 length, t_Byte *buffer)
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

                   /* Preset radar service message: */
		memset (&rsrv, 0, sizeof (rsrv));

                   /* Set type of radar service message: */
		rsrv.rsrv_type = e_rsrv_supervisory;

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

					lrc = data_item (0, frn, std_uap[frn],
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
			rsrv.frame_date.present = TRUE;
			rsrv.frame_date.value = frame_date;
		}

                   /* Add frame time, if available: */
        if (frame_time_present)
        {
            rsrv.frame_time.present = TRUE;
            rsrv.frame_time.value = frame_time;
        }

                   /* Process this radar service message: */
		lrc = process_rsrv (&rsrv);
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
                   /* Description of data item I000/010: */
    desc_i000_010.category = 0;
    desc_i000_010.data_item = 10;
    desc_i000_010.item_type = e_fixed_length_data_item;
    desc_i000_010.fixed_length = 2;
    desc_i000_010.proc_fptr = proc_i000_010;
    desc_i000_010.read_fptr = NULL;

                   /* Description of data item I000/020: */
    desc_i000_020.category = 0;
    desc_i000_020.data_item = 20;
    desc_i000_020.item_type = e_fixed_length_data_item;
    desc_i000_020.fixed_length = 3;
    desc_i000_020.proc_fptr = proc_i000_020;
    desc_i000_020.read_fptr = NULL;

                   /* Description of data item I000/030: */
    desc_i000_030.category = 0;
    desc_i000_030.data_item = 30;
    desc_i000_030.item_type = e_fixed_length_data_item;
    desc_i000_030.fixed_length = 1;
    desc_i000_030.proc_fptr = proc_i000_030;
    desc_i000_030.read_fptr = NULL;

                   /* Description of data item I000/040: */
    desc_i000_040.category = 0;
    desc_i000_040.data_item = 40;
    desc_i000_040.item_type = e_repetitive_data_item;
    desc_i000_040.fixed_length = 3;
    desc_i000_040.proc_fptr = proc_i000_040;
    desc_i000_040.read_fptr = NULL;

                   /* Description of data item I000/050: */
    desc_i000_050.category = 0;
    desc_i000_050.data_item = 50;
    desc_i000_050.item_type = e_fixed_length_data_item;
    desc_i000_050.fixed_length = 1;
    desc_i000_050.proc_fptr = proc_i000_050;
    desc_i000_050.read_fptr = NULL;

                   /* Description of SPF data item: */
    desc_i000_spf.category = 0;
    desc_i000_spf.data_item = M_SPF_INDICATOR;
    desc_i000_spf.item_type = e_immediate_data_item;
    desc_i000_spf.fixed_length = 0;
    desc_i000_spf.proc_fptr = NULL;
    desc_i000_spf.read_fptr = proc_i000_spf;

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

	std_uap[1] = &desc_i000_010;
	std_uap[2] = &desc_i000_020;
	std_uap[3] = &desc_i000_030;
	std_uap[4] = &desc_i000_040;
	std_uap[5] = &desc_i000_050;

	std_uap[14] = &desc_i000_spf;

	return;
}

/*----------------------------------------------------------------------------*/
/* proc_i000_010   -- Process I000/010 data item                              */
/*----------------------------------------------------------------------------*/

 static
 t_Retc proc_i000_010 (t_Ui16 length, t_Byte *buffer)
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
	rsrv.data_source_identifier.present = TRUE;
	rsrv.data_source_identifier.value = dsi;
    rsrv.data_source_identifier.sac = df1;
    rsrv.data_source_identifier.sic = df2;

				   /* Set the return code: */
	ret = RC_OKAY;

	return ret;
}

/*----------------------------------------------------------------------------*/
/* proc_i000_020   -- Process I000/020 data item                              */
/*----------------------------------------------------------------------------*/

 static
 t_Retc proc_i000_020 (t_Ui16 length, t_Byte *buffer)
{
	t_Byte df1;    /* Data field octet 1 */
	t_Byte df2;    /* Data field octet 2 */
	t_Byte df3;    /* Data field octet 3 */
	t_Retc ret;    /* Return code */
	t_Secs tm;     /* Time (of day); seconds */
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
	tm = (1.0 / 128.0) * tod;

#if LISTER
				   /* List data field: */
	list_text (2, ";  Time of Day:");
	list_text (2, " " M_FMT_ATOD " (" M_FMT_UI32 "; %s UTC)",
                  tod, tod, utc_text (tm));
	list_text (2, "\n");
#endif /* LISTER */

                   /* Remember this information: */
	last_tod_available = TRUE;
	last_tod = tod;

				   /* Store this information: */
	rsrv.message_time.present = TRUE;
	rsrv.message_time.value = tm;
	rsrv.time_of_day.present = TRUE;
	rsrv.time_of_day.value = tod;

				   /* Set the return code: */
	ret = RC_OKAY;

	return ret;
}

/*----------------------------------------------------------------------------*/
/* proc_i000_030   -- Process I000/030 data item                              */
/*----------------------------------------------------------------------------*/

 static
 t_Retc proc_i000_030 (t_Ui16 length, t_Byte *buffer)
{
	t_Byte df1;    /* Data field octet 1 */
	t_Retc ret;    /* Return code */
	t_Byte stn;    /* Step reference number */

				   /* Preset the return code: */
	ret = RC_FAIL;

				   /* Check parameters: */
	Assert (length == 1, "Invalid parameter");
	Assert (buffer != NULL, "Invalid parameter");

				   /* Extract octets: */
	df1 = buffer[0];

				   /* Extract information: */
	stn = df1;

#if LISTER
				   /* List data field: */
	list_text (2, ";  Step Reference Number:");
	list_text (2, " " M_FMT_BYTE, stn);
	list_text (2, "\n");
#endif /* LISTER */

				   /* Store this information: */
	rsrv.step_number.present = TRUE;
	rsrv.step_number.value = stn;

				   /* Set the return code: */
	ret = RC_OKAY;

	return ret;
}

/*----------------------------------------------------------------------------*/
/* proc_i000_040   -- Process I000/040 data item                              */
/*----------------------------------------------------------------------------*/

 static
 t_Retc proc_i000_040 (t_Ui16 length, t_Byte *buffer)
{
	t_Byte df1;    /* Data field octet 1 */
	t_Byte df2;    /* Data field octet 2 */
	t_Byte df3;    /* Data field octet 3 */
	t_Ui16 dsi;    /* Data source identifier */
	int ix;        /* Auxiliary */
	t_Byte num;    /* Number of radars */
	t_Retc ret;    /* Return code */
	t_Byte sac;    /* Source area code */
	t_Byte sic;    /* Source identification code */

				   /* Preset the return code: */
	ret = RC_FAIL;

				   /* Check parameters: */
	Assert (length > 0, "Invalid parameter");
	Assert (buffer != NULL, "Invalid parameter");

                   /* Extract number of radars: */
	df1 = buffer[0];
	num = df1;

                   /* Check number of radars: */
	if (num == 0)
	{
		error_msg ("No radars");

		ret = RC_SKIP;
		goto done;
	}

                   /* Check length (again): */
	Assert (length == 1 + 3 * num, "Invalid length");

#if LISTER
				   /* List data field: */
	list_text (2, ";  Radar Configuration and Status:");
	list_text (2, "\n");
	for (ix = 0; ix < num; ix ++)
	{
		df1 = buffer[1 + ix * 3];
		df2 = buffer[1 + ix * 3 + 1];
		df3 = buffer[1 + ix * 3 + 2];

		sac = df1;
		sic = df2;
		dsi = make_ui16 (sac, sic);

		list_text (2, ";   0x" M_FMT_UI16_HEX, dsi);
		list_text (2, " (SAC=" M_FMT_BYTE "; SIC=" M_FMT_BYTE "):", sac, sic);
		list_text (2, " " M_FMT_BYTE_HEX, df3);
		list_text (2, "\n");
	}
#endif /* LISTER */

				   /* Store this information: */
	if (num > M_MAX_NUMBER_OF_SENSORS)
	{
		error_msg ("Buffer overrun - too many sensors");
		goto done;
	}
	rsrv.radar_configuration.count = num;
	rsrv.radar_configuration.present = TRUE;
	for (ix = 0; ix < num; ix ++)
	{
		df1 = buffer[1 + ix * 3];
		df2 = buffer[1 + ix * 3 + 1];
		df3 = buffer[1 + ix * 3 + 2];

		sac = df1;
		sic = df2;
		dsi = make_ui16 (sac, sic);

		rsrv.radar_configuration.value[ix].dsi = dsi;
		rsrv.radar_configuration.value[ix].value = df3;
	}

				   /* Set the return code: */
	ret = RC_OKAY;

	done:          /* We are done */
	return ret;
}

/*----------------------------------------------------------------------------*/
/* proc_i000_050   -- Process I000/050 data item                              */
/*----------------------------------------------------------------------------*/

 static
 t_Retc proc_i000_050 (t_Ui16 length, t_Byte *buffer)
{
	t_Byte df1;    /* Data field octet 1 */
	t_Byte pst;    /* Processing status */
	t_Retc ret;    /* Return code */

				   /* Preset the return code: */
	ret = RC_FAIL;

				   /* Check parameters: */
	Assert (length == 1, "Invalid parameter");
	Assert (buffer != NULL, "Invalid parameter");

				   /* Extract octet: */
	df1 = buffer[0];

				   /* Extract information: */
	pst = df1;

#if LISTER
				   /* List data field: */
	list_text (2, ";  Processing Status:");
	list_text (2, " 0x " M_FMT_BYTE_HEX, pst);
	list_text (2, "\n");
#endif /* LISTER */

				   /* Store this information: */
	rsrv.processing_status.present = TRUE;
	rsrv.processing_status.value = pst;

				   /* Set the return code: */
	ret = RC_OKAY;

	return ret;
}

/*----------------------------------------------------------------------------*/
/* proc_i000_spf   -- Process SPF indicator data item                         */
/*----------------------------------------------------------------------------*/

 static
 t_Retc proc_i000_spf (t_Ui16 length, t_Byte *buffer, t_Ui16 *pos_ptr)
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
	list_buffer (1, ";  I000/SPF:", len, buffer + pos);
#endif /* LISTER */

				   /* Set position: */
	*pos_ptr = pos + len;

				   /* Set the return code: */
	ret = RC_OKAY;

	done:          /* We are done */
	return ret;
}
/* end-of-file */
