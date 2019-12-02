#include "files/project.h"

#include "files/JsonHelper.h"

#include <list>

namespace dasm {
namespace files {
namespace project {

memory::AddrPtr deserializeAddrPtr(const rapidjson::Value& node) {
  auto segment = json::get_uint(node, "segment");
  auto offset = json::get_uint(node, "offset");
  auto res = std::make_shared<memory::Addr>(offset, segment);
  return res;
}

memory::Addr deserializeAddr(const rapidjson::Value& node) {
  auto segment = json::get_uint(node, "segment");
  auto offset = json::get_uint(node, "offset");
  return memory::Addr(offset, segment);
}

core::Label deserializeLabel(const rapidjson::Value& node) {
  core::Label res;
  res.name = json::get_string(node, "name");
  res.addr = deserializeAddr(json::get_object(node, "addr"));
  return res;
}

std::shared_ptr<postprocessors::AutoCommenter> deserializeAutocommenter(const rapidjson::Value& node) {
  auto ac = json::get_optional_object(node, "auto_commenter");
  if (ac == node.MemberEnd()) {
    return nullptr;
  }
  //auto res = get_array<std::string>(val, "field", [](const rapidjson::Value& v) -> std::string { return v.GetString();});

  auto klj = json::get_array<core::Label>(ac->value, "known_labels", [](const rapidjson::Value& v)->core::Label {
    return deserializeLabel(v);
  });
  bughere
}

void Serializer::deserialize_file(const std::string& file_name, core::DisassemblerCore& core) {
  rapidjson::Document doc{};
  auto& allocator = doc.GetAllocator();

  json::parse_file(file_name, doc);

  const auto& descr = json::get_object(doc, "descr");

  auto vers = json::get_string(descr, "version");
  auto arch = json::get_string(descr, "arch");
  auto fname = json::get_string(descr, "file_name");

  auto entry_point = json::get_optional_object(descr, "entry_point");

  core.clear();
  core.setFileName(fname);
  if (entry_point != descr.MemberEnd()) {
    core.setEntryPoint(deserializeAddrPtr(entry_point->value));
  }

  auto autoc = deserializeAutocommenter(doc);
}

}
}
}
