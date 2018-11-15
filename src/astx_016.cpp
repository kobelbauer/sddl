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
/* File:           src/astx_016.cpp                                           */
/* Contents:       Process ASTERIX category 016 data block                    */
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

#define M_MAX_FRN 21
				   /* Maximum field reference number */
#define M_MAX_FSPEC_LENGTH 3
				   /* Maximum fields specification length
					  for ASTERIX category 016 */

				   /* Local variables: */
				   /* ---------------- */

static t_Data_Item_Desc desc_i016_010;
                   /* Description of data item I016/010 */
static t_Data_Item_Desc desc_i016_020;
                   /* Description of data item I016/020 */
static t_Data_Item_Desc desc_i016_040;
                   /* Description of data item I016/040 */
static t_Data_Item_Desc desc_i016_042;
                   /* Description of data item I016/042 */
static t_Data_Item_Desc desc_i016_070;
                   /* Description of data item I016/070 */
static t_Data_Item_Desc desc_i016_090;
                   /* Description of data item I016/090 */
static t_Data_Item_Desc desc_i016_130;
                   /* Description of data item I016/130 */
static t_Data_Item_Desc desc_i016_140;
                   /* Description of data item I016/140 */
static t_Data_Item_Desc desc_i016_161;
                   /* Description of data item I016/161 */
static t_Data_Item_Desc desc_i016_170;
                   /* Description of data item I016/170 */
static t_Data_Item_Desc desc_i016_210;
                   /* Description of data item I016/210 */
static t_Data_Item_Desc desc_i016_220;
                   /* Description of data item I016/220 */
static t_Data_Item_Desc desc_i016_230;
                   /* Description of data item I016/230 */
static t_Data_Item_Desc desc_i016_240;
                   /* Description of data item I016/240 */
static t_Data_Item_Desc desc_i016_250;
                   /* Description of data item I016/250 */
static t_Data_Item_Desc desc_i016_260;
                   /* Description of data item I016/260 */
static t_Data_Item_Desc desc_i016_rfs;
                   /* Description of RFS data item */
static t_Data_Item_Desc desc_i016_spf;
                   /* Description of SPF data item */
static t_Bool initiated = FALSE;
				   /* Package initiated flag */
static t_Bool is_a_rtrk;
                   /* Radar track flag */
static t_Data_Item_Desc *std_uap[M_MAX_FRN + 1];
				   /* Standard User Application Profile */

				   /* Local functions: */
				   /* ---------------- */

static void init_desc (void);
                   /* Initiate data item descriptions */
static void load_std_uap (void);
                   /* Load standard UAP */
static t_Retc proc_i016_010 (t_Ui16 length, t_Byte *buffer);
                   /* Process I016/010 data item */
static t_Retc proc_i016_020 (t_Ui16 length, t_Byte *buffer);
                   /* Process I016/020 data item */
static t_Retc proc_i016_040 (t_Ui16 length, t_Byte *buffer);
                   /* Process I016/040 data item */
static t_Retc proc_i016_042 (t_Ui16 length, t_Byte *buffer);
                   /* Process I016/042 data item */
static t_Retc proc_i016_070 (t_Ui16 length, t_Byte *buffer);
                   /* Process I016/070 data item */
static t_Retc proc_i016_090 (t_Ui16 length, t_Byte *buffer);
                   /* Process I016/090 data item */
static t_Retc proc_i016_130 (t_Ui16 length, t_Byte *buffer);
                   /* Process I016/130 data item */
static t_Retc proc_i016_140 (t_Ui16 length, t_Byte *buffer);
                   /* Process I016/140 data item */
static t_Retc proc_i016_161 (t_Ui16 length, t_Byte *buffer);
                   /* Process I016/161 data item */
static t_Retc proc_i016_170 (t_Ui16 length, t_Byte *buffer);
                   /* Process I016/170 data item */
static t_Retc proc_i016_210 (t_Ui16 length, t_Byte *buffer);
                   /* Process I016/210 data item */
static t_Retc proc_i016_220 (t_Ui16 length, t_Byte *buffer);
                   /* Process I016/220 data item */
static t_Retc proc_i016_230 (t_Ui16 length, t_Byte *buffer);
                   /* Process I016/230 data item */
static t_Retc proc_i016_240 (t_Ui16 length, t_Byte *buffer);
                   /* Process I016/240 data item */
static t_Retc proc_i016_250 (t_Ui16 length, t_Byte *buffer);
                   /* Process I016/250 data item */
static t_Retc proc_i016_260 (t_Ui16 length, t_Byte *buffer);
                   /* Process I016/260 data item */
static t_Retc proc_i016_rfs (t_Ui16 length, t_Byte *buffer, t_Ui16 *pos_ptr);
                   /* Process RFS indicator data item */
