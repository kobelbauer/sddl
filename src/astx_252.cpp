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
/* File:           src/astx_252.cpp                                           */
/* Contents:       Process ASTERIX category 252 data block                    */
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

#define M_MAX_FRN 7
				   /* Maximum field reference number */
#define M_MAX_FSPEC_LENGTH 1
				   /* Maximum fields specification length
					  for ASTERIX category 252 */

				   /* Local variables: */
				   /* ---------------- */

static t_Data_Item_Desc desc_i252_010;
                   /* Description of data item I252/010 */
static t_Data_Item_Desc desc_i252_015;
                   /* Description of data item I252/015 */
static t_Data_Item_Desc desc_i252_020;
                   /* Description of data item I252/020 */
static t_Data_Item_Desc desc_i252_035;
                   /* Description of data item I252/035 */
static t_Data_Item_Desc desc_i252_110;
                   /* Description of data item I252/110 */
static t_Data_Item_Desc desc_i252_330;
                   /* Description of data item I252/330 */
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
static t_Retc proc_i252_010 (t_Ui16 length, t_Byte *buffer);
                   /* Process I252/010 data item */
static t_Retc proc_i252_015 (t_Ui16 length, t_Byte *buffer);
                   /* Process I252/015 data item */
static t_Retc proc_i252_020 (t_Ui16 length, t_Byte *buffer);
                   /* Process I252/020 data item */
static t_Retc proc_i252_035 (t_Ui16 length, t_Byte *buffer);
                   /* Process I252/035 data item */
static t_Retc proc_i252_110 (t_Ui16 length, t_Byte *buffer);
                   /* Process I252/110 data item */
static t_Retc proc_i252_330 (t_Ui16 length, t_Byte *buffer);
                   /* Process I252/330 data item */

/*----------------------------------------------------------------------------*/
/* astx_252        -- Process ASTERIX category 252 data block                 */
/*----------------------------------------------------------------------------*/

 t_Retc astx_252 (t_Ui16 length, t_Byte *buffer)
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

					lrc = data_item (252, frn, std_uap[frn],
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
                   /* Description of data item I252/010: */
	desc_i252_010.category = 252;
	desc_i252_010.data_item = 10;
	desc_i252_010.item_type = e_fixed_length_data_item;
	desc_i252_010.fixed_length = 2;
	desc_i252_010.proc_fptr = proc_i252_010;
	desc_i252_010.read_fptr = NULL;

                   /* Description of data item I252/015: */
	desc_i252_015.category = 252;
	desc_i252_015.data_item = 15;
	desc_i252_015.item_type = e_fixed_length_data_item;
	desc_i252_015.fixed_length = 2;
	desc_i252_015.proc_fptr = proc_i252_015;
	desc_i252_015.read_fptr = NULL;

                   /* Description of data item I252/020: */
	desc_i252_020.category = 252;
	desc_i252_020.data_item = 20;
	desc_i252_020.item_type = e_fixed_length_data_item;
	desc_i252_020.fixed_length = 3;
	desc_i252_020.proc_fptr = proc_i252_020;
	desc_i252_020.read_fptr = NULL;

                   /* Description of data item I252/035: */
	desc_i252_035.category = 252;
	desc_i252_035.data_item = 35;
	desc_i252_035.item_type = e_fixed_length_data_item;
	desc_i252_035.fixed_length = 1;
	desc_i252_035.proc_fptr = proc_i252_035;
	desc_i252_035.read_fptr = NULL;

                   /* Description of data item I252/110: */
	desc_i252_110.category = 252;
	desc_i252_110.data_item = 110;
	desc_i252_110.item_type = e_variable_length_data_item;
	desc_i252_110.fixed_length = 0;
	desc_i252_110.proc_fptr = proc_i252_110;
	desc_i252_110.read_fptr = NULL;

                   /* Description of data item I252/330: */
	desc_i252_330.category = 252;
	desc_i252_330.data_item = 330;
	desc_i252_330.item_type = e_repetitive_data_item;
	desc_i252_330.fixed_length = 2;
	desc_i252_330.proc_fptr = proc_i252_330;
	desc_i252_330.read_fptr = NULL;

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

	std_uap[1] = &desc_i252_010;
	std_uap[2] = &desc_i252_015;
	std_uap[3] = &desc_i252_020;
	std_uap[4] = &desc_i252_035;
	std_uap[5] = &desc_i252_110;
	std_uap[6] = &desc_i252_330;

	return;
}

