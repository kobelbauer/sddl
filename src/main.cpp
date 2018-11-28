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
/* File:           src/main.cpp                                               */
/* Contents:       Main program for Surveillance Data Decoder and Lister      */
/* Author(s):      kb                                                         */
/* Last change:    2018-04-27                                                 */
/*----------------------------------------------------------------------------*/

#include "basics.h"
                   /* Basic system definitions */
#include "common.h"
                   /* Common declarations and definitions */
#include "progid.h"
                   /* Program identity and version definitions */

#include <malloc.h>
#include <stddef.h>
#include <stdio.h>
#include <string.h>
#include <sys/time.h>
#include <time.h>

#include "global.h"

#if USE_JSON
#include "jsonwriter.h"
#endif

                   /* Public variables: */
                   /* ----------------- */

t_Adsb adsb;
                   /* Buffer for ADS-B report information */
t_Bool after_midnight;
				   /* After midnight flag */
				   /* Set from 00:00:00 to approx. 00:01:00 */
int current_line_number;
                   /* Current board/line number */
t_Bool current_line_number_defined;
                   /* Current board/line number defined */
t_Byte current_stns;
                   /* Current system track numbering scheme */
t_Bool current_stns_defined;
                   /* Current system track numbering scheme defined */
t_Ui16 current_stns_dsi;
                   /* Associated data source identifier */
FILE *exc_file;
                   /* Pointer to special output file */
t_Frame_Time first_frame_time;
                   /* First frame time */
t_Date frame_date;
                   /* Frame date */
t_Bool frame_date_present;
                   /* Frame date present */
t_Secs frame_time;
                   /* Frame time; seconds */
t_Bool frame_time_present;
                   /* Frame time present */
t_Ui32 frames_count;
				   /* Input frames count */
FILE *input_file;
				   /* Pointer to input file */
t_Ui32 input_frames;
                   /* Number of frames read (and listed) from
                      input file */
t_Ui32 input_length;
                   /* Number of bytes read (and listed) from
                      input file */
t_Ui32 input_offset;
				   /* Offset within input file */
t_Ui32 input_rtm;
                   /* Relative time within input file */
t_Frame_Time last_frame_time;
                   /* Last frame time */
t_Ui16 last_sacsic;
                   /* Last radar SAC/SIC */
t_Bool last_sacsic_available;
                   /* Last radar SAC/SIC */
t_Ui32 last_tod;
                   /* Last time of day; 1/128 seconds */
				   /* Needed to transfer full time_of_day values
				      between radar links, i.e. to allow radar
				      links without radar service messages to
				      fill up the truncated time_of_day */
t_Bool last_tod_available;
                   /* Last time of day available flag */ 
#if LISTER
FILE *list_file;
				   /* Pointer to list file */
#endif /* LISTER */
t_Listed_Sensor listed_sensors[M_MAX_NUMBER_OF_SENSORS];
                   /* Listed sensors */
t_Mlat mlat;
                   /* Buffer for multilateration report information */
t_Ui16 number_of_sensor_descriptions;
                   /* Number of sensor descriptions */
t_Ui16 records_in_current_frame;
                   /* Number of records in current frame */
t_Rsrv rsrv;
                   /* Buffer for radar service information */
t_Rtgt rtgt;
                   /* Buffer for radar target information */
t_Sensor_Ident sacsic_list[M_MAX_NUMBER_OF_SENSORS];
                   /* List of sensor identities */
t_Ssta ssta;
                   /* Buffer for sensor status information */
t_Time start_time;
				   /* Start time of input; milliseconds */
t_Bool start_time_available;
				   /* Start time available flag */
t_Wall_Time start_wall_time;
				   /* Wall time for start of input */
t_Step step;
                   /* System picture step */
t_Wall_Time stop_wall_time;
				   /* Wall time for stop of input */
t_Bool stop_time_available;
				   /* Stop time available flag */
t_Strk strk;
                   /* Buffer for system track information */

#if CHKASS
jmp_buf on_fatal_error;
                   /* Label for longjmp() in case of fatal error */
#endif

				   /* Local functions: */
				   /* ---------------- */

static void print_categories (void);
                   /* Print list of supported ASTERIX categories */
static void print_info (void);
                   /* Print some helpful information */
static void print_formats (void);
                   /* Print list of supported formats */
static void print_subtypes (void);
                   /* Print list of supported format subtypes */

