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
/* File:           src/astx_063.cpp                                           */
/* Contents:       Process ASTERIX category 063 data block                    */
/* Author(s):      kb                                                         */
/* Last change:    2018-04-27                                                 */
/*----------------------------------------------------------------------------*/

/* Reference document:
   -------------------

   Eurocontrol Standard Document for Surveillance Data Exchange
   Part 10: Category 63 - Sensor Status Messages
   SUR.ET1.ST05.2000-STD-10-01
   Version: 1.0
   Edition Date: March 2004
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
					  for ASTERIX category 063 */

				   /* Local variables: */
				   /* ---------------- */

static t_Data_Item_Desc desc_i063_010;
                   /* Description of data item I063/010 */
static t_Data_Item_Desc desc_i063_015;
                   /* Description of data item I063/015 */
static t_Data_Item_Desc desc_i063_030;
                   /* Description of data item I063/030 */
static t_Data_Item_Desc desc_i063_050;
                   /* Description of data item I063/050 */
static t_Data_Item_Desc desc_i063_060;
                   /* Description of data item I063/060 */
static t_Data_Item_Desc desc_i063_070;
                   /* Description of data item I063/070 */
static t_Data_Item_Desc desc_i063_080;
                   /* Description of data item I063/080 */
static t_Data_Item_Desc desc_i063_081;
                   /* Description of data item I063/081 */
static t_Data_Item_Desc desc_i063_090;
                   /* Description of data item I063/090 */
static t_Data_Item_Desc desc_i063_091;
                   /* Description of data item I063/091 */
static t_Data_Item_Desc desc_i063_092;
                   /* Description of data item I063/092 */
static t_Data_Item_Desc desc_i063_ref;
                   /* Description of data item I063/REF */
static t_Data_Item_Desc desc_i063_spf;
                   /* Description of data item I063/SPF */
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
static t_Retc proc_i063_010 (t_Ui16 length, t_Byte *buffer);
                   /* Process I063/010 data item */
static t_Retc proc_i063_015 (t_Ui16 length, t_Byte *buffer);
                   /* Process I063/015 data item */
static t_Retc proc_i063_030 (t_Ui16 length, t_Byte *buffer);
                   /* Process I063/030 data item */
static t_Retc proc_i063_050 (t_Ui16 length, t_Byte *buffer);
                   /* Process I063/050 data item */
static t_Retc proc_i063_060 (t_Ui16 length, t_Byte *buffer);
                   /* Process I063/060 data item */
static t_Retc proc_i063_070 (t_Ui16 length, t_Byte *buffer);
                   /* Process I063/070 data item */
static t_Retc proc_i063_080 (t_Ui16 length, t_Byte *buffer);
                   /* Process I063/080 data item */
static t_Retc proc_i063_081 (t_Ui16 length, t_Byte *buffer);
                   /* Process I063/081 data item */
static t_Retc proc_i063_090 (t_Ui16 length, t_Byte *buffer);
                   /* Process I063/090 data item */
static t_Retc proc_i063_091 (t_Ui16 length, t_Byte *buffer);
                   /* Process I063/091 data item */
static t_Retc proc_i063_092 (t_Ui16 length, t_Byte *buffer);
                   /* Process I063/092 data item */
static t_Retc proc_i063_ref (t_Ui16 length, t_Byte *buffer, t_Ui16 *pos_ptr);
                   /* Process RE indicator data item */
static t_Retc proc_i063_spf (t_Ui16 length, t_Byte *buffer, t_Ui16 *pos_ptr);
                   /* Process SP indicator data item */

