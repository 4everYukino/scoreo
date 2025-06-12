ACE_INLINE const std::string& JSON_Dump::str() const
{
    return val_;
}

ACE_INLINE const char* JSON_Dump::c_str() const
{
    return val_.c_str();
}

ACE_INLINE JSON_Dump::size_type JSON_Dump::size() const
{
    return val_.size();
}

// vim: set ts=4 sw=4 sts=4 et:
