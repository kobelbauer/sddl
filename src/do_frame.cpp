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
/* File:           src/do_frame.cpp                                           */
/* Contents:       Process a data frame                                       */
/* Author(s):      kb                                                         */
/* Last change:    2018-04-27                                                 */
/*----------------------------------------------------------------------------*/

#include "basics.h"
                   /* Basic system definitions */
#include "common.h"
                   /* Common declarations and definitions */

#include <stddef.h>
#include <stdio.h>
#include <string.h>

/*----------------------------------------------------------------------------*/
/* do_frame        -- Process a data frame                                    */
/*----------------------------------------------------------------------------*/

 t_Retc do_frame (t_Ui32 offset, t_Ui16 source, t_Ui16 length, t_Byte *buffer)
{
	t_Retc lrc;    /* Local return code */
	t_Retc ret;    /* Return code */

				   /* Preset the return code: */
	ret = RC_FAIL;

				   /* Check parameters: */
	Assert (length > 0, "Invalid parameter");
	Assert (buffer != NULL, "Invalid parameter");

				   /* Check if some data format defined: */
	Assert (data_format != e_data_format_undefined, "No data format defined");

                   /* Preset number of records in current frame: */
	records_in_current_frame = 0;

				   /* Check for ASTERIX data: */
	if (data_format == e_data_format_asterix)
	{
		lrc = asterix_frame (offset, source, length, buffer);
	}

                   /* Check for unknown data format: */
	else if (data_format == e_data_format_zzz)
	{
		lrc = RC_SKIP;
	}

	else
	{
		Assert (FALSE, "Unknown data format");

		lrc = RC_FAIL;
	}

	if (lrc != RC_OKAY && lrc != RC_SKIP)
	{
		if (stop_on_error)
		{
			ret = lrc;
			goto done;
		}
	}

				   /* Set the return code: */
	ret = lrc;
 
	done:          /* We are done: */
	return ret;
}
/* end-of-file */
