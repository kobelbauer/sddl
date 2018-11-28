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
/* File:           src/options.cpp                                            */
/* Contents:       Handling program call options                              */
/* Author(s):      kb                                                         */
/* Last change:    2018-04-27                                                 */
/*----------------------------------------------------------------------------*/

#include "basics.h"
                   /* Basic system definitions */
#include "common.h"
                   /* Common declarations and definitions */

#include <ctype.h>
#include <malloc.h>
#include <stddef.h>
#include <stdio.h>
#include <string.h>

                   /* Global variables: */
                   /* ----------------- */

t_Real assumed_height;
                   /* Assumed height; metres */
t_Bool assumed_height_defined;
                   /* Assumed height defined */
t_Bool big_endian;
                   /* Sequence of records with big-endian length field */
t_Bool cat_selection[256];
                   /* Selection list for ASTERIX categories */
t_Bool cat_selection_defined;
                   /* Selection for ASTERIX categories defined */
t_Bool cat_selection_initiated;
                   /* Selection for ASTERIX categories initiated */
t_Bool categories_required;
                   /* List of supported ASTERIX categories required */
t_Bool checking_fd;
                   /* Checking frame against data times */
t_Data_Format data_format;
				   /* Data format */
t_Bool data_format_defined;
				   /* Data format defined flag */
t_Bool excel_output;
                   /* Special output for Microsoft Excel */
t_Bool forced_overwrite;
                   /* Forced overwrite (on list file) */
t_Bool formats_list_required;
				   /* List of input and data formats required flag */
t_Ui32 frames_limit;
				   /* Input frames limit */
t_Bool frames_limit_defined;
                   /* Input frames limit defined */
t_Bool handle_multiple_lines;
                   /* Handle multiple lines */
t_Bool help_required;
				   /* Help for program use required flag */
t_Input_Format input_format;
				   /* Input format */
t_Bool input_format_defined;
				   /* Input format defined flag */
char *input_path;
				   /* Pointer to path name of input file */
t_Bool input_path_defined;
				   /* Path name of input file defined flag */
t_Bool input_reftrj;
                   /* Input file holds reference trajectories */
t_Ui32 length_limit;
				   /* Input length limit; bytes */
t_Bool length_limit_defined;
                   /* Input length limit defined */
#if LISTER
t_Bool list_all_levels;
                   /* List all levels */
t_Bool list_astx_065;
                   /* List ASTERIX category 065 */
t_Bool list_cat;
                   /* List ASTERIX category */
t_Bool list_dsis;
                   /* List data source identifications */
t_Bool list_frame_numbers;
                   /* List frame numbers */
t_Bool list_frame_time;
                   /* List frame time */
t_Bool list_geometric_height;
                   /* List geometric height */
t_Bool list_ground_vector;
                   /* List ground vector (for radar and system tracks) */
t_Bool list_hex_dump;
                   /* List hex dump */
#endif /* LISTER */
t_Bool list_large_height_difference;
                   /* List large height difference between data items
                      I030/150 and I030/160 in ARTAS system tracks */
t_Bool list_last_updating_sensor;
                   /* List last updating sensor (for system tracks) */
int list_level;
				   /* Level of listing */
t_Bool list_mode_c_height;
                   /* List SSR mode C height */
t_Bool list_mode_s_address;
                   /* List SSR mode S address */
t_Bool list_mof;
                   /* List mode of flight */
t_Bool list_no_mode_s;
                   /* List without SSR mode S components */
t_Bool list_only_background_service;
                   /* List only system track messages from background service */
t_Bool list_only_complementary_service;
                   /* List only system track messages from complementary service
                      (and mark as such) */
const char *list_path;
                   /* Pointer to path name of list file */
t_Bool list_path_defined;
                   /* Path name of list file defined flag */
t_Bool list_received_power;
                   /* List received power */
t_Bool list_sensor_name;
                   /* List sensor name */
t_Bool list_sensor_number;
                   /* List sensor number */
t_Bool list_service_id;
                   /* List service identification (for system tracks) */
t_Bool list_ssta_info;
                   /* List sensor status information */
t_Bool list_ssta_info_for_excel;
                   /* List sensor status information (for Excel) */
t_Bool list_time_as_utc_text;
                   /* List time (of day) as UTC text */
t_Bool list_time_of_day;
                   /* List time of day */
t_Bool list_wgs84;
                   /* List WGS-84 position */
