#include "stdafx.h"
#include "HapticDirectoryTools.h"
#include <boost/filesystem.hpp>
#include <iostream>

#include <boost/algorithm/string.hpp>
using namespace HapticDirectoryTools;
using namespace boost::filesystem;

class JsonValueInvalidException : public std::runtime_error {
public:
	JsonValueInvalidException(const std::string& key) : std::runtime_error(std::string("Key '" + key + "' has an invalid value or is not present").c_str()) {}

};
EnumNode::EnumNode()
{
	
}

EnumNode::EnumNode(HapticEnumList data)
{
	this->Data = data;
	this->Namespace = data.Namespace;
	
}

EnumNode::~EnumNode()
{
}



HapticEnumerator::HapticEnumerator(const std::string& path) :
	_validExtensions{ ".pattern",  ".pat",
					 ".sequence", ".seq",
					 ".experience", ".exp" },
	_basePath(path)
{
}

HapticEnumerator::~HapticEnumerator() {

}

HapticConfig::HapticConfig()
{
	
}

HapticConfig::~HapticConfig()
{
	
}

bool HapticConfig::Validate() const
{
	return this->Package != "INVALID" && this->Studio != "INVALID";
}
Json::Value parseKeyOrThrow(const Json::Value& root, const std::string& key)
{
	if (root.isMember(key))
	{
		return root[key];
	} else
	{
		throw JsonValueInvalidException(key);
	}
}

void HapticConfig::Deserialize(const Json::Value& root)
{

	this->Package = parseKeyOrThrow(root, "package").asString();
	this->Studio = parseKeyOrThrow(root, "studio").asString();
	this->Version = root.get("version", "1.0").asString();
}

void HapticConfig::Serialize(const Json::Value& root)
{
	
}


const std::string HapticEnumerator::MISSING_OR_INVALID_KEY = "INVALID";
const std::string HapticEnumerator::EXPERIENCE_DIR = "experiences";
const std::string HapticEnumerator::PATTERN_DIR = "patterns";
const std::string HapticEnumerator::SEQUENCE_DIR = "sequences";

EnumNode HapticEnumerator::GenerateNodes(std::vector<HapticEnumList> enumLists) const
{
	EnumNode root;
	for (auto list : enumLists)
	{
		insert(root, list);
	}
	return root;
	
}

 std::vector<boost::filesystem::path> HapticEnumerator::GetDirectories(const std::string& path)
{
	std::vector<boost::filesystem::path> dirs;
	directory_iterator it{ path };
	while (it != directory_iterator{}) {
		if (is_directory(*it)) {
			dirs.push_back(it->path());
		}
		*it++;
	}
	return dirs;
}

 Json::Value readJsonFromStream(const boost::filesystem::path& path)
 {
	 Json::Value val;
	 std::ifstream configStream(path.string(), std::ifstream::binary);
	 configStream >> val;
	 return val;
 }
std::vector<std::tuple<path, HapticConfig>> HapticEnumerator::EnumerateFiles() const
{
	auto potentialPackages = GetDirectories(_basePath);
	std::vector<std::tuple<path, HapticConfig>> packages;
	for (const auto& path : potentialPackages)
	{
		auto configPath = path;
		configPath /= "config.json";
		if (exists(configPath)) {
			Json::Value configJson = readJsonFromStream(configPath);
			HapticConfig tempConfig;
			try {
				tempConfig.Deserialize(configJson);
				packages.push_back(std::make_tuple(path, tempConfig));
			} catch (JsonValueInvalidException e)
			{
				std::cout << "In config file for directory " << path.filename() << ": " << e.what() << "\n";
				std::cout << "Not loading the haptics found in directory " << path.filename() << "\n";
			}
			
		}
	}

	return packages;

}

boost::filesystem::path PathCombine(const path& p1, const std::string& p2)
 {
	path result = p1;
	result /= path(p2);
	return result;
 }
std::vector<HapticEnumList> HapticEnumerator::GetEnums(std::vector<std::tuple<path, HapticConfig>> configs) const
{
	std::vector<HapticEnumList> hapticEnums;
	hapticEnums.reserve(configs.size());

	for (auto configTuple : configs) {
		HapticEnumList enumList;
		path directory;
		HapticConfig config;
		std::tie(directory, config) = configTuple;
		enumList.Patterns = GetFileNames(PathCombine(directory, PATTERN_DIR));
		enumList.Sequences = GetFileNames(PathCombine(directory, SEQUENCE_DIR));
		enumList.Experiences = GetFileNames(PathCombine(directory, EXPERIENCE_DIR));
	
		enumList.Namespace = config.Package;
		enumList.Studio = config.Studio;
		enumList.Directory = directory;
		hapticEnums.push_back(enumList);
	}

	return hapticEnums;




}

bool HapticEnumerator::isValidFileExtension(const std::string& extension) const
{
	return std::find(_validExtensions.begin(), _validExtensions.end(), extension) != _validExtensions.end();

}

std::vector<std::string> HapticEnumerator::GetFileNames(path path) const {
	if (!is_directory(path)) {
		return std::vector<std::string>();
	}
	std::vector<std::string> names;
	directory_iterator end_itr;

	for (directory_iterator itr(path); itr != end_itr; ++itr)
	{
		if (is_regular_file(itr->path()))
		{
			if (!isValidFileExtension(itr->path().extension().string()))
			{
				continue;
			}
			names.push_back(itr->path().filename().string());
		}
	}
	

	return names;

}
std::vector<std::string> GetNamespaceComponents(const std::string& nameSpace)
{
	std::vector<std::string> result;
	boost::split(result, nameSpace, boost::algorithm::is_any_of("."));
	return result;
}
void HapticEnumerator::insert(EnumNode& node,  HapticEnumList& list) const
{
	auto components = GetNamespaceComponents(list.Namespace);
	std::string topLevel = components[0];
	if (components.size() == 1)
	{
		node.Children[topLevel] = EnumNode(list);
			
	} else
	{
		list.Namespace = list.Namespace.substr(list.Namespace.find_first_of(".")+1);

		if (node.Children.find(topLevel) != node.Children.end())
		{
			insert(node.Children[topLevel], list);
		} else
		{
			node.Children[topLevel] = EnumNode();
			insert(node.Children[topLevel], list);
		}
	}
	
}


