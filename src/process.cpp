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
/* File:           src/process.cpp                                            */
/* Contents:       Process decoded input message                              */
/* Author(s):      kb                                                         */
/* Last change:    2018-04-27                                                 */
/*----------------------------------------------------------------------------*/

#include "global.h"
#include "basics.h"
/* Basic system definitions */
#include "common.h"
/* Common declarations and definitions */

#if USE_JSON
#include "jsonwriter.h"
#endif

#include <stddef.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include <iostream>

/*----------------------------------------------------------------------------*/
/* process_adsb    -- Process ADS-B report                                    */
/*----------------------------------------------------------------------------*/
t_Retc process_adsb (t_Adsb *adsb_ptr)
{
    t_Retc ret;    /* Return code */

    /* Preset the return code: */
    ret = RC_FAIL;

    /* Check parameters: */
    Assert (adsb_ptr != NULL, "Invalid parameter");

#if LISTER
    /* And list: */
    list_adsb (adsb_ptr);
#endif /* LISTER */

#if USE_JSON
    if (json_output_type != JSON_NONE)
    {
        Assert (json_writer != nullptr, "JSON writer doesn't exist");
        json_writer->write(adsb_ptr);
    }
#endif

    /* Set the return code: */
    ret = RC_OKAY;

    return ret;
}

/*----------------------------------------------------------------------------*/
/* process_init    -- Initiate message processing                             */
/*----------------------------------------------------------------------------*/

void process_init (void)
{
    /* We are done: */
    return;
}

/*----------------------------------------------------------------------------*/
/* process_mlat    -- Process multilateration target report                   */
/*----------------------------------------------------------------------------*/

t_Retc process_mlat (t_Mlat *mlat_ptr)
{
    t_Retc ret;    /* Return code */

    /* Preset the return code: */
    ret = RC_FAIL;

    /* Check parameters: */
    Assert (mlat_ptr != NULL, "Invalid parameter");

#if LISTER
    /* And list: */
    list_mlat (mlat_ptr);
#endif /* LISTER */

#if USE_JSON
    if (json_output_type != JSON_NONE)
    {
        Assert (json_writer != nullptr, "JSON writer doesn't exist");
        json_writer->write(mlat_ptr);
    }
#endif

    /* Set the return code: */
    ret = RC_OKAY;

    return ret;
}

/*----------------------------------------------------------------------------*/
/* process_rsrv    -- Process radar service message                           */
/*----------------------------------------------------------------------------*/

t_Retc process_rsrv (t_Rsrv *rsrv_ptr)
{
    t_Retc ret;    /* Return code */

    /* Preset the return code: */
    ret = RC_FAIL;

    /* Check parameters: */
    Assert (rsrv_ptr != NULL, "Invalid parameter");

#if LISTER
    /* And list: */
    list_rsrv (rsrv_ptr);
#endif /* LISTER */

#if USE_JSON
    if (json_output_type != JSON_NONE)
    {
        Assert (json_writer != nullptr, "JSON writer doesn't exist");
        json_writer->write(rsrv_ptr);
    }
#endif

    /* Set the return code: */
    ret = RC_OKAY;

    return ret;
}

/*----------------------------------------------------------------------------*/
/* process_rtgt    -- Process radar target report                             */
/*----------------------------------------------------------------------------*/

t_Retc process_rtgt (t_Rtgt *rtgt_ptr)
{
    t_Retc ret;    /* Return code */

    /* Preset the return code: */
    ret = RC_FAIL;

    /* Check parameters: */
    Assert (rtgt_ptr != NULL, "Invalid parameter");

#if LISTER
    /* And list: */
    list_rtgt (rtgt_ptr);
#endif /* LISTER */

#if USE_JSON
    if (json_output_type != JSON_NONE)
    {
        Assert (json_writer != nullptr, "JSON writer doesn't exist");
        json_writer->write(rtgt_ptr);
    }
#endif

    /* Set the return code: */
    ret = RC_OKAY;

    return ret;
}

/*----------------------------------------------------------------------------*/
/* process_strk    -- Process system track message                            */
/*----------------------------------------------------------------------------*/

t_Retc process_strk (t_Strk *strk_ptr)
{
    t_Retc ret;    /* Return code */

    /* Preset the return code: */
    ret = RC_FAIL;

    /* Check parameters: */
    Assert (strk_ptr != NULL, "Invalid parameter");

#if LISTER
    /* And list: */
    list_strk (strk_ptr);
#endif /* LISTER */

#if USE_JSON
    if (json_output_type != JSON_NONE)
    {
        Assert (json_writer != nullptr, "JSON writer doesn't exist");
        json_writer->write(strk_ptr);
    }
#endif

    /* Set the return code: */
    ret = RC_OKAY;

    return ret;
}

/*----------------------------------------------------------------------------*/
/* process_term    -- Terminate message processing                            */
/*----------------------------------------------------------------------------*/

void process_term (void)
{
    /* We are done: */
    return;
}
/* end-of-file */
