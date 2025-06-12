#include "bjson_value.h"

#include <sstream>
#include <stdexcept>

namespace bjson {

const Value Value::null;

Value::Value() : v_(Null_Fn())
{
}

// destructor stub to get rid of 'W' symbol in share objects that depend on.
Value::~Value()
{
}

Value::Value(const char* value) : v_(std::string(value))
{
}

Value::Value(const std::string& value) : v_(value)
{
}

Value::Value(std::string&& value) : v_(std::move(value))
{
}

Value::Value(const Object& value) : v_(value)
{
}

Value::Value(Object&& value) : v_(std::move(value))
{
}

Value::Value(const Array& value) : v_(value)
{
}

Value::Value(Array&& value) : v_(std::move(value))
{
}

Value::Value(bool value) : v_(value)
{
}

Value::Value(int value) : v_(static_cast<int64_t>(value))
{
}

Value::Value(unsigned value) : v_(static_cast<uint64_t>(value))
{
}

#if __SIZEOF_LONG__ == 4
Value::Value(long value) : v_(static_cast<int64_t>(value))
{
}

Value::Value(unsigned long value) : v_(static_cast<uint64_t>(value))
{
}
#endif // __SIZEOF_LONG__ == 4

Value::Value(int64_t value) : v_(value)
{
}

Value::Value(uint64_t value) : v_(value)
{
}

Value::Value(double value) : v_(value)
{
}

Value::Value(const Value& other) : v_(other.v_)
{
}

Value::Value(Value&& other) : v_(std::move(other.v_))
{
}

Value& Value::operator=(const Value& rhs)
{
    v_ = rhs.v_;
    return *this;
}

Value& Value::operator=(Value&& rhs)
{
    v_ = std::move(rhs.v_);
    return *this;
}

bool Value::operator==(const Value& rhs) const
{
    return this == &rhs || (type() == rhs.type() && v_ == rhs.v_);
}

bool Value::compare_only_value(const Value& rhs) const
{
    switch (type()) {
        case int_type:
            return (rhs.type() == int_type && get_uint64() == rhs.get_uint64()) ||
                       (rhs.type() == real_type && get_uint64() == 0 && rhs.get_real() == 0);
        case real_type:
            return rhs.type() == int_type && get_real() == 0 && rhs.get_uint64() == 0;
        case obj_type:
        case array_type:
        case bool_type:
        case null_type:
            break;
    }

    return *this == rhs;
}

Vtype Value::type() const
{
    return is_uint64() ? int_type : static_cast<Vtype>(v_.which());
}

bool Value::is_null() const
{
    return type() == null_type;
}

bool Value::is_uint64() const
{
    return v_.which() == null_type + 1;
}

Object& Value::get_obj()
{
    check_type(obj_type);
    return *boost::get<Object>(&v_);
}

const Object& Value::get_obj() const
{
    check_type(obj_type);
    return *boost::get<Object>(&v_);
}

Array& Value::get_array()
{
    check_type(array_type);
    return *boost::get<Array>(&v_);
}

const Array& Value::get_array() const
{
    check_type(array_type);
    return *boost::get<Array>(&v_);
}

std::string& Value::get_str()
{
    check_type(str_type);
    return *boost::get<std::string>(&v_);
}

const std::string& Value::get_str() const
{
    check_type(str_type);
    return *boost::get<std::string>(&v_);
}

bool Value::get_bool() const
{
    check_type(bool_type);
    return boost::get<bool>(v_);
}

int Value::get_int() const
{
    check_type(int_type);
    return static_cast<int>(get_int64());
}

unsigned Value::get_uint() const
{
    check_type(int_type);
    return static_cast<unsigned>(get_uint64());
}

long Value::get_long() const
{
    check_type(int_type);
    return static_cast<long>(get_int64());
}

unsigned long Value::get_ulong() const
{
    check_type(int_type);
    return static_cast<unsigned long>(get_uint64());
}

int64_t Value::get_int64() const
{
    check_type(int_type);
    return is_uint64() ? static_cast<int64_t>(boost::get<uint64_t>(v_))
                       : boost::get<int64_t>(v_);
}

uint64_t Value::get_uint64() const
{
    check_type(int_type);
    return is_uint64() ? boost::get<uint64_t>(v_)
                       : static_cast<uint64_t>(boost::get<int64_t>(v_));
}

double Value::get_real() const
{
    if (type() == int_type)
        return is_uint64() ? static_cast<double>(get_uint64())
                           : static_cast<double>(get_int64());

    check_type(real_type);
    return boost::get<double>(v_);
}

Object& Value::to_new_object()
{
    v_ = Object();
    return *boost::get<Object>(&v_);
}

Array& Value::to_new_array()
{
    v_ = Array();
    return *boost::get<Array>(&v_);
}

inline Object& Value::to_object()
{
    return type() == obj_type ? get_obj() : to_new_object();
}

inline Array& Value::to_array()
{
    return type() == array_type ? get_array() : to_new_array();
}

void Value::check_type(const Vtype vtype) const
{
    if (type() != vtype) {
        std::ostringstream os;
        os << "value type is " << type() << " not " << vtype;
        throw std::runtime_error(os.str());
    }
}

};
