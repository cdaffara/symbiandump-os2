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
// Defines the test key stream sink object for the streaming test agent.
// 
//

/**
 @file
 @internalComponent
 @test
*/


#ifndef TESTKEYSTREAMSINK_H_
#define TESTKEYSTREAMSINK_H_

#include <caf/streaming/keystreamsink.h>
#include <s32strm.h>
#include <e32const.h>
#include <f32file.h>
#include "testkeyassociation.h"

namespace StreamAccess
	{
	
	NONSHARABLE_CLASS(CTestKeyStreamSink) : public CKeyStreamSink
	/**
		A specific key stream sink for the streaming test agent.
		Differs from the correspondent implementations of IPSec, SRTP and ISMACryp.
	 */
		{
	public:
		IMPORT_C static CTestKeyStreamSink* NewL(RReadStream& aStream);
		IMPORT_C static CTestKeyStreamSink* NewL(const TDesC& aOutput);
		IMPORT_C ~CTestKeyStreamSink();
		
		IMPORT_C CKeyStreamSink *CloneLC() const;
		
		// CKeyStreamSink interface
		IMPORT_C void ProcessNewKeyAssociationL(const CKeyAssociation& aKeyAssociation);
		IMPORT_C void SetEncryptionAlgorithmL(const TEncryptionAlgorithm& aEncryptionAlgorithm);
		IMPORT_C void SetAuthenticationAlgorithmL(const TAuthenticationAlgorithm& aAuthenticationAlgorithm);
	protected:
		IMPORT_C void DoExternalizeL(RWriteStream& aStream) const;
	private:
		CTestKeyStreamSink();
		void ConstructL(const TDesC& aOutput);
		void InternalizeL(RReadStream& aStream);
		
	private:
		HBufC* iOutput; ///< The fully qualified path of the output file in which the key association is stored.
		RFs iFs;
		};
	
	}//End of the namespace StreamAccess

#endif /*TESTKEYSTREAMSINK_H_*/
