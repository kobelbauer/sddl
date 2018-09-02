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
/* File:           src/do_netto.cpp                                           */
/* Contents:       Process "netto" input data                                 */
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

#define M_NETTO_BUFFER_MAX 4096
                   /* Max length of "netto" ASTERIX buffer */

/*----------------------------------------------------------------------------*/
/* netto_frame     -- Read and process "netto" frame                          */
/*----------------------------------------------------------------------------*/

 t_Retc netto_frame (void)
{
	t_Byte b0;     /* Auxiliary */
	t_Byte b1;     /* Auxiliary */
	t_Byte buffer[M_NETTO_BUFFER_MAX];
				   /* Buffer for "netto" frame */
	t_Byte cat;    /* Category */
	t_Ui16 length; /* Frame length */
	t_Retc lrc;    /* Local return code */
	int n;         /* Auxiliary */
	t_Retc ret;    /* Return code */

				   /* Preset the return code: */
	ret = RC_FAIL;

				   /* Preset the buffer: */
	memset (buffer, 0, M_NETTO_BUFFER_MAX);

				   /* Check if input file available: */
	Assert (input_file != NULL, "No input file");

                   /* "Netto" input format means that we have no
                      real frame structure - input and data format
                      are rather the same thing - decoding is
                      dependent upon the data format */

	if (data_format == e_data_format_asterix)
	{
                   /* Get next byte (category): */
		n = fgetc (input_file);
		if (n == EOF)
		{
			ret = RC_DONE;
			goto done;
		}
		cat = (t_Byte) n;

                   /* Get next two bytes (length): */
		n = fgetc (input_file);
		if (n == EOF)
		{
			error_msg ("Incomplete ASTERIX data block");
			goto done;
		}
		b0 = (t_Byte) n;
		n = fgetc (input_file);
		if (n == EOF)
		{
			error_msg ("Incomplete ASTERIX data block");
			goto done;
		}
		b1 = (t_Byte) n;

		length = make_ui16 (b0, b1);

                   /* Check length: */
		if (length < 3)
		{
			error_msg ("Invalid ASTERIX data block length");
			goto done;
		}
		else if (length > M_NETTO_BUFFER_MAX)
		{
			error_msg ("ASTERIX data block too large (" M_FMT_UI16 " bytes)",
                       length);
			goto done;
		}

                   /* Clear buffer: */
		memset (buffer, 0, M_NETTO_BUFFER_MAX);

                   /* Enter header information: */
		buffer[0] = cat;
		buffer[1] = b0;
		buffer[2] = b1;

                   /* Read the "rest" of the frame: */
		n = fread (buffer + 3, 1, length - 3, input_file);
		if (n != length - 3)
		{
			error_msg ("Read error at ASTERIX data block (data)");
			goto done;
		}
	}

	else
	{
		error_msg ("Unexpected data format");
		goto done;
	}

#if LISTER
				   /* List the "netto" frame (lowest level): */
	list_text (1, "; Netto frame " M_FMT_UI32 " (length=" M_FMT_UI16 ") "
                  "at offset 0x" M_FMT_UI32_HEX " (" M_FMT_UI32 "):\n",
                  frames_count, length,
                  input_offset, input_offset);
	list_frame (1, length, buffer);

	list_text (2, "; Netto frame " M_FMT_UI32 ":\n", frames_count);
#endif /* LISTER */

                   /* We don't have a frame time: */
	frame_time_present = FALSE;

				   /* Process this data frame: */
	lrc = do_frame (input_offset, 0x0000, length, buffer);
	if (lrc != RC_OKAY && lrc != RC_SKIP)
	{
		ret = lrc;
		goto done;
	}

				   /* Increment frames count: */
	++ frames_count;

                   /* Increment number of frames read and listed: */
	++ input_frames;

                   /* Increment number of bytes read and listed: */
	input_length += length;

				   /* Increment offset into input file: */
	input_offset += length;

				   /* Set the return code: */
	ret = RC_OKAY;

	done:          /* We are done */
	return ret;
}
/* end-of-file */
