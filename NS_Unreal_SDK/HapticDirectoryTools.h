#pragma once
#include <string>
#include <vector>
#include <unordered_map>
#include "boost/filesystem.hpp"
#include "json/json.h"
#include "IJsonSerializable.h"

namespace HapticDirectoryTools {

	class HapticEnumList {
	public:
		std::vector<std::string> Patterns;
		std::vector<std::string> Sequences;
		std::vector<std::string> Experiences;

		std::string Namespace;
		std::string Studio;
		boost::filesystem::path Directory;

	};

	class EnumNode
	{
	public:
		EnumNode();
		EnumNode(HapticEnumList data);
		~EnumNode();
		std::string Namespace;
		HapticEnumList Data;
		std::unordered_map<std::string, EnumNode> Children;
	};

	class HapticConfig : public IJsonSerializable{
	public:
		HapticConfig();
		~HapticConfig();
		std::string Version;
		std::string Studio;
		std::string Package;
		virtual void Serialize(const Json::Value& root);
		virtual void Deserialize(const Json::Value& root);
		bool Validate() const;
	};

	
	class HapticEnumerator {
	public:
		HapticEnumerator(const std::string& path);
		~HapticEnumerator();
		EnumNode GenerateNodes(std::vector<HapticEnumList> enumLists) const;
		std::vector<std::tuple<boost::filesystem::path, HapticConfig>> EnumerateFiles() const;
		std::vector<HapticEnumList> GetEnums(std::vector<std::tuple<boost::filesystem::path, HapticConfig>> configs) const;
		std::vector<std::string> GetFileNames(boost::filesystem::path path) const;
		static std::vector<boost::filesystem::path> GetDirectories(const std::string& path);

	private:
		std::vector<std::string> _validExtensions;
		std::string _basePath;
		void insert(EnumNode& node, HapticEnumList& list) const;
		bool isValidFileExtension(const std::string& fileExtension) const;
		static const std::string PATTERN_DIR;
		static const std::string SEQUENCE_DIR;
		static const std::string EXPERIENCE_DIR;
		static const std::string MISSING_OR_INVALID_KEY;
	};
}

