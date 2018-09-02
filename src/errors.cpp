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
/* File:           errors.cpp                                                 */
/* Contents:       Error handling                                             */
/* Author(s):      kb                                                         */
/* Last change:    2018-04-27                                                 */
/*----------------------------------------------------------------------------*/

#include "basics.h"
                   /* Basic system definitions */
#include "common.h"
                   /* Common declarations and definitions */

#include <stdarg.h>
#include <stddef.h>
#include <stdio.h>
#include <string.h>

/*----------------------------------------------------------------------------*/
/* error_msg       -- Error message                                           */
/*----------------------------------------------------------------------------*/

 void error_msg (const char *format, ...)
{
	char msg[1024];
	va_list ptr;

                   /* Check parameter(s): */
	if (format == NULL)
	{
		goto done;
	}
	if (*format == '\0')
	{
		goto done;
	}

	memset (msg, 0, 1024);
	va_start (ptr, format);
	vsprintf (msg, format, ptr);
	va_end (ptr);

				   /* Check message length: */
	Assert (strlen (msg) <= 1023, "Local memory overwritten");

	fprintf (stderr, "E> Near offset " M_FMT_UI32, input_offset);
	if (input_rtm != 0)
	{
		fprintf (stderr, " (rtm=" M_FMT_UI32 ")", input_rtm);
	}
	fprintf (stderr, ": %s\n", msg);

#if LISTER
	list_text (-1, "; Error near offset " M_FMT_UI32, input_offset);
	if (input_rtm != 0)
	{
		list_text (-1, " (rtm=" M_FMT_UI32 ")", input_rtm);
	}
	list_text (-1, ": %s\n", msg);
#endif /* LISTER */

	done:          /* We are done */
	return;
}
/* end-of-file */
