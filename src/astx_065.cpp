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
/* File:           src/astx_065.cpp                                           */
/* Contents:       Process ASTERIX category 065 data block                    */
/* Author(s):      kb                                                         */
/* Last change:    2018-04-27                                                 */
/*----------------------------------------------------------------------------*/

/* Reference document:
   -------------------

   Eurocontrol Standard Document for Surveillance Data Exchange
   Part 15: Category 65 - SDPS Service Status Messages
   SUR.ET1.ST05.2000-STD-15-01
   Edition: 0.12
   Edition Date: March 2003

   Edition: 1.3
   Edition Date: April 2007

   Appendix A: Coding rules for "Reserved Expansion Field"
   Edition: 1.0
   Edition Date: April 2010
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
					  for ASTERIX category 065 */

                   /* Local texts: */
                   /* ------------ */

#if LISTER
static const char* text_message_type[3] =
{
	"SDPS status",
	"end of batch",
	"service status report"
};
static const char* text_report[16] =
{
	"service degradation",
	"service degradation ended",
	"main radar out of service",
	"service interrupted by the operator",
	"service interrupted due to contingency",
	"ready for service restart after contingency",
	"service ended by the operator",
	"failure of user main radar",
	"service restarted by the operator",
	"main radar becoming operational",
	"main radar becoming degraded",
	"service continuity interrupted due to disconnection with adjacent unit",
	"service continuity restarted",
	"service synchronised on backup radar",
	"service synchronised on main radar",
	"main and backup radar, if any, failed"
};
#endif /* LISTER */

				   /* Local variables: */
				   /* ---------------- */

static t_Data_Item_Desc desc_i065_000;
                   /* Description of data item I065/000 */
static t_Data_Item_Desc desc_i065_010;
                   /* Description of data item I065/010 */
static t_Data_Item_Desc desc_i065_015;
                   /* Description of data item I065/015 */
static t_Data_Item_Desc desc_i065_020;
                   /* Description of data item I065/020 */
static t_Data_Item_Desc desc_i065_030;
                   /* Description of data item I065/030 */
static t_Data_Item_Desc desc_i065_040;
                   /* Description of data item I065/040 */
static t_Data_Item_Desc desc_i065_050;
                   /* Description of data item I065/050 */
static t_Data_Item_Desc desc_i065_ref;
                   /* Description of data item I065/REF */
static t_Data_Item_Desc desc_i065_spf;
                   /* Description of data item I065/SPF */
static t_Bool initiated = FALSE;
				   /* Package initiated flag */
static t_Data_Item_Desc *std_uap[M_MAX_FRN + 1];
				   /* Standard User Application Profile */
static t_Ui16 reference_vsn = 1;
                   /* Current version of reference document */
                   /* Implemented editions:
                      1 ... 0.12 (March 2003)
                      2 ... 1.3  (April 2007)
                   */

				   /* Local functions: */
				   /* ---------------- */

static void init_desc (void);
                   /* Initiate data item descriptions */
static void load_std_uap (void);
                   /* Load standard UAP */
static t_Retc proc_i065_000 (t_Ui16 length, t_Byte *buffer);
                   /* Process I065/000 data item */
static t_Retc proc_i065_010 (t_Ui16 length, t_Byte *buffer);
                   /* Process I065/010 data item */
static t_Retc proc_i065_015 (t_Ui16 length, t_Byte *buffer);
                   /* Process I065/015 data item */
static t_Retc proc_i065_020 (t_Ui16 length, t_Byte *buffer);
                   /* Process I065/020 data item */
static t_Retc proc_i065_030 (t_Ui16 length, t_Byte *buffer);
                   /* Process I065/030 data item */
static t_Retc proc_i065_040 (t_Ui16 length, t_Byte *buffer);
                   /* Process I065/040 data item */
static t_Retc proc_i065_050 (t_Ui16 length, t_Byte *buffer);
                   /* Process I065/050 data item */
static t_Retc proc_i065_ref (t_Ui16 length, t_Byte *buffer, t_Ui16 *pos_ptr);
                   /* Process RE indicator data item */
static t_Retc proc_i065_spf (t_Ui16 length, t_Byte *buffer, t_Ui16 *pos_ptr);
                   /* Process SP indicator data item */

