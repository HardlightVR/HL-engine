#include "Parser.h"
#include "json\json.h"



Parser::Parser()
{
	
}

Parser::Parser(std::string path) : 
	_basePath(path)
{
}

Parser::~Parser()
{
}

std::string Parser::GetDirectory(std::string package) {
	if (_paths.find(package) != _paths.end())
	{
		return _paths[package];
	}
	throw PackageNotFoundException(package);
}
void Parser::Traverse(EnumNode node, std::string prefix)
{
	
}




void SequenceItem::Serialize(const Json::Value& root) {

}

void SequenceItem::Deserialize(const Json::Value& root) {
	this->Time = root.get("time", 0.0f).asFloat();
	this->Duration = root.get("duration", 0.0f).asFloat();
	//TODO: decide if we should warn or be optimistic
	this->Waveform = root.get("waveform", "INVALID_WAVEFORM").asString();
	this->Repeat = root.get("repeat", 0).asInt();
	
}



void Effect::Serialize(const Json::Value& root) {

}

void Effect::Deserialize(const Json::Value& root) {
	this->Location = root.get("location", "INVALID_LOCATION").asString();
	this->Sequence = root.get("sequence", "INVALID_SEQUENCE").asString();
	this->Side = root.get("side", "INVALID_SIDE").asString();

}

void Frame::Serialize(const Json::Value& root) {

}

void Frame::Deserialize(const Json::Value& root) {
	this->Time = root.get("time", 0.0f).asFloat();
	Json::Value frames = root["frame"];
	if (frames.isArray()) {
		for (auto effect : frames) {
			Effect e;
			e.Deserialize(effect);
			this->FrameSet.push_back(e);
		}
	}
}

void Pattern::Serialize(const Json::Value& root) {

}

void Pattern::Deserialize(const Json::Value& root) {
	Json::Value frames = root["pattern"];
	if (frames.isArray()) {
		for (auto frame : frames) {
			Frame f;
			f.Deserialize(frame);
			this->Frames.push_back(f);
		}
	}
}