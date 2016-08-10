#include "Loader.h"
#include "HapticFileInfo.h"
#include <iostream>

using namespace boost::filesystem;

Loader::Loader()
{
}


Loader::~Loader()
{
}

SequenceLoader::SequenceLoader(shared_ptr<Parser> p, shared_ptr<unordered_map<string, vector<SequenceItem>>> s)
	:_parser(p), _sequences(s) {
}

SequenceLoader::~SequenceLoader()
{
}

bool SequenceLoader::Load(const HapticFileInfo& fileInfo)
{
	if (_sequences->find(fileInfo.FullId) != _sequences->end()) {
		return true;
	}

	auto validNames = fileInfo.GetValidFileNames();
	path directory = _parser->GetDirectory(fileInfo.FullyQualifiedPackage) /= fileInfo.GetDirectory();
	for (auto name : validNames)
	{
		
		auto path = directory /= name;
		if (exists(path)) {
			vector<SequenceItem> sequence = _parser->ParseSequence(path);
			_sequences->at(fileInfo.FullId) = sequence;
			return true;
		} 
		
	}

	std::cout << "Failed to load " << fileInfo.ToString() << ": file not found (in package " << fileInfo.FullyQualifiedPackage << ")\n";
	return false;
}

PatternLoader::PatternLoader(shared_ptr<Parser> p, unique_ptr<SequenceLoader> seq, shared_ptr<unordered_map<string, vector<Frame>>> files)
:_sequenceLoader(std::move(seq)), _parser(p), _patterns(files){
}

PatternLoader::~PatternLoader()
{
}

bool PatternLoader::Load(const HapticFileInfo& fileInfo)
{
	if (_patterns->find(fileInfo.FullId) != _patterns->end()) {
		return true;
	}
	return false;
}
