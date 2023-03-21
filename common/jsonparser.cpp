#include <locale>
#include "jsonparser.h"
#include "v_detail.h"
#include "v_service.h"


namespace common {

static const int json_max_depth = 10000;
///  @brief scan a string literal
static void undump_string( std::string& strvalue );

enum {
    jsBeginArray = '[',    //0x5b,
    jsBeginObject = '{',   //0x7b,
    jsEndArray = ']',      //0x5d,
    jsEndObject = '}',     //0x7d,
    jsNameSeparator = ':', //0x3a,
    jsValueSeparator = ',',//0x2c,
    jsQuote = '\"'      //0x22
};

JsonFree JsonParser::parse(const std::string& new_str)
{
    JsonFree obj;

    if( !new_str.empty() ) {
        set_string( new_str );
    }

    cur_pos = 0;
    skip_space_comment();

    if( jsontext[cur_pos] == jsBeginObject ) {
        obj = JsonFree::object();
        parse_object( 0, obj );
    }
    else if( jsontext[cur_pos] == jsBeginArray ) {
        obj = JsonFree::array();
        parse_array( 0, obj );
    }
    else { // update value Scalar
        set_scalar(obj);
    }
    skip_space_comment();
    ErrorIf( cur_pos < end_pos, "JsonParser", "extra value after close: " + err_part() );
    return obj;
}

//  object = { "<key1>" : <value1>, ... , "<keyN>" : <valueN> }
void JsonParser::parse_object( int depth, JsonFree& object)
{
    std::string keyn;
    if( jsontext[cur_pos] == jsBeginObject ) {
        cur_pos++;
    }
    ErrorIf( !skip_space_comment(), "JsonParser", "unterminated object: " + err_part() );

    // empty object
    if( jsontext[cur_pos] == jsEndObject ) {
        cur_pos++;
        return;
    }

    do {
        if( !parse_string( keyn ) ) {
            Error(  "JsonParser",  "missing key of object: " + err_part() );
        }
        ErrorIf( !skip_space_comment(), "JsonParser", "missing value of object: " + err_part() );

        if( jsontext[cur_pos] == jsNameSeparator  ) {
            cur_pos++;
        }
        else {
            Error(  "JsonParser", "missing ':' : " + err_part() );
        }

        parse_value( depth, keyn, object );
        ErrorIf( !skip_space_comment(), "JsonParser", "unterminated object: " + err_part() );

        if( jsontext[cur_pos] == jsEndObject ) {
            cur_pos++;
            return;
        }
    } while( jsontext[cur_pos++] == jsValueSeparator );

    Error(  "JsonParser", "illegal symbol : '" + jsontext.substr(cur_pos-1, err_block_size)+"'" );
}

//    array = [ <value1>, ... <valueN> ]
void JsonParser::parse_array( int depth, JsonFree& object)
{
    if( jsontext[cur_pos] == jsBeginArray ) {
        cur_pos++;
    }
    ErrorIf( !skip_space_comment(), "JsonParser", "unterminated array: " + err_part() );

    // empty array
    if( jsontext[cur_pos] == jsEndArray ) {
        cur_pos++;
        return;
    }

    do {
        parse_value( depth, "",  object);
        ErrorIf( !skip_space_comment(), "JsonParser", "unterminated array: " + err_part() );
        if( jsontext[cur_pos] == jsEndArray ) {
            cur_pos++;
            return;
        }
    } while( jsontext[cur_pos++] == jsValueSeparator );

    Error(  "JsonParser", "illegal symbol : '" + jsontext.substr(cur_pos-1, err_block_size)+"'" );
}

std::string JsonParser::err_part() const
{
    std::string asubstr;
    if( cur_pos < end_pos - err_block_size) {
        asubstr =  jsontext.substr(cur_pos, err_block_size);
    }
    else if( end_pos < err_block_size ) {
        asubstr = jsontext;
    }
    else {
        asubstr = jsontext.substr( end_pos - err_block_size);
    }

    return "'" + asubstr  + "'";
}


bool JsonParser::skip_space()
{
    for( ; cur_pos<end_pos; ++cur_pos ) {
        if( !std::isspace(jsontext[cur_pos], std::locale()) ) {
            break;
        }
    }
    return (cur_pos < end_pos);
}

bool JsonParser::skip_space_comment()
{
    bool haznext = skip_space();
    while( haznext && jsontext[cur_pos] == '#' ) {
        cur_pos = jsontext.find_first_of( "\n", cur_pos );
        if( cur_pos == std::string::npos ) {
            return  false;
        }
        haznext = skip_space();
    }
    return haznext;
}

bool JsonParser::parse_string( std::string &str )
{
    bool lastCh = false;
    str = "";
    ErrorIf( !skip_space_comment(), "JsonParser", "must be string: " + err_part() );

    if( jsontext[cur_pos++] != jsQuote ) {
        return false;
    }

    while( jsontext[cur_pos] != jsQuote || lastCh ) {
        // non-UTF-8 sequence
        if(!std::isspace(jsontext[cur_pos], std::locale())
                && static_cast<uint8_t>(jsontext[cur_pos]) < 0x20) {
            // control character
            Error( "JsonParser", "Unexpected control character" + err_part() );
        }

        str += jsontext[cur_pos];
        if( jsontext[cur_pos] == '\\' ) {
            lastCh = !lastCh;
        }
        else {
            lastCh = false;
        }

        cur_pos++;
        if( cur_pos>=end_pos ) {
            return false;
        }
    }
    cur_pos++;
    // conwert all pair ('\\''\n') to one simbol '\n' and other
    //undumpString( str );
    return true;
}

void JsonParser::parse_value( int depth, const std::string &name, JsonFree& object)
{
    ErrorIf( depth > json_max_depth, "JsonParser", "exceeded maximum nesting depth " + err_part() );
    ErrorIf( !skip_space_comment(), "JsonParser", "must be value " + err_part() );

    switch( jsontext[cur_pos] )  {
    case jsQuote: {
        std::string str;
        parse_string( str );
        // conwert all pair ('\\''\n') to one simbol '\n' and other
        undump_string( str );
        if(name.empty()) {
            object.push_back(JsonFree::scalar(str));
        }
        else {
            object[name] = str;
        }
    }
        break;

    case jsBeginArray: {
        auto obj = JsonFree::array(name);
        parse_array( depth+1, obj );
        if(name.empty()) {
            object.push_back(std::move(obj));
        }
        else {
            object[name] = obj;
        }
    }
        break;

    case jsBeginObject: {
        auto obj = JsonFree::object(name);
        parse_object( depth+1, obj );
        if(name.empty()) {
            object.push_back(std::move(obj));
        }
        else {
            object[name] = obj;
        }
    }
        break;

    default:  { // addScalar true/false/null/number

        auto pos_end_value = jsontext.find_first_of( ",]}", cur_pos );
        auto end_size = pos_end_value;
        if( pos_end_value != std::string::npos ) {
            end_size -= cur_pos;
        }
        auto valuestr = jsontext.substr(cur_pos, end_size);
        trim(valuestr);
        ErrorIf( valuestr.empty(), "JsonParser", "must be value " + err_part() );
        if(name.empty()) {
            object.push_back(JsonFree::scalar(valuestr));
        }
        else {
            object[name] = JsonFree::scalar(valuestr);
        }
        cur_pos = pos_end_value;
    }
        break;
    }
}

// decision about value type
void JsonParser::set_scalar( JsonFree& object )
{
    if( jsontext[cur_pos] == jsQuote ) {
        std::string str;
        parse_string( str );
        undump_string( str );
        object.set_from( str );
        return;
    }

    std::string valuestr(jsontext.substr(cur_pos));
    cur_pos = std::string::npos;
    object = JsonFree::scalar(valuestr);
}



// use decoder --------------------------------------------
// https://github.com/dropbox/json11/blob/master/json11.cpp
// see other https://github.com/nlohmann/json/blob/develop/include/nlohmann/detail/input/lexer.hpp

template < class T>
bool in_range( const T& x, const T& lower, const T& upper)
{
    return (x >= lower && x <= upper);
}

/* esc(c)
 *
 * Format char c suitable for printing in an error message.
 */
static inline std::string esc(char c) {
    char buf[12];
    if (static_cast<uint8_t>(c) >= 0x20 && static_cast<uint8_t>(c) <= 0x7f) {
        snprintf(buf, sizeof buf, "'%c' (%d)", c, c);
    } else {
        snprintf(buf, sizeof buf, "(%d)", c);
    }
    return std::string(buf);
}

/* encode_utf8(pt, out)
 *
 * Encode pt as UTF-8 and add it to out.
 */
static void encode_utf8(long pt, std::string & out) {
    if (pt < 0)
        return;

    if (pt < 0x80) {
        out += static_cast<char>(pt);
    } else if (pt < 0x800) {
        out += static_cast<char>((pt >> 6) | 0xC0);
        out += static_cast<char>((pt & 0x3F) | 0x80);
    } else if (pt < 0x10000) {
        out += static_cast<char>((pt >> 12) | 0xE0);
        out += static_cast<char>(((pt >> 6) & 0x3F) | 0x80);
        out += static_cast<char>((pt & 0x3F) | 0x80);
    } else {
        out += static_cast<char>((pt >> 18) | 0xF0);
        out += static_cast<char>(((pt >> 12) & 0x3F) | 0x80);
        out += static_cast<char>(((pt >> 6) & 0x3F) | 0x80);
        out += static_cast<char>((pt & 0x3F) | 0x80);
    }
}


///  @brief scan a string literal
static void undump_string( std::string& strvalue )
{
    if(strvalue.empty()) {
        return;
    }

    if( strvalue.find_first_of("\\") != std::string::npos ) {
        std::string resstr = "";
        size_t ii=0;
        long last_escaped_codepoint = -1;

        while( ii<strvalue.length() )  {
            if( strvalue[ii]  == '\\') {
                ii++;
                auto ch = strvalue[ii++];

                if( ch == 'u') {
                    // Extract 4-byte escape sequence
                    std::string esc = strvalue.substr(ii, 4);
                    // Explicitly check length of the substring. The following loop
                    // relies on std::string returning the terminating NUL when
                    // accessing str[length]. Checking here reduces brittleness.
                    ErrorIf( esc.length() < 4, "JsonParser", "bad \\u escape: " + esc );
                    for (size_t j = 0; j < 4; j++) {
                        ErrorIf( !in_range(esc[j], 'a', 'f') && !in_range(esc[j], 'A', 'F') && !in_range(esc[j], '0', '9'),
                                 "JsonParser", "bad \\u escape: " + esc );
                    }
                    long codepoint = strtol(esc.data(), nullptr, 16);

                    // JSON specifies that characters outside the BMP shall be encoded as a pair
                    // of 4-hex-digit \u escapes encoding their surrogate pair components. Check
                    // whether we're in the middle of such a beast: the previous codepoint was an
                    // escaped lead (high) surrogate, and this is a trail (low) surrogate.
                    if (in_range<long>(last_escaped_codepoint, 0xD800, 0xDBFF)
                            && in_range<long>(codepoint, 0xDC00, 0xDFFF)) {
                        // Reassemble the two surrogate pairs into one astral-plane character, per
                        // the UTF-16 algorithm.
                        encode_utf8((((last_escaped_codepoint - 0xD800) << 10)
                                     | (codepoint - 0xDC00)) + 0x10000, resstr);
                        last_escaped_codepoint = -1;
                    } else {
                        encode_utf8(last_escaped_codepoint, resstr);
                        last_escaped_codepoint = codepoint;
                    }

                    ii += 4;
                    continue;
                }

                encode_utf8(last_escaped_codepoint, resstr);
                last_escaped_codepoint = -1;

                switch( ch ) {
                case '\"': resstr += '\"'; break;
                case '\'': resstr += '\''; break;
                case '\\': resstr += '\\'; break;
                case '/':  resstr += '/';  break;
                case 'n':  resstr += '\n'; break;
                case 'r':  resstr += '\r'; break;
                case 't':  resstr += '\t'; break;
                case 'b':  resstr += '\b'; break;
                case 'f':  resstr += '\f'; break;
                default:
                    Error( "JsonParser", "invalid escape character " + esc(ch) );
                }
            }
            else {
                encode_utf8(last_escaped_codepoint, resstr);
                last_escaped_codepoint = -1;
                auto ch = strvalue[ii++];
                ErrorIf( !std::isspace(ch, std::locale()) && in_range<long>(ch, 0, 0x1f),
                         "JsonParser", "unescaped in string" );
                resstr += ch;
            }
        }
        encode_utf8(last_escaped_codepoint, resstr);
        strvalue =  resstr;
    }
}

} // namespace common
