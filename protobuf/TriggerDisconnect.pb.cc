// Generated by the protocol buffer compiler.  DO NOT EDIT!
// source: TriggerDisconnect.proto

#include "TriggerDisconnect.pb.h"

#include <algorithm>

#include <google/protobuf/io/coded_stream.h>
#include <google/protobuf/extension_set.h>
#include <google/protobuf/wire_format_lite.h>
#include <google/protobuf/descriptor.h>
#include <google/protobuf/generated_message_reflection.h>
#include <google/protobuf/reflection_ops.h>
#include <google/protobuf/wire_format.h>
// @@protoc_insertion_point(includes)
#include <google/protobuf/port_def.inc>

PROTOBUF_PRAGMA_INIT_SEG
namespace xpilot {
constexpr TriggerDisconnect::TriggerDisconnect(
  ::PROTOBUF_NAMESPACE_ID::internal::ConstantInitialized)
  : reason_(&::PROTOBUF_NAMESPACE_ID::internal::fixed_address_empty_string){}
struct TriggerDisconnectDefaultTypeInternal {
  constexpr TriggerDisconnectDefaultTypeInternal()
    : _instance(::PROTOBUF_NAMESPACE_ID::internal::ConstantInitialized{}) {}
  ~TriggerDisconnectDefaultTypeInternal() {}
  union {
    TriggerDisconnect _instance;
  };
};
PROTOBUF_ATTRIBUTE_NO_DESTROY PROTOBUF_CONSTINIT TriggerDisconnectDefaultTypeInternal _TriggerDisconnect_default_instance_;
}  // namespace xpilot
static ::PROTOBUF_NAMESPACE_ID::Metadata file_level_metadata_TriggerDisconnect_2eproto[1];
static constexpr ::PROTOBUF_NAMESPACE_ID::EnumDescriptor const** file_level_enum_descriptors_TriggerDisconnect_2eproto = nullptr;
static constexpr ::PROTOBUF_NAMESPACE_ID::ServiceDescriptor const** file_level_service_descriptors_TriggerDisconnect_2eproto = nullptr;

const ::PROTOBUF_NAMESPACE_ID::uint32 TableStruct_TriggerDisconnect_2eproto::offsets[] PROTOBUF_SECTION_VARIABLE(protodesc_cold) = {
  PROTOBUF_FIELD_OFFSET(::xpilot::TriggerDisconnect, _has_bits_),
  PROTOBUF_FIELD_OFFSET(::xpilot::TriggerDisconnect, _internal_metadata_),
  ~0u,  // no _extensions_
  ~0u,  // no _oneof_case_
  ~0u,  // no _weak_field_map_
  PROTOBUF_FIELD_OFFSET(::xpilot::TriggerDisconnect, reason_),
  0,
};
static const ::PROTOBUF_NAMESPACE_ID::internal::MigrationSchema schemas[] PROTOBUF_SECTION_VARIABLE(protodesc_cold) = {
  { 0, 6, sizeof(::xpilot::TriggerDisconnect)},
};

static ::PROTOBUF_NAMESPACE_ID::Message const * const file_default_instances[] = {
  reinterpret_cast<const ::PROTOBUF_NAMESPACE_ID::Message*>(&::xpilot::_TriggerDisconnect_default_instance_),
};

const char descriptor_table_protodef_TriggerDisconnect_2eproto[] PROTOBUF_SECTION_VARIABLE(protodesc_cold) =
  "\n\027TriggerDisconnect.proto\022\006xpilot\"3\n\021Tri"
  "ggerDisconnect\022\023\n\006reason\030\001 \001(\tH\000\210\001\001B\t\n\007_"
  "reasonB\031\252\002\026Vatsim.Xpilot.Protobufb\006proto"
  "3"
  ;
