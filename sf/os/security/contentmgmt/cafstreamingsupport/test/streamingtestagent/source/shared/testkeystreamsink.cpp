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
// Implements the test key stream sink object for the streaming test agent.
// 
//

#include "testkeystreamsink.h"

using namespace StreamAccess;


CTestKeyStreamSink::CTestKeyStreamSink()
/**
 	Constructor for CTestKeyStreamSink object.
 */
	{
	//empty
	}

EXPORT_C CTestKeyStreamSink::~CTestKeyStreamSink()
/**
 	Destructor for CTestKeyStreamSink object.
 */
	{
	delete iOutput;
	iFs.Close();
	}


EXPORT_C CTestKeyStreamSink* CTestKeyStreamSink::NewL(RReadStream& aStream)
/**
 	Creates a new test key stream sink object from a stream.
	@param aOutput Stream to read.
	@return A pointer to the new test key stream object.
 */
	{
	CTestKeyStreamSink* self = new (ELeave) CTestKeyStreamSink();
	CleanupStack::PushL(self);
	User::LeaveIfError(self->iFs.Connect());
	self->InternalizeL(aStream);
	CleanupStack::Pop(self);
	return self;
	}


EXPORT_C CTestKeyStreamSink* CTestKeyStreamSink::NewL(const TDesC& aOutput)
/**
 	Creates a new test key stream sink object.
	@param aOutput The fully qualified output path.
	@return A pointer to the new test key stream object.
 */
	{
	CTestKeyStreamSink* self = new (ELeave) CTestKeyStreamSink();
	CleanupStack::PushL(self);
	User::LeaveIfError(self->iFs.Connect());
	self->ConstructL(aOutput);
	CleanupStack::Pop(self);
	return self;
	}
	
void CTestKeyStreamSink::ConstructL(const TDesC& aOutput)
/**
 	Second phase constructor for CTestKeyStreamSink object.
 	@param aOutput The fully qualified output path.
 */
	{
	iOutput = aOutput.AllocL();
	}

EXPORT_C CKeyStreamSink *CTestKeyStreamSink::CloneLC() const
/**
 	Clone the key stream sink.
 */
	{
	CTestKeyStreamSink *newSink = CTestKeyStreamSink::NewL(*iOutput);
	CleanupStack::PushL(newSink);
	return newSink;
	}

EXPORT_C void CTestKeyStreamSink::ProcessNewKeyAssociationL(const CKeyAssociation& aKeyAssociation)
/**
 	Implementation from CKeyStreamSink.
 	@see CKeyStreamSink::ProcessNewKeyAssociationL
 */
	{
	RFile file;
	User::LeaveIfError(file.Create(iFs, *iOutput, EFileWrite));
	CleanupClosePushL(file);
	const CTestKeyAssociation* key = dynamic_cast<const CTestKeyAssociation*>(&aKeyAssociation);
	ASSERT(key);
	User::LeaveIfError(file.Write(*(key->GetTestKey())));
	CleanupStack::PopAndDestroy(&file);
	}
		
EXPORT_C void CTestKeyStreamSink::SetEncryptionAlgorithmL(const TEncryptionAlgorithm& /*aEncryptionAlgorithm*/)
/**
 	Implementation from CKeyStreamSink.
 	@see CKeyStreamSink::SetEncryptionAlgorithmL
 */
	{
	//empty
	}

EXPORT_C void CTestKeyStreamSink::SetAuthenticationAlgorithmL(const TAuthenticationAlgorithm& /*aAuthenticationAlgorithm*/)
/**
 	Implementation from CKeyStreamSink.
 	@see CKeyStreamSink::SetEncryptionAlgorithmL
 */
	{
	//empty
	}

EXPORT_C void CTestKeyStreamSink::DoExternalizeL(RWriteStream& aStream) const
/**
 	Externalizes CTestKeyStreamSink object.
 */
	{
	aStream<<*iOutput;
	}


void CTestKeyStreamSink::InternalizeL(RReadStream& aStream)
/**
 	Internalizes CTestKeyStreamSink object.
 	@param aStream Stream to read from.
 */
	{
	delete iOutput;
	iOutput = NULL;
	iOutput = HBufC::NewL(aStream,KMaxFileName);
	}
