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

    struct JSON_SPIRIT_Export Null {

    };

    class Value;
    using Object = std::map<std::string, Value>;
    using Array = std::vector<Value>;

    class JSON_SPIRIT_Export Value
    {
    public:
        using Const_str_ptr = std::string::const_pointer;

        Value ();
        Value (Const_str_ptr        value);

        Value (const std::string&   value);
        Value (std::string&&        value);

        Value (const secure_string& value);
        Value (secure_string&&      value);

        Value (const Object&        value);
        Value (Object&&             value);

        Value (const Array&         value);
        Value (Array&&              value);

        Value (bool                 value);
        Value (int                  value);
        Value (unsigned             value);

#if __SIZEOF_LONG__ == 4
        Value (long                 value);
        Value (unsigned long        value);
#endif // __SIZEOF_LONG__ == 4

        Value (int64_t              value);
        Value (uint64_t             value);
        Value (double               value);

        template <class T>
        Value (const T*) = delete;

        Value (const Value& other);
        Value (Value&&) COMPAT_NOEXCEPT;

        Value& operator = (const Value& lhs);
        Value& operator = (Value&&) COMPAT_NOEXCEPT;

        ~Value ();

        bool operator == (const Value& rhs) const;

        bool compare_only_value(const Value& rhs) const;

        Vtype type () const;
        bool is_uint64 () const;
        bool is_null () const;

        const Object&           get_obj () const;
        const Array&            get_array () const;
        const std::string&      get_str () const;
        bool                    get_bool () const;
        int                     get_int () const;
        unsigned                get_uint () const;
        long                    get_long () const;
        unsigned long           get_ulong () const;
        int64_t                 get_int64 () const;
        uint64_t                get_uint64 () const;
        double                  get_real () const;

        std::string& get_str();
        Object&      get_obj();
        Array&       get_array();

        Object&      to_new_object();
        Array&       to_new_array();

        Object&      to_object();
        Array&       to_array();

        static const Value null;

    private:
        using Variant = boost::variant<boost::recursive_wrapper<Object>,
                                       boost::recursive_wrapper<Array>,
                                       std::string,
                                       bool,
                                       int64_t,
                                       uint64_t,
                                       double,
                                       Null>;

        void check_type(const Vtype vtype) const;

        Variant v_;
    };

    inline bool operator==(const Null&, const Null&) {
        return true;
    }

    using object_key_value_pair = std::pair<const std::string, Value>;
}

#include "json_spirit_value.inl"

#endif
