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

#ifndef COMMON_STRUCTS_CPP
#define COMMON_STRUCTS_CPP

#include "global.h"
//#define USE_JSON 1

#include "common_data_types.h"

#if USE_JSON
#include "json/json.hpp"
#include "json_conv.h"

#include <string>

using namespace std;

extern bool write_json_nulls;
#endif

/* Geodetical point: */
typedef struct
{
    t_Real altitude;
    /* Altitude above WGS-84 reference ellipsoid;
   metres */
    t_Real latitude;
    /* Geodetical latitude; radians */
    t_Real longitude;
    /* Geodetical longitude; radians */

} t_Geo_Point;

/* Geodetical point in list: */
typedef struct
{
    t_Bool defined;
    /* Point defined */
    t_Real latitude;
    /* Geodetical latitude; radians */
    t_Real longitude;
    /* Geodetical longitude; radians */
} t_GPos;

/* ACAS resolution advisory report: */
typedef struct
{
    t_Bool present;
    /* ACAS resolution advisory report present */
    t_Byte value[M_ACAS_RESOLUTION_ADVISORY_LENGTH];
    /* Currently active resolution advisory, if any */
    /* Comm-B message data of BDS register 3,0 */

#if USE_JSON
    void toJSON (nlohmann::json& j, const string& name)
    {
        if (present)
            j[name] = value;
        else if (write_json_nulls)
            j[name] = nullptr;

    }
#endif
} t_ACAS_Resolution_Advisory_Report;

/* Aircraft address: */
typedef struct
{
    t_Bool present;
    /* Aircraft address present */
    t_Ui32 value;
    /* Aircraft address */

#if USE_JSON
    void toJSON (nlohmann::json& j, const string& name)
    {
        if (present)
            j[name] = value;
        else if (write_json_nulls)
            j[name] = nullptr;
    }
#endif
} t_Aircraft_Address;

/* Aircraft identification: */
typedef struct
{
    t_Bool present;
    /* Aircraft identification present */
    t_Bool sti_present;
    /* Source of target identification present */
    char value_idt[M_AIRCRAFT_IDENTIFICATION_LENGTH + 1];
    /* Aircraft identification */
    t_Byte value_sti;
    /* Source of target identification */

#if USE_JSON
    void toJSON (nlohmann::json& j, const string& name)
    {
        if (present)
        {
            j[name]["sti_present"] = sti_present;
            j[name]["value_idt"] = value_idt;
            j[name]["value_sti"] = value_sti;
        }
        else if (write_json_nulls)
            j[name] = nullptr;

    }
#endif
} t_Aircraft_Identification;

/* Aircraft operational status: */
typedef struct
{
    t_Bool present;
    /* Aircraft operational status present */
    t_Bool value_arv;
    /* Air-referenced velocity report capability */
    t_Bool value_cdti;
    /* Cockpit display of traffic information (airborne) */
    t_Bool value_ra;
    /* TCAS resolution advisory active */
    t_Bool value_sa;
    /* Single antenna */
    t_Byte value_tc;
    /* Target change report capability */
    t_Byte value_tcas;
    /* TCAS system status */
    t_Bool value_ts;
    /* Target state report capability */

#if USE_JSON
    void toJSON (nlohmann::json& j, const string& name)
    {
        if (present)
        {
            j[name]["value_arv"] = value_arv;
            j[name]["value_cdti"] = value_cdti;
            j[name]["value_ra"] = value_ra;
            j[name]["value_sa"] = value_sa;
            j[name]["value_tc"] = value_tc;
            j[name]["value_tcas"] = value_tcas;
            j[name]["value_ts"] = value_ts;
        }
        else if (write_json_nulls)
            j[name] = nullptr;

    }
#endif
} t_Aircraft_Operational_Status;

/* Air speed: */
typedef struct
{
    t_Bool present;
    /* Air speed present */
    t_Real value;
    /* Air speed */
    /* In knots if value_im = 0;
   in Mach if value_im = 1 */
    t_Byte value_im;
    /* IAS or Mach */

#if USE_JSON
    void toJSON (nlohmann::json& j, const string& name)
    {
        if (present)
        {
            j[name]["value"] = value;
            j[name]["value_im"] = value_im;

            switch (value_im)
            {
            case 0:
                j[name]["value_im_info"] = "knots";
                break;
            case 1:
                j[name]["value_im_info"] = "mach";
                break;
            default:
                j[name]["value_im_info"] = nullptr;
                break;
            }
        }
        else if (write_json_nulls)
            j[name] = nullptr;

    }
#endif
} t_Air_Speed;

/* Altitude reporting capability: */
typedef struct
{
    t_Bool present;
    /* Altitude reporting capability present */
    t_Byte value;
    /* Altitude reporting capability */
    /*  0 = 25 feet
    1 = 100 feet
    2 = unknown
    3 = invalid
*/

#if USE_JSON
    void toJSON (nlohmann::json& j, const string& name)
    {
        if (present)
        {
            j[name]["value"] = value;

            switch (value)
            {
            case 0:
                j[name]["value_info"] = "25ft";
                break;
            case 1:
                j[name]["value_info"] = "100ft";
                break;
            case 2:
                j[name]["value_info"] = "unknown";
                break;
            default:
                j[name]["value_info"] = nullptr;
                break;
            }
        }
        else if (write_json_nulls)
            j[name] = nullptr;

    }
#endif
} t_Altitude_Reporting_Capability;

/* Amplitude: */
typedef struct
{
    t_Bool present;
    /* Amplitude present */
    t_Ui16 value;
    /* Amplitude */
} t_Amplitude;

/* Antenna azimuth: */
typedef struct
{
    t_Bool present;
    /* Antenna azimuth present */
    t_Real value;
    /* Antenna azimuth; radians */

#if USE_JSON
    void toJSON (nlohmann::json& j, const string& name)
    {
        if (present)
            j[name] = value;
        else if (write_json_nulls)
            j[name] = nullptr;

    }
#endif
} t_Antenna_Azimuth;

/* Antenna number: */
typedef struct
{
    t_Bool present;
    /* Antenna number present */
    t_Ui16 value;
    /* Antenna number */
    /* 0 means: undefined or unknown */

#if USE_JSON
    void toJSON (nlohmann::json& j, const string& name)
    {
        if (present)
            j[name] = value;
        else if (write_json_nulls)
            j[name] = nullptr;

    }
#endif
} t_Antenna_Number;

/* Antenna rotation speed: */
typedef struct
{
    t_Bool present;
    /* Antenna rotation speed present */
    t_Real value;
    /* Antenna rotation speed; radians/second */

#if USE_JSON
    void toJSON (nlohmann::json& j, const string& name)
    {
        if (present)
            j[name] = value;
        else if (write_json_nulls)
            j[name] = nullptr;

    }
#endif
} t_Antenna_Rotation_Speed;

/* Assumed height: */
typedef struct
{
    t_Bool present;
    /* Assumed height present */
    t_Real value;
    /* Assumed height; metres */

#if USE_JSON
    void toJSON (nlohmann::json& j, const string& name)
    {
        if (present)
            j[name] = value;
        else if (write_json_nulls)
            j[name] = nullptr;

    }
#endif
} t_Assumed_Height;

/* Azimuth difference: */
typedef struct
{
    t_Bool present;
    /* Azimuth difference present */
    t_Real value;
    /* Azimuth difference; radians */

#if USE_JSON
    void toJSON (nlohmann::json& j, const string& name)
    {
        if (present)
            j[name] = value;
        else if (write_json_nulls)
            j[name] = nullptr;

    }
#endif
} t_Azimuth_Difference;

/* Barometric vertical rate: */
typedef struct
{
    t_Bool present;
    /* Barometric vertical rate present */
    t_Tres range_exceeded;
    /* "Range exceeded" indicator */
    t_Real value;
    /* Barometric vertical rate; metres/second */

#if USE_JSON
    void toJSON (nlohmann::json& j, const string& name)
    {
        if (present)
        {
            convertToJSON(range_exceeded, "range_exceeded", j[name]);
            j[name]["value"] = value;
        }
        else if (write_json_nulls)
            j[name] = nullptr;

    }
#endif
} t_Barometric_Vertical_Rate;

/* Batch number: */
typedef struct
{
    t_Bool present;
    /* Batch number present */
    t_Byte value;
    /* Batch number */
} t_Batch_Number;

/* BDS register: */
typedef struct
{
    t_Byte number;
    /* Number of BDS register */
    t_Bool present;
    /* BDS register present */
    t_Byte value[M_BDS_REGISTER_LENGTH];
    /* Value of BDS register */

#if USE_JSON
    void toJSON (nlohmann::json& j, const string& name)
    {
        if (present)
        {
            char buffer [2*M_BDS_REGISTER_LENGTH+1];
            buffer[2*M_BDS_REGISTER_LENGTH] = 0;
            for(unsigned int j = 0; j < M_BDS_REGISTER_LENGTH; j++)
                sprintf(&buffer[2*j], "%02X", value[j]);

            j[name]["number"] = number;
            j[name]["value"] = buffer;
        }
        else if (write_json_nulls)
            j[name] = nullptr;

    }
#endif
} t_BDS_Register;

/* Callsign: */
typedef struct
{
    t_Bool present;
    /* Callsign present */
    char value[8];
    /* Callsign */
} t_Callsign;

/* Collimation error: */
typedef struct
{
    t_Bool present;
    /* Collimation error present */
    t_Real value_azm;
    /* Azimuth error; radians */
    t_Real value_rng;
    /* Slant range error; metres */

#if USE_JSON
    void toJSON (nlohmann::json& j, const string& name)
    {
        if (present)
        {
            j[name]["value_azm_rad"] = value_azm;
            j[name]["value_rng_m"] = value_rng;
        }
        else if (write_json_nulls)
            j[name] = nullptr;

    }
#endif
} t_Collimation_Error;

/* Communications/ACAS capability: */
typedef struct
{
    t_Bool present;
    /* Communications/ACAS capability present */
    t_Byte value_com;
    /* Communications/ACAS capability */
    /* 0 = no communications capability
   1 = Comm. A and Comm. B capability
   2 = Comm. A, Comm. B and Uplink ELM
   3 = Comm. A, Comm. B, Uplink ELM and Downlink ELM
   4 = level 5 transponder capability
   5 to 7 = not assigned
*/

#if USE_JSON
    void toJSON (nlohmann::json& j, const string& name)
    {
        if (present)
        {
            j[name]["value"] = value_com;

            switch (value_com) {
            case 0:
                j[name]["value_info"] = "no communications capability";
                break;
            case 1:
                j[name]["value_info"] = "Comm. A and Comm. B capability";
                break;
            case 2:
                j[name]["value_info"] = "Comm. A, Comm. B and Uplink ELM";
                break;
            case 3:
                j[name]["value_info"] = "Comm. A, Comm. B, Uplink ELM and Downlink ELM";
                break;
            case 4:
                j[name]["value_info"] = "level 5 transponder capability";
                break;
            default:
                j[name]["value_info"] = nullptr;
                break;
            }
        }
        else if (write_json_nulls)
            j[name] = nullptr;

    }
#endif
} t_Communications_Capability;

/* Computed Cartesian acceleration: */
typedef struct
{
    t_Bool present;
    /* Computed acceleration present */
    t_Real value_ax;
    /* Computed x component; metres/second**2 */
    t_Real value_ay;
    /* Computed y component; metres/second**2 */

#if USE_JSON
    void toJSON (nlohmann::json& j, const string& name)
    {
        if (present)
        {
            j[name]["value_ax_ms2"] = value_ax;
            j[name]["value_ay_ms2"] = value_ay;
        }
        else if (write_json_nulls)
            j[name] = nullptr;

    }
#endif
} t_Computed_Acceleration;

/* Computed altitude: */
typedef struct
{
    t_Bool present;
    /* Computed altitude present */
    t_Byte source;
    /* Source of computed track altitude */
    /* 0 ... 3D height
   1 ... triangulated height
   2 ... height derived from coverage
   3 ... assumed height

   4 ... no source
   5 ... GNSS
   6 ... 3D radar
   7 ... triangulation
   8 ... height from coverage
   9 ... speed look-up table
   10 ... default height
   11 ... multilateration
*/
    t_Real value;
    /* Computed altitude; metres */

#if USE_JSON
    void toJSON (nlohmann::json& j, const string& name)
    {
        if (present)
        {
            j[name]["source"] = source;

            switch (source) {
            case 0:
                j[name]["source_info"] = "3D height";
                break;
            case 1:
                j[name]["source_info"] = "triangulated height";
                break;
            case 2:
                j[name]["source_info"] = "height derived from coverage";
                break;
            case 3:
                j[name]["source_info"] = "assumed height";
                break;
            case 4:
                j[name]["source_info"] = "no source";
                break;
            case 5:
                j[name]["source_info"] = "GNSS";
                break;
            case 6:
                j[name]["source_info"] = "3D radar";
                break;
            case 7:
                j[name]["source_info"] = "triangulation";
                break;
            case 8:
                j[name]["source_info"] = "height from coverage";
                break;
            case 9:
                j[name]["source_info"] = "speed look-up table";
                break;
            case 10:
                j[name]["source_info"] = "default height";
                break;
            case 11:
                j[name]["source_info"] = "multilateration";
                break;
            default:
                break;
            }
            j[name]["value_m"] = value;
        }
        else if (write_json_nulls)
            j[name] = nullptr;

    }
#endif
} t_Computed_Altitude;

/* Computed Cartesian position: */
typedef struct
{
    t_Bool present;
    /* Computed position present */
    t_Real value_x;
    /* Computed x coordinate; metres */
    t_Real value_y;
    /* Computed y coordinate; metres */

#if USE_JSON
    void toJSON (nlohmann::json& j, const string& name)
    {
        if (present)
        {
            j[name]["value_x"] = value_x;
            j[name]["value_y"] = value_y;
        }
        else if (write_json_nulls)
            j[name] = nullptr;

    }
#endif
} t_Computed_Position;

/* Accuracy of computed Cartesian position: */
typedef struct
{
    t_Bool present;
    /* Accuracy of computed Cartesian position present */
    t_Real value_x;
    /* Standard deviation of x component; metres */
    t_Real value_xy;
    /* Correlation coefficient */
    t_Real value_y;
    /* Standard deviation of y component; metres */

#if USE_JSON
    void toJSON (nlohmann::json& j, const string& name)
    {
        if (present)
        {
            j[name]["value_x_m"] = value_x;
            j[name]["value_xy_m"] = value_xy;
            j[name]["value_y_m"] = value_y;
        }
        else if (write_json_nulls)
            j[name] = nullptr;

    }
#endif
} t_Computed_Position_Accuracy;

/* Computed Cartesian velocity: */
typedef struct
{
    t_Bool present;
    /* Computed velocity present */
    t_Real value_vx;
    /* Computed x component; metres/second */
    t_Real value_vy;
    /* Computed y component; metres/second */

#if USE_JSON
    void toJSON (nlohmann::json& j, const string& name)
    {
        if (present)
        {
            j[name]["value_vx_ms"] = value_vx;
            j[name]["value_vy_ms"] = value_vy;
        }
        else if (write_json_nulls)
            j[name] = nullptr;

    }
#endif
} t_Computed_Velocity;

/* Confidence level: */
typedef struct
{
    t_Bool present;
    /* Confidence level present */
    t_Byte value;
    /* Confidence level */

#if USE_JSON
    void toJSON (nlohmann::json& j, const string& name)
    {
        if (present)
        {
            j[name] = value;
        }
        else if (write_json_nulls)
            j[name] = nullptr;

    }
#endif
} t_Confidence_Level;

/* Data ages (within system track): */
typedef struct
{
    t_Bool acs_present;
    /* Age of DAP "ACAS Resolution Advisory Report" present */
    t_Bool bps_present;
    /* Age of DAP "Barometric Pressure Setting" present */
    t_Bool bvr_present;
    /* Age of DAP "Barometric Vertical Rate" present */
    t_Bool com_present;
    /* Age of DAP "Communication/ACAS Capability and
   Flight Status"; seconds */
    t_Bool emc_present;
    /* Age of DAP "Emitter Category" present */
    t_Bool fss_present;
    /* Age of DAP "Final State Selected Altitude" present */
    t_Bool gal_present;
    /* Age of DAP "Geometric Altitude" present */
    t_Bool gsp_present;
    /* Age of DAP "Ground Speed" present */
    t_Bool gvr_present;
    /* Age of DAP "Geometrical Vertical Rate" present */
    t_Bool iar_present;
    /* Age of DAP "Indicated Airspeed" present */
    t_Bool ias_present;
    /* Age of DAP "Indicated Airspeed / Mach Number" present */
    t_Bool mac_present;
    /* Age of DAP "Mach Number" present */
    t_Bool mb_present;
    /* Age of DAP "Mode S MB Data" present */
    t_Bool md1_present;
    /* Age of last SSR mode 1 code present */
    t_Bool md2_present;
    /* Age of last SSR mode 2 code present */
    t_Bool md4_present;
    /* Age of last SSR mode 4 code present */
    t_Bool md5_present;
    /* Age of last SSR mode 5 code present */
    t_Bool mda_present;
    /* Age of last SSR mode 3/A code present */
    t_Bool met_present;
    /* Age of DAP "Meteorological Data" present */
    t_Bool mfl_present;
    /* Age of last valid and credible flight level present */
    t_Bool mhg_present;
    /* Age of DAP "Magnetic Heading" present */
    t_Bool pos_present;
    /* Age of DAP "Position" present */
    t_Bool present;
    /* Data ages present */
    t_Bool pun_present;
    /* Age of DAP "Position Uncertainty" present */
    t_Bool ran_present;
    /* Age of DAP "Roll Angle" present */
    t_Bool sab_present;
    /* Age of DAP "Status Reported by ADS-B" present */
    t_Bool sal_present;
    /* Age of DAP "Selected Altitude" present */
    t_Bool tan_present;
    /* Age of DAP "Track Angle" present */
    t_Bool tar_present;
    /* Age of DAP "Track Angle Rate" present */
    t_Bool tas_present;
    /* Age of DAP "True Airspeed" present */
    t_Bool tid_present;
    /* Age of DAP "Trajectory Intent" present */
    t_Real value_acs;
    /* Age of DAP "ACAS Resolution Advisory Report"; seconds */
    t_Real value_bps;
    /* Age of DAP "Barometric Pressure Setting"; seconds */
    t_Real value_bvr;
    /* Age of DAP "Barometric Vertical Rate"; seconds */
    t_Real value_com;
    /* Age of DAP "Communication/ACAS Capability and
   Flight Status"; seconds */
    t_Real value_emc;
    /* Age of DAP "Emitter Category"; seconds */
    t_Real value_fss;
    /* Age of DAP "Final State Selected Altitude"; seconds */
    t_Real value_gal;
    /* Age of DAP "Geometric Altitude"; seconds */
    t_Real value_gsp;
    /* Age of DAP "Ground Speed"; seconds */
    t_Real value_gvr;
    /* Age of DAP "Geometrical Vertical Rate"; seconds */
    t_Real value_iar;
    /* Age of DAP "Indicated Airspeed"; seconds */
    t_Real value_ias;
    /* Age of DAP "Indicated Airspeed / Mach Number"; seconds */
    t_Real value_mac;
    /* Age of DAP "Mach Number"; seconds */
    t_Real value_mb;
    /* Age of DAP "Mode S MB Data"; seconds */
    t_Real value_md1;
    /* Age of last valid and credible SSR mode 1 code used
   to update the track; seconds */
    t_Real value_md2;
    /* Age of last valid and credible SSR mode 2 code used
   to update the track; seconds */
    t_Real value_md4;
    /* Age of last valid and credible SSR mode 4 code used
   to update the track; seconds */
    t_Real value_md5;
    /* Age of last valid and credible SSR mode 5 code used
   to update the track; seconds */
    t_Real value_mda;
    /* Age of last valid and credible SSR mode 3/A code used
   to update the track; seconds */
    t_Real value_met;
    /* Age of DAP "Meteorological Data"; seconds */
    t_Real value_mfl;
    /* Age of last valid and credible SSR mode C code or
   barometric altitude from ADS-B used to update the
   track; seconds */
    t_Real value_mhg;
    /* Age of DAP "Magnetic Heading"; seconds */
    t_Real value_pos;
    /* Age of DAP "Position"; seconds */
    t_Real value_pun;
    /* Age of DAP "Position Uncertianty"; seconds */
    t_Real value_ran;
    /* Age of DAP "Roll Angle"; seconds */
    t_Real value_sab;
    /* Age of DAP "Status Reported by ADS-B"; seconds */
    t_Real value_sal;
    /* Age of DAP "Selected Altitude"; seconds */
    t_Real value_tan;
    /* Age of DAP "Track Angle"; seconds */
    t_Real value_tar;
    /* Age of DAP "Track Angle Rate"; seconds */
    t_Real value_tas;
    /* Age of DAP "True Airspeed"; seconds */
    t_Real value_tid;
    /* Age of DAP "Trajectory Intent"; seconds */
    t_Real value_vun;
    /* Age of DAP "Velocity Uncertainty"; seconds */
    t_Bool vun_present;
    /* Age of DAP "Velocity Uncertainty" present */

#if USE_JSON
    void toJSON (nlohmann::json& j, const string& name)
    {
        if (present)
        {
            // t_Bool acs_present;
            /* Age of DAP "ACAS Resolution Advisory Report" present */
            // t_Real value_acs;
            /* Age of DAP "ACAS Resolution Advisory Report"; seconds */
            if (acs_present)
                j[name]["value_acs"] = value_acs;
            else if (write_json_nulls)
                j[name]["value_acs"] = nullptr;

            // t_Bool bps_present;
            /* Age of DAP "Barometric Pressure Setting" present */
            // t_Real value_bps;
            /* Age of DAP "Barometric Pressure Setting"; seconds */
            if (bps_present)
                j[name]["value_bps"] = value_bps;
            else if (write_json_nulls)
                j[name]["value_bps"] = nullptr;

            // t_Bool bvr_present;
            /* Age of DAP "Barometric Vertical Rate" present */
            // t_Real value_bvr;
            /* Age of DAP "Barometric Vertical Rate"; seconds */
            if (bvr_present)
                j[name]["value_bvr"] = value_bvr;
            else if (write_json_nulls)
                j[name]["value_bvr"] = nullptr;

            // t_Bool com_present;
            /* Age of DAP "Communication/ACAS Capability and
           Flight Status"; seconds */
            // t_Real value_com;
            /* Age of DAP "Communication/ACAS Capability and
           Flight Status"; seconds */
            if (com_present)
                j[name]["value_com"] = value_com;
            else if (write_json_nulls)
                j[name]["value_com"] = nullptr;

            // t_Bool emc_present;
            /* Age of DAP "Emitter Category" present */
            // t_Real value_emc;
            /* Age of DAP "Emitter Category"; seconds */
            if (emc_present)
                j[name]["value_emc"] = value_emc;
            else if (write_json_nulls)
                j[name]["value_emc"] = nullptr;

            // t_Bool fss_present;
            /* Age of DAP "Final State Selected Altitude" present */
            // t_Real value_fss;
            /* Age of DAP "Final State Selected Altitude"; seconds */
            if (fss_present)
                j[name]["value_fss"] = value_fss;
            else if (write_json_nulls)
                j[name]["value_fss"] = nullptr;

            // t_Bool gal_present;
            /* Age of DAP "Geometric Altitude" present */
            // t_Real value_gal;
            /* Age of DAP "Geometric Altitude"; seconds */
            if (gal_present)
                j[name]["value_gal"] = value_gal;
            else if (write_json_nulls)
                j[name]["value_gal"] = nullptr;

            // t_Bool gsp_present;
            /* Age of DAP "Ground Speed" present */
            // t_Real value_gsp;
            /* Age of DAP "Ground Speed"; seconds */
            if (gsp_present)
                j[name]["value_gsp"] = value_gsp;
            else if (write_json_nulls)
                j[name]["value_gsp"] = nullptr;

            // t_Bool gvr_present;
            /* Age of DAP "Geometrical Vertical Rate" present */
            // t_Real value_gvr;
            /* Age of DAP "Geometrical Vertical Rate"; seconds */
            if (gvr_present)
                j[name]["value_gvr"] = value_gvr;
            else if (write_json_nulls)
                j[name]["value_gvr"] = nullptr;

            // t_Bool iar_present;
            /* Age of DAP "Indicated Airspeed" present */
            // t_Real value_iar;
            /* Age of DAP "Indicated Airspeed"; seconds */
            if (iar_present)
                j[name]["value_iar"] = value_iar;
            else if (write_json_nulls)
                j[name]["value_iar"] = nullptr;

            // t_Bool ias_present;
            /* Age of DAP "Indicated Airspeed / Mach Number" present */
            // t_Real value_ias;
            /* Age of DAP "Indicated Airspeed / Mach Number"; seconds */
            if (ias_present)
                j[name]["value_ias"] = value_ias;
            else if (write_json_nulls)
                j[name]["value_ias"] = nullptr;

            // t_Bool mac_present;
            /* Age of DAP "Mach Number" present */
            // t_Real value_mac;
            /* Age of DAP "Mach Number"; seconds */
            if (mac_present)
                j[name]["value_mac"] = value_mac;
            else if (write_json_nulls)
                j[name]["value_mac"] = nullptr;

            // t_Bool mb_present;
            /* Age of DAP "Mode S MB Data" present */
            // t_Real value_mb;
            /* Age of DAP "Mode S MB Data"; seconds */
            if (mb_present)
                j[name]["value_mb"] = value_mb;
            else if (write_json_nulls)
                j[name]["value_mb"] = nullptr;

            // t_Bool md1_present;
            /* Age of last SSR mode 1 code present */
            // t_Real value_md1;
            /* Age of last valid and credible SSR mode 1 code used
           to update the track; seconds */
            if (md1_present)
                j[name]["value_md1"] = value_md1;
            else if (write_json_nulls)
                j[name]["value_md1"] = nullptr;

            // t_Bool md2_present;
            /* Age of last SSR mode 2 code present */
            // t_Real value_md2;
            /* Age of last valid and credible SSR mode 2 code used
           to update the track; seconds */
            if (md2_present)
                j[name]["value_md2"] = value_md2;
            else if (write_json_nulls)
                j[name]["value_md2"] = nullptr;

            // t_Bool md4_present;
            /* Age of last SSR mode 4 code present */
            // t_Real value_md4;
            /* Age of last valid and credible SSR mode 4 code used
           to update the track; seconds */
            if (md4_present)
                j[name]["value_md4"] = value_md4;
            else if (write_json_nulls)
                j[name]["value_md4"] = nullptr;

            // t_Bool md5_present;
            /* Age of last SSR mode 5 code present */
            // t_Real value_md5;
            /* Age of last valid and credible SSR mode 5 code used
           to update the track; seconds */
            if (md5_present)
                j[name]["value_md5"] = value_md5;
            else if (write_json_nulls)
                j[name]["value_md5"] = nullptr;

            // t_Bool mda_present;
            /* Age of last SSR mode 3/A code present */
            // t_Real value_mda;
            /* Age of last valid and credible SSR mode 3/A code used
           to update the track; seconds */
            if (mda_present)
                j[name]["value_mda"] = value_mda;
            else if (write_json_nulls)
                j[name]["value_mda"] = nullptr;

            // t_Bool met_present;
            /* Age of DAP "Meteorological Data" present */
            // t_Real value_met;
            /* Age of DAP "Meteorological Data"; seconds */
            if (met_present)
                j[name]["value_met"] = value_met;
            else if (write_json_nulls)
                j[name]["value_met"] = nullptr;

            // t_Bool mfl_present;
            /* Age of last valid and credible flight level present */
            // t_Real value_mfl;
            /* Age of last valid and credible SSR mode C code or
           barometric altitude from ADS-B used to update the
           track; seconds */
            if (mfl_present)
                j[name]["value_mfl"] = value_mfl;
            else if (write_json_nulls)
                j[name]["value_mfl"] = nullptr;

            // t_Bool mhg_present;
            /* Age of DAP "Magnetic Heading" present */
            // t_Real value_mhg;
            /* Age of DAP "Magnetic Heading"; seconds */
            if (mhg_present)
                j[name]["value_mhg"] = value_mhg;
            else if (write_json_nulls)
                j[name]["value_mhg"] = nullptr;


            // t_Bool pos_present;
            /* Age of DAP "Position" present */
            // t_Real value_pos;
            /* Age of DAP "Position"; seconds */
            if (pos_present)
                j[name]["value_pos"] = value_pos;
            else if (write_json_nulls)
                j[name]["value_pos"] = nullptr;

            // t_Bool pun_present;
            /* Age of DAP "Position Uncertainty" present */
            // t_Real value_pun;
            /* Age of DAP "Position Uncertianty"; seconds */
            if (pun_present)
                j[name]["value_pun"] = value_pun;
            else if (write_json_nulls)
                j[name]["value_pun"] = nullptr;

            // t_Bool ran_present;
            /* Age of DAP "Roll Angle" present */
            // t_Real value_ran;
            /* Age of DAP "Roll Angle"; seconds */
            if (ran_present)
                j[name]["value_ran"] = value_ran;
            else if (write_json_nulls)
                j[name]["value_ran"] = nullptr;

            // t_Bool sab_present;
            /* Age of DAP "Status Reported by ADS-B" present */
            // t_Real value_sab;
            /* Age of DAP "Status Reported by ADS-B"; seconds */
            if (sab_present)
                j[name]["value_sab"] = value_sab;
            else if (write_json_nulls)
                j[name]["value_sab"] = nullptr;

            // t_Bool sal_present;
            /* Age of DAP "Selected Altitude" present */
            // t_Real value_sal;
            /* Age of DAP "Selected Altitude"; seconds */
            if (sal_present)
                j[name]["value_sal"] = value_sal;
            else if (write_json_nulls)
                j[name]["value_sal"] = nullptr;

            // t_Bool tan_present;
            /* Age of DAP "Track Angle" present */
            // t_Real value_tan;
            /* Age of DAP "Track Angle"; seconds */
            if (tan_present)
                j[name]["value_tan"] = value_tan;
            else if (write_json_nulls)
                j[name]["value_tan"] = nullptr;

            // t_Bool tas_present;
            /* Age of DAP "True Airspeed" present */
            // t_Real value_tas;
            /* Age of DAP "True Airspeed"; seconds */
            if (tas_present)
                j[name]["value_tas"] = value_tas;
            else if (write_json_nulls)
                j[name]["value_tas"] = nullptr;

            // t_Bool tar_present;
            /* Age of DAP "Track Angle Rate" present */
            // t_Real value_tar;
            /* Age of DAP "Track Angle Rate"; seconds */
            if (tar_present)
                j[name]["value_tar"] = value_tar;
            else if (write_json_nulls)
                j[name]["value_tar"] = nullptr;

            // t_Bool tid_present;
            /* Age of DAP "Trajectory Intent" present */
            // t_Real value_tid;
            /* Age of DAP "Trajectory Intent"; seconds */
            if (tid_present)
                j[name]["value_tid"] = value_tid;
            else if (write_json_nulls)
                j[name]["value_tid"] = nullptr;

            // t_Real value_vun;
            /* Age of DAP "Velocity Uncertainty"; seconds */
            // t_Bool vun_present;
            /* Age of DAP "Velocity Uncertainty" present */
            if (vun_present)
                j[name]["value_vun"] = value_vun;
            else if (write_json_nulls)
                j[name]["value_vun"] = nullptr;
        }
        else if (write_json_nulls)
            j[name] = nullptr;
    }
#endif
} t_Data_Ages;

