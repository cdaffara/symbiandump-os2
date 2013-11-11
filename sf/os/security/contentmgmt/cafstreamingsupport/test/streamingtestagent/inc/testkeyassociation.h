// Copyright (c) 2007-2009 Nokia Corporation and/or its subsidiary(-ies).
// All rights reserved.
// This component and the accompanying materials are made available
// under the terms of the License "Eclipse Public License v1.0"
// which accompanies this distribution, and is available
// at the URL "http://www.eclipse.org/legal/epl-v10.html".
//
// Initial Contributors:
// Nokia Corporation - initial contribution.
//
// Contributors:
//
// Description:
// Defines the test key association object for the streaming test agent.
// 
//

/**
 @file
 @internalComponent
 @test
*/


#ifndef TESTKEYASSOCIATION_H_
#define TESTKEYASSOCIATION_H_

#include <e32base.h>
#include <s32strm.h>
#include <caf/streaming/keyassociation.h>

namespace StreamAccess
	{
	NONSHARABLE_CLASS(CTestKeyAssociation) : public CKeyAssociation
	/**
	 	A specific key association object for the streaming test agent.
	 	The test key association is different from the key associations 
	 	of IPSec, SRTP and ISMACryp.
	 */
		{
	public:
		IMPORT_C static CTestKeyAssociation* NewL(RReadStream& aStream);
		IMPORT_C static CTestKeyAssociation* NewL(const TDesC8& aKey);
		IMPORT_C const HBufC8* GetTestKey() const;
		IMPORT_C void ExternalizeL(RWriteStream& aStream) const;
		IMPORT_C ~CTestKeyAssociation();
	
	private:
		CTestKeyAssociation();
		void ConstructL(const TDesC8& aKey);
		void InternalizeL(RReadStream& aStream);
		
	private:
		HBufC8* iKey;
		};
	}//End of the namespace StreamAccess

#endif /*TESTKEYASSOCIATION_H_*/
