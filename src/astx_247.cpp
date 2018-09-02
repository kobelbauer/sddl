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
/* File:           src/astx_247.cpp                                           */
/* Contents:       Process ASTERIX category 247 data block                    */
/* Author(s):      kb                                                         */
/* Last change:    2018-04-27                                                 */
/*----------------------------------------------------------------------------*/

/* Reference document:
   -------------------

   Eurocontrol Standard Document for Surveillance Data Exchange.
   Part 20: Category 247 - Version Number Exchange.
   Edition: 1.2
   Edition Date: February 2008
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

#define M_MAX_FRN 7
				   /* Maximum field reference number */
#define M_MAX_FSPEC_LENGTH 1
				   /* Maximum fields specification length
					  for ASTERIX category 247 */

				   /* Local variables: */
				   /* ---------------- */

static t_Data_Item_Desc desc_i247_010;
                   /* Description of data item I247/010 */
static t_Data_Item_Desc desc_i247_015;
                   /* Description of data item I247/015 */
static t_Data_Item_Desc desc_i247_140;
                   /* Description of data item I247/140 */
static t_Data_Item_Desc desc_i247_550;
                   /* Description of data item I247/550 */
static t_Data_Item_Desc desc_i247_ref;
                   /* Description of data item I247/REF */
static t_Data_Item_Desc desc_i247_spf;
                   /* Description of data item I247/SPF */
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
static t_Retc proc_i247_010 (t_Ui16 length, t_Byte *buffer);
				   /* Process I247/010 data field */
static t_Retc proc_i247_015 (t_Ui16 length, t_Byte *buffer);
				   /* Process I247/015 data field */
static t_Retc proc_i247_140 (t_Ui16 length, t_Byte *buffer);
				   /* Process I247/140 data field */
static t_Retc proc_i247_550 (t_Ui16 length, t_Byte *buffer);
				   /* Process I247/550 data field */
static t_Retc proc_i247_ref (t_Ui16 length, t_Byte *buffer, t_Ui16 *pos_ptr);
                   /* Process REF indicator data item */
static t_Retc proc_i247_spf (t_Ui16 length, t_Byte *buffer, t_Ui16 *pos_ptr);
                   /* Process SPF indicator data item */

/*----------------------------------------------------------------------------*/
/* astx_247        -- Process ASTERIX category 247 data block                 */
/*----------------------------------------------------------------------------*/

 t_Retc astx_247 (t_Ui16 length, t_Byte *buffer)
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

					lrc = data_item (247, frn, std_uap[frn],
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
                   /* Description of data item I247/010: */
	desc_i247_010.category = 247;
	desc_i247_010.data_item = 10;
	desc_i247_010.item_type = e_fixed_length_data_item;
	desc_i247_010.fixed_length = 2;
	desc_i247_010.proc_fptr = proc_i247_010;
	desc_i247_010.read_fptr = NULL;

                   /* Description of data item I247/015: */
	desc_i247_015.category = 247;
	desc_i247_015.data_item = 15;
	desc_i247_015.item_type = e_fixed_length_data_item;
	desc_i247_015.fixed_length = 1;
	desc_i247_015.proc_fptr = proc_i247_015;
	desc_i247_015.read_fptr = NULL;

                   /* Description of data item I247/140: */
	desc_i247_140.category = 247;
	desc_i247_140.data_item = 140;
	desc_i247_140.item_type = e_fixed_length_data_item;
	desc_i247_140.fixed_length = 3;
	desc_i247_140.proc_fptr = proc_i247_140;
	desc_i247_140.read_fptr = NULL;

                   /* Description of data item I247/550: */
	desc_i247_550.category = 247;
	desc_i247_550.data_item = 550;
	desc_i247_550.item_type = e_repetitive_data_item;
	desc_i247_550.fixed_length = 3;
	desc_i247_550.proc_fptr = proc_i247_550;
	desc_i247_550.read_fptr = NULL;

                   /* Description of data item I247/REF: */
	desc_i247_ref.category = 247;
	desc_i247_ref.data_item = M_REF_INDICATOR;
	desc_i247_ref.item_type = e_immediate_data_item;
	desc_i247_ref.fixed_length = 0;
	desc_i247_ref.proc_fptr = NULL;
	desc_i247_ref.read_fptr = proc_i247_ref;

                   /* Description of data item I247/SPF: */
	desc_i247_spf.category = 247;
	desc_i247_spf.data_item = M_SPF_INDICATOR;
	desc_i247_spf.item_type = e_immediate_data_item;
	desc_i247_spf.fixed_length = 0;
	desc_i247_spf.proc_fptr = NULL;
	desc_i247_spf.read_fptr = proc_i247_spf;

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

                   /* This is the standard UAP */

	std_uap[1] = &desc_i247_010;
    std_uap[2] = &desc_i247_015;
	std_uap[3] = &desc_i247_140;
	std_uap[4] = &desc_i247_550;
	std_uap[6] = &desc_i247_spf;
	std_uap[7] = &desc_i247_ref;

	return;
}

