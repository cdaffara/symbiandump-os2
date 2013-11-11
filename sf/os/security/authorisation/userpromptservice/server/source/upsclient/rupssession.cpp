/*
* Copyright (c) 2007-2010 Nokia Corporation and/or its subsidiary(-ies).
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
* RUpsSession implementation.	 See class and function definitions
* for more detail.
*
*/


/**
 @file
*/


#include <ups/upsclient.h>
#include <e32property.h>
#include <e32debug.h>
#include "upscommon.h"
#include "upsclientconfig.h"

namespace UserPromptService
	{

NONSHARABLE_CLASS(CPolicyChangeWatcher) : public CActive
	{
public:
	static CPolicyChangeWatcher *NewL(RUpsSession &aUpsSession);
	~CPolicyChangeWatcher();
private:
	CPolicyChangeWatcher(RUpsSession &aUpsSession);
	void ConstructL();
	virtual void DoCancel();
	virtual void RunL();
	virtual TInt RunError(TInt aError);

private:
	RProperty iUpsPolicyChangeProperty;
	RUpsSession &iUpsSession;
	};

EXPORT_C RUpsSession::RUpsSession()
/**
	This constructor provides a single point of definition from
	which the superclass constructor is called.
 */
:	RScsClientBase(),
	iPolicyChangeWatcher(0),
	iClientConfig(0)
	{
	// empty.
	}

EXPORT_C TInt RUpsSession::Connect()
/**
	Connect to the UPS server.

	The thread which calls this function must be the same one which later calls Close().

	Once connected, this session can be shared by multiple RUpsSubsession objects.

	The RUpsSubsession objects are allowed to be in different threads, in which case ShareAuto() must be called before they are created.

	@return					Symbian OS error code where KErrNone indicates
							success and any other value indicates failure.
 */
	{
	TVersion v = UserPromptService::Version();
	TUidType serverFullUid = UserPromptService::ServerImageFullUid();

	TInt r = RScsClientBase::Connect(
		UserPromptService::KUpsServerName, v, UserPromptService::KServerImageName, serverFullUid);
	
	if (r == KErrNone)	
		{
		// preload the configuration
		TRAP(r, RetrieveClientConfigL());
		}

	if((r == KErrNone) && (CActiveScheduler::Current() != 0))
		{
		delete iPolicyChangeWatcher;
		iPolicyChangeWatcher = 0;
		TRAP(r, iPolicyChangeWatcher = CPolicyChangeWatcher::NewL(*this));
		}

	if(r != KErrNone)
		{
		Close();
		}
	
	return r;
	}
	

static void deleteArrayOfTServiceConfig(TAny *aPtr)
	{
	TServiceConfig *array = reinterpret_cast<TServiceConfig *>(aPtr);
	delete [] array;
	}

void RUpsSession::RetrieveClientConfigL()
	{
	// Query how many TServiceConfig entries there are.
	TPckgBuf<TInt> countBuf;
	User::LeaveIfError(CallSessionFunction(EGetClientConfigLength, TIpcArgs(&countBuf)));
	// Retrieve count from buffer
	TInt count = countBuf();
	
	// Create buffer to hold entries
	TServiceConfig *rawServiceConfig = new(ELeave) TServiceConfig[count];
	CleanupStack::PushL(TCleanupItem(deleteArrayOfTServiceConfig, rawServiceConfig));

	// Wrap array in a TPtr8 and use it to read array from server
	TInt size = count * sizeof(TServiceConfig);
	TPtr8 ptr((TUint8*)rawServiceConfig, size, size);
	User::LeaveIfError(CallSessionFunction(EGetClientConfigData, TIpcArgs(&ptr)));

	CUpsClientConfig *tmp = CUpsClientConfig::NewL(count, rawServiceConfig);
	delete iClientConfig;
	iClientConfig = tmp;

	CleanupStack::PopAndDestroy(); // get rid of rawServiceConfig array
	}
	

EXPORT_C void RUpsSession::Close()
/**
Frees the configuration object and calls RScsClientBase::Close

This function MUST be called by the same thread which Connect().
*/
	{
	delete iClientConfig;
	iClientConfig = 0;

	if(iPolicyChangeWatcher)
		{
		iPolicyChangeWatcher->Cancel();
		}
	delete iPolicyChangeWatcher;
	iPolicyChangeWatcher = 0;
	
	RScsClientBase::Close();
	}

void RUpsSession::NotifyPolicyFileChangedL()
/**
	@internalComponent
	@released

	Policy files have changed so update serviceconfig cache.
*/
	{
	RetrieveClientConfigL();
	}

CUpsClientConfig::CUpsClientConfig()
	: iServiceConfig(1 /* granularity */, 0 /* key offset */)
	{
	}
	
CUpsClientConfig::~CUpsClientConfig()
	{
	iServiceConfig.Close();
	}
	
CUpsClientConfig* CUpsClientConfig::NewL(TInt aCount, TServiceConfig *aRawServiceConfig)
	/**
	   @param serviceConfig

	   Takes ownership of the serviceConfig array via swap. Callers array will be cleared.
	*/
	{
	CUpsClientConfig *self = new(ELeave) CUpsClientConfig();
	CleanupStack::PushL(self);
	self->ConstructL(aCount, aRawServiceConfig);
	CleanupStack::Pop(self);
	return self;
	}

	
void CUpsClientConfig::ConstructL(TInt aCount, TServiceConfig *aRawServiceConfig)
	{
	iServiceConfig.ReserveL(aCount);
	for(TInt i=0; i < aCount; ++i)
		{
		iServiceConfig.InsertInUnsignedKeyOrderL(aRawServiceConfig[i]);
		}
	}

CUpsClientConfig::TQueryUpsResult
CUpsClientConfig::QueryUps(TBool aServerChecksPassed,
							  const TServiceId &aServiceId, 
							  const TSecureId &aClientSid,
							  const TProcessId &aClientProcessId) const
	{
	TServiceConfig sc = {0};
	sc.iServiceId = aServiceId.iUid; /* Only service id is used in lookup */
	TInt i = iServiceConfig.FindInUnsignedKeyOrder(sc);
	if(i == KErrNotFound)
		{
		// Must be no policy file for this service so no point in querying UPS
		return aServerChecksPassed ? EAllow: EReject;
		}
	
	
	// Read entry
	sc = iServiceConfig[i];

	switch(sc.iPolicy)
		{
		case EAlwaysCheck:
			return EQueryUps;
			
		case ECheckPostManufacture:
			{
			TBool isProtected = (TUint(aClientSid) & 0x80000000) == 0;
			if(! isProtected)
				{
				return EQueryUps;
				}

			// Need to obtain the drive letter for the exe
			//
			// This requires opening the RProcess and two copies of a full filename (one 8 bit the other unicode)......
			// We could optimse this by doing it when Initialise() is called, but that then slows down all Initialise calls even ones
			// not doing the  ECheckPostManufacture check.
			RProcess clientProcess;
			if(clientProcess.Open(aClientProcessId) != KErrNone)
				{
				return EReject; // Presumably it exited...
				}
			
			TFileName clientExeName = clientProcess.FileName();
			clientProcess.Close();
			TChar driveChar= clientExeName[0];
			
			if(aServerChecksPassed && 
			   (driveChar == 'z' || driveChar == 'Z'))
				{
				return EAllow;
				}

			return EQueryUps;
			}
			
		case ECheckUnprotectedSids:
			{
			TBool isProtected = (TUint(aClientSid) & 0x80000000) == 0;
			
			if(aServerChecksPassed && isProtected)
				{
				return EAllow;
				}
			
			return EQueryUps;
			}
			
		case ECheckIfFailed:
			return aServerChecksPassed ? EAllow : EQueryUps;

		BULLSEYE_OFF
		default:
		BULLSEYE_RESTORE
		case ENeverCheck:
			return aServerChecksPassed ? EAllow: EReject;
			}
	// Never gets here
	}

//
// CPolicyChangeWatcher member functions
//
CPolicyChangeWatcher *CPolicyChangeWatcher::NewL(RUpsSession &aUpsSession)
	{
	CPolicyChangeWatcher *self = new(ELeave) CPolicyChangeWatcher(aUpsSession);
	CleanupStack::PushL(self);
	self->ConstructL();
	CleanupStack::Pop(self);
	return self;
	}

CPolicyChangeWatcher::CPolicyChangeWatcher(RUpsSession &aUpsSession)
	:	CActive(CActive::EPriorityStandard+1), 
		iUpsSession(aUpsSession)
	{
	CActiveScheduler::Add(this);
	}

void CPolicyChangeWatcher::ConstructL()
	{
	User::LeaveIfError(iUpsPolicyChangeProperty.Attach(KUpsServerUid, KUpsServiceConfigProperty, EOwnerThread));
	iUpsPolicyChangeProperty.Subscribe(iStatus);
	SetActive();
	}

CPolicyChangeWatcher::~CPolicyChangeWatcher()
	{
	Cancel();
	iUpsPolicyChangeProperty.Close();
	}

void CPolicyChangeWatcher::DoCancel()
	{
	iUpsPolicyChangeProperty.Cancel();
	}

void CPolicyChangeWatcher::RunL()
	{
	// Re-subscribe for policy file change notifications
	iUpsPolicyChangeProperty.Subscribe(iStatus);
	SetActive();

	// Tell session to update its cache.
	iUpsSession.NotifyPolicyFileChangedL();
	}

TInt CPolicyChangeWatcher::RunError(TInt /* aError */)
	{
	// Ignore all errors
	//RDebug::Printf("CPolicyChangeWatcher::RunError(%d)\n", aError);
	return KErrNone;
	}


} // End of namespace UserPromptService

// End of file

