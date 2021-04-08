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

ADSKEdEmployeeReactor* pEdEmployeeReactor =NULL ;
ADSKEmployeeReactor*   pEmployeeReactor   =NULL ;

//-----------------------------------------------------------------------------
//----- ObjectARX EntryPoint
class CStep07App : public AcRxArxApp {

public:
	CStep07App () : AcRxArxApp () {}

	virtual AcRx::AppRetCode On_kInitAppMsg (void *pkt) {
		// TODO: Load dependencies here

		// You *must* call On_kInitAppMsg here
		AcRx::AppRetCode retCode =AcRxArxApp::On_kInitAppMsg (pkt) ;
		
		// TODO: Add your initialization code here
		pEdEmployeeReactor =new ADSKEdEmployeeReactor (true) ;
		pEmployeeReactor =new ADSKEmployeeReactor () ;

		return (retCode) ;
	}

	virtual AcRx::AppRetCode On_kUnloadAppMsg (void *pkt) {
		// TODO: Add your code here

		// You *must* call On_kUnloadAppMsg here
		AcRx::AppRetCode retCode =AcRxArxApp::On_kUnloadAppMsg (pkt) ;

		// TODO: Unload dependencies here
		detachAllEmployeeReactors();
		delete pEdEmployeeReactor ;
		delete pEmployeeReactor ;
		delete DocVars.docData ().m_pDbEmployeeReactor ;

		return (retCode) ;
	}

	virtual AcRx::AppRetCode On_kLoadDwgMsg (void* pkt) {
		AcRx::AppRetCode retCode =AcRxArxApp::On_kLoadDwgMsg (pkt) ;
		DocVars.docData ().m_pDbEmployeeReactor = new ADSKDbEmployeeReactor (acdbHostApplicationServices ()->workingDatabase ()) ;
		return (retCode) ;
	}

	virtual void RegisterServerComponents () {
	}

	static void ADSKStep7_CREATE() {
		// Add your code for command AsdkStep05._CREATE here
		// Add your code for command AsdkStep03._CREATE here
		// TODO: Implement the command

		// Create a new layer named "USER"
		// createLayer returns the object ID of the newly created layer
		AcDbObjectId layerId;
		if (createLayer(_T("USER"), layerId) != Acad::eOk) {
			acutPrintf(_T("\nERROR: Couldn't create layer record."));
			return;
		}
		// This is not always needed, but a call to 'applyCurDwgLayerTableChanges()'
		// will synchronize the newly created layer table change with the 
		// rest of the current DWG database.
		applyCurDwgLayerTableChanges();

		acutPrintf(_T("\nLayer USER successfully created."));

		// Create a new block definition named "EMPLOYEE"
		if (createBlockRecord(_T("EMPLOYEE")) != Acad::eOk)
			acutPrintf(_T("\nERROR: Couldn't create block record."));
		else
			acutPrintf(_T("\nBlock EMPLOYEE successfully created."));
	}

	static void ADSKStep7_SETLAYER() {
		// Add your code for command AsdkStep05._SETLAYER here
		// Iterate through Model Space to find every instance of the EMPLOYEE block
		// When found, change its layer to "USER"

		Acad::ErrorStatus es;
		AcDbBlockTable* pBlockTbl;
		AcDbBlockTableRecord* pMS;

		// Get the block table
		if ((es = acdbHostApplicationServices()->workingDatabase()->getBlockTable(pBlockTbl, AcDb::kForRead)) != Acad::eOk) {
			acutPrintf(_T("\nCouldn't open the block table!"));
			return;
		}
		// Get the Model Space record and open it for read.
		if ((es = pBlockTbl->getAt(ACDB_MODEL_SPACE, pMS, AcDb::kForWrite)) != Acad::eOk) {
			acutPrintf(_T("\nCouldn't get Model Space! Drawing corrupt.\n"));
			pBlockTbl->close();
			return;
		}
		pBlockTbl->close();

		// Declare the appropriate iterator type
		// Get the iterator from the object to be iterated through

		// In this case, the Model Space block table record will provide the iterator
		// start at the beginning of the record and skip deleted entities
		AcDbBlockTableRecordIterator* pBtrIter;
		if ((es = pMS->newIterator(pBtrIter)) != Acad::eOk) {
			acutPrintf(_T("\nCouldn't create Model Space iterator."));
			pMS->close();
			return;
		}

		TCHAR* blockName;
		AcDbEntity* pEnt;
		AcDbBlockTableRecord* pCurEntBlock;
		AcDbObjectId blockId;

		for (pBtrIter->start(); !pBtrIter->done(); pBtrIter->step()) {
			// First open each entity for read, just to check its class
			// if it's what we want, we can upgrade open later
			// Don't bother with erased entities
			if ((es = pBtrIter->getEntity(pEnt, AcDb::kForRead)) != Acad::eOk) {
				acutPrintf(_T("\nCouldn't open entity."));
				continue;
			}
			if (pEnt->isA() != AcDbBlockReference::desc()) {
				pEnt->close();
				continue;
			}
			// Get the insert's block table record and compare its name
			// to make sure we've got the right block.  If so, set the layer
			blockId = (AcDbBlockReference::cast(pEnt))->blockTableRecord();
			if (acdbOpenObject((AcDbObject*&)pCurEntBlock, blockId, AcDb::kForRead) == Acad::eOk) {
				pCurEntBlock->getName(blockName);
				if (_tcscmp(blockName, _T("EMPLOYEE")) == 0) {
					if (pEnt->upgradeOpen() == Acad::eOk)
						// setLayer also has an overload that takes a layer ID
						// but to avoid global variables we specify the layer name
						pEnt->setLayer(_T("USER"));
				}
				pCurEntBlock->close();
				acdbFree(blockName);
			}
			pEnt->close();
		}

		// delete, rather than close, the iterator object
		delete pBtrIter;
		pMS->close();
	}
};

//-----------------------------------------------------------------------------
IMPLEMENT_ARX_ENTRYPOINT(CStep07App)

ACED_ARXCOMMAND_ENTRY_AUTO(CStep07App, ADSKStep7, _CREATE, CREATE, ACRX_CMD_TRANSPARENT, NULL)
ACED_ARXCOMMAND_ENTRY_AUTO(CStep07App, ADSKStep7, _SETLAYER, SETLAYER, ACRX_CMD_TRANSPARENT, NULL)
