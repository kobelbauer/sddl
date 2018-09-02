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

#include "json_conv.h"

nlohmann::json convertToJSON (t_Tres& tres)
{
    switch (tres)
    {
    case e_is_undefined:
        return nullptr;
        break;
    case e_is_false:
        return false;
        break;
    case e_is_true:
        return true;
        break;
    default:
        return nullptr;
        break;
    }

}

nlohmann::json convertToJSON (t_Rsrv_Type& rsrv)
{
    switch (rsrv)
    {
    case e_rsrv_undefined:
        return "undefined";
        break;
    case e_rsrv_north_marker:
        return "nort marker";
        break;
    case e_rsrv_south_marker:
        return "south marker";
        break;
    case e_rsrv_sector_crossing:
        return "sector crossing";
        break;
    case e_rsrv_supervisory:
        return "supervisory";
        break;
    case e_rsrv_filtering_zone:
        return "filtering zone";
        break;
    case e_rsrv_jamming_strobe:
        return "jamming strobe";
        break;
    default:
        return nullptr;
        break;
    }

}

nlohmann::json convertToJSON (t_Target_Address_Enum_Type& ta_type)
{
    switch (ta_type)
    {
    case e_target_address_undefined:
        return "undefined";
        break;
    case e_target_address_non_unique:
        return "non unique address";
        break;
    case e_target_address_icao_24_bit:
        return "24-bit ICAO address";
        break;
    case e_target_address_surface_vehicle:
        return "surface vehicle address";
        break;
    case e_target_address_anonymous:
        return "anonymous";
        break;
    default:
        return nullptr;
        break;
    }

}