/*----------------------------------------------------------------------------*/
/* astx_063        -- Process ASTERIX category 063 data block                 */
/*----------------------------------------------------------------------------*/

 t_Retc astx_063 (t_Ui16 length, t_Byte *buffer)
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

                   /* Clear buffer for sensor status information: */
		memset (&ssta, 0, sizeof (t_Ssta));

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

					lrc = data_item (63, frn, std_uap[frn],
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

#if LISTER
                   /* List sensor status information, if required: */
		if (list_ssta_info)
		{
			list_ssta (&ssta);
		}
#endif /* LISTER */
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
                   /* Description of data item I063/010: */
	desc_i063_010.category = 63;
	desc_i063_010.data_item = 10;
	desc_i063_010.item_type = e_fixed_length_data_item;
	desc_i063_010.fixed_length = 2;
	desc_i063_010.proc_fptr = proc_i063_010;
	desc_i063_010.read_fptr = NULL;

                   /* Description of data item I063/015: */
	desc_i063_015.category = 63;
	desc_i063_015.data_item = 15;
	desc_i063_015.item_type = e_fixed_length_data_item;
	desc_i063_015.fixed_length = 1;
	desc_i063_015.proc_fptr = proc_i063_015;
	desc_i063_015.read_fptr = NULL;

                   /* Description of data item I063/030: */
	desc_i063_030.category = 63;
	desc_i063_030.data_item = 30;
	desc_i063_030.item_type = e_fixed_length_data_item;
	desc_i063_030.fixed_length = 3;
	desc_i063_030.proc_fptr = proc_i063_030;
	desc_i063_030.read_fptr = NULL;

                   /* Description of data item I063/050: */
	desc_i063_050.category = 63;
	desc_i063_050.data_item = 50;
	desc_i063_050.item_type = e_fixed_length_data_item;
	desc_i063_050.fixed_length = 2;
	desc_i063_050.proc_fptr = proc_i063_050;
	desc_i063_050.read_fptr = NULL;

                   /* Description of data item I063/060: */
	desc_i063_060.category = 63;
	desc_i063_060.data_item = 60;
	desc_i063_060.item_type = e_variable_length_data_item;
	desc_i063_060.fixed_length = 0;
	desc_i063_060.proc_fptr = proc_i063_060;
	desc_i063_060.read_fptr = NULL;

                   /* Description of data item I063/070: */
	desc_i063_070.category = 63;
	desc_i063_070.data_item = 70;
	desc_i063_070.item_type = e_fixed_length_data_item;
	desc_i063_070.fixed_length = 2;
	desc_i063_070.proc_fptr = proc_i063_070;
	desc_i063_070.read_fptr = NULL;

                   /* Description of data item I063/080: */
	desc_i063_080.category = 63;
	desc_i063_080.data_item = 80;
	desc_i063_080.item_type = e_fixed_length_data_item;
	desc_i063_080.fixed_length = 4;
	desc_i063_080.proc_fptr = proc_i063_080;
	desc_i063_080.read_fptr = NULL;

                   /* Description of data item I063/081: */
	desc_i063_081.category = 63;
	desc_i063_081.data_item = 81;
	desc_i063_081.item_type = e_fixed_length_data_item;
	desc_i063_081.fixed_length = 2;
	desc_i063_081.proc_fptr = proc_i063_081;
	desc_i063_081.read_fptr = NULL;

                   /* Description of data item I063/090: */
	desc_i063_090.category = 63;
	desc_i063_090.data_item = 90;
	desc_i063_090.item_type = e_fixed_length_data_item;
	desc_i063_090.fixed_length = 4;
	desc_i063_090.proc_fptr = proc_i063_090;
	desc_i063_090.read_fptr = NULL;

                   /* Description of data item I063/091: */
	desc_i063_091.category = 63;
	desc_i063_091.data_item = 91;
	desc_i063_091.item_type = e_fixed_length_data_item;
	desc_i063_091.fixed_length = 2;
	desc_i063_091.proc_fptr = proc_i063_091;
	desc_i063_091.read_fptr = NULL;

                   /* Description of data item I063/092: */
	desc_i063_092.category = 63;
	desc_i063_092.data_item = 92;
	desc_i063_092.item_type = e_fixed_length_data_item;
	desc_i063_092.fixed_length = 2;
	desc_i063_092.proc_fptr = proc_i063_092;
	desc_i063_092.read_fptr = NULL;

                   /* Description of RE data item: */
	desc_i063_ref.category = 63;
	desc_i063_ref.data_item = M_REF_INDICATOR;
	desc_i063_ref.item_type = e_immediate_data_item;
	desc_i063_ref.fixed_length = 0;
	desc_i063_ref.proc_fptr = NULL;
	desc_i063_ref.read_fptr = proc_i063_ref;

                   /* Description of SP data item: */
	desc_i063_spf.category = 63;
	desc_i063_spf.data_item = M_SPF_INDICATOR;
	desc_i063_spf.item_type = e_immediate_data_item;
	desc_i063_spf.fixed_length = 0;
	desc_i063_spf.proc_fptr = NULL;
	desc_i063_spf.read_fptr = proc_i063_spf;

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

	std_uap[1] = &desc_i063_010;
	std_uap[2] = &desc_i063_015;
	std_uap[3] = &desc_i063_030;
	std_uap[4] = &desc_i063_050;
	std_uap[5] = &desc_i063_060;
	std_uap[6] = &desc_i063_070;
	std_uap[7] = &desc_i063_080;

	std_uap[8] = &desc_i063_081;
	std_uap[9] = &desc_i063_090;
	std_uap[10] = &desc_i063_091;
	std_uap[11] = &desc_i063_092;
	std_uap[13] = &desc_i063_ref;
	std_uap[14] = &desc_i063_spf;

	return;
}

