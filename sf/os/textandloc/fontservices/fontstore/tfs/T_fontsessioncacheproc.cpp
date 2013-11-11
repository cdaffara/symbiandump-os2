/*
* Copyright (c) 2006-2010 Nokia Corporation and/or its subsidiary(-ies).
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

/**
 @file
 @test
 @internalComponent - Internal Symbian test code
*/

#include "FNTSTORE.H"
#include <e32math.h>

_LIT(KOpenFont, "DejaVu Sans Condensed");

#ifdef __WINSCW__
//this is used for winscw breakpoints
#define BR _asm( int 3);
#endif

const TInt KTimeOut = 1000 * 1000;


_LIT(KTCacheDeletionProcess,"T_fontsessioncacheproc");

/* it is expected that the main in this file will be called to test multiple 
process cache searching which is in the shared heap.
*/

class CRunProc: public CBase
    {
public:
    void RunTestL();
    CRunProc();
    ~CRunProc();
    
    inline void setFont(CFont*);
    inline void setHandle(TInt);
private:
    void CreateFontL();

private:
    CBitmapFont* iFont;
    TInt iSessionHandle;
    };

CRunProc::CRunProc()
    {
    
    }

CRunProc::~CRunProc()
    {
    
    }

inline void CRunProc::setFont(CFont* aFont)
    {
    iFont = static_cast<CBitmapFont*>(aFont);
    }

inline void CRunProc::setHandle(TInt aSessionHandle)
    {
    iSessionHandle = aSessionHandle;
    }


void CRunProc::RunTestL()
    {
    TTime theTime;
    theTime.UniversalTime();
    TInt64 randSeed(theTime.Int64());
    TInt random(Math::Rand(randSeed) % (1000 * 1000));
    User::After(random);

    RTimer timer;
    timer.CreateLocal();
    TRequestStatus timerStatus = KRequestPending;
    TTimeIntervalMicroSeconds32 timeout(KTimeOut);
    timer.After(timerStatus, timeout);

    TText ch;
    const TUint8 *bitmap = NULL;
    TSize bitmapsize;
    TOpenFontCharMetrics Metrics;    
    do
        {
        TInt hitcount = 0;
        for (ch = 'A'; ch <= 'z'; ch++)
            {
            if(iFont->GetCharacterData(iSessionHandle, (TInt)ch, Metrics,bitmap))
                {
                //RDebug::Print(_L("%c hit bitmap[0]=%x"),ch,bitmap[0]);
                TUint8 testbyte = bitmap[0];
                testbyte += testbyte;
                __ASSERT_ALWAYS((testbyte & 0x01) == 0, User::Panic(KTCacheDeletionProcess, KErrGeneral));
                hitcount++;
                }
            else 
                {
                //RDebug::Print(_L("%c missed"),ch);
                }
            }
        __ASSERT_ALWAYS(hitcount > 0, User::Panic(KTCacheDeletionProcess, KErrNotFound));
        }
    while (timerStatus == KRequestPending);

    timer.Cancel();
    timer.Close();
    }

void MainL()
    {
    RChunk gChunk;
    User::LeaveIfError(gChunk.Open(1));
    CleanupClosePushL(gChunk);
    
    TInt offset;
    User::LeaveIfError(User::GetTIntParameter(2,offset));
    TInt SessionHandle;
    User::LeaveIfError(User::GetTIntParameter(3,SessionHandle));
    
    CRunProc *test = new (ELeave) CRunProc;
    
    test->setFont(reinterpret_cast<CFont*>(offset + reinterpret_cast<TInt>(gChunk.Base())));
    test->setHandle(SessionHandle);
    CleanupStack::PushL(test);

    RDebug::Print(_L("T_fontsessioncacheproc MainL()"));
    test->RunTestL();
    
    CleanupStack::PopAndDestroy(2);
    }

// Cleanup stack harness
GLDEF_C TInt E32Main()
    {
    __UHEAP_MARK;
    CTrapCleanup* cleanupStack = CTrapCleanup::New();
    TRAPD(error, MainL());    
    __ASSERT_ALWAYS(!error, User::Panic(KTCacheDeletionProcess, error));
    delete cleanupStack;
    __UHEAP_MARKEND;
    return 0;
    }