/*----------------------------------------------------------------------------*/
/* proc_i247_010   -- Process I247/010 data item                              */
/*----------------------------------------------------------------------------*/

 static
 t_Retc proc_i247_010 (t_Ui16 length, t_Byte *buffer)
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
/* proc_i247_015   -- Process I247/015 data item                              */
/*----------------------------------------------------------------------------*/

 static
 t_Retc proc_i247_015 (t_Ui16 length, t_Byte *buffer)
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

				   /* List data field: */
	list_text (2, ";  Service Identification:");
	list_text (2, " " M_FMT_BYTE, df1);
	list_text (2, "\n");
#endif /* LISTER */

				   /* Set the return code: */
	ret = RC_OKAY;

	return ret;
}

/*----------------------------------------------------------------------------*/
/* proc_i247_140   -- Process I247/140 data item                              */
/*----------------------------------------------------------------------------*/

 static
 t_Retc proc_i247_140 (t_Ui16 length, t_Byte *buffer)
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

                   /* Convert to milliseconds: */
	tod_in_secs = (1.0 / 128.0) * tod;

				   /* List data field: */
	list_text (2, ";  Time of Day:");
	list_text (2, " " M_FMT_ATOD " (" M_FMT_UI32 "; %.6f secs; %s UTC)",
                  tod, tod, tod_in_secs, utc_text (tod_in_secs));
	list_text (2, "\n");
#endif /* LISTER */

				   /* Set the return code: */
	ret = RC_OKAY;

	return ret;
}

/*----------------------------------------------------------------------------*/
/* proc_i247_550   -- Process I247/550 data item                              */
/*----------------------------------------------------------------------------*/

 static
 t_Retc proc_i247_550 (t_Ui16 length, t_Byte *buffer)
{
#if LISTER
	t_Byte df1;    /* Data field octet 1 */
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

                   /* Extract the repetition factor: */
	rep = buffer[0];

                   /* Check data item length (again): */
	Assert (length == 1 + 3 * rep, "Invalid length");

				   /* Extract information: */
	/* tba ... */

#if LISTER
				   /* List data field: */
	list_text (2, ";  Category Version Number Report:");
	list_text (2, "\n");
	for (ix = 0; ix < rep; ix ++)
	{
		df1 = buffer[1 + 3 * ix];
		df2 = buffer[1 + 3 * ix + 1];
		df3 = buffer[1 + 3 * ix + 2];

		list_text (2, ";   Category " M_FMT_BYTE ": Version "
                      M_FMT_BYTE "." M_FMT_BYTE "\n",
                      df1, df2, df3);
	}
#endif /* LISTER */

				   /* Store this information: */
	/* tba ... */

				   /* Set the return code: */
	ret = RC_OKAY;

	return ret;
}

/*----------------------------------------------------------------------------*/
/* proc_i247_ref   -- Process REF indicator data item                         */
/*----------------------------------------------------------------------------*/

 static
 t_Retc proc_i247_ref (t_Ui16 length, t_Byte *buffer, t_Ui16 *pos_ptr)
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
		error_msg ("Invalid buffer length (I247/REF)");
		goto done;
	}

#if LISTER
				   /* List raw data field: */
	list_buffer (1, ";  I247/REF:", len, buffer + pos);
#endif /* LISTER */

				   /* Set position: */
	*pos_ptr = pos + len;

				   /* Set the return code: */
	ret = RC_OKAY;

	done:          /* We are done */
	return ret;
}

/*----------------------------------------------------------------------------*/
/* proc_i247_spf   -- Process SPF indicator data item                         */
/*----------------------------------------------------------------------------*/

 static
 t_Retc proc_i247_spf (t_Ui16 length, t_Byte *buffer, t_Ui16 *pos_ptr)
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
		error_msg ("Invalid buffer length (I247/SPF)");
		goto done;
	}

#if LISTER
				   /* List raw data field: */
	list_buffer (1, ";  I247/SPF:", len, buffer + pos);
#endif /* LISTER */

				   /* Set position: */
	*pos_ptr = pos + len;

				   /* Set the return code: */
	ret = RC_OKAY;

	done:          /* We are done */
	return ret;
}
/* end-of-file */
