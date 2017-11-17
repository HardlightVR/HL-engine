#pragma once

#include <string>
  
namespace boost {
	namespace interprocess {
		namespace ipcdetail {
			//not in boost 1.61 :(
			void get_shared_dir(std::string& shared_dir) {
				shared_dir = R"(C:\ProgramData\Hardlight VR\ipc)";
			}
		}
	}
}