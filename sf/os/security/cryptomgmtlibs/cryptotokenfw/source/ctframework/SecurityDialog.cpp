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
*
*/


#include "CRefSecurityDialog.h"

// ----------------------------------------------------------------------------
// SecurityDialogFactory
//

EXPORT_C MSecurityDialog* SecurityDialogFactory::CreateL()
	{
	return CRefSecurityDialog::NewL();
	}



// ----------------------------------------------------------------------------
// CRefSecurityDialog
//



CRefSecurityDialog::~CRefSecurityDialog()
	{
	delete iRequester;
	}

void CRefSecurityDialog::Release()
	{
	delete this;
	}


CRefSecurityDialog* CRefSecurityDialog::NewL()
	{
	CRefSecurityDialog* self = new (ELeave) CRefSecurityDialog();
	CleanupStack::PushL( self );
	self->ConstructL();
	CleanupStack::Pop( self );
	return self;
	}



void CRefSecurityDialog::ConstructL()
	{
	iRequester = CDialogRequester::NewL( *this );
	}



void CRefSecurityDialog::EnterPIN( const TPINParams& aPINParams, TBool aRetry,
										   TPINValue& aPINValue, TRequestStatus& aStatus )
	{
	iClientPINValuePtr = &aPINValue;
	DoBasicPINOperation( EEnterPIN, aPINParams, aRetry, iPINValueBuf, aStatus );
	}



void CRefSecurityDialog::EnablePIN( const TPINParams& aPINParams, TBool aRetry,
											TPINValue& aPINValue, TRequestStatus& aStatus )
	{
	iClientPINValuePtr = &aPINValue;
	DoBasicPINOperation( EEnablePIN, aPINParams, aRetry, iPINValueBuf, aStatus );
	}



void CRefSecurityDialog::DisablePIN( const TPINParams& aPINParams, TBool aRetry,
											 TPINValue& aPINValue, TRequestStatus& aStatus )
	{
	iClientPINValuePtr = &aPINValue;
	DoBasicPINOperation( EDisablePIN, aPINParams, aRetry, iPINValueBuf, aStatus );
	}



void CRefSecurityDialog::ChangePIN( const TPINParams& aPINParams, TBool aRetry,
											TPINValue& aOldPINValue, TPINValue& aNewPINValue,
											TRequestStatus& aStatus )
	{
	iClientPINValuePtr = &aOldPINValue;
	iClientSecondPINValuePtr = &aNewPINValue;
	DoBasicPINOperation( EChangePIN, aPINParams, aRetry, iTwoPINOutputBuf, aStatus );
	}



void CRefSecurityDialog::UnblockPIN( const TPINParams& aBlockedPINParams,
						 const TPINParams& aUnblockingPINParams, TBool aRetry,
						 TPINValue& aUnblockingPINValue, TPINValue& aNewPINValue,
						 TRequestStatus& aStatus )
	{
	iCurrentOperation = EUnblockPIN;
	InitClientStatus( aStatus );

	TUnblockPINInput unblockPINInput;
	unblockPINInput.iOperation = EUnblockPIN;
	if ( aRetry )
		{
		unblockPINInput.iOperation |= EPINValueIncorrect;
		}
	unblockPINInput.iPIN = aBlockedPINParams;
	unblockPINInput.iUnblockPIN = aUnblockingPINParams;
	TUnblockPINInputBuf unblockPINInputBuf( unblockPINInput );
	iUnblockPINInputBuf.Copy( unblockPINInputBuf );
	iClientPINValuePtr = &aUnblockingPINValue;
	iClientSecondPINValuePtr = &aNewPINValue;

	iRequester->RequestDialog( iUnblockPINInputBuf, iTwoPINOutputBuf );
	}

void CRefSecurityDialog::UnblockPINInClear( const TPINParams& aBlockedPINParams,
									const TPINParams& aUnblockingPINParams, TBool aRetry,
									TUnblockPINValue& aUnblockingPINValue, TPINValue& aNewPINValue,
									TRequestStatus& aStatus )
	{
	iCurrentOperation = EUnblockPINInClear;
	InitClientStatus( aStatus );

	TUnblockPINInput unblockPINInput;
	unblockPINInput.iOperation = EUnblockPINInClear;
	if ( aRetry )
		{
		unblockPINInput.iOperation |= EPINValueIncorrect;
		}
	unblockPINInput.iPIN = aBlockedPINParams;
	unblockPINInput.iUnblockPIN = aUnblockingPINParams;
	TUnblockPINInputBuf unblockPINInputBuf( unblockPINInput );

	iClientUnblockPINValuePtr = &aUnblockingPINValue;
	iClientSecondPINValuePtr = &aNewPINValue;
	iUnblockPINInputBuf.Copy( unblockPINInputBuf );
	iRequester->RequestDialog( iUnblockPINInputBuf, iUnblockPINInClearOutputBuf );
	}

void CRefSecurityDialog::Cancel()
	{
	iRequester->Cancel();
	if (iClientStatus)
		{
		User::RequestComplete(iClientStatus, KErrCancel);
		}
	}

void CRefSecurityDialog::PINBlocked( const TPINParams& aPINParams,
											  TRequestStatus& aStatus )
	{
	iCurrentOperation = EPINBlocked;
	TPckg<TPINValue> pinValueBuf( KNullDesC() );
	DoBasicPINOperation( EPINBlocked, aPINParams, EFalse, pinValueBuf, aStatus );
	}


void CRefSecurityDialog::TotalBlocked( const TPINParams& aPINParams,
											  TRequestStatus& aStatus )
	{
	iCurrentOperation = ETotalBlocked;
	TPckg<TPINValue> pinValueBuf( KNullDesC() );
	DoBasicPINOperation( ETotalBlocked, aPINParams, EFalse, pinValueBuf, aStatus );
	}



void CRefSecurityDialog::EstablishSecureConnection( const TDesC8& aCertData,
						const RArray<TCTTokenObjectHandle>& aCertHandleList,
						MSecurityDialog::TConnectionType aConnectionType,
						TBool& aDoClientAuthentication, TCTTokenObjectHandle& aCertHandle,
						TRequestStatus& aStatus )
	{
	iCurrentOperation = ESecureConnection;
	InitClientStatus( aStatus );
	
	TInt certDataSize = aCertData.Size();
	TSignInput signInput;
	signInput.iOperation = ESecureConnection;
	signInput.iVariableDataSize = certDataSize;
	signInput.iCertHandleArrayTotal = aCertHandleList.Count();
	signInput.iDoClientAuthentication = aDoClientAuthentication;
	signInput.iConnectionType = aConnectionType;
	TPckgC<TSignInput> signInputBufPtr( signInput );
	// iSignInputBuf is persistent because further input processing occurs in the AO before
	// calling the notifier.
	iSignInputBuf.Copy( signInputBufPtr );
	iVariableDataPtr.Set( aCertData );

	iClientDoClientAuthenticationPtr = &aDoClientAuthentication;
	iClientCertInfoHandlePtr = &aCertHandle;
	
	iRequester->RequestVariableBufferDialog( iSignInputBuf, iVariableDataPtr,
											aCertHandleList, iCertInfoHandleBuf );
	}



void CRefSecurityDialog::SignText( const TDesC& aTextToSign,
									const RArray<TCTTokenObjectHandle>& aCertHandleList, 
									TCTTokenObjectHandle& aCertHandle,
									TRequestStatus& aStatus )
	{
	iCurrentOperation = ESignText;
	InitClientStatus( aStatus );

	TInt textSize = aTextToSign.Size();
	TSignInput signInput;
	signInput.iOperation = ESignText;
	signInput.iVariableDataSize = textSize;
	signInput.iCertHandleArrayTotal = aCertHandleList.Count();
	TPckgC<TSignInput> signInputBufPtr( signInput );
	// iSignInputBuf is persistent because further input processing occurs in the AO before
	// calling the notifier.
	iSignInputBuf.Copy( signInputBufPtr );
	const TUint8* textToSignPtr = reinterpret_cast<const TUint8*>( aTextToSign.Ptr() );
	iVariableDataPtr.Set( textToSignPtr, textSize );

	iClientCertInfoHandlePtr = &aCertHandle;

	iRequester->RequestVariableBufferDialog( iSignInputBuf, iVariableDataPtr,
											aCertHandleList, iCertInfoHandleBuf );
	}

void CRefSecurityDialog::ServerAuthenticationFailure(const TDesC8& aServerName,const TValidationError& aFailureReason, const TDesC8& aEncodedCert,TRequestStatus& aStatus )
	{
	iCurrentOperation = EServerAuthenticationFailure;
	InitClientStatus( aStatus );

	CServerAuthenticationFailureInput* serverAuthenticationFailureInput = NULL;
	TRAPD(err, serverAuthenticationFailureInput = CServerAuthenticationFailureInput::NewL(aServerName, aFailureReason, aEncodedCert));

	// Ownership of the inputBuffer will pass to the requester.
	HBufC8* inputBuffer = NULL;
	if (err == KErrNone)
		TRAP(err, inputBuffer = serverAuthenticationFailureInput->PackBufferL());

	delete serverAuthenticationFailureInput;

	if (err == KErrNone)
		iRequester->RequestDialog(inputBuffer, iServerAuthenticationOutputBuf);
	else
		HandleResponse(err);
	}

void CRefSecurityDialog::DoBasicPINOperation( TSecurityDialogOperation aOperation,
								const TPINParams& aPINParams, TBool aRetry, TDes8& aPINValue,
								TRequestStatus& aStatus )
	{
	iCurrentOperation = aOperation;
	InitClientStatus( aStatus );
	TPINInput pinInput;
	pinInput.iOperation = aOperation;
	if ( aRetry )
		{
		pinInput.iOperation |= EPINValueIncorrect;
		}
	pinInput.iPIN = aPINParams;
	TPINInputBuf pinInputBuf( pinInput );
	iPINInputBuf.Copy( pinInputBuf );
	iRequester->RequestDialog( iPINInputBuf, aPINValue );
	}



void CRefSecurityDialog::InitClientStatus( TRequestStatus& aStatus )
	{
	iClientStatus = &aStatus;
	*iClientStatus = KRequestPending;
	}



void CRefSecurityDialog::HandleResponse( TInt aResult )
	{
	if ( aResult >= KErrNone )
		{
		switch ( iCurrentOperation )
			{
			case EServerAuthenticationFailure:
				{
				TServerAuthenticationFailureDialogResult serverAuthenticationOutput = iServerAuthenticationOutputBuf();
							
				if(serverAuthenticationOutput == EContinue)
					{
					aResult 	   = KErrNone;
					}
				else if(serverAuthenticationOutput == EStop)
					{
					aResult 	   = KErrAbort;	
					}				
				}
				break;
			case ESecureConnection:
				{
				if ( aResult == KClientAuthenticationRequested )
					{
					*iClientDoClientAuthenticationPtr = ETrue;
					aResult = KErrNone;
					}
				else
					{
					*iClientDoClientAuthenticationPtr = EFalse;
					}
				*iClientCertInfoHandlePtr = iCertInfoHandleBuf();
				break;
				}
			case ESignText:
				{
				*iClientCertInfoHandlePtr = iCertInfoHandleBuf();
				break;
				}
			case EEnterPIN:
			case EEnablePIN:
			case EDisablePIN:
				*iClientPINValuePtr = iPINValueBuf();
				break;
			case EChangePIN:
			case EUnblockPIN:
				{
				TTwoPINOutput& twoPINOutput = iTwoPINOutputBuf();
				*iClientPINValuePtr = twoPINOutput.iPINValueToCheck;
				*iClientSecondPINValuePtr = twoPINOutput.iNewPINValue;
				break;
				}

			case EUnblockPINInClear:
				{
				TUnblockPINInClearOutput& unblockPINOutput = iUnblockPINInClearOutputBuf();
				*iClientUnblockPINValuePtr = unblockPINOutput.iPINValueToCheck;
				*iClientSecondPINValuePtr = unblockPINOutput.iNewPINValue;
				break;
				}

			case EPINBlocked:
		case ETotalBlocked:
				// No data to return
				break;
			default:
				__ASSERT_DEBUG( EFalse, _L( "Invalid operation" ) );
			}
		}

	User::RequestComplete( iClientStatus, aResult );
	}



// ----------------------------------------------------------------------------
// CDialogRequester
//

CDialogRequester::CDialogRequester( CRefSecurityDialog& aSecDialog )
	: CActive( EPriorityStandard ), iSecDialog( aSecDialog )
	{
	CActiveScheduler::Add( this );
	}



CDialogRequester::~CDialogRequester()
	{
	Cancel();
	iNotifier.Close();
	}



CDialogRequester* CDialogRequester::NewL( CRefSecurityDialog& aSecDialog )
	{
	CDialogRequester* self = new (ELeave) CDialogRequester( aSecDialog );
	CleanupStack::PushL( self );
	self->ConstructL();
	CleanupStack::Pop( self );
	return self;
	}



void CDialogRequester::ConstructL()
	{
	User::LeaveIfError( iNotifier.Connect() );
	}



void CDialogRequester::RequestDialog( const TDesC8& aData, TDes8& aResponse )
	{
	iNotifier.StartNotifierAndGetResponse( iStatus, KUidSecurityDialogNotifier,
											aData, aResponse );
	iState = KMakingRequest;
	SetActive();
	}



void CDialogRequester::DoCancel()
	{
	delete iInputBuffer;
	iInputBuffer = NULL;
	iNotifier.CancelNotifier( KUidSecurityDialogNotifier );
	}



void CDialogRequester::RequestVariableBufferDialog( const TDesC8& aOperationData,
										const TDesC8& aVariableData,
										const RArray<TCTTokenObjectHandle>& aCertHandleList,
										TDes8& aResponse )
	{
	iOperationDataPtr = &aOperationData;
	iVariableDataPtr = &aVariableData;
	iCertHandleListPtr = &aCertHandleList;
	iResponsePtr = &aResponse;
	iState = KFillingVariableInputBuffer;
	SetActive();
	// Complete the AO immediately so that buffer processing can occur in RunL().
	TRequestStatus* statusPtr = &iStatus;
	User::RequestComplete( statusPtr, KErrNone );
	}

void CDialogRequester::RequestDialog(HBufC8* aInputBuffer, TDes8& aResponse)
{
iInputBuffer = aInputBuffer;
RequestDialog(*iInputBuffer, aResponse);
}


void CDialogRequester::RunL()
	{
	switch ( iState )
		{
		case KFillingVariableInputBuffer:
			{
			TInt operationDataSize = iOperationDataPtr->Size();
			TInt variableDataSize = iVariableDataPtr->Size();
			TInt arraySize = sizeof( TCTTokenObjectHandle ) * iCertHandleListPtr->Count();
			TInt bufferSize = operationDataSize + variableDataSize + arraySize;
			iInputBuffer = HBufC8::NewL( bufferSize );
			TPtr8 inputBufferPtr( iInputBuffer->Des() );
			inputBufferPtr.Append( *iOperationDataPtr );
			for ( TInt h = 0, total = iCertHandleListPtr->Count(); h < total; ++h )
				{
				const TCTTokenObjectHandle& certHandle = (*iCertHandleListPtr)[h];
				TPckgC<TCTTokenObjectHandle> certHandleBuf( certHandle );
				inputBufferPtr.Append( certHandleBuf );
				}
			inputBufferPtr.Append( *iVariableDataPtr );
			RequestDialog( *iInputBuffer, *iResponsePtr );
			break;
			}
		case KMakingRequest:
			{
			delete iInputBuffer;
			iInputBuffer = NULL;
			iSecDialog.HandleResponse( iStatus.Int() );
			break;
			}
		default:
			__ASSERT_DEBUG( EFalse, _L( "Invalid state" ) );
		}
	}



/**
 * RunError should only be called if the buffer allocation fails, in which case
 * let client handle it.
 */
TInt CDialogRequester::RunError( TInt aError )
	{
	iSecDialog.HandleResponse( aError );
	return KErrNone;
	}

// ----------------------------------------------------------------------------
// CServerAuthenticationFailureInput
//

CServerAuthenticationFailureInput::CServerAuthenticationFailureInput()
	{
	}


CServerAuthenticationFailureInput::~CServerAuthenticationFailureInput()
	{
	if(iServerName)
		delete iServerName;
	if(iEncodedCert)
		delete iEncodedCert;
	}

CServerAuthenticationFailureInput* CServerAuthenticationFailureInput::NewL(const TDesC8& aServerName,const TValidationError& aFailureReason, const TDesC8& aEncodedCert, const TBool aCreateOwnBuffers)
	{
	CServerAuthenticationFailureInput* self = CServerAuthenticationFailureInput::NewLC(aServerName, aFailureReason, aEncodedCert, aCreateOwnBuffers);
	CleanupStack::Pop(self);
	return self;
	}

CServerAuthenticationFailureInput* CServerAuthenticationFailureInput::NewLC(const TDesC8& aServerName,const TValidationError& aFailureReason, const TDesC8& aEncodedCert, const TBool aCreateOwnBuffers)
	{
	CServerAuthenticationFailureInput* self = new(ELeave) CServerAuthenticationFailureInput();
	CleanupStack::PushL(self);
	self->ConstructL(aServerName, aFailureReason, aEncodedCert, aCreateOwnBuffers);
	return self;
	}

EXPORT_C CServerAuthenticationFailureInput* CServerAuthenticationFailureInput::NewL(const TDesC8& aBuffer, const TBool aCreateOwnBuffers)
	{
	CServerAuthenticationFailureInput* self = CServerAuthenticationFailureInput::NewLC(aBuffer, aCreateOwnBuffers);
	CleanupStack::Pop(self);
	return self;
	}

EXPORT_C CServerAuthenticationFailureInput* CServerAuthenticationFailureInput::NewLC(const TDesC8& aBuffer, const TBool aCreateOwnBuffers)
	{
	CServerAuthenticationFailureInput* self = new(ELeave) CServerAuthenticationFailureInput();
	CleanupStack::PushL(self);
	self->ConstructL(aBuffer, aCreateOwnBuffers);
	return self;
	}

void CServerAuthenticationFailureInput::ConstructL(const TDesC8& aServerName,const TValidationError& aFailureReason, const TDesC8& aEncodedCert, const TBool aCreateOwnBuffers)
	{
	iFailureReason = aFailureReason;
	
	if (aCreateOwnBuffers)
		{
		iServerName = aServerName.AllocL();
		iServerNamePtr.Set(iServerName->Des());
		iEncodedCert = aEncodedCert.AllocL();
		iEncodedCertPtr.Set(iEncodedCert->Des());
		}
	else
		{
		iServerNamePtr.Set(aServerName);
		iEncodedCertPtr.Set(aEncodedCert);
		}
	}


void CServerAuthenticationFailureInput::ConstructL(const TDesC8& aBuffer, const TBool aCreateOwnBuffers)
	{
	const TServerAuthenticationFailureInput* srvAuthFail = 
						reinterpret_cast<const TServerAuthenticationFailureInput*>
							( aBuffer.Ptr() );

	const TUint8* binaryDataPtr = aBuffer.Ptr() + sizeof(TServerAuthenticationFailureInput);
	
	iFailureReason = srvAuthFail->iFailureReason;

	if(aCreateOwnBuffers)
		{
		TPtrC8 dataPtr(binaryDataPtr, srvAuthFail->iServerNameLength);
		iServerName = dataPtr.AllocL();
		iServerNamePtr.Set(iServerName->Des());
		
		dataPtr.Set(binaryDataPtr + srvAuthFail->iServerNameLength, srvAuthFail->iEncodedCertLength);
		iEncodedCert = dataPtr.AllocL();
		iEncodedCertPtr.Set(iEncodedCert->Des());
		}
	else
		{
		iServerNamePtr.Set(binaryDataPtr, srvAuthFail->iServerNameLength);
		iEncodedCertPtr.Set(binaryDataPtr + srvAuthFail->iServerNameLength, srvAuthFail->iEncodedCertLength);
		}
	
	}

EXPORT_C TValidationError CServerAuthenticationFailureInput::FailureReason()
	{
	return iFailureReason;
	}

EXPORT_C void CServerAuthenticationFailureInput::GetServerName(TPtrC8& aServerNamePtr)
	{
	aServerNamePtr.Set(iServerNamePtr);
	}

EXPORT_C void CServerAuthenticationFailureInput::GetEncodedCert(TPtrC8& aEncodedCertPtr)
	{
	aEncodedCertPtr.Set(iEncodedCertPtr);
	}

EXPORT_C HBufC8* CServerAuthenticationFailureInput::PackBufferL() const
	{
	TServerAuthenticationFailureInput serverAuthenticationInput;
	serverAuthenticationInput.iOperation     = EServerAuthenticationFailure;
	serverAuthenticationInput.iFailureReason = iFailureReason;
	serverAuthenticationInput.iServerNameLength = iServerNamePtr.Length(); 
	serverAuthenticationInput.iEncodedCertLength = iEncodedCertPtr.Length();
		
	TServerAuthenticationFailureInputBuf serverAuthenticationInputBuf(serverAuthenticationInput);

	TInt bufferSize = sizeof(serverAuthenticationInputBuf) + iServerNamePtr.Length() + iEncodedCertPtr.Length();
	HBufC8* packedBuffer = HBufC8::NewL(bufferSize);
	TPtr8 packedBufferPtr(packedBuffer->Des());

	packedBufferPtr.Append(serverAuthenticationInputBuf);
	packedBufferPtr.Append(iServerNamePtr);
	packedBufferPtr.Append(iEncodedCertPtr);
	
	return packedBuffer;
	}

