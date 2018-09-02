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
/* File:           src/do_asx.cpp                                             */
/* Contents:       Process an ASTERIX data frame                              */
/* Author(s):      kb                                                         */
/* Last change:    2018-04-27                                                 */
/*----------------------------------------------------------------------------*/

#include "basics.h"
                   /* Basic system definitions */
#include "common.h"
                   /* Common declarations and definitions */

#include <stddef.h>
#include <stdio.h>
#include <stdlib.h>

                   /* Local functions: */
                   /* ---------------- */

static t_Retc fixed_length (t_Byte category, t_Si16 item_number,
                            t_Ui16 item_length, t_Ui16 buffer_length,
                            t_Byte *buffer, t_Ui16 *pos_ptr);
                   /* Read a fixed length data item */
static t_Retc repetitive_item (t_Byte category, t_Si16 item_number,
                               t_Ui16 sub_item_length,
                               t_Ui16 length, t_Byte *buffer, t_Ui16 *pos_ptr);
                   /* Read a repetitive data item */
static t_Retc variable_length (t_Byte category, t_Si16 item_number,
                               t_Ui16 length, t_Byte *buffer, t_Ui16 *pos_ptr);
                   /* Read a variable length data item */

/*----------------------------------------------------------------------------*/
/* asterix_frame   -- Process an ASTERIX data frame                           */
/*----------------------------------------------------------------------------*/

 t_Retc asterix_frame
	(t_Ui32 offset, t_Ui16 source, t_Ui16 length, t_Byte *buffer)
{
	t_Byte cat;    /* ASTERIX category */
	t_Ui16 len;    /* Data block length */
	t_Retc lrc;    /* Local return code */
	t_Ui16 pos;    /* Position within the data frame */
	t_Retc ret;    /* Return code */

				   /* Preset the return code: */
	ret = RC_FAIL;

                   /* Check parameters: */
	Assert (length > 0, "Invalid parameter");
	Assert (buffer != NULL, "Invalid parameter");

				   /* Check for minimal frame length: */
	if (length < 3)
	{
		error_msg ("ASTERIX frame too short");
		goto done;
	}

				   /* Handle all data blocks within the data frame: */
	pos = 0;
	while (pos < length)
	{
				   /* Check for data block header: */
		if (pos + 3 >= length)
		{
			error_msg ("Data block header too short");
			goto done;
		}

				   /* Get the ASTERIX category: */
		cat = buffer[pos];

				   /* Get the data block length: */
		len = make_ui16 (buffer[pos + 1], buffer[pos + 2]);

				   /* Check the data block length: */
		if (len < 3)
		{
			error_msg ("ASTERIX data block too short");

			ret = RC_SKIP;
			goto done;
		}
		if (pos + len > length)
		{
			error_msg ("ASTERIX data block too large"
                       " (pos=" M_FMT_UI16 "; len=" M_FMT_UI16
                       "; length=" M_FMT_UI16 ")",
                       pos, len, length);

			ret = RC_SKIP;
			goto done;
		}

#if LISTER
				   /* List this ASTERIX data block: */
		list_text (1, "; ASTERIX data block at pos %d:"
                      " cat=" M_FMT_BYTE "; len=" M_FMT_UI16 "\n",
                      pos, cat, len);
		list_frame (1, len, buffer + pos);
#endif /* LISTER */

				   /* Process according to ASTERIX category: */
		switch (cat)
		{
		case 0:

			lrc = astx_000 (len - 3, buffer + pos + 3);
			break;

		case 1:

			lrc = astx_001 (len - 3, buffer + pos + 3);
			break;

		case 2:

			lrc = astx_002 (len - 3, buffer + pos + 3);
			break;

		case 3:

			lrc = astx_003 (len - 3, buffer + pos + 3);
			break;

		case 4:

			lrc = astx_004 (len - 3, buffer + pos + 3);
			break;

		case 7:

			lrc = RC_SKIP;
			break;

		case 8:

			lrc = astx_008 (len - 3, buffer + pos + 3);
			break;

		case 9:

			lrc = RC_SKIP;
			break;

		case 10:

			lrc = astx_010 (len - 3, buffer + pos + 3);
			break;

		case 11:

			lrc = astx_011 (len - 3, buffer + pos + 3);
			break;

		case 16:

			lrc = astx_016 (len - 3, buffer + pos + 3);
			break;

		case 17:

			lrc = astx_017 (len - 3, buffer + pos + 3);
			break;

		case 18:

			lrc = RC_SKIP;
			break;

		case 19:

			lrc = astx_019 (len - 3, buffer + pos + 3);
			break;

		case 20:

			lrc = astx_020 (len - 3, buffer + pos + 3);
			break;

		case 21:

			lrc = astx_021 (len - 3, buffer + pos + 3);
			break;

		case 23:

			lrc = astx_023 (len - 3, buffer + pos + 3);
			break;

		case 30:

			lrc = astx_030 (len - 3, buffer + pos + 3);
			break;

		case 31:

			lrc = astx_031 (len - 3, buffer + pos + 3);
			break;

		case 32:

			lrc = astx_032 (len - 3, buffer + pos + 3);
			break;

		case 34:

			lrc = astx_034 (len - 3, buffer + pos + 3);
			break;

		case 48:

			lrc = astx_048 (len - 3, buffer + pos + 3);
			break;

		case 62:

			lrc = astx_062 (len - 3, buffer + pos + 3);
			break;

		case 63:

			lrc = astx_063 (len - 3, buffer + pos + 3);
			break;

		case 65:

			lrc = astx_065 (len - 3, buffer + pos + 3);
			break;

		case 221:

			lrc = astx_221 (len - 3, buffer + pos + 3);
			break;

		case 247:

			lrc = astx_247 (len - 3, buffer + pos + 3);
			break;

		case 252:

			lrc = astx_252 (len - 3, buffer + pos + 3);
			break;

		case 253:

			lrc = RC_SKIP;
			break;

		default:

			printf ("z> ASTERIX category %u - "
                    "sorry, not yet implemented\n", cat);
			break;
		}

				   /* Check return code: */
		if (lrc != RC_OKAY && lrc != RC_SKIP)
		{
			bad_frame ("Bad ASTERIX data block near", offset,
                       len, buffer + pos);

			if (stop_on_error)
			{
				ret = lrc;
				goto done;
			}

			break;
		}

				   /* Go on to next data block: */
		pos += len;
	}

                   /* Clear last SAC/SIC, if any: */
	last_sacsic = 0;
	last_sacsic_available = FALSE;

				   /* Set the return code: */
	ret = RC_OKAY;

	done:          /* We are done */
	return ret;
}

