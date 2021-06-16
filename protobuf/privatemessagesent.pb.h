// Generated by the protocol buffer compiler.  DO NOT EDIT!
// source: PrivateMessageSent.proto

#ifndef GOOGLE_PROTOBUF_INCLUDED_PrivateMessageSent_2eproto
#define GOOGLE_PROTOBUF_INCLUDED_PrivateMessageSent_2eproto

#include <limits>
#include <string>

#include <google/protobuf/port_def.inc>
#if PROTOBUF_VERSION < 3017000
#error This file was generated by a newer version of protoc which is
#error incompatible with your Protocol Buffer headers. Please update
#error your headers.
#endif
#if 3017001 < PROTOBUF_MIN_PROTOC_VERSION
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
#define PROTOBUF_INTERNAL_EXPORT_PrivateMessageSent_2eproto
PROTOBUF_NAMESPACE_OPEN
namespace internal {
class AnyMetadata;
}  // namespace internal
PROTOBUF_NAMESPACE_CLOSE

// Internal implementation detail -- do not use these members.
struct TableStruct_PrivateMessageSent_2eproto {
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
extern const ::PROTOBUF_NAMESPACE_ID::internal::DescriptorTable descriptor_table_PrivateMessageSent_2eproto;
namespace xpilot {
class PrivateMessageSent;
struct PrivateMessageSentDefaultTypeInternal;
extern PrivateMessageSentDefaultTypeInternal _PrivateMessageSent_default_instance_;
}  // namespace xpilot
PROTOBUF_NAMESPACE_OPEN
template<> ::xpilot::PrivateMessageSent* Arena::CreateMaybeMessage<::xpilot::PrivateMessageSent>(Arena*);
PROTOBUF_NAMESPACE_CLOSE
namespace xpilot {

// ===================================================================

class PrivateMessageSent final :
    public ::PROTOBUF_NAMESPACE_ID::Message /* @@protoc_insertion_point(class_definition:xpilot.PrivateMessageSent) */ {
 public:
  inline PrivateMessageSent() : PrivateMessageSent(nullptr) {}
  ~PrivateMessageSent() override;
  explicit constexpr PrivateMessageSent(::PROTOBUF_NAMESPACE_ID::internal::ConstantInitialized);

  PrivateMessageSent(const PrivateMessageSent& from);
  PrivateMessageSent(PrivateMessageSent&& from) noexcept
    : PrivateMessageSent() {
    *this = ::std::move(from);
  }

  inline PrivateMessageSent& operator=(const PrivateMessageSent& from) {
    CopyFrom(from);
    return *this;
  }
  inline PrivateMessageSent& operator=(PrivateMessageSent&& from) noexcept {
    if (this == &from) return *this;
    if (GetOwningArena() == from.GetOwningArena()) {
      InternalSwap(&from);
    } else {
      CopyFrom(from);
    }
    return *this;
  }

  static const ::PROTOBUF_NAMESPACE_ID::Descriptor* descriptor() {
    return GetDescriptor();
  }
  static const ::PROTOBUF_NAMESPACE_ID::Descriptor* GetDescriptor() {
    return default_instance().GetMetadata().descriptor;
  }
  static const ::PROTOBUF_NAMESPACE_ID::Reflection* GetReflection() {
    return default_instance().GetMetadata().reflection;
  }
  static const PrivateMessageSent& default_instance() {
    return *internal_default_instance();
  }
  static inline const PrivateMessageSent* internal_default_instance() {
    return reinterpret_cast<const PrivateMessageSent*>(
               &_PrivateMessageSent_default_instance_);
  }
  static constexpr int kIndexInFileMessages =
    0;

  friend void swap(PrivateMessageSent& a, PrivateMessageSent& b) {
    a.Swap(&b);
  }
  inline void Swap(PrivateMessageSent* other) {
    if (other == this) return;
    if (GetOwningArena() == other->GetOwningArena()) {
      InternalSwap(other);
    } else {
      ::PROTOBUF_NAMESPACE_ID::internal::GenericSwap(this, other);
    }
  }
  void UnsafeArenaSwap(PrivateMessageSent* other) {
    if (other == this) return;
    GOOGLE_DCHECK(GetOwningArena() == other->GetOwningArena());
    InternalSwap(other);
  }

  // implements Message ----------------------------------------------

  inline PrivateMessageSent* New() const final {
    return new PrivateMessageSent();
  }

  PrivateMessageSent* New(::PROTOBUF_NAMESPACE_ID::Arena* arena) const final {
    return CreateMaybeMessage<PrivateMessageSent>(arena);
  }
  void CopyFrom(const ::PROTOBUF_NAMESPACE_ID::Message& from) final;
  void MergeFrom(const ::PROTOBUF_NAMESPACE_ID::Message& from) final;
  void CopyFrom(const PrivateMessageSent& from);
  void MergeFrom(const PrivateMessageSent& from);
  PROTOBUF_ATTRIBUTE_REINITIALIZES void Clear() final;
  bool IsInitialized() const final;

  size_t ByteSizeLong() const final;
  const char* _InternalParse(const char* ptr, ::PROTOBUF_NAMESPACE_ID::internal::ParseContext* ctx) final;
  ::PROTOBUF_NAMESPACE_ID::uint8* _InternalSerialize(
      ::PROTOBUF_NAMESPACE_ID::uint8* target, ::PROTOBUF_NAMESPACE_ID::io::EpsCopyOutputStream* stream) const final;
  int GetCachedSize() const final { return _cached_size_.Get(); }

  private:
  void SharedCtor();
  void SharedDtor();
  void SetCachedSize(int size) const final;
  void InternalSwap(PrivateMessageSent* other);
  friend class ::PROTOBUF_NAMESPACE_ID::internal::AnyMetadata;
  static ::PROTOBUF_NAMESPACE_ID::StringPiece FullMessageName() {
    return "xpilot.PrivateMessageSent";
  }
  protected:
  explicit PrivateMessageSent(::PROTOBUF_NAMESPACE_ID::Arena* arena);
  private:
  static void ArenaDtor(void* object);
  inline void RegisterArenaDtor(::PROTOBUF_NAMESPACE_ID::Arena* arena);
  public:

  ::PROTOBUF_NAMESPACE_ID::Metadata GetMetadata() const final;

  // nested types ----------------------------------------------------

  // accessors -------------------------------------------------------

  enum : int {
    kToFieldNumber = 1,
    kMessageFieldNumber = 2,
  };
  // optional string to = 1;
  bool has_to() const;
  private:
  bool _internal_has_to() const;
  public:
  void clear_to();
  const std::string& to() const;
  template <typename ArgT0 = const std::string&, typename... ArgT>
  void set_to(ArgT0&& arg0, ArgT... args);
  std::string* mutable_to();
  PROTOBUF_FUTURE_MUST_USE_RESULT std::string* release_to();
  void set_allocated_to(std::string* to);
  private:
  const std::string& _internal_to() const;
  inline PROTOBUF_ALWAYS_INLINE void _internal_set_to(const std::string& value);
  std::string* _internal_mutable_to();
  public:

  // optional string message = 2;
  bool has_message() const;
  private:
  bool _internal_has_message() const;
  public:
  void clear_message();
  const std::string& message() const;
  template <typename ArgT0 = const std::string&, typename... ArgT>
  void set_message(ArgT0&& arg0, ArgT... args);
  std::string* mutable_message();
  PROTOBUF_FUTURE_MUST_USE_RESULT std::string* release_message();
  void set_allocated_message(std::string* message);
  private:
  const std::string& _internal_message() const;
  inline PROTOBUF_ALWAYS_INLINE void _internal_set_message(const std::string& value);
  std::string* _internal_mutable_message();
  public:

  // @@protoc_insertion_point(class_scope:xpilot.PrivateMessageSent)
 private:
  class _Internal;

  template <typename T> friend class ::PROTOBUF_NAMESPACE_ID::Arena::InternalHelper;
  typedef void InternalArenaConstructable_;
  typedef void DestructorSkippable_;
  ::PROTOBUF_NAMESPACE_ID::internal::HasBits<1> _has_bits_;
  mutable ::PROTOBUF_NAMESPACE_ID::internal::CachedSize _cached_size_;
  ::PROTOBUF_NAMESPACE_ID::internal::ArenaStringPtr to_;
  ::PROTOBUF_NAMESPACE_ID::internal::ArenaStringPtr message_;
  friend struct ::TableStruct_PrivateMessageSent_2eproto;
};
// ===================================================================


// ===================================================================

#ifdef __GNUC__
  #pragma GCC diagnostic push
  #pragma GCC diagnostic ignored "-Wstrict-aliasing"
#endif  // __GNUC__
// PrivateMessageSent

// optional string to = 1;
inline bool PrivateMessageSent::_internal_has_to() const {
  bool value = (_has_bits_[0] & 0x00000001u) != 0;
  return value;
}
inline bool PrivateMessageSent::has_to() const {
  return _internal_has_to();
}
inline void PrivateMessageSent::clear_to() {
  to_.ClearToEmpty();
  _has_bits_[0] &= ~0x00000001u;
}
inline const std::string& PrivateMessageSent::to() const {
  // @@protoc_insertion_point(field_get:xpilot.PrivateMessageSent.to)
  return _internal_to();
}
template <typename ArgT0, typename... ArgT>
inline PROTOBUF_ALWAYS_INLINE
void PrivateMessageSent::set_to(ArgT0&& arg0, ArgT... args) {
 _has_bits_[0] |= 0x00000001u;
 to_.Set(::PROTOBUF_NAMESPACE_ID::internal::ArenaStringPtr::EmptyDefault{}, static_cast<ArgT0 &&>(arg0), args..., GetArenaForAllocation());
  // @@protoc_insertion_point(field_set:xpilot.PrivateMessageSent.to)
}
inline std::string* PrivateMessageSent::mutable_to() {
  // @@protoc_insertion_point(field_mutable:xpilot.PrivateMessageSent.to)
  return _internal_mutable_to();
}
inline const std::string& PrivateMessageSent::_internal_to() const {
  return to_.Get();
}
inline void PrivateMessageSent::_internal_set_to(const std::string& value) {
  _has_bits_[0] |= 0x00000001u;
  to_.Set(::PROTOBUF_NAMESPACE_ID::internal::ArenaStringPtr::EmptyDefault{}, value, GetArenaForAllocation());
}
inline std::string* PrivateMessageSent::_internal_mutable_to() {
  _has_bits_[0] |= 0x00000001u;
  return to_.Mutable(::PROTOBUF_NAMESPACE_ID::internal::ArenaStringPtr::EmptyDefault{}, GetArenaForAllocation());
}
inline std::string* PrivateMessageSent::release_to() {
  // @@protoc_insertion_point(field_release:xpilot.PrivateMessageSent.to)
  if (!_internal_has_to()) {
    return nullptr;
  }
  _has_bits_[0] &= ~0x00000001u;
  return to_.ReleaseNonDefault(&::PROTOBUF_NAMESPACE_ID::internal::GetEmptyStringAlreadyInited(), GetArenaForAllocation());
}
inline void PrivateMessageSent::set_allocated_to(std::string* to) {
  if (to != nullptr) {
    _has_bits_[0] |= 0x00000001u;
  } else {
    _has_bits_[0] &= ~0x00000001u;
  }
  to_.SetAllocated(&::PROTOBUF_NAMESPACE_ID::internal::GetEmptyStringAlreadyInited(), to,
      GetArenaForAllocation());
  // @@protoc_insertion_point(field_set_allocated:xpilot.PrivateMessageSent.to)
}

// optional string message = 2;
inline bool PrivateMessageSent::_internal_has_message() const {
  bool value = (_has_bits_[0] & 0x00000002u) != 0;
  return value;
}
inline bool PrivateMessageSent::has_message() const {
  return _internal_has_message();
}
inline void PrivateMessageSent::clear_message() {
  message_.ClearToEmpty();
  _has_bits_[0] &= ~0x00000002u;
}
inline const std::string& PrivateMessageSent::message() const {
  // @@protoc_insertion_point(field_get:xpilot.PrivateMessageSent.message)
  return _internal_message();
}
template <typename ArgT0, typename... ArgT>
inline PROTOBUF_ALWAYS_INLINE
void PrivateMessageSent::set_message(ArgT0&& arg0, ArgT... args) {
 _has_bits_[0] |= 0x00000002u;
 message_.Set(::PROTOBUF_NAMESPACE_ID::internal::ArenaStringPtr::EmptyDefault{}, static_cast<ArgT0 &&>(arg0), args..., GetArenaForAllocation());
  // @@protoc_insertion_point(field_set:xpilot.PrivateMessageSent.message)
}
inline std::string* PrivateMessageSent::mutable_message() {
  // @@protoc_insertion_point(field_mutable:xpilot.PrivateMessageSent.message)
  return _internal_mutable_message();
}
inline const std::string& PrivateMessageSent::_internal_message() const {
  return message_.Get();
}
inline void PrivateMessageSent::_internal_set_message(const std::string& value) {
  _has_bits_[0] |= 0x00000002u;
  message_.Set(::PROTOBUF_NAMESPACE_ID::internal::ArenaStringPtr::EmptyDefault{}, value, GetArenaForAllocation());
}
inline std::string* PrivateMessageSent::_internal_mutable_message() {
  _has_bits_[0] |= 0x00000002u;
  return message_.Mutable(::PROTOBUF_NAMESPACE_ID::internal::ArenaStringPtr::EmptyDefault{}, GetArenaForAllocation());
}
inline std::string* PrivateMessageSent::release_message() {
  // @@protoc_insertion_point(field_release:xpilot.PrivateMessageSent.message)
  if (!_internal_has_message()) {
    return nullptr;
  }
  _has_bits_[0] &= ~0x00000002u;
  return message_.ReleaseNonDefault(&::PROTOBUF_NAMESPACE_ID::internal::GetEmptyStringAlreadyInited(), GetArenaForAllocation());
}
inline void PrivateMessageSent::set_allocated_message(std::string* message) {
  if (message != nullptr) {
    _has_bits_[0] |= 0x00000002u;
  } else {
    _has_bits_[0] &= ~0x00000002u;
  }
  message_.SetAllocated(&::PROTOBUF_NAMESPACE_ID::internal::GetEmptyStringAlreadyInited(), message,
      GetArenaForAllocation());
  // @@protoc_insertion_point(field_set_allocated:xpilot.PrivateMessageSent.message)
}

#ifdef __GNUC__
  #pragma GCC diagnostic pop
#endif  // __GNUC__

// @@protoc_insertion_point(namespace_scope)

}  // namespace xpilot

// @@protoc_insertion_point(global_scope)

#include <google/protobuf/port_undef.inc>
#endif  // GOOGLE_PROTOBUF_INCLUDED_GOOGLE_PROTOBUF_INCLUDED_PrivateMessageSent_2eproto
