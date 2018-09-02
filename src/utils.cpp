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
/* File:           src/utils.cpp                                              */
/* Contents:       General utility functions                                  */
/* Author(s):      kb                                                         */
/* Last change:    2018-04-27                                                 */
/*----------------------------------------------------------------------------*/

#include "basics.h"
                   /* Basic system definitions */
#include "common.h"
                   /* Common declarations and definitions */

#include <ctype.h>
#include <limits.h>
#include <math.h>
#include <stddef.h>
#include <stdio.h>
#include <string.h>

				   /* Local macros: */
				   /* ------------- */

#define TSTOUT_DTM_DIFF 0
                   /* Debugging aid output for dtm_diff() */

                   /* Local data: */
                   /* ----------- */

                   /* Special tables for mc_to_alt(): */
static t_Si16 gg_table[8] = { 0, 7, 3, 4, 1, 6, 2, 5 };
static t_Si16 gz_table[8] = { 7, 0, 4, 3, 6, 1, 5, 2 };
static t_Si16 pg_table[8] = { M_UTL_NVA, 4, 2, 3, 0, M_UTL_NVA, 1, M_UTL_NVA };
static t_Si16 pz_table[8] = { M_UTL_NVA, 0, 2, 1, 4, M_UTL_NVA, 3, M_UTL_NVA };

static t_Ui16 month_length[12] =
	{ 31, 28, 31, 30, 31, 30, 31, 31, 30, 31, 30, 31 };
                   /* Number of days per month (in a "normal" year) */

/*----------------------------------------------------------------------------*/
/* azimuth         -- Compute azimuth (in degrees)                            */
/*----------------------------------------------------------------------------*/

 t_Real azimuth (t_Real xpos, t_Real ypos)
{
	t_Real azf;

                   /* Suppress very small values to inhibit
                      arithmetic overflow/underflow: */
	if (fabs (xpos) <= 1.0e-6)
	{
		xpos = 0.0;
	}
	if (fabs (ypos) <= 1.0e-6)
	{
		ypos = 0.0;
	}

                   /* Prevent abnormal behaviour of atan2(): */
	if (xpos == 0.0 && ypos == 0.0)
	{
		azf = 0.0;
		goto done;
	}

                   /* Calculate azimuth: */
	azf = atan2 (xpos, ypos);

                   /* Convert from radians to degrees
                      and map to [0.0 .. 360.0[ domain: */
	azf = M_RAD2DEG * azf;
	if (azf < 0.0)
	{
		azf += 360.0;
	}

	done:
	return azf;
}

/*----------------------------------------------------------------------------*/
/* check_date      -- Check date                                              */
/*----------------------------------------------------------------------------*/

 t_Bool check_date (int year, int month, int day)
{
	t_Bool ret;

	ret = FALSE;

	if (year < M_YEAR_MIN || year > M_YEAR_MAX)
	{
		goto done;
	}

	if (month < 1 || month > 12)
	{
		goto done;
	}

	if (day < 1 || day > 31)
	{
		goto done;
	}

	if (day > days_per_month (year, month))
	{
		goto done;
	}

	ret = TRUE;

	done:
	return ret;
}

/*----------------------------------------------------------------------------*/
/* cnv_wtm_to_tod  -- Convert wall time to time-of-day                        */
/*----------------------------------------------------------------------------*/

 t_Time cnv_wtm_to_tod (t_Wall_Time wtm)
{
	t_Time tm;

	tm = 0;

	tm = (t_Time) wtm.hours * (t_Time) 3600 +
		 (t_Time) wtm.minutes * (t_Time) 60 +
		 (t_Time) wtm.seconds;
	tm = tm * (t_Time) 1000 + wtm.milliseconds;

	return tm;
}

/*----------------------------------------------------------------------------*/
/* cnv_wtm_to_txt  -- Convert wall time to text                               */
/*----------------------------------------------------------------------------*/

 char * cnv_wtm_to_txt (t_Wall_Time wtm)
{
	static char txt[81];

	memset (txt, 0, 81);

    sprintf (txt, M_FMT_DATE " %02d:%02d:%02d",
				  wtm.year, wtm.month, wtm.day,
				  wtm.hours, wtm.minutes, wtm.seconds);

	return txt;
}

/*----------------------------------------------------------------------------*/
/* days_per_month  -- Give number of days per month                           */
/*----------------------------------------------------------------------------*/

 t_Ui16 days_per_month (t_Ui16 year, t_Ui16 month)
{
	t_Ui16 ml;     /* Month length */

                   /* Check parameter(s): */
	Assert (M_YEAR_MIN <= year && year <= M_YEAR_MAX, "Invalid parameter");
	Assert (1 <= month && month <= 12, "Invalid parameter");

	ml = month_length[month - 1];
	if (is_a_leap_year (year) && month == 2)
	{
		++ ml;
	}

	return ml;
}

/*----------------------------------------------------------------------------*/
/* days_per_year   -- Give number of days per year                            */
/*----------------------------------------------------------------------------*/

 t_Ui16 days_per_year (t_Ui16 year)
{
	t_Ui16 ret_value;
                   /* Return value */

                   /* Check parameter(s): */
	Assert (M_YEAR_MIN <= year && year <= M_YEAR_MAX, "Invalid parameter");

	if (is_a_leap_year (year))
	{
		ret_value = 366;
	}
	else
	{
		ret_value = 365;
	}

	return ret_value;
}

