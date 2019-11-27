//
// Created by aglebov on 3/19/2019.
//

#include "JsonHelper.h"

#include <fstream>
#include <sstream> //std::stringstream

#include "rapidjson/stringbuffer.h"
#include "rapidjson/writer.h"
#include "rapidjson/error/en.h"
#include "rapidjson/error/error.h"

namespace json {

static const char *TAG = "JsonHelper";

void add_string_field(rapidjson::Value &res, const std::string &field_name, const std::string &str,
                      rapidjson::Document::AllocatorType &allocator) {
  rapidjson::Value key(field_name.c_str(), allocator);
  rapidjson::Value value(str.c_str(), allocator);
  res.AddMember(key, value, allocator);
}

void add_int_field(rapidjson::Value &res, const std::string &field_name, int value,
                   rapidjson::Document::AllocatorType &allocator) {
  rapidjson::Value key(field_name.c_str(), allocator);
  rapidjson::Value val;
  val.SetInt(value);
  res.AddMember(key, val, allocator);
}

void add_uint_field(rapidjson::Value &res, const std::string &field_name, uint64_t value,
                    rapidjson::Document::AllocatorType &allocator) {
  rapidjson::Value key(field_name.c_str(), allocator);
  rapidjson::Value val;
  val.SetUint64(value);
  res.AddMember(key, val, allocator);
}

void add_bool_field(rapidjson::Value &res, const std::string &field_name, bool value,
                    rapidjson::Document::AllocatorType &allocator) {
  rapidjson::Value key(field_name.c_str(), allocator);
  rapidjson::Value val;
  val.SetBool(value);
  res.AddMember(key, val, allocator);
}

void add_float_field(rapidjson::Value &res, const std::string &field_name, float value,
                     rapidjson::Document::AllocatorType &allocator) {
  rapidjson::Value key(field_name.c_str(), allocator);
  rapidjson::Value val;
  val.SetFloat(value);
  res.AddMember(key, val, allocator);
}

void add_string_list_field(rapidjson::Value &res, rapidjson::Value &key, const std::list<std::string> &lst,
                           rapidjson::Document::AllocatorType &allocator) {
  rapidjson::Value jlst{};
  jlst.SetArray();
  for (const auto &t: lst) {
    rapidjson::Value jt{};
    jt.SetString(t, allocator);
    jlst.PushBack(jt, allocator);
  }
  res.AddMember(key, jlst, allocator);
}

void add_string_list_field(rapidjson::Value &res, const std::string &field_name, const std::list<std::string> &lst,
                           rapidjson::Document::AllocatorType &allocator) {
  rapidjson::Value key(field_name.c_str(), allocator);
  rapidjson::Value jlst{};
  jlst.SetArray();
  for (const auto &t: lst) {
    rapidjson::Value jt{};
    jt.SetString(t, allocator);
    jlst.PushBack(jt, allocator);
  }
  res.AddMember(key, jlst, allocator);
}

void add_string_field(rapidjson::Value &res, rapidjson::Value &key, const std::string &str,
                      rapidjson::Document::AllocatorType &allocator) {
  rapidjson::Value value(str.c_str(), allocator);
  res.AddMember(key, value, allocator);
}

std::string doc_to_string(const rapidjson::Value &document) {
  rapidjson::StringBuffer sb;
  rapidjson::Writer<rapidjson::StringBuffer> writer(sb);
  document.Accept(writer);    // Accept() traverses the DOM and generates Handler events.
  return sb.GetString();
}

void parse_file(const std::string &file_name, rapidjson::Document &out_doc) {
  std::ifstream config_file(file_name);
  if (config_file.fail() || config_file.bad() || config_file.eof() || !config_file.is_open()) {
    throw std::runtime_error("file not found: " + file_name);
  }

  std::stringstream strStream;
  strStream << config_file.rdbuf(); //read the file
  std::string jc = strStream.str(); //str holds the content of the file
  parse_string(jc, out_doc);
}

void parse_string(const std::string &json_string, rapidjson::Document &out_doc) {
  rapidjson::ParseResult pe = out_doc.Parse(json_string);
  if (!pe) {
    throw std::runtime_error("json parse error: " + std::string(rapidjson::GetParseError_En(pe.Code())) + ", offs: " + std::to_string(pe.Offset()));
  }
}

std::string
get_optional_string(const rapidjson::Value &json_node, const std::string &field_name, const std::string &def_value) {
  auto member = json_node.FindMember(field_name);
  if (member == json_node.MemberEnd()) {
    return def_value;
  }
  return member->value.GetString();
}

int64_t get_optional_int(const rapidjson::Value &json_node, const std::string &field_name, int64_t def_value) {
  auto member = json_node.FindMember(field_name);
  if (member == json_node.MemberEnd()) {
    return def_value;
  }
  return member->value.GetInt64();
}

uint64_t get_optional_uint(const rapidjson::Value &json_node, const std::string &field_name, uint64_t def_value) {
  auto member = json_node.FindMember(field_name);
  if (member == json_node.MemberEnd()) {
    return def_value;
  }
  return member->value.GetUint64();
}

float get_optional_float(const rapidjson::Value &json_node, const std::string &field_name, float def_value) {
  auto member = json_node.FindMember(field_name);
  if (member == json_node.MemberEnd()) {
    return def_value;
  }
  return member->value.GetFloat();
}

std::string get_string(const rapidjson::Value &json_node, const std::string &field_name) {
  auto member = json_node.FindMember(field_name);
  if (member == json_node.MemberEnd()) {
    throw JsonValueNotFound(field_name);
  }
  return member->value.GetString();
}

bool get_bool(const rapidjson::Value &json_node, const std::string &field_name) {
  auto member = json_node.FindMember(field_name);
  if (member == json_node.MemberEnd()) {
    throw JsonValueNotFound(field_name);
  }
  return member->value.GetBool();
}

double get_double(const rapidjson::Value &json_node, const std::string &field_name) {
  auto member = json_node.FindMember(field_name);
  if (member == json_node.MemberEnd()) {
    throw JsonValueNotFound(field_name);
  }
  return member->value.GetDouble();
}

bool get_optional_bool(const rapidjson::Value &json_node, const std::string &field_name, bool def_val) {
  auto member = json_node.FindMember(field_name);
  if (member == json_node.MemberEnd()) {
    return def_val;
  }
  return member->value.GetBool();
}

double get_optional_double(const rapidjson::Value &json_node, const std::string &field_name, double def_val) {
  auto member = json_node.FindMember(field_name);
  if (member == json_node.MemberEnd()) {
    return def_val;
  }
  return member->value.GetDouble();
}

int64_t get_int(const rapidjson::Value &json_node, const std::string &field_name) {
  auto member = json_node.FindMember(field_name);
  if (member == json_node.MemberEnd()) {
    throw JsonValueNotFound(field_name);
  }
  return member->value.GetInt64();
}

uint64_t get_uint(const rapidjson::Value &json_node, const std::string &field_name) {
  auto member = json_node.FindMember(field_name);
  if (member == json_node.MemberEnd()) {
    throw JsonValueNotFound(field_name);
  }
  return member->value.GetUint64();
}
}
