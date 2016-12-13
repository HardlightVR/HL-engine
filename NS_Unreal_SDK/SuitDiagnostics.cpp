#include "stdafx.h"
#include "SuitDiagnostics.h"



SuitDiagnostics::SuitDiagnostics()
{
}


SuitDiagnostics::~SuitDiagnostics()
{
}

void SuitDiagnostics::ReceiveDiagnostics(const SuitDiagnosticInfo & info)
{
	std::cout << "Got diagnostics!" << '\n';
	std::cout << "Count = " << info.Count << '\n';
}
