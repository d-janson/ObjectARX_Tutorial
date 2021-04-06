#include "StdAfx.h"

#include "utilities.h"

//
// Create a new layer or return the ObjectId if it already exists
//
// In :
// const TCHAR* layerName : layer name
// Out :
// AcDbObjectId& layerId : ObjectId of the created or existing layer
//
Acad::ErrorStatus createLayer(const TCHAR* layerName, AcDbObjectId& layerId)
{
	Acad::ErrorStatus st;
	AcDbDatabase* db = acdbHostApplicationServices()->workingDatabase();

	AcDbSymbolTable* layerTbl = NULL;
	st = db->getLayerTable(layerTbl, AcDb::kForRead);
	if (st != Acad::eOk) {
		acutPrintf(_T("\nCommand internal error: getLayerTable() = %d"), st);
		return st;
	}

	st = layerTbl->getAt(layerName, layerId);
	if (st == Acad::eOk) {
		layerTbl->close();
		acutPrintf(_T("\nFound layer <%s>: %d-%d"), layerName, layerId.handle().high(), layerId.handle().low());
		return Acad::eOk;
	}

	acutPrintf(_T("\nupgrading opened table for writing"));
	st = layerTbl->upgradeOpen();
	if ((st != Acad::eOk) && (st != Acad::eWasOpenForWrite)) {
		acutPrintf(_T("\nCommand internal error: upgradeOpen() = %d"), st);
		layerTbl->close();
		return st;
	}
	AcDbLayerTableRecord* layerRec = new AcDbLayerTableRecord();
	layerRec->setName(layerName);
	st = static_cast<AcDbLayerTable*>(layerTbl)->add(layerId, layerRec);
	if (st != Acad::eOk) {
		acutPrintf(_T("\nCommand internal error: add() = %d"), st);
		layerTbl->close();
		return st;
	}
	acutPrintf(_T("\nCreated layer <%s>: %d-%d"), layerName, layerId.handle().high(), layerId.handle().low());

	layerRec->close();
	layerTbl->close();

	return Acad::eOk;
}

// 
// Create a new block table record and add the entities of the employee to it 
// 
// In : 
// const TCHAR* name : Name of block table record 
// 
Acad::ErrorStatus createBlockRecord(const TCHAR* name)
{
	Acad::ErrorStatus st = Acad::eOk;
	AcDbDatabase* db = acdbHostApplicationServices()->workingDatabase();

	AcDbBlockTable* blockTbl = NULL;
	st = db->getBlockTable(blockTbl, kForRead);
	if (st != Acad::eOk) {
		return st;
	}
	else if (blockTbl->has(name)) {
		blockTbl->close();
		acutPrintf(_T("\nAlready has block: %s"), name);
		return Acad::eDuplicateKey;
	}

	if ((st = blockTbl->upgradeOpen()) == eOk) {
		AcDbBlockTableRecord* blockRec = new AcDbBlockTableRecord();
		blockRec->setName(name);
		blockRec->setOrigin(AcGePoint3d::kOrigin);
		if ((st = blockTbl->add(blockRec)) == Acad::eOk) {
			AcDbCircle* face = new AcDbCircle(AcGePoint3d::kOrigin, AcGeVector3d::kZAxis, 1.0);
			face->setColorIndex(2);
			AcDbCircle* leftEye = new AcDbCircle(AcGePoint3d(0.33, 0.25, 0.0), AcGeVector3d::kZAxis, 0.1);
			leftEye->setColorIndex(5);
			AcDbCircle* rightEye = new AcDbCircle(AcGePoint3d(-0.33, 0.25, 0.0), AcGeVector3d::kZAxis, 0.1);
			rightEye->setColorIndex(5);
			constexpr double pi = 3.141592654;
			AcDbArc* mouth = new AcDbArc(AcGePoint3d(0.0, 0.5, 0.0), 1.0, (pi + (pi * 0.3)), (pi + (pi * 0.7)));
			mouth->setColorIndex(1);

			blockRec->appendAcDbEntity(face);
			blockRec->appendAcDbEntity(leftEye);
			blockRec->appendAcDbEntity(rightEye);
			blockRec->appendAcDbEntity(mouth);

			face->close();
			leftEye->close();
			rightEye->close();
			mouth->close();
			blockRec->close();
		}
	}

	blockTbl->close();
	return st;
}