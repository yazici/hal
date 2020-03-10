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

#pragma once

#include "def.h"

#include <cctype>
#include <fstream>
#include <map>
#include <set>
#include <sstream>
#include <string>
#include <vector>

/* forward declaration*/
class netlist;

/**
 * @ingroup hdl_parsers
 */
class HDL_PARSER_API hdl_parser
{
public:
    /**
     * @param[in] stream - The string stream filled with the hdl code.
     */
    explicit hdl_parser(std::stringstream& stream);

    virtual ~hdl_parser() = default;

    /**
     * Parses hdl code for a specific netlist library.
     *
     * @param[in] gate_library - The gate library name.
     * @returns The netlist representation of the hdl code or a nullptr on error.
     */
    virtual bool parse() = 0;

    std::shared_ptr<netlist> instantiate(const std::string& gate_library);

protected:
    struct signal
    {
        u32 line_number;

        // name may either be the identifier of the signal or a binary string in case of direct assignments
        std::string name;
        std::vector<std::pair<i32, i32>> bounds;
        bool binary;

        bool operator==(const signal& other) const
        {
            return (name == other.name && bounds == other.bounds && binary == other.binary);
        }

        bool operator<(const signal& other) const
        {
            return name < other.name;
        }
    };

    struct instance
    {
        u32 line_number;

        std::string name;
        std::string type;
        std::map<std::string, std::vector<signal>> port_assignments;
        std::map<std::string, std::string> generic_assignments;

        bool operator<(const instance& other) const
        {
            return name < other.name;
        }
    };

    struct entity
    {
        u32 line_number;

        // name
        std::string name;

        // ports
        std::set<signal> in_ports;
        std::set<signal> out_ports;
        std::set<signal> inout_ports;

        // generics
        std::set<std::string> generics;

        // signals
        std::set<signal> signals;

        // assignments
        std::map<std::vector<signal>, std::vector<signal>> assignments;

        // instances
        std::set<instance> instances;

        bool operator<(const entity& other) const
        {
            return name < other.name;
        }
    };

    // stores the netlist
    std::shared_ptr<netlist> m_netlist;

    // stores the input stream to the file
    std::stringstream& m_fs;

    // stores all entities parsed from the HDL file
    std::set<entity> m_entities;
};
