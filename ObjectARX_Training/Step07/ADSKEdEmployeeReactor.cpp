// (C) Copyright 2002-2007 by Autodesk, Inc. 
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
//----- ADSKEdEmployeeReactor.cpp : Implementation of ADSKEdEmployeeReactor
//-----------------------------------------------------------------------------
#include "StdAfx.h"
#include "ADSKEdEmployeeReactor.h"

//-----------------------------------------------------------------------------
ADSKEdEmployeeReactor::ADSKEdEmployeeReactor (const bool autoInitAndRelease) : AcEditorReactor(), mbAutoInitAndRelease(autoInitAndRelease) {
	if ( autoInitAndRelease ) {
		if ( acedEditor )
			acedEditor->addReactor (this) ;
		else
			mbAutoInitAndRelease =false ;
	}
}

//-----------------------------------------------------------------------------
ADSKEdEmployeeReactor::~ADSKEdEmployeeReactor () {
	Detach () ;
}

//-----------------------------------------------------------------------------
void ADSKEdEmployeeReactor::Attach () {
	Detach () ;
	if ( !mbAutoInitAndRelease ) {
		if ( acedEditor ) {
			acedEditor->addReactor (this) ;
			mbAutoInitAndRelease =true ;
		}
	}
}

void ADSKEdEmployeeReactor::Detach () {
	if ( mbAutoInitAndRelease ) {
		if ( acedEditor ) {
			acedEditor->removeReactor (this) ;
			mbAutoInitAndRelease =false ;
		}
	}
}

AcEditor *ADSKEdEmployeeReactor::Subject () const {
	return (acedEditor) ;
}

bool ADSKEdEmployeeReactor::IsAttached () const {
	return (mbAutoInitAndRelease) ;
}

void ADSKEdEmployeeReactor::commandWillStart (const ACHAR* cmdStr) {
	if ( !isMonitoredCommand(cmdStr) ) {
		return ;
	}

	DocVars.docData().m_editCommand =true ;
	DocVars.docData().m_doRepositioning =false ;
	DocVars.docData().m_changedObjects.setLogicalLength(0) ;
	DocVars.docData().m_employeePositions.setLogicalLength(0) ;
}

void ADSKEdEmployeeReactor::commandEnded (const ACHAR* cmdStr) {
	DocVars.docData ().m_editCommand =false ;
	if ( !isMonitoredCommand (cmdStr) ) {
		return ;
	}

	DocVars.docData ().m_doRepositioning =true ;

	for ( int i = 0; i < DocVars.docData ().m_changedObjects.length (); ++i ) {
		AcDbObject* ent =NULL ;
		AcDbObjectId entId =DocVars.docData ().m_changedObjects[i] ;
		if ( acdbOpenObject (ent, entId, AcDb::kForRead) != Acad::eOk ) {
			acutPrintf(_T("\nERROR: cannot open object for reading")) ;
			continue ;
		}

		AcGePoint3d curPos =AcDbBlockReference::cast (ent)->position () ;
		AcGePoint3d oldPos =DocVars.docData ().m_employeePositions[i] ;
		if ( curPos != oldPos ) {
			ent->upgradeOpen () ;
			AcDbBlockReference::cast (ent)->setPosition (oldPos) ;
		}
		ent->close () ;
	}
}

bool ADSKEdEmployeeReactor::isMonitoredCommand(const ACHAR* cmdStr) {
	for ( const TCHAR* cmd : m_commands ) {
		if ( _tcscmp(cmdStr, cmd) == 0 ) {
			return true ;
		}
	}
	return false ;
}