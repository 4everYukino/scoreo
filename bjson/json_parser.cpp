#include "json_parser.h"
#include "number_to_value.h"
#include "ace/Log_Msg.h"

using namespace std;
using namespace json_spirit;

template <class T>
bool prepare_structured_value (stack<Value*>& current, const string& key)
{
	if (current.empty ())
		ACE_ERROR_RETURN ((LM_DEBUG,
			"prepare_structured_value: current stack is empty, return false!"),
			false);

	Value*& top = current.top ();
	switch (top->type ()) {
		case obj_type:
			{
				Object& obj = top->get_obj ();
				obj[key] =  T ();
				current.push (&obj[key]);
			}
			break;
		case array_type:
			{
				Array& arr = top->get_array ();
				arr.push_back (T ());
				current.push (&arr.back ());
			}
			break;
		default:
			if (current.size () == 1) {
				*top = T ();
			} else {
				ACE_ASSERT (false);
				ACE_ERROR_RETURN ((LM_ERROR,
					"prepare_structured_value: the top item of current stack is neither object nor array, return false!\n"),
					false);
			}
	}

	return true;
}

template <class T>
bool set_value (stack<Value*>& current, const string& key, const T& value)
{
	if (current.empty ())
		ACE_ERROR_RETURN ((LM_DEBUG,
			"set_value: current stack is empty!\n"),
			false);

	Value*& top = current.top ();
	if (!key.empty ()) {
		switch (top->type ()) {
			case obj_type:
				top->get_obj ()[key] = value;
				break;
			case array_type:
				top->get_array ().push_back (value);
				break;
			default:
				ACE_ERROR_RETURN ((LM_ERROR,
					"set_value: the top item(type=%d) of current stack must be object nor array!\n",
					top->type ()),
					false);
		}
	} else if (top->type () == array_type) {
		top->get_array ().push_back (value);
	} else {
		*top =  value;
	}

	return true;
}

DEFAULT_CTOR_DTOR_DEFINES(JSON_Parser)

bool JSON_Parser::handle_null_i ()
{
	return set_value <Value> (current_, key_, Value::null);
}

bool JSON_Parser::handle_boolean_i (bool val)
{
	return set_value <Value> (current_, key_, Value (val));
}

bool JSON_Parser::handle_number_i (const char* val, size_t len)
{
	Value v;
	number_to_value (val, len, v);
	return set_value <Value> (current_, key_, v);
}

bool JSON_Parser::handle_string_i (const char* val, size_t len)
{
	return set_value <Value> (current_, key_, Value (string (val, len)));
}

bool JSON_Parser::handle_start_map_i ()
{
	return prepare_structured_value<Object> (current_, key_);
}

bool JSON_Parser::handle_map_key_i (const char* key, size_t len)
{
	key_.assign (key, len);
	return true;
}

bool JSON_Parser::handle_end_map_i ()
{
	return pop_current ();
}

bool JSON_Parser::handle_start_array_i ()
{
	return prepare_structured_value<Array> (current_, key_);
}

bool JSON_Parser::handle_end_array_i ()
{
	return pop_current ();
}

bool JSON_Parser::pop_current ()
{
	if (current_.empty ())
		return false;

	current_.pop ();
	return true;
}

void JSON_Parser::result (json_spirit::Value* val)
{
	if (val) {
		while (!current_.empty ())
			current_.pop ();
		current_.push (val);
	}
}

size_t JSON_Parser::levels() const
{
	return current_.size();
}

const string& JSON_Parser::key() const
{
	return key_;
}
