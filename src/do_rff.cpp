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
/* File:           src/do_rff.cpp                                             */
/* Contents:       Process COMSOFT RFF (TM) input format                      */
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

static t_Secs last_frame_tod = 0.0;
                   /* Last frame time of day; seconds */
static t_Bool last_frame_tod_present = FALSE;
                   /* Last frame time of day present */

                   /* Local functions: */
                   /* ---------------- */

static t_Retc get_wall_time (char *buffer, t_Wall_Time *wtm_ptr);
                   /* Try to get wall time from text */

/*----------------------------------------------------------------------------*/
/* get_wall_time   -- Try to get wall time from text                          */
/*----------------------------------------------------------------------------*/

 static
 t_Retc get_wall_time (char *buffer, t_Wall_Time *wtm_ptr)
{
	int n;
	t_Retc ret;    /* Return code */
	int y, mo, d, h, mi, s, tmp;

				   /* Preset the return code: */
	ret = RC_FAIL;

				   /* Check parameters: */
	Assert (buffer != NULL, "Invalid parameter");
	Assert (wtm_ptr != NULL, "Invalid parameter");

				   /* Preset wall time: */
	wtm_ptr->year = 0;
	wtm_ptr->month = 0;
	wtm_ptr->day = 0;
	wtm_ptr->hours = 0;
	wtm_ptr->minutes = 0;
	wtm_ptr->seconds = 0;
	wtm_ptr->milliseconds = 0;

                   /* Check for US style: */
	if (buffer[3] == '/' && buffer[6] == '/' &&
        buffer[9] == ' ' &&
        buffer[12] == ':' && buffer[15] == ':')
	{
		n = sscanf (buffer + 1, "%d", &mo);
		if (n != 1)
		{
			goto done;
		}
		n = sscanf (buffer + 4, "%d", &d);
		if (n != 1)
		{
			goto done;
		}
		n = sscanf (buffer + 7, "%d", &y);
		if (n != 1)
		{
			goto done;
		}
		n = sscanf (buffer + 10, "%d", &h);
		if (n != 1)
		{
			goto done;
		}
		n = sscanf (buffer + 13, "%d", &mi);
		if (n != 1)
		{
			goto done;
		}
		n = sscanf (buffer + 16, "%d", &s);
		if (n != 1)
		{
			goto done;
		}

		if (y >= 70)
		{
			wtm_ptr->year = 1900 + y;
		}
		else
		{
			wtm_ptr->year = 2000 + y;
		}
		wtm_ptr->month = mo;
		wtm_ptr->day = d;
		wtm_ptr->hours = h;
		wtm_ptr->minutes = mi;
		wtm_ptr->seconds = s;
	}
	else
                   /* Check for German style: */
	if (buffer[2] == '/' && buffer[5] == '/' &&
        buffer[8] == ' ' &&
        buffer[11] == ':' && buffer[14] == ':')
	{
		n = sscanf (buffer, "%d", &d);
		if (n != 1)
		{
			goto done;
		}
		n = sscanf (buffer + 3, "%d", &mo);
		if (n != 1)
		{
			goto done;
		}
		n = sscanf (buffer + 6, "%d", &y);
		if (n != 1)
		{
			goto done;
		}
		n = sscanf (buffer + 9, "%d", &h);
		if (n != 1)
		{
			goto done;
		}
		n = sscanf (buffer + 12, "%d", &mi);
		if (n != 1)
		{
			goto done;
		}
		n = sscanf (buffer + 15, "%d", &s);
		if (n != 1)
		{
			goto done;
		}

		if (mo > 12 && d <= 12)
		{
			tmp = d;
			d = mo;
			mo = tmp;
		}

		if (y >= 70)
		{
			wtm_ptr->year = 1900 + y;
		}
		else
		{
			wtm_ptr->year = 2000 + y;
		}
		wtm_ptr->month = mo;
		wtm_ptr->day = d;
		wtm_ptr->hours = h;
		wtm_ptr->minutes = mi;
		wtm_ptr->seconds = s;
	}
	else
                   /* Check for Austrian style: */
	if (buffer[4] == '-' && buffer[7] == '-' &&
        buffer[10] == ' ' &&
        buffer[13] == ':' && buffer[16] == ':')
	{
    	n = sscanf (buffer, "%d", &y);
        if (n != 1)
		{
        	goto done;
		}
		n = sscanf (buffer + 5, "%d", &mo);
		if (n != 1)
		{
			goto done;
		}
		n = sscanf (buffer + 8, "%d", &d);
		if (n != 1)
		{
			goto done;
		}
		n = sscanf (buffer + 11, "%d", &h);
		if (n != 1)
		{
			goto done;
		}
		n = sscanf (buffer + 14, "%d", &mi);
		if (n != 1)
		{
			goto done;
		}
		n = sscanf (buffer + 17, "%d", &s);
		if (n != 1)
		{
			goto done;
		}

		wtm_ptr->year = y;
		wtm_ptr->month = mo;
		wtm_ptr->day = d;
		wtm_ptr->hours = h;
		wtm_ptr->minutes = mi;
		wtm_ptr->seconds = s;
	}

				   /* Set the return code: */
	ret = RC_OKAY;

	done:          /* We are done */
	return ret;
}

/*----------------------------------------------------------------------------*/
/* rff_frame       -- Read and process RFF frame                              */
/*----------------------------------------------------------------------------*/

 t_Retc rff_frame (void)
{
#if LISTER
	t_Time atm;    /* Absolute time (since UTC midnight); milliseconds */
#endif /* LISTER */
	t_Byte buffer[M_MAX_RFF_FRAME_LENGTH];
                   /* Buffer for RFF data frame (including frame header) */
	t_Ui32 f_time; /* Frame time; milliseconds */
	t_Ui16 length; /* Frame data length */
	t_Ui16 length2;
                   /* Overall frame length */
	t_Retc lrc;    /* Local return code */
	int n;         /* Auxiliary */
	t_Retc ret;    /* Return code */
	t_Ui32 rtm;    /* Relative time (since start of recording);
                      milliseconds */
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

				   /* Preset the buffer: */
	memset (buffer, 0, M_MAX_RFF_FRAME_LENGTH);

				   /* Check if input file available: */
	Assert (input_file != NULL, "No input file");

				   /* Try to read the frame header: */
                   /* ----------------------------- */

	n = fread (buffer, 1, M_RFF_FRAME_HEADER_LENGTH, input_file);

                   /* Check return code: */
	if (n <= 0)
	{
		ret = RC_DONE;
		goto done;
	}
	else if (n != M_RFF_FRAME_HEADER_LENGTH)
	{
		error_msg ("Read error at RFF (frame) header");
		goto done;
	}

				   /* Extract the frame data length: */
	length = make_ui16 (buffer[n - 1], buffer[n - 2]);
                   /* Byte ordering is LSB + MSB */

				   /* Set overall frame length: */
	length2 = M_RFF_FRAME_HEADER_LENGTH + length;

				   /* Check length: */
	if (length == 0)
	{
		error_msg ("Invalid RFF frame length");
		goto done;
	}
	else if (length2 > M_MAX_RFF_FRAME_LENGTH)
	{
		error_msg ("RFF frame too long (%hu)", length2);
		goto done;
	}

				   /* Try to read the "rest" of the RFF frame: */
                   /* ---------------------------------------- */

	n = fread (buffer + M_RFF_FRAME_HEADER_LENGTH, 1, length, input_file);

                   /* Check return code: */
	if (n != length)
	{
		error_msg ("Read error at RFF frame data");
		goto done;
	}

				   /* Get the relative time: */
	rtm = make_ui32 (buffer[3], buffer[2], buffer[1], buffer[0]);

                   /* Remember this relative time: */
	input_rtm = rtm;

                   /* Check against start relative time: */
	if (start_rtm > 0 && (t_Time) rtm < start_rtm)
	{
                   /* This frame is not (yet) to be listed ... */

				   /* Increment frames count: */
		++ frames_count;

				   /* Increment offset into input file: */
		input_offset += length2;

                   /* Set return code: */
		ret = RC_SKIP;

                   /* We are done: */
		goto done;
	}

                   /* Try to set frame date: */
	frame_date_present = FALSE;
	if (start_date_defined && start_time_available)
	{
		t_Date current_date;
                   /* Current date */
		t_Ui32 day_length;
                   /* Number of milliseconds per day */
		int days;  /* Number of days */
		t_Ui32 tm; /* Current time; milliseconds */

		day_length = M_MILLISECONDS_PER_DAY;

		current_date = start_date;

		tm = start_time + rtm;

		days = 0;
		while (tm >= day_length)
		{
			tm -= day_length;
			++ days;
		}

		current_date.day += days;

		lrc = normalize (&current_date);
		if (lrc == RC_OKAY)
		{
			frame_date = current_date;
			frame_date_present = TRUE;
		}
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

		Assert (length > sizeof (t_Ui32), "RFF frame too short (no sqn)");

		p = buffer + M_RFF_FRAME_HEADER_LENGTH;

		sqn = make_ui32 (*(p + 3), *(p + 2), *(p + 1), *p);
#endif /* LISTER */
		sqn_present = TRUE;
	}

                   /* Set frame time: */
	if (start_time_available)
	{
		f_time = start_time + rtm;
	}
	else
	{
		f_time = rtm;
	}

                   /* Normalize: */
	while (f_time >= M_MILLISECONDS_PER_DAY)
	{
		f_time -= M_MILLISECONDS_PER_DAY;
	}

                   /* Remember this frame time: */
	frame_time_present = TRUE;
	frame_time = 0.001 * f_time;

                   /* Check frame time-of-day: */
	Assert (0.0 <= frame_time && frame_time < M_SECONDS_PER_DAY,
            "Invalid frame time-of-day");

                   /* Check against last frame time, if present: */
	if (last_frame_tod_present)
	{
		t_Secs dtf;

		dtf = frame_time - last_frame_tod;

		if (dtf < -1.0 && dtf > - 0.5 * M_SECONDS_PER_DAY)
		{
#if LISTER
			list_text (-1, "; Near %s UTC:"
                           " Backjump in RFF frame_time (%.3f [secs])\n",
                           utc_text (frame_time), dtf);
#endif /* LISTER */

			printf ("E> Near %s UTC:"
                    " Backjump in RFF frame time (%.3f seconds)\n",
                    utc_text (frame_time), dtf);
		}
		else if (dtf > +1.5 && dtf < + 0.5 * M_SECONDS_PER_DAY)
		{
#if LISTER
			list_text (-1, "; Near %s UTC:"
                           " Forejump in RFF frame_time (%.3f [secs])\n",
                           utc_text (frame_time), dtf);
#endif /* LISTER */

			printf ("-> Near %s UTC:"
                    " Forejump in RFF frame time (%.3f seconds)\n",
                    utc_text (frame_time), dtf);
		}
	}

                   /* Remember frame time: */
	last_frame_tod = frame_time;
	last_frame_tod_present = TRUE;

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

				   /* List the RFF frame (lowest level): */
	if (to_be_listed)
	{
		list_text (1, "; RFF frame " M_FMT_UI32 " ", frames_count);
		list_text (1, "(length=" M_FMT_UI16, length2);
		if (sqn_present)
		{
			list_text (1, "; sqn=" M_FMT_UI32, sqn);
		}
		list_text (1, ") at offset 0x" M_FMT_UI32_HEX " (" M_FMT_UI32 "):\n",
                      input_offset, input_offset);
		list_frame (1, M_RFF_FRAME_HEADER_LENGTH, buffer);
		list_frame (1, length, buffer + M_RFF_FRAME_HEADER_LENGTH);
		if (list_level == 2)
		{
			list_text (2, "; RFF frame " M_FMT_UI32 ":\n", frames_count);
		}
		else if (list_level == 3 && list_frame_numbers)
		{
			list_text (3, "; RFF frame " M_FMT_UI32 ":\n", frames_count);
		}

                   /* List date: */
		if (frame_date_present)
		{
			list_text (1, "; date=" M_FMT_DATE,
                          frame_date.year, frame_date.month, frame_date.day);
		}

				   /* List time: */
		if (start_time_available)
		{
				   /* Compute absolute time: */
			atm = start_time + (t_Time) rtm;

			list_text (1, "; rtm=" M_FMT_UI32 "; atm=%s\n",
                          rtm, utc_text (0.001 * atm));
		}
		else
		{
			list_text (1, "; rtm=%s\n",
                          utc_text (0.001 * rtm));
		}
	}
#endif /* LISTER */

				   /* Process this data frame: */
	if (sqn_present)
	{
		lrc = do_frame (input_offset, 0x0000,
                        length - sizeof (t_Ui32),
                        buffer + M_RFF_FRAME_HEADER_LENGTH + sizeof (t_Ui32));
	}
	else
	{
		lrc = do_frame (input_offset, 0x0000,
                        length, buffer + M_RFF_FRAME_HEADER_LENGTH);
	}

                   /* Check local return code: */
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

/*----------------------------------------------------------------------------*/
/* rff_header      -- Read and process RFF file header                        */
/*----------------------------------------------------------------------------*/

 t_Retc rff_header (void)
{
	t_Byte header[M_RFF_FILE_HEADER_LENGTH];
                   /* Buffer for RFF file header */
	t_Retc lrc;    /* Local return code */
	int n;         /* Auxiliary */
	t_Retc ret;    /* Return code */
	t_Wall_Time wtm;
                   /* Buffer for wall time */

				   /* Preset the return code: */
	ret = RC_FAIL;

				   /* Preset the buffer: */
	memset (header, 0, M_RFF_FILE_HEADER_LENGTH);

				   /* Check if input file available: */
	Assert (input_file != NULL, "No input file");

				   /* Check if at the beginning of the input file: */
	Assert (input_offset == 0, "Not at the beginning");

				   /* Try to read the RFF file header: */
                   /* -------------------------------- */

	n = fread (header, 1, M_RFF_FILE_HEADER_LENGTH, input_file);

                   /* Check return code: */
	if (n != M_RFF_FILE_HEADER_LENGTH)
	{
		error_msg ("Read error at RFF file header");
		goto done;
	}

#if LISTER
				   /* List the RFF header (lowest level): */
	list_text (1, "; RFF header (length=%hu) "
                  "at offset 0x%08lx (%lu):\n",
                  (t_Ui16) n, input_offset, input_offset);
	list_frame (1, M_RFF_FILE_HEADER_LENGTH, header);

	list_text (2, "; RFF header:\n");
#endif /* LISTER */

				   /* Try to find a start wall time: */
	lrc = get_wall_time ((char *) header, &wtm);
	if (lrc == RC_OKAY)
	{
		start_time = cnv_wtm_to_tod (wtm);
		start_time_available = TRUE;
		start_wall_time = wtm;
	}

				   /* Try to find a stop wall time: */
	lrc = get_wall_time ((char *) (header + 20), &wtm);
	if (lrc == RC_OKAY)
	{
		stop_time_available = TRUE;
		stop_wall_time = wtm;
	}

#if LISTER
				   /* List these wall times: */
	if (start_time_available)
	{
		list_text (1, "; recording started at: %s\n",
                      cnv_wtm_to_txt (start_wall_time));
	}
	if (stop_time_available)
	{
		list_text (1, "; recording stopped at: %s\n",
                      cnv_wtm_to_txt (stop_wall_time));
	}
	list_text (2, "\n");
#endif /* LISTER */

                   /* Set start date: */
	if (start_time_available)
	{
		start_date.year = start_wall_time.year;
		start_date.month = start_wall_time.month;
		start_date.day = start_wall_time.day;

		start_date_defined = TRUE;
	}

                   /* Increment number of bytes read and listed: */
	if (start_rtm == 0)
	{
		input_length += M_RFF_FILE_HEADER_LENGTH;
	}

				   /* Increment input file offset: */
	input_offset += M_RFF_FILE_HEADER_LENGTH;

				   /* Set the return code: */
	ret = RC_OKAY;

	done:          /* We are done */
	return ret;
}
/* end-of-file */