#if USE_JSON
JSON_OUTPUT_TYPE json_output_type {JSON_NONE};

std::map<std::string, JSON_OUTPUT_TYPE> json_output_type_map {
    {"none", JSON_NONE},
    {"test", JSON_TEST},
    {"print", JSON_PRINT},
    {"text", JSON_TEXT},
    {"cbor", JSON_CBOR},
    {"msgpack", JSON_MESSAGE_PACK},
    {"ubjson", JSON_UBJSON},
    {"zip-text", JSON_ZIP_TEXT},
    {"zip-cbor", JSON_ZIP_CBOR},
    {"zip-msgpack", JSON_MESSAGE_PACK},
    {"zip-ubjson", JSON_ZIP_MESSAGE_PACK}
};

std::string json_path;
bool write_json_nulls {false};

JSONWriter* json_writer {nullptr};
#endif

t_Byte service_identification;
                   /* Service identification (for ARTAS output) */
t_Bool service_identification_defined;
                   /* Service identification defined */
t_Bool show_progress_indication;
                   /* Show progress indication */
t_Date start_date;
                   /* Start date */
t_Bool start_date_defined;
                   /* Start date defined */
t_Ui32 start_offset;
                   /* Input offset for start of listing */
t_Bool start_offset_defined;
                   /* Input offset for start of listing defined */
t_Time start_rtm;
                   /* Relative time for start of listing; milliseconds */
t_Bool stop_on_error;
                   /* Stop on (ASTERIX) error */
t_Bool subtypes_required;
                   /* List of supported format subtypes required */
t_Secs time_bias;
                   /* Time bias; seconds */
t_Bool time_bias_defined;
                   /* Time bias defined */
t_Bool track_data_from_dec;
                   /* JDiff (track_data) file from DEC */
int track_number_bits;
                   /* Number of bits in a track number */
t_Bool use_mmcc;
                   /* Use minimum mode C condition in ZSF processing */
t_Bool with_sequence_number;
                   /* Each RFF frame holds a 4 bytes sequence number */

                   /* Local functions: */
                   /* ---------------- */

static void set_data_format (const char *option, t_Data_Format format);
                   /* Set data format */
static void set_input_format (const char *option, t_Input_Format format);
                   /* Set input format */
static void select_cat (t_Byte category);
                   /* Select ASTERIX category */

