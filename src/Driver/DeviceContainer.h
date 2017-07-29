#pragma once
#include <memory>
#include "IHardwareDevice.h"
#include <unordered_map>

struct matches_region {

};
// Should break this into two interfaces: adding/removing, and the data operations
class DeviceContainer {
public:
	void AddDevice(const std::string&, std::vector<std::unique_ptr<NodalDevice>>);
	void RemoveDevice(const std::string&);
	
	
	template<typename Comparator, typename Result>
	void ForEachNode(Comparator, std::function<void(Node*, Result)>);





private:
	
	std::unordered_map<std::string, std::unique_ptr<NodalDevice>> m_devices;

	
};

template<typename Comparator, typename Result>
inline void DeviceContainer::ForEachNode(Comparator compare, std::function<void(Node*, Result)> handler)
{
	for (auto& node : m_devices) {
		if (compare(node.second)) {
			handler(node.second.get(), compare.result);
		}
	}
}
