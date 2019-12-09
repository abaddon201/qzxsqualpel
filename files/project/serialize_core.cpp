#include "files/project.h"

#include "files/JsonHelper.h"
#include "core/navigation_stack.h"

namespace dasm {
namespace files {
namespace project {

rapidjson::Value  serializeLabel(core::LabelPtr lbl, rapidjson::Document::AllocatorType& allocator) {
  rapidjson::Value v{};
  v.SetObject();

  json::add_uint_field(v, "addr", lbl->addr, allocator);
  json::add_string_field(v, "name", lbl->name, allocator);
  return v;
}

rapidjson::Value serializeKnownLabel(std::pair < uint16_t, core::LabelPtr> lbl, rapidjson::Document::AllocatorType& allocator) {
  auto jvl = serializeLabel(lbl.second, allocator);
  return jvl;
}

void serializeAutocommenter(const core::DisassemblerCore& core, rapidjson::Document& doc) {
  if (core.autoCommenter() == nullptr || core.autoCommenter()->getKnownLabels().empty()) {
    return;
  }
  auto& allocator = doc.GetAllocator();

  rapidjson::Value ac{};
  ac.SetObject();

  rapidjson::Value klsj{};
  klsj.SetArray();

  for (const auto& kl : core.autoCommenter()->getKnownLabels()) {
    json::push_object(klsj, serializeKnownLabel(kl, allocator), allocator);
  }

  ac.AddMember("known_labels", klsj, allocator);

  doc.AddMember("auto_commenter", ac, allocator);
}

std::string segmentTypeToString(memory::Segment::Type type) {
  switch (type) {
    case memory::Segment::Type::RAW:
      return "RAW";
    case memory::Segment::Type::CODE:
      return "CODE";
    case memory::Segment::Type::DATA:
      return "DATA";
  }
  throw std::runtime_error("unknown segment id: " + std::to_string((int)type));
}
rapidjson::Value serializeSegment(const memory::SegmentPtr& seg, rapidjson::Document::AllocatorType& allocator) {
  rapidjson::Value v{};
  v.SetObject();

  json::add_uint_field(v, "data_size", seg->dataSize(), allocator);
  json::add_uint_field(v, "size", seg->size(), allocator);
  json::add_uint_field(v, "id", seg->id(), allocator);
  json::add_string_field(v, "type", segmentTypeToString(seg->type()), allocator);

  rapidjson::Value bj{};
  bj.SetArray();
  for (const auto& b : seg->bytes()) {
    json::push_uint(bj, (uint8_t)b, allocator);
  }
  v.AddMember("bytes", bj, allocator);
  return v;
}


void serializeMemory(const core::DisassemblerCore& core, rapidjson::Document& doc) {
  auto& allocator = doc.GetAllocator();

  rapidjson::Value mem{};
  mem.SetObject();

  rapidjson::Value cont{};
  cont.SetArray();

  for (const auto& seg : core.memory().segments()) {
    json::push_object(cont, serializeSegment(seg.second, allocator), allocator);
  }
  mem.AddMember("segments", cont, allocator);

  doc.AddMember("memory", mem, allocator);
}

std::string referenceTypeToString(const memory::Reference::Type type) {
  switch (type) {
    case memory::Reference::Type::UNKNOWN: return "UNKNOWN";
    case memory::Reference::Type::JUMP: return "JUMP";
    case memory::Reference::Type::CALL: return "CALL";
    case memory::Reference::Type::READ_BYTE: return "READ_BYTE";
    case memory::Reference::Type::WRITE_BYTE: return "WRITE_BYTE";
    case memory::Reference::Type::READ_WORD:return "READ_WORD";
    case memory::Reference::Type::WRITE_WORD: return "WRITE_WORD";
  }
  throw std::runtime_error("unknown reference type: " + std::to_string((int)type));
}

rapidjson::Value serializeReference(const memory::ReferencePtr ref, rapidjson::Document::AllocatorType& allocator) {
  rapidjson::Value v{};
  v.SetObject();

  json::add_uint_field(v, "addr", ref->addr, allocator);
  json::add_string_field(v, "type", referenceTypeToString(ref->type), allocator);
  return v;
}

std::string argumentTypeToString(core::ArgType type) {
  switch (type) {
    case core::ArgType::ARG_DEFAULT: return "DEFAULT";
    case core::ArgType::ARG_FLAG: return "FLAG";
    case core::ArgType::ARG_REGISTER8: return "REGISTER8";
    case core::ArgType::ARG_REGISTER16: return "REGISTER16";
    case core::ArgType::ARG_REGISTER_OFFSET: return "REGISTER_OFFSET";
    case core::ArgType::ARG_REGISTER_REF: return "REGISTER_REF";
    case core::ArgType::ARG_MEMORY_REF: return "MEMORY_REF";
    case core::ArgType::ARG_PORT: return "PORT";
    case core::ArgType::ARG_BYTE_ARRAY: return "BYTE_ARRAY";
  }
  throw std::runtime_error("Unknown argument type: " + std::to_string((int)type));
}

std::string argumentSizeToString(core::ArgSize size) {
  switch (size) {
    case core::ArgSize::Flag: return "FLAG";
    case core::ArgSize::Byte: return "BYTE";
    case core::ArgSize::Word: return "WORD";
  }
  throw std::runtime_error("Unknown argument size: " + std::to_string((int)size));
}

rapidjson::Value serializeArgument(const core::ArgPtr arg, rapidjson::Document::AllocatorType& allocator) {
  rapidjson::Value v{};
  v.SetObject();

  json::add_string_field(v, "type", argumentTypeToString(arg->arg_type), allocator);
  json::add_string_field(v, "size", argumentSizeToString(arg->size), allocator);

  switch (arg->arg_type) {
    case core::ArgType::ARG_DEFAULT:
      json::add_bool_field(v, "is_hex", core::argConvert<core::ArgDefault>(arg)->is_hex(), allocator);
      json::add_uint_field(v, "value", core::argConvert<core::ArgDefault>(arg)->value(), allocator);
      break;
    case core::ArgType::ARG_FLAG:
      json::add_string_field(v, "flag", core::argConvert<core::ArgFlag>(arg)->flag.toString(), allocator);
      break;
    case core::ArgType::ARG_REGISTER8:
      json::add_string_field(v, "reg8", core::argConvert<core::ArgRegister8>(arg)->reg_id.toString(), allocator);
      break;
    case core::ArgType::ARG_REGISTER16:
      json::add_string_field(v, "reg16", core::argConvert<core::ArgRegister16>(arg)->reg_id.toString(), allocator);
      break;
    case core::ArgType::ARG_REGISTER_OFFSET:
      json::add_string_field(v, "reg16", core::argConvert<core::ArgRegisterOffset>(arg)->reg_id.toString(), allocator);
      json::add_bool_field(v, "is_positive", core::argConvert<core::ArgRegisterOffset>(arg)->is_positive, allocator);
      json::add_uint_field(v, "offset", core::argConvert<core::ArgRegisterOffset>(arg)->offset, allocator);
      break;
    case core::ArgType::ARG_REGISTER_REF:
      json::add_string_field(v, "reg16", core::argConvert<core::ArgRegisterReference>(arg)->reg_id.toString(), allocator);
      break;
    case core::ArgType::ARG_MEMORY_REF:
      json::add_bool_field(v, "is_ref", core::argConvert<core::ArgMemoryReference>(arg)->isReference, allocator);
      json::add_uint_field(v, "addr", core::argConvert<core::ArgMemoryReference>(arg)->addr, allocator);
      if (core::argConvert<core::ArgMemoryReference>(arg)->label != nullptr) {
        json::add_object(v, "label", serializeLabel(core::argConvert<core::ArgMemoryReference>(arg)->label, allocator), allocator);
      }
      break;
    case core::ArgType::ARG_PORT:
      json::add_uint_field(v, "port", core::argConvert<core::ArgPort>(arg)->value(), allocator);
      break;
    case core::ArgType::ARG_BYTE_ARRAY: {
      rapidjson::Value args{};
      args.SetArray();
      for (const auto& b : core::argConvert<core::ArgByteArray>(arg)->bytes()) {
        json::push_uint(args, (uint8_t)b, allocator);
      }
      json::add_object(v, "bytes", args, allocator);
      break;
    }
    default:
      throw std::runtime_error("Unknown argument type: " + std::to_string((int)arg->arg_type));
  }
  return v;
}

rapidjson::Value serializeCommand(const core::CommandPtr cmd, rapidjson::Document::AllocatorType& allocator) {
  rapidjson::Value v{};
  v.SetObject();

  json::add_string_field(v, "code", cmd->command_code.toString(), allocator);
  if (!cmd->comment.empty()) {
    json::add_string_field(v, "comment", cmd->comment, allocator);
  }
  if (!cmd->blockComment().empty()) {
    json::add_string_field(v, "block_comment", cmd->blockComment(), allocator);
  }
  if (!cmd->auto_comment.empty()) {
    json::add_string_field(v, "auto_comment", cmd->auto_comment, allocator);
  }
  if (cmd->label() != nullptr) {
    json::add_object(v, "label", serializeLabel(cmd->label(), allocator), allocator);
  }
  if (!cmd->references().empty()) {
    rapidjson::Value args{};
    args.SetArray();
    for (const auto& arg : cmd->references()) {
      json::push_object(args, serializeReference(arg, allocator), allocator);
    }
    json::add_object(v, "refs", args, allocator);

  }
  json::add_uint_field(v, "addr", cmd->addr, allocator);
  json::add_uint_field(v, "len", cmd->len, allocator);

  if (cmd->getArgsCount() != 0) {
    rapidjson::Value args{};
    args.SetArray();
    for (const auto& arg : cmd->args()) {
      json::push_object(args, serializeArgument(arg, allocator), allocator);
    }
    json::add_object(v, "arguments", args, allocator);

  }
  return v;
}

void serializeLabels(const core::DisassemblerCore& core, rapidjson::Document& doc) {
  auto& allocator = doc.GetAllocator();

  rapidjson::Value cont{};
  cont.SetArray();

  for (const auto& lbl : core.labels()) {
    json::push_object(cont, serializeLabel(lbl.second, allocator), allocator);
  }
  doc.AddMember("labels", cont, allocator);
}

void serializeCommands(const core::DisassemblerCore& core, rapidjson::Document& doc) {
  auto& allocator = doc.GetAllocator();

  rapidjson::Value cont{};
  cont.SetArray();

  for (const auto& elem : core.commands().whole()) {
    if (elem.offset() == 0) {
      json::push_object(cont, serializeCommand(elem.elem(), allocator), allocator);
    }
  }
  doc.AddMember("commands", cont, allocator);
}

void serializeNavigationStack(const core::DisassemblerCore& core, rapidjson::Document& doc) {
  if (!dasm::core::NavigationStack::inst().hasAddr()) {
    return;
  }
  auto& allocator = doc.GetAllocator();

  rapidjson::Value cont{};
  cont.SetArray();

  for (const auto& elem : dasm::core::NavigationStack::inst().items()) {
      json::push_uint(cont, elem, allocator);
  }
  doc.AddMember("navigation_stack", cont, allocator);
}

std::string Serializer::serialize(const core::DisassemblerCore& core) {
  rapidjson::Document doc{};
  auto& allocator = doc.GetAllocator();
  doc.SetObject();

  rapidjson::Value info{};
  info.SetObject();

  json::add_string_field(info, "version", "0.2", allocator);
  json::add_string_field(info, "arch", "z80", allocator);
  json::add_string_field(info, "file_name", core.fileName(), allocator);

  json::add_int_field(doc, "current_position", core.currentPosition(), allocator);

  if (core.entryPointExists()) {
    json::add_uint_field(info, "entry_point", core.entryPoint(), allocator);
  }
  doc.AddMember("descr", info, allocator);

  serializeAutocommenter(core, doc);
  serializeNavigationStack(core, doc);
  serializeCommands(core, doc);
  serializeMemory(core, doc);
  serializeLabels(core, doc);
  return json::doc_to_string(doc);
}

}
}
}
