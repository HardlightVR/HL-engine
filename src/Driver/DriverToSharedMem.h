#pragma once
#include "SharedTypes.h"
#include "PluginAPI.h"
namespace DriverToShmem {


	static NullSpace::SharedMemory::Quaternion from(const nsvr_quaternion& quat) {
		return NullSpace::SharedMemory::Quaternion(quat.x, quat.y, quat.z, quat.w);
	}

	static NullSpace::SharedMemory::Vector3 from(const nsvr_vector3& vec3) {
		return NullSpace::SharedMemory::Vector3(vec3.x, vec3.y, vec3.z);
	}
}