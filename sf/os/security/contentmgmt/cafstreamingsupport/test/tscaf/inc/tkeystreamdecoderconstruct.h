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
// Defines the Key Stream Decoder Construction Test Case
// 
//

/**
 @file
 @internalComponent
 @test
*/

#ifndef TKEYSTREAMDECODERCONSTRUCT_H_
#define TKEYSTREAMDECODERCONSTRUCT_H_

#include "tscafstep.h"

class CScafKeyStreamDecoderConstruct : public CScafStep
	{
public:
	CScafKeyStreamDecoderConstruct(CScafServer& aParent);
	~CScafKeyStreamDecoderConstruct();
	TVerdict doTestL();

private:
	};

// Test case identifier definition
_LIT(KScafKeyStreamDecoderConstruct,"SCAFKeyStreamDecoderConstruct");

#endif /*TKEYSTREAMDECODERCONSTRUCT_H_*/
