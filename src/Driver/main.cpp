
#include "stdafx.h"

#include "NSDriverApi.h"




int main()
{
	
	NSVR_Driver_Context_t* context = NSVR_Driver_Create();
	NSVR_Driver_StartThread(context);
	std::cin.get();
	NSVR_Driver_Shutdown(context);
	NSVR_Driver_Destroy(context);
	return 0;
}

