/*
* Copyright (c) 2004-2009 Nokia Corporation and/or its subsidiary(-ies).
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


#ifndef _TPADDINGTESTSTEPS_H_
#define _TPADDINGTESTSTEPS_H_

#include <test/testexecutestepbase.h>

// Base class for all padding tests.
class CPaddingStep : public CTestStep
{
   public:
      CPaddingStep();
      ~CPaddingStep();
      virtual TVerdict doTestStepPreambleL();
      virtual TVerdict doTestStepPostambleL();
};

#endif
