/*
* Copyright (c) 2005-2009 Nokia Corporation and/or its subsidiary(-ies).
* All rights reserved.
* This component and the accompanying materials are made available
* under the terms of "Eclipse Public License v1.0"
* which accompanies this distribution, and is available
* at the URL "http://www.eclipse.org/legal/epl-v10.html".
*
* Initial Contributors:
* Nokia Corporation - initial contribution.
*
* Contributors:
*
* Description:  
* This file define all the common values thoughout your test project
*
*/



/**
 @file Te_RegressionSuiteDefs.h
*/
#if (!defined __TE_REG_STEPTEST_SUITEDEFS_H__)
#define __TE_REG_STEPTEST_SUITEDEFS_H__

// Please modify below value with your project and must match with your configuration ini file which is required to be modified as well
_LIT(KServerName,"te_RegStepTestSuite");
_LIT(KOOMTestStep,"OOMTestStep");
_LIT(KMacrosTestStep,"MacrosTestStep");
_LIT(KShortFunsTestStep,"ShortFunsTestStep");
_LIT(KAbortTestStep,"AbortTestStep");
_LIT(KReadWriteConfigStep, "ReadWriteConfigStep");
_LIT(KReadWriteConfigNegStep, "ReadWriteConfigNegStep");
_LIT(KPanicTestStep, "PanicTestStep");

_LIT(KTe_RegStepTestSuiteString, "TheString");
_LIT(KTe_RegStepTestSuiteInt, "TheInt");
_LIT(KTe_RegStepTestSuiteBool, "TheBool");
_LIT(KTe_RegStepTestSuiteHex, "TheHex");

// For test step panics
_LIT(KTe_RegStepTestSuitePanic, "te_RegStepTestSuite");

#endif