/*----------------------------------------------------------------------------*/
/* proc_i063_010   -- Process I063/010 data item                              */
/*----------------------------------------------------------------------------*/

 static
 t_Retc proc_i063_010 (t_Ui16 length, t_Byte *buffer)
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
/* proc_i063_015   -- Process I063/015 data item                              */
/*----------------------------------------------------------------------------*/

 static
 t_Retc proc_i063_015 (t_Ui16 length, t_Byte *buffer)
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

				   /* Store this information: */
	/* tba ... */

				   /* Set the return code: */
	ret = RC_OKAY;

	return ret;
}

/*----------------------------------------------------------------------------*/
/* proc_i063_030   -- Process I063/030 data item                              */
/*----------------------------------------------------------------------------*/

 static
 t_Retc proc_i063_030 (t_Ui16 length, t_Byte *buffer)
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
	ssta.reporting_time = tod_in_secs;
	ssta.reporting_time_present = TRUE;

				   /* Set the return code: */
	ret = RC_OKAY;

	return ret;
}

/*----------------------------------------------------------------------------*/
/* proc_i063_050   -- Process I063/050 data item                              */
/*----------------------------------------------------------------------------*/

 static
 t_Retc proc_i063_050 (t_Ui16 length, t_Byte *buffer)
{
	t_Byte df1;    /* Data field octet 1 */
	t_Byte df2;    /* Data field octet 2 */
	t_Ui16 dsi;    /* Sensor identifier */
	t_Retc ret;    /* Return code */
#if LISTER
	t_Byte sac;    /* Source area code */
	t_Byte sic;    /* Source identity code */
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
	list_text (2, ";  Sensor Identifier:");
	list_text (2, " 0x" M_FMT_UI16_HEX, dsi);
	list_text (2, " (SAC=" M_FMT_BYTE "; SIC=" M_FMT_BYTE ")", sac, sic);
	list_text (2, "\n");
#endif /* LISTER */

				   /* Store this information: */
	ssta.sensor_id = dsi;
	ssta.sensor_id_present = TRUE;

				   /* Set the return code: */
	ret = RC_OKAY;

	return ret;
}

/*----------------------------------------------------------------------------*/
/* proc_i063_060   -- Process I063/060 data item                              */
/*----------------------------------------------------------------------------*/

 static
 t_Retc proc_i063_060 (t_Ui16 length, t_Byte *buffer)
{
	t_Byte dfo;    /* Data field octet */
	int ix;        /* Auxiliary */
	t_Retc ret;    /* Return code */

				   /* Preset the return code: */
	ret = RC_FAIL;

				   /* Check parameters: */
	Assert (length > 0, "Invalid parameter");
	Assert (buffer != NULL, "Invalid parameter");

#if LISTER
				   /* List data field: */
	list_text (2, ";  Sensor Configuration and Status:");
	for (ix = 0; ix < length; ix ++)
	{
		dfo = buffer[ix];
		if (ix == 0)
		{
			t_Byte con;

			con = (dfo >> 6) & 0x03;
			list_text (2, " con=" M_FMT_BYTE, con);
			switch (con)
			{
			case 0:
				list_text (2, " (operational)");
				break;
			case 1:
				list_text (2, " (degraded)");
				break;
			case 2:
				list_text (2, " (initialization)");
				break;
			case 3:
				list_text (2, " (not currently connected)");
				break;
			}

			list_text (2, ";");
			if (dfo & 0x20)
			{
				list_text (2, " PSR NOGO");
			}
			else
			{
				list_text (2, " PSR GO");
			}

			list_text (2, ";");
			if (dfo & 0x10)
			{
				list_text (2, " SSR NOGO");
			}
			else
			{
				list_text (2, " SSR GO");
			}

			list_text (2, ";");
			if (dfo & 0x08)
			{
				list_text (2, " ModeS NOGO");
			}
			else
			{
				list_text (2, " ModeS GO");
			}

			list_text (2, ";");
			if (dfo & 0x04)
			{
				list_text (2, " ADS NOGO");
			}
			else
			{
				list_text (2, " ADS GO");
			}

			list_text (2, ";");
			if (dfo & 0x02)
			{
				list_text (2, " MLT NOGO");
			}
			else
			{
				list_text (2, " MLT GO");
			}
		}
		else if (ix == 1)
		{
			if ((dfo & 0x80) == 0x00)
			{
				list_text (2, " OPS");
			}

			if (dfo & 0x40)
			{
				list_text (2, " ODP");
			}

			if (dfo & 0x20)
			{
				list_text (2, " OXT");
			}

			if ((dfo & 0x10) == 0x00)
			{
				list_text (2, " MSC");
			}

			if (dfo & 0x08)
			{
				list_text (2, " TSV-invalid");
			}

			if (dfo & 0x04)
			{
				list_text (2, " NPW");
			}
		}
	}
	list_text (2, "\n");
#endif /* LISTER */

				   /* Store this information: */
	for (ix = 0; ix < length; ix ++)
	{
		dfo = buffer[ix];
		if (ix == 0)
		{
			t_Byte con;

			con = (dfo >> 6) & 0x03;

			ssta.connection_status = con;
			ssta.connection_status_present = TRUE;
		}
	}
	/* tba ... */

				   /* Set the return code: */
	ret = RC_OKAY;

	return ret;
}

