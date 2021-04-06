// (C) Copyright 2002-2012 by Autodesk, Inc. 
//
// Permission to use, copy, modify, and distribute this software in
// object code form for any purpose and without fee is hereby granted, 
// provided that the above copyright notice appears in all copies and 
// that both that copyright notice and the limited warranty and
// restricted rights notice below appear in all supporting 
// documentation.
//
// AUTODESK PROVIDES THIS PROGRAM "AS IS" AND WITH ALL FAULTS. 
// AUTODESK SPECIFICALLY DISCLAIMS ANY IMPLIED WARRANTY OF
// MERCHANTABILITY OR FITNESS FOR A PARTICULAR USE.  AUTODESK, INC. 
// DOES NOT WARRANT THAT THE OPERATION OF THE PROGRAM WILL BE
// UNINTERRUPTED OR ERROR FREE.
//
// Use, duplication, or disclosure by the U.S. Government is subject to 
// restrictions set forth in FAR 52.227-19 (Commercial Computer
// Software - Restricted Rights) and DFAR 252.227-7013(c)(1)(ii)
// (Rights in Technical Data and Computer Software), as applicable.
//

//-----------------------------------------------------------------------------
//----- acrxEntryPoint.cpp
//-----------------------------------------------------------------------------
#include "StdAfx.h"
#include "resource.h"

//-----------------------------------------------------------------------------
#define szRDS _RXST("Adsk")

//-----------------------------------------------------------------------------
//----- ObjectARX EntryPoint
class CStep02App : public AcRxArxApp {

public:
	CStep02App () : AcRxArxApp () {}

	virtual AcRx::AppRetCode On_kInitAppMsg (void *pkt) {
		// TODO: Load dependencies here

		// You *must* call On_kInitAppMsg here
		AcRx::AppRetCode retCode =AcRxArxApp::On_kInitAppMsg (pkt) ;
		
		// TODO: Add your initialization code here

		return (retCode) ;
	}

	virtual AcRx::AppRetCode On_kUnloadAppMsg (void *pkt) {
		// TODO: Add your code here

		// You *must* call On_kUnloadAppMsg here
		AcRx::AppRetCode retCode =AcRxArxApp::On_kUnloadAppMsg (pkt) ;

		// TODO: Unload dependencies here

		return (retCode) ;
	}

	virtual void RegisterServerComponents () {
	}

	static void AdskStep2_MyInput () {
		int res = RTNONE;

		int opts = RSG_NONULL | RSG_NOZERO | RSG_NONEG;
		res = acedInitGet(opts, NULL);
		if (res == RTERROR) {
			acutPrintf(_T("Command internal error.\n"));
			return;
		}
		int intVal = 0;
		res = acedGetInt(_T("Enter a positive nonzero integer value: "), &intVal);
		if (res == RTNORM) {
			acutPrintf(_T("%d\n"), intVal);
		}
		else if (res == RTCAN) {
			return;
		}
		else if (res == RTERROR) {
			acutPrintf(_T("Command internal error.\n"));
			return;
		}

		opts = 0;
		TCHAR kwords[] = _T("PI A B C");
		res = acedInitGet(opts, kwords);
		if (res == RTERROR) {
			acutPrintf(_T("Command internal error.\n"));
			return;
		}
		constexpr double piVal = 3.14;
		constexpr double aVal = 10.0;
		constexpr double bVal = 11.0;
		constexpr double cVal = 12.0;
		double realVal = cVal;
		res = acedGetReal(_T("Enter a real value: "), &realVal);
		if (res == RTKWORD) {
			constexpr size_t sz = 50;
			TCHAR kw[sz];
			res = acedGetInput(kw, sz);
			if (res == RTERROR) {
				acutPrintf(_T("Command internal error.\n"));
				return;
			}

			if (!_tcsncmp(kw, _T("PI"), sz)) {
				realVal = piVal;
			}
			else if (!_tcsncmp(kw, _T("A"), sz)) {
				realVal = aVal;
			}
			else if (!_tcsncmp(kw, _T("B"), sz)) {
				realVal = bVal;
			}
			else if (!_tcsncmp(kw, _T("C"), sz)) {
				realVal = cVal;
			}
		}
		else if (res == RTERROR) {
			acutPrintf(_T("Command internal error.\n"));
			return;
		}
		if ((res == RTNORM) || (res == RTNONE)) { // no else if: checking res of acedGetReal() or res of acedGetInput()
			acutPrintf(_T("%.20q0\n"), realVal);
		}
	}
	
	static void AdskStep2_MySelect() {
		int res = RTNONE;

		ads_name entRes;
		ads_point entPnt;
		res = acedEntSel(NULL, entRes, entPnt);
		if (res == RTNORM) {
			acutPrintf(_T("%I64\n"), entRes[0]);
		}
		else if (res == RTERROR) {
			acutPrintf(_T("Command internal error.\n"));
			return;
		}
	}
};

//-----------------------------------------------------------------------------
IMPLEMENT_ARX_ENTRYPOINT(CStep02App)

ACED_ARXCOMMAND_ENTRY_AUTO(CStep02App, AdskStep2, _MyInput, MyInput, ACRX_CMD_TRANSPARENT, NULL)
ACED_ARXCOMMAND_ENTRY_AUTO(CStep02App, AdskStep2, _MySelect, MySelect, ACRX_CMD_TRANSPARENT, NULL)

