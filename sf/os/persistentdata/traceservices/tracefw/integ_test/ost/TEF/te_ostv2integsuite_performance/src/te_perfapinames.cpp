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
// UTrace Performance Tests API Name string generator.
//



/**
 @file te_perfapinames.cpp
 @internalTechnology
 @prototype
*/
#include <e32base.h> 


#include "te_perfapinames.h"


TApiNames::TApiNames()
	{
	}

TApiNames::~TApiNames()
	{
	}

TPtrC8 TApiNames::GetApiIdString(const TInt aApiId)
	{
/*	
	RBuf8 apiString;//[1024];
	CleanupClosePushL(apiString);
	TInt numbytes = 1024;// 2*sizeof(TPtrC8);
	apiString.CreateL(numbytes);
*/
	TBuf8<0x100> apiString;
	
		switch(aApiId)
			{
			//User side
				//printf
			case EUptPrintTDesC8_big:
				{
				apiString.Append(_L("UptPrintTDesC8_big"));
				}
			break;			
			case EUptPrintTDesC8_huge:
				{
				apiString.Append(_L("UptPrintTDesC8_huge"));
				}
			break;			
			case EUptPrintTDesC16_big:
				{
				apiString.Append(_L("UptPrintTDesC16_big"));
				}
			break;
			case EUptPrintTDesC16_huge:
				{
				apiString.Append(_L("UptPrintTDesC16_huge"));
				}
			break;
			case EUptPrintfchar:
				{
				apiString.Append(_L("UptPrintfchar"));
				}
			break;
            case EUptPrintfTRefByValueTDesC8:
                {
                apiString.Append(_L("UptPrintfTRefByValueTDesC8"));
                }
            break;			
			case EUptPrintfTRefByValueTDesC16:
				{
				apiString.Append(_L("UptPrintfTRefByValueTDesC16"));
				}
			break;
				//trace
			case EUptTrace:
				{
				apiString.Append(_L("UptTrace"));
				}
			break;
			case EUptTraceTUint32:
				{
				apiString.Append(_L("UptTraceTUint32"));
				}
			break;
			case EUptTraceTUint32TUint32:
				{
				apiString.Append(_L("UptTraceTUint32TUint32"));
				}
			break;
			case EUptTraceTAnySmallStruct:
				{
				apiString.Append(_L("UptTraceTAnySmallStruct"));
				}
			break;
			case EUptTraceTAnyBigStruct:
				{
				apiString.Append(_L("UptTraceTAnyBigStruct"));
				}
			break;
			case EUptTraceTAnyHugeStruct:
				{
				apiString.Append(_L("UptTraceTAnyHugeStruct"));
				}
			break;

			case EUptTraceTemplatedSmallStruct:
				{
				apiString.Append(_L("UptTraceTemplatedSmallStruct"));
				}
			break;
			case EUptTraceTemplatedBigStruct:
				{
				apiString.Append(_L("UptTraceTemplatedBigStruct"));
				}
			break;
			case EUptTraceTemplatedHugeStruct:
				{
				apiString.Append(_L("UptTraceTemplatedHugeStruct"));
				}
			break;
			case EUptIsTraceActive:
				{
				apiString.Append(_L("UptNonstaticIsTraceActive"));
				}
			break;
			case EUptTraceTDesC8_big:
				{
				apiString.Append(_L("UptTraceTDesC8_big"));
				}
			break;
			case EUptTraceTDesC8_huge:
				{
				apiString.Append(_L("UptTraceTDesC8_huge"));
				}
			break;
			case EUptTraceTDesC16_big:
				{
				apiString.Append(_L("UptTraceTDesC16_big"));
				}
			break;
			case EUptTraceTDesC16_huge:
				{
				apiString.Append(_L("UptTraceTDesC16_huge"));
				}
			break;
				
			//Kernel side
				//printf
			case EUptKernelPrintfchar:
				{
				apiString.Append(_L("UptKernelPrintfchar"));
				}
			break;
			case EUptKernelPrintfDesc8_big:
				{
				apiString.Append(_L("UptKernelPrintfDesc8_big"));
				}
			break;				

			case EUptKernelPrintfDesc8_huge:
				{
				apiString.Append(_L("UptKernelPrintfDesc8_huge"));
				}
			break;				
				//trace
			case EUptKernelTrace:
				{
				apiString.Append(_L("UptKernelTrace"));
				}
			break; 
			case EUptKernelTraceTUint32:
				{
				apiString.Append(_L("UptKernelTraceTUint32"));
				}
			break;
			case EUptKernelTraceTUint32TUint32:
				{
				apiString.Append(_L("UptKernelTraceTUint32TUint32"));
				}
			break;
			case EUptKernelTraceTAnySmallStruct:
				{
				apiString.Append(_L("UptKernelTraceTAnySmallStruct"));
				}
			break;
			case EUptKernelTraceTAnyBigStruct:
				{
				apiString.Append(_L("UptKernelTraceTAnyBigStruct"));
				}
			break;
			case EUptKernelTraceTAnyHugeStruct:
				{
				apiString.Append(_L("UptKernelTraceTAnyHugeStruct"));
				}
			break;
			case EUptKernelTraceTemplatedSmallStruct:
				{
				apiString.Append(_L("UptKernelTraceTemplatedSmallStruct"));
				}
			break;
			case EUptKernelTraceTemplatedBigStruct:
				{
				apiString.Append(_L("UptKernelTraceTemplatedBigStruct"));
				}
			break;
			case EUptKernelTraceTemplatedHugeStruct:
				{
				apiString.Append(_L("UptKernelTraceTemplatedHugeStruct"));
				}
			break;
			case EUptKernelIsTraceActive:
				{
				apiString.Append(_L("UptKernelNonstaticIsTraceActive"));
				}
			break;
				
			//test
				/**
				 * Used to verify that the figures we get out are ok. 
				 * Minimum execution (cached) time roughly: 2938ns
				 * This time should be equivalent to the number produced by
				 * Typical execution (non-cached) time roghly: 2773ns
				 * Stackbase: 400 byte
				 */
			case UPTTraceTest:
				{
				apiString.Append(_L("UPTTraceTest"));
				}
			break;
			case UPTKernelTraceTest:
				{
				apiString.Append(_L("UPTKernelTraceTest"));
				}
			break;
			case UPTKernelNegativeFunctionalityTest:
				{
				apiString.Append(_L("UPTKernelNegativeFunctionalityTest"));
				}
			break;
			case UPTKernelMultipleFunctionalityTest:
				{
				apiString.Append(_L("UPTKernelMultipleFunctionalityTest"));
				}
			break;
			//btrace
			case BTraceFiltered4:
				{
				apiString.Append(_L("BTraceFiltered4"));
				}
			break;
			case BTraceFiltered8:
				{
				apiString.Append(_L("BTraceFiltered8"));
				}
			break;
			case BTraceFiltered12:
				{
				apiString.Append(_L("BTraceFiltered12"));
				}
			break;
			case BTraceFilteredBig:
				{
				apiString.Append(_L("BTraceFilteredBig"));
				}
			break;
			case BTrace0:
				{
				apiString.Append(_L("BTrace0"));
				}
			break;
			case BTrace4:
				{
				apiString.Append(_L("BTrace4"));
				}
			break;
			case BTrace8:
				{
				apiString.Append(_L("BTrace8"));
				}
			break;
			case BTrace12:
				{
				apiString.Append(_L("BTrace12"));
				}
			break;
				
				
				
			//baseline
			case UPTBaseline:
				{
				apiString.Append(_L("UPTBaseline"));
				}
			break;	
			case UPTKernelBaseline:
				{
				apiString.Append(_L("UPTKernelBaseline"));
				}
			break;
			
			case EOstTraceDef0:
                {
                apiString.Append(_L("OstTraceDef0"));
                }
            break;

            case EOstTraceDef1:
                {
                apiString.Append(_L("OstTraceDef1"));
                }
            break;
            
            case EOstTraceDefData:
                {
                apiString.Append(_L("OstTraceDefData"));
                }
            break;
            
            case EOstTraceDefExt1:
                {
                apiString.Append(_L("OstTraceDefExt1"));
                }
            break;
            
            case EOstTraceDefExt2:
                {
                apiString.Append(_L("OstTraceDefExt2"));
                }
            break;
            
            case EOstTraceDefExt3:
                {
                apiString.Append(_L("OstTraceDefExt3"));
                }
            break;
            
            case EOstTraceDefExt4:
                {
                apiString.Append(_L("OstTraceDefExt4"));
                }
            break;
            
            case EOstTraceDefExt5:
                {
                apiString.Append(_L("OstTraceDefExt5"));
                }
            break;
            
            case EOstTrace0:
                {
                apiString.Append(_L("OstTrace0"));
                }
            break;
            
            case EOstTrace1:
                {
                apiString.Append(_L("OstTrace1"));
                }
            break;
            
            case EOstTraceData:
                {
                apiString.Append(_L("OstTraceData"));
                }
            break;
            
            case EOstTraceExt1:
                {
                apiString.Append(_L("OstTraceExt1"));
                }
            break;
            
            case EOstTraceExt2:
                {
                apiString.Append(_L("OstTraceExt2"));
                }
            break;
            
            case EOstTraceExt3:
                {
                apiString.Append(_L("OstTraceExt3"));
                }
            break;
            
            case EOstTraceExt4:
                {
                apiString.Append(_L("OstTraceExt4"));
                }
            break;
            
            case EOstTraceExt5:
                {
                apiString.Append(_L("OstTraceExt5"));
                }
            break;
            
            case EOstTraceFunctionEntry0:
                {
                apiString.Append(_L("OstTraceFunctionEntry0"));
                }
            break;
            
            case EOstTraceFunctionEntry1:
                {
                apiString.Append(_L("OstTraceFunctionEntry1"));
                }
            break;
            
            case EOstTraceFunctionEntryExt:
                {
                apiString.Append(_L("OstTraceFunctionEntryExt"));
                }
            break;
            
            case EOstTraceFunctionExit0:
                {
                apiString.Append(_L("OstTraceFunctionExit0"));
                }
            break;
            
            case EOstTraceFunctionExit1:
                {
                apiString.Append(_L("OstTraceFunctionExit1"));
                }
            break;
            
            case EOstTraceFunctionExitExt:
                {
                apiString.Append(_L("OstTraceFunctionExitExt"));
                }
            break;
            
            case EOstTraceEventStart0:
                {
                apiString.Append(_L("OstTraceEventStart0"));
                }
            break;
            
            case EOstTraceEventStart1:
                {
                apiString.Append(_L("OstTraceEventStart1"));
                }
            break;
            
            case EOstTraceEventStop:
                {
                apiString.Append(_L("OstTraceEventStop"));
                }
            break;          
            
            case EOstTraceState0:
                {
                apiString.Append(_L("OstTraceState0"));
                }
            break;
            
            case EOstTraceState1:
                {
                apiString.Append(_L("OstTraceState1"));
                }
            break;
            
            case EOstTraceData96:
                {
                apiString.Append(_L("OstTraceData96"));
                }
            break;
            
            case EOstTraceData1000:
                {
                apiString.Append(_L("OstTraceData1000"));
                }
            break;
            
			default:
				{
				apiString.Append(_L("N/A"));
				}
			}
//		TPtrC8 returnString(apiString);
//		returnString.Set(apiString);
//		returnString.Set(apiString, 0);
		
//		apiString.Close();
//		CleanupStack::PopAndDestroy(&apiString);
//		return returnString;
		
		return apiString;
	}
//EOF


