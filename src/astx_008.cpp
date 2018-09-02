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
/* File:           src/astx_008.cpp                                           */
/* Contents:       Process ASTERIX category 008 data block                    */
/* Author(s):      kb                                                         */
/* Last change:    2018-04-27                                                 */
/*----------------------------------------------------------------------------*/

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
					  for ASTERIX category 008 */

				   /* Local variables: */
				   /* ---------------- */

static t_Data_Item_Desc desc_i008_000;
                   /* Description of data item I008/000 */
static t_Data_Item_Desc desc_i008_010;
                   /* Description of data item I008/010 */
static t_Data_Item_Desc desc_i008_020;
                   /* Description of data item I008/020 */
static t_Data_Item_Desc desc_i008_034;
                   /* Description of data item I008/034 */
static t_Data_Item_Desc desc_i008_036;
                   /* Description of data item I008/036 */
static t_Data_Item_Desc desc_i008_038;
                   /* Description of data item I008/038 */
static t_Data_Item_Desc desc_i008_040;
                   /* Description of data item I008/040 */
static t_Data_Item_Desc desc_i008_050;
                   /* Description of data item I008/050 */
static t_Data_Item_Desc desc_i008_090;
                   /* Description of data item I008/090 */
static t_Data_Item_Desc desc_i008_100;
                   /* Description of data item I008/100 */
static t_Data_Item_Desc desc_i008_110;
                   /* Description of data item I008/110 */
static t_Data_Item_Desc desc_i008_120;
                   /* Description of data item I008/120 */
static t_Data_Item_Desc desc_i008_rfs;
                   /* Description of RFS data item */
static t_Data_Item_Desc desc_i008_spf;
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
static t_Retc proc_i008_000 (t_Ui16 length, t_Byte *buffer);
                   /* Process I008/000 data item */
static t_Retc proc_i008_010 (t_Ui16 length, t_Byte *buffer);
                   /* Process I008/010 data item */
static t_Retc proc_i008_020 (t_Ui16 length, t_Byte *buffer);
                   /* Process I008/020 data item */
static t_Retc proc_i008_034 (t_Ui16 length, t_Byte *buffer);
                   /* Process I008/034 data item */
static t_Retc proc_i008_036 (t_Ui16 length, t_Byte *buffer);
                   /* Process I008/036 data item */
static t_Retc proc_i008_038 (t_Ui16 length, t_Byte *buffer);
                   /* Process I008/038 data item */
static t_Retc proc_i008_040 (t_Ui16 length, t_Byte *buffer);
                   /* Process I008/040 data item */
static t_Retc proc_i008_050 (t_Ui16 length, t_Byte *buffer);
                   /* Process I008/050 data item */
static t_Retc proc_i008_090 (t_Ui16 length, t_Byte *buffer);
                   /* Process I008/090 data item */
static t_Retc proc_i008_100 (t_Ui16 length, t_Byte *buffer, t_Ui16 *pos_ptr);
                   /* Process I008/100 data item */
static t_Retc proc_i008_110 (t_Ui16 length, t_Byte *buffer);
                   /* Process I008/110 data item */
static t_Retc proc_i008_120 (t_Ui16 length, t_Byte *buffer);
                   /* Process I008/120 data item */
static t_Retc proc_i008_rfs (t_Ui16 length, t_Byte *buffer, t_Ui16 *pos_ptr);
                   /* Process RFS indicator data item */
static t_Retc proc_i008_spf (t_Ui16 length, t_Byte *buffer, t_Ui16 *pos_ptr);
                   /* Process SPF indicator data item */

