/*
* Copyright (c) 2002-2009 Nokia Corporation and/or its subsidiary(-ies).
* All rights reserved.
* This component and the accompanying materials are made available
* under the terms of the License "Eclipse Public License v1.0"
* which accompanies this distribution, and is available
* at the URL "http://www.eclipse.org/legal/epl-v10.html".
*
* Initial Contributors:
* Nokia Corporation - initial contribution.
*
* Contributors:
*
* Description: 
*
*/




#ifndef __BITSETSTEP_H__
#define __BITSETSTEP_H__

#include <test/testexecutestepbase.h>
#include "cafstep.h"

namespace ContentAccess
	{
	class CBitset;
	}

_LIT(KBasicBitsetStep,"BasicBitsetStep");
_LIT(KBitsetListStep,"BitsetListStep");
_LIT(KBitsetEqualityStep,"BitsetEqualityStep");
_LIT(KBitsetCopyStep,"BitsetCopyStep");
_LIT(KBitsetSerialiseStep,"BitsetSerialiseStep");
_LIT(KBitsetPanicStep,"BitsetPanicStep");

// not a step, but baseclass for all bitset tests
class CBitsetBaseStep : public CCAFStep
	{
public:
	CBitsetBaseStep(const TDesC& aStepName);
	virtual TVerdict doTestStepPreambleL();
	virtual TVerdict doTestStepPostambleL();

protected:
	// Create bitset based on data from the script.
	// aHeader specifies how the key is formed. eg if aHeader is "left-",
	// then the key "left-bitcount" specifies how many bits there are,
	// and "left-bit0" specifies bit0, etc
	ContentAccess::CBitset* GetBitsetLC(const TDesC& aHeader);
	};

class CBasicBitsetStep : public CBitsetBaseStep
	{
public:
	CBasicBitsetStep();
	virtual TVerdict doTestStepL();
	};

class CBitsetListStep : public CBitsetBaseStep
	{
public:
	CBitsetListStep();
	virtual TVerdict doTestStepL();
	};

class CBitsetEqualityStep : public CBitsetBaseStep
	{
public:
	CBitsetEqualityStep();
	virtual TVerdict doTestStepL();
	};

class CBitsetCopyStep : public CBitsetBaseStep
	{
public:
	CBitsetCopyStep();
	virtual TVerdict doTestStepL();
	};

// Test the internalize and externalize methods
class CBitsetSerialiseStep : public CBitsetBaseStep
	{
public:
	CBitsetSerialiseStep();
	virtual TVerdict doTestStepL();
	};

// Test the panic when invalid bits are set, cleared or checked
class CBitsetPanicStep : public CBitsetBaseStep
	{
public:
	CBitsetPanicStep();
	virtual TVerdict doTestStepL();
	};

#endif
