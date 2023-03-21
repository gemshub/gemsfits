#pragma once

#include <memory>
#include <algorithm>
#include <vector>
#include <set>
#include <sstream>
#include <limits>

#include "verror.h"
#include "type_test.h"

namespace common {

namespace detail {

extern const char* infiniteValue;

/// Serializations a numeric value to a string.
template < typename T >
std::string value2string( const T& value )
{
    std::ostringstream os;
    os << value;
    return os.str();
}
template <> std::string value2string( const double& value );
template <> std::string value2string( const float& value );
template<> std::string value2string( const bool& value );
template <> std::string value2string( const std::string& value );
std::string value2string( const char* value );

/// Deserialization a numeric value from a string.
template <class T>
bool string2value( const std::string& data, T& value )
{
    if( data.find(infiniteValue) != std::string::npos )
    {
        value = std::numeric_limits<T>::min();
        return true;
    }
    std::istringstream iss(data);
    return iss >> value && !iss.ignore();
}
template <>  bool string2value( const std::string& data, std::string& value );
template <>  bool string2value( const std::string& data, bool& value );

}

/// @brief a class to store JSON object
class JsonFree
{

public:

    enum Type {
        Null = 1,
        Bool = 2,    // T_BOOL
        Int = 8,     // T_I32
        Double = 4,  // T_DOUBLE
        String = 11, // T_STRING
        Object = 12, // T_STRUCT
        Array = 15   // T_LIST
    };

    /// An iterator for a JsonFree container
    using iterator = std::vector<std::shared_ptr<JsonFree>>::iterator;
    /// A const iterator for a JsonFree container
    using const_iterator = std::vector<std::shared_ptr<JsonFree>>::const_iterator;
    auto find_key(const std::string &key) const
    {
        return std::find_if( children.begin(), children.end(),
                             [=]( const auto& value ) { return value->get_key() == key; });
    }


    /// Create object JSON value
    static  JsonFree object( const std::string& key = "top" )
    {
        return JsonFree( Object, key, "", nullptr );
    }

    /// Create array JSON value
    static  JsonFree array( const std::string& key = "top" )
    {
        return JsonFree( Array, key, "", nullptr );
    }

    static JsonFree scalar(const std::string& value);

    /// Create object JSON value
    static  JsonFree parse( const std::string& json_str );

    /// Create object JSON value
    static  JsonFree parse(std::fstream& ff);


    /// Constructor
    JsonFree();

    /// Copy constructor
    JsonFree(const JsonFree &obj );
    /// Move constructor
    JsonFree( JsonFree &&obj ) noexcept;
    //JsonFree( JsonFree &&obj ) = default; not worked
    /// Destructor
    virtual ~JsonFree()  {}

    /// Copy assignment
    JsonFree &operator =( const JsonFree &other)
    {
        if ( this != &other)
        {
            copy(other);
        }
        return *this;
    }

    /// Move assignment
    JsonFree &operator =( JsonFree &&other)
    {
        if ( this != &other)
        {
            move(std::move(other));
        }
        return *this;
    }

    /// Other type assignment
    template <class T>
    JsonFree &operator =( const T& value )
    {
        this->set_from( value );
        return *this;
    }

    /// Return a const reference to arr[i] if this is an array, exception otherwise.
    const JsonFree &operator[](size_t idx) const
    {
        if( is_array() || is_object()  )
        {
            return get_child( idx );
        }
        Error( "JsonFree", " cannot use operator[] with a numeric argument with " + std::string( type_name() ) );
    }

    /// Return a reference to arr[i] if this is an array, exception otherwise.
    JsonFree &operator[](size_t idx)
    {
        if( is_array() || is_object()  )
        {
            return get_child( idx );
        }
        Error( "JsonFree", "cannot use operator[] with a numeric argument with " + std::string( type_name() ) );
    }


    /// Return a const reference to object[key] if this is an object, exception otherwise.
    const JsonFree &operator[](const std::string &key) const
    {
        if( is_object() )
        {
            return get_child( key );
        }
        Error( "JsonFree", "cannot use operator[] with a string argument with " + std::string( type_name() ) );
    }

    /// Return a reference to object[key] if this is an object, exception otherwise.
    JsonFree &operator[](const std::string &key)
    {
        if( is_null() )
            update_node( Object, "" );
        if( is_array() || is_object()  )
        {
            return get_child( key );
        }
        Error( "JsonFree", "cannot use operator[] with a string argument with " + std::string( type_name() ) );
    }

    /// @brief returns an iterator to one past the last element.
    iterator end()
    { return children.end(); }

    /// @brief returns an iterator to the first element.
    iterator begin()
    { return children.begin(); }

    /// @brief returns a const iterator to one past the last element.
    const_iterator end() const
    { return children.cend(); }

    /// @brief returns a const iterator to the first element
    const_iterator begin() const
    { return children.cbegin(); }

    // Test methods  --------------------------------------------

    /// This function returns true if and only if the JSON type is Object.
    virtual bool is_object() const
    { return type() == Type::Object; }

    /// This function returns true if and only if the JSON type is Array.
    virtual bool is_array() const
    { return  type() == Type::Array; }

    /// This function returns true if and only if the JSON type is a numeric value.
    virtual bool is_number() const
    { return  type() == Type::Int || type() == Type::Double; }

    /// This function returns true if and only if the JSON type is boolean.
    virtual bool is_bool() const
    { return type() == Type::Bool; }

    /// This function returns true if and only if the JSON type is Null object.
    virtual bool is_null() const
    { return type() == Type::Null; }

    /// This function returns true if and only if the JSON type is String
    virtual bool is_string() const
    { return type() == Type::String; }

    /// @brief return whether type is primitive
    /// This function returns true if and only if the JSON type is primitive
    /// (string, number, boolean, or null).
    bool is_primitive() const
    {
        return is_null() || is_string() || is_bool() || is_number();
    }

    /// @brief return whether type is structured
    /// This function returns true if and only if the JSON type is structured
    /// (array or object).
    bool is_structured() const
    {
        return is_array() || is_object();
    }

    /// Test top object
    bool is_top()
    { return parent_object == nullptr; }

    // Get methods  --------------------------

    const std::string& get_key()
    {   return  field_key;   }

    /// @brief Dump object to JSON string.
    std::string dump( bool dense = false ) const;

    /// Serialize object as a JSON formatted stream.
    void dump(std::ostream &os, bool dense = false) const;

    /// Returns a std::string with the json contained in this object.
    virtual std::string to_string(bool dense = false) const;
    /// Returns the object converted to a double value.
    virtual double to_double() const;
    /// Returns the object converted to a long value.
    virtual long   to_int() const;
    /// Returns the object converted to a boolean value.
    virtual bool   to_bool() const;

    Type type() const
    {   return  field_type;  }

    /// @brief return the type as string
    ///   Returns the type name as string to be used in error messages - usually to
    ///   indicate that a function was called on a wrong JSON type.
    const char* type_name() const
    { return  type_name( type() ); }

    std::size_t size() const
    { return  children.size(); }

    bool empty() const
    { return size() < 1; }


    // Get values  --------------------------

    /// Get internal data to json string
    bool get_to(std::string& value) const
    {
        value = to_string(true);
        return true;
    }


    /// Explicit type conversion between the JSON value and a compatible primitive value.
    /// The value is filled into the input parameter.
    /// @return true if JSON value can  be converted to value type.
    template <class T,
              std::enable_if_t<!is_container<T>{}&!is_mappish<T>{}, int> = 0 >
    bool get_to( T& value  ) const
    {
        if( is_primitive() )
            return detail::string2value( field_value, value );
        return false;
    }

    /// Explicit type conversion between the JSON value and a compatible array-like value.
    /// The value is filled into the input parameter.
    /// @return true if JSON value can  be converted to value type.
    template <class T,
              std::enable_if_t<is_container<T>{}&!is_mappish<T>{}, int> = 0 >
    bool get_to( T& value  ) const
    {
        return get_to_list( value );
    }

    /// Explicit type conversion between the JSON value and a compatible  map-like value.
    /// The value is filled into the input parameter.
    /// @return true if JSON value can  be converted to value type.
    template <class T,
              std::enable_if_t<is_container<T>{}&is_mappish<T>{}, int> = 0 >
    bool get_to( T& value  ) const
    {
        return get_to_map( value );
    }

    /// Explicit type conversion between the JSON value and a compatible array-like value.
    /// The value (std::list, std::vector ) is filled into the input parameter.
    /// @return true if JSON value can  be converted to value type.
    /// Container must have emplace_back function.
    template <class T,
              class = typename std::enable_if<is_container<T>{}, bool>::type >
    bool get_to_list( T& values  ) const
    {
        values.clear();
        if( is_null() )
            return true;
        if( !is_structured() )
            return false;
        typename T::value_type val;
        for ( size_t ii=0; ii<size(); ii++)
        {
            if( children[ii]->get_to( val ) )
                values.emplace_back(val);
        }
        return true;
    }

    /// Explicit type conversion between the JSON value and a compatible  map-like value.
    /// The value (std::map, std::unordered_map ) is filled into the input parameter.
    /// @return true if JSON value can  be converted to value type.
    template <class Map,
              class = typename std::enable_if<is_mappish<Map>{}, bool>::type >
    bool get_to_map( Map& values  ) const
    {
        values.clear();
        if( is_null() )
            return true;
        if( !is_structured() )
            return false;
        using Tkey = std::remove_const_t<typename Map::value_type::first_type>;
        using Tval = std::remove_const_t<typename Map::value_type::second_type>;

        Tval val;
        Tkey key;
        for ( size_t ii=0; ii<size(); ii++)
        {
            if( !string2v( children[ii]->get_key(), key ) )
                continue;
            if( children[ii]->get_to( val ) )
                values.emplace( key, val);
        }
        return true;
    }

    bool contains(const typename std::string& key) const
    {  return find_key(key) != children.end(); }


    std::string value(const std::string& key, const std::string& default_value) const
    {
        std::string val = default_value;
        if( contains(key) )
            get_child(key).get_to(val);
        return val;
    }

    std::string value(const std::string& key, const char* default_value) const
    {
        return value( key, std::string(default_value));
    }

    template<class T>
    T value(const std::string& key, const T& default_value) const
    {
        T val = default_value;
        if( contains(key) )
            get_child(key).get_to<T>(val);
        return val;
    }


    /// @brief merge objects
    void merge_patch(const JsonFree& other_object)
    {
        if (other_object.is_object())
        {
            if (!is_object())
            {
                *this = object();
            }
            for (auto it = other_object.begin(); it != other_object.end(); ++it)
            {
                get_child(it->get()->get_key()).merge_patch(*it->get());
            }
        }
        else
        {
            *this = other_object;
        }
    }


    // Set methods  --------------------------

    bool set_null()
    {
        update_node(  Null, "null" );
        return true;
    }

    bool set_from( const char* value )
    {
        return set_from(std::string(value));
    }

    bool set_from( const std::string& value  )
    {
        update_node(  String, (value) );
        return true;
    }

    bool set_from(bool value)
    {
        update_node(Bool, detail::value2string(value));
        return true;
    }

    /// Set Value to current Node
    template <class T,
              std::enable_if_t<!is_container<T>{}&!is_mappish<T>{}, int> = 0 >
    bool set_from( const T& value  )
    {
        if( std::is_integral<T>::value )
        {
            update_node(Int, detail::value2string(value));
        }
        else if( std::is_floating_point<T>::value )
        {
            update_node(Double, detail::value2string(value));
        }
        else
        {
            set_null();
        }
        return true;
    }

    /// Set Array to current Node
    template <class T,
              std::enable_if_t<is_container<T>{}&!is_mappish<T>{}, int> = 0 >
    bool set_from( const T& value  )
    {
        set_list_from( value );
        return true;
    }

    /// Set Map to current Node
    template <class T,
              std::enable_if_t<is_container<T>{}&is_mappish<T>{}, int> = 0 >
    bool set_from( const T& value  )
    {
        set_map_from( value );
        return true;
    }

    /// Set  vector-like objects (std::list, std::vector, std::set, etc) to current Node
    template <class T,
              class = typename std::enable_if<is_container<T>{}, bool>::type >
    void set_list_from( const T& values  )
    {
        int ii{0};
        update_node(  Array, "" );
        for( const auto& el: values )
        {
            auto obj = append_node( std::to_string(ii++), Null, "" );
            obj.set_from(el);
        }
    }

    /// Set map-like objects (std::map, std::unordered_map, etc) to current Node
    template <class T,
              class = typename std::enable_if<is_mappish<T>{}, bool>::type >
    void set_map_from( const T& values  )
    {
        update_node(  Object, "" );
        for( const auto& el: values )
        {
            auto obj = append_node( v2string(el.first), Null, "" );
            obj.set_from(el.second);
        }
    }

    /// @brief add an object to an array
    void push_back(JsonFree&& val)
    {

        if( is_null() )
            update_node( Array, "" );
        if( is_array() )
        {
            children.push_back(std::make_shared<JsonFree>(std::move(val)));
            children.back()->parent_object = this;
            children.back()->ndx_in_parent = children.size()-1;
            return;
        }
        Error( "JsonFree", "cannot use push_back() with " + std::string( type_name() ) );
    }

    // Update methods  --------------------------

    /// Clear field and set value to default (null).
    bool clear();

    /// @brief return the type as string
    ///   Returns the type name as string to be used in error messages - usually to
    ///   indicate that a function was called on a wrong JSON type.
    static const char* type_name(Type type);

private:

    /// Object type
    Type field_type;
    /// Object key  ( number for arrays )
    std::string field_key;
    /// Object value ( empty for arrays and objects )
    std::string field_value;

    /// Index into parent object ( using in GUI )
    size_t ndx_in_parent;
    /// Parent object
    JsonFree *parent_object;
    /// Children objects for Object or Array
    std::vector<std::shared_ptr<JsonFree>> children;

    /// Object constructor
    JsonFree( Type atype, const std::string &akey, const std::string& avalue, JsonFree *aparent  );

    void update_node(  Type atype, const std::string& avalue );
    JsonFree &append_node( const std::string& akey, Type atype, const std::string& avalue );

    /// Deep copy children
    void copy(const JsonFree &obj);
    /// Move children
    void move( JsonFree &&obj);

    const JsonFree &get_child(std::size_t idx) const;
    JsonFree &get_child( std::size_t idx );
    const JsonFree &get_child(const std::string& key) const;
    JsonFree &get_child(const std::string& key);
    JsonFree &get_parent() const;

    void dump2stream(std::ostream &os, int depth, bool dense) const;
};


} // namespace common


common::JsonFree fromJsonString(const std::string& json_str);
common::JsonFree fromYamlString(const std::string& yaml_str);
common::JsonFree fromString(bool is_json, const std::string& data_str);

extern void csvToBson(common::JsonFree& object,
                      const std::vector<std::string>& headline,
                      const std::vector<std::string>& row);
