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
/* File:           src/do_ioss.cpp                                            */
/* Contents:       Process SASS-C IOSS data frame                             */
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

                   /* Local data: */
                   /* ----------- */

static t_Bool initiated = FALSE;
                   /* Package initiated */
static t_Ui32 last_rtm;
                   /* Last relative time; milliseconds */
static t_Bool last_rtm_present;
                   /* Last relative time present */
static int midnight_jumps;
                   /* Number of midnight jumps */

/*----------------------------------------------------------------------------*/
/* ioss_frame      -- Read and process SASS-C IOSS frame                      */
/*----------------------------------------------------------------------------*/

 t_Retc ioss_frame (void)
{
	t_Byte board_number;
                   /* Board or line number */
	t_Byte buffer[M_MAX_IOSS_FRAME_LENGTH];
				   /* Buffer for IOSS data frame (including
					  frame header) */
	t_Ui16 length; /* Frame data length */
	t_Ui16 length2;
                   /* Inner data length */
	t_Retc lrc;    /* Local return code */
	int n;         /* Auxiliary */
	t_Byte recording_day;
                   /* Recording day */
	t_Retc ret;    /* Return code */
	t_Ui32 rtm;    /* Time; milliseconds */
	t_Secs rtm_in_secs;
                   /* Time (of day); seconds */
#if LISTER
	t_Ui32 sqn;    /* Sequence number */
#endif /* LISTER */
	t_Bool sqn_present;
                   /* Sequence number present */
	t_Real tm_in_secs;
                   /* Auxiliary */
#if LISTER
	t_Bool to_be_listed;
                   /* Frame to be listed */
#endif /* LISTER */

				   /* Preset the return code: */
	ret = RC_FAIL;

                   /* Check whether package is already initiated: */
	if (!initiated)
	{
		last_rtm = 0;
		last_rtm_present = FALSE;
		midnight_jumps = 0;

		initiated = TRUE;
	}

				   /* Preset the buffer: */
	memset (buffer, 0, M_MAX_IOSS_FRAME_LENGTH);

				   /* Check if input file available: */
	Assert (input_file != NULL, "No input file");

				   /* Try to read the frame header: */
	n = fread (buffer, 1, M_IOSS_FRAME_HEADER_LENGTH, input_file);
	if (n <= 0)
	{
		ret = RC_DONE;
		goto done;
	}
	else if (n != M_IOSS_FRAME_HEADER_LENGTH)
	{
		error_msg ("Read error at IOSS (frame) header");
		goto done;
	}

				   /* Extract the frame data length: */
	length = make_ui16 (buffer[0], buffer[1]);
                   /* Byte ordering is MSB + LSB */
                   /* "Brutto" length, i.e. including frame header */

				   /* Check length: */
	if (length < M_IOSS_FRAME_HEADER_LENGTH)
	{
		error_msg ("Invalid IOSS frame length");
		goto done;
	}
	else if (length > M_MAX_IOSS_FRAME_LENGTH)
	{
		error_msg ("IOSS frame too long (" M_FMT_UI16 ")", length);
		goto done;
	}

                   /* Get board/line number: */
	board_number = buffer[3];

                   /* Store this board/line number: */
	current_line_number = board_number;
	current_line_number_defined = TRUE;

                   /* Get recording day: */
	recording_day = buffer[4];

				   /* Get time: */
	rtm = make_ui32 (0x00, buffer[5], buffer[6], buffer[7]);

                   /* Convert to milliseconds: */
	rtm = rtm * 10;

                   /* Convert to seconds: */
	rtm_in_secs = 0.001 * rtm;

                   /* Evaluate relative time (checking for hidden
                      midnight jumps): */
	if (last_rtm_present)
	{
		t_Secs last_rtm_in_secs;

		last_rtm_in_secs = 0.001 * last_rtm;

		if (rtm_in_secs < 60.0 &&
            last_rtm_in_secs > (M_SECONDS_PER_DAY - 60.0))
		{
			if (recording_day == 0)
			{
				printf ("W> Hidden midnight jump found\n");

				++ midnight_jumps;
			}
		}
	}

                   /* Remember last relative time: */
	last_rtm = rtm;
	last_rtm_present = TRUE;

                   /* Set inner data length: */
	length2 = length - M_IOSS_FRAME_HEADER_LENGTH;

				   /* Try to read the "rest" of the IOSS frame: */
	n = fread (buffer + M_IOSS_FRAME_HEADER_LENGTH, 1, length2, input_file);
	if (n != length2)
	{
		error_msg ("Read error at IOSS frame data");
		goto done;
	}

                   /* Check for sequence number: */
#if LISTER
	sqn = 0;
#endif /* LISTER */
	sqn_present = FALSE;
	if (with_sequence_number)
	{
#if LISTER
		t_Byte *p;

		Assert (length > sizeof (t_Ui32), "IOSS frame too short (no sqn)");

		p = buffer + M_IOSS_FRAME_HEADER_LENGTH;

		sqn = make_ui32 (*(p + 3), *(p + 2), *(p + 1), *p);
#endif /* LISTER */
		sqn_present = TRUE;
	}

                   /* Check for padding bytes: */
	Assert (length2 >= 4, "Invalid inner data length");

	if (buffer[length - 1] != 0xa5 || buffer[length - 2] != 0xa5 ||
        buffer[length - 3] != 0xa5 || buffer[length - 4] != 0xa5)
	{
		error_msg ("Padding byte error at IOSS frame data");
		goto done;
	}

                   /* Remember this relative time: */
	input_rtm = rtm;

                   /* Check against start relative time: */
	if (start_rtm > 0 && (t_Time) rtm < start_rtm)
	{
                   /* This frame is not (yet) to be listed ... */

                   /* Increment frames count: */
		++ frames_count;

				   /* Increment offset into input file: */
		input_offset += length;

                   /* Set return code: */
		ret = RC_SKIP;

                   /* We are done: */
		goto done;
	}

                   /* Try to compute frame date: */
	frame_date_present = FALSE;
	if (start_date_defined)
	{
		t_Date current_date;
                   /* Current date */

		current_date = start_date;

		current_date.day += recording_day;

		if (recording_day == 0 && midnight_jumps > 0)
		{
			current_date.day += midnight_jumps;
		}

		lrc = normalize (&current_date);
		if (lrc == RC_OKAY)
		{
			frame_date = current_date;
			frame_date_present = TRUE;
		}
	}

                   /* Remember frame time: */
	frame_time = 0.001 * rtm;
	frame_time_present = TRUE;

                   /* Compute effective frame time: */
	tm_in_secs = frame_time;
	if (time_bias_defined)
	{
		tm_in_secs += time_bias;
	}

#if LISTER
                   /* Check whether frame is to be listed: */
	to_be_listed = FALSE;
	if (0 < list_level && list_level <= 3)
	{
		to_be_listed = TRUE;
	}

				   /* List the IOSS frame (lowest level): */
	if (to_be_listed)
	{
		list_text (1, "; IOSS frame " M_FMT_UI32 " ", frames_count);
		list_text (1, "(length=" M_FMT_UI16, length2);
		if (sqn_present)
		{
			list_text (1, "; sqn=" M_FMT_UI32, sqn);
		}
		list_text (1, ") at offset 0x" M_FMT_UI32_HEX " (" M_FMT_UI32 "):\n",
                      input_offset, input_offset);
		list_frame (1, M_IOSS_FRAME_HEADER_LENGTH, buffer);
		list_frame (1, length2 - 4, buffer + M_IOSS_FRAME_HEADER_LENGTH);
		if (list_level == 2)
		{
			list_text (2, "; IOSS frame " M_FMT_UI32 ":\n", frames_count);
		}
		else if (list_level == 3 && list_frame_numbers)
		{
			list_text (3, "; IOSS frame " M_FMT_UI32 ":\n", frames_count);
		}

                   /* List date: */
		if (frame_date_present)
		{
			list_text (1, "; date=" M_FMT_DATE,
                          frame_date.year, frame_date.month, frame_date.day);
		}

                   /* List time: */
		list_text (1, "; rtm=" M_FMT_UI32 " (%s UTC)",
                      rtm, utc_text (frame_time));
		if (time_bias_defined)
		{
			list_text (1, "; effective_rtm=%s UTC", utc_text (tm_in_secs));
		}
		list_text (1, "\n");
	}
#endif /* LISTER */

				   /* Process this data frame: */
	if (sqn_present)
	{
		lrc = do_frame (input_offset, 0x0000,
                        length2 - 4 - sizeof (t_Ui32),
                        buffer + M_IOSS_FRAME_HEADER_LENGTH + sizeof (t_Ui32));
	}
	else
	{
		lrc = do_frame (input_offset, 0x0000,
                        length2 - 4, buffer + M_IOSS_FRAME_HEADER_LENGTH);
	}

                   /* Check local return code: */
	if (lrc != RC_OKAY && lrc != RC_SKIP)
	{
		bad_frame ("Bad IOSS frame at", input_offset, length, buffer);

		if (stop_on_error)
		{
			ret = lrc;
			goto done;
		}

		ret = RC_SKIP;
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