/*----------------------------------------------------------------------------*/
/* astx_008        -- Process ASTERIX category 008 data block                 */
/*----------------------------------------------------------------------------*/

 t_Retc astx_008 (t_Ui16 length, t_Byte *buffer)
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

					lrc = data_item (8, frn, std_uap[frn],
                                    length, buffer, &pos);
					if (lrc != RC_OKAY && lrc != RC_SKIP)
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
                   /* Description of data item I008/000: */
	desc_i008_000.category = 8;
	desc_i008_000.data_item = 0;
	desc_i008_000.item_type = e_fixed_length_data_item;
	desc_i008_000.fixed_length = 1;
	desc_i008_000.proc_fptr = proc_i008_000;
	desc_i008_000.read_fptr = NULL;

                   /* Description of data item I008/010: */
	desc_i008_010.category = 8;
	desc_i008_010.data_item = 10;
	desc_i008_010.item_type = e_fixed_length_data_item;
	desc_i008_010.fixed_length = 2;
	desc_i008_010.proc_fptr = proc_i008_010;
	desc_i008_010.read_fptr = NULL;

                   /* Description of data item I008/020: */
	desc_i008_020.category = 8;
	desc_i008_020.data_item = 20;
	desc_i008_020.item_type = e_variable_length_data_item;
	desc_i008_020.fixed_length = 0;
	desc_i008_020.proc_fptr = proc_i008_020;
	desc_i008_020.read_fptr = NULL;

                   /* Description of data item I008/034: */
	desc_i008_034.category = 8;
	desc_i008_034.data_item = 34;
	desc_i008_034.item_type = e_repetitive_data_item;
	desc_i008_034.fixed_length = 4;
	desc_i008_034.proc_fptr = proc_i008_034;
	desc_i008_034.read_fptr = NULL;

                   /* Description of data item I008/036: */
	desc_i008_036.category = 8;
	desc_i008_036.data_item = 36;
	desc_i008_036.item_type = e_repetitive_data_item;
	desc_i008_036.fixed_length = 3;
	desc_i008_036.proc_fptr = proc_i008_036;
	desc_i008_036.read_fptr = NULL;

                   /* Description of data item I008/038: */
	desc_i008_038.category = 8;
	desc_i008_038.data_item = 38;
	desc_i008_038.item_type = e_repetitive_data_item;
	desc_i008_038.fixed_length = 4;
	desc_i008_038.proc_fptr = proc_i008_038;
	desc_i008_038.read_fptr = NULL;

                   /* Description of data item I008/040: */
	desc_i008_040.category = 8;
	desc_i008_040.data_item = 40;
	desc_i008_040.item_type = e_fixed_length_data_item;
	desc_i008_040.fixed_length = 2;
	desc_i008_040.proc_fptr = proc_i008_040;
	desc_i008_040.read_fptr = NULL;

                   /* Description of data item I008/050: */
	desc_i008_050.category = 8;
	desc_i008_050.data_item = 50;
	desc_i008_050.item_type = e_repetitive_data_item;
	desc_i008_050.fixed_length = 2;
	desc_i008_050.proc_fptr = proc_i008_050;
	desc_i008_050.read_fptr = NULL;

                   /* Description of data item I008/090: */
	desc_i008_090.category = 8;
	desc_i008_090.data_item = 90;
	desc_i008_090.item_type = e_fixed_length_data_item;
	desc_i008_090.fixed_length = 3;
	desc_i008_090.proc_fptr = proc_i008_090;
	desc_i008_090.read_fptr = NULL;

                   /* Description of data item I008/100: */
	desc_i008_100.category = 8;
	desc_i008_100.data_item = 100;
	desc_i008_100.item_type = e_immediate_data_item;
	desc_i008_100.fixed_length = 0;
	desc_i008_100.proc_fptr = NULL;
	desc_i008_100.read_fptr = proc_i008_100;

                   /* Description of data item I008/110: */
	desc_i008_110.category = 8;
	desc_i008_110.data_item = 110;
	desc_i008_110.item_type = e_variable_length_data_item;
	desc_i008_110.fixed_length = 0;
	desc_i008_110.proc_fptr = proc_i008_110;
	desc_i008_110.read_fptr = NULL;

                   /* Description of data item I008/120: */
	desc_i008_120.category = 8;
	desc_i008_120.data_item = 120;
	desc_i008_120.item_type = e_fixed_length_data_item;
	desc_i008_120.fixed_length = 2;
	desc_i008_120.proc_fptr = proc_i008_120;
	desc_i008_120.read_fptr = NULL;

                   /* Description of RFS data item: */
	desc_i008_rfs.category = 8;
	desc_i008_rfs.data_item = M_RFS_INDICATOR;
	desc_i008_rfs.item_type = e_immediate_data_item;
	desc_i008_rfs.fixed_length = 0;
	desc_i008_rfs.proc_fptr = NULL;
	desc_i008_rfs.read_fptr = proc_i008_rfs;

                   /* Description of SPF data item: */
	desc_i008_spf.category = 8;
	desc_i008_spf.data_item = M_SPF_INDICATOR;
	desc_i008_spf.item_type = e_immediate_data_item;
	desc_i008_spf.fixed_length = 0;
	desc_i008_spf.proc_fptr = NULL;
	desc_i008_spf.read_fptr = proc_i008_spf;

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

	std_uap[1] = &desc_i008_010;
	std_uap[2] = &desc_i008_000;
	std_uap[3] = &desc_i008_020;
	std_uap[4] = &desc_i008_036;
	std_uap[5] = &desc_i008_034;
	std_uap[6] = &desc_i008_040;
	std_uap[7] = &desc_i008_050;

	std_uap[8] = &desc_i008_090;
	std_uap[9] = &desc_i008_100;
	std_uap[10] = &desc_i008_110;
	std_uap[11] = &desc_i008_120;
	std_uap[12] = &desc_i008_038;
	std_uap[13] = &desc_i008_spf;
	std_uap[14] = &desc_i008_rfs;

	return;
}

