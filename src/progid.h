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
/* File:           src/progid.h                                               */
/* Contents:       Program identity and version definitions                   */
/* Author(s):      kb                                                         */
/* Last change:    2018-09-30                                                 */
/*----------------------------------------------------------------------------*/

#ifndef INCLUDED_PROGID_H
#define INCLUDED_PROGID_H

#include "basics.h"

                   /* Basic macros: */
                   /* ------------- */

#define M_PRGNAM "sddl"
#define M_PROMPT "Surveillance Data Decoder and Lister"

#define M_VERSNO "Version V0.42-001c"
#define M_COPYRT "2000-2018 by Helmut Kobelbauer, Sinabelkirchen/Austria."
#define M_DISCL1 "SDDL is free software: you can redistribute it and/or modify it under\n" \
                 " the terms of the GNU General Public License as published by the Free Software\n" \
                 " Foundation, either version 3 of the License, or (at your option) any later version."

#define M_DISCL2 "SDDL is distributed in the hope that it will be useful, but WITHOUT\n"\
                 " ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or\n" \
                 " FITNESS FOR A PARTICULAR PURPOSE. See the GNU General Public License for more details"

#define M_DISCL3 "Users are invited to report issues or requests for improvement\n"\
                 " however the provider has no obligation to process users\'"\
                 " requests and\n provide fixes and/or enhancements."
#define M_E_MAIL "sddl@gmx.at"

#endif /* INCLUDED_PROGID_H */
