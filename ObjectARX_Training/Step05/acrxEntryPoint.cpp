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
#define szRDS _RXST("ADSK")

//-----------------------------------------------------------------------------
//----- ObjectARX EntryPoint
class CStep05App : public AcRxArxApp {

private:
	static constexpr TCHAR dictName[] = _T("ADSK_EMPLOYEE_DICTIONARY");
	static constexpr TCHAR empDetName[] = _T("DETAILS");

public:
	CStep05App () : AcRxArxApp () {}

	virtual AcRx::AppRetCode On_kInitAppMsg (void *pkt) {
		// TODO: Load dependencies here
		AcRxObject* pSvc;
		if (!(pSvc = acrxServiceDictionary->at(ADSKEMPLOYEEDETAILS_DBXSERVICE)))
		{
			// Try to load the module, if it is not yet present 
			if (!acrxDynamicLinker->loadModule(_T("ADSKEmployeeDetails.dbx"), 0))
			{
				acutPrintf(_T("Unable to load ADSKEmployeeDetails.dbx. Unloading this application...\n"));
				return (AcRx::kRetError);
			}
		}

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
	
	static void ADSKStep05_CREATE() {
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

	static void ADSKStep05_SETLAYER() {
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

	static void ADSKStep05_ADDDETAIL() {
		int res = RTNONE;
		ErrorStatus st = Acad::eOk;

		AcDbObject* ent = NULL;
		AcDbObjectId entId = AcDbObjectId::kNull;
		if ((st = getSelBlock(ent, entId)) != Acad::eOk) {
			acutPrintf(_T("\nERROR: getBlock() = %d"), st);
			return;
		}

		int empId = 0;
		if ((res = acedGetInt(_T("\nEnter employee's ID: "), &empId)) != RTNORM) {
			ent->close();
			if (res == RTERROR) {
				acutPrintf(_T("\nERROR: acedGetInt() = %d"), res);
			}
			return;
		}
		int empCube = 0;
		if ((res = acedGetInt(_T("\nEnter employee's cubicle number: "), &empCube)) != RTNORM) {
			ent->close();
			if (res == RTERROR) {
				acutPrintf(_T("\nERROR: acedGetInt() = %d"), res);
			}
			return;
		}
		constexpr size_t bufSize = 133;
		TCHAR empFirstName[bufSize];
		if ((res = acedGetString(0, _T("\nEnter employee's first name: "), empFirstName, bufSize)) != RTNORM) {
			ent->close();
			if (res == RTERROR) {
				acutPrintf(_T("\nERROR: acedGetString() = %d"), res);
			}
			return;
		}
		TCHAR empLastName[bufSize];
		if ((res = acedGetString(0, _T("\nEnter employee's last name: "), empLastName, bufSize)) != RTNORM) {
			ent->close();
			if (res == RTERROR) {
				acutPrintf(_T("\nERROR: acedGetString() = %d"), res);
			}
			return;
		}

		AcDbObjectId extDictId;
		if ((extDictId = ent->extensionDictionary()) == AcDbObjectId::kNull) {
			acutPrintf(_T("\nCreating new extension dictionary at the block"));
			ent->upgradeOpen();
			ent->createExtensionDictionary();
			extDictId = ent->extensionDictionary();
			ent->downgradeOpen();
		}
		AcDbDictionary* extDict = NULL;
		if ((st = acdbOpenObject(extDict, extDictId, AcDb::kForRead, true)) != Acad::eOk) {
			acutPrintf(_T("\nERROR: cannot open extension dictionary = %d"), st);
			ent->close();
			return;
		}
		if (extDict->isErased()) {
			extDict->upgradeOpen();
			acutPrintf(_T("\nUnerasing the extension dictionary"));
			extDict->erase(false);
			extDict->downgradeOpen();
		}

		AcDbDictionary* dict = NULL;
		if ((st = extDict->getAt(dictName, dict, AcDb::kForRead)) == Acad::eKeyNotFound) {
			acutPrintf(_T("\nAdding new dictionary <%s> to the extension dictionary"), dictName);
			AcDbObjectId dictId;
			dict = new AcDbDictionary();
			extDict->upgradeOpen();
			extDict->setAt(dictName, dict, dictId);
		}
		else if (st != Acad::eOk) {
			acutPrintf(_T("\nERROR: cannot open dictionary <%s> = %d"), dictName, st);
			extDict->close();
			ent->close();
			return;
		}

		ADSKEmployeeDetails* empDet = NULL;
		if ((st = dict->getAt(empDetName, empDet, AcDb::kForWrite)) == Acad::eKeyNotFound) {
			acutPrintf(_T("\nAdding new ADSKEmployeeDetails object to the dictionary <%s>"), dictName);
			dict->upgradeOpen();
			empDet = new ADSKEmployeeDetails();
			empDet->setId(empId);
			empDet->setCube(empCube);
			empDet->setFirstName(empFirstName);
			empDet->setLastName(empLastName);
			AcDbObjectId empDetId;
			dict->setAt(empDetName, empDet, empDetId);
		}
		else {
			acutPrintf(_T("\nChanging ADSKEmployeeDetails object at the dictionary <%s>"), dictName);
			empDet->setId(empId);
			empDet->setCube(empCube);
			empDet->setFirstName(empFirstName);
			empDet->setLastName(empLastName);
		}


		empDet->close();
		dict->close();
		extDict->close();
		ent->close();
	}

	static void ADSKStep05_LISTDETAILS() {
		ErrorStatus st = Acad::eOk;

		AcDbObject* ent = NULL;
		AcDbObjectId entId = AcDbObjectId::kNull;
		if ((st = getSelBlock(ent, entId)) != Acad::eOk) {
			acutPrintf(_T("\nERROR: getBlock() = %d"), st);
			return;
		}

		AcDbObjectId extDictId;
		if ((extDictId = ent->extensionDictionary()) == AcDbObjectId::kNull) {
			acutPrintf(_T("\nERROR: No extension dictionary at the block"));
			ent->close();
			return;
		}
		AcDbDictionary* extDict = NULL;
		if ((st = acdbOpenObject(extDict, extDictId, AcDb::kForRead, true)) != Acad::eOk) {
			acutPrintf(_T("\nERROR: cannot open extension dictionary = %d"), st);
			ent->close();
			return;
		}

		AcDbDictionary* dict = NULL;
		if ((st = extDict->getAt(dictName, dict, AcDb::kForRead)) != Acad::eOk) {
			acutPrintf(_T("\nERROR: cannot open dictionary <%s> = %d"), dictName, st);
			extDict->close();
			ent->close();
			return;
		}

		ADSKEmployeeDetails* empDet = NULL;
		if ((st = dict->getAt(empDetName, empDet, AcDb::kForRead)) == Acad::eKeyNotFound) {
			acutPrintf(_T("\nNo employee details for the EMPLOYEE block"));
		}
		else {
			int id = 0;
			int cube = 0;
			TCHAR* fn = NULL;
			TCHAR* ln = NULL;
			empDet->id(id);
			empDet->cube(cube);
			empDet->firstName(fn);
			empDet->lastName(ln);
			acutPrintf(_T("\nEmployee details: %d, %d, %s, %s"), id, cube, fn, ln);
			free(fn);
			free(ln);
			empDet->close();
		}

		dict->close();
		extDict->close();
		ent->close();
	}

	static void ADSKStep05_REMOVEDETAIL() {
		ErrorStatus st = Acad::eOk;

		AcDbObject* ent = NULL;
		AcDbObjectId entId = AcDbObjectId::kNull;
		if ((st = getSelBlock(ent, entId)) != Acad::eOk) {
			acutPrintf(_T("\nERROR: getBlock() = %d"), st);
			return;
		}

		AcDbObjectId extDictId;
		if ((extDictId = ent->extensionDictionary()) == AcDbObjectId::kNull) {
			acutPrintf(_T("\nERROR: No extension dictionary at the block"));
			ent->close();
			return;
		}
		AcDbDictionary* extDict = NULL;
		if ((st = acdbOpenObject(extDict, extDictId, AcDb::kForRead, true)) != Acad::eOk) {
			acutPrintf(_T("\nERROR: cannot open extension dictionary = %d"), st);
			ent->close();
			return;
		}

		AcDbDictionary* dict = NULL;
		if ((st = extDict->getAt(dictName, dict, AcDb::kForRead)) != Acad::eOk) {
			acutPrintf(_T("\nERROR: cannot open dictionary <%s> = %d"), dictName, st);
			extDict->close();
			ent->close();
			return;
		}

		ADSKEmployeeDetails* empDet = NULL;
		if ((st = dict->getAt(empDetName, empDet, AcDb::kForWrite)) == Acad::eKeyNotFound) {
			acutPrintf(_T("\nNo employee details for the EMPLOYEE block"));
		}
		else {
			int id = 0;
			int cube = 0;
			TCHAR* fn = NULL;
			TCHAR* ln = NULL;
			empDet->id(id);
			empDet->cube(cube);
			empDet->firstName(fn);
			empDet->lastName(ln);
			acutPrintf(_T("\nErasing employee details: %d, %d, %s, %s"), id, cube, fn, ln);
			free(fn);
			free(ln);
			empDet->erase();
			empDet->close();
		}

		if (dict->numEntries() == 0) {
			dict->upgradeOpen();
			dict->erase();
			dict->close();
		}
		if (extDict->numEntries() == 0) {
			extDict->upgradeOpen();
			extDict->erase();
		}

		dict->close();
		extDict->close();
		ent->close();

	}
};

//-----------------------------------------------------------------------------
IMPLEMENT_ARX_ENTRYPOINT(CStep05App)

ACED_ARXCOMMAND_ENTRY_AUTO(CStep05App, ADSKStep05, _CREATE, CREATE, ACRX_CMD_TRANSPARENT, NULL)
ACED_ARXCOMMAND_ENTRY_AUTO(CStep05App, ADSKStep05, _SETLAYER, SETLAYER, ACRX_CMD_TRANSPARENT, NULL)
ACED_ARXCOMMAND_ENTRY_AUTO(CStep05App, ADSKStep05, _ADDDETAIL, ADDDETAIL, ACRX_CMD_TRANSPARENT, NULL)
ACED_ARXCOMMAND_ENTRY_AUTO(CStep05App, ADSKStep05, _LISTDETAILS, LISTDETAILS, ACRX_CMD_TRANSPARENT, NULL)
ACED_ARXCOMMAND_ENTRY_AUTO(CStep05App, ADSKStep05, _REMOVEDETAIL, REMOVEDETAIL, ACRX_CMD_TRANSPARENT, NULL)


