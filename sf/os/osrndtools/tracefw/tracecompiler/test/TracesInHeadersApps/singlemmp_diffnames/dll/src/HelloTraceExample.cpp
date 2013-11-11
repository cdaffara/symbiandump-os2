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


#include "HelloTraceExample.h"
#include "HelloTraceTypes.h"
#include "HelloTraceInterface.h"
#include "HelloTraceFn.h"
#include "HelloTracePanic.h"

#include "OstTraceDefinitions.h"
#ifdef OST_TRACE_COMPILER_IN_USE
#include "HelloTraceExampleTraces.h"
#endif

EXPORT_C void HelloTraceExample::JustTypes()
    {
    OstTrace0( TRACE_EXAMPLE, HELLOTRACEEXAMPLE_JUSTTYPES, "*********************************************** " );
    
    HelloTraceTypes::SignedIntegers();
    HelloTraceTypes::UnsignedIntegers();
    HelloTraceTypes::Descriptors();    
    }

EXPORT_C void HelloTraceExample::Interface()
    {
    OstTrace0( TRACE_EXAMPLE, HELLOTRACEEXAMPLE_INTERFACE, "*********************************************** " );
    
    TVersion version = CHelloTraceInterface::Version();   
    if (version.iMajor >= 1) 
        {
        CHelloTraceInterface* example = CHelloTraceInterface::NewLC();
        example->AddCharL('H');
        example->AddCharL('e');
        example->AddCharL('l');
        example->AddCharL('l');
        example->AddCharL('o');
        example->AddCharL('T');        
        example->RemoveLast();        
        example->AddCharL(' ');
        example->AddCharL('T');
        example->AddCharL('r');
        example->AddCharL('a');
        example->AddCharL('c');
        example->AddCharL('e');
        example->AddCharL('!');
        CleanupStack::PopAndDestroy(example);
        }
    else
        {
        OstTrace0( TRACE_ERROR, HELLOTRACEEXAMPLE_UNSUPPORTED_VERSION, "Unsupported version!");
        }
    }

EXPORT_C void HelloTraceExample::FnEntryExit()
    {
    OstTrace0( TRACE_EXAMPLE, HELLOTRACEEXAMPLE_FNENTRYEXIT, "*********************************************** " );
       
    CHelloTraceFn* example = CHelloTraceFn::NewLC();
    CIdle* idle = CIdle::NewL(CActive::EPriorityIdle);
    CleanupStack::PushL(idle);

    CHelloTraceFn::StaticOutputsParamsL(1);
   
    example->Simple();
    example->OutputsParams(2, CHelloTraceFn::EExample1);
    example->OutputsParams2(2, CHelloTraceFn::EExample1);
    example->OutputsParams3(2, CHelloTraceFn::EExample1);
    example->OutputsParams(3, CHelloTraceFn::EExample2);
    example->OutputsSignedIntegers(-4, -5, -6, -7);
    example->OutputsUnsignedIntegers(8, 9, 10, 11);
    example->OutputsTIntReturnValue();    
    example->OutputsTInt64ReturnValue(); 
    example->OutputsTFnEnumReturnValue();
    example->OutputsUnknownPtrType(idle);    
    example->OutputsUnknownRefType(*idle);
#ifdef _DEBUG
    example->PreProcessedFn(12);
#else
    example->PreProcessedFn();
#endif
    example->OutputMissingParams(13, 14, 15);

    CleanupStack::PopAndDestroy(idle);
    CleanupStack::PopAndDestroy(example);
    }

EXPORT_C void HelloTraceExample::PanicTrace()
    {
    OstTrace0( TRACE_EXAMPLE, HELLOTRACEEXAMPLE_PANIC, "*********************************************** " );

    Panic(EHelloTraceDllExamplePanic);
    }
