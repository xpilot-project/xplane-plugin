// Generated by the protocol buffer compiler.  DO NOT EDIT!
// source: SetTransponderCode.proto

#include "SetTransponderCode.pb.h"

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
constexpr SetTransponderCode::SetTransponderCode(
  ::PROTOBUF_NAMESPACE_ID::internal::ConstantInitialized)
  : code_(0){}
struct SetTransponderCodeDefaultTypeInternal {
  constexpr SetTransponderCodeDefaultTypeInternal()
    : _instance(::PROTOBUF_NAMESPACE_ID::internal::ConstantInitialized{}) {}
  ~SetTransponderCodeDefaultTypeInternal() {}
  union {
    SetTransponderCode _instance;
  };
};
PROTOBUF_ATTRIBUTE_NO_DESTROY PROTOBUF_CONSTINIT SetTransponderCodeDefaultTypeInternal _SetTransponderCode_default_instance_;
}  // namespace xpilot
static ::PROTOBUF_NAMESPACE_ID::Metadata file_level_metadata_SetTransponderCode_2eproto[1];
static constexpr ::PROTOBUF_NAMESPACE_ID::EnumDescriptor const** file_level_enum_descriptors_SetTransponderCode_2eproto = nullptr;
static constexpr ::PROTOBUF_NAMESPACE_ID::ServiceDescriptor const** file_level_service_descriptors_SetTransponderCode_2eproto = nullptr;

const ::PROTOBUF_NAMESPACE_ID::uint32 TableStruct_SetTransponderCode_2eproto::offsets[] PROTOBUF_SECTION_VARIABLE(protodesc_cold) = {
  PROTOBUF_FIELD_OFFSET(::xpilot::SetTransponderCode, _has_bits_),
  PROTOBUF_FIELD_OFFSET(::xpilot::SetTransponderCode, _internal_metadata_),
  ~0u,  // no _extensions_
  ~0u,  // no _oneof_case_
  ~0u,  // no _weak_field_map_
  PROTOBUF_FIELD_OFFSET(::xpilot::SetTransponderCode, code_),
  0,
};
static const ::PROTOBUF_NAMESPACE_ID::internal::MigrationSchema schemas[] PROTOBUF_SECTION_VARIABLE(protodesc_cold) = {
  { 0, 6, sizeof(::xpilot::SetTransponderCode)},
};

static ::PROTOBUF_NAMESPACE_ID::Message const * const file_default_instances[] = {
  reinterpret_cast<const ::PROTOBUF_NAMESPACE_ID::Message*>(&::xpilot::_SetTransponderCode_default_instance_),
};

const char descriptor_table_protodef_SetTransponderCode_2eproto[] PROTOBUF_SECTION_VARIABLE(protodesc_cold) =
  "\n\030SetTransponderCode.proto\022\006xpilot\"0\n\022Se"
  "tTransponderCode\022\021\n\004Code\030\001 \001(\005H\000\210\001\001B\007\n\005_"
  "CodeB\031\252\002\026Vatsim.Xpilot.Protobufb\006proto3"
  ;
static ::PROTOBUF_NAMESPACE_ID::internal::once_flag descriptor_table_SetTransponderCode_2eproto_once;
const ::PROTOBUF_NAMESPACE_ID::internal::DescriptorTable descriptor_table_SetTransponderCode_2eproto = {
  false, false, 119, descriptor_table_protodef_SetTransponderCode_2eproto, "SetTransponderCode.proto", 
  &descriptor_table_SetTransponderCode_2eproto_once, nullptr, 0, 1,
  schemas, file_default_instances, TableStruct_SetTransponderCode_2eproto::offsets,
  file_level_metadata_SetTransponderCode_2eproto, file_level_enum_descriptors_SetTransponderCode_2eproto, file_level_service_descriptors_SetTransponderCode_2eproto,
};
PROTOBUF_ATTRIBUTE_WEAK ::PROTOBUF_NAMESPACE_ID::Metadata
descriptor_table_SetTransponderCode_2eproto_metadata_getter(int index) {
  ::PROTOBUF_NAMESPACE_ID::internal::AssignDescriptors(&descriptor_table_SetTransponderCode_2eproto);
  return descriptor_table_SetTransponderCode_2eproto.file_level_metadata[index];
}

// Force running AddDescriptors() at dynamic initialization time.
PROTOBUF_ATTRIBUTE_INIT_PRIORITY static ::PROTOBUF_NAMESPACE_ID::internal::AddDescriptorsRunner dynamic_init_dummy_SetTransponderCode_2eproto(&descriptor_table_SetTransponderCode_2eproto);
namespace xpilot {

// ===================================================================

class SetTransponderCode::_Internal {
 public:
  using HasBits = decltype(std::declval<SetTransponderCode>()._has_bits_);
  static void set_has_code(HasBits* has_bits) {
    (*has_bits)[0] |= 1u;
  }
};

SetTransponderCode::SetTransponderCode(::PROTOBUF_NAMESPACE_ID::Arena* arena)
  : ::PROTOBUF_NAMESPACE_ID::Message(arena) {
  SharedCtor();
  RegisterArenaDtor(arena);
  // @@protoc_insertion_point(arena_constructor:xpilot.SetTransponderCode)
}
SetTransponderCode::SetTransponderCode(const SetTransponderCode& from)
  : ::PROTOBUF_NAMESPACE_ID::Message(),
      _has_bits_(from._has_bits_) {
  _internal_metadata_.MergeFrom<::PROTOBUF_NAMESPACE_ID::UnknownFieldSet>(from._internal_metadata_);
  code_ = from.code_;
  // @@protoc_insertion_point(copy_constructor:xpilot.SetTransponderCode)
}

void SetTransponderCode::SharedCtor() {
code_ = 0;
}

SetTransponderCode::~SetTransponderCode() {
  // @@protoc_insertion_point(destructor:xpilot.SetTransponderCode)
  SharedDtor();
  _internal_metadata_.Delete<::PROTOBUF_NAMESPACE_ID::UnknownFieldSet>();
}

void SetTransponderCode::SharedDtor() {
  GOOGLE_DCHECK(GetArena() == nullptr);
}

void SetTransponderCode::ArenaDtor(void* object) {
  SetTransponderCode* _this = reinterpret_cast< SetTransponderCode* >(object);
  (void)_this;
}
void SetTransponderCode::RegisterArenaDtor(::PROTOBUF_NAMESPACE_ID::Arena*) {
}
void SetTransponderCode::SetCachedSize(int size) const {
  _cached_size_.Set(size);
}

void SetTransponderCode::Clear() {
// @@protoc_insertion_point(message_clear_start:xpilot.SetTransponderCode)
  ::PROTOBUF_NAMESPACE_ID::uint32 cached_has_bits = 0;
  // Prevent compiler warnings about cached_has_bits being unused
  (void) cached_has_bits;

  code_ = 0;
  _has_bits_.Clear();
  _internal_metadata_.Clear<::PROTOBUF_NAMESPACE_ID::UnknownFieldSet>();
}

const char* SetTransponderCode::_InternalParse(const char* ptr, ::PROTOBUF_NAMESPACE_ID::internal::ParseContext* ctx) {
#define CHK_(x) if (PROTOBUF_PREDICT_FALSE(!(x))) goto failure
  _Internal::HasBits has_bits{};
  while (!ctx->Done(&ptr)) {
    ::PROTOBUF_NAMESPACE_ID::uint32 tag;
    ptr = ::PROTOBUF_NAMESPACE_ID::internal::ReadTag(ptr, &tag);
    CHK_(ptr);
    switch (tag >> 3) {
      // int32 Code = 1;
      case 1:
        if (PROTOBUF_PREDICT_TRUE(static_cast<::PROTOBUF_NAMESPACE_ID::uint8>(tag) == 8)) {
          _Internal::set_has_code(&has_bits);
          code_ = ::PROTOBUF_NAMESPACE_ID::internal::ReadVarint64(&ptr);
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

::PROTOBUF_NAMESPACE_ID::uint8* SetTransponderCode::_InternalSerialize(
    ::PROTOBUF_NAMESPACE_ID::uint8* target, ::PROTOBUF_NAMESPACE_ID::io::EpsCopyOutputStream* stream) const {
  // @@protoc_insertion_point(serialize_to_array_start:xpilot.SetTransponderCode)
  ::PROTOBUF_NAMESPACE_ID::uint32 cached_has_bits = 0;
  (void) cached_has_bits;

  // int32 Code = 1;
  if (_internal_has_code()) {
    target = stream->EnsureSpace(target);
    target = ::PROTOBUF_NAMESPACE_ID::internal::WireFormatLite::WriteInt32ToArray(1, this->_internal_code(), target);
  }

  if (PROTOBUF_PREDICT_FALSE(_internal_metadata_.have_unknown_fields())) {
    target = ::PROTOBUF_NAMESPACE_ID::internal::WireFormat::InternalSerializeUnknownFieldsToArray(
        _internal_metadata_.unknown_fields<::PROTOBUF_NAMESPACE_ID::UnknownFieldSet>(::PROTOBUF_NAMESPACE_ID::UnknownFieldSet::default_instance), target, stream);
  }
  // @@protoc_insertion_point(serialize_to_array_end:xpilot.SetTransponderCode)
  return target;
}

size_t SetTransponderCode::ByteSizeLong() const {
// @@protoc_insertion_point(message_byte_size_start:xpilot.SetTransponderCode)
  size_t total_size = 0;

  ::PROTOBUF_NAMESPACE_ID::uint32 cached_has_bits = 0;
  // Prevent compiler warnings about cached_has_bits being unused
  (void) cached_has_bits;

  // int32 Code = 1;
  cached_has_bits = _has_bits_[0];
  if (cached_has_bits & 0x00000001u) {
    total_size += 1 +
      ::PROTOBUF_NAMESPACE_ID::internal::WireFormatLite::Int32Size(
        this->_internal_code());
  }

  if (PROTOBUF_PREDICT_FALSE(_internal_metadata_.have_unknown_fields())) {
    return ::PROTOBUF_NAMESPACE_ID::internal::ComputeUnknownFieldsSize(
        _internal_metadata_, total_size, &_cached_size_);
  }
  int cached_size = ::PROTOBUF_NAMESPACE_ID::internal::ToCachedSize(total_size);
  SetCachedSize(cached_size);
  return total_size;
}

void SetTransponderCode::MergeFrom(const ::PROTOBUF_NAMESPACE_ID::Message& from) {
// @@protoc_insertion_point(generalized_merge_from_start:xpilot.SetTransponderCode)
  GOOGLE_DCHECK_NE(&from, this);
  const SetTransponderCode* source =
      ::PROTOBUF_NAMESPACE_ID::DynamicCastToGenerated<SetTransponderCode>(
          &from);
  if (source == nullptr) {
  // @@protoc_insertion_point(generalized_merge_from_cast_fail:xpilot.SetTransponderCode)
    ::PROTOBUF_NAMESPACE_ID::internal::ReflectionOps::Merge(from, this);
  } else {
  // @@protoc_insertion_point(generalized_merge_from_cast_success:xpilot.SetTransponderCode)
    MergeFrom(*source);
  }
}

void SetTransponderCode::MergeFrom(const SetTransponderCode& from) {
// @@protoc_insertion_point(class_specific_merge_from_start:xpilot.SetTransponderCode)
  GOOGLE_DCHECK_NE(&from, this);
  _internal_metadata_.MergeFrom<::PROTOBUF_NAMESPACE_ID::UnknownFieldSet>(from._internal_metadata_);
  ::PROTOBUF_NAMESPACE_ID::uint32 cached_has_bits = 0;
  (void) cached_has_bits;

  if (from._internal_has_code()) {
    _internal_set_code(from._internal_code());
  }
}

void SetTransponderCode::CopyFrom(const ::PROTOBUF_NAMESPACE_ID::Message& from) {
// @@protoc_insertion_point(generalized_copy_from_start:xpilot.SetTransponderCode)
  if (&from == this) return;
  Clear();
  MergeFrom(from);
}

void SetTransponderCode::CopyFrom(const SetTransponderCode& from) {
// @@protoc_insertion_point(class_specific_copy_from_start:xpilot.SetTransponderCode)
  if (&from == this) return;
  Clear();
  MergeFrom(from);
}

bool SetTransponderCode::IsInitialized() const {
  return true;
}

void SetTransponderCode::InternalSwap(SetTransponderCode* other) {
  using std::swap;
  _internal_metadata_.Swap<::PROTOBUF_NAMESPACE_ID::UnknownFieldSet>(&other->_internal_metadata_);
  swap(_has_bits_[0], other->_has_bits_[0]);
  swap(code_, other->code_);
}

::PROTOBUF_NAMESPACE_ID::Metadata SetTransponderCode::GetMetadata() const {
  return GetMetadataStatic();
}


// @@protoc_insertion_point(namespace_scope)
}  // namespace xpilot
PROTOBUF_NAMESPACE_OPEN
template<> PROTOBUF_NOINLINE ::xpilot::SetTransponderCode* Arena::CreateMaybeMessage< ::xpilot::SetTransponderCode >(Arena* arena) {
  return Arena::CreateMessageInternal< ::xpilot::SetTransponderCode >(arena);
}
PROTOBUF_NAMESPACE_CLOSE

// @@protoc_insertion_point(global_scope)
#include <google/protobuf/port_undef.inc>