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
/* File:           src/astx_031.cpp                                           */
/* Contents:       Process ASTERIX category 031 data block                    */
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

#define M_MAX_FRN 14
				   /* Maximum field reference number */
#define M_MAX_FSPEC_LENGTH 2
				   /* Maximum fields specification length
					  for ASTERIX category 031 */

				   /* Local variables: */
				   /* ---------------- */

static t_Data_Item_Desc desc_i031_010;
                   /* Description of data item I031/010 */
static t_Data_Item_Desc desc_i031_015;
                   /* Description of data item I031/015 */
static t_Data_Item_Desc desc_i031_020;
                   /* Description of data item I031/020 */
static t_Data_Item_Desc desc_i031_030;
                   /* Description of data item I031/030 */
static t_Data_Item_Desc desc_i031_040;
                   /* Description of data item I031/040 */
static t_Data_Item_Desc desc_i031_050;
                   /* Description of data item I031/050 */
static t_Data_Item_Desc desc_i031_060;
                   /* Description of data item I031/060 */
static t_Data_Item_Desc desc_i031_070;
                   /* Description of data item I031/070 */
static t_Data_Item_Desc desc_i031_080;
                   /* Description of data item I031/080 */
static t_Data_Item_Desc desc_i031_090;
                   /* Description of data item I031/090 */
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
static t_Retc proc_i031_010 (t_Ui16 length, t_Byte *buffer);
                   /* Process I031/010 data item */
static t_Retc proc_i031_015 (t_Ui16 length, t_Byte *buffer);
                   /* Process I031/015 data item */
static t_Retc proc_i031_020 (t_Ui16 length, t_Byte *buffer);
                   /* Process I031/020 data item */
static t_Retc proc_i031_030 (t_Ui16 length, t_Byte *buffer);
                   /* Process I031/030 data item */
static t_Retc proc_i031_040 (t_Ui16 length, t_Byte *buffer);
                   /* Process I031/040 data item */
static t_Retc proc_i031_050 (t_Ui16 length, t_Byte *buffer);
                   /* Process I031/050 data item */
static t_Retc proc_i031_060 (t_Ui16 length, t_Byte *buffer);
                   /* Process I031/060 data item */
static t_Retc proc_i031_070 (t_Ui16 length, t_Byte *buffer);
                   /* Process I031/070 data item */
static t_Retc proc_i031_080 (t_Ui16 length, t_Byte *buffer);
                   /* Process I031/080 data item */
static t_Retc proc_i031_090 (t_Ui16 length, t_Byte *buffer);
                   /* Process I031/090 data item */

/*----------------------------------------------------------------------------*/
/* astx_031        -- Process ASTERIX category 031 data block                 */
/*----------------------------------------------------------------------------*/

 t_Retc astx_031 (t_Ui16 length, t_Byte *buffer)
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

                   /* Preset sensor status information: */
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

					lrc = data_item (31, frn, std_uap[frn],
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
                   /* Description of data item I031/010: */
	desc_i031_010.category = 31;
	desc_i031_010.data_item = 10;
	desc_i031_010.item_type = e_fixed_length_data_item;
	desc_i031_010.fixed_length = 2;
	desc_i031_010.proc_fptr = proc_i031_010;
	desc_i031_010.read_fptr = NULL;

                   /* Description of data item I031/015: */
	desc_i031_015.category = 31;
	desc_i031_015.data_item = 15;
	desc_i031_015.item_type = e_fixed_length_data_item;
	desc_i031_015.fixed_length = 2;
	desc_i031_015.proc_fptr = proc_i031_015;
	desc_i031_015.read_fptr = NULL;

                   /* Description of data item I031/020: */
	desc_i031_020.category = 31;
	desc_i031_020.data_item = 20;
	desc_i031_020.item_type = e_fixed_length_data_item;
	desc_i031_020.fixed_length = 3;
	desc_i031_020.proc_fptr = proc_i031_020;
	desc_i031_020.read_fptr = NULL;

                   /* Description of data item I031/030: */
	desc_i031_030.category = 31;
	desc_i031_030.data_item = 30;
	desc_i031_030.item_type = e_fixed_length_data_item;
	desc_i031_030.fixed_length = 2;
	desc_i031_030.proc_fptr = proc_i031_030;
	desc_i031_030.read_fptr = NULL;

                   /* Description of data item I031/040: */
	desc_i031_040.category = 31;
	desc_i031_040.data_item = 40;
	desc_i031_040.item_type = e_fixed_length_data_item;
	desc_i031_040.fixed_length = 1;
	desc_i031_040.proc_fptr = proc_i031_040;
	desc_i031_040.read_fptr = NULL;

                   /* Description of data item I031/050: */
	desc_i031_050.category = 31;
	desc_i031_050.data_item = 50;
	desc_i031_050.item_type = e_fixed_length_data_item;
	desc_i031_050.fixed_length = 2;
	desc_i031_050.proc_fptr = proc_i031_050;
	desc_i031_050.read_fptr = NULL;

                   /* Description of data item I031/060: */
	desc_i031_060.category = 31;
	desc_i031_060.data_item = 60;
	desc_i031_060.item_type = e_fixed_length_data_item;
	desc_i031_060.fixed_length = 4;
	desc_i031_060.proc_fptr = proc_i031_060;
	desc_i031_060.read_fptr = NULL;

                   /* Description of data item I031/070: */
	desc_i031_070.category = 31;
	desc_i031_070.data_item = 70;
	desc_i031_070.item_type = e_fixed_length_data_item;
	desc_i031_070.fixed_length = 2;
	desc_i031_070.proc_fptr = proc_i031_070;
	desc_i031_070.read_fptr = NULL;

                   /* Description of data item I031/080: */
	desc_i031_080.category = 31;
	desc_i031_080.data_item = 80;
	desc_i031_080.item_type = e_fixed_length_data_item;
	desc_i031_080.fixed_length = 4;
	desc_i031_080.proc_fptr = proc_i031_080;
	desc_i031_080.read_fptr = NULL;

                   /* Description of data item I031/090: */
	desc_i031_090.category = 31;
	desc_i031_090.data_item = 90;
	desc_i031_090.item_type = e_fixed_length_data_item;
	desc_i031_090.fixed_length = 2;
	desc_i031_090.proc_fptr = proc_i031_090;
	desc_i031_090.read_fptr = NULL;

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

	std_uap[1] = &desc_i031_010;
	std_uap[2] = &desc_i031_015;
	std_uap[3] = &desc_i031_020;
	std_uap[4] = &desc_i031_030;
	std_uap[5] = &desc_i031_040;
	std_uap[6] = &desc_i031_050;
	std_uap[7] = &desc_i031_060;

	std_uap[8] = &desc_i031_070;
	std_uap[9] = &desc_i031_080;
	std_uap[10] = &desc_i031_090;

	return;
}

/*----------------------------------------------------------------------------*/
/* proc_i031_010   -- Process I031/010 data item                              */
/*----------------------------------------------------------------------------*/

 static
 t_Retc proc_i031_010 (t_Ui16 length, t_Byte *buffer)
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
/* proc_i031_015   -- Process I031/015 data item                              */
/*----------------------------------------------------------------------------*/

 static
 t_Retc proc_i031_015 (t_Ui16 length, t_Byte *buffer)
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
	list_text (2, " " M_FMT_BYTE, usr);
	list_text (2, "\n");
#endif /* LISTER */

				   /* Store this information: */
	ssta.user_number.present = TRUE;
	ssta.user_number.value = usr;

				   /* Set the return code: */
	ret = RC_OKAY;

	return ret;
}

/*----------------------------------------------------------------------------*/
/* proc_i031_020   -- Process I031/020 data item                              */
/*----------------------------------------------------------------------------*/

 static
 t_Retc proc_i031_020 (t_Ui16 length, t_Byte *buffer)
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
/* proc_i031_030   -- Process I031/030 data item                              */
/*----------------------------------------------------------------------------*/

 static
 t_Retc proc_i031_030 (t_Ui16 length, t_Byte *buffer)
{
	t_Byte df1;    /* Data field octet 1 */
	t_Byte df2;    /* Data field octet 2 */
	t_Ui16 dsi;    /* Data source identifier */
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
	list_text (2, ";  Sensor Identification Tag:");
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
/* proc_i031_040   -- Process I031/040 data item                              */
/*----------------------------------------------------------------------------*/

 static
 t_Retc proc_i031_040 (t_Ui16 length, t_Byte *buffer)
{
	t_Byte df1;    /* Data field octet 1 */
	t_Retc ret;    /* Return code */
	t_Byte st;     /* Sensor status */

				   /* Preset the return code: */
	ret = RC_FAIL;

				   /* Check parameters: */
	Assert (length == 1, "Invalid parameter");
	Assert (buffer != NULL, "Invalid parameter");

				   /* Extract octets: */
	df1 = buffer[0];

				   /* Extract information: */
	st = (df1 >> 5) & 0x07;

#if LISTER
				   /* List data field: */
	list_text (2, ";  Sensor Status:");
	list_text (2, " " M_FMT_BYTE, st);
	list_text (2, "\n");
#endif /* LISTER */

				   /* Store this information: */
	ssta.connection_status = st;
	ssta.connection_status_present = TRUE;

				   /* Set the return code: */
	ret = RC_OKAY;

	return ret;
}

/*----------------------------------------------------------------------------*/
/* proc_i031_050   -- Process I031/050 data item                              */
/*----------------------------------------------------------------------------*/

 static
 t_Retc proc_i031_050 (t_Ui16 length, t_Byte *buffer)
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
	bias = make_si16 (df1 & 0x3f, df2);
	if (df1 & 0x20)
	{
		bias |= 0xc000;
	}

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
/* proc_i031_060   -- Process I031/060 data item                              */
/*----------------------------------------------------------------------------*/

 static
 t_Retc proc_i031_060 (t_Ui16 length, t_Byte *buffer)
{
	t_Si16 bias;   /* SSR range bias; 1/128 nmi */
	t_Byte df1;    /* Data field octet 1 */
	t_Byte df2;    /* Data field octet 2 */
	t_Byte df3;    /* Data field octet 3 */
	t_Byte df4;    /* Data field octet 4 */
	t_Si32 gain;   /* SSR range gain; 10**-6 */
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
	bias = make_si16 (df1 & 0x3f, df2 & 0xf0);
	if (df1 & 0x40)
	{
		bias |= 0xc000;
	}
	bias = (bias >> 4);
	if (df2 & 0x08)
	{
		gain = make_si32 (0xff, df2 | 0xf0, df3, df4);
	}
	else
	{
		gain = make_si32 (0x00, df2 & 0x0f, df3, df4);
	}

#if LISTER
				   /* List data field: */
	list_text (2, ";  SSR Range Gain and Bias:");
	list_text (2, " bias=" M_FMT_SI16 " (%.3f nmi);",
                  bias, (1.0 / 128.0) * bias);
	list_text (2, " gain=" M_FMT_SI16 " (%.6f)",
                  gain, 0.000001 * gain);
	list_text (2, "\n");
#endif /* LISTER */

				   /* Store this information: */
	ssta.ssr_rng_bias = (M_NMI2MTR / 128.0) * bias;
	ssta.ssr_rng_bias_present = TRUE;
	ssta.ssr_rng_gain = 0.000001 * gain;
	ssta.ssr_rng_gain_present = TRUE;

				   /* Set the return code: */
	ret = RC_OKAY;

	return ret;
}

/*----------------------------------------------------------------------------*/
/* proc_i031_070   -- Process I031/070 data item                              */
/*----------------------------------------------------------------------------*/

 static
 t_Retc proc_i031_070 (t_Ui16 length, t_Byte *buffer)
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
	list_text (2, ";  SSR Azimuth Bias:");
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
/* proc_i031_080   -- Process I031/080 data item                              */
/*----------------------------------------------------------------------------*/

 static
 t_Retc proc_i031_080 (t_Ui16 length, t_Byte *buffer)
{
	t_Si16 bias;   /* PR range bias; 1/128 nmi */
	t_Byte df1;    /* Data field octet 1 */
	t_Byte df2;    /* Data field octet 2 */
	t_Byte df3;    /* Data field octet 3 */
	t_Byte df4;    /* Data field octet 4 */
	t_Si32 gain;   /* PR range gain; 10**-6 */
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
	bias = make_si16 (df1 & 0x3f, df2 & 0xf0);
	if (df1 & 0x40)
	{
		bias |= 0xc000;
	}
	bias = (bias >> 4);
	if (df2 & 0x08)
	{
		gain = make_si32 (0xff, df2 | 0xf0, df3, df4);
	}
	else
	{
		gain = make_si32 (0x00, df2 & 0x0f, df3, df4);
	}

#if LISTER
				   /* List data field: */
	list_text (2, ";  PR Range Gain and Bias:");
	list_text (2, " bias=" M_FMT_SI16 " (%.3f nmi);",
                  bias, (1.0 / 128.0) * bias);
	list_text (2, " gain=" M_FMT_SI16 " (%.6f)",
                  gain, 0.000001 * gain);
	list_text (2, "\n");
#endif /* LISTER */

                   /* Store this information: */
	ssta.psr_rng_bias = (M_NMI2MTR / 128.0) * bias;
	ssta.psr_rng_bias_present = TRUE;
	ssta.psr_rng_gain = 0.000001 * gain;
	ssta.psr_rng_gain_present = TRUE;
				   /* Set the return code: */
	ret = RC_OKAY;

	return ret;
}

/*----------------------------------------------------------------------------*/
/* proc_i031_090   -- Process I031/090 data item                              */
/*----------------------------------------------------------------------------*/

 static
 t_Retc proc_i031_090 (t_Ui16 length, t_Byte *buffer)
{
	t_Si16 bias;   /* PR azimuth bias; 360/2**16 degrees */
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
	list_text (2, ";  PR Azimuth Bias:");
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
/* end-of-file */
