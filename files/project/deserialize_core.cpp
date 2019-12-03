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

core::LabelPtr deserializeLabel(const rapidjson::Value& node) {
  core::LabelPtr res = std::make_shared<core::Label>();
  res->name = json::get_string(node, "name");
  res->addr = deserializeAddr(json::get_object(node, "addr"));
  return res;
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
    bytes.emplace_back(core::Byte(b));
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

core::Chunk::Type chunkTypeFromString(const std::string& type) {
  if (type == "UNKNOWN") {
    return core::Chunk::Type::UNKNOWN;
  } else if (type == "UNPARSED") {
    return core::Chunk::Type::UNPARSED;
  } else if (type == "CODE") {
    return core::Chunk::Type::CODE;
  } else if (type == "DATA_BYTE") {
    return core::Chunk::Type::DATA_BYTE;
  } else if (type == "DATA_WORD") {
    return core::Chunk::Type::DATA_WORD;
  } else if (type == "DATA_BYTE_ARRAY") {
    return core::Chunk::Type::DATA_BYTE_ARRAY;
  } else if (type == "DATA_WORD_ARRAY") {
    return core::Chunk::Type::DATA_WORD_ARRAY;
  }
  throw std::runtime_error("Wrong chunk type: " + type);
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

memory::Reference deserializeReference(const rapidjson::Value& node) {
  memory::Reference ref{};
  ref.addr = deserializeAddr(json::get_object(node, "addr"));
  ref.type = referenceTypeFromString(json::get_string(node, "type"));
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
    default:
      throw std::runtime_error("Unknown argument type: " + std::to_string((int)arg->arg_type));
  }
  return arg;
}

core::Command deserializeCommand(const rapidjson::Value& node) {
  core::Command cmd{};
  cmd.command_code = core::CmdCode(json::get_string(node, "code"));
  cmd.setComment(json::get_optional_string(node, "comment", ""));
  cmd.setAutoComment(json::get_optional_string(node, "auto_comment", ""));
  cmd.addr = deserializeAddr(json::get_object(node, "addr"));
  cmd.len = json::get_uint(node, "len");

  auto arr = json::get_optional_array<core::ArgPtr>(node, "arguments", [](const rapidjson::Value& v)->core::ArgPtr {
    return deserializeArgument(v);
  });

  for (auto& a : arr) {
    cmd.args().emplace_back(a);
  }
  return cmd;
}

core::ChunkPtr deserializeChunk(const rapidjson::Value& node) {
  auto chunk = std::make_shared<core::Chunk>();
  auto comment = json::get_optional_string(node, "comment", "");
  if (!comment.empty()) {
    chunk->setComment(comment);
  }
  auto lblj = json::get_optional_object(node, "label");
  if (lblj != node.MemberEnd()) {
    chunk->setLabel(deserializeLabel(lblj->value));
  }
  chunk->setLength(json::get_uint(node, "length"));
  chunk->setType(chunkTypeFromString(json::get_string(node, "type")));
  auto& saj = json::get_object(node, "start_addr");
  chunk->setStartAddr(deserializeAddr(saj));
  auto& laj = json::get_object(node, "last_addr");
  chunk->setLastAddr(deserializeAddr(laj));
  //References
  {
    auto arr = json::get_optional_array<memory::Reference>(node, "references", [](const rapidjson::Value& v)->memory::Reference {
      return deserializeReference(v);
    });

    for (auto& r : arr) {
      chunk->references().emplace_back(r);
    }
  }
  //commands
  {
    auto arr = json::get_optional_array<core::Command>(node, "commands", [](const rapidjson::Value& v)->core::Command {
      return deserializeCommand(v);
    });
    for (auto& r : arr) {
      chunk->commands().emplace_back(r);
    }
  }
  return chunk;
}

void deserializeChunks(const rapidjson::Value& node, core::DisassemblerCore& core) {
  //auto& parent = json::get_object(node, "chunks");

  auto arr = json::get_array<core::ChunkPtr>(node, "chunks", [](const rapidjson::Value& v)->core::ChunkPtr {
    return deserializeChunk(v);
  });

  for (auto& s : arr) {
    core.chunks().chunks()[s->addr()] = s;
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

  auto entry_point = json::get_optional_object(descr, "entry_point");

  core.clear();
  core.setFileName(fname);
  if (entry_point != descr.MemberEnd()) {
    core.setEntryPoint(deserializeAddrPtr(entry_point->value));
  }

  auto autoc = deserializeAutocommenter(doc);
  core.setAutoCommenter(autoc);

  deserializeMemory(doc, core);
  deserializeChunks(doc, core);
}

}
}
}