/*----------------------------------------------------------------------------*/
/* do_option       -- Process a program call option                           */
/*----------------------------------------------------------------------------*/

 void do_option (char *option_ptr)
{
	char fct;      /* Size factor */
	char *fp;      /* Pointer to field */
	size_t len;    /* Auxiliary */
	t_Retc lrc;    /* Local return code */
	int n;         /* Auxiliary */
	t_Time t;      /* Auxiliary */
	unsigned int u;
                   /* Auxiliary */
	t_Ui32 u32;    /* Auxiliary */
	char *vp;      /* Pointer to value */

				   /* Check parameter: */
	Assert (option_ptr != NULL, "Invalid parameter");

				   /* Skip empty option: */
	if (*option_ptr == '\0')
	{
		goto done;
	}

#if LISTER
                   /* Handle 'all' option: */
	if (strcmp (option_ptr, "all") == 0)
	{
                   /* List all levels */

		printf ("-> List all levels\n");

		list_all_levels = TRUE;
		goto done;
	}
#endif /* LISTER */

#if USE_JSON
    {
        std::string tmp {option_ptr};

        if (tmp.find("json-file=") != std::string::npos)
        {
            if (tmp.size() == 10)
            {
                printf ("-> Export JSON to filename failed since no filename was supplied\n");
                goto done;
            }

            std::string filename = tmp.substr (10);
            printf ("-> Export JSON to filename '%s'\n", filename.c_str());
            json_path = filename;

            if (json_output_type == JSON_NONE) // not yet set
                json_output_type = JSON_TEXT; // default

            goto done;
        }
    }

    {
        std::string tmp {option_ptr};

        if (tmp.find("json-type=") != std::string::npos)
        {
            if (tmp.size() == 10)
            {
                printf ("-> Output JSON type failed since no type was supplied\n");
                goto done;
            }

            std::string type = tmp.substr (10);

            if (json_output_type_map.count(type) == 0)
            {
                printf ("-> Output JSON type failed since unknown type '%s' was supplied\n", type.c_str());
                goto done;
            }

            printf ("-> Output JSON type '%s'\n", type.c_str());
            json_output_type = json_output_type_map.at(type);

            goto done;
        }
    }

    {
        std::string tmp {option_ptr};

        if (tmp.find("json-write-nulls") != std::string::npos)
        {
            write_json_nulls = true;
            printf ("-> Output JSON null values\n");

            goto done;
        }
    }
#endif

                   /* Handle 'amplitude' option: */
	if (strcmp (option_ptr, "amplitude") == 0)
	{
                   /* List received power */

		printf ("-> List received power\n");

		list_received_power = TRUE;
		goto done;
	}

                   /* Handle 'asf' option: */
	if (strcmp (option_ptr, "asf") == 0)
	{
                   /* ASTERIX in Final Format */

		set_input_format (option_ptr, e_input_format_ioss);
		set_data_format (option_ptr, e_data_format_asterix);
		goto done;
	}

				   /* Handle 'asterix' option: */
	if (strcmp (option_ptr, "asterix") == 0 ||
        strcmp (option_ptr, "asx") == 0)
	{
				   /* ASTERIX data format */

		set_data_format (option_ptr, e_data_format_asterix);
		goto done;
	}

                   /* Handle 'be' option: */
	if (strcmp (option_ptr, "be") == 0)
	{
                   /* Big-endian length field */

		printf ("-> Big-endian length field\n");

		big_endian = TRUE;
		goto done;
	}

#if LISTER
                   /* Handle 'cat' option: */
	if (strcmp (option_ptr, "cat") == 0)
	{
                   /* List ASTERIX category */

		printf ("-> List ASTERIX category\n");

		list_cat = TRUE;
		goto done;
	}
#endif /* LISTER */

                   /* Handle 'categories' option: */
	if (strcmp (option_ptr, "categories") == 0)
	{
                   /* Print list of supported ASTERIX categories */

		categories_required = TRUE;
		goto done;
	}

				   /* Handle 'excel' option: */
	if (strcmp (option_ptr, "excel") == 0)
	{
				   /* Special output for Microsoft (TM) Excel */

				   /* Set the respective flag: */
		excel_output = TRUE;
		goto done;
	}

				   /* Handle 'f' option: */
	if (strcmp (option_ptr, "f") == 0 ||
        strcmp (option_ptr, "-forced") == 0)
	{
				   /* Forced overwrite (on list file) */

				   /* Set the respective flag: */
		forced_overwrite = TRUE;
		goto done;
	}

				   /* Handle 'fd' option: */
	if (strcmp (option_ptr, "fd") == 0)
	{
				   /* Checking frame against data time */

				   /* Set the respective flag: */
		checking_fd = TRUE;
		goto done;
	}

				   /* Handle 'formats' option: */
	if (strcmp (option_ptr, "formats") == 0)
	{
		formats_list_required = TRUE;
		goto done;
	}

#if LISTER
				   /* Handle 'gv' option: */
	if (strcmp (option_ptr, "gv") == 0)
	{
				   /* List ground vector (for radar and system tracks) */

		printf ("-> List ground vector (for radar and system tracks)\n");

				   /* Set the respective flag: */
		list_ground_vector = TRUE;
		goto done;
	}
#endif /* LISTER */

                   /* Handle 'help' option: */
	if (strcmp (option_ptr, "help") == 0)
	{
		help_required = TRUE;
		goto done;
	}

#if LISTER
                   /* Handle 'hex' option: */
	if (strcmp (option_ptr, "hex") == 0)
	{
                   /* List hex dump */

		printf ("-> List hex dump ...\n");

                   /* Set the respective flag: */
		list_hex_dump = TRUE;
		goto done;
	}
#endif /* LISTER */

                   /* Handle 'ioss' option: */
	if (strcmp (option_ptr, "ioss") == 0)
	{
                   /* SASS-C IOSS format */

		set_input_format (option_ptr, e_input_format_ioss);
		goto done;
	}

#if LISTER
                   /* Handle 'list_065' option: */
	if (strcmp (option_ptr, "list_065") == 0)
	{
                   /* List ASTERIX category 065 */

		printf ("-> List ASTERIX category 065\n");

		list_astx_065 = TRUE;
		goto done;
	}
#endif /* LISTER */

#if LISTER
                   /* Handle 'list_dsi' option: */
	if (strcmp (option_ptr, "list_dsi") == 0)
	{
                   /* List data source identifications */

		printf ("-> List data source identifications\n");

		list_dsis = TRUE;
		goto done;
	}
#endif /* LISTER */

#if LISTER
                   /* Handle 'lfn' option: */
	if (strcmp (option_ptr, "lfn") == 0 ||
        strcmp (option_ptr, "list_frame_numbers") == 0)
	{
                   /* List frame numbers */

		printf ("-> List frame numbers\n");

		list_frame_numbers = TRUE;
		goto done;
	}
#endif /* LISTER */

#if LISTER
				   /* Handle 'list_gh' option: */
	if (strcmp (option_ptr, "list_gh") == 0)
	{
				   /* List geometric height */

		printf ("-> List geometric height\n");

				   /* Set the respective flag: */
		list_geometric_height = TRUE;
		goto done;
	}
#endif /* LISTER */

                   /* Handle 'list_lhd' option: */
	if (strcmp (option_ptr, "list_lhd") == 0)
	{
                   /* List large height difference */

		printf ("-> List large height difference\n");

		list_large_height_difference = TRUE;
		goto done;
	}

                   /* Handle 'list_lus' option: */
	if (strcmp (option_ptr, "list_lus") == 0)
	{
                   /* List last updating sensor */

		printf ("-> List last updating sensor\n");

		list_last_updating_sensor = TRUE;
		goto done;
	}

                   /* Handle 'list_only_bg' option: */
	if (strcmp (option_ptr, "list_only_bg") == 0)
	{
                   /* List only system tracks from background service: */

		printf ("-> List only system tracks from background service\n");

		list_only_background_service = TRUE;
		goto done;
	}

                   /* Handle 'list_only_cy' option: */
	if (strcmp (option_ptr, "list_only_cy") == 0)
	{
                   /* List only system tracks from background service: */

		printf ("-> List only system tracks from complementary service\n");

		list_only_complementary_service = TRUE;
		goto done;
	}

                   /* Handle 'list_service_id' option: */
	if (strcmp (option_ptr, "list_service_id") == 0)
	{
                   /* List service identification (for system tracks) */

		printf ("-> List service identification (for system tracks)\n");

		list_service_id = TRUE;
		goto done;
	}

                   /* Handle 'ml' option: */
	if (strcmp (option_ptr, "ml") == 0)
	{
                   /* Handle multiple lines (per sensor) */

		printf ("-> Handle multiple lines (per sensor)\n");

		handle_multiple_lines = TRUE;
		goto done;
	}

                   /* Handle 'mof' option: */
	if (strcmp (option_ptr, "mof") == 0)
	{
                   /* List mode of flight */

		printf ("-> List mode of flight\n");

		list_mof = TRUE;
		goto done;
	}

				   /* Handle 'net' option: */
	if (strcmp (option_ptr, "net") == 0 ||
        strcmp (option_ptr, "netto") == 0)
	{
				   /* Binary "netto" recording */

		set_input_format (option_ptr, e_input_format_netto);
		goto done;
	}

#if LISTER
                   /* Handle 'no_frame_time' option: */
	if (strcmp (option_ptr, "nft") == 0 ||
        strcmp (option_ptr, "no_frame_time") == 0)
	{
                   /* List frame time */

		printf ("-> Do not list frame time\n");

		list_frame_time = FALSE;
		goto done;
	}
#endif /* LISTER */

				   /* Handle 'no_mds' option: */
	if (strcmp (option_ptr, "no_mds") == 0)
	{
				   /* Do not list SSR mode S components */

				   /* Set the respective flag: */
		list_no_mode_s = TRUE;
		goto done;
	}

				   /* Handle 'no_msa' option: */
	if (strcmp (option_ptr, "no_msa") == 0)
	{
				   /* Do not list SSR mode S address */

		list_mode_s_address = FALSE;
		goto done;
	}

				   /* Handle 'no_snm' option: */
	if (strcmp (option_ptr, "no_snm") == 0)
	{
				   /* Do not list sensor name */

		list_sensor_name = FALSE;
		goto done;
	}

				   /* Handle 'no_sno' option: */
	if (strcmp (option_ptr, "no_sno") == 0)
	{
				   /* Do not list sensor number */

		list_sensor_number = FALSE;
		goto done;
	}

				   /* Handle 'no_utc' option: */
	if (strcmp (option_ptr, "no_utc") == 0)
	{
				   /* List time in flat seconds */

		list_time_as_utc_text = FALSE;
		goto done;
	}

				   /* Handle 'progress' option: */
	if (strcmp (option_ptr, "progress") == 0)
	{
				   /* Showing some progress indication: */

		printf ("-> Show progress indication\n");

		show_progress_indication = TRUE;
		goto done;
	}

                   /* Handle 'rec' option: */
	if (strcmp (option_ptr, "rec") == 0)
	{
				   /* Sequence of records */

		set_input_format (option_ptr, e_input_format_sequence_of_records);
		goto done;
	}

				   /* Handle 'reftrj' option: */
	if (strcmp (option_ptr, "reftrj") == 0)
	{
				   /* Input file holds reference trajectories */

		input_reftrj = TRUE;
		goto done;
	}

				   /* Handle 'rff' option: */
	if (strcmp (option_ptr, "rff") == 0)
	{
				   /* RFF recording */

		set_input_format (option_ptr, e_input_format_rff);
		goto done;
	}

                   /* Handle 'soe' option: */
	if (strcmp (option_ptr, "soe") == 0)
	{
                   /* Stop on (ASTERIX) error */

		printf ("-> Stop on (ASTERIX) error\n");

		stop_on_error = TRUE;
		goto done;
	}

                   /* Handle 'sqn' option: */
	if (strcmp (option_ptr, "sqn") == 0)
	{
                   /* Each RFF frame holds a sequence number */

		with_sequence_number = TRUE;
		goto done;
	}

                   /* Handle 'ssta' option: */
	if (strcmp (option_ptr, "ssta") == 0)
	{
                   /* List sensor status information */

		printf ("-> List sensor status information\n");

		list_ssta_info = TRUE;
		goto done;
	}

                   /* Handle 'ssta_excel' option: */
	if (strcmp (option_ptr, "ssta_excel") == 0)
	{
                   /* List sensor status information */

		printf ("-> List sensor status information\n");

		list_ssta_info = TRUE;
		list_ssta_info_for_excel = TRUE;
		goto done;
	}

                   /* Handle 'subtypes' option: */
	if (strcmp (option_ptr, "subtypes") == 0)
	{
                   /* Print list of supported format subtypes */

		subtypes_required = TRUE;
		goto done;
	}

                   /* Handle 'tn12' option: */
	if (strcmp (option_ptr, "tn12") == 0)
	{
                   /* System track number with 12 bits (legacy) */

		printf ("-> System track number with 12 bits (legacy)\n");

		track_number_bits = 12;
		goto done;
	}

                   /* Handle 'use_mmcc' option: */
	if (strcmp (option_ptr, "use_mmcc") == 0)
	{
                   /* Use minimum mode C condition for ZSF processing */

		printf ("-> Use minimum mode C condition for ZSF processing\n");

		use_mmcc = TRUE;
		goto done;
	}

				   /* Handle 'utc' option: */
	if (strcmp (option_ptr, "utc") == 0)
	{
				   /* List time as UTC text */

		list_time_as_utc_text = TRUE;
		goto done;
	}

                   /* Handle 'wgs84' option: */
	if (strcmp (option_ptr, "wgs84") == 0)
	{
                   /* List WGS-84 position */

		printf ("-> List WGS-84 position\n");

		list_wgs84 = TRUE;
		goto done;
	}

				   /* Handle 'zzz' option: */
	if (strcmp (option_ptr, "zzz") == 0)
	{
				   /* Unknown data format - ignore */

		set_data_format (option_ptr, e_data_format_zzz);
		goto done;
	}

                   /* Check if a clause and isolate field and value: */
	fp = option_ptr;
    vp = strchr (option_ptr, '=');

                   /* Check value string: */
	if (vp == NULL)
	{
		printf ("t> Unknown option '%s' - skipped\n", fp);
		goto done;
	}

                   /* Zero-terminate the value string: */
   	*vp = '\0';
	++ vp;

				   /* Check for 'ah=xxx' option: */
	if (strcmp (fp, "ah") == 0)
	{
		t_Real f;

		n = sscanf (vp, "%lf", &f);
		if (n == 1)
		{
			printf ("-> Assumed height set to %.2f FL\n", f);

			assumed_height = M_FL2MTR * f;
			assumed_height_defined = TRUE;
			goto done;
		}
	}

                   /* Check for 'cat=xxx' option: */
	if (strcmp (fp, "cat") == 0)
	{
                   /* List particular ASTERIX category */

		n = sscanf (vp, "%u", &u);
		if (n == 1 && 0 <= u && u <= 255)
		{
			printf ("-> ASTERIX category %u selected\n", u);

			select_cat (u);
			goto done;
		}
	}

				   /* Check for 'fl=nn' option: */
	if (strcmp (fp, "fl") == 0)
	{
		len = strlen (vp);
		fct = vp[len - 1];
		if (fct == 'K' || fct == 'M')
		{
			vp[len - 1] = '\0';
		}

		n = sscanf (vp, M_FMT_UI32, &u32);
		if (n == 1)
		{
			if (fct == 'K')
			{
				u32 *= (t_Ui32) 1024;
			}
			else if (fct == 'M')
			{
				u32 *= (t_Ui32) 1048576;
			}

			printf ("-> Frames limit set to " M_FMT_UI32 " frame(s)\n", u32);

			frames_limit = u32;
			frames_limit_defined = TRUE;
			goto done;
		}
	}

#if LISTER
				   /* Check for 'l=nn' option: */
	if (strcmp (fp, "l") == 0)
	{
		if (strcmp (vp, "all") == 0)
		{
			printf ("-> List all levels\n");

			list_all_levels = TRUE;
			goto done;
		}

		n = sscanf (vp, M_FMT_UI32, &u32);
		if (n == 1)
		{
			printf ("-> List level set to " M_FMT_UI32 "\n", u32);

			list_level = u32;
			goto done;
		}
	}
#endif /* LISTER */

				   /* Check for 'll=nn' option: */
	if (strcmp (fp, "ll") == 0)
	{
		len = strlen (vp);
		fct = vp[len - 1];
		if (fct == 'K' || fct == 'M')
		{
			vp[len - 1] = '\0';
		}

		n = sscanf (vp, M_FMT_UI32, &u32);
		if (n == 1)
		{
			if (fct == 'K')
			{
				u32 *= (t_Ui32) 1024;
			}
			else if (fct == 'M')
			{
				u32 *= (t_Ui32) 1048576;
			}

			printf ("-> Length limit set to " M_FMT_UI32 " byte(s)\n", u32);

			length_limit = u32;
			length_limit_defined = TRUE;
			goto done;
		}
	}

				   /* Check for 'so=nn' option: */
	if (strcmp (fp, "so") == 0)
	{
		len = strlen (vp);
		fct = vp[len - 1];
		if (fct == 'K' || fct == 'M')
		{
			vp[len - 1] = '\0';
		}

		n = sscanf (vp, M_FMT_UI32, &u32);
		if (n == 1)
		{
			if (fct == 'K')
			{
				u32 *= (t_Ui32) 1024;
			}
			else if (fct == 'M')
			{
				u32 *= (t_Ui32) 1048576;
			}

			printf ("-> Start offset set to " M_FMT_UI32 " byte(s)\n", u32);

			start_offset = u32;
			start_offset_defined = TRUE;
			goto done;
		}
	}

				   /* Check for 'srv_id=xxx' option: */
	if (strcmp (fp, "srv_id") == 0)
	{
		n = sscanf (vp, "%u", &u);
		if (n == 1)
		{
			if (0 < u && u < 256)
			{
				printf ("-> Filter ARTAS output by"
                        " service_identification %u\n", u);

				service_identification = (t_Byte) u;
				service_identification_defined = TRUE;
				goto done;
			}
		}
	}

				   /* Check for 'st=nn' option: */
	if (strcmp (fp, "st") == 0)
	{
		n = sscanf (vp, M_FMT_UI32, &u32);
		if (n == 1)
		{
			printf ("-> Start relative time set to " M_FMT_UI32
                    " sec(s)\n", u32);

			start_rtm = 1000 * u32;
			goto done;
		}
	}

                   /* Check for 'start_date=yyyy-mm-dd' option: */
	if (strcmp (fp, "start_date") == 0)
	{
		t_Ui16 y, m, d;

		n = sscanf (vp, "%hu-%hu-%hu", &y, &m, &d);
		if (n == 3)
		{
			if (check_date (y, m, d))
			{
				printf ("-> Start date set to " M_FMT_DATE "\n", y, m, d);

				start_date.year = y;
				start_date.month = m;
				start_date.day = d;
				start_date_defined = TRUE;

				goto done;
			}
		}
	}

				   /* Check for 'start_offset=nn' option: */
	if (strcmp (fp, "start_offset") == 0)
	{
		len = strlen (vp);
		fct = vp[len - 1];
		if (fct == 'K' || fct == 'M')
		{
			vp[len - 1] = '\0';
		}

		n = sscanf (vp, M_FMT_UI32, &u32);
		if (n == 1)
		{
			if (fct == 'K')
			{
				u32 *= (t_Ui32) 1024;
			}
			else if (fct == 'M')
			{
				u32 *= (t_Ui32) 1048576;
			}

			printf ("-> Start offset set to " M_FMT_UI32 " byte(s)\n", u32);

			start_offset = u32;
			start_offset_defined = TRUE;
			goto done;
		}
	}

				   /* Check for 'timebias=ttt' option: */
	if (strcmp (fp, "timebias") == 0)
	{
		n = sscanf (vp, "%ld", &t);
		if (n == 1)
		{
			if (time_bias_defined)
			{
				fprintf (stderr, "W> Time bias defined twice\n");
				goto done;
			}

			printf ("-> Time bias set to %ld msec(s)\n", t);

			time_bias = 0.001 * t;
			time_bias_defined = TRUE;
			goto done;
		}
	}

                   /* Check for 'vsn010=xxx' option: */
	if (strcmp (fp, "vsn010") == 0)
	{
		lrc = set_vsn010 (vp);
		if (lrc == RC_OKAY)
		{
			printf ("-> Reference version for ASTERIX category 010"
                    " set to \"%s\"\n", vp);

			goto done;
		}
	}

                   /* Check for 'vsn011=xxx' option: */
	if (strcmp (fp, "vsn011") == 0)
	{
		lrc = set_vsn011 (vp);
		if (lrc == RC_OKAY)
		{
			printf ("-> Reference version for ASTERIX category 011"
                    " set to \"%s\"\n", vp);

			goto done;
		}
	}

                   /* Check for 'vsn020=xxx' option: */
	if (strcmp (fp, "vsn020") == 0)
	{
		lrc = set_vsn020 (vp);
		if (lrc == RC_OKAY)
		{
			printf ("-> Reference version for ASTERIX category 020"
                    " set to \"%s\"\n", vp);

			goto done;
		}
	}

                   /* Check for 'vsn021=xxx' option: */
	if (strcmp (fp, "vsn021") == 0)
	{
		lrc = set_vsn021 (vp);
		if (lrc == RC_OKAY)
		{
			printf ("-> Reference version for ASTERIX category 021"
                    " set to \"%s\"\n", vp);

			goto done;
		}
	}

                   /* Check for 'vsn023=xxx' option: */
	if (strcmp (fp, "vsn023") == 0)
	{
		lrc = set_vsn023 (vp);
		if (lrc == RC_OKAY)
		{
			printf ("-> Reference version for ASTERIX category 023"
                    " set to \"%s\"\n", vp);

			goto done;
		}
	}

                   /* Check for 'vsn048=xxx' option: */
	if (strcmp (fp, "vsn048") == 0)
	{
		lrc = set_vsn048 (vp);
		if (lrc == RC_OKAY)
		{
			printf ("-> Reference version for ASTERIX category 048"
                    " set to \"%s\"\n", vp);

			goto done;
		}
	}

                   /* Check for 'vsn065=xxx' option: */
	if (strcmp (fp, "vsn065") == 0)
	{
		lrc = set_vsn065 (vp);
		if (lrc == RC_OKAY)
		{
			printf ("-> Reference version for ASTERIX category 065"
                    " set to \"%s\"\n", vp);

			goto done;
		}
	}

	printf ("t> Option '%s' ignored\n", option_ptr);

	done:          /* We are done */
	return;
}

/*----------------------------------------------------------------------------*/
/* init_options    -- Initiate program call options                           */
/*----------------------------------------------------------------------------*/

 void init_options (void)
{
	int ix;        /* Auxiliary index */

                   /* Preset all variables for program call options: */
	assumed_height = 0.0;
	assumed_height_defined = FALSE;
	big_endian = FALSE;
	for (ix = 0; ix < 256; ix ++)
	{
		cat_selection[ix] = FALSE;
	}
	cat_selection_defined = FALSE;
	cat_selection_initiated = FALSE;
	categories_required = FALSE;
	checking_fd = FALSE;
	data_format = e_data_format_undefined;
	data_format_defined = FALSE;
	excel_output = FALSE;
	forced_overwrite = FALSE;
	formats_list_required = FALSE;
	frames_limit = 0;
	frames_limit_defined = FALSE;
	handle_multiple_lines = FALSE;
	help_required = FALSE;
	input_format = e_input_format_undefined;
	input_format_defined = FALSE;
	input_path = NULL;
	input_path_defined = FALSE;
	input_reftrj = FALSE;
	length_limit = 0;
	length_limit_defined = FALSE;
#if LISTER
	list_all_levels = FALSE;
	list_astx_065 = FALSE;
	list_cat = FALSE;
	list_dsis = TRUE;
	list_frame_numbers = FALSE;
	list_frame_time = TRUE;
	list_geometric_height = FALSE;
	list_ground_vector = FALSE;
	list_hex_dump = FALSE;
#endif /* LISTER */
	list_large_height_difference = FALSE;
	list_last_updating_sensor = FALSE;
	list_level = 3;
	list_mode_c_height = TRUE;
	list_mode_s_address = TRUE;
	list_mof = FALSE;
	list_no_mode_s = FALSE;
	list_only_background_service = FALSE;
	list_only_complementary_service = FALSE;
	list_path = NULL;
	list_path_defined = FALSE;
	list_received_power = FALSE;
	list_sensor_name = TRUE;
	list_sensor_number = TRUE;
	list_service_id = FALSE;
	list_ssta_info = FALSE;
	list_ssta_info_for_excel = FALSE;
	list_time_as_utc_text = TRUE;
	list_time_of_day = TRUE;
	list_wgs84 = FALSE;
	service_identification = 0;
	service_identification_defined = FALSE;
	show_progress_indication = FALSE;
	start_date.year = 0;
	start_date.month = 0;
	start_date.day = 0;
	start_date_defined = FALSE;
	start_offset = 0;
	start_offset_defined = TRUE;
	start_rtm = 0;
	stop_on_error = FALSE;
	subtypes_required = FALSE;
	time_bias = 0.0;
	time_bias_defined = FALSE;
	track_data_from_dec = FALSE;
	track_number_bits = 16;
	use_mmcc = FALSE;
	with_sequence_number = FALSE;

	return;
}

/*----------------------------------------------------------------------------*/
/* select_cat      -- Select ASTERIX category                                 */
/*----------------------------------------------------------------------------*/

 static
 void select_cat (t_Byte category)
{
	if (!cat_selection_initiated)
	{
		int ix;

		for (ix = 0; ix < 256; ix ++)
		{
			cat_selection[ix] = FALSE;
		}

		cat_selection_initiated = TRUE;
	}

	cat_selection[category] = TRUE;

	cat_selection_defined = TRUE;

	return;
}

/*----------------------------------------------------------------------------*/
/* set_data_format -- Set data format                                         */
/*----------------------------------------------------------------------------*/

 static
 void set_data_format (const char *option, t_Data_Format format)
{
                   /* Check parameters: */
	Assert (option != NULL, "Invalid parameter");
	Assert (e_data_format_undefined < format && format < e_data_format_sentinel,
            "Invalid parameter");

				   /* Is the data format already defined ? */
	if (data_format_defined)
	{
				   /* Is it defined differently ? */
		if (data_format != format)
		{
			fprintf (stderr, "W> Data format defined twice "
							 "(option '%s' ignored)\n", option);
		}
		goto done;
	}

				   /* Set the data format: */
	data_format = format;
	data_format_defined = TRUE;

	done:          /* We are done */
	return;
}

/*----------------------------------------------------------------------------*/
/* set_input_format -- Set input format                                       */
/*----------------------------------------------------------------------------*/

 static
 void set_input_format (const char *option, t_Input_Format format)
{
                   /* Check parameters: */
	Assert (option != NULL, "Invalid parameter");
	Assert (e_input_format_undefined < format &&
            format < e_input_format_sentinel, "Invalid parameter");

				   /* Is the input format already defined ? */
	if (input_format_defined)
	{
				   /* Is it defined differently ? */
		if (input_format != format)
		{
			fprintf (stderr, "W> Input format defined twice "
							 "(option '%s' ignored)\n", option);
		}
		goto done;
	}

				   /* Set the input format: */
	input_format = format;
	input_format_defined = TRUE;

	done:          /* We are done */
	return;
}
