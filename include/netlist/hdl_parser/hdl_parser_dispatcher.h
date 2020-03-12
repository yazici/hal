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

#include "core/program_options.h"

/** forward declaration */
class netlist;
class hdl_parser;

#include <set>
#include <string>
#include <tuple>

/**
 * @ingroup hdl_parsers
 */
namespace hdl_parser_dispatcher
{
    /**
     * Returns the command line interface options of the hdl parser dispatcher
     *
     * @returns The options.
     */
    program_options get_cli_options();

    /**
     * Returns the command language options for the parse() function.
     *
     * @returns The language options.
     */
    std::set<std::string> get_gui_option();

    /**
     * Returns the netlist for the file and specified command line options.
     *
     * @param[in] file_name - The input file.
     * @param[in] args - The command line options.
     * @returns The netlist representation of the hdl code or a nullpointer on error.
     */
    std::shared_ptr<netlist> parse(const hal::path& file_name, const program_arguments& args);

    /**
     * Returns the netlist for a file, parsed with a defined parser_name and gate library.
     *
     * @param[in] gate_library - The gate library used in the file.
     * @param[in] parser_name - The name of the parser to use, e.g. vhdl, verilog...
     * @param[in] file_name - The input file.
     * @returns The netlist representation of the hdl code or a nullpointer on error.
     */
    std::shared_ptr<netlist> parse(const std::string& gate_library, const std::string& parser_name, const hal::path& file_name);

    /**
    * Returns the netlist for a file, parsed with a defined parser_name and gate library.
    *
    * @param[in] gate_library - The gate library used in the file.
    * @param[in] parser_name - The name of the parser to use, e.g. vhdl, verilog...
    * @param[in] file_name - The input file. (as std::string, used for python call)
    * @returns The netlist representation of the hdl code or a nullpointer on error.
    */
    std::shared_ptr<netlist> parse(const std::string& gate_library, const std::string& parser_name, const std::string& file_name);
}    // namespace hdl_parser_dispatcher
