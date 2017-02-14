#pragma once
#include "IntermediateHapticFormats.h"
#include "flatbuffers\flatbuffers.h"
#include "ExecutionCommand_generated.h"
#include "SharedCommunication\SharedTypes.h"
#include <mutex>
class Encoder {
public: 

	void AquireEncodingLock() {
		_encodingLock.lock();
	}
	void ReleaseEncodingLock() {
		_encodingLock.unlock();
	}
	flatbuffers::Offset<NullSpace::HapticFiles::ExecutionCommand> Encode(const NullSpace::SharedMemory::ExecutionCommand& ec) {
		
		NullSpace::HapticFiles::ExecutionCommandBuilder effectBuilder(_builder);
		effectBuilder.add_command(NullSpace::HapticFiles::PlayCommand(ec.Command));
		effectBuilder.add_location(ec.Location);
		effectBuilder.add_effect(ec.Effect);
		return effectBuilder.Finish();
	}

	void _finalize(struct flatbuffers::Offset<NullSpace::HapticFiles::ExecutionCommand> input, std::function<void(uint8_t*, int)> callback) {

		_builder.Finish(input);
		
		if (VerifyExecutionCommand(_builder)) {
			callback(_builder.GetBufferPointer(), _builder.GetSize());
		}
		_builder.Clear();
	}

	bool VerifyExecutionCommand(flatbuffers::FlatBufferBuilder& builder) {
		flatbuffers::Verifier verifier(builder.GetBufferPointer(), builder.GetSize());
		return NullSpace::HapticFiles::VerifyExecutionCommandBuffer(verifier);
	}

	static NullSpace::SharedMemory::ExecutionCommand Decode(const NullSpace::HapticFiles::ExecutionCommand* effect) {
		NullSpace::SharedMemory::ExecutionCommand c;
		c.Command = effect->command();
		c.Effect = effect->effect();
		c.Location = effect->location();
		return c;
		
	}
private:
	flatbuffers::FlatBufferBuilder _builder;
	std::mutex _encodingLock;
};