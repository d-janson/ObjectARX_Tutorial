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
#include "utilities.h"

//-----------------------------------------------------------------------------
#define szRDS _RXST("Adsk")

//-----------------------------------------------------------------------------
//----- ObjectARX EntryPoint
class CStep03App : public AcRxArxApp {

public:
	CStep03App () : AcRxArxApp () {}

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
	
	static void AdskStep3_Create() {
		AcDbObjectId layerId;
		createLayer(_T("USER"), layerId);
		createBlockRecord(_T("EMPLOYEE"));
		applyCurDwgLayerTableChanges();
	}
	static void AdskStep3_SetLayer() {
		Acad::ErrorStatus st = Acad::eOk;
		AcDbDatabase* db = acdbHostApplicationServices()->workingDatabase();

		AcDbBlockTable* blockTbl = NULL;
		st = db->getBlockTable(blockTbl, AcDb::kForRead);
		AcDbBlockTableRecord* ModelSpaceRec = NULL;
		if ((st = blockTbl->getAt(ACDB_MODEL_SPACE, ModelSpaceRec, AcDb::kForRead)) != Acad::eOk) {
			blockTbl->close();
			acutPrintf(_T("\nCommand internal error: getAt() = %d"), st);
			return;
		}

		AcDbBlockTableRecordIterator* blockIter = NULL;
		ModelSpaceRec->newIterator(blockIter);
		int i = 0;
		for (blockIter->start(); !blockIter->done(); blockIter->step()) {
			acutPrintf(_T("\n%d"), i++);
			AcDbEntity* ent = NULL;
			blockIter->getEntity(ent, AcDb::kForRead);
			if (ent->isA() == AcDbBlockReference::desc()) {
				acutPrintf(_T(" is a block"));
				AcDbObjectId blockId = static_cast<AcDbBlockReference*>(ent)->blockTableRecord();
				AcDbBlockTableRecord* blockRec = NULL;
				if ((st = acdbOpenObject(blockRec, blockId, AcDb::kForRead)) != Acad::eOk) {
					ent->close();
					acutPrintf(_T("\nCommand internal error: acdbOpenObject() = %d"), st);
					break;
				}

				TCHAR* name;
				blockRec->getName(name);
				if (_tcscmp(name, _T("EMPLOYEE")) == 0) {
					ent->upgradeOpen();
					ent->setLayer(_T("USER"));
				}
				delete name;

				blockRec->close();
			}
			else {
				acutPrintf(_T(" is not a block"));
			}
			ent->close();
		}
		delete blockIter;

		ModelSpaceRec->close();
		blockTbl->close();
	}
};

//-----------------------------------------------------------------------------
IMPLEMENT_ARX_ENTRYPOINT(CStep03App)

ACED_ARXCOMMAND_ENTRY_AUTO(CStep03App, AdskStep3, _Create, Create, ACRX_CMD_MODAL, NULL)
ACED_ARXCOMMAND_ENTRY_AUTO(CStep03App, AdskStep3, _SetLayer, SetLayer, ACRX_CMD_MODAL, NULL)

