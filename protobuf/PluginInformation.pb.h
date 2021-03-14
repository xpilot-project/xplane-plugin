// Generated by the protocol buffer compiler.  DO NOT EDIT!
// source: PluginInformation.proto

#ifndef GOOGLE_PROTOBUF_INCLUDED_PluginInformation_2eproto
#define GOOGLE_PROTOBUF_INCLUDED_PluginInformation_2eproto

#include <limits>
#include <string>

#include <google/protobuf/port_def.inc>
#if PROTOBUF_VERSION < 3015000
#error This file was generated by a newer version of protoc which is
#error incompatible with your Protocol Buffer headers. Please update
#error your headers.
#endif
#if 3015002 < PROTOBUF_MIN_PROTOC_VERSION
#error This file was generated by an older version of protoc which is
#error incompatible with your Protocol Buffer headers. Please
#error regenerate this file with a newer version of protoc.
#endif

#include <google/protobuf/port_undef.inc>
#include <google/protobuf/io/coded_stream.h>
#include <google/protobuf/arena.h>
#include <google/protobuf/arenastring.h>
#include <google/protobuf/generated_message_table_driven.h>
#include <google/protobuf/generated_message_util.h>
#include <google/protobuf/metadata_lite.h>
#include <google/protobuf/generated_message_reflection.h>
#include <google/protobuf/message.h>
#include <google/protobuf/repeated_field.h>  // IWYU pragma: export
#include <google/protobuf/extension_set.h>  // IWYU pragma: export
#include <google/protobuf/unknown_field_set.h>
// @@protoc_insertion_point(includes)
#include <google/protobuf/port_def.inc>
#define PROTOBUF_INTERNAL_EXPORT_PluginInformation_2eproto
PROTOBUF_NAMESPACE_OPEN
namespace internal {
class AnyMetadata;
}  // namespace internal
PROTOBUF_NAMESPACE_CLOSE