/*----------------------------------------------------------------------------*/
/* distance        -- Compute Euclidean distance                              */
/*----------------------------------------------------------------------------*/

 t_Real distance (t_Real x, t_Real y)
{
	t_Real ret_value;

	ret_value = sqrt (x * x + y * y);

	return ret_value;
}

/*----------------------------------------------------------------------------*/
/* distance        -- Compute Euclidean distance                              */
/*----------------------------------------------------------------------------*/

 t_Real distance (t_Real x, t_Real y, t_Real z)
{
	t_Real ret_value;

	ret_value = sqrt (x * x + y * y + z * z);

	return ret_value;
}

/*----------------------------------------------------------------------------*/
/* dtm_diff        -- Compute difference in date and time                     */
/*----------------------------------------------------------------------------*/

 t_Real dtm_diff (t_Date_Time dt1, t_Date_Time dt2)
{
	t_Ui32 count;
	int days;
	t_Secs diff;
	t_Date dt;
	t_Retc lrc;
	t_Real ret;

	ret = 0.0;

	days = 0;
	diff = 0.0;

	if (dt1.date_present && dt2.date_present)
	{
		if (same_day (dt1.date, dt2.date))
		{
			diff = dt1.time - dt2.time;
		}
		else if (later (dt1, dt2))
		{
			dt = dt1.date;

			count = 0;
			while (TRUE)
			{
				++ count;
				Assert (count <= 2, "Possibly infinite loop");

				-- dt.day;

				lrc = normalize (&dt);

				if (lrc != RC_OKAY)
				{
					goto done;
				}

				++ days;

				if (same_day (dt, dt2.date))
				{
					break;
				}
			}

			diff = dt1.time - dt2.time;
		}
		else
		{
			dt = dt1.date;

			count = 0;
			while (TRUE)
			{
				++ count;
				Assert (count <= 1000, "Possibly infinite loop");

				++ dt.day;

				lrc = normalize (&dt);

				if (lrc != RC_OKAY)
				{
					goto done;
				}

				-- days;

				if (same_day (dt, dt2.date))
				{
					break;
				}
			}

			diff = dt1.time - dt2.time;
		}
	}
	else
	{
		diff = dt1.time - dt2.time;
	}

	ret = days * M_SECONDS_PER_DAY + diff;

	done:          /* We are done */
	return ret;
}

/*----------------------------------------------------------------------------*/
/* earlier         -- Is date+time dt1 earlier than date+time dt2             */
/*----------------------------------------------------------------------------*/

 t_Bool earlier (t_Date_Time dt1, t_Date_Time dt2)
{
	t_Real diff;
	t_Bool ret;

	ret = FALSE;

	if (dt1.date_present && dt2.date_present)
	{
		if (dt1.date.year < dt2.date.year)
		{
			ret = TRUE;
		}
		else if (dt1.date.year == dt2.date.year &&
                 dt1.date.month < dt2.date.month)
		{
			ret = TRUE;
		}
		else if (dt1.date.year == dt2.date.year &&
                 dt1.date.month == dt2.date.month &&
                 dt1.date.day < dt2.date.day)
		{
			ret = TRUE;
		}
		else if (dt1.date.year == dt2.date.year &&
                 dt1.date.month == dt2.date.month &&
                 dt1.date.day == dt2.date.day)
		{
			diff = dt1.time - dt2.time;
			if (diff < 0.0)
			{
				ret = TRUE;
			}
		}
	}
	else
	{
		diff = dt1.time - dt2.time;
		if (diff < 0.0)
		{
			ret = TRUE;
		}
	}

	return ret;
}

/*----------------------------------------------------------------------------*/
/* eval_latitude   -- Evaluate a geographic latitude value                    */
/*----------------------------------------------------------------------------*/

 t_Retc eval_latitude (const char *text_ptr, t_Real *value_ptr)
{
    char ch;       /* Current character */
    int degrees;   /* Number of degrees */
    int dig;       /* Digit value */
    int fcnt;      /* Count for fraction digits */
	t_Si32 frac;   /* Decimal fraction */
	t_Real fval;   /* Auxiliary */
    int ival;      /* Integer value */
    int maxdiv;    /* Limit value */
    int maxmod;    /* Limit value */
    int minutes;   /* Number of minutes */
    const char *p; /* Auxiliary pointer */
	t_Retc ret;    /* Return code */
    int seconds;   /* Number of seconds */
    int sign;      /* Sign value */
	t_Real val;    /* Value */

                   /* Preset the return code: */
    ret = RC_FAIL;

                   /* Check parameters: */
	Assert (text_ptr != NULL, "Invalid parameter");
	Assert (value_ptr != NULL, "Invalid parameter");

				   /* Set pointer: */
	p = text_ptr;

                   /* Skip white space: */
    ch = *p ++;
    while (ch == ' ' || ch == '\t')
	{
        ch = *p ++;
	}

                   /* Check first significant character: */
    sign = 0;
    if (ch == 'N' || ch == '+')
	{
        sign = 1;
	}
    else if (ch == 'S' || ch == '-')
	{
        sign = -1;
	}
    else if (!isdigit (ch))
	{
        goto done;
	}
    else
	{
        -- p;
	}

                   /* Skip white space: */
    ch = *p;
    while (ch == ' ' || ch == '\t')
	{
        ch = *(++ p);
	}

                   /* Set some limits: */
    maxdiv = INT_MAX / 10;
    maxmod = INT_MAX % 10;

                   /* Initiate local data: */
    degrees = 0;
    minutes = 0;
    seconds = 0;
    val = 0.0;

                   /* Get number of degrees: */
    if (ch == '\0')
	{
        goto done;
	}
    ival = 0;
    while (isdigit (ch))
    {
        dig = ch - '0';
        if (ival > maxdiv)
		{
            goto done;
		}
        if (ival == maxdiv && dig > maxmod)
		{
            goto done;
		}
        ival = 10 * ival + dig;
        ch = *(++ p);
    }
    if (ival > 90)
	{
        goto done;
	}
    degrees = ival;
    val = degrees;

                   /* Check separation character: */
    if (ch == ':')
	{
        ch = *(++ p);
	}
    else if (ch == '.')
    {
        fcnt = 0;
        frac = 0L;
        fval = 1.0;
        ch = *(++ p);
        while (isdigit (ch))
        {
            dig = ch - '0';
            ++ fcnt;
            frac = frac * 10L + dig;
            fval *= 10.0;
            ch = *(++ p);
        }
        fval = frac / fval;
        val += fval;
        goto eval;
    }
	else if (ch == 'N' || ch == 'S')
	{
    	goto eval;
	}
    else
	{
        goto done;
	}

                   /* Get number of minutes: */
    if (ch == '\0')
	{
        goto done;
	}
    ival = 0;
    while (isdigit (ch))
    {
        dig = ch - '0';
        if (ival > maxdiv)
		{
            goto done;
		}
        if (ival == maxdiv && dig > maxmod)
		{
            goto done;
		}
        ival = 10 * ival + dig;
        ch = *(++ p);
    }
    if (ival > 59)
	{
        goto done;
	}
    minutes = ival;
    val = val + minutes / 60.0;

                   /* Check separation character: */
    if (ch == ':')
	{
        ch = *(++ p);
	}
    else if (ch == '.')
    {
        fcnt = 0;
        frac = 0L;
        fval = 1.0;
        ch = *(++ p);
        while (isdigit (ch))
		{
            dig = ch - '0';
            ++ fcnt;
            frac = frac * 10L + dig;
            fval *= 10.0;
            ch = *(++ p);
        }
        fval = frac / fval;
        val += fval / 60.0;
        goto eval;
    }
	else if (ch == 'N' || ch == 'S')
	{
    	goto eval;
	}
    else
	{
        goto done;
	}

                   /* Get number of seconds: */
    if (ch == '\0')
	{
        goto done;
	}
    ival = 0;
    while (isdigit (ch))
    {
        dig = ch - '0';
        if (ival > maxdiv)
		{
            goto done;
		}
        if (ival == maxdiv && dig > maxmod)
		{
            goto done;
		}
        ival = 10 * ival + dig;
        ch = *(++ p);
    }
    if (ival > 59)
	{
        goto done;
	}
    seconds = ival;
    val = val + seconds / 3600.0;

                   /* Check for separation character: */
    if (ch == '.')
    {
        fcnt = 0;
        frac = 0L;
        fval = 1.0;
        ch = *(++ p);
        while (isdigit (ch))
        {
            dig = ch - '0';
            ++ fcnt;
            frac = frac * 10L + dig;
            fval *= 10.0;
            ch = *(++ p);
        }
        fval = frac / fval;
        val += fval / 3600.0;
    }

    eval:
                   /* Skip white space: */
    ch = *p;
    while (ch == ' ' || ch == '\t')
	{
        ch = *(++ p);
	}
    if (ch != '\0')
    {
        if (ch == 'N')
        {
            if (sign)
			{
                goto done;
			}
			++ p;
			sign = 1;
		}
		else if (ch == 'S')
		{
			if (sign)
			{
				goto done;
			}
			++ p;
            sign = -1;
        }
        else
		{
            goto done;
		}
    }

                   /* Save value: */
    if (sign == 0)
	{
        sign = 1;
	}
    val = sign * val;
    if (val < -90.0 || val > +90.0)
	{
        goto done;
	}
    *value_ptr = val;

                   /* Set the return code: */
    ret = RC_OKAY;

    done:          /* We are done */
    return ret;
}

