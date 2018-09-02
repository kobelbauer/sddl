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
/* File:           src/common.h                                               */
/* Contents:       Common declarations and definitions                        */
/* Author(s):      kb                                                         */
/* Last change:    2018-04-27                                                 */
/*----------------------------------------------------------------------------*/

#ifndef INCLUDED_COMMON_H
#define INCLUDED_COMMON_H

#ifndef INCLUDED_BASICS_H
#include "basics.h"
                   /* Basic system definitions */
#endif

#if CHKASS
#include <setjmp.h>
#endif

#include <stdio.h>

                   /* Basic macros: */
                   /* ------------- */

#define FALSE      0
#define TRUE       1

#ifndef M_PI
#define M_PI       3.14159265358979323846
#endif

#define M_PI_HALF  (0.5 * M_PI)
#define M_TWO_PI   (2.0 * M_PI)

#define M_MILLISECONDS_PER_DAY 86400000
                   /* Number of milliseconds per day */
#define M_SECONDS_PER_DAY 86400.0
                   /* Number of seconds per day */
#define M_UTL_NVA 0x7fff
                   /* Not a valid altitude */

#define RC_DONE    2
                   /* Return code: end of input file(s) */
#define RC_EXIT    -2
                   /* Return code: exit the application */
#define RC_FAIL    -1
                   /* Return code: error, not okay, failed */
#define RC_OKAY    0
                   /* Return code: okay, success */
#define RC_SKIP    1
                   /* Return code: not okay but not of importance - skip */

                   /* Application constants: */
                   /* ---------------------- */

#define M_ACAS_RESOLUTION_ADVISORY_LENGTH 7
                   /* Length of ACAS resolution advisory */
#define M_AIRCRAFT_IDENTIFICATION_LENGTH 8
                   /* Length of aircraft identification */
#define M_BDS_REGISTER_LENGTH 7
                   /* Length of BDS register (value) */
#define M_IOSS_FRAME_HEADER_LENGTH 8
                   /* Length of IOSS (Final) frame header */
#define M_OUTPUT_BUFFER_SIZE 4096
                   /* Size of output buffer; bytes */
#define M_RFF_FILE_HEADER_LENGTH 128
                   /* Length of RFF file header */
#define M_RFF_FRAME_HEADER_LENGTH 6
                   /* Length of RFF frame header */

                   /* Implementation limits: */
                   /* ---------------------- */

#define M_MAX_BDS_REGISTERS 12
                   /* Max. number of BDS registers */
#define M_MAX_IOSS_FRAME_LENGTH 8192
                   /* Max. length of IOSS (Final) frame data */
#define M_MAX_NUMBER_OF_RADAR_SECTORS 256
                   /* Max. number of radar sectors */
#define M_MAX_NUMBER_OF_SENSORS 256
                   /* Max. number of sensors */
#define M_MAX_NUMBER_OF_WEC 20
                   /* Max. number of warning/error conditions */
#define M_MAX_RFF_FRAME_LENGTH 2048
                   /* Max. length of RFF frame data */

                   /* Special ASTERIX data items: */
#define M_REF_INDICATOR 997
                   /* Reserved expansion field */
#define M_RFS_INDICATOR 998
                   /* Random field sequence */
#define M_SPF_INDICATOR 999
                   /* Special purpose field */

                   /* Special powers of two: */
#define M_TWO_POWER_23 8388608.0
                   /* 2**23 */
#define M_TWO_POWER_25 33554432.0
                   /* 2**25 */
#define M_TWO_POWER_30 1073741824.0
                   /* 2**30 */
#define M_TWO_POWER_31 2147483648.0
                   /* 2**31 */

#define M_YEAR_MAX 2030
                   /* Latest allowable year */
#define M_YEAR_MIN 1979
                   /* Earliest allowable year */

                   /* Setting tri-state truth value: */
#define M_TRES(cond) ((cond) ? e_is_true : e_is_false)

                   /* Conversion factors: */
                   /* ------------------- */

#define M_DEG2RAD  (M_PI / 180.0)
                   /* Conversion factor: from degrees to radians */
#define M_FL2MTR   30.48
                   /* Conversion factor: flight levels to metres */
#define M_FPM2MPS  (0.3048 / 60.0)
                   /* Conversion factor: feet/minute to metres/second */