// Internal implementation detail -- do not use these members.
struct TableStruct_PluginInformation_2eproto {
  static const ::PROTOBUF_NAMESPACE_ID::internal::ParseTableField entries[]
    PROTOBUF_SECTION_VARIABLE(protodesc_cold);
  static const ::PROTOBUF_NAMESPACE_ID::internal::AuxiliaryParseTableField aux[]
    PROTOBUF_SECTION_VARIABLE(protodesc_cold);
  static const ::PROTOBUF_NAMESPACE_ID::internal::ParseTable schema[1]
    PROTOBUF_SECTION_VARIABLE(protodesc_cold);
  static const ::PROTOBUF_NAMESPACE_ID::internal::FieldMetadata field_metadata[];
  static const ::PROTOBUF_NAMESPACE_ID::internal::SerializationTable serialization_table[];
  static const ::PROTOBUF_NAMESPACE_ID::uint32 offsets[];
};
extern const ::PROTOBUF_NAMESPACE_ID::internal::DescriptorTable descriptor_table_PluginInformation_2eproto;
::PROTOBUF_NAMESPACE_ID::Metadata descriptor_table_PluginInformation_2eproto_metadata_getter(int index);
namespace xpilot {
class PluginInformation;
struct PluginInformationDefaultTypeInternal;
extern PluginInformationDefaultTypeInternal _PluginInformation_default_instance_;
}  // namespace xpilot
PROTOBUF_NAMESPACE_OPEN
template<> ::xpilot::PluginInformation* Arena::CreateMaybeMessage<::xpilot::PluginInformation>(Arena*);
PROTOBUF_NAMESPACE_CLOSE
namespace xpilot {

// ===================================================================

class PluginInformation PROTOBUF_FINAL :
    public ::PROTOBUF_NAMESPACE_ID::Message /* @@protoc_insertion_point(class_definition:xpilot.PluginInformation) */ {
 public:
  inline PluginInformation() : PluginInformation(nullptr) {}
  virtual ~PluginInformation();
  explicit constexpr PluginInformation(::PROTOBUF_NAMESPACE_ID::internal::ConstantInitialized);

  PluginInformation(const PluginInformation& from);
  PluginInformation(PluginInformation&& from) noexcept
    : PluginInformation() {
    *this = ::std::move(from);
  }

  inline PluginInformation& operator=(const PluginInformation& from) {
    CopyFrom(from);
    return *this;
  }
  inline PluginInformation& operator=(PluginInformation&& from) noexcept {
    if (GetArena() == from.GetArena()) {
      if (this != &from) InternalSwap(&from);
    } else {
      CopyFrom(from);
    }
    return *this;
  }

  static const ::PROTOBUF_NAMESPACE_ID::Descriptor* descriptor() {
    return GetDescriptor();
  }
  static const ::PROTOBUF_NAMESPACE_ID::Descriptor* GetDescriptor() {
    return GetMetadataStatic().descriptor;
  }
  static const ::PROTOBUF_NAMESPACE_ID::Reflection* GetReflection() {
    return GetMetadataStatic().reflection;
  }
  static const PluginInformation& default_instance() {
    return *internal_default_instance();
  }
  static inline const PluginInformation* internal_default_instance() {
    return reinterpret_cast<const PluginInformation*>(
               &_PluginInformation_default_instance_);
  }
  static constexpr int kIndexInFileMessages =
    0;

  friend void swap(PluginInformation& a, PluginInformation& b) {
    a.Swap(&b);
  }
  inline void Swap(PluginInformation* other) {
    if (other == this) return;
    if (GetArena() == other->GetArena()) {
      InternalSwap(other);
    } else {
      ::PROTOBUF_NAMESPACE_ID::internal::GenericSwap(this, other);
    }
  }
  void UnsafeArenaSwap(PluginInformation* other) {
    if (other == this) return;
    GOOGLE_DCHECK(GetArena() == other->GetArena());
    InternalSwap(other);
  }

  // implements Message ----------------------------------------------

  inline PluginInformation* New() const final {
    return CreateMaybeMessage<PluginInformation>(nullptr);
  }

  PluginInformation* New(::PROTOBUF_NAMESPACE_ID::Arena* arena) const final {
    return CreateMaybeMessage<PluginInformation>(arena);
  }
  void CopyFrom(const ::PROTOBUF_NAMESPACE_ID::Message& from) final;
  void MergeFrom(const ::PROTOBUF_NAMESPACE_ID::Message& from) final;
  void CopyFrom(const PluginInformation& from);
  void MergeFrom(const PluginInformation& from);
  PROTOBUF_ATTRIBUTE_REINITIALIZES void Clear() final;
  bool IsInitialized() const final;

  size_t ByteSizeLong() const final;
  const char* _InternalParse(const char* ptr, ::PROTOBUF_NAMESPACE_ID::internal::ParseContext* ctx) final;
  ::PROTOBUF_NAMESPACE_ID::uint8* _InternalSerialize(
      ::PROTOBUF_NAMESPACE_ID::uint8* target, ::PROTOBUF_NAMESPACE_ID::io::EpsCopyOutputStream* stream) const final;
  int GetCachedSize() const final { return _cached_size_.Get(); }

  private:
  inline void SharedCtor();
  inline void SharedDtor();
  void SetCachedSize(int size) const final;
  void InternalSwap(PluginInformation* other);
  friend class ::PROTOBUF_NAMESPACE_ID::internal::AnyMetadata;
  static ::PROTOBUF_NAMESPACE_ID::StringPiece FullMessageName() {
    return "xpilot.PluginInformation";
  }
  protected:
  explicit PluginInformation(::PROTOBUF_NAMESPACE_ID::Arena* arena);
  private:
  static void ArenaDtor(void* object);
  inline void RegisterArenaDtor(::PROTOBUF_NAMESPACE_ID::Arena* arena);
  public:

  ::PROTOBUF_NAMESPACE_ID::Metadata GetMetadata() const final;
  private:
  static ::PROTOBUF_NAMESPACE_ID::Metadata GetMetadataStatic() {
    return ::descriptor_table_PluginInformation_2eproto_metadata_getter(kIndexInFileMessages);
  }

  public:

  // nested types ----------------------------------------------------

  // accessors -------------------------------------------------------

  enum : int {
    kHashFieldNumber = 2,
    kVersionFieldNumber = 1,
  };
  // string hash = 2;
  bool has_hash() const;
  private:
  bool _internal_has_hash() const;
  public:
  void clear_hash();
  const std::string& hash() const;
  void set_hash(const std::string& value);
  void set_hash(std::string&& value);
  void set_hash(const char* value);
  void set_hash(const char* value, size_t size);
  std::string* mutable_hash();
  std::string* release_hash();
  void set_allocated_hash(std::string* hash);
  private:
  const std::string& _internal_hash() const;
  void _internal_set_hash(const std::string& value);
  std::string* _internal_mutable_hash();
  public:

  // int32 version = 1;
  bool has_version() const;
  private:
  bool _internal_has_version() const;
  public:
  void clear_version();
  ::PROTOBUF_NAMESPACE_ID::int32 version() const;
  void set_version(::PROTOBUF_NAMESPACE_ID::int32 value);
  private:
  ::PROTOBUF_NAMESPACE_ID::int32 _internal_version() const;
  void _internal_set_version(::PROTOBUF_NAMESPACE_ID::int32 value);
  public:

  // @@protoc_insertion_point(class_scope:xpilot.PluginInformation)
 private:
  class _Internal;

  template <typename T> friend class ::PROTOBUF_NAMESPACE_ID::Arena::InternalHelper;
  typedef void InternalArenaConstructable_;
  typedef void DestructorSkippable_;
  ::PROTOBUF_NAMESPACE_ID::internal::HasBits<1> _has_bits_;
  mutable ::PROTOBUF_NAMESPACE_ID::internal::CachedSize _cached_size_;
  ::PROTOBUF_NAMESPACE_ID::internal::ArenaStringPtr hash_;
  ::PROTOBUF_NAMESPACE_ID::int32 version_;
  friend struct ::TableStruct_PluginInformation_2eproto;
};
// ===================================================================


// ===================================================================

#ifdef __GNUC__
  #pragma GCC diagnostic push
  #pragma GCC diagnostic ignored "-Wstrict-aliasing"
#endif  // __GNUC__
// PluginInformation

// int32 version = 1;
inline bool PluginInformation::_internal_has_version() const {
  bool value = (_has_bits_[0] & 0x00000002u) != 0;
  return value;
}
inline bool PluginInformation::has_version() const {
  return _internal_has_version();
}
inline void PluginInformation::clear_version() {
  version_ = 0;
  _has_bits_[0] &= ~0x00000002u;
}
inline ::PROTOBUF_NAMESPACE_ID::int32 PluginInformation::_internal_version() const {
  return version_;
}
inline ::PROTOBUF_NAMESPACE_ID::int32 PluginInformation::version() const {
  // @@protoc_insertion_point(field_get:xpilot.PluginInformation.version)
  return _internal_version();
}
inline void PluginInformation::_internal_set_version(::PROTOBUF_NAMESPACE_ID::int32 value) {
  _has_bits_[0] |= 0x00000002u;
  version_ = value;
}
inline void PluginInformation::set_version(::PROTOBUF_NAMESPACE_ID::int32 value) {
  _internal_set_version(value);
  // @@protoc_insertion_point(field_set:xpilot.PluginInformation.version)
}

// string hash = 2;
inline bool PluginInformation::_internal_has_hash() const {
  bool value = (_has_bits_[0] & 0x00000001u) != 0;
  return value;
}
inline bool PluginInformation::has_hash() const {
  return _internal_has_hash();
}
inline void PluginInformation::clear_hash() {
  hash_.ClearToEmpty();
  _has_bits_[0] &= ~0x00000001u;
}
inline const std::string& PluginInformation::hash() const {
  // @@protoc_insertion_point(field_get:xpilot.PluginInformation.hash)
  return _internal_hash();
}
inline void PluginInformation::set_hash(const std::string& value) {
  _internal_set_hash(value);
  // @@protoc_insertion_point(field_set:xpilot.PluginInformation.hash)
}
inline std::string* PluginInformation::mutable_hash() {
  // @@protoc_insertion_point(field_mutable:xpilot.PluginInformation.hash)
  return _internal_mutable_hash();
}
inline const std::string& PluginInformation::_internal_hash() const {
  return hash_.Get();
}
inline void PluginInformation::_internal_set_hash(const std::string& value) {
  _has_bits_[0] |= 0x00000001u;
  hash_.Set(::PROTOBUF_NAMESPACE_ID::internal::ArenaStringPtr::EmptyDefault{}, value, GetArena());
}
inline void PluginInformation::set_hash(std::string&& value) {
  _has_bits_[0] |= 0x00000001u;
  hash_.Set(
    ::PROTOBUF_NAMESPACE_ID::internal::ArenaStringPtr::EmptyDefault{}, ::std::move(value), GetArena());
  // @@protoc_insertion_point(field_set_rvalue:xpilot.PluginInformation.hash)
}
inline void PluginInformation::set_hash(const char* value) {
  GOOGLE_DCHECK(value != nullptr);
  _has_bits_[0] |= 0x00000001u;
  hash_.Set(::PROTOBUF_NAMESPACE_ID::internal::ArenaStringPtr::EmptyDefault{}, ::std::string(value), GetArena());
  // @@protoc_insertion_point(field_set_char:xpilot.PluginInformation.hash)
}
inline void PluginInformation::set_hash(const char* value,
    size_t size) {
  _has_bits_[0] |= 0x00000001u;
  hash_.Set(::PROTOBUF_NAMESPACE_ID::internal::ArenaStringPtr::EmptyDefault{}, ::std::string(
      reinterpret_cast<const char*>(value), size), GetArena());
  // @@protoc_insertion_point(field_set_pointer:xpilot.PluginInformation.hash)
}
inline std::string* PluginInformation::_internal_mutable_hash() {
  _has_bits_[0] |= 0x00000001u;
  return hash_.Mutable(::PROTOBUF_NAMESPACE_ID::internal::ArenaStringPtr::EmptyDefault{}, GetArena());
}
inline std::string* PluginInformation::release_hash() {
  // @@protoc_insertion_point(field_release:xpilot.PluginInformation.hash)
  if (!_internal_has_hash()) {
    return nullptr;
  }
  _has_bits_[0] &= ~0x00000001u;
  return hash_.ReleaseNonDefault(&::PROTOBUF_NAMESPACE_ID::internal::GetEmptyStringAlreadyInited(), GetArena());
}
inline void PluginInformation::set_allocated_hash(std::string* hash) {
  if (hash != nullptr) {
    _has_bits_[0] |= 0x00000001u;
  } else {
    _has_bits_[0] &= ~0x00000001u;
  }
  hash_.SetAllocated(&::PROTOBUF_NAMESPACE_ID::internal::GetEmptyStringAlreadyInited(), hash,
      GetArena());
  // @@protoc_insertion_point(field_set_allocated:xpilot.PluginInformation.hash)
}

#ifdef __GNUC__
  #pragma GCC diagnostic pop
#endif  // __GNUC__

// @@protoc_insertion_point(namespace_scope)

}  // namespace xpilot

// @@protoc_insertion_point(global_scope)

#include <google/protobuf/port_undef.inc>
#endif  // GOOGLE_PROTOBUF_INCLUDED_GOOGLE_PROTOBUF_INCLUDED_PluginInformation_2eproto