static ::PROTOBUF_NAMESPACE_ID::internal::once_flag descriptor_table_TriggerDisconnect_2eproto_once;
const ::PROTOBUF_NAMESPACE_ID::internal::DescriptorTable descriptor_table_TriggerDisconnect_2eproto = {
  false, false, 121, descriptor_table_protodef_TriggerDisconnect_2eproto, "TriggerDisconnect.proto", 
  &descriptor_table_TriggerDisconnect_2eproto_once, nullptr, 0, 1,
  schemas, file_default_instances, TableStruct_TriggerDisconnect_2eproto::offsets,
  file_level_metadata_TriggerDisconnect_2eproto, file_level_enum_descriptors_TriggerDisconnect_2eproto, file_level_service_descriptors_TriggerDisconnect_2eproto,
};
PROTOBUF_ATTRIBUTE_WEAK ::PROTOBUF_NAMESPACE_ID::Metadata
descriptor_table_TriggerDisconnect_2eproto_metadata_getter(int index) {
  ::PROTOBUF_NAMESPACE_ID::internal::AssignDescriptors(&descriptor_table_TriggerDisconnect_2eproto);
  return descriptor_table_TriggerDisconnect_2eproto.file_level_metadata[index];
}

// Force running AddDescriptors() at dynamic initialization time.
PROTOBUF_ATTRIBUTE_INIT_PRIORITY static ::PROTOBUF_NAMESPACE_ID::internal::AddDescriptorsRunner dynamic_init_dummy_TriggerDisconnect_2eproto(&descriptor_table_TriggerDisconnect_2eproto);
namespace xpilot {

// ===================================================================

class TriggerDisconnect::_Internal {
 public:
  using HasBits = decltype(std::declval<TriggerDisconnect>()._has_bits_);
  static void set_has_reason(HasBits* has_bits) {
    (*has_bits)[0] |= 1u;
  }
};

TriggerDisconnect::TriggerDisconnect(::PROTOBUF_NAMESPACE_ID::Arena* arena)
  : ::PROTOBUF_NAMESPACE_ID::Message(arena) {
  SharedCtor();
  RegisterArenaDtor(arena);
  // @@protoc_insertion_point(arena_constructor:xpilot.TriggerDisconnect)
}
TriggerDisconnect::TriggerDisconnect(const TriggerDisconnect& from)
  : ::PROTOBUF_NAMESPACE_ID::Message(),
      _has_bits_(from._has_bits_) {
  _internal_metadata_.MergeFrom<::PROTOBUF_NAMESPACE_ID::UnknownFieldSet>(from._internal_metadata_);
  reason_.UnsafeSetDefault(&::PROTOBUF_NAMESPACE_ID::internal::GetEmptyStringAlreadyInited());
  if (from._internal_has_reason()) {
    reason_.Set(::PROTOBUF_NAMESPACE_ID::internal::ArenaStringPtr::EmptyDefault{}, from._internal_reason(), 
      GetArena());
  }
  // @@protoc_insertion_point(copy_constructor:xpilot.TriggerDisconnect)
}

void TriggerDisconnect::SharedCtor() {
reason_.UnsafeSetDefault(&::PROTOBUF_NAMESPACE_ID::internal::GetEmptyStringAlreadyInited());
}

TriggerDisconnect::~TriggerDisconnect() {
  // @@protoc_insertion_point(destructor:xpilot.TriggerDisconnect)
  SharedDtor();
  _internal_metadata_.Delete<::PROTOBUF_NAMESPACE_ID::UnknownFieldSet>();
}

void TriggerDisconnect::SharedDtor() {
  GOOGLE_DCHECK(GetArena() == nullptr);
  reason_.DestroyNoArena(&::PROTOBUF_NAMESPACE_ID::internal::GetEmptyStringAlreadyInited());
}

void TriggerDisconnect::ArenaDtor(void* object) {
  TriggerDisconnect* _this = reinterpret_cast< TriggerDisconnect* >(object);
  (void)_this;
}
void TriggerDisconnect::RegisterArenaDtor(::PROTOBUF_NAMESPACE_ID::Arena*) {
}
void TriggerDisconnect::SetCachedSize(int size) const {
  _cached_size_.Set(size);
}

void TriggerDisconnect::Clear() {
// @@protoc_insertion_point(message_clear_start:xpilot.TriggerDisconnect)
  ::PROTOBUF_NAMESPACE_ID::uint32 cached_has_bits = 0;
  // Prevent compiler warnings about cached_has_bits being unused
  (void) cached_has_bits;

  cached_has_bits = _has_bits_[0];
  if (cached_has_bits & 0x00000001u) {
    reason_.ClearNonDefaultToEmpty();
  }
  _has_bits_.Clear();
  _internal_metadata_.Clear<::PROTOBUF_NAMESPACE_ID::UnknownFieldSet>();
}

const char* TriggerDisconnect::_InternalParse(const char* ptr, ::PROTOBUF_NAMESPACE_ID::internal::ParseContext* ctx) {
#define CHK_(x) if (PROTOBUF_PREDICT_FALSE(!(x))) goto failure
  _Internal::HasBits has_bits{};
  while (!ctx->Done(&ptr)) {
    ::PROTOBUF_NAMESPACE_ID::uint32 tag;
    ptr = ::PROTOBUF_NAMESPACE_ID::internal::ReadTag(ptr, &tag);
    CHK_(ptr);
    switch (tag >> 3) {
      // string reason = 1;
      case 1:
        if (PROTOBUF_PREDICT_TRUE(static_cast<::PROTOBUF_NAMESPACE_ID::uint8>(tag) == 10)) {
          auto str = _internal_mutable_reason();
          ptr = ::PROTOBUF_NAMESPACE_ID::internal::InlineGreedyStringParser(str, ptr, ctx);
          CHK_(::PROTOBUF_NAMESPACE_ID::internal::VerifyUTF8(str, "xpilot.TriggerDisconnect.reason"));
          CHK_(ptr);
        } else goto handle_unusual;
        continue;
      default: {
      handle_unusual:
        if ((tag & 7) == 4 || tag == 0) {
          ctx->SetLastTag(tag);
          goto success;
        }
        ptr = UnknownFieldParse(tag,
            _internal_metadata_.mutable_unknown_fields<::PROTOBUF_NAMESPACE_ID::UnknownFieldSet>(),
            ptr, ctx);
        CHK_(ptr != nullptr);
        continue;
      }
    }  // switch
  }  // while
success:
  _has_bits_.Or(has_bits);
  return ptr;
failure:
  ptr = nullptr;
  goto success;
#undef CHK_
}

::PROTOBUF_NAMESPACE_ID::uint8* TriggerDisconnect::_InternalSerialize(
    ::PROTOBUF_NAMESPACE_ID::uint8* target, ::PROTOBUF_NAMESPACE_ID::io::EpsCopyOutputStream* stream) const {
  // @@protoc_insertion_point(serialize_to_array_start:xpilot.TriggerDisconnect)
  ::PROTOBUF_NAMESPACE_ID::uint32 cached_has_bits = 0;
  (void) cached_has_bits;

  // string reason = 1;
  if (_internal_has_reason()) {
    ::PROTOBUF_NAMESPACE_ID::internal::WireFormatLite::VerifyUtf8String(
      this->_internal_reason().data(), static_cast<int>(this->_internal_reason().length()),
      ::PROTOBUF_NAMESPACE_ID::internal::WireFormatLite::SERIALIZE,
      "xpilot.TriggerDisconnect.reason");
    target = stream->WriteStringMaybeAliased(
        1, this->_internal_reason(), target);
  }

  if (PROTOBUF_PREDICT_FALSE(_internal_metadata_.have_unknown_fields())) {
    target = ::PROTOBUF_NAMESPACE_ID::internal::WireFormat::InternalSerializeUnknownFieldsToArray(
        _internal_metadata_.unknown_fields<::PROTOBUF_NAMESPACE_ID::UnknownFieldSet>(::PROTOBUF_NAMESPACE_ID::UnknownFieldSet::default_instance), target, stream);
  }
  // @@protoc_insertion_point(serialize_to_array_end:xpilot.TriggerDisconnect)
  return target;
}

size_t TriggerDisconnect::ByteSizeLong() const {
// @@protoc_insertion_point(message_byte_size_start:xpilot.TriggerDisconnect)
  size_t total_size = 0;

  ::PROTOBUF_NAMESPACE_ID::uint32 cached_has_bits = 0;
  // Prevent compiler warnings about cached_has_bits being unused
  (void) cached_has_bits;

  // string reason = 1;
  cached_has_bits = _has_bits_[0];
  if (cached_has_bits & 0x00000001u) {
    total_size += 1 +
      ::PROTOBUF_NAMESPACE_ID::internal::WireFormatLite::StringSize(
        this->_internal_reason());
  }

  if (PROTOBUF_PREDICT_FALSE(_internal_metadata_.have_unknown_fields())) {
    return ::PROTOBUF_NAMESPACE_ID::internal::ComputeUnknownFieldsSize(
        _internal_metadata_, total_size, &_cached_size_);
  }
  int cached_size = ::PROTOBUF_NAMESPACE_ID::internal::ToCachedSize(total_size);
  SetCachedSize(cached_size);
  return total_size;
}

void TriggerDisconnect::MergeFrom(const ::PROTOBUF_NAMESPACE_ID::Message& from) {
// @@protoc_insertion_point(generalized_merge_from_start:xpilot.TriggerDisconnect)
  GOOGLE_DCHECK_NE(&from, this);
  const TriggerDisconnect* source =
      ::PROTOBUF_NAMESPACE_ID::DynamicCastToGenerated<TriggerDisconnect>(
          &from);
  if (source == nullptr) {
  // @@protoc_insertion_point(generalized_merge_from_cast_fail:xpilot.TriggerDisconnect)
    ::PROTOBUF_NAMESPACE_ID::internal::ReflectionOps::Merge(from, this);
  } else {
  // @@protoc_insertion_point(generalized_merge_from_cast_success:xpilot.TriggerDisconnect)
    MergeFrom(*source);
  }
}

void TriggerDisconnect::MergeFrom(const TriggerDisconnect& from) {
// @@protoc_insertion_point(class_specific_merge_from_start:xpilot.TriggerDisconnect)
  GOOGLE_DCHECK_NE(&from, this);
  _internal_metadata_.MergeFrom<::PROTOBUF_NAMESPACE_ID::UnknownFieldSet>(from._internal_metadata_);
  ::PROTOBUF_NAMESPACE_ID::uint32 cached_has_bits = 0;
  (void) cached_has_bits;

  if (from._internal_has_reason()) {
    _internal_set_reason(from._internal_reason());
  }
}

void TriggerDisconnect::CopyFrom(const ::PROTOBUF_NAMESPACE_ID::Message& from) {
// @@protoc_insertion_point(generalized_copy_from_start:xpilot.TriggerDisconnect)
  if (&from == this) return;
  Clear();
  MergeFrom(from);
}

void TriggerDisconnect::CopyFrom(const TriggerDisconnect& from) {
// @@protoc_insertion_point(class_specific_copy_from_start:xpilot.TriggerDisconnect)
  if (&from == this) return;
  Clear();
  MergeFrom(from);
}

bool TriggerDisconnect::IsInitialized() const {
  return true;
}

void TriggerDisconnect::InternalSwap(TriggerDisconnect* other) {
  using std::swap;
  _internal_metadata_.Swap<::PROTOBUF_NAMESPACE_ID::UnknownFieldSet>(&other->_internal_metadata_);
  swap(_has_bits_[0], other->_has_bits_[0]);
  reason_.Swap(&other->reason_, &::PROTOBUF_NAMESPACE_ID::internal::GetEmptyStringAlreadyInited(), GetArena());
}

::PROTOBUF_NAMESPACE_ID::Metadata TriggerDisconnect::GetMetadata() const {
  return GetMetadataStatic();
}


// @@protoc_insertion_point(namespace_scope)
}  // namespace xpilot
PROTOBUF_NAMESPACE_OPEN
template<> PROTOBUF_NOINLINE ::xpilot::TriggerDisconnect* Arena::CreateMaybeMessage< ::xpilot::TriggerDisconnect >(Arena* arena) {
  return Arena::CreateMessageInternal< ::xpilot::TriggerDisconnect >(arena);
}
PROTOBUF_NAMESPACE_CLOSE

// @@protoc_insertion_point(global_scope)
#include <google/protobuf/port_undef.inc>