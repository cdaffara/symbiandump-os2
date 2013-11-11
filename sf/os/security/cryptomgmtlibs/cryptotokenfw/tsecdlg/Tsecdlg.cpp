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
* secdlgImpl.cpp
*
*/


#include "Tsecdlg.h"
#include <secdlgimpldefs.h>
#include <certificateapps.h>
#include <x509cert.h>


// These are not really allocated to us, but this is only for finding errors
// while debugging, so it doesn't really matter
const TInt KErrTooManyDialogs			= -12000;
const TInt KErrLabelMismatch 			= -12001;
const TInt KErrOperationMismatch		= -12002;
const TInt KErrOperationNotSupported	= -12003;

_LIT(KpinValue,"pinkcloud");
_LIT(KYes,"Yes");
	
// ----------------------------------------------------------------------------
// Lib main entry point.
// This can leave and should leave (if failure occurs) despite the lack of trailing L.
//

#ifdef _T_SECDLG_TEXTSHELL
EXPORT_C CArrayPtr<MNotifierBase2>* NotifierArray()
#else
CArrayPtr<MNotifierBase2>* NotifierArray()
#endif
	{
	//The notifierArray function CAN leave, despite no trailing L
	CArrayPtrFlat<MNotifierBase2>* subjects = new (ELeave) CArrayPtrFlat<MNotifierBase2>( 1 );
	CleanupStack::PushL(subjects);
	CTestSecDlgNotifier* notifier = CTestSecDlgNotifier::NewL();
	CleanupStack::PushL( notifier );
	subjects->AppendL( notifier );
	CleanupStack::Pop( 2,subjects);	//notifier, subjects
	return subjects;
	}

// ----------------------------------------------------------------------------
// Ecom plugin implementation for UI notifier
//

#ifndef _T_SECDLG_TEXTSHELL

const TImplementationProxy ImplementationTable[] =
	{
		IMPLEMENTATION_PROXY_ENTRY(KTSecDlgNotiferUid, NotifierArray)
	};

EXPORT_C const TImplementationProxy* ImplementationGroupProxy(TInt& aTableCount)
	{
	aTableCount = sizeof(ImplementationTable) / sizeof(TImplementationProxy);
	return (ImplementationTable);
	}

#endif

// ----------------------------------------------------------------------------
// CInputSpec
//

CInputSpec::CInputSpec(TSecurityDialogOperation aOp, HBufC* aLabelSpec, HBufC* aResponse1, HBufC* aResponse2) :
	iOp(aOp), iLabelSpec(aLabelSpec), iResponse1(aResponse1), iResponse2(aResponse2)
	{
	}

CInputSpec::~CInputSpec()
	{
	delete iLabelSpec;
	delete iResponse1;
	delete iResponse2;
	}


// ----------------------------------------------------------------------------
// CTestSecDlgNotifier
//

_LIT(KInputFile, "\\t_secdlg_in.dat");
_LIT(KOutputFile, "\\t_secdlg_out.dat");

CTestSecDlgNotifier* CTestSecDlgNotifier::NewL()
	{
	CTestSecDlgNotifier* self=new (ELeave) CTestSecDlgNotifier();
	CleanupStack::PushL(self);
	self->ConstructL();
	CleanupStack::Pop(self);
	return self;
	}

CTestSecDlgNotifier::CTestSecDlgNotifier()
	{
	iInfo.iUid = KUidSecurityDialogNotifier;
	iInfo.iChannel = TUid::Uid(0x00001234); // dummy
	iInfo.iPriority = ENotifierPriorityHigh;
	}

void CTestSecDlgNotifier::ConstructL()
	{
	User::LeaveIfError(iFs.Connect());
	}


TInt CTestSecDlgNotifier::GetInputIndexL()
	{
	RFileReadStream stream;
	TDriveUnit sysDrive (RFs::GetSystemDrive());
	TDriveName driveName(sysDrive.Name());
	TBuf<128> outputFile (driveName);
	outputFile.Append(KOutputFile);
	
	TInt err = stream.Open(iFs, outputFile, EFileRead | EFileShareExclusive);
	// If the file doesn't exist yet just return zero
	if (err == KErrNotFound)
		{
		return 0;
		}
	User::LeaveIfError(err);
	stream.PushL();
	TInt index = stream.ReadInt32L();
	CleanupStack::PopAndDestroy(); // stream
	return index;
	}

void CTestSecDlgNotifier::WriteDialogCountL(TInt aCount)
	{
	RFileWriteStream stream;
	TDriveUnit sysDrive (RFs::GetSystemDrive());
	TDriveName driveName(sysDrive.Name());
	TBuf<128> outputFile (driveName);
	outputFile.Append(KOutputFile);
	
	TInt err = stream.Replace(iFs, outputFile, EFileWrite | EFileShareExclusive);
	if (err == KErrNotFound)
		{
		err = stream.Create(iFs, outputFile, EFileWrite | EFileShareExclusive);
		}
	User::LeaveIfError(err);
	stream.PushL();
	stream.WriteInt32L(aCount);
	stream.CommitL();
	CleanupStack::PopAndDestroy(); // stream	
	}

CInputSpec* CTestSecDlgNotifier::ReadInputSpecL(TInt aIndex)
	{
	RFileReadStream stream;
	TDriveUnit sysDrive (RFs::GetSystemDrive());
	TDriveName driveName(sysDrive.Name());
	TBuf<128> inputFile (driveName);
	inputFile.Append(KInputFile);
	User::LeaveIfError(stream.Open(iFs, inputFile, EFileRead | EFileShareExclusive));
	stream.PushL();

	// Update dialog count here so test code can see how many dialogs were
	// requested if there were more than expected
	WriteDialogCountL(aIndex + 1);
	
	MStreamBuf* streamBuf = stream.Source();
	TInt labelSize, response1Size, response2Size;

	// Skip records until we reach the one we want
	for (TInt i = 0 ; i < aIndex ; ++i)
		{
		stream.ReadInt32L();
		labelSize = stream.ReadInt32L();
		streamBuf->SeekL(MStreamBuf::ERead, EStreamMark, labelSize * 2);
		response1Size = stream.ReadInt32L();
		streamBuf->SeekL(MStreamBuf::ERead, EStreamMark, response1Size * 2);
		response2Size = stream.ReadInt32L();
		streamBuf->SeekL(MStreamBuf::ERead, EStreamMark, response2Size * 2);
		}

	TSecurityDialogOperation op = static_cast<TSecurityDialogOperation>(stream.ReadInt32L());

	labelSize = stream.ReadInt32L();
	HBufC* labelSpec = HBufC::NewMaxLC(labelSize);
	TPtr labelPtr(labelSpec->Des());
	stream.ReadL(labelPtr, labelSize);
	
	response1Size = stream.ReadInt32L();
	HBufC* response1 = HBufC::NewMaxLC(response1Size);
	TPtr response1Ptr(response1->Des());
	stream.ReadL(response1Ptr, response1Size);
	
	response2Size = stream.ReadInt32L();
	HBufC* response2 = HBufC::NewMaxLC(response2Size);
	TPtr response2Ptr(response2->Des());
	stream.ReadL(response2Ptr, response2Size);

	CInputSpec* inputSpec = new (ELeave) CInputSpec(op, labelSpec, response1, response2);	
	CleanupStack::Pop(3, labelSpec);	
	CleanupStack::PopAndDestroy(); // stream

	return inputSpec;
	}


void CTestSecDlgNotifier::DoEnterPINL(const CInputSpec& aSpec, const TDesC8& aBuffer, TInt aReplySlot, const RMessagePtr2& aMessage)
	{
	const TPINInput& pinInput = reinterpret_cast<const TPINInput&>(*aBuffer.Ptr());

	if (pinInput.iPIN.iPINLabel.Find(aSpec.LabelSpec()) == KErrNotFound)
		{
		User::Leave(KErrLabelMismatch);
		}
	
	TPINValue pinValue = aSpec.Response1();
	TPckg<TPINValue> pinValueBufPtr(pinValue);
	aMessage.WriteL(aReplySlot, pinValueBufPtr);
	}

void CTestSecDlgNotifier::DoChangePINL(const CInputSpec& aSpec, const TDesC8& aBuffer, TInt aReplySlot, const RMessagePtr2& aMessage)
	{
	const TPINInput& input = reinterpret_cast<const TPINInput&>(*aBuffer.Ptr());

	if (input.iPIN.iPINLabel.Find(aSpec.LabelSpec()) == KErrNotFound)
		{
		User::Leave(KErrLabelMismatch);
		}

	TTwoPINOutput output;
	output.iPINValueToCheck = aSpec.Response1();
	output.iNewPINValue = aSpec.Response2();
	TPckg<TTwoPINOutput> outputPckg(output);
	aMessage.WriteL(aReplySlot, outputPckg);
	}


void CTestSecDlgNotifier::Release()
	{
	delete this;
	}



CTestSecDlgNotifier::TNotifierInfo CTestSecDlgNotifier::RegisterL()
	{
	return iInfo;
	}



CTestSecDlgNotifier::TNotifierInfo CTestSecDlgNotifier::Info() const
	{
	return iInfo;
	}



void CTestSecDlgNotifier::StartL(const TDesC8& aBuffer, TInt aReplySlot, const RMessagePtr2& aMessage)
	{
	TRAPD(err, DoStartL(aBuffer, aReplySlot, aMessage));
	aMessage.Complete(err);
	}
	
	
void CTestSecDlgNotifier::DoStartL(const TDesC8& aBuffer, TInt aReplySlot, const RMessagePtr2& aMessage)
	{
	// Minimum length is 4
	__ASSERT_DEBUG( aBuffer.Length() >= 4, User::Panic(_L("CTestSecDlgNotifier"), 0));

	TUint operation = *reinterpret_cast<const TInt *>(aBuffer.Ptr()) & KSecurityDialogOperationMask;
	
	TInt index = GetInputIndexL();
	CInputSpec* spec = NULL;

	TRAPD(err, spec = ReadInputSpecL(index));
	
	// If the input file doesn't exist then we will answer PIN requests with the
	// "pinkcloud" passphrase - this is so the certstore tests work independantly
	// from keystore
	if (err == KErrNotFound)
		{
		switch(operation)
			{
			case EEnterPIN:
				{
				TPINValue pinValue(KpinValue);
				TPckg<TPINValue> pinValueBufPtr(pinValue);
				aMessage.WriteL(aReplySlot, pinValueBufPtr);
				break;
				}
			case EServerAuthenticationFailure:
				{
				TServerAuthenticationFailureDialogResult output;
				output = EStop;				
	    		TServerAuthenticationFailureOutputBuf outputPckg(output);	
				aMessage.WriteL(aReplySlot, outputPckg);
				break;
				}
			default:
				{
				User::Leave(KErrOperationMismatch);
				break;
				}			
			}
		return;	
		}
	
	if (err == KErrEof)
		{
		User::Leave(KErrTooManyDialogs);
		}

	User::LeaveIfError(err);

	CleanupStack::PushL(spec);
	
	if (operation != spec->Operation())
		{
		User::Leave(KErrOperationMismatch);
		}
		
	switch (operation)
		{
		case EEnterPIN:
			DoEnterPINL(*spec, aBuffer, aReplySlot, aMessage);
			break;

		case EChangePIN:
			DoChangePINL(*spec, aBuffer, aReplySlot, aMessage);
			break;

		case ESecureConnection:
 			DoSecureConnectionL(*spec, aBuffer, aReplySlot, aMessage);
 			break;

		case ESignText:
		case EEnablePIN:
		case EDisablePIN:
		case EUnblockPIN:		
		case EUnblockPINInClear:
		case EPINBlocked:
			// these operations are not yet implemented in this test harness
			User::Leave(KErrOperationNotSupported);
			break;

		case EServerAuthenticationFailure:
			DoServerAuthenticationFailureL(*spec, aBuffer, aReplySlot, aMessage);
			break;

		default:
			User::Panic(_L("CTestSecDlgNotifier"), 0);
		}
	CleanupStack::PopAndDestroy(spec);
	}

void CTestSecDlgNotifier::DoServerAuthenticationFailureL(const CInputSpec& aSpec, const TDesC8& aBuffer, TInt aReplySlot, const RMessagePtr2& aMessage )
	{
	// Test for valid packing of dialog data by extracting the encoded certificate
	// and attempting to construct an X509 certificate from it.
	CServerAuthenticationFailureInput* srvAuthFail = CServerAuthenticationFailureInput::NewLC(aBuffer);
	TPtrC8 encodedCert;
	srvAuthFail->GetEncodedCert(encodedCert);
	
	// If CX509Certificate::NewL leaves the encoded cert buffer must not be valid.
	const CX509Certificate* cert = CX509Certificate::NewL(encodedCert);

	// There is no further need for the cert, so it can be deleted immediately.
	delete cert;
	
	CleanupStack::PopAndDestroy(srvAuthFail);
	
	TServerAuthenticationFailureDialogResult output;
	output = EStop;
	if( (aSpec.Response1()).CompareF(KYes) == KErrNone )
		{
		output = EContinue;			
		}
	TServerAuthenticationFailureOutputBuf outputPckg(output);	
	aMessage.WriteL(aReplySlot, outputPckg);
	}

void CTestSecDlgNotifier::DoSecureConnectionL(const CInputSpec& aSpec, const TDesC8& aBuffer, TInt aReplySlot, const RMessagePtr2& aMessage )
	{
	// If the client does not want to continue
	if( (aSpec.Response1()).CompareF(KYes) != KErrNone )
		{
		User::Leave(KErrCancel);	
		}
	else
		{
		const TSignInput* secureConnectionInput =
								reinterpret_cast<const TSignInput*>( aBuffer.Ptr() );
		// if the client certificate is requested
		if (secureConnectionInput->iDoClientAuthentication)
			{			
			TLex lexi(aSpec.Response2());
			TInt32 selectionId=0;
			TInt err=lexi.Val(selectionId);

			TInt certHandleTotal = secureConnectionInput->iCertHandleArrayTotal;
			if (selectionId>certHandleTotal)
				{
				User::Leave(KErrNotFound);	
				}
												
			// Get index at start of list of TCTTokenObjectHandle objects
			TInt bufferIndex = sizeof( TSignInput );
			TInt certHandleSize = sizeof( TCTTokenObjectHandle );
			TPckgBuf<TCTTokenObjectHandle> certHandleBuf;
			TPtrC8 certHandleData( aBuffer.Mid( bufferIndex+(selectionId-1)*certHandleSize, certHandleSize ) );
			certHandleBuf.Copy( certHandleData );
			aMessage.WriteL( aReplySlot, certHandleBuf );			
			}			
		}	
	}	

	
TPtrC8 CTestSecDlgNotifier::StartL( const TDesC8& /*aBuffer*/ )
	{
	User::Panic(_L("CTestSecDlgNotifier"), 0);
	return TPtrC8(KNullDesC8);
	}


void CTestSecDlgNotifier::Cancel()
	{
	// Don't think there is much we can do here. If a client deletes the
	// client-side security dialog instance, after calling a method that 
	// displays a dialog, this will not get called until the user dismisses
	// the dialog. We can't do anything then.
	}


TPtrC8 CTestSecDlgNotifier::UpdateL( const TDesC8& /*aBuffer*/ )
	{
	User::Panic(_L("CTestSecDlgNotifier"), 0);
	return NULL;
	}