/*----------------------------------------------------------------------------*/
/* proc_i063_070   -- Process I063/070 data item                              */
/*----------------------------------------------------------------------------*/

 static
 t_Retc proc_i063_070 (t_Ui16 length, t_Byte *buffer)
{
	t_Si16 bias;   /* Plot time stamping bias; milliseconds */
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
	bias = make_si16 (df1, df2);

#if LISTER
				   /* List data field: */
	list_text (2, ";  Time Stamping Bias:");
	list_text (2, " " M_FMT_SI16 " msec", bias);
	list_text (2, "\n");
#endif /* LISTER */

				   /* Store this information: */
	ssta.time_stamping_bias = (t_Secs) (0.001 * bias);
	ssta.time_stamping_bias_present = TRUE;

				   /* Set the return code: */
	ret = RC_OKAY;

	return ret;
}

/*----------------------------------------------------------------------------*/
/* proc_i063_080   -- Process I063/080 data item                              */
/*----------------------------------------------------------------------------*/

 static
 t_Retc proc_i063_080 (t_Ui16 length, t_Byte *buffer)
{
	t_Si16 bias;   /* SSR range bias; 1/128 nmi */
	t_Byte df1;    /* Data field octet 1 */
	t_Byte df2;    /* Data field octet 2 */
	t_Byte df3;    /* Data field octet 3 */
	t_Byte df4;    /* Data field octet 4 */
	t_Si16 gain;   /* SSR range gain; 10**-5 */
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
	gain = make_si16 (df1, df2);
	bias = make_si16 (df3, df4);

#if LISTER
				   /* List data field: */
	list_text (2, ";  SSR/ModeS Range Gain and Bias:");
	list_text (2, " bias=" M_FMT_SI16 " (%.3f nmi)",
                  bias, (1.0 / 128.0) * bias);
	list_text (2, " gain=" M_FMT_SI16 " (%.6f)",
                  gain, 0.00001 * gain);
	list_text (2, "\n");
#endif /* LISTER */

				   /* Store this information: */
	ssta.ssr_rng_bias = (M_NMI2MTR / 128.0) * bias;
	ssta.ssr_rng_bias_present = TRUE;
	ssta.ssr_rng_gain = 0.00001 * gain;
	ssta.ssr_rng_gain_present = TRUE;

				   /* Set the return code: */
	ret = RC_OKAY;

	return ret;
}

/*----------------------------------------------------------------------------*/
/* proc_i063_081   -- Process I063/081 data item                              */
/*----------------------------------------------------------------------------*/

 static
 t_Retc proc_i063_081 (t_Ui16 length, t_Byte *buffer)
{
	t_Si16 bias;   /* SSR azimuth bias; 360/2**16 degrees */
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
	bias = make_si16 (df1, df2);

#if LISTER
				   /* List data field: */
	list_text (2, ";  SSR/ModeS Azimuth Bias:");
	list_text (2, " " M_FMT_SI16 " (%.3f deg)",
                  bias, (360.0 / 65536.0) * bias);
	list_text (2, "\n");
#endif /* LISTER */

				   /* Store this information: */
	ssta.ssr_azm_bias = (360.0 / 65536.0) * bias;
	ssta.ssr_azm_bias_present = TRUE;

				   /* Set the return code: */
	ret = RC_OKAY;

	return ret;
}

