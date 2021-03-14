// Generated by the protocol buffer compiler.  DO NOT EDIT!
// source: RadioStack.proto

#ifndef GOOGLE_PROTOBUF_INCLUDED_RadioStack_2eproto
#define GOOGLE_PROTOBUF_INCLUDED_RadioStack_2eproto

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
#define PROTOBUF_INTERNAL_EXPORT_RadioStack_2eproto
PROTOBUF_NAMESPACE_OPEN
namespace internal {
class AnyMetadata;
}  // namespace internal
PROTOBUF_NAMESPACE_CLOSE

// Internal implementation detail -- do not use these members.
struct TableStruct_RadioStack_2eproto {
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
extern const ::PROTOBUF_NAMESPACE_ID::internal::DescriptorTable descriptor_table_RadioStack_2eproto;
::PROTOBUF_NAMESPACE_ID::Metadata descriptor_table_RadioStack_2eproto_metadata_getter(int index);
namespace xpilot {
class RadioStack;
struct RadioStackDefaultTypeInternal;
extern RadioStackDefaultTypeInternal _RadioStack_default_instance_;
}  // namespace xpilot
PROTOBUF_NAMESPACE_OPEN
template<> ::xpilot::RadioStack* Arena::CreateMaybeMessage<::xpilot::RadioStack>(Arena*);
PROTOBUF_NAMESPACE_CLOSE
namespace xpilot {

// ===================================================================

class RadioStack PROTOBUF_FINAL :
    public ::PROTOBUF_NAMESPACE_ID::Message /* @@protoc_insertion_point(class_definition:xpilot.RadioStack) */ {
 public:
  inline RadioStack() : RadioStack(nullptr) {}
  virtual ~RadioStack();
  explicit constexpr RadioStack(::PROTOBUF_NAMESPACE_ID::internal::ConstantInitialized);

  RadioStack(const RadioStack& from);
  RadioStack(RadioStack&& from) noexcept
    : RadioStack() {
    *this = ::std::move(from);
  }

  inline RadioStack& operator=(const RadioStack& from) {
    CopyFrom(from);
    return *this;
  }
  inline RadioStack& operator=(RadioStack&& from) noexcept {
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
  static const RadioStack& default_instance() {
    return *internal_default_instance();
  }
  static inline const RadioStack* internal_default_instance() {
    return reinterpret_cast<const RadioStack*>(
               &_RadioStack_default_instance_);
  }
  static constexpr int kIndexInFileMessages =
    0;

  friend void swap(RadioStack& a, RadioStack& b) {
    a.Swap(&b);
  }
  inline void Swap(RadioStack* other) {
    if (other == this) return;
    if (GetArena() == other->GetArena()) {
      InternalSwap(other);
    } else {
      ::PROTOBUF_NAMESPACE_ID::internal::GenericSwap(this, other);
    }
  }
  void UnsafeArenaSwap(RadioStack* other) {
    if (other == this) return;
    GOOGLE_DCHECK(GetArena() == other->GetArena());
    InternalSwap(other);
  }

  // implements Message ----------------------------------------------

  inline RadioStack* New() const final {
    return CreateMaybeMessage<RadioStack>(nullptr);
  }

  RadioStack* New(::PROTOBUF_NAMESPACE_ID::Arena* arena) const final {
    return CreateMaybeMessage<RadioStack>(arena);
  }
  void CopyFrom(const ::PROTOBUF_NAMESPACE_ID::Message& from) final;
  void MergeFrom(const ::PROTOBUF_NAMESPACE_ID::Message& from) final;
  void CopyFrom(const RadioStack& from);
  void MergeFrom(const RadioStack& from);
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
  void InternalSwap(RadioStack* other);
  friend class ::PROTOBUF_NAMESPACE_ID::internal::AnyMetadata;
  static ::PROTOBUF_NAMESPACE_ID::StringPiece FullMessageName() {
    return "xpilot.RadioStack";
  }
  protected:
  explicit RadioStack(::PROTOBUF_NAMESPACE_ID::Arena* arena);
  private:
  static void ArenaDtor(void* object);
  inline void RegisterArenaDtor(::PROTOBUF_NAMESPACE_ID::Arena* arena);
  public:

  ::PROTOBUF_NAMESPACE_ID::Metadata GetMetadata() const final;
  private:
  static ::PROTOBUF_NAMESPACE_ID::Metadata GetMetadataStatic() {
    return ::descriptor_table_RadioStack_2eproto_metadata_getter(kIndexInFileMessages);
  }

  public:

  // nested types ----------------------------------------------------

  // accessors -------------------------------------------------------

  enum : int {
    kAudioComSelectionFieldNumber = 1,
    kCom1FreqFieldNumber = 3,
    kCom1VolumeFieldNumber = 5,
    kCom1PowerFieldNumber = 2,
    kCom1AudioSelectionFieldNumber = 4,
    kCom2PowerFieldNumber = 6,
    kCom2AudioSelectionFieldNumber = 8,
    kCom2FreqFieldNumber = 7,
    kCom2VolumeFieldNumber = 9,
    kTransponderCodeFieldNumber = 11,
    kAvionicsPowerOnFieldNumber = 10,
    kTransponderIdentFieldNumber = 13,
    kTransponderModeFieldNumber = 12,
  };
  // int32 audio_com_selection = 1;
  bool has_audio_com_selection() const;
  private:
  bool _internal_has_audio_com_selection() const;
  public:
  void clear_audio_com_selection();
  ::PROTOBUF_NAMESPACE_ID::int32 audio_com_selection() const;
  void set_audio_com_selection(::PROTOBUF_NAMESPACE_ID::int32 value);
  private:
  ::PROTOBUF_NAMESPACE_ID::int32 _internal_audio_com_selection() const;
  void _internal_set_audio_com_selection(::PROTOBUF_NAMESPACE_ID::int32 value);
  public:

  // int32 com1_freq = 3;
  bool has_com1_freq() const;
  private:
  bool _internal_has_com1_freq() const;
  public:
  void clear_com1_freq();
  ::PROTOBUF_NAMESPACE_ID::int32 com1_freq() const;
  void set_com1_freq(::PROTOBUF_NAMESPACE_ID::int32 value);
  private:
  ::PROTOBUF_NAMESPACE_ID::int32 _internal_com1_freq() const;
  void _internal_set_com1_freq(::PROTOBUF_NAMESPACE_ID::int32 value);
  public:

  // float com1_volume = 5;
  bool has_com1_volume() const;
  private:
  bool _internal_has_com1_volume() const;
  public:
  void clear_com1_volume();
  float com1_volume() const;
  void set_com1_volume(float value);
  private:
  float _internal_com1_volume() const;
  void _internal_set_com1_volume(float value);
  public:

  // bool com1_power = 2;
  bool has_com1_power() const;
  private:
  bool _internal_has_com1_power() const;
  public:
  void clear_com1_power();
  bool com1_power() const;
  void set_com1_power(bool value);
  private:
  bool _internal_com1_power() const;
  void _internal_set_com1_power(bool value);
  public:

  // bool com1_audio_selection = 4;
  bool has_com1_audio_selection() const;
  private:
  bool _internal_has_com1_audio_selection() const;
  public:
  void clear_com1_audio_selection();
  bool com1_audio_selection() const;
  void set_com1_audio_selection(bool value);
  private:
  bool _internal_com1_audio_selection() const;
  void _internal_set_com1_audio_selection(bool value);
  public:

  // bool com2_power = 6;
  bool has_com2_power() const;
  private:
  bool _internal_has_com2_power() const;
  public:
  void clear_com2_power();
  bool com2_power() const;
  void set_com2_power(bool value);
  private:
  bool _internal_com2_power() const;
  void _internal_set_com2_power(bool value);
  public:

  // bool com2_audio_selection = 8;
  bool has_com2_audio_selection() const;
  private:
  bool _internal_has_com2_audio_selection() const;
  public:
  void clear_com2_audio_selection();
  bool com2_audio_selection() const;
  void set_com2_audio_selection(bool value);
  private:
  bool _internal_com2_audio_selection() const;
  void _internal_set_com2_audio_selection(bool value);
  public:

  // int32 com2_freq = 7;
  bool has_com2_freq() const;
  private:
  bool _internal_has_com2_freq() const;
  public:
  void clear_com2_freq();
  ::PROTOBUF_NAMESPACE_ID::int32 com2_freq() const;
  void set_com2_freq(::PROTOBUF_NAMESPACE_ID::int32 value);
  private:
  ::PROTOBUF_NAMESPACE_ID::int32 _internal_com2_freq() const;
  void _internal_set_com2_freq(::PROTOBUF_NAMESPACE_ID::int32 value);
  public:

  // float com2_volume = 9;
  bool has_com2_volume() const;
  private:
  bool _internal_has_com2_volume() const;
  public:
  void clear_com2_volume();
  float com2_volume() const;
  void set_com2_volume(float value);
  private:
  float _internal_com2_volume() const;
  void _internal_set_com2_volume(float value);
  public:

  // int32 transponder_code = 11;
  bool has_transponder_code() const;
  private:
  bool _internal_has_transponder_code() const;
  public:
  void clear_transponder_code();
  ::PROTOBUF_NAMESPACE_ID::int32 transponder_code() const;
  void set_transponder_code(::PROTOBUF_NAMESPACE_ID::int32 value);
  private:
  ::PROTOBUF_NAMESPACE_ID::int32 _internal_transponder_code() const;
  void _internal_set_transponder_code(::PROTOBUF_NAMESPACE_ID::int32 value);
  public:

  // bool avionics_power_on = 10;
  bool has_avionics_power_on() const;
  private:
  bool _internal_has_avionics_power_on() const;
  public:
  void clear_avionics_power_on();
  bool avionics_power_on() const;
  void set_avionics_power_on(bool value);
  private:
  bool _internal_avionics_power_on() const;
  void _internal_set_avionics_power_on(bool value);
  public:

  // bool transponder_ident = 13;
  bool has_transponder_ident() const;
  private:
  bool _internal_has_transponder_ident() const;
  public:
  void clear_transponder_ident();
  bool transponder_ident() const;
  void set_transponder_ident(bool value);
  private:
  bool _internal_transponder_ident() const;
  void _internal_set_transponder_ident(bool value);
  public:

  // int32 transponder_mode = 12;
  bool has_transponder_mode() const;
  private:
  bool _internal_has_transponder_mode() const;
  public:
  void clear_transponder_mode();
  ::PROTOBUF_NAMESPACE_ID::int32 transponder_mode() const;
  void set_transponder_mode(::PROTOBUF_NAMESPACE_ID::int32 value);
  private:
  ::PROTOBUF_NAMESPACE_ID::int32 _internal_transponder_mode() const;
  void _internal_set_transponder_mode(::PROTOBUF_NAMESPACE_ID::int32 value);
  public:

  // @@protoc_insertion_point(class_scope:xpilot.RadioStack)
 private:
  class _Internal;

  template <typename T> friend class ::PROTOBUF_NAMESPACE_ID::Arena::InternalHelper;
  typedef void InternalArenaConstructable_;
  typedef void DestructorSkippable_;
  ::PROTOBUF_NAMESPACE_ID::internal::HasBits<1> _has_bits_;
  mutable ::PROTOBUF_NAMESPACE_ID::internal::CachedSize _cached_size_;
  ::PROTOBUF_NAMESPACE_ID::int32 audio_com_selection_;
  ::PROTOBUF_NAMESPACE_ID::int32 com1_freq_;
  float com1_volume_;
  bool com1_power_;
  bool com1_audio_selection_;
  bool com2_power_;
  bool com2_audio_selection_;
  ::PROTOBUF_NAMESPACE_ID::int32 com2_freq_;
  float com2_volume_;
  ::PROTOBUF_NAMESPACE_ID::int32 transponder_code_;
  bool avionics_power_on_;
  bool transponder_ident_;
  ::PROTOBUF_NAMESPACE_ID::int32 transponder_mode_;
  friend struct ::TableStruct_RadioStack_2eproto;
};
// ===================================================================


// ===================================================================

#ifdef __GNUC__
  #pragma GCC diagnostic push
  #pragma GCC diagnostic ignored "-Wstrict-aliasing"
#endif  // __GNUC__
// RadioStack

// int32 audio_com_selection = 1;
inline bool RadioStack::_internal_has_audio_com_selection() const {
  bool value = (_has_bits_[0] & 0x00000001u) != 0;
  return value;
}
inline bool RadioStack::has_audio_com_selection() const {
  return _internal_has_audio_com_selection();
}
inline void RadioStack::clear_audio_com_selection() {
  audio_com_selection_ = 0;
  _has_bits_[0] &= ~0x00000001u;
}
inline ::PROTOBUF_NAMESPACE_ID::int32 RadioStack::_internal_audio_com_selection() const {
  return audio_com_selection_;
}
inline ::PROTOBUF_NAMESPACE_ID::int32 RadioStack::audio_com_selection() const {
  // @@protoc_insertion_point(field_get:xpilot.RadioStack.audio_com_selection)
  return _internal_audio_com_selection();
}
inline void RadioStack::_internal_set_audio_com_selection(::PROTOBUF_NAMESPACE_ID::int32 value) {
  _has_bits_[0] |= 0x00000001u;
  audio_com_selection_ = value;
}
inline void RadioStack::set_audio_com_selection(::PROTOBUF_NAMESPACE_ID::int32 value) {
  _internal_set_audio_com_selection(value);
  // @@protoc_insertion_point(field_set:xpilot.RadioStack.audio_com_selection)
}

// bool com1_power = 2;
inline bool RadioStack::_internal_has_com1_power() const {
  bool value = (_has_bits_[0] & 0x00000008u) != 0;
  return value;
}
inline bool RadioStack::has_com1_power() const {
  return _internal_has_com1_power();
}
inline void RadioStack::clear_com1_power() {
  com1_power_ = false;
  _has_bits_[0] &= ~0x00000008u;
}
inline bool RadioStack::_internal_com1_power() const {
  return com1_power_;
}
inline bool RadioStack::com1_power() const {
  // @@protoc_insertion_point(field_get:xpilot.RadioStack.com1_power)
  return _internal_com1_power();
}
inline void RadioStack::_internal_set_com1_power(bool value) {
  _has_bits_[0] |= 0x00000008u;
  com1_power_ = value;
}
inline void RadioStack::set_com1_power(bool value) {
  _internal_set_com1_power(value);
  // @@protoc_insertion_point(field_set:xpilot.RadioStack.com1_power)
}

// int32 com1_freq = 3;
inline bool RadioStack::_internal_has_com1_freq() const {
  bool value = (_has_bits_[0] & 0x00000002u) != 0;
  return value;
}
inline bool RadioStack::has_com1_freq() const {
  return _internal_has_com1_freq();
}
inline void RadioStack::clear_com1_freq() {
  com1_freq_ = 0;
  _has_bits_[0] &= ~0x00000002u;
}
inline ::PROTOBUF_NAMESPACE_ID::int32 RadioStack::_internal_com1_freq() const {
  return com1_freq_;
}
inline ::PROTOBUF_NAMESPACE_ID::int32 RadioStack::com1_freq() const {
  // @@protoc_insertion_point(field_get:xpilot.RadioStack.com1_freq)
  return _internal_com1_freq();
}
inline void RadioStack::_internal_set_com1_freq(::PROTOBUF_NAMESPACE_ID::int32 value) {
  _has_bits_[0] |= 0x00000002u;
  com1_freq_ = value;
}
inline void RadioStack::set_com1_freq(::PROTOBUF_NAMESPACE_ID::int32 value) {
  _internal_set_com1_freq(value);
  // @@protoc_insertion_point(field_set:xpilot.RadioStack.com1_freq)
}

// bool com1_audio_selection = 4;
inline bool RadioStack::_internal_has_com1_audio_selection() const {
  bool value = (_has_bits_[0] & 0x00000010u) != 0;
  return value;
}
inline bool RadioStack::has_com1_audio_selection() const {
  return _internal_has_com1_audio_selection();
}
inline void RadioStack::clear_com1_audio_selection() {
  com1_audio_selection_ = false;
  _has_bits_[0] &= ~0x00000010u;
}
inline bool RadioStack::_internal_com1_audio_selection() const {
  return com1_audio_selection_;
}
inline bool RadioStack::com1_audio_selection() const {
  // @@protoc_insertion_point(field_get:xpilot.RadioStack.com1_audio_selection)
  return _internal_com1_audio_selection();
}
inline void RadioStack::_internal_set_com1_audio_selection(bool value) {
  _has_bits_[0] |= 0x00000010u;
  com1_audio_selection_ = value;
}
inline void RadioStack::set_com1_audio_selection(bool value) {
  _internal_set_com1_audio_selection(value);
  // @@protoc_insertion_point(field_set:xpilot.RadioStack.com1_audio_selection)
}

// float com1_volume = 5;
inline bool RadioStack::_internal_has_com1_volume() const {
  bool value = (_has_bits_[0] & 0x00000004u) != 0;
  return value;
}
inline bool RadioStack::has_com1_volume() const {
  return _internal_has_com1_volume();
}
inline void RadioStack::clear_com1_volume() {
  com1_volume_ = 0;
  _has_bits_[0] &= ~0x00000004u;
}
inline float RadioStack::_internal_com1_volume() const {
  return com1_volume_;
}
inline float RadioStack::com1_volume() const {
  // @@protoc_insertion_point(field_get:xpilot.RadioStack.com1_volume)
  return _internal_com1_volume();
}
inline void RadioStack::_internal_set_com1_volume(float value) {
  _has_bits_[0] |= 0x00000004u;
  com1_volume_ = value;
}
inline void RadioStack::set_com1_volume(float value) {
  _internal_set_com1_volume(value);
  // @@protoc_insertion_point(field_set:xpilot.RadioStack.com1_volume)
}

// bool com2_power = 6;
inline bool RadioStack::_internal_has_com2_power() const {
  bool value = (_has_bits_[0] & 0x00000020u) != 0;
  return value;
}
inline bool RadioStack::has_com2_power() const {
  return _internal_has_com2_power();
}
inline void RadioStack::clear_com2_power() {
  com2_power_ = false;
  _has_bits_[0] &= ~0x00000020u;
}
inline bool RadioStack::_internal_com2_power() const {
  return com2_power_;
}
inline bool RadioStack::com2_power() const {
  // @@protoc_insertion_point(field_get:xpilot.RadioStack.com2_power)
  return _internal_com2_power();
}
inline void RadioStack::_internal_set_com2_power(bool value) {
  _has_bits_[0] |= 0x00000020u;
  com2_power_ = value;
}
inline void RadioStack::set_com2_power(bool value) {
  _internal_set_com2_power(value);
  // @@protoc_insertion_point(field_set:xpilot.RadioStack.com2_power)
}

// int32 com2_freq = 7;
inline bool RadioStack::_internal_has_com2_freq() const {
  bool value = (_has_bits_[0] & 0x00000080u) != 0;
  return value;
}
inline bool RadioStack::has_com2_freq() const {
  return _internal_has_com2_freq();
}
inline void RadioStack::clear_com2_freq() {
  com2_freq_ = 0;
  _has_bits_[0] &= ~0x00000080u;
}
inline ::PROTOBUF_NAMESPACE_ID::int32 RadioStack::_internal_com2_freq() const {
  return com2_freq_;
}
inline ::PROTOBUF_NAMESPACE_ID::int32 RadioStack::com2_freq() const {
  // @@protoc_insertion_point(field_get:xpilot.RadioStack.com2_freq)
  return _internal_com2_freq();
}
inline void RadioStack::_internal_set_com2_freq(::PROTOBUF_NAMESPACE_ID::int32 value) {
  _has_bits_[0] |= 0x00000080u;
  com2_freq_ = value;
}
inline void RadioStack::set_com2_freq(::PROTOBUF_NAMESPACE_ID::int32 value) {
  _internal_set_com2_freq(value);
  // @@protoc_insertion_point(field_set:xpilot.RadioStack.com2_freq)
}

// bool com2_audio_selection = 8;
inline bool RadioStack::_internal_has_com2_audio_selection() const {
  bool value = (_has_bits_[0] & 0x00000040u) != 0;
  return value;
}
inline bool RadioStack::has_com2_audio_selection() const {
  return _internal_has_com2_audio_selection();
}
inline void RadioStack::clear_com2_audio_selection() {
  com2_audio_selection_ = false;
  _has_bits_[0] &= ~0x00000040u;
}
inline bool RadioStack::_internal_com2_audio_selection() const {
  return com2_audio_selection_;
}
inline bool RadioStack::com2_audio_selection() const {
  // @@protoc_insertion_point(field_get:xpilot.RadioStack.com2_audio_selection)
  return _internal_com2_audio_selection();
}
inline void RadioStack::_internal_set_com2_audio_selection(bool value) {
  _has_bits_[0] |= 0x00000040u;
  com2_audio_selection_ = value;
}
inline void RadioStack::set_com2_audio_selection(bool value) {
  _internal_set_com2_audio_selection(value);
  // @@protoc_insertion_point(field_set:xpilot.RadioStack.com2_audio_selection)
}

// float com2_volume = 9;
inline bool RadioStack::_internal_has_com2_volume() const {
  bool value = (_has_bits_[0] & 0x00000100u) != 0;
  return value;
}
inline bool RadioStack::has_com2_volume() const {
  return _internal_has_com2_volume();
}
inline void RadioStack::clear_com2_volume() {
  com2_volume_ = 0;
  _has_bits_[0] &= ~0x00000100u;
}
inline float RadioStack::_internal_com2_volume() const {
  return com2_volume_;
}
inline float RadioStack::com2_volume() const {
  // @@protoc_insertion_point(field_get:xpilot.RadioStack.com2_volume)
  return _internal_com2_volume();
}
inline void RadioStack::_internal_set_com2_volume(float value) {
  _has_bits_[0] |= 0x00000100u;
  com2_volume_ = value;
}
inline void RadioStack::set_com2_volume(float value) {
  _internal_set_com2_volume(value);
  // @@protoc_insertion_point(field_set:xpilot.RadioStack.com2_volume)
}

// bool avionics_power_on = 10;
inline bool RadioStack::_internal_has_avionics_power_on() const {
  bool value = (_has_bits_[0] & 0x00000400u) != 0;
  return value;
}
inline bool RadioStack::has_avionics_power_on() const {
  return _internal_has_avionics_power_on();
}
inline void RadioStack::clear_avionics_power_on() {
  avionics_power_on_ = false;
  _has_bits_[0] &= ~0x00000400u;
}
inline bool RadioStack::_internal_avionics_power_on() const {
  return avionics_power_on_;
}
inline bool RadioStack::avionics_power_on() const {
  // @@protoc_insertion_point(field_get:xpilot.RadioStack.avionics_power_on)
  return _internal_avionics_power_on();
}
inline void RadioStack::_internal_set_avionics_power_on(bool value) {
  _has_bits_[0] |= 0x00000400u;
  avionics_power_on_ = value;
}
inline void RadioStack::set_avionics_power_on(bool value) {
  _internal_set_avionics_power_on(value);
  // @@protoc_insertion_point(field_set:xpilot.RadioStack.avionics_power_on)
}

// int32 transponder_code = 11;
inline bool RadioStack::_internal_has_transponder_code() const {
  bool value = (_has_bits_[0] & 0x00000200u) != 0;
  return value;
}
inline bool RadioStack::has_transponder_code() const {
  return _internal_has_transponder_code();
}
inline void RadioStack::clear_transponder_code() {
  transponder_code_ = 0;
  _has_bits_[0] &= ~0x00000200u;
}
inline ::PROTOBUF_NAMESPACE_ID::int32 RadioStack::_internal_transponder_code() const {
  return transponder_code_;
}
inline ::PROTOBUF_NAMESPACE_ID::int32 RadioStack::transponder_code() const {
  // @@protoc_insertion_point(field_get:xpilot.RadioStack.transponder_code)
  return _internal_transponder_code();
}
inline void RadioStack::_internal_set_transponder_code(::PROTOBUF_NAMESPACE_ID::int32 value) {
  _has_bits_[0] |= 0x00000200u;
  transponder_code_ = value;
}
inline void RadioStack::set_transponder_code(::PROTOBUF_NAMESPACE_ID::int32 value) {
  _internal_set_transponder_code(value);
  // @@protoc_insertion_point(field_set:xpilot.RadioStack.transponder_code)
}

// int32 transponder_mode = 12;
inline bool RadioStack::_internal_has_transponder_mode() const {
  bool value = (_has_bits_[0] & 0x00001000u) != 0;
  return value;
}
inline bool RadioStack::has_transponder_mode() const {
  return _internal_has_transponder_mode();
}
inline void RadioStack::clear_transponder_mode() {
  transponder_mode_ = 0;
  _has_bits_[0] &= ~0x00001000u;
}
inline ::PROTOBUF_NAMESPACE_ID::int32 RadioStack::_internal_transponder_mode() const {
  return transponder_mode_;
}
inline ::PROTOBUF_NAMESPACE_ID::int32 RadioStack::transponder_mode() const {
  // @@protoc_insertion_point(field_get:xpilot.RadioStack.transponder_mode)
  return _internal_transponder_mode();
}
inline void RadioStack::_internal_set_transponder_mode(::PROTOBUF_NAMESPACE_ID::int32 value) {
  _has_bits_[0] |= 0x00001000u;
  transponder_mode_ = value;
}
inline void RadioStack::set_transponder_mode(::PROTOBUF_NAMESPACE_ID::int32 value) {
  _internal_set_transponder_mode(value);
  // @@protoc_insertion_point(field_set:xpilot.RadioStack.transponder_mode)
}

// bool transponder_ident = 13;
inline bool RadioStack::_internal_has_transponder_ident() const {
  bool value = (_has_bits_[0] & 0x00000800u) != 0;
  return value;
}
inline bool RadioStack::has_transponder_ident() const {
  return _internal_has_transponder_ident();
}
inline void RadioStack::clear_transponder_ident() {
  transponder_ident_ = false;
  _has_bits_[0] &= ~0x00000800u;
}
inline bool RadioStack::_internal_transponder_ident() const {
  return transponder_ident_;
}
inline bool RadioStack::transponder_ident() const {
  // @@protoc_insertion_point(field_get:xpilot.RadioStack.transponder_ident)
  return _internal_transponder_ident();
}
inline void RadioStack::_internal_set_transponder_ident(bool value) {
  _has_bits_[0] |= 0x00000800u;
  transponder_ident_ = value;
}
inline void RadioStack::set_transponder_ident(bool value) {
  _internal_set_transponder_ident(value);
  // @@protoc_insertion_point(field_set:xpilot.RadioStack.transponder_ident)
}

#ifdef __GNUC__
  #pragma GCC diagnostic pop
#endif  // __GNUC__

// @@protoc_insertion_point(namespace_scope)

}  // namespace xpilot

// @@protoc_insertion_point(global_scope)

#include <google/protobuf/port_undef.inc>
#endif  // GOOGLE_PROTOBUF_INCLUDED_GOOGLE_PROTOBUF_INCLUDED_RadioStack_2eproto
