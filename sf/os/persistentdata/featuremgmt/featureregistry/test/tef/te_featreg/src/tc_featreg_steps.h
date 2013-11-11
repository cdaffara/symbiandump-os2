// Copyright (c) 2005-2009 Nokia Corporation and/or its subsidiary(-ies).
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
// Feature Registry component test steps
// 
//

/**
 @file
 @internalComponent
 @test
*/

#ifndef TC_FEATREG_STEPS_H
#define TC_FEATREG_STEPS_H

#include <test/testexecutestepbase.h>
#include "te_featreg_stepbase.h"

_LIT(KQueryAnyFeatureStep,"QueryAnyFeature");
class CQueryAnyFeatureStep : public CTe_FeatReg_StepBase
	{
public:
	CQueryAnyFeatureStep();
	virtual TVerdict doTestStepL();
	};

_LIT(KRunFeatRegSetupCorruptStep,"RunFeatRegSetupCorrupt");
class CRunFeatRegSetupCorruptStep : public CTe_FeatReg_StepBase
	{
public:
	CRunFeatRegSetupCorruptStep();
	virtual TVerdict doTestStepL();
	};

_LIT(KQueryAnyFeatureCorruptStep,"QueryAnyFeatureCorrupt");
class CQueryAnyFeatureCorruptStep : public CTe_FeatReg_StepBase
	{
public:
	CQueryAnyFeatureCorruptStep();
	virtual TVerdict doTestStepL();
	};

_LIT(KQueryFeaturesNotSupportedStep,"QueryFeaturesNotSupported");
class CQueryFeaturesNotSupportedStep : public CTe_FeatReg_StepBase
	{
public:
	CQueryFeaturesNotSupportedStep();
	virtual TVerdict doTestStepL();
	};

_LIT(KQueryFeaturesValidSmallStep,"QueryFeaturesValidSmall");
class CQueryFeaturesValidSmallStep : public CTe_FeatReg_StepBase
	{
public:
	CQueryFeaturesValidSmallStep();
	virtual TVerdict doTestStepL();
	};

_LIT(KQueryFeaturesValidLargeStep,"QueryFeaturesValidLarge");
class CQueryFeaturesValidLargeStep : public CTe_FeatReg_StepBase
	{
public:
	CQueryFeaturesValidLargeStep();
	virtual TVerdict doTestStepL();
	};

_LIT(KFeatRegNotifyStep,"FeatRegNotify");
class CFeatRegNotifyStep : public CTe_FeatReg_StepBase
	{
public:
	CFeatRegNotifyStep();
	virtual TVerdict doTestStepL();
	};

_LIT(KInvalidUseQueryNotOpenStep,"InvalidUseQueryNotOpen");
class CInvalidUseQueryNotOpenStep : public CTe_FeatReg_StepBase
	{
public:
	CInvalidUseQueryNotOpenStep();
	virtual TVerdict doTestStepL();
	};

_LIT(KInvalidUseSubscribeNotOpenStep,"InvalidUseSubscribeNotOpen");
class CInvalidUseSubscribeNotOpenStep : public CTe_FeatReg_StepBase
	{
public:
	CInvalidUseSubscribeNotOpenStep();
	virtual TVerdict doTestStepL();
	};

_LIT(KInvalidUseCancelNotOpenStep,"InvalidUseCancelNotOpen");
class CInvalidUseCancelNotOpenStep : public CTe_FeatReg_StepBase
	{
public:
	CInvalidUseCancelNotOpenStep();
	virtual TVerdict doTestStepL();
	};

_LIT(KFeatRegMeasPerformanceStep,"FeatRegMeasPerformance");
class CFeatRegMeasPerformanceStep : public CTe_FeatReg_StepBase
	{
public:
	CFeatRegMeasPerformanceStep();
	virtual TVerdict doTestStepL();
	};
	_LIT(KFeatRegOpenCloseStep,"FeatRegOpenClose");
class CFeatRegOpenCloseStep: public CTe_FeatReg_StepBase
	{
public:
	CFeatRegOpenCloseStep();
	virtual TVerdict doTestStepL();
	};
#endif