/*----------------------------------------------------------------------------*/
/* proc_i063_090   -- Process I063/090 data item                              */
/*----------------------------------------------------------------------------*/

 static
 t_Retc proc_i063_090 (t_Ui16 length, t_Byte *buffer)
{
	t_Si16 bias;   /* PSR range bias; 1/128 nmi */
	t_Byte df1;    /* Data field octet 1 */
	t_Byte df2;    /* Data field octet 2 */
	t_Byte df3;    /* Data field octet 3 */
	t_Byte df4;    /* Data field octet 4 */
	t_Si16 gain;   /* PSR range gain; 10**-5 */
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
	gain = make_si16 (df1, df2);
	bias = make_si16 (df3, df4);

#if LISTER
				   /* List data field: */
	list_text (2, ";  PSR Range Gain and Bias:");
	list_text (2, " bias=" M_FMT_SI16 " (%.3f nmi)",
                  bias, (1.0 / 128.0) * bias);
	list_text (2, " gain=" M_FMT_SI16 " (%.6f)",
                  gain, 0.00001 * gain);
	list_text (2, "\n");
#endif /* LISTER */

				   /* Store this information: */
	ssta.psr_rng_bias = (M_NMI2MTR / 128.0) * bias;
	ssta.psr_rng_bias_present = TRUE;
	ssta.psr_rng_gain = 0.00001 * gain;
	ssta.psr_rng_gain_present = TRUE;

				   /* Set the return code: */
	ret = RC_OKAY;

	return ret;
}

/*----------------------------------------------------------------------------*/
/* proc_i063_091   -- Process I063/091 data item                              */
/*----------------------------------------------------------------------------*/

 static
 t_Retc proc_i063_091 (t_Ui16 length, t_Byte *buffer)
{
	t_Si16 bias;   /* PSR azimuth bias; 360/2**16 degrees */
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
	bias = make_si16 (df1, df2);

#if LISTER
				   /* List data field: */
	list_text (2, ";  PSR Azimuth Bias:");
	list_text (2, " " M_FMT_SI16 " (%.3f deg)",
                  bias, (360.0 / 65536.0) * bias);
	list_text (2, "\n");
#endif /* LISTER */

				   /* Store this information: */
	ssta.psr_azm_bias = (360.0 / 65536.0) * bias;
	ssta.psr_azm_bias_present = TRUE;

				   /* Set the return code: */
	ret = RC_OKAY;

	return ret;
}

/*----------------------------------------------------------------------------*/
/* proc_i063_092   -- Process I063/092 data item                              */
/*----------------------------------------------------------------------------*/

 static
 t_Retc proc_i063_092 (t_Ui16 length, t_Byte *buffer)
{
#if LISTER
	t_Si16 bias;   /* PSR elevation bias; 360/2**16 degrees */
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
	bias = make_si16 (df1, df2);

				   /* List data field: */
	list_text (2, ";  PSR Elevation Bias:");
	list_text (2, " " M_FMT_SI16 " (%.3f deg)",
                  bias, (360.0 / 65536.0) * bias);
	list_text (2, "\n");
#endif /* LISTER */

				   /* Store this information: */
	/* tba ... */

				   /* Set the return code: */
	ret = RC_OKAY;

	return ret;
}

/*----------------------------------------------------------------------------*/
/* proc_i063_ref   -- Process RE indicator data item                          */
/*----------------------------------------------------------------------------*/

 static
 t_Retc proc_i063_ref (t_Ui16 length, t_Byte *buffer, t_Ui16 *pos_ptr)
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
		error_msg ("Invalid buffer length (I063/REF)");
		goto done;
	}

#if LISTER
				   /* List raw data field: */
	list_buffer (1, ";  I063/REF:", len, buffer + pos);
#endif /* LISTER */

				   /* Set position: */
	*pos_ptr = pos + len;

                   /* Set the return code: */
	ret = RC_OKAY;

	done:          /* We are done */
	return ret;
}

/*----------------------------------------------------------------------------*/
/* proc_i063_spf   -- Process SP indicator data item                          */
/*----------------------------------------------------------------------------*/

 static
 t_Retc proc_i063_spf (t_Ui16 length, t_Byte *buffer, t_Ui16 *pos_ptr)
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
	list_buffer (1, ";  I063/SPF:", len, buffer + pos);
#endif /* LISTER */

				   /* Set position: */
	*pos_ptr = pos + len;

				   /* Set the return code: */
	ret = RC_OKAY;

	done:          /* We are done */
	return ret;
}
/* end-of-file */
