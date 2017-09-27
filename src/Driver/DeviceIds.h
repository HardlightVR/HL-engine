#pragma once
#include <cstdint>
#include "PluginAPI.h"


template<typename T>
struct DeviceId {
	explicit DeviceId(uint32_t id) : value(id) {}
	DeviceId() : value(0) {}
	uint32_t value;
};

template<typename T>
struct NodeId {
	explicit NodeId(uint32_t id) : value(id) {}
	NodeId() : value(0) {}
	uint32_t value;
};

struct global {};
struct local {};



template<typename T>
bool operator==(const NodeId<T>& lhs, const NodeId<T>& rhs) noexcept {
	return lhs.value == rhs.value;
}


template<typename T>
bool operator==(const DeviceId<T>& lhs, const DeviceId<T>& rhs) noexcept {
	return lhs.value == rhs.value;
}

namespace std {
	
	template<typename T>
	struct hash<typename DeviceId<T>> {
		inline size_t operator()(const DeviceId<T>& id) const noexcept {
			return std::hash<uint32_t>{}(id.value);
		}
	};

	template<typename T>
	struct hash<typename NodeId<T>> {
		inline size_t operator()(const NodeId<T>& id) const noexcept{
			return std::hash<uint32_t>{}(id.value);
		}
	};

}