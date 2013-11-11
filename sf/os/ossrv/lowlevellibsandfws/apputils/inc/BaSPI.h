// Copyright (c) 2004-2009 Nokia Corporation and/or its subsidiary(-ies).
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
//

#ifndef __BASPI_H__
#define __BASPI_H__

#include <f32file.h>

//Forward declarations
class CResourceFile;
class CResourceArchiveImpl;

/** 
Class for accessing the resource archive SPI file
@internalAll 
*/
class RResourceArchive
	{
public:
	IMPORT_C RResourceArchive();

	IMPORT_C void Close();
	
	IMPORT_C void OpenL(RFs& aFs,const TDesC &aName);
	
	IMPORT_C void OpenL(RFs& aFs,const TDesC& aSpiPath,const TDesC& aSpiName);

	IMPORT_C CResourceFile* NextL(HBufC*& aRscFileName);
	
	IMPORT_C TBool End();
	
	IMPORT_C void Reset();
	
	IMPORT_C TUid Type();
private:
	CResourceArchiveImpl* iArchiveImpl;
	};

#endif//__BASPI_H__
