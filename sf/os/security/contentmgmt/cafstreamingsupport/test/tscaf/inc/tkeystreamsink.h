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
// Defines the Key Stream Sink usage tests.
// 
//

/**
 @file 
 @internalComponent
 @test
*/

#ifndef TKEYSTREAMSINK_H_
#define TKEYSTREAMSINK_H_

#include "tscafstep.h"
#include "srarightsobject.h"
#include "testagentkeystreamdecoder.h"

class CScafKeyStreamSink : public CScafStep
	{
public:
	CScafKeyStreamSink(CScafServer& aParent);
	~CScafKeyStreamSink();
	TVerdict doTestL();
	};
	
// Constant used to name this test case
_LIT(KScafKeyStreamSinkStep,"SCAFKeyStreamSink");


class CScafConcurrentKeyStreamSinks : public CScafStep
/**
 	Defines a test showing usage of key stream sinks in concurrent sessions.
 */
	{
public:
	CScafConcurrentKeyStreamSinks(CScafServer& aParent);
	~CScafConcurrentKeyStreamSinks();
	TVerdict doTestL();
	};
	
// Constant used to name this test case	
_LIT(KScafConcurrentKeyStreamSinksStep,"SCAFConcurrentKeyStreamSinks");
	
#endif /*TKEYSTREAMSINK_H_*/
