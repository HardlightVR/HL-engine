#pragma once
#include <string>
#include <unordered_map>
#include "../include/PluginAPI.h"

#include <chrono>
#include <boost/function.hpp>
#include <boost/dll.hpp>

#include "events/briefTaxel.h"
#include "events/lastingTaxel.h"

class RegionRegistry;
class PluginInstance
{
public:
	

	explicit PluginInstance(std::string fileName);
	~PluginInstance();
	bool Link();
	bool Load();
	bool Configure(RegionRegistry& registry);

	bool Unload();
	bool IsLoaded() const;
	std::string GetFileName() const;
	std::string GetDisplayName() const;
	//This is temporary
	//I swear
	NSVR_Provider* GetRawHandle();
	//testing purposes only!

	PluginInstance(const PluginInstance&) = delete;
	const PluginInstance& operator=(const PluginInstance&) = delete;
	PluginInstance(PluginInstance&&);

	

	template<class THapticType>
	void Dispatch( const char* region, const THapticType* input);

private:
	typedef std::function<int(NSVR_Provider**)> plugin_creator_t;
	typedef std::function<int(NSVR_Provider**)> plugin_destructor_t;
	typedef std::function<int(NSVR_Provider*, NSVR_Core*)> plugin_configure_t;
	std::unique_ptr<boost::dll::shared_library> m_lib;
	
	NSVR_Provider* m_rawPtr;

	plugin_creator_t m_creator;
	plugin_destructor_t m_destructor;
	plugin_configure_t m_configure;

	std::string m_displayName;
	std::string m_fileName;
	bool m_loaded;





};

template<class TFunc>
bool tryLoad(std::unique_ptr<boost::dll::shared_library>& lib, const std::string& symbol, std::function<TFunc>& result) {
	result = lib->get<TFunc>(symbol);
	return result ? true : false;

}


template<class THapticType>
void PluginInstance::Dispatch(const char* region, const THapticType* input)
{
	static_assert(getSymbolName<THapticType>() != "unknown", 
		"You must use REGISTER_INTERFACE on this type.\n" 
		"If you have implemented a new consumer type, simply put REGISTER_INTERFACE(TypeNameWithoutNSVRPrefixHere)\n" 
		"as the last line of your .h");


	//This matches the signature of all NSVR_Provider_Consume_X functions
	typedef std::function<bool(NSVR_Provider*, const char*, const THapticType*)> SourceDllFunc;
	
	constexpr const char* dllFunctionName = getSymbolName<THapticType>();

	SourceDllFunc dllFunction;

	if (tryLoad(m_lib, dllFunctionName, dllFunction)) {
		dllFunction(m_rawPtr, region, input);
	}
	
}


