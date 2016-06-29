#include "StdAfx.h"
#include <iostream>
#include <stdio.h>




#include "SerialAdapter.h"
int main() {
	SerialAdapter adapter;
	adapter.Connect();
	
	std::cin.get();
}