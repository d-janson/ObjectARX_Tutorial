#include "StdAfx.h"

//Declaration of createLayer():
Acad::ErrorStatus createLayer (const TCHAR* layerName, AcDbObjectId& layerId);
//Declaration of createBlockRecord():
Acad::ErrorStatus createBlockRecord (const TCHAR *name);
//getBlock() opens a selected block for reading, you'll close it
Acad::ErrorStatus getSelBlock(AcDbObject*& block, AcDbObjectId& blockId);
