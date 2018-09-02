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
/* File:           src/do_rec.cpp                                             */
/* Contents:       Process REC input format                                   */
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

#define M_REC_MAX_FRAME_LENGTH 1024
				   /* Max. length of REC frame data */

/*----------------------------------------------------------------------------*/
/* rec_frame       -- Read and process REC frame                              */
/*----------------------------------------------------------------------------*/

 t_Retc rec_frame (void)
{
	t_Byte b0, b1; /* Auxiliaries */
	t_Byte buffer[M_REC_MAX_FRAME_LENGTH];
				   /* Buffer for REC data frame */
	t_Ui16 length; /* Frame data length */
	t_Ui16 length2;
                   /* Overall frame length */
	t_Retc lrc;    /* Local return code */
	int n;         /* Auxiliary */
	t_Retc ret;    /* Return code */

				   /* Preset the return code: */
	ret = RC_FAIL;

				   /* Preset the buffer: */
	memset (buffer, 0, M_REC_MAX_FRAME_LENGTH);

				   /* Check if input file available: */
	Assert (input_file != NULL, "No input file");

                   /* Try to read the record length: */
	n = fgetc (input_file);
	if (n == EOF)
	{
		ret = RC_DONE;
		goto done;
	}
	b0 = (t_Byte) (n & 0x00ff);
	n = fgetc (input_file);
	if (n == EOF)
	{
		goto done;
	}
	b1 = (t_Byte) (n & 0x00ff);

                   /* Compute record length: */
	if (big_endian)
	{
		length = make_ui16 (b0, b1);
                   /* Byte ordering is MSB + LSB */
	}
	else
	{
		length = make_ui16 (b1, b0);
                   /* Byte ordering is LSB + MSB */
	}

				   /* Set overall frame length: */
	length2 = 2 + length;

				   /* Check length: */
	if (length == 0)
	{
		error_msg ("Invalid REC frame length");
		goto done;
	}
	else if (length > M_REC_MAX_FRAME_LENGTH)
	{
		error_msg ("REC frame too long (%hu)", length);
		goto done;
	}

				   /* Try to read the "rest" of the REC frame: */
	n = fread (buffer, 1, length, input_file);

	if (n != length)
	{
		error_msg ("Read error at REC frame data");
		goto done;
	}

#if LISTER
				   /* List the REC frame (lowest level): */
	list_text (1, "; REC frame %lu (length=%hu) "
                  "at offset 0x%08lx (%lu):\n",
                  frames_count, length,
                  input_offset, input_offset);
	list_frame (1, length, buffer);

	list_text (2, "; REC frame %lu:\n", frames_count);
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
	input_length += length2;

				   /* Increment offset into input file: */
	input_offset += length2;

				   /* Set the return code: */
	ret = RC_OKAY;

	done:          /* We are done */
	return ret;
}
/* end-of-file */
