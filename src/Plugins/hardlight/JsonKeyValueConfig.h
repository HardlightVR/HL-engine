#pragma once
#include "Json/json.h"
#include <unordered_map>
#include <boost/filesystem.hpp>
namespace nsvr {
namespace tools {
namespace json {

	
template<typename TKey, typename TValue>
std::unordered_map<TKey, TValue> parseDictFromDict(std::string path, std::function<TKey(const Json::Value&)> parseKey, std::function<TValue(const Json::Value&)> parseValue, Json::Value defaultValue = Json::Value()) {
	
	Json::Value root;
	std::ifstream json((boost::filesystem::current_path() / path).string(), std::ifstream::binary);

	//this may throw a Json::Exception
	json >> root;
	
	std::unordered_map<TKey, TValue> mapResult;

	auto names = root.getMemberNames();
	for (const std::string& keyName : names) {
		auto key = parseKey(keyName);
		auto value = parseValue(root.get(keyName, defaultValue));
		mapResult[key] = value;
	}

	return mapResult;
}


template<typename TKey, typename TValue>
std::unordered_map<TKey, TValue> parseDictFromArray(std::string path, std::function<std::tuple<TKey, TValue>(const Json::Value&)> parser) {

	Json::Value root;
	std::ifstream json((boost::filesystem::current_path() / path).string(), std::ifstream::binary);

	//this may throw a Json::Exception
	json >> root;

	std::unordered_map<TKey, TValue> mapResult;

	std::size_t numInstructions = root.size();

	for (std::size_t i = 0; i < numInstructions; ++i) {
		auto parsed = parser(root[i]);
		mapResult[std::get<0>(parsed)] = std::get<1>(parsed);
	}

	return mapResult;
}


}
}
}