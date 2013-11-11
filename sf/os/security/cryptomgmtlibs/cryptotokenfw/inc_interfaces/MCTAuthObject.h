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

#ifndef __MCTAOSTORE_H__
#define __MCTAOSTORE_H__

#include <ct.h>

/** The UID for the authentication object interface. */
const TInt KCTInterfaceAuthenticationObject = 0x101F51AE;

/**
 * A timeout value for an auth object indicating that it stays open until
 * explicity closed.
 */
const TInt KTimeoutNever = -1;

/**
 * A timeout value for an auth object indicating that the authentication data
 * must be entered every time the protected objects are used.
 */
const TInt KTimeoutImmediate = 0;

/**
 * The status of an authentication object.
 */
enum TCTAuthenticationStatus
	{
	/** The authentication object is enabled. If it is not enabled, the objects protected 
	* by this authentication object can be accessed without authentication. */
	EEnabled		= 0x80,
	/** The reference data cannot be changed. */
	EChangeDisabled	= 0x40,
	/** The authentication cannot be unblocked. */
	EUnblockDisabled	= 0x20,
	/** The authentication object can be disabled. */
	EDisableAllowed	= 0x10,
	/** The authentication object is blocked, meaning that the
	* unblocking PIN must be entered to re-enable the authentication object. */
	EAuthObjectBlocked= 0x08,
	};	

/** 
 * This class allows authentication objects to be queried and manipulated.
 * 
 * Authentication objects are obtained from the MCTAuthenticationObjectList class, 
 * which is the class returned as the token interface. 
 */
class MCTAuthenticationObject: public MCTTokenObject
	{
public:
	/** Constructor */
	inline MCTAuthenticationObject(MCTToken& aToken);

	// Listing Protected Objects
	/** 
	 * Gets a list of all the objects that this authentication object protects.
	 * 
	 * @param aObjects	The returned objects will be appended to this array.
	 * @param aStatus	Completed with the return code when the operation completes. 
	 */
	virtual void ListProtectedObjects(RMPointerArray<MCTTokenObject>& aObjects, TRequestStatus& aStatus) = 0;

	/** Cancels an asynchronous ListProtectedObjects() operation. */
	virtual void CancelListProtectedObjects() = 0;
	
	// Changing the reference data
	/** 
	 * Changes the reference data (e.g. PIN value).
	 * 
	 * The security dialog component will prompt for the old and new reference data.
	 *
	 * The authentication object may not allow its reference data to be changed -
	 * this is indicated by the EChangeDisabled bit of Status() being set.
	 * 
	 * @param aStatus	Completed with the return code when the operation completes.
	 *
	 * @leave KErrNotFound If no reference data was originally set.
	 */
	virtual void ChangeReferenceData(TRequestStatus &aStatus) = 0;

	/** Cancels an asynchronous ChangeReferenceData() operation. */
	virtual void CancelChangeReferenceData() = 0;
	
	/** 
	 * Unblocks the authentication object.
	 * 
	 * The security dialog component will prompt for the unblocking
	 * authentication object.
	 *
	 * It is only valid to call this method when the authentication object is
	 * blocked, ie when the EAuthObjectBlocked bit of Status() is set.
	 * 
	 * The object may not allow unblocking (either because of failed unblock
	 * attempts or because it doesn't support the concept) - this is indicated by
	 * the EUnblockDisabled bit of Status() being set.
	 * 
	 * @param aStatus	Completed with the return code when the operation completes. 
	 */
	virtual void Unblock(TRequestStatus &aStatus) = 0;

	/** Cancels an asynchronous Unblock() operation. */
	virtual void CancelUnblock() = 0;
	
	/** 
	 * Gets the status of the authentication object.
	 * 
	 * @return	See TCTAuthenticationStatus() for the format of the return value. 
	 */
	virtual TUint32 Status() const = 0;

	// Enabling and Disabling
	/** 
	 * Disables the authentication object.
	 *
	 *  It is only valid to call this method if the object is enabled, indicated
	 *  by the EEnabled bit of Status() being set.
	 *
	 *  Also, the authentication object may not support being enabled/disabled -
	 *  the EDisableAllowed bit of Status() must be set for this to work.
	 *  
	 * @param aStatus	Completed with the return code when the operation completes. 
	 */
	virtual void Disable(TRequestStatus &aStatus) = 0;

	/** Cancels an asynchronous Disable() operation. */
	virtual void CancelDisable() = 0;

	/** 
	 * Enables the authentication object.
	 *
	 *  It is only valid to call this method if the object is disabled, indicated
	 *  by the EEnabled bit of Status() being clear.
	 *
	 *  Also, the authentication object may not support being enabled/disabled -
	 *  the EDisableAllowed bit of Status() must be set for this to work.
	 *  
	 * @param aStatus	Completed with the return code when the operation completes. 
	 */
	virtual void Enable(TRequestStatus &aStatus) = 0;

	// Cancel an ongoing Enable operation
	/** Cancels an asynchronous Enable() operation. */
	virtual void CancelEnable() = 0;
	
	/** 
	 * Opens the authentication object.
	 * 
	 * This means that the protected objects can be accessed without provoking
	 * the authentication mechanism for the duration of the timeout period.
	 * 	
	 * Note that it is not strictly necessary to call this function, as the
	 * authentication object will be opened when any operation that needs it to
	 * be opened is called, but it is sometimes useful to control the timing of
	 * authentication dialogs. Note also that this function will do nothing if
	 * the authentication object is open, or if the authentication object
	 * requires the authentication data to be entered every time.
	 * 
	 * @param aStatus	Completed with the return code when the operation completes.
	 */
	virtual void Open(TRequestStatus& aStatus) = 0;

	/** 
	 * Closes an authentication object. 
	 * 
	 * @param aStatus	Completed with the return code when the operation completes.
	 */
	virtual void Close(TRequestStatus& aStatus) = 0;

	/** 
	 * Gets the amount of time in seconds that the authentication object
	 * will remain open for, or 0 if it is closed.
	 * 
	 * @param aStime		Time in seconds when the operation completes.
	 * @param aStatus	Completed with the return code when the operation completes.
	 */
	virtual void TimeRemaining(TInt& aStime, TRequestStatus& aStatus) = 0;
		
	/** 
	 * Sets the time in seconds for this authentication object. 
	 * 
	 * Permitted values include 0, meaning always ask, and -1,
	 * meaning until it's explicitly closed. Particular authentication
	 * objects might restrict the range of values permitted.
	 * 
	 * @param aTime		Time in seconds
	 * @param aStatus	Completed with the return code when the operation completes
	 *
	 * @leave KErrArgument If the timeout specified is invalid.
	 */
	virtual void SetTimeout(TInt aTime, TRequestStatus& aStatus) = 0;

	/** 
	 * Gets the current timeout value, in seconds. 
	 * 
	 * For an explanation of the values, see SetTimeout().
	 * 
	 * @param aTime		Time in seconds.
	 * @param aStatus	Completed with the return code when the operation completes.
	 */
	virtual void Timeout(TInt& aTime, TRequestStatus& aStatus) = 0;
		
	/** Cancels an asynchronous Open() operation. */
	virtual void CancelOpen() {};

	/** Cancels an asynchronous Close() operation. */
  	virtual void CancelClose() {};

	/** Cancels an asynchronous TimeRemaining() operation. */
	virtual void CancelTimeRemaining() {};
	
	/** Cancels an asynchronous SetTimeout() operation. */
	virtual void CancelSetTimeout() {};

	/** Cancels an asynchronous Timeout() operation. */
	virtual void CancelTimeout() {};

	};

/** 
 * An interface that enables clients to list all the authentication objects on 
 * a token, find out which objects they protect, and change/unblock/enable/disable them.
 * 
 * It may be used to implement a 'PIN manager' control panel item. This could 
 * list the PINs (by label), allow one to be selected, list what it is used for 
 * (including information such as what operations on that object are protected 
 * by that PIN), and then allow the user to change, enable, disable, and unblock 
 * the PIN.
 * 
 * Note that no PINs appear anywhere in this API. The code implementing this 
 * API will display a PIN dialog via the secure dialog interface when a PIN is 
 * required. The main advantage of this is that if some hostile code were to 
 * capture a PIN, it couldn't do anything with it as none of the APIs take a 
 * PIN. A secondary benefit is that the same API can work with biometrics or 
 * other methods of authentication. 
 */
class MAuthenticationObjectList
	{
public:	
	/** 
	 * Gets a list of all the authenticaiton objects in the token.
	 * 
	 * This is an asynchronous request.
	 * 
	 * @param aAuthObjects	On return, a list of all the authentication objects.
	 * @param aStatus		The request status object; contains the result of the List() request 
	 * 						when complete. Set to KErrCancel if an outstanding request is cancelled. 
	 */
	// @param aAuthObjects	The returned authentication objects will be appended to this array
	// @param aStatus		This will be completed with the final status code
	virtual void List(RMPointerArray<MCTAuthenticationObject>& aAuthObjects,	TRequestStatus& aStatus) = 0;

	/** 
	 * Cancels an asynchronous List() operation.
	 * 
	 * The operation completes with KErrCancel. 
	 */
	virtual void CancelList() = 0;
	};


/**
 * The class returned as the token interface.
 * 
 * The class does not define any extra member functions or data. 
 */
class MCTAuthenticationObjectList : public MCTTokenInterface, 
									public MAuthenticationObjectList
	{
	};

inline MCTAuthenticationObject::MCTAuthenticationObject(MCTToken& aToken)
		: MCTTokenObject(aToken)
	{
	}

#endif //__MCTAOSTORE_H__