/*----------------------------------------------------------------------------*/
/* data_item       -- Process an ASTERIX data item                            */
/*----------------------------------------------------------------------------*/

 t_Retc data_item
	(t_Byte category, t_Ui16 field_reference_number,
     t_Data_Item_Desc *desc_ptr,
     t_Ui16 length, t_Byte *buffer, t_Ui16 *pos_ptr)
{
	t_Ui16 base_pos;
                   /* Base position within buffer */
	t_Ui16 item_len;
                   /* Data item length */
	t_Si16 item_num;
                   /* Data item number */
	t_Retc lrc;    /* Local return code */
	t_P_Fp pfp;    /* Pointer to data item processing function */
	t_Retc ret;    /* Return code */
	t_R_Fp rfp;    /* Pointer to data item reading function */

                   /* Preset the return code: */
	ret = RC_FAIL;

                   /* Check parameters: */
	Assert (length > 0, "Invalid parameter");
	Assert (buffer != NULL, "Invalid parameter");
	Assert (pos_ptr != NULL, "Invalid parameter");

                   /* Check data item description: */
	if (desc_ptr == NULL)
	{
		error_msg ("No data item defined for FRN=" M_FMT_UI16,
                   field_reference_number);
		goto done;
	}

                   /* Check category: */
	Assert (desc_ptr->category == category, "Wrong category");

                   /* Check data item type: */
	if (desc_ptr->item_type == e_undefined_data_item)
	{
		error_msg ("Undefined data item type");
		goto done;
	}
	
	else if (desc_ptr->item_type == e_fixed_length_data_item)
	{
                   /* Check fixed length: */
		Assert (desc_ptr->fixed_length > 0, "Invalid fixed length");

                   /* Remember start position (for later processing): */
		base_pos = *pos_ptr;

                   /* Check start position: */
		if (base_pos >= length)
		{
			error_msg ("Bad ASTERIX data block");
			goto done;
		}

                   /* Remember data item length: */
		item_len = desc_ptr->fixed_length;

                   /* Remember data item number: */
		item_num = desc_ptr->data_item;

                   /* Read this fixed length data item: */
		lrc = fixed_length (category, item_num, item_len,
                            length, buffer, pos_ptr);
		if (lrc != RC_OKAY)
		{
			error_msg ("Invalid data item");
			goto done;
		}

                   /* Get pointer to processing function: */
		pfp = desc_ptr->proc_fptr;

                   /* Check this pointer: */
		Assert (pfp != NULL, "Data item without processing function");

                   /* Process this data item: */
		lrc = (*pfp) (item_len, buffer + base_pos);
		if (lrc != RC_OKAY && lrc != RC_SKIP)
		{
			error_msg ("Invalid data item");
			goto done;
		}
	}

	else if (desc_ptr->item_type == e_immediate_data_item)
	{
                   /* Remember start position (for later processing): */
		base_pos = *pos_ptr;

                   /* Check start position: */
		if (base_pos >= length)
		{
			error_msg ("Bad ASTERIX data block");
			goto done;
		}

                   /* Remember data item number: */
		item_num = desc_ptr->data_item;

                   /* Get pointer to reading function: */
		rfp = desc_ptr->read_fptr;

                   /* Check this pointer: */
		if (rfp == NULL)
		{
			error_msg ("Data item without reading function");
			goto done;
		}

		lrc = (*rfp) (length, buffer, pos_ptr);
		if (lrc != RC_OKAY && lrc != RC_SKIP)
		{
			error_msg ("Invalid data item");
			goto done;
		}
	}

	else if (desc_ptr->item_type == e_repetitive_data_item)
	{
                   /* Check length of repetitive sub-item: */
		Assert (desc_ptr->fixed_length > 0, "Invalid repetitive length");

                   /* Remember start position (for later processing): */
		base_pos = *pos_ptr;

                   /* Check start position: */
		if (base_pos >= length)
		{
			error_msg ("Bad ASTERIX data block");
			goto done;
		}

                   /* Remember data item number: */
		item_num = desc_ptr->data_item;

                   /* Read this repetitive data item: */
		lrc = repetitive_item (category, item_num, desc_ptr->fixed_length,
                               length, buffer, pos_ptr);
		if (lrc != RC_OKAY && lrc != RC_SKIP)
		{
			error_msg ("Invalid data item");
			goto done;
		}

                   /* Get data item length: */
		item_len = *pos_ptr - base_pos;

                   /* Check data item length: */
		if (item_len == 0)
		{
			error_msg ("Invalid data item length");

			ret = RC_SKIP;
			goto done;
		}

                   /* Get pointer to processing function: */
		pfp = desc_ptr->proc_fptr;

                   /* Check this pointer: */
		Assert (pfp != NULL, "Data item without processing function");

                   /* Process this data item: */
		lrc = (*pfp) (item_len, buffer + base_pos);
		if (lrc != RC_OKAY)
		{
			error_msg ("Invalid data item");
			goto done;
		}
	}

	else if (desc_ptr->item_type == e_variable_length_data_item)
	{
                   /* Remember start position (for later processing): */
		base_pos = *pos_ptr;

                   /* Check start position: */
		if (base_pos >= length)
		{
			error_msg ("Bad ASTERIX data block");
			goto done;
		}

                   /* Remember data item number: */
		item_num = desc_ptr->data_item;

                   /* Read this variable length data item: */
		lrc = variable_length (category, item_num,
                               length, buffer, pos_ptr);
		if (lrc != RC_OKAY)
		{
			error_msg ("Invalid data item");
			goto done;
		}

                   /* Get data item length: */
		item_len = *pos_ptr - base_pos;

                   /* Check data item length: */
		if (item_len == 0)
		{
			error_msg ("Invalid data item length");

			ret = RC_SKIP;
			goto done;
		}

                   /* Get pointer to processing function: */
		pfp = desc_ptr->proc_fptr;

                   /* Check this pointer: */
		Assert (pfp != NULL, "Data item without processing function");

                   /* Process this data item: */
		lrc = (*pfp) (item_len, buffer + base_pos);
		if (lrc != RC_OKAY)
		{
			error_msg ("Invalid data item");
			goto done;
		}
	}

	else
	{
		Assert (FALSE, "Unexpected data item type");
	}

                   /* Set the return code: */
	ret = RC_OKAY;

	done:          /* We are done */
	return ret;
}

