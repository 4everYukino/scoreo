#include "json_spirit_value.h"
#include <sstream>
#include <stdexcept>

#if !defined(__ACE_INLINE__)
#	include "json_spirit_value.inl"
#endif // !__ACE_INLINE__

namespace json_spirit {

const Value Value::null;

Value::Value(): v_(Null())
{
}

// destructor stub to get rid of 'W' symbol in share objects that depend on.
Value::~Value()
{
}

Value::Value(const Const_str_ptr value): v_(String_type(value))
{
}

Value::Value(const String_type& value): v_(value)
{
}

Value::Value(String_type&& value): v_(std::move(value))
{
}

Value::Value(const secure_string& value): v_(value)
{
}

Value::Value(secure_string&& value): v_(std::move(value))
{
}

Value::Value(const Object& value): v_(value)
{
}

Value::Value(Object&& value): v_(std::move(value))
{
}

Value::Value(const Array& value): v_(value)
{
}

Value::Value(Array&& value): v_(std::move(value))
{
}

Value::Value(bool value): v_(value)
{
}

Value::Value(int value): v_(static_cast <int64_t> (value))
{
}

Value::Value(unsigned value): v_(static_cast <uint64_t> (value))
{
}

#if __SIZEOF_LONG__ == 4
Value::Value(long value): v_(static_cast <int64_t> (value))
{
}

Value::Value(unsigned long value): v_(static_cast <uint64_t> (value))
{
}
#endif // __SIZEOF_LONG__ == 4

Value::Value(int64_t value): v_(value)
{
}

Value::Value(uint64_t value): v_(value)
{
}

Value::Value(double value): v_(value)
{
}

Value::Value(const Value& other): v_(other.v_)
{
}

Value::Value(Value&& other) COMPAT_NOEXCEPT
    : v_(std::move(other.v_))
{
}

bool Value::operator == (const Value& rhs) const
{
    return this == &rhs || (type() == rhs.type() && v_ == rhs.v_);
}

bool Value::compare_only_value(const Value& rhs) const
{
    switch (type()) {
    case int_type:
        return (rhs.type() == int_type && get_uint64() == rhs.get_uint64()) ||
                   (rhs.type() == real_type &&
                    get_uint64() == 0 &&
                    rhs.get_real() == 0);
    case real_type:
        return rhs.type() == int_type &&
                   get_real() == 0 &&
                   rhs.get_uint64() == 0;
    case str_type:
        if (rhs.type() == secure_str_type) {
            const auto v = get_str();
            return rhs.get_secure_str() == secure_string(v.c_str(), v.size());
        }

        break;
    case secure_str_type:
        if (rhs.type() == str_type) {
            const auto v = rhs.get_str();
            return get_secure_str() == secure_string(v.c_str(), v.size());
        }

        break;
    case obj_type:
    case array_type:
    case bool_type:
    case null_type:
        break;
    }

    return *this == rhs;
}

Value_type Value::type() const
{
    return is_uint64() ? int_type : static_cast <Value_type> (v_.which());
}

bool Value::is_uint64() const
{
    return v_.which() == null_type + 1;
}

Value& Value::operator = (const Value& rhs)
{
    v_ = rhs.v_;
    return *this;
}

Value& Value::operator = (Value&& rhs) COMPAT_NOEXCEPT
{
    v_ = std::move(rhs.v_);
    return *this;
}

void Value::check_type(const Value_type vtype) const
{
    if (type() != vtype) {
        std::ostringstream os;
        os << "value type is " << type() << " not " << vtype;
        throw std::runtime_error(os.str());
    }
}

const String_type& Value::get_str() const
{
    check_type(str_type);
    return *boost::get <String_type> (&v_);
}

const secure_string& Value::get_secure_str() const
{
    check_type(secure_str_type);
    return *boost::get <secure_string> (&v_);
}

String_type& Value::get_str()
{
    check_type(str_type);
    return *boost::get <String_type> (&v_);
}

secure_string& Value::get_secure_str()
{
    check_type(secure_str_type);
    return *boost::get <secure_string> (&v_);
}

const Object& Value::get_obj() const
{
    check_type(obj_type);
    return *boost::get <Object> (&v_);
}

const Array& Value::get_array() const
{
    check_type(array_type);
    return *boost::get <Array> (&v_);
}

bool Value::get_bool() const
{
    check_type(bool_type);
    return boost::get <bool> (v_);
}

int Value::get_int() const
{
    check_type(int_type);
    return static_cast <int> (get_int64());
}

unsigned Value::get_uint() const
{
    check_type(int_type);
    return static_cast <unsigned> (get_uint64());
}

long Value::get_long() const
{
    check_type(int_type);
    return static_cast <long> (get_int64());
}

unsigned long Value::get_ulong() const
{
    check_type(int_type);
    return static_cast <unsigned long> (get_uint64());
}

int64_t Value::get_int64() const
{
    check_type(int_type);
    return is_uint64() ? static_cast <int64_t> (boost::get <uint64_t> (v_)) : boost::get <int64_t> (v_);
}

uint64_t Value::get_uint64() const
{
    check_type(int_type);
    return is_uint64() ? boost::get <uint64_t> (v_) : static_cast <uint64_t> (boost::get <int64_t> (v_));
}

double Value::get_real() const
{
    if (type() == int_type)
        return is_uint64() ? static_cast <double> (get_uint64()) : static_cast <double> (get_int64());

    check_type(real_type);
    return boost::get <double> (v_);
}

Object& Value::get_obj()
{
    check_type(obj_type);
    return *boost::get <Object> (&v_);
}

Array& Value::get_array()
{
    check_type(array_type);
    return *boost::get <Array> (&v_);
}

Object& Value::to_new_object ()
{
    v_ = Object ();
    return *boost::get <Object> (&v_);
}

Array& Value::to_new_array ()
{
    v_ = Array ();
    return *boost::get <Array> (&v_);
}

}; // namespace json_spirit
// vim: set ts=4 sw=4 sts=4 et:
