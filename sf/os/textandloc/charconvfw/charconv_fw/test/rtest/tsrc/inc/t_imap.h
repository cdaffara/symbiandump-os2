/*
* Copyright (c) 1997-2009 Nokia Corporation and/or its subsidiary(-ies).
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


#ifndef __T_IMAP_H__
#define __T_IMAP_H__

//#include <w32std.h>
#include <test/testexecutestepbase.h>

class CT_IMAP : public CTestStep
    {
public:
    CT_IMAP();
protected:  
    TVerdict doTestStepL();
private:
    void TestIMAP();
    };

_LIT(KTestStep_T_IMAP, "T_IMAP");

#endif
