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
#define szRDS _RXST("ADSK")

//-----------------------------------------------------------------------------
//----- ObjectARX EntryPoint
class CStep06App : public AcRxArxApp {

public:
	CStep06App () : AcRxArxApp () {}

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
	
	static void ADSKStep6_CREATEEMPLOYEE() {
		int res = RTNONE;
		Acad::ErrorStatus st = Acad::eOk;

		int id = 0;
		int cube = 0;
		constexpr size_t bufSize = 133;
		TCHAR fn[bufSize];
		TCHAR ln[bufSize];
		ads_point pt;
		if ((acedGetInt(_T("\nEnter employee id: "), &id)                       != RTNORM) ||
		    (acedGetInt(_T("\nEnter employee cubicle number: "), &cube)         != RTNORM) ||
		    (acedGetString(0, _T("\nEnter employee first name: "), fn, bufSize) != RTNORM) ||
		    (acedGetString(0, _T("\nEnter employee last name: "), ln, bufSize)  != RTNORM) ||
		    (acedGetPoint(NULL, _T("\nPick employee location..."), pt) != RTNORM))
		{
			acutPrintf(_T("\nERROR: wrong input"));
			return;
		}
		AcGePoint3d emplLoc = asPnt3d(pt);

		AcDbDatabase* db = acdbHostApplicationServices()->workingDatabase();
		AcDbBlockTable* blockTbl = NULL;
		st = db->getBlockTable(blockTbl, AcDb::kForRead);
		AcDbBlockTableRecord* ModelSpaceRec = NULL;
		if ((st = blockTbl->getAt(ACDB_MODEL_SPACE, ModelSpaceRec, AcDb::kForWrite)) != Acad::eOk) {
			blockTbl->close();
			acutPrintf(_T("\nERROR: cannot get MODEL_SPACE block record; getAt() = %d"), st);
			return;
		}

		ADSKEmployee* empl = new ADSKEmployee(emplLoc);
		acutPrintf(_T("#1"));
		empl->setId(id);
		acutPrintf(_T("#2"));
		empl->setCube(cube);
		empl->setFirstName(fn);
		empl->setLastName(ln);
		ModelSpaceRec->appendAcDbEntity(empl);

		empl->close();
		ModelSpaceRec->close();
		blockTbl->close();
	}
} ;

//-----------------------------------------------------------------------------
IMPLEMENT_ARX_ENTRYPOINT(CStep06App)

ACED_ARXCOMMAND_ENTRY_AUTO(CStep06App, ADSKStep6, _CREATEEMPLOYEE, CREATEEMPLOYEE, ACRX_CMD_TRANSPARENT, NULL)

