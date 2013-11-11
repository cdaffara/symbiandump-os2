/**
* Copyright (c) 2004-2009 Nokia Corporation and/or its subsidiary(-ies).
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
* Declaration of RABClientSession
* 
*
*/



/**
 @file
*/
#ifndef __ABCLIENTSESSION_H__
#define __ABCLIENTSESSION_H__

#include <e32std.h>
#include <e32cmn.h>
#include <connect/sbdefs.h>
#include <connect/abclientserver.h>

namespace conn
	{ 

	class TServerStart;
	class RABClientSession : public RSessionBase
    /** The active backup client to the Secure Backup Engine.

	@internalComponent
    */
    {
public:
	RABClientSession();
	TInt Connect();
	void Close();
	TVersion Version() const;
	
	// IPC calls for the callbacks
	HBufC8* GetDataFromServerLC(const TInt aDataLength, TABCallbackCommands aCallbackRequestingData);
	HBufC8* GetDataFromServerLC(const TInt aDataLength, TABCallbackCommands aCallbackRequestingData, TDriveNumber& aDriveNum);
	void SendDataLengthToServerL(TDesC8& aData, TBool aFinished, TABCallbackCommands aCallbackSendingData);
	void PrimeServerForCallbackL(TPckgBuf<TABCallbackCommands>& aCallback, TPckgBuf<TInt>& aArg1, 
		TPckgBuf<TInt>& aArg2, TRequestStatus& aStatus);
	void PrimeServerForCallbackWithResponseL(TPckgBuf<TABCallbackCommands>& aCallback, TPckgBuf<TInt>& aArg1, 
		TPckgBuf<TInt>& aArg2, TInt aResult, TRequestStatus& aStatus);
	void PrimeServerForCallbackWithResponseL(TPckgBuf<TABCallbackCommands>& aCallback, TPckgBuf<TInt>& aArg1, 
		TPckgBuf<TInt>& aArg2, TDesC8& aResult, TRequestStatus& aStatus);
	void PropagateLeave(TInt aLeaveCode);
	
	// IPC calls on the ABClient interface
	void BURModeInfoL(TDriveList& aDriveList, TBURPartType& aBackupType, TBackupIncType& aIncBackupType);
	TBool DoesPartialBURAffectMeL();
	void ConfirmReadyForBURL(TInt aErrorCode);
	void CancelServerCallbackL();

private:
	static TInt StartServer();

    };

  } // end namespace

#endif // __ABCLIENTSESSION_H__
