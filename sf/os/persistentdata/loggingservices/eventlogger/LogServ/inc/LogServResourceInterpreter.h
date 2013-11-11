// Copyright (c) 2002-2010 Nokia Corporation and/or its subsidiary(-ies).
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

#ifndef __LOGSERVRESOURCEINTERPRETER_H__
#define __LOGSERVRESOURCEINTERPRETER_H__

#include <barsc.h>
#include <barsread.h>
#include <logcli.h>

/**
Gives an access to the LogWrap resource file and maintains a cache of the retrieved resource entries.

@internalComponent
*/
class CLogServResourceInterpreter : public CBase
	{
public:
	static CLogServResourceInterpreter* NewL(RFs& aFs);
	~CLogServResourceInterpreter();
    void CreateResourceReaderLC(TResourceReader& aReader, TInt aId);

private:
    /**
    Resource file item.
    Holds one resource string identified by an id.
    @internalComponent
    */
	struct TResourceString
	    {
		inline TResourceString(TInt aResourceId, HBufC8* aResourceString) :
			iId(aResourceId),
			iString(aResourceString)
			{
			}
		TInt		iId;
		HBufC8*		iString;
	    };

    CLogServResourceInterpreter(RFs& aFs);
    void ConstructL();
	HBufC8* GetStringL(TInt aId);
	static TInt Compare(const TResourceString& aLeft, const TResourceString& aRight);
		
private:
	RFs& 					iFs;
	RResourceFile 			iFile;
	RArray<TResourceString>	iStrings;
	};

#endif
