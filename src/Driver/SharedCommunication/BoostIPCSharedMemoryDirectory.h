#pragma once

#include <string>
  
namespace boost {
	namespace interprocess {
		namespace ipcdetail {
			void get_shared_dir(std::string& shared_dir) {
				//NOTE: this is obviously windows only! And will clearly fail if C:\ProgramData doesn't exist!
				shared_dir = R"(C:\ProgramData\Hardlight VR\ipc)";
			}
		}
	}
}