#define M_FT2MTR   0.3048
                   /* Conversion factor: feet to metres */
#define M_KTS2MPS  (1852.0 / 3600.0)
                   /* Conversion factor: knots to metres/second */
#define M_MPS2FPM  (60.0 / 0.3048)
                   /* Conversion factor: metres/second to feet/minute */
#define M_MPS2KTS  (3600.0 / 1852.0)
                   /* Conversion factor: metres/second to knots */
#define M_MTR2FL   (1.0 / 30.48)
                   /* Conversion factor: metres to flight levels */
#define M_MTR2FT   (1.0 / 0.3048)
                   /* Conversion factor: metres to feet */
#define M_MTR2NMI  (1.0 / 1852.0)
                   /* Conversion factor: metres to nautical miles */
#define M_NMI2MTR  1852.0
                   /* Conversion factor: nautical miles to metres */
#define M_RAD2DEG  (180.0 / M_PI)
                   /* Conversion factor: from radians to degrees */

                   /* Output formats for basic data types: */
                   /* ------------------------------------ */

#define M_FMT_BOOL "%u"
                   /* For data type t_Bool */
#define M_FMT_BYTE "%u"
                   /* For data type t_Byte (in decimal) */
#define M_FMT_BYTE_HEX "%02x"
                   /* For data type t_Byte (in hexadecimal) */
#define M_FMT_CHAR "%c"
                   /* For data type char */
#if WORDSIZE == 16
#define M_FMT_PNTR "0x%04x"
                   /* For data type t_Pntr */
#define M_FMT_SI08 "%d"
                   /* For data type t_Si08 (in decimal) */
#define M_FMT_SI16 "%hd"
                   /* For data type t_Si16 (in decimal) */
#define M_FMT_SI32 "%ld"
                   /* For data type t_Si32 (in decimal) */
#define M_FMT_UI16 "%hu"
                   /* For data type t_Ui16 (in decimal) */
#define M_FMT_UI16_HEX "%04x"
                   /* For data type t_Ui16 (in hexadecimal) */
#define M_FMT_UI32 "%lu"
                   /* For data type t_Ui32 (in decimal) */
#define M_FMT_UI32_HEX "%08lx"
                   /* For data type t_Ui32 (in hexadecimal) */
#elif WORDSIZE == 32
#define M_FMT_PNTR "0x%08x"
                   /* For data type t_Pntr */
#define M_FMT_SI08 "%d"
                   /* For data type t_Si08 (in decimal) */
#define M_FMT_SI16 "%hd"
                   /* For data type t_Si16 (in decimal) */
#define M_FMT_SI32 "%ld"
                   /* For data type t_Si32 (in decimal) */
#define M_FMT_UI16 "%hu"
                   /* For data type t_Ui16 (in decimal) */
#define M_FMT_UI16_HEX "%04x"
                   /* For data type t_Ui16 (in hexadecimal) */
#define M_FMT_UI32 "%lu"
                   /* For data type t_Ui32 (in decimal) */
#define M_FMT_UI32_HEX "%08lx"
                   /* For data type t_Ui32 (in hexadecimal) */
#elif WORDSIZE == 64
#define M_FMT_PNTR "0x%08x"
                   /* For data type t_Pntr */
#define M_FMT_SI08 "%d"
                   /* For data type t_Si08 (in decimal) */
#define M_FMT_SI16 "%hd"
                   /* For data type t_Si16 (in decimal) */
#define M_FMT_SI32 "%d"
                   /* For data type t_Si32 (in decimal) */
#define M_FMT_UI16 "%hu"
                   /* For data type t_Ui16 (in decimal) */
#define M_FMT_UI16_HEX "%04x"
                   /* For data type t_Ui16 (in hexadecimal) */
#define M_FMT_UI32 "%u"
                   /* For data type t_Ui32 (in decimal) */
#define M_FMT_UI32_HEX "%08x"
                   /* For data type t_Ui32 (in hexadecimal) */
#else
#error "Unexpected WORDSIZE"
#endif

                   /* Output formats for application data types: */
                   /* ------------------------------------------ */

#if WORDSIZE == 16
#define M_FMT_ADDR "0x%06lx"
#elif WORDSIZE == 32
#define M_FMT_ADDR "0x%06lx"
#else
#define M_FMT_ADDR "0x%06x"
#endif
                   /* For SSR mode S address */
