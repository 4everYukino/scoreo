/// \file json_pointer.h
/// \brief JSON Pointer used to identify a specific value in JSON
/// \author GaoYang Cao <caogaoyang@scutech.com>

#ifndef JSON_POINTER_H_
#define JSON_POINTER_H_

#include "json_spirit_export.h"
#include "json_spirit_value.h"

#include "scrt/compat_optional.h"

#include <ace/Auto_Ptr.h>
#include <memory>
#include <type_traits>

namespace json_spirit {

class JSON_SPIRIT_Export JSON_Pointer {
public:
    JSON_Pointer() = default;
    explicit JSON_Pointer(const char* path, bool log = false, bool copy = true);

    void path(const char* path, bool copy = true);
    const char* path() const;

    #define JSON_POINTER_SET_DECLARE(TYPE) \
        bool set(Value& doc, TYPE val) const

    JSON_POINTER_SET_DECLARE(bool);
    JSON_POINTER_SET_DECLARE(double);
    JSON_POINTER_SET_DECLARE(const Object&);
    JSON_POINTER_SET_DECLARE(const Array&);
    JSON_POINTER_SET_DECLARE(const Value&);
    JSON_POINTER_SET_DECLARE(const std::string&);
    JSON_POINTER_SET_DECLARE(const secure_string&);
    JSON_POINTER_SET_DECLARE(const char*);
    JSON_POINTER_SET_DECLARE(int64_t);
    JSON_POINTER_SET_DECLARE(uint64_t);
    JSON_POINTER_SET_DECLARE(int);
    JSON_POINTER_SET_DECLARE(unsigned);

    JSON_POINTER_SET_DECLARE(std::string&&);
    JSON_POINTER_SET_DECLARE(secure_string&&);
    JSON_POINTER_SET_DECLARE(Object&&);
    JSON_POINTER_SET_DECLARE(Array&&);
    JSON_POINTER_SET_DECLARE(Value&&);

#if __SIZEOF_LONG__ == 4
    JSON_POINTER_SET_DECLARE(long);
    JSON_POINTER_SET_DECLARE(unsigned long);
#endif // __SIZEOF_LONG__ == 4

    #undef JSON_POINTER_SET_DECLARE

    #define JSON_POINTER_GET_DECLARE(TYPE) \
        bool get(const Value& doc, TYPE& val) const; \
        bool get(const Object& doc, TYPE& val) const; \
        bool get(const Array& doc, TYPE& val) const;

    JSON_POINTER_GET_DECLARE(bool);
    JSON_POINTER_GET_DECLARE(double);
    JSON_POINTER_GET_DECLARE(int);
    JSON_POINTER_GET_DECLARE(unsigned);

#if __SIZEOF_LONG__ == 4
    JSON_POINTER_GET_DECLARE(long);
    JSON_POINTER_GET_DECLARE(unsigned long);
#endif // __SIZEOF_LONG__ == 4

    JSON_POINTER_GET_DECLARE(int64_t);
    JSON_POINTER_GET_DECLARE(uint64_t);

    #define JSON_POINTER_MOVABLE_GET_DECLARE(TYPE) \
        JSON_POINTER_GET_DECLARE(TYPE); \
        bool get(Value&& doc, TYPE& val) const; \
        bool get(Object&& doc, TYPE& val) const; \
        bool get(Array&& doc, TYPE& val) const;

    JSON_POINTER_MOVABLE_GET_DECLARE(std::string);
    JSON_POINTER_MOVABLE_GET_DECLARE(secure_string);
    JSON_POINTER_MOVABLE_GET_DECLARE(Object);
    JSON_POINTER_MOVABLE_GET_DECLARE(Array);
    JSON_POINTER_MOVABLE_GET_DECLARE(Value);

    #undef JSON_POINTER_MOVABLE_GET_DECLARE
    #undef JSON_POINTER_GET_DECLARE

    template<typename T>
    OPT_NS::optional<T> get(const Value& doc) const;

    template<typename T>
    OPT_NS::optional<T> get(Value&& doc) const;

    template<typename T>
    OPT_NS::optional<T> get(const Object& doc) const;

    template<typename T>
    OPT_NS::optional<T> get(Object&& doc) const;

    template<typename T>
    OPT_NS::optional<T> get(const Array& doc) const;

    template<typename T>
    OPT_NS::optional<T> get(Array&& doc) const;

    #define JSON_POINTER_GET_POINTER_DECLARE(TYPE) \
        bool get(Value& doc, TYPE* & val) const; \
        bool get(Object& doc, TYPE* & val) const; \
        bool get(Array& doc, TYPE* & val) const;

    JSON_POINTER_GET_POINTER_DECLARE(Value);
    JSON_POINTER_GET_POINTER_DECLARE(Object);
    JSON_POINTER_GET_POINTER_DECLARE(Array);
    JSON_POINTER_GET_POINTER_DECLARE(std::string);
    JSON_POINTER_GET_POINTER_DECLARE(secure_string);

    #undef JSON_POINTER_GET_POINTER_DECLARE

    #define JSON_POINTER_GET_CONST_POINTER_DECLARE(TYPE) \
        bool get(const Value& doc, const TYPE* & val) const; \
        bool get(const Object& doc, const TYPE* & val) const; \
        bool get(const Array& doc, const TYPE* & val) const;

    JSON_POINTER_GET_CONST_POINTER_DECLARE(Value);
    JSON_POINTER_GET_CONST_POINTER_DECLARE(Object);
    JSON_POINTER_GET_CONST_POINTER_DECLARE(Array);
    JSON_POINTER_GET_CONST_POINTER_DECLARE(std::string);
    JSON_POINTER_GET_CONST_POINTER_DECLARE(secure_string);
    JSON_POINTER_GET_CONST_POINTER_DECLARE(char);

    #undef JSON_POINTER_GET_CONST_POINTER_DECLARE

private:
    bool locate(Value** val, std::string& segment) const;

    bool assign_segment(Value** curr,
                        const char* segment,
                        const char* next_segment) const;

    bool assign_array(Value** curr, const char* segment) const;

    bool assign_object(Value** curr, const char* segment) const;

    template <typename T>
    bool assign_value(Value* head, std::string& key, T&& t) const;

    static bool assign_root(Value* root, const char* segment);

    private:
    template <typename T>
    using const_value_wrapper_t = typename std::conditional<
            std::is_const<T>::value,
            const Value*,
            Value *>::type;

    template <typename T>
    const_value_wrapper_t<T> retrieve(T& root) const;

    template <typename T>
    using enable_if_is_object_t = typename std::enable_if<
            std::is_same<typename std::decay<T>::type, Object>::value, bool>::type;

    template <typename T>
    using enable_if_is_value_t = typename std::enable_if<
            std::is_same<typename std::decay<T>::type, Value>::value, bool>::type;

    template <typename T>
    using enable_if_is_array_t = typename std::enable_if<
            std::is_same<typename std::decay<T>::type, Array>::value, bool>::type;

    template <typename T, enable_if_is_object_t<T> = true>
    const_value_wrapper_t<T> get_fake_root(T& root) const;

    template <typename T, enable_if_is_value_t<T> = true>
    const_value_wrapper_t<T> get_fake_root(T& root) const;

    template <typename T, enable_if_is_array_t<T> = true>
    const_value_wrapper_t<T> get_fake_root(T& root) const;

    template <typename T, enable_if_is_object_t<T> = true>
    const_value_wrapper_t<T> get_root(T& root, const char* & p,
                                      const char* & q, const char* end,
                                      char* seg) const;

    template <typename T, enable_if_is_value_t<T> = true>
    JSON_Pointer::const_value_wrapper_t<T> get_root(T& root, const char* & p,
                                                    const char* & q,
                                                    const char* end,
                                                    char* seg) const;

    template <typename T, enable_if_is_array_t<T> = true>
    JSON_Pointer::const_value_wrapper_t<T> get_root(T& root, const char* & p,
                                                    const char* & q,
                                                    const char* end,
                                                    char* seg) const;

    template <typename T>
    T* step_into(T* root, const char* segment) const;

    static Value* get_object(Object& obj, const char* segment);
    Value* get_array(Array& arr, const char* segment) const;

    static const Value* get_object(const Object& obj, const char* segment);
    const Value* get_array(const Array& arr, const char* segment) const;

private:
    const char* path_ = nullptr;
    const bool log_ = false;
    std::string path_buf_;

    mutable std::unique_ptr<char[]> buf_;
};

#ifndef JSON_SPIRIT_BUILD_DLL
#define JSON_POINTER_GET_OPT_EXPLICT_INSTANTIATION_DECLARE(TYPE) \
    extern template \
    OPT_NS::optional<TYPE> JSON_Pointer::get<TYPE>(const Value&) const; \
    extern template \
    OPT_NS::optional<TYPE> JSON_Pointer::get<TYPE>(const Object&) const; \
    extern template \
    OPT_NS::optional<TYPE> JSON_Pointer::get<TYPE>(const Array&) const;

#define JSON_POINTER_MOVE_GET_OPT_EXPLICT_INSTANTIATION_DECLARE(TYPE) \
    JSON_POINTER_GET_OPT_EXPLICT_INSTANTIATION_DECLARE(TYPE) \
    extern template \
    OPT_NS::optional<TYPE> JSON_Pointer::get<TYPE>(Value&&) const; \
    extern template \
    OPT_NS::optional<TYPE> JSON_Pointer::get<TYPE>(Object&&) const; \
    extern template \
    OPT_NS::optional<TYPE> JSON_Pointer::get<TYPE>(Array&&) const;

JSON_POINTER_GET_OPT_EXPLICT_INSTANTIATION_DECLARE(bool)
JSON_POINTER_GET_OPT_EXPLICT_INSTANTIATION_DECLARE(double)
JSON_POINTER_GET_OPT_EXPLICT_INSTANTIATION_DECLARE(int)
JSON_POINTER_GET_OPT_EXPLICT_INSTANTIATION_DECLARE(int64_t)
JSON_POINTER_GET_OPT_EXPLICT_INSTANTIATION_DECLARE(uint64_t)

JSON_POINTER_MOVE_GET_OPT_EXPLICT_INSTANTIATION_DECLARE(std::string)
JSON_POINTER_MOVE_GET_OPT_EXPLICT_INSTANTIATION_DECLARE(secure_string)
JSON_POINTER_MOVE_GET_OPT_EXPLICT_INSTANTIATION_DECLARE(Object)
JSON_POINTER_MOVE_GET_OPT_EXPLICT_INSTANTIATION_DECLARE(Array)
JSON_POINTER_MOVE_GET_OPT_EXPLICT_INSTANTIATION_DECLARE(Value)

#undef JSON_POINTER_MOVE_GET_OPT_EXPLICT_INSTANTIATION_DECLARE
#undef JSON_POINTER_GET_OPT_EXPLICT_INSTANTIATION_DECLARE
#endif // JSON_SPIRIT_BUILD_DLL

}  // namespace json_spirit

#endif // JSON_POINTER_H_
// vim: set ts=4 sw=4 sts=4 et:
