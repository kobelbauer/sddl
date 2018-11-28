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

#ifndef COMMON_DATA_TYPES_H
#define COMMON_DATA_TYPES_H

#include "basics.h"

/* Basic data types: */
/* ----------------- */

typedef unsigned char t_Bool;
/* Boolean */
typedef unsigned char t_Byte;
/* Byte, i.e. unsigned 8 bit integer */
typedef void* t_Pntr;
/* Untyped pointer */
typedef double t_Real;
/* Floating-point number */
typedef unsigned int t_Uint;
/* Unsigned integer */


#if WORDSIZE == 16
typedef signed char t_Si08;
                   /* Signed 8 bit integer */
typedef signed int Si16;
                   /* A signed 16-bits natural */
typedef signed long int Ui32;
                   /* A signed 32-bits natural */
typedef unsigned int Ui16;
                   /* An unsigned 16-bits natural */
typedef unsigned long int Ui32;
                   /* An unsigned 32-bits natural */
#elif WORDSIZE == 32
typedef signed char t_Si08;
                   /* Signed 8 bit integer */
typedef signed short int t_Si16;
                   /* Signed 16 bit integer */
typedef signed long int t_Si32;
                   /* Signed 32 bit integer */
typedef unsigned short int t_Ui16;
                   /* Unsigned 16 bit integer */
typedef unsigned long int t_Ui32;
                   /* Unsigned 32 bit integer */
#elif WORDSIZE == 64
typedef signed char t_Si08;
                   /* Signed 8 bit integer */
typedef signed short int t_Si16;
                   /* Signed 16 bit integer */
typedef signed int t_Si32;
                   /* Signed 32 bit integer */
typedef unsigned short int t_Ui16;
                   /* Unsigned 16 bit integer */
typedef unsigned int t_Ui32;
                   /* Unsigned 32 bit integer */
#else
#error "Unexpected WORDSIZE"
#endif

typedef int t_Retc;
                   /* Return code */
typedef double t_Secs;
                   /* Time of day; seconds */
typedef signed long int t_Time;
                   /* System clock (milliseconds) */
typedef enum
{
        e_is_undefined = 0,
        e_is_false = 1,
        e_is_true = 2
} t_Tres;
                   /* Tri-state truth value */

/* Application data types: */
/* ----------------------- */

typedef t_Retc (*t_P_Fp) (t_Ui16, t_Byte *);
/* Pointer to ASTERIX data item processing function */
typedef t_Retc (*t_R_Fp) (t_Ui16, t_Byte *, t_Ui16 *);
/* Pointer to ASTERIX data item reading function */

/* Radar service message types: */
typedef enum
{
    e_rsrv_undefined = 0,
    /* Undefined service message */
    e_rsrv_north_marker,
    /* North crossing */
    e_rsrv_south_marker,
    /* South crossing */
    e_rsrv_sector_crossing,
    /* Sector crossing */
    e_rsrv_supervisory,
    /* Supervisory message */
    e_rsrv_filtering_zone,
    /* Activation or deactivation of a
   filtering zone */
    e_rsrv_jamming_strobe,
    /* Jamming strobe */
    e_rsrv_sentinel
    /* Sentinel */
    /* Must be the last entry - do not remove */
} t_Rsrv_Type;

/* Target address type enumeration: */
typedef enum
{
    e_target_address_undefined = 0,
    /* Undefined or unknown target address type */
    e_target_address_non_unique,
    /* Non unique address */
    e_target_address_icao_24_bit,
    /* 24-bit ICAO address */
    e_target_address_surface_vehicle,
    /* Surface vehicle address */
    e_target_address_anonymous
    /* Anonymous address */
} t_Target_Address_Enum_Type;


#endif // COMMON_DATA_TYPES_H