#define M_FMT_ATOD "0x%06lx"
                   /* For ASTERIX time of day (1/128 seconds) */
#define M_FMT_AZIM_DEG "%7.3f"
                   /* For azimuth values (in degrees) */
#define M_FMT_CODE "0%04o"
                   /* For SSR mode 1, 2, 3/A code */
#define M_FMT_DATE "%04d-%02d-%02d"
                   /* For Christian date */
#define M_FMT_LINE "L#%02u"
                   /* For board/line number */
#define M_FMT_RNGE_NMI "%7.3f"
                   /* For range values (in nautical miles) */
#define M_FMT_STNO "#%04d"
                   /* For system track number */
                   /* Beware:
                      We have 16-bits system track numbers now
                   */
#define M_FMT_STRG "\"%s\""
                   /* For textual string */

                   /* Envelope for chk_assertion(): */
#if CHKASS
#define Assert(a,b) chk_assertion ((a), (b), __FILE__, __LINE__)
#else
#define Assert(a,b)
#endif

#include "common_structs.h"

				   /* Global variables: */
                   /* ----------------- */

extern t_Adsb adsb;
                   /* Buffer for ADS-B report information */
extern t_Bool after_midnight;
                   /* After midnight flag */
extern t_Real assumed_height;
                   /* Assumed height; metres */
extern t_Bool assumed_height_defined;
                   /* Assumed height defined */
extern t_Bool big_endian;
                   /* Sequence of records with big-endian length field */
extern t_Bool cat_selection[256];
                   /* Selection list for ASTERIX categories */
extern t_Bool cat_selection_defined;
                   /* Selection for ASTERIX categories defined */
extern t_Bool cat_selection_initiated;
                   /* Selection for ASTERIX categories initiated */
extern t_Bool categories_required;
                   /* List of supported ASTERIX categories required */
extern t_Bool checking_fd;
                   /* Checking frame against data times */
extern int current_line_number;
                   /* Current board/line number */
extern t_Bool current_line_number_defined;
                   /* Current board/line number defined */
extern t_Byte current_stns;
                   /* Current system track numbering scheme */
extern t_Bool current_stns_defined;
                   /* Current system track numbering scheme defined */
extern t_Ui16 current_stns_dsi;
                   /* Associated data source identifier */
extern t_Data_Format data_format;
				   /* Data format */
extern t_Bool data_format_defined;
				   /* Data format defined flag */
extern t_Bool excel_output;
                   /* Special output for Microsoft Excel */
extern FILE *exc_file;
                   /* Pointer to special output file */
extern t_Frame_Time first_frame_time;
                   /* First frame time */
extern t_Bool forced_overwrite;
                   /* Forced overwrite (on list file) */
extern t_Bool formats_list_required;
				   /* List of input and data formats required flag */
extern t_Date frame_date;
                   /* Frame date */
extern t_Bool frame_date_present;
                   /* Frame date present */
extern t_Secs frame_time;
                   /* Frame time; seconds */
extern t_Bool frame_time_present;
                   /* Frame time present */
extern t_Ui32 frames_count;
				   /* Input frames count */
extern t_Ui32 frames_limit;
				   /* Input frames limit */
extern t_Bool frames_limit_defined;
                   /* Input frames limit defined */
extern t_Bool handle_multiple_lines;
                   /* Handle multiple lines */
extern t_Bool help_required;
				   /* Help for program use required flag */
extern FILE *input_file;
				   /* Pointer to input file */
extern t_Input_Format input_format;
				   /* Input format */
extern t_Bool input_format_defined;
				   /* Input format defined flag */
extern t_Ui32 input_frames;
                   /* Number of frames read (and listed) from
                      input file */
extern t_Ui32 input_length;
                   /* Number of bytes read (and listed) from
                      input file */
extern t_Ui32 input_offset;
				   /* Offset within input file */
extern char *input_path;
				   /* Pointer to path name of input file */
extern t_Bool input_path_defined;
				   /* Path name of input file defined flag */
extern t_Bool input_reftrj;
                   /* Input file holds reference trajectories */
extern t_Ui32 input_rtm;
                   /* Relative time within input file */
extern t_Frame_Time last_frame_time;
                   /* Last frame time */
