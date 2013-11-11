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


#ifndef TUNDO_H_
#define TUNDO_H_

#include <e32cmn.h> 
#include <test/testexecutestepbase.h>

class CTUndoStep : public CTestStep
    {    
public:
    CTUndoStep(){};
    void print(const TDesC& aBuf, const TText8* text, int line);
    void testpoint(TBool aCondition, const TText8* text, int line );
protected:  
    TVerdict doTestStepL();
    };

inline void CTUndoStep::print(const TDesC& aBuf, const TText8* text, int line)
    {
    Logger().LogExtra((text), line, ESevrInfo, (aBuf));
    }

inline void CTUndoStep::testpoint(TBool aCondition, const TText8* text, int line )
    {
    testBooleanTrue((aCondition), text, line); 
    }

_LIT(KTestStep_T_Undo, "T_Undo");

#endif /* TUNDO_H_ */
