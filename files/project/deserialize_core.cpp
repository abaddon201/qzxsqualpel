#include "files/project.h"

#include "files/JsonHelper.h"

namespace dasm {
namespace files {
namespace project {

void deserialize(const std::string& src, core::DisassemblerCore& core) {
  rapidjson::Document doc{};
  auto& allocator = doc.GetAllocator();

  json::parse_string(src, doc);

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
