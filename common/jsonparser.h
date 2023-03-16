#pragma once

#include "v_json.h"

namespace jsonio {


/// Class for read JsonBase structure from json string.
class JsonParser final
{

    const std::size_t err_block_size{20};

public:

    /// Constructor
    explicit JsonParser( const std::string &jsondata ):jsontext()
    {
        set_string( jsondata );
    }

    /// Update string to parse.
    void set_string( const std::string &jsondata )
    {
        jsontext = jsondata;
        cur_pos = 0;
        end_pos = jsondata.length();
    }

    /// Parse internal string to the JSON object
    JsonFree parse(const std::string &new_str);

protected:

    /// Internal string to parse
    std::string jsontext;
    std::size_t cur_pos{0};
    std::size_t end_pos{0};

    bool skip_space();
    bool skip_space_comment();
    bool parse_string( std::string& str );
    void parse_value( int depth, const std::string& name, JsonFree& object );
    /// Parse internal jsontext string to json structure (with first {)
    void parse_object( int depth, JsonFree& object );
    /// Parse internal jsontext string to json structure (with first [)
    void parse_array( int depth, JsonFree& object );

    /// Get substr with error
    std::string err_part() const;
    void set_scalar(JsonFree& object);

};

} // namespace jsonio
