/*
* Copyright (c) 2005-2009 Nokia Corporation and/or its subsidiary(-ies).
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



#include "T_EComSessionData.h"
#include "T_ImplementationInformationData.h"
#include "TestEComInterface.h"

/*@{*/
///	Parameters
_LIT(KUid,							"uid");
_LIT(KResolver,						"resolver");
_LIT(KExpectedResult,				"expected");
_LIT(KResolverUid,					"resolver_uid");
_LIT(KOffset,						"offset");
_LIT(KInteger,						"integer");
_LIT(KDescriptor,					"descriptor");
_LIT(KEmpty,						"empty");
_LIT(KSaveTo,						"saveto");
_LIT(KSaveIndex,					"saveindex");

///	REComSession
_LIT(KCmdOpenL,		 				"OpenL");
_LIT(KCmdClose,					 	"Close");
_LIT(KCmdFinalClose,				"FinalClose");
_LIT(KCmdNotifyOnChange,			"NotifyOnChange");
_LIT(KCmdCancelNotifyOnChange,		"CancelNotifyOnChange");
_LIT(KCmdListImplementationsL,		"ListImplementationsL");
_LIT(KCmdCreateImplementationL,		"CreateImplementationL");
_LIT(KCmdDestroyedImplementation,	"DestroyedImplementation");
_LIT(KCmdClean,						"~");

_LIT(KEmptyString,					"");
_LIT(KDefault,						"default");
/*@}*/

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

/**
 * Two phase constructor
 */
CT_EComSessionData* CT_EComSessionData::NewL()
	{
	CT_EComSessionData*	ret=new (ELeave) CT_EComSessionData();
	CleanupStack::PushL(ret);
	ret->ConstructL();
	CleanupStack::Pop(ret);
	return ret;
	}

/**
 * Constructor. First phase construction
 */
CT_EComSessionData::CT_EComSessionData()
:	iActiveNotifyOnChange(NULL)
,	iImplementationInterface(NULL)
	{
	iInitParams.iInteger=0;
	iInitParams.iDescriptor=&iInterfaceDesc;
	}

/**
 * Second phase construction
 */
void CT_EComSessionData::ConstructL()
	{
	iInterfaceDesc=KEmptyString;
	iDTorKey=TUid::Uid(0);
	iActiveNotifyOnChange=CActiveCallback::NewL(*this);
	}

/**
 * Public destructor
 */
CT_EComSessionData::~CT_EComSessionData()
	{
	DestroyData();
	iInfoArray.ResetAndDestroy();
	iInfoArray.Close();
	delete iActiveNotifyOnChange;
	iActiveNotifyOnChange=NULL;
	}

TAny* CT_EComSessionData::GetObject()
/**
 * Return a pointer to the object that the data wraps
 *
 * @return	Pointer to the object that the data wraps
 */
	{
	return &iSession;
	}

void CT_EComSessionData::DestroyData()
	{
	if (iImplementationInterface)
		{
		delete static_cast<CBase*>(iImplementationInterface);
		iImplementationInterface=NULL;
		};
	}

//////////////////////////////////////////////////////////////////////
// Read data from INI file
//////////////////////////////////////////////////////////////////////

void CT_EComSessionData::GetUid(const TDesC& aSection, TUid& aUid)
	{
	TInt	uidValue=0;

	GetHexFromConfig(aSection, KUid(), uidValue );
	aUid = TUid::Uid(uidValue);
	}

TBool CT_EComSessionData::GetResolver(const TDesC& aSection, TPtrC& aResolver)
	{
	return GetStringFromConfig(aSection, KResolver(), aResolver);
	}

TBool CT_EComSessionData::GetResolverUid(const TDesC& aSection, TUid& aUid)
	{
	TInt	uidValue=0;
	TBool	ret=GetHexFromConfig(aSection, KResolverUid(), uidValue );
	aUid = TUid::Uid(uidValue);
	return ret;
	}

TBool CT_EComSessionData::GetEmpty(const TDesC& aSection, TBool& aEmpty)
	{
	aEmpty = EFalse;
	return GetBoolFromConfig(aSection, KEmpty(), aEmpty );
	}

TBool CT_EComSessionData::GetExpectedResult(const TDesC& aSection, TInt& aExpectedResult)
	{
	aExpectedResult = 0;
	return GetIntFromConfig(aSection, KExpectedResult(), aExpectedResult );
	}

TBool CT_EComSessionData::GetOffset(const TDesC& aSection, TInt& aOffset)
	{
	aOffset=_FOFF(CTestEComInterface, iDtor_ID_Key);

	TPtrC	offset;
	TBool	ret=GetStringFromConfig(aSection, KOffset(), offset);
	if ( ret )
		{
		if ( offset == KDefault )
			{
			INFO_PRINTF2(_L("Default offset %d"), aOffset);
			}
		else
			{
			ret=GetIntFromConfig(aSection, KOffset(), aOffset);
			}
		}

	return ret;
	}

TBool CT_EComSessionData::GetDescriptor(const TDesC& aSection, TPtrC& aDescriptor)
	{
	aDescriptor.Set(KEmptyString);
	return GetStringFromConfig(aSection, KDescriptor(), aDescriptor );
	}

TBool CT_EComSessionData::GetInteger(const TDesC& aSection, TInt& aInt)
	{
	return GetIntFromConfig(aSection, KInteger(), aInt );
	}


TBool CT_EComSessionData::DoCommandL(const TTEFFunction& aCommand, const TTEFSectionName& aSection, const TInt aAsyncErrorIndex)
/**
 * Process a command read from the ini file
 *
 * @param aCommand			The command to process
 * @param aSection			The section in the ini containing data for the command
 * @param aAsyncErrorIndex	Command index for async calls to return errors to
 *
 * @return					ETrue if the command is processed
 * @pre						N/A
 * @post					N/A
 *
 * @leave					System wide error
 */
	{
	TBool	retVal=ETrue;

	if ( aCommand==KCmdOpenL )
		{
		DoCmdOpenL();
		}
	else if ( aCommand==KCmdClose )
		{
		DoCmdClose();
		}
	else if ( aCommand==KCmdFinalClose )
		{
		DoCmdFinalClose();
		}
	else if ( aCommand==KCmdNotifyOnChange )
		{
		DoCmdNotifyOnChange(aAsyncErrorIndex);
		}
	else if ( aCommand==KCmdCancelNotifyOnChange )
		{
		DoCmdCancelNotifyOnChange();
		}
	else if ( aCommand==KCmdListImplementationsL )
		{
		DoCmdListImplementationsL(aSection);
		}
	else if ( aCommand==KCmdCreateImplementationL )
		{
		DoCmdCreateImplementationL(aSection);
		}
	else if ( aCommand==KCmdDestroyedImplementation )
		{
		DoCmdDestroyedImplementation(aSection);
		}
	else if ( aCommand==KCmdClean )
		{
		DestroyData();
		}
	else
		{
		retVal=EFalse;
		}
	return retVal;
	}

void CT_EComSessionData::DoCmdOpenL()
	{
	INFO_PRINTF1(_L("OpenL Call"));
	TRAPD ( err, iSession = REComSession::OpenL() );
	if ( err!=KErrNone )
		{
		ERR_PRINTF2(_L("OpenL Error %d"), err);
		SetError(err);
		}
	else
		{
		INFO_PRINTF1(_L("Session opened"));
		}
	}

void CT_EComSessionData::DoCmdClose()
	{
	INFO_PRINTF1(_L("Close"));
	iSession.Close();
	}

void CT_EComSessionData::DoCmdFinalClose()
	{
	INFO_PRINTF1(_L("Final Close"));
	REComSession::FinalClose();
	}

void CT_EComSessionData::DoCmdNotifyOnChange(const TInt aIndex)
	{
	INFO_PRINTF1(_L("NotifyOnChange"));
	iSession.NotifyOnChange(iActiveNotifyOnChange->iStatus);
	iActiveNotifyOnChange->Activate(aIndex);
   	IncOutstanding();
	}

void CT_EComSessionData::DoCmdCancelNotifyOnChange()
	{
	INFO_PRINTF1(_L("CancelNotifyOnChange"));
	iSession.CancelNotifyOnChange(iActiveNotifyOnChange->iStatus);
	}

// Found defects:
// 1. ListImplementationsL with impossible aResolutionParameters argument don't leave
//    Status: possible defect
void CT_EComSessionData::DoCmdListImplementationsL(const TDesC& aSection)
	{
	INFO_PRINTF1(_L("Getting Implementnation List"));

	TUid	impUid;
	GetUid(aSection, impUid);

	TUid	resolverUid;
	TBool	hasResolverUid=GetResolverUid(aSection, resolverUid);

	TInt	err = KErrNone;

	TPtrC	resolver;
	TBool	hasResolver=GetResolver(aSection, resolver);
	if ( hasResolver )
		{
		TEComResolverParams*	resolverParams=static_cast<TEComResolverParams*>(GetDataObjectL(resolver));
		if ( hasResolverUid )
			{
			INFO_PRINTF1(_L("Getting Implementnation List(3) by Uid, ResolverParams as a filter params && Resolver Uid as a filter"));
			TRAP(err, REComSession::ListImplementationsL(impUid, *resolverParams, resolverUid, iInfoArray));
			}
		else
			{
			INFO_PRINTF1(_L("Getting Implementnation List(1) by Uid && ResolverParams as a filter"));
			TRAP(err, REComSession::ListImplementationsL(impUid, *resolverParams, iInfoArray));
			}
		}
	else
		{
		INFO_PRINTF1(_L("Getting Implementnation List(2) by Uid"));
		TRAP(err, REComSession::ListImplementationsL(impUid, iInfoArray));
		}

	if ( err==KErrNone )
		{
		//	Check to see if we the data object has to be set in a CT_ImplementationInformationData
		TPtrC	saveTo;
		if ( GetStringFromConfig(aSection, KSaveTo(), saveTo) )
			{
			TInt	index=0;
			GetIntFromConfig(aSection, KSaveIndex(), index);
			CDataWrapper*	wrapper=static_cast<CDataWrapper*>(GetDataWrapperL(saveTo));
			wrapper->SetObjectL(iInfoArray[index]);
			}
		}

	if( err != KErrNone )
		{
		ERR_PRINTF2(_L("Failed to get implementation list with error %d"), err);
		SetError(err);
		}
	else
		{
		TInt	expectedResult;
		if ( GetExpectedResult(aSection, expectedResult) )
			{
			if (iInfoArray.Count() != expectedResult)
				{
				ERR_PRINTF3(_L("List retrieved. Found %d imlementations, expected %d"),
				iInfoArray.Count(), expectedResult );
				SetBlockResult(EFail);
				}
			}

		for (TInt i = 0; i < iInfoArray.Count(); i++)
			{
			const TDesC8&	name8=iInfoArray[i]->DataType();
			TBuf<KMaxTestExecuteCommandLength>	name16;
			name16.Copy(name8);
			INFO_PRINTF3(_L("Implementation UID %d: - \"%S\""), iInfoArray[i]->ImplementationUid().iUid, &(iInfoArray[i]->DisplayName()));
			INFO_PRINTF2(_L("Data Type is \"%S\""), &name16);
			};
		}
	}

// Found defects:
// 1. CreateImplementationL with any aKeyOffset argument - aKeyOffset can have any value (-1, 0, 10000, ...)
//    but CreateImplementationL returns no error and generates memory leak.
//    Status: possible defects
// 2. CreateImplementationL with impossible aResolutionParameters argument - returns KErrnotFound, but in documentation
//    CreateImplementationL levaes only with KErrNoMemory, KErrNotConnected, KErrArgument or KErrPermissionDenied error
//    Status: documentation  defect
void CT_EComSessionData::DoCmdCreateImplementationL(const TDesC& aSection)
	{
	DestroyData();

	TUid	impUid;
	GetUid(aSection,impUid);

	TInt	offset;
	TBool	hasOffset=GetOffset(aSection,offset);

	TUid	resolverUid;
	TBool	hasResolverUid=GetResolverUid(aSection,	resolverUid);

	TBool	hasIntegerValue=GetInteger(aSection, iInitParams.iInteger);

	TBool	emptyArgs;
	TBool	hasEmptyArgs=GetEmpty(aSection, emptyArgs);

	TBool	hasInitParams=hasIntegerValue || hasEmptyArgs;
	TAny*	initParamsPtr=NULL;
	if (hasInitParams)
		{
		if (!emptyArgs)
			{
			initParamsPtr=&iInitParams;
			}
		}

	TInt	err = KErrNone;
	TBool	getKeyFromOffset=EFalse;

	TPtrC	resolver;
	if ( GetResolver(aSection, resolver) )
		{
		TEComResolverParams*	resolverParams=static_cast<TEComResolverParams*>(GetDataObjectL(resolver));
		if (hasResolverUid && hasInitParams && hasOffset)
			{
			INFO_PRINTF1(_L("Creating Implementnation(12) by Interface Uid,  Offset, Init Params, ResolverParams as a filter params && Resolver Uid as a filter"));
		
			TRAP( err, iImplementationInterface =
				REComSession::CreateImplementationL(impUid, offset, initParamsPtr, *resolverParams, resolverUid));
			getKeyFromOffset=(err==KErrNone);
			}
		else if (hasResolverUid && hasInitParams)
			{
			INFO_PRINTF1(_L("Creating Implementnation(11) by Interface Uid,  Init Params, ResolverParams as a filter params && Resolver Uid as a filter"));
			TRAP( err, iImplementationInterface =
				REComSession::CreateImplementationL(impUid, iDTorKey, initParamsPtr, *resolverParams, resolverUid));
			}
		else if (hasResolverUid && hasOffset)
			{
			INFO_PRINTF1(_L("Creating Implementnation(10) by Interface Uid, Offset, ResolverParams as a filter params && Resolver Uid as a filter"));
			TRAP( err, iImplementationInterface =
				REComSession::CreateImplementationL(impUid, offset, *resolverParams, resolverUid));
			getKeyFromOffset=(err==KErrNone);
			}
		else if (hasResolverUid)
			{
			INFO_PRINTF1(_L("Creating Implementnation(9) by Interface Uid, ResolverParams as a filter params && Resolver Uid as a filter"));
			TRAP( err, iImplementationInterface =
				REComSession::CreateImplementationL(impUid, iDTorKey, *resolverParams, resolverUid));
			}
		else if (hasInitParams && hasOffset)
			{
			INFO_PRINTF1(_L("Creating Implementnation(8) by Interface Uid, Offset, Init Params && Resolver Parameters as a filter"));
			TRAP( err, iImplementationInterface =
				REComSession::CreateImplementationL(impUid, offset, initParamsPtr, *resolverParams));

			getKeyFromOffset=(err==KErrNone);
			}
		else if (hasInitParams)
			{
			INFO_PRINTF1(_L("Creating Implementnation(7) by Interface Uid, Init Params && Resolver Parameters as a filter"));
			TRAP( err, iImplementationInterface =
				REComSession::CreateImplementationL(impUid, iDTorKey, initParamsPtr, *resolverParams));
			}
		else if (hasOffset)
			{
			INFO_PRINTF1(_L("Creating Implementnation(6) by Interface Uid, Offset && Resolver Parameters as a filter"));

			TRAP( err, iImplementationInterface =
				REComSession::CreateImplementationL(impUid, offset, *resolverParams));

			getKeyFromOffset=(err==KErrNone);
			}
		else
			{
			INFO_PRINTF1(_L("Creating Implementnation(5) by Interface Uid && Resolver Parameters as a filter"));

			TRAP( err, iImplementationInterface =
				REComSession::CreateImplementationL(impUid, iDTorKey, *resolverParams));

			}
		}
	else
		{
		if (hasInitParams && hasOffset)
			{
			INFO_PRINTF1(_L("Creating Implementnation(4) by Uid, Offset && Construction Parameters"));

			TRAP( err, iImplementationInterface =
				REComSession::CreateImplementationL(impUid, offset, initParamsPtr));

			getKeyFromOffset=(err==KErrNone);
			}
		else if (hasInitParams)
			{
			INFO_PRINTF1(_L("Creating Implementnation(3) by Uid && Construction Parameters"));

			TRAP( err, iImplementationInterface =
				REComSession::CreateImplementationL(impUid, iDTorKey, initParamsPtr));

			}
		else if (hasOffset)
			{
			INFO_PRINTF1(_L("Creating Implementnation(2) by Uid && offset defined by interface"));

			TRAP( err, iImplementationInterface =
				REComSession::CreateImplementationL(impUid, offset));

			getKeyFromOffset=(err==KErrNone);
			}
		else
			{
			INFO_PRINTF1(_L("Creating Implementnation(1) by Uid"));

			TRAP( err, iImplementationInterface =
				REComSession::CreateImplementationL(impUid, iDTorKey ));

			}
		}

	if ( getKeyFromOffset )
		{
		TUint8*	uidPtr=REINTERPRET_CAST(TUint8*, iImplementationInterface) + offset;
		TUid*	uid=REINTERPRET_CAST(TUid*, uidPtr);
		iDTorKey=*uid;
		*uid=TUid::Null();
		}

	INFO_PRINTF2(_L("Implementation created successfully. iDTorKey is %d"), iDTorKey.iUid);
	if (BlockResult()==EPass)
		{
		if( err != KErrNone )
			{
			ERR_PRINTF2(_L("Failed to create implementation with error %d"), err);
			SetError(err);
			if (iImplementationInterface != NULL)
				{
				SetBlockResult(EFail);
				ERR_PRINTF1(_L("Data verification failed: CreateImplementationL returns an error, but implementation was created"));
				}
			else
				{
				INFO_PRINTF1(_L("Data verification succcess: CreateImplementationL returns an error and implementation was not created as expected"));
				}
			}
		}
	}

void CT_EComSessionData::DoCmdDestroyedImplementation(const TDesC& aSection)
	{
	TInt	uidValue;
	if ( GetIntFromConfig(aSection, KUid(), uidValue) )
		{
		iDTorKey = TUid::Uid(uidValue);
		}

	INFO_PRINTF2(_L("Destroy Interface Implementation. UID is %d"), iDTorKey.iUid);
	REComSession::DestroyedImplementation(iDTorKey);
	INFO_PRINTF1(_L("REComSession::DestroyedImplementation(iDTorKey) completed"));
	DestroyData();
	INFO_PRINTF1(_L("Interface implementation destroyed"));
	}

/**
 Virtual RunL - Called on completion of an asynchronous command
 @internalComponent
 @see MTPActiveCallback
 @param aActive Active Object that RunL has been called on
 @pre N/A
 @post N/A
 @leave system wide error code
*/
void CT_EComSessionData::RunL(CActive* aActive, const TInt aIndex)
	{
	if ( aActive==iActiveNotifyOnChange )
		{
		RunNotifyOnChangeL( aIndex );
		}
	else
		{
		ERR_PRINTF1(_L("Stray signal"));
		SetBlockResult(EFail);
		}
	}

/**
 RunNotifyOnChangeL - Process RunL for iActiveNotifyOnChange
 @internalComponent
 @pre N/A
 @post N/A
 @leave system wide error code
*/
void CT_EComSessionData::RunNotifyOnChangeL(const TInt aIndex)
	{
	TInt	err=iActiveNotifyOnChange->iStatus.Int();
	if ( err!=KErrNone )
		{
		ERR_PRINTF2(_L("RunL Error %d"), err);
		SetAsyncError(aIndex, err);
		}
	else
		{
		INFO_PRINTF1(_L("RunNotifyOnChangeL OK"));
		}

   	DecOutstanding();
	}

