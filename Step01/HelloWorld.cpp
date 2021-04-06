#include "pch.h"
#include "tchar.h"
#include <aced.h>
#include <rxregsvc.h>

void initApp();
void unloadApp();
void helloWorld();

void initApp()
{
	acedRegCmds->addCommand(_T("HELLOWORLD_COMMANDS"),
		                    _T("Hello"),
		                    _T("Bonjour"),
		                    ACRX_CMD_TRANSPARENT,
		                    helloWorld);
}

void unloadApp()
{
	acedRegCmds->removeGroup(_T("HELLOWORLD_COMMANDS"));
}

void helloWorld()
{
	acutPrintf(_T("Hello, World!"));
}

extern "C"
AcRx::AppRetCode acrxEntryPoint(AcRx::AppMsgCode msg, void* pkt)
{
	switch (msg)
	{
	case AcRx::kInitAppMsg:
		acrxDynamicLinker->unlockApplication(pkt);
		acrxRegisterAppMDIAware(pkt);
		initApp();
		break;
	case AcRx::kUnloadAppMsg:
		unloadApp();
		break;
	default:
		break;
	}
	return AcRx::kRetOK;
}