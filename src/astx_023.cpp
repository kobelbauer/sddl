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
/* File:           src/astx_023.cpp                                           */
/* Contents:       Process ASTERIX category 023 data block                    */
/* Author(s):      kb                                                         */
/* Last change:    2018-04-27                                                 */
/*----------------------------------------------------------------------------*/

/* Reference document:
   -------------------

   Eurocontrol Standard Document for Surveillance Data Exchange.
   Part 16: CNS/ATM Ground Station Service Messages.
   SUR.ET1.ST05.2000-STD-16-01

   Edition: 0.11
   Edition Date: December 2002
   Status: Working Draft

   Edition: 1.0P
   Edition Date: April 2008
   Status: Proposed Issue

   Edition: 1.1
   Edition Date: September 2008
   Status: Released Issue

   Edition: 1.2
   Edition Date: March 2009
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
					  for ASTERIX category 023 */

				   /* Local variables: */
				   /* ---------------- */

static t_Data_Item_Desc desc_i023_000;
                   /* Description of data item I023/000 */
static t_Data_Item_Desc desc_i023_010;
                   /* Description of data item I023/010 */
static t_Data_Item_Desc desc_i023_015;
                   /* Description of data item I023/015 */
static t_Data_Item_Desc desc_i023_070;
                   /* Description of data item I023/070 */
static t_Data_Item_Desc desc_i023_100;
                   /* Description of data item I023/100 */
static t_Data_Item_Desc desc_i023_101;
                   /* Description of data item I023/101 */
static t_Data_Item_Desc desc_i023_110;
                   /* Description of data item I023/110 */
static t_Data_Item_Desc desc_i023_120;
                   /* Description of data item I023/120 */
static t_Data_Item_Desc desc_i023_200;
                   /* Description of data item I023/200 */
static t_Data_Item_Desc desc_i023_ref;
                   /* Description of RE data item */
static t_Data_Item_Desc desc_i023_spf;
                   /* Description of SP data item */
static t_Bool initiated = FALSE;
				   /* Package initiated flag */
static t_Data_Item_Desc *std_uap[M_MAX_FRN + 1];
				   /* Standard User Application Profile */
static t_Ui16 reference_vsn = 4;
                   /* Current version of reference document */
                   /* Implemented editions:
                      1 ... 0.11 (December 2002)
                      2 ... 1.0 (August 2008)
                      3 ... 1.1 (September 2008)
                      4 ... 1.2 (March 2009)
                   */

				   /* Local functions: */
				   /* ---------------- */

static void init_desc (void);
                   /* Initiate data item descriptions */
static void load_std_uap (void);
                   /* Load standard UAP */
static t_Retc proc_i023_000 (t_Ui16 length, t_Byte *buffer);
                   /* Process I023/000 data item */
static t_Retc proc_i023_010 (t_Ui16 length, t_Byte *buffer);
                   /* Process I023/010 data item */
static t_Retc proc_i023_015 (t_Ui16 length, t_Byte *buffer);
                   /* Process I023/015 data item */
static t_Retc proc_i023_070 (t_Ui16 length, t_Byte *buffer);
                   /* Process I023/070 data item */
static t_Retc proc_i023_100 (t_Ui16 length, t_Byte *buffer);
                   /* Process I023/100 data item */
static t_Retc proc_i023_101 (t_Ui16 length, t_Byte *buffer, t_Ui16 *pos_ptr);
                   /* Process I023/101 data item */
static t_Retc proc_i023_110 (t_Ui16 length, t_Byte *buffer);
                   /* Process I023/110 data item */
static t_Retc proc_i023_120 (t_Ui16 length, t_Byte *buffer);
                   /* Process I023/120 data item */
static t_Retc proc_i023_200 (t_Ui16 length, t_Byte *buffer);
                   /* Process I023/200 data item */
static t_Retc proc_i023_ref (t_Ui16 length, t_Byte *buffer, t_Ui16 *pos_ptr);
                   /* Process RE indicator data item */
