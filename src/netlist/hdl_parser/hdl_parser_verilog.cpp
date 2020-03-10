#include "netlist/hdl_parser/hdl_parser_verilog.h"

#include "core/log.h"
#include "core/utils.h"

#include "netlist/gate.h"
#include "netlist/net.h"
#include "netlist/netlist.h"

#include "netlist/netlist_factory.h"

#include <queue>
#include <iomanip>

hdl_parser_verilog::hdl_parser_verilog(std::stringstream& stream) : hdl_parser(stream)
{
}

// ###########################################################################
// ###########          Parse HDL into intermediate format          ##########
// ###########################################################################

bool hdl_parser_verilog::parse()
{
    // tokenize file
    if (!tokenize())
    {
        return false;
    }

    // parse tokens into intermediate format
    try
    {
        if (!parse_tokens())
        {
            return false;
        }
    }
    catch (token_stream::token_stream_exception& e)
    {
        if (e.line_number != (u32)-1)
        {
            log_error("hdl_parser", "{} near line {}.", e.message, e.line_number);
        }
        else
        {
            log_error("hdl_parser", "{}.", e.message);
        }
        return false;
    }

    return true;
}

bool hdl_parser_verilog::tokenize()
{
    std::string delimiters = ",()[]{}\\#: ;=.";
    std::string current_token;
    u32 line_number = 0;

    std::string line;
    bool escaped             = false;
    bool multi_line_comment  = false;
    bool multi_line_property = false;

    std::vector<token> parsed_tokens;
    while (std::getline(m_fs, line))
    {
        line_number++;
        this->remove_comments(line, multi_line_comment, multi_line_property);

        for (char c : line)
        {
            if (c == '\\')
            {
                escaped = true;
                continue;
            }
            else if (escaped && std::isspace(c))
            {
                escaped = false;
            }

            if ((!std::isspace(c) && delimiters.find(c) == std::string::npos) || escaped)
            {
                current_token += c;
            }
            else
            {
                if (!current_token.empty())
                {
                    parsed_tokens.emplace_back(line_number, current_token);
                    current_token.clear();
                }

                if (c == '(' && parsed_tokens.back() == "#")
                {
                    parsed_tokens.back() = "#(";
                }
                else if (!std::isspace(c))
                {
                    parsed_tokens.emplace_back(line_number, std::string(1, c));
                }
            }
        }
        if (!current_token.empty())
        {
            parsed_tokens.emplace_back(line_number, current_token);
            current_token.clear();
        }
    }

    m_token_stream = token_stream(parsed_tokens, {"(", "["}, {")", "]"});
    return true;
}

bool hdl_parser_verilog::parse_tokens()
{
    while (m_token_stream.remaining() > 0)
    {
        if (!parse_entity())
        {
            return false;
        }
    }

    return true;
}

bool hdl_parser_verilog::parse_entity()
{
    entity e;

    m_token_stream.consume("module", true);
    e.line_number = m_token_stream.peek().number;
    e.name        = m_token_stream.consume();

    // parse generic list and port list
    parser_info p_info;
    parse_generic_list(e.generics);
    parse_port_list(p_info.port_names);

    m_token_stream.consume(";", true);

    auto next_token = m_token_stream.peek();
    while (next_token != "endmodule")
    {
        if (next_token == "input" || next_token == "output" || next_token == "inout")
        {
            if (!parse_port_definition(e, p_info.port_names))
            {
                return false;
            }
        }
        else if (next_token == "wire")
        {
            if (!parse_signal_definition(e))
            {
                return false;
            }
        }
        else if (next_token == "assign")
        {
            if (!parse_assign(e))
            {
                return false;
            }
        }
        else
        {
            if (!parse_instance(e))
            {
                return false;
            }
        }

        next_token = m_token_stream.peek();
    }

    m_token_stream.consume("endmodule", true);

    m_entities.insert(e);

    return true;
}

void hdl_parser_verilog::parse_generic_list(std::set<std::string>& generic_names)
{
    if (m_token_stream.consume("#("))
    {
        auto generics = m_token_stream.extract_until(")", token_stream::END_OF_STREAM, true, true);

        while (generics.remaining() > 0)
        {
            generic_names.insert(generics.consume().string);
            generics.consume(",", generics.remaining() > 0);
        }

        m_token_stream.consume(")", true);
    }
}

