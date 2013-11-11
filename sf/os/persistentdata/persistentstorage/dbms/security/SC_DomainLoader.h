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
// CPolicyDomainLoader class
// 
//

#ifndef __SC_DOMAINLOADER_H__
#define __SC_DOMAINLOADER_H__

#include <e32base.h>
#include "D32Map.h"
#include "SC_Policy.h"

namespace DBSC
{

/**
CPolicyDomainLoader is a special policy file loader.
The "special" thing is that it will trap all errors during processing of
the binary policy files. If there is an error, the loader will check it
anld leave if it is KErrNoMemory. The rest of errors, which are caused by
things such as file system problems, hardware problems and binary policy file problems 
(bad format, bad names,...), will be supressed.
@internalComponent
*/
NONSHARABLE_CLASS(CPolicyDomainLoader) : public CBase
	{
public:
	static CPolicyDomainLoader* NewLC(RFs& aFs, const TDesC& aPolicyDir, 
									  RMap<TInt,CPolicyDomain*>& aPDCollection);
	virtual ~CPolicyDomainLoader();
	void RunL();

private:
	inline CPolicyDomainLoader(RFs& aFs, const TDesC& aPolicyDir, 
							   RMap<TInt,CPolicyDomain*>& aPDCollection);
	void ConstructL();
	void CreatePolicyDomainL(TInt aPolicyFileIndex);
	void DoCreatePolicyDomainL(TInt aPolicyFileIndex);
	TUid CreatePolicyFilePathL(TInt aPolicyFileIndex);

private:
	RFs&						iFs;
	const TDesC&				iPolicyDir;
	RMap<TInt, CPolicyDomain*>&	iPDCollection;
	CDir*						iFoundPolicyFiles;
	TParse						iParser;
	TBuf<KMaxPath>				iPolicyFilePath;

	};

} //end of - namespace DBSC

#endif//__SC_DOMAINLOADER_H__
