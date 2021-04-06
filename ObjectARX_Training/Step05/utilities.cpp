#include "StdAfx.h"
#include "utilities.h"

// 
// Create a new layer or return the ObjectId if it already exists 
// 
// In : 
//     const TCHAR* layerName     : layer name 
// Out : 
//     AcDbObjectId& layerId     : ObjectId of the created or existing layer 
// 

Acad::ErrorStatus createLayer (const TCHAR *layerName, AcDbObjectId &layerId) {
	Acad::ErrorStatus es ;
	AcDbLayerTable *pTable ;

	layerId =AcDbObjectId::kNull ;
	if ( (es =acdbHostApplicationServices ()->workingDatabase ()->getLayerTable (pTable, AcDb::kForRead)) == Acad::eOk ) {
		// Use the overload of AcDbLayerTable::getAt() that returns the id
		if ( (es =pTable->getAt (layerName, layerId, Adesk::kFalse)) != Acad::eOk ) {
			// Create a new layer table record using the layer name passed in
			AcDbLayerTableRecord *pLTRec =new AcDbLayerTableRecord ;
			pLTRec->setName (layerName) ;
			// Set other properties - color, linetype, state - if desired
			// this will require more input than this simple example provides
			if ( (es =pTable->upgradeOpen ()) == Acad::eOk ) {
				es =pTable->add (layerId, pLTRec) ;
				// Since the new layer was successfully added to the database,
				// close it - DON'T delete it
				pLTRec->close () ;
			} else {
				// The object is not part of the database, therefore we should
				// delete it to avoid a memory leak
				delete pLTRec ;
			}
		}
		pTable->close () ;
	}
	return (es) ;
}

// 
// Create a new block table record and add the entities of the employee to it 
// 
// In : 
//     const TCHAR* name : Name of block table record 
// 

Acad::ErrorStatus createBlockRecord (const TCHAR *name) {
	// First, check if a block of the same name already exists
	// by verifying in the current database block table.
	AcDbBlockTable *pBlockTable ;
	// Open the block table for read
	Acad::ErrorStatus es ;
	if ( (es =acdbHostApplicationServices ()->workingDatabase ()->getBlockTable (pBlockTable, AcDb::kForRead)) != Acad::eOk )
		return (es) ;

	if ( pBlockTable->has (name) == Adesk::kTrue ) {
		pBlockTable->close () ;
		return (Acad::eDuplicateKey) ;
	}
	// Now we know the block does not exist, so we create it
	// using the name passed in.
	AcDbBlockTableRecord *pBlockTableRecord =new AcDbBlockTableRecord ;
	pBlockTableRecord->setName (name) ;
	// To keep it simple, we use the origin for the insertion point
	pBlockTableRecord->setOrigin (AcGePoint3d::kOrigin) ;
	// Open the block table for write
	// since we are adding a new block definition
	if ( (es =pBlockTable->upgradeOpen ()) != Acad::eOk ) {
		delete pBlockTableRecord ;
		pBlockTable->close () ;
		return (es) ;
	}
	// Add the new block table record to the block table.
	// For now, the block table record is empty.
	if ( (es =pBlockTable->add (pBlockTableRecord)) != Acad::eOk ) {
		// The block table record has not been added
		// to the block table, so we have to delete it.
		pBlockTable->close();
		delete pBlockTableRecord;
		return (es) ;
	}
	pBlockTable->close () ;
	// Now the block table record is in the database, but is empty
	// (has no sub-entity).
	// Note that after having been added to the database, an object or an entity
	// is implicitely opened for write.
	//
	// So we create the sub entities to append to the block
	// which will represent a "happy face":
	// the block should consist of a round yellow face (circle)
	// two blue eyes (circles) and a red mouth (arc)
	AcDbCircle *pFace =new AcDbCircle (AcGePoint3d::kOrigin, AcGeVector3d::kZAxis, 1.0) ;
	AcDbCircle *pLeftEye =new AcDbCircle (AcGePoint3d (0.33, 0.25, 0.0), AcGeVector3d::kZAxis, 0.1) ;
	AcDbCircle *pRightEye =new AcDbCircle (AcGePoint3d (-0.33, 0.25, 0.0), AcGeVector3d::kZAxis, 0.1) ;
	AcDbArc *pMouth =new AcDbArc (AcGePoint3d (0, 0.5, 0), 1.0, 3.141592 + (3.141592 * 0.3), 3.141592 + (3.141592 * 0.7)) ;
	// Set the color property.
	pFace->setColorIndex (2) ;
	pLeftEye->setColorIndex (5) ;
	pRightEye->setColorIndex (5) ;
	pMouth->setColorIndex (1) ;
	// add the entities to the new block table record
	if ( (es =pBlockTableRecord->appendAcDbEntity (pFace)) != Acad::eOk ) {
		delete pFace ;
		delete pLeftEye ;
		delete pRightEye ;
		delete pMouth ;
		pBlockTableRecord->erase () ;
		pBlockTableRecord->close () ;
		return (es) ;
	}
	pFace->close () ;

	if ( (es =pBlockTableRecord->appendAcDbEntity (pLeftEye)) != Acad::eOk ) {
		delete pLeftEye ;
		delete pRightEye ;
		delete pMouth ;
		pBlockTableRecord->erase () ;
		pBlockTableRecord->close () ;
		return (es) ;
	}
	pLeftEye->close () ;

	if ( (es =pBlockTableRecord->appendAcDbEntity (pRightEye)) != Acad::eOk ) {
		delete pRightEye ;
		delete pMouth ;
		pBlockTableRecord->erase () ;
		pBlockTableRecord->close () ;
		return (es) ;
	}
	pRightEye->close () ;

	if ( (es =pBlockTableRecord->appendAcDbEntity (pMouth)) != Acad::eOk ) {
		delete pMouth ;
		pBlockTableRecord->erase () ;
		pBlockTableRecord->close () ;
		return (es) ;
	}
	pMouth->close () ;

	pBlockTableRecord->close () ;

	return (Acad::eOk) ;
}

Acad::ErrorStatus getSelBlock(AcDbObject*& blockRef, AcDbObjectId& blockRefId) {
	int res = RTNONE;
	ErrorStatus st = Acad::eOk;
	blockRef = NULL;
	blockRefId = AcDbObjectId::kNull;

	ads_name entName = { 0, 0 };
	ads_point pt = { 0, 0, 0 };
	if ((res = acedEntSel(_T("\nSelect a block"), entName, pt)) != RTNORM) {
		if (res == RTERROR) {
			acutPrintf(_T("\nERROR: wtf have you selected? acedEntSel() = %d"), res);
		}
		return Acad::eInvalidInput;
	}

	AcDbObjectId entId;
	acdbGetObjectId(entId, entName);
	AcDbObject* ent = NULL;
	if ((res = acdbOpenAcDbObject(ent, entId, AcDb::kForRead)) != Acad::eOk) {
		acutPrintf(_T("\nERROR: acdbOpenAcDbObject() = %d"), res);
		return st;
	}
	if (!ent->isKindOf(AcDbBlockReference::desc())) {
		acutPrintf(_T("\nERROR: you have not selected a block"));
		ent->close();
		return Acad::eInvalidInput;
	}
	AcDbObjectId blockId = (AcDbBlockReference::cast(ent))->blockTableRecord();
	AcDbBlockTableRecord* curEntBlock;
	if ((st = acdbOpenObject((AcDbObject*&)curEntBlock, blockId, AcDb::kForRead)) != Acad::eOk) {
		acutPrintf(_T("\nERROR: acdbOpenObject() = %d"), st);
		ent->close();
		return st;
	}

	TCHAR* blockName = NULL;
	curEntBlock->getName(blockName);
	curEntBlock->close();
	if (_tcscmp(blockName, _T("EMPLOYEE")) != 0) {
		acutPrintf(_T("\nERROR: you have not selected an EMPLOYEE block"));
		acdbFree(blockName);
		ent->close();
		return Acad::eInvalidInput;
	}
	acdbFree(blockName);
	blockRef = AcDbBlockReference::cast(ent);
	blockRefId = entId;
	return Acad::eOk;
}

