//  MIT License
//
//  Copyright (c) 2019 Ruhr-University Bochum, Germany, Chair for Embedded Security. All Rights reserved.
//  Copyright (c) 2019 Marc Fyrbiak, Sebastian Wallat, Max Hoffmann ("ORIGINAL AUTHORS"). All rights reserved.
//
//  Permission is hereby granted, free of charge, to any person obtaining a copy
//  of this software and associated documentation files (the "Software"), to deal
//  in the Software without restriction, including without limitation the rights
//  to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
//  copies of the Software, and to permit persons to whom the Software is
//  furnished to do so, subject to the following conditions:
//
//  The above copyright notice and this permission notice shall be included in all
//  copies or substantial portions of the Software.
//
//  THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
//  IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
//  FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
//  AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
//  LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
//  OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
//  SOFTWARE.

#include "pragma_once.h"
#ifndef __HAL_FILE_MANAGER_H__
#define __HAL_FILE_MANAGER_H__

#include "def.h"

#include "core/callback_hook.h"

#define RAPIDJSON_HAS_STDSTRING 1
#include "rapidjson/document.h"

class netlist;

/**
 * Plugin manager to load and unload plugins
 *
 * @ingroup core
 */
namespace hal_file_manager
{
    /**
     * Starts serialization to a .hal file.<br>
     * This fires all registered on_serialize callbacks.
     *
     * @param[in] file - The file path.
     * @param[in] document - The JSON document to serialize to.
     * @returns True if all registered callbacks succeed.
     */
    bool serialize(const hal::path& file, std::shared_ptr<netlist> netlist, rapidjson::Document& document);

    /**
     * Starts deserialization of a .hal file.<br>
     * This fires all registered on_deserialize callbacks.
     *
     * @param[in] file - The file path.
     * @param[in] document - The JSON document to deserialize from.
     * @returns True if all registered callbacks succeed.
     */
    bool deserialize(const hal::path& file, std::shared_ptr<netlist> netlist, rapidjson::Document& document);

    /**
     * Add a callback to notify when a .hal file is being serialized.
     *
     * @param[in] callback - The callback function. Parameters are:
     * * bool - Returns whether data was successfully extended, saving is aborted if False.
     * * const hal::path& - The hal file.
     * * rapidjson::Document& - The content to fill.
    */
    void register_on_serialize_callback(std::function<bool(const hal::path&, std::shared_ptr<netlist> netlist, rapidjson::Document&)> callback);

    /**
     * Add a callback to notify when a .hal file is being deserialized.
     *
     * @param[in] callback - The callback function. Parameters are:
     * * bool - Returns whether data was successfully read, loading is aborted if False.
     * * const hal::path& - The hal file.
     * * rapidjson::Document& - The content to fill.
    */
    void register_on_deserialize_callback(std::function<bool(const hal::path&, std::shared_ptr<netlist> netlist, rapidjson::Document&)> callback);
}    // namespace hal_file_manager

#endif /* __HAL_FILE_MANAGER_H__ */
