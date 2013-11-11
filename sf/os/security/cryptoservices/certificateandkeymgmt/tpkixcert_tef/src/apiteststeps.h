/*
* Copyright (c) 2008-2009 Nokia Corporation and/or its subsidiary(-ies).
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


/**
 @file 
 @internalTechnology
*/

#ifndef APITESTSTEPS_H
#define APITESTSTEPS_H

#include "pkixcertstepbase.h"
//#include <e32std.h>

_LIT(KAddSupportedOidsTestStep, "AddSupportedOidsTest");  
_LIT(KRemoveSupportedOidsTestStep, "RemoveSupportedOidsTest");
_LIT(KSetSupportedOidsTestStep, "SetSupportedOidsTest");
_LIT(KResetSupportedOidsTestStep, "ResetSupportedOidsTest");

class CPkixCertAddSupportedOidsStep : public CPkixCertStepBase
	{
public:
	CPkixCertAddSupportedOidsStep();

	virtual void PerformTestL();
	};	

class CPkixCertRemoveSupportedOidsStep : public CPkixCertStepBase
	{
public:
	CPkixCertRemoveSupportedOidsStep();

	virtual void PerformTestL();
	};

class CPkixCertSetSupportedOidsStep : public CPkixCertStepBase
	{
public:
	CPkixCertSetSupportedOidsStep();

	virtual void PerformTestL();
	};
	
class CPkixCertResetSupportedOidsStep : public CPkixCertStepBase
	{
public:
	CPkixCertResetSupportedOidsStep();

	virtual void PerformTestL();
	};	

#endif // APITESTSTEPS_H
