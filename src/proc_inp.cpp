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
/* File:           src/proc_inp.cpp                                           */
/* Contents:       Process input file                                         */
/* Author(s):      kb                                                         */
/* Last change:    2018-04-27                                                 */
/*----------------------------------------------------------------------------*/

#include "basics.h"
                   /* Basic system definitions */
#include "common.h"
                   /* Common declarations and definitions */

#include <ctype.h>
#include <stddef.h>
#include <stdio.h>
#include <string.h>

                   /* Local macros: */
                   /* ------------- */

#define M_BYTES_PER_LINE 16
                   /* Number of bytes per line in hex dump */
#define M_NOT_PRINTABLE 183
                   /* Representation of a byte which is not
                      printable */

                   /* Local data: */
                   /* ----------- */

static t_Ui32 progress_block;
                   /* Block size in progress indication */
static t_Ui16 progress_count;
                   /* Count for progress indication */
static t_Ui16 progress_index;
                   /* Current index in progress indication */
                   /* 0=none, 1=100 KB, 2=1 MB, 3=10 MB */
static t_Ui32 progress_limit;
                   /* Limit for next progress indication; bytes */

                   /* Local functions: */
                   /* ---------------- */

#if LISTER
static t_Retc hex_dump (void);
                   /* Generate hex dump */
#endif /* LISTER */

#if LISTER
/*----------------------------------------------------------------------------*/
/* hex_dump        -- Generate hex dump                                       */
/*----------------------------------------------------------------------------*/

 static
 t_Retc hex_dump (void)
{
	t_Byte b;      /* Current byte */
	int c;         /* Current byte (or EOF) */
	int out;       /* Count for listed bytes */
	t_Retc ret;    /* Return code */
	char txt_buffer[M_BYTES_PER_LINE + 1];
                   /* Buffer for textual representation */

				   /* Preset the return code: */
	ret = RC_FAIL;

				   /* Check if input file is available: */
	Assert (input_file != NULL, "Input file not available");

                   /* Check if list file is available: */
	Assert (list_file != NULL, "List file not available");

                   /* Read the input file and dump its contents: */
	out = 0;
	memset (txt_buffer, 0, M_BYTES_PER_LINE + 1);
	while (TRUE)
	{
		c = fgetc (input_file);
		if (c == EOF)
		{
			break;
		}

		b = (t_Byte) (c & 0x00ff);

		if (out == 0)
		{
			list_text (-1, "  0x" M_FMT_UI32_HEX " : 0x", input_offset);
		}

		++ input_length;
		++ input_offset;

		list_text (-1, " " M_FMT_BYTE_HEX, b);

		if (isprint (b))
		{
			txt_buffer[out] = b;
		}
		else
		{
			txt_buffer[out] = (t_Byte) M_NOT_PRINTABLE;
		}

		++ out;

		if (out >= M_BYTES_PER_LINE)
		{
			list_text (-1, "  ");
			list_text (-1, "[%s]", txt_buffer);
			list_text (-1, "\n");

			memset (txt_buffer, 0, M_BYTES_PER_LINE + 1);
			out = 0;
		}
		else if ((out % 4) == 0)
		{
			list_text (-1, " ");
		}

				   /* Check against length limit: */
		if (length_limit_defined && length_limit > 0)
		{
			if (input_length >= length_limit)
			{
				if (list_file != stdout)
				{
					printf ("-> Length limit reached\n");
				}
				break;
			}
		}
	}

	if (out > 0)
	{
		while (out < M_BYTES_PER_LINE)
		{
			list_text (-1, "   ");
			txt_buffer[out] = ' ';

			++ out;

			if (out < M_BYTES_PER_LINE)
			{
				if ((out % 4) == 0)
				{
					list_text (-1, " ");
				}
			}
		}

		list_text (-1, "  ");
		list_text (-1, "[%s]", txt_buffer);
		list_text (-1, "\n");
	}

	if (list_file == stdout)
	{
		if (length_limit_defined && length_limit > 0)
		{
			if (input_length >= length_limit)
			{
				printf ("-> Length limit reached\n");
			}
		}
	}

	list_text (-1, "; end of input file\n");
	list_text (-1, "; length=%lu byte(s)\n", input_offset);

	printf ("-> End of input file reached\n");

                   /* Set the return code: */
	ret = RC_OKAY;

	return ret;
}
#endif /* LISTER */

