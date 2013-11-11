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
// Describes the reference agent test case verifying the validity of the reference agent implementation.
// 
//

/**
 @file 
 @internalComponent
 @test
*/

#ifndef TREFERENCEAGENT_H
#define TREFERENCEAGENT_H

#include "tipsec.h"

// The reference agent test sends and validates IPSec associations, 
// thus it shares much functionality with CScafIpSec class. Therefore,
// CScafRefAgent class derives from CScafIpSec.

class CScafRefAgent: public CScafIpSec
	{
public:
	CScafRefAgent(CScafServer& aParent);
	TVerdict doTestL();
	};

// Test case identifier definition
_LIT(KScafRefAgent,"SCAFRefAgent");
// Reference agent private folder
_LIT(KSraPrivateFolder, "!:\\private\\10285A1C\\");

#endif /* REFERENCEAGENT_H */
