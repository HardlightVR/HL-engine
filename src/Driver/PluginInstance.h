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

	bool Configure();
	int RegisterInterface(NSVR_RegParams params);
	bool Unload();
	bool IsLoaded() const;
	std::string GetFileName() const;
	std::string GetDisplayName() const;


	PluginInstance(const PluginInstance&) = delete;
	const PluginInstance& operator=(const PluginInstance&) = delete;
	PluginInstance(PluginInstance&&);

	

	template<class THapticType>
	void Dispatch(const std::string& region, const std::string& iface, const THapticType* input);

private:
	std::vector<NSVR_Provider*> getProviders(const std::string& region, const std::string& iface);


	typedef std::function<int(NSVR_Plugin**)> plugin_creator_t;
	typedef std::function<int(NSVR_Plugin**)> plugin_destructor_t;
	typedef std::function<int(NSVR_Plugin*, NSVR_Core*)> plugin_configure_t;
	std::unique_ptr<boost::dll::shared_library> m_lib;
	
	NSVR_Plugin* m_rawPtr;

	plugin_creator_t m_creator;
	plugin_destructor_t m_destructor;
	plugin_configure_t m_configure;

	std::string m_displayName;
	std::string m_fileName;
	bool m_loaded;

	struct Interface {
		std::string Name;
		NSVR_Provider* Provider;
		Interface(std::string name, NSVR_Provider* provider) : Name(name), Provider(provider) {}
	};

	struct InterfaceList {
		std::vector<Interface> Interfaces;
		InterfaceList(): Interfaces() {};
		InterfaceList(std::vector<Interface>&& i) : Interfaces(std::move(i)) {}
	};
	std::unordered_map<std::string, InterfaceList> m_interfaces;




};

template<class TFunc>
bool tryLoad(std::unique_ptr<boost::dll::shared_library>& lib, const std::string& symbol, std::function<TFunc>& result) {
	result = lib->get<TFunc>(symbol);
	return result ? true : false;

}

//dispatches an event to any providers registered to the given region and interface
template<class THapticType>
void PluginInstance::Dispatch(const std::string& region, const std::string& iface, const THapticType* input)
{
	static_assert(getSymbolName<THapticType>() != "unknown", 
		"You must use REGISTER_INTERFACE on this type.\n" 
		"If you have implemented a new consumer type, simply put REGISTER_INTERFACE(TypeNameWithoutNSVRPrefixHere)\n" 
		"as the last line of your .h");


	std::vector<NSVR_Provider*> providers = getProviders(region, iface);

	typedef std::function<bool(NSVR_Provider*, const char*, const THapticType*)> SourceDllFunc;

	constexpr const char* dllFunctionName = getSymbolName<THapticType>();
	SourceDllFunc dllFunction;

	if (!tryLoad(m_lib, dllFunctionName, dllFunction)) {
		return;//can't load function
	} 


	for (NSVR_Provider* provider : providers) {
		dllFunction(provider, region.c_str(), input);	
	}
	
}


