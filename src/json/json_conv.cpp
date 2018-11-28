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
#include "common.h"

void convertToJSON (t_Tres& tres, const std::string& name, nlohmann::json& parent)
{
    switch (tres)
    {
    case e_is_undefined:
        if (write_json_nulls)
            parent[name] = nullptr;
        break;
    case e_is_false:
        parent[name] = false;
        break;
    case e_is_true:
        parent[name] = true;
        break;
    default:
        if (write_json_nulls)
            parent[name] = nullptr;
        break;
    }

}

void convertToJSON (t_Rsrv_Type& rsrv, const std::string& name, nlohmann::json& parent)
{
    switch (rsrv)
    {
    case e_rsrv_undefined:
        parent[name] = "undefined";
        break;
    case e_rsrv_north_marker:
        parent[name] = "nort marker";
        break;
    case e_rsrv_south_marker:
        parent[name] = "south marker";
        break;
    case e_rsrv_sector_crossing:
        parent[name] = "sector crossing";
        break;
    case e_rsrv_supervisory:
        parent[name] = "supervisory";
        break;
    case e_rsrv_filtering_zone:
        parent[name] = "filtering zone";
        break;
    case e_rsrv_jamming_strobe:
        parent[name] = "jamming strobe";
        break;
    default:
        if (write_json_nulls)
            parent[name] = nullptr;
        break;
    }

}

void convertToJSON (t_Target_Address_Enum_Type& ta_type, const std::string& name, nlohmann::json& parent)
{
    switch (ta_type)
    {
    case e_target_address_undefined:
        parent[name] = "undefined";
        break;
    case e_target_address_non_unique:
        parent[name] = "non unique address";
        break;
    case e_target_address_icao_24_bit:
        parent[name] = "24-bit ICAO address";
        break;
    case e_target_address_surface_vehicle:
        parent[name] = "surface vehicle address";
        break;
    case e_target_address_anonymous:
        parent[name] = "anonymous";
        break;
    default:
        if (write_json_nulls)
            parent[name] = nullptr;
        break;
    }
}