static t_Retc proc_i023_spf (t_Ui16 length, t_Byte *buffer, t_Ui16 *pos_ptr);
                   /* Process SP indicator data item */

/*----------------------------------------------------------------------------*/
/* astx_023        -- Process ASTERIX category 023 data block                 */
/*----------------------------------------------------------------------------*/

 t_Retc astx_023 (t_Ui16 length, t_Byte *buffer)
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

					lrc = data_item (23, frn, std_uap[frn],
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
                   /* Description of data item I023/000: */
	desc_i023_000.category = 23;
	desc_i023_000.data_item = 0;
	desc_i023_000.item_type = e_fixed_length_data_item;
	desc_i023_000.fixed_length = 1;
	desc_i023_000.proc_fptr = proc_i023_000;
	desc_i023_000.read_fptr = NULL;

                   /* Description of data item I023/010: */
	desc_i023_010.category = 23;
	desc_i023_010.data_item = 10;
	desc_i023_010.item_type = e_fixed_length_data_item;
	desc_i023_010.fixed_length = 2;
	desc_i023_010.proc_fptr = proc_i023_010;
	desc_i023_010.read_fptr = NULL;

                   /* Description of data item I023/015: */
	desc_i023_015.category = 23;
	desc_i023_015.data_item = 15;
	desc_i023_015.item_type = e_fixed_length_data_item;
	desc_i023_015.fixed_length = 1;
	desc_i023_015.proc_fptr = proc_i023_015;
	desc_i023_015.read_fptr = NULL;

                   /* Description of data item I023/070: */
	desc_i023_070.category = 23;
	desc_i023_070.data_item = 70;
	desc_i023_070.item_type = e_fixed_length_data_item;
	desc_i023_070.fixed_length = 3;
	desc_i023_070.proc_fptr = proc_i023_070;
	desc_i023_070.read_fptr = NULL;

                   /* Description of data item I023/100: */
	desc_i023_100.category = 23;
	desc_i023_100.data_item = 100;
	desc_i023_100.item_type = e_variable_length_data_item;
	desc_i023_100.fixed_length = 0;
	desc_i023_100.proc_fptr = proc_i023_100;
	desc_i023_100.read_fptr = NULL;

                   /* Description of data item I023/101: */
	if (reference_vsn > 1)
	{
		desc_i023_101.category = 23;
		desc_i023_101.data_item = 101;
		desc_i023_101.item_type = e_immediate_data_item;
		desc_i023_101.fixed_length = 0;
		desc_i023_101.proc_fptr = NULL;
		desc_i023_101.read_fptr = proc_i023_101;
	}

                   /* Description of data item I023/110: */
	desc_i023_110.category = 23;
	desc_i023_110.data_item = 110;
	desc_i023_110.item_type = e_variable_length_data_item;
	desc_i023_110.fixed_length = 0;
	desc_i023_110.proc_fptr = proc_i023_110;
	desc_i023_110.read_fptr = NULL;

                   /* Description of data item I023/120: */
	desc_i023_120.category = 23;
	desc_i023_120.data_item = 120;
	desc_i023_120.item_type = e_repetitive_data_item;
	desc_i023_120.fixed_length = 6;
	desc_i023_120.proc_fptr = proc_i023_120;
	desc_i023_120.read_fptr = NULL;

                   /* Description of data item I023/200: */
	if (reference_vsn > 1)
	{
		desc_i023_200.category = 23;
		desc_i023_200.data_item = 200;
		desc_i023_200.item_type = e_fixed_length_data_item;
		desc_i023_200.fixed_length = 1;
		desc_i023_200.proc_fptr = proc_i023_200;
		desc_i023_200.read_fptr = NULL;
	}

                   /* Description of RE data item: */
    desc_i023_ref.category = 23;
    desc_i023_ref.data_item = M_REF_INDICATOR;
    desc_i023_ref.item_type = e_immediate_data_item;
    desc_i023_ref.fixed_length = 0;
    desc_i023_ref.proc_fptr = NULL;
    desc_i023_ref.read_fptr = proc_i023_ref;

                   /* Description of SP data item: */
    desc_i023_spf.category = 23;
    desc_i023_spf.data_item = M_SPF_INDICATOR;
    desc_i023_spf.item_type = e_immediate_data_item;
    desc_i023_spf.fixed_length = 0;
    desc_i023_spf.proc_fptr = NULL;
    desc_i023_spf.read_fptr = proc_i023_spf;

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

	if (reference_vsn == 1)
	{
		std_uap[1] = &desc_i023_000;
		std_uap[2] = &desc_i023_010;
		std_uap[3] = &desc_i023_015;
		std_uap[4] = &desc_i023_070;
		std_uap[5] = &desc_i023_100;
		std_uap[6] = &desc_i023_110;
		std_uap[7] = &desc_i023_120;
		std_uap[13] = &desc_i023_ref;
		std_uap[14] = &desc_i023_spf;
	}
	else
	{
		std_uap[1] = &desc_i023_010;
		std_uap[2] = &desc_i023_000;
		std_uap[3] = &desc_i023_015;
		std_uap[4] = &desc_i023_070;
		std_uap[5] = &desc_i023_100;
		std_uap[6] = &desc_i023_101;
		std_uap[7] = &desc_i023_200;

		std_uap[8] = &desc_i023_110;
		std_uap[9] = &desc_i023_120;
		std_uap[13] = &desc_i023_ref;
		std_uap[14] = &desc_i023_spf;
	}

	return;
}

/*----------------------------------------------------------------------------*/
/* proc_i023_000   -- Process I023/000 data item                              */
/*----------------------------------------------------------------------------*/

 static
 t_Retc proc_i023_000 (t_Ui16 length, t_Byte *buffer)
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
	list_text (2, " mtp=" M_FMT_BYTE, mtp);
	if (mtp == 1)
	{
		list_text (2, " (Ground Station Status message)");
	}
	else if (mtp == 2)
	{
		list_text (2, " (Service Status message)");
	}
	else if (mtp == 3)
	{
		list_text (2, " (Service Statistics message)");
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
/* proc_i023_010   -- Process I023/010 data item                              */
/*----------------------------------------------------------------------------*/

 static
 t_Retc proc_i023_010 (t_Ui16 length, t_Byte *buffer)
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
/* proc_i023_015   -- Process I023/015 data item                              */
/*----------------------------------------------------------------------------*/

 static
 t_Retc proc_i023_015 (t_Ui16 length, t_Byte *buffer)
{
#if LISTER
	t_Byte df1;    /* Data field octet 1 */
#endif /* LISTER */
	t_Retc ret;    /* Return code */
#if LISTER
	t_Byte sid;    /* Service identification */
	t_Byte styp;   /* Type of service */
#endif /* LISTER */

				   /* Preset the return code: */
	ret = RC_FAIL;

				   /* Check parameters: */
	Assert (length == 1, "Invalid length");
	Assert (buffer != NULL, "NULL pointer");

#if LISTER
				   /* Extract octets: */
	df1 = buffer[0];

				   /* Extract information: */
	sid = (df1 >> 4) & 0x0f;
	styp = df1 & 0x0f;

				   /* List data field: */
	list_text (2, ";  Service Type and Identification:");
	list_text (2, " sid=" M_FMT_BYTE ";", sid);
	list_text (2, " styp=" M_FMT_BYTE, styp);
	switch (styp)
	{
	case 1:
		list_text (2, " (ADS-B VDL4)");
		break;
	case 2:
		list_text (2, " (ADS-B Ext Squitter)");
		break;
	case 3:
		list_text (2, " (ADS-B UAT)");
		break;
	case 4:
		list_text (2, " (TIS-B VDL4)");
		break;
	case 5:
		list_text (2, " (TIS-B Ext Squitter)");
		break;
	case 6:
		list_text (2, " (TIS-B UAT)");
		break;
	case 7:
		list_text (2, " (FIS-B VDL4)");
		break;
	case 8:
		list_text (2, " (GRAS VDL4)");
		break;
	case 9:
		list_text (2, " (MLT)");
		break;
	default:
		break;
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
/* proc_i023_070   -- Process I023/070 data item                              */
/*----------------------------------------------------------------------------*/

 static
 t_Retc proc_i023_070 (t_Ui16 length, t_Byte *buffer)
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

				   /* Compute time (of day): */
	tm = (1.0 / 128.0) * tod;

				   /* List data field: */
	list_text (2, ";  Time of Day:");
	list_text (2, " " M_FMT_ATOD " (" M_FMT_UI32 "; %s UTC)",
                  tod, tod, utc_text (tm));
	list_text (2, "\n");
#endif /* LISTER */

				   /* Store this information: */
	/* tba ... */

				   /* Set the return code: */
	ret = RC_OKAY;

	return ret;
}

/*----------------------------------------------------------------------------*/
/* proc_i023_100   -- Process I023/100 data item                              */
/*----------------------------------------------------------------------------*/

 static
 t_Retc proc_i023_100 (t_Ui16 length, t_Byte *buffer)
{
#if LISTER
	t_Byte df1;    /* Data field octet 1 */
	t_Byte df2;    /* Data field octet 2 */
	t_Byte gssp;   /* Ground station status reporting period; seconds */
	t_Byte msc;    /* Monitoring system connected status */
	t_Byte nogo;   /* Operational release status of the data */
	t_Byte odp;    /* Data processor overload indicator */
	t_Byte op;     /* Operational release status of the system */
	t_Byte oxt;    /* Ground interface data communications overload */
#endif /* LISTER */
	t_Retc ret;    /* Return code */
#if LISTER
	t_Byte rn;     /* Renumbering indication for track ID */
	t_Byte spo;    /* Indication of spoofing attack */
	t_Byte tsv;    /* Time source validity */
#endif /* LISTER */

				   /* Preset the return code: */
	ret = RC_FAIL;

				   /* Check parameters: */
	Assert (length >= 1, "Invalid parameter");
	Assert (buffer != NULL, "Invalid parameter");

#if LISTER
				   /* Extract octets: */
	df1 = buffer[0];
	df2 = 0;
	if (reference_vsn == 4 && length >= 2)
	{
		df2 = buffer[1];
	}

                   /* Preset local data: */
	gssp = msc = nogo = odp = op = oxt = rn = spo = tsv = 0;

				   /* Extract information: */
	if (reference_vsn == 1)
	{
		op = (df1 >> 7) & 0x01;
	}
	else
	{
		nogo = (df1 >> 7) & 0x01;
	}
	odp = (df1 >> 6) & 0x01;
	oxt = (df1 >> 5) & 0x01;
	msc = (df1 >> 4) & 0x01;
	tsv = (df1 >> 3) & 0x01;
	if (reference_vsn > 1)
	{
		spo = (df1 >> 2) & 0x01;
		rn = (df1 >> 1) & 0x01;
		gssp = 0;
		if (reference_vsn == 4 && length >= 2)
		{
			gssp = (df2 >> 1) & 0x7f;
		}
	}

				   /* List data field: */
	if (reference_vsn == 1)
	{
		list_text (2, ";  Ground Station Configuration and Status:");
	}
	else
	{
		list_text (2, ";  Ground Station Status:");
	}
	if (reference_vsn == 1)
	{
		list_text (2, " op=" M_FMT_BYTE ";", nogo);
	}
	else
	{
		list_text (2, " nogo=" M_FMT_BYTE ";", nogo);
	}
	list_text (2, " odp=" M_FMT_BYTE ";", odp);
	list_text (2, " oxt=" M_FMT_BYTE ";", oxt);
	list_text (2, " msc=" M_FMT_BYTE ";", msc);
	list_text (2, " tsv=" M_FMT_BYTE, tsv);
	if (reference_vsn > 1)
	{
		list_text (2, ";");
		list_text (2, " spo=" M_FMT_BYTE ";", spo);
		list_text (2, " rn=" M_FMT_BYTE, rn);
		if (reference_vsn == 4 && length >= 2)
		{
			list_text (2, ";");
			list_text (2, " gssp=" M_FMT_BYTE, gssp);
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
/* proc_i023_101   -- Process I023/101 data item                              */
/*----------------------------------------------------------------------------*/

 static
 t_Retc proc_i023_101 (t_Ui16 length, t_Byte *buffer, t_Ui16 *pos_ptr)
{
#if LISTER
	t_Byte df1;    /* Data field octet 1 */
	t_Byte df2;    /* Data field octet 2 */
	t_Byte df3;    /* Data field octet 3 */
#endif /* LISTER */
	t_Ui16 inx;    /* Auxiliary */
#if LISTER
	t_Ui16 j;      /* Auxiliary */
#endif /* LISTER */
	t_Ui16 len;    /* Data item length */
	t_Ui16 pos;    /* Position within buffer */
	t_Retc ret;    /* Return code */
#if LISTER
	t_Byte rp;     /* Report period */
	t_Byte sc;     /* Service class */
	t_Byte ssrp;   /* Service status reporting period; seconds */
#endif /* LISTER */

				   /* Preset the return code: */
	ret = RC_FAIL;

				   /* Check parameters: */
	Assert (length > 0, "Invalid parameter");
	Assert (buffer != NULL, "Invalid parameter");
	Assert (pos_ptr != NULL, "Invalid parameter");

                   /* Check ASTERIX version: */
	Assert (reference_vsn > 1, "Unexpected ASTERIX version");

				   /* Set position within buffer: */
	pos = *pos_ptr;

				   /* Check position: */
	Assert (pos < length, "Invalid position");

				   /* Get length of data field: */
	inx = 1;
	len = 2;
	while (buffer[pos + inx] & 0x01)
	{
		if (pos + inx >= length)
		{
			error_msg ("Data buffer overrun");
			goto done;
		}

		++ inx;
		++ len;
	}

				   /* Check length of data field: */
	if (len < 2)
	{
		error_msg ("Invalid length of data field");
		goto done;
	}

				   /* Check against buffer length: */
	if (pos + len > length)
	{
		error_msg ("Invalid buffer length (I023/101)");
		goto done;
	}

#if LISTER
                   /* Preset local data: */
	rp = sc = ssrp = 0;

                   /* Extract octets: */
	df1 = buffer[pos];
	df2 = buffer[pos + 1];
	df3 = 0;
	if (reference_vsn == 4 && len >= 3)
	{
		df3 = buffer[pos + 2];
	}

                   /* List raw data field: */
	list_text (1, ";  I023/101: 0x");
	j = 0;
	for (inx = 0; inx < len; inx ++)
	{
		list_text (1, " " M_FMT_BYTE_HEX, buffer[pos + inx]);
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

                   /* Extract information: */
	rp = df1;
	sc = (df2 >> 5) & 0x07;
	if (reference_vsn == 4 && len >= 3)
	{
		ssrp = (df3 >> 1) & 0x7f;
	}

                   /* List data field: */
	if (reference_vsn == 4)
	{
		list_text (2, ";  Service Configuration:");
	}
	else
	{
		list_text (2, ";  Ground Station Configuration:");
	}
	list_text (2, " rp=" M_FMT_BYTE, rp);
	if (rp > 0)
	{
		list_text (2, " (%.1f seconds)", 0.5 * rp);
	}
	else
	{
		list_text (2, " (data driven mode)");
	}
	list_text (2, ";");
	if (reference_vsn == 2)
	{
		list_text (2, " gsc=" M_FMT_BYTE, sc);
	}
	else if (reference_vsn == 3 || reference_vsn == 4)
	{
		list_text (2, " sc=" M_FMT_BYTE, sc);
	}
	if (reference_vsn == 4 && len >= 3)
	{
		list_text (2, ";");
		list_text (2, " ssrp=" M_FMT_BYTE " seconds", ssrp);
	}
	list_text (2, "\n");
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
/* proc_i023_110   -- Process I023/110 data item                              */
/*----------------------------------------------------------------------------*/

 static
 t_Retc proc_i023_110 (t_Ui16 length, t_Byte *buffer)
{
#if LISTER
	t_Byte df1;    /* Data field octet 1 */
#endif /* LISTER */
	t_Retc ret;    /* Return code */
#if LISTER
	t_Byte stat;   /* Status of the service */
#endif /* LISTER */

				   /* Preset the return code: */
	ret = RC_FAIL;

				   /* Check parameters: */
	Assert (length >= 1, "Invalid parameter");
	Assert (buffer != NULL, "Invalid parameter");

#if LISTER
				   /* Extract octets: */
	df1 = buffer[0];

				   /* Extract information: */
	stat = (df1 >> 1) & 0x07;

				   /* List data field: */
	list_text (2, ";  Service Status:");
	list_text (2, " stat=" M_FMT_BYTE, stat);
	switch (stat)
	{
	case 0:
		list_text (2, " (unknown)");
		break;
	case 1:
		list_text (2, " (failed)");
		break;
	case 2:
		list_text (2, " (disabled)");
		break;
	case 3:
		list_text (2, " (degraded)");
		break;
	case 4:
		list_text (2, " (normal)");
		break;
	case 5:
		if (reference_vsn == 3)
		{
			list_text (2, " (initialisation)");
		}
	default:
		break;
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
/* proc_i023_120   -- Process I023/120 data item                              */
/*----------------------------------------------------------------------------*/

 static
 t_Retc proc_i023_120 (t_Ui16 length, t_Byte *buffer)
{
#if LISTER
	t_Ui32 counter;
                   /* Counter value */
	t_Byte df1;    /* Data field octet 1 */
	t_Byte df2;    /* Data field octet 2 */
	t_Byte df3;    /* Data field octet 3 */
	t_Byte df4;    /* Data field octet 4 */
	t_Byte df5;    /* Data field octet 5 */
	t_Byte df6;    /* Data field octet 6 */
	int ix;        /* Auxiliary */
	t_Byte ref;    /* Reference from which the messages are counted */
#endif /* LISTER */
	t_Byte rep;    /* Repetition factor */
	t_Retc ret;    /* Return code */
#if LISTER
	t_Byte type;   /* Type of report counter */
#endif /* LISTER */

				   /* Preset the return code: */
	ret = RC_FAIL;

				   /* Check parameters: */
	Assert (length >= 1, "Invalid parameter");
	Assert (buffer != NULL, "Invalid parameter");

                   /* Extract repetition factor: */
	rep = buffer[0];

                   /* Check repetition factor: */
	if (rep == 0)
	{
		error_msg ("Invalid repetition factor");
		goto done;
	}

                   /* Check length (again): */
	Assert (length == 1 + rep * 6, "Invalid length");

#if LISTER
				   /* List data field: */
	list_text (2, ";  Service Statistics:");
	list_text (2, "\n");
	for (ix = 0; ix < rep; ix ++)
	{
		df1 = buffer[1 + 6 * ix];
		df2 = buffer[1 + 6 * ix + 1];
		df3 = buffer[1 + 6 * ix + 2];
		df4 = buffer[1 + 6 * ix + 3];
		df5 = buffer[1 + 6 * ix + 4];
		df6 = buffer[1 + 6 * ix + 5];

		type = df1;
		ref = (df2 >> 7) & 0x01;
		counter = make_ui32 (df3, df4, df5, df6);

		list_text (2, ";   type=" M_FMT_BYTE ";", type);
		list_text (2, " ref=" M_FMT_BYTE ";", ref);
		list_text (2, " counter=" M_FMT_UI32, counter);
		list_text (2, "\n");
	}
#endif /* LISTER */

				   /* Store this information: */
	/* tba ... */

				   /* Set the return code: */
	ret = RC_OKAY;

	done:          /* We are done */
	return ret;
}

/*----------------------------------------------------------------------------*/
/* proc_i023_200   -- Process I023/200 data item                              */
/*----------------------------------------------------------------------------*/

 static
 t_Retc proc_i023_200 (t_Ui16 length, t_Byte *buffer)
{
#if LISTER
	t_Byte df1;    /* Data field octet 1 */
#endif /* LISTER */
	t_Retc ret;    /* Return code */
#if LISTER
	t_Byte rng;    /* Range; nautical miles */
#endif /* LISTER */

				   /* Preset the return code: */
	ret = RC_FAIL;

				   /* Check parameters: */
	Assert (length == 1, "Invalid parameter");
	Assert (buffer != NULL, "Invalid parameter");

                   /* Check ASTERIX version: */
	Assert (reference_vsn > 1, "Unexpected ASTERIX version");

#if LISTER
				   /* Extract octets: */
	df1 = buffer[0];

				   /* Extract information: */
	rng = df1;

				   /* List data field: */
	list_text (2, ";  Operational Range:");
	list_text (2, " rng=" M_FMT_BYTE " NM", rng);
	list_text (2, "\n");
#endif /* LISTER */

				   /* Store this information: */
	/* tba ... */

				   /* Set the return code: */
	ret = RC_OKAY;

	return ret;
}

/*----------------------------------------------------------------------------*/
/* proc_i023_ref   -- Process REF indicator data item                         */
/*----------------------------------------------------------------------------*/

 static
 t_Retc proc_i023_ref (t_Ui16 length, t_Byte *buffer, t_Ui16 *pos_ptr)
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
		error_msg ("Invalid buffer length (I023/RE)");
		goto done;
	}

#if LISTER
				   /* List raw data field: */
	list_buffer (1, ";  I023/RE :", len, buffer + pos);
#endif /* LISTER */

				   /* Set position: */
	*pos_ptr = pos + len;

                   /* Set the return code: */
	ret = RC_OKAY;

	done:          /* We are done */
	return ret;
}

/*----------------------------------------------------------------------------*/
/* proc_i023_spf   -- Process SPF indicator data item                         */
/*----------------------------------------------------------------------------*/

 static
 t_Retc proc_i023_spf (t_Ui16 length, t_Byte *buffer, t_Ui16 *pos_ptr)
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
		error_msg ("Invalid buffer length (I023/SP)");
		goto done;
	}

#if LISTER
				   /* List raw data field: */
	list_buffer (1, ";  I023/SP :", len, buffer + pos);
#endif /* LISTER */

				   /* Set position: */
	*pos_ptr = pos + len;

				   /* Set the return code: */
	ret = RC_OKAY;

	done:          /* We are done */
	return ret;
}

/*----------------------------------------------------------------------------*/
/* set_vsn023      -- Set ASTERIX category 023 reference version              */
/*----------------------------------------------------------------------------*/

 t_Retc set_vsn023 (char *vsn_text)
{
	t_Retc ret;    /* Return code */

                   /* Preset the return code: */
	ret = RC_FAIL;

                   /* Check parameters: */
	Assert (vsn_text != NULL, "Invalid parameter");

                   /* Check against implemented reference versions: */
	if (strcmp (vsn_text, "0.11") == 0)
	{
		reference_vsn = 1;
		ret = RC_OKAY;
	}
	else if (strcmp (vsn_text, "1.0P") == 0)
	{
		reference_vsn = 2;
		ret = RC_OKAY;
	}
	else if (strcmp (vsn_text, "1.1") == 0)
	{
		reference_vsn = 3;
		ret = RC_OKAY;
	}
	else if (strcmp (vsn_text, "1.2") == 0)
	{
		reference_vsn = 4;
		ret = RC_OKAY;
	}

	return ret;
}
/* end-of-file */