/* Data item types (for ASTERIX): */
typedef enum
{
    e_undefined_data_item = 0,
    /* Undefined data item */
    e_fixed_length_data_item,
    /* Fixed length data item */
    e_repetitive_data_item,
    /* Repetitive data item comprising a repetition
   factor of one octet, followed by fixed-length
   sub-items */
    e_variable_length_data_item,
    /* Variable length data item comprising a first part
   of one octet, followed by one-octet extents as
   necessary */
    e_immediate_data_item
    /* Data item with "immediate" reading function */
} t_Data_Item_Type;

/* Description of an ASTERIX data item: */
typedef struct
{
    t_Ui16 category;
    /* Category */
    t_Si16 data_item;
    /* Data item (number) */
    t_Data_Item_Type item_type;
    /* Data item type */
    t_Ui16 fixed_length;
    /* Fixed length of data item or (in case of an
   repetitive data item) sub-item; bytes */
    t_P_Fp proc_fptr;
    /* Pointer to processing function */
    t_R_Fp read_fptr;
    /* Pointer to reading function */
} t_Data_Item_Desc;

/* Data formats: */
typedef enum
{
    e_data_format_undefined = 0,
    /* Undefined data format */
    e_data_format_asterix,
    /* ASTERIX format */
    e_data_format_zzz,
    /* Unknown data format - ignore */
    e_data_format_sentinel
    /* Sentinel - DO NOT REMOVE */
    /* MUST BE THE LAST ELEMENT */
} t_Data_Format;

/* Data reference: */
typedef struct
{
    t_Ui16 buffer;
    /* Local buffer */
    /* Used if only one or two bytes are being
   kept, otherwise "empty" (not used) */
    t_Byte *data_ptr;
    /* Pointer to special data memory */
    /* Holds all data bytes if more than two */
    t_Ui16 length;
    /* Number of bytes stored in this reference */
    /* If just one or two bytes, they are stored
   in the local buffer, else in some memory
   especially allocated for this purpose */
    t_Bool present;
    /* Data reference present */
} t_Data_Reference;

/* Data source identifier: */
typedef struct
{
    t_Bool present;
    /* Data source identifier present */
    t_Bool supplemented;
    /* Data source identifier supplemented */
    t_Ui16 value;
    /* Data source identifier (SAC/SIC) */
    t_Byte sac;
    /* Data source system area code */
    t_Byte sic;
    /* Data source system identification code */

#if USE_JSON
    void toJSON (nlohmann::json& j, const string& name)
    {
        if (present)
        {
            j[name]["supplemented"] = supplemented;
            j[name]["value"] = value;
            j[name]["sac"] = sac;
            j[name]["sic"] = sic;
        }
        else if (write_json_nulls)
            j[name] = nullptr;

    }
#endif
} t_Data_Source_Identifier;

/* Data source position: */
typedef struct
{
    t_Bool present;
    /* Data source position present */
    t_Real value_altitude;
    /* Altitude; metres */
    t_Real value_latitude;
    /* WGS-84 latitude; radians */
    t_Real value_longitude;
    /* WGS-84 longitude; radians */
} t_Data_Source_Position;

/* Date: */
typedef struct
{
    int year;
    int month;
    int day;
} t_Date;

/* Date and time (of day): */
typedef struct
{
    t_Date date;
    /* Date information */
    t_Bool date_present;
    /* Date information present */
    t_Secs time;
    /* Time (of day); seconds */
} t_Date_Time;

/* Detection type: */
typedef struct
{
    t_Tres from_fixed_field_transponder;
    /* Target report originating from Fixed Field
   Transponder (FFT) which is also called a
   "parrot" or RABM (range and bearing monitor)
   sometimes */
    t_Tres is_radar_track;
    /* Is a radar track (not a radar plot) */
    t_Tres is_raw_plot;
    /* Is a so-called "raw" plot, i. e. rather
   unprocessed target from an early stage */
    t_Tres mode_s_all_call;
    /* Response to an SSR mode S all-call */
    t_Tres mode_s_roll_call;
    /* Response to an SSR mode S roll-call */
    t_Bool present;
    /* Detection type present */
    t_Tres reported_from_ads;
    /* Reported from an ADS-B groundstation */
    t_Tres reported_from_mds;
    /* Reported from a Mode S sensor (tracker) */
    t_Tres reported_from_mlt;
    /* Reported from a multilateration system */
    t_Tres reported_from_psr;
    /* Reported from a Primary Surveillance Radar */
    t_Tres reported_from_ssr;
    /* Reported from a Secondary Surveillance Radar */
    t_Bool sector_crossing;
    /* Sector crossing */
    t_Tres simulated;
    /* Simulated (not actual) target */
    t_Tres test_target;
    /* (Internal or external) test target */

#if USE_JSON
    void toJSON (nlohmann::json& j, const string& name)
    {
        if (present)
        {
            convertToJSON(from_fixed_field_transponder, "from_fixed_field_transponder", j[name]);
            convertToJSON(is_radar_track, "is_radar_track", j[name]);
            convertToJSON(is_raw_plot, "is_raw_plot", j[name]);
            convertToJSON(mode_s_all_call, "mode_s_all_call", j[name]);
            convertToJSON(mode_s_roll_call, "mode_s_roll_call", j[name]);
            convertToJSON(reported_from_ads, "reported_from_ads", j[name]);
            convertToJSON(reported_from_mds, "reported_from_mds", j[name]);
            convertToJSON(reported_from_mlt, "reported_from_mlt", j[name]);
            convertToJSON(reported_from_psr, "reported_from_psr", j[name]);
            convertToJSON(reported_from_ssr, "reported_from_ssr", j[name]);
            j[name]["sector_crossing"] = sector_crossing;
            convertToJSON(simulated, "simulated", j[name]);
            convertToJSON(test_target, "test_target", j[name]);
        }
        else if (write_json_nulls)
            j[name] = nullptr;
    }
#endif
} t_Detection_Type;

/* DOP of position: */
typedef struct
{
    t_Bool present;
    /* DOP of position present */
    t_Real value_x;
    /* DOP along x axis */
    t_Real value_xy;
    /* Correlation coefficient */
    t_Real value_y;
    /* DOP along y axis */

#if USE_JSON
    void toJSON (nlohmann::json& j, const string& name)
    {
        if (present)
        {
            j[name]["value_x"] = value_x;
            j[name]["value_xy"] = value_xy;
            j[name]["value_y"] = value_y;
        }
        else if (write_json_nulls)
            j[name] = nullptr;
    }
#endif
} t_DOP_of_Position;

/* Doppler speed: */
typedef struct
{
    t_Tres is_doubtful;
    /* Doppler speed is doubtful */
    t_Bool present;
    /* Doppler speed present */
    t_Real value;
    /* Doppler speed; metres/second */

#if USE_JSON
    void toJSON (nlohmann::json& j, const string& name)
    {
        if (present)
        {
            convertToJSON(is_doubtful, "is_doubtful", j[name]);
            j[name]["value"] = value;
        }
        else if (write_json_nulls)
            j[name] = nullptr;

    }
#endif
} t_Doppler_Speed;

/* Emitter category: */
typedef struct
{
    t_Bool present;
    /* Emitter category present */
    int value;
    /* Emitter category */
    /*  0 = No ADS-B emitter category information
    1 = light aircraft
    2 = small aircraft
    3 = medium aircraft
    4 = high vortex large
    5 = heavy aircraft
    6 = highly manoeuvrable and high speed
    7 = reserved
    8 = reserved
    9 = reserved
    10 = rotocraft
    11 = glider / sailplane
    12 = lighter-than-air
    13 = unmanned aerial vehicle
    14 = space / transatmospheric vehicle
    15 = ultralight / handglider / paraglider
    16 = parachutist / skydiver
    17 = reserved
    18 = reserved
    19 = reserved
    20 = surface emergency vehicle
    21 = surface service vehicle
    22 = fixed ground or tethered obstacle
    23 = cluster obstacle
    24 = line obstacle
 */

#if USE_JSON
    void toJSON (nlohmann::json& j, const string& name)
    {
        if (present)
        {
            j[name]["value"] = value;

            switch (value)
            {
//            0 = No ADS-B emitter category information
            case 0:
                j[name]["value_info"] = "no ADS-B emitter category information";
                break;
//            1 = light aircraft
            case 1:
                j[name]["value_info"] = "light aircraft";
                break;
//            2 = small aircraft
            case 2:
                j[name]["value_info"] = "small aircraft";
                break;
//            3 = medium aircraft
            case 3:
                j[name]["value_info"] = "medium aircraft";
                break;
//            4 = high vortex large
            case 4:
                j[name]["value_info"] = "high vortex large";
                break;
//            5 = heavy aircraft
            case 5:
                j[name]["value_info"] = "heavy aircraft";
                break;
//            6 = highly manoeuvrable and high speed
            case 6:
                j[name]["value_info"] = "highly manoeuvrable and high speed";
                break;
//            7,8,9 = reserved
            case 7:
            case 8:
            case 9:
                j[name]["value_info"] = "reserved";
                break;
//            10 = rotocraft
            case 10:
                j[name]["value_info"] = "rotocraft";
                break;
//            11 = glider / sailplane
            case 11:
                j[name]["value_info"] = "glider / sailplane";
                break;
//            12 = lighter-than-air
            case 12:
                j[name]["value_info"] = "lighter-than-air";
                break;
//            13 = unmanned aerial vehicle
            case 13:
                j[name]["value_info"] = "unmanned aerial vehicle";
                break;
//            14 = space / transatmospheric vehicle
            case 14:
                j[name]["value_info"] = "space / transatmospheric vehicle";
                break;
//            15 = ultralight / handglider / paraglider
            case 15:
                j[name]["value_info"] = "ultralight / handglider / paraglider";
                break;
//            16 = parachutist / skydiver
            case 16:
                j[name]["value_info"] = "parachutist / skydiver";
                break;
//            17,18,19 = reserved
            case 17:
            case 18:
            case 19:
                j[name]["value_info"] = "reserved";
                break;
//            20 = surface emergency vehicle
            case 20:
                j[name]["value_info"] = "surface emergency vehicle";
                break;
//            21 = surface service vehicle
            case 21:
                j[name]["value_info"] = "surface service vehicle";
                break;
//            22 = fixed ground or tethered obstacle
            case 22:
                j[name]["value_info"] = "fixed ground or tethered obstacle";
                break;
//            23 = cluster obstacle
            case 23:
                j[name]["value_info"] = "cluster obstacle";
                break;
//            24 = line obstacle
            case 34:
                j[name]["value_info"] = "line obstacle";
                break;
            default:
                j[name]["value_info"] = nullptr;
                break;
            }
        }
        else if (write_json_nulls)
            j[name] = nullptr;

    }
#endif
} t_Emitter_Category;

/* Figure of merit: */
typedef struct
{
    t_Bool present;
    /* Figure of merit present */
    t_Byte value;
    /* Figure of merit */

#if USE_JSON
    void toJSON (nlohmann::json& j, const string& name)
    {
        if (present)
            j[name] = value;
        else if (write_json_nulls)
            j[name] = nullptr;
    }
#endif
} t_Figure_of_Merit;

/* File header: */
typedef struct
{
    t_Bool present;
    /* File header present */
    t_Date_Time start_date;
    /* Start date */
    t_Date_Time stop_date;
    /* Stop date */
} t_File_Header;

/* Final state selected altitude: */
typedef struct
{
    t_Bool present;
    /* Final state selected altitude present */
    t_Real value;
    /* Altitude; metres */
    t_Byte value_ah;
    /* Altitude hold */
    t_Byte value_am;
    /* Approach mode */
    t_Si32 value_in_feet;
    /* Altitude; feet */
    t_Byte value_mv;
    /* Manage vertical mode */

#if USE_JSON
    void toJSON (nlohmann::json& j, const string& name)
    {
        if (present)
        {
            j[name]["value_m"] = value;
            j[name]["value_ah"] = value_ah;
            j[name]["value_am"] = value_am;
            j[name]["value_feet"] = value_in_feet;
            j[name]["value_mv"] = value_mv;
        }
        else if (write_json_nulls)
            j[name] = nullptr;

    }
#endif
} t_Final_State_Selected_Altitude;

/* Flight level: */
typedef struct
{
    t_Bool present;
    /* Flight level present */
    t_Real value;
    /* Flight level; metres */
    t_Si32 value_in_feet;
    /* Flight level; feet */

#if USE_JSON
    void toJSON (nlohmann::json& j, const string& name)
    {
        if (present)
        {
            j[name]["value_m"] = value;
            j[name]["value_feet"] = value_in_feet;
        }
        else if (write_json_nulls)
            j[name] = nullptr;

    }
#endif
} t_Flight_Level;

/* Flight status: */
typedef struct
{
    t_Bool present;
    /* Flight status present */
    t_Byte value_aic;
    /* Aircraft identification capability */
    /* 0 = no, 1 = yes */
    t_Byte value_arc;
    /* Altitude reporting capability */
    /* 0 = 100 ft resolution, 1 = 25 ft resolution */
    t_Byte value_b1a;
    /* BDS 1,0 bit 16 */
    t_Byte value_b1b;
    /* BDS 1,0 bits 37 to 40 */
    t_Byte value_mssc;
    /* Mode S specific service capability */
    /* 0 = no, 1 = yes */
    t_Byte value_status;
    /* Flight status */
    /* 0 = no alert, no SPI, aircraft airborne
   1 = no alert, no SPI, aircraft on ground
   2 = alert, no SPI, aircraft airborne
   3 = alert, no SPI, aircraft on ground
   4 = alert, SPI, aircraft airborne or on ground
   5 = no alert, SPI, aircraft airborne or on ground
   6 = not assigned
   7 = information not yet extracted
*/

#if USE_JSON
    void toJSON (nlohmann::json& j, const string& name)
    {
        if (present)
        {
            j[name]["value_aic"] = value_aic;
            j[name]["value_arc"] = value_arc;
            j[name]["value_b1a"] = value_b1a;
            j[name]["value_b1b"] = value_b1b;
            j[name]["value_mssc"] = value_mssc;
            j[name]["value_status"] = value_status;

            switch (value_status)
            {
            case 0:
                j[name]["value_status_info"] = "no alert, no SPI, aircraft airborne";
                break;
            case 1:
                j[name]["value_status_info"] = "no alert, no SPI, aircraft on ground";
                break;
            case 2:
                j[name]["value_status_info"] = "alert, no SPI, aircraft airborne";
                break;
            case 3:
                j[name]["value_status_info"] = "alert, no SPI, aircraft on ground";
                break;
            case 4:
                j[name]["value_status_info"] = "alert, SPI, aircraft airborne or on ground";
                break;
            case 5:
                j[name]["value_status_info"] = "no alert, SPI, aircraft airborne or on ground";
                break;
            default:
                if (write_json_nulls)
                    j[name]["value_status_info"] = nullptr;
                break;
            }
        }
        else if (write_json_nulls)
            j[name] = nullptr;
    }
#endif
} t_Flight_Status;

/* Frame date: */
typedef struct
{
    t_Bool present;
    /* Frame date present */
    t_Date value;
    /* Frame date */

#if USE_JSON
    void toJSON (nlohmann::json& j, const string& name)
    {
        if (present)
        {
            j[name]["year"] = value.year;
            j[name]["month"] = value.month;
            j[name]["day"] = value.day;
        }
        else if (write_json_nulls)
            j[name] = nullptr;
    }
#endif
} t_Frame_Date;

/* Frame time (of day): */
typedef struct
{
    t_Bool present;
    /* Frame time present */
    t_Secs value;
    /* Frame time (of day); seconds */

#if USE_JSON
    void toJSON (nlohmann::json& j, const string& name)
    {
        if (present)
            j[name] = value;
        else if (write_json_nulls)
            j[name] = nullptr;
    }
#endif
} t_Frame_Time;

/* Geometric altitude: */
typedef struct
{
    t_Bool present;
    /* Geometric altitude present */
    t_Real value;
    /* Geometric altitude; metres */

#if USE_JSON
    void toJSON (nlohmann::json& j, const string& name)
    {
        if (present)
            j[name] = value;
        else if (write_json_nulls)
            j[name] = nullptr;
    }
#endif
} t_Geometric_Altitude;

/* Geometric vertical rate: */
typedef struct
{
    t_Bool present;
    /* Geometric vertical rate present */
    t_Tres range_exceeded;
    /* "Range exceeded" indicator */
    t_Real value;
    /* Geometric vertical rate; metres/second */

#if USE_JSON
    void toJSON (nlohmann::json& j, const string& name)
    {
        if (present)
        {
            convertToJSON(range_exceeded, "range_exceeded", j[name]);
            j[name]["value_ms"] = value;
        }
        else if (write_json_nulls)
            j[name] = nullptr;
    }
#endif
} t_Geometric_Vertical_Rate;

/* Ground vector: */
typedef struct
{
    t_Bool present;
    /* Ground vector present */
    t_Tres range_exceeded;
    /* "Range exceeded" indication */
    t_Real value_gsp;
    /* Ground speed; metres/second */
    t_Real value_hdg;
    /* Heading; radians */

#if USE_JSON
    void toJSON (nlohmann::json& j, const string& name)
    {
        if (present)
        {
            convertToJSON(range_exceeded, "range_exceeded", j[name]);
            j[name]["value_gsp_ms"] = value_gsp;
            j[name]["value_hdg_rad"] = value_hdg;
        }
        else if (write_json_nulls)
            j[name] = nullptr;
    }
#endif
} t_Ground_Vector;

/* Height from 3D radar: */
typedef struct
{
    t_Bool present;
    /* Height from 3D radar present */
    t_Tres valid;
    /* Height from 3D radar valid */
    t_Real value;
    /* Height from 3D radar; metres */
    t_Si32 value_in_feet;
    /* Height from 3D radar; feet */

#if USE_JSON
    void toJSON (nlohmann::json& j, const string& name)
    {
        if (present)
        {
            convertToJSON(valid, "valid", j[name]);
            j[name]["value_m"] = value;
            j[name]["value_ft"] = value_in_feet;
        }
        else if (write_json_nulls)
            j[name] = nullptr;
    }
#endif
} t_Height_3D;

/* Indicated airspeed: */
typedef struct
{
    t_Bool present;
    /* Indicated airspeed present */
    t_Real value;
    /* Indicated airspeed; metres/second */

#if USE_JSON
    void toJSON (nlohmann::json& j, const string& name)
    {
        if (present)
            j[name] = value;
        else if (write_json_nulls)
            j[name] = nullptr;
    }
#endif
} t_Indicated_Airspeed;

/* Input formats: */
typedef enum
{
    e_input_format_undefined = 0,
    /* Undefined input format */
    e_input_format_ioss,
    /* SASS-C IOSS input */
    e_input_format_netto,
    /* Binary "netto" input */
    e_input_format_rff,
    /* RFF input */
    e_input_format_sequence_of_records,
    /* Sequence of records */
    e_input_format_sentinel
    /* Sentinel - DO NOT REMOVE */
    /* MUST BE THE LAST ELEMENT */
} t_Input_Format;

/* Intermediate state selected altitude: */
typedef struct
{
    t_Bool present;
    /* Intermediate state selected altitude present */
    t_Real value;
    /* Altitude; metres */
    t_Si32 value_in_feet;
    /* Altitude; feet */
    t_Byte value_sas;
    /* Source availability */
    t_Byte value_src;
    /* Source */
    /* 0 = unknown, 1 = aircraft altitude (holding altitude),
   2 = FCU/MCP selected altitude, 3 = FMS selected
   altitude
*/

#if USE_JSON
    void toJSON (nlohmann::json& j, const string& name)
    {
        if (present)
        {
            j[name]["value_m"] = value;
            j[name]["value_ft"] = value_in_feet;
            j[name]["value_sas"] = value_sas;
            j[name]["value_src"] = value_src;

            switch (value_src)
            {
            case 0:
                j[name]["value_src_info"] = "unknown";
                break;
            case 1:
                j[name]["value_src_info"] = "aircraft altitude (holding altitude)";
                break;
            case 2:
                j[name]["value_src_info"] = "FCU/MCP selected altitude";
                break;
            case 3:
                j[name]["value_src_info"] = "FMS selected";
                break;
            default:
                j[name]["value_src_info"] = nullptr;
                break;
            }
        }
        else if (write_json_nulls)
            j[name] = nullptr;
    }
#endif
} t_Intermediate_State_Selected_Altitude;

/* Link technology indicator: */
typedef struct
{
    t_Bool present;
    /* Link technology indicator present */
    t_Byte value;
    /* Link technology indicator */

#if USE_JSON
    void toJSON (nlohmann::json& j, const string& name)
    {
        if (present)
            j[name] = value;
        else if (write_json_nulls)
            j[name] = nullptr;
    }
#endif
} t_Link_Technology;

/* Listed sensor: */
typedef struct
{
    t_Bool asterix_categories[256];
    /* ASTERIX categories */
    t_Data_Format data_format;
    /* Data format */
    t_Ui16 data_source_identifier;
    /* Data source identifier (SAC/SIC) */
    t_Bool defined;
    /* Listed sensor defined */
    t_Ui16 line_number;
    /* Line number (in IOSS frame) */
    t_Bool line_number_defined;
    /* Line number defined */
    char *name;
    /* Sensor name */
    t_Bool name_defined;
    /* Sensor name defined */
    t_Ui16 sensor_number;
    /* Sensor number (in some context) */
    t_Bool sensor_number_defined;
    /* Sensor number defined */
} t_Listed_Sensor;

/* Load counter: */
typedef struct
{
    t_Ui32 current_count;
    /* Count for current observation period */
    t_Secs current_start_time;
    /* Start time of current observation period;
   seconds */
    t_Bool in_first_period;
    /* In first (incomplete) observation period */
    t_Secs next_start_time;
    /* Start time of next observation period;
   seconds */
    t_Real observation_period;
    /* Observation period; seconds */
    t_Bool started;
    /* Load counter started */
} t_Load_Counter;

/* MD5 context: */
typedef struct
{
    t_Byte buffer[64];
    /* Input buffer */
    t_Ui32 count[2];
    /* Number of bits modulo 2**64 */
    t_Ui32 state[4];
    /* State (ABCD) */
} t_MD5_Context;

/* MOPS version: */
typedef struct
{
    t_Bool present;
    /* MOPS version present */
    t_Byte value_ltt;
    /* Link technology type */
    t_Byte value_vn;
    /* Version number */
    t_Byte value_vns;
    /* Version not supported */

#if USE_JSON
    void toJSON (nlohmann::json& j, const string& name)
    {
        if (present)
        {
            j[name]["value_ltt"] = value_ltt;
            j[name]["value_vn"] = value_vn;
            j[name]["value_vns"] = value_vns;
        }
        else if (write_json_nulls)
            j[name] = nullptr;
    }
#endif
} t_MOPS_Version;

/* Mach number: */
typedef struct
{
    t_Bool present;
    /* Mach number present */
    t_Real value;
    /* Air speed as multiple of Mach speed */

#if USE_JSON
    void toJSON (nlohmann::json& j, const string& name)
    {
        if (present)
            j[name] = value;
        else if (write_json_nulls)
            j[name] = nullptr;
    }
#endif
} t_Mach_Number;

/* Magnetic heading: */
typedef struct
{
    t_Bool present;
    /* Magnetic heading present */
    t_Real value;
    /* Magnetic heading; radians */

#if USE_JSON
    void toJSON (nlohmann::json& j, const string& name)
    {
        if (present)
            j[name] = value;
        else if (write_json_nulls)
            j[name] = nullptr;
    }
#endif
} t_Magnetic_Heading;

/* Mapped position: */
typedef struct
{
    t_Bool present;
    /* Mapped position present */
    t_Real value_h;
    /* Height used for mapping; metres */
    t_Real value_u;
    /* Mapped position component; metres */
    t_Real value_v;
    /* Mapped position component; metres */

#if USE_JSON
    void toJSON (nlohmann::json& j, const string& name)
    {
        if (present)
        {
            j[name]["value_h"] = value_h;
            j[name]["value_u"] = value_u;
            j[name]["value_v"] = value_v;
        }
        else if (write_json_nulls)
            j[name] = nullptr;
    }
#endif
} t_Mapped_Position;

/* Measured amplitude: */
typedef struct
{
    t_Bool present;
    /* Measured amplitude present */
    t_Si16 value;
    /* Measured amplitude; dBm */

#if USE_JSON
    void toJSON (nlohmann::json& j, const string& name)
    {
        if (present)
            j[name] = value;
        else if (write_json_nulls)
            j[name] = nullptr;
    }
#endif
} t_Measured_Amplitude;

/* Measured azimuth: */
typedef struct
{
    t_Bool present;
    /* Measured azimuth present */
    t_Real value;
    /* Measured azimuth; radians */

#if USE_JSON
    void toJSON (nlohmann::json& j, const string& name)
    {
        if (present)
            j[name] = value;
        else if (write_json_nulls)
            j[name] = nullptr;
    }
#endif
} t_Measured_Azimuth;

/* Measured height: */
typedef struct
{
    t_Bool present;
    /* Measured height present */
    t_Real value;
    /* Measured height; metres */

#if USE_JSON
    void toJSON (nlohmann::json& j, const string& name)
    {
        if (present)
            j[name] = value;
        else if (write_json_nulls)
            j[name] = nullptr;
    }
#endif
} t_Measured_Height;

/* Measured information (within system track): */
typedef struct
{
    t_Bool h3d_present;
    /* Measured 3-D height present */
    t_Bool mda_present;
    /* Last measured mode 3/A code present */
    t_Bool mdc_present;
    /* Last measured mode C code present */
    t_Bool pos_present;
    /* Measured position present */
    t_Bool present;
    /* Measured information present */
    t_Bool sid_present;
    /* Sensor identification present */
    t_Bool typ_present;
    /* Report type present */
    t_Si16 value_h3d;
    /* Measured 3-D height; 25 feet */
    t_Ui16 value_mda;
    /* Last measured mode 3/A code */
    t_Bool value_mda_garbled;
    /* Last measured mode 3/A code garbled */
    t_Bool value_mda_invalid;
    /* Last measured mode 3/A code not valid(ated) */
    t_Bool value_mda_smoothed;
    /* Last measured mode 3/A code smoothed */
    t_Si16 value_mdc;
    /* Last measured mode C code; 25 feet */
    t_Bool value_mdc_garbled;
    /* Last measured mode C code garbled */
    t_Bool value_mdc_invalid;
    /* Last measured mode C code not valid(ated) */
    t_Real value_rho;
    /* Measured distance; metres */
    t_Ui16 value_sid;
    /* Sensor identification */
    t_Real value_theta;
    /* Measured azimuth; radians */
    t_Bool value_typ_mds;
    /* Report type: Mode S detection */
    t_Bool value_typ_mds_all_call;
    /* Report type: Mode S all-call */
    t_Bool value_typ_mds_roll_call;
    /* Report type: Mode S roll-call */
    t_Bool value_typ_psr;
    /* Report type: PSR detection */
    t_Bool value_typ_rab;
    /* Report type: report from field monitor */
    t_Bool value_typ_sim;
    /* Report type: simulated target report */
    t_Bool value_typ_ssr;
    /* Report type: SSR detection */
    t_Bool value_typ_tst;
    /* Report type: test target report */

#if USE_JSON
    void toJSON (nlohmann::json& j, const string& name)
    {
        if (present)
        {
            // t_Bool h3d_present;
            /* Measured 3-D height present */
            // t_Si16 value_h3d;
            /* Measured 3-D height; 25 feet */
            if (h3d_present)
                j[name]["value_h3d_ft"] = value_h3d;
            else if (write_json_nulls)
                j[name]["value_h3d_ft"] = nullptr;

            // t_Bool mda_present;
            /* Last measured mode 3/A code present */
            // t_Ui16 value_mda;
            /* Last measured mode 3/A code */
            // t_Bool value_mda_garbled;
            /* Last measured mode 3/A code garbled */
            // t_Bool value_mda_invalid;
            /* Last measured mode 3/A code not valid(ated) */
            // t_Bool value_mda_smoothed;
            /* Last measured mode 3/A code smoothed */
            if (mda_present)
            {
                j[name]["value_mda"] = value_mda;
                j[name]["value_mda_garbled"] = value_mda_garbled;
                j[name]["value_mda_invalid"] = value_mda_invalid;
                j[name]["value_mda_smoothed"] = value_mda_smoothed;
            }
            else if (write_json_nulls)
            {
                j[name]["value_mda"] = nullptr;
                j[name]["value_mda_garbled"] = nullptr;
                j[name]["value_mda_invalid"] = nullptr;
                j[name]["value_mda_smoothed"] = nullptr;
            }

            // t_Bool mdc_present;
            /* Last measured mode C code present */
            // t_Si16 value_mdc;
            /* Last measured mode C code; 25 feet */
            // t_Bool value_mdc_garbled;
            /* Last measured mode C code garbled */
            // t_Bool value_mdc_invalid;
            /* Last measured mode C code not valid(ated) */
            if (mdc_present)
            {
                j[name]["value_mdc_ft"] = value_mdc;
                j[name]["value_mdc_garbled"] = value_mdc_garbled;
                j[name]["value_mdc_invalid"] = value_mdc_invalid;
            }
            else if (write_json_nulls)
            {
                j[name]["value_mdc_ft"] = nullptr;
                j[name]["value_mdc_garbled"] = nullptr;
                j[name]["value_mdc_invalid"] = nullptr;
            }

            // t_Bool pos_present;
            /* Measured position present */
            // t_Real value_rho;
            /* Measured distance; metres */
            // t_Real value_theta;
            /* Measured azimuth; radians */
            if (pos_present)
            {
                j[name]["value_rho_m"] = value_theta;
                j[name]["value_theta_rad"] = value_theta;
            }
            else if (write_json_nulls)
            {
                j[name]["value_rho_m"] = nullptr;
                j[name]["value_theta_rad"] = nullptr;
            }

            // t_Bool sid_present;
            /* Sensor identification present */
            // t_Ui16 value_sid;
            /* Sensor identification */
            if (sid_present)
                j[name]["value_sid"] = value_sid;
            else if (write_json_nulls)
                j[name]["value_sid"] = nullptr;

            // t_Bool typ_present;
            /* Report type present */
            // t_Bool value_typ_mds;
            /* Report type: Mode S detection */
            // t_Bool value_typ_mds_all_call;
            /* Report type: Mode S all-call */
            // t_Bool value_typ_mds_roll_call;
            /* Report type: Mode S roll-call */
            // t_Bool value_typ_psr;
            /* Report type: PSR detection */
            // t_Bool value_typ_rab;
            /* Report type: report from field monitor */
            // t_Bool value_typ_sim;
            /* Report type: simulated target report */
            // t_Bool value_typ_ssr;
            /* Report type: SSR detection */
            // t_Bool value_typ_tst;
            /* Report type: test target report */

            if (typ_present)
            {
                j[name]["value_typ_mds"] = value_typ_mds;
                j[name]["value_typ_mds_all_call"] = value_typ_mds_all_call;
                j[name]["value_typ_mds_roll_call"] = value_typ_mds_roll_call;
                j[name]["value_typ_psr"] = value_typ_psr;
                j[name]["value_typ_rab"] = value_typ_rab;
                j[name]["value_typ_sim"] = value_typ_sim;
                j[name]["value_typ_ssr"] = value_typ_ssr;
                j[name]["value_typ_tst"] = value_typ_tst;
            }
            else if (write_json_nulls)
            {
                j[name]["value_typ_mds"] = nullptr;
                j[name]["value_typ_mds_all_call"] = nullptr;
                j[name]["value_typ_mds_roll_call"] = nullptr;
                j[name]["value_typ_psr"] = nullptr;
                j[name]["value_typ_rab"] = nullptr;
                j[name]["value_typ_sim"] = nullptr;
                j[name]["value_typ_ssr"] = nullptr;
                j[name]["value_typ_tst"] = nullptr;            }
        }
        else if (write_json_nulls)
            j[name] = nullptr;
    }
#endif

} t_Measured_Information;

/* Measured range: */
typedef struct
{
    t_Bool present;
    /* Measured range present */
    t_Real value;
    /* Measured range; metres */

#if USE_JSON
    void toJSON (nlohmann::json& j, const string& name)
    {
        if (present)
            j[name] = value;
        else if (write_json_nulls)
            j[name] = nullptr;
    }
#endif
} t_Measured_Range;

/* Message amplitude: */
typedef struct
{
    t_Bool present;
    /* Message amplitude present */
    t_Si16 value;
    /* Message amplitude; dBm */

#if USE_JSON
    void toJSON (nlohmann::json& j, const string& name)
    {
        if (present)
            j[name] = value;
        else if (write_json_nulls)
            j[name] = nullptr;
    }
#endif
} t_Message_Amplitude;

/* Message type: */
typedef struct
{
    t_Bool present;
    /* Message type present */
    t_Byte value;
    /* Message type */
} t_Message_Type;

/* MET information: */
typedef struct
{
    t_Bool present;
    /* MET information present */
    t_Bool present_tmp;
    /* Temperature present */
    t_Bool present_trb;
    /* Turbulence present */
    t_Bool present_wd;
    /* Wind direction present */
    t_Bool present_ws;
    /* Wind speed present */
    t_Real value_tmp;
    /* Temperature; °C */
    t_Byte value_trb;
    /* Turbulence */
    t_Real value_wd;
    /* Wind direction; radians */
    t_Real value_ws;
    /* Wind speed; metres/second */

#if USE_JSON
    void toJSON (nlohmann::json& j, const string& name)
    {
        if (present)
        {
            if (present_tmp)
                j[name]["value_tmp_c"] = value_tmp;
            else
                j[name]["value_tmp_c"] = nullptr;

            if (present_trb)
                j[name]["value_trb"] = value_trb;
            else
                j[name]["value_trb"] = nullptr;

            if (present_wd)
                j[name]["value_wd_rad"] = value_wd;
            else
                j[name]["value_wd_rad"] = nullptr;

            if (present_ws)
                j[name]["value_ws_ms"] = value_ws;
            else
                j[name]["value_ws_ms"] = nullptr;
        }
        else if (write_json_nulls)
            j[name] = nullptr;
    }
#endif
} t_Met_Information;

/* Metric height: */
typedef struct
{
    t_Tres is_relative;
    /* Metric height is relative to (some) aerodrome */
    t_Bool present;
    /* Metric height present */
    t_Real value;
    /* Metric height; metres */

#if USE_JSON
    void toJSON (nlohmann::json& j, const string& name)
    {
        if (present)
        {
            convertToJSON(is_relative, "is_relative", j[name]);
            j[name]["value_m"] = value;
        }
        else if (write_json_nulls)
            j[name] = nullptr;
    }
#endif
} t_Metric_Height;

/* SSR mode 4 information: */
typedef struct
{
    t_Tres invalid;
    /* SSR mode 4 information invalid */
    t_Bool present;
    /* SSR mode 4 information present */
    t_Ui16 value;
    /* SSR mode 4 information */
    /*  0 ... no SSR mode 4 interrogation,
    1 ... friendly target,
    2 ... unknown target,
    3 ... no reply */

#if USE_JSON
    void toJSON (nlohmann::json& j, const string& name)
    {
        if (present)
        {
            convertToJSON(invalid, "invalid", j[name]);
            j[name]["value"] = value;

            switch (value)
            {
            case 0:
                j[name]["value_info"] = "no SSR mode 4 interrogation";
                break;
            case 1:
                j[name]["value_info"] = "friendly target";
                break;
            case 2:
                j[name]["value_info"] = "unknown target";
                break;
            case 3:
                j[name]["value_info"] = "no reply";
                break;
            default:
                j[name]["value_info"] = nullptr;
            }
        }
        else if (write_json_nulls)
            j[name] = nullptr;
    }
#endif
} t_Mode_4_Info;

/* SSR mode C code: */
typedef struct
{
    t_Ui16 code;
    /* SSR mode C code */
    /* This is the SSR mode C code as sent down
   by the transponder */
    t_Ui16 code_confidence;
    /* SSR mode C code confidence */
    /* Not inverted, i.e. 0=high confidence */
    t_Bool code_confidence_present;
    /* SSR mode C code confidence present */
    t_Tres code_garbled;
    /* Garbled SSR mode C code */
    t_Tres code_invalid;
    /* Invalid SSR mode C code */
    t_Bool present;
    /* SSR mode C code present */

#if USE_JSON
    void toJSON (nlohmann::json& j, const string& name)
    {
        if (present)
        {
            j[name]["code"] = code;

            if (code_confidence_present)
                j[name]["code_confidence"] = code_confidence;
            else if (write_json_nulls)
                j[name]["code_confidence"] = nullptr;

            convertToJSON(code_garbled, "code_garbled", j[name]);
            convertToJSON(code_invalid, "code_invalid", j[name]);
        }
        else if (write_json_nulls)
            j[name] = nullptr;
    }
#endif
} t_Mode_C_Code;

/* SSR mode C height: */
typedef struct
{
    t_Tres garbled;
    /* Garbled SSR mode C height */
    t_Tres height_in_error;
    /* SSR mode C height is not a valid altitude,
   i.e. it is out of (regular) bounds */
    t_Tres in_25_feet;
    /* SSR mode C height in 25 feet resolution */
    t_Tres invalid;
    /* Invalid SSR mode C height */
    t_Bool present;
    /* SSR mode C height present */
    t_Real value;
    /* SSR mode C height; metres */
    t_Si32 value_in_feet;
    /* SSR mode C height; feet */

#if USE_JSON
    void toJSON (nlohmann::json& j, const string& name)
    {
        if (present)
        {
            convertToJSON(garbled, "garbled", j[name]);
            convertToJSON(height_in_error, "height_in_error", j[name]);
            convertToJSON(in_25_feet, "in_25_feet", j[name]);
            convertToJSON(invalid, "invalid", j[name]);

            j[name]["value_m"] = value;
            j[name]["value_ft"] = value_in_feet;
        }
        else if (write_json_nulls)
            j[name] = nullptr;
    }
#endif
} t_Mode_C_Height;

/* SSR mode C information: */
typedef struct
{
    t_Ui16 code;
    /* SSR mode C code (as sent down by the transponder) */
    t_Ui16 code_confidence;
    /* SSR mode C code confidence */
    /* Not inverted, i. e. 0=high confidence */
    t_Bool code_garbled;
    /* SSR mode C code garbled */
    t_Bool code_invalid;
    /* SSR mode C code not valid */
    t_Ui16 hits;
    /* Number of SSR mode C replies */
    t_Bool present;
    /* SSR mode C information present */

#if USE_JSON
    void toJSON (nlohmann::json& j, const string& name)
    {
        if (present)
        {
            j[name]["code"] = code;
            j[name]["code_confidence"] = code_confidence;
            j[name]["code_invalid"] = code_invalid;
            j[name]["hits"] = hits;
        }
        else if (write_json_nulls)
            j[name] = nullptr;
    }
#endif
} t_Mode_C_Info;

/* SSR mode 1, 2, 3/A information: */
typedef struct
{
    t_Ui16 code;
    /* SSR mode 1, 2, or 3/A code */
    t_Ui16 code_confidence;
    /* SSR mode 1, 2, or 3/A code confidence */
    /* Not inverted, i. e. 0=high confidence */
    t_Bool code_confidence_present;
    /* SSR mode 1, 2, or 3/A code confidence present */
    t_Tres code_garbled;
    /* SSR mode 1, 2, or 3/A code garbled */
    t_Tres code_invalid;
    /* SSR mode 1, 2, or 3/A code not valid */
    t_Tres code_smoothed;
    /* SSR mode 1, 2, or 3/A code smoothed */
    t_Bool present;
    /* SSR mode 1, 2, or 3/A information present */
    t_Ui16 replies;
    /* Number of SSR mode 1, 2, or 3/A replies */

#if USE_JSON
    void toJSON (nlohmann::json& j, const string& name)
    {
        if (present)
        {
            j[name]["code"] = code;

            if (code_confidence_present)
                j[name]["code_confidence"] = code_confidence;
            else if (write_json_nulls)
                j[name]["code_confidence"] = nullptr;

            convertToJSON(code_garbled, "code_garbled", j[name]);
            convertToJSON(code_invalid, "code_invalid", j[name]);
            convertToJSON(code_smoothed, "code_smoothed", j[name]);

            j[name]["replies"] = replies;
        }
        else if (write_json_nulls)
            j[name] = nullptr;
    }
#endif
} t_Mode_Info;

/* SSR mode S AC information: */
typedef struct
{
    t_Ui16 code;
    /* SSR mode S AC code */
    t_Ui16 code_confidence;
    /* SSR mode S AC code confidence */
    t_Bool invalid;
    /* SSR mode S AC information not valid */
    t_Bool present;
    /* SSR mode S AC information present */
    t_Bool undecodable;
    /* SSR mode S AC information undecodable */
    t_Si32 value;
    /* SSR mode S AC value; feet */
    t_Bool value_defined;
    /* SSR mode S AC value defined */
    t_Bool value_in_25_feet;
    /* SSR mode S AC value in 25 feet resolution */
} t_Mode_S_AC_Info;

/* SSR mode S DF information: */
typedef struct
{
    t_Bool present;
    /* SSR mode S DF information present */
    t_Byte value;
    /* SSR mode S DF */
} t_Mode_S_DF_Info;

/* SSR mode S flags: */
typedef struct
{
    t_Bool present;
    /* SSR mode S flags present */
    t_Byte value_aic;
    /* AIC */
    t_Byte value_arc;
    /* ARC (0 = 100 feet, 1 = 25 feet) */
    t_Byte value_b1a;
    /* B1A */
    t_Byte value_b1b;
    /* B1B */
    t_Byte value_cc;
    /* CC */
    t_Byte value_fs;
    /* FS */
    t_Byte value_mssc;
    /* MSSC */
    t_Tres value_si;
    /* SI */

#if USE_JSON
    void toJSON (nlohmann::json& j, const string& name)
    {
        if (present)
        {
            j[name]["value_aic"] = value_aic;
            j[name]["value_arc"] = value_arc;
            j[name]["value_b1a"] = value_b1a;
            j[name]["value_b1b"] = value_b1b;
            j[name]["value_cc"] = value_cc;
            j[name]["value_fs"] = value_fs;
            j[name]["value_mssc"] = value_mssc;

            convertToJSON(value_si, "value_si", j[name]);
        }
        else if (write_json_nulls)
            j[name] = nullptr;
    }
#endif
} t_Mode_S_Flags;

/* SSR mode S ID information: */
typedef struct
{
    t_Bool invalid;
    /* SSR mode S ID not valid */
    t_Bool present;
    /* SSR mode S ID information present */
    t_Ui16 value;
    /* SSR mode S ID */
} t_Mode_S_ID_Info;

/* SSR mode S MB data: */
typedef struct
{
    int count;
    /* Number of BDS registers */
    t_Bool present;
    /* SSR mode S MB data present */
    t_BDS_Register value[M_MAX_BDS_REGISTERS];
    /* Values of downloaded BDS registers */
#if USE_JSON
    void toJSON (nlohmann::json& j, const string& name)
    {
        if (present)
        {
            for (int cnt=0; cnt < count; ++cnt)
                value[cnt].toJSON(j, "bds"+std::to_string(cnt));
        }
        else if (write_json_nulls)
            j[name] = nullptr;

    }
#endif
} t_Mode_S_MB_Data;

/* SSR mode S ME information: */
typedef struct
{
    t_Byte me_bits[11];
    /* SSR mode S ME data bits */
    t_Byte me_conf[11];
    /* SSR mode S ME data confidence bits */
    t_Bool present;
    /* SSR mode S ME information present */
} t_Mode_S_ME_Info;