/*----------------------------------------------------------------------------*/
/* proc_i252_010   -- Process I252/010 data item                              */
/*----------------------------------------------------------------------------*/

 static
 t_Retc proc_i252_010 (t_Ui16 length, t_Byte *buffer)
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
	list_text (2, ";  Server Identification Tag:");
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
/* proc_i252_015   -- Process I252/015 data item                              */
/*----------------------------------------------------------------------------*/

 static
 t_Retc proc_i252_015 (t_Ui16 length, t_Byte *buffer)
{
#if LISTER
	t_Byte df1;    /* Data field octet 1 */
	t_Byte df2;    /* Data field octet 2 */
#endif /* LISTER */
	t_Retc ret;    /* Return code */
#if LISTER
	t_Ui16 usr;    /* User number */
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
	usr = make_ui16 (df1, df2);

				   /* List data field: */
	list_text (2, ";  User Number:");
	list_text (2, " " M_FMT_BYTE, usr);
	list_text (2, "\n");
#endif /* LISTER */

				   /* Store this information: */
	/* tba ... */

				   /* Set the return code: */
	ret = RC_OKAY;

	return ret;
}

/*----------------------------------------------------------------------------*/
/* proc_i252_020   -- Process I252/020 data item                              */
/*----------------------------------------------------------------------------*/

 static
 t_Retc proc_i252_020 (t_Ui16 length, t_Byte *buffer)
{
#if LISTER
	t_Byte df1;    /* Data field octet 1 */
	t_Byte df2;    /* Data field octet 2 */
	t_Byte df3;    /* Data field octet 3 */
#endif /* LISTER */
	t_Retc ret;    /* Return code */
#if LISTER
	t_Ui32 tod;    /* Time of day; 1/128 seconds */
	t_Secs tod_in_secs;
                   /* Time of day; seconds */
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

				   /* Compute time (of day): */
	tod_in_secs = (1.0 / 128.0) * tod;

				   /* List data field: */
	list_text (2, ";  Time of Message:");
	list_text (2, " " M_FMT_ATOD " (" M_FMT_UI32 "; %s UTC)",
                  tod, tod, utc_text (tod_in_secs));
	list_text (2, "\n");
#endif /* LISTER */

				   /* Store this information: */
	/* tba ... */

				   /* Set the return code: */
	ret = RC_OKAY;

	return ret;
}

/*----------------------------------------------------------------------------*/
/* proc_i252_035   -- Process I252/035 data item                              */
/*----------------------------------------------------------------------------*/

 static
 t_Retc proc_i252_035 (t_Ui16 length, t_Byte *buffer)
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
	list_text (2, " nature=" M_FMT_BYTE, family, nature);
	list_text (2, "\n");
	if (family == 1)
	{
		list_text (2, ";   Connections:");
		if (nature == 1)
		{
			list_text (2, " request for connection");
		}
		else if (nature == 2)
		{
			list_text (2, " request for disconnection");
		}
		else if (nature == 3)
		{
			list_text (2, " connection related report");
		}
		list_text (2, "\n");
	}
	else if (family == 2)
	{
		list_text (2, ";   Track information service definition:");
		if (nature == 1)
		{
			list_text (2, " background service");
		}
		else if (nature == 2)
		{
			list_text (2, " synchronised complementary service");
		}
		else if (nature == 3)
		{
			list_text (2, " independent complementary service");
		}
		else if (nature == 4)
		{
			list_text (2, " service request update:"
                          " stop track/item transmission");
		}
		else if (nature == 5)
		{
			list_text (2, " service request update:"
                          " start track/item transmission");
		}
		else if (nature == 6)
		{
			list_text (2, " service control request: interruption");
		}
		else if (nature == 7)
		{
			list_text (2, " service control request: restart");
		}
		else if (nature == 8)
		{
			list_text (2, " service control request: end of service");
		}
		list_text (2, "\n");
	}
	else if (family == 3)
	{
		list_text (2, ";   Server messages:");
		if (nature == 1)
		{
			list_text (2, " server status message");
		}
		else if (nature == 2)
		{
			list_text (2, " track service related report");
		}
		else if (nature == 3)
		{
			list_text (2, " sensor information service related report");
		}
		list_text (2, "\n");
	}
	else if (family == 4)
	{
		list_text (2, ";   Sensor information service definition:");
		if (nature == 1)
		{
			list_text (2, " service definition");
		}
		else if (nature == 2)
		{
			list_text (2, " service control request: interruption");
		}
		else if (nature == 3)
		{
			list_text (2, " service control request: restart");
		}
		else if (nature == 4)
		{
			list_text (2, " service control request: end of service");
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
/* proc_i252_110   -- Process I252/110 data item                              */
/*----------------------------------------------------------------------------*/

 static
 t_Retc proc_i252_110 (t_Ui16 length, t_Byte *buffer)
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
	list_text (2, ";  Service Identification:");
	for (ix = 0; ix < length; ix ++)
	{
		df1 = buffer[ix];

		if (ix == 0)
		{
			if (df1 & 0x04)
			{
				list_text (2, " BS");
			}

			if (df1 & 0x02)
			{
				list_text (2, " C1");
			}
		}
		else if (ix == 1)
		{
			if (df1 & 0x80)
			{
				list_text (2, " C2");
			}

			if (df1 & 0x40)
			{
				list_text (2, " C3");
			}

			if (df1 & 0x20)
			{
				list_text (2, " C4");
			}

			if (df1 & 0x10)
			{
				list_text (2, " C5");
			}
		}
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
/* proc_i252_330   -- Process I252/330 data item                              */
/*----------------------------------------------------------------------------*/

 static
 t_Retc proc_i252_330 (t_Ui16 length, t_Byte *buffer)
{
#if LISTER
	t_Ui16 code;   /* Report code */
	t_Byte df1;    /* Data field octet 1 */
	t_Byte df2;    /* Data field octet 2 */
	int ix;        /* Auxiliary */
	t_Byte nature; /* Nature of message */
#endif /* LISTER */
	t_Byte rep;    /* Repetition factor */
	t_Retc ret;    /* Return code */

				   /* Preset the return code: */
	ret = RC_FAIL;

				   /* Check parameters: */
	Assert (length >= 1, "Invalid parameter");
	Assert (buffer != NULL, "Invalid parameter");

                   /* Extract repetition factor: */
	rep = buffer[0];

                   /* Check length (again): */
	Assert (length == 1 + rep * 2, "Invalid length");

#if LISTER
				   /* List data field: */
	for (ix = 0; ix < rep; ix ++)
	{
		df1 = buffer[1 + 2 * ix];
		df2 = buffer[1 + 2 * ix + 1];

		list_text (2, ";  Service Related Report:");

		nature = (df1 >> 3) & 0x1f;
		code = make_ui16 (df1 & 0x07, df2);

		list_text (2, " nature=" M_FMT_BYTE ";", nature);
		list_text (2, " code=" M_FMT_UI16, code);
		list_text (2, "\n");

		if (nature == 8)
		{
			list_text (2, ";   Service synchronisation report:"
                          " batch/sector " M_FMT_UI16, code);
			list_text (2, "\n");
		}
	}
#endif /* LISTER */

				   /* Store this information: */
	/* tba ... */

				   /* Set the return code: */
	ret = RC_OKAY;

	return ret;
}
/* end-of-file */
