// Copyright (c) 2008-2009 Nokia Corporation and/or its subsidiary(-ies).
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

#ifndef CREGEN_H_
#define CREGEN_H_

#include <e32base.h>
#include <f32file.h>

class CHeapRepository;

/**
@internalTechnology
*/
class CCreGenerator : public CBase
	{
public:
	void static CommitChangesToCreL(RFs& aFs,TUint8 aPersistVersion,CHeapRepository& aRep,const TDesC& aTargetFilePath);
	void static CreateReposFromCreL(RFs& aFs,CHeapRepository& aRep, const TDesC& aFilePath
#ifdef SYMBIAN_CENTREP_SUPPORT_MULTIROFS	
	,TUint8& aCreVersion
#endif	
	);
	
	void static ExternalizeCre(TUint8 aPersistVersion,const CHeapRepository& aRep, RWriteStream& aStream);
	void static InternalizeCreL(CHeapRepository& aRep, RReadStream& aStream
#ifdef SYMBIAN_CENTREP_SUPPORT_MULTIROFS	
	,TUint8& aCreVersion
#endif	
	);	
	};

#endif /*CREGEN_H_*/
