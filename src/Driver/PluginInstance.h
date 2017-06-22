#pragma once
#include <string>
#include <unordered_map>
#include "../include/PluginAPI.h"

#include <chrono>
#include <boost/function.hpp>
#include <boost/dll.hpp>

#include "events/briefhapticprimitive.h"

#define concat(first, second) first second

#define consumer_function(type) concat("NSVR_Provider_Consume_", type)

template <typename T>
constexpr const char* getUnmangledConsumeFunction(void) { return "unknown"; }

template <>
constexpr const char* getUnmangledConsumeFunction<NSVR_BriefHapticPrimitive>(void) { return consumer_function("BriefHapticPrimitive"); }
class PluginInstance
{
public:
	

	explicit PluginInstance(std::string fileName);
	~PluginInstance();
	bool Load();
	bool Link();
	bool Unload();
	bool IsLoaded() const;
	std::string GetFileName() const;
	std::string GetDisplayName() const;

	PluginInstance(const PluginInstance&) = delete;
	const PluginInstance& operator=(const PluginInstance&) = delete;
	PluginInstance(PluginInstance&&);

	bool RegisterRegions(NSVR_Region* regions);
	

	template<class T>
	void Consume( const T* input);
private:
	typedef std::function<int(NSVR_Provider**)> plugin_creator_t;
	typedef std::function<int(NSVR_Provider**)> plugin_destructor_t;
	typedef std::function<int(NSVR_Provider*, NSVR_Region*)> plugin_register_regions_t;
	std::unique_ptr<boost::dll::shared_library> m_lib;
	
	NSVR_Provider* m_rawPtr;

	plugin_creator_t m_creator;
	plugin_destructor_t m_destructor;
	plugin_register_regions_t m_registerRegions;

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
void PluginInstance::Consume(const THapticType* input)
{
	//This matches the signature of all NSVR_Provider_Consume_X functions
	typedef std::function<bool(NSVR_Provider*, const THapticType*)> SourceDllFunc;
	
	constexpr const char* dllFunctionName = getUnmangledConsumeFunction<THapticType>();

	SourceDllFunc dllFunction;

	if (tryLoad(m_lib, dllFunctionName, dllFunction)) {
		dllFunction(m_rawPtr, input);
	}
	
}
