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

#include "jsonwriter.h"
#include "common.h"

#include <vector>

#include <archive.h>
#include <archive_entry.h>

const unsigned int DATA_WRITE_SIZE = 1000;

JSONWriter::JSONWriter(JSON_OUTPUT_TYPE json_output_type, const std::string& json_path)
    : json_output_type_{json_output_type}, json_path_ {json_path}
{
    switch (json_output_type_)
    {
    case JSON_NONE:
    case JSON_TEST:
    case JSON_PRINT:
        break;
    case JSON_TEXT:
    case JSON_CBOR:
    case JSON_MESSAGE_PACK:
    case JSON_UBJSON:
        openJsonFile();
        break;
    case JSON_ZIP_TEXT:
    case JSON_ZIP_CBOR:
    case JSON_ZIP_MESSAGE_PACK:
    case JSON_ZIP_UBJSON:
        openJsonZipFile();
        break;
    default:
        printf ("-> Unhandled JSON output type '%d' during construction\n", json_output_type_);
        break;
    }
}

JSONWriter::~JSONWriter ()
{
    if (data_.size())
        writeData();

    if (json_file_open_)
        closeJsonFile ();

    if (json_zip_file_open_)
        closeJsonZipFile();
}

void JSONWriter::write(t_Adsb *adsb_ptr)
{
    assert (adsb_ptr);

    switch (json_output_type_)
    {
    case JSON_TEST:
        break;
    case JSON_PRINT:
    {
        nlohmann::json j;
        adsb_ptr->toJSON(j);
        j["rec_num"] = rec_num_cnt_++;
        std::cout << "adsb json: " << j.dump(4) << std::endl;
    }
        break;
    case JSON_TEXT:
    case JSON_CBOR:
    case JSON_MESSAGE_PACK:
    case JSON_UBJSON:
//        writeJsonToFile (j);
//        break;
    case JSON_ZIP_TEXT:
    case JSON_ZIP_CBOR:
    case JSON_ZIP_MESSAGE_PACK:
    case JSON_ZIP_UBJSON:
    {
        t_Adsb* adsb_copy = new t_Adsb();
        *adsb_copy = *adsb_ptr;
        data_.push_back(dynamic_cast<JSONConvertible*>(adsb_copy));
        //writeJsonToZipFile (j);
    }
        break;
    default:
        printf ("-> Unhandled JSON output type '%d' write\n", json_output_type_);
        break;
    }

    if (data_.size() > DATA_WRITE_SIZE)
        writeData();
}

void JSONWriter::write(t_Mlat *mlat_ptr)
{
    assert (mlat_ptr);

    switch (json_output_type_)
    {
    case JSON_TEST:
        break;
    case JSON_PRINT:
    {
        nlohmann::json j;
        mlat_ptr->toJSON(j);
        j["rec_num"] = rec_num_cnt_++;
        std::cout << "mlat json: " << j.dump(4) << std::endl;
    }
        break;
    case JSON_TEXT:
    case JSON_CBOR:
    case JSON_MESSAGE_PACK:
    case JSON_UBJSON:
//        writeJsonToFile (j);
//        break;
    case JSON_ZIP_TEXT:
    case JSON_ZIP_CBOR:
    case JSON_ZIP_MESSAGE_PACK:
    case JSON_ZIP_UBJSON:
    {
        //writeJsonToZipFile (j);
        t_Mlat* mlat_copy = new t_Mlat();
        *mlat_copy = *mlat_ptr;
        data_.push_back(dynamic_cast<JSONConvertible*>(mlat_copy));
    }
        break;
    default:
        printf ("-> Unhandled JSON output type '%d' write\n", json_output_type_);
        break;
    }

    if (data_.size() > DATA_WRITE_SIZE)
        writeData();
}

void JSONWriter::write(t_Rsrv *rsrv_ptr)
{
    assert (rsrv_ptr);

    switch (json_output_type_)
    {
    case JSON_TEST:
        break;
    case JSON_PRINT:
    {
        nlohmann::json j;
        rsrv_ptr->toJSON(j);
        j["rec_num"] = rec_num_cnt_++;
        std::cout << "rsvr json: " << j.dump(4) << std::endl;
    }
        break;
    case JSON_TEXT:
    case JSON_CBOR:
    case JSON_MESSAGE_PACK:
    case JSON_UBJSON:
//        writeJsonToFile (j);
//        break;
    case JSON_ZIP_TEXT:
    case JSON_ZIP_CBOR:
    case JSON_ZIP_MESSAGE_PACK:
    case JSON_ZIP_UBJSON:
    {
        t_Rsrv* rsrv_copy = new t_Rsrv();
        *rsrv_copy = *rsrv_ptr;
        data_.push_back(dynamic_cast<JSONConvertible*>(rsrv_copy));
        //writeJsonToZipFile (j);
    }
        break;
    default:
        printf ("-> Unhandled JSON output type '%d' write\n", json_output_type_);
        break;
    }

    if (data_.size() > DATA_WRITE_SIZE)
        writeData();
}

void JSONWriter::write(t_Rtgt *rtgt_ptr)
{
    assert (rtgt_ptr);

    switch (json_output_type_)
    {
    case JSON_TEST:
        break;
    case JSON_PRINT:
    {
        nlohmann::json j;
        rtgt_ptr->toJSON(j);
        j["rec_num"] = rec_num_cnt_++;
        std::cout << "rtgt json: " << j.dump(4) << std::endl;
    }
        break;
    case JSON_TEXT:
    case JSON_CBOR:
    case JSON_MESSAGE_PACK:
    case JSON_UBJSON:
//        writeJsonToFile (j);
//        break;
    case JSON_ZIP_TEXT:
    case JSON_ZIP_CBOR:
    case JSON_ZIP_MESSAGE_PACK:
    case JSON_ZIP_UBJSON:
    {
        t_Rtgt* rtgt_copy = new t_Rtgt();
        *rtgt_copy = *rtgt_ptr;
        data_.push_back(dynamic_cast<JSONConvertible*>(rtgt_copy));
        //writeJsonToZipFile (j);
    }
        break;
    default:
        printf ("-> Unhandled JSON output type '%d' write\n", json_output_type_);
        break;
    }

    if (data_.size() > DATA_WRITE_SIZE)
        writeData();
}

void JSONWriter::write(t_Strk *strk_ptr)
{
    assert (strk_ptr);

    switch (json_output_type_)
    {
    case JSON_TEST:
        break;
    case JSON_PRINT:
    {
        nlohmann::json j;
        strk_ptr->toJSON(j);
        j["rec_num"] = rec_num_cnt_++;
        std::cout << "track json: " << j.dump(4) << std::endl;
    }
        break;
    case JSON_TEXT:
    case JSON_CBOR:
    case JSON_MESSAGE_PACK:
    case JSON_UBJSON:
//        writeJsonToFile (j);
//        break;
    case JSON_ZIP_TEXT:
    case JSON_ZIP_CBOR:
    case JSON_ZIP_MESSAGE_PACK:
    case JSON_ZIP_UBJSON:
    {
        t_Strk* strk_copy = new t_Strk();
        *strk_copy = *strk_ptr;
        data_.push_back(dynamic_cast<JSONConvertible*>(strk_copy));
        //writeJsonToZipFile (j);
    }
        break;
    default:
        printf ("-> Unhandled JSON output type '%d' write\n", json_output_type_);
        break;
    }

    if (data_.size() > DATA_WRITE_SIZE)
        writeData();
}

void JSONWriter::writeData()
{
    assert (data_.size());

    for (JSONConvertible* data_ptr : data_)
    {
        nlohmann::json j;
        if (dynamic_cast<t_Adsb*>(data_ptr))
            dynamic_cast<t_Adsb*>(data_ptr)->toJSON (j);
        else if (dynamic_cast<t_Mlat*>(data_ptr))
            dynamic_cast<t_Mlat*>(data_ptr)->toJSON (j);
        else if (dynamic_cast<t_Rsrv*>(data_ptr))
            dynamic_cast<t_Rsrv*>(data_ptr)->toJSON (j);
        else if (dynamic_cast<t_Rtgt*>(data_ptr))
            dynamic_cast<t_Rtgt*>(data_ptr)->toJSON (j);
        else if (dynamic_cast<t_Strk*>(data_ptr))
            dynamic_cast<t_Strk*>(data_ptr)->toJSON (j);
        else
            assert (false);

        switch (json_output_type_)
        {
        case JSON_TEXT:
        case JSON_CBOR:
        case JSON_MESSAGE_PACK:
        case JSON_UBJSON:
            writeJsonToFile (j);
            break;
        case JSON_ZIP_TEXT:
        case JSON_ZIP_CBOR:
        case JSON_ZIP_MESSAGE_PACK:
        case JSON_ZIP_UBJSON:
            writeJsonToZipFile (j);
            break;
        case JSON_TEST:
        case JSON_PRINT:
        default:
            printf ("-> Unhandled JSON output type '%d' writeData\n", json_output_type_);
            break;
        }

        delete data_ptr;
    }

    data_.clear();
}

void JSONWriter::openJsonFile ()
{
    Assert (json_file_open_ == FALSE, "JSON export file already open");

    switch (json_output_type_)
    {
    case JSON_TEXT:
        json_file_.open (json_path);
        break;
    case JSON_CBOR:
    case JSON_MESSAGE_PACK:
    case JSON_UBJSON:
        json_file_.open (json_path, ios::out | ios::binary);
        break;
    default:
        printf ("-> Unhandled JSON output type '%d' during open\n", json_output_type_);
        break;
    }

    json_file_open_ = TRUE;
}

