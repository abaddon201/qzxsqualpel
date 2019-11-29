#include "files/project.h"

#include "files/JsonHelper.h"

namespace dasm {
namespace files {
namespace project {

rapidjson::Value serializeAddr(memory::Addr& addr, rapidjson::Document::AllocatorType& allocator) {
  rapidjson::Value v{};
  v.SetObject();

  json::add_uint_field(v, "segment", addr.segment(), allocator);
  json::add_uint_field(v, "offset", addr.offset(), allocator);
  return v;
}

rapidjson::Value  serializeLabel(core::LabelPtr lbl, rapidjson::Document::AllocatorType& allocator) {
  rapidjson::Value v{};
  v.SetObject();

  v.AddMember("addr", serializeAddr(lbl->addr, allocator), allocator);
  json::add_string_field(v, "name", lbl->name, allocator);
  return v;
}

rapidjson::Value serializeKnownLabel(std::pair < memory::Addr, core::LabelPtr> lbl, rapidjson::Document::AllocatorType& allocator) {
  auto jvl = serializeLabel(lbl.second, allocator);
  return jvl;
}

void serializeAutocommenter(const core::DisassemblerCore& core, rapidjson::Document& doc) {
  if (core.getAutocommenter() == nullptr || core.getAutocommenter()->getKnownLabels().empty()) {
    return;
  }
  auto& allocator = doc.GetAllocator();

  rapidjson::Value ac{};
  ac.SetObject();

  rapidjson::Value klsj{};
  klsj.SetObject();

  for (const auto& kl : core.getAutocommenter()->getKnownLabels()) {
    rapidjson::Value klj = serializeKnownLabel(kl, allocator);
    klsj.AddMember("label", klj, allocator);
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
  return v;
}


void serializeMemory(const core::DisassemblerCore& core, rapidjson::Document& doc) {
  auto& allocator = doc.GetAllocator();

  rapidjson::Value mem{};
  mem.SetObject();

  rapidjson::Value cont{};
  cont.SetObject();

  for (const auto &seg : core.getMemory().getSegments()) {
    rapidjson::Value klj = serializeSegment(seg.second, allocator);
    cont.AddMember("segment", klj, allocator);
  }
  mem.AddMember("segments", cont, allocator);

  doc.AddMember("memory", mem, allocator);
}

std::string Serializer::serialize(const core::DisassemblerCore& core) {
  rapidjson::Document doc{};
  auto& allocator = doc.GetAllocator();
  doc.SetObject();

  rapidjson::Value info{};
  info.SetObject();

  json::add_string_field(info, "version", "0.2", allocator);
  json::add_string_field(info, "arch", "z80", allocator);
  json::add_string_field(info, "file_name", core.getFileName(), allocator);
  if (core.getEntryPoint() != nullptr) {
    json::add_string_field(info, "entry_point", core.getEntryPoint()->toString(), allocator);
  }

  doc.AddMember("descr", info, allocator);

  serializeAutocommenter(core, doc);
  serializeMemory(core, doc);
  return json::doc_to_string(doc);
}

}
}
}