/*----------------------------------------------------------------------------*/
/* astx_065        -- Process ASTERIX category 065 data block                 */
/*----------------------------------------------------------------------------*/

 t_Retc astx_065 (t_Ui16 length, t_Byte *buffer)
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

                   /* Preset system picture step: */
		memset (&step, 0, sizeof (step));

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

					lrc = data_item (65, frn, std_uap[frn],
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
			error_msg ("Empty ASTERIX record");
			goto done;
		}

                   /* Add frame date, if available: */
		if (frame_date_present)
		{
			step.frame_date.present = TRUE;
			step.frame_date.value = frame_date;
		}
 
                   /* Add frame time, if available: */
        if (frame_time_present)
        {
            step.frame_time.present = TRUE;
            step.frame_time.value = frame_time;
        }

#if LISTER
                   /* List system picture step: */
		if (list_astx_065)
		{
			list_step (&step);
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
                   /* Description of data item I065/000: */
	desc_i065_000.category = 65;
	desc_i065_000.data_item = 0;
	desc_i065_000.item_type = e_fixed_length_data_item;
	desc_i065_000.fixed_length = 1;
	desc_i065_000.proc_fptr = proc_i065_000;
	desc_i065_000.read_fptr = NULL;

                   /* Description of data item I065/010: */
	desc_i065_010.category = 65;
	desc_i065_010.data_item = 10;
	desc_i065_010.item_type = e_fixed_length_data_item;
	desc_i065_010.fixed_length = 2;
	desc_i065_010.proc_fptr = proc_i065_010;
	desc_i065_010.read_fptr = NULL;

                   /* Description of data item I065/015: */
	desc_i065_015.category = 65;
	desc_i065_015.data_item = 15;
	desc_i065_015.item_type = e_fixed_length_data_item;
	desc_i065_015.fixed_length = 1;
	desc_i065_015.proc_fptr = proc_i065_015;
	desc_i065_015.read_fptr = NULL;

                   /* Description of data item I065/020: */
	desc_i065_020.category = 65;
	desc_i065_020.data_item = 20;
	desc_i065_020.item_type = e_fixed_length_data_item;
	desc_i065_020.fixed_length = 1;
	desc_i065_020.proc_fptr = proc_i065_020;
	desc_i065_020.read_fptr = NULL;

                   /* Description of data item I065/030: */
	desc_i065_030.category = 65;
	desc_i065_030.data_item = 30;
	desc_i065_030.item_type = e_fixed_length_data_item;
	desc_i065_030.fixed_length = 3;
	desc_i065_030.proc_fptr = proc_i065_030;
	desc_i065_030.read_fptr = NULL;

                   /* Description of data item I065/040: */
	desc_i065_040.category = 65;
	desc_i065_040.data_item = 40;
	desc_i065_040.item_type = e_fixed_length_data_item;
	desc_i065_040.fixed_length = 1;
	desc_i065_040.proc_fptr = proc_i065_040;
	desc_i065_040.read_fptr = NULL;

                   /* Description of data item I065/050: */
	desc_i065_050.category = 65;
	desc_i065_050.data_item = 50;
	desc_i065_050.item_type = e_fixed_length_data_item;
	desc_i065_050.fixed_length = 1;
	desc_i065_050.proc_fptr = proc_i065_050;
	desc_i065_050.read_fptr = NULL;

                   /* Description of REF data item: */
	desc_i065_ref.category = 65;
	desc_i065_ref.data_item = M_REF_INDICATOR;
	desc_i065_ref.item_type = e_immediate_data_item;
	desc_i065_ref.fixed_length = 0;
	desc_i065_ref.proc_fptr = NULL;
	desc_i065_ref.read_fptr = proc_i065_ref;

                   /* Description of SPF data item: */
	desc_i065_spf.category = 65;
	desc_i065_spf.data_item = M_SPF_INDICATOR;
	desc_i065_spf.item_type = e_immediate_data_item;
	desc_i065_spf.fixed_length = 0;
	desc_i065_spf.proc_fptr = NULL;
	desc_i065_spf.read_fptr = proc_i065_spf;

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

	std_uap[1] = &desc_i065_010;
	std_uap[2] = &desc_i065_000;
	std_uap[3] = &desc_i065_015;
	std_uap[4] = &desc_i065_030;
	std_uap[5] = &desc_i065_020;
	std_uap[6] = &desc_i065_040;
	std_uap[7] = &desc_i065_050;

	std_uap[13] = &desc_i065_ref;
	std_uap[14] = &desc_i065_spf;

	return;
}

/*----------------------------------------------------------------------------*/
/* proc_i065_000   -- Process I065/000 data item                              */
/*----------------------------------------------------------------------------*/

 static
 t_Retc proc_i065_000 (t_Ui16 length, t_Byte *buffer)
{
	t_Byte df1;    /* Data field octet 1 */
	t_Byte mtp;    /* Message type */
	t_Retc ret;    /* Return code */

				   /* Preset the return code: */
	ret = RC_FAIL;

				   /* Check parameters: */
	Assert (length == 1, "Invalid parameter");
	Assert (buffer != NULL, "Invalid parameter");

				   /* Extract octets: */
	df1 = buffer[0];

                   /* Extract information: */
	mtp = df1;

#if LISTER
				   /* List data field: */
	list_text (2, ";  Message Type:");
	list_text (2, " " M_FMT_BYTE, mtp);
	if (1 <= mtp && mtp <= 3)
	{
		list_text (2, " (%s)", text_message_type[mtp - 1]);
	}
	list_text (2, "\n");
#endif /* LISTER */

                   /* Store this information: */
	step.message_type.present = TRUE;
	step.message_type.value = mtp;

                   /* Set the return code: */
	ret = RC_OKAY;

	return ret;
}

/*----------------------------------------------------------------------------*/
/* proc_i065_010   -- Process I065/010 data item                              */
/*----------------------------------------------------------------------------*/

 static
 t_Retc proc_i065_010 (t_Ui16 length, t_Byte *buffer)
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
	step.data_source_identifier.present = TRUE;
	step.data_source_identifier.value = dsi;

				   /* Set the return code: */
	ret = RC_OKAY;

	return ret;
}

/*----------------------------------------------------------------------------*/
/* proc_i065_015   -- Process I065/015 data item                              */
/*----------------------------------------------------------------------------*/

 static
 t_Retc proc_i065_015 (t_Ui16 length, t_Byte *buffer)
{
	t_Byte df1;    /* Data field octet 1 */
	t_Retc ret;    /* Return code */

                   /* Preset the return code: */
	ret = RC_FAIL;

                   /* Check parameters: */
	Assert (length == 1, "Invalid parameter");
	Assert (buffer != NULL, "Invalid parameter");

                   /* Extract octets: */
	df1 = buffer[0];

#if LISTER
                   /* List data field: */
	list_text (2, ";  Service Identification:");
	list_text (2, " " M_FMT_BYTE, df1);
	list_text (2, "\n");
#endif /* LISTER */

                   /* Store this information: */
	step.service_identification.present = TRUE;
	step.service_identification.value = df1;

                   /* Set the return code: */
	ret = RC_OKAY;

	return ret;
}

/*----------------------------------------------------------------------------*/
/* proc_i065_020   -- Process I065/020 data item                              */
/*----------------------------------------------------------------------------*/

 static
 t_Retc proc_i065_020 (t_Ui16 length, t_Byte *buffer)
{
	t_Byte btn;    /* Batch number */
	t_Byte df1;    /* Data field octet 1 */
	t_Retc ret;    /* Return code */

				   /* Preset the return code: */
	ret = RC_FAIL;

				   /* Check parameters: */
	Assert (length == 1, "Invalid parameter");
	Assert (buffer != NULL, "Invalid parameter");

				   /* Extract octets: */
	df1 = buffer[0];

                   /* Extract information: */
	btn = df1;

#if LISTER
				   /* List data field: */
	list_text (2, ";  Batch Number:");
	list_text (2, " " M_FMT_BYTE, btn);
	list_text (2, "\n");
#endif /* LISTER */

				   /* Store this information: */
	step.batch_number.present = TRUE;
	step.batch_number.value = btn;

				   /* Set the return code: */
	ret = RC_OKAY;

	return ret;
}

/*----------------------------------------------------------------------------*/
/* proc_i065_030   -- Process I065/030 data item                              */
/*----------------------------------------------------------------------------*/

 static
 t_Retc proc_i065_030 (t_Ui16 length, t_Byte *buffer)
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
	step.time_of_message.present = TRUE;
	step.time_of_message.value = tod_in_secs;

				   /* Set the return code: */
	ret = RC_OKAY;

	return ret;
}

/*----------------------------------------------------------------------------*/
/* proc_i065_040   -- Process I065/040 data item                              */
/*----------------------------------------------------------------------------*/

 static
 t_Retc proc_i065_040 (t_Ui16 length, t_Byte *buffer)
{
	t_Byte df1;    /* Data field octet 1 */
	t_Byte nogo;   /* SDPS operational status */
	t_Byte ovl;    /* Overload indication */
	t_Byte pss;    /* SDPS configuration */
	t_Retc ret;    /* Return code */
	t_Byte tsv;    /* Time source valid indication */

				   /* Preset the return code: */
	ret = RC_FAIL;

				   /* Check parameters: */
	Assert (length == 1, "Invalid parameter");
	Assert (buffer != NULL, "Invalid parameter");

				   /* Extract octets: */
	df1 = buffer[0];

                   /* Extract information: */
	nogo = (df1 >> 6) & 0x03;
	ovl = (df1 >> 5) & 0x01;
	tsv = (df1 >> 4) & 0x01;
	pss = (df1 >> 2) & 0x03;

#if LISTER
				   /* List data field: */
	list_text (2, ";  SDPS Configuration and Status:");
	list_text (2, " nogo=" M_FMT_BYTE, nogo);
	switch (nogo)
	{
	case 0:
		list_text (2, " (operational)");
		break;
	case 1:
		list_text (2, " (degraded)");
		break;
	case 2:
		list_text (2, " (not currently connected)");
		break;
	case 3:
		list_text (2, " (unknown)");
		break;
	}
	list_text (2, ";");
	list_text (2, " ovl=" M_FMT_BYTE ";", ovl);
	list_text (2, " tsv=" M_FMT_BYTE ";", tsv);
	list_text (2, " pss=" M_FMT_BYTE, pss);
	list_text (2, "\n");
#endif /* LISTER */

				   /* Store this information: */
	step.sdps_configuration_and_status.present = TRUE;
	step.sdps_configuration_and_status.value_nogo = nogo;
	step.sdps_configuration_and_status.value_ovl = ovl;
	step.sdps_configuration_and_status.value_pss = pss;
	step.sdps_configuration_and_status.value_tsv = tsv;

				   /* Set the return code: */
	ret = RC_OKAY;

	return ret;
}

/*----------------------------------------------------------------------------*/
/* proc_i065_050   -- Process I065/050 data item                              */
/*----------------------------------------------------------------------------*/

 static
 t_Retc proc_i065_050 (t_Ui16 length, t_Byte *buffer)
{
	t_Byte df1;    /* Data field octet 1 */
	t_Retc ret;    /* Return code */
	t_Byte report; /* Report indication */

				   /* Preset the return code: */
	ret = RC_FAIL;

				   /* Check parameters: */
	Assert (length == 1, "Invalid parameter");
	Assert (buffer != NULL, "Invalid parameter");

				   /* Extract octets: */
	df1 = buffer[0];

                   /* Extract information: */
	report = df1;

#if LISTER
				   /* List data field: */
	list_text (2, ";  Service Status Report:");
	list_text (2, " " M_FMT_BYTE, report);
	if (1 <= report && report <= 16)
	{
		list_text (2, " (%s)", text_report[report - 1]);
	}
	list_text (2, "\n");
#endif /* LISTER */

				   /* Store this information: */
	step.service_status_report.present = TRUE;
	step.service_status_report.value = report;

				   /* Set the return code: */
	ret = RC_OKAY;

	return ret;
}

/*----------------------------------------------------------------------------*/
/* proc_i065_ref   -- Process RE indicator data item                          */
/*----------------------------------------------------------------------------*/

 static
 t_Retc proc_i065_ref (t_Ui16 length, t_Byte *buffer, t_Ui16 *pos_ptr)
{
#if LISTER
	int ix;        /* Auxiliary */
	int j;         /* Auxiliary */
#endif /* LISTER */
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
		error_msg ("Invalid buffer length (I065/REF)");
		goto done;
	}

