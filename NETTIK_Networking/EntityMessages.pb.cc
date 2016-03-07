// Generated by the protocol buffer compiler.  DO NOT EDIT!
// source: EntityMessages.proto

#define INTERNAL_SUPPRESS_PROTOBUF_FIELD_DEPRECATION
#include "EntityMessages.pb.h"

#include <algorithm>

#include <google/protobuf/stubs/common.h>
#include <google/protobuf/stubs/once.h>
#include <google/protobuf/io/coded_stream.h>
#include <google/protobuf/wire_format_lite_inl.h>
#include <google/protobuf/descriptor.h>
#include <google/protobuf/generated_message_reflection.h>
#include <google/protobuf/reflection_ops.h>
#include <google/protobuf/wire_format.h>
// @@protoc_insertion_point(includes)

namespace {

const ::google::protobuf::Descriptor* INetVector2_descriptor_ = NULL;
const ::google::protobuf::internal::GeneratedMessageReflection*
  INetVector2_reflection_ = NULL;
const ::google::protobuf::Descriptor* IMessageNewObject_New_descriptor_ = NULL;
const ::google::protobuf::internal::GeneratedMessageReflection*
  IMessageNewObject_New_reflection_ = NULL;
const ::google::protobuf::Descriptor* IMessageNewObject_Del_descriptor_ = NULL;
const ::google::protobuf::internal::GeneratedMessageReflection*
  IMessageNewObject_Del_reflection_ = NULL;

}  // namespace


void protobuf_AssignDesc_EntityMessages_2eproto() {
  protobuf_AddDesc_EntityMessages_2eproto();
  const ::google::protobuf::FileDescriptor* file =
    ::google::protobuf::DescriptorPool::generated_pool()->FindFileByName(
      "EntityMessages.proto");
  GOOGLE_CHECK(file != NULL);
  INetVector2_descriptor_ = file->message_type(0);
  static const int INetVector2_offsets_[2] = {
    GOOGLE_PROTOBUF_GENERATED_MESSAGE_FIELD_OFFSET(INetVector2, x_),
    GOOGLE_PROTOBUF_GENERATED_MESSAGE_FIELD_OFFSET(INetVector2, y_),
  };
  INetVector2_reflection_ =
    ::google::protobuf::internal::GeneratedMessageReflection::NewGeneratedMessageReflection(
      INetVector2_descriptor_,
      INetVector2::default_instance_,
      INetVector2_offsets_,
      -1,
      -1,
      -1,
      sizeof(INetVector2),
      GOOGLE_PROTOBUF_GENERATED_MESSAGE_FIELD_OFFSET(INetVector2, _internal_metadata_),
      GOOGLE_PROTOBUF_GENERATED_MESSAGE_FIELD_OFFSET(INetVector2, _is_default_instance_));
  IMessageNewObject_New_descriptor_ = file->message_type(1);
  static const int IMessageNewObject_New_offsets_[3] = {
    GOOGLE_PROTOBUF_GENERATED_MESSAGE_FIELD_OFFSET(IMessageNewObject_New, id_),
    GOOGLE_PROTOBUF_GENERATED_MESSAGE_FIELD_OFFSET(IMessageNewObject_New, type_),
    GOOGLE_PROTOBUF_GENERATED_MESSAGE_FIELD_OFFSET(IMessageNewObject_New, name_),
  };
  IMessageNewObject_New_reflection_ =
    ::google::protobuf::internal::GeneratedMessageReflection::NewGeneratedMessageReflection(
      IMessageNewObject_New_descriptor_,
      IMessageNewObject_New::default_instance_,
      IMessageNewObject_New_offsets_,
      -1,
      -1,
      -1,
      sizeof(IMessageNewObject_New),
      GOOGLE_PROTOBUF_GENERATED_MESSAGE_FIELD_OFFSET(IMessageNewObject_New, _internal_metadata_),
      GOOGLE_PROTOBUF_GENERATED_MESSAGE_FIELD_OFFSET(IMessageNewObject_New, _is_default_instance_));
  IMessageNewObject_Del_descriptor_ = file->message_type(2);
  static const int IMessageNewObject_Del_offsets_[1] = {
    GOOGLE_PROTOBUF_GENERATED_MESSAGE_FIELD_OFFSET(IMessageNewObject_Del, id_),
  };
  IMessageNewObject_Del_reflection_ =
    ::google::protobuf::internal::GeneratedMessageReflection::NewGeneratedMessageReflection(
      IMessageNewObject_Del_descriptor_,
      IMessageNewObject_Del::default_instance_,
      IMessageNewObject_Del_offsets_,
      -1,
      -1,
      -1,
      sizeof(IMessageNewObject_Del),
      GOOGLE_PROTOBUF_GENERATED_MESSAGE_FIELD_OFFSET(IMessageNewObject_Del, _internal_metadata_),
      GOOGLE_PROTOBUF_GENERATED_MESSAGE_FIELD_OFFSET(IMessageNewObject_Del, _is_default_instance_));
}

namespace {

GOOGLE_PROTOBUF_DECLARE_ONCE(protobuf_AssignDescriptors_once_);
inline void protobuf_AssignDescriptorsOnce() {
  ::google::protobuf::GoogleOnceInit(&protobuf_AssignDescriptors_once_,
                 &protobuf_AssignDesc_EntityMessages_2eproto);
}

void protobuf_RegisterTypes(const ::std::string&) {
  protobuf_AssignDescriptorsOnce();
  ::google::protobuf::MessageFactory::InternalRegisterGeneratedMessage(
      INetVector2_descriptor_, &INetVector2::default_instance());
  ::google::protobuf::MessageFactory::InternalRegisterGeneratedMessage(
      IMessageNewObject_New_descriptor_, &IMessageNewObject_New::default_instance());
  ::google::protobuf::MessageFactory::InternalRegisterGeneratedMessage(
      IMessageNewObject_Del_descriptor_, &IMessageNewObject_Del::default_instance());
}

}  // namespace

void protobuf_ShutdownFile_EntityMessages_2eproto() {
  delete INetVector2::default_instance_;
  delete INetVector2_reflection_;
  delete IMessageNewObject_New::default_instance_;
  delete IMessageNewObject_New_reflection_;
  delete IMessageNewObject_Del::default_instance_;
  delete IMessageNewObject_Del_reflection_;
}

void protobuf_AddDesc_EntityMessages_2eproto() {
  static bool already_here = false;
  if (already_here) return;
  already_here = true;
  GOOGLE_PROTOBUF_VERIFY_VERSION;

  ::google::protobuf::DescriptorPool::InternalAddGeneratedFile(
    "\n\024EntityMessages.proto\"#\n\013INetVector2\022\t\n"
    "\001x\030\001 \001(\002\022\t\n\001y\030\002 \001(\002\"\?\n\025IMessageNewObject"
    "_New\022\n\n\002id\030\001 \001(\r\022\014\n\004type\030\002 \001(\r\022\014\n\004name\030\003"
    " \001(\t\"#\n\025IMessageNewObject_Del\022\n\n\002id\030\001 \001("
    "\rb\006proto3", 169);
  ::google::protobuf::MessageFactory::InternalRegisterGeneratedFile(
    "EntityMessages.proto", &protobuf_RegisterTypes);
  INetVector2::default_instance_ = new INetVector2();
  IMessageNewObject_New::default_instance_ = new IMessageNewObject_New();
  IMessageNewObject_Del::default_instance_ = new IMessageNewObject_Del();
  INetVector2::default_instance_->InitAsDefaultInstance();
  IMessageNewObject_New::default_instance_->InitAsDefaultInstance();
  IMessageNewObject_Del::default_instance_->InitAsDefaultInstance();
  ::google::protobuf::internal::OnShutdown(&protobuf_ShutdownFile_EntityMessages_2eproto);
}

// Force AddDescriptors() to be called at static initialization time.
struct StaticDescriptorInitializer_EntityMessages_2eproto {
  StaticDescriptorInitializer_EntityMessages_2eproto() {
    protobuf_AddDesc_EntityMessages_2eproto();
  }
} static_descriptor_initializer_EntityMessages_2eproto_;

namespace {

static void MergeFromFail(int line) GOOGLE_ATTRIBUTE_COLD;
static void MergeFromFail(int line) {
  GOOGLE_CHECK(false) << __FILE__ << ":" << line;
}

}  // namespace


// ===================================================================

#ifndef _MSC_VER
const int INetVector2::kXFieldNumber;
const int INetVector2::kYFieldNumber;
#endif  // !_MSC_VER

INetVector2::INetVector2()
  : ::google::protobuf::Message(), _internal_metadata_(NULL) {
  SharedCtor();
  // @@protoc_insertion_point(constructor:INetVector2)
}

void INetVector2::InitAsDefaultInstance() {
  _is_default_instance_ = true;
}

INetVector2::INetVector2(const INetVector2& from)
  : ::google::protobuf::Message(),
    _internal_metadata_(NULL) {
  SharedCtor();
  MergeFrom(from);
  // @@protoc_insertion_point(copy_constructor:INetVector2)
}

void INetVector2::SharedCtor() {
    _is_default_instance_ = false;
  _cached_size_ = 0;
  x_ = 0;
  y_ = 0;
}

INetVector2::~INetVector2() {
  // @@protoc_insertion_point(destructor:INetVector2)
  SharedDtor();
}

void INetVector2::SharedDtor() {
  if (this != default_instance_) {
  }
}

void INetVector2::SetCachedSize(int size) const {
  GOOGLE_SAFE_CONCURRENT_WRITES_BEGIN();
  _cached_size_ = size;
  GOOGLE_SAFE_CONCURRENT_WRITES_END();
}
const ::google::protobuf::Descriptor* INetVector2::descriptor() {
  protobuf_AssignDescriptorsOnce();
  return INetVector2_descriptor_;
}

const INetVector2& INetVector2::default_instance() {
  if (default_instance_ == NULL) protobuf_AddDesc_EntityMessages_2eproto();
  return *default_instance_;
}

INetVector2* INetVector2::default_instance_ = NULL;

INetVector2* INetVector2::New(::google::protobuf::Arena* arena) const {
  INetVector2* n = new INetVector2;
  if (arena != NULL) {
    arena->Own(n);
  }
  return n;
}

void INetVector2::Clear() {
#define ZR_HELPER_(f) reinterpret_cast<char*>(\
  &reinterpret_cast<INetVector2*>(16)->f)

#define ZR_(first, last) do {\
  ::memset(&first, 0,\
           ZR_HELPER_(last) - ZR_HELPER_(first) + sizeof(last));\
} while (0)

  ZR_(x_, y_);

#undef ZR_HELPER_
#undef ZR_

}

bool INetVector2::MergePartialFromCodedStream(
    ::google::protobuf::io::CodedInputStream* input) {
#define DO_(EXPRESSION) if (!(EXPRESSION)) goto failure
  ::google::protobuf::uint32 tag;
  // @@protoc_insertion_point(parse_start:INetVector2)
  for (;;) {
    ::std::pair< ::google::protobuf::uint32, bool> p = input->ReadTagWithCutoff(127);
    tag = p.first;
    if (!p.second) goto handle_unusual;
    switch (::google::protobuf::internal::WireFormatLite::GetTagFieldNumber(tag)) {
      // optional float x = 1;
      case 1: {
        if (tag == 13) {
          DO_((::google::protobuf::internal::WireFormatLite::ReadPrimitive<
                   float, ::google::protobuf::internal::WireFormatLite::TYPE_FLOAT>(
                 input, &x_)));

        } else {
          goto handle_unusual;
        }
        if (input->ExpectTag(21)) goto parse_y;
        break;
      }

      // optional float y = 2;
      case 2: {
        if (tag == 21) {
         parse_y:
          DO_((::google::protobuf::internal::WireFormatLite::ReadPrimitive<
                   float, ::google::protobuf::internal::WireFormatLite::TYPE_FLOAT>(
                 input, &y_)));

        } else {
          goto handle_unusual;
        }
        if (input->ExpectAtEnd()) goto success;
        break;
      }

      default: {
      handle_unusual:
        if (tag == 0 ||
            ::google::protobuf::internal::WireFormatLite::GetTagWireType(tag) ==
            ::google::protobuf::internal::WireFormatLite::WIRETYPE_END_GROUP) {
          goto success;
        }
        DO_(::google::protobuf::internal::WireFormatLite::SkipField(input, tag));
        break;
      }
    }
  }
success:
  // @@protoc_insertion_point(parse_success:INetVector2)
  return true;
failure:
  // @@protoc_insertion_point(parse_failure:INetVector2)
  return false;
#undef DO_
}

void INetVector2::SerializeWithCachedSizes(
    ::google::protobuf::io::CodedOutputStream* output) const {
  // @@protoc_insertion_point(serialize_start:INetVector2)
  // optional float x = 1;
  if (this->x() != 0) {
    ::google::protobuf::internal::WireFormatLite::WriteFloat(1, this->x(), output);
  }

  // optional float y = 2;
  if (this->y() != 0) {
    ::google::protobuf::internal::WireFormatLite::WriteFloat(2, this->y(), output);
  }

  // @@protoc_insertion_point(serialize_end:INetVector2)
}

::google::protobuf::uint8* INetVector2::SerializeWithCachedSizesToArray(
    ::google::protobuf::uint8* target) const {
  // @@protoc_insertion_point(serialize_to_array_start:INetVector2)
  // optional float x = 1;
  if (this->x() != 0) {
    target = ::google::protobuf::internal::WireFormatLite::WriteFloatToArray(1, this->x(), target);
  }

  // optional float y = 2;
  if (this->y() != 0) {
    target = ::google::protobuf::internal::WireFormatLite::WriteFloatToArray(2, this->y(), target);
  }

  // @@protoc_insertion_point(serialize_to_array_end:INetVector2)
  return target;
}

int INetVector2::ByteSize() const {
  int total_size = 0;

  // optional float x = 1;
  if (this->x() != 0) {
    total_size += 1 + 4;
  }

  // optional float y = 2;
  if (this->y() != 0) {
    total_size += 1 + 4;
  }

  GOOGLE_SAFE_CONCURRENT_WRITES_BEGIN();
  _cached_size_ = total_size;
  GOOGLE_SAFE_CONCURRENT_WRITES_END();
  return total_size;
}

void INetVector2::MergeFrom(const ::google::protobuf::Message& from) {
  if (GOOGLE_PREDICT_FALSE(&from == this)) MergeFromFail(__LINE__);
  const INetVector2* source = 
      ::google::protobuf::internal::DynamicCastToGenerated<const INetVector2>(
          &from);
  if (source == NULL) {
    ::google::protobuf::internal::ReflectionOps::Merge(from, this);
  } else {
    MergeFrom(*source);
  }
}

void INetVector2::MergeFrom(const INetVector2& from) {
  if (GOOGLE_PREDICT_FALSE(&from == this)) MergeFromFail(__LINE__);
  if (from.x() != 0) {
    set_x(from.x());
  }
  if (from.y() != 0) {
    set_y(from.y());
  }
}

void INetVector2::CopyFrom(const ::google::protobuf::Message& from) {
  if (&from == this) return;
  Clear();
  MergeFrom(from);
}

void INetVector2::CopyFrom(const INetVector2& from) {
  if (&from == this) return;
  Clear();
  MergeFrom(from);
}

bool INetVector2::IsInitialized() const {

  return true;
}

void INetVector2::Swap(INetVector2* other) {
  if (other == this) return;
  InternalSwap(other);
}
void INetVector2::InternalSwap(INetVector2* other) {
  std::swap(x_, other->x_);
  std::swap(y_, other->y_);
  _internal_metadata_.Swap(&other->_internal_metadata_);
  std::swap(_cached_size_, other->_cached_size_);
}

::google::protobuf::Metadata INetVector2::GetMetadata() const {
  protobuf_AssignDescriptorsOnce();
  ::google::protobuf::Metadata metadata;
  metadata.descriptor = INetVector2_descriptor_;
  metadata.reflection = INetVector2_reflection_;
  return metadata;
}

#if PROTOBUF_INLINE_NOT_IN_HEADERS
// INetVector2

// optional float x = 1;
void INetVector2::clear_x() {
  x_ = 0;
}
 float INetVector2::x() const {
  // @@protoc_insertion_point(field_get:INetVector2.x)
  return x_;
}
 void INetVector2::set_x(float value) {
  
  x_ = value;
  // @@protoc_insertion_point(field_set:INetVector2.x)
}

// optional float y = 2;
void INetVector2::clear_y() {
  y_ = 0;
}
 float INetVector2::y() const {
  // @@protoc_insertion_point(field_get:INetVector2.y)
  return y_;
}
 void INetVector2::set_y(float value) {
  
  y_ = value;
  // @@protoc_insertion_point(field_set:INetVector2.y)
}

#endif  // PROTOBUF_INLINE_NOT_IN_HEADERS

// ===================================================================

#ifndef _MSC_VER
const int IMessageNewObject_New::kIdFieldNumber;
const int IMessageNewObject_New::kTypeFieldNumber;
const int IMessageNewObject_New::kNameFieldNumber;
#endif  // !_MSC_VER

IMessageNewObject_New::IMessageNewObject_New()
  : ::google::protobuf::Message(), _internal_metadata_(NULL) {
  SharedCtor();
  // @@protoc_insertion_point(constructor:IMessageNewObject_New)
}

void IMessageNewObject_New::InitAsDefaultInstance() {
  _is_default_instance_ = true;
}

IMessageNewObject_New::IMessageNewObject_New(const IMessageNewObject_New& from)
  : ::google::protobuf::Message(),
    _internal_metadata_(NULL) {
  SharedCtor();
  MergeFrom(from);
  // @@protoc_insertion_point(copy_constructor:IMessageNewObject_New)
}

void IMessageNewObject_New::SharedCtor() {
    _is_default_instance_ = false;
  ::google::protobuf::internal::GetEmptyString();
  _cached_size_ = 0;
  id_ = 0u;
  type_ = 0u;
  name_.UnsafeSetDefault(&::google::protobuf::internal::GetEmptyStringAlreadyInited());
}

IMessageNewObject_New::~IMessageNewObject_New() {
  // @@protoc_insertion_point(destructor:IMessageNewObject_New)
  SharedDtor();
}

void IMessageNewObject_New::SharedDtor() {
  name_.DestroyNoArena(&::google::protobuf::internal::GetEmptyStringAlreadyInited());
  if (this != default_instance_) {
  }
}

void IMessageNewObject_New::SetCachedSize(int size) const {
  GOOGLE_SAFE_CONCURRENT_WRITES_BEGIN();
  _cached_size_ = size;
  GOOGLE_SAFE_CONCURRENT_WRITES_END();
}
const ::google::protobuf::Descriptor* IMessageNewObject_New::descriptor() {
  protobuf_AssignDescriptorsOnce();
  return IMessageNewObject_New_descriptor_;
}

const IMessageNewObject_New& IMessageNewObject_New::default_instance() {
  if (default_instance_ == NULL) protobuf_AddDesc_EntityMessages_2eproto();
  return *default_instance_;
}

IMessageNewObject_New* IMessageNewObject_New::default_instance_ = NULL;

IMessageNewObject_New* IMessageNewObject_New::New(::google::protobuf::Arena* arena) const {
  IMessageNewObject_New* n = new IMessageNewObject_New;
  if (arena != NULL) {
    arena->Own(n);
  }
  return n;
}

void IMessageNewObject_New::Clear() {
#define ZR_HELPER_(f) reinterpret_cast<char*>(\
  &reinterpret_cast<IMessageNewObject_New*>(16)->f)

#define ZR_(first, last) do {\
  ::memset(&first, 0,\
           ZR_HELPER_(last) - ZR_HELPER_(first) + sizeof(last));\
} while (0)

  ZR_(id_, type_);
  name_.ClearToEmptyNoArena(&::google::protobuf::internal::GetEmptyStringAlreadyInited());

#undef ZR_HELPER_
#undef ZR_

}

bool IMessageNewObject_New::MergePartialFromCodedStream(
    ::google::protobuf::io::CodedInputStream* input) {
#define DO_(EXPRESSION) if (!(EXPRESSION)) goto failure
  ::google::protobuf::uint32 tag;
  // @@protoc_insertion_point(parse_start:IMessageNewObject_New)
  for (;;) {
    ::std::pair< ::google::protobuf::uint32, bool> p = input->ReadTagWithCutoff(127);
    tag = p.first;
    if (!p.second) goto handle_unusual;
    switch (::google::protobuf::internal::WireFormatLite::GetTagFieldNumber(tag)) {
      // optional uint32 id = 1;
      case 1: {
        if (tag == 8) {
          DO_((::google::protobuf::internal::WireFormatLite::ReadPrimitive<
                   ::google::protobuf::uint32, ::google::protobuf::internal::WireFormatLite::TYPE_UINT32>(
                 input, &id_)));

        } else {
          goto handle_unusual;
        }
        if (input->ExpectTag(16)) goto parse_type;
        break;
      }

      // optional uint32 type = 2;
      case 2: {
        if (tag == 16) {
         parse_type:
          DO_((::google::protobuf::internal::WireFormatLite::ReadPrimitive<
                   ::google::protobuf::uint32, ::google::protobuf::internal::WireFormatLite::TYPE_UINT32>(
                 input, &type_)));

        } else {
          goto handle_unusual;
        }
        if (input->ExpectTag(26)) goto parse_name;
        break;
      }

      // optional string name = 3;
      case 3: {
        if (tag == 26) {
         parse_name:
          DO_(::google::protobuf::internal::WireFormatLite::ReadString(
                input, this->mutable_name()));
          ::google::protobuf::internal::WireFormat::VerifyUTF8StringNamedField(
            this->name().data(), this->name().length(),
            ::google::protobuf::internal::WireFormat::PARSE,
            "IMessageNewObject_New.name");
        } else {
          goto handle_unusual;
        }
        if (input->ExpectAtEnd()) goto success;
        break;
      }

      default: {
      handle_unusual:
        if (tag == 0 ||
            ::google::protobuf::internal::WireFormatLite::GetTagWireType(tag) ==
            ::google::protobuf::internal::WireFormatLite::WIRETYPE_END_GROUP) {
          goto success;
        }
        DO_(::google::protobuf::internal::WireFormatLite::SkipField(input, tag));
        break;
      }
    }
  }
success:
  // @@protoc_insertion_point(parse_success:IMessageNewObject_New)
  return true;
failure:
  // @@protoc_insertion_point(parse_failure:IMessageNewObject_New)
  return false;
#undef DO_
}

void IMessageNewObject_New::SerializeWithCachedSizes(
    ::google::protobuf::io::CodedOutputStream* output) const {
  // @@protoc_insertion_point(serialize_start:IMessageNewObject_New)
  // optional uint32 id = 1;
  if (this->id() != 0) {
    ::google::protobuf::internal::WireFormatLite::WriteUInt32(1, this->id(), output);
  }

  // optional uint32 type = 2;
  if (this->type() != 0) {
    ::google::protobuf::internal::WireFormatLite::WriteUInt32(2, this->type(), output);
  }

  // optional string name = 3;
  if (this->name().size() > 0) {
    ::google::protobuf::internal::WireFormat::VerifyUTF8StringNamedField(
      this->name().data(), this->name().length(),
      ::google::protobuf::internal::WireFormat::SERIALIZE,
      "IMessageNewObject_New.name");
    ::google::protobuf::internal::WireFormatLite::WriteStringMaybeAliased(
      3, this->name(), output);
  }

  // @@protoc_insertion_point(serialize_end:IMessageNewObject_New)
}

::google::protobuf::uint8* IMessageNewObject_New::SerializeWithCachedSizesToArray(
    ::google::protobuf::uint8* target) const {
  // @@protoc_insertion_point(serialize_to_array_start:IMessageNewObject_New)
  // optional uint32 id = 1;
  if (this->id() != 0) {
    target = ::google::protobuf::internal::WireFormatLite::WriteUInt32ToArray(1, this->id(), target);
  }

  // optional uint32 type = 2;
  if (this->type() != 0) {
    target = ::google::protobuf::internal::WireFormatLite::WriteUInt32ToArray(2, this->type(), target);
  }

  // optional string name = 3;
  if (this->name().size() > 0) {
    ::google::protobuf::internal::WireFormat::VerifyUTF8StringNamedField(
      this->name().data(), this->name().length(),
      ::google::protobuf::internal::WireFormat::SERIALIZE,
      "IMessageNewObject_New.name");
    target =
      ::google::protobuf::internal::WireFormatLite::WriteStringToArray(
        3, this->name(), target);
  }

  // @@protoc_insertion_point(serialize_to_array_end:IMessageNewObject_New)
  return target;
}

int IMessageNewObject_New::ByteSize() const {
  int total_size = 0;

  // optional uint32 id = 1;
  if (this->id() != 0) {
    total_size += 1 +
      ::google::protobuf::internal::WireFormatLite::UInt32Size(
        this->id());
  }

  // optional uint32 type = 2;
  if (this->type() != 0) {
    total_size += 1 +
      ::google::protobuf::internal::WireFormatLite::UInt32Size(
        this->type());
  }

  // optional string name = 3;
  if (this->name().size() > 0) {
    total_size += 1 +
      ::google::protobuf::internal::WireFormatLite::StringSize(
        this->name());
  }

  GOOGLE_SAFE_CONCURRENT_WRITES_BEGIN();
  _cached_size_ = total_size;
  GOOGLE_SAFE_CONCURRENT_WRITES_END();
  return total_size;
}

void IMessageNewObject_New::MergeFrom(const ::google::protobuf::Message& from) {
  if (GOOGLE_PREDICT_FALSE(&from == this)) MergeFromFail(__LINE__);
  const IMessageNewObject_New* source = 
      ::google::protobuf::internal::DynamicCastToGenerated<const IMessageNewObject_New>(
          &from);
  if (source == NULL) {
    ::google::protobuf::internal::ReflectionOps::Merge(from, this);
  } else {
    MergeFrom(*source);
  }
}

void IMessageNewObject_New::MergeFrom(const IMessageNewObject_New& from) {
  if (GOOGLE_PREDICT_FALSE(&from == this)) MergeFromFail(__LINE__);
  if (from.id() != 0) {
    set_id(from.id());
  }
  if (from.type() != 0) {
    set_type(from.type());
  }
  if (from.name().size() > 0) {

    name_.AssignWithDefault(&::google::protobuf::internal::GetEmptyStringAlreadyInited(), from.name_);
  }
}

void IMessageNewObject_New::CopyFrom(const ::google::protobuf::Message& from) {
  if (&from == this) return;
  Clear();
  MergeFrom(from);
}

void IMessageNewObject_New::CopyFrom(const IMessageNewObject_New& from) {
  if (&from == this) return;
  Clear();
  MergeFrom(from);
}

bool IMessageNewObject_New::IsInitialized() const {

  return true;
}

void IMessageNewObject_New::Swap(IMessageNewObject_New* other) {
  if (other == this) return;
  InternalSwap(other);
}
void IMessageNewObject_New::InternalSwap(IMessageNewObject_New* other) {
  std::swap(id_, other->id_);
  std::swap(type_, other->type_);
  name_.Swap(&other->name_);
  _internal_metadata_.Swap(&other->_internal_metadata_);
  std::swap(_cached_size_, other->_cached_size_);
}

::google::protobuf::Metadata IMessageNewObject_New::GetMetadata() const {
  protobuf_AssignDescriptorsOnce();
  ::google::protobuf::Metadata metadata;
  metadata.descriptor = IMessageNewObject_New_descriptor_;
  metadata.reflection = IMessageNewObject_New_reflection_;
  return metadata;
}

#if PROTOBUF_INLINE_NOT_IN_HEADERS
// IMessageNewObject_New

// optional uint32 id = 1;
void IMessageNewObject_New::clear_id() {
  id_ = 0u;
}
 ::google::protobuf::uint32 IMessageNewObject_New::id() const {
  // @@protoc_insertion_point(field_get:IMessageNewObject_New.id)
  return id_;
}
 void IMessageNewObject_New::set_id(::google::protobuf::uint32 value) {
  
  id_ = value;
  // @@protoc_insertion_point(field_set:IMessageNewObject_New.id)
}

// optional uint32 type = 2;
void IMessageNewObject_New::clear_type() {
  type_ = 0u;
}
 ::google::protobuf::uint32 IMessageNewObject_New::type() const {
  // @@protoc_insertion_point(field_get:IMessageNewObject_New.type)
  return type_;
}
 void IMessageNewObject_New::set_type(::google::protobuf::uint32 value) {
  
  type_ = value;
  // @@protoc_insertion_point(field_set:IMessageNewObject_New.type)
}

// optional string name = 3;
void IMessageNewObject_New::clear_name() {
  name_.ClearToEmptyNoArena(&::google::protobuf::internal::GetEmptyStringAlreadyInited());
}
 const ::std::string& IMessageNewObject_New::name() const {
  // @@protoc_insertion_point(field_get:IMessageNewObject_New.name)
  return name_.GetNoArena(&::google::protobuf::internal::GetEmptyStringAlreadyInited());
}
 void IMessageNewObject_New::set_name(const ::std::string& value) {
  
  name_.SetNoArena(&::google::protobuf::internal::GetEmptyStringAlreadyInited(), value);
  // @@protoc_insertion_point(field_set:IMessageNewObject_New.name)
}
 void IMessageNewObject_New::set_name(const char* value) {
  
  name_.SetNoArena(&::google::protobuf::internal::GetEmptyStringAlreadyInited(), ::std::string(value));
  // @@protoc_insertion_point(field_set_char:IMessageNewObject_New.name)
}
 void IMessageNewObject_New::set_name(const char* value, size_t size) {
  
  name_.SetNoArena(&::google::protobuf::internal::GetEmptyStringAlreadyInited(),
      ::std::string(reinterpret_cast<const char*>(value), size));
  // @@protoc_insertion_point(field_set_pointer:IMessageNewObject_New.name)
}
 ::std::string* IMessageNewObject_New::mutable_name() {
  
  // @@protoc_insertion_point(field_mutable:IMessageNewObject_New.name)
  return name_.MutableNoArena(&::google::protobuf::internal::GetEmptyStringAlreadyInited());
}
 ::std::string* IMessageNewObject_New::release_name() {
  
  return name_.ReleaseNoArena(&::google::protobuf::internal::GetEmptyStringAlreadyInited());
}
 void IMessageNewObject_New::set_allocated_name(::std::string* name) {
  if (name != NULL) {
    
  } else {
    
  }
  name_.SetAllocatedNoArena(&::google::protobuf::internal::GetEmptyStringAlreadyInited(), name);
  // @@protoc_insertion_point(field_set_allocated:IMessageNewObject_New.name)
}

#endif  // PROTOBUF_INLINE_NOT_IN_HEADERS

// ===================================================================

#ifndef _MSC_VER
const int IMessageNewObject_Del::kIdFieldNumber;
#endif  // !_MSC_VER

IMessageNewObject_Del::IMessageNewObject_Del()
  : ::google::protobuf::Message(), _internal_metadata_(NULL) {
  SharedCtor();
  // @@protoc_insertion_point(constructor:IMessageNewObject_Del)
}

void IMessageNewObject_Del::InitAsDefaultInstance() {
  _is_default_instance_ = true;
}

IMessageNewObject_Del::IMessageNewObject_Del(const IMessageNewObject_Del& from)
  : ::google::protobuf::Message(),
    _internal_metadata_(NULL) {
  SharedCtor();
  MergeFrom(from);
  // @@protoc_insertion_point(copy_constructor:IMessageNewObject_Del)
}

void IMessageNewObject_Del::SharedCtor() {
    _is_default_instance_ = false;
  _cached_size_ = 0;
  id_ = 0u;
}

IMessageNewObject_Del::~IMessageNewObject_Del() {
  // @@protoc_insertion_point(destructor:IMessageNewObject_Del)
  SharedDtor();
}

void IMessageNewObject_Del::SharedDtor() {
  if (this != default_instance_) {
  }
}

void IMessageNewObject_Del::SetCachedSize(int size) const {
  GOOGLE_SAFE_CONCURRENT_WRITES_BEGIN();
  _cached_size_ = size;
  GOOGLE_SAFE_CONCURRENT_WRITES_END();
}
const ::google::protobuf::Descriptor* IMessageNewObject_Del::descriptor() {
  protobuf_AssignDescriptorsOnce();
  return IMessageNewObject_Del_descriptor_;
}

const IMessageNewObject_Del& IMessageNewObject_Del::default_instance() {
  if (default_instance_ == NULL) protobuf_AddDesc_EntityMessages_2eproto();
  return *default_instance_;
}

IMessageNewObject_Del* IMessageNewObject_Del::default_instance_ = NULL;

IMessageNewObject_Del* IMessageNewObject_Del::New(::google::protobuf::Arena* arena) const {
  IMessageNewObject_Del* n = new IMessageNewObject_Del;
  if (arena != NULL) {
    arena->Own(n);
  }
  return n;
}

void IMessageNewObject_Del::Clear() {
  id_ = 0u;
}

bool IMessageNewObject_Del::MergePartialFromCodedStream(
    ::google::protobuf::io::CodedInputStream* input) {
#define DO_(EXPRESSION) if (!(EXPRESSION)) goto failure
  ::google::protobuf::uint32 tag;
  // @@protoc_insertion_point(parse_start:IMessageNewObject_Del)
  for (;;) {
    ::std::pair< ::google::protobuf::uint32, bool> p = input->ReadTagWithCutoff(127);
    tag = p.first;
    if (!p.second) goto handle_unusual;
    switch (::google::protobuf::internal::WireFormatLite::GetTagFieldNumber(tag)) {
      // optional uint32 id = 1;
      case 1: {
        if (tag == 8) {
          DO_((::google::protobuf::internal::WireFormatLite::ReadPrimitive<
                   ::google::protobuf::uint32, ::google::protobuf::internal::WireFormatLite::TYPE_UINT32>(
                 input, &id_)));

        } else {
          goto handle_unusual;
        }
        if (input->ExpectAtEnd()) goto success;
        break;
      }

      default: {
      handle_unusual:
        if (tag == 0 ||
            ::google::protobuf::internal::WireFormatLite::GetTagWireType(tag) ==
            ::google::protobuf::internal::WireFormatLite::WIRETYPE_END_GROUP) {
          goto success;
        }
        DO_(::google::protobuf::internal::WireFormatLite::SkipField(input, tag));
        break;
      }
    }
  }
success:
  // @@protoc_insertion_point(parse_success:IMessageNewObject_Del)
  return true;
failure:
  // @@protoc_insertion_point(parse_failure:IMessageNewObject_Del)
  return false;
#undef DO_
}

void IMessageNewObject_Del::SerializeWithCachedSizes(
    ::google::protobuf::io::CodedOutputStream* output) const {
  // @@protoc_insertion_point(serialize_start:IMessageNewObject_Del)
  // optional uint32 id = 1;
  if (this->id() != 0) {
    ::google::protobuf::internal::WireFormatLite::WriteUInt32(1, this->id(), output);
  }

  // @@protoc_insertion_point(serialize_end:IMessageNewObject_Del)
}

::google::protobuf::uint8* IMessageNewObject_Del::SerializeWithCachedSizesToArray(
    ::google::protobuf::uint8* target) const {
  // @@protoc_insertion_point(serialize_to_array_start:IMessageNewObject_Del)
  // optional uint32 id = 1;
  if (this->id() != 0) {
    target = ::google::protobuf::internal::WireFormatLite::WriteUInt32ToArray(1, this->id(), target);
  }

  // @@protoc_insertion_point(serialize_to_array_end:IMessageNewObject_Del)
  return target;
}

int IMessageNewObject_Del::ByteSize() const {
  int total_size = 0;

  // optional uint32 id = 1;
  if (this->id() != 0) {
    total_size += 1 +
      ::google::protobuf::internal::WireFormatLite::UInt32Size(
        this->id());
  }

  GOOGLE_SAFE_CONCURRENT_WRITES_BEGIN();
  _cached_size_ = total_size;
  GOOGLE_SAFE_CONCURRENT_WRITES_END();
  return total_size;
}

void IMessageNewObject_Del::MergeFrom(const ::google::protobuf::Message& from) {
  if (GOOGLE_PREDICT_FALSE(&from == this)) MergeFromFail(__LINE__);
  const IMessageNewObject_Del* source = 
      ::google::protobuf::internal::DynamicCastToGenerated<const IMessageNewObject_Del>(
          &from);
  if (source == NULL) {
    ::google::protobuf::internal::ReflectionOps::Merge(from, this);
  } else {
    MergeFrom(*source);
  }
}

void IMessageNewObject_Del::MergeFrom(const IMessageNewObject_Del& from) {
  if (GOOGLE_PREDICT_FALSE(&from == this)) MergeFromFail(__LINE__);
  if (from.id() != 0) {
    set_id(from.id());
  }
}

void IMessageNewObject_Del::CopyFrom(const ::google::protobuf::Message& from) {
  if (&from == this) return;
  Clear();
  MergeFrom(from);
}

void IMessageNewObject_Del::CopyFrom(const IMessageNewObject_Del& from) {
  if (&from == this) return;
  Clear();
  MergeFrom(from);
}

bool IMessageNewObject_Del::IsInitialized() const {

  return true;
}

void IMessageNewObject_Del::Swap(IMessageNewObject_Del* other) {
  if (other == this) return;
  InternalSwap(other);
}
void IMessageNewObject_Del::InternalSwap(IMessageNewObject_Del* other) {
  std::swap(id_, other->id_);
  _internal_metadata_.Swap(&other->_internal_metadata_);
  std::swap(_cached_size_, other->_cached_size_);
}

::google::protobuf::Metadata IMessageNewObject_Del::GetMetadata() const {
  protobuf_AssignDescriptorsOnce();
  ::google::protobuf::Metadata metadata;
  metadata.descriptor = IMessageNewObject_Del_descriptor_;
  metadata.reflection = IMessageNewObject_Del_reflection_;
  return metadata;
}

#if PROTOBUF_INLINE_NOT_IN_HEADERS
// IMessageNewObject_Del

// optional uint32 id = 1;
void IMessageNewObject_Del::clear_id() {
  id_ = 0u;
}
 ::google::protobuf::uint32 IMessageNewObject_Del::id() const {
  // @@protoc_insertion_point(field_get:IMessageNewObject_Del.id)
  return id_;
}
 void IMessageNewObject_Del::set_id(::google::protobuf::uint32 value) {
  
  id_ = value;
  // @@protoc_insertion_point(field_set:IMessageNewObject_Del.id)
}

#endif  // PROTOBUF_INLINE_NOT_IN_HEADERS

// @@protoc_insertion_point(namespace_scope)

// @@protoc_insertion_point(global_scope)
