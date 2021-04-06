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
//----- ADSKEmployee.cpp : Implementation of ADSKEmployee
//-----------------------------------------------------------------------------

#include "StdAfx.h"
#include "ADSKEmployee.h"

//-----------------------------------------------------------------------------
Adesk::UInt32 ADSKEmployee::kCurrentVersionNumber =1 ;

//-----------------------------------------------------------------------------
ACRX_DXF_DEFINE_MEMBERS (
	ADSKEmployee, AcDbEllipse,
	AcDb::kDHL_CURRENT, AcDb::kMReleaseCurrent, 
	AcDbProxyEntity::kNoOperation, ADSKEMPLOYEE,
ADSKEMPLOYEEAPP
|Product Desc:     A description for your object
|Company:          Your company name
|WEB Address:      Your company WEB site address
)

//-----------------------------------------------------------------------------
ADSKEmployee::ADSKEmployee () : ADSKEmployee(AcGePoint3d()) {
}

ADSKEmployee::ADSKEmployee (const AcGePoint3d& center) : AcDbEllipse(center, AcGeVector3d(0.0, 0.0, 1.0), AcGeVector3d(1.0, 0.0, 0.0), 1) {
	m_id = 0;
	m_cube = 0;
	m_firstName = NULL;
	m_lastName = NULL;
}

ADSKEmployee::~ADSKEmployee () {
	if (m_firstName) {
		free(m_firstName);
	}
	if (m_lastName) {
		free(m_lastName);
	}
}

//-----------------------------------------------------------------------------
//----- AcDbObject protocols
//- Dwg Filing protocol
Acad::ErrorStatus ADSKEmployee::dwgOutFields (AcDbDwgFiler *pFiler) const {
	assertReadEnabled () ;
	//----- Save parent class information first.
	Acad::ErrorStatus es =AcDbEllipse::dwgOutFields (pFiler) ;
	if ( es != Acad::eOk )
		return (es) ;
	//----- Object version number needs to be saved first
	if ( (es =pFiler->writeUInt32 (ADSKEmployee::kCurrentVersionNumber)) != Acad::eOk )
		return (es) ;
	//----- Output params
	//.....

	return (pFiler->filerStatus ()) ;
}

Acad::ErrorStatus ADSKEmployee::dwgInFields (AcDbDwgFiler *pFiler) {
	assertWriteEnabled () ;
	//----- Read parent class information first.
	Acad::ErrorStatus es =AcDbEllipse::dwgInFields (pFiler) ;
	if ( es != Acad::eOk )
		return (es) ;
	//----- Object version number needs to be read first
	Adesk::UInt32 version =0 ;
	if ( (es =pFiler->readUInt32 (&version)) != Acad::eOk )
		return (es) ;
	if ( version > ADSKEmployee::kCurrentVersionNumber )
		return (Acad::eMakeMeProxy) ;
	//- Uncomment the 2 following lines if your current object implementation cannot
	//- support previous version of that object.
	//if ( version < ADSKEmployee::kCurrentVersionNumber )
	//	return (Acad::eMakeMeProxy) ;
	//----- Read params
	//.....

	return (pFiler->filerStatus ()) ;
}

//- Dxf Filing protocol
Acad::ErrorStatus ADSKEmployee::dxfOutFields (AcDbDxfFiler *pFiler) const {
	assertReadEnabled () ;
	//----- Save parent class information first.
	Acad::ErrorStatus es =AcDbEllipse::dxfOutFields (pFiler) ;
	if ( es != Acad::eOk )
		return (es) ;
	es =pFiler->writeItem (AcDb::kDxfSubclass, _RXST("ADSKEmployee")) ;
	if ( es != Acad::eOk )
		return (es) ;
	//----- Object version number needs to be saved first
	if ( (es =pFiler->writeUInt32 (kDxfInt32, ADSKEmployee::kCurrentVersionNumber)) != Acad::eOk )
		return (es) ;
	//----- Output params
	//.....

	return (pFiler->filerStatus ()) ;
}

Acad::ErrorStatus ADSKEmployee::dxfInFields (AcDbDxfFiler *pFiler) {
	assertWriteEnabled () ;
	//----- Read parent class information first.
	Acad::ErrorStatus es =AcDbEllipse::dxfInFields (pFiler) ;
	if ( es != Acad::eOk || !pFiler->atSubclassData (_RXST("ADSKEmployee")) )
		return (pFiler->filerStatus ()) ;
	//----- Object version number needs to be read first
	struct resbuf rb ;
	pFiler->readItem (&rb) ;
	if ( rb.restype != AcDb::kDxfInt32 ) {
		pFiler->pushBackItem () ;
		pFiler->setError (Acad::eInvalidDxfCode, _RXST("\nError: expected group code %d (version #)"), AcDb::kDxfInt32) ;
		return (pFiler->filerStatus ()) ;
	}
	Adesk::UInt32 version =(Adesk::UInt32)rb.resval.rlong ;
	if ( version > ADSKEmployee::kCurrentVersionNumber )
		return (Acad::eMakeMeProxy) ;
	//- Uncomment the 2 following lines if your current object implementation cannot
	//- support previous version of that object.
	//if ( version < ADSKEmployee::kCurrentVersionNumber )
	//	return (Acad::eMakeMeProxy) ;
	//----- Read params in non order dependant manner
	while ( es == Acad::eOk && (es =pFiler->readResBuf (&rb)) == Acad::eOk ) {
		//switch ( rb.restype ) {
		//	//----- Read params by looking at their DXF code (example below)
		//	//case AcDb::kDxfXCoord:
		//	//	if ( version == 1 )
		//	//		cen3d =asPnt3d (rb.resval.rpoint) ;
		//	//	else 
		//	//		cen2d =asPnt2d (rb.resval.rpoint) ;
		//	//	break ;
		//	//.....

		//	default:
		//		//----- An unrecognized group. Push it back so that the subclass can read it again.
		//		pFiler->pushBackItem () ;
		//		es =Acad::eEndOfFile ;
		//		break ;
		//}
	}
	//----- At this point the es variable must contain eEndOfFile
	//----- - either from readResBuf() or from pushback. If not,
	//----- it indicates that an error happened and we should
	//----- return immediately.
	if ( es != Acad::eEndOfFile )
		return (Acad::eInvalidResBuf) ;

	return (pFiler->filerStatus ()) ;
}

//-----------------------------------------------------------------------------
//----- AcDbEntity protocols
Adesk::Boolean ADSKEmployee::subWorldDraw (AcGiWorldDraw *mode) {
	assertReadEnabled();
	AcDbEllipse::subWorldDraw(mode);
	
	constexpr size_t bufSize = 100;
	TCHAR buf[bufSize];
	AcGiTextStyle style;
	style.setTextSize(minorRadius() / 7.0);
	acutSPrintf(buf, bufSize, _T("id: %d"), m_id);
	AcGePoint3d pos = center() + ((style.textSize() + majorRadius()) * majorAxis() + 0.5 * minorRadius() * minorAxis());
	mode->geometry().text(pos, normal(), majorAxis(), buf, -1, Adesk::kTrue, style);
	acutSPrintf(buf, bufSize, _T("cubicle: %d"), m_cube);
	pos = pos - (2 * style.textSize() * minorAxis());
	mode->geometry().text(pos, normal(), majorAxis(), buf, -1, Adesk::kTrue, style);
	if (m_firstName) {
		pos = pos - (2 * style.textSize() * minorAxis());
		mode->geometry().text(pos, normal(), majorAxis(), m_firstName, -1, Adesk::kTrue, style);
	}
	if (m_lastName) {
		pos = pos - (2 * style.textSize() * minorAxis());
		mode->geometry().text(pos, normal(), majorAxis(), m_lastName, -1, Adesk::kTrue, style);
	}

	return Adesk::kFalse;
}


Adesk::UInt32 ADSKEmployee::subSetAttributes (AcGiDrawableTraits *traits) {
	assertReadEnabled () ;
	return (AcDbEllipse::subSetAttributes (traits)) ;
}

Acad::ErrorStatus ADSKEmployee::setId(const Adesk::Int32 id) {
	assertWriteEnabled();
	m_id = id;
	return Acad::eOk;
}

Acad::ErrorStatus ADSKEmployee::id(Adesk::Int32& id) {
	assertReadEnabled();
	id = m_id;
	return Acad::eOk;
}

Acad::ErrorStatus ADSKEmployee::setCube(const Adesk::Int32 cube) {
	assertWriteEnabled();
	m_cube = cube;
	return Acad::eOk;
}

Acad::ErrorStatus ADSKEmployee::cube(Adesk::Int32& cube) {
	assertReadEnabled();
	cube = m_cube;
	return Acad::eOk;
}

Acad::ErrorStatus ADSKEmployee::setFirstName(const TCHAR* firstName) {
	assertWriteEnabled();
	if (m_firstName) {
		free(m_firstName);
	}
	m_firstName = _tcsdup(firstName);
	return Acad::eOk;
}

Acad::ErrorStatus ADSKEmployee::firstName(TCHAR*& firstName) {
	assertReadEnabled();
	firstName = _tcsdup(m_firstName);
	return Acad::eOk;
}

Acad::ErrorStatus ADSKEmployee::setLastName(const TCHAR* lastName) {
	assertWriteEnabled();
	if (m_lastName) {
		free(m_lastName);
	}
	m_lastName = _tcsdup(lastName);
	return Acad::eOk;
}

Acad::ErrorStatus ADSKEmployee::lastName(TCHAR*& lastName) {
	assertReadEnabled();
	lastName = _tcsdup(m_lastName);
	return Acad::eOk;
}
