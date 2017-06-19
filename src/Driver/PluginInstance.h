#include <string>
#include <unordered_map>
#include "../include/PluginAPI.h"


#include <boost/function.hpp>
#include <boost/dll.hpp>
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
	
private:
	typedef std::function<bool(NSVR_Plugin**)> plugin_creator_t;
	typedef std::function<bool(NSVR_Plugin**)> plugin_destructor_t;
	typedef std::function<bool(NSVR_Plugin*, NSVR_Region*)> plugin_register_regions_t;
	std::unique_ptr<boost::dll::shared_library> m_lib;
	
	NSVR_Plugin* m_rawPtr;

	plugin_creator_t m_creator;
	plugin_destructor_t m_destructor;
	plugin_register_regions_t m_registerRegions;

	std::string m_displayName;
	std::string m_fileName;
	bool m_loaded;



};