/*----------------------------------------------------------------------------*/
/* proc_i008_000   -- Process I008/000 data item                              */
/*----------------------------------------------------------------------------*/

 static
 t_Retc proc_i008_000 (t_Ui16 length, t_Byte *buffer)
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
				   /* Extract octet: */
	df1 = buffer[0];

				   /* Extract information: */
	mtp = df1;

				   /* List data field: */
	list_text (2, ";  Message Type: ");
	if (mtp == 1)
	{
		list_text (2, "Polar vector");
	}
	else if (mtp == 2)
	{
		list_text (2, "Cartesian vector of start point and length");
	}
	else if (mtp == 3)
	{
		list_text (2, "Contour record");
	}
	else if (mtp == 4)
	{
		list_text (2, "Cartesian start point and end point vector");
	}
	else if (mtp == 254)
	{
		list_text (2, "SOP message");
	}
	else if (mtp == 255)
	{
		list_text (2, "EOP message");
	}
	else
	{
		list_text (2, "unknown (" M_FMT_BYTE ")", mtp);
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
/* proc_i008_010   -- Process I008/010 data item                              */
/*----------------------------------------------------------------------------*/

 static
 t_Retc proc_i008_010 (t_Ui16 length, t_Byte *buffer)
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

				   /* Store this information: */
	/* tba ... */

				   /* Set the return code: */
	ret = RC_OKAY;

	return ret;
}