/*----------------------------------------------------------------------------*/
/* process_input   -- Process input file                                      */
/*----------------------------------------------------------------------------*/

 t_Retc process_input (void)
{
	const char *mode;
				   /* Access mode for input file */
	t_Retc lrc;    /* Local return code */
	t_Retc ret;    /* Return code */

				   /* Preset the return code: */
	ret = RC_FAIL;

				   /* Check if some path name for input file: */
	Assert (input_path != NULL, "No path name for input file");

				   /* Check if input file not yet available: */
	Assert (input_file == NULL, "Input file already available");

                   /* Preset some local data: */
	progress_block = 0;
	progress_count = 0;
	progress_index = 0;
	progress_limit = 0;

#if LISTER
	if (list_hex_dump)
	{
		mode = "rb";
	}
	else
#endif /* LISTER */
	{
				   /* Check if some input format defined: */
		Assert (input_format != e_input_format_undefined,
                "No input format defined");

		mode = "rb";
	}

				   /* Try to open input file: */
	input_file = fopen (input_path, mode);
	if (input_file == NULL)
	{
		error_msg ("Cannot find or open input file '%s'", input_path);
		goto done;
	}

				   /* Notify user: */
	printf ("-> Input file '%s' opened ...\n", input_path);

#if LISTER
                   /* Check for hex dump listing: */
	if (list_hex_dump)
	{
		lrc = hex_dump ();

		ret = lrc;
		goto done;
	}
#endif /* LISTER */

                   /* Prepare for progress indication: */
	if (show_progress_indication)
	{
		progress_block = 100 * 1024;
		progress_count = 0;
		progress_index = 1;
		progress_limit = 100 * 1024;
	}

                   /* Check for RFF header: */
	if (input_format == e_input_format_rff)
	{
		lrc = rff_header ();
		if (lrc != RC_OKAY)
		{
			goto done;
		}
	}

                   /* Read along ... */
	frames_count = 1;
	while (TRUE)
	{
                   /* Check against frames limit: */
		if (frames_limit_defined && frames_limit > 0)
		{
			if (input_frames >= frames_limit)
			{
				printf ("-> Frames limit reached\n");
				break;
			}
		}

                   /* Check against length limit: */
		if (length_limit_defined && length_limit > 0)
		{
			if (input_length >= length_limit)
			{
				printf ("-> Length limit reached\n");
				break;
			}
		}

                   /* Check against progress indication: */
		if (show_progress_indication)
		{
			if (input_length >= progress_limit)
			{
				t_Bool overflow;

				overflow = FALSE;
				++ progress_count;

				if (progress_count >= 10 && progress_index < 3)
				{
					overflow = TRUE;
					progress_count = 1;

					++ progress_index;
				}

				if (progress_index == 1)
				{
					printf ("-> " M_FMT_UI16
                            " KB of input data read and processed\n",
                            100 * progress_count);
				}
				else if (progress_index == 2)
				{
					printf ("-> " M_FMT_UI16
                            " MB of input data read and processed\n",
                            progress_count);
				}
				else
				{
					printf ("-> " M_FMT_UI16
                            " MB of input data read and processed\n",
                            10 * progress_count);
				}

				if (overflow)
				{
					progress_limit += progress_block;

					if (progress_index == 2)
					{
						progress_block = 1024 * 1024;
					}
					else if (progress_index == 3)
					{
						progress_block = 10 * 1024 * 1024;
					}
				}
				else
				{
					progress_limit += progress_block;
				}
			}
		}

                   /* Process SASS-C IOSS frame: */
		if (input_format == e_input_format_ioss)
		{
			lrc = ioss_frame ();
			if (lrc == RC_DONE)
			{
#if LISTER
				list_text (3, "\n");
				list_text (-1, "; end of input file\n");
				list_text (-1, "; length=" M_FMT_UI32 " byte(s)\n",
                               input_offset);
#endif /* LISTER */

				printf ("-> End of input file reached\n");
				break;
			}
			else if (lrc == RC_FAIL)
			{
				break;
			}
		}

				   /* Process "netto" frame: */
		else if (input_format == e_input_format_netto)
		{
			lrc = netto_frame ();
			if (lrc == RC_DONE)
			{
#if LISTER
				list_text (3, "\n");
				list_text (-1, "; end of input file\n");
				list_text (-1, "; length=" M_FMT_UI32 " byte(s)\n",
                               input_offset);
#endif /* LISTER */

				printf ("-> End of input file reached\n");
				break;
			}
			else if (lrc == RC_FAIL)
			{
				break;
			}
		}

                   /* Process RFF frame: */
		else if (input_format == e_input_format_rff)
		{
			lrc = rff_frame ();
			if (lrc == RC_DONE)
			{
#if LISTER
				list_text (3, "\n");
				list_text (-1, "; end of input file\n");
				list_text (-1, "; length=" M_FMT_UI32 " byte(s)\n",
                               input_offset);
#endif /* LISTER */

				printf ("-> End of input file reached\n");
				break;
			}
			else if (lrc == RC_FAIL)
			{
				break;
			}
		}

				   /* Process 'sequence of records' frame: */
		else if (input_format == e_input_format_sequence_of_records)
		{
			lrc = rec_frame ();
			if (lrc == RC_DONE)
			{
#if LISTER
				list_text (3, "\n");
				list_text (-1, "; end of input file\n");
				list_text (-1, "; length=" M_FMT_UI32 " byte(s)\n",
                               input_offset);
#endif /* LISTER */

				printf ("-> End of input file reached\n");
				break;
			}
			else if (lrc == RC_FAIL)
			{
				break;
			}
		}

		else
		{
			Assert (FALSE, "Not yet implemented");
		}

		if (lrc == RC_OKAY)
		{
#if LISTER
			if (0 < list_level && list_level < 3)
			{
				list_text (-1, "\n");
			}
#endif /* LISTER */
		}
	}

				   /* Set the return code: */
	ret = RC_OKAY;

	done:          /* We are done */
	return ret;
}
/* end-of-file */
