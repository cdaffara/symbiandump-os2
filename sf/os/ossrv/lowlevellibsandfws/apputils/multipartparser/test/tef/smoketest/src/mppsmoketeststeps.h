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

/**
 @file
 @internalTechnology
*/


#if (!defined MPPSMOKETESTSTEPSDOM_H)
#define MPPSMOKETESTSTEPSDOM_H

#include <e32std.h>
#include "mppsmoketestlib.h"

_LIT(KCParseBase64Encoding, "ParseBase64Encoding");
class CParseBase64Encoding : public CTestStep
	{
public:
	inline CParseBase64Encoding(){SetTestStepName(KCParseBase64Encoding);};
	virtual TVerdict doTestStepL(void);
	
	};


#endif
