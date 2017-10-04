#pragma once
#include <unordered_map>
#include <string>
class DiagnosticsMenu {
public:
	void keyval(std::string key, std::string value);
private:
	std::unordered_map<std::string, std::string> m_labels;
};

/*

core_get_plugins()


core_render_diagnostics_plugin(0)


*/