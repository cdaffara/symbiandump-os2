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
// Implements the test protected stream description object for the streaming test agent.
// 
//

#include "testprotstrdesc.h"
#include "testkeystreamsink.h"

using namespace StreamAccess;

CTestProtectedStreamDesc::CTestProtectedStreamDesc()
/**
 	Constructor for CTestProtectedStreamDesc object. 
 */
	{
	//empty
	}

EXPORT_C CTestProtectedStreamDesc::~CTestProtectedStreamDesc()
/**
 	Destructor for CTestProtectedStreamDesc object.
 */
	{
	delete iOutput;
	}

EXPORT_C CTestProtectedStreamDesc* CTestProtectedStreamDesc::NewL(const TDesC& aOutput)
/**
 	Creates a new test protected stream description object.
	@param aOutput The fully qualified output path.
	@return A pointer to the new test protected stream description object.
 */
	{
	CTestProtectedStreamDesc* self = new (ELeave) CTestProtectedStreamDesc();
	CleanupStack::PushL(self);
	self->ConstructL(aOutput);
	CleanupStack::Pop(self);
	return self;
	}
	
void CTestProtectedStreamDesc::ConstructL(const TDesC& aOutput)
/**
 	Second phase constructor for CTestProtectedStreamDesc object.
 	@param aOutput The fully qualified output path.
 */
	{
	iOutput = aOutput.AllocL();
	}

EXPORT_C CKeyStreamSink* CTestProtectedStreamDesc::CreateKeyStreamSinkLC() const
/**
 	Implementation from CProtectedStreamDesc
 	@see CProtectedStreamDesc::CreateKeyStreamSinkLC
 */
	{
	CTestKeyStreamSink* sink = CTestKeyStreamSink::NewL(iOutput->Des());
	CleanupStack::PushL(sink);
	return sink;
	}
