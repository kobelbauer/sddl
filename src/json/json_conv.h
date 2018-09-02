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

#ifndef JSON_CONV_H
#define JSON_CONV_H

#include "common_data_types.h"

#include "json.hpp"

extern nlohmann::json convertToJSON (t_Tres& tres);
extern nlohmann::json convertToJSON (t_Rsrv_Type& rsrv);
extern nlohmann::json convertToJSON (t_Target_Address_Enum_Type& ta_type);

#endif // JSON_CONV_H