void hdl_parser_verilog::parse_port_list(std::set<std::string>& port_names)
{
    m_token_stream.consume("(", true);
    auto ports = m_token_stream.extract_until(")", token_stream::END_OF_STREAM, true, true);

    while (ports.remaining() > 0)
    {
        port_names.insert(ports.consume().string);
        ports.consume(",", ports.remaining() > 0);
    }

    m_token_stream.consume(")", true);
}

bool hdl_parser_verilog::parse_port_definition(entity& e, const std::set<std::string>& port_names)
{
    auto line_number = m_token_stream.peek().number;
    auto direction   = m_token_stream.consume();
    auto ports       = parse_signal_list();

    if (ports.empty())
    {
        // error already displayed
        return false;
    }

    for (const auto& port : ports)
    {
        if (port_names.find(port.name) == port_names.end())
        {
            log_error("hdl_parser", "port name '{}' in line {} has not been declared in entity port list.", port.name, line_number);
            return false;
        }
    }

    if (direction == "input")
    {
        e.in_ports.insert(ports.begin(), ports.end());
    }
    else if (direction == "output")
    {
        e.out_ports.insert(ports.begin(), ports.end());
    }
    else if (direction == "inout")
    {
        e.inout_ports.insert(ports.begin(), ports.end());
    }

    return true;
}

bool hdl_parser_verilog::parse_signal_definition(entity& e)
{
    m_token_stream.consume("wire", true);
    auto signals = parse_signal_list();

    if (signals.empty())
    {
        // error already displayed
        return false;
    }

    e.signals.insert(signals.begin(), signals.end());
    return true;
}

bool hdl_parser_verilog::parse_assign(entity& e)
{
    m_token_stream.consume("assign", true);
    auto left_str = m_token_stream.extract_until("=", token_stream::END_OF_STREAM, true, true);
    m_token_stream.consume("=", true);
    auto right_str = m_token_stream.extract_until(";", token_stream::END_OF_STREAM, true, true);
    m_token_stream.consume(";", true);

    // extract assignments for each bit
    auto left_parts  = get_assignment_signals(left_str, false);
    auto right_parts = get_assignment_signals(right_str, true);

    // verify correctness
    if (left_parts.empty() || right_parts.empty())
    {
        // error already printed in subfunction
        return false;
    }

    e.assignments.emplace(left_parts, right_parts);

    return true;
}

bool hdl_parser_verilog::parse_instance(entity& e)
{
    instance inst;
    inst.line_number = m_token_stream.peek().number;
    inst.type        = m_token_stream.consume();

    // parse generics map
    if (m_token_stream.consume("#("))
    {
        auto generic_str = m_token_stream.extract_until(")", token_stream::END_OF_STREAM, true, true);

        while (generic_str.remaining() > 0)
        {
            std::string processed_rhs;

            generic_str.consume(".", true);

            auto generic_lhs = generic_str.extract_until("(", token_stream::END_OF_STREAM, true, true).join("");

            generic_str.consume("(", true);

            auto generic_rhs = generic_str.extract_until(")", token_stream::END_OF_STREAM, true, true).join("");

            generic_str.consume(")", true);

            generic_str.consume(",", generic_str.remaining() > 0);

            if (generic_rhs.string.find('\'') != std::string::npos)
            {
                processed_rhs = get_hex_from_literal(generic_rhs);
            }
            else
            {
                processed_rhs = generic_rhs.string;
            }

            inst.generic_assignments.emplace(generic_lhs.string, processed_rhs);
        }

        m_token_stream.consume(")", true);
    }

    // parse instance name
    inst.name = m_token_stream.consume();

    // parse port map
    m_token_stream.consume("(", true);
    auto port_str = m_token_stream.extract_until(")", token_stream::END_OF_STREAM, true, true);

    while (port_str.remaining() > 0)
    {
        port_str.consume(".", true);

        auto port_lhs = port_str.extract_until("(", token_stream::END_OF_STREAM, true, true).join("").string;

        port_str.consume("(", true);

        auto port_rhs = port_str.extract_until(")", token_stream::END_OF_STREAM, true, true);

        port_str.consume(")", true);

        port_str.consume(",", port_str.remaining() > 0);

        // check if port unconnected
        if (port_rhs.size() != 0)
        {
            auto processed_rhs = get_assignment_signals(port_rhs, true);

            // verify correctness
            if (processed_rhs.empty())
            {
                // error already printed in subfunction
                return false;
            }

            inst.port_assignments.emplace(port_lhs, processed_rhs);
        }
    }

    m_token_stream.consume(")", true);
    m_token_stream.consume(";", true);

    // add to vector of instances of current entity
    e.instances.insert(inst);

    return true;
}

