/*
* Copyright (c) 2010 Nokia Corporation and/or its subsidiary(-ies).
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
* @file
* @internalComponent 
*
*/

#ifndef TTMCODE_H_
#define TTMCODE_H_

#include <e32cmn.h> 
#include <test/testexecutestepbase.h>

class CTTmCodeStep : public CTestStep
    {    
public:
    CTTmCodeStep(){};

protected:  
    TVerdict doTestStepL();
    };

_LIT(KTestStep_T_TmCode, "T_TmCode");


#endif /* TTMCODE_H_ */
