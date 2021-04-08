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
extern ADSKEmployeeReactor* pEmployeeReactor;

Acad::ErrorStatus attachEmployeeReactorToAllEmployee (bool attach) {
	Acad::ErrorStatus es =Acad::eOk ;

	AcDbBlockTable* pBlockTable =NULL ;
	if ( (es =acdbHostApplicationServices ()->workingDatabase ()->getBlockTable (pBlockTable, AcDb::kForRead)) != Acad::eOk )
		return (es) ;

	AcDbBlockTableRecord* pSpaceRecord =NULL ;
	if ( (es =pBlockTable->getAt (ACDB_MODEL_SPACE, pSpaceRecord, AcDb::kForRead)) != Acad::eOk ) {
		pBlockTable->close () ;
		return (es) ;
	}
	pBlockTable->close () ;

	AcDbBlockTableRecordIterator* pSpaceRecordIter =NULL ;
	if ( (es =pSpaceRecord->newIterator (pSpaceRecordIter)) != Acad::eOk ) {
		return (es) ;
	}
	pSpaceRecord->close () ;

	for ( ; !pSpaceRecordIter->done (); pSpaceRecordIter->step () ) {
		AcDbEntity* pEntity =NULL ;
		if (pSpaceRecordIter->getEntity(pEntity, AcDb::kForRead) != Acad::eOk) {
			return (es) ;
		}

		if ( pEntity->isA () == AcDbBlockReference::desc () ) {
			AcDbObjectId blockId =AcDbBlockReference::cast (pEntity)->blockTableRecord () ;
			AcDbBlockTableRecord* pBlockRec =NULL ;
			if ( (es =acdbOpenObject (pBlockRec, blockId, AcDb::kForRead)) != Acad::eOk ) {
				pEntity->close () ;
				return (es) ;
			}

			TCHAR* name =NULL ;
			pBlockRec->getName (name) ;
			pBlockRec->close () ;
			if ( _tcscmp (name, _T ("EMPLOYEE")) == 0 ) {
				es =(attach ? pEntity->addReactor (pEmployeeReactor) : pEntity->removeReactor (pEmployeeReactor)) ;
				if ( es != Acad::eOk ) {
					delete name ;
					pEntity->close () ;
					return (es) ;
				}
			}
			delete name ;
		}
		pEntity->close () ;
	}
	delete pSpaceRecordIter ;

	return (es) ;
}

void detachAllEmployeeReactors() {
	Acad::ErrorStatus es =Acad::eOk ;
	AcApDocument* pCurDoc =acDocManager->curDocument () ;
	AcApDocumentIterator* pDocIter =acDocManager->newAcApDocumentIterator () ;
	for ( ; !pDocIter->done (); pDocIter->step () ) {
		AcApDocument* pDoc =pDocIter->document () ;
		if ( (es =acDocManager->setCurDocument (pDoc)) == Acad::eOk ) {
			es =attachEmployeeReactorToAllEmployee (false) ;
		}
		if ( es != Acad::eOk ) {
			acutPrintf (_T ("\nERROR: cannot detach object reactor for <%s> (%d)"), pDoc->docTitle (), es) ;
		}
	}
	acDocManager->setCurDocument (pCurDoc) ;
}

bool isObjectAnEmployeeBlock (const AcDbObject* pObj) {
	if ( pObj->isA () != AcDbBlockReference::desc () ) {
		acutPrintf (_T ("\nERROR: not a block object")) ;
		return false ;
	}

	AcDbObjectId blockId =AcDbBlockReference::cast (pObj)->blockTableRecord () ;
	AcDbBlockTableRecord* pBlockRec =NULL ;
	if ( acdbOpenObject (pBlockRec, blockId, AcDb::kForRead) != Acad::eOk ) {
		acutPrintf (_T ("\nERROR: cannot open block object")) ;
		return false ;
	}

	bool isEmployee =false ;
	TCHAR* name =NULL ;
	pBlockRec->getName (name) ;
	pBlockRec->close () ;
	if ( _tcscmp (name, _T ("EMPLOYEE")) == 0 ) {
		isEmployee =true ;
	}
	delete name ;

	return (isEmployee) ;
}