extern t_Ui16 last_sacsic;
                   /* Last radar SAC/SIC */
extern t_Bool last_sacsic_available;
                   /* Last radar SAC/SIC available flag */
extern t_Ui32 last_tod;
                   /* Last time of day; 1/128 seconds */
extern t_Bool last_tod_available;
                   /* Last time of day available flag */
extern t_Ui32 length_limit;
				   /* Input length limit */
extern t_Bool length_limit_defined;
                   /* Input length limit defined */
#if LISTER
extern t_Bool list_all_levels;
                   /* List all levels */
extern t_Bool list_astx_065;
                   /* List ASTERIX category 065 */
extern t_Bool list_cat;
                   /* List ASTERIX category */
extern t_Bool list_dsis;
                   /* List data source identifications */
extern FILE *list_file;
				   /* Pointer to list file */
#endif /* LISTER */
extern t_Bool list_frame_numbers;
                   /* List frame numbers */
extern t_Bool list_frame_time;
                   /* List frame time */
extern t_Bool list_geometric_height;
                   /* List geometric height */
extern t_Bool list_ground_vector;
                   /* List ground vector (for radar and system tracks) */
#if LISTER
extern t_Bool list_hex_dump;
                   /* List hex dump */
#endif /* LISTER */
extern t_Bool list_large_height_difference;
                   /* List large height difference between data items
                      I030/150 and I030/160 in ARTAS system tracks */
extern t_Bool list_last_updating_sensor;
                   /* List last updating sensor (for system tracks) */
extern int list_level;
				   /* Level of listing */
extern t_Bool list_mode_c_height;
                   /* List SSR mode C height */
extern t_Bool list_mode_s_address;
                   /* List SSR mode S address */
extern t_Bool list_mof;
                   /* List mode of flight */
extern t_Bool list_no_mode_s;
                   /* List without SSR mode S components */
extern t_Bool list_only_background_service;
                   /* List only system track messages from background service */
extern t_Bool list_only_complementary_service;
                   /* List only system track messages from complementary service
                      (and mark as such) */
extern const char *list_path;
				   /* Pointer to path name of list file */
extern t_Bool list_path_defined;
				   /* Path name of list file defined flag */
extern t_Bool list_received_power;
                   /* List received power */
extern t_Bool list_sa;
                   /* List selected altitude */
extern t_Bool list_sensor_name;
                   /* List sensor name */
extern t_Bool list_sensor_number;
                   /* List sensor number */
extern t_Bool list_service_id;
                   /* List service identification (for system tracks) */
extern t_Bool list_ssta_info;
                   /* List sensor status information */
extern t_Bool list_ssta_info_for_excel;
                   /* List sensor status information (for Excel) */
extern t_Bool list_time_as_utc_text;
                   /* List time (of day) as UTC text */
extern t_Bool list_time_of_day;
                   /* List time of day */
extern t_Bool list_wgs84;
                   /* List WGS-84 position */
extern t_Listed_Sensor listed_sensors[M_MAX_NUMBER_OF_SENSORS];
                   /* Listed sensors */
#if USE_JSON
#include <map>
#include <string>

class JSONWriter;

extern JSON_OUTPUT_TYPE json_output_type;
extern std::map<std::string, JSON_OUTPUT_TYPE> json_output_type_map;
extern std::string json_path;

extern JSONWriter* json_writer;
#endif

extern t_Mlat mlat;
                   /* Buffer for multilateration report information */
extern t_Ui16 number_of_sensor_descriptions;
                   /* Number of sensor descriptions */
extern t_Ui16 records_in_current_frame;
                   /* Number of records in current frame */
extern t_Rsrv rsrv;
                   /* Buffer for radar service information */
extern t_Rtgt rtgt;
                   /* Buffer for radar target information */
extern t_Sensor_Ident sacsic_list[M_MAX_NUMBER_OF_SENSORS];
                   /* List of sensor identities */
extern t_Byte service_identification;
                   /* Service identification (for ARTAS output) */
extern t_Bool service_identification_defined;
                   /* Service identification defined */
extern t_Bool show_progress_indication;
                   /* Show progress indication */
extern t_Ssta ssta;
                   /* Sensor status */
extern t_Date start_date;
                   /* Start date */
extern t_Bool start_date_defined;
                   /* Start date defined */
extern t_Ui32 start_offset;
                   /* Input offset for start of listing */
extern t_Bool start_offset_defined;
                   /* Input offset for start of listing defined */
extern t_Time start_rtm;
                   /* Relative time for start of listing;
                      milliseconds */
extern t_Time start_time;
				   /* Start time of input; milliseconds */
extern t_Bool start_time_available;
				   /* Start time available flag */
extern t_Wall_Time start_wall_time;
				   /* Wall time for start of input */
extern t_Step step;
                   /* System picture step */
extern t_Bool stop_on_error;
                   /* Stop on (ASTERIX) error */
extern t_Wall_Time stop_wall_time;
				   /* Wall time for stop of input */
extern t_Bool stop_time_available;
				   /* Stop time available flag */
extern t_Strk strk;
                   /* Buffer for system track information */
extern t_Bool subtypes_required;
                   /* List of supported format subtypes required */
extern t_Secs time_bias;
                   /* Time bias; seconds */
extern t_Bool time_bias_defined;
                   /* Time bias defined */
extern int track_number_bits;
                   /* Number of bits in a track number */
extern t_Bool use_mmcc;
                   /* Use minimum mode C condition in ZSF processing */
extern t_Secs wanted_start_time;
                   /* Wanted start time; seconds */
extern t_Bool wanted_start_time_defined;
                   /* Wanted start time defined */
extern t_Secs wanted_stop_time;
                   /* Wanted stop time; seconds */
extern t_Bool wanted_stop_time_defined;
                   /* Wanted stop time defined */
extern t_Bool with_sequence_number;
                   /* Each RFF frame holds a 4 bytes sequence number */

#if CHKASS
extern jmp_buf on_fatal_error;
                   /* Label for longjmp() in case of fatal error */
#endif

                   /* Global functions: */
                   /* ----------------- */

extern t_Retc asterix_frame (t_Ui32 offset, t_Ui16 source,
                             t_Ui16 length, t_Byte *buffer);
                   /* Process an ASTERIX data frame */
extern t_Retc astx_000 (t_Ui16 length, t_Byte *buffer);
                   /* Process ASTERIX category 000 data block */
extern t_Retc astx_001 (t_Ui16 length, t_Byte *buffer);
                   /* Process ASTERIX category 001 data block */
extern t_Retc astx_002 (t_Ui16 length, t_Byte *buffer);
                   /* Process ASTERIX category 002 data block */
extern t_Retc astx_003 (t_Ui16 length, t_Byte *buffer);
                   /* Process ASTERIX category 003 data block */
extern t_Retc astx_004 (t_Ui16 length, t_Byte *buffer);
                   /* Process ASTERIX category 004 data block */
extern t_Retc astx_008 (t_Ui16 length, t_Byte *buffer);
                   /* Process ASTERIX category 008 data block */
extern t_Retc astx_010 (t_Ui16 length, t_Byte *buffer);
                   /* Process ASTERIX category 010 data block */
extern t_Retc astx_011 (t_Ui16 length, t_Byte *buffer);
                   /* Process ASTERIX category 011 data block */
extern t_Retc astx_016 (t_Ui16 length, t_Byte *buffer);
                   /* Process ASTERIX category 016 data block */
extern t_Retc astx_017 (t_Ui16 length, t_Byte *buffer);
                   /* Process ASTERIX category 017 data block */
extern t_Retc astx_019 (t_Ui16 length, t_Byte *buffer);
                   /* Process ASTERIX category 019 data block */
extern t_Retc astx_020 (t_Ui16 length, t_Byte *buffer);
                   /* Process ASTERIX category 020 data block */
extern t_Retc astx_021 (t_Ui16 length, t_Byte *buffer);
                   /* Process ASTERIX category 021 data block */
extern t_Retc astx_023 (t_Ui16 length, t_Byte *buffer);
                   /* Process ASTERIX category 023 data block */
extern t_Retc astx_030 (t_Ui16 length, t_Byte *buffer);
                   /* Process ASTERIX category 030 data block */
extern t_Retc astx_031 (t_Ui16 length, t_Byte *buffer);
                   /* Process ASTERIX category 031 data block */
extern t_Retc astx_032 (t_Ui16 length, t_Byte *buffer);
                   /* Process ASTERIX category 032 data block */
extern t_Retc astx_034 (t_Ui16 length, t_Byte *buffer);
                   /* Process ASTERIX category 034 data block */
extern t_Retc astx_048 (t_Ui16 length, t_Byte *buffer);
                   /* Process ASTERIX category 048 data block */
extern t_Retc astx_062 (t_Ui16 length, t_Byte *buffer);
                   /* Process ASTERIX category 062 data block */
extern t_Retc astx_063 (t_Ui16 length, t_Byte *buffer);
                   /* Process ASTERIX category 063 data block */
extern t_Retc astx_065 (t_Ui16 length, t_Byte *buffer);
                   /* Process ASTERIX category 065 data block */
extern t_Retc astx_221 (t_Ui16 length, t_Byte *buffer);
                   /* Process ASTERIX category 221 data block */
extern t_Retc astx_235 (t_Ui16 length, t_Byte *buffer);
                   /* Process ASTERIX category 235 data block */
extern t_Retc astx_236 (t_Ui16 length, t_Byte *buffer);
                   /* Process ASTERIX category 236 data block */
extern t_Retc astx_247 (t_Ui16 length, t_Byte *buffer);
                   /* Process ASTERIX category 247 data block */
extern t_Retc astx_252 (t_Ui16 length, t_Byte *buffer);
                   /* Process ASTERIX category 252 data block */
extern t_Real azimuth (t_Real xpos, t_Real ypos);
                   /* Compute azimuth (in degrees) */
extern void bad_frame (const char *text_ptr,
                       t_Ui32 offset, t_Ui16 length, t_Byte *buffer);
                   /* Dump bad frame */
extern t_Bool check_date (int year, int month, int day);
                   /* Check date */
#if CHKASS
extern void chk_assertion (t_Bool condition, const char *text_ptr,
                           const char *file_name, int line_number);
                   /* Check some assertion */
#endif /* CHKASS */
extern t_Time cnv_wtm_to_tod (t_Wall_Time wtm);
                   /* Convert wall time to time-of-day */
extern char * cnv_wtm_to_txt (t_Wall_Time wtm);
                   /* Convert wall time to text */
extern t_Retc data_item (t_Byte category, t_Ui16 field_reference_number,
                         t_Data_Item_Desc *desc_ptr,
                         t_Ui16 length, t_Byte *buffer, t_Ui16 *pos_ptr);
                   /* Process an ASTERIX data item */
extern t_Ui16 days_per_month (t_Ui16 year, t_Ui16 month);
                   /* Give number of days per month */
extern t_Ui16 days_per_year (t_Ui16 year);
                   /* Give number of days per year */
extern t_Real distance (t_Real x, t_Real y);
                   /* Compute Euclidean distance */
extern t_Real distance (t_Real x, t_Real y, t_Real z);
                   /* Compute Euclidean distance */
extern t_Retc do_frame (t_Ui32 offset, t_Ui16 source,
                        t_Ui16 length, t_Byte *buffer);
                   /* Process a data frame */
extern void do_option (char *option_ptr);
                   /* Process a program call option */
extern t_Real dtm_diff (t_Date_Time dt1, t_Date_Time dt2);
                   /* Compute difference in date and time */
extern t_Bool earlier (t_Date_Time dt1, t_Date_Time dt2);
                   /* Is date+time dt1 earlier than date+time dt2 */
extern void error_msg (const char *format, ...);
                   /* Error message */
extern t_Retc eval_latitude (const char *text_ptr, t_Real *value_ptr);
                   /* Evaluate a geographical latitude value */
extern t_Retc eval_longitude (const char *text_ptr, t_Real *value_ptr);
                   /* Evaluate a geographical longitude value */
extern char expand_c (t_Byte c);
                   /* Expand 6-bits character */
extern t_Retc fillup_tod (t_Ui16 partial_tod, t_Ui32 *tod_ptr);
                   /* Fill up truncated time of day */
extern t_Retc find_sensor (t_Ui16 dsi, t_Ui16 *sno_ptr);
                   /* Find sensor (number) by data source identifier */
extern t_Byte hi_byte (t_Ui16 u16);
                   /* Extract high byte */
extern void init_options (void);
                   /* Initiate program call options */
extern t_Retc init_radars (void);
                   /* Initiate radar descriptions */
extern t_Bool is_a_leap_year (t_Ui16 year);
                   /* Decide whether year is a leap year */
extern t_Retc ioss_frame (void);
                   /* Read and process SASS-C IOSS frame */
extern char * lat_text (t_Real latitude);
                   /* Return text for geographical latitude */
extern t_Bool later (t_Date_Time dt1, t_Date_Time dt2);
                   /* Is date+time dt1 later than date+time dt2 */
#if LISTER
extern void list_adsb (t_Adsb *adsb_ptr);
                   /* List an ADS-B report */
extern void list_adsb (t_Adsb *adsb_ptr, t_Ui16 length, char *buffer);
                   /* List ADS-B report information */
extern void list_aid (t_Object_Type object, t_Pntr pntr,
                      t_Ui16 length, char *buffer);
                   /* List aircraft identification (callsign or tail number) */
extern void list_ano (t_Object_Type object, t_Pntr pntr,
                      t_Ui16 length, char *buffer);
                   /* List antenna number */
extern void list_buffer (int level, const char *header,
                         t_Ui16 length, t_Byte *buffer);
                   /* List buffer contents */
extern void list_dsi (t_Object_Type object, t_Pntr pntr,
                      t_Ui16 length, char *buffer);
                   /* List data source identifier */
extern void list_frame (int level, int length, t_Byte *buffer);
                   /* List input frame */
extern void list_ftm (t_Object_Type object, t_Pntr pntr,
                      t_Ui16 length, char *buffer);
                   /* List frame time (of day) in UTC format */
extern void list_idt (t_Object_Type object, t_Pntr pntr,
                      t_Ui16 length, char *buffer);
                   /* List SSR identification */
extern void list_mlat (t_Mlat *mlat_ptr);
                   /* List multilateration report */
extern void list_mlat (t_Mlat *mlat_ptr, t_Ui16 length, char *buffer);
                   /* List multilateration report */
extern void list_msa (t_Object_Type object, t_Pntr pntr,
                      t_Ui16 length, char *buffer);
                   /* List SSR mode S address */
extern void list_mtp (t_Object_Type object, t_Pntr pntr,
                      t_Ui16 length, char *buffer);
                   /* List message or detection type */
extern void list_num (t_Object_Type object, t_Pntr pntr,
                      t_Ui16 length, char *buffer);
                   /* List generic number */
extern void list_pos (t_Object_Type object, t_Pntr pntr,
                      t_Ui16 length, char *buffer);
                   /* List position */
extern void list_rsrv (t_Rsrv *rsrv_ptr);
                   /* List radar service message */
extern void list_rsrv (t_Rsrv *rsrv_ptr, t_Ui16 length, char *buffer);
                   /* List radar service message */
extern void list_rtgt (t_Rtgt *rtgt_ptr);
                   /* List radar target report */
extern void list_rtgt (t_Rtgt *rtgt_ptr, t_Ui16 length, char *buffer);
                   /* List radar target report */
extern void list_sno (t_Object_Type object, t_Pntr pntr,
                      t_Ui16 length, char *buffer);
                   /* List sensor number */
extern void list_ssta (t_Ssta *ssta_ptr);
                   /* List sensor status information */
extern void list_step (t_Step *step_ptr);
                   /* List system picture step */
extern void list_stp (t_Object_Type object, t_Pntr pntr,
                      t_Ui16 length, char *buffer);
                   /* List stereographic position */
extern void list_strk (t_Strk *strk_ptr);
                   /* List system track information */
extern void list_strk (t_Strk *strk_ptr, t_Ui16 length, char *buffer);
                   /* List system track information */
extern void list_text (int level, const char *format, ...);
                   /* List some text */
extern void list_tod (t_Object_Type object, t_Pntr pntr,
                      t_Ui16 length, char *buffer);
                   /* List time of day */
#endif /* LISTER */
extern t_Byte lo_byte (t_Ui16 u16);
                   /* Extract low byte */
extern char * lon_text (t_Real longitude);
                   /* Return text for geographical longitude */
extern t_Si16 make_si16 (t_Byte high_byte, t_Byte low_byte);
                   /* Make a signed 16 bits integer */
extern t_Ui16 make_ui16 (t_Byte high_byte, t_Byte low_byte);
                   /* Make a unsigned 16 bits integer */
extern t_Si32 make_si32 (t_Byte high_byte, t_Byte b2, t_Byte b3, t_Byte b4);
                   /* Make a signed 32 bits integer */
extern t_Ui32 make_ui32 (t_Byte high_byte, t_Byte b2, t_Byte b3, t_Byte b4);
                   /* Make a unsigned 32 bits integer */
extern t_Retc map_llh_to_ecef (t_Geo_Point geo_pnt, t_Point *point_ptr);
                   /* Map geographical point to ECEF coordinates */
extern t_Si16 mc_to_alt (t_Ui16 mc);
                   /* Convert SSR mode C code to altitude */
extern t_Retc netto_frame (void);
                   /* Read and process "netto" frame */
extern void nl_add_bytes (t_Secs frame_tod, t_Ui16 length);
                   /* Network load monitor: add input length */
extern void nl_add_frame (t_Secs frame_tod);
                   /* Network load monitor: add input frame */
extern void nl_add_report (t_Secs frame_tod);
                   /* Network load monitor: add target report */
extern t_Retc nl_initiate (void);
                   /* Initiate network load monitor */
extern t_Retc nl_set_period (t_Ui16 period_in_milliseconds);
                   /* Network load monitor: set evaluation period */
extern void nl_terminate (void);
                   /* Terminate network load monitor */
extern t_Retc normalize (t_Date *date_ptr);
                   /* Normalize date */
extern int ord (t_Bool bval);
                   /* Return ordinal for Boolean value */
extern t_Retc process_adsb (t_Adsb *adsb_ptr);
                   /* Process ADS-B report */
extern void process_init (void);
                   /* Initiate message processing */
extern t_Retc process_input (void);
                   /* Process input file */
extern t_Retc process_mlat (t_Mlat *mlat_ptr);
                   /* Process multilateration target report */
extern t_Retc process_rsrv (t_Rsrv *rsrv_ptr);
                   /* Process radar service message */
extern t_Retc process_rtgt (t_Rtgt *rtgt_ptr);
                   /* Process radar target report */
extern t_Retc process_strk (t_Strk *strk_ptr);
                   /* Process system track message */
extern void process_term (void);
                   /* Terminate message processing */
extern t_Retc rec_frame (void);
                   /* Read and process REC frame */
extern t_Retc rff_frame (void);
                   /* Read and process RFF frame */
extern t_Retc rff_header (void);
                   /* Read and process RFF file header */
extern int rounds (t_Real f);
                   /* Rounds to the nearest integer */
extern t_Bool same_day (t_Date dt1, t_Date dt2);
                   /* Decide whether from same day */
extern t_Retc set_vsn010 (char *vsn_text);
                   /* Set ASTERIX category 010 reference version */
extern t_Retc set_vsn011 (char *vsn_text);
                   /* Set ASTERIX category 011 reference version */
extern t_Retc set_vsn020 (char *vsn_text);
                   /* Set ASTERIX category 020 reference version */
extern t_Retc set_vsn021 (char *vsn_text);
                   /* Set ASTERIX category 021 reference version */
extern t_Retc set_vsn023 (char *vsn_text);
                   /* Set ASTERIX category 023 reference version */
extern t_Retc set_vsn048 (char *vsn_text);
                   /* Set ASTERIX category 048 reference version */
extern t_Retc set_vsn065 (char *vsn_text);
                   /* Set ASTERIX category 065 reference version */
extern t_Bool stat_fits (t_Statistical_Item *item_ptr, t_Real value);
                   /* Check if value fits the statistical item */
extern void stat_init (t_Statistical_Item *item_ptr);
                   /* Initiate a statistical item */
extern void stat_updt (t_Statistical_Item *item_ptr, t_Real value);
                   /* Update a statistical item */
#if LISTER
extern void term_lister (void);
                   /* Terminate the lister */
#endif /* LISTER */
extern t_Secs time_diff (t_Secs tm_1, t_Secs tm_2);
                   /* Compute difference in time */
extern t_Ui16 ungarble (t_Ui16 bits);
                   /* Ungarble an SSR mode 2 or 3/A code */
extern char * utc_secs (t_Secs utc_time);
                   /* Return text for UTC time in seconds */
extern char * utc_text (t_Secs utc_time);
                   /* Return text for UTC time in seconds */

#endif /* INCLUDED_COMMON_H */
/* end-of-file */
