/*
* Copyright (c) 2009 Nokia Corporation and/or its subsidiary(-ies).
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

#ifndef SYMBIANUNITTESTCONSOLEUIMAIN_H
#define SYMBIANUNITTESTCONSOLEUIMAIN_H

// INCLUDES
#include "symbianunittestuicallback.h"
#include <e32cons.h>
#include <e32base.h>

// FORWARD DECLARATIONS
class CSymbianUnitTestRunner;
class CSymbianUnitTestCommandLineParser;

// CLASS DECLARATION
class CSymbianUnitTestConsoleUi : 
    public CActive, 
    public MSymbianUnitTestUiCallBack
    {
    public: // Constructors and destructor
    
        static CSymbianUnitTestConsoleUi* NewLC();
        static CSymbianUnitTestConsoleUi* NewL();
        ~CSymbianUnitTestConsoleUi();
    
    protected: // From CActive
        
        void RunL();
        void DoCancel();
    
    public: // From MSymbianUnitTestUiCallBack
        
        void InfoMsg( const TDesC& aMessage );
        void InfoMsg( const TDesC& aFormat, const TDesC& aMessage );
        void TestPass(const TDesC& aTestCaseName);
        void TestFailed(const TDesC& aTestCaseName, const TDesC8& aErrMsg);
        
    private: // Constructors
    
        CSymbianUnitTestConsoleUi();
        void ConstructL();
        
    private: // New functions
    
        void PrintAllocFailureSimulationText();
        void IncrementExecutedTestsCount();
        
    private: // Data
    
        CConsoleBase* iConsole;
        CSymbianUnitTestCommandLineParser* iCommandLineParser;
        CSymbianUnitTestRunner* iTestRunner;
        TInt iExecutedTestCount;
        TInt iTestCount;
    };

// Local functions:
LOCAL_C void MainL();

#endif // SYMBIANUNITTESTCONSOLEUIMAIN_H