/*----------------------------------------------------------------------------*/
/* eval_longitude  -- Evaluate a geographic longitude value                   */
/*----------------------------------------------------------------------------*/

 t_Retc eval_longitude (const char *text_ptr, t_Real *value_ptr)
{
    char ch;       /* Current character */
    int degrees;   /* Number of degrees */
    int dig;       /* Digit value */
    int fcnt;      /* Count for fraction digits */
	t_Si32 frac;   /* Decimal fraction */
	t_Real fval;   /* Auxiliary */
    int ival;      /* Integer value */
    int maxdiv;    /* Limit value */
    int maxmod;    /* Limit value */
    int minutes;   /* Number of minutes */
    const char *p; /* Auxiliary pointer */
	t_Retc ret;    /* Return code */
    int seconds;   /* Number of seconds */
    int sign;      /* Sign value */
	t_Real val;    /* Value */

                   /* Preset the return code: */
    ret = RC_FAIL;

                   /* Check parameter(s): */
	Assert (text_ptr != NULL, "Invalid parameter");
	Assert (value_ptr != NULL, "Invalid parameter");

				   /* Set pointer: */
	p = text_ptr;

                   /* Skip white space: */
    ch = *p ++;
    while (ch == ' ' || ch == '\t')
	{
        ch = *p ++;
	}

                   /* Check first significant character: */
    sign = 0;
    if (ch == 'E' || ch == '+')
	{
        sign = 1;
	}
    else if (ch == 'W' || ch == '-')
	{
        sign = -1;
	}
    else if (!isdigit (ch))
	{
        goto done;
	}
    else
	{
        -- p;
	}

                   /* Skip white space: */
    ch = *p;
    while (ch == ' ' || ch == '\t')
	{
        ch = *(++ p);
	}

                   /* Set some limits: */
    maxdiv = INT_MAX / 10;
    maxmod = INT_MAX % 10;

                   /* Initiate local data: */
    degrees = 0;
    minutes = 0;
    seconds = 0;
    val = 0.0;

                   /* Get number of degrees: */
    if (ch == '\0')
	{
        goto done;
	}
    ival = 0;
    while (isdigit (ch))
    {
        dig = ch - '0';
        if (ival > maxdiv)
		{
            goto done;
		}
        if (ival == maxdiv && dig > maxmod)
		{
            goto done;
		}
        ival = 10 * ival + dig;
        ch = *(++ p);
    }
    if (ival > 180)
	{
        goto done;
	}
    degrees = ival;
    val = degrees;

                   /* Check separation character: */
    if (ch == ':')
	{
        ch = *(++ p);
	}
    else if (ch == '.')
    {
        fcnt = 0;
        frac = 0L;
        fval = 1.0;
        ch = *(++ p);
        while (isdigit (ch))
        {
            dig = ch - '0';
            ++ fcnt;
            frac = frac * 10L + dig;
            fval *= 10.0;
            ch = *(++ p);
        }
        fval = frac / fval;
        val += fval;
        goto eval;
    }
	else if (ch == 'E' || ch == 'W')
	{
    	goto eval;
	}
    else
	{
        goto done;
	}

                   /* Get number of minutes: */
    if (ch == '\0')
	{
        goto done;
	}
    ival = 0;
    while (isdigit (ch))
    {
        dig = ch - '0';
        if (ival > maxdiv)
		{
            goto done;
		}
        if (ival == maxdiv && dig > maxmod)
		{
            goto done;
		}
        ival = 10 * ival + dig;
        ch = *(++ p);
    }
    if (ival > 59)
	{
        goto done;
	}
    minutes = ival;
    val = val + minutes / 60.0;

                   /* Check separation character: */
    if (ch == ':')
	{
        ch = *(++ p);
	}
    else if (ch == '.')
    {
        fcnt = 0;
        frac = 0L;
        fval = 1.0;
        ch = *(++ p);
        while (isdigit (ch))
		{
            dig = ch - '0';
            ++ fcnt;
            frac = frac * 10L + dig;
            fval *= 10.0;
            ch = *(++ p);
        }
        fval = frac / fval;
        val += fval / 60.0;
        goto eval;
    }
	else if (ch == 'E' || ch == 'W')
	{
    	goto eval;
	}
    else
	{
        goto done;
	}

                   /* Get number of seconds: */
    if (ch == '\0')
	{
        goto done;
	}
    ival = 0;
    while (isdigit (ch))
    {
        dig = ch - '0';
        if (ival > maxdiv)
		{
            goto done;
		}
        if (ival == maxdiv && dig > maxmod)
		{
            goto done;
		}
        ival = 10 * ival + dig;
        ch = *(++ p);
    }
    if (ival > 59)
	{
        goto done;
	}
    seconds = ival;
    val = val + seconds / 3600.0;

                   /* Check for separation character: */
    if (ch == '.')
    {
        fcnt = 0;
        frac = 0L;
        fval = 1.0;
        ch = *(++ p);
        while (isdigit (ch))
        {
            dig = ch - '0';
            ++ fcnt;
            frac = frac * 10L + dig;
            fval *= 10.0;
            ch = *(++ p);
        }
        fval = frac / fval;
        val += fval / 3600.0;
    }

    eval:
				   /* Skip white space: */
    ch = *p;
    while (ch == ' ' || ch == '\t')
	{
        ch = *(++ p);
	}
    if (ch != '\0')
    {
        if (ch == 'E')
        {
            if (sign)
			{
                goto done;
			}
			++ p;
			sign = 1;
		}
		else if (ch == 'W')
		{
			if (sign)
			{
				goto done;
			}
			++ p;
            sign = -1;
        }
        else
		{
            goto done;
		}
    }

                   /* Save value: */
    if (sign == 0)
	{
        sign = 1;
	}
    val = sign * val;
    if (val < -180.0 || val > +180.0)
	{
        goto done;
	}
    *value_ptr = val;

                   /* Set the return code: */
    ret = RC_OKAY;

    done:          /* We are done */
	return ret;
}

