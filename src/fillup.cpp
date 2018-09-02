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
/* File:           fillup.cpp                                                 */
/* Contents:       Filling up partial time-of-day values                      */
/* Author(s):      kb                                                         */
/* Last change:    2018-04-27                                                 */
/*----------------------------------------------------------------------------*/

#include "basics.h"
                   /* Basic system definitions */
#include "common.h"
                   /* Common declarations and definitions */

#include <stddef.h>

                   /* Local macros: */
                   /* ------------- */
 
#define M_AFTER_MIDNIGHT 7680
                   /* Special (short) period after midnight,
				      i.e. 1 minute in 1/128 seconds */
#define M_C2_THRESHOLD 5
                   /* Threshold for condition in case 2 */
                   /* Each unit corresponds to 2 seconds */

                   /* Local data: */
                   /* ----------- */

static const t_Byte max_ub0_after_midnight = 1;
                   /* For fill-up of partial time of day in ASTERIX */

/*----------------------------------------------------------------------------*/
/* fillup_tod      -- Fill up truncated time of day                           */
/*----------------------------------------------------------------------------*/

 t_Retc fillup_tod (t_Ui16 partial_tod, t_Ui32 *tod_ptr)
{
	t_Bool c2_new; /* Condition for case 2 (new implementation) */
	t_Bool merged; /* Values successfully merged */
	t_Retc ret;    /* Return code */
	t_Ui32 tod;    /* Full time of day; 1/128 seconds */
	t_Si16 ub_diff;
                   /* Absolute difference in upper bytes */
	t_Byte ub0;    /* Upper byte of last time of day */
	t_Byte ub1;    /* Upper byte of partial time of day */

                   /* Preset the return code: */
	ret = RC_FAIL;

                   /* Check parameters: */
	Assert (tod_ptr != NULL, "Invalid parameter");

                   /* Is last time of day available ? */
	if (!last_tod_available)
	{
		ret = RC_SKIP;
    	goto done;
	}

                   /* Extract the missing byte: */
	tod = last_tod & 0x00ff0000;

                   /* Extract upper byte of the two values
                      to be merged: */
	ub0 = (t_Byte) ((last_tod >> 8) & 0x000000ff);
	ub1 = (t_Byte) ((partial_tod >> 8) & 0x00ff);

                   /* Evaluate condition for case 2: */

	c2_new = FALSE;
	ub_diff = (t_Si16) ub0 - (t_Si16) ub1;
	if (ub_diff < 0)
	{
		ub_diff = -ub_diff;
	}
	if (ub_diff <= M_C2_THRESHOLD || ub_diff >= (255 - M_C2_THRESHOLD))
	{
		c2_new = TRUE;
	}

	merged = FALSE;

                   /* Case 1: Partial time of day is one unit higher */
	if (ub0 == 255 && ub1 == 0)
	{
		tod |= ((t_Time) partial_tod & 0x0000ffff);
		tod += 0x00010000;
		merged = TRUE;
	}

                   /* Case 2: Upper bytes fit (somehow) */
	else if (c2_new)
	{
		if (after_midnight &&
            ub0 <= max_ub0_after_midnight && ub1 >= 0xbd)
        {
                   /* Go back before midnight (which is 0x00a8c000) */
			tod = 0x00a80000 | ((t_Ui32) partial_tod & 0x0000ffff);
		}
		else
		{
			tod |= ((t_Ui32) partial_tod & 0x0000ffff);

			if (partial_tod > (t_Ui16) 0xfa00)
			{
				if ((t_Ui16) (last_tod & 0x0000ffff) < 0x0500)
				{
					tod -= 0x00010000;
				}
			}
		}
		merged = TRUE;
	}

                   /* Case 3: Fit near overflow */
	else if (ub0 > 245 && 256 + ub1 < ub0 + 10)
	{
		tod |= ((t_Time) partial_tod & 0x0000ffff);
		merged = TRUE;
	}

                   /* Case 4: Crossing midnight */
	else if ((last_tod & 0x00ffff00) >= 0x00a8bd00)
	{
		tod = (t_Time) partial_tod;
		merged = TRUE;
	}

                   /* Have we successfully merged the available
				      time of day information ? */
	if (!merged)
	{
		goto done;
	}

                   /* Update last time of day, if higher/later: */
	if (tod > last_tod)
    {
		if (!after_midnight || (tod <= M_AFTER_MIDNIGHT))
		{
			last_tod = tod;
		}
        after_midnight = (last_tod <= M_AFTER_MIDNIGHT);
                   /* This is necessary especially with long-period
                      north-marker-only radars where radar service
                      messages (with full time of day) are sparse */
	}

                   /* Set the return value: */
	*tod_ptr = tod;

                   /* Set the return code: */
	ret = RC_OKAY;

	done:          /* We are done */
	return ret;
}
/* end-of-file */
