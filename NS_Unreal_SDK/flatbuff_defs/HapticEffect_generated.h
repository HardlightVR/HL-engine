// automatically generated by the FlatBuffers compiler, do not modify

#ifndef FLATBUFFERS_GENERATED_HAPTICEFFECT_NULLSPACE_HAPTICFILES_H_
#define FLATBUFFERS_GENERATED_HAPTICEFFECT_NULLSPACE_HAPTICFILES_H_

#include "flatbuffers\flatbuffers.h"

namespace NullSpace {
namespace HapticFiles {

struct HapticEffect;

struct HapticEffect FLATBUFFERS_FINAL_CLASS : private flatbuffers::Table {
  enum {
    VT_EFFECT = 4,
    VT_LOCATION = 6,
    VT_DURATION = 8,
    VT_PRIORITY = 10,
    VT_TIME = 12
  };
  uint16_t effect() const { return GetField<uint16_t>(VT_EFFECT, 0); }
  uint16_t location() const { return GetField<uint16_t>(VT_LOCATION, 0); }
  float duration() const { return GetField<float>(VT_DURATION, 0.0f); }
  uint16_t priority() const { return GetField<uint16_t>(VT_PRIORITY, 0); }
  float time() const { return GetField<float>(VT_TIME, 0.0f); }
  bool Verify(flatbuffers::Verifier &verifier) const {
    return VerifyTableStart(verifier) &&
           VerifyField<uint16_t>(verifier, VT_EFFECT) &&
           VerifyField<uint16_t>(verifier, VT_LOCATION) &&
           VerifyField<float>(verifier, VT_DURATION) &&
           VerifyField<uint16_t>(verifier, VT_PRIORITY) &&
           VerifyField<float>(verifier, VT_TIME) &&
           verifier.EndTable();
  }
};

struct HapticEffectBuilder {
  flatbuffers::FlatBufferBuilder &fbb_;
  flatbuffers::uoffset_t start_;
  void add_effect(uint16_t effect) { fbb_.AddElement<uint16_t>(HapticEffect::VT_EFFECT, effect, 0); }
  void add_location(uint16_t location) { fbb_.AddElement<uint16_t>(HapticEffect::VT_LOCATION, location, 0); }
  void add_duration(float duration) { fbb_.AddElement<float>(HapticEffect::VT_DURATION, duration, 0.0f); }
  void add_priority(uint16_t priority) { fbb_.AddElement<uint16_t>(HapticEffect::VT_PRIORITY, priority, 0); }
  void add_time(float time) { fbb_.AddElement<float>(HapticEffect::VT_TIME, time, 0.0f); }
  HapticEffectBuilder(flatbuffers::FlatBufferBuilder &_fbb) : fbb_(_fbb) { start_ = fbb_.StartTable(); }
  HapticEffectBuilder &operator=(const HapticEffectBuilder &);
  flatbuffers::Offset<HapticEffect> Finish() {
    auto o = flatbuffers::Offset<HapticEffect>(fbb_.EndTable(start_, 5));
    return o;
  }
};

inline flatbuffers::Offset<HapticEffect> CreateHapticEffect(flatbuffers::FlatBufferBuilder &_fbb,
    uint16_t effect = 0,
    uint16_t location = 0,
    float duration = 0.0f,
    uint16_t priority = 0,
    float time = 0.0f) {
  HapticEffectBuilder builder_(_fbb);
  builder_.add_time(time);
  builder_.add_duration(duration);
  builder_.add_priority(priority);
  builder_.add_location(location);
  builder_.add_effect(effect);
  return builder_.Finish();
}

inline const NullSpace::HapticFiles::HapticEffect *GetHapticEffect(const void *buf) {
  return flatbuffers::GetRoot<NullSpace::HapticFiles::HapticEffect>(buf);
}

inline bool VerifyHapticEffectBuffer(flatbuffers::Verifier &verifier) {
  return verifier.VerifyBuffer<NullSpace::HapticFiles::HapticEffect>(nullptr);
}

inline void FinishHapticEffectBuffer(flatbuffers::FlatBufferBuilder &fbb, flatbuffers::Offset<NullSpace::HapticFiles::HapticEffect> root) {
  fbb.Finish(root);
}

}  // namespace HapticFiles
}  // namespace NullSpace

#endif  // FLATBUFFERS_GENERATED_HAPTICEFFECT_NULLSPACE_HAPTICFILES_H_