#if LISTER
				   /* List raw data field: */
	list_text (1, ";  I065/REF: 0x");
	j = 0;
	for (ix = 0; ix < len; ix ++)
	{
		list_text (1, " " M_FMT_BYTE_HEX, buffer[pos + ix]);
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
#endif /* LISTER */

                   /* Try to decode if edition 1.3: */
	if (reference_vsn == 2)
	{
		t_Byte df1;
                   /* Data field octet 1 */
		t_Byte df2;
                   /* Data field octet 2 */
		t_Byte df3;
                   /* Data field octet 3 */
		t_Byte df4;
                   /* Data field octet 4 */
		t_Byte df5;
                   /* Data field octet 5 */
		t_Byte df6;
                   /* Data field octet 6 */
		t_Byte df7;
                   /* Data field octet 7 */
		t_Byte df8;
                   /* Data field octet 8 */
		t_Byte df9;
                   /* Data field octet 9 */
		t_Si32 lat;
                   /* WGS-84 latitude of system reference point;
                      180/2**30 degrees */
		t_Real latf;
                   /* WGS-84 latitude of system reference point; degrees */
		t_Si32 lon;
                   /* WGS-84 longitude of system reference point;
                      180/2**30 degrees */
		t_Real lonf;
                   /* WGS-84 longitude of system reference point; degrees */
		t_Byte srp;
                   /* System reference point present */

		Assert (len > 1, "Invalid length of data item I065/REF");

                   /* Get first data field octet: */
		df1 = buffer[pos + 1];

                   /* Extract indication for system reference point: */
		srp = (df1 >> 7) & 0x01;

#if LISTER
		list_text (2, ";  I065/REF: srp=" M_FMT_BYTE, srp);
		list_text (2, "\n");
#endif /* LISTER */

		if (srp)
		{
			Assert (len >= 10, "Invalid length of data item I065/REF");

			df2 = buffer[pos + 2];
			df3 = buffer[pos + 3];
			df4 = buffer[pos + 4];
			df5 = buffer[pos + 5];
			df6 = buffer[pos + 6];
			df7 = buffer[pos + 7];
			df8 = buffer[pos + 8];
			df9 = buffer[pos + 9];

                   /* Extract WGS-84 position: */
			lat = make_si32 (df2, df3, df4, df5);
			lon = make_si32 (df6, df7, df8, df9);

                   /* Convert: */
			latf = (180.0 / M_TWO_POWER_30) * lat;
			lonf = (180.0 / M_TWO_POWER_30) * lon;

#if LISTER
				   /* List data field: */
			list_text (2, ";  System Reference Position (WGS-84 Coordinates):");
			list_text (2, "\n");
			list_text (2, ";   lat=" M_FMT_SI32 " (%s);", lat, lat_text (latf));
			list_text (2, " lon=" M_FMT_SI32 " (%s)", lon, lon_text (lonf));
			list_text (2, "\n");
#endif /* LISTER */

                   /* Store this information: */
			step.srp_position.present = TRUE;
			step.srp_position.value_lat = M_DEG2RAD * latf;
			step.srp_position.value_lon = M_DEG2RAD * lonf;
                   /* Values in radians */
		}
	}

				   /* Set position: */
	*pos_ptr = pos + len;

                   /* Set the return code: */
	ret = RC_OKAY;

	done:          /* We are done */
	return ret;
}

/*----------------------------------------------------------------------------*/
/* proc_i065_spf   -- Process SP indicator data item                          */
/*----------------------------------------------------------------------------*/

 static
 t_Retc proc_i065_spf (t_Ui16 length, t_Byte *buffer, t_Ui16 *pos_ptr)
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
	list_buffer (1, ";  I065/SPF:", len, buffer + pos);
#endif /* LISTER */

				   /* Set position: */
	*pos_ptr = pos + len;

				   /* Set the return code: */
	ret = RC_OKAY;

	done:          /* We are done */
	return ret;
}

/*----------------------------------------------------------------------------*/
/* set_vsn065      -- Set ASTERIX category 065 reference version              */
/*----------------------------------------------------------------------------*/

 t_Retc set_vsn065 (char *vsn_text)
{
	t_Retc ret;    /* Return code */

                   /* Preset the return code: */
	ret = RC_FAIL;

                   /* Check parameters: */
	Assert (vsn_text != NULL, "Invalid parameter");

                   /* Check against implemented reference versions: */
	if (strcmp (vsn_text, "0.12") == 0)
	{
		reference_vsn = 1;
		ret = RC_OKAY;
	}
	else if (strcmp (vsn_text, "1.3") == 0)
	{
		reference_vsn = 2;
		ret = RC_OKAY;
	}

	return ret;
}
/* end-of-file */
