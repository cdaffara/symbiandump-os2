/*
* Copyright (c) 2002-2009 Nokia Corporation and/or its subsidiary(-ies).
* All rights reserved.
* This component and the accompanying materials are made available
* under the terms of "Eclipse Public License v1.0"
* which accompanies this distribution, and is available
* at the URL "http://www.eclipse.org/legal/epl-v10.html".
*
* Initial Contributors:
* Nokia Corporation - initial contribution.
*
* Contributors:
*
* Description:
*
*/



#ifndef COMMON_CONTENT_POLICY_H
#define COMMON_CONTENT_POLICY_H

#include <e32base.h>

class RFile;
class RFs;

NONSHARABLE_CLASS(CCommonContentPolicy) : public CBase
/**
@deprecated
@see CApfMimeContentPolicy
@note For compatibility with S60 only
*/
	{
public: 
	IMPORT_C static CCommonContentPolicy* NewL();
	IMPORT_C static CCommonContentPolicy* NewLC();
	IMPORT_C static CCommonContentPolicy* NewL(RFs& aFs);
	IMPORT_C static CCommonContentPolicy* NewLC(RFs& aFs);
	IMPORT_C ~CCommonContentPolicy();

	IMPORT_C TBool IsClosedType(const TDesC& aMimeType);
	IMPORT_C TBool IsClosedExtension(const TDesC& aFileExtension);
	IMPORT_C TBool IsClosedFileL(const TDesC& aFileName);
	IMPORT_C TBool IsDRMEnvelopeL(const TDesC& aFileName);
	IMPORT_C TBool IsClosedFileL(RFile& aFileHandle);
	IMPORT_C TBool IsDRMEnvelopeL(RFile& aFileHandle);
private:
	CCommonContentPolicy();
	};
	
#endif  // COMMON_CONTENT_POLICY_H
