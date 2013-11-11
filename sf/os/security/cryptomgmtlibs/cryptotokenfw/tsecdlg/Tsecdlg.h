/*
* Copyright (c) 2002-2009 Nokia Corporation and/or its subsidiary(-ies).
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


/**
 @file 
 @internalTechnology
*/
 
#ifndef __TSECDLG_H__
#define __TSECDLG_H__

#include <e32base.h>
#include <secdlgimpldefs.h>

const TUint KTSecDlgNotiferUid = 0x10204789;

#ifdef _T_SECDLG_TEXTSHELL

#include <twintnotifier.h>

/** Method at ordinal 1 to get a list of notifiers from this dll. */
IMPORT_C CArrayPtr<MNotifierBase2>* NotifierArray();

#else

#include <eiknotapi.h>
#include <ecom/implementationproxy.h>
#define MNotifierBase2 MEikSrvNotifierBase2

IMPORT_C const TImplementationProxy* ImplementationGroupProxy(TInt& aTableCount);
CArrayPtr<MNotifierBase2>* NotifierArray();

#endif

class CInputSpec : public CBase
	{
 public:
	CInputSpec(TSecurityDialogOperation aOp, HBufC* aLabelSpec, HBufC* aResponse1, HBufC* aResponse2);
	~CInputSpec();
	TSecurityDialogNotification Operation() const { return iOp; }
	const TDesC& LabelSpec() const { return *iLabelSpec; }
	const TDesC& Response1() const { return *iResponse1; }
	const TDesC& Response2() const { return *iResponse2; }
 private:
	TSecurityDialogNotification iOp;
	HBufC* iLabelSpec;
	HBufC* iResponse1;
	HBufC* iResponse2;
	};

/**
 * This is a dummy implementation of the security dialog notifier.  This version
 * responds to dialogs with data read from t_secdlg_in.dat present on system drive.  
 * This contains the expected sequence of dialogs, and the desired responses.  It keeps a
 * count of how many dialogs it has answered in t_secdlg_out.dat present on system drive.
 */
class CTestSecDlgNotifier : public CBase, public MNotifierBase2
	{
public:
	static CTestSecDlgNotifier* NewL();
	CTestSecDlgNotifier();

private:
	void ConstructL();

	/**
	 * Get the index of the next response by reading how many dialogs we have
	 * already answered.
	 */
	TInt GetInputIndexL();

	/**
	 * Write the number of dalogs answered to the output file.
	 */
	void WriteDialogCountL(TInt aCount);

	/**
	 * Read an input specification.
	 */
	CInputSpec* ReadInputSpecL(TInt aIndex);

	void DoEnterPINL(const CInputSpec& aSpec, const TDesC8& aBuffer, TInt aReplySlot, const RMessagePtr2& aMessage);
	void DoChangePINL(const CInputSpec& aSpec, const TDesC8& aBuffer, TInt aReplySlot, const RMessagePtr2& aMessage);
	void DoStartL( const TDesC8& aBuffer, TInt aReplySlot, const RMessagePtr2& aMessage );
	void DoServerAuthenticationFailureL(const CInputSpec& aSpec, const TDesC8& aBuffer, TInt aReplySlot, const RMessagePtr2& aMessage );
 	void DoSecureConnectionL(const CInputSpec& aSpec, const TDesC8& aBuffer, TInt aReplySlot, const RMessagePtr2& aMessage );

private:
	void Release();
	TNotifierInfo RegisterL();
	TNotifierInfo Info() const;
	
	virtual void StartL( const TDesC8& aBuffer, TInt aReplySlot, const RMessagePtr2& aMessage );
	virtual TPtrC8 StartL( const TDesC8& aBuffer );
	
	void Cancel();
	TPtrC8 UpdateL( const TDesC8& aBuffer );

private:
	RFs iFs;
	TNotifierInfo iInfo;
	};

#endif
