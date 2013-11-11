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
// Defines the Multiple Key Stream Decoder Test Case
// 
//

/**
 @file
 @internalComponent
 @test
*/

#ifndef TMULTIKEYSTREAMDECODERS_H_
#define TMULTIKEYSTREAMDECODERS_H_

#include "tscafstep.h"
#include <caf/streaming/keystreamdecoder.h>

class CScafMultiKeyStreamDecoders : public CScafStep
	{
public:
	CScafMultiKeyStreamDecoders(CScafServer& aParent);
	~CScafMultiKeyStreamDecoders();
	TVerdict doTestStepPreambleL();
	TVerdict doTestStepPostambleL();
	TVerdict doTestL();

private:
	};

// Test case identifier definition
_LIT(KScafMultiKeyStreamDecoders,"SCAFMultiKeyStreamDecoders");

#endif /*TMULTIKEYSTREAMDECODERS_H_*/