void JSONWriter::writeJsonToFile (nlohmann::json& j)
{
    Assert (json_file_open_ == TRUE, "JSON export file not open");

    switch (json_output_type_)
    {
    case JSON_TEXT:
        json_file_ << j.dump(4) << std::endl;
        break;
    case JSON_CBOR:
    {
        std::vector<std::uint8_t> v_cbor = nlohmann::json::to_cbor(j);
        json_file_.write (reinterpret_cast<const char*>(v_cbor.data()), v_cbor.size());
    }
        break;
    case JSON_MESSAGE_PACK:
    {
        std::vector<std::uint8_t> v_msgpack = nlohmann::json::to_msgpack(j);
        json_file_.write (reinterpret_cast<const char*>(v_msgpack.data()), v_msgpack.size());
    }
        break;
    case JSON_UBJSON:
    {
        std::vector<std::uint8_t> v_ubjson = nlohmann::json::to_ubjson(j);
        json_file_.write (reinterpret_cast<const char*>(v_ubjson.data()), v_ubjson.size());
    }
        break;
    default:
        printf ("-> Unhandled JSON output type '%d' during write\n", json_output_type_);
        break;
    }
}

void JSONWriter::closeJsonFile ()
{
    json_file_.close();
    json_file_open_ = FALSE;
}

void JSONWriter::openJsonZipFile ()
{
    Assert (json_zip_file_open_ == FALSE, "JSON export ZIP file already open");

    json_zip_file_ = archive_write_new();
    archive_write_set_format_zip(json_zip_file_);
    archive_write_open_filename(json_zip_file_, json_path_.c_str());

    std::string filename{"text.json"};

    switch (json_output_type_)
    {
    case JSON_ZIP_TEXT:
        break;
    case JSON_ZIP_CBOR:
        filename = "text.json.cbor";
        break;
    case JSON_ZIP_MESSAGE_PACK:
        filename = "text.json.msgpack";
        break;
    case JSON_ZIP_UBJSON:
        filename = "text.json.ubjson";
        break;
    default:
        printf ("-> Unhandled JSON output type '%d' during open\n", json_output_type_);
        break;
    }

    json_zip_file_entry_ = archive_entry_new();
    archive_entry_set_pathname(json_zip_file_entry_, filename.c_str());
    archive_entry_set_filetype(json_zip_file_entry_, AE_IFREG);
    archive_entry_set_perm(json_zip_file_entry_, 0644);
    archive_write_header(json_zip_file_, json_zip_file_entry_);

    json_zip_file_open_ = TRUE;
}
void JSONWriter::writeJsonToZipFile (nlohmann::json& j)
{
    Assert (json_zip_file_open_ == TRUE, "JSON export file not open");

    switch (json_output_type_)
    {
    case JSON_ZIP_TEXT:
    {
        std::string tmp {j.dump(4)};
        tmp += "\n";
        archive_write_data (json_zip_file_, tmp.c_str(), tmp.size());
    }
        break;
    case JSON_ZIP_CBOR:
    {
        std::vector<std::uint8_t> v_cbor = nlohmann::json::to_cbor(j);
        //cout << "cbor-zip write " << v_cbor.size() << std::endl;
        archive_write_data (json_zip_file_,reinterpret_cast<const void*>(v_cbor.data()), v_cbor.size());
        //            if (bytes_written != v_cbor.size())
        //            {
        //                printf("Error writing output archive %d\n", bytes_written);
        //                assert (false);
        //            }
    }
        break;
    case JSON_ZIP_MESSAGE_PACK:
    {
        std::vector<std::uint8_t> v_msgpack = nlohmann::json::to_msgpack(j);
        //cout << "msgpack-zip write " << v_msgpack.size() << std::endl;
        archive_write_data (json_zip_file_,reinterpret_cast<const void*>(v_msgpack.data()), v_msgpack.size());
    }
        break;
    case JSON_ZIP_UBJSON:
    {
        std::vector<std::uint8_t> v_ubjson = nlohmann::json::to_ubjson(j);
        //cout << "ubjson-zip write " << v_ubjson.size() << std::endl;
        archive_write_data (json_zip_file_,reinterpret_cast<const void*>(v_ubjson.data()), v_ubjson.size());
    }
        break;
    default:
        printf ("-> Unhandled JSON output type '%d' during write\n", json_output_type_);
        break;
    }
    //archive_write_data(json_zip_file_, buff, len);
}
void JSONWriter::closeJsonZipFile ()
{
    archive_entry_free(json_zip_file_entry_);
    archive_write_close(json_zip_file_); // Note 4
    archive_write_free(json_zip_file_); // Note 5

    json_zip_file_open_ = FALSE;
}
