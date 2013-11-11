/*
* Copyright (c) 2008-2010 Nokia Corporation and/or its subsidiary(-ies).
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

#include "referencepolicyplugin.h"
#include <centralrepository.h>
#include <e32std.h>
#include <usb/usblogger.h>
#include <usb/hostms/msmm_policy_def.h>
#include "refppnotificationman.h"
#include "srvpanic.h"
#include "OstTraceDefinitions.h"
#ifdef OST_TRACE_COMPILER_IN_USE
#include "referencepolicypluginTraces.h"
#endif

 

//  Global Variables
const TUid KHostMsRepositoryUid = {0x10285c46};
const TUint32 KPermittedRangeUid = 0x00010000;
const TUint32 KForbiddenListUid = 0x00010001;
const TUint32 KMaxHistoryCountUid = 0x00010002;
const TUint32 KOTGCapableSuspendTimeUid = 0x00010003;
const TUint32 KMediaPollingTimeUid = 0x00010004;
const TUint32 KHistoryCountUid = 0x00010100;
const TUint32 KFirstHistoryUid = 0x00010101;

const TUint KHistoryGranularity = 0x8;
const TUint KPermittedDrvRangeBufLen = 0x3;

CReferencePolicyPlugin::~CReferencePolicyPlugin()
    {
    OstTraceFunctionEntry0( REF_CREFERENCEPOLICYPLUGIN_CREFERENCEPOLICYPLUGIN_DES_ENTRY );
    
    Cancel();
    ClearHistory(); // Remove all buffered history record.
    delete iRepository;
    delete iNotificationMan;
    iFs.Close();
    OstTraceFunctionExit0( REF_CREFERENCEPOLICYPLUGIN_CREFERENCEPOLICYPLUGIN_DES_EXIT );
    }

CReferencePolicyPlugin* CReferencePolicyPlugin::NewL()
    {
    OstTraceFunctionEntry0( REF_CREFERENCEPOLICYPLUGIN_NEWL_ENTRY );
    
    CReferencePolicyPlugin* self = new (ELeave) CReferencePolicyPlugin;
    CleanupStack::PushL(self);
    self->ConstructL();
    CleanupStack::Pop(self);
    
    OstTraceFunctionExit0( REF_CREFERENCEPOLICYPLUGIN_NEWL_EXIT );
    return self;
    }

void CReferencePolicyPlugin::RetrieveDriveLetterL(TText& aDriveName,
        const TPolicyRequestData& aData, TRequestStatus& aStatus)
    {
    OstTraceFunctionEntry0( REF_CREFERENCEPOLICYPLUGIN_RETRIEVEDRIVELETTERL_TTEXT_TPOLICYREQUESTDATA_TREQUESTSTATUS_ENTRY );
    
    Cancel();
    aStatus = KRequestPending;
    iClientStatus = &aStatus;    

    RetrieveDriveLetterL(aDriveName, aData);
    // In a licensee owned policy plugin, it shall complete client 
    // request in RunL() in general 
    Complete(KErrNone);
    OstTraceFunctionExit0( REF_CREFERENCEPOLICYPLUGIN_RETRIEVEDRIVELETTERL_TTEXT_TPOLICYREQUESTDATA_TREQUESTSTATUS_EXIT );
    }

void CReferencePolicyPlugin::CancelRetrieveDriveLetter()
    {
    OstTraceFunctionEntry0( REF_CREFERENCEPOLICYPLUGIN_CANCELRETRIEVEDRIVELETTER_ENTRY );
    
    Cancel();
    OstTraceFunctionExit0( REF_CREFERENCEPOLICYPLUGIN_CANCELRETRIEVEDRIVELETTER_EXIT );
    }

void CReferencePolicyPlugin::SaveLatestMountInfoL(
        const TPolicyMountRecord& aData, TRequestStatus& aStatus)
    {
    OstTraceFunctionEntry0( REF_CREFERENCEPOLICYPLUGIN_SAVELATESTMOUNTINFOL_TPOLICYMOUNTRECORD_TREQUESTSTATUS_ENTRY );
    
    Cancel();
    aStatus = KRequestPending;
    iClientStatus = &aStatus;

    SaveLatestMountInfoL(aData);
    // In a licensee owned policy plugin, it shall complete client 
    // request in RunL() in general 
    Complete(KErrNone);
    OstTraceFunctionExit0( REF_CREFERENCEPOLICYPLUGIN_SAVELATESTMOUNTINFOL_TPOLICYMOUNTRECORD_TREQUESTSTATUS_EXIT );
    }

void CReferencePolicyPlugin::CancelSaveLatestMountInfo()
    {
    OstTraceFunctionEntry0( REF_CREFERENCEPOLICYPLUGIN_CANCELSAVELATESTMOUNTINFO_ENTRY );
    
    Cancel();
    OstTraceFunctionExit0( REF_CREFERENCEPOLICYPLUGIN_CANCELSAVELATESTMOUNTINFO_EXIT );
    }

void CReferencePolicyPlugin::SendErrorNotificationL(
        const THostMsErrData& aErrData)
    {
    OstTraceFunctionEntry0( REF_CREFERENCEPOLICYPLUGIN_SENDERRORNOTIFICATIONL_ENTRY );
    
    iNotificationMan->SendErrorNotificationL(aErrData);
    OstTraceFunctionExit0( REF_CREFERENCEPOLICYPLUGIN_SENDERRORNOTIFICATIONL_EXIT );
    }

void CReferencePolicyPlugin::GetSuspensionPolicy(TSuspensionPolicy& aPolicy)
    {
    OstTraceFunctionEntry0( REF_CREFERENCEPOLICYPLUGIN_GETSUSPENSIONPOLICY_ENTRY );
    
    aPolicy = iSuspensionPolicy;
    OstTraceFunctionExit0( REF_CREFERENCEPOLICYPLUGIN_GETSUSPENSIONPOLICY_EXIT );
    }

void CReferencePolicyPlugin::DoCancel()
    {
    OstTraceFunctionEntry0( REF_CREFERENCEPOLICYPLUGIN_DOCANCEL_ENTRY );
    
    // No more work need to do in current implementation of reference
    // policy plugin. 
    // In a licensee owned policy plugin, it shall complete client 
    // request here with KErrCancel.
    OstTraceFunctionExit0( REF_CREFERENCEPOLICYPLUGIN_DOCANCEL_EXIT );
    }

void CReferencePolicyPlugin::RunL()
    {
    OstTraceFunctionEntry0( REF_CREFERENCEPOLICYPLUGIN_RUNL_ENTRY );
    
    // No more work need to do in current implementation of reference
    // policy plugin. 
    // In a licensee owned policy plugin, it shall complete client 
    // request here with a proper error code.
    OstTraceFunctionExit0( REF_CREFERENCEPOLICYPLUGIN_RUNL_EXIT );
    }

CReferencePolicyPlugin::CReferencePolicyPlugin() :
CMsmmPolicyPluginBase(),
iHistory(KHistoryGranularity)
    {
    OstTraceFunctionEntry0( REF_CREFERENCEPOLICYPLUGIN_CREFERENCEPOLICYPLUGIN_CONS_ENTRY );
    
    CActiveScheduler::Add(this);
    OstTraceFunctionExit0( REF_CREFERENCEPOLICYPLUGIN_CREFERENCEPOLICYPLUGIN_CONS_EXIT );
    }

void CReferencePolicyPlugin::ConstructL()
    {
    OstTraceFunctionEntry0( REF_CREFERENCEPOLICYPLUGIN_CONSTRUCTL_ENTRY );
    
    iRepository = CRepository::NewL(KHostMsRepositoryUid);
    User::LeaveIfError(iFs.Connect());
    iNotificationMan = CMsmmPolicyNotificationManager::NewL();
    RetrieveHistoryL();
    AvailableDriveListL();
    TInt value = 0;
    User::LeaveIfError(iRepository->Get(
            KOTGCapableSuspendTimeUid, value));
    iSuspensionPolicy.iOtgSuspendTime = value;
    User::LeaveIfError(iRepository->Get(
            KMediaPollingTimeUid, value));
    iSuspensionPolicy.iStatusPollingInterval = value;
    OstTraceFunctionExit0( REF_CREFERENCEPOLICYPLUGIN_CONSTRUCTL_EXIT );
    }

void CReferencePolicyPlugin::RetrieveDriveLetterL(TText& aDriveName,
        const TPolicyRequestData& aData)
    {
    OstTraceFunctionEntry0( REF_CREFERENCEPOLICYPLUGIN_RETRIEVEDRIVELETTERL_TTEXT_TPOLICYREQUESTDATA_ENTRY );
    
    TDriveList availableNames;
    FilterFsForbiddenDriveListL(availableNames);

    if (!availableNames.Length())
        {
        // Not any drive letter available
        User::Leave(KErrNotFound);
        }

    // According to REQ8922, When a particular Logical Unit is mounted 
    // for the first time, RefPP shall always try to allocate an 
    // available and unused drive letter to it. Only if such a drive letter
    // can not be found, RefPP shall use the first one in available name
    // list;
    
    // Initialize aDriveName by the first available drive letter
    aDriveName = availableNames[0];
    // Find first such drive letter from available letter list. If it can
    // be found, it will be used.
    FindFirstNotUsedDriveLetter(availableNames, aDriveName);    
    // Search history record
    TInt historyIndex = SearchHistoryByLogicUnit(aData);
    if (KErrNotFound != historyIndex)
        {
        // Find a match one in history
        const TPolicyMountRecord& history = *iHistory[historyIndex];
        TInt location = availableNames.Locate(TChar(history.iDriveName));
        if (KErrNotFound != location)
            {
            // And it is available now. RefPP allocate it to the 
            // LU currently mounted.
            aDriveName = history.iDriveName;
            }
        }
    OstTraceFunctionExit0( REF_CREFERENCEPOLICYPLUGIN_RETRIEVEDRIVELETTERL_TTEXT_TPOLICYREQUESTDATA_EXIT );
    }

void CReferencePolicyPlugin::SaveLatestMountInfoL(
        const TPolicyMountRecord& aData)
    {
  OstTraceFunctionEntry0( REF_CREFERENCEPOLICYPLUGIN_SAVELATESTMOUNTINFOL_TPOLICYMOUNTRECORD_ENTRY );
  
    if (iMaxHistoryRecCount == 0) // This policy disable history
        {
        OstTraceFunctionExit0( REF_CREFERENCEPOLICYPLUGIN_SAVELATESTMOUNTINFOL_TPOLICYMOUNTRECORD_EXIT );
        return;
        }
    
    TPolicyMountRecord* historyRecord = 
            new (ELeave) TPolicyMountRecord(aData);
    CleanupStack::PushL(historyRecord);
    TInt historyIndex = SearchHistoryByLogicUnit(aData.iLogicUnit);
    if (KErrNotFound == historyIndex)
    	{
        // No matched record exist
		if (iHistory.Count() == iMaxHistoryRecCount)
			{
			// Remove the oldest entity
			delete iHistory[0];
			iHistory.Remove(0);
			}
    	}
    else
    	{
    	// Remove the replaced entity
    	delete iHistory[historyIndex];
    	iHistory.Remove(historyIndex);
    	}
    iHistory.AppendL(historyRecord); // Push the new entity
    CleanupStack::Pop(historyRecord);

    TUint32 historyRecordUid = KFirstHistoryUid;
    User::LeaveIfError(iRepository->Set(KHistoryCountUid, iHistory.Count()));
    for (TInt index = 0; index < iHistory.Count(); index++)
        {
        TPckg<TPolicyMountRecord> historyPckg(*iHistory[index]);
        User::LeaveIfError(iRepository->Set(historyRecordUid++, historyPckg));
        }
    OstTraceFunctionExit0( REF_CREFERENCEPOLICYPLUGIN_SAVELATESTMOUNTINFOL_TPOLICYMOUNTRECORD_EXIT_DUP1 );
    }

void CReferencePolicyPlugin::Complete(TInt aError)
    {
    OstTraceFunctionEntry0( REF_CREFERENCEPOLICYPLUGIN_COMPLETE_ENTRY );
    
    User::RequestComplete(iClientStatus, aError);
    OstTraceFunctionExit0( REF_CREFERENCEPOLICYPLUGIN_COMPLETE_EXIT );
    }

void CReferencePolicyPlugin::PrepareAvailableDriveList()
    {
    OstTraceFunctionEntry0( REF_CREFERENCEPOLICYPLUGIN_PREPAREAVAILABLEDRIVELIST_ENTRY );
    
    iAvailableDrvList.SetLength(KMaxDrives);
    iAvailableDrvList.Fill(0, KMaxDrives);
    OstTraceFunctionExit0( REF_CREFERENCEPOLICYPLUGIN_PREPAREAVAILABLEDRIVELIST_EXIT );
    }

void CReferencePolicyPlugin::AvailableDriveListL()
    {
    OstTraceFunctionEntry0( REF_CREFERENCEPOLICYPLUGIN_AVAILABLEDRIVELISTL_ENTRY );
    
    TBuf8<KPermittedDrvRangeBufLen> permittedRange;
    TDriveList forbiddenList;

    PrepareAvailableDriveList();

    User::LeaveIfError(iRepository->Get(KPermittedRangeUid, permittedRange));
    User::LeaveIfError(iRepository->Get(KForbiddenListUid, forbiddenList));

    for (TInt index = 'A'; index <= 'Z'; index++ )
        {
        if ((index >= permittedRange[0]) && (index <= permittedRange[1]))
            {
            if (KErrNotFound == forbiddenList.Locate(TChar(index)))
                {
                // Permitted
                iAvailableDrvList[index - 'A'] = 0x01;
                }
            }
        }
    OstTraceFunctionExit0( REF_CREFERENCEPOLICYPLUGIN_AVAILABLEDRIVELISTL_EXIT );
    }

void CReferencePolicyPlugin::FilterFsForbiddenDriveListL(
        TDriveList& aAvailableNames)
    {
    OstTraceFunctionEntry0( REF_CREFERENCEPOLICYPLUGIN_FILTERFSFORBIDDENDRIVELISTL_ENTRY );
    
    TDriveList names;
    names.SetLength(KMaxDrives);

    TDriveList drives;
    User::LeaveIfError(iFs.DriveList(drives));

    TUint count(0);
    for (TInt index = 0; index < KMaxDrives; index++ )
        {
        if ((drives[index] == 0x0) && (iAvailableDrvList[index]))
            {
            names[count++] = index+'A';
            }
        }
    names.SetLength(count);
    aAvailableNames = names;
    OstTraceFunctionExit0( REF_CREFERENCEPOLICYPLUGIN_FILTERFSFORBIDDENDRIVELISTL_EXIT );
    }

void CReferencePolicyPlugin::FindFirstNotUsedDriveLetter(
        const TDriveList& aAvailableNames,
        TText& aDriveName)
    {
    OstTraceFunctionEntry0( REF_CREFERENCEPOLICYPLUGIN_FINDFIRSTNOTUSEDDRIVELETTER_ENTRY );
    
    TDriveList usedLetter;
    TUint index = 0;
    for (index = 0; index < iHistory.Count(); index++)
        {
        const TPolicyMountRecord& record = *iHistory[index];
        usedLetter.Append(TChar(record.iDriveName));
        }
    for (index = 0; index < aAvailableNames.Length(); index++)
        {
        if (usedLetter.Locate(aAvailableNames[index]) == KErrNotFound)
            {
            aDriveName = aAvailableNames[index];
            OstTraceFunctionExit0( REF_CREFERENCEPOLICYPLUGIN_FINDFIRSTNOTUSEDDRIVELETTER_EXIT );
            return; // A unused drive letter found out
            }
        }
    OstTraceFunctionExit0( REF_CREFERENCEPOLICYPLUGIN_FINDFIRSTNOTUSEDDRIVELETTER_EXIT_DUP1 );
    }

// Retrieve history from CR
void CReferencePolicyPlugin::RetrieveHistoryL()
    {
    OstTraceFunctionEntry0( REF_CREFERENCEPOLICYPLUGIN_RETRIEVEHISTORYL_ENTRY );
    
    // Read history record number from CR
    TInt historyCount(0);
    User::LeaveIfError(
            iRepository->Get(KMaxHistoryCountUid, iMaxHistoryRecCount));
    User::LeaveIfError(iRepository->Get(KHistoryCountUid, historyCount));

    TUint32 historyRecordUid = KFirstHistoryUid;
    if (historyCount)
        {
        TPolicyMountRecord historyRecord;
        TPckg<TPolicyMountRecord> historyArray(historyRecord);        
        for (TInt index = 0; index < historyCount; index++)
            {
            User::LeaveIfError(iRepository->Get(historyRecordUid++, 
                    historyArray));
            TPolicyMountRecord* record = new (ELeave) TPolicyMountRecord;
            memcpy(record, &historyRecord, sizeof(TPolicyMountRecord));
            CleanupStack::PushL(record);
            iHistory.AppendL(record);
            CleanupStack::Pop(record);
            }
        }
    OstTraceFunctionExit0( REF_CREFERENCEPOLICYPLUGIN_RETRIEVEHISTORYL_EXIT );
    }

// Remove all buffered history
void CReferencePolicyPlugin::ClearHistory()
    {
    OstTraceFunctionEntry0( REF_CREFERENCEPOLICYPLUGIN_CLEARHISTORY_ENTRY );
    
    iHistory.ResetAndDestroy();
    iHistory.Close();
    OstTraceFunctionExit0( REF_CREFERENCEPOLICYPLUGIN_CLEARHISTORY_EXIT );
    }

// Search in history for a logic unit	
TInt CReferencePolicyPlugin::SearchHistoryByLogicUnit(
        const TPolicyRequestData& aLogicUnit) const
    {
    OstTraceFunctionEntry0( REF_CREFERENCEPOLICYPLUGIN_SEARCHHISTORYBYLOGICUNIT_ENTRY );
    
    TInt ret(KErrNotFound);
    TUint count = iHistory.Count();
    for (TUint index = 0; index < count; index ++)
        {
        const TPolicyMountRecord& record = *iHistory[index];
        const TPolicyRequestData& logicalUnit = record.iLogicUnit;

        if ((logicalUnit.iVendorId == aLogicUnit.iVendorId) &&
                (logicalUnit.iProductId == aLogicUnit.iProductId) &&
                (logicalUnit.iBcdDevice == aLogicUnit.iBcdDevice) &&
                (logicalUnit.iConfigurationNumber == aLogicUnit.iConfigurationNumber) &&
                (logicalUnit.iInterfaceNumber == aLogicUnit.iInterfaceNumber) &&
                (logicalUnit.iSerialNumber == aLogicUnit.iSerialNumber) &&
                (logicalUnit.iOtgInformation == aLogicUnit.iOtgInformation))
            {
            // Matched
            OstTraceFunctionExit0( REF_CREFERENCEPOLICYPLUGIN_SEARCHHISTORYBYLOGICUNIT_EXIT );
            return index;
            }
        }
    // Can't find any matched records
    OstTraceFunctionExit0( REF_CREFERENCEPOLICYPLUGIN_SEARCHHISTORYBYLOGICUNIT_EXIT_DUP1 );
    return ret;
    }

// End of file
