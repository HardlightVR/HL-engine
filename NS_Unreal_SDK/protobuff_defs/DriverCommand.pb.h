// Generated by the protocol buffer compiler.  DO NOT EDIT!
// source: DriverCommand.proto

#ifndef PROTOBUF_DriverCommand_2eproto__INCLUDED
#define PROTOBUF_DriverCommand_2eproto__INCLUDED

#include <string>

#include <google/protobuf/stubs/common.h>

#if GOOGLE_PROTOBUF_VERSION < 3000000
#error This file was generated by a newer version of protoc which is
#error incompatible with your Protocol Buffer headers.  Please update
#error your headers.
#endif
#if 3000000 < GOOGLE_PROTOBUF_MIN_PROTOC_VERSION
#error This file was generated by an older version of protoc which is
#error incompatible with your Protocol Buffer headers.  Please
#error regenerate this file with a newer version of protoc.
#endif

#include <google/protobuf/arena.h>
#include <google/protobuf/arenastring.h>
#include <google/protobuf/generated_message_util.h>
#include <google/protobuf/metadata.h>
#include <google/protobuf/message.h>
#include <google/protobuf/repeated_field.h>
#include <google/protobuf/extension_set.h>
#include <google/protobuf/map.h>
#include <google/protobuf/map_field_inl.h>
#include <google/protobuf/generated_enum_reflection.h>
#include <google/protobuf/unknown_field_set.h>
// @@protoc_insertion_point(includes)

namespace NullSpaceIPC {

// Internal implementation detail -- do not call these.
void protobuf_AddDesc_DriverCommand_2eproto();
void protobuf_AssignDesc_DriverCommand_2eproto();
void protobuf_ShutdownFile_DriverCommand_2eproto();

class DriverCommand;

enum DriverCommand_Command {
  DriverCommand_Command_UNKNOWN = 0,
  DriverCommand_Command_ENABLE_TRACKING = 1,
  DriverCommand_Command_DISABLE_TRACKING = 2,
  DriverCommand_Command_ENABLE_AUDIO = 3,
  DriverCommand_Command_DISABLE_AUDIO = 4,
  DriverCommand_Command_RAW_COMMAND = 5,
  DriverCommand_Command_DUMP_DEVICE_DIAGNOSTICS = 6,
  DriverCommand_Command_DriverCommand_Command_INT_MIN_SENTINEL_DO_NOT_USE_ = ::google::protobuf::kint32min,
  DriverCommand_Command_DriverCommand_Command_INT_MAX_SENTINEL_DO_NOT_USE_ = ::google::protobuf::kint32max
};
bool DriverCommand_Command_IsValid(int value);
const DriverCommand_Command DriverCommand_Command_Command_MIN = DriverCommand_Command_UNKNOWN;
const DriverCommand_Command DriverCommand_Command_Command_MAX = DriverCommand_Command_DUMP_DEVICE_DIAGNOSTICS;
const int DriverCommand_Command_Command_ARRAYSIZE = DriverCommand_Command_Command_MAX + 1;

const ::google::protobuf::EnumDescriptor* DriverCommand_Command_descriptor();
inline const ::std::string& DriverCommand_Command_Name(DriverCommand_Command value) {
  return ::google::protobuf::internal::NameOfEnum(
    DriverCommand_Command_descriptor(), value);
}
inline bool DriverCommand_Command_Parse(
    const ::std::string& name, DriverCommand_Command* value) {
  return ::google::protobuf::internal::ParseNamedEnum<DriverCommand_Command>(
    DriverCommand_Command_descriptor(), name, value);
}
// ===================================================================

class DriverCommand : public ::google::protobuf::Message /* @@protoc_insertion_point(class_definition:NullSpaceIPC.DriverCommand) */ {
 public:
  DriverCommand();
  virtual ~DriverCommand();

  DriverCommand(const DriverCommand& from);

  inline DriverCommand& operator=(const DriverCommand& from) {
    CopyFrom(from);
    return *this;
  }

  static const ::google::protobuf::Descriptor* descriptor();
  static const DriverCommand& default_instance();

  void Swap(DriverCommand* other);

  // implements Message ----------------------------------------------

  inline DriverCommand* New() const { return New(NULL); }

  DriverCommand* New(::google::protobuf::Arena* arena) const;
  void CopyFrom(const ::google::protobuf::Message& from);
  void MergeFrom(const ::google::protobuf::Message& from);
  void CopyFrom(const DriverCommand& from);
  void MergeFrom(const DriverCommand& from);
  void Clear();
  bool IsInitialized() const;

  int ByteSize() const;
  bool MergePartialFromCodedStream(
      ::google::protobuf::io::CodedInputStream* input);
  void SerializeWithCachedSizes(
      ::google::protobuf::io::CodedOutputStream* output) const;
  ::google::protobuf::uint8* InternalSerializeWithCachedSizesToArray(
      bool deterministic, ::google::protobuf::uint8* output) const;
  ::google::protobuf::uint8* SerializeWithCachedSizesToArray(::google::protobuf::uint8* output) const {
    return InternalSerializeWithCachedSizesToArray(false, output);
  }
  int GetCachedSize() const { return _cached_size_; }
  private:
  void SharedCtor();
  void SharedDtor();
  void SetCachedSize(int size) const;
  void InternalSwap(DriverCommand* other);
  private:
  inline ::google::protobuf::Arena* GetArenaNoVirtual() const {
    return _internal_metadata_.arena();
  }
  inline void* MaybeArenaPtr() const {
    return _internal_metadata_.raw_arena_ptr();
  }
  public:

  ::google::protobuf::Metadata GetMetadata() const;

  // nested types ----------------------------------------------------


  typedef DriverCommand_Command Command;
  static const Command UNKNOWN =
    DriverCommand_Command_UNKNOWN;
  static const Command ENABLE_TRACKING =
    DriverCommand_Command_ENABLE_TRACKING;
  static const Command DISABLE_TRACKING =
    DriverCommand_Command_DISABLE_TRACKING;
  static const Command ENABLE_AUDIO =
    DriverCommand_Command_ENABLE_AUDIO;
  static const Command DISABLE_AUDIO =
    DriverCommand_Command_DISABLE_AUDIO;
  static const Command RAW_COMMAND =
    DriverCommand_Command_RAW_COMMAND;
  static const Command DUMP_DEVICE_DIAGNOSTICS =
    DriverCommand_Command_DUMP_DEVICE_DIAGNOSTICS;
  static inline bool Command_IsValid(int value) {
    return DriverCommand_Command_IsValid(value);
  }
  static const Command Command_MIN =
    DriverCommand_Command_Command_MIN;
  static const Command Command_MAX =
    DriverCommand_Command_Command_MAX;
  static const int Command_ARRAYSIZE =
    DriverCommand_Command_Command_ARRAYSIZE;
  static inline const ::google::protobuf::EnumDescriptor*
  Command_descriptor() {
    return DriverCommand_Command_descriptor();
  }
  static inline const ::std::string& Command_Name(Command value) {
    return DriverCommand_Command_Name(value);
  }
  static inline bool Command_Parse(const ::std::string& name,
      Command* value) {
    return DriverCommand_Command_Parse(name, value);
  }

  // accessors -------------------------------------------------------

  // optional .NullSpaceIPC.DriverCommand.Command command = 1;
  void clear_command();
  static const int kCommandFieldNumber = 1;
  ::NullSpaceIPC::DriverCommand_Command command() const;
  void set_command(::NullSpaceIPC::DriverCommand_Command value);

  // map<string, int32> params = 2;
  int params_size() const;
  void clear_params();
  static const int kParamsFieldNumber = 2;
  const ::google::protobuf::Map< ::std::string, ::google::protobuf::int32 >&
      params() const;
  ::google::protobuf::Map< ::std::string, ::google::protobuf::int32 >*
      mutable_params();

  // optional bytes raw_command = 3;
  void clear_raw_command();
  static const int kRawCommandFieldNumber = 3;
  const ::std::string& raw_command() const;
  void set_raw_command(const ::std::string& value);
  void set_raw_command(const char* value);
  void set_raw_command(const void* value, size_t size);
  ::std::string* mutable_raw_command();
  ::std::string* release_raw_command();
  void set_allocated_raw_command(::std::string* raw_command);

  // @@protoc_insertion_point(class_scope:NullSpaceIPC.DriverCommand)
 private:

  ::google::protobuf::internal::InternalMetadataWithArena _internal_metadata_;
  bool _is_default_instance_;
  typedef ::google::protobuf::internal::MapEntryLite<
      ::std::string, ::google::protobuf::int32,
      ::google::protobuf::internal::WireFormatLite::TYPE_STRING,
      ::google::protobuf::internal::WireFormatLite::TYPE_INT32,
      0 >
      DriverCommand_ParamsEntry;
  ::google::protobuf::internal::MapField<
      ::std::string, ::google::protobuf::int32,
      ::google::protobuf::internal::WireFormatLite::TYPE_STRING,
      ::google::protobuf::internal::WireFormatLite::TYPE_INT32,
      0 > params_;
  ::google::protobuf::internal::ArenaStringPtr raw_command_;
  int command_;
  mutable int _cached_size_;
  friend void  protobuf_AddDesc_DriverCommand_2eproto();
  friend void protobuf_AssignDesc_DriverCommand_2eproto();
  friend void protobuf_ShutdownFile_DriverCommand_2eproto();

