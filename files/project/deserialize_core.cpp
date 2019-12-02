#include "files/project.h"

#include "files/JsonHelper.h"

namespace dasm {
namespace files {
namespace project {

void Serializer::deserialize_file(const std::string& file_name, core::DisassemblerCore& core) {
  rapidjson::Document doc{};
  auto& allocator = doc.GetAllocator();

  json::parse_file(file_name, doc);

  const auto& descr = doc.FindMember("descr");
  if (descr == doc.MemberEnd()) {
    throw std::runtime_error("descr section not found");
  }

  auto vers = json::get_string(descr->value, "version");
  auto arch = json::get_string(descr->value, "arch");
  auto fname = json::get_string(descr->value, "file_name");

  /*  if (core.entryPoint() != nullptr) {
      json::add_object(info, "entry_point", serializeAddr(*core.entryPoint(), allocator), allocator);
    }
    */
}

}
}
}
