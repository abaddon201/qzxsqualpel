//
// Created by aglebov on 3/19/2019.
//

#ifndef JSONHELPER_H
#define JSONHELPER_H

#include <optional>
#include <string>
#include <list>
#include <stdexcept>

#define RAPIDJSON_ASSERT_THROWS 1
#define RAPIDJSON_PARSE_ERROR_NORETURN(parseErrorCode, offset)  throw ParseException(parseErrorCode, #parseErrorCode, offset)
#define RAPIDJSON_GET_VALUE_FAILURE(name)   throw JsonValueNotFound(name)
#define RAPIDJSON_TEST_TYPE(test, expected, val)   do {if (!(test)) throw JsonWrongType("JSON ERROR: Expected " expected "."); } while (false)
#define RAPIDJSON_HAS_CXX11_RVALUE_REFS 1
#define RAPIDJSON_HAS_STDSTRING 1
#define RAPIDJSON_ASSERT(check) do{if(!(check))  throw JsonException("Assertion failed: " #check);}while(false)

#include "rapidjson/error/error.h" // rapidjson::ParseResult

class JsonException : public std::runtime_error {
public:
  explicit JsonException(const char* str) : runtime_error(str) {}

  explicit JsonException(const std::string& str) : runtime_error(str) {}
};

class ParseException : public JsonException, public rapidjson::ParseResult {
public:
  ParseException(rapidjson::ParseErrorCode code, const char* msg, size_t offset)
    : JsonException(msg), ParseResult(code, offset) {}
};

class JsonValueNotFound : public JsonException {
public:
  explicit JsonValueNotFound(const std::string& field_name) : JsonException(
    "JSON ERROR: No value found for key: " + field_name) {}

  template<typename T>
  explicit JsonValueNotFound(const T& val)
    : JsonException(val.IsString()
                    ? std::string("JSON ERROR: No value found for key: ") + val.GetString()
                    : std::string("JSON ERROR: No value found for non-string key.")) {}
};

class JsonWrongType : public JsonException {
public:
  explicit JsonWrongType(const char* msg) : JsonException(msg) {}
};

#include "rapidjson/document.h"

namespace json {

void add_object(rapidjson::Value& res, const std::string& field_name, rapidjson::Value& value,
                rapidjson::Document::AllocatorType& allocator);

void add_string_field(rapidjson::Value& res, const std::string& field_name, const std::string& str,
                      rapidjson::Document::AllocatorType& allocator);

void add_int_field(rapidjson::Value& res, const std::string& field_name, int value,
                   rapidjson::Document::AllocatorType& allocator);

void add_uint_field(rapidjson::Value& res, const std::string& field_name, uint64_t value,
                    rapidjson::Document::AllocatorType& allocator);

void add_bool_field(rapidjson::Value& res, const std::string& field_name, bool value,
                    rapidjson::Document::AllocatorType& allocator);

void add_float_field(rapidjson::Value& res, const std::string& field_name, float value,
                     rapidjson::Document::AllocatorType& allocator);

void add_string_list_field(rapidjson::Value& res, rapidjson::Value& key, const std::list<std::string>& lst,
                           rapidjson::Document::AllocatorType& allocator);

void add_string_list_field(rapidjson::Value& res, const std::string& key, const std::list<std::string>& lst,
                           rapidjson::Document::AllocatorType& allocator);

void add_string_field(rapidjson::Value& res, rapidjson::Value& key, const std::string& str,
                      rapidjson::Document::AllocatorType& allocator);

std::string doc_to_string(const rapidjson::Value& document);

void parse_file(const std::string& file_name, rapidjson::Document& out_doc);

void parse_string(const std::string& json_string, rapidjson::Document& out_doc);

std::string
get_optional_string(const rapidjson::Value& json_node, const std::string& field_name, const std::string& def_value);

bool get_optional_bool(const rapidjson::Value& json_node, const std::string& field_name, bool def_value);

double get_optional_double(const rapidjson::Value& json_node, const std::string& field_name, double def_val);

int64_t get_optional_int(const rapidjson::Value& json_node, const std::string& field_name, int64_t def_value);

uint64_t get_optional_uint(const rapidjson::Value& json_node, const std::string& field_name, uint64_t def_value);

float get_optional_float(const rapidjson::Value& json_node, const std::string& field_name, float def_value);

std::string
get_string(const rapidjson::Value& json_node, const std::string& field_name);

bool get_bool(const rapidjson::Value& json_node, const std::string& field_name);

double get_double(const rapidjson::Value& json_node, const std::string& field_name);

int64_t get_int(const rapidjson::Value& json_node, const std::string& field_name);

uint64_t get_uint(const rapidjson::Value& json_node, const std::string& field_name);

//auto res = get_array<std::string>(val, "field", [](const rapidjson::Value& v) -> std::string { return v.GetString();});
template<class T, class UnaryFunction>
std::list<T> get_array(const rapidjson::Value& json_node, const std::string& field_name, UnaryFunction function) {
  auto member = json_node.FindMember(field_name);
  if (member == json_node.MemberEnd()) {
    throw JsonValueNotFound(field_name);
  }
  if (!member->value.IsArray()) {
    throw JsonValueNotFound(field_name);
  }
  std::list<T> result;
  for (auto it = member->value.Begin(); it != member->value.End(); ++it) {
    result.push_back(function(*it));
  }
  return result;
}

template<class T, class UnaryFunction>
void
add_array(rapidjson::Value& json_node, const std::string& field_name, std::list<T>& values, UnaryFunction function,
          rapidjson::Document::AllocatorType& allocator) {
  rapidjson::Value res;
  res.SetArray();
  for (auto& v : values) {
    function(res, v);
  }
  rapidjson::Value key;
  key.SetString(field_name, allocator);
  json_node.AddMember(key, res, allocator);
}

//auto res = get_array<std::string>(val, "field", [](const rapidjson::Value& v) -> std::string { return v.GetString();});
template<class T, class UnaryFunction>
std::list<T>
get_optional_array(const rapidjson::Value& json_node, const std::string& field_name, UnaryFunction function) {
  std::list<T> result;
  auto member = json_node.FindMember(field_name);
  if (member == json_node.MemberEnd()) {
    return result;
  }
  if (!member->value.IsArray()) {
    throw JsonValueNotFound(field_name);
  }
  for (auto it = member->value.Begin(); it != member->value.End(); ++it) {
    result.push_back(function(*it));
  }
  return result;
}

inline const rapidjson::Value& get_object(const rapidjson::Value& node, const std::string& field_name) {
  const auto& obj = node.FindMember(field_name);
  if (obj == node.MemberEnd()) {
    throw std::runtime_error(field_name + " section not found");
  }
  return obj->value;
}

inline const rapidjson::Value::ConstMemberIterator get_optional_object(const rapidjson::Value& node, const std::string& field_name) {
  const auto obj = node.FindMember(field_name);
  return obj;
}

inline void push_uint(rapidjson::Value& node, uint32_t val, rapidjson::Document::AllocatorType& allocator) {
  node.PushBack(val, allocator);
}

inline void push_object(rapidjson::Value& node, rapidjson::Value& val, rapidjson::Document::AllocatorType& allocator) {
  node.PushBack(val, allocator);
}
}

#endif //JSONHELPER_H
