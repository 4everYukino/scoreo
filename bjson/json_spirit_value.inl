namespace bjson {

inline bool Value::is_null() const
{
    return type() == null_type;
}

inline Object& Value::to_object()
{
    return type() == obj_type ? get_obj () : to_new_object ();
}

inline Array& Value::to_array()
{
    return type() == array_type ? get_array () : to_new_array ();
}

}