/* Partial time of day: */
typedef struct
{
    t_Bool present;
    /* Partial time of day present */
    t_Ui16 value;
    /* Partial time of day; 1/128 seconds */

#if USE_JSON
    void toJSON (nlohmann::json& j, const string& name)
    {
        if (present)
            j[name] = value;
        else if (write_json_nulls)
            j[name] = nullptr;

    }
#endif
} t_Partial_Time_of_Day;

/* Plot qualification: */
typedef struct
{
    t_Bool defined;
    /* Plot qualification defined */
    t_Bool parent_of_shadow;
    /* Parent of PSR shadow plot */
    t_Bool parent_of_split;
    /* Parent of split target */
    int qualification_factor;
    /* Qualification factor */
    t_Bool shadow;
    /* PSR shadow plot of CMB or mode S plot */
    t_Bool split;
    /* Split target */
    t_Bool split_pair;
    /* Split pair */

#if USE_JSON
    void toJSON (nlohmann::json& j, const string& name)
    {
        if (defined)
        {
            j[name]["parent_of_shadow"] = parent_of_shadow;
            j[name]["parent_of_split"] = parent_of_split;
            j[name]["qualification_factor"] = qualification_factor;
            j[name]["shadow"] = shadow;
            j[name]["split"] = split;
            j[name]["split_pair"] = split_pair;
        }
        else if (write_json_nulls)
            j[name] = nullptr;
    }
#endif
} t_Plot_Qualification;

/* Cartesian point: */
typedef struct
{
    t_Real x;
    /* x coordinate; metres */
    t_Real y;
    /* y coordinate; metres */
    t_Real z;
    /* z coordinate; metres */
} t_Point;

/* Polar window: */
typedef struct
{
    t_Bool present;
    /* Polar window present */
    t_Real value_azm_start;
    /* Azimuth of start angle; radians */
    t_Real value_azm_stop;
    /* Azimuth of stop angle; radians */
    t_Real value_rng_start;
    /* Start range; metres */
    t_Real value_rng_stop;
    /* Stop range; metres */
} t_Polar_Window;

/* Processing status: */
typedef struct
{
    t_Bool present;
    /* Processing status present */
    t_Byte value;
    /* Processing status */
} t_Processing_Status;

/* Quality indicators: */
typedef struct
{
    t_Bool nacp_present;
    /* NACp present */
    t_Bool nic_baro_present;
    /* NICbaro present */
    t_Bool present;
    /* Quality indicators present */
    t_Bool sil_present;
    /* SIL present */
    t_Byte value_nacp;
    /* NACp */
    t_Byte value_nic_baro;
    /* NICbaro */
    t_Byte value_nucp_or_nic;
    /* NUCp or NIC */
    t_Byte value_nucr_or_nacv;
    /* NUCr or NACv */
    t_Byte value_sil;
    /* SIL */

#if USE_JSON
    void toJSON (nlohmann::json& j, const string& name)
    {
        if (present)
        {
            if (nacp_present)
                j[name]["value_nacp"] = value_nacp;
            else if (write_json_nulls)
                j[name]["value_nacp"] = nullptr;

            if (nic_baro_present)
                j[name]["value_nic_baro"] = value_nic_baro;
            else if (write_json_nulls)
                j[name]["value_nic_baro"] = nullptr;

            if (sil_present)
                j[name]["value_sil"] = value_sil;
            else if (write_json_nulls)
                j[name]["value_sil"] = nullptr;

            j[name]["value_nucp_or_nic"] = value_nucp_or_nic;
            j[name]["value_nucr_or_nacv"] = value_nucr_or_nacv;
        }
        else if (write_json_nulls)
            j[name] = nullptr;
    }
#endif
} t_Quality_Indicators;

/* Radar configuration and status: */
typedef struct
{
    t_Ui16 dsi;
    /* Data source identifier (SAC/SIC) */
    t_Byte value;
    /* Association configuration and status */
} t_Radar_Conf;

typedef struct
{
    t_Ui16 count;
    /* Number of entries */
    t_Bool present;
    /* Radar configuration and status present */
    t_Radar_Conf value[M_MAX_NUMBER_OF_SENSORS];
    /* Values */
} t_Radar_Configuration;

/* Radar sector: */
typedef struct
{
    t_Secs computed_tod;
    /* Computed time of day; seconds */
    t_Bool computed_tod_present;
    /* Computed time of day present */
    t_Bool defined;
    /* Radar sector defined */
    t_Bool late_sct;
    /* Late sector crossing message */
    t_Bool missing;
    /* Missing radar sector */
    t_Secs reported_tod;
    /* Reported time of day; seconds */
    t_Bool reported_tod_present;
    /* Reported time of day present */
    t_Bool swapped;
    /* Swapped radar sector */
} t_Radar_Sector;

/* Selected altitude: */
typedef struct
{
    t_Bool present;
    /* Selected altitude present */
    t_Byte source;
    /* Source for selected altitude */
    /* 0=undefined, 1=aircraft, 2=MCP/FCU, 3=FMS */
    t_Si32 value;
    /* Selected altitude; feet */
    t_Tres value_alt_hold;
    /* MCP/FCU: ALT HOLD mode */
    t_Tres value_approach;
    /* MCP/FCU: APPROACH mode */
    t_Tres value_vnav;
    /* MCP/FCU: VNAV mode */

#if USE_JSON
    void toJSON (nlohmann::json& j, const string& name)
    {
        if (present)
        {
            j[name]["source"] = source;

            switch (source)
            {
            case 0:
                j[name]["source_info"] = "undefined";
                break;
            case 1:
                j[name]["source_info"] = "aircraft";
                break;
            case 2:
                j[name]["source_info"] = "MCP/FCU";
                break;
            case 3:
                j[name]["source_info"] = "FMS";
                break;

            default:
                if (write_json_nulls)
                    j[name]["source_info"] = nullptr;
                break;
            }

            j[name]["value_ft"] = value;
            convertToJSON(value_alt_hold, "value_alt_hold", j[name]);
            convertToJSON(value_approach, "value_approach", j[name]);
            convertToJSON(value_vnav, "value_vnav", j[name]);
        }
        else if (write_json_nulls)
            j[name] = nullptr;
    }
#endif
} t_Selected_Altitude;

/* Service status report: */
typedef struct
{
    t_Bool present;
    /* Service status report present */
    t_Byte value;
    /* Service status report */
} t_Service_Status_Report;

/* Radar scan information: */
typedef struct t_Radar_Scan
{
    t_Bool defined;
    /* Radar scan information defined */
    t_Ui16 last_observed_rsn;
    /* Last observed radar sector number */
    t_Bool last_observed_rsn_present;
    /* Last observed radar sector number present */
    t_Radar_Sector radar_sectors[M_MAX_NUMBER_OF_RADAR_SECTORS];
    /* Radar sectors */
    t_Ui32 scan_number;
    /* Scan number */
} t_Radar_Scan;

/* Radial speed: */
typedef struct
{
    t_Bool present;
    /* Radial speed present */
    t_Real value;
    /* Radial speed; metres/second */

#if USE_JSON
    void toJSON (nlohmann::json& j, const string& name)
    {
        if (present)
            j[name] = value;
        else if (write_json_nulls)
            j[name] = nullptr;
    }
#endif
} t_Radial_Speed;

/* Range difference: */
typedef struct
{
    t_Bool present;
    /* Range difference present */
    t_Real value;
    /* Range difference; metres */

#if USE_JSON
    void toJSON (nlohmann::json& j, const string& name)
    {
        if (present)
            j[name] = value;
        else if (write_json_nulls)
            j[name] = nullptr;
    }
#endif
} t_Range_Difference;

/* Raw Doppler speed: */
typedef struct
{
    t_Real ambiguity_range;
    /* Raw Doppler speed ambiguity range; metres/second */
    t_Bool present;
    /* Raw Doppler speed present */
    t_Ui16 transmitter_frequency;
    /* Transmitter frequency; MHz */
    t_Real value;
    /* Raw Doppler speed; metres/second */

#if USE_JSON
    void toJSON (nlohmann::json& j, const string& name)
    {
        if (present)
        {
            j[name]["ambiguity_range_ms"] = ambiguity_range;
            j[name]["transmitter_frequency_mhz"] = transmitter_frequency;
            j[name]["value_ms"] = value;
        }
        else if (write_json_nulls)
            j[name] = nullptr;
    }
#endif
} t_Raw_Doppler_Speed;

/* Received power: */
typedef struct
{
    t_Bool present;
    /* Received power present */
    t_Si16 value;
    /* Received power; dBm */

#if USE_JSON
    void toJSON (nlohmann::json& j, const string& name)
    {
        if (present)
            j[name] = value;
        else if (write_json_nulls)
            j[name] = nullptr;
    }
#endif
} t_Received_Power;

/* Replies count: */
typedef struct
{
    t_Bool present;
    /* Replies count present */
    t_Ui16 value;
    /* Replies count */

#if USE_JSON
    void toJSON (nlohmann::json& j, const string& name)
    {
        if (present)
            j[name] = value;
        else if (write_json_nulls)
            j[name] = nullptr;
    }
#endif
} t_Replies_Count;

/* Reported track quality: */
typedef struct
{
    t_Ui16 length;
    /* Length */
    t_Bool present;
    /* Reported track quality present */
    t_Ui16 value;
    /* Reported track quality */
    /* Encoded values are NOT standardized */

#if USE_JSON
    void toJSON (nlohmann::json& j, const string& name)
    {
        if (present)
        {
            j[name]["length"] = length;
            j[name]["value"] = value;
        }
        else if (write_json_nulls)
            j[name] = nullptr;
    }
#endif
} t_Reported_Track_Quality;

/* Reported track status: */
typedef struct
{
    t_Byte attitude;
    /* Vertical attitude or trend */
    /* 0 = maintaining, 1 = climbing,
   2 = descending, 3 = invalid */
    t_Tres coasted;
    /* Coasted (extrapolated) track */
    t_Tres confirmed;
    /* Confirmed track */
    t_Tres doubtful_association;
    /* Doubtful association */
    t_Tres ghost;
    /* Ghost track */
    t_Tres horizontal_manoeuvre;
    /* Horizontal manoeuvre */
    t_Bool present;
    /* Reported track status present */
    t_Tres primary_track;
    /* Primary track */
    t_Tres secondary_track;
    /* Secondary track */
    t_Tres smoothed;
    /* Smoothed position */
    t_Tres tre;
    /* Last report for track */

#if USE_JSON
    void toJSON (nlohmann::json& j, const string& name)
    {
        if (present)
        {
            j[name]["attitude"] = attitude;

            switch (attitude)
            {
            case 0:
                j[name]["attitude_info"] = "maintaining";
                break;
            case 1:
                j[name]["attitude_info"] = "climbing";
                break;
            case 2:
                j[name]["attitude_info"] = "descending";
                break;
            case 3:
                j[name]["attitude_info"] = "invalid";
                break;

            default:
                if (write_json_nulls)
                    j[name]["attitude_info"] = nullptr;
                break;
            }

            convertToJSON(coasted, "coasted", j[name]);
            convertToJSON(confirmed, "confirmed", j[name]);
            convertToJSON(doubtful_association, "doubtful_association", j[name]);
            convertToJSON(ghost, "ghost", j[name]);
            convertToJSON(horizontal_manoeuvre, "horizontal_manoeuvre", j[name]);
            convertToJSON(primary_track, "primary_track", j[name]);
            convertToJSON(secondary_track, "secondary_track", j[name]);
            convertToJSON(smoothed, "smoothed", j[name]);
            convertToJSON(tre, "tre", j[name]);
        }
        else if (write_json_nulls)
            j[name] = nullptr;
    }
#endif
} t_Reported_Track_Status;

/* Roll angle: */
typedef struct
{
    t_Bool present;
    /* Roll angle present */
    t_Real value;
    /* Roll angle; radians */

#if USE_JSON
    void toJSON (nlohmann::json& j, const string& name)
    {
        if (present)
            j[name] = value;
        else if (write_json_nulls)
            j[name] = nullptr;
    }
#endif
} t_Roll_Angle;

/* Runlength: */
typedef struct
{
    t_Bool present;
    /* Runlength present */
    t_Real value;
    /* Runlength; degrees */
    t_Ui16 value_in_acps;
    /* Runlength; ACPs */

#if USE_JSON
    void toJSON (nlohmann::json& j, const string& name)
    {
        if (present)
        {
            j[name]["value_deg"] = value;
            j[name]["value_acps"] = value_in_acps;
        }
        else if (write_json_nulls)
            j[name] = nullptr;
    }
#endif
} t_Runlength;

/* SDPS configuration and status: */
typedef struct
{
    t_Bool present;
    /* SDPS configuration and status present */
    t_Byte value_nogo;
    /* Value of operational status */
    /* 0 = operational, 1 = degraded,
   2 = not currently connected, 3 = inknown */
    t_Byte value_ovl;
    /* Value of overload status */
    /* 0 = default, 1 = overload */
    t_Byte value_pss;
    /* Value of processing system status */
    /* 0 = not applicable, 1 = SDPS-1 selected,
   2 = SDPS-2 selected, 3 = SDPS-3 selected */
    t_Byte value_tsv;
    /* Value of time source status */
    /* 0 = default, 1 = invalid time source */
} t_SDPS_Configuration_and_Status;

/* Sector number: */
typedef struct
{
    t_Bool present;
    /* Sector number present */
    t_Ui16 value;
    /* Sector number */
    /* 0 = (radar) north crossing */

#if USE_JSON
    void toJSON (nlohmann::json& j, const string& name)
    {
        if (present)
            j[name] = value;
        else if (write_json_nulls)
            j[name] = nullptr;
    }
#endif
} t_Sector_Number;

/* Selected altitude: */
typedef struct
{
    t_Bool present;
    /* Selected altitude present */
    t_Si32 value;
    /* Selected altitude; feet */
    t_Byte value_sas;
    /* Source of selected altitude provided */
    t_Byte value_src;
    /* Source of selected altitude */
    /* 0 = unknown, 1 = aircraft altitude,
   2 = FCU/MCP selected altitude,
   3 = FMS selected altitude */

#if USE_JSON
    void toJSON (nlohmann::json& j, const string& name)
    {
        if (present)
        {
            j[name]["value_ft"] = value;
            j[name]["value_sas"] = value_sas;
            j[name]["value_src"] = value_src;

            switch (value_src) {
            case 0:
                j[name]["value_src_info"] = "unknown";
                break;
            case 1:
                j[name]["value_src_info"] = "aircraft altitude";
                break;
            case 2:
                j[name]["value_src_info"] = "FCU/MCP selected altitude";
                break;
            case 3:
                j[name]["value_src_info"] = "FMS selected altitude";
                break;
            default:
                if (write_json_nulls)
                    j[name]["value_src_info"] = nullptr;
                break;
            }
        }
        else if (write_json_nulls)
            j[name] = nullptr;
    }
#endif
} t_Strk_Selected_Altitude;

/* Sensor identity: */
typedef struct
{
    t_Bool orig_defined;
    /* Original SAC/SIC defined */
    t_Bool rsap_defined;
    /* RSAP, i. e. RMCDE SAC/SIC, defined */
    t_Ui16 orig;
    /* Original SAC/SIC */
    t_Ui16 rsap;
    /* RSAP, i. e. RMCDE SAC/SIC */
} t_Sensor_Ident;

/* Sensor number: */
typedef struct
{
    t_Bool present;
    /* Sensor number present */
    int value;
    /* Sensor number */
    /* Must be from range 1 ... MAX_NUMBER_OF_SENSORS */

#if USE_JSON
    void toJSON (nlohmann::json& j, const string& name)
    {
        if (present)
            j[name] = value;
        else if (write_json_nulls)
            j[name] = nullptr;
    }
#endif
} t_Sensor_Number;

/* Service identification: */
typedef struct
{
    t_Bool present;
    /* Service identification present */
    t_Byte value;
    /* Service identification */

#if USE_JSON
    void toJSON (nlohmann::json& j, const string& name)
    {
        if (present)
            j[name] = value;
        else if (write_json_nulls)
            j[name] = nullptr;
    }
#endif
} t_Service_Identification;

/* Service management: */
typedef struct
{
    t_Bool present;
    /* Service management present */
    t_Real value_rp;
    /* Report period; seconds */

#if USE_JSON
    void toJSON (nlohmann::json& j, const string& name)
    {
        if (present)
            j[name] = value_rp;
        else if (write_json_nulls)
            j[name] = nullptr;
    }
#endif
} t_Service_Management;

/* Statistical item: */
typedef struct
{
    t_Real avg_value;
    /* Average of observed values */
    t_Bool avg_value_present;
    /* Average of observed values present */
    t_Ui32 count;
    /* Number of values loaded into the item */
    t_Bool is_initiated;
    /* Item is initiated */
    t_Bool is_loaded;
    /* Item is loaded, i.e. at least one value
   has been evaluated */
    t_Real max_value;
    /* Max observed value */
    t_Real min_value;
    /* Min observed value */
    t_Real rms_value;
    /* RMS of observed values */
    t_Bool rms_value_present;
    /* RMS of observed values present */
    t_Real std_value;
    /* Standard deviation of observed values */
    t_Bool std_value_present;
    /* Standard deviation of observed values present */
    t_Real sum;
    /* Sum of observed values */
    t_Real sum_squared;
    /* Sum of observed values squared */
} t_Statistical_Item;

/* Slant range: */
typedef struct
{
    t_Bool present;
    /* Slant range present */
    t_Real value;
    /* Slant range; metres */
} t_Slant_Range;

/* Step number: */
typedef struct
{
    t_Bool present;
    /* Step number present */
    t_Ui16 value;
    /* Step number */

#if USE_JSON
    void toJSON (nlohmann::json& j, const string& name)
    {
        if (present)
            j[name] = value;
        else if (write_json_nulls)
            j[name] = nullptr;
    }
#endif
} t_Step_Number;

/* SSR mode 2 or 3/A information within system track: */
typedef struct
{
    t_Ui16 code;
    /* SSR mode code */
    t_Bool code_changed;
    /* SSR mode code changed */
    t_Bool code_garbled;
    /* SSR mode code garbled */
    t_Bool code_invalid;
    /* SSR mode code not valid(ated) */
    t_Bool present;
    /* SSR mode information present */

#if USE_JSON
    void toJSON (nlohmann::json& j, const string& name)
    {
        if (present)
        {
            j[name]["code"] = code;
            j[name]["code_changed"] = code_changed;
            j[name]["code_garbled"] = code_garbled;
            j[name]["code_invalid"] = code_invalid;
        }
        else if (write_json_nulls)
            j[name] = nullptr;
    }
#endif
} t_Strk_Mode_Info;

/* SSR mode 5 data within system track: */
typedef struct
{
    t_Bool em1_present;
    /* Extended SSR mode 1 code present */
    t_Bool ga_present;
    /* GNSS-derived altitude present */
    t_Bool pnm_present;
    /* PIN code, national origin, and mission code present */
    t_Bool pos_present;
    /* WGS-84 position present */
    t_Bool present;
    /* SSR mode 5 data present */
    t_Bool sum_present;
    /* SSR mode 5 summary present */
    t_Bool tos_present;
    /* Time offset present */
    t_Ui16 value_em1;
    /* Extended SSR mode 1 code */
    t_Si16 value_ga;
    /* GNSS-derived altitude; 25 feet */
    t_Real value_lat;
    /* WGS-84 latitude; degrees */
    t_Real value_lon;
    /* WGS-84 longitude; degrees */
    t_Byte value_mis;
    /* Mission code */
    t_Byte value_nat;
    /* National origin */
    t_Ui16 value_pin;
    /* PIN code */
    t_Byte value_res;
    /* Resolution of GNSS-derived altitude */
    t_Byte value_sum;
    /* SSR mode 5 summary */
    t_Byte value_tos;
    /* Time offset for position and altitude; 1/128 seconds */
    t_Byte value_xp;
    /* X pulse presence */
    t_Bool xp_present;
    /* X pulse presence information present */

#if USE_JSON
    void toJSON (nlohmann::json& j, const string& name)
    {
        if (present)
        {
            // t_Bool em1_present;
            /* Extended SSR mode 1 code present */
            // t_Ui16 value_em1;
            /* Extended SSR mode 1 code */
            if (em1_present)
                j[name]["value_em1"] = value_em1;
            else if (write_json_nulls)
                j[name]["value_em1"] = nullptr;

            // t_Bool ga_present;
            /* GNSS-derived altitude present */
            // t_Si16 value_ga;
            /* GNSS-derived altitude; 25 feet */
            // t_Byte value_res;
            /* Resolution of GNSS-derived altitude */
            if (ga_present)
            {
                j[name]["value_ga"] = value_ga;
                j[name]["value_res"] = value_res;
            }
            else if (write_json_nulls)
            {
                j[name]["value_ga"] = nullptr;
                j[name]["value_res"] = nullptr;
            }

            // t_Bool pnm_present;
            /* PIN code, national origin, and mission code present */
            // t_Byte value_mis;
            /* Mission code */
            // t_Byte value_nat;
            /* National origin */
            // t_Ui16 value_pin;
            /* PIN code */
            if (pnm_present)
            {
                j[name]["value_mis"] = value_mis;
                j[name]["value_nat"] = value_nat;
                j[name]["value_pin"] = value_pin;
            }
            else if (write_json_nulls)
            {
                j[name]["value_mis"] = nullptr;
                j[name]["value_nat"] = nullptr;
                j[name]["value_pin"] = nullptr;
            }

            // t_Bool pos_present;
            /* WGS-84 position present */
            // t_Real value_lat;
            /* WGS-84 latitude; degrees */
            // t_Real value_lon;
            /* WGS-84 longitude; degrees */
            if (pos_present)
            {
                j[name]["value_lat_deg"] = value_lat;
                j[name]["value_lon_deg"] = value_lon;
            }
            else if (write_json_nulls)
            {
                j[name]["value_lat_deg"] = nullptr;
                j[name]["value_lon_deg"] = nullptr;
            }

            // t_Bool sum_present;
            /* SSR mode 5 summary present */
            // t_Byte value_sum;
            /* SSR mode 5 summary */
            if (sum_present)
                j[name]["value_sum"] = value_sum;
            else if (write_json_nulls)
                j[name]["value_sum"] = nullptr;

            // t_Bool tos_present;
            /* Time offset present */
            // t_Byte value_tos;
            /* Time offset for position and altitude; 1/128 seconds */
            if (tos_present)
                j[name]["value_tos"] = value_tos;
            else if (write_json_nulls)
                j[name]["value_tos"] = nullptr;

            // t_Bool xp_present;
            /* X pulse presence information present */
            // t_Byte value_xp;
            /* X pulse presence */
            if (xp_present)
                j[name]["value_xp"] = value_xp;
            else if (write_json_nulls)
                j[name]["value_xp"] = value_xp;

        }
        else if (write_json_nulls)
            j[name] = nullptr;
    }
#endif
} t_Strk_Mode5_Data;

/* SSR mode 5 information within system track: */
typedef struct
{
    t_Bool present;
    /* SSR mode 5 information present */
    t_Byte value;
    /* SSR mode 5 code */

#if USE_JSON
    void toJSON (nlohmann::json& j, const string& name)
    {
        if (present)
            j[name] = value;
        else if (write_json_nulls)
            j[name] = nullptr;
    }
#endif
} t_Strk_Mode5_Info;

/* Calculated mode of flight within system track: */
typedef struct
{
    t_Bool present;
    /* Calculated mode of flight present */
    t_Byte value_longitudinal;
    /* Longitudinal acceleration */
    /* 0 ... constant groundspeed
   1 ... intentionally increasing groundspeed
   2 ... intentionally decreasing groundspeed
   3 ... undetermined
*/
    t_Byte value_transversal;
    /* Transversal acceleration */
    /* 0 ... constant course
   1 ... intentional right turn
   2 ... intentional left turn
   3 ... undetermined
*/
    t_Byte value_vertical;
    /* Vertical rate */
    /* 0 ... level flight
   1 ... climb
   2 ... descent
   3 ... undetermined
*/

#if USE_JSON
    void toJSON (nlohmann::json& j, const string& name)
    {
        if (present)
        {
            switch (value_longitudinal) {
            case 0:
                j[name]["value_longitudinal"] = "constant groundspeed";
                break;
            case 1:
                j[name]["value_longitudinal"] = "intentionally increasing groundspeed";
                break;
            case 2:
                j[name]["value_longitudinal"] = "intentionally decreasing groundspeed";
                break;
            case 3:
                j[name]["value_longitudinal"] = "undetermined";
                break;
            default:
                if (write_json_nulls)
                    j[name]["value_longitudinal"] = nullptr;
                break;
            }

            switch (value_transversal) {
            case 0:
                j[name]["value_transversal"] = "constant course";
                break;
            case 1:
                j[name]["value_transversal"] = "intentional right turn";
                break;
            case 2:
                j[name]["value_transversal"] = "intentional left turn";
                break;
            case 3:
                j[name]["value_transversal"] = "undetermined";
                break;
            default:
                if (write_json_nulls)
                    j[name]["value_transversal"] = nullptr;
                break;
            }

            switch (value_vertical) {
            case 0:
                j[name]["value_vertical"] = "level flight";
                break;
            case 1:
                j[name]["value_vertical"] = "climb";
                break;
            case 2:
                j[name]["value_vertical"] = "descent";
                break;
            case 3:
                j[name]["value_vertical"] = "undetermined";
                break;
            default:
                if (write_json_nulls)
                    j[name]["value_vertical"] = nullptr;
                break;
            }
        }
        else if (write_json_nulls)
            j[name] = nullptr;
    }
#endif
} t_Strk_Mode_of_Flight;

/* Target size and orientation within system track: */
typedef struct
{
    t_Bool length_present;
    /* Length present */
    t_Bool orientation_present;
    /* Orientation present */
    t_Bool present;
    /* Target size and orientation present */
    t_Real value_length;
    /* Length of target; metres */
    t_Real value_orientation;
    /* Orientation of target; degrees */
    t_Real value_width;
    /* Width of target; metres */
    t_Bool width_present;
    /* Width present */

#if USE_JSON
    void toJSON (nlohmann::json& j, const string& name)
    {
        if (present)
        {
            // t_Bool length_present;
            /* Length present */
            // t_Real value_length;
            /* Length of target; metres */
            if (length_present)
                j[name]["value_length_m"] = value_length;
            else if (write_json_nulls)
                j[name]["value_length_m"] = nullptr;

            // t_Bool orientation_present;
            /* Orientation present */
            // t_Real value_orientation;
            /* Orientation of target; degrees */
            if (orientation_present)
                j[name]["value_orientation_deg"] = value_orientation;
            else if (write_json_nulls)
                j[name]["value_orientation_deg"] = nullptr;

            // t_Bool width_present;
            /* Width present */
            // t_Real value_width;
            /* Width of target; metres */
            if (width_present)
                j[name]["value_width_m"] = value_width;
            else if (write_json_nulls)
                j[name]["value_width_m"] = nullptr;
        }
        else if (write_json_nulls)
            j[name] = nullptr;
    }
#endif
} t_Strk_Target_Size;

/* System track update ages: */
typedef struct
{
    t_Bool ads_present;
    /* Age of last ADS-C report present */
    t_Bool es_present;
    /* Age of last 1090 Extended Squitter ADS-B report
   present */
    t_Bool lop_present;
    /* Age of last magnetic loop detection present */
    t_Bool mds_present;
    /* Age of last mode S detection present */
    t_Bool mlt_present;
    /* Age of last MLT detection present */
    t_Bool present;
    /* System track update ages present */
    t_Bool psr_present;
    /* Age of last PSR detection present */
    t_Bool ssr_present;
    /* Age of last SSR detection present */
    t_Bool trk_present;
    /* Actual track age present */
    t_Bool uat_present;
    /* Age of last UAT ADS-B report present */
    t_Real value_ads;
    /* Age of last ADS-C report used to update
   this track; seconds */
    t_Real value_es;
    /* Age of last 1090 Extended Squitter ADS-B report used to
   update this track; seconds */
    t_Real value_lop;
    /* Age of last magnetic loop detection used to update
   this track; seconds */
    t_Real value_mds;
    /* Age of last mode S detection used to update
   this track; seconds */
    t_Real value_mlt;
    /* Age of last MLT detection used to update
   this track; seconds */
    t_Real value_psr;
    /* Age of last PSR detection used to update
   this track; seconds */
    t_Real value_ssr;
    /* Age of last SSR detection used to update
   this track; seconds */
    t_Real value_trk;
    /* Actual track age since first occurence; seconds */
    t_Real value_uat;
    /* Age of last UAT ADS-B report used to update
   this track; seconds */
    t_Real value_vdl;
    /* Age of last VDL Mode 4 ADS-B report used to update
   this track; seconds */
    t_Bool vdl_present;
    /* Age of last VDL Mode 4 ADS-B report present */

#if USE_JSON
    void toJSON (nlohmann::json& j, const string& name)
    {
        if (present)
        {
            // t_Bool ads_present;
            /* Age of last ADS-C report present */
            // t_Real value_ads;
            /* Age of last ADS-C report used to update
           this track; seconds */
            if (ads_present)
                j[name]["value_ads"] = value_ads;
            else if (write_json_nulls)
                j[name]["value_ads"] = nullptr;

            // t_Bool es_present;
            /* Age of last 1090 Extended Squitter ADS-B report
           present */
            // t_Real value_es;
            /* Age of last 1090 Extended Squitter ADS-B report used to
           update this track; seconds */
            if (es_present)
                j[name]["value_es"] = value_es;
            else if (write_json_nulls)
                j[name]["value_es"] = nullptr;

            // t_Bool lop_present;
            /* Age of last magnetic loop detection present */
            // t_Real value_lop;
            /* Age of last magnetic loop detection used to update
           this track; seconds */
            if (lop_present)
                j[name]["value_lop"] = value_lop;
            else if (write_json_nulls)
                j[name]["value_lop"] = nullptr;

            // t_Bool mds_present;
            /* Age of last mode S detection present */
            // t_Real value_mds;
            /* Age of last mode S detection used to update
           this track; seconds */
            if (mds_present)
                j[name]["value_mds"] = value_mds;
            else if (write_json_nulls)
                j[name]["value_mds"] = nullptr;

            // t_Bool mlt_present;
            /* Age of last MLT detection present */
            // t_Real value_mlt;
            /* Age of last MLT detection used to update
           this track; seconds */
            if (mlt_present)
                j[name]["value_mlt"] = value_mlt;
            else if (write_json_nulls)
                j[name]["value_mlt"] = nullptr;

            // t_Bool psr_present;
            /* Age of last PSR detection present */
            // t_Real value_psr;
            /* Age of last PSR detection used to update
           this track; seconds */
            if (psr_present)
                j[name]["value_psr"] = value_psr;
            else if (write_json_nulls)
                j[name]["value_psr"] = nullptr;

            // t_Bool ssr_present;
            /* Age of last SSR detection present */
            // t_Real value_ssr;
            /* Age of last SSR detection used to update
           this track; seconds */
            if (ssr_present)
                j[name]["value_ssr"] = value_ssr;
            else if (write_json_nulls)
                j[name]["value_ssr"] = nullptr;

            // t_Bool trk_present;
            /* Actual track age present */
            // t_Real value_trk;
            /* Actual track age since first occurence; seconds */
            if (trk_present)
                j[name]["value_trk"] = value_trk;
            else if (write_json_nulls)
                j[name]["value_trk"] = nullptr;

            // t_Bool uat_present;
            /* Age of last UAT ADS-B report present */
            // t_Real value_uat;
            /* Age of last UAT ADS-B report used to update
           this track; seconds */
            if (uat_present)
                j[name]["value_uat"] = value_uat;
            else if (write_json_nulls)
                j[name]["value_uat"] = nullptr;

            // t_Bool vdl_present;
            /* Age of last VDL Mode 4 ADS-B report present */
            // t_Real value_vdl;
            /* Age of last VDL Mode 4 ADS-B report used to update
           this track; seconds */
            if (vdl_present)
                j[name]["value_vdl"] = value_vdl;
            else if (write_json_nulls)
                j[name]["value_vdl"] = nullptr;
        }
        else if (write_json_nulls)
            j[name] = nullptr;
    }
#endif
} t_Strk_Update_Ages;

/* Calculated vertical rate within system track: */
typedef struct
{
    t_Bool present;
    /* Calculated vertical rate present */
    t_Real value;
    /* Calculated vertical rate; metres/second */

#if USE_JSON
    void toJSON (nlohmann::json& j, const string& name)
    {
        if (present)
            j[name] = value;
        else if (write_json_nulls)
            j[name] = nullptr;
    }
#endif
} t_Strk_Vertical_Rate;

/* Calculated WGS-84 position within system track: */
typedef struct
{
    t_Bool present;
    /* WGS-84 position present */
    t_Real value_latitude;
    /* WGS-84 latitude; radians */
    t_Real value_longitude;
    /* WGS-84 longitude; radians */

#if USE_JSON
    void toJSON (nlohmann::json& j, const string& name)
    {
        if (present)
        {
            j[name]["value_latitude_rad"] = value_latitude;
            j[name]["value_longitude_rad"] = value_longitude;
        }
        else if (write_json_nulls)
            j[name] = nullptr;
    }
#endif
} t_Strk_WGS84_Position;

/* Target address type: */
typedef struct
{
    t_Bool present;
    /* Target address type present */
    t_Target_Address_Enum_Type value;
    /* Target address enumeration type */

#if USE_JSON
    void toJSON (nlohmann::json& j, const string& name)
    {
        if (present)
            convertToJSON(value, name, j);
        else if (write_json_nulls)
            j[name] = nullptr;
    }
#endif
} t_Target_Address_Type;

/* Target status: */
typedef struct
{
    t_Bool present;
    /* Target status present */
    t_Byte value_ps;
    /* Priority status */
    t_Byte value_ss;
    /* Surveillance status */

#if USE_JSON
    void toJSON (nlohmann::json& j, const string& name)
    {
        if (present)
        {
            j[name]["value_ps"] = value_ps;
            j[name]["value_ss"] = value_ss;
        }
        else if (write_json_nulls)
            j[name] = nullptr;
    }
#endif
} t_Target_Status;

/* Time delay: */
typedef struct
{
    t_Bool present;
    /* Time delay present */
    t_Secs value;
    /* Time delay; seconds */
    /* Difference between receiving and detection time */

#if USE_JSON
    void toJSON (nlohmann::json& j, const string& name)
    {
        if (present)
            j[name] = value;
        else if (write_json_nulls)
            j[name] = nullptr;
    }
#endif
} t_Time_Delay;

/* Time of day: */
typedef struct
{
    t_Bool present;
    /* Time of day present */
    t_Secs value;
    /* Time of day; seconds */

#if USE_JSON
    void toJSON (nlohmann::json& j, const string& name)
    {
        if (present)
            j[name] = value;
        else if (write_json_nulls)
            j[name] = nullptr;
    }
#endif
} t_Time_of_Day;

/* Timejump information: */
typedef struct
{
    t_Bool jump_occured;
    /* Timejump occured */
    t_Ui16 jump_type;
    /* Type of timejump */
    t_Bool present;
    /* Timejump information present */
    t_Secs value;
    /* Estimated timejump value; seconds */
} t_Timejump_Info;

/* Track angle rate: */
typedef struct
{
    t_Bool present;
    /* Track angle rate present */
    t_Real value;
    /* Track angle rate; radians/second */

#if USE_JSON
    void toJSON (nlohmann::json& j, const string& name)
    {
        if (present)
            j[name] = value;
        else if (write_json_nulls)
            j[name] = nullptr;
    }
#endif
} t_Track_Angle_Rate;

/* Track number: */
typedef struct
{
    t_Bool present;
    /* Track number present */
    t_Ui16 value;
    /* Track number */

#if USE_JSON
    void toJSON (nlohmann::json& j, const string& name)
    {
        if (present)
            j[name] = value;
        else if (write_json_nulls)
            j[name] = nullptr;
    }
#endif
} t_Track_Number;

/* True airspeed: */
typedef struct
{
    t_Bool present;
    /* True airspeed present */
    t_Tres range_exceeded;
    /* "Range exceeded" indication */
    t_Real value;
    /* True airspeed; metres/second */

#if USE_JSON
    void toJSON (nlohmann::json& j, const string& name)
    {
        if (present)
        {
            convertToJSON(range_exceeded, "range_exceeded", j[name]);
            j[name]["value_ms"] = value;
        }
        else if (write_json_nulls)
            j[name] = nullptr;
    }
#endif
} t_True_Airspeed;

/* User number: */
typedef struct
{
    t_Bool present;
    /* User number present */
    t_Byte value;
    /* User number */
} t_User_Number;

/* Vehicle fleet identification: */
typedef struct
{
    t_Bool present;
    /* Vehicle fleet identification present */
    int value;
    /* Vehicle fleet identification */

#if USE_JSON
    void toJSON (nlohmann::json& j, const string& name)
    {
        if (present)
            j[name] = value;
        else if (write_json_nulls)
            j[name] = nullptr;
    }
#endif
} t_Vehicle_Fleet_Identification;

/* Velocity accuracy: */
typedef struct
{
    t_Bool present;
    /* Velocity accuracy present */
    t_Byte value;
    /* Velocity accuracy */

#if USE_JSON
    void toJSON (nlohmann::json& j, const string& name)
    {
        if (present)
            j[name] = value;
        else if (write_json_nulls)
            j[name] = nullptr;
    }
#endif
} t_Velocity_Accuracy;

/* List of warning/error conditions: */
typedef struct
{
    t_Ui16 count;
    /* Number of warning/error conditions */
    t_Byte list[M_MAX_NUMBER_OF_WEC];
    /* List of warning/error conditions */
    t_Bool present;
    /* List of warning/error conditions present */

#if USE_JSON
    void toJSON (nlohmann::json& j, const string& name)
    {
        if (present)
        {
            for (unsigned int cnt=0; cnt < count; ++cnt)
                j[name].push_back (list[cnt]);
        }
        else if (write_json_nulls)
            j[name] = nullptr;
    }
#endif
} t_WEC_List;

/* WGS-84 altitude: */
typedef struct
{
    t_Bool present;
    /* WGS-84 altitude present */
    t_Real value;
    /* WGS-84 altitude; metres */

#if USE_JSON
    void toJSON (nlohmann::json& j, const string& name)
    {
        if (present)
            j[name] = value;
        else if (write_json_nulls)
            j[name] = nullptr;
    }
#endif
} t_WGS84_Altitude;

/* Accuracy of WGS-84 altitude: */
typedef struct
{
    t_Bool present;
    /* Accuracy of WGS-84 altitude present */
    t_Real value;
    /* Accuracy of WGS-84 altitude; metres */

#if USE_JSON
    void toJSON (nlohmann::json& j, const string& name)
    {
        if (present)
            j[name] = value;
        else if (write_json_nulls)
            j[name] = nullptr;
    }
#endif
} t_WGS84_Altitude_Accuracy;

/* WGS-84 position: */
typedef struct
{
    t_Bool high_precision;
    /* High-precision position */
    t_Bool present;
    /* WGS-84 position present */
    t_Real value_lat;
    /* WGS-84 latitude; radians */
    t_Real value_lon;
    /* WGS-84 longitude; radians */

#if USE_JSON
    void toJSON (nlohmann::json& j, const string& name)
    {
        if (present)
        {
            j[name]["high_precision"] = high_precision;
            j[name]["value_lat_rad"] = value_lat;
            j[name]["value_lon_rad"] = value_lon;
        }
        else if (write_json_nulls)
            j[name] = nullptr;
    }
#endif
} t_WGS84_Position;

/* X pulses: */
typedef struct
{
    t_Bool present;
    /* X pulse(s) present */
    t_Tres value_m2;
    /* X pulse from SSR mode 2 */
    t_Tres value_m3a;
    /* X pulse from SSR mode 3/A */
    t_Tres value_mc;
    /* X pulse from SSR mode C */
} t_X_Pulses;

/* Conglomerates for surveillance data input messages: */
/* =================================================== */

/* ADS-B report: */
typedef struct
{
    t_ACAS_Resolution_Advisory_Report acas_resolution_advisory_report;
    /* ACAS resolution advisory report */
    t_Aircraft_Operational_Status aircraft_operational_status;
    /* Aircraft operational status */
    t_Air_Speed air_speed;
    /* Air speed */
    t_Altitude_Reporting_Capability altitude_reporting_capability;
    /* Altitude reporting capability */
    t_Byte asterix_category;
    /* ASTERIX category */
    t_Barometric_Vertical_Rate barometric_vertical_rate;
    /* Barometric vertical rate */
    t_BDS_Register bds_registers[M_MAX_BDS_REGISTERS];
    /* BDS registers */
    t_Tres compact_position_reporting;
    /* CPR validation */
    t_Confidence_Level confidence_level;
    /* Confidence level */
    t_Data_Format data_format;
    /* Data format (origin) */
    t_Data_Source_Identifier data_source_identifier;
    /* Data source identifier */
    t_Time_of_Day detection_time;
    /* Measured detection time */
    t_Detection_Type detection_type;
    /* Detection type */
    t_Tres differential_correction;
    /* Differential correction */
    t_Emitter_Category emitter_category;
    /* Emitter category */
    t_Figure_of_Merit figure_of_merit;
    /* Figure of merit (position accuracy indication) */
    t_Final_State_Selected_Altitude final_selected_altitude;
    /* Final state selected altitude */
    t_Frame_Date frame_date;
    /* Frame date */
    t_Frame_Time frame_time;
    /* Frame time */
    t_Geometric_Altitude geometric_altitude;
    /* Geometric height (above WGS-84); metres */
    t_Geometric_Vertical_Rate geometric_vertical_rate;
    /* Geometric vertical rate */
    t_Bool ground_bit_set;
    /* ADS-B target reported to be on ground */
    t_Ground_Vector ground_vector;
    /* Ground speed and heading */
    t_Bool has_been_listed;
    /* ADS-B report has been listed */
    t_Tres independent_position_check_failed;
    /* Independent position check failed */
    t_Intermediate_State_Selected_Altitude intermediate_selected_altitude;
    /* Intermediate state selected altitude */
    t_Byte line_number;
    /* Board/line number */
    t_Link_Technology link_technology;
    /* Link technology indicator */
    t_Tres list_lookup_check_failed;
    /* List lookup check failed */
    t_Tres local_decoding_position_jump;
    /* Local decoding position jump */
    t_Magnetic_Heading magnetic_heading;
    /* Magnetic heading */
    t_Message_Amplitude message_amplitude;
    /* Message amplitude */
    t_Met_Information met_information;
    /* MET information */
    t_Mode_Info mode_3a_info;
    /* SSR mode 3/A information */
    t_Mode_C_Height mode_c_height;
    /* SSR mode C height */
    t_MOPS_Version mops_version;
    /* MOPS version */
    t_Tres nogo_bit_status;
    /* NOGO bit status */
    t_Quality_Indicators quality_indicators;
    /* Quality indicators */
    t_Tres range_check;
    /* Range check passed, CPR validation pending */
    t_Tres range_check_failed;
    /* Range check failed */
    t_Roll_Angle roll_angle;
    /* Roll angle */
    t_Tres selected_altitude_available;
    /* Selected altitude available */
    t_Sensor_Number sensor_number;
    /* Sensor number */
    t_Service_Identification service_identification;
    /* Service identification */
    t_Service_Management service_management;
    /* Service management */
    t_Bool special_position_indication;
    /* Special Position Indication flag */
    t_Aircraft_Address target_address;
    /* Target address */
    t_Target_Address_Type target_address_type;
    /* Target address type */
    t_Aircraft_Identification target_identification;
    /* Target identification */
    t_Target_Status target_status;
    /* Target status */
    t_Time_of_Day time_of_receipt;
    /* Time of receipt */
    t_Time_of_Day time_of_report;
    /* Time of ASTERIX report transmission */
    t_Bool to_be_listed;
    /* ADS-B report to be listed */
    t_Time_of_Day toa_position;
    /* Time of applicability for position */
    t_Time_of_Day toa_velocity;
    /* Time of applicability for velocity */
    t_Time_of_Day tor_position;
    /* Time of message receipt for position */
    t_Time_of_Day tor_velocity;
    /* Time of message receipt for velocity */
    t_Track_Angle_Rate track_angle_rate;
    /* Track angle rate */
    t_Track_Number track_number;
    /* Track number */
    t_True_Airspeed true_airspeed;
    /* True airspeed */
    t_Velocity_Accuracy velocity_accuracy;
    /* Velocity accuracy indication */
    t_WGS84_Position wgs84_position;
    /* WGS-84 position */

#if USE_JSON
    void toJSON (nlohmann::json& j)
    {
        j["message_type"] = "ads-b target";
        j["version"] = "1.0";

        // t_ACAS_Resolution_Advisory_Report acas_resolution_advisory_report;
        acas_resolution_advisory_report.toJSON(j, "acas_resolution_advisory_report");
        /* ACAS resolution advisory report */

        // t_Aircraft_Operational_Status aircraft_operational_status;
        aircraft_operational_status.toJSON(j, "aircraft_operational_status");
        /* Aircraft operational status */

        // t_Air_Speed air_speed;
        air_speed.toJSON(j, "air_speed");
        /* Air speed */

        // t_Altitude_Reporting_Capability altitude_reporting_capability;
        altitude_reporting_capability.toJSON(j, "altitude_reporting_capability");
        /* Altitude reporting capability */

        // t_Byte asterix_category;
        j["asterix_category"] = asterix_category;
        /* ASTERIX category */

        // t_Barometric_Vertical_Rate barometric_vertical_rate;
        barometric_vertical_rate.toJSON(j, "barometric_vertical_rate");
        /* Barometric vertical rate */

        // t_BDS_Register bds_registers[M_MAX_BDS_REGISTERS];
        for (unsigned int cnt=0; cnt < M_MAX_BDS_REGISTERS; cnt++)
            if (bds_registers[cnt].present)
                bds_registers[cnt].toJSON(j, "bds"+std::to_string(cnt));
        /* BDS registers */

//        t_Tres compact_position_reporting;
//        j["compact_position_reporting"] = convertToJSON(compact_position_reporting);
//        /* CPR validation */

        // t_Confidence_Level confidence_level;
        confidence_level.toJSON(j, "confidence_level");
        /* Confidence level */

//        t_Data_Format data_format;
//        /* Data format (origin) */

        // t_Data_Source_Identifier data_source_identifier;
        data_source_identifier.toJSON(j, "data_source_identifier");
        /* Data source identifier */

        // t_Time_of_Day detection_time;
        detection_time.toJSON(j, "detection_time");
        /* Measured detection time */

        // t_Detection_Type detection_type;
        detection_type.toJSON(j, "detection_type");
        /* Detection type */

        // t_Tres differential_correction;
        convertToJSON(differential_correction, "differential_correction", j);
        /* Differential correction */

        // t_Emitter_Category emitter_category;
        emitter_category.toJSON(j, "emitter_category");
        /* Emitter category */

        // t_Figure_of_Merit figure_of_merit;
        figure_of_merit.toJSON(j, "figure_of_merit");
        /* Figure of merit (position accuracy indication) */

        // t_Final_State_Selected_Altitude final_selected_altitude;
        final_selected_altitude.toJSON(j, "final_selected_altitude");
        /* Final state selected altitude */

        // t_Frame_Date frame_date;
        frame_date.toJSON(j, "frame_date");
        /* Frame date */

        // t_Frame_Time frame_time;
        frame_time.toJSON(j, "frame_time");
        /* Frame time */

        // t_Geometric_Altitude geometric_altitude;
        geometric_altitude.toJSON(j, "geometric_altitude_m");
        /* Geometric height (above WGS-84); metres */

        // t_Geometric_Vertical_Rate geometric_vertical_rate;
        geometric_vertical_rate.toJSON(j, "geometric_vertical_rate");
        /* Geometric vertical rate */

        // t_Bool ground_bit_set;
        j["ground_bit_set"] = ground_bit_set;
        /* ADS-B target reported to be on ground */

        // t_Ground_Vector ground_vector;
        ground_vector.toJSON(j, "ground_vector");
        /* Ground speed and heading */

//        t_Bool has_been_listed;
//        /* ADS-B report has been listed */

        // t_Tres independent_position_check_failed;
        convertToJSON(independent_position_check_failed, "independent_position_check_failed", j);
        /* Independent position check failed */

        // t_Intermediate_State_Selected_Altitude intermediate_selected_altitude;
        intermediate_selected_altitude.toJSON(j, "intermediate_selected_altitude");
        /* Intermediate state selected altitude */

        // t_Byte line_number;
        j["line_number"] = line_number;
        /* Board/line number */

        // t_Link_Technology link_technology;
        link_technology.toJSON(j, "link_technology");
        /* Link technology indicator */

        // t_Tres list_lookup_check_failed;
        convertToJSON(list_lookup_check_failed, "list_lookup_check_failed", j);
        /* List lookup check failed */

        // t_Tres local_decoding_position_jump;
        convertToJSON(local_decoding_position_jump, "local_decoding_position_jump", j);
        /* Local decoding position jump */

        // t_Magnetic_Heading magnetic_heading;
        magnetic_heading.toJSON(j, "magnetic_heading_rad");
        /* Magnetic heading */

        // t_Message_Amplitude message_amplitude;
        message_amplitude.toJSON(j, "message_amplitude_dbm");
        /* Message amplitude */

        // t_Met_Information met_information;
        met_information.toJSON(j, "met_information");
        /* MET information */

        // t_Mode_Info mode_3a_info;
        mode_3a_info.toJSON(j, "mode_3a_info");
        /* SSR mode 3/A information */

        // t_Mode_C_Height mode_c_height;
        mode_c_height.toJSON(j, "mode_c_height");
        /* SSR mode C height */

        // t_MOPS_Version mops_version;
        mops_version.toJSON(j, "mops_version");
        /* MOPS version */

        // t_Tres nogo_bit_status;
        convertToJSON(nogo_bit_status, "nogo_bit_status", j);
        /* NOGO bit status */

        // t_Quality_Indicators quality_indicators;
        quality_indicators.toJSON(j, "quality_indicators");
        /* Quality indicators */

        // t_Tres range_check;
        convertToJSON(range_check, "range_check", j);
        /* Range check passed, CPR validation pending */

        // t_Tres range_check_failed;
        convertToJSON(range_check_failed, "range_check_failed", j);
        /* Range check failed */

        // t_Roll_Angle roll_angle;
        roll_angle.toJSON(j, "roll_angle_rad");
        /* Roll angle */

        // t_Tres selected_altitude_available;
        convertToJSON(selected_altitude_available, "selected_altitude_available", j);
        /* Selected altitude available */

//        t_Sensor_Number sensor_number;
//        /* Sensor number */

        // t_Service_Identification service_identification;
        service_identification.toJSON(j, "service_identification");
        /* Service identification */

        // t_Service_Management service_management;
        service_management.toJSON(j, "service_management_rp_s");
        /* Service management */

        // t_Bool special_position_indication;
        j["special_position_indication"] = special_position_indication;
        /* Special Position Indication flag */

        // t_Aircraft_Address target_address;
        target_address.toJSON(j, "target_address");
        /* Target address */

        // t_Target_Address_Type target_address_type;
        target_address_type.toJSON(j, "target_address_type");
        /* Target address type */

        // t_Aircraft_Identification target_identification;
        target_identification.toJSON(j, "target_identification");
        /* Target identification */

        // t_Target_Status target_status;
        target_status.toJSON(j, "target_status");
        /* Target status */

        // t_Time_of_Day time_of_receipt;
        time_of_receipt.toJSON(j, "time_of_receipt");
        /* Time of receipt */

        // t_Time_of_Day time_of_report;
        time_of_report.toJSON(j, "time_of_report");
        /* Time of ASTERIX report transmission */

//        t_Bool to_be_listed;
//        /* ADS-B report to be listed */

        // t_Time_of_Day toa_position;
        toa_position.toJSON(j, "toa_position");
        /* Time of applicability for position */

        // t_Time_of_Day toa_velocity;
        toa_velocity.toJSON(j, "toa_velocity");
        /* Time of applicability for velocity */

        // t_Time_of_Day tor_position;
        tor_position.toJSON(j, "tor_position");
        /* Time of message receipt for position */

        // t_Time_of_Day tor_velocity;
        tor_velocity.toJSON(j, "tor_velocity");
        /* Time of message receipt for velocity */

        // t_Track_Angle_Rate track_angle_rate;
        track_angle_rate.toJSON(j, "track_angle_rate_rads");
        /* Track angle rate */

        // t_Track_Number track_number;
        track_number.toJSON(j, "track_number");
        /* Track number */

        // t_True_Airspeed true_airspeed;
        true_airspeed.toJSON(j, "true_airspeed");
        /* True airspeed */

        // t_Velocity_Accuracy velocity_accuracy;
        velocity_accuracy.toJSON(j, "velocity_accuracy");
        /* Velocity accuracy indication */

        // t_WGS84_Position wgs84_position;
        wgs84_position.toJSON(j, "wgs84_position");
        /* WGS-84 position */
    }
#endif
} t_Adsb;

/* Multilateration report: */
typedef struct
{
    t_ACAS_Resolution_Advisory_Report acas_resolution_advisory_report;
    /* ACAS resolution advisory report */
    t_Byte asterix_category;
    /* ASTERIX category */
    t_BDS_Register bds_registers[M_MAX_BDS_REGISTERS];
    /* BDS registers */
    t_Communications_Capability communications_capability;
    /* Communications/ACAS capability */
    t_Computed_Acceleration computed_acceleration;
    /* Computed Cartesian acceleration */
    t_Computed_Position computed_position;
    /* Computed Cartesian position */
    t_Computed_Position_Accuracy computed_position_stdev;
    /* Standard deviation of computed position */
    t_Computed_Velocity computed_velocity;
    /* Computed Cartesian velocity */
    t_Bool corrupted_replies;
    /* Corrupted replies in multilateration */
    t_Data_Format data_format;
    /* Data format (origin) */
    t_Data_Source_Identifier data_source_identifier;
    /* Data source identifier */
    t_Bool detected_by_adsb;
    /* Detected from ADS-B squitter */
    t_Bool detected_by_dme;
    /* Detected by DME/TACAN multilateration */
    t_Bool detected_by_hf;
    /* Detected by HF multilateration */
    t_Bool detected_by_mds;
    /* Detected by Mode S multilateration */
    t_Bool detected_by_ot;
    /* Detected by other technology multilateration */
    t_Bool detected_by_ssr;
    /* Detected by SSR multilateration */
    t_Bool detected_by_uat;
    /* Detected by UAT multilateration */
    t_Bool detected_by_vdl;
    /* Detected by VDL mode 4 multilateration */
    t_Time_of_Day detection_time;
    /* Measured detection time */
    t_Detection_Type detection_type;
    /* Detection type */
    t_DOP_of_Position dop_of_position;
    /* DOP of position */
    t_Flight_Status flight_status;
    /* Flight status */
    t_Frame_Date frame_date;
    /* Frame date */
    t_Frame_Time frame_time;
    /* Frame time */
    t_Tres fs_airborne;
    /* SSR mode S flight status: aircraft airborne */
    t_Tres fs_alert;
    /* SSR mode S flight status: alert */
    t_Tres fs_on_ground;
    /* SSR mode S flight status: aircraft on ground */
    t_Tres fs_spi;
    /* SSR mode S flight status: SPI */
    t_WGS84_Altitude geometric_altitude;
    /* Geometric (WGS-84) altitude */
    t_WGS84_Altitude_Accuracy geometric_altitude_stdev;
    /* Standard deviation of geometric (WGS-84) altitude */
    t_Bool ground_bit_set;
    /* Transponder ground bit set */
    t_Ground_Vector ground_vector;
    /* Ground vector */
    t_Bool has_been_listed;
    /* Multilateration report has been listed */
    t_Bool is_status_message;
    /* Indication for status message */
    t_Byte line_number;
    /* Board/line number */
    t_Measured_Height measured_height;
    /* Measured height */
    t_Mode_Info mode_1_info;
    /* SSR mode 1 information */
    t_Mode_Info mode_2_info;
    /* SSR mode 2 information */
    t_Mode_Info mode_3a_info;
    /* SSR mode 3/A information */
    t_Mode_C_Height mode_c_height;
    /* SSR mode C height */
    t_Mode_C_Info mode_c_info;
    /* SSR mode C information */
    t_Plot_Qualification plot_qualification;
    /* Plot qualification */
    t_Reported_Track_Status reported_track_status;
    /* Reported track status */
    t_Selected_Altitude selected_altitude;
    /* Selected altitude */
    t_Sensor_Number sensor_number;
    /* Sensor number */
    t_Bool special_position_indication;
    /* Special position indication */
    t_Aircraft_Address target_address;
    /* Target address */
    t_Aircraft_Identification target_identification;
    /* Target identification */
    t_Bool to_be_listed;
    /* Multilateration report to be listed */
    t_Track_Number track_number;
    /* Track number */
    t_Vehicle_Fleet_Identification vehicle_fleet_identification;
    /* Vehicle fleet identification */
    t_WEC_List wec_list;
    /* List of warning/error conditions */
    t_WGS84_Position wgs84_position;
    /* WGS-84 position */

#if USE_JSON
    void toJSON (nlohmann::json& j)
    {
        j["message_type"] = "mlat target";
        j["version"] = "1.0";

        // t_ACAS_Resolution_Advisory_Report acas_resolution_advisory_report;
        acas_resolution_advisory_report.toJSON(j, "acas_resolution_advisory_report");
        /* ACAS resolution advisory report */

        // t_Byte asterix_category;
        j["asterix_category"] = asterix_category;
        /* ASTERIX category */

        // t_BDS_Register bds_registers[M_MAX_BDS_REGISTERS];
        for (unsigned int cnt=0; cnt < M_MAX_BDS_REGISTERS; cnt++)
            if (bds_registers[cnt].present)
                bds_registers[cnt].toJSON(j, "bds"+std::to_string(cnt));
        /* BDS registers */

        // t_Communications_Capability communications_capability;
        communications_capability.toJSON(j, "communications_capability");
        /* Communications/ACAS capability */

        // t_Computed_Acceleration computed_acceleration;
        computed_acceleration.toJSON(j, "computed_acceleration");
        /* Computed Cartesian acceleration */

        // t_Computed_Position computed_position;
        computed_position.toJSON(j, "computed_position");
        /* Computed Cartesian position */

        // t_Computed_Position_Accuracy computed_position_stdev;
        computed_position_stdev.toJSON(j, "computed_position_stdev");
        /* Standard deviation of computed position */

        // t_Computed_Velocity computed_velocity;
        computed_velocity.toJSON(j, "computed_velocity");
        /* Computed Cartesian velocity */

        // t_Bool corrupted_replies;
        j["corrupted_replies"] = corrupted_replies;
        /* Corrupted replies in multilateration */

//        t_Data_Format data_format;
//        /* Data format (origin) */

        // t_Data_Source_Identifier data_source_identifier;
        data_source_identifier.toJSON(j, "data_source_identifier");
        /* Data source identifier */

        // t_Bool detected_by_adsb;
        j["detected_by_adsb"] = detected_by_adsb;
        /* Detected from ADS-B squitter */

        // t_Bool detected_by_dme;
        j["detected_by_dme"] = detected_by_dme;
        /* Detected by DME/TACAN multilateration */

        // t_Bool detected_by_hf;
        j["detected_by_hf"] = detected_by_hf;
        /* Detected by HF multilateration */

        // t_Bool detected_by_mds;
        j["detected_by_mds"] = detected_by_mds;
        /* Detected by Mode S multilateration */

        // t_Bool detected_by_ot;
        j["detected_by_ot"] = detected_by_ot;
        /* Detected by other technology multilateration */

        // t_Bool detected_by_ssr;
        j["detected_by_ssr"] = detected_by_ssr;
        /* Detected by SSR multilateration */

        // t_Bool detected_by_uat;
        j["detected_by_uat"] = detected_by_uat;
        /* Detected by UAT multilateration */

        // t_Bool detected_by_vdl;
        j["detected_by_vdl"] = detected_by_vdl;
        /* Detected by VDL mode 4 multilateration */

        //t_Time_of_Day detection_time;
        detection_time.toJSON(j, "detection_time");
        /* Measured detection time */

        //t_Detection_Type detection_type;
        detection_type.toJSON(j, "detection_type");
        /* Detection type */

        // t_DOP_of_Position dop_of_position;
        dop_of_position.toJSON(j, "dop_of_position");
        /* DOP of position */

        //t_Flight_Status flight_status;
        flight_status.toJSON(j, "flight_status");
        /* Flight status */

        // t_Frame_Date frame_date;
        frame_date.toJSON(j, "frame_date");
        /* Frame date */

        // t_Frame_Time frame_time;
        frame_time.toJSON(j, "frame_time");
        /* Frame time */

        // t_Tres fs_airborne;
        convertToJSON(fs_airborne, "fs_airborne", j);
        /* SSR mode S flight status: aircraft airborne */

        // t_Tres fs_alert;
        convertToJSON(fs_alert, "fs_alert", j);
        /* SSR mode S flight status: alert */

        // t_Tres fs_on_ground;
        convertToJSON(fs_on_ground, "fs_on_ground", j);
        /* SSR mode S flight status: aircraft on ground */

        // t_Tres fs_spi;
        convertToJSON(fs_spi, "fs_spi", j);
        /* SSR mode S flight status: SPI */

        // t_WGS84_Altitude geometric_altitude;
        geometric_altitude.toJSON(j, "geometric_altitude_m");
        /* Geometric (WGS-84) altitude */

        // t_WGS84_Altitude_Accuracy geometric_altitude_stdev;
        geometric_altitude_stdev.toJSON(j, "geometric_altitude_stdev");
        /* Standard deviation of geometric (WGS-84) altitude */

        // t_Bool ground_bit_set;
        j["ground_bit_set"] = ground_bit_set;
        /* Transponder ground bit set */

        // t_Ground_Vector ground_vector;
        ground_vector.toJSON(j, "ground_vector");
        /* Ground vector */

//        t_Bool has_been_listed;
//        /* Multilateration report has been listed */

        //t_Bool is_status_message;
        j["is_status_message"] = is_status_message;
        /* Indication for status message */

        // t_Byte line_number;
        j["line_number"] = line_number;
        /* Board/line number */

        // t_Measured_Height measured_height;
        measured_height.toJSON(j, "measured_height");
        /* Measured height */

        // t_Mode_Info mode_1_info;
        mode_1_info.toJSON(j, "mode_1_info");
        /* SSR mode 1 information */

        // t_Mode_Info mode_2_info;
        mode_2_info.toJSON(j, "mode_2_info");
        /* SSR mode 2 information */

        // t_Mode_Info mode_3a_info;
        mode_3a_info.toJSON(j, "mode_3a_info");
        /* SSR mode 3/A information */

        // t_Mode_C_Height mode_c_height;
        mode_c_height.toJSON(j, "mode_c_height");
        /* SSR mode C height */

        // t_Mode_C_Info mode_c_info;
        mode_c_info.toJSON(j, "mode_c_info");
        /* SSR mode C information */

        // t_Plot_Qualification plot_qualification;
        plot_qualification.toJSON(j, "plot_qualification");
        /* Plot qualification */

        // t_Reported_Track_Status reported_track_status;
        reported_track_status.toJSON(j, "reported_track_status");
        /* Reported track status */

        // t_Selected_Altitude selected_altitude;
        selected_altitude.toJSON(j, "selected_altitude");
        /* Selected altitude */

//        t_Sensor_Number sensor_number;
//        /* Sensor number */

        // t_Bool special_position_indication;
        j["special_position_indication"] = special_position_indication;
        /* Special position indication */

        // t_Aircraft_Address target_address;
        target_address.toJSON(j, "target_address");
        /* Target address */

        // t_Aircraft_Identification target_identification;
        target_identification.toJSON(j, "target_identification");
        /* Target identification */

//        t_Bool to_be_listed;
//        /* Multilateration report to be listed */

        // t_Track_Number track_number;
        track_number.toJSON(j, "track_number");
        /* Track number */

        // t_Vehicle_Fleet_Identification vehicle_fleet_identification;
        vehicle_fleet_identification.toJSON(j, "vehicle_fleet_identification");
        /* Vehicle fleet identification */

        // t_WEC_List wec_list;
        wec_list.toJSON(j, "wec_list");
        /* List of warning/error conditions */

        // t_WGS84_Position wgs84_position;
        wgs84_position.toJSON(j, "wgs84_position");
        /* WGS-84 position */

    }
#endif
} t_Mlat;

/* Object types: */
typedef enum
{
    e_no_object = 0,
    /* No object */
    e_object_adsb,
    /* ADS-B report */
    e_object_mlat,
    /* Multilateration report */
    e_object_rsrv,
    /* Radar service message */
    e_object_rtgt,
    /* Radar target report */
    e_object_scv6,
    /* SASS-C V6 listbox line */
    e_object_step,
    /* System picture step */
    e_object_strk,
    /* System track report */
    e_object_sentinel
    /* Sentinel */
    /* Must be the last entry - do not remove */
} t_Object_Type;

/* Radar service message: */
typedef struct
{
    t_Rsrv_Type rsrv_type;
    /* Type of radar service message */

    t_Bool activate_filtering_zone;
    /* Activate filtering zone */
    t_Antenna_Azimuth antenna_azimuth;
    /* Antenna azimuth */
    t_Antenna_Rotation_Speed antenna_rotation_speed;
    /* Antenna rotation speed */
    t_Byte asterix_category;
    /* ASTERIX category */
    t_Collimation_Error collimation_error;
    /* Collimation error */
    t_Data_Format data_format;
    /* Data format (origin) */
    t_Data_Source_Identifier data_source_identifier;
    /* Data source identifier */
    t_Data_Source_Position data_source_position;
    /* Data source position */
    t_Bool deactivate_filtering_zone;
    /* Deactivate filtering zone */
    t_Frame_Date frame_date;
    /* Frame date */
    t_Frame_Time frame_time;
    /* Frame time */
    t_Bool has_been_listed;
    /* Radar service message has been listed */
    t_Byte line_number;
    /* Board/line number */
    t_Time_of_Day message_time;
    /* Measured or receiving time */
    t_Bool north_crossing;
    /* North crossing indication */
    t_Polar_Window polar_window;
    /* Polar window (for activation or
   deactivation of a filtering zone) */
    t_Processing_Status processing_status;
    /* Processing status */
    t_Radar_Configuration radar_configuration;
    /* Radar configuration and status */
    t_Sector_Number sector_number;
    /* Sector number */
    t_Sensor_Number sensor_number;
    /* Sensor number */
    t_Slant_Range slant_range;
    /* Slant range */
    t_Data_Reference station_configuration_status;
    /* Station configuration status */
    t_Data_Reference station_processing_mode;
    /* Station processing mode */
    t_Step_Number step_number;
    /* Step number */
    t_Time_Delay time_delay;
    /* Time delay, i. e., difference between receiving and
   detection time */
    t_Time_of_Day time_of_day;
    /* Measured time of day */
    t_Timejump_Info timejump_info;
    /* Timejump information */
    t_Bool to_be_listed;
    /* Radar service message to be listed */
    t_Data_Reference warning_error_conditions;
    /* Warning/error conditions */

#if USE_JSON
    void toJSON (nlohmann::json& j)
    {
        j["message_type"] = "radar service";
        j["version"] = "1.0";

        // t_Rsrv_Type rsrv_type;
        convertToJSON(rsrv_type, "rsrv_type", j);
        /* Type of radar service message */

//        t_Bool activate_filtering_zone;
//        /* Activate filtering zone */

        // t_Antenna_Azimuth antenna_azimuth;
        antenna_azimuth.toJSON(j, "antenna_azimuth_rad");
        /* Antenna azimuth */

//        t_Antenna_Rotation_Speed antenna_rotation_speed;
//        /* Antenna rotation speed */

        // t_Byte asterix_category;
        j["asterix_category"] = asterix_category;
        /* ASTERIX category */

//        t_Collimation_Error collimation_error;
//        /* Collimation error */

//        t_Data_Format data_format;
//        /* Data format (origin) */

        // t_Data_Source_Identifier data_source_identifier;
        data_source_identifier.toJSON(j, "data_source_identifier");
        /* Data source identifier */

//        t_Data_Source_Position data_source_position;
//        /* Data source position */

//        t_Bool deactivate_filtering_zone;
//        /* Deactivate filtering zone */

        // t_Frame_Date frame_date;
        frame_date.toJSON(j, "frame_date");
        /* Frame date */

        // t_Frame_Time frame_time;
        frame_time.toJSON(j, "frame_time");
        /* Frame time */

//        t_Bool has_been_listed;
//        /* Radar service message has been listed */

        // t_Byte line_number;
        j["line_number"] = line_number;
        /* Board/line number */

        // t_Time_of_Day message_time;
        message_time.toJSON(j, "message_time");
        /* Measured or receiving time */

        // t_Bool north_crossing;
        j["north_crossing"] = north_crossing;
        /* North crossing indication */

//        t_Polar_Window polar_window;
//        /* Polar window (for activation or
//       deactivation of a filtering zone) */

//        t_Processing_Status processing_status;
//        /* Processing status */

//        t_Radar_Configuration radar_configuration;
//        /* Radar configuration and status */

        // t_Sector_Number sector_number;
        sector_number.toJSON(j, "sector_number");
        /* Sector number */

//        t_Sensor_Number sensor_number;
//        /* Sensor number */

//        t_Slant_Range slant_range;
//        /* Slant range */

//        t_Data_Reference station_configuration_status;
//        /* Station configuration status */

//        t_Data_Reference station_processing_mode;
//        /* Station processing mode */

//        t_Step_Number step_number;
//        /* Step number */

//        t_Time_Delay time_delay;
//        /* Time delay, i. e., difference between receiving and
//       detection time */

        // t_Time_of_Day time_of_day;
        time_of_day.toJSON(j, "time_of_day");
        /* Measured time of day */

//        t_Timejump_Info timejump_info;
//        /* Timejump information */

//        t_Bool to_be_listed;
//        /* Radar service message to be listed */

//        t_Data_Reference warning_error_conditions;
//        /* Warning/error conditions */
    }
#endif
} t_Rsrv;

/* Radar target report: */
typedef struct
{
    t_ACAS_Resolution_Advisory_Report acas_resolution_advisory_report;
    /* ACAS resolution advisory report */
    t_Aircraft_Identification aircraft_identification;
    /* Aircraft identification */
    t_Antenna_Number antenna_number;
    /* Antenna number */
    t_Assumed_Height assumed_height;
    /* Assumed height */
    t_Byte asterix_category;
    /* ASTERIX category */
    t_Azimuth_Difference azimuth_difference;
    /* Azimuth difference between PSR and SSR plot */
    t_BDS_Register bds_registers[M_MAX_BDS_REGISTERS];
    /* BDS registers */
    t_Doppler_Speed calculated_doppler_speed;
    /* Calculated Doppler speed */
    t_Tres civil_emergency;
    /* Civil emergency indication */
    t_Computed_Position computed_position;
    /* Computed (Cartesian) position */
    t_Data_Format data_format;
    /* Data format (origin) */
    t_Data_Source_Identifier data_source_identifier;
    /* Data source identifier */
    t_Partial_Time_of_Day detection_ptod;
    /* Measured detection time (as partial time of day) */
    t_Time_of_Day detection_time;
    /* Measured detection time */
    t_Detection_Type detection_type;
    /* Detection type */
    t_Real elevation;
    /* Target elevation (above radar horizon); radians */
    t_Bool elevation_defined;
    /* Target elevation defined */
    t_Tres emergency_1;
    /* Emergency indication (A7500) */
    t_Tres emergency_2;
    /* Emergency indication (A7600) */
    t_Tres emergency_3;
    /* Emergency indication (A7700) */
    t_Time_of_Day estimated_time;
    /* Estimated detection time */
    t_Bool excessive_dt_fd;
    /* Excessive difference of time between frame
   time and detection time */
    t_Bool excessive_dt_fe;
    /* Excessive difference of time between frame
   time and estimated time */
    t_Frame_Date frame_date;
    /* Frame date */
    t_Frame_Time frame_time;
    /* Frame time */
    t_Bool from_rdp_chain_1;
    /* From RDP chain 1 */
    t_Bool from_rdp_chain_2;
    /* From RDP chain 2 */
    t_Tres fs_airborne;
    /* SSR mode S flight status: aircraft airborne */
    t_Tres fs_alert;
    /* SSR mode S flight status: alert */
    t_Tres fs_on_ground;
    /* SSR mode S flight status: aircraft on ground */
    t_Tres fs_spi;
    /* SSR mode S flight status: SPI */
    t_Ground_Vector ground_vector;
    /* Ground speed and heading */
    t_Bool has_been_listed;
    /* Radar target report has been listed */
    t_Height_3D height_3d;
    /* 3D radar height */
    t_Byte line_number;
    /* Board/line number */
    t_Mapped_Position mapped_position;
    /* Mapped (stereographic) position */
    t_Measured_Azimuth measured_azm;
    /* Measured azimuth */
    t_Measured_Range measured_rng;
    /* Measured range */
    t_Metric_Height metric_height;
    /* Metric height */
    t_Tres military_emergency;
    /* Military emergency indication */
    t_Tres military_ident;
    /* Military ident indication */
    t_Mode_Info mode_1_info;
    /* SSR mode 1 information */
    t_Mode_Info mode_2_info;
    /* SSR mode 2 information */
    t_Mode_Info mode_3a_info;
    /* SSR mode 3/A information */
    t_Mode_4_Info mode_4_info;
    /* SSR mode 4 information */
    t_Mode_C_Code mode_c_code;
    /* SSR mode C code */
    t_Mode_C_Height mode_c_height;
    /* SSR mode C height */
    t_Mode_S_Flags mode_s_flags;
    /* SSR mode S flags */
    t_Plot_Qualification plot_qualification;
    /* Plot qualification */
    t_Measured_Amplitude psr_amplitude;
    /* Amplitude of PSR reply */
    t_Runlength psr_runlength;
    /* PSR plot runlength */
    t_Radial_Speed radial_speed;
    /* Measured radial Doppler speed */
    t_Range_Difference range_difference;
    /* Range difference between PSR and SSR plot */
    t_Raw_Doppler_Speed raw_doppler_speed;
    /* Raw Doppler speed */
    t_Received_Power received_power;
    /* Received power */
    t_Reported_Track_Quality reported_track_quality;
    /* Reported (radar) track quality */
    t_Reported_Track_Status reported_track_status;
    /* Reported (radar) track status */
    t_Selected_Altitude selected_altitude;
    /* Selected altitude */
    t_Sensor_Number sensor_number;
    /* Sensor number */
    t_Tres special_position_indication;
    /* Special Position Indication flag */
    t_Measured_Amplitude ssr_amplitude;
    /* Amplitude of SSR reply */
    t_Replies_Count ssr_replies;
    /* Number of SSR replies */
    t_Runlength ssr_runlength;
    /* SSR plot runlength */
    t_Aircraft_Address target_address;
    /* Target address */
    t_Time_Delay time_delay;
    /* Time delay, i. e., difference between receiving and
   detection time */
    t_Tres to_be_cancelled;
    /* Radar track to be cancelled */
    t_Bool to_be_listed;
    /* Radar target report to be listed */
    t_Track_Number track_number;
    /* Track number */
    t_WEC_List wec_list;
    /* List of warning/error conditions */
    t_X_Pulses x_pulses;
    /* X pulses */

#if USE_JSON
    void toJSON (nlohmann::json& j)
    {
        j["message_type"] = "radar target";
        j["version"] = "1.0";

        // t_ACAS_Resolution_Advisory_Report acas_resolution_advisory_report;
        acas_resolution_advisory_report.toJSON(j, "acas_resolution_advisory_report");
        /* ACAS resolution advisory report */

        // t_Aircraft_Identification aircraft_identification;
        aircraft_identification.toJSON(j, "aircraft_identification");
        /* Aircraft identification */

        // t_Antenna_Number antenna_number;
        antenna_number.toJSON(j, "antenna_number");
        /* Antenna number */

        // t_Assumed_Height assumed_height;
        assumed_height.toJSON(j, "assumed_height");
        /* Assumed height */

        // t_Byte asterix_category;
        j["asterix_category"] = asterix_category;
        /* ASTERIX category */

        // t_Azimuth_Difference azimuth_difference;
        azimuth_difference.toJSON(j, "azimuth_difference");
        /* Azimuth difference between PSR and SSR plot */

        // t_BDS_Register bds_registers[M_MAX_BDS_REGISTERS];
        for (unsigned int cnt=0; cnt < M_MAX_BDS_REGISTERS; cnt++)
            if (bds_registers[cnt].present)
                bds_registers[cnt].toJSON(j, "bds"+std::to_string(cnt));
        /* BDS registers */

        // t_Doppler_Speed calculated_doppler_speed;
        calculated_doppler_speed.toJSON(j, "calculated_doppler_speed");
        /* Calculated Doppler speed */

        // t_Tres civil_emergency;
        convertToJSON(civil_emergency, "civil_emergency", j);
        /* Civil emergency indication */

        // t_Computed_Position computed_position;
        computed_position.toJSON(j, "computed_position");
        /* Computed (Cartesian) position */

//        t_Data_Format data_format;
//        /* Data format (origin) */

        // t_Data_Source_Identifier data_source_identifier;
        data_source_identifier.toJSON(j, "data_source_identifier");
        /* Data source identifier */

        // t_Partial_Time_of_Day detection_ptod;
        detection_ptod.toJSON(j, "detection_ptod");
        /* Measured detection time (as partial time of day) */

        // t_Time_of_Day detection_time;
        detection_time.toJSON(j, "detection_time");
        /* Measured detection time */

        // t_Detection_Type detection_type;
        detection_type.toJSON(j, "detection_type");
        /* Detection type */

        if (elevation_defined)
            j ["elevation"] = elevation;
        else if (write_json_nulls)
            j ["elevation"] = nullptr;

        // t_Real elevation;
        /* Target elevation (above radar horizon); radians */
        // t_Bool elevation_defined;
        /* Target elevation defined */

        // t_Tres emergency_1;
        convertToJSON(emergency_1, "emergency_1_a7500", j);
        /* Emergency indication (A7500) */

        // t_Tres emergency_2;
        convertToJSON(emergency_2, "emergency_2_a7600", j);
        /* Emergency indication (A7600) */

        // t_Tres emergency_3;
        convertToJSON(emergency_3, "emergency_3_a7700", j);
        /* Emergency indication (A7700) */

        // t_Time_of_Day estimated_time;
        detection_time.toJSON(j, "estimated_time");
        /* Estimated detection time */

//        t_Bool excessive_dt_fd;
//        /* Excessive difference of time between frame
//       time and detection time */

//        t_Bool excessive_dt_fe;
//        /* Excessive difference of time between frame
//       time and estimated time */

        // t_Frame_Date frame_date;
        frame_date.toJSON(j, "frame_date");
        /* Frame date */

        // t_Frame_Time frame_time;
        frame_time.toJSON(j, "frame_time");
        /* Frame time */

        // t_Bool from_rdp_chain_1;
        j["from_rdp_chain_1"] = from_rdp_chain_1;
        /* From RDP chain 1 */

        // t_Bool from_rdp_chain_2;
        j["from_rdp_chain_2"] = from_rdp_chain_2;
        /* From RDP chain 2 */

        // t_Tres fs_airborne;
        convertToJSON(fs_airborne, "fs_airborne", j);
        /* SSR mode S flight status: aircraft airborne */

        // t_Tres fs_alert;
        convertToJSON(fs_alert, "fs_alert", j);
        /* SSR mode S flight status: alert */

        // t_Tres fs_on_ground;
        convertToJSON(fs_on_ground, "fs_on_ground", j);
        /* SSR mode S flight status: aircraft on ground */

        // t_Tres fs_spi;
        convertToJSON(fs_spi, "fs_spi", j);
        /* SSR mode S flight status: SPI */

        // t_Ground_Vector ground_vector;
        ground_vector.toJSON(j, "ground_vector");
        /* Ground speed and heading */

//        t_Bool has_been_listed;
//        /* Radar target report has been listed */

        // t_Height_3D height_3d;
        height_3d.toJSON(j, "height_3d");
        /* 3D radar height */

        // t_Byte line_number;
        j ["line_number"] = line_number;
        /* Board/line number */

        // t_Mapped_Position mapped_position;
        mapped_position.toJSON(j, "mapped_position");
        /* Mapped (stereographic) position */

        // t_Measured_Azimuth measured_azm;
        measured_azm.toJSON(j, "measured_azm_rad");
        /* Measured azimuth */

        // t_Measured_Range measured_rng;
        measured_rng.toJSON(j, "measured_rng_m");
        /* Measured range */

        // t_Metric_Height metric_height;
        metric_height.toJSON(j, "metric_height");
        /* Metric height */

        // t_Tres military_emergency;
        convertToJSON(military_emergency, "military_emergency", j);
        /* Military emergency indication */

        // t_Tres military_ident;
        convertToJSON(military_ident, "military_ident", j);
        /* Military ident indication */

        // t_Mode_Info mode_1_info;
        mode_1_info.toJSON(j, "mode_1_info");
        /* SSR mode 1 information */

        // t_Mode_Info mode_2_info;
        mode_2_info.toJSON(j, "mode_2_info");
        /* SSR mode 2 information */

        // t_Mode_Info mode_3a_info;
        mode_3a_info.toJSON(j, "mode_3_info");
        /* SSR mode 3/A information */

         t_Mode_4_Info mode_4_info;
        mode_4_info.toJSON(j, "mode_4_info");
        /* SSR mode 4 information */

        // t_Mode_C_Code mode_c_code;
        mode_c_code.toJSON(j, "mode_c_code");
        /* SSR mode C code */

        // t_Mode_C_Height mode_c_height;
        mode_c_height.toJSON(j, "mode_c_height");
        /* SSR mode C height */

        // t_Mode_S_Flags mode_s_flags;
        mode_s_flags.toJSON(j, "mode_s_flags");
        /* SSR mode S flags */

        // t_Plot_Qualification plot_qualification;
        mode_s_flags.toJSON(j, "mode_s_flags");
        /* Plot qualification */

        // t_Measured_Amplitude psr_amplitude;
        psr_amplitude.toJSON(j, "psr_amplitude_dbm");
        /* Amplitude of PSR reply */

        // t_Runlength psr_runlength;
        psr_runlength.toJSON(j, "psr_runlength");
        /* PSR plot runlength */

        // t_Radial_Speed radial_speed;
        radial_speed.toJSON(j, "radial_speed_ms");
        /* Measured radial Doppler speed */

        // t_Range_Difference range_difference;
        radial_speed.toJSON(j, "radial_speed_m");
        /* Range difference between PSR and SSR plot */

        // t_Raw_Doppler_Speed raw_doppler_speed;
        raw_doppler_speed.toJSON(j, "raw_doppler_speed");
        /* Raw Doppler speed */

        // t_Received_Power received_power;
        received_power.toJSON(j, "received_power_dbm");
        /* Received power */

        //t_Reported_Track_Quality reported_track_quality;
        reported_track_quality.toJSON(j, "reported_track_quality");
        /* Reported (radar) track quality */

        // t_Reported_Track_Status reported_track_status;
        reported_track_status.toJSON(j, "reported_track_status");
        /* Reported (radar) track status */

        // t_Selected_Altitude selected_altitude;
        selected_altitude.toJSON(j, "selected_altitude");
        /* Selected altitude */

        // t_Sensor_Number sensor_number;
        sensor_number.toJSON(j, "sensor_number");
        /* Sensor number */

        // t_Tres special_position_indication;
        convertToJSON(special_position_indication, "special_position_indication", j);
        /* Special Position Indication flag */

        // t_Measured_Amplitude ssr_amplitude;
        ssr_amplitude.toJSON(j, "ssr_amplitude");
        /* Amplitude of SSR reply */

        // t_Replies_Count ssr_replies;
        ssr_replies.toJSON(j, "ssr_replies");
        /* Number of SSR replies */

        // t_Runlength ssr_runlength;
        ssr_runlength.toJSON(j, "ssr_runlength");
        /* SSR plot runlength */

        // t_Aircraft_Address target_address;
        target_address.toJSON(j, "target_address");
        /* Target address */

//        t_Time_Delay time_delay;
//        /* Time delay, i. e., difference between receiving and
//       detection time */

        // t_Tres to_be_cancelled;
        convertToJSON(to_be_cancelled, "to_be_cancelled", j);
        /* Radar track to be cancelled */

//        t_Bool to_be_listed;
//        /* Radar target report to be listed */

        // t_Track_Number track_number;
        track_number.toJSON(j, "track_number");
        /* Track number */

        // t_WEC_List wec_list;
        wec_list.toJSON(j, "wec_list");
        /* List of warning/error conditions */

//        t_X_Pulses x_pulses;
//        /* X pulses */
    }
#endif
} t_Rtgt;

/* Sensor status: */
typedef struct
{
    t_Byte connection_status;
    /* Connection status */
    t_Bool connection_status_present;
    /* Connection status present */
    t_Real psr_azm_bias;
    /* PSR azimuth bias; degrees */
    t_Bool psr_azm_bias_present;
    /* PSR azimuth bias present */
    t_Real psr_rng_bias;
    /* PSR range bias; meters */
    t_Bool psr_rng_bias_present;
    /* PSR range bias present */
    t_Real psr_rng_gain;
    /* PSR range gain */
    t_Bool psr_rng_gain_present;
    /* PSR range gain present */
    t_Secs reporting_time;
    /* Reporting time; seconds */
    t_Bool reporting_time_present;
    /* Reporting time present */
    t_Ui16 sensor_id;
    /* Sensor identification (SAC/SIC) */
    t_Bool sensor_id_present;
    /* Sensor identification present */
    t_Real ssr_azm_bias;
    /* SSR azimuth bias; degrees */
    t_Bool ssr_azm_bias_present;
    /* SSR azimuth bias present */
    t_Real ssr_rng_bias;
    /* SSR range bias; meters */
    t_Bool ssr_rng_bias_present;
    /* SSR range bias present */
    t_Real ssr_rng_gain;
    /* SSR range gain */
    t_Bool ssr_rng_gain_present;
    /* SSR range gain present */
    t_Secs time_stamping_bias;
    /* Time stamping bias; seconds */
    t_Bool time_stamping_bias_present;
    /* Time stamping bias present */
    t_User_Number user_number;
    /* User number */
} t_Ssta;

/* System picture step: */
typedef struct
{
    t_Batch_Number batch_number;
    /* Batch number */
    t_Data_Source_Identifier data_source_identifier;
    /* Data source identifier */
    t_Frame_Date frame_date;
    /* Frame date */
    t_Frame_Time frame_time;
    /* Frame time */
    t_Message_Type message_type;
    /* Message type */
    t_SDPS_Configuration_and_Status sdps_configuration_and_status;
    /* SDPS configuration and status */
    t_Service_Identification service_identification;
    /* Service identification */
    t_Service_Status_Report service_status_report;
    /* Service status report */
    t_WGS84_Position srp_position;
    /* WGS-84 position of system reference point */
    t_Time_of_Day time_of_message;
    /* Time of message */
} t_Step;

/* System track information: */
typedef struct
{
    t_Byte asterix_category;
    /* ASTERIX category */
    t_Bool calculated_cartesian_velocity_accuracy_present;
    /* Accuracy of calculated Cartesian track
   velocity present */
    t_Bool calculated_polar_velocity_accuracy_present;
    /* Accuracy of calculated polar track velocity
   present */
    t_Bool calculated_polar_velocity_present;
    /* Calculated polar track velocity present */
    t_Bool calculated_position_accuracy_present;
    /* Calculated position accuracy present */
    t_Bool calculated_rate_of_climb_descent_accuracy_present;
    /* Accuracy of calculated rate of climb/descent
   present */
    t_Bool calculated_rate_of_turn_accuracy_present;
    /* Accuracy of calculated rate of turn present */
    t_Bool calculated_rate_of_turn_present;
    /* Calculated rate of turn present */
    t_Bool calculated_track_altitude_accuracy_present;
    /* Calculated track altitude accuracy present */
    t_Bool calculated_track_flight_level_accuracy_present;
    /* Accuracy of calculated track flight level
   present */
    t_Bool callsign_present;
    /* Callsign present */
    t_Bool category_of_turbulence_present;
    /* Category of turbulence present */
    t_Bool cleared_flight_level_present;
    /* Cleared flight level present */
    t_Bool control_position_present;
    /* Control position present */
    t_Data_Format data_format;
    /* Data format (origin) */
    t_Bool departure_airport_present;
    /* Departure airport present */
    t_Bool destination_airport_present;
    /* Destination airport present */
    t_Bool flight_category_present;
    /* Flight category present */
    t_Bool fpps_sacsic_present;
    /* FPPS SAC/SIC present */
    t_Bool mode_4_info_present;
    /* SSR mode 4 information present */
    t_Bool mode_of_flight_probabilities_present;
    /* Calculated mode of flight probabilities present */
    t_Bool most_reliable_height_present;
    /* Most reliable height present */
    t_Bool pln_number_present;
    /* PLN number present */
    t_Bool plot_mode_3a_age_present;
    /* Plot age for SSR mode 3/A present */
    t_Bool plot_mode_c_age_present;
    /* Plot age for SSR mode C present */
    t_Bool position_uncertainty_present;
    /* Position uncertainty present */
    t_Bool track_mode_3a_age_present;
    /* Track age for SSR mode 3/A code present */
    t_Bool track_psr_age_present;
    /* Track age for PSR detection present */
    t_Bool track_ssr_age_present;
    /* Track age for SSR detection present */
    t_Bool track_numbering_indicator_present;
    /* Track numbering indicator present */
    t_Bool track_quality_present;
    /* Track quality present */
    t_Bool type_of_aircraft_present;
    /* Type of aircraft present */
    t_Bool type_of_message_present;
    /* Type of message present */
    t_Bool user_number_present;
    /* User number present */

    t_ACAS_Resolution_Advisory_Report acas_resolution_advisory_report;
    /* ACAS resolution advisory report */
    t_Tres adsb_blunder_detected;
    /* ADS-B blunder detected */
    t_Aircraft_Address aircraft_address;
    /* Technical SSR mode S address */
    t_Aircraft_Identification aircraft_identification;
    /* Aircraft identification */
    t_Tres amalgamated_track;
    /* Amalgamated track */
    t_Tres assigned_code_conflict;
    /* Conflict in assigned SSR mode 3/A code */
    t_Tres background_service;
    /* Background (not complementary) service */
    t_Barometric_Vertical_Rate barometric_vertical_rate;
    /* Barometric vertical rate */
    t_Computed_Acceleration calculated_acceleration;
    /* Calculated Cartesian acceleration */
    t_Computed_Velocity calculated_cartesian_velocity;
    /* Calculated Cartesian velocity */
    t_Computed_Position calculated_position;
    /* Calculated Cartesian position */
    t_Strk_Vertical_Rate calculated_rate_of_climb_descent;
    /* Calculated rate of climb/descent */
    t_Real calculated_rate_of_climb_descent_accuracy;
    /* Accuracy of calculated rate of climb/descent;
   feet/minute */
    t_Real calculated_rate_of_turn;
    /* Calculated rate of turn; degrees/second */
    t_Real calculated_rate_of_turn_accuracy;
    /* Accuracy of calculated rate of turn;
   degrees/second */
    t_Computed_Altitude calculated_track_altitude;
    /* Calculated track altitude */
    t_Real calculated_track_altitude_accuracy;
    /* Accuracy of calculated track altitude; meters */
    t_Flight_Level calculated_track_flight_level;
    /* Calculated track flight level */
    t_Si16 calculated_track_flight_level_accuracy;
    /* Accuracy of calculated track flight level;
   25 feet */
    t_Real calculated_track_heading;
    /* Calculated track heading; radians */
    t_Real calculated_track_heading_accuracy;
    /* Accuracy of calculated track heading;
   degrees */
    t_Real calculated_track_speed;
    /* Calculated track speed; metres/second */
    t_Real calculated_track_speed_accuracy;
    /* Accuracy of calculated track speed; knots */
    t_Strk_WGS84_Position calculated_wgs84_position;
    /* Calculated WGS-84 position */
    t_Real calculated_x_position_accuracy;
    /* Calculated x position accuracy; nautical miles */
    t_Real calculated_x_velocity_accuracy;
    /* Accuracy of calculated x velocity; knots */
    t_Real calculated_y_position_accuracy;
    /* Calculated y position accuracy; nautical miles */
    t_Real calculated_y_velocity_accuracy;
    /* Accuracy of calculated y velocity; knots */
    t_Callsign callsign;
    /* Callsign */
    char category_of_turbulence;
    /* Category of turbulence */
    t_Si16 cleared_flight_level;
    /* Current cleared flight level; 25 feet */
    t_Tres coasted_track;
    /* Coasted track indication */
    t_Communications_Capability communications_capability;
    /* Communications capability of the transponder */
    t_Ui16 control_position;
    /* Current control position */
    t_Data_Ages data_ages;
    /* Data ages */
    char departure_airport[4 + 1];
    /* Departure airport */
    char destination_airport[4 + 1];
    /* Destination airport */
    t_Final_State_Selected_Altitude final_state_selected_altitude;
    /* Final state selected altitude */
    t_Byte flight_category;
    /* Flight category */
    t_Tres flight_plan_correlated;
    /* Flight plan correlated */
    t_Flight_Status flight_status;
    /* Flight status */
    t_Tres formation_flight;
    /* Formation flight indication */
    t_Ui16 fpps_sacsic;
    /* FPPS SAC/SIC */
    t_Frame_Date frame_date;
    /* Frame date */
    t_Frame_Time frame_time;
    /* Frame time */
    t_Bool from_reference_trajectory;
    /* From reference trajectory */
    t_Geometric_Altitude geometric_altitude;
    /* Geometric altitude */
    t_Geometric_Vertical_Rate geometric_vertical_rate;
    /* Geometric vertical rate */
    t_Tres ghost_track;
    /* System track marked as ghost */
    t_Ground_Vector ground_vector;
    /* Ground speed and heading */
    t_Bool has_been_listed;
    /* System track information has been listed */
    t_Indicated_Airspeed indicated_airspeed;
    /* Indicated airspeed */
    t_Data_Source_Identifier last_updating_sensor;
    /* Last updating sensor (SAC/SIC) */
    t_Byte line_number;
    /* Board/line number */
    t_Track_Number local_track_number;
    /* Local track number */
    t_Mach_Number mach_number;
    /* Air speed - Mach number */
    t_Magnetic_Heading magnetic_heading;
    /* Magnetic heading */
    t_Tres manoeuvring_track;
    /* System track marked as manoeuvring */
    t_Measured_Information measured_information;
    /* Measured information */
    t_Flight_Level measured_track_mode_c_height;
    /* Measured track mode C height */
    t_Tres military_emergency;
    /* Military emergency indication */
    t_Tres military_ident;
    /* Military identification */
    t_Bool miss_update;
    /* MISS (not PLOT) update */
    t_Strk_Mode_Info mode_1_info;
    /* SSR mode 1 information */
    t_Strk_Mode_Info mode_2_info;
    /* SSR mode 2 information */
    t_Strk_Mode_Info mode_3a_info;
    /* SSR mode 3/A information */
    t_Byte mode_4_info;
    /* SSR mode 4 information */
    t_Strk_Mode5_Data mode_5_data;
    /* SSR mode 5 data */
    t_Strk_Mode5_Info mode_5_info;
    /* SSR mode 5 information */
    t_Strk_Mode_of_Flight mode_of_flight;
    /* Calculated mode of flight */
    t_Mode_S_MB_Data mode_s_mb_data;
    /* SSR mode S MB data (BDS registers) */
    t_Byte mof_longitudinal_probability;
    /* Probability for this mode of flight */
    t_Byte mof_transversal_probability;
    /* Probability for this mode of flight */
    t_Byte mof_vertical_probability;
    /* Probability for this mode of flight */
    t_Byte most_reliable_height;
    /* Most reliable height */
    /*  0 ... barometric altitude,
    1 ... geometric altitude
*/
    t_Tres multi_sensor_track;
    /* Multi-sensor or mono-sensor track */
    t_Tres observed_by_ads;
    /* Observed by ADS */
    t_Tres observed_by_mds;
    /* Observed by SSR mode S */
    t_Tres observed_by_psr;
    /* Observed by PSR */
    t_Tres observed_by_ssr;
    /* Observed by SSR */
    t_Ui16 pln_number;
    /* PLN number */
    t_Real plot_mode_3a_age;
    /* Age of the last SSR mode 3/A used to update
   the track; seconds */
    t_Real plot_mode_c_age;
    /* Age of the last SSR mode C used to update
   the track; seconds */
    t_Byte position_uncertainty;
    /* Position uncertainty */
    t_Tres possible_split_pair;
    /* Possibly a component of a split track pair */
    t_Tres possible_split_parent;
    /* Parent of a possible split track */
    t_Tres possible_split_track;
    /* Possibly a split track */
    t_Strk_Selected_Altitude selected_altitude;
    /* Selected altitude */
    t_Sensor_Number sensor_number;
    /* Sensor number */
    t_Data_Source_Identifier server_sacsic;
    /* Server SAC/SIC */
    t_Service_Identification service_identification;
    /* Service identification */
    t_Tres simulated;
    /* Simulated or active system track flag */
    t_Tres slave_track_promotion;
    /* Slave track promotion */
    t_Tres special_position_indication;
    /* Special position indication */
    t_Tres special_used_code;
    /* Special used SSR mode 3/A code */
    int split_qualification_factor;
    /* Qualification factor for split pair */
    t_Strk_Target_Size target_size;
    /* Target size and orientation */
    t_Tres tentative_track;
    /* Tentative or confirmed track */
    t_Tres test_target;
    /* Test target track flag */
    t_Time_Delay time_delay;
    /* Time delay, i. e., difference between receiving and
   last update time */
    t_Time_of_Day time_of_last_update;
    /* Time of last update */
    t_Time_of_Day time_of_message;
    /* Time of message */
    t_Bool to_be_listed;
    /* System track information to be listed */
    t_Tres track_created;
    /* Track created */
    t_Real track_mode_3a_age;
    /* Age of last detection of SSR mode 3/A;
   seconds */
    t_Track_Number track_number;
    /* Track number */
    t_Byte track_numbering_indicator;
    /* Track numbering indicator */
    t_Byte track_position_coding_precision;
    /* Track position coding precision:
   0 ... 1/64 nmi
   1 ... 1/32 nmi
*/
    t_Real track_psr_age;
    /* Age of last primary plot or local track
   used to update the track; seconds */
    t_Byte track_quality;
    /* Track quality */
    t_Real track_ssr_age;
    /* Age of last secondary plot or local track
   used to update the track; seconds */
    t_Bool track_status_present;
    /* Track status present */
    t_Tres track_terminated;
    /* Track terminated */
    t_Tres track_with_aircraft_derived_data;
    /* Track updated with aircraft derived data */
    t_Tres transponder_delay_correction;
    /* Accurate estimation of transponder delay */
    t_True_Airspeed true_airspeed;
    /* True airspeed */
    char type_of_aircraft[4 + 1];
    /* Type of aircraft */
    t_Byte type_of_message;
    /* Type of message (family, nature) */
    t_Strk_Update_Ages update_ages;
    /* System track update ages */
    t_Ui16 user_number;
    /* User number */
    t_Vehicle_Fleet_Identification vehicle_fleet_identification;
    /* Vehicle fleet identification */

#if USE_JSON
    void toJSON (nlohmann::json& j)
    {
        j["message_type"] = "track update";
        j["version"] = "1.0";

        // t_Byte asterix_category;
        j ["asterix_category"] = asterix_category;
        /* ASTERIX category */

        // t_Bool calculated_cartesian_velocity_accuracy_present;
        /* Accuracy of calculated Cartesian track
       velocity present */
        //t_Real calculated_x_velocity_accuracy;
        /* Accuracy of calculated x velocity; knots */
        //t_Real calculated_y_velocity_accuracy;
        /* Accuracy of calculated y velocity; knots */
        if (calculated_cartesian_velocity_accuracy_present)
        {
            j ["calculated_x_velocity_accuracy_kn"] = calculated_x_velocity_accuracy;
            j ["calculated_y_velocity_accuracy_kn"] = calculated_y_velocity_accuracy;
        }
        else if (write_json_nulls)
        {
            j ["calculated_x_velocity_accuracy_kn"] = nullptr;
            j ["calculated_y_velocity_accuracy_kn"] = nullptr;
        }

        //t_Bool calculated_polar_velocity_accuracy_present;
        /* Accuracy of calculated polar track velocity
       present */
        //t_Real calculated_track_speed_accuracy;
        /* Accuracy of calculated track speed; knots */
        // t_Real calculated_track_heading_accuracy;
        /* Accuracy of calculated track heading;
       degrees */
        if (calculated_polar_velocity_accuracy_present)
        {
            j ["calculated_track_speed_accuracy_kn"] = calculated_track_speed_accuracy;
            j ["calculated_track_heading_accuracy_deg"] = calculated_track_heading_accuracy;
        }
        else if (write_json_nulls)
        {
            j ["calculated_track_speed_accuracy_kn"] = nullptr;
            j ["calculated_track_heading_accuracy_deg"] = nullptr;
        }

        //t_Bool calculated_polar_velocity_present;
        /* Calculated polar track velocity present */
        // t_Real calculated_track_speed;
        /* Calculated track speed; metres/second */
        // t_Real calculated_track_heading;
        /* Calculated track heading; radians */
        if (calculated_polar_velocity_present)
        {
            j ["calculated_track_speed_ms"] = calculated_track_speed;
            j ["calculated_track_heading_rad"] = calculated_track_heading;
        }
        else if (write_json_nulls)
        {
            j ["calculated_track_speed_ms"] = nullptr;
            j ["calculated_track_heading_rad"] = nullptr;
        }

        // t_Bool calculated_position_accuracy_present;
        /* Calculated position accuracy present */
        // t_Real calculated_x_position_accuracy;
        /* Calculated x position accuracy; nautical miles */
        // t_Real calculated_y_position_accuracy;
        /* Calculated y position accuracy; nautical miles */
        if (calculated_position_accuracy_present)
        {
            j ["calculated_x_position_accuracy_nm"] = calculated_x_position_accuracy;
            j ["calculated_y_position_accuracy_nm"] = calculated_y_position_accuracy;
        }
        else if (write_json_nulls)
        {
            j ["calculated_x_position_accuracy_nm"] = nullptr;
            j ["calculated_y_position_accuracy_nm"] = nullptr;
        }

        // t_Bool calculated_rate_of_climb_descent_accuracy_present;
        /* Accuracy of calculated rate of climb/descent
       present */
        // t_Real calculated_rate_of_climb_descent_accuracy;
        /* Accuracy of calculated rate of climb/descent;
       feet/minute */
        if (calculated_rate_of_climb_descent_accuracy_present)
            j ["calculated_rate_of_climb_descent_accuracy_fm"] = calculated_rate_of_climb_descent_accuracy;
        else if (write_json_nulls)
            j ["calculated_rate_of_climb_descent_accuracy_fm"] = nullptr;

        // t_Bool calculated_rate_of_turn_accuracy_present;
        /* Accuracy of calculated rate of turn present */
        // t_Real calculated_rate_of_turn_accuracy;
        /* Accuracy of calculated rate of turn;
       degrees/second */
        if (calculated_rate_of_turn_accuracy_present)
            j ["calculated_rate_of_turn_accuracy_degs"] = calculated_rate_of_turn_accuracy;
        else if (write_json_nulls)
            j ["calculated_rate_of_turn_accuracy_degs"] = nullptr;

        // t_Bool calculated_rate_of_turn_present;
        /* Calculated rate of turn present */
        // t_Real calculated_rate_of_turn;
        /* Calculated rate of turn; degrees/second */
        if (calculated_rate_of_turn_present)
            j ["calculated_rate_of_turn_degs"] = calculated_rate_of_turn;
        else if (write_json_nulls)
            j ["calculated_rate_of_turn_degs"] = nullptr;

        // t_Bool calculated_track_altitude_accuracy_present;
        /* Calculated track altitude accuracy present */
        // t_Real calculated_track_altitude_accuracy;
        /* Accuracy of calculated track altitude; meters */
        if (calculated_track_altitude_accuracy_present)
            j ["calculated_track_altitude_accuracy_m"] = calculated_track_altitude_accuracy;
        else if (write_json_nulls)
            j ["calculated_track_altitude_accuracy_m"] = nullptr;

        // t_Bool calculated_track_flight_level_accuracy_present;
        /* Accuracy of calculated track flight level
       present */
        // t_Si16 calculated_track_flight_level_accuracy;
        /* Accuracy of calculated track flight level;
       25 feet */
        if (calculated_track_flight_level_accuracy_present)
            j ["calculated_track_flight_level_accuracy_ft"] = calculated_track_flight_level_accuracy;
        else if (write_json_nulls)
            j ["calculated_track_flight_level_accuracy_ft"] = nullptr;

        // t_Bool callsign_present;
        /* Callsign present */
        // t_Callsign callsign;
        /* Callsign */
        if (callsign_present)
            j ["callsign"] = callsign.value;
        else if (write_json_nulls)
            j ["callsign"] = nullptr;

        // t_Bool category_of_turbulence_present;
        /* Category of turbulence present */
        // char category_of_turbulence;
        /* Category of turbulence */
        if (category_of_turbulence_present)
            j ["category_of_turbulence"] = category_of_turbulence;
        else if (write_json_nulls)
            j ["category_of_turbulence"] = nullptr;

        // t_Bool cleared_flight_level_present;
        /* Cleared flight level present */
        // t_Si16 cleared_flight_level;
        /* Current cleared flight level; 25 feet */
        if (cleared_flight_level_present)
            j ["cleared_flight_level_ft"] = cleared_flight_level;
        else if (write_json_nulls)
            j ["cleared_flight_level_ft"] = nullptr;

        // t_Bool control_position_present;
        /* Control position present */
        // t_Ui16 control_position;
        /* Current control position */
        if (control_position_present)
            j ["control_position"] = control_position;
        else if (write_json_nulls)
            j ["control_position"] = nullptr;

//        t_Data_Format data_format;
//        /* Data format (origin) */

        // t_Bool departure_airport_present;
        /* Departure airport present */
        // char departure_airport[4 + 1];
        /* Departure airport */
        if (departure_airport_present)
            j ["departure_airport"] = departure_airport;
        else if (write_json_nulls)
            j ["departure_airport"] = nullptr;

        // t_Bool destination_airport_present;
        /* Destination airport present */
        // char destination_airport[4 + 1];
        /* Destination airport */
        if (destination_airport_present)
            j ["destination_airport"] = destination_airport;
        else if (write_json_nulls)
            j ["destination_airport"] = nullptr;

        // t_Bool flight_category_present;
        /* Flight category present */
        // t_Byte flight_category;
        /* Flight category */
        if (flight_category_present)
            j ["flight_category"] = flight_category;
        else if (write_json_nulls)
            j ["flight_category"] = nullptr;

        // t_Bool fpps_sacsic_present;
        /* FPPS SAC/SIC present */
        // t_Ui16 fpps_sacsic;
        /* FPPS SAC/SIC */
        if (fpps_sacsic_present)
            j ["fpps_sacsic"] = fpps_sacsic;
        else if (write_json_nulls)
            j ["fpps_sacsic"] = nullptr;

        // t_Bool mode_4_info_present;
        /* SSR mode 4 information present */
        // t_Byte mode_4_info;
        /* SSR mode 4 information */
        if (mode_4_info_present)
            j ["mode_4_info"] = mode_4_info;
        else if (write_json_nulls)
            j ["mode_4_info"] = nullptr;

        // t_Bool mode_of_flight_probabilities_present;
        /* Calculated mode of flight probabilities present */
        // t_Byte mof_longitudinal_probability;
        /* Probability for this mode of flight */
        // t_Byte mof_transversal_probability;
        /* Probability for this mode of flight */
        // t_Byte mof_vertical_probability;
        /* Probability for this mode of flight */
        if (mode_of_flight_probabilities_present)
        {
            j ["mof_longitudinal_probability"] = mof_longitudinal_probability;
            j ["mof_transversal_probability"] = mof_transversal_probability;
            j ["mof_vertical_probability"] = mof_vertical_probability;
        }
        else if (write_json_nulls)
        {
            j ["mof_longitudinal_probability"] = nullptr;
            j ["mof_transversal_probability"] = nullptr;
            j ["mof_vertical_probability"] = nullptr;
        }

        // t_Bool most_reliable_height_present;
        /* Most reliable height present */
        // t_Byte most_reliable_height;
        /* Most reliable height */
        /*  0 ... barometric altitude,
        1 ... geometric altitude */
        if (most_reliable_height_present)
        {
            if (most_reliable_height)
                j ["most_reliable_height"] = "baro";
            else if (most_reliable_height)
                j ["most_reliable_height"] = "geom";
            else if (write_json_nulls)
                j ["most_reliable_height"] = nullptr;
        }
        else if (write_json_nulls)
            j ["most_reliable_height"] = nullptr;

        // t_Bool pln_number_present;
        /* PLN number present */
        // t_Ui16 pln_number;
        /* PLN number */
        if (pln_number_present)
            j ["pln_number"] = pln_number;
        else if (write_json_nulls)
            j ["pln_number"] = nullptr;

        // t_Bool plot_mode_3a_age_present;
        /* Plot age for SSR mode 3/A present */
        // t_Real plot_mode_3a_age;
        /* Age of the last SSR mode 3/A used to update
       the track; seconds */
        if (plot_mode_3a_age_present)
            j ["plot_mode_3a_age"] = plot_mode_3a_age;
        else if (write_json_nulls)
            j ["plot_mode_3a_age"] = nullptr;

        // t_Bool plot_mode_c_age_present;
        /* Plot age for SSR mode C present */
        // t_Real plot_mode_c_age;
        /* Age of the last SSR mode C used to update
       the track; seconds */
        if (plot_mode_c_age_present)
            j ["plot_mode_c_age"] = plot_mode_c_age;
        else if (write_json_nulls)
            j ["plot_mode_c_age"] = nullptr;

        // t_Bool position_uncertainty_present;
        /* Position uncertainty present */
        // t_Byte position_uncertainty;
        /* Position uncertainty */
        if (position_uncertainty_present)
            j ["position_uncertainty"] = position_uncertainty;
        else if (write_json_nulls)
            j ["position_uncertainty"] = nullptr;

        // t_Bool track_mode_3a_age_present;
        /* Track age for SSR mode 3/A code present */
        // t_Real track_mode_3a_age;
        /* Age of last detection of SSR mode 3/A;
       seconds */
        if (track_mode_3a_age_present)
            j ["track_mode_3a_age"] = track_mode_3a_age;
        else if (write_json_nulls)
            j ["track_mode_3a_age"] = nullptr;

        // t_Bool track_psr_age_present;
        /* Track age for PSR detection present */
        // t_Real track_psr_age;
        /* Age of last primary plot or local track
       used to update the track; seconds */
        if (track_psr_age_present)
            j ["track_psr_age"] = track_psr_age;
        else if (write_json_nulls)
            j ["user_number"] = nullptr;

        // t_Bool track_ssr_age_present;
        /* Track age for SSR detection present */
        // t_Real track_ssr_age;
        /* Age of last secondary plot or local track
       used to update the track; seconds */
        if (track_ssr_age_present)
            j ["track_ssr_age"] = track_ssr_age;
        else if (write_json_nulls)
            j ["track_ssr_age"] = nullptr;

        // t_Bool track_numbering_indicator_present;
        /* Track numbering indicator present */
        // t_Byte track_numbering_indicator;
        /* Track numbering indicator */
        if (track_numbering_indicator_present)
            j ["track_numbering_indicator"] = track_numbering_indicator;
        else if (write_json_nulls)
            j ["track_numbering_indicator"] = nullptr;

        // t_Bool track_quality_present;
        /* Track quality present */
        // t_Byte track_quality;
        /* Track quality */
        if (track_quality_present)
            j ["track_quality"] = track_quality;
        else if (write_json_nulls)
            j ["track_quality"] = nullptr;

        // t_Bool type_of_aircraft_present;
        /* Type of aircraft present */
        // char type_of_aircraft[4 + 1];
        /* Type of aircraft */
        if (type_of_aircraft_present)
            j ["type_of_aircraft"] = type_of_aircraft;
        else if (write_json_nulls)
            j ["type_of_aircraft"] = nullptr;

        // t_Bool type_of_message_present;
        /* Type of message present */
        // t_Byte type_of_message;
        /* Type of message (family, nature) */
        if (type_of_message_present)
            j ["type_of_message"] = type_of_message;
        else if (write_json_nulls)
            j ["type_of_message"] = nullptr;

        // t_Bool user_number_present;
        /* User number present */
        // t_Ui16 user_number;
        /* User number */
        if (user_number_present)
            j ["user_number"] = user_number;
        else if (write_json_nulls)
            j ["user_number"] = nullptr;

        // t_ACAS_Resolution_Advisory_Report acas_resolution_advisory_report;
        acas_resolution_advisory_report.toJSON(j, "acas_resolution_advisory_report");
        /* ACAS resolution advisory report */

        // t_Tres adsb_blunder_detected;
        convertToJSON(adsb_blunder_detected, "adsb_blunder_detected", j);
        /* ADS-B blunder detected */

        // t_Aircraft_Address aircraft_address;
        aircraft_address.toJSON(j, "aircraft_address");
        /* Technical SSR mode S address */

        // t_Aircraft_Identification aircraft_identification;
        aircraft_identification.toJSON(j, "aircraft_identification");
        /* Aircraft identification */

        // t_Tres amalgamated_track;
        convertToJSON(amalgamated_track, "amalgamated_track", j);
        /* Amalgamated track */

        // t_Tres assigned_code_conflict;
        convertToJSON(assigned_code_conflict, "assigned_code_conflict", j);
        /* Conflict in assigned SSR mode 3/A code */

        // t_Tres background_service;
        convertToJSON(background_service, "background_service", j);
        /* Background (not complementary) service */

        // t_Barometric_Vertical_Rate barometric_vertical_rate;
        barometric_vertical_rate.toJSON(j, "barometric_vertical_rate");
        /* Barometric vertical rate */

        // t_Computed_Acceleration calculated_acceleration;
        calculated_acceleration.toJSON(j, "calculated_acceleration");
        /* Calculated Cartesian acceleration */

        // t_Computed_Velocity calculated_cartesian_velocity;
        calculated_cartesian_velocity.toJSON(j, "calculated_cartesian_velocity");
        /* Calculated Cartesian velocity */

        // t_Computed_Position calculated_position;
        calculated_position.toJSON(j, "calculated_position");
        /* Calculated Cartesian position */

        // t_Strk_Vertical_Rate calculated_rate_of_climb_descent;
        calculated_rate_of_climb_descent.toJSON(j, "calculated_rate_of_climb_descent");
        /* Calculated rate of climb/descent */

        // t_Computed_Altitude calculated_track_altitude;
        calculated_track_altitude.toJSON(j, "calculated_track_altitude");
        /* Calculated track altitude */

        // t_Flight_Level calculated_track_flight_level;
        calculated_track_flight_level.toJSON(j, "calculated_track_flight_level");
        /* Calculated track flight level */

        // t_Strk_WGS84_Position calculated_wgs84_position;
        calculated_wgs84_position.toJSON(j, "calculated_wgs84_position");
        /* Calculated WGS-84 position */

        // t_Tres coasted_track;
        convertToJSON(coasted_track, "coasted_track", j);
        /* Coasted track indication */

        // t_Communications_Capability communications_capability;
        communications_capability.toJSON(j, "communications_capability");
        /* Communications capability of the transponder */

        // t_Data_Ages data_ages;
        data_ages.toJSON(j, "data_ages");
        /* Data ages */

        // t_Final_State_Selected_Altitude final_state_selected_altitude;
        final_state_selected_altitude.toJSON(j, "final_state_selected_altitude");
        /* Final state selected altitude */

        // t_Tres flight_plan_correlated;
        convertToJSON(flight_plan_correlated, "flight_plan_correlated", j);
        /* Flight plan correlated */

        // t_Flight_Status flight_status;
        flight_status.toJSON(j, "flight_status");
        /* Flight status */

        // t_Tres formation_flight;
        convertToJSON(formation_flight, "formation_flight", j);
        /* Formation flight indication */

        // t_Frame_Date frame_date;
        frame_date.toJSON(j, "frame_date");
        /* Frame date */

        // t_Frame_Time frame_time;
        frame_time.toJSON(j, "frame_time");
        /* Frame time */

        // t_Bool from_reference_trajectory;
        j["from_reference_trajectory"] = from_reference_trajectory;
        /* From reference trajectory */

        // t_Geometric_Altitude geometric_altitude;
        geometric_altitude.toJSON(j, "geometric_altitude");
        /* Geometric altitude */

        // t_Geometric_Vertical_Rate geometric_vertical_rate;
        geometric_vertical_rate.toJSON(j, "geometric_vertical_rate");
        /* Geometric vertical rate */

        // t_Tres ghost_track;
        convertToJSON(ghost_track, "ghost_track", j);
        /* System track marked as ghost */

        // t_Ground_Vector ground_vector;
        geometric_altitude.toJSON(j, "geometric_altitude");
        /* Ground speed and heading */

//        t_Bool has_been_listed;
//        /* System track information has been listed */

        // t_Indicated_Airspeed indicated_airspeed;
        indicated_airspeed.toJSON(j, "indicated_airspeed_ms");
        /* Indicated airspeed */

        //t_Data_Source_Identifier last_updating_sensor;
        last_updating_sensor.toJSON(j, "last_updating_sensor");
        /* Last updating sensor (SAC/SIC) */

        // t_Byte line_number;
        j["line_number"] = line_number;
        /* Board/line number */

        // t_Track_Number local_track_number;
        local_track_number.toJSON(j, "local_track_number");
        /* Local track number */

        // t_Mach_Number mach_number;
        mach_number.toJSON(j, "mach_number");
        /* Air speed - Mach number */

        // t_Magnetic_Heading magnetic_heading;
        magnetic_heading.toJSON(j, "magnetic_heading");
        /* Magnetic heading */

        // t_Tres manoeuvring_track;
        convertToJSON(manoeuvring_track, "manoeuvring_track", j);
        /* System track marked as manoeuvring */

        //t_Measured_Information measured_information;
        measured_information.toJSON(j, "measured_information");
        /* Measured information */

        // t_Flight_Level measured_track_mode_c_height;
        measured_track_mode_c_height.toJSON(j, "measured_information");
        /* Measured track mode C height */

        // t_Tres military_emergency;
        convertToJSON(military_emergency, "military_emergency", j);
        /* Military emergency indication */

        // t_Tres military_ident;
        convertToJSON(military_ident, "military_ident", j);
        /* Military identification */

        // t_Bool miss_update;
        j["miss_update"] = miss_update;
        /* MISS (not PLOT) update */

        // t_Strk_Mode_Info mode_1_info;
        mode_1_info.toJSON(j, "mode_1_info");
        /* SSR mode 1 information */

        // t_Strk_Mode_Info mode_2_info;
        mode_2_info.toJSON(j, "mode_2_info");
        /* SSR mode 2 information */

        // t_Strk_Mode_Info mode_3a_info;
        mode_3a_info.toJSON(j, "mode_3a_info");
        /* SSR mode 3/A information */

        // t_Strk_Mode5_Data mode_5_data;
        mode_5_data.toJSON(j, "mode_5_data");
        /* SSR mode 5 data */

        // t_Strk_Mode5_Info mode_5_info;
        mode_5_info.toJSON(j, "mode_5_info");
        /* SSR mode 5 information */

        // t_Strk_Mode_of_Flight mode_of_flight;
        mode_of_flight.toJSON(j, "mode_of_flight");
        /* Calculated mode of flight */

        // t_Mode_S_MB_Data mode_s_mb_data;
        mode_s_mb_data.toJSON(j, "bds");
        /* SSR mode S MB data (BDS registers) */

        // t_Tres multi_sensor_track;
        convertToJSON(multi_sensor_track, "multi_sensor_track", j);
        /* Multi-sensor or mono-sensor track */

        // t_Tres observed_by_ads;
        convertToJSON(observed_by_ads, "observed_by_ads", j);
        /* Observed by ADS */

        // t_Tres observed_by_mds;
        convertToJSON(observed_by_mds, "observed_by_mds", j);
        /* Observed by SSR mode S */

        // t_Tres observed_by_psr;
        convertToJSON(observed_by_psr, "observed_by_psr", j);
        /* Observed by PSR */

        // t_Tres observed_by_ssr;
        convertToJSON(observed_by_ssr, "observed_by_ssr", j);
        /* Observed by SSR */

        // t_Tres possible_split_pair;
        convertToJSON(possible_split_pair, "possible_split_pair", j);
        /* Possibly a component of a split track pair */

        // t_Tres possible_split_parent;
        convertToJSON(possible_split_parent, "possible_split_parent", j);
        /* Parent of a possible split track */

        // t_Tres possible_split_track;
        convertToJSON(possible_split_track, "possible_split_track", j);
        /* Possibly a split track */

        // t_Strk_Selected_Altitude selected_altitude;
        selected_altitude.toJSON(j, "selected_altitude");
        /* Selected altitude */

//        t_Sensor_Number sensor_number;
//        /* Sensor number */

        // t_Data_Source_Identifier server_sacsic;
        server_sacsic.toJSON(j, "server_sacsic");
        /* Server SAC/SIC */

        // t_Service_Identification service_identification;
        service_identification.toJSON(j, "service_identification");
        /* Service identification */

        // t_Tres simulated;
        convertToJSON(simulated, "simulated", j);
        /* Simulated or active system track flag */

        // t_Tres slave_track_promotion;
        convertToJSON(slave_track_promotion, "slave_track_promotion", j);
        /* Slave track promotion */

        // t_Tres special_position_indication;
        convertToJSON(special_position_indication, "special_position_indication", j);
        /* Special position indication */

        // t_Tres special_used_code;
        convertToJSON(special_used_code, "special_used_code", j);
        /* Special used SSR mode 3/A code */

        // int split_qualification_factor;
        j["split_qualification_factor"] = split_qualification_factor;
        /* Qualification factor for split pair */

        // t_Strk_Target_Size target_size;
        target_size.toJSON(j, "target_size");
        /* Target size and orientation */

        // t_Tres tentative_track;
        convertToJSON(tentative_track, "tentative_track", j);
        /* Tentative or confirmed track */

        // t_Tres test_target;
        convertToJSON(test_target, "test_target", j);
        /* Test target track flag */

        // t_Time_Delay time_delay;
        time_delay.toJSON(j, "time_delay_s");
        /* Time delay, i. e., difference between receiving and
       last update time */

        // t_Time_of_Day time_of_last_update;
        time_of_last_update.toJSON(j, "time_of_last_update");
        /* Time of last update */

        // t_Time_of_Day time_of_message;
        time_of_message.toJSON(j, "time_of_message");
        /* Time of message */

//        t_Bool to_be_listed;
//        /* System track information to be listed */

        // t_Tres track_created;
        convertToJSON(track_created, "track_created", j);
        /* Track created */

        // t_Track_Number track_number;
        track_number.toJSON(j, "track_number");
        /* Track number */

        // t_Byte track_position_coding_precision;
        /* Track position coding precision:
       0 ... 1/64 nmi
       1 ... 1/32 nmi */
        switch (track_position_coding_precision) {
        case 0:
            j["track_position_coding_precision"] = "1/64 nmi";
            break;
        case 1:
            j["track_position_coding_precision"] = "1/32 nmi";
            break;
        default:
            if (write_json_nulls)
                j["track_position_coding_precision"] = nullptr;
            break;
        }

        // t_Bool track_status_present;
        j["track_status_present"] = track_status_present;
        /* Track status present */

        // t_Tres track_terminated;
        convertToJSON(track_terminated, "track_terminated", j);
        /* Track terminated */

        // t_Tres track_with_aircraft_derived_data;
        convertToJSON(track_with_aircraft_derived_data, "track_with_aircraft_derived_data", j);
        /* Track updated with aircraft derived data */

        // t_Tres transponder_delay_correction;
        convertToJSON(transponder_delay_correction, "transponder_delay_correction", j);
        /* Accurate estimation of transponder delay */

        // t_True_Airspeed true_airspeed;
        true_airspeed.toJSON(j, "true_airspeed");
        /* True airspeed */

        // t_Strk_Update_Ages update_ages;
        update_ages.toJSON(j, "update_ages");
        /* System track update ages */

        // t_Vehicle_Fleet_Identification vehicle_fleet_identification;
        vehicle_fleet_identification.toJSON(j, "vehicle_fleet_identification");
        /* Vehicle fleet identification */
    }
#endif
} t_Strk;

/* Wall (date and) time: */
typedef struct
{
    int year;
    int month;
    int day;
    int hours;
    int minutes;
    int seconds;
    int milliseconds;
} t_Wall_Time;

#endif // COMMON_STRUCTS_CPP
