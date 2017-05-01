// Generated by the protocol buffer compiler.  DO NOT EDIT!
// source: ExecutionCommand.proto

#ifndef PROTOBUF_ExecutionCommand_2eproto__INCLUDED
#define PROTOBUF_ExecutionCommand_2eproto__INCLUDED

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
#include <google/protobuf/unknown_field_set.h>
// @@protoc_insertion_point(includes)

namespace NullSpaceIPC {

// Internal implementation detail -- do not call these.
void protobuf_AddDesc_ExecutionCommand_2eproto();
void protobuf_AssignDesc_ExecutionCommand_2eproto();
void protobuf_ShutdownFile_ExecutionCommand_2eproto();

class ExecutionCommand;

// ===================================================================

class ExecutionCommand : public ::google::protobuf::Message /* @@protoc_insertion_point(class_definition:NullSpaceIPC.ExecutionCommand) */ {
 public:
  ExecutionCommand();
  virtual ~ExecutionCommand();

  ExecutionCommand(const ExecutionCommand& from);

  inline ExecutionCommand& operator=(const ExecutionCommand& from) {
    CopyFrom(from);
    return *this;
  }

  static const ::google::protobuf::Descriptor* descriptor();
  static const ExecutionCommand& default_instance();

  void Swap(ExecutionCommand* other);

  // implements Message ----------------------------------------------

  inline ExecutionCommand* New() const { return New(NULL); }

  ExecutionCommand* New(::google::protobuf::Arena* arena) const;
  void CopyFrom(const ::google::protobuf::Message& from);
  void MergeFrom(const ::google::protobuf::Message& from);
  void CopyFrom(const ExecutionCommand& from);
  void MergeFrom(const ExecutionCommand& from);
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
  void InternalSwap(ExecutionCommand* other);
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

  // accessors -------------------------------------------------------

  // optional uint32 location = 1;
  void clear_location();
  static const int kLocationFieldNumber = 1;
  ::google::protobuf::uint32 location() const;
  void set_location(::google::protobuf::uint32 value);

  // optional uint32 command = 2;
  void clear_command();
  static const int kCommandFieldNumber = 2;
  ::google::protobuf::uint32 command() const;
  void set_command(::google::protobuf::uint32 value);

  // optional uint32 effect = 3;
  void clear_effect();
  static const int kEffectFieldNumber = 3;
  ::google::protobuf::uint32 effect() const;
  void set_effect(::google::protobuf::uint32 value);

  // @@protoc_insertion_point(class_scope:NullSpaceIPC.ExecutionCommand)
 private:

  ::google::protobuf::internal::InternalMetadataWithArena _internal_metadata_;
  bool _is_default_instance_;
  ::google::protobuf::uint32 location_;
  ::google::protobuf::uint32 command_;
  ::google::protobuf::uint32 effect_;
  mutable int _cached_size_;
  friend void  protobuf_AddDesc_ExecutionCommand_2eproto();
  friend void protobuf_AssignDesc_ExecutionCommand_2eproto();
  friend void protobuf_ShutdownFile_ExecutionCommand_2eproto();

  void InitAsDefaultInstance();
  static ExecutionCommand* default_instance_;
};
// ===================================================================


// ===================================================================

#if !PROTOBUF_INLINE_NOT_IN_HEADERS
// ExecutionCommand

// optional uint32 location = 1;
inline void ExecutionCommand::clear_location() {
  location_ = 0u;
}
inline ::google::protobuf::uint32 ExecutionCommand::location() const {
  // @@protoc_insertion_point(field_get:NullSpaceIPC.ExecutionCommand.location)
  return location_;
}
inline void ExecutionCommand::set_location(::google::protobuf::uint32 value) {
  
  location_ = value;
  // @@protoc_insertion_point(field_set:NullSpaceIPC.ExecutionCommand.location)
}

// optional uint32 command = 2;
inline void ExecutionCommand::clear_command() {
  command_ = 0u;
}
inline ::google::protobuf::uint32 ExecutionCommand::command() const {
  // @@protoc_insertion_point(field_get:NullSpaceIPC.ExecutionCommand.command)
  return command_;
}
inline void ExecutionCommand::set_command(::google::protobuf::uint32 value) {
  
  command_ = value;
  // @@protoc_insertion_point(field_set:NullSpaceIPC.ExecutionCommand.command)
}

// optional uint32 effect = 3;
inline void ExecutionCommand::clear_effect() {
  effect_ = 0u;
}
inline ::google::protobuf::uint32 ExecutionCommand::effect() const {
  // @@protoc_insertion_point(field_get:NullSpaceIPC.ExecutionCommand.effect)
  return effect_;
}
inline void ExecutionCommand::set_effect(::google::protobuf::uint32 value) {
  
  effect_ = value;
  // @@protoc_insertion_point(field_set:NullSpaceIPC.ExecutionCommand.effect)
}

#endif  // !PROTOBUF_INLINE_NOT_IN_HEADERS

// @@protoc_insertion_point(namespace_scope)

}  // namespace NullSpaceIPC

// @@protoc_insertion_point(global_scope)

#endif  // PROTOBUF_ExecutionCommand_2eproto__INCLUDED
