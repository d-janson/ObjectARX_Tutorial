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
class CStep04App : public AcRxArxApp {

public:
	CStep04App () : AcRxArxApp () {}

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

	static void ADSKStep4_ADDENTRY()
	{
		constexpr TCHAR dictName[] = _T("ASDK_EMPLOYEE_DICTIONARY");
		int st = RTNONE;

		constexpr size_t bufSize = 100;
		TCHAR emplName[bufSize];
		if ((st = acedGetString(0, _T("\nEnter employee name to add: "), emplName, bufSize) != RTNORM)) {
			if (st != RTCAN) {
				acutPrintf(_T("\nERROR: acedGetString() = %d"), st);
			}
			return;
		}

		acutPrintf(_T("\nGetting NOD..."));
		AcDbDictionary* nod = NULL;
		if ((st = acdbHostApplicationServices()->workingDatabase()->getNamedObjectsDictionary(nod, AcDb::kForRead)) != Acad::eOk) {
			acutPrintf(_T("\nERROR: getNamedObjectsDictionary() = %d"), st);
			return;
		}

		acutPrintf(_T("\nGetting dictionary <%s> from NOD..."), dictName);
		AcDbDictionary* dict = NULL;
		if (nod->getAt(dictName, dict, AcDb::kForRead) == Acad::eKeyNotFound) {
			acutPrintf(_T("\nAdding new dictionary <%s> to NOD"), dictName);
			nod->upgradeOpen();
			dict = new AcDbDictionary;
			AcDbObjectId dictId;
			nod->setAt(dictName, dict, dictId);
		}

		acutPrintf(_T("\nGetting record <%s> in the dictionary <%s> from NOD..."), emplName, dictName);
		AcDbXrecord* emplRec = NULL;
		if (dict->getAt(emplName, emplRec, AcDb::kForRead) == Acad::eOk) {
			acutPrintf(_T("\nEmployee <%s> already in the dictionary <%s>"), emplName, dictName);
			dict->close();
			nod->close();
			return;
		}

		acutPrintf(_T("\nSetting record <%s> to the dictionary <%s> from NOD..."), emplName, dictName);
		dict->upgradeOpen();
		emplRec = new AcDbXrecord();
		AcDbObjectId emplId;
		if ((st = dict->setAt(emplName, emplRec, emplId)) == Acad::eInvalidKey) {
			acutPrintf(_T("\nERROR: cannot use <%s> in the dictionary <%s>; setAt() = %d"), emplName, dictName, st);
			emplRec->close();
			dict->close();
			nod->close();
			return;
		}

		emplRec->close();
		dict->close();
		nod->close();
	}
	
	static void ADSKStep4_LISTENTRIES()
	{
		constexpr TCHAR dictName[30] = _T("ASDK_EMPLOYEE_DICTIONARY");
		int st = RTNONE;

		acutPrintf(_T("\nGetting NOD..."));
		AcDbDictionary* nod = NULL;
		if ((st = acdbHostApplicationServices()->workingDatabase()->getNamedObjectsDictionary(nod, AcDb::kForRead)) != Acad::eOk) {
			acutPrintf(_T("\nERROR: getNamedObjectsDictionary() = %d"), st);
			return;
		}

		acutPrintf(_T("\nGetting dictionary <%s> from NOD..."), dictName);
		AcDbDictionary* dict = NULL;
		if (nod->getAt(dictName, dict, AcDb::kForRead) == Acad::eKeyNotFound) {
			acutPrintf(_T("\nNo such dictionary <%s> at NOD"), dictName);
			nod->close();
			return;
		}

		acutPrintf(_T("\nTravesring the dictionary <%s>..."), dictName);
		int i = 0;
		AcDbDictionaryIterator* dictIter = dict->newIterator();
		for (; !dictIter->done(); dictIter->next()) {
			acutPrintf(_T("\n%d %s"), i, dictIter->name());
			++i;
		}
		if (i == 0) {
			acutPrintf(_T("\nThe dictionary <%s> is empty"), dictName);
		}

		delete dictIter;
		dict->close();
		nod->close();
	}

	static void ADSKStep4_REMOVEENTRY()
	{
		constexpr TCHAR dictName[30] = _T("ASDK_EMPLOYEE_DICTIONARY");
		int st = RTNONE;

		constexpr size_t bufSize = 100;
		TCHAR emplName[bufSize];
		if ((st = acedGetString(0, _T("\nEnter employee name to remove: "), emplName, bufSize) != RTNORM)) {
			if (st != RTCAN) {
				acutPrintf(_T("\nERROR: acedGetString() = %d"), st);
			}
			return;
		}

		acutPrintf(_T("\nGetting NOD..."));
		AcDbDictionary* nod = NULL;
		if ((st = acdbHostApplicationServices()->workingDatabase()->getNamedObjectsDictionary(nod, AcDb::kForRead)) != Acad::eOk) {
			acutPrintf(_T("\nERROR: getNamedObjectsDictionary() = %d"), st);
			return;
		}

		acutPrintf(_T("\nGetting dictionary <%s> from NOD..."), dictName);
		AcDbDictionary* dict = NULL;
		if (nod->getAt(dictName, dict, AcDb::kForRead) == Acad::eKeyNotFound) {
			acutPrintf(_T("\nNo such dictionary <%s> at NOD"), dictName);
			nod->close();
			return;
		}

		acutPrintf(_T("\nGetting record <%s> in the dictionary <%s> from NOD..."), emplName, dictName);
		AcDbXrecord* emplRec = NULL;
		if (dict->getAt(emplName, emplRec, AcDb::kForWrite) == Acad::eKeyNotFound) {
			acutPrintf(_T("\nNo such record <%s> in the dictionary <%s>"), emplName, dictName);
			dict->close();
			nod->close();
			return;
		}

		emplRec->erase();

		emplRec->close();
		dict->close();
		nod->close();
	}
};

//-----------------------------------------------------------------------------

IMPLEMENT_ARX_ENTRYPOINT(CStep04App)

ACED_ARXCOMMAND_ENTRY_AUTO(CStep04App, ADSKStep4, _ADDENTRY, ADDENTRY, ACRX_CMD_MODAL, NULL)
ACED_ARXCOMMAND_ENTRY_AUTO(CStep04App, ADSKStep4, _LISTENTRIES, LISTENTRIES, ACRX_CMD_MODAL, NULL)
ACED_ARXCOMMAND_ENTRY_AUTO(CStep04App, ADSKStep4, _REMOVEENTRY, REMOVEENTRY, ACRX_CMD_MODAL, NULL)
