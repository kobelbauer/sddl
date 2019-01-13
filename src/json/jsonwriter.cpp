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
#include "jsonfilewritetask.h"
#include "common.h"

#include <vector>
#include <tbb/tbb.h>

#include <archive.h>
#include <archive_entry.h>

#include <chrono>
#include <thread>

const unsigned int DATA_WRITE_SIZE = 1000;

using namespace tbb;

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

    while (file_write_in_progress_)
        std::this_thread::sleep_for(std::chrono::milliseconds(5));

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

    // convert to json
    assert (!json_data_.size());
    json_data_.resize(data_.size());

    //size_t cnt = 0;
    size_t size = data_.size();

//    for( size_t i=0; i!=n; ++i )
//            Foo(a[i]);

    tbb::parallel_for( size_t(0), size, [&]( size_t cnt )
    {
        JSONConvertible* data_ptr = data_.at(cnt);

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

        delete data_ptr;

        json_data_[cnt] = std::move (j);
    } );

    for (nlohmann::json& j_it : json_data_)
        j_it["rec_num"] = rec_num_cnt_++;

//    for (JSONConvertible* data_ptr : data_)
//    {
//        nlohmann::json j;
//        if (dynamic_cast<t_Adsb*>(data_ptr))
//            dynamic_cast<t_Adsb*>(data_ptr)->toJSON (j);
//        else if (dynamic_cast<t_Mlat*>(data_ptr))
//            dynamic_cast<t_Mlat*>(data_ptr)->toJSON (j);
//        else if (dynamic_cast<t_Rsrv*>(data_ptr))
//            dynamic_cast<t_Rsrv*>(data_ptr)->toJSON (j);
//        else if (dynamic_cast<t_Rtgt*>(data_ptr))
//            dynamic_cast<t_Rtgt*>(data_ptr)->toJSON (j);
//        else if (dynamic_cast<t_Strk*>(data_ptr))
//            dynamic_cast<t_Strk*>(data_ptr)->toJSON (j);
//        else
//            assert (false);

//        delete data_ptr;

//        json_data_[cnt] = std::move (j);
//        ++cnt;
//    }
    assert (data_.size() == json_data_.size());
    data_.clear();

    // convert to string or binary data

    switch (json_output_type_)
    {
    case JSON_TEXT:
    case JSON_ZIP_TEXT:
        convertJSON2Text();
        break;
    case JSON_CBOR:
    case JSON_ZIP_CBOR:
        convertJSON2CBOR();
        break;
    case JSON_MESSAGE_PACK:
    case JSON_ZIP_MESSAGE_PACK:
        convertJSON2MsgPack();
        break;
    case JSON_UBJSON:
    case JSON_ZIP_UBJSON:
        convertJSON2UBJSON();
        break;
    case JSON_TEST:
    case JSON_PRINT:
    default:
        printf ("-> Unhandled JSON output type '%d' writeData\n", json_output_type_);
        break;
    }

    assert (!json_data_.size());

    switch (json_output_type_)
    {
    case JSON_TEXT:
        writeTextToFile ();
        break;
    case JSON_CBOR:
    case JSON_MESSAGE_PACK:
    case JSON_UBJSON:
        writeBinaryToFile ();
        break;
    case JSON_ZIP_TEXT:
        writeTextToZipFile();
        break;
    case JSON_ZIP_CBOR:
    case JSON_ZIP_MESSAGE_PACK:
    case JSON_ZIP_UBJSON:
        writeBinaryToZipFile ();
        break;
    case JSON_TEST:
    case JSON_PRINT:
    default:
        printf ("-> Unhandled JSON output type '%d' writeData\n", json_output_type_);
        break;
    }

    assert (!text_data_.size());
    assert (!binary_data_.size());
}

void JSONWriter::convertJSON2Text ()
{
    assert (!text_data_.size());
    text_data_.resize(json_data_.size());

    size_t size = json_data_.size();

    tbb::parallel_for( size_t(0), size, [&]( size_t cnt )
    {
        text_data_[cnt] = json_data_.at(cnt).dump(4) + "\n";
    } );

//    size_t cnt = 0;

//    for (nlohmann::json& j_it : json_data_)
//        text_data_[cnt++] = j_it.dump(4) + "\n";

    assert (text_data_.size() == json_data_.size());
    json_data_.clear();
}

void JSONWriter::convertJSON2CBOR ()
{
    assert (!binary_data_.size());
    binary_data_.resize(json_data_.size());

    size_t size = json_data_.size();

    tbb::parallel_for( size_t(0), size, [&]( size_t cnt )
    {
        binary_data_[cnt] = nlohmann::json::to_cbor(json_data_.at(cnt));
    } );

//    size_t cnt = 0;
//    for (nlohmann::json& j_it : json_data_)
//        binary_data_[cnt++] = nlohmann::json::to_cbor(j_it);

    assert (json_data_.size() == binary_data_.size());
    json_data_.clear();
}

void JSONWriter::convertJSON2UBJSON ()
{
    assert (!binary_data_.size());
    binary_data_.resize(json_data_.size());

    size_t size = json_data_.size();

    tbb::parallel_for( size_t(0), size, [&]( size_t cnt )
    {
        binary_data_[cnt] = nlohmann::json::to_ubjson(json_data_.at(cnt));
    } );

//    size_t cnt = 0;
//    for (nlohmann::json& j_it : json_data_)
//        binary_data_[cnt++] = nlohmann::json::to_ubjson(j_it);

    assert (json_data_.size() == binary_data_.size());
    json_data_.clear();
}
void JSONWriter::convertJSON2MsgPack ()
{
    assert (!binary_data_.size());
    binary_data_.resize(json_data_.size());

    size_t size = json_data_.size();

    tbb::parallel_for( size_t(0), size, [&]( size_t cnt )
    {
        binary_data_[cnt] = nlohmann::json::to_msgpack(json_data_.at(cnt));
    } );

//    size_t cnt = 0;
//    for (nlohmann::json& j_it : json_data_)
//        binary_data_[cnt++] = nlohmann::json::to_msgpack(j_it);

    assert (json_data_.size() == binary_data_.size());
    json_data_.clear();
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

void JSONWriter::writeTextToFile ()
{
    Assert (json_file_open_ == TRUE, "JSON export file not open");
    assert (text_data_.size());

//    for (const std::string str_it : text_data_)
//        json_file_ << str_it;

//    text_data_.clear();

    while (file_write_in_progress_)
        std::this_thread::sleep_for(std::chrono::milliseconds(5));

    file_write_in_progress_ = true;

    JSONTextFileWriteTask* write_task = new (tbb::task::allocate_root()) JSONTextFileWriteTask (
                json_file_, std::move(text_data_), *this);
    tbb::task::enqueue(*write_task);

    assert (!text_data_.size());
}

void JSONWriter::writeBinaryToFile ()
{
    Assert (json_file_open_ == TRUE, "JSON export file not open");
    assert (binary_data_.size());

//    for (const std::vector<std::uint8_t>& bin_it : binary_data_)
//        json_file_.write (reinterpret_cast<const char*>(bin_it.data()), bin_it.size());

//    binary_data_.clear();

    while (file_write_in_progress_)
        std::this_thread::sleep_for(std::chrono::milliseconds(5));

    file_write_in_progress_ = true;

    JSONBinaryFileWriteTask* write_task = new (tbb::task::allocate_root()) JSONBinaryFileWriteTask (
                json_file_, std::move(binary_data_), *this);
    tbb::task::enqueue(*write_task);

    assert (!binary_data_.size());
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

void JSONWriter::writeTextToZipFile ()
{
    Assert (json_zip_file_open_ == TRUE, "JSON export file not open");
    assert (text_data_.size());

//    for (const std::string str_it : text_data_)
//        archive_write_data (json_zip_file_, str_it.c_str(), str_it.size());

    while (file_write_in_progress_)
        std::this_thread::sleep_for(std::chrono::milliseconds(5));

    file_write_in_progress_ = true;

    JSONTextZipFileWriteTask* write_task = new (tbb::task::allocate_root()) JSONTextZipFileWriteTask (
                json_zip_file_, std::move(text_data_), *this);
    tbb::task::enqueue(*write_task);

    text_data_.clear();
}

void JSONWriter::writeBinaryToZipFile ()
{
    Assert (json_zip_file_open_ == TRUE, "JSON export file not open");
    assert (binary_data_.size());

//    for (const std::vector<std::uint8_t> bin_it : binary_data_)
//        archive_write_data (json_zip_file_, reinterpret_cast<const void*>(bin_it.data()), bin_it.size());

//    binary_data_.clear();

    while (file_write_in_progress_)
        std::this_thread::sleep_for(std::chrono::milliseconds(5));

    file_write_in_progress_ = true;

    JSONBinaryZipFileWriteTask* write_task = new (tbb::task::allocate_root()) JSONBinaryZipFileWriteTask (
                json_zip_file_, std::move(binary_data_), *this);
    tbb::task::enqueue(*write_task);

    assert (!binary_data_.size());
}

void JSONWriter::closeJsonZipFile ()
{
    archive_entry_free(json_zip_file_entry_);
    archive_write_close(json_zip_file_); // Note 4
    archive_write_free(json_zip_file_); // Note 5

    json_zip_file_open_ = FALSE;
}