static t_Retc proc_i016_spf (t_Ui16 length, t_Byte *buffer, t_Ui16 *pos_ptr);
                   /* Process SPF indicator data item */

/*----------------------------------------------------------------------------*/
/* astx_016        -- Process ASTERIX category 016 data block                 */
/*----------------------------------------------------------------------------*/

 t_Retc astx_016 (t_Ui16 length, t_Byte *buffer)
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

                   /* Preset radar target information: */
		memset (&rtgt, 0, sizeof (rtgt));

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

					lrc = data_item (16, frn, std_uap[frn],
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
			rtgt.frame_date.present = TRUE;
			rtgt.frame_date.value = frame_date;
		}

                   /* Add frame time, if available: */
		if (frame_time_present)
		{
			rtgt.frame_time.present = TRUE;
			rtgt.frame_time.value = frame_time;
		}

                   /* Add board/line number, if present: */
		if (current_line_number_defined)
		{
			rtgt.line_number = current_line_number;
		}

                   /* If radar target doesn't hold a SAC/SIC, but a
                      last SAC/SIC is available, copy this SAC/SIC
                      to the radar target: */
		if (!rtgt.data_source_identifier.present && last_sacsic_available)
		{
			rtgt.data_source_identifier.present = TRUE;
			rtgt.data_source_identifier.value = last_sacsic;
            rtgt.data_source_identifier.sac = last_sacsic >> 8;
            rtgt.data_source_identifier.sic = last_sacsic % 256;
		}

                   /* Process this radar target: */
		lrc = process_rtgt (&rtgt);
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
                   /* Description of data item I016/010: */
	desc_i016_010.category = 16;
	desc_i016_010.data_item = 10;
	desc_i016_010.item_type = e_fixed_length_data_item;
	desc_i016_010.fixed_length = 2;
	desc_i016_010.proc_fptr = proc_i016_010;
	desc_i016_010.read_fptr = NULL;

                   /* Description of data item I016/020: */
	desc_i016_020.category = 16;
	desc_i016_020.data_item = 20;
	desc_i016_020.item_type = e_variable_length_data_item;
	desc_i016_020.fixed_length = 0;
	desc_i016_020.proc_fptr = proc_i016_020;
	desc_i016_020.read_fptr = NULL;

                   /* Description of data item I016/040: */
	desc_i016_040.category = 16;
	desc_i016_040.data_item = 40;
	desc_i016_040.item_type = e_fixed_length_data_item;
	desc_i016_040.fixed_length = 4;
	desc_i016_040.proc_fptr = proc_i016_040;
	desc_i016_040.read_fptr = NULL;

                   /* Description of data item I016/042: */
	desc_i016_042.category = 16;
	desc_i016_042.data_item = 42;
	desc_i016_042.item_type = e_fixed_length_data_item;
	desc_i016_042.fixed_length = 4;
	desc_i016_042.proc_fptr = proc_i016_042;
	desc_i016_042.read_fptr = NULL;

                   /* Description of data item I016/070: */
	desc_i016_070.category = 16;
	desc_i016_070.data_item = 70;
	desc_i016_070.item_type = e_fixed_length_data_item;
	desc_i016_070.fixed_length = 2;
	desc_i016_070.proc_fptr = proc_i016_070;
	desc_i016_070.read_fptr = NULL;

                   /* Description of data item I016/090: */
	desc_i016_090.category = 16;
	desc_i016_090.data_item = 90;
	desc_i016_090.item_type = e_fixed_length_data_item;
	desc_i016_090.fixed_length = 2;
	desc_i016_090.proc_fptr = proc_i016_090;
	desc_i016_090.read_fptr = NULL;

                   /* Description of data item I016/130: */
	desc_i016_130.category = 16;
	desc_i016_130.data_item = 130;
	desc_i016_130.item_type = e_variable_length_data_item;
	desc_i016_130.fixed_length = 0;
	desc_i016_130.proc_fptr = proc_i016_130;
	desc_i016_130.read_fptr = NULL;

                   /* Description of data item I016/140: */
	desc_i016_140.category = 16;
	desc_i016_140.data_item = 140;
	desc_i016_140.item_type = e_fixed_length_data_item;
	desc_i016_140.fixed_length = 3;
	desc_i016_140.proc_fptr = proc_i016_140;
	desc_i016_140.read_fptr = NULL;

                   /* Description of data item I016/161: */
	desc_i016_161.category = 16;
	desc_i016_161.data_item = 161;
	desc_i016_161.item_type = e_fixed_length_data_item;
	desc_i016_161.fixed_length = 2;
	desc_i016_161.proc_fptr = proc_i016_161;
	desc_i016_161.read_fptr = NULL;

                   /* Description of data item I016/170: */
	desc_i016_170.category = 16;
	desc_i016_170.data_item = 170;
	desc_i016_170.item_type = e_variable_length_data_item;
	desc_i016_170.fixed_length = 0;
	desc_i016_170.proc_fptr = proc_i016_170;
	desc_i016_170.read_fptr = NULL;

                   /* Description of data item I016/210: */
	desc_i016_210.category = 16;
	desc_i016_210.data_item = 210;
	desc_i016_210.item_type = e_variable_length_data_item;
	desc_i016_210.fixed_length = 0;
	desc_i016_210.proc_fptr = proc_i016_210;
	desc_i016_210.read_fptr = NULL;

                   /* Description of data item I016/220: */
	desc_i016_220.category = 16;
	desc_i016_220.data_item = 220;
	desc_i016_220.item_type = e_fixed_length_data_item;
	desc_i016_220.fixed_length = 3;
	desc_i016_220.proc_fptr = proc_i016_220;
	desc_i016_220.read_fptr = NULL;

                   /* Description of data item I016/230: */
	desc_i016_230.category = 16;
	desc_i016_230.data_item = 230;
	desc_i016_230.item_type = e_fixed_length_data_item;
	desc_i016_230.fixed_length = 1;
	desc_i016_230.proc_fptr = proc_i016_230;
	desc_i016_230.read_fptr = NULL;

                   /* Description of data item I016/240: */
	desc_i016_240.category = 16;
	desc_i016_240.data_item = 240;
	desc_i016_240.item_type = e_fixed_length_data_item;
	desc_i016_240.fixed_length = 6;
	desc_i016_240.proc_fptr = proc_i016_240;
	desc_i016_240.read_fptr = NULL;

                   /* Description of data item I016/250: */
	desc_i016_250.category = 16;
	desc_i016_250.data_item = 250;
	desc_i016_250.item_type = e_repetitive_data_item;
	desc_i016_250.fixed_length = 8;
	desc_i016_250.proc_fptr = proc_i016_250;
	desc_i016_250.read_fptr = NULL;

                   /* Description of data item I016/260: */
	desc_i016_260.category = 16;
	desc_i016_260.data_item = 260;
	desc_i016_260.item_type = e_fixed_length_data_item;
	desc_i016_260.fixed_length = 6;
	desc_i016_260.proc_fptr = proc_i016_260;
	desc_i016_260.read_fptr = NULL;

                   /* Description of RFS data item: */
	desc_i016_rfs.category = 16;
	desc_i016_rfs.data_item = M_RFS_INDICATOR;
	desc_i016_rfs.item_type = e_immediate_data_item;
	desc_i016_rfs.fixed_length = 0;
	desc_i016_rfs.proc_fptr = NULL;
	desc_i016_rfs.read_fptr = proc_i016_rfs;

                   /* Description of SPF data item: */
	desc_i016_spf.category = 16;
	desc_i016_spf.data_item = M_SPF_INDICATOR;
	desc_i016_spf.item_type = e_immediate_data_item;
	desc_i016_spf.fixed_length = 0;
	desc_i016_spf.proc_fptr = NULL;
	desc_i016_spf.read_fptr = proc_i016_spf;

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

                   /* Load the standard UAP: */

	std_uap[1] = &desc_i016_010;
	std_uap[2] = &desc_i016_020;
	std_uap[3] = &desc_i016_040;
	std_uap[4] = &desc_i016_042;
	std_uap[5] = &desc_i016_070;
	std_uap[6] = &desc_i016_220;
	std_uap[7] = &desc_i016_140;

	std_uap[8] = &desc_i016_230;
	std_uap[9] = &desc_i016_250;
	std_uap[10] = &desc_i016_240;
	std_uap[11] = &desc_i016_161;
	std_uap[12] = &desc_i016_170;
	std_uap[13] = &desc_i016_210;
	std_uap[14] = &desc_i016_090;

	std_uap[15] = &desc_i016_260;
	std_uap[16] = &desc_i016_130;
	std_uap[20] = &desc_i016_spf;
	std_uap[21] = &desc_i016_rfs;

	return;
}

/*----------------------------------------------------------------------------*/
/* proc_i016_010   -- Process I016/010 data item                              */
/*----------------------------------------------------------------------------*/

 static
 t_Retc proc_i016_010 (t_Ui16 length, t_Byte *buffer)
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
	rtgt.data_source_identifier.present = TRUE;
	rtgt.data_source_identifier.value = dsi;
    rtgt.data_source_identifier.sac = df1;
    rtgt.data_source_identifier.sic = df2;

				   /* Set the return value: */
	ret = RC_OKAY;

	return ret;
}

/*----------------------------------------------------------------------------*/
/* proc_i016_020   -- Process I016/020 data item                              */
/*----------------------------------------------------------------------------*/

 static
 t_Retc proc_i016_020 (t_Ui16 length, t_Byte *buffer)
{
	t_Byte dfo;    /* Data field octet */
	int ix;        /* Auxiliary */
	t_Retc ret;    /* Return code */

				   /* Preset the return code: */
	ret = RC_FAIL;

				   /* Check parameters: */
	Assert (length > 0, "Invalid parameter");
	Assert (buffer != NULL, "Invalid parameter");

                   /* Determine whether plot or radar track: */
	dfo = buffer[0];
	if ((dfo & 0x80) == 0)
	{
		is_a_rtrk = FALSE;
	}
	else
	{
		is_a_rtrk = TRUE;
	}

#if LISTER
				   /* List data field: */
	list_text (2, ";  Target Descriptor:");
	for (ix = 0; ix < length; ix ++)
	{
		dfo = buffer[ix];

		if (ix == 0)
		{
			t_Byte rdt;

			if (dfo & 0x80)
			{
				list_text (2, " TRK");
			}
			else
			{
				list_text (2, " PLT");
			}

			if (dfo & 0x40)
			{
				list_text (2, " SIM");
			}
			else
			{
				list_text (2, " ACT");
			}

			rdt = (dfo >> 4) & 0x03;
			if (rdt == 0)
			{
				list_text (2, " ---");
			}
			else if (rdt == 1)
			{
				list_text (2, " PRI");
			}
			else if (rdt == 2)
			{
				list_text (2, " SEC");
			}
			else
			{
				list_text (2, " CMB");
			}

			if (dfo & 0x08)
			{
				list_text (2, " A2");
			}
			else
			{
				list_text (2, " A1");
			}

			if (dfo & 0x04)
			{
				list_text (2, " SPI");
			}

			if (dfo & 0x02)
			{
				list_text (2, " RAB");
			}
		}
		else if (ix == 1)
		{
			t_Byte emg;

			if (dfo & 0x80)
			{
				list_text (2, " TST");
			}

			emg = (dfo >> 5) & 0x03;
			if (emg == 1)
			{
				list_text (2, " EM1");
			}
			else if (emg == 2)
			{
				list_text (2, " EM2");
			}
			else if (emg == 3)
			{
				list_text (2, " EM3");
			}

			if (dfo & 0x10)
			{
				list_text (2, " ME");
			}

			if (dfo & 0x08)
			{
				list_text (2, " MI");
			}

			if (dfo & 0x04)
			{
				list_text (2, " TA");
			}

			if (dfo & 0x02)
			{
				list_text (2, " DUP");
			}
		}
	}
	list_text (2, "\n");
#endif /* LISTER */

				   /* Evaluate and store this information: */
	rtgt.detection_type.present = TRUE;
	for (ix = 0; ix < length; ix ++)
	{
    	dfo = buffer[ix];

		if (ix == 0)
		{
			rtgt.detection_type.is_radar_track = M_TRES (dfo & 0x80);

			rtgt.detection_type.simulated = M_TRES (dfo & 0x40);

			rtgt.detection_type.reported_from_ssr = M_TRES (dfo & 0x20);
			rtgt.detection_type.reported_from_psr = M_TRES (dfo & 0x10);

			rtgt.antenna_number.present = TRUE;
			if (dfo & 0x08)
			{
				rtgt.antenna_number.value = 2;
			}
			else
			{
				rtgt.antenna_number.value = 1;
			}

			rtgt.special_position_indication = M_TRES (dfo & 0x04);

			rtgt.detection_type.from_fixed_field_transponder =
				M_TRES (dfo & 0x02);
		}
		else if (ix == 1)
		{
			/* tba ... */
		}
	}

				   /* Set the return code: */
	ret = RC_OKAY;

	return ret;
}

/*----------------------------------------------------------------------------*/
/* proc_i016_040   -- Process I016/040 data item                              */
/*----------------------------------------------------------------------------*/

 static
 t_Retc proc_i016_040 (t_Ui16 length, t_Byte *buffer)
{
	t_Byte df1;    /* Data field octet 1 */
	t_Byte df2;    /* Data field octet 2 */
	t_Byte df3;    /* Data field octet 3 */
	t_Byte df4;    /* Data field octet 4 */
	t_Retc ret;    /* Return code */
	t_Ui16 rho;    /* Slant range; 1/128 nautical miles */
	t_Ui16 theta;  /* Azimuth; 360/2**16 degrees */

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
	rho = make_ui16 (df1, df2);
	theta = make_ui16 (df3, df4);

#if LISTER
				   /* List data field: */
	list_text (2, ";  Measured Position:");
	list_text (2, " rng=" M_FMT_UI16 " (%.3f nmi);",
                  rho, (1.0 / 128.0) * rho);
	list_text (2, " azm=" M_FMT_UI16 " (%.3f deg)",
                  theta, (360.0 / 65536.0) * theta);
	list_text (2, "\n");
#endif /* LISTER */

				   /* Store this information: */
	rtgt.measured_azm.present = TRUE;
	rtgt.measured_azm.value = (M_TWO_PI / 65536.0) * theta;
	rtgt.measured_rng.present = TRUE;
	rtgt.measured_rng.value = (M_NMI2MTR / 128.0) * rho;

				   /* Set the return code: */
	ret = RC_OKAY;

	return ret;
}

/*----------------------------------------------------------------------------*/
/* proc_i016_042   -- Process I016/042 data item                              */
/*----------------------------------------------------------------------------*/

 static
 t_Retc proc_i016_042 (t_Ui16 length, t_Byte *buffer)
{
	t_Byte df1;    /* Data field octet 1 */
	t_Byte df2;    /* Data field octet 2 */
	t_Byte df3;    /* Data field octet 3 */
	t_Byte df4;    /* Data field octet 4 */
	t_Retc ret;    /* Return code */
	t_Si16 x;      /* x component; 1/64 nautical miles */
	t_Si16 y;      /* y component; 1/64 nautical miles */

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
	x = make_si16 (df1, df2);
	y = make_si16 (df3, df4);

#if LISTER
				   /* List data field: */
	list_text (2, ";  Calculated Position:");
	list_text (2, " x=" M_FMT_SI16 " (%.3f nmi);", x, (1.0 / 64.0) * x);
	list_text (2, " y=" M_FMT_SI16 " (%.3f nmi)", y, (1.0 / 64.0) * y);
	list_text (2, "\n");
#endif /* LISTER */

				   /* Store this information: */
	rtgt.computed_position.present = TRUE;
	rtgt.computed_position.value_x = (M_NMI2MTR / 64.0) * x;
	rtgt.computed_position.value_y = (M_NMI2MTR / 64.0) * y;

				   /* Set the return code: */
	ret = RC_OKAY;

	return ret;
}

/*----------------------------------------------------------------------------*/
/* proc_i016_070   -- Process I016/070 data item                              */
/*----------------------------------------------------------------------------*/

 static
 t_Retc proc_i016_070 (t_Ui16 length, t_Byte *buffer)
{
	t_Byte df1;    /* Data field octet 1 */
	t_Byte df2;    /* Data field octet 2 */
	t_Ui16 m3c;    /* Mode 3/A code */
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
	list_text (2, ";  Mode 3/A Code:");
	list_text (2, " v=" M_FMT_BYTE ";", (df1 >> 7) & 0x01);
	list_text (2, " g=" M_FMT_BYTE ";", (df1 >> 6) & 0x01);
	list_text (2, " l=" M_FMT_BYTE ";", (df1 >> 5) & 0x01);
	list_text (2, " code=" M_FMT_CODE, m3c);
	list_text (2, "\n");
#endif /* LISTER */

				   /* Store this information: */
	if (m3c != 00000 && (df1 & 0x80) == 0)
	{
		rtgt.mode_3a_info.code = m3c;
		rtgt.mode_3a_info.code_garbled = M_TRES (df1 & 0x40);
		rtgt.mode_3a_info.code_invalid = M_TRES (df1 & 0x80);
		rtgt.mode_3a_info.code_smoothed = M_TRES (df1 & 0x20);
		rtgt.mode_3a_info.present = TRUE;
	}

				   /* Set the return code: */
	ret = RC_OKAY;

	return ret;
}

/*----------------------------------------------------------------------------*/
/* proc_i016_090   -- Process I016/090 data item                              */
/*----------------------------------------------------------------------------*/

 static
 t_Retc proc_i016_090 (t_Ui16 length, t_Byte *buffer)
{
	t_Byte df1;    /* Data field octet 1 */
	t_Byte df2;    /* Data field octet 2 */
	t_Si16 mch;    /* Mode C height; 25 feet */
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
	mch = make_si16 (df1 & 0x1f, df2);
	if (df1 & 0x20)
	{
		mch |= 0xc000;
                   /* Negative values in two's complement */
	}

#if LISTER
				   /* List data field: */
	list_text (2, ";  Mode C Code:");
	list_text (2, " v=" M_FMT_BYTE ";", (df1 >> 7) & 0x01);
	list_text (2, " g=" M_FMT_BYTE ";", (df1 >> 6) & 0x01);
	list_text (2, " code=" M_FMT_SI16 " (%.2f FL)", mch, 0.25 * mch);
	list_text (2, "\n");
#endif /* LISTER */

				   /* Store this information: */
	if (mch != 0 && (df1 & 0x80) == 0)
	{
		rtgt.mode_c_height.garbled = M_TRES (df1 & 0x40);
		rtgt.mode_c_height.height_in_error = e_is_false;
		rtgt.mode_c_height.in_25_feet = e_is_true;
		rtgt.mode_c_height.invalid = M_TRES (df1 & 0x80);
		rtgt.mode_c_height.present = TRUE;
		rtgt.mode_c_height.value = 25.0 * M_FT2MTR * mch;
		rtgt.mode_c_height.value_in_feet = 25 * (t_Si32) mch;
	}

				   /* Set the return code: */
	ret = RC_OKAY;

	return ret;
}

