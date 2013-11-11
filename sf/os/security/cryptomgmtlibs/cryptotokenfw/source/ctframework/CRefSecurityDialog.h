/*
* Copyright (c) 2001-2009 Nokia Corporation and/or its subsidiary(-ies).
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
* CRefSecurityDialog class implementation
*
*/


/**
 @file 
 @internalTechnology
*/
 
#ifndef __CREFSECURITYDIALOG_H__
#define __CREFSECURITYDIALOG_H__

#include "secdlgImplDefs.h"

class CDialogRequester;

/**
 * Reference implementation of the MSecurityDialog API. It wraps the arguments into
 * a form suitable for use with the RNotifier dialog framework.
 */
NONSHARABLE_CLASS(CRefSecurityDialog) : public CBase, public MSecurityDialog 
	{
public:
	 ~CRefSecurityDialog();
	 static CRefSecurityDialog* NewL();

public: // from MSecurityDialog
     void ServerAuthenticationFailure(const TDesC8& aServerName,const TValidationError& aFailureReason,const TDesC8& aEncodedCert,TRequestStatus& aStatus );
	 void Release();
	 void EnterPIN( const TPINParams& aPINParams, TBool aRetry, TPINValue& aPINValue,
						   TRequestStatus& aStatus );
	 void EnablePIN( const TPINParams& aPINParams, TBool aRetry, TPINValue& aPINValue,
							TRequestStatus& aStatus );
	 void DisablePIN( const TPINParams& aPINParams, TBool aRetry, TPINValue& aPINValue,
							 TRequestStatus& aStatus );
	 void ChangePIN( const TPINParams& aPINParams, TBool aRetry,
							TPINValue& aOldPINValue, TPINValue& aNewPINValue,
							TRequestStatus& aStatus );
	 void UnblockPIN( const TPINParams& aBlockedPINParams,
							 const TPINParams& aUnblockingPINParams, TBool aRetry,
							 TPINValue& aUnblockingPINValue, TPINValue& aNewPINValue,
							 TRequestStatus& aStatus );
	 void UnblockPINInClear( const TPINParams& aBlockedPINParams,
							 const TPINParams& aUnblockingPINParams, TBool aRetry,
							 TUnblockPINValue& aUnblockingPINValue, TPINValue& aNewPINValue,
							 TRequestStatus& aStatus );
	 void Cancel();
	 void PINBlocked( const TPINParams& aPINParams, TRequestStatus& aStatus );
	 void TotalBlocked( const TPINParams& aPINParams, TRequestStatus& aStatus );
	 void EstablishSecureConnection( const TDesC8& aCertData,
						const RArray<TCTTokenObjectHandle>& aCertHandleList,
						MSecurityDialog::TConnectionType aConnectionType,
						TBool& aDoClientAuthentication, TCTTokenObjectHandle& aCertId,
						TRequestStatus& aStatus );
	 void SignText( const TDesC& aText,
							const RArray<TCTTokenObjectHandle>& aCertIdList,
							TCTTokenObjectHandle& aCertId,
							TRequestStatus& aStatus );

	void HandleResponse( TInt aResult );

private:
	void ConstructL();
	void DoBasicPINOperation( TSecurityDialogOperation aOperation,
								const TPINParams& aPINParams, TBool aRetry, TDes8& aPINValue,
								TRequestStatus& aStatus );
	void InitClientStatus( TRequestStatus& aStatus );

private:
	CDialogRequester* iRequester;
	TSecurityDialogOperation iCurrentOperation;
	TRequestStatus* iClientStatus;
	TPINValue* iClientPINValuePtr;
	TUnblockPINValue* iClientUnblockPINValuePtr;
	TPINValue* iClientSecondPINValuePtr;
	TPINValueBuf iPINValueBuf;
	TTwoPINOutputBuf iTwoPINOutputBuf;
	TUnblockPINInClearOutputBuf iUnblockPINInClearOutputBuf;
	TSignInputBuf iSignInputBuf;
	TPtrC8 iVariableDataPtr;
	TCTTokenObjectHandle* iClientCertInfoHandlePtr;
	TBool* iClientDoClientAuthenticationPtr;
	TCTTokenObjectHandleBuf iCertInfoHandleBuf;
	TUnblockPINInputBuf iUnblockPINInputBuf;
	TPINInputBuf iPINInputBuf;
	TServerAuthenticationFailureOutputBuf iServerAuthenticationOutputBuf;
	};

/**
 * An AO used to call the notifier, which allows packaging and unpackaging of the
 * arguments and return values for the client.
 */
NONSHARABLE_CLASS(CDialogRequester) : public CActive
	{
private:
	enum TState { KFillingVariableInputBuffer, KMakingRequest };

public:
	static CDialogRequester* NewL( CRefSecurityDialog& aSecDialog );
	~CDialogRequester();
	void RequestDialog( const TDesC8& aData, TDes8& aResponse );
	void RequestVariableBufferDialog( const TDesC8& aOperationData,
										const TDesC8& aVariableData,
										const RArray<TCTTokenObjectHandle>& aCertHandleList,
										TDes8& aResponse );

	void RequestDialog(HBufC8* iInputBuffer, TDes8& aResponse);

private:
	CDialogRequester( CRefSecurityDialog& aSecDialog );
	void ConstructL();

private: // from CActive
	void RunL();
	void DoCancel();
	TInt RunError( TInt aError );

private:
	TState iState;
	RNotifier iNotifier;
	CRefSecurityDialog& iSecDialog;
	const TDesC8* iOperationDataPtr;
	const TDesC8* iVariableDataPtr;
	const RArray<TCTTokenObjectHandle>* iCertHandleListPtr;
	HBufC8* iInputBuffer;
	TDes8* iResponsePtr;
	};

#endif
