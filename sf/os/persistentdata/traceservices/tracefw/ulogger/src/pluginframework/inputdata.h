// Copyright (c) 2007-2009 Nokia Corporation and/or its subsidiary(-ies).
// All rights reserved.
// This component and the accompanying materials are made available
// under the terms of "Eclipse Public License v1.0"
// which accompanies this distribution, and is available
// at the URL "http://www.eclipse.org/legal/epl-v10.html".
//
// Initial Contributors:
// Nokia Corporation - initial contribution.
//
// Contributors:
//
// Description:
// ULogger
// 
//

/**
 @file
 @internalTechnology
 @prototype
*/

#ifndef INPUTDATA_H_
#define INPUTDATA_H_

#ifdef __SYMBIAN32__
#include <e32base.h>
#include <e32cmn.h>
#else
#include <iostream.h>
#include <memory.h>
#endif


//define ControlData type
typedef char ControlData; 

//const values
const char DATA_SEPARATOR = ' '; //space 
const char CD_CR = 0x0D; //end os line in MacOS style
const char CD_LF = 0x0A; //end of line in Unix/Linux style
const int SIZE_OF_EOL = 2;
const char CD_EOL[SIZE_OF_EOL] = {CD_CR,CD_LF}; //end of line in Windows OS style



/**This class should be used to create, update and parse Control Data packages.
Write type functions create new control data chunk and update existing one, whereas 
read type functions can parse and return certain data from existing control data packages.
*/
class CInputData
{
public:
	IMPORT_C CInputData(unsigned int aMinPackageSize=2);
	IMPORT_C virtual ~CInputData();
	
	//read type functions
	IMPORT_C unsigned long GetSize(const ControlData *aDataPtr);
	IMPORT_C unsigned long GetChunksCount(const ControlData *aDataPtr);
	IMPORT_C const void* GetChunk(const ControlData* aDataPtr, unsigned long aChunkNumber, unsigned long &aChunkSize);
	
	//write type functions
	IMPORT_C ControlData* CreatePackage(void* aCommand, signed long aCommandSize);
	IMPORT_C unsigned long AppendNewData(ControlData *&aDataPtr, const void* aAddData, unsigned long aAddDataSize);
	
private:
	unsigned int iMinPackageSize;
	unsigned long FindAvailablePlace(const ControlData *aDataPtr);
	unsigned long CalcNumberOfPaddingChars(const ControlData *aDataPtr);
	
};

#endif /*CONTROLDATAMANAGER_H_*/
