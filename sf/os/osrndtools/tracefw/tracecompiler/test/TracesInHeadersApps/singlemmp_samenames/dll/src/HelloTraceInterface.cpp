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



#include "HelloTraceInterface.h"
#include "HelloTraceInterfaceDup.h"
#include "HelloTraceInterface.inl"
#include "HelloTracePanic.h"
#include "OstTraceDefinitions.h"
#ifdef OST_TRACE_COMPILER_IN_USE
#include "HelloTraceInterfaceTraces.h"
#endif

EXPORT_C CHelloTraceInterface* CHelloTraceInterface::NewLC()
    {
    CHelloTraceInterface* self = new (ELeave) CHelloTraceInterface;
    CleanupStack::PushL(self);
    self->ConstructL();
    return self;
    }

EXPORT_C CHelloTraceInterface* CHelloTraceInterface::NewL()
    {
    CHelloTraceInterface* self = CHelloTraceInterface::NewLC();
    CleanupStack::Pop(self);
    return self;
    }

CHelloTraceInterface::CHelloTraceInterface()
    {
    OstTrace1( TRACE_NORMAL, CHELLOTRACEDLL_CHELLOTRACEDLL_CTOR, "[0x%08x]", (TUint32) this );    
    }

void CHelloTraceInterface::ConstructL()
    {
    iString = new (ELeave) THelloTraceDllExampleString;
    }

EXPORT_C CHelloTraceInterface::~CHelloTraceInterface()
    {
    delete iString;
    OstTrace1( TRACE_NORMAL, CHELLOTRACEDLL_CHELLOTRACEDLL_DTOR, "[0x%08x]", (TUint32) this );    
    }

EXPORT_C TVersion CHelloTraceInterface::Version()
    {
    const TInt KMajor = 1;
    const TInt KMinor = 0;
    const TInt KBuild = 1;
    TVersion version = TVersion(KMajor, KMinor, KBuild);
    OstTraceExt1( TRACE_BORDER, CHELLOTRACEDLL_VERSION, "CHelloTraceInterface::Version = %S", version.Name());   
    return version;
    }

EXPORT_C const TPtrC CHelloTraceInterface::String() const
    {
    __ASSERT_ALWAYS(iString != NULL, Panic(EHelloTraceDllNullPointer));

    OstTraceDefExt2( OST_TRACE_CATEGORY_DEBUG, TRACE_BORDER, CHELLOTRACEDLL_STRING, 
                     "[0x%08x] Returned %S", (TUint32) this, *iString);   
    return *iString;
    }

EXPORT_C void CHelloTraceInterface::AddCharL(const TChar& aChar)
    {
    __ASSERT_ALWAYS(iString != NULL, Panic(EHelloTraceDllNullPointer));

    OstTraceExt3( TRACE_BORDER, CHELLOTRACEDLL_ADDCHARL, 
                  "[0x%08x] Adding %d to %S", (TUint32) this, (TInt32) aChar, *iString);   
    if (iString->Length() >= KHelloTraceDllBufferLength)
        {
        OstTrace1( TRACE_ERROR, CHELLOTRACEDLL_ADDCHARL_ERR,
                   "[0x%08x] Not enough space to add character", (TUint32) this);
        User::Leave( KErrTooBig);
        }
    iString->Append(aChar);
    }

EXPORT_C void CHelloTraceInterface::RemoveLast()
    {
    __ASSERT_ALWAYS(iString != NULL, Panic(EHelloTraceDllNullPointer));

    OstTraceExt2( TRACE_BORDER, CHELLOTRACEDLL_REMOVELAST, 
                  "[0x%08x] Attempting to remove last character from %S", (TUint32) this, *iString);
    if (iString->Length() > 0)
        {
        iString->SetLength(iString->Length() - 1);
        }
    else
        {
        OstTrace1( TRACE_WARNING, CHELLOTRACEDLL_REMOVELAST_WARN, "[0x%08x] String already empty", (TUint32) this);   
        }
    }

EXPORT_C void CHelloTraceInterface::ResetString()
    {
    OstTraceExt2( TRACE_BORDER, CHELLOTRACEDLL_RESETSTRING, "[0x%08x] Reseting string %S", (TUint32) this, *iString);
    if (iString == NULL)
        {
        iString = new (ELeave) THelloTraceDllExampleString;
        }
    else
        {
        iString->SetLength(0);
        }
    }
