// Copyright (c) 1999-2009 Nokia Corporation and/or its subsidiary(-ies).
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
// Declaration for FileBufferManager class which facilitates the aynchronous compression and 
// decompression of data streams.
// 
//

#ifndef __EZFILEBUFFERMANAGER_H__
#define __EZFILEBUFFERMANAGER_H__

#include <e32base.h>
#include <f32file.h>

#include <ezbufman.h>

/**
Concrete class to manage the input and output buffers for compression and de-compression

@publishedAll
@released
*/
class CEZFileBufferManager : public CBase, public MEZBufferManager
	{
public:

	enum { EBadInitialization = 1 };

public:
	~CEZFileBufferManager();
	IMPORT_C static CEZFileBufferManager* NewLC(RFile &aInput, RFile &aOutput, TInt aBufferSize = 0x8000);
	IMPORT_C static CEZFileBufferManager* NewL(RFile &aInput, RFile &aOutput, TInt aBufferSize = 0x8000);

	void InitializeL(CEZZStream &aZStream);
	void NeedInputL(CEZZStream &aZStream);
	void NeedOutputL(CEZZStream &aZStream);
	void FinalizeL(CEZZStream &aZStream);

protected:
	void ConstructL(TInt aBufferSize);
	CEZFileBufferManager(RFile &aInput, RFile &aOutput);

protected:
	RFile &iInputFile;
	RFile &iOutputFile;
	TInt iBufferSize;
	TUint8* iInputBuffer;
	TUint8* iOutputBuffer;
	TPtr8 iInputDescriptor;  // always points to start of the input Buffer
	TPtr8 iOutputDescriptor; // always points to start of the output Buffer
	};


#endif
