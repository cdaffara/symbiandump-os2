#include "te_configfile.h"

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
// Example CWrapper derived implementation
//



/**
 @file te_configfile.cpp
 @internalTechnology
*/
#include <uloggerclient.h>
#include <opensystemtrace.h>
#include "te_configfile.h"
#include "te_ostv2integsuite_defs.h"
#include "te_dataparameters.h"

using namespace Ulogger; //CA:added so can use old ulogger api
/*******************************************************************************/
/*********************************Public File Test***************************/
/*******************************************************************************/

TInt CPublicConfigFileWrapper::aTestCount=0;

CPublicConfigFileWrapper::~CPublicConfigFileWrapper()
/**
 * Destructor
 */
	{
	}

CPublicConfigFileWrapper::CPublicConfigFileWrapper()
/**
 * Constructor
 */
	{
	aTestCount++;
	}


CPublicConfigFileWrapper* CPublicConfigFileWrapper::NewLC()
	{
	CPublicConfigFileWrapper* self = new (ELeave)CPublicConfigFileWrapper();
	CleanupStack::PushL(self);
	self->ConstructL();
	return self;
	}

CPublicConfigFileWrapper* CPublicConfigFileWrapper::NewL()
	{
	CPublicConfigFileWrapper* self=CPublicConfigFileWrapper::NewLC();
	CleanupStack::Pop(); // self;
	return self;
	}

void CPublicConfigFileWrapper::ConstructL()
	{
	}

TAny* CPublicConfigFileWrapper::GetObject()
	{
	return NULL;
	}

TBool CPublicConfigFileWrapper::DoCommandL(	const TTEFFunction& /*aCommand*/,
					const TTEFSectionName& /*aSection*/, 
					const TInt /*aAsyncErrorIndex*/)
	{
	 if (BlockResult()==EPass)
		 {
		//  ************** Delete the Block, the block start ****************
		TInt Result=0;
		RULogger logger;
		Result=logger.Connect();
		INFO_PRINTF2(_L("connect err %d"), Result);

		/**************First get primary filter*************/

		CArrayFixFlat<TUint8> *getprimfilter = new (ELeave)CArrayFixFlat<TUint8>(1);
		logger.GetPrimaryFiltersEnabled(*getprimfilter);//C.A. previously:logger.GetEnabledClassifications(*getprimfilter);
		INFO_PRINTF2(_L("getprimfilter->Count() %d"), getprimfilter->Count());
		if(getprimfilter->At(0)==(TGroupId)255&&getprimfilter->Count()==1)
			{
			INFO_PRINTF1(_L("Get primary filter successful, correctly picked up from public file with correct count"));
			SetBlockResult(EPass);
			}
		else
			{
			INFO_PRINTF1(_L("Get primary filter failed, returned filter(s):"));
			for(TInt i=0;i!=getprimfilter->Count();i++)
				{
				INFO_PRINTF2(_L("%d "),getprimfilter->At(i));
				}
			SetBlockResult(EFail);
			}
		/**************Now get secondary filter, if this has been successful*************/

		if(BlockResult()==EPass)
		{
			RArray<TUint32> getsecondfilter;
			logger.GetSecondaryFiltersEnabled(getsecondfilter);//C.A. previously:logger.GetEnabledModuleUids(getsecondfilter);
			if(getsecondfilter[0]==(TComponentId)333&&getsecondfilter.Count()==1)
			{
				INFO_PRINTF1(_L("Get Secondary filter successful, correctly picked up from public file with correct count"));
				SetBlockResult(EPass);

			}
			else
			{
				INFO_PRINTF1(_L("Get Secondary filter failed, returned filter(s):"));
				for(TInt i=0;i!=getsecondfilter.Count();i++)
				{
					INFO_PRINTF2(_L("%d "),getsecondfilter[i]);
				}
				SetBlockResult(EFail);
			}
			/**************Now do some tracing, if this has been successful*************/

			if(BlockResult()==EPass)
			{
				logger.Start();//C.A. previously:logger.StartOutputting();

				TTraceContext attrs((TComponentId)333, (TGroupId)255, KContextId, KPc);
				Result = OstTrace(attrs, KInitialClientFormat, KAny16);
				if(Result==1)
					{
					INFO_PRINTF2(_L("Tracing successful, check outputlog PublicConfigULogger%d.utf has first trace statement, primary filter=255 and secondary filter=333"), aTestCount);
					//if standard tracing has passed - try setting with a single GroupId
					RArray<TUint32> setfilterarray; //C.A. added so can use old ulogger api
					setfilterarray.Append(KDefaultComponentId); //C.A. added so can use old ulogger api
					Result = logger.SetSecondaryFiltersEnabled(setfilterarray,ETrue);//C.A. previously:Result = logger.EnableModuleUid(KDefaultComponentId);
					Result = logger.Restart();//C.A. previously:Result = logger.RestartOutputting();
					TTraceContext attrs1((TGroupId)255, KContextId, KPc);
					Result = OstTrace(attrs1, KInitialClientFormat, KAny16);
					logger.SetSecondaryFiltersEnabled(setfilterarray,EFalse);//C.A. previously:logger.DisableModuleUid(KDefaultComponentId);
					setfilterarray.Reset();//C.A. close array
					}
				logger.Stop();//C.A. previously:logger.StopOutputting();
				if(Result==1)
				{
					INFO_PRINTF2(_L("Tracing successful, check outputlog PublicConfigULogger%d.utf has second trace statement, primary filter=255 and secondary filter=UID3"), aTestCount);
					SetBlockResult(EPass);
				}
				else
				{
					INFO_PRINTF2(_L("Tracing failed, with error return %d"), Result);
					SetBlockResult(EFail);
				}
			}

		}
		//  **************   Block end ****************
		if(getprimfilter)
			delete getprimfilter;
		logger.Close();
	}
	 return ETrue;
	}





/*******************************************************************************/
/*********************************Private File Test***************************/
/*******************************************************************************/


CPrivateConfigFileWrapper::~CPrivateConfigFileWrapper()
/**
 * Destructor
 */
	{
	}

CPrivateConfigFileWrapper::CPrivateConfigFileWrapper()
/**
 * Constructor
 */
	{
	}


CPrivateConfigFileWrapper* CPrivateConfigFileWrapper::NewLC()
	{
	CPrivateConfigFileWrapper* self = new (ELeave)CPrivateConfigFileWrapper();
	CleanupStack::PushL(self);
	self->ConstructL();
	return self;
	}

CPrivateConfigFileWrapper* CPrivateConfigFileWrapper::NewL()
	{
	CPrivateConfigFileWrapper* self=CPrivateConfigFileWrapper::NewLC();
	CleanupStack::Pop(); // self;
	return self;
	}

void CPrivateConfigFileWrapper::ConstructL()
	{
	}

TAny* CPrivateConfigFileWrapper::GetObject()
	{
	return NULL;
	}
TBool CPrivateConfigFileWrapper::DoCommandL(	const TTEFFunction& /*aCommand*/,
					const TTEFSectionName& /*aSection*/, 
					const TInt /*aAsyncErrorIndex*/)
	{
	if (BlockResult()==EPass)
	{
		//  ************** Delete the Block, the block start ****************
		TInt Result=0;
		RULogger logger;
		Result=logger.Connect();
		INFO_PRINTF2(_L("connect err %d"), Result);
		CClearConfig configIni;
		configIni.DefaultL(logger);

		/**************First get primary filter*************/

		CArrayFixFlat<TUint8> *getprimfilter = new (ELeave)CArrayFixFlat<TUint8>(1);
		logger.GetPrimaryFiltersEnabled(*getprimfilter);//C.A. previously:logger.GetEnabledClassifications(*getprimfilter);
		if(getprimfilter->Count()>0)
			{
			//if((getprimfilter->At(0)==3||getprimfilter->At(1)==3)&&(getprimfilter->At(1)==13||getprimfilter->At(1)==13)&&getprimfilter->Count()==2)
			/*NB: The above line should be reintroduced for ulogger v2 when the default ulogger config file
			      is updated again to include the metadata default value for the SAW viewer */			
			if(getprimfilter->At(0)==(TGroupId)3&&getprimfilter->Count()==1)	
				{
					INFO_PRINTF1(_L("Get primary filter successful, correctly picked up from private file with correct count"));
					SetBlockResult(EPass);
				}
				else
				{
					INFO_PRINTF1(_L("Get primary filter failed (default uloggerconfig.ini may have changed), returned filter(s):"));
					for(TInt i=0;i!=getprimfilter->Count();i++)
						{
						INFO_PRINTF2(_L("%d "),getprimfilter->At(i));
						}
					SetBlockResult(EFail);
				}
			}
		else
			{
			INFO_PRINTF1(_L("Get primary filter failed, returns a zero count array, the default filters should be returned"));
			SetBlockResult(EFail);
			}



		/**************Now get secondary filter, if this has been successful*************/

		if(BlockResult()==EPass)
		{
			RArray<TUint32> getsecondfilter;
			Result=logger.GetSecondaryFiltersEnabled(getsecondfilter);//C.A. previously:Result=logger.GetEnabledModuleUids(getsecondfilter);
			if(Result==KErrNone&&getsecondfilter.Count()==0) //i.e. return empty array
			{
				INFO_PRINTF1(_L("Get Secondary filter successful, correctly picked up that there were no secondary filters from private config file"));
				SetBlockResult(EPass);

			}
			else
			{
				INFO_PRINTF2(_L("Get Secondary filter failed and returned error %d, returned filter(s):"), Result);
				for(TInt i=0;i!=getsecondfilter.Count();i++)
				{
					INFO_PRINTF2(_L("%d "),getsecondfilter[i]);
				}
				SetBlockResult(EFail);
			}
			/**************Now do some tracing, if this has been successful*************/

			if(BlockResult()==EPass)
			{
				RArray<TUint32> setsecondfilter;//C.A.added so can use old ulogger api
				setsecondfilter.Append(KDefaultComponentId);//C.A.added so can use old ulogger api
				//need to set the ComponentId filter in order for the trace to be carried out
				//this is achieved as TTraceContext with set the ComponentId by default for the user.
				//if you want to investigate true fast, GroupId only, tracing - use BTrace
				Result = logger.SetSecondaryFiltersEnabled(setsecondfilter,ETrue);//C.A. previously:Result = logger.EnableModuleUid(KDefaultComponentId);
				Result = logger.Start();//C.A. previously:Result = logger.StartOutputting();
				TTraceContext attrs((TGroupId)3, KContextId, KPc);
				Result = OstTrace(attrs, KInitialClientFormat, KAny16);

				if(Result==1)
				{
					INFO_PRINTF1(_L("Tracing successful, check outputlog PrivateConfigULogger.utf has trace statements only of order primary filter=3"));
				}
				else
				{
					INFO_PRINTF2(_L("Tracing failed, with error return %d"), Result);
					SetBlockResult(EFail);
				}
				//reset back to the default scenario
				Result = logger.SetSecondaryFiltersEnabled(setsecondfilter,EFalse);//C.A. previously:Result = logger.DisableModuleUid(KDefaultComponentId);
				Result = logger.Stop();//C.A. previously:Result = logger.StopOutputting();
				setsecondfilter.Reset();
			}

		}
		//  **************   Block end ****************


		if(getprimfilter)
			delete getprimfilter;
		logger.Close();
	}
	  return ETrue;
	}