/*----------------------------------------------------------------------------*/
/* proc_i008_020   -- Process I008/020 data item                              */
/*----------------------------------------------------------------------------*/

 static
 t_Retc proc_i008_020 (t_Ui16 length, t_Byte *buffer)
{
#if LISTER
	t_Byte df1;    /* Data field octet 1 */
	t_Byte intensity;
                   /* Intensity level */
	int ix;        /* Auxiliary */
#endif /* LISTER */
	t_Retc ret;    /* Return code */
#if LISTER
	t_Byte shading;
                   /* Shading orientation with respect to North */
#endif /* LISTER */

				   /* Preset the return code: */
	ret = RC_FAIL;

				   /* Check parameters: */
	Assert (length > 0, "Invalid parameter");
	Assert (buffer != NULL, "Invalid parameter");

#if LISTER
				   /* List data field: */
	list_text (2, ";  Vector Qualifier: ");
	for (ix = 0; ix < length; ix ++)
	{
		df1 = buffer[ix];
		if (ix == 0)
		{
			if (df1 & 0x80)
			{
				list_text (2, "system coordinates, ");
			}
			else
			{
				list_text (2, "local coordinates, ");
			}
			intensity = (df1 >> 4) & 0x07;
			list_text (2, "intensity " M_FMT_BYTE ", ", intensity);
			shading = (df1 >> 1) & 0x07;
			list_text (2, " shading %5.1f degrees", 22.5 * shading);
		}
		else if (ix == 1)
		{
			if (df1 & 0x04)
			{
				list_text (2, ", test vector");
			}
			if (df1 & 0x02)
			{
				list_text (2, ", error condition");
			}
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
/* proc_i008_034   -- Process I008/034 data item                              */
/*----------------------------------------------------------------------------*/

 static
 t_Retc proc_i008_034 (t_Ui16 length, t_Byte *buffer)
{
#if LISTER
	t_Ui16 azimuth;
                   /* Azimuth */
	t_Byte df1;    /* Data field octet 1 */
	t_Byte df2;    /* Data field octet 2 */
	t_Byte df3;    /* Data field octet 3 */
	t_Byte df4;    /* Data field octet 4 */
	int ix;        /* Auxiliary */
#endif /* LISTER */
	t_Byte rep;    /* Repetition indicator */
	t_Retc ret;    /* Return code */

				   /* Preset the return code: */
	ret = RC_FAIL;

				   /* Check parameters: */
	Assert (length > 0, "Invalid parameter");
	Assert (buffer != NULL, "Invalid parameter");

                   /* Extract repetition indicator: */
	rep = buffer[0];

				   /* Check length (again): */
	Assert (length == 1 + rep * 4, "Invalid length");

#if LISTER
				   /* List data field: */
	list_text (2, ";  Sequence of Polar Vectors:\n");
	for (ix = 0; ix < rep; ix ++)
	{
		df1 = buffer[1 + 4 * ix];
		df2 = buffer[1 + 4 * ix + 1];
		df3 = buffer[1 + 4 * ix + 2];
		df4 = buffer[1 + 4 * ix + 3];

		azimuth = make_ui16 (df3, df4);

		list_text (2, ";   Vector %d: ", 1 + ix);
		list_text (2, "start=" M_FMT_BYTE "; stop=" M_FMT_BYTE
                      "; azimuth=" M_FMT_UI16 "\n",
                      df1, df2, azimuth);
	}
#endif /* LISTER */

				   /* Store this information: */
	/* tba ... */

				   /* Set the return code: */
	ret = RC_OKAY;

	return ret;
}

/*----------------------------------------------------------------------------*/
/* proc_i008_036   -- Process I008/036 data item                              */
/*----------------------------------------------------------------------------*/

 static
 t_Retc proc_i008_036 (t_Ui16 length, t_Byte *buffer)
{
#if LISTER
	t_Byte df1;    /* Data field octet 1 */
	t_Byte df2;    /* Data field octet 2 */
	t_Byte df3;    /* Data field octet 3 */
	int ix;        /* Auxiliary */
#endif /* LISTER */
	t_Byte rep;    /* Repetition indicator */
	t_Retc ret;    /* Return code */

				   /* Preset the return code: */
	ret = RC_FAIL;

				   /* Check parameters: */
	Assert (length > 0, "Invalid parameter");
	Assert (buffer != NULL, "Invalid parameter");

                   /* Extract repetition indicator: */
	rep = buffer[0];

				   /* Check length (again): */
	Assert (length == 1 + rep * 3, "Invalid length");

#if LISTER
				   /* List data field: */
	list_text (2, ";  Sequence of Cartesian Vectors:\n");
	for (ix = 0; ix < rep; ix ++)
	{
		df1 = buffer[1 + 3 * ix];
		df2 = buffer[1 + 3 * ix + 1];
		df3 = buffer[1 + 3 * ix + 2];

		list_text (2, ";   Vector %d: ", 1 + ix);
		list_text (2, "x=" M_FMT_BYTE "; y=" M_FMT_BYTE
                      "; length=" M_FMT_BYTE "\n", df1, df2, df3);
	}
#endif /* LISTER */

				   /* Store this information: */
	/* tba ... */

				   /* Set the return code: */
	ret = RC_OKAY;

	return ret;
}

/*----------------------------------------------------------------------------*/
/* proc_i008_038   -- Process I008/038 data item                              */
/*----------------------------------------------------------------------------*/

 static
 t_Retc proc_i008_038 (t_Ui16 length, t_Byte *buffer)
{
#if LISTER
	t_Byte df1;    /* Data field octet 1 */
	t_Byte df2;    /* Data field octet 2 */
	t_Byte df3;    /* Data field octet 3 */
	t_Byte df4;    /* Data field octet 4 */
	int ix;        /* Auxiliary */
#endif /* LISTER */
	t_Byte rep;    /* Repetition indicator */
	t_Retc ret;    /* Return code */

				   /* Preset the return code: */
	ret = RC_FAIL;

				   /* Check parameters: */
	Assert (length > 0, "Invalid parameter");
	Assert (buffer != NULL, "Invalid parameter");

                   /* Extract repetition indicator: */
	rep = buffer[0];

				   /* Check length (again): */
	Assert (length == 1 + rep * 4, "Invalid length");

#if LISTER
				   /* List data field: */
	list_text (2, ";  Sequence of Weather Vectors:\n");
	for (ix = 0; ix < rep; ix ++)
	{
		df1 = buffer[1 + 4 * ix];
		df2 = buffer[1 + 4 * ix + 1];
		df3 = buffer[1 + 4 * ix + 2];
		df4 = buffer[1 + 4 * ix + 3];

		list_text (2, ";   Vector %d: ", 1 + ix);
		list_text (2, "x1=" M_FMT_BYTE "; y1=" M_FMT_BYTE
                      "; x2=" M_FMT_BYTE "; y2=" M_FMT_BYTE "\n",
                      df1, df2, df3, df4);
	}
#endif /* LISTER */

				   /* Store this information: */
	/* tba ... */

				   /* Set the return code: */
	ret = RC_OKAY;

	return ret;
}

/*----------------------------------------------------------------------------*/
/* proc_i008_040   -- Process I008/040 data item                              */
/*----------------------------------------------------------------------------*/

 static
 t_Retc proc_i008_040 (t_Ui16 length, t_Byte *buffer)
{
#if LISTER
	t_Byte df1;    /* Data field octet 1 */
	t_Byte df2;    /* Data field octet 2 */
	t_Byte fst_lst;
                   /* First/last indicator */
	t_Byte intensity;
                   /* Intensity level */
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

				   /* List data field: */
	list_text (2, ";  Contour Identifier:");
	if (df1 & 0x80)
	{
		list_text (2, " system coordinates,");
	}
	else
	{
		list_text (2, " local coordinates,");
	}
	intensity = (df1 >> 4) & 0x07;
	list_text (2, " intensity=" M_FMT_BYTE ",", intensity);
	fst_lst = df1 & 0x03;
	list_text (2, " fst_lst=" M_FMT_BYTE ",", fst_lst);
	list_text (2, " serial=" M_FMT_BYTE, df2);
	list_text (2, "\n");
#endif /* LISTER */

				   /* Store this information: */
	/* tba ... */

				   /* Set the return code: */
	ret = RC_OKAY;

	return ret;
}

/*----------------------------------------------------------------------------*/
/* proc_i008_050   -- Process I008/050 data item                              */
/*----------------------------------------------------------------------------*/

 static
 t_Retc proc_i008_050 (t_Ui16 length, t_Byte *buffer)
{
#if LISTER
	t_Byte df1;    /* Data field octet 1 */
	t_Byte df2;    /* Data field octet 2 */
	int ix;        /* Auxiliary */
#endif /* LISTER */
	t_Byte rep;    /* Repetition indicator */
	t_Retc ret;    /* Return code */

				   /* Preset the return code: */
	ret = RC_FAIL;

				   /* Check parameters: */
	Assert (length > 0, "Invalid parameter");
	Assert (buffer != NULL, "Invalid parameter");

                   /* Extract repetition indicator: */
	rep = buffer[0];

				   /* Check length (again): */
	Assert (length == 1 + rep * 2, "Invalid length");

#if LISTER
				   /* List data field: */
	list_text (2, ";  Sequence of Contour Points:\n");
	for (ix = 0; ix < rep; ix ++)
	{
		df1 = buffer[1 + 2 * ix];
		df2 = buffer[1 + 2 * ix + 1];

		list_text (2, ";   Point %d: ", 1 + ix);
		list_text (2, "x=" M_FMT_BYTE "; y=" M_FMT_BYTE "\n", df1, df2);
	}
#endif /* LISTER */

				   /* Store this information: */
	/* tba ... */

				   /* Set the return code: */
	ret = RC_OKAY;

	return ret;
}

/*----------------------------------------------------------------------------*/
/* proc_i008_090   -- Process I008/090 data item                              */
/*----------------------------------------------------------------------------*/

 static
 t_Retc proc_i008_090 (t_Ui16 length, t_Byte *buffer)
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

				   /* Compute absolute time: */
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
/* proc_i008_100   -- Process I008/100 data item                              */
/*----------------------------------------------------------------------------*/

 static
 t_Retc proc_i008_100 (t_Ui16 length, t_Byte *buffer, t_Ui16 *pos_ptr)
{
	int cnt;       /* Count for data field octets */
	t_Byte df;     /* Data field octet */
#if LISTER
	t_Byte df1;    /* Data field octet 1 */
	t_Byte df2;    /* Data field octet 2 */
#endif /* LISTER */
	t_Byte df3;    /* Data field octet 3 */
#if LISTER
	t_Si16 f;      /* Scaling factor */
	int ix;        /* Auxiliary */
#endif /* LISTER */
	t_Ui16 pos;    /* Position within buffer */
#if LISTER
	t_Ui16 q;      /* Processing parameters */
	t_Byte r;      /* Current reduction stage in use */
#endif /* LISTER */
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

				   /* Check against buffer length: */
	if (pos + 2 >= length)
	{
		error_msg ("Invalid buffer length");
		goto done;
	}

				   /* Extract octets: */
#if LISTER
	df1 = buffer[pos ++];
	df2 = buffer[pos ++];
#endif /* LISTER */
	df3 = buffer[pos ++];

				   /* Count and extract additional octets: */
	cnt = 0;
	df = df3;
	while (df & 0x01)
	{
		++ cnt;

				   /* Check against buffer length: */
		if (pos + cnt >= length)
		{
			error_msg ("Invalid buffer length");
			goto done;
		}

		df = buffer[pos + cnt];
	}

#if LISTER
				   /* List raw data field: */
	list_text (1, ";  I008/100: 0x " M_FMT_BYTE_HEX " " M_FMT_BYTE_HEX
                  " " M_FMT_BYTE_HEX, df1, df2, df3);
	for (ix = 0; ix < cnt; ix ++)
	{
		list_text (1, " " M_FMT_BYTE_HEX, buffer[pos + ix]);
	}
	list_text (1, "\n");

				   /* Extract information: */
	f = (df1 >> 3) & 0x0f;
	if (df1 & 0x80)
	{
		f |= 0xfff0;
	}
	r = df1 & 0x07;
	q = (df2 << 8) | df3;
	q >>= 1;

				   /* List data field: */
	list_text (2, ";  Processing Status:");
	list_text (2, " f=" M_FMT_BYTE "; R=" M_FMT_BYTE
                  "; Q=" M_FMT_BYTE, f, r, q);
	list_text (2, "\n");
#endif /* LISTER */

				   /* Store this information: */
	/* tba ... */

				   /* Set position: */
	*pos_ptr = pos + cnt;

				   /* Set the return code: */
	ret = RC_OKAY;

	done:          /* We are done */
	return ret;
}

/*----------------------------------------------------------------------------*/
/* proc_i008_110   -- Process I008/110 data item                              */
/*----------------------------------------------------------------------------*/

 static
 t_Retc proc_i008_110 (t_Ui16 length, t_Byte *buffer)
{
#if LISTER
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
	list_text (2, ";  Station Configuration Status:");
	for (ix = 0; ix < length; ix ++)
	{
		list_text (2, " " M_FMT_BYTE, buffer[ix]);
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
/* proc_i008_120   -- Process I008/120 data item                              */
/*----------------------------------------------------------------------------*/

 static
 t_Retc proc_i008_120 (t_Ui16 length, t_Byte *buffer)
{
#if LISTER
	t_Byte df1;    /* Data field octet 1 */
	t_Byte df2;    /* Data field octet 2 */
	t_Ui16 num;    /* Total number of items */
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
	list_text (2, ";  Total number of items: " M_FMT_UI16, num);
	list_text (2, "\n");
#endif /* LISTER */

				   /* Store this information: */
	/* tba ... */

				   /* Set the return code: */
	ret = RC_OKAY;

	return ret;
}

/*----------------------------------------------------------------------------*/
/* proc_i008_rfs   -- Process RFS indicator data item                         */
/*----------------------------------------------------------------------------*/

 static
 t_Retc proc_i008_rfs (t_Ui16 length, t_Byte *buffer, t_Ui16 *pos_ptr)
{
	t_Byte frn;    /* Field reference number */
	int ix;        /* Auxiliary */
	t_Retc lrc;    /* Local return code */
	t_Byte n;      /* Number of data fields */
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

				   /* Get number of data fields: */
	n = buffer[pos ++];

				   /* Check number of data fields: */
	if (n == 0)
	{
		error_msg ("Invalid number of data fields");
		goto done;
	}

#if LISTER
				   /* List raw data field: */
	list_text (1, ";  I008/RFS: 0x " M_FMT_BYTE_HEX " ...\n", n);
#endif /* LISTER */

				   /* Process data fields: */
	for (ix = 0; ix < n; ix ++)
	{
				   /* Check against buffer length: */
		if (pos >= length)
		{
			error_msg ("Invalid buffer length");
			goto done;
		}

				   /* Get field reference number: */
		frn = buffer[pos ++];

#if LISTER
				   /* List raw data field: */
		list_text (1, ";  I008/RFS: frn=" M_FMT_BYTE "\n", frn);
#endif /* LISTER */

				   /* Check field reference number: */
		if (frn <= 0)
		{
			error_msg ("Invalid FRN");
			goto done;
		}
		else if (frn > M_MAX_FRN)
		{
			error_msg ("FRN too large");
			goto done;
		}

		lrc = data_item (8, frn, std_uap[frn],
                         length, buffer, &pos);
		if (lrc != RC_OKAY)
		{
			error_msg ("Invalid data item");
			goto done;
		}
	}

				   /* Set position: */
	*pos_ptr = pos;

				   /* Set the return code: */
	ret = RC_OKAY;

	done:          /* We are done */
	return ret;
}

/*----------------------------------------------------------------------------*/
/* proc_i008_spf   -- Process SPF indicator data item                         */
/*----------------------------------------------------------------------------*/

 static
 t_Retc proc_i008_spf (t_Ui16 length, t_Byte *buffer, t_Ui16 *pos_ptr)
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
	list_buffer (1, ";  I008/SPF:", len, buffer + pos);
#endif /* LISTER */

				   /* Set position: */
	*pos_ptr = pos + len;

				   /* Set the return code: */
	ret = RC_OKAY;

	done:          /* We are done */
	return ret;
}
/* end-of-file */
