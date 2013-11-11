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


#ifndef __HELLOTRACEINTERFACEDUP2_H__
#define __HELLOTRACEINTERFACEDUP2_H__

#include "HelloTracePanic.h"
#include "OstTraceDefinitions.h"
#ifdef OST_TRACE_COMPILER_IN_USE
#include "HelloTraceInterfaceDup2Traces.h"
#endif

 inline EXPORT_C TVersion CHelloTraceInterface::VersionDup2()
        {
        const TInt KMajor = 1;
        const TInt KMinor = 0;
        const TInt KBuild = 1;
        TVersion version = TVersion(KMajor, KMinor, KBuild);
        OstTraceExt1( TRACE_BORDER, CHELLOTRACEDLL_VERSION_DUP2, "CHelloTraceInterface::Version = %S", version.Name());   
        return version;
        }

 inline EXPORT_C const TPtrC CHelloTraceInterface::StringDup2() const
        {
        __ASSERT_ALWAYS(iString != NULL, Panic(EHelloTraceDllNullPointer));

        OstTraceDefExt2( OST_TRACE_CATEGORY_DEBUG, TRACE_BORDER, CHELLOTRACEDLL_STRING_DUP2, 
                         "[0x%08x] Returned %S", (TUint32) this, *iString);   
        return *iString;
        }

 inline EXPORT_C void CHelloTraceInterface::AddCharDup2L(const TChar& aChar)
        {
        __ASSERT_ALWAYS(iString != NULL, Panic(EHelloTraceDllNullPointer));

        OstTraceExt3( TRACE_BORDER, CHELLOTRACEDLL_ADDCHARL_DUP2, 
                      "[0x%08x] Adding %d to %S", (TUint32) this, (TInt32) aChar, *iString);   
        if (iString->Length() >= KHelloTraceDllBufferLength)
            {
            OstTrace1( TRACE_ERROR, CHELLOTRACEDLL_ADDCHARL_ERR_DUP2,
                       "[0x%08x] Not enough space to add character", (TUint32) this);
            User::Leave( KErrTooBig);
            }
        iString->Append(aChar);
        }

 inline EXPORT_C void CHelloTraceInterface::RemoveLastDup2()
        {
        __ASSERT_ALWAYS(iString != NULL, Panic(EHelloTraceDllNullPointer));

        OstTraceExt2( TRACE_BORDER, CHELLOTRACEDLL_REMOVELAST_DUP2, 
                      "[0x%08x] Attempting to remove last character from %S", (TUint32) this, *iString);
        if (iString->Length() > 0)
            {
            iString->SetLength(iString->Length() - 1);
            }
        else
            {
            OstTrace1( TRACE_WARNING, CHELLOTRACEDLL_REMOVELAST_WARN_DUP2, "[0x%08x] String already empty", (TUint32) this);   
            }
        }

 inline EXPORT_C void CHelloTraceInterface::ResetStringDup2()
        {
        OstTraceExt2( TRACE_BORDER, CHELLOTRACEDLL_RESETSTRING_DUP2, "[0x%08x] Reseting string %S", (TUint32) this, *iString);
        if (iString == NULL)
            {
            iString = new (ELeave) THelloTraceDllExampleString;
            }
        else
            {
            iString->SetLength(0);
            }
        }
#endif  // __HELLOTRACEINTERFACEDUP2_H__

