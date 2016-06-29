#pragma once
#include "IJsonSerializable.h"
#include "json\json.h"
class JsonSerializer {
public:
	static bool Serialize(IJsonSerializable* obj, std::string& out) {
		if (obj == NULL) {
			return false;
		}
		Json::Value serializeRoot;
		obj->Serialize(serializeRoot);
		Json::StyledWriter writer;
		out = writer.write(serializeRoot);
		return true;
	}
	static bool Deserialize(IJsonSerializable* obj, std::string& in) {
		if (obj == NULL) {
			return false;
		} 
		Json::Value deserializeRoot;
		Json::Reader reader;
		if (!reader.parse(in, deserializeRoot)) {
			return false;
		}
		obj->Deserialize(deserializeRoot);
		return true;
	}
private:
	JsonSerializer(void) {};
};