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
*
*/
#ifndef __T_GENERALFOREIGN_H__
#define __T_GENERALFOREIGN_H__

//#include <w32std.h>
#include <test/testexecutestepbase.h>

class CCnvCharacterSetConverter;

class CT_GENERALFOREIGN : public CTestStep
    {
public:
    CT_GENERALFOREIGN();
protected:  
    TVerdict doTestStepL();
private:
    void DoE32MainL();
    
    };

_LIT(KTestStep_T_GENERALFOREIGN, "T_GENERALFOREIGN");

#endif //__T_GENERALFOREIGN_H__
