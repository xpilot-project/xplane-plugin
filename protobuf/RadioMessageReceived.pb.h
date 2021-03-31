// Generated by the protocol buffer compiler.  DO NOT EDIT!
// source: RadioMessageReceived.proto

#ifndef GOOGLE_PROTOBUF_INCLUDED_RadioMessageReceived_2eproto
#define GOOGLE_PROTOBUF_INCLUDED_RadioMessageReceived_2eproto

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
#define PROTOBUF_INTERNAL_EXPORT_RadioMessageReceived_2eproto
PROTOBUF_NAMESPACE_OPEN
namespace internal {
class AnyMetadata;
}  // namespace internal
PROTOBUF_NAMESPACE_CLOSE

// Internal implementation detail -- do not use these members.
struct TableStruct_RadioMessageReceived_2eproto {
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
extern const ::PROTOBUF_NAMESPACE_ID::internal::DescriptorTable descriptor_table_RadioMessageReceived_2eproto;
::PROTOBUF_NAMESPACE_ID::Metadata descriptor_table_RadioMessageReceived_2eproto_metadata_getter(int index);
namespace xpilot {
class RadioMessageReceived;
struct RadioMessageReceivedDefaultTypeInternal;
extern RadioMessageReceivedDefaultTypeInternal _RadioMessageReceived_default_instance_;
}  // namespace xpilot
PROTOBUF_NAMESPACE_OPEN
template<> ::xpilot::RadioMessageReceived* Arena::CreateMaybeMessage<::xpilot::RadioMessageReceived>(Arena*);
PROTOBUF_NAMESPACE_CLOSE
namespace xpilot {

// ===================================================================

class RadioMessageReceived PROTOBUF_FINAL :
    public ::PROTOBUF_NAMESPACE_ID::Message /* @@protoc_insertion_point(class_definition:xpilot.RadioMessageReceived) */ {
 public:
  inline RadioMessageReceived() : RadioMessageReceived(nullptr) {}
  virtual ~RadioMessageReceived();
  explicit constexpr RadioMessageReceived(::PROTOBUF_NAMESPACE_ID::internal::ConstantInitialized);

  RadioMessageReceived(const RadioMessageReceived& from);
  RadioMessageReceived(RadioMessageReceived&& from) noexcept
    : RadioMessageReceived() {
    *this = ::std::move(from);
  }

  inline RadioMessageReceived& operator=(const RadioMessageReceived& from) {
    CopyFrom(from);
    return *this;
  }
  inline RadioMessageReceived& operator=(RadioMessageReceived&& from) noexcept {
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
  static const RadioMessageReceived& default_instance() {
    return *internal_default_instance();
  }
  static inline const RadioMessageReceived* internal_default_instance() {
    return reinterpret_cast<const RadioMessageReceived*>(
               &_RadioMessageReceived_default_instance_);
  }
  static constexpr int kIndexInFileMessages =
    0;

  friend void swap(RadioMessageReceived& a, RadioMessageReceived& b) {
    a.Swap(&b);
  }
  inline void Swap(RadioMessageReceived* other) {
    if (other == this) return;
    if (GetArena() == other->GetArena()) {
      InternalSwap(other);
    } else {
      ::PROTOBUF_NAMESPACE_ID::internal::GenericSwap(this, other);
    }
  }
  void UnsafeArenaSwap(RadioMessageReceived* other) {
    if (other == this) return;
    GOOGLE_DCHECK(GetArena() == other->GetArena());
    InternalSwap(other);
  }

  // implements Message ----------------------------------------------

  inline RadioMessageReceived* New() const final {
    return CreateMaybeMessage<RadioMessageReceived>(nullptr);
  }

  RadioMessageReceived* New(::PROTOBUF_NAMESPACE_ID::Arena* arena) const final {
    return CreateMaybeMessage<RadioMessageReceived>(arena);
  }
  void CopyFrom(const ::PROTOBUF_NAMESPACE_ID::Message& from) final;
  void MergeFrom(const ::PROTOBUF_NAMESPACE_ID::Message& from) final;
  void CopyFrom(const RadioMessageReceived& from);
  void MergeFrom(const RadioMessageReceived& from);
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
  void InternalSwap(RadioMessageReceived* other);
  friend class ::PROTOBUF_NAMESPACE_ID::internal::AnyMetadata;
  static ::PROTOBUF_NAMESPACE_ID::StringPiece FullMessageName() {
    return "xpilot.RadioMessageReceived";
  }
  protected:
  explicit RadioMessageReceived(::PROTOBUF_NAMESPACE_ID::Arena* arena);
  private:
  static void ArenaDtor(void* object);
  inline void RegisterArenaDtor(::PROTOBUF_NAMESPACE_ID::Arena* arena);
  public:

  ::PROTOBUF_NAMESPACE_ID::Metadata GetMetadata() const final;
  private:
  static ::PROTOBUF_NAMESPACE_ID::Metadata GetMetadataStatic() {
    return ::descriptor_table_RadioMessageReceived_2eproto_metadata_getter(kIndexInFileMessages);
  }

  public:

  // nested types ----------------------------------------------------

  // accessors -------------------------------------------------------

  enum : int {
    kFromFieldNumber = 1,
    kMessageFieldNumber = 2,
    kColorFieldNumber = 3,
  };
  // string from = 1;
  bool has_from() const;
  private:
  bool _internal_has_from() const;
  public:
  void clear_from();
  const std::string& from() const;
  void set_from(const std::string& value);
  void set_from(std::string&& value);
  void set_from(const char* value);
  void set_from(const char* value, size_t size);
  std::string* mutable_from();
  std::string* release_from();
  void set_allocated_from(std::string* from);
  private:
  const std::string& _internal_from() const;
  void _internal_set_from(const std::string& value);
  std::string* _internal_mutable_from();
  public:

  // string message = 2;
  bool has_message() const;
  private:
  bool _internal_has_message() const;
  public:
  void clear_message();
  const std::string& message() const;
  void set_message(const std::string& value);
  void set_message(std::string&& value);
  void set_message(const char* value);
  void set_message(const char* value, size_t size);
  std::string* mutable_message();
  std::string* release_message();
  void set_allocated_message(std::string* message);
  private:
  const std::string& _internal_message() const;
  void _internal_set_message(const std::string& value);
  std::string* _internal_mutable_message();
  public:

  // int32 color = 3;
  bool has_color() const;
  private:
  bool _internal_has_color() const;
  public:
  void clear_color();
  ::PROTOBUF_NAMESPACE_ID::int32 color() const;
  void set_color(::PROTOBUF_NAMESPACE_ID::int32 value);
  private:
  ::PROTOBUF_NAMESPACE_ID::int32 _internal_color() const;
  void _internal_set_color(::PROTOBUF_NAMESPACE_ID::int32 value);
  public:

  // @@protoc_insertion_point(class_scope:xpilot.RadioMessageReceived)
 private:
  class _Internal;

  template <typename T> friend class ::PROTOBUF_NAMESPACE_ID::Arena::InternalHelper;
  typedef void InternalArenaConstructable_;
  typedef void DestructorSkippable_;
  ::PROTOBUF_NAMESPACE_ID::internal::HasBits<1> _has_bits_;
  mutable ::PROTOBUF_NAMESPACE_ID::internal::CachedSize _cached_size_;
  ::PROTOBUF_NAMESPACE_ID::internal::ArenaStringPtr from_;
  ::PROTOBUF_NAMESPACE_ID::internal::ArenaStringPtr message_;
  ::PROTOBUF_NAMESPACE_ID::int32 color_;
  friend struct ::TableStruct_RadioMessageReceived_2eproto;
};
// ===================================================================


// ===================================================================

#ifdef __GNUC__
  #pragma GCC diagnostic push
  #pragma GCC diagnostic ignored "-Wstrict-aliasing"
#endif  // __GNUC__
// RadioMessageReceived

// string from = 1;
inline bool RadioMessageReceived::_internal_has_from() const {
  bool value = (_has_bits_[0] & 0x00000001u) != 0;
  return value;
}
inline bool RadioMessageReceived::has_from() const {
  return _internal_has_from();
}
inline void RadioMessageReceived::clear_from() {
  from_.ClearToEmpty();
  _has_bits_[0] &= ~0x00000001u;
}
inline const std::string& RadioMessageReceived::from() const {
  // @@protoc_insertion_point(field_get:xpilot.RadioMessageReceived.from)
  return _internal_from();
}
inline void RadioMessageReceived::set_from(const std::string& value) {
  _internal_set_from(value);
  // @@protoc_insertion_point(field_set:xpilot.RadioMessageReceived.from)
}
inline std::string* RadioMessageReceived::mutable_from() {
  // @@protoc_insertion_point(field_mutable:xpilot.RadioMessageReceived.from)
  return _internal_mutable_from();
}
inline const std::string& RadioMessageReceived::_internal_from() const {
  return from_.Get();
}
inline void RadioMessageReceived::_internal_set_from(const std::string& value) {
  _has_bits_[0] |= 0x00000001u;
  from_.Set(::PROTOBUF_NAMESPACE_ID::internal::ArenaStringPtr::EmptyDefault{}, value, GetArena());
}
inline void RadioMessageReceived::set_from(std::string&& value) {
  _has_bits_[0] |= 0x00000001u;
  from_.Set(
    ::PROTOBUF_NAMESPACE_ID::internal::ArenaStringPtr::EmptyDefault{}, ::std::move(value), GetArena());
  // @@protoc_insertion_point(field_set_rvalue:xpilot.RadioMessageReceived.from)
}
inline void RadioMessageReceived::set_from(const char* value) {
  GOOGLE_DCHECK(value != nullptr);
  _has_bits_[0] |= 0x00000001u;
  from_.Set(::PROTOBUF_NAMESPACE_ID::internal::ArenaStringPtr::EmptyDefault{}, ::std::string(value), GetArena());
  // @@protoc_insertion_point(field_set_char:xpilot.RadioMessageReceived.from)
}
inline void RadioMessageReceived::set_from(const char* value,
    size_t size) {
  _has_bits_[0] |= 0x00000001u;
  from_.Set(::PROTOBUF_NAMESPACE_ID::internal::ArenaStringPtr::EmptyDefault{}, ::std::string(
      reinterpret_cast<const char*>(value), size), GetArena());
  // @@protoc_insertion_point(field_set_pointer:xpilot.RadioMessageReceived.from)
}
inline std::string* RadioMessageReceived::_internal_mutable_from() {
  _has_bits_[0] |= 0x00000001u;
  return from_.Mutable(::PROTOBUF_NAMESPACE_ID::internal::ArenaStringPtr::EmptyDefault{}, GetArena());
}
inline std::string* RadioMessageReceived::release_from() {
  // @@protoc_insertion_point(field_release:xpilot.RadioMessageReceived.from)
  if (!_internal_has_from()) {
    return nullptr;
  }
  _has_bits_[0] &= ~0x00000001u;
  return from_.ReleaseNonDefault(&::PROTOBUF_NAMESPACE_ID::internal::GetEmptyStringAlreadyInited(), GetArena());
}
inline void RadioMessageReceived::set_allocated_from(std::string* from) {
  if (from != nullptr) {
    _has_bits_[0] |= 0x00000001u;
  } else {
    _has_bits_[0] &= ~0x00000001u;
  }
  from_.SetAllocated(&::PROTOBUF_NAMESPACE_ID::internal::GetEmptyStringAlreadyInited(), from,
      GetArena());
  // @@protoc_insertion_point(field_set_allocated:xpilot.RadioMessageReceived.from)
}

// string message = 2;
inline bool RadioMessageReceived::_internal_has_message() const {
  bool value = (_has_bits_[0] & 0x00000002u) != 0;
  return value;
}
inline bool RadioMessageReceived::has_message() const {
  return _internal_has_message();
}
inline void RadioMessageReceived::clear_message() {
  message_.ClearToEmpty();
  _has_bits_[0] &= ~0x00000002u;
}
inline const std::string& RadioMessageReceived::message() const {
  // @@protoc_insertion_point(field_get:xpilot.RadioMessageReceived.message)
  return _internal_message();
}
inline void RadioMessageReceived::set_message(const std::string& value) {
  _internal_set_message(value);
  // @@protoc_insertion_point(field_set:xpilot.RadioMessageReceived.message)
}
inline std::string* RadioMessageReceived::mutable_message() {
  // @@protoc_insertion_point(field_mutable:xpilot.RadioMessageReceived.message)
  return _internal_mutable_message();
}
inline const std::string& RadioMessageReceived::_internal_message() const {
  return message_.Get();
}
inline void RadioMessageReceived::_internal_set_message(const std::string& value) {
  _has_bits_[0] |= 0x00000002u;
  message_.Set(::PROTOBUF_NAMESPACE_ID::internal::ArenaStringPtr::EmptyDefault{}, value, GetArena());
}
inline void RadioMessageReceived::set_message(std::string&& value) {
  _has_bits_[0] |= 0x00000002u;
  message_.Set(
    ::PROTOBUF_NAMESPACE_ID::internal::ArenaStringPtr::EmptyDefault{}, ::std::move(value), GetArena());
  // @@protoc_insertion_point(field_set_rvalue:xpilot.RadioMessageReceived.message)
}
inline void RadioMessageReceived::set_message(const char* value) {
  GOOGLE_DCHECK(value != nullptr);
  _has_bits_[0] |= 0x00000002u;
  message_.Set(::PROTOBUF_NAMESPACE_ID::internal::ArenaStringPtr::EmptyDefault{}, ::std::string(value), GetArena());
  // @@protoc_insertion_point(field_set_char:xpilot.RadioMessageReceived.message)
}
inline void RadioMessageReceived::set_message(const char* value,
    size_t size) {
  _has_bits_[0] |= 0x00000002u;
  message_.Set(::PROTOBUF_NAMESPACE_ID::internal::ArenaStringPtr::EmptyDefault{}, ::std::string(
      reinterpret_cast<const char*>(value), size), GetArena());
  // @@protoc_insertion_point(field_set_pointer:xpilot.RadioMessageReceived.message)
}
inline std::string* RadioMessageReceived::_internal_mutable_message() {
  _has_bits_[0] |= 0x00000002u;
  return message_.Mutable(::PROTOBUF_NAMESPACE_ID::internal::ArenaStringPtr::EmptyDefault{}, GetArena());
}
inline std::string* RadioMessageReceived::release_message() {
  // @@protoc_insertion_point(field_release:xpilot.RadioMessageReceived.message)
  if (!_internal_has_message()) {
    return nullptr;
  }
  _has_bits_[0] &= ~0x00000002u;
  return message_.ReleaseNonDefault(&::PROTOBUF_NAMESPACE_ID::internal::GetEmptyStringAlreadyInited(), GetArena());
}
inline void RadioMessageReceived::set_allocated_message(std::string* message) {
  if (message != nullptr) {
    _has_bits_[0] |= 0x00000002u;
  } else {
    _has_bits_[0] &= ~0x00000002u;
  }
  message_.SetAllocated(&::PROTOBUF_NAMESPACE_ID::internal::GetEmptyStringAlreadyInited(), message,
      GetArena());
  // @@protoc_insertion_point(field_set_allocated:xpilot.RadioMessageReceived.message)
}

// int32 color = 3;
inline bool RadioMessageReceived::_internal_has_color() const {
  bool value = (_has_bits_[0] & 0x00000004u) != 0;
  return value;
}
inline bool RadioMessageReceived::has_color() const {
  return _internal_has_color();
}
inline void RadioMessageReceived::clear_color() {
  color_ = 0;
  _has_bits_[0] &= ~0x00000004u;
}
inline ::PROTOBUF_NAMESPACE_ID::int32 RadioMessageReceived::_internal_color() const {
  return color_;
}
inline ::PROTOBUF_NAMESPACE_ID::int32 RadioMessageReceived::color() const {
  // @@protoc_insertion_point(field_get:xpilot.RadioMessageReceived.color)
  return _internal_color();
}
inline void RadioMessageReceived::_internal_set_color(::PROTOBUF_NAMESPACE_ID::int32 value) {
  _has_bits_[0] |= 0x00000004u;
  color_ = value;
}
inline void RadioMessageReceived::set_color(::PROTOBUF_NAMESPACE_ID::int32 value) {
  _internal_set_color(value);
  // @@protoc_insertion_point(field_set:xpilot.RadioMessageReceived.color)
}

#ifdef __GNUC__
  #pragma GCC diagnostic pop
#endif  // __GNUC__

// @@protoc_insertion_point(namespace_scope)

}  // namespace xpilot

// @@protoc_insertion_point(global_scope)

#include <google/protobuf/port_undef.inc>
#endif  // GOOGLE_PROTOBUF_INCLUDED_GOOGLE_PROTOBUF_INCLUDED_RadioMessageReceived_2eproto
