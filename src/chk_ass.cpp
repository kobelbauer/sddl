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
/* File:           src/chk_ass.cpp                                            */
/* Contents:       Check some assertion                                       */
/* Author(s):      kb                                                         */
/* Last change:    2018-04-27                                                 */
/*----------------------------------------------------------------------------*/

#include "basics.h"
                   /* Basic system definitions */
#include "common.h"
                   /* Common declarations and definitions */

#include <stddef.h>
#include <stdio.h>
#if CHKASS
#include <setjmp.h>
#endif

#if CHKASS

/*----------------------------------------------------------------------------*/
/* chk_assertion   -- Check some assertion                                    */
/*----------------------------------------------------------------------------*/

 void chk_assertion (t_Bool condition, const char *text_ptr,
                     const char *file_name, int line_number)
{
	if (condition)
	{
		return;
	}

	if (text_ptr == NULL || *text_ptr == '\0')
	{
		text_ptr = "Unspecified reason";
	}

	printf ("E> Assertion violated:\n");
	printf ("   In file '%s' near line %d: %s.\n",
			file_name, line_number, text_ptr);

	longjmp (on_fatal_error, 10001);
}

#endif /* CHKASS */
/* end-of-file */
