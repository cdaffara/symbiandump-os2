// Copyright (c) 2007-2009 Nokia Corporation and/or its subsidiary(-ies).
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
// EFM test steps for characterisation of feature manager performance.
// 
//

/**
 @file
 @internalComponent
 @test
*/

#ifndef __CHARACTESTSTEPS__
#define __CHARACTESTSTEPS__

#include <test/testexecutestepbase.h>
#include <featmgr/featurecontrol.h>

class CCharacteriseBaseStep : public CTestStep
	{
public:
	CCharacteriseBaseStep() {};
	~CCharacteriseBaseStep() {};
	virtual TVerdict doTestStepPreambleL(void);
	virtual TVerdict doTestStepPostambleL(void);
protected:
	RFeatureControl icontrol;
	TInt itimeout, icountfreq;
	TReal icountperusec;
	};

_LIT(KCharacteriseSupported, "CharacteriseSupported");
class CCharacteriseSupportedStep : public CCharacteriseBaseStep 
	{
public:
	CCharacteriseSupportedStep();
	virtual TVerdict doTestStepL(void);
	};

_LIT(KCharacteriseSupportedArray, "CharacteriseSupportedArray");
class CCharacteriseSupportedArrayStep : public CCharacteriseBaseStep 
	{
public:
	CCharacteriseSupportedArrayStep();
	virtual TVerdict doTestStepL(void);
	};

#endif