  void InitAsDefaultInstance();
  static DriverCommand* default_instance_;
};
// ===================================================================


// ===================================================================

#if !PROTOBUF_INLINE_NOT_IN_HEADERS
// DriverCommand

// optional .NullSpaceIPC.DriverCommand.Command command = 1;
inline void DriverCommand::clear_command() {
  command_ = 0;
}
inline ::NullSpaceIPC::DriverCommand_Command DriverCommand::command() const {
  // @@protoc_insertion_point(field_get:NullSpaceIPC.DriverCommand.command)
  return static_cast< ::NullSpaceIPC::DriverCommand_Command >(command_);
}
inline void DriverCommand::set_command(::NullSpaceIPC::DriverCommand_Command value) {
  
  command_ = value;
  // @@protoc_insertion_point(field_set:NullSpaceIPC.DriverCommand.command)
}

// map<string, int32> params = 2;
inline int DriverCommand::params_size() const {
  return params_.size();
}
inline void DriverCommand::clear_params() {
  params_.Clear();
}
inline const ::google::protobuf::Map< ::std::string, ::google::protobuf::int32 >&
DriverCommand::params() const {
  // @@protoc_insertion_point(field_map:NullSpaceIPC.DriverCommand.params)
  return params_.GetMap();
}
inline ::google::protobuf::Map< ::std::string, ::google::protobuf::int32 >*
DriverCommand::mutable_params() {
  // @@protoc_insertion_point(field_mutable_map:NullSpaceIPC.DriverCommand.params)
  return params_.MutableMap();
}

// optional bytes raw_command = 3;
inline void DriverCommand::clear_raw_command() {
  raw_command_.ClearToEmptyNoArena(&::google::protobuf::internal::GetEmptyStringAlreadyInited());
}
inline const ::std::string& DriverCommand::raw_command() const {
  // @@protoc_insertion_point(field_get:NullSpaceIPC.DriverCommand.raw_command)
  return raw_command_.GetNoArena(&::google::protobuf::internal::GetEmptyStringAlreadyInited());
}
inline void DriverCommand::set_raw_command(const ::std::string& value) {
  
  raw_command_.SetNoArena(&::google::protobuf::internal::GetEmptyStringAlreadyInited(), value);
  // @@protoc_insertion_point(field_set:NullSpaceIPC.DriverCommand.raw_command)
}
inline void DriverCommand::set_raw_command(const char* value) {
  
  raw_command_.SetNoArena(&::google::protobuf::internal::GetEmptyStringAlreadyInited(), ::std::string(value));
  // @@protoc_insertion_point(field_set_char:NullSpaceIPC.DriverCommand.raw_command)
}
inline void DriverCommand::set_raw_command(const void* value, size_t size) {
  
  raw_command_.SetNoArena(&::google::protobuf::internal::GetEmptyStringAlreadyInited(),
      ::std::string(reinterpret_cast<const char*>(value), size));
  // @@protoc_insertion_point(field_set_pointer:NullSpaceIPC.DriverCommand.raw_command)
}
inline ::std::string* DriverCommand::mutable_raw_command() {
  
  // @@protoc_insertion_point(field_mutable:NullSpaceIPC.DriverCommand.raw_command)
  return raw_command_.MutableNoArena(&::google::protobuf::internal::GetEmptyStringAlreadyInited());
}
inline ::std::string* DriverCommand::release_raw_command() {
  // @@protoc_insertion_point(field_release:NullSpaceIPC.DriverCommand.raw_command)
  
  return raw_command_.ReleaseNoArena(&::google::protobuf::internal::GetEmptyStringAlreadyInited());
}
inline void DriverCommand::set_allocated_raw_command(::std::string* raw_command) {
  if (raw_command != NULL) {
    
  } else {
    
  }
  raw_command_.SetAllocatedNoArena(&::google::protobuf::internal::GetEmptyStringAlreadyInited(), raw_command);
  // @@protoc_insertion_point(field_set_allocated:NullSpaceIPC.DriverCommand.raw_command)
}

#endif  // !PROTOBUF_INLINE_NOT_IN_HEADERS

// @@protoc_insertion_point(namespace_scope)

}  // namespace NullSpaceIPC

#ifndef SWIG
namespace google {
namespace protobuf {

template <> struct is_proto_enum< ::NullSpaceIPC::DriverCommand_Command> : ::google::protobuf::internal::true_type {};
template <>
inline const EnumDescriptor* GetEnumDescriptor< ::NullSpaceIPC::DriverCommand_Command>() {
  return ::NullSpaceIPC::DriverCommand_Command_descriptor();
}

}  // namespace protobuf
}  // namespace google
#endif  // SWIG

// @@protoc_insertion_point(global_scope)

#endif  // PROTOBUF_DriverCommand_2eproto__INCLUDED
