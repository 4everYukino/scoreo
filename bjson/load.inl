#if __cpp_ref_qualifiers >= 200710
ACE_INLINE const json_spirit::Value& JSON_Load::value() const&
{
    return val_;
}

ACE_INLINE json_spirit::Value JSON_Load::value() &&
{
    return std::move(val_);
}
#else // __cpp_ref_qualifiers < 200710
ACE_INLINE const json_spirit::Value& JSON_Load::value() const
{
    return val_;
}

ACE_INLINE json_spirit::Value JSON_Load::value()
{
    return val_;
}
#endif // __cpp_ref_qualifiers < 200710

ACE_INLINE JSON_Load::operator bool () const
{
    return !err_;
}

// vim: set ts=4 sw=4 sts=4 et:
