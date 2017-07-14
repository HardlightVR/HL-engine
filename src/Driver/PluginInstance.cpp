#include "stdafx.h"
#include "PluginInstance.h"

#include <boost/type_index.hpp>
#include <iostream>

#include "FunctionPointerTemplates.h"

PluginInstance::PluginInstance(std::string fileName, HardwareDataModel& model) :
	m_fileName(fileName), 
	m_loaded{ false },
	m_model(model)
{
}




PluginInstance::~PluginInstance()
{
}


//Precondition: linked successfully
//does nothing if already loaded. May want to reinstantiate instead. We'll see.
bool PluginInstance::Load()
{
	if (m_loaded) {
		return true;
	}

	m_loaded = true;

	if (m_creator) {
		return m_creator(&m_rawPtr);
	}	
	else {
		return false;
	}
	
}

//Helper function to assign a lambda and context to an NSVR_Callback.
template<typename TContext, typename TCallable>
void registerCallback(NSVR_Configuration* config, const std::string& name, TContext& context, TCallable&& lambda) {
	//We need it to be static because we can't point to a temp lambda
	static auto static_pointer = to_function_pointer(lambda);

	constexpr bool is_same = std::is_same<TContext*, typename function_traits<TCallable>::ctx>::value;
	static_assert(is_same, "You must pass in the same type for the context and your lambda!");
	

	config->Callbacks[name].callback = static_pointer;
	config->Callbacks[name].context = reinterpret_cast<nsvr_core_ctx*>(&context);
}


//precondition: successfully loaded
bool PluginInstance::Configure()
{
	NSVR_Configuration config;

	


	if (m_configure) {
		return m_configure(m_rawPtr, &config);
	}
	else {
		return false;
	}
}

bool PluginInstance::Link()
{
	boost::system::error_code loadFailure;
	m_lib = std::make_unique<boost::dll::shared_library>(m_fileName, boost::dll::load_mode::append_decorations, loadFailure);
	if (loadFailure) {
		std::cout << "Failed to load " << m_fileName << ": " << loadFailure.message() << ".\n";
		return false;
	}

	std::cout << "Loaded " << m_fileName << ".\n";

	if (!tryLoad(m_lib, "NSVR_Init", m_creator)) {
		return false;
	}

	if (!tryLoad(m_lib, "NSVR_Free", m_destructor)) {
		return false;
	}
	
	if (!tryLoad(m_lib, "NSVR_Configure", m_configure)) {
		return false;
	}

	
	return true;

}

//precondition: linked successfully
bool PluginInstance::Unload()
{
	bool result = false;
	if (m_destructor) {
		result = m_destructor(&m_rawPtr);
	}
	m_lib.reset();

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

std::string PluginInstance::GetDisplayName() const
{
	return m_displayName;
}


