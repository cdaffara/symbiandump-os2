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
 @internalComponent
*/

#ifndef REFERENCEPOLICYPLUGIN_H
#define REFERENCEPOLICYPLUGIN_H

#include <usb/hostms/msmmpolicypluginbase.h>
#include <f32file.h>

typedef RPointerArray<TPolicyMountRecord> RMountRecordArray;

class CRepository;
class CMsmmPolicyNotificationManager;

NONSHARABLE_CLASS(CReferencePolicyPlugin) : 
    public CMsmmPolicyPluginBase
    {
public:
    virtual ~CReferencePolicyPlugin();
    
    static CReferencePolicyPlugin* NewL();

    // CMsmmPolicyPluginBase implementation
    void RetrieveDriveLetterL(TText& aDriveName, 
            const TPolicyRequestData& aData, TRequestStatus& aStatus);
    void CancelRetrieveDriveLetter();
    void SaveLatestMountInfoL(const TPolicyMountRecord& aData, 
            TRequestStatus& aStatus);
    void CancelSaveLatestMountInfo();
    void SendErrorNotificationL(const THostMsErrData& aErrData);
    void GetSuspensionPolicy(TSuspensionPolicy& aPolicy);
    
protected:

    // CActive implementation
    void DoCancel();
    void RunL();
    
private: // Constructor
    CReferencePolicyPlugin();
    void ConstructL();

    void RetrieveDriveLetterL(TText& aDriveName, 
            const TPolicyRequestData& aData);
    void SaveLatestMountInfoL(const TPolicyMountRecord& aData);
    
    void Complete(TInt aError = KErrNone);
    
    // Prepare available drive name list buffer
    void PrepareAvailableDriveList();
    // Get available drive names by policy (Forbidden list will be removed)
    void AvailableDriveListL();
    // Filter F32 forbidden drive names from available list
    void FilterFsForbiddenDriveListL(TDriveList& aAvailableNames);
    // Called to filter the used drive letters
    void FindFirstNotUsedDriveLetter(
            const TDriveList& aAvailableNames,
            TText& aDriveName);
    // Retrieve history from CR
    void RetrieveHistoryL();
    // Remove all buffered history
    void ClearHistory();
    // Search in history array for a logic unit
    TInt SearchHistoryByLogicUnit(const TPolicyRequestData& aLogicUnit) const;

private:
    CRepository* iRepository;    // Owned
    TInt iMaxHistoryRecCount;
    TDriveList iAvailableDrvList;  // Drive list available by policy
    RMountRecordArray iHistory;  // Device history
    TSuspensionPolicy iSuspensionPolicy; // Suspension policy
    RFs iFs;
    CMsmmPolicyNotificationManager* iNotificationMan;
    TRequestStatus* iClientStatus; // No ownership
    };

#endif /*REFERENCEPOLICYPLUGIN_H*/