/*----------------------------------------------------------------------------*/
/* print_categories -- Print list of supported ASTERIX categories             */
/*----------------------------------------------------------------------------*/

 static
 void print_categories (void)
{
	printf ("The '%s' utility at the moment supports the "
            "following ASTERIX categories:\n", M_PRGNAM);
    printf ("\n");
	printf (" ASTERIX category 000\tn.a.\tApril 1998\n");
	printf (" ASTERIX category 001\t1.1 \tAugust 2002\n");
	printf (" ASTERIX category 002\t1.0 \tNovember 1997\n");
	printf (" ASTERIX category 003\tn.a.\tApril 1998\n");
	printf (" ASTERIX category 004\t1.2 \tMarch 2007\n");
	printf (" ASTERIX category 007\t----\tnot supported\n");
	printf (" ASTERIX category 008\t1.0 \tNovember 1997\n");
	printf (" ASTERIX category 009\t----\tnot supported\n");
	printf (" ASTERIX category 010\t1.1 \tMarch 2007\n");
	printf ("  option -vsn010=0.24s\t0.24*\tSensis"
            " (Heathrow MDS modifications)\n");
	printf (" ASTERIX category 011\t0.17\tDecember 2001\n");
	printf ("  option -vsn011=0.14\t0.14\tOctober 2000\n");
	printf ("  option -vsn011=0.14i\t0.14*\tSensis"
            " (Inn valley modification)\n");
	printf (" ASTERIX category 016\t    \tunknown\n");
	printf (" ASTERIX category 017\t0.5\tFebruary 1999\n");
	printf (" ASTERIX category 018\t----\tnot supported\n");
	printf (" ASTERIX category 019\t1.1\tMarch 2007\n");
	printf (" ASTERIX category 020\t1.5\tApril 2008\n");
	printf ("  option -vsn020=1.0\t1.0\tNovember 2005\n");
	printf ("  option -vsn020=1.2\t1.2\tApril 2007\n");
	printf ("  option -vsn020=1.5\t1.5\tApril 2008\n");
	printf (" ASTERIX category 021\t2.1\tMay 2011\n");
	printf ("  option -vsn021=0.12\t0.12\tFebruary 2001\n");
	printf ("  option -vsn021=0.13\t0.13\tJune 2001\n");
	printf ("  option -vsn021=0.20\t0.20\tDecember 2002\n");
	printf ("  option -vsn021=0.23\t0.23\tNovember 2003\n");
	printf ("  option -vsn021=1.0P\t1.0P\tApril 2008\n");
	printf ("  option -vsn021=1.4\t1.4\tJuly 2009\n");
	printf ("  option -vsn021=2.1\t2.1\tMay 2011\n");
	printf ("  option -vsn021=2.4\t2.4\t15 June 2015\n");
	printf (" ASTERIX category 023\t1.2\tMarch 2009\n");
	printf ("  option -vsn023=0.11\t0.11\tDecember 2002\n");
	printf ("  option -vsn023=1.0P\t1.0P\tApril 2008\n");
	printf ("  option -vsn023=1.1\t1.1\tSeptember 2008\n");
	printf ("  option -vsn023=1.2\t1.2\tMarch 2009\n");
	printf (" ASTERIX category 030\t2.8.1\t26 February 1999\n");
	printf (" ASTERIX category 031\t2.8.1\t26 February 1999\n");
	printf (" ASTERIX category 032\t2.8.1\t26 February 1999\n");
	printf (" ASTERIX category 034\t1.27\tMay 2007\n");
	printf (" ASTERIX category 048\t1.15\tApril 2007\n");
	printf ("  option -vsn048=1.14\t1.14\tNovember 2000\n");
	printf ("  option -vsn048=1.15\t1.15\tApril 2007\n");
	printf ("  option -vsn048=1.16\t1.16\tMarch 2009\n");
	printf (" ASTERIX category 062\t1.3\tApril 2005\n");
	printf (" ASTERIX category 063\t1.0\tMarch 2004\n");
	printf (" ASTERIX category 065\t0.12\tMarch 2003\n");
	printf ("  option -vsn065=0.12\t0.12\tMarch 2003\n");
	printf ("  option -vsn065=1.3\t1.3\tApril 2007\n");
	printf (" ASTERIX category 221\t?\t?\n");
	printf (" ASTERIX category 247\t1.2\tFebruary 2008\n");
	printf (" ASTERIX category 252\t2.8.1\t26 February 1999\n");
	printf ("\n");
	printf ("If you need additional categories or versions, please"
            " send us an e-mail:\n");
	printf (" %s\n", M_E_MAIL);

	return;
}

/*----------------------------------------------------------------------------*/
/* print_formats   -- Print list of supported formats                         */
/*----------------------------------------------------------------------------*/

 static
 void print_formats (void)
{
	printf ("The '%s' utility at the moment supports the "
            "following recording formats:\n", M_PRGNAM);
    printf ("\n");
	printf (" -asf     (ASTERIX in) IOSS Final Format recording\n");
	printf (" -ioss    SASS-C IOSS (Final) recording (default)\n");
	printf (" -net     Binary 'netto' recording\n");
	printf (" -rec     Sequence of records\n");
	printf (" -rff     Comsoft (TM) RFF recording\n");

	printf ("\n");
	printf ("Our '%s' utility at the moment supports the "
            "following data formats:\n", M_PRGNAM);
    printf ("\n");
	printf (" -asf     ASTERIX (in IOSS Final Format recording)\n");
	printf (" -asx     ASTERIX data format (default)\n");
	printf (" -zzz     Unknown data format - ignore\n");
	printf ("\n");
	printf ("Please be aware that NOT EVERY combination of recording "
            "and data formats\n");
	printf (" is reasonable.\n");
	printf ("\n");
    printf ("Thank you for using our software.\n");

	return;
}

/*----------------------------------------------------------------------------*/
/* print_info      -- Print some helpful information                          */
/*----------------------------------------------------------------------------*/

 static
 void print_info (void)
{
	printf ("The '%s' utility may be used as follows:\n", M_PROMPT);
    printf ("\n");
    printf (" %s { option } [ input_path [ list_path ] ]\n",
            M_PRGNAM);
    printf ("\n");
    printf ("where the following options are supported:\n");
	printf (" -ah=xxx\t\tuse value (in FL) as assumed height\n");
	printf (" -all\t\t\tlist all levels\n");
	printf (" -cat\t\t\tlist ASTERIX category\n");
	printf (" -cat=xxx\t\tonly this ASTERIX category to be listed\n");
	printf (" -categories\t\tprint list of supported ASTERIX categories\n");
	printf (" -f\t\t\tforced overwrite for list file\n");
	printf (" -fd\t\t\tchecking frame against data time\n");
	printf (" -fl=nn\t\t\tframe limit (only first nn frames are listed)\n");
	printf (" -formats\t\tprint list of recording and data formats\n");
	printf (" -gv\t\t\tlist ground vector (for radar and system tracks)\n");
	printf (" -help\t\t\tprint some help info (and abort)\n");
#if LISTER
	printf (" -hex\t\t\tlist hex dump\n");
#endif /* LISTER */
	printf (" -if=pathname\t\tpath name of input file\n");
#if LISTER
	printf (" -l=nn\t\t\tlist level (1/2=verbose, 3=one message per line)\n");
	printf (" -lf=pathname\t\tpath name of list file\n");
#endif /* LISTER */
#if USE_JSON
    printf (" -json-type=type\toutput type of json file to be written, possible types: none,test,print,text,\n"
            "\t\t\t\tcbor,msgpack,ubjson,zip-text,zip-cbor,zip-msgpack,zip-ubjson\n");
    printf (" -json-file=pathname\tpath name of json file to be written\n");
    printf (" -json-write-nulls\twrite null values in json output\n");
#endif
	printf (" -list_065\t\tlist ASTERIX category 065 messages\n");
	printf (" -list_dsi\t\tlist data source identifiers (SAC/SIC)\n");
	printf (" -list_gh\t\tlist geometric height\n");
	printf (" -list_lus\t\tlist last updating sensor (for system tracks)\n");
	printf (" -list_only_bg\t\tlist only system tracks from background service\n");
	printf (" -list_only_cy\t\tlist only system tracks from complementary service\n");
	printf (" -list_service_id\tlist service identification (for system tracks)\n");
	printf (" -ll=nn\t\t\tlength limit (only first nn bytes are listed)\n");
	printf (" -ml\t\t\thandle multiple lines (per sensor)\n");
	printf (" -mof\t\t\tlist mode of flight (for system tracks)\n");
	printf (" -nft\t\t\tdon't list frame time\n");
	printf (" -no_utc\t\tNo UTC time of day in list file\n");
	printf (" -progress\t\tshow some progress indication\n");
	printf (" -reftrj\t\tinput file holds reference trajectories\n");
	printf (" -soe\t\t\tstop on (ASTERIX) error\n");
	printf (" -sqn\t\t\twith sequence numbers\n");
	printf (" -srv_id=xxx\t\tfilter ARTAS output by service_identification\n");
	printf (" -ssta\t\t\tlist sensor status information\n");
	printf (" -start_date=yyyy-mm-dd\tstart date\n");
	printf (" -start_offset=nnn\tstart offset in input file\n");
	printf (" -subtypes\t\tprint list of supported format subtypes\n");
	printf (" -timebias=ttt\t\ttime bias (milliseconds)\n");
	printf (" -tn12\t\t\tsystem track number with 12 bits (legacy)\n");
	printf (" -utc\t\t\tUTC time of day in list file (default)\n");
	printf (" -wgs84\t\t\tlist WGS-84 position\n");
    printf ("\n");
	printf ("'input_path' and 'list_path' are the (local or full)\n");
	printf (" path names of the respective files.\n");
	printf ("\n");
    printf ("For comments or questions our e-mail address is: %s\n",
	        M_E_MAIL);
    printf ("Thank you for using this software.\n");

	return;
}

/*----------------------------------------------------------------------------*/
/* print_subtypes  -- Print list of supported format subtypes                 */
/*----------------------------------------------------------------------------*/

 static
 void print_subtypes (void)
{
	printf ("The '%s' utility at the moment supports the "
            "following format subtypes:\n", M_PRGNAM);
    printf ("\n");
	printf (" ASTERIX category versions:\n");
	printf ("  -vsn011=0.14      Category 011 edition 0.14 "
            "(October 2000)\n");
	printf ("  -vsn011=0.14Sensis ICD Inn Valley (Sensis - "
            "August 6, 2003)\n");
	printf ("  -vsn011=0.17      Category 011 edition 0.17 "
            "(December 2001)\n");
    printf ("\n");
	printf ("  -vsn020=1.0       Category 020 edition 1.0 "
            "(November 2005)\n");
	printf ("  -vsn020=1.2       Category 020 edition 1.2 "
            "(April 2007)\n");
	printf ("  -vsn020=1.5       Category 020 edition 1.5 "
            "(April 2008)\n");
    printf ("\n");
	printf ("  -vsn021=0.13      Category 021 edition 0.13 "
            "(June 2001)\n");
	printf ("  -vsn021=0.20      Category 021 edition 0.20 "
            "(December 2002)\n");
	printf ("  -vsn021=0.23      Category 021 edition 0.23 "
            "(November 2003)\n");
    printf ("\n");
	printf ("  -vsn023=1.0P      Category 023 edition 1.0P "
            "(August 2008)\n");
	printf ("  -vsn023=1.1       Category 023 edition 1.1 "
            "(September 2008)\n");
	printf ("  -vsn023=1.2       Category 023 edition 1.2 "
            "(March 2009)\n");
    printf ("\n");

	return;
}

/*----------------------------------------------------------------------------*/
/* main            -- The (main) program                                      */
/*----------------------------------------------------------------------------*/

 int main (int argc, char **argv)
{
	t_Bool aborted;
                   /* Program aborted */
	int arg;       /* Argument number */
	int fcnt;      /* Count for file names */
	int sys;       /* System return code */
	long t_diff;   /* Time difference; microseconds */
	struct timeval tv0;
                   /* Time at start */
	struct timeval tv1;
                   /* Time at end */

    printf ("*** " M_PROMPT "\t\t\t   %s ***\n", PACKAGE_VERSION.c_str());
	printf (M_COPYRT "\n");
	printf ("\n");
	printf (M_DISCL1 "\n");
	printf ("\n");
	printf (M_DISCL2 "\n");
	printf ("\n");
	printf (M_DISCL3 "\n");
	printf ("\n");

                   /* Get time at start: */
	sys = gettimeofday (&tv0, NULL);
	if (sys)
	{
		return -1;
	}

                   /* Initiate program call options: */
	init_options ();

				   /* Preset all public variables: */
	after_midnight = FALSE;
	current_line_number = -1;
	current_line_number_defined = FALSE;
	current_stns = 0;
	current_stns_defined = FALSE;
	current_stns_dsi = 0;
	exc_file = NULL;
	first_frame_time.present = FALSE;
	first_frame_time.value = 0.0;
	frame_date.year = 0;
	frame_date.month = 0;
	frame_date.day = 0;
	frame_date_present = FALSE;
	frame_time = 0.0;
	frame_time_present = FALSE;
	frames_count = 0;
	input_frames = 0;
	input_length = 0;
	input_offset = 0;
	input_rtm = 0;
	last_frame_time.present = FALSE;
	last_frame_time.value = 0.0;
	last_sacsic = 0;
	last_sacsic_available = FALSE;
	last_tod = 0;
	last_tod_available = FALSE;
#if LISTER
	list_file = NULL;
#endif /* LISTER */
	number_of_sensor_descriptions = 0;
	records_in_current_frame = 0;
	memset (&rsrv, 0, sizeof (rsrv));
	memset (&rtgt, 0, sizeof (rtgt));
	start_time = 0;
	start_time_available = FALSE;
	start_wall_time.year = 0;
	start_wall_time.month = 0;
	start_wall_time.day = 0;
	start_wall_time.hours = 0;
	start_wall_time.minutes = 0;
	start_wall_time.seconds = 0;
	start_wall_time.milliseconds = 0;
	memset (&step, 0, sizeof (t_Step));
	stop_time_available = FALSE;
	stop_wall_time.year = 0;
	stop_wall_time.month = 0;
	stop_wall_time.day = 0;
	stop_wall_time.hours = 0;
	stop_wall_time.minutes = 0;
	stop_wall_time.seconds = 0;
	stop_wall_time.milliseconds = 0;

                   /* Preset local variables: */
	aborted = FALSE;

#if CHKASS
                   /* Set label for longjmp() in case of
                      fatal error: */
	if (setjmp (on_fatal_error))
	{
		aborted = TRUE;
		goto term;
	}
#endif /* CHKASS */

				   /* Check if minimal program call: */
	if (argc < 2)
	{
		print_info ();
		goto term;
	}

				   /* Evaluate path names in program call options: */
	arg = 1;
	fcnt = 0;
	while (arg < argc)
	{
		if (argv[arg][0] != '-')
		{
			++ fcnt;
			if (fcnt == 1)
			{
				input_path = argv[arg];
				input_path_defined = TRUE;
			}
			else if (fcnt == 2)
			{
				list_path = argv[arg];
				list_path_defined = TRUE;
			}
		}

		++ arg;
	}

				   /* Evaluate program call options: */
	arg = 1;
	while (arg < argc)
	{
		if (argv[arg][0] == '-')
		{
			do_option (&argv[arg][1]);
		}

		++ arg;
	}

				   /* Check if help required: */
	if (argc < 2 || help_required)
	{
		print_info ();
		goto term;
	}

                   /* Check if list of ASTERIX categories required: */
	if (categories_required)
	{
		print_categories ();
		goto term;
	}

                   /* Check if formats list required: */
	if (formats_list_required)
	{
		print_formats ();
		goto term;
	}

                   /* Check if list of format subtypes required: */
	if (subtypes_required)
	{
		print_subtypes ();
		goto term;
	}

#if LISTER
	if (!list_hex_dump)
#endif /* LISTER */
	{
				   /* Check for input format: */
		if (!input_format_defined)
		{
			printf ("-> Using IOSS as default input format ...\n");
			input_format = e_input_format_ioss;
			input_format_defined = TRUE;
		}

				   /* Check for data format: */
		if (!data_format_defined)
		{
			printf ("-> Using ASTERIX as default data format ...\n");
			data_format = e_data_format_asterix;
			data_format_defined = TRUE;
		}
	}

				   /* Check for input path: */
	if (!input_path_defined)
	{
		fprintf (stderr, "E> No input file given.\n");
		printf ("-> Use '-help' for program use advice.\n");

		goto term;
	}

#if LISTER
				   /* Check for list path: */
	if (!list_path_defined)
	{
		list_file = stdout;
	}
	else
	{
		if (!forced_overwrite)
		{
			t_Bool terminate;

			terminate = FALSE;
			list_file = fopen (list_path, "r");
			if (list_file != NULL)
			{
				char ch;

				terminate = TRUE;

				printf ("-> List file '%s' already exists"
                        " - overwrite (y/n) ?\n", list_path);

				while (TRUE)
				{
					ch = fgetc (stdin);
					if (ch == '\n')
					{
						break;
					}
					if (ch == 'Y' || ch == 'y')
					{
						terminate = FALSE;
					}
				}

				(void) fclose (list_file);
				list_file = NULL;
			}
			if (terminate)
			{
				printf ("-> Existing list file must to be overwritten"
                        " - program aborted\n");
				goto term;
			}
		}

		list_file = fopen (list_path, "w");
		if (list_file == NULL)
		{
			printf ("E> Cannot open list file '%s'.\n", list_path);
			goto term;
		}
	}
#endif /* LISTER */

#if LISTER
				   /* List path name of input file: */
	if (list_file != stdout && list_level >= 0)
	{
		t_Date date;
		t_Bool date_present;
		int hour;
		int minute;
		time_t now;
		int second;
		struct tm *tmb;
		char tmp[81];

		(void) time (&now);
		tmb = gmtime (&now);

		if (tmb != NULL)
		{
			date.year = 1900 + tmb->tm_year;
			date.month = 1 + tmb->tm_mon;
			date.day = tmb->tm_mday;
			date_present = TRUE;

			hour = tmb->tm_hour;
			minute = tmb->tm_min;
			second = tmb->tm_sec;
		}

		memset (tmp, 0, 81);
		if (date_present)
		{
			sprintf (tmp, M_FMT_DATE " %02d:%02d:%02d",
                          date.year, date.month, date.day,
                          hour, minute, second);
		}

		list_text (-1, "; list of file '%s':\n", input_path);
        list_text (-1, "; printed %s by %s (%s)\n", tmp, M_PRGNAM, PACKAGE_VERSION);
		list_text (-1, "\n");
	}
#endif /* LISTER */

#if USE_JSON
    switch (json_output_type)
    {
    case JSON_NONE:
    case JSON_TEST:
    case JSON_PRINT:
        break;
    case JSON_TEXT:
    case JSON_CBOR:
    case JSON_MESSAGE_PACK:
    case JSON_UBJSON:
    case JSON_ZIP_TEXT:
    case JSON_ZIP_CBOR:
    case JSON_ZIP_MESSAGE_PACK:
    case JSON_ZIP_UBJSON:
        if (!json_path.size())
        {
            printf ("-> ERROR: JSON output type requires file to be set, disabling output\n");
            json_output_type = JSON_NONE;
        }
        break;
    default:
        printf ("-> ERROR: Unhandled JSON output type '%d' during main\n", json_output_type);
        json_output_type = JSON_NONE;
        break;
    }

    Assert (json_writer == nullptr, "JSON writer already exists");
    json_writer = new JSONWriter {json_output_type, json_path};
#endif

                   /* Initiate message processing: */
    process_init ();

				   /* Process input file: */
	process_input ();

	term:          /* Terminate program */

	if (aborted)
	{
		printf ("-> Program aborted\n");
		goto done;
	}

				   /* Close input file: */
	if (input_file != NULL)
	{
		fclose (input_file);

		input_file = NULL;
	}

#if LISTER
				   /* Close list file: */
	if (list_file != NULL)
	{
                   /* Terminate the lister: */
		term_lister ();

		if (list_file != stdout && list_level > 0)
		{
			list_text (-1, "\n");
			list_text (-1, "; end of listing\n");

			fclose (list_file);
		}

		list_file = NULL;
	}
#endif /* LISTER */

#if USE_JSON
    if (json_writer != nullptr)
    {
        delete json_writer;
        json_writer = nullptr;
    }
#endif

                   /* Terminate message processing: */
	process_term ();

	done:          /* We are done */

                   /* Get time at end: */
	sys = gettimeofday (&tv1, NULL);
	if (sys)
	{
		return -1;
	}

	if (tv1.tv_sec == tv0.tv_sec)
	{
		t_diff = tv1.tv_usec - tv0.tv_usec;
	}
	else
	{
		if (tv1.tv_sec > tv0.tv_sec)
		{
			t_diff = tv1.tv_sec - tv0.tv_sec;
			t_diff *= 1000000;
			t_diff += (tv1.tv_usec - tv0.tv_usec);
		}
		else
		{
			t_diff = 0;
		}
	}

	if (input_offset > 0 && t_diff > 0)
	{
		t_Real dt_in_secs;
		t_Ui32 throughput;

		dt_in_secs = 1.0e-6 * t_diff;

		throughput = (t_Ui32) (0.5 + input_offset / dt_in_secs);

		printf ("\n");
		printf ("-> Processed " M_FMT_UI32 " byte", input_offset);
		if (input_offset > 1)
		{
			printf ("s");
		}
		printf (" in %.3f seconds (about %.3f MB/sec",
                dt_in_secs, throughput / (1024.0 * 1024.0));

		throughput = (t_Ui32) (0.5 + frames_count / dt_in_secs);

		printf ("; " M_FMT_UI32 " frames/sec", throughput);
		printf (")\n");
	}

	printf ("\n");
	printf ("*** End of " M_PROMPT " ***\n");

	return 0;
}
/* end-of-file */