// ###########################################################################
// ###################          Helper functions          ####################
// ###########################################################################

void hdl_parser_verilog::remove_comments(std::string& line, bool& multi_line_comment, bool& multi_line_property)
{
    bool repeat = true;

    while (repeat)
    {
        repeat = false;

        // skip empty lines
        if (line.empty())
        {
            break;
        }

        auto single_line_comment_begin = line.find("//");
        auto multi_line_comment_begin  = line.find("/*");
        auto multi_line_comment_end    = line.find("*/");
        auto multi_line_property_begin = line.find("(*");
        auto multi_line_property_end   = line.find("*)");

        std::string begin = "";
        std::string end   = "";

        if (multi_line_comment == true)
        {
            if (multi_line_comment_end != std::string::npos)
            {
                // multi-line comment ends in current line
                multi_line_comment = false;
                line               = line.substr(multi_line_comment_end + 2);
                repeat             = true;
            }
            else
            {
                // current line entirely within multi-line comment
                line = "";
                break;
            }
        }
        else if (multi_line_property == true)
        {
            if (multi_line_property_end != std::string::npos)
            {
                // multi-line property ends in current line
                multi_line_property = false;
                line                = line.substr(multi_line_property_end + 2);
                repeat              = true;
            }
            else
            {
                // current line entirely in multi-line property
                line = "";
                break;
            }
        }
        else
        {
            if (single_line_comment_begin != std::string::npos)
            {
                if (multi_line_comment_begin == std::string::npos || (multi_line_comment_begin != std::string::npos && multi_line_comment_begin > single_line_comment_begin))
                {
                    // single-line comment
                    line   = line.substr(0, single_line_comment_begin);
                    repeat = true;
                }
            }
            else if (multi_line_comment_begin != std::string::npos)
            {
                if (multi_line_comment_end != std::string::npos)
                {
                    // multi-line comment entirely in current line
                    line   = line.substr(0, multi_line_comment_begin) + line.substr(multi_line_comment_end + 2);
                    repeat = true;
                }
                else
                {
                    // multi-line comment starts in current line
                    multi_line_comment = true;
                    line               = line.substr(0, multi_line_comment_begin);
                }
            }
            else if (multi_line_property_begin != std::string::npos)
            {
                if (multi_line_property_end != std::string::npos)
                {
                    // multi-line property entirely in current line
                    line   = line.substr(0, multi_line_property_begin) + line.substr(multi_line_property_end + 2);
                    repeat = true;
                }
                else
                {
                    // multi-line property starts in current line
                    multi_line_property = true;
                    line                = line.substr(0, multi_line_property_begin);
                }
            }
        }
    }
}

std::set<hdl_parser_verilog::signal> hdl_parser_verilog::parse_signal_list()
{
    std::set<signal> signals;

    std::vector<std::pair<i32, i32>> bounds;

    auto signal_str = m_token_stream.extract_until(";", token_stream::END_OF_STREAM, true, true);
    m_token_stream.consume(";", true);

    // extract bounds
    while (signal_str.peek() == "[")
    {
        signal_str.consume("[", true);
        auto lower = signal_str.consume();
        signal_str.consume(":", true);
        auto upper = signal_str.consume();
        signal_str.consume("]", true);

        try
        {
            bounds.emplace_back(std::stoi(lower.string), std::stoi(upper.string));
        }
        catch (std::invalid_argument& e)
        {
            log_error("hdl_parser", "no integer bounds given in entity defintion in line {}: [{}:{}] .", lower.string, upper.string, signal_str.peek().number);
            return {};
        }
        catch (std::out_of_range& e)
        {
            log_error("hdl_parser", "bounds out of range in entity defintion in line {}: [{}:{}].", lower.string, upper.string, signal_str.peek().number);
            return {};
        }
    }

    // extract names
    do
    {
        signal s;
        s.line_number = signal_str.peek().number;
        s.name        = signal_str.consume();
        s.bounds      = bounds;
        signals.insert(s);
    } while (signal_str.consume(",", false));

    return signals;
}

std::vector<hdl_parser_verilog::signal> hdl_parser_verilog::get_assignment_signals(token_stream& signal_str, bool allow_numerics)
{
    // PARSE ASSIGNMENT
    //   assignment can currently be one of the following:
    //   (1) NAME *single-dimensional*
    //   (2) NAME *multi-dimensional*
    //   (3) NUMBER
    //   (4) NAME[INDEX1][INDEX2]...
    //   (5) NAME[BEGIN_INDEX1:END_INDEX1][BEGIN_INDEX2:END_INDEX2]...
    //   (6) {(1) - (5), (1) - (5), ...}

    std::vector<signal> result;
    std::vector<token_stream> parts;

    // (6) {(1) - (5), (1) - (5), ...}
    if (signal_str.peek() == "{")
    {
        signal_str.consume("{", true);

        auto assignment_list_str = signal_str.extract_until("}", token_stream::END_OF_STREAM, true, true);

        while (assignment_list_str.remaining() > 0)
        {
            parts.push_back(assignment_list_str.extract_until(",", token_stream::END_OF_STREAM, true, false));
            assignment_list_str.consume(",", false);
        }

        signal_str.consume("}", true);
    }
    else
    {
        parts.push_back(signal_str);
    }

    for (auto& part_stream : parts)
    {
        auto stream_backup = part_stream;
        auto signal_name   = part_stream.consume();

        // (3) NUMBER
        if (isdigit(signal_name.string[0]) || signal_name.string[0] == '\'')
        {
            if (!allow_numerics)
            {
                log_error("hdl_parser", "numeric value {} not allowed at this position in line {}.", signal_name.string, signal_name.number);
                return {};
            }

            signal s;
            s.line_number = signal_name.number;
            s.binary      = true;
            s.name        = get_bin_from_literal(signal_name);
            s.bounds      = {std::make_pair(signal_name.string.size() - 1, 0)};

            result.push_back(s);

            continue;
        }

        // create new signal for assign
        signal s;
        s.line_number = signal_name.number;
        s.name        = signal_name.string;

        std::vector<std::pair<i32, i32>> bounds;

        // any bounds specified?
        if (part_stream.consume("["))
        {
            // (5) NAME[BEGIN_INDEX1:END_INDEX1][BEGIN_INDEX2:END_INDEX2]...
            if (part_stream.find_next(":", part_stream.position() + 2) != part_stream.position() + 2)
            {
                do
                {
                    i32 left_bound = std::stoi(part_stream.consume());
                    part_stream.consume(":", true);
                    i32 right_bound = std::stoi(part_stream.consume());

                    bounds.emplace_back(left_bound, right_bound);

                    part_stream.consume("]", true);
                } while (part_stream.consume("[", false));
            }
            // (4) NAME[INDEX1][INDEX2]...
            else
            {
                do
                {
                    i32 index = std::stoi(part_stream.consume());

                    bounds.emplace_back(index, index);

                    part_stream.consume("]", true);
                } while (part_stream.consume("["));
            }
        }
        else
        {
            // (1) NAME *single-dimensional*
            // (2) NAME *multi-dimensional*
            bounds.emplace_back(-1, -1);
        }

        s.bounds = bounds;

        result.push_back(s);
    }

    return result;
}

static std::map<char, std::string> oct_to_bin = {{'0', "000"}, {'1', "001"}, {'2', "010"}, {'3', "011"}, {'4', "100"}, {'5', "101"}, {'6', "110"}, {'7', "111"}};
static std::map<char, std::string> hex_to_bin = {{'0', "0000"},
                                                 {'1', "0001"},
                                                 {'2', "0010"},
                                                 {'3', "0011"},
                                                 {'4', "0100"},
                                                 {'5', "0101"},
                                                 {'6', "0110"},
                                                 {'7', "0111"},
                                                 {'8', "1000"},
                                                 {'9', "1001"},
                                                 {'a', "1010"},
                                                 {'b', "1011"},
                                                 {'c', "1100"},
                                                 {'d', "1101"},
                                                 {'e', "1110"},
                                                 {'f', "1111"}};

std::string hdl_parser_verilog::get_bin_from_literal(token& value_token)
{
    auto line_number  = value_token.number;
    std::string value = core_utils::to_lower(core_utils::trim(core_utils::replace(value_token, "_", "")));

    i32 len;
    std::string prefix;
    std::string number;
    u32 base;
    std::string res;

    // base specified?
    if (value.find('\'') == std::string::npos)
    {
        len    = -1;
        prefix = "d";
        number = value;
    }
    else
    {
        len    = std::stoi(value.substr(0, value.find('\'')));
        prefix = value.substr(value.find('\'') + 1, 1);
        number = value.substr(value.find('\'') + 2);
    }

    // select base
    switch (prefix.at(0))
    {
        case 'b':
            base = 2;
            break;

        case 'o':
            base = 8;
            break;

        case 'd':
            base = 10;
            break;

        case 'h':
            base = 16;
            break;

        default:
            log_error("hdl_parser", "invalid base '{}' within number literal {} in line {}.", prefix, value, line_number);
            return "";
    }

    // parse number literal
    if (base == 2)
    {
        for (const auto& c : number)
        {
            if (c == 'x' || c == 'z' || c == '?')
            {
                log_error("hdl_parser", "character '{}' is not yet supported within binary number literal {} in line {}.", c, value, line_number);
                return "";
            }
            else if (c >= '0' && c <= '1')
            {
                res += c;
            }
            else
            {
                log_error("hdl_parser", "invalid character '{}' within binary number literal {} in line {}.", c, value, line_number);
                return "";
            }
        }
    }
    else if (base == 8)
    {
        for (const auto& c : number)
        {
            if (c == 'x' || c == 'z' || c == '?')
            {
                log_error("hdl_parser", "character '{}' is not yet supported within octal number literal {} in line {}.", c, value, line_number);
                return "";
            }
            else if (c >= '0' && c <= '7')
            {
                res += oct_to_bin[c];
            }
            else
            {
                log_error("hdl_parser", "invalid character '{}' within octal number literal {} in line {}.", c, value, line_number);
                return "";
            }
        }
    }
    else if (base == 10)
    {
        u64 tmp_val = 0;

        for (const auto& c : number)
        {
            if (c >= '0' && c <= '9')
            {
                tmp_val = (tmp_val * 10) + (c - '0');
            }
            else
            {
                log_error("hdl_parser", "invalid character '{}' within octal number literal {} in line {}.", c, value, line_number);
                return "";
            }
        }

        do
        {
            res = std::to_string(tmp_val & 0x1) + res;
            tmp_val >>= 1;
        } while (tmp_val != 0);
    }
    else if (base == 16)
    {
        for (const auto& c : number)
        {
            if (c == 'x' || c == 'z' || c == '?')
            {
                log_error("hdl_parser", "character '{}' is not yet supported within hexadecimal number literal {} in line {}.", c, value, line_number);
                return "";
            }
            else if ((c >= '0' && c <= '9') || (c >= 'a' && c <= 'f'))
            {
                res += hex_to_bin[c];
            }
            else
            {
                log_error("hdl_parser", "invalid character '{}' within hexadecimal number literal {} in line {}.", c, value, line_number);
                return "";
            }
        }
    }

    for (i32 i = 0; i < len - (i32)res.size(); i++)
    {
        res = "0" + res;
    }

    return res;
}

std::string hdl_parser_verilog::get_hex_from_literal(token& value_token)
{
    auto line_number  = value_token.number;
    std::string value = core_utils::to_lower(core_utils::trim(core_utils::replace(value_token, "_", "")));

    i32 len;
    std::string prefix;
    std::string number;
    u32 base;
    std::string res;

    // base specified?
    if (value.find('\'') == std::string::npos)
    {
        len    = -1;
        prefix = "d";
        number = value;
    }
    else
    {
        len    = std::stoi(value.substr(0, value.find('\'')));
        prefix = value.substr(value.find('\'') + 1, 1);
        number = value.substr(value.find('\'') + 2);
    }

    if (number.find('x') != std::string::npos)
    {
        log_error("hdl_parser", "character 'x' is not yet supported within number literal {} in line {}.", value, line_number);
        return "";
    }
    else if (number.find('z') != std::string::npos)
    {
        log_error("hdl_parser", "character 'z' is not yet supported within number literal {} in line {}.", value, line_number);
        return "";
    }
    else if (number.find('?') != std::string::npos)
    {
        log_error("hdl_parser", "character '?' is not yet supported within number literal {} in line {}.", value, line_number);
        return "";
    }

    // select base
    switch (prefix.at(0))
    {
        case 'b':
            base = 2;
            break;

        case 'o':
            base = 8;
            break;

        case 'd':
            base = 10;
            break;

        case 'h':
            base = 16;
            break;

        default:
            log_error("hdl_parser", "invalid base '{}' within number literal {} in line {}.", prefix, value, line_number);
            return "";
    }

    std::stringstream ss;
    ss << std::setfill('0') << std::setw((len + 3) / 4) << std::hex << stoull(number, 0, base);
    return ss.str();
}