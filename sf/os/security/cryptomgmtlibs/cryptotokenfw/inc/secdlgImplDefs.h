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


/**
 @file
 @publishedPartner
 @released
*/

#ifndef __SECDLGIMPLDEFS_H__
#define __SECDLGIMPLDEFS_H__

#include <e32std.h>

#include <secdlg.h>

/** */
// KUidSecurityDialogNotifier is named KUidWIMDialogNotifier in the UID database
const TUid KUidSecurityDialogNotifier = { 0x101F3CBB };

/** */
typedef TUint TSecurityDialogNotification;

/** */
typedef TPckgBuf<TSecurityDialogNotification> TSecurityDialogNotificationBuf;


/**
 * Defines the operations provided by the security dialog. 
 *
 * It is used to identify the operation when transferring over to the notifier plugin.
 */
enum TSecurityDialogOperation
	{
	/** Secure connection */
	ESecureConnection = 0x01,
	/** Sign text */
	ESignText = 0x02,
	/** Enter PIN */
	EEnterPIN = 0x04,
	/** Enable PIN */
	EEnablePIN = 0x08,
	/** Disable PIN */
	EDisablePIN = 0x10,
	/** Change PIN */
	EChangePIN = 0x20,
	/** Unblock PIN */
	EUnblockPIN = 0x40,
	/** PIN blocked */
	EPINBlocked = 0x80,
	/** Total blocked */
	ETotalBlocked = 0x100,
	/** Unblock PIN in clear */
	EUnblockPINInClear = 0x200,
	/** ServerAuthenticationFailure */
	EServerAuthenticationFailure = 0x400
	};

/** */
const TInt KSecurityDialogOperationMask = 0xFFFF;
const TInt KServerNameBuf    = 0xFF;  // 0xFF  = 255
const TInt KEncodedCertBuf   = 0x400; // 0x400 = 1024

/** Security dialog error condition */
enum TSecurityDialogErrorCondition
	{
	/** Incorrect PIN value */
	EPINValueIncorrect = 0x10000000
	};

/** */
//Passed back in TRequestStatus, if client agrees to authenticate/ The client TRequeststatus never gets set to this
const TInt KClientAuthenticationRequested = 1;	

/**
 * Class used with corresponding TPckgBuf to build buffer for the
 * Server Authentication Failure Dialog operation.
 */
class TServerAuthenticationFailureInput
	{
public:
	/** EServerAuthenticationFailure */
	TSecurityDialogNotification iOperation;
	
	/** Server name */
	TInt iServerNameLength;

	/** Encoded Certificate */
	TInt iEncodedCertLength;

	/** Failure Reason */
	TValidationError iFailureReason;
	};

/** */
typedef TPckgBuf<TServerAuthenticationFailureInput> TServerAuthenticationFailureInputBuf;

/**
 * The input buffer for the secure connection and sign text operations.
 */
class TSignInput
	{
public:
	/**  */
	TSecurityDialogNotification iOperation;
	
	/**  */
	TUint iVariableDataSize;
	
	/**  */
	TUint iCertHandleArrayTotal;
	
	/** 
	 * Whether to do client authentication 
	 *
	 * Used only for secure connection dialog
	 */
	TBool iDoClientAuthentication;
	
	/** The connection type */
	MSecurityDialog::TConnectionType iConnectionType;
	};

/** */
typedef TPckgBuf<TSignInput> TSignInputBuf;

/** */
typedef TPckgBuf<TCTTokenObjectHandle> TCTTokenObjectHandleBuf;


/**
 * Input data for the EEnterPIN, EChangePIN, EPINBlocked operations.
 */
class TPINInput
	{
public:
	/**  */
	TSecurityDialogNotification iOperation;

	/** Information associated with the PIN */
	TPINParams iPIN;
	};

/** */
typedef TPckgBuf<TPINInput> TPINInputBuf;



/**
 * Input data for the EUnblockPIN operation.
 */
class TUnblockPINInput
	{
public:
	TSecurityDialogNotification iOperation;
	TPINParams iPIN;
	TPINParams iUnblockPIN;
	};

/** */
typedef TPckgBuf<TUnblockPINInput> TUnblockPINInputBuf;

/**
 * Output from the EServerAuthenticationFailure operations.
 */
 /** Security dialog error condition */
enum TServerAuthenticationFailureDialogResult
	{	
	EContinue = 0x01,
	EStop     = 0x02
	}; 
/** */
typedef TPckgBuf<TServerAuthenticationFailureDialogResult> TServerAuthenticationFailureOutputBuf;

/**
 * Output from the EChangePIN and EUnblockPIN operations.
 */
class TTwoPINOutput
	{
public:
	// This could be for the current PIN or the unblocking PIN
	TPINValue iPINValueToCheck;
	TPINValue iNewPINValue;
	};

/** */
typedef TPckgBuf<TTwoPINOutput> TTwoPINOutputBuf;

/** */
typedef TPckgBuf<TPINValue> TPINValueBuf;



/**
 * Output from the EUnblockPINInClear operation.
 */
class TUnblockPINInClearOutput
	{
public:
	// This could be for the current PIN or the unblocking PIN
	TUnblockPINValue iPINValueToCheck;
	TPINValue iNewPINValue;
	};

/** */
typedef TPckgBuf<TUnblockPINInClearOutput> TUnblockPINInClearOutputBuf;


/**
 * This class is used by the reference implementation of the security dialog,
 * CRefSecurityDialog, to pack the notifier buffer for the Server
 * Authentication Failure Dialog operation, and by the notifier server plugin
 * to unpack the buffer.
 */

class CServerAuthenticationFailureInput : public CBase
	{
public:

	virtual ~CServerAuthenticationFailureInput();

	/**	
	 * @internalComponent
	 * 
	* Static constructor function that uses the data passed in the call to
	* MSecurityDialog::ServerAuthenticationFailure to instantiate the object.
	*
	* Depending on the value of aCreateOwnBuffers, the object can allocate
	* new buffers with copies of the server name and encoded cert data, or it
	* can use pointers to the buffers for the existing descriptors.  It should
	* only use the existing buffers if they won't be freed before an input
	* buffer for the notifier is created using PackBufferL.
	*
	* @param aServerName		Name of the server with the certificate that
	*							failed authentication.
	* @param aFailureReason		Reason for certificate authentication failure.
	* @param aEncodedCert		Encoded X509 Certificate data.
	* @param aCreateOwnBuffers	Indicates whether new heap buffers are needed.
	*
	* @return					The new CServerAuthenticationFailureInput
	*							object. */

	static CServerAuthenticationFailureInput* NewL(const TDesC8& aServerName,const TValidationError& aFailureReason, const TDesC8& aEncodedCert, const TBool aCreateOwnBuffers = EFalse);
	/** @internalComponent */
	static CServerAuthenticationFailureInput* NewLC(const TDesC8& aServerName,const TValidationError& aFailureReason, const TDesC8& aEncodedCert, const TBool aCreateOwnBuffers = EFalse);

	/**	
	* Static constructor function that constructs the object using the data
	* in a supplied buffer.  This can be used in the implementation of a
	* plugin for the extended notifier framework; the buffer supplied to the
	* notifier for this dialog operation will be in the correct format to
	* construct the class with this function.
	*
	* Depending on the value of aCreateOwnBuffers, new buffers can be
	* allocated to hold copies of the server name and encoded cert data, or
	* the object can use pointers to the data positions within the input
	* buffer.  It should only use the existing buffer if it will persist
	* until the data is no longer needed.
	*
	* @param aBuffer			A data buffer containing the data needed
	*							for the dialog.
	* @param aCreateOwnBuffers	Indicates whether new heap buffers are
	*							needed.
	*
	* @return					The new CServerAuthenticationFailureInput
	*							object. */

	IMPORT_C static CServerAuthenticationFailureInput* NewL(const TDesC8& aBuffer, const TBool aCreateOwnBuffers = EFalse);
	IMPORT_C static CServerAuthenticationFailureInput* NewLC(const TDesC8& aBuffer, const TBool aCreateOwnBuffers = EFalse);
	

	/**
	* Gets the reason for validation failure.	
	*
	* @return	The failure reason. */
	IMPORT_C TValidationError FailureReason();

	/**
	* Sets the supplied TPtrC8 to point to the buffer for the server name.	
	*
	* @param aServerNamePtr	A reference to a TPtrC8 to set to point to
	*						the server name data. */
	IMPORT_C void GetServerName(TPtrC8& aServerNamePtr);

	/**
	* Sets the supplied TPtrC8 to point to the buffer for the encoded cert.	
	*
	* @param aServerNamePtr	A reference to a TPtrC8 to set to point to
	*						the encoded cert data. */
	IMPORT_C void GetEncodedCert(TPtrC8& aEncodedCertPtr);
	
	/**
	 * @internalComponent
	 * 
	* Allocates and fills a heap buffer with the data to be sent to the
	* notifier server.  Ownership of this buffer passes to the calling
	* method.
	*
	* @return	The allocated buffer. */

	HBufC8* PackBufferL() const;
	 
private:
	CServerAuthenticationFailureInput();
	
	void ConstructL(const TDesC8& aServerName,const TValidationError& aFailureReason, const TDesC8& aEncodedCert, const TBool aCreateOwnBuffers = EFalse);
	void ConstructL(const TDesC8& aBuffer, const TBool aCreateOwnBuffers = EFalse);

private:
	/** EServerAuthenticationFailure */
	TSecurityDialogNotification iOperation;

	/** Failure Reason */
	TValidationError iFailureReason;

	/** Server name */
	HBufC8* iServerName;
	TPtrC8 iServerNamePtr;

	/** Encoded Certificate */
	HBufC8* iEncodedCert;
	TPtrC8 iEncodedCertPtr;

	};

#endif