/*----------------------------------------------------------------------------*/
/* expand_c        -- Expand ICAO 6-bits character                            */
/*----------------------------------------------------------------------------*/

 char expand_c (t_Byte c)
{
	char ch;

                   /* Check parameters: */
	Assert (c < 64, "Invalid parameter");

                   /* Expand: */
	ch = '?';
	if (1 <= c && c <= 26)
	{
		ch = 'A' + (c - 1);
	}
	else if (c == 32)
	{
		ch = ' ';
	}
	else if (48 <= c && c <= 57)
	{
		ch = '0' + (c - 48);
	}

	return ch;
}

/*----------------------------------------------------------------------------*/
/* hi_byte         -- Extract high byte                                       */
/*----------------------------------------------------------------------------*/

 t_Byte hi_byte (t_Ui16 u16)
{
	t_Byte b;

	b = (t_Byte) ((u16 >> 8) & 0x00ff);

	return b;
}

/*----------------------------------------------------------------------------*/
/* is_a_leap_year  -- Decide whether year is a leap year                      */
/*----------------------------------------------------------------------------*/

 t_Bool is_a_leap_year (t_Ui16 year)
{
	t_Bool leap;   /* Leap year flag */

                   /* Check parameters: */
	Assert (M_YEAR_MIN <= year && year <= M_YEAR_MAX, "Invalid parameter");

	leap = FALSE;
	if ((year % 4) == 0)
	{
		leap = TRUE;
	}
	if ((year % 100) == 0)
	{
		leap = FALSE;
	}
	if ((year % 400) == 0)
	{
		leap = TRUE;
	}

	return leap;
}

/*----------------------------------------------------------------------------*/
/* lat_text        -- Return text for geographical latitude                   */
/*----------------------------------------------------------------------------*/

 char * lat_text (t_Real latitude)
{
	t_Real abslat;
	int deg, min, sec, msec;
	static char txt[31];

    memset (txt, 0, 31);

	abslat = latitude;
	if (latitude < 0.0)
	{
		abslat = -latitude;
	}

	deg = (int) abslat;
	abslat -= (t_Real) deg;
	min = (int) (60.0 * abslat);
	abslat = 60.0 * abslat - min;
	sec = (int) (60.0 * abslat);
	abslat = 60.0 * abslat - sec;
	msec = (int) (1000.0 * abslat + 0.5);

	if (msec < 0)
	{
		msec = 0;
	}
	else if (msec > 999)
	{
		msec = 0;
		++ sec;
		if (sec >= 60)
		{
			sec = 0;
			++ min;
			if (min >= 60)
			{
				min = 0;
				++ deg;
			}
		}
	}

    sprintf (txt, "%02d:%02d:%02d.%03d", deg, min, sec, msec);

	if (latitude >= 0.0)
	{
		strcat (txt, "N");
	}
	else
	{
		strcat (txt, "S");
	}

	return txt;
}

/*----------------------------------------------------------------------------*/
/* later           -- Is date+time dt1 later than date+time dt2               */
/*----------------------------------------------------------------------------*/

 t_Bool later (t_Date_Time dt1, t_Date_Time dt2)
{
	t_Real diff;
	t_Bool ret;

	ret = FALSE;

	if (dt1.date_present && dt2.date_present)
	{
		if (dt1.date.year > dt2.date.year)
		{
			ret = TRUE;
		}
		else if (dt1.date.year == dt2.date.year &&
                 dt1.date.month > dt2.date.month)
		{
			ret = TRUE;
		}
		else if (dt1.date.year == dt2.date.year &&
                 dt1.date.month == dt2.date.month &&
                 dt1.date.day > dt2.date.day)
		{
			ret = TRUE;
		}
		else if (dt1.date.year == dt2.date.year &&
                 dt1.date.month == dt2.date.month &&
                 dt1.date.day == dt2.date.day)
		{
			diff = dt1.time - dt2.time;
			if (diff > 0.0)
			{
				ret = TRUE;
			}
		}
	}
	else
	{
		diff = dt1.time - dt2.time;
		if (diff > 0.0)
		{
			ret = TRUE;
		}
	}

	return ret;
}

/*----------------------------------------------------------------------------*/
/* lo_byte         -- Extract low byte                                        */
/*----------------------------------------------------------------------------*/

 t_Byte lo_byte (t_Ui16 u16)
{
	t_Byte b;

	b = (t_Byte) (u16 & 0x00ff);

	return b;
}

/*----------------------------------------------------------------------------*/
/* lon_text        -- Return text for geographical longitude                  */
/*----------------------------------------------------------------------------*/

 char * lon_text (t_Real longitude)
{
	t_Real abslon;
	int deg, min, sec, msec;
	static char txt[31];

    memset (txt, 0, 31);

	abslon = longitude;
	if (longitude < 0.0)
	{
		abslon = -longitude;
	}

	deg = (int) abslon;
	abslon -= (t_Real) deg;
	min = (int) (60.0 * abslon);
	abslon = 60.0 * abslon - min;
	sec = (int) (60.0 * abslon);
	abslon = 60.0 * abslon - sec;
	msec = (int) (1000.0 * abslon + 0.5);

	if (msec < 0)
	{
		msec = 0;
	}
	else if (msec > 999)
	{
		msec = 0;
		++ sec;
		if (sec >= 60)
		{
			sec = 0;
			++ min;
			if (min >= 60)
			{
				min = 0;
				++ deg;
			}
		}
	}

    sprintf (txt, "%03d:%02d:%02d.%03d", deg, min, sec, msec);

	if (longitude >= 0.0)
	{
		strcat (txt, "E");
	}
	else
	{
		strcat (txt, "W");
	}

	return txt;
}

/*----------------------------------------------------------------------------*/
/* make_si16       -- Make a signed 16 bits integer                           */
/*----------------------------------------------------------------------------*/

 t_Si16 make_si16 (t_Byte high_byte, t_Byte low_byte)
{
	t_Si16 s;
	t_Ui16 u;

	u = (t_Ui16) high_byte;
	u <<= 8;
	u |= (t_Ui16) low_byte;

	s = (t_Si16) u;

	return s;
}

/*----------------------------------------------------------------------------*/
/* make_ui16       -- Make a unsigned 16 bits integer                         */
/*----------------------------------------------------------------------------*/

 t_Ui16 make_ui16 (t_Byte high_byte, t_Byte low_byte)
{
	t_Ui16 u;

	u = (t_Ui16) high_byte;
	u <<= 8;
	u |= (t_Ui16) low_byte;

	return u;
}

/*----------------------------------------------------------------------------*/
/* make_si32       -- Make a signed 32 bits integer                           */
/*----------------------------------------------------------------------------*/

 t_Si32 make_si32 (t_Byte high_byte, t_Byte b2, t_Byte b3, t_Byte b4)
{
	t_Si32 u;

	u = (t_Si32) high_byte;
	u <<= 8;
	u |= (t_Ui32) b2;
	u <<= 8;
	u |= (t_Ui32) b3;
	u <<= 8;
	u |= (t_Ui32) b4;

	return u;
}

/*----------------------------------------------------------------------------*/
/* make_ui32       -- Make a unsigned 32 bits integer                         */
/*----------------------------------------------------------------------------*/

 t_Ui32 make_ui32 (t_Byte high_byte, t_Byte b2, t_Byte b3, t_Byte b4)
{
	t_Ui32 u;

	u = (t_Ui32) high_byte;
	u <<= 8;
	u |= (t_Ui32) b2;
	u <<= 8;
	u |= (t_Ui32) b3;
	u <<= 8;
	u |= (t_Ui32) b4;

	return u;
}

/*----------------------------------------------------------------------------*/
/* mc_to_alt       -- Convert SSR mode C code to altitude                     */
/*----------------------------------------------------------------------------*/

 t_Si16 mc_to_alt (t_Ui16 mc)
{
	t_Ui16 a, b, c, d;
	t_Si16 alt;
	t_Si16 v, x, y, z;
 
                   /* Preset the return value: */
	alt = M_UTL_NVA;
 
                   /* Highest nibble must be zero: */
	if (mc & 0xf000)
	{
		goto done;
	}
 
                   /* Check D1 bit (should be zero): */
	if (mc & 0x0001)
	{
		goto done;
	}
 
                   /* Extract octal digits: */
	a = ((mc & 0x0e00) >> 9);
	b = ((mc & 0x01c0) >> 6);
	c = ((mc & 0x0038) >> 3);
	d = (mc & 0x0007);
 
                   /* Compute factors: */
	x = gg_table[d];
	if (x & 0x0001)
	{
		y = gz_table[a];
	}
	else
	{
		y = gg_table[a];
	}
	if (y & 0x0001)
	{
		z = gz_table[b];
	}
	else
	{
		z = gg_table[b];
	}
	if (z & 0x0001)
	{
		v = pz_table[c];
	}
	else
	{
		v = pg_table[c];
	}
 
                   /* Check if an allowable SSR mode C code: */
	if (v == M_UTL_NVA)
	{
		goto done;
	}
 
                   /* Compute height (in 100 feet): */
	alt = x * 320 + y * 40 + z * 5 + v - 12;
 
	done:
	return alt;
}

/*----------------------------------------------------------------------------*/
/* normalize       -- Normalize date                                          */
/*----------------------------------------------------------------------------*/

 t_Retc normalize (t_Date *date_ptr)
{
	int day;       /* Day (within month) */
	int ml;        /* Month length (in days) */
	int month;     /* Month */
	t_Retc ret;    /* Return code */
	int year;      /* Year */

                   /* Preset the return code: */
	ret = RC_FAIL;

                   /* Check parameters: */
	Assert (date_ptr != NULL, "Invalid parameter");

                   /* Check year: */
	year = date_ptr->year;
	if (year < M_YEAR_MIN || year > M_YEAR_MAX)
	{
		goto done;
	}

                   /* Check month: */
	month = date_ptr->month;
	if (month < 1 || month > 12)
	{
		goto done;
	}

                   /* Get day (may be out of bounds): */
	day = date_ptr->day;

                   /* Set length of current month: */
	ml = days_per_month (year, month);

                   /* Take care of "overshoot" in day: */
	while (day > ml)
	{
		day -= ml;
		++ month;

		if (month > 12)
		{
			++ year;
			if (year > M_YEAR_MAX)
			{
				goto done;
			}
			month = 1;
		}

		ml = days_per_month (year, month);
	}

                   /* Take care of "negative" day: */
	while (day <= 0)
	{
		-- month;

		if (month < 1)
		{
			-- year;
			if (year < M_YEAR_MIN)
			{
				goto done;
			}
			month = 12;
		}

		ml = days_per_month (year, month);
		day += ml;
	}

                   /* Store normalized date: */
	date_ptr->year = year;
	date_ptr->month = month;
	date_ptr->day = day;

                   /* Set the return code: */
	ret = RC_OKAY;

	done:          /* We are done */
	return ret;
}

/*----------------------------------------------------------------------------*/
/* ord             -- Return ordinal number for Boolean value                 */
/*----------------------------------------------------------------------------*/

 int ord (t_Bool bval)
{
	if (bval)
	{
		return 1;
	}
	else
	{
		return 0;
	}
}

/*----------------------------------------------------------------------------*/
/* rounds          -- Rounds to the nearest integer                           */
/*----------------------------------------------------------------------------*/

 int rounds (t_Real f)
{
	int i;

	if (f < INT_MIN)
	{
		i = INT_MIN;
	}
	else if (f < 0.0)
	{
		i = (int) (f - 0.5);
	}
	else if (f > INT_MAX)
	{
		i = INT_MAX;
	}
	else
	{
		i = (int) (f + 0.5);
	}

	return i;
}

/*----------------------------------------------------------------------------*/
/* same_day        -- Decide whether from same day                            */
/*----------------------------------------------------------------------------*/

 t_Bool same_day (t_Date dt1, t_Date dt2)
{
	t_Bool ret;

	ret = FALSE;

	if (dt1.year == dt2.year &&
        dt1.month == dt2.month &&
        dt1.day == dt2.day)
	{
			ret = TRUE;
	}

	return ret;
}

/*----------------------------------------------------------------------------*/
/* stat_fits       -- Check if value fits the statistical item                */
/*----------------------------------------------------------------------------*/

 t_Bool stat_fits (t_Statistical_Item *item_ptr, t_Real value)
{
	t_Real diff;
	t_Real max_value;
	t_Real min_value;
	t_Bool ret;    /* Return value */

                   /* Preset the return value: */
	ret = TRUE;

                   /* Check parameters: */
	Assert (item_ptr != NULL, "Invalid parameter");

                   /* The statistical item must be initiated: */
	Assert (item_ptr->is_initiated, "Statistical item not initiated");

                   /* Check if this statistical item is loaded: */
	if (!item_ptr->is_loaded)
	{
		goto done;
	}

                   /* Check if we have some average value: */
	if (!item_ptr->avg_value_present)
	{
		goto done;
	}

                   /* Check if we have some standard deviation: */
	if (!item_ptr->std_value_present)
	{
		goto done;
	}

                   /* Compute the allowable range: */

	diff = 3.5 * item_ptr->std_value;
	if (diff < 0.10 * fabs (item_ptr->avg_value))
	{
		diff = 0.10 * fabs (item_ptr->avg_value);
	}

	max_value = item_ptr->avg_value + diff;
	min_value = item_ptr->avg_value - diff;

                   /* Check given value against allowable range: */
	if (value < min_value || value > max_value)
	{
		ret = FALSE;
	}

	done:          /* We are done */
	return ret;
}

/*----------------------------------------------------------------------------*/
/* stat_init       -- Initiate a statistical item                             */
/*----------------------------------------------------------------------------*/

 void stat_init (t_Statistical_Item *item_ptr)
{
                   /* Check parameters: */
	Assert (item_ptr != NULL, "Invalid parameter");

                   /* Preset this statistical item: */
	item_ptr->avg_value = 0.0;
	item_ptr->avg_value_present = FALSE;
	item_ptr->count = 0;
	item_ptr->is_initiated = TRUE;
	item_ptr->is_loaded = FALSE;
	item_ptr->max_value = 0.0;
	item_ptr->min_value = 0.0;
	item_ptr->rms_value = 0.0;
	item_ptr->rms_value_present = FALSE;
	item_ptr->std_value = 0.0;
	item_ptr->std_value_present = FALSE;
    item_ptr->sum = 0.0;
	item_ptr->sum_squared = 0.0;

	return;
}

/*----------------------------------------------------------------------------*/
/* stat_updt       -- Update a statistical item                               */
/*----------------------------------------------------------------------------*/

 void stat_updt (t_Statistical_Item *item_ptr, t_Real value)
{
                   /* Check parameters: */
	Assert (item_ptr != NULL, "Invalid parameter");

                   /* The statistical item must be initiated: */
	Assert (item_ptr->is_initiated, "Statistical item not initiated");

                   /* Update this statistical item: */
	if (!item_ptr->is_loaded)
	{
		Assert (item_ptr->count == 0, 
                "Statistical item already loaded");

		item_ptr->avg_value = value;
		item_ptr->avg_value_present = TRUE;
		item_ptr->count = 1;
		item_ptr->is_loaded = TRUE;
		item_ptr->max_value = value;
		item_ptr->min_value = value;
		item_ptr->rms_value = 0.0;
		item_ptr->rms_value_present = FALSE;
		item_ptr->std_value = 0.0;
		item_ptr->std_value_present = FALSE;
		item_ptr->sum = value;
		item_ptr->sum_squared = value * value;
	}
	else
	{
		++ item_ptr->count;
		Assert (item_ptr->count != 0, "Range overrun");

		if (value < item_ptr->min_value)
		{
			item_ptr->min_value = value;
		}
		if (value > item_ptr->max_value)
		{
			item_ptr->max_value = value;
		}

    	item_ptr->sum += value;
		item_ptr->sum_squared += value * value;

		if (item_ptr->count >= 2)
		{
			item_ptr->avg_value = item_ptr->sum / (t_Real) item_ptr->count;
			item_ptr->avg_value_present = TRUE;
		}

		if (item_ptr->count >= 2)
		{
			t_Real f;
			t_Real n;

			n = item_ptr->count;

			f = item_ptr->sum_squared / n;

			Assert (f >= 0.0, "Domain error");

			item_ptr->rms_value = sqrt (f);
			item_ptr->rms_value_present = TRUE;
		}

		if (item_ptr->count >= 2)
		{
			t_Real f;
			t_Real n;

			n = item_ptr->count;

			Assert (item_ptr->avg_value_present, "No average value");
			f = item_ptr->sum_squared - 
                n * item_ptr->avg_value * item_ptr->avg_value;

			if (f >= 0.0)
			{
				item_ptr->std_value = sqrt (f / (n - 1.0));
				item_ptr->std_value_present = TRUE;
			}
		}
	}

	return;
}

/*----------------------------------------------------------------------------*/
/* time_diff       -- Compute difference in time                              */
/*----------------------------------------------------------------------------*/

 t_Secs time_diff (t_Secs tm_1, t_Secs tm_2)
{
	t_Secs dt;     /* Difference in time; seconds */
	t_Secs tm_diff;
                   /* Difference in time; seconds */

	tm_diff = tm_1 - tm_2;
	if (tm_diff < -M_SECONDS_PER_DAY / 2.0)
	{
		tm_diff += M_SECONDS_PER_DAY;
	}
	else if (tm_diff > +M_SECONDS_PER_DAY / 2.0)
	{
		tm_diff -= M_SECONDS_PER_DAY;
	}

	dt = tm_diff;

	return dt;
}

/*----------------------------------------------------------------------------*/
/* ungarble        -- Ungarble an SSR mode 2 or 3/A code                      */
/*----------------------------------------------------------------------------*/

 t_Ui16 ungarble (t_Ui16 bits)
{
	t_Ui16 a1, a2, a4, b1, b2, b4, c1, c2, c4, d1, d2, d4;
	t_Ui16 u;
 
	c1 = (bits >> 12) & 0x0001;
	a1 = (bits >> 11) & 0x0001;
	c2 = (bits >> 10) & 0x0001;
	a2 = (bits >>  9) & 0x0001;
	c4 = (bits >>  8) & 0x0001;
	a4 = (bits >>  7) & 0x0001;
	b1 = (bits >>  5) & 0x0001;
	d1 = (bits >>  4) & 0x0001;
	b2 = (bits >>  3) & 0x0001;
	d2 = (bits >>  2) & 0x0001;
	b4 = (bits >>  1) & 0x0001;
	d4 = bits & 0x0001;
 
	u = (a4 << 11) | (a2 << 10) | (a1 << 9) |
        (b4 << 8) | (b2 << 7) | (b1 << 6) |
        (c4 << 5) | (c2 << 4) | (c1 << 3) |
        (d4 << 2) | (d2 << 1) | d1;
 
	return u;
}

/*----------------------------------------------------------------------------*/
/* utc_secs        -- Return text for UTC time in seconds                     */
/*----------------------------------------------------------------------------*/

 char * utc_secs (t_Secs utc_time)
{
	int h, m, ms, s;
	t_Ui32 t;
	static char txt[21];

    memset (txt, 0, 21);

	t = (t_Ui32) (1000.0 * utc_time + 0.5);
	if (t > M_MILLISECONDS_PER_DAY)
	{
		t = t % M_MILLISECONDS_PER_DAY;
	}

	ms = (int) (t % 1000);
	t = t / 1000;
	s = (int) (t % 60);
	t = t / 60;
	m = (int) (t % 60);
	t = t / 60;
	h = (int) (t % 24);

	if (ms >= 500)
	{
		++ s;

		if (s >= 60)
		{
			++ m;

			if (m >= 60)
			{
				++ h;

				if (h >= 24)
				{
					h = 0;
				}

				m = 0;
			}

			s = 0;
		}
	}

    sprintf (txt, "%02d:%02d:%02d", h, m, s);

	return txt;
}

/*----------------------------------------------------------------------------*/
/* utc_text        -- Return text for UTC time in seconds                     */
/*----------------------------------------------------------------------------*/

 char * utc_text (t_Secs utc_time)
{
	int h, m, ms, s;
	t_Ui32 t;
	static char txt[21];

    memset (txt, 0, 21);

	t = (t_Ui32) (1000.0 * utc_time + 0.5);
	if (t > M_MILLISECONDS_PER_DAY)
	{
		t = t % M_MILLISECONDS_PER_DAY;
	}

	ms = (int) (t % 1000);
	t = t / 1000;
	s = (int) (t % 60);
	t = t / 60;
	m = (int) (t % 60);
	t = t / 60;
	h = (int) (t % 24);

    sprintf (txt, "%02d:%02d:%02d.%03d", h, m, s, ms);

	return txt;
}
/* end-of-file */
