#include "stdafx.h"
#include "PluginInstance.h"

#include <boost/type_index.hpp>
#include <iostream>
#include "PluginAPI.h"
#include "DeviceContainer.h"


PluginInstance::~PluginInstance()
{
	{
		std::cout << "PL INSTANCE DESTRUCTOR\n";

	}

	
}

PluginInstance::PluginInstance(boost::asio::io_service& io,  std::string fileName, uint32_t id) 
	: m_fileName(fileName)
	, m_loaded{false}
	, m_id(id)
	, m_io(io)
	, m_logger(std::make_shared<my_logger>(boost::log::keywords::channel = "plugin"))
	, m_resources(std::make_unique<DeviceResources>())
	, m_pluginFunctions{}
	, m_pluginPointer(nullptr)
	, m_eventHandler()
	, m_apis()
	, m_pluginRegisterFunction{}
	, m_dll()

	
	
{
	boost::filesystem::path pluginPath(m_fileName);
	

	m_logger->add_attribute("Plugin", boost::log::attributes::constant<std::string>(pluginPath.filename().string()));


}






//Precondition: linked successfully
//does nothing if already loaded. May want to reinstantiate instead. We'll see.
bool PluginInstance::Instantiate()
{
	if (m_loaded) {
		return true;
	}


	if (m_pluginRegisterFunction) {
		m_pluginRegisterFunction(&m_pluginFunctions);

		m_loaded = m_pluginFunctions.init(&m_pluginPointer);
		
	}	
	
	return m_loaded;
	
	
}



//precondition: successfully loaded
bool PluginInstance::Configure()
{
	if (m_pluginFunctions.configure) {
		return m_pluginFunctions.configure(m_pluginPointer, reinterpret_cast<nsvr_core*>(this));
	}
	
	return false;
}

bool PluginInstance::tick_once(uint64_t dt)
{
	if (auto api = m_apis.GetApi<updateloop_api>()) {
		try {
			api->submit_update(dt);
		}
		catch (const std::runtime_error& err) {
			LOG_ERROR() << "Runtime error in plugin " << m_fileName << ": " << err.what();
			return false;
		}
	}

	bool result = true;
	return result;
}

bool PluginInstance::Link()
{
	boost::system::error_code loadFailure;

	m_dll = std::make_unique<boost::dll::shared_library>(m_fileName, boost::dll::load_mode::append_decorations | boost::dll::load_mode::load_with_altered_search_path, loadFailure);
	
	if (loadFailure) {
		LOG_ERROR() << "Failed to load " << m_fileName << " (.dll/.so): " << loadFailure.message();
		if (loadFailure.value() == 126) {
			LOG_INFO() << "--> Common causes besides not being able to find the dll:";
			LOG_INFO() << "--> 1) The DLL name does not correspond to the manifest name (Plugin_manifest.json => Plugin.dll)";
			LOG_INFO() << "--> 2) The DLL has additional dependencies which were not found";

		}
		return false;
	}


	if (!tryLoad(m_dll, "nsvr_plugin_register", m_pluginRegisterFunction)) {
		LOG_ERROR() << "Couldn't locate required symbol nsvr_plugin_register in " << m_fileName;

		return false;
	}
	else {
		LOG_INFO() << "Loaded " << m_fileName;
	}
	
	return true;

}

//precondition: linked successfully
bool PluginInstance::Unload()
{
	bool result = false;
	if (m_pluginFunctions.free) {
		result = m_pluginFunctions.free(m_pluginPointer);
		m_pluginPointer = nullptr;
	}
	m_dll.reset();

	m_loaded = false;
	return result;
}

bool PluginInstance::IsLoaded() const
{
	return m_loaded;
}

std::string PluginInstance::GetFileName() const
{
	return m_fileName;
}




uint32_t PluginInstance::GetId() const
{
	return m_id;
}

PluginApis & PluginInstance::apis()
{
	return m_apis;
}

void PluginInstance::RaiseEvent(nsvr_device_event_type type, nsvr_device_id id)
{
	m_eventHandler->Raise(type, id, this);
}
void PluginInstance::LogNow(nsvr_severity level, const std::string& component, const std::string& message)
{
	m_logger->add_attribute("Component", boost::log::attributes::mutable_constant<std::string>(component));
	BOOST_LOG_SEV(*m_logger, level) << message;
}
void PluginInstance::LogAsync(nsvr_severity level, const char * component, const char * message)
{
	//Problem: when the handler is invoked, the plugin may be destroyed (in the case of shutting down the app)
	//Tentative solution: make the PluginManager store shared_ptrs of each PluginInstance. Use enable_shared_from_this.
	//Then, when the handler is invoked, attempt to lock a weak_ptr which was created when the handler was created. 
	//If that works, then we can assume the plugin is still alive. (?)
	//void Widget::doThing() { io_service.post([weak_obj = std::weak_ptr<Widget>(shared_from_this())]() { //lock the weak_ptr };); }
	m_io.post([weak_plugin = std::weak_ptr<PluginInstance>(shared_from_this()), level, cmp = std::string(component), msg = std::string(message)]() {
		

		if (auto plugin = weak_plugin.lock()) {
			plugin->LogNow(level, cmp, msg);
		}
		else {
			//If you see this assert trigger, don't fear. Instead, delete the assert and the else statement, and add a note to the comment above
			//with the date, and that you observed the assert. If we stop getting crashes, and we see this a few times, I'd say the bug is solved..
			assert(false);
		}
	});
}

void PluginInstance::setDispatcher(std::unique_ptr<PluginEventSource> dispatcher)
{
	m_eventHandler = std::move(dispatcher);
}

void PluginInstance::addDeviceResources(DeviceResourceBundle resources)
{
	assert(resources->deviceDescriptor);
	nsvr_device_id id = resources->id.value;
	m_resources = std::move(resources);
	RaiseEvent(nsvr_device_event_device_connected, id);
}

//see if this can be const ref
PluginInstance::DeviceResources* PluginInstance::resources()
{
	return m_resources.get();
}

int PluginInstance::GetWorkingDirectory(nsvr_directory* outDir)
{
	std::fill(outDir->path, outDir->path + 500, 0);
	//this may not correspond to the dll location, but for now it does, so we use that
	boost::filesystem::path dllPath(m_fileName);
	std::string path = dllPath.parent_path().string();

	std::copy(path.begin(), path.end(), outDir->path);
	return 1;
}

