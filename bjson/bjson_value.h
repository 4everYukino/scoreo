#ifndef BJSON_VALUE_H
#define BJSON_VALUE_H

#include <map>
#include <string>
#include <vector>

#include <boost/config.hpp>
#include <boost/cstdint.hpp>
#include <boost/variant.hpp>

namespace bjson {
enum Vtype
{
    obj_type,
    array_type,
    str_type,
    bool_type,
    int_type,
    real_type,
    null_type
};

struct Null_Fn
{
};

class Value;
using Object = std::map<std::string, Value>;
using Array = std::vector<Value>;

class Value
{
public:
    Value();
    ~Value();

    Value(const char* value);
    Value(const std::string& value);
    Value(std::string&& value);

    Value(const Object& value);
    Value(Object&& value);

    Value(const Array& value);
    Value(Array&& value);

    Value(bool value);
    Value(int value);
    Value(unsigned value);

#if __SIZEOF_LONG__ == 4
    Value(long value);
    Value(unsigned long value);
#endif // __SIZEOF_LONG__ == 4

    Value(int64_t value);
    Value(uint64_t value);
    Value(double value);

    template <class T>
    Value(const T*) = delete;

    Value(const Value& other);
    Value(Value&&);

    Value& operator=(const Value& rhs);
    Value& operator=(Value&& rhs);

    bool operator==(const Value& rhs) const;

    bool compare_only_value(const Value& rhs) const;

    Vtype type() const;
    bool is_null() const;
    bool is_uint64() const;

    Object& get_obj();
    const Object& get_obj() const;

    Array& get_array();
    const Array& get_array() const;

    std::string& get_str();
    const std::string& get_str() const;

    bool get_bool() const;

    int get_int() const;
    unsigned get_uint() const;

    long get_long() const;
    unsigned long get_ulong() const;

    int64_t get_int64() const;
    uint64_t get_uint64() const;

    double get_real() const;

    Object& to_new_object();
    Array& to_new_array();

    Object& to_object();
    Array& to_array();

    static const Value null;

private:
    using Variant = boost::variant<boost::recursive_wrapper<Object>,
                                   boost::recursive_wrapper<Array>,
                                   std::string,
                                   bool,
                                   int64_t,
                                   uint64_t,
                                   double,
                                   Null_Fn>;

    void check_type(const Vtype vtype) const;

    Variant v_;
};

inline bool operator==(const Null_Fn&, const Null_Fn&)
{
    return true;
}

}

#endif