/*----------------------------------------------------------------------------*/
/* fixed_length    -- Read a fixed length data item                           */
/*----------------------------------------------------------------------------*/

 static
 t_Retc fixed_length
	(t_Byte category, t_Si16 item_number, t_Ui16 item_length,
     t_Ui16 buffer_length, t_Byte *buffer, t_Ui16 *pos_ptr)
{
#if LISTER
	int ix;        /* Auxiliary */
	int out;       /* Auxiliary */
#endif /* LISTER */
	t_Ui16 pos;    /* Position within buffer */
	t_Retc ret;    /* Return code */

				   /* Preset the return code: */
	ret = RC_FAIL;

				   /* Check parameters: */
	Assert (item_length > 0, "Invalid parameter");
	Assert (buffer_length > 0, "Invalid parameter");
	Assert (buffer != NULL, "Invalid parameter");
	Assert (pos_ptr != NULL, "Invalid parameter");

				   /* Set position within buffer: */
	pos = *pos_ptr;

				   /* Check position: */
	Assert (pos < buffer_length, "Invalid position");

				   /* Check against buffer length: */
	if (pos + item_length > buffer_length)
	{
		error_msg ("Invalid buffer length");
		goto done;
	}

#if LISTER
				   /* List raw data field: */
	list_text (1, ";  I");
	list_text (1, "%03u/", category);
	list_text (1, "%03hd: =0x", item_number);

	out = 0;
	for (ix = 0; ix < item_length; ix ++)
	{
		if (ix > 0 && out == 0)
		{
			list_text (1, ";            +0x");
		}

		list_text (1, " " M_FMT_BYTE_HEX, buffer[pos + ix]);
		++ out;

		if (out >= 16)
		{
			list_text (1, "\n");

			out = 0;
		}
		else if ((out % 4) == 0)
		{
			list_text (1, " ");
		}
	}

	if (out > 0)
	{
		list_text (1, "\n");
	}
#endif /* LISTER */

				   /* Set position: */
	*pos_ptr = pos + item_length;

				   /* Set the return code: */
	ret = RC_OKAY;

	done:          /* We are done */
	return ret;
}

/*----------------------------------------------------------------------------*/
/* repetitive_item -- Read a repetitive data item                             */
/*----------------------------------------------------------------------------*/

 static
 t_Retc repetitive_item
	(t_Byte category, t_Si16 item_number, t_Ui16 sub_item_length,
      t_Ui16 length, t_Byte *buffer, t_Ui16 *pos_ptr)
{
#if LISTER
	int ix;        /* Auxiliary */
	int out;       /* Auxiliary */
#endif /* LISTER */
	t_Ui16 pos;    /* Position within buffer */
	t_Byte rep;    /* Repetition factor */
	t_Retc ret;    /* Return code */

				   /* Preset the return code: */
	ret = RC_FAIL;

				   /* Check parameters: */
	Assert (sub_item_length > 0, "Invalid parameter");
	Assert (length > 0, "Invalid parameter");
	Assert (buffer != NULL, "Invalid parameter");
	Assert (pos_ptr != NULL, "Invalid parameter");

				   /* Set position within buffer: */
	pos = *pos_ptr;

				   /* Check position: */
	Assert (pos < length, "Invalid position");

                   /* Get repetition factor: */
	rep = buffer[pos ++];

                   /* Check repetition factor: */
	if (rep == 0)
	{
		error_msg ("Invalid repetition factor (0)");

		*pos_ptr = pos;

		ret = RC_SKIP;
		goto done;
	}

                   /* Check against buffer length: */
	if (pos + rep * sub_item_length > length)
	{
		error_msg ("Invalid buffer length");
		goto done;
	}

#if LISTER
				   /* List raw data field: */
	list_text (1, ";  I");
	list_text (1, "%03u/", category);
	list_text (1, "%03hd: =0x", item_number);

	out = 0;
	list_text (1, " " M_FMT_BYTE_HEX, rep);
	++ out;
	for (ix = 0; ix < rep * sub_item_length; ix ++)
	{
		if (ix > 0 && out == 0)
		{
			list_text (1, ";            +0x");
		}

		list_text (1, " " M_FMT_BYTE_HEX, buffer[pos + ix]);
		++ out;

		if (out >= 16)
		{
			list_text (1, "\n");

			out = 0;
		}
		else if ((out % 4) == 0)
		{
			list_text (1, " ");
		}
	}

	if (out > 0)
	{
		list_text (1, "\n");
	}
#endif /* LISTER */

				   /* Set position: */
	*pos_ptr = pos + rep * sub_item_length;

				   /* Set the return code: */
	ret = RC_OKAY;

	done:          /* We are done */
	return ret;
}

/*----------------------------------------------------------------------------*/
/* variable_length -- Read a variable length data item                        */
/*----------------------------------------------------------------------------*/

 static
 t_Retc variable_length
	(t_Byte category, t_Si16 item_number,
     t_Ui16 length, t_Byte *buffer, t_Ui16 *pos_ptr)
{
	int cnt;       /* Count for data field octets */
	t_Byte dfo;    /* Data field octet */
#if LISTER
	int ix;        /* Auxiliary */
	int out;       /* Auxiliary */
#endif /* LISTER */
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

				   /* Count and extract octets: */
	cnt = 0;
	dfo = buffer[pos];
	while (dfo & 0x01)
	{
		++ cnt;

				   /* Check against buffer length: */
		if (pos + cnt >= length)
		{
			error_msg ("Invalid buffer length");
			goto done;
		}

		dfo = buffer[pos + cnt];
	}
	++ cnt;

#if LISTER
				   /* List raw data field: */
	list_text (1, ";  I");
	list_text (1, "%03u/", category);
	list_text (1, "%03hd: =0x", item_number);

	out = 0;
	for (ix = 0; ix < cnt; ix ++)
	{
		if (ix > 0 && out == 0)
		{
			list_text (1, ";            +0x");
		}

		list_text (1, " " M_FMT_BYTE_HEX, buffer[pos + ix]);
		++ out;

		if (out >= 16)
		{
			list_text (1, "\n");

			out = 0;
		}
		else if ((out % 4) == 0)
		{
			list_text (1, " ");
		}
	}

	if (out > 0)
	{
		list_text (1, "\n");
	}
#endif /* LISTER */

				   /* Set position: */
	*pos_ptr = pos + cnt;

				   /* Set the return code: */
	ret = RC_OKAY;

	done:          /* We are done */
	return ret;
}
/* end-of-file */
