/*
* Copyright (c) 2008-2009 Nokia Corporation and/or its subsidiary(-ies).
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

/**
 @file
 @publishedPartner
 @released
*/

#ifndef MSMMPOLICYPLUGINBASE_H
#define MSMMPOLICYPLUGINBASE_H

// INCLUDES
#include <e32std.h>
#include <e32base.h>
#include <ecom/ecom.h>
#include <usb/hostms/msmm_policy_def.h>
#include <usb/hostms/srverr.h>

// CLASS DECLARATION

/**
 CMsmmPolicyPluginBase
*/
class CMsmmPolicyPluginBase : public CActive
    {
public: // Constructors and destructor

	virtual ~CMsmmPolicyPluginBase();
	static CMsmmPolicyPluginBase* NewL();
    
public:
    /** 
     Called when the MSMM retrieves a drive letter to mount a logical unit on it.

     @param aDriveName This is the drive name buffer for the new logical unit. 
     The policy plugin uses it bring the drive name back.
     @param aData The data used for retrieve a drive name. It includes number of
     the new logical unit and the information of the USB mass storage function
     and the device that current logical unit belongs to.
     @param aStatus The reference of client request status object.
     */
    virtual void RetrieveDriveLetterL(TText& aDriveName, 
            const TPolicyRequestData& aData, TRequestStatus& aStatus) = 0;
    /** 
     Called to cancel a outstanding drive letter retrieving request.
     */
    virtual void CancelRetrieveDriveLetter() = 0;

    /** 
     Called when the MSMM saves mount operation informaion to policy plugin.

     @param aData This is the data that will be used to retrieve a drive letter
     and perform a mounting operation before. It will be saved
     @param aStatus The reference of client request status object. 
     */
    virtual void SaveLatestMountInfoL(const TPolicyMountRecord& aData, 
            TRequestStatus& aStatus) = 0;
    /** 
     Called to cancel a outstanding saving mount information request.
     */
    virtual void CancelSaveLatestMountInfo() = 0;
    
    /** 
     Called when the MSMM send a error notification to policy plugin.

     @param aData This is the data that describles the error.
     */
    virtual void SendErrorNotificationL(const THostMsErrData& aErrData) = 0;
    
    /** 
     Called when the MSMM need suspension policy passing to MSC interface.

     @param aPolicy This is the buffer for suspension policy data.
     */    
    virtual void GetSuspensionPolicy(TSuspensionPolicy& aPolicy) = 0;
    
protected:
    CMsmmPolicyPluginBase();
    
    // Derivde from CActive
    virtual void DoCancel() = 0;
    virtual void RunL() = 0;

private:
	TUid iDtor_ID_Key;
    };

#include "usb/hostms/msmmpolicypluginbase.inl"

#endif // MSMMPOLICYPLUGINBASE_H

// End of file
