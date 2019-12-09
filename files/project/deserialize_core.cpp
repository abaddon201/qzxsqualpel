#include "files/project.h"

#include "files/JsonHelper.h"

#include "core/navigation_stack.h"

#include <list>

namespace dasm {
namespace files {
namespace project {

core::LabelPtr deserializeLabel(const rapidjson::Value& node) {
  auto name = json::get_string(node, "name");
  auto addr = json::get_uint(node, "addr");
  auto& lbls = core::DisassemblerCore::inst().labels();
  if (lbls.find(addr) == lbls.end()) {
    core::LabelPtr res = std::make_shared<core::Label>();
    res->name = name;
    res->addr = addr;
    lbls[addr] = res;
    return res;
  } else {
    return lbls[addr];
  }
}

std::shared_ptr<postprocessors::AutoCommenter> deserializeAutocommenter(const rapidjson::Value& node) {
  auto ac = json::get_optional_object(node, "auto_commenter");
  if (ac == node.MemberEnd()) {
    return nullptr;
  }

  auto klj = json::get_array<core::LabelPtr>(ac->value, "known_labels", [](const rapidjson::Value& v)->core::LabelPtr {
    return deserializeLabel(v);
  });
  auto acomm = std::make_shared<postprocessors::AutoCommenter>();
  auto& labels = acomm->knownLabels();
  for (auto& lbl : klj) {
    labels[lbl->addr] = lbl;
  }
  return acomm;
}

memory::Segment::Type segmentTypeFromString(const std::string& t) {
  if (t == "RAW") {
    return memory::Segment::Type::RAW;
  } else if (t == "CODE") {
    return memory::Segment::Type::CODE;
  } else if (t == "DATA") {
    return memory::Segment::Type::DATA;
  }
  throw std::runtime_error("unknown segment id: " + t);
}

memory::SegmentPtr deserializeSegment(const rapidjson::Value& node) {
  auto seg = std::make_shared<memory::Segment>();
  seg->setDataSize(json::get_uint(node, "data_size"));
  seg->setSize(json::get_uint(node, "size"));
  seg->setId(json::get_uint(node, "id"));
  seg->setType(segmentTypeFromString(json::get_string(node, "type")));

  auto arr = json::get_array<uint8_t>(node, "bytes", [](const rapidjson::Value& v)->uint8_t {
    return v.GetUint();
  });
  auto& bytes = seg->bytes();
  for (uint8_t b : arr) {
    bytes.emplace_back(b);
  }
  return seg;
}

void deserializeMemory(const rapidjson::Value& node, core::DisassemblerCore& core) {
  auto& parent = json::get_object(node, "memory");

  auto arr = json::get_array<memory::SegmentPtr>(parent, "segments", [](const rapidjson::Value& v)->memory::SegmentPtr {
    return deserializeSegment(v);
  });

  for (auto& s : arr) {
    core.memory().placeSegment(s);
  }
}

memory::Reference::Type referenceTypeFromString(const std::string& type) {
  if (type == "UNKNOWN") {
    return memory::Reference::Type::UNKNOWN;
  } else if (type == "JUMP") {
    return memory::Reference::Type::JUMP;
  } else if (type == "CALL") {
    return memory::Reference::Type::CALL;
  } else if (type == "READ_BYTE") {
    return memory::Reference::Type::READ_BYTE;
  } else if (type == "WRITE_BYTE") {
    return memory::Reference::Type::WRITE_BYTE;
  } else if (type == "READ_WORD") {
    return memory::Reference::Type::READ_WORD;
  } else if (type == "WRITE_WORD") {
    return memory::Reference::Type::WRITE_WORD;
  }
  throw std::runtime_error("unknown reference type: " + type);
}

memory::ReferencePtr deserializeReference(const rapidjson::Value& node) {
  memory::ReferencePtr ref = std::make_shared<memory::Reference>();
  ref->addr = json::get_uint(node, "addr");
  ref->type = referenceTypeFromString(json::get_string(node, "type"));
  return ref;
}

core::ArgType argumentTypeFromString(const std::string& type) {
  if (type == "DEFAULT") {
    return core::ArgType::ARG_DEFAULT;
  } else if (type == "FLAG") {
    return core::ArgType::ARG_FLAG;
  } else if (type == "REGISTER8") {
    return core::ArgType::ARG_REGISTER8;
  } else if (type == "REGISTER16") {
    return core::ArgType::ARG_REGISTER16;
  } else if (type == "REGISTER_OFFSET") {
    return core::ArgType::ARG_REGISTER_OFFSET;
  } else if (type == "REGISTER_REF") {
    return core::ArgType::ARG_REGISTER_REF;
  } else if (type == "MEMORY_REF") {
    return core::ArgType::ARG_MEMORY_REF;
  } else if (type == "PORT") {
    return core::ArgType::ARG_PORT;
  } else if (type == "BYTE_ARRAY") {
    return core::ArgType::ARG_BYTE_ARRAY;
  }
  throw std::runtime_error("Unknown argument type: " + type);
}

core::ArgSize argumentSizeFromString(const std::string& size) {
  if (size == "FLAG") {
    return core::ArgSize::Flag;
  } else if (size == "BYTE") {
    return core::ArgSize::Byte;
  } else if (size == "WORD") {
    return core::ArgSize::Word;
  }
  throw std::runtime_error("Unknown argument size: " + size);
}


core::ArgPtr deserializeArgument(const rapidjson::Value& node) {
  core::ArgPtr arg;
  auto atype = argumentTypeFromString(json::get_string(node, "type"));
  auto asize = argumentSizeFromString(json::get_string(node, "size"));

  switch (atype) {
    case core::ArgType::ARG_DEFAULT:
      arg = std::make_shared<core::ArgDefault>();
      core::argConvert<core::ArgDefault>(arg)->setIsHex(json::get_bool(node, "is_hex"));
      core::argConvert<core::ArgDefault>(arg)->setValue(json::get_uint(node, "value"));
      break;
    case core::ArgType::ARG_FLAG:
      arg = std::make_shared<core::ArgFlag>();
      core::argConvert<core::ArgFlag>(arg)->flag = core::Flag::getFlag(json::get_string(node, "flag"));
      break;
    case core::ArgType::ARG_REGISTER8:
      arg = std::make_shared<core::ArgRegister8>();
      core::argConvert<core::ArgRegister8>(arg)->reg_id = core::Register8::getRegister(json::get_string(node, "reg8"));
      break;
    case core::ArgType::ARG_REGISTER16:
      arg = std::make_shared<core::ArgRegister16>();
      core::argConvert<core::ArgRegister16>(arg)->reg_id = core::Register16::getRegister(json::get_string(node, "reg16"));
      break;
    case core::ArgType::ARG_REGISTER_OFFSET:
      arg = std::make_shared<core::ArgRegisterOffset>();
      core::argConvert<core::ArgRegisterOffset>(arg)->reg_id = core::Register16::getRegister(json::get_string(node, "reg16"));
      core::argConvert<core::ArgRegisterOffset>(arg)->is_positive = json::get_bool(node, "is_positive");
      core::argConvert<core::ArgRegisterOffset>(arg)->offset = json::get_uint(node, "offset");
      break;
    case core::ArgType::ARG_REGISTER_REF:
      arg = std::make_shared<core::ArgRegisterReference>();
      core::argConvert<core::ArgRegisterReference>(arg)->reg_id = core::Register16::getRegister(json::get_string(node, "reg16"));
      break;
    case core::ArgType::ARG_MEMORY_REF: {
      arg = std::make_shared<core::ArgMemoryReference>();
      core::argConvert<core::ArgMemoryReference>(arg)->isReference = json::get_bool(node, "is_ref");
      core::argConvert<core::ArgMemoryReference>(arg)->addr = json::get_uint(node, "addr");
      auto& lbl = json::get_optional_object(node, "label");
      if (lbl != node.MemberEnd()) {
        core::argConvert<core::ArgMemoryReference>(arg)->label = deserializeLabel(lbl->value);
      }
      break;
    }
    case core::ArgType::ARG_PORT:
      arg = std::make_shared<core::ArgPort>();
      core::argConvert<core::ArgPort>(arg)->setPort(json::get_uint(node, "port"));
      break;
    case core::ArgType::ARG_BYTE_ARRAY: {
      arg = std::make_shared<core::ArgByteArray>();
      //FIXME: unneeded copy of bytes
      json::get_array<uint8_t>(node, "bytes", [&](const rapidjson::Value& v)->uint8_t {
        auto b = (uint8_t)v.GetUint();
        core::argConvert<core::ArgByteArray>(arg)->pushByte(b);
        return b;
      });
      break;
    }
    default:
      throw std::runtime_error("Unknown argument type: " + std::to_string((int)arg->arg_type));
  }
  arg->size = asize;
  return arg;
}

core::CommandPtr deserializeCommand(const rapidjson::Value& node) {
  core::CommandPtr cmd = std::make_shared<core::Command>();
  cmd->command_code = core::CmdCode(json::get_string(node, "code"));
  cmd->setComment(json::get_optional_string(node, "comment", ""));
  cmd->setAutoComment(json::get_optional_string(node, "auto_comment", ""));
  cmd->setBlockComment(json::get_optional_string(node, "block_comment", ""));
  cmd->addr = json::get_uint(node, "addr");
  cmd->len = json::get_uint(node, "len");

  auto lblj = json::get_optional_object(node, "label");
  if (lblj != node.MemberEnd()) {
    cmd->setLabel(deserializeLabel(lblj->value));
  }
  auto arr = json::get_optional_array<core::ArgPtr>(node, "arguments", [](const rapidjson::Value& v)->core::ArgPtr {
    return deserializeArgument(v);
  });

  for (auto& a : arr) {
    cmd->args().emplace_back(a);
  }

  auto arr2 = json::get_optional_array<memory::ReferencePtr>(node, "refs", [](const rapidjson::Value& v)->memory::ReferencePtr {
    return deserializeReference(v);
  });

  for (auto& a : arr2) {
    cmd->references().emplace_back(a);
  }
  return cmd;
}

void deserializeLabels(const rapidjson::Value& node, core::DisassemblerCore& core) {
  auto arr = json::get_array<core::LabelPtr>(node, "labels", [](const rapidjson::Value& v)->core::LabelPtr {
    return deserializeLabel(v);
  });

  for (auto& s : arr) {
    core.labels()[s->addr] = s;
  }
}

void deserializeCommands(const rapidjson::Value& node, core::DisassemblerCore& core) {
  size_t sz = 0;
  auto arr = json::get_array<core::CommandPtr>(node, "commands", [&](const rapidjson::Value& v)->core::CommandPtr {
    auto cmd = deserializeCommand(v);
    sz += cmd->len;
    return cmd;
  });

  core.commands().reset(sz);
  for (auto& s : arr) {
    core.commands().put(s->addr, s->len, s);
  }
}

void deserializeNavigationStack(const rapidjson::Value& node, core::DisassemblerCore& core) {
  size_t sz = 0;
  auto arr = json::get_optional_array<core::NavigationStack::StackItem>(node, "navigation_stack", [&](const rapidjson::Value& v)->core::NavigationStack::StackItem {
    return v.GetUint();
  });

  for (auto& s : arr) {
    core::NavigationStack::inst().push(s);
  }
}

void Serializer::deserialize_file(const std::string& file_name, core::DisassemblerCore& core) {
  rapidjson::Document doc{};
  auto& allocator = doc.GetAllocator();

  json::parse_file(file_name, doc);

  const auto& descr = json::get_object(doc, "descr");

  auto vers = json::get_string(descr, "version");
  auto arch = json::get_string(descr, "arch");
  auto fname = json::get_string(descr, "file_name");

  auto entry_point = json::get_optional_uint(descr, "entry_point", std::numeric_limits<uint32_t>::max());
  core.clear();
  core.setFileName(fname);
  core.setEntryPoint(entry_point);

  auto autoc = deserializeAutocommenter(doc);
  core.setAutoCommenter(autoc);

  deserializeNavigationStack(doc, core);

  deserializeMemory(doc, core);
  deserializeLabels(doc, core);
  deserializeCommands(doc, core);
  core.setCurrentPosition(json::get_int(doc, "current_position"));
}

}
}
}
