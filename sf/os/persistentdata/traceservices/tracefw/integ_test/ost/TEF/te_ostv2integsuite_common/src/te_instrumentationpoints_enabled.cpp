// Copyright (c) 2007-2009 Nokia Corporation and/or its subsidiary(-ies).
// All rights reserved.
// This component and the accompanying materials are made available
// under the terms of "Eclipse Public License v1.0"
// which accompanies this distribution, and is available
// at the URL "http://www.eclipse.org/legal/epl-v10.html".
//
// Initial Contributors:
// Nokia Corporation - initial contribution.
//
// Contributors:
//
// Description:
// Ost Performance Tests Ost Call.
//



/**
 @file te_instrumentationpoints_enabled.cpp
 @internalTechnology
 @prototype
*/


#include "te_instrumentationpoints.h"
#include "te_instrumentationpoints_enabledTraces.h"
#ifndef __KERNEL_MODE__
#include <e32base.h> //for cleanupstack
#endif

#include "te_perfstackinfo.h"
#include "te_dataparameters.h"
#include "sanity/te_perfsanityhelpers.h"
#include "te_ctor.h"



/*dummy function for Test function - used to calibrate and debug the timer class with the Lauterbach*/
void tracefoo()
	 {
	 volatile int gt[100]; //expect 400 bytes stack use - i.e. 100 words of 4bytes each
		for(int i=0; i<100;++i)
			gt[i] = i;
	 }


TBool TUptTraceCalls::IsKernelTrace(TInt aApiId)
	{
	TBool isKernelTrace = EFalse;
	switch(aApiId)
	{
		case EUptKernelPrintfchar:
		case EUptKernelPrintfDesc8_big:
		case EUptKernelPrintfDesc8_huge:
		case EUptKernelTrace:
		case EUptKernelTraceTUint32:
		case EUptKernelTraceTUint32TUint32:
		case EUptKernelTraceTAnySmallStruct:
		case EUptKernelTraceTAnyBigStruct:
		case EUptKernelTraceTAnyHugeStruct:
		case EUptKernelTraceTemplatedSmallStruct:
		case EUptKernelTraceTemplatedBigStruct:
		case EUptKernelTraceTemplatedHugeStruct:
		case EUptKernelIsTraceActive:
		case UPTKernelTraceTest:
		case UPTKernelNegativeFunctionalityTest:
		case UPTKernelMultipleFunctionalityTest:
		case UPTKernelBaseline:
		isKernelTrace = ETrue;
		break;
		default:
		break;
		}
	return isKernelTrace;
	}


TInt TUptTraceCalls::DoSendTraceL(const TApiRunConfig& aApiRunConfig, TApiRunResults& aApiRunResults)
	{
	TInt error = KErrGeneral;
	TBool logged = EFalse;
	TUint32 stackbase=0;
	TUint32 before=0;
	aApiRunResults.iApiStackUsage = 0;
	
    switch(aApiRunConfig.iApiId)
	{		
		
	case EUptPrintTDesC8_big:
	case EUptKernelPrintfDesc8_big:
		{
		__MARK_INITIAL_STACK(aApiRunConfig.iDoStack, stackbase, before);
		TTraceContext attributes(KComponentId, KGroupId, aApiRunConfig.iHasContext, aApiRunConfig.iHasProgramCounter);
		logged = OstPrint(attributes, KStringDesc8 );//TPtrC8((const TUint8*)KBigData8, KBigSize8)
		__RETURN_STACK_SIZE(aApiRunConfig.iDoStack, stackbase, before,  aApiRunResults.iApiStackUsage);
		}
	break;

	case EUptPrintTDesC8_huge:
	case EUptKernelPrintfDesc8_huge:
		{
		__MARK_INITIAL_STACK(aApiRunConfig.iDoStack, stackbase, before);
		TTraceContext attributes(KComponentId, KGroupId, aApiRunConfig.iHasContext, aApiRunConfig.iHasProgramCounter);
		logged = OstPrint(attributes, KStringDescHuge8);//TPtrC8((const TUint8*)KHugeData8, KHugeSize8)
		__RETURN_STACK_SIZE(aApiRunConfig.iDoStack, stackbase, before,  aApiRunResults.iApiStackUsage);
		}
	break;
		
	case EUptPrintTDesC16_big:
#ifndef __KERNEL_MODE__
		{
		__MARK_INITIAL_STACK(aApiRunConfig.iDoStack, stackbase, before);
		TTraceContext attributes(KComponentId, KGroupId,  aApiRunConfig.iHasContext, aApiRunConfig.iHasProgramCounter);
		logged = OstPrint(attributes, KStringDesc16);//TPtrC16((const TUint16*)KBigData16, KBigSize16)
		__RETURN_STACK_SIZE(aApiRunConfig.iDoStack, stackbase, before,  aApiRunResults.iApiStackUsage);
		}
#endif	
	break;
		
	case EUptPrintTDesC16_huge:
#ifndef __KERNEL_MODE__
		{
		__MARK_INITIAL_STACK(aApiRunConfig.iDoStack, stackbase, before);
		TTraceContext attributes(KComponentId, KGroupId,  aApiRunConfig.iHasContext, aApiRunConfig.iHasProgramCounter);
		logged = OstPrint(attributes, KStringDescHuge16);//TPtrC16((const TUint16*)KHugeData16, KHugeSize16)
		__RETURN_STACK_SIZE(aApiRunConfig.iDoStack, stackbase, before,  aApiRunResults.iApiStackUsage);
		}
#endif	
	break;
	
	
	case EUptTraceTDesC8_big:
	case EUptKernelTraceDesc8_big:
		{
		__MARK_INITIAL_STACK(aApiRunConfig.iDoStack, stackbase, before);
		TTraceContext attributes(KComponentId, KGroupId, aApiRunConfig.iHasContext, aApiRunConfig.iHasProgramCounter);
		logged = OstTrace(attributes, KTraceId, KStringDesc8);
		__RETURN_STACK_SIZE(aApiRunConfig.iDoStack, stackbase, before,  aApiRunResults.iApiStackUsage);
		}
	break;

	case EUptTraceTDesC8_huge:
	case EUptKernelTraceDesc8_huge:
		{
		__MARK_INITIAL_STACK(aApiRunConfig.iDoStack, stackbase, before);
		TTraceContext attributes(KComponentId, KGroupId, aApiRunConfig.iHasContext, aApiRunConfig.iHasProgramCounter);
		logged = OstTrace(attributes, KTraceId, KStringDescHuge8);
		__RETURN_STACK_SIZE(aApiRunConfig.iDoStack, stackbase, before,  aApiRunResults.iApiStackUsage);
		}
	break;
	

	
	case EUptTraceTDesC16_big:
#ifndef __KERNEL_MODE__
		{
		__MARK_INITIAL_STACK(aApiRunConfig.iDoStack, stackbase, before);
		TTraceContext attributes(KComponentId, KGroupId,  aApiRunConfig.iHasContext, aApiRunConfig.iHasProgramCounter);
		logged = OstTrace(attributes, KTraceId, KStringDesc16);
		__RETURN_STACK_SIZE(aApiRunConfig.iDoStack, stackbase, before,  aApiRunResults.iApiStackUsage);
		}
#endif	
	break;

	case EUptTraceTDesC16_huge:
#ifndef __KERNEL_MODE__
		{
		__MARK_INITIAL_STACK(aApiRunConfig.iDoStack, stackbase, before);
		TTraceContext attributes(KComponentId, KGroupId,  aApiRunConfig.iHasContext, aApiRunConfig.iHasProgramCounter);
		logged = OstTrace(attributes, KTraceId, KStringDescHuge16);
		__RETURN_STACK_SIZE(aApiRunConfig.iDoStack, stackbase, before,  aApiRunResults.iApiStackUsage);
		}
#endif	
	break;
	
	
	case EUptPrintfchar:
	case EUptKernelPrintfchar:
		{
		__MARK_INITIAL_STACK(aApiRunConfig.iDoStack, stackbase, before);
		TTraceContext attributes(KComponentId, KGroupId, aApiRunConfig.iHasContext, aApiRunConfig.iHasProgramCounter);
		logged = OstPrintf(attributes, KPrintfCStyleString);
		__RETURN_STACK_SIZE(aApiRunConfig.iDoStack, stackbase, before,  aApiRunResults.iApiStackUsage);
		}
	break;

	case EUptPrintfTRefByValueTDesC8:
	case EUptKernelPrintfTRefByValueTDesC8:
	    {
	    __MARK_INITIAL_STACK(aApiRunConfig.iDoStack, stackbase, before);
	    TTraceContext attributes(KComponentId, KGroupId, aApiRunConfig.iHasContext, aApiRunConfig.iHasProgramCounter);
	    logged = OstPrintf(attributes, KDesC8Formatted,2);
	    __RETURN_STACK_SIZE(aApiRunConfig.iDoStack, stackbase, before,  aApiRunResults.iApiStackUsage);
	    }

	break;
	case EUptPrintfTRefByValueTDesC16:
#ifndef __KERNEL_MODE__
		{
		__MARK_INITIAL_STACK(aApiRunConfig.iDoStack, stackbase, before);
		TTraceContext attributes(KComponentId, KGroupId, aApiRunConfig.iHasContext, aApiRunConfig.iHasProgramCounter);
		logged = OstPrintf(attributes,KDesCFormatted16,2);
		__RETURN_STACK_SIZE(aApiRunConfig.iDoStack, stackbase, before,  aApiRunResults.iApiStackUsage);
		}
#endif
	break;
		
	case EUptTrace:
	case EUptKernelTrace:
		{
		__MARK_INITIAL_STACK(aApiRunConfig.iDoStack, stackbase, before);
		TTraceContext attributes(KComponentId, KGroupId, aApiRunConfig.iHasContext, aApiRunConfig.iHasProgramCounter);
		logged = OstTrace(attributes, KTraceId);
		__RETURN_STACK_SIZE(aApiRunConfig.iDoStack, stackbase, before,  aApiRunResults.iApiStackUsage);
		}
	break;
		
	case EUptTraceTUint32:
	case EUptKernelTraceTUint32:
		{
		__MARK_INITIAL_STACK(aApiRunConfig.iDoStack, stackbase, before);
		TTraceContext attributes(KComponentId, KGroupId, aApiRunConfig.iHasContext, aApiRunConfig.iHasProgramCounter);
		logged = OstTrace(attributes, KTraceId, KData1);
		__RETURN_STACK_SIZE(aApiRunConfig.iDoStack, stackbase, before,  aApiRunResults.iApiStackUsage);
		}
	break;
		
	case EUptTraceTUint32TUint32:
	case EUptKernelTraceTUint32TUint32:
		{
		__MARK_INITIAL_STACK(aApiRunConfig.iDoStack, stackbase, before);
		TTraceContext attributes(KComponentId, KGroupId, aApiRunConfig.iHasContext, aApiRunConfig.iHasProgramCounter);
		logged = OstTrace(attributes, KTraceId, KData1, KData2);
		__RETURN_STACK_SIZE(aApiRunConfig.iDoStack, stackbase, before,  aApiRunResults.iApiStackUsage);				
		}
	break;
		
	case EUptTraceTAnySmallStruct:
	case EUptKernelTraceTAnySmallStruct:
		{
		__MARK_INITIAL_STACK(aApiRunConfig.iDoStack, stackbase, before);
		TTraceContext attributes(KComponentId, KGroupId, aApiRunConfig.iHasContext, aApiRunConfig.iHasProgramCounter);
		logged = OstTrace(attributes, KTraceId, KSmallData, KSmallSize);
		__RETURN_STACK_SIZE(aApiRunConfig.iDoStack, stackbase, before,  aApiRunResults.iApiStackUsage);
		}
	break;
	case EUptTraceTAnyBigStruct:
	case EUptKernelTraceTAnyBigStruct:
		{
		__MARK_INITIAL_STACK(aApiRunConfig.iDoStack, stackbase, before);
		TTraceContext attributes(KComponentId, KGroupId, aApiRunConfig.iHasContext, aApiRunConfig.iHasProgramCounter);
		logged = OstTrace(attributes, KTraceId, KBigData8, KBigSize8);
		__RETURN_STACK_SIZE(aApiRunConfig.iDoStack, stackbase, before,  aApiRunResults.iApiStackUsage);
		}
	break;

	case EUptTraceTAnyHugeStruct:
	case EUptKernelTraceTAnyHugeStruct:
		{
		__MARK_INITIAL_STACK(aApiRunConfig.iDoStack, stackbase, before);
		TTraceContext attributes(KComponentId, KGroupId, aApiRunConfig.iHasContext, aApiRunConfig.iHasProgramCounter);
		logged = OstTrace(attributes, KTraceId, KHugeData8, KHugeSize8);
		__RETURN_STACK_SIZE(aApiRunConfig.iDoStack, stackbase, before,  aApiRunResults.iApiStackUsage);
		}
	break;
		
	
	case EUptTraceTemplatedSmallStruct:
	case EUptKernelTraceTemplatedSmallStruct:
		{
		__MARK_INITIAL_STACK(aApiRunConfig.iDoStack, stackbase, before);
		TTraceContext attributes(KComponentId, KGroupId, aApiRunConfig.iHasContext, aApiRunConfig.iHasProgramCounter);
		logged = OstTrace(attributes, KTraceId, KStructForTemplated);
		__RETURN_STACK_SIZE(aApiRunConfig.iDoStack, stackbase, before,  aApiRunResults.iApiStackUsage);
		}
	break;
	
	case EUptTraceTemplatedBigStruct:
	case EUptKernelTraceTemplatedBigStruct:
		{
		__MARK_INITIAL_STACK(aApiRunConfig.iDoStack, stackbase, before);
		TTraceContext attributes(KComponentId, KGroupId, aApiRunConfig.iHasContext, aApiRunConfig.iHasProgramCounter);
		logged = OstTrace(attributes, KTraceId, KBigData8);
		__RETURN_STACK_SIZE(aApiRunConfig.iDoStack, stackbase, before,  aApiRunResults.iApiStackUsage);
		}
	break;
	
	case EUptTraceTemplatedHugeStruct:
	case EUptKernelTraceTemplatedHugeStruct:
		{
		__MARK_INITIAL_STACK(aApiRunConfig.iDoStack, stackbase, before);
		TTraceContext attributes(KComponentId, KGroupId, aApiRunConfig.iHasContext, aApiRunConfig.iHasProgramCounter);
		logged = OstTrace(attributes, KTraceId, KHugeData8);
		__RETURN_STACK_SIZE(aApiRunConfig.iDoStack, stackbase, before,  aApiRunResults.iApiStackUsage);
		}
	break;
		
	case EUptIsTraceActive:
	case EUptKernelIsTraceActive:
		{
		__MARK_INITIAL_STACK(aApiRunConfig.iDoStack, stackbase, before);
		TTraceContext attributes(KComponentId, KGroupId, aApiRunConfig.iHasContext, aApiRunConfig.iHasProgramCounter);
		logged = IsTraceActive(attributes);
		__RETURN_STACK_SIZE(aApiRunConfig.iDoStack, stackbase, before,  aApiRunResults.iApiStackUsage);
		}
	break;
		

	case UPTTraceTest:
	case UPTKernelTraceTest:
		{
		__MARK_INITIAL_STACK(aApiRunConfig.iDoStack, stackbase, before);
		SanityFoo();
		__RETURN_STACK_SIZE(aApiRunConfig.iDoStack, stackbase, before,  aApiRunResults.iApiStackUsage);
		}
	break;
	
	case UPTKernelNegativeFunctionalityTest:
		{
		logged = OstTrace(TTraceContext(KComponentId+1, KGroupId+1, aApiRunConfig.iHasContext, aApiRunConfig.iHasProgramCounter), KTraceId);
		logged = OstTrace(TTraceContext(KComponentId+2, KGroupId+2, aApiRunConfig.iHasContext, aApiRunConfig.iHasProgramCounter), KTraceId);
		}
	break;
	
	case UPTKernelMultipleFunctionalityTest:
		{
		logged = OstTrace(TTraceContext(KComponentId, KGroupId, aApiRunConfig.iHasContext, aApiRunConfig.iHasProgramCounter), KTraceId);
		logged = OstTrace(TTraceContext(KComponentId + 1, KGroupId, aApiRunConfig.iHasContext, aApiRunConfig.iHasProgramCounter), KTraceId);
		logged = OstTrace(TTraceContext(KComponentId + 2, KGroupId, aApiRunConfig.iHasContext, aApiRunConfig.iHasProgramCounter), KTraceId);
		logged = OstTrace(TTraceContext(KComponentId, KGroupId + 2, aApiRunConfig.iHasContext, aApiRunConfig.iHasProgramCounter), KTraceId);
		logged = OstTrace(TTraceContext(KComponentId + 2, KGroupId + 2, aApiRunConfig.iHasContext, aApiRunConfig.iHasProgramCounter), KTraceId);
		logged = OstTrace(TTraceContext(KComponentId + 1, KGroupId + 2, aApiRunConfig.iHasContext, aApiRunConfig.iHasProgramCounter), KTraceId);
		logged = OstTrace(TTraceContext(KComponentId, KGroupId + 1, aApiRunConfig.iHasContext, aApiRunConfig.iHasProgramCounter), KTraceId);
		}
		break;
		
	case BTraceFiltered4:
		{
		__MARK_INITIAL_STACK(aApiRunConfig.iDoStack, stackbase, before);
		logged = BTraceFiltered4(KGroupId,0 ,KComponentId);
		__RETURN_STACK_SIZE(aApiRunConfig.iDoStack, stackbase, before,  aApiRunResults.iApiStackUsage);
		}
	break;
	case BTraceFiltered8:
		{
		__MARK_INITIAL_STACK(aApiRunConfig.iDoStack, stackbase, before);
		logged = BTraceFiltered8(KGroupId,0 ,KComponentId,KData1);
		__RETURN_STACK_SIZE(aApiRunConfig.iDoStack, stackbase, before,  aApiRunResults.iApiStackUsage);
		}
	break;
	case BTraceFiltered12:
		{
		__MARK_INITIAL_STACK(aApiRunConfig.iDoStack, stackbase, before);
		logged = BTraceFiltered12(KGroupId,0 ,KComponentId,KData1,KData2);
		__RETURN_STACK_SIZE(aApiRunConfig.iDoStack, stackbase, before,  aApiRunResults.iApiStackUsage);
		}
	
	break;
	case BTraceFilteredBig:
		{
		__MARK_INITIAL_STACK(aApiRunConfig.iDoStack, stackbase, before);
		logged = BTraceFilteredBig(KGroupId,0 ,KComponentId,KHugeData8,sizeof(KHugeData8));
		__RETURN_STACK_SIZE(aApiRunConfig.iDoStack, stackbase, before,  aApiRunResults.iApiStackUsage);
		}
	break;
	case BTrace0:
		{
		__MARK_INITIAL_STACK(aApiRunConfig.iDoStack, stackbase, before);
		logged = BTrace0(KGroupId,0 );
		__RETURN_STACK_SIZE(aApiRunConfig.iDoStack, stackbase, before,  aApiRunResults.iApiStackUsage);
		}
	break;
	case BTrace4:
		{
		__MARK_INITIAL_STACK(aApiRunConfig.iDoStack, stackbase, before);
		logged = BTrace4(KGroupId,0 ,KData1);
		__RETURN_STACK_SIZE(aApiRunConfig.iDoStack, stackbase, before,  aApiRunResults.iApiStackUsage);						
		}
	break;
	case BTrace8:
		{
		__MARK_INITIAL_STACK(aApiRunConfig.iDoStack, stackbase, before);
		logged = BTrace8(KGroupId,0 ,KData1, KData2);
		__RETURN_STACK_SIZE(aApiRunConfig.iDoStack, stackbase, before,  aApiRunResults.iApiStackUsage);		
		}
	break;
	case BTrace12:
		{
		__MARK_INITIAL_STACK(aApiRunConfig.iDoStack, stackbase, before);
		logged = BTrace12(KGroupId,0 ,KData1, KData2, KData3);
		__RETURN_STACK_SIZE(aApiRunConfig.iDoStack, stackbase, before,  aApiRunResults.iApiStackUsage);		
		}
	break;		
	case UPTKernelBaseline:
	case UPTBaseline:
		{
		__MARK_INITIAL_STACK(aApiRunConfig.iDoStack, stackbase, before);
		__RETURN_STACK_SIZE(aApiRunConfig.iDoStack, stackbase, before,  aApiRunResults.iApiStackUsage);
		}
	break;
	
	case EOstTraceDef0:
        {
        /**
        Preprocessor level for all traces on. 
        This should not be used from traces
        */
        #define OST_TRACE_CATEGORY_ALL 0xFFFFFFFF
        __MARK_INITIAL_STACK(aApiRunConfig.iDoStack, stackbase, before);
        OstTraceDef0( OST_TRACE_CATEGORY_ALL, TRACE_FATAL, TEST_OstTraceDef0, "EOstTraceDef0" ) ;
        logged = ETrue;
        __RETURN_STACK_SIZE(aApiRunConfig.iDoStack, stackbase, before,  aApiRunResults.iApiStackUsage); 
        }
    break;
    
	case EOstTraceDef1:
        {
        /**
        Preprocessor level for all traces on. 
        This should not be used from traces
        */
        #define OST_TRACE_CATEGORY_ALL 0xFFFFFFFF
        __MARK_INITIAL_STACK(aApiRunConfig.iDoStack, stackbase, before);
        OstTraceDef1( OST_TRACE_CATEGORY_ALL, TRACE_FATAL, TEST_OstTraceDef1, "EOstTraceDef1 - %u" , KData1) ;
        logged = ETrue;
        __RETURN_STACK_SIZE(aApiRunConfig.iDoStack, stackbase, before,  aApiRunResults.iApiStackUsage);  
            
        }
    break;
    
	case EOstTraceDefData:
        {
        /**
        Preprocessor level for all traces on. 
        This should not be used from traces
        */
        #define OST_TRACE_CATEGORY_ALL 0xFFFFFFFF
        const TAny* y = &KData1;
        
        __MARK_INITIAL_STACK(aApiRunConfig.iDoStack, stackbase, before);
        OstTraceDefData( OST_TRACE_CATEGORY_ALL, TRACE_FATAL, TEST_OstTraceDefData, "EOstTraceDefData", y, sizeof(KData1) ) ;
        logged = ETrue;
        __RETURN_STACK_SIZE(aApiRunConfig.iDoStack, stackbase, before,  aApiRunResults.iApiStackUsage); 

        }
    break;
    
	case EOstTraceDefExt1:
        {
        /**
        Preprocessor level for all traces on. 
        This should not be used from traces
        */
        #define OST_TRACE_CATEGORY_ALL 0xFFFFFFFF
        
        __MARK_INITIAL_STACK(aApiRunConfig.iDoStack, stackbase, before);
        OstTraceDefExt1( OST_TRACE_CATEGORY_ALL, TRACE_FATAL, TEST_OstTraceDefExt1, "OstTraceDefExt1 - %s", KDefault8 ) ;
        logged = ETrue;
        __RETURN_STACK_SIZE(aApiRunConfig.iDoStack, stackbase, before,  aApiRunResults.iApiStackUsage); 

        }
    break;
    
    case EOstTraceDefExt2:
        {
        /**
        Preprocessor level for all traces on. 
        This should not be used from traces
        */
        #define OST_TRACE_CATEGORY_ALL 0xFFFFFFFF
        
        __MARK_INITIAL_STACK(aApiRunConfig.iDoStack, stackbase, before);
        
        OstTraceDefExt2( OST_TRACE_CATEGORY_ALL, TRACE_FATAL, TEST_OstTraceDefExt2, "OstTraceDefExt2 - %u - %u", KData1 , KData1) ;
        logged = ETrue;
        __RETURN_STACK_SIZE(aApiRunConfig.iDoStack, stackbase, before,  aApiRunResults.iApiStackUsage); 

        }
    break;
    
    case EOstTraceDefExt3:
        {
        /**
        Preprocessor level for all traces on. 
        This should not be used from traces
        */
        #define OST_TRACE_CATEGORY_ALL 0xFFFFFFFF
        
        __MARK_INITIAL_STACK(aApiRunConfig.iDoStack, stackbase, before);
        
        OstTraceDefExt3( OST_TRACE_CATEGORY_ALL, TRACE_FATAL, TEST_OstTraceDefExt3, "OstTraceDefExt3 - %u - %u - %u", KData1 , KData1, KData1) ;
        logged = ETrue;
        __RETURN_STACK_SIZE(aApiRunConfig.iDoStack, stackbase, before,  aApiRunResults.iApiStackUsage); 

        }
    break;
    
    case EOstTraceDefExt4:
        {
        /**
        Preprocessor level for all traces on. 
        This should not be used from traces
        */
        #define OST_TRACE_CATEGORY_ALL 0xFFFFFFFF
        
        __MARK_INITIAL_STACK(aApiRunConfig.iDoStack, stackbase, before);
        
        OstTraceDefExt4( OST_TRACE_CATEGORY_ALL, TRACE_FATAL, TEST_OstTraceDefExt4, "OstTraceDefExt4 - %u - %u - %u - %u", KData1 , KData1, KData1, KData1) ;
        logged = ETrue;
        __RETURN_STACK_SIZE(aApiRunConfig.iDoStack, stackbase, before,  aApiRunResults.iApiStackUsage); 

        }
    break;
    
    case EOstTraceDefExt5:
        {
        /**
        Preprocessor level for all traces on. 
        This should not be used from traces
        */
        #define OST_TRACE_CATEGORY_ALL 0xFFFFFFFF
        
        __MARK_INITIAL_STACK(aApiRunConfig.iDoStack, stackbase, before);
        
        OstTraceDefExt5( OST_TRACE_CATEGORY_ALL, TRACE_FATAL, TEST_OstTraceDefExt5, "OstTraceDefExt5 - %u - %u - %u - %u - %u", KData1, KData1, KData1, KData1, KData1 ) ;
        logged = ETrue;
        __RETURN_STACK_SIZE(aApiRunConfig.iDoStack, stackbase, before,  aApiRunResults.iApiStackUsage); 

        }
    break;
    
    case EOstTrace0:
        {
        /**
        Preprocessor level for all traces on. 
        This should not be used from traces
        */
        #define OST_TRACE_CATEGORY_ALL 0xFFFFFFFF
        
        __MARK_INITIAL_STACK(aApiRunConfig.iDoStack, stackbase, before);
        OstTrace0( TRACE_FATAL, TEST_OSTTRACE0, "EOstTrace0" );
        logged = ETrue;
        __RETURN_STACK_SIZE(aApiRunConfig.iDoStack, stackbase, before,  aApiRunResults.iApiStackUsage); 
        }
    break;
    
    case EOstTrace1:
        {
        /**
        Preprocessor level for all traces on. 
        This should not be used from traces
        */
        #define OST_TRACE_CATEGORY_ALL 0xFFFFFFFF
        
        __MARK_INITIAL_STACK(aApiRunConfig.iDoStack, stackbase, before);
        OstTrace1( TRACE_FATAL, TEST_OSTTRACE1, "EOstTrace1  - %u" , KData1);
        logged = ETrue;
        __RETURN_STACK_SIZE(aApiRunConfig.iDoStack, stackbase, before,  aApiRunResults.iApiStackUsage);

        }
    break;
    
    case EOstTraceData:
        {
        /**
        Preprocessor level for all traces on. 
        This should not be used from traces
        */
        #define OST_TRACE_CATEGORY_ALL 0xFFFFFFFF
        const TAny* y = &KData1;
        
        __MARK_INITIAL_STACK(aApiRunConfig.iDoStack, stackbase, before);
        OstTraceData( TRACE_FATAL, TEST_OSTTRACEDATA, "EOSTTraceData", y, sizeof(KData1) );
        logged = ETrue;
        __RETURN_STACK_SIZE(aApiRunConfig.iDoStack, stackbase, before,  aApiRunResults.iApiStackUsage); 

        }
    break;
    
    case EOstTraceExt1:
        {
        /**
        Preprocessor level for all traces on. 
        This should not be used from traces
        */
        #define OST_TRACE_CATEGORY_ALL 0xFFFFFFFF
        
        __MARK_INITIAL_STACK(aApiRunConfig.iDoStack, stackbase, before);
        OstTraceExt1( TRACE_FATAL, TEST_OSTTRACEEXT1, "OSTTraceExt1 - %hhd", KData8 ) ;
        logged = ETrue;
        __RETURN_STACK_SIZE(aApiRunConfig.iDoStack, stackbase, before,  aApiRunResults.iApiStackUsage); 

        }
    break;
    
    case EOstTraceExt2:
        {
        /**
        Preprocessor level for all traces on. 
        This should not be used from traces
        */
        #define OST_TRACE_CATEGORY_ALL 0xFFFFFFFF
        
        __MARK_INITIAL_STACK(aApiRunConfig.iDoStack, stackbase, before);
        OstTraceExt2( TRACE_FATAL, TEST_OSTTRACEEXT2, "OSTTraceExt2 - %u - %u", KData1 ,KData1) ;
        logged = ETrue;
        __RETURN_STACK_SIZE(aApiRunConfig.iDoStack, stackbase, before,  aApiRunResults.iApiStackUsage); 

        }
    break;
    
    case EOstTraceExt3:
        {
        /**
        Preprocessor level for all traces on. 
        This should not be used from traces
        */
        #define OST_TRACE_CATEGORY_ALL 0xFFFFFFFF          
        
        __MARK_INITIAL_STACK(aApiRunConfig.iDoStack, stackbase, before);
        OstTraceExt3( TRACE_FATAL, TEST_OSTTRACEEXT3, "OSTTraceExt3 - %u - %u - %u", KData1 ,KData1, KData1) ;
        logged = ETrue;
        __RETURN_STACK_SIZE(aApiRunConfig.iDoStack, stackbase, before,  aApiRunResults.iApiStackUsage); 

        }
    break;
    
    case EOstTraceExt4:
        {
        /**
        Preprocessor level for all traces on. 
        This should not be used from traces
        */
        #define OST_TRACE_CATEGORY_ALL 0xFFFFFFFF
        
        __MARK_INITIAL_STACK(aApiRunConfig.iDoStack, stackbase, before);
        OstTraceExt4( TRACE_FATAL, TEST_OSTTRACEEXT4, "OSTTraceExt4 - %u - %u - %u - %u", KData1 ,KData1, KData1, KData1) ;
        logged = ETrue;
        __RETURN_STACK_SIZE(aApiRunConfig.iDoStack, stackbase, before,  aApiRunResults.iApiStackUsage); 

        }
    break;
    
    case EOstTraceExt5:
        {
        /**
        Preprocessor level for all traces on. 
        This should not be used from traces
        */
        #define OST_TRACE_CATEGORY_ALL 0xFFFFFFFF
        
        __MARK_INITIAL_STACK(aApiRunConfig.iDoStack, stackbase, before);
        OstTraceExt5( TRACE_FATAL, TEST_OSTTRACEEXT5, "OSTTraceExt5 - %u - %u - %u - %u - %u", KData1 ,KData1, KData1, KData1, KData1) ;
        logged = ETrue;
        __RETURN_STACK_SIZE(aApiRunConfig.iDoStack, stackbase, before,  aApiRunResults.iApiStackUsage); 

        }
    break;
    
    case EOstTraceFunctionEntry0:
        {
        __MARK_INITIAL_STACK(aApiRunConfig.iDoStack, stackbase, before);
        OstTraceFunctionEntry0( TEST_OSTTraceFunctionalityEntry0 );
        logged = ETrue;
        __RETURN_STACK_SIZE(aApiRunConfig.iDoStack, stackbase, before,  aApiRunResults.iApiStackUsage); 
        }
    break;
    
    case EOstTraceFunctionEntry1:
        {
        //const TAny* y = &KData1;
        
        __MARK_INITIAL_STACK(aApiRunConfig.iDoStack, stackbase, before);
        OstTraceFunctionEntry1( TEST_OSTTraceFunctionalityEntry1, KData1 );
        logged = ETrue;
        __RETURN_STACK_SIZE(aApiRunConfig.iDoStack, stackbase, before,  aApiRunResults.iApiStackUsage); 

        }
    break;
    
    case EOstTraceFunctionEntryExt:
        {
        
        __MARK_INITIAL_STACK(aApiRunConfig.iDoStack, stackbase, before);
        OstTraceFunctionEntryExt( TEST_OSTTraceFunctionalityExt, KData1 );
        logged = ETrue;
        __RETURN_STACK_SIZE(aApiRunConfig.iDoStack, stackbase, before,  aApiRunResults.iApiStackUsage); 

        }
    break;
    
    case EOstTraceFunctionEntryExtCtorInitList:
        {        
        __MARK_INITIAL_STACK(aApiRunConfig.iDoStack, stackbase, before);
        TTestCtor       testCtorA( 1, 2 );
        logged = ETrue;
        __RETURN_STACK_SIZE(aApiRunConfig.iDoStack, stackbase, before,  aApiRunResults.iApiStackUsage); 

        }
    break;
    
    case EOstTraceFunctionEntryExtCtorNoInitList:
        {        
        __MARK_INITIAL_STACK(aApiRunConfig.iDoStack, stackbase, before);
        TTestCtor       testCtorB( 3 );
        logged = ETrue;
        __RETURN_STACK_SIZE(aApiRunConfig.iDoStack, stackbase, before,  aApiRunResults.iApiStackUsage); 

        }
    break;
    
    case EOstTraceFunctionExit0:
        {
        __MARK_INITIAL_STACK(aApiRunConfig.iDoStack, stackbase, before);
        OstTraceFunctionExit0( TEST_OstTraceFunctionExit0 );
        logged = ETrue;
        __RETURN_STACK_SIZE(aApiRunConfig.iDoStack, stackbase, before,  aApiRunResults.iApiStackUsage); 
        }
    break;
    
    case EOstTraceFunctionExit1:
        {
        //const TAny* y = &KData1;
        
        __MARK_INITIAL_STACK(aApiRunConfig.iDoStack, stackbase, before);
        OstTraceFunctionExit1( TEST_OSTTRACEFUNCTIONEXIT1, KData1 );
        logged = ETrue;
        __RETURN_STACK_SIZE(aApiRunConfig.iDoStack, stackbase, before,  aApiRunResults.iApiStackUsage); 

        }
    break;
    
    case EOstTraceFunctionExitExt:
        {
        TInt returnCode = 0;
        
        __MARK_INITIAL_STACK(aApiRunConfig.iDoStack, stackbase, before);
        OstTraceFunctionExitExt( TEST_OSTTRACEFUNCTIONEXITEXT, KData1 , returnCode );
        logged = ETrue;
        __RETURN_STACK_SIZE(aApiRunConfig.iDoStack, stackbase, before,  aApiRunResults.iApiStackUsage); 

        }
    break;
    
    case EOstTraceEventStart0:
        {
        __MARK_INITIAL_STACK(aApiRunConfig.iDoStack, stackbase, before);
        OstTraceEventStart0( TEST_OSTTRACEEVENTSTART0, "EOstTraceEventStart0" );
        logged = ETrue;
        __RETURN_STACK_SIZE(aApiRunConfig.iDoStack, stackbase, before,  aApiRunResults.iApiStackUsage); 
        }
    break;
    
    case EOstTraceEventStart1:
        {
        __MARK_INITIAL_STACK(aApiRunConfig.iDoStack, stackbase, before);
        OstTraceEventStart1( TEST_OSTTRACEEVENTSTART1, "EOstTraceEventStart1 - %u", KData1 ) ;
        logged = ETrue;
        __RETURN_STACK_SIZE(aApiRunConfig.iDoStack, stackbase, before,  aApiRunResults.iApiStackUsage); 

        }
    break;
    
    case EOstTraceEventStop:
        {
        __MARK_INITIAL_STACK(aApiRunConfig.iDoStack, stackbase, before);
        OstTraceEventStop( TEST_OSTTRACEEVENTSTOP, "EOstTraceEventStop", TEST_OSTTRACEEVENTSTART0 );
        logged = ETrue;
        __RETURN_STACK_SIZE(aApiRunConfig.iDoStack, stackbase, before,  aApiRunResults.iApiStackUsage); 
        }
    break;
    
    case EOstTraceState0:
        {
        __MARK_INITIAL_STACK(aApiRunConfig.iDoStack, stackbase, before);
        OstTraceState0( TEST_OSTTRACESTATE0, "Test State Name", "Test State Value" );
        logged = ETrue;
        __RETURN_STACK_SIZE(aApiRunConfig.iDoStack, stackbase, before,  aApiRunResults.iApiStackUsage); 
        }
    break;
	
    case EOstTraceState1:
        {
      //  const TAny* y = &KData1;
        
        __MARK_INITIAL_STACK(aApiRunConfig.iDoStack, stackbase, before);
        OstTraceState1( TEST_OSTTRACESTATE1, "Test State Name", "Test State Value", KData1 );
        logged = ETrue;
        __RETURN_STACK_SIZE(aApiRunConfig.iDoStack, stackbase, before,  aApiRunResults.iApiStackUsage); 

        }
    break;
    
    case EOstTraceData96:
        {
        /**
        Preprocessor level for all traces on. 
        This should not be used from traces
        */
        #define OST_TRACE_CATEGORY_ALL 0xFFFFFFFF
        
        __MARK_INITIAL_STACK(aApiRunConfig.iDoStack, stackbase, before);
        OstTraceData( TRACE_FATAL, TEST_OSTTRACEDATA96, "EOSTTraceData96", KBigData8, KBigSize8 );
        logged = ETrue;
        __RETURN_STACK_SIZE(aApiRunConfig.iDoStack, stackbase, before,  aApiRunResults.iApiStackUsage); 

        }
    break;
    
    case EOstTraceData1000:
        {
        /**
        Preprocessor level for all traces on. 
        This should not be used from traces
        */
        #define OST_TRACE_CATEGORY_ALL 0xFFFFFFFF
        
        __MARK_INITIAL_STACK(aApiRunConfig.iDoStack, stackbase, before);
        OstTraceData( TRACE_FATAL, TEST_OSTTRACEDATA1000, "EOSTTraceData1000", KHugeData8, KHugeSize8 );
        logged = ETrue;
        __RETURN_STACK_SIZE(aApiRunConfig.iDoStack, stackbase, before,  aApiRunResults.iApiStackUsage); 

        }
    break;
    
	default:
		error = KErrNotSupported;
		break;
	}
 	
	//if the "trace" is in fact not an actual trace and trace not sent - this is a true result 
	TBool realTrace = ETrue;
	switch(aApiRunConfig.iApiId)
	{
	case UPTTraceTest:
	case UPTKernelTraceTest:
	case UPTKernelNegativeFunctionalityTest:
	case UPTKernelMultipleFunctionalityTest:
	case UPTBaseline:
	case UPTKernelBaseline:
	case UPTFooUserTime:
	case UPTFooKernelTime:
		realTrace = EFalse;
	break;
	default:
	break;		
	}		
	
	//ensure correct error values for the current test configurations are returned to the test framework
	if(!logged)
		{
		if(error!=KErrNotSupported)
			error = KErrGeneral;
		}
	else
		error = KErrNone;
		

	//if trace has been disabled and trace not sent - this is a true result 
	#ifdef TE_UPT_TRACE_DISABLED
	if(error == KErrGeneral)
			error = KErrNone;
	#endif

	if(error == KErrGeneral && realTrace == EFalse)
		error = KErrNone;


	//returns KErrNone if logged, KErrNotLogged if not logged, KErrGeneral if any other problem...
	return error;
	}



//eof