/*----------------------------------------------------------------------------*/
/* proc_i016_130   -- Process I016/130 data item                              */
/*----------------------------------------------------------------------------*/

 static
 t_Retc proc_i016_130 (t_Ui16 length, t_Byte *buffer)
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
	list_text (2, ";  Radar Plot Characteristics: 0x");
	for (ix = 0; ix < length; ix ++)
	{
		df1 = buffer[ix];
		list_text (2, " " M_FMT_BYTE_HEX, df1);
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
/* proc_i016_140   -- Process I016/140 data item                              */
/*----------------------------------------------------------------------------*/

 static
 t_Retc proc_i016_140 (t_Ui16 length, t_Byte *buffer)
{
	t_Secs atm;    /* Absolute time; seconds */
	t_Byte df1;    /* Data field octet 1 */
	t_Byte df2;    /* Data field octet 2 */
	t_Byte df3;    /* Data field octet 3 */
	t_Retc ret;    /* Return code */
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
	tod = make_ui32 (0x00, df1, df2, df3);

                   /* Compute absolute time (of day): */
	atm = (1.0 / 128.0) * tod;

#if LISTER
				   /* List data field: */
	list_text (2, ";  Time of Day:");
	list_text (2, " " M_FMT_ATOD " (" M_FMT_UI32 "; %s UTC)",
                  tod, tod, utc_text (atm));
	list_text (2, "\n");
#endif /* LISTER */

				   /* Store this information: */
	rtgt.detection_time.present = TRUE;
	rtgt.detection_time.value = atm;

				   /* Set the return code: */
	ret = RC_OKAY;

	return ret;
}

/*----------------------------------------------------------------------------*/
/* proc_i016_161   -- Process I016/161 data item                              */
/*----------------------------------------------------------------------------*/

 static
 t_Retc proc_i016_161 (t_Ui16 length, t_Byte *buffer)
{
	t_Byte df1;    /* Data field octet 1 */
	t_Byte df2;    /* Data field octet 2 */
	t_Ui16 num;    /* Track/Plot number */
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
	num = make_ui16 (df1, df2);

#if LISTER
				   /* List data field: */
	list_text (2, ";  Track/Plot Number:");
	list_text (2, " num=" M_FMT_UI16, num);
	list_text (2, "\n");
#endif /* LISTER */

				   /* Store this information: */
	rtgt.track_number.present = TRUE;
	rtgt.track_number.value = num;

				   /* Set the return code: */
	ret = RC_OKAY;

	return ret;
}

/*----------------------------------------------------------------------------*/
/* proc_i016_170   -- Process I016/170 data item                              */
/*----------------------------------------------------------------------------*/

 static
 t_Retc proc_i016_170 (t_Ui16 length, t_Byte *buffer)
{
	t_Byte df1;    /* Data field octet 1 */
	int ix;        /* Auxiliary */
	t_Retc ret;    /* Return code */

				   /* Preset the return code: */
	ret = RC_FAIL;

				   /* Check parameters: */
	Assert (length > 0, "Invalid parameter");
	Assert (buffer != NULL, "Invalid parameter");

#if LISTER
				   /* List data field: */
	list_text (2, ";  Track Status:");
	for (ix = 0; ix < length; ix ++)
	{
		df1 = buffer[ix];
		if (ix == 0)
		{
			if (df1 & 0x80)
			{
				list_text (2, " INI");
			}
			else
			{
				list_text (2, " CNF");
			}

			if (df1 & 0x40)
			{
				list_text (2, " SRT");
			}
			else
			{
				list_text (2, " PRT");
			}

			if (df1 & 0x20)
			{
				list_text (2, " MAN");
			}

			if (df1 & 0x10)
			{
				list_text (2, " DOU");
			}

			if (df1 & 0x08)
			{
				list_text (2, " CH2");
			}
			else
			{
				list_text (2, " CH1");
			}

			if (df1 & 0x02)
			{
				list_text (2, " GHO");
			}
		}
		else if (ix == 1)
		{
			if (df1 & 0x80)
			{
				list_text (2, " TRE");
			}
		}
	}
	list_text (2, "\n");
#endif /* LISTER */

				   /* Store this information: */
	for (ix = 0; ix < length; ix ++)
	{
		df1 = buffer[ix];

		if (ix == 0)
		{
			/* tba ... */
		}
		else if (ix == 1)
		{
			if (df1 & 0x80)
			{
				rtgt.to_be_cancelled = e_is_true;
			}
		}
	}

				   /* Set the return code: */
	ret = RC_OKAY;

	return ret;
}

/*----------------------------------------------------------------------------*/
/* proc_i016_210   -- Process I016/210 data item                              */
/*----------------------------------------------------------------------------*/

 static
 t_Retc proc_i016_210 (t_Ui16 length, t_Byte *buffer)
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
	list_text (2, ";  Track Quality:");
	for (ix = 0; ix < length; ix ++)
	{
		df1 = buffer[ix];
		list_text (2, " " M_FMT_BYTE, df1);
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
/* proc_i016_220   -- Process I016/220 data item                              */
/*----------------------------------------------------------------------------*/

 static
 t_Retc proc_i016_220 (t_Ui16 length, t_Byte *buffer)
{
	t_Byte df1;    /* Data field octet 1 */
	t_Byte df2;    /* Data field octet 2 */
	t_Byte df3;    /* Data field octet 3 */
	t_Ui32 msa;    /* SSR mode S address */
	t_Retc ret;    /* Return code */

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
	msa = make_ui32 (0x00, df1, df2, df3);

#if LISTER
				   /* List data field: */
	list_text (2, ";  Aircraft Address:");
	list_text (2, " " M_FMT_ADDR " (" M_FMT_UI32 ")", msa, msa);
	list_text (2, "\n");
#endif /* LISTER */

				   /* Store this information: */
	rtgt.target_address.present = TRUE;
	rtgt.target_address.value = msa;

				   /* Set the return code: */
	ret = RC_OKAY;

	return ret;
}

/*----------------------------------------------------------------------------*/
/* proc_i016_230   -- Process I016/230 data item                              */
/*----------------------------------------------------------------------------*/

 static
 t_Retc proc_i016_230 (t_Ui16 length, t_Byte *buffer)
{
#if LISTER
	t_Byte acas;   /* Capability of on-board equipment */
	t_Byte com;    /* Communications capability of the transponder */
	t_Byte dfo;    /* Data field octet */
#endif /* LISTER */
	t_Retc ret;    /* Return code */
#if LISTER
	t_Byte stat;   /* Flight status */
#endif /* LISTER */

				   /* Preset the return code: */
	ret = RC_FAIL;

				   /* Check parameters: */
	Assert (length == 1, "Invalid parameter");
	Assert (buffer != NULL, "Invalid parameter");

#if LISTER
				   /* Extract octets: */
	dfo = buffer[0];

				   /* Extract information: */
	com = (dfo >> 5) & 0x07;
	stat = (dfo >> 2) & 0x07;
	acas = dfo & 0x03;

				   /* List data field: */
	list_text (2, ";  Communications/ACAS Capability"
                  " and Flight Status:");
	list_text (2, " COM=" M_FMT_BYTE ";", com);
	list_text (2, " STAT=" M_FMT_BYTE ";", stat);
	list_text (2, " ACAS=" M_FMT_BYTE, acas);
	list_text (2, "\n");
#endif /* LISTER */

				   /* Store this information: */
	/* tba ... */

				   /* Set the return code: */
	ret = RC_OKAY;

	return ret;
}

/*----------------------------------------------------------------------------*/
/* proc_i016_240   -- Process I016/240 data item                              */
/*----------------------------------------------------------------------------*/

 static
 t_Retc proc_i016_240 (t_Ui16 length, t_Byte *buffer)
{
#if CCWARN
	t_Byte df1;    /* Data field octet 1 */
	t_Byte df2;    /* Data field octet 2 */
	t_Byte df3;    /* Data field octet 3 */
	t_Byte df4;    /* Data field octet 4 */
	t_Byte df5;    /* Data field octet 5 */
	t_Byte df6;    /* Data field octet 6 */
#endif
	t_Retc ret;    /* Return code */

				   /* Preset the return code: */
	ret = RC_FAIL;

				   /* Check parameters: */
	Assert (length == 6, "Invalid parameter");
	Assert (buffer != NULL, "Invalid parameter");

				   /* Extract octets: */
#if CCWARN
	df1 = buffer[0];
	df2 = buffer[1];
	df3 = buffer[2];
	df4 = buffer[3];
	df5 = buffer[4];
	df6 = buffer[5];
#endif

				   /* Extract information: */
	/* tba ... */

#if LISTER
				   /* List data field: */
	list_text (2, ";  Aircraft Identification:");
	/* tba ... */
	list_text (2, "\n");
#endif /* LISTER */

				   /* Store this information: */
	/* tba ... */

				   /* Set the return code: */
	ret = RC_OKAY;

	return ret;
}

/*----------------------------------------------------------------------------*/
/* proc_i016_250   -- Process I016/250 data item                              */
/*----------------------------------------------------------------------------*/

 static
 t_Retc proc_i016_250 (t_Ui16 length, t_Byte *buffer)
{
#if LISTER
	t_Byte bds1;   /* Comm B Data Buffer Store 1 Address */
	t_Byte bds2;   /* Comm B Data Buffer Store 2 Address */
	t_Byte df1;    /* Data field octet 1 */
	t_Byte df2;    /* Data field octet 2 */
	t_Byte df3;    /* Data field octet 3 */
	t_Byte df4;    /* Data field octet 4 */
	t_Byte df5;    /* Data field octet 5 */
	t_Byte df6;    /* Data field octet 6 */
	t_Byte df7;    /* Data field octet 7 */
	t_Byte df8;    /* Data field octet 8 */
	int ix;        /* Auxiliary */
#endif /* LISTER */
	t_Byte rep;    /* Repetition factor */
	t_Retc ret;    /* Return code */

				   /* Preset the return code: */
	ret = RC_FAIL;

				   /* Check parameters: */
	Assert (length > 0, "Invalid parameter");
	Assert (buffer != NULL, "Invalid parameter");

                   /* Extract repetition factor: */
	rep = buffer[0];

                   /* Check length (again): */
	Assert (length == 1 + 8 * rep, "Invalid length");

#if LISTER
				   /* List data field: */
	list_text (2, ";  Mode S MB Data:\n");
	for (ix = 0; ix < rep; ix ++)
	{
		df1 = buffer[1 + 8 * ix];
		df2 = buffer[1 + 8 * ix + 1];
		df3 = buffer[1 + 8 * ix + 2];
		df4 = buffer[1 + 8 * ix + 3];
		df5 = buffer[1 + 8 * ix + 4];
		df6 = buffer[1 + 8 * ix + 5];
		df7 = buffer[1 + 8 * ix + 6];
		df8 = buffer[1 + 8 * ix + 7];

		bds1 = (df8 >> 4) & 0x0f;
		bds2 = df8 & 0x0f;

		list_text (2, ";   BDS=" M_FMT_BYTE "," M_FMT_BYTE " -", bds1, bds2);
		list_text (2, " data=0x " M_FMT_BYTE_HEX " " M_FMT_BYTE_HEX
                      " " M_FMT_BYTE_HEX " " M_FMT_BYTE_HEX " " M_FMT_BYTE_HEX
                      " " M_FMT_BYTE_HEX " " M_FMT_BYTE_HEX,
                      df1, df2, df3, df4, df5, df6, df7);
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
/* proc_i016_260   -- Process I016/260 data item                              */
/*----------------------------------------------------------------------------*/

 static
 t_Retc proc_i016_260 (t_Ui16 length, t_Byte *buffer)
{
#if CCWARN
	t_Byte df1;    /* Data field octet 1 */
	t_Byte df2;    /* Data field octet 2 */
	t_Byte df3;    /* Data field octet 3 */
	t_Byte df4;    /* Data field octet 4 */
	t_Byte df5;    /* Data field octet 5 */
	t_Byte df6;    /* Data field octet 6 */
#endif
	t_Retc ret;    /* Return code */

				   /* Preset the return code: */
	ret = RC_FAIL;

				   /* Check parameters: */
	Assert (length == 6, "Invalid parameter");
	Assert (buffer != NULL, "Invalid parameter");

				   /* Extract octets: */
#if CCWARN
	df1 = buffer[0];
	df2 = buffer[1];
	df3 = buffer[2];
	df4 = buffer[3];
	df5 = buffer[4];
	df6 = buffer[5];
#endif

				   /* Extract information: */
	/* tba ... */

#if LISTER
				   /* List data field: */
	list_text (2, ";  ACAS Resolution Advisory Report:");
	/* tba ... */
	list_text (2, "\n");
#endif /* LISTER */

				   /* Store this information: */
	/* tba ... */

				   /* Set the return code: */
	ret = RC_OKAY;

	return ret;
}

/*----------------------------------------------------------------------------*/
/* proc_i016_rfs   -- Process RFS indicator data item                         */
/*----------------------------------------------------------------------------*/

 static
 t_Retc proc_i016_rfs (t_Ui16 length, t_Byte *buffer, t_Ui16 *pos_ptr)
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
	list_text (1, ";  I016/RFS: 0x " M_FMT_BYTE_HEX " ...\n", n);
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
		list_text (1, ";  I016/RFS: frn=" M_FMT_BYTE "\n", frn);
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

		lrc = data_item (16, frn, std_uap[frn], length, buffer, &pos);
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
/* proc_i016_spf   -- Process SPF indicator data item                         */
/*----------------------------------------------------------------------------*/

 static
 t_Retc proc_i016_spf (t_Ui16 length, t_Byte *buffer, t_Ui16 *pos_ptr)
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
	list_buffer (1, ";  I016/SPF:", len, buffer + pos);
#endif /* LISTER */

				   /* Set position: */
	*pos_ptr = pos + len;

				   /* Set the return code: */
	ret = RC_OKAY;

	done:          /* We are done */
	return ret;
}
/* end-of-file */
