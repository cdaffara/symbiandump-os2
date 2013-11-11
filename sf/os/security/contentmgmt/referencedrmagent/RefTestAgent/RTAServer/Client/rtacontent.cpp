/*
* Copyright (c) 2004-2009 Nokia Corporation and/or its subsidiary(-ies).
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
* sisregistry - client registry session interface implementation
*
*/


/**
 @file 
 @test
 @internalComponent
*/

#include <s32mem.h>
#include "clientserver.h"
#include "rtacontent.h"

using namespace ReferenceTestAgent;
using namespace ContentAccess;

EXPORT_C RRtaContent::RRtaContent()
	{
	
	}

EXPORT_C TInt RRtaContent::Open(const TDesC& aFileName, TUint aMode)
	{
	TPckg <TUint> fileModePckg(aMode);
	TInt err = RRtaClient::Connect();
	
	if(err == KErrNone)
		{
		if(IsDrmCapabilityEnforced())
			{
			err = SendReceive(ECreateContentMessageHandler, TIpcArgs());
			}
		else
			{
			err = SendReceive(ENoEnforceCreateContentMessageHandler, TIpcArgs());
			}
		}

	if(err == KErrNone)
		{
		ShareAuto();
		if(IsDrmCapabilityEnforced())
			{
			err = SendReceive(EOpenFile, TIpcArgs(&aFileName, &fileModePckg));
			}
		else
			{
			err = SendReceive(ENoEnforceOpenFile, TIpcArgs(&aFileName, &fileModePckg));
			}
		}
		
	// check final result
	if(err != KErrNone)
		{
		// either connect and open file or don't connect
		Close();
		}
	return err;
	}
	
EXPORT_C TInt RRtaContent::Open(RFile& aFile)
	{
	TIpcArgs ipcArgs;
	aFile.TransferToServer(ipcArgs, 0, 1);
	
	TInt err = RRtaClient::Connect();
	if(err == KErrNone)
		{
		if(IsDrmCapabilityEnforced())
			{
			err = SendReceive(ECreateContentMessageHandler, TIpcArgs());
			}
		else
			{
			err = SendReceive(ENoEnforceCreateContentMessageHandler, TIpcArgs());
			}
		}
		
	if(err == KErrNone)
		{
		ShareAuto();
		if(IsDrmCapabilityEnforced())
			{
			err = SendReceive(EOpenFileHandle, ipcArgs);
			}
		else
			{
			err = SendReceive(ENoEnforceOpenFileHandle, ipcArgs);
			}
		}

	if(err != KErrNone)
		{
		// either connect and open file or don't connect
		Close();
		}
	return err;
	}

EXPORT_C TInt RRtaContent::OpenContainer(const TDesC& aUniqueId) const
	{
	TInt err = KErrNone;
	
	if(IsDrmCapabilityEnforced())
		{
		err = SendReceive(EOpenContainer, TIpcArgs(&aUniqueId));
		}
	else
		{
		err = SendReceive(ENoEnforceOpenContainer, TIpcArgs(&aUniqueId));
		}
	return err;
	}
EXPORT_C TInt RRtaContent::CloseContainer() const
	{
	TInt err = KErrNone;
	
	if(IsDrmCapabilityEnforced())
		{
		err = SendReceive(ECloseContainer, TIpcArgs());
		}
	else
		{
		err = SendReceive(ENoEnforceCloseContainer, TIpcArgs());
		}
	return err;
	
	}
	
EXPORT_C void RRtaContent::GetEmbeddedObjectsL(RStreamablePtrArray< CEmbeddedObject > &aArray) const
	{
	TInt err = KErrOverflow;
	
	// arbitary length for array of embedded objects
	TInt length = 40 * KMaxPath;
	TPckg<TInt> lengthPckg(length);
	
	while(err == KErrOverflow)
		{
		HBufC8* transferBuffer = HBufC8::NewLC(length);
		TPtr8 transferPtr = transferBuffer->Des();
	
		// attempt to retrieve the attributes
		err = SendReceive(EGetEmbeddedObjects1, TIpcArgs(&transferPtr));	
		if(err == KErrOverflow)
			{
			// Find out the length required to receive the resulting attribute set
			lengthPckg.Copy(transferPtr.Left(lengthPckg.MaxLength()));	
			}
		else if(err == KErrNone)
			{
			// read in the attribute values from the buffer
			RDesReadStream readStream(transferPtr);
			CleanupClosePushL(readStream);
			aArray.InternalizeL(readStream);
			CleanupStack::PopAndDestroy(&readStream);
			}
		CleanupStack::PopAndDestroy(transferBuffer);	
		}
		
	// if any other error occured, leave
	User::LeaveIfError(err);
	}
	
EXPORT_C void RRtaContent::GetEmbeddedObjectsL(RStreamablePtrArray< CEmbeddedObject > &aArray, TEmbeddedType aType) const
	{
	// arbitary length for array of embedded objects
	TInt length = 40 * KMaxPath;
	TPckg<TInt> lengthPckg(length);
	
	TPckg<TEmbeddedType> typePckg(aType);
	
	TInt err = KErrOverflow;
	
	while(err == KErrOverflow)
		{
		HBufC8* transferBuffer = HBufC8::NewLC(length);
		TPtr8 transferPtr = transferBuffer->Des();
	
		// attempt to retrieve the attributes
		err = SendReceive(EGetEmbeddedObjects2, TIpcArgs(&transferPtr, &typePckg));	
		if(err == KErrOverflow)
			{
			// Find out the length required to receive the resulting attribute set
			lengthPckg.Copy(transferPtr.Left(lengthPckg.MaxLength()));	
			}
		else if(err == KErrNone)
			{
			// read in the attribute values from the buffer
			RDesReadStream readStream(transferPtr);
			CleanupClosePushL(readStream);
			aArray.InternalizeL(readStream);
			CleanupStack::PopAndDestroy(&readStream);
			}
		CleanupStack::PopAndDestroy(transferBuffer);	
		}
		
	// if any other error occured, leave
	User::LeaveIfError(err);
	}
	
EXPORT_C TInt RRtaContent::Search(RStreamablePtrArray< CEmbeddedObject > &aArray, const TDesC8 &aMimeType, TBool aRecursive) const
	{
	TRAPD(err, SearchL(aArray, aMimeType, aRecursive));
	return err;
	}
	
void RRtaContent::SearchL(RStreamablePtrArray< CEmbeddedObject > &aArray, const TDesC8 &aMimeType, TBool aRecursive) const
	{
	// arbitary length for array of embedded objects
	TInt length = 40 * KMaxPath;
	TPckg<TInt> lengthPckg(length);
	TPckg<TBool> recursivePckg(aRecursive);
	TInt err = KErrOverflow;
	
	while(err == KErrOverflow)
		{
		HBufC8* transferBuffer = HBufC8::NewLC(length);
		TPtr8 transferPtr = transferBuffer->Des();
	
		// attempt to retrieve the attributes
		err = SendReceive(EGetEmbeddedObjects1, TIpcArgs(&transferPtr, &aMimeType, &recursivePckg));	
		if(err == KErrOverflow)
			{
			// Find out the length required to receive the resulting attribute set
			lengthPckg.Copy(transferPtr.Left(lengthPckg.MaxLength()));	
			}
		else if(err == KErrNone)
			{
			// read in the attribute values from the buffer
			RDesReadStream readStream(transferPtr);
			CleanupClosePushL(readStream);
			aArray.InternalizeL(readStream);
			CleanupStack::PopAndDestroy(&readStream);
			}
		CleanupStack::PopAndDestroy(transferBuffer);	
		}
	}

EXPORT_C TInt RRtaContent::GetAttribute (const TDesC& aUniqueId, TInt aAttribute, TInt &aValue) const
	{
	TPckg <TInt> attributePckg(aAttribute);
	TPckg <TInt> valuePckg(aValue);			
	return SendReceive(EGetAttribute, TIpcArgs(&aUniqueId, &attributePckg, &valuePckg));	
	}
	
EXPORT_C TInt RRtaContent::GetAttributeSet (const TDesC& aUniqueId, RAttributeSet &aAttributeSet) const
	{
	TRAPD( err, GetAttributeSetL(aUniqueId, aAttributeSet) );
	return err;
	}

#ifdef SYMBIAN_ENABLE_SDP_WMDRM_SUPPORT
void RRtaContent::GetAttributeSetL (const TDesC& aUniqueId, RAttributeSet &aAttributeSet, TBool aWmdrmFlag) const     
#else  
void RRtaContent::GetAttributeSetL (const TDesC& aUniqueId, RAttributeSet &aAttributeSet) const
#endif //SYMBIAN_ENABLE_SDP_WMDRM_SUPPORT
	{
	TInt err = KErrOverflow;
	TInt length = 0;
	TPckg <TInt> lengthPckg(length);
	
	// Write the object out to a buffer, send to client
	CBufFlat* buf = CBufFlat::NewL(50);
	CleanupStack::PushL(buf);
	
	// create write stream
	RBufWriteStream writeStream(*buf);
	CleanupClosePushL(writeStream);

	// write the directory to the stream
	aAttributeSet.ExternalizeL(writeStream);
	CleanupStack::PopAndDestroy(&writeStream);
	
	
	TPtr8 bufPtr = buf->Ptr(0);
	// allocate arbitary receive buffer twice the size of the source buffer
	length = bufPtr.Length() * 2;
	
	while(err == KErrOverflow)
		{
		HBufC8* transferBuffer = HBufC8::NewLC(length);
		transferBuffer->Des().Copy(bufPtr);
		TPtr8 transferPtr = transferBuffer->Des();

#ifdef SYMBIAN_ENABLE_SDP_WMDRM_SUPPORT
        if(aWmdrmFlag)     
            err = SendReceive(EWMDRMGetAttributeSet, TIpcArgs(&aUniqueId, &transferPtr));     
        else     
            err = SendReceive(EGetAttributeSet, TIpcArgs(&aUniqueId, &transferPtr));     
#else 
		// attempt to retrieve the attributes
		err = SendReceive(EGetAttributeSet, TIpcArgs(&aUniqueId, &transferPtr));
#endif //SYMBIAN_ENABLE_SDP_WMDRM_SUPPORT
		if(err == KErrOverflow)
			{
			// Find out the length required to receive the resulting attribute set
			lengthPckg.Copy(transferPtr.Left(lengthPckg.MaxLength()));	
			}
		else if(err == KErrNone)
			{
			// read in the attribute values from the buffer
			RDesReadStream readStream(transferPtr);
			CleanupClosePushL(readStream);
			aAttributeSet.InternalizeL(readStream);
			CleanupStack::PopAndDestroy(&readStream);
			}
		CleanupStack::PopAndDestroy(transferBuffer);	
		}

	CleanupStack::PopAndDestroy(buf);
	}
		
EXPORT_C TInt RRtaContent::GetStringAttribute (const TDesC& aUniqueId, TInt aAttribute, TDes &aValue) const
	{
	TPckg <TInt> attributePckg(aAttribute);
	return SendReceive(EGetStringAttribute, TIpcArgs(&aUniqueId, &attributePckg, &aValue));
	}

EXPORT_C TInt RRtaContent::GetStringAttributeSet (const TDesC& aUniqueId, RStringAttributeSet &aStringAttributeSet) const
	{
	TRAPD( err, GetStringAttributeSetL(aUniqueId, aStringAttributeSet) );
	return err;
	}

#ifdef SYMBIAN_ENABLE_SDP_WMDRM_SUPPORT
void RRtaContent::GetStringAttributeSetL(const TDesC& aUniqueId, RStringAttributeSet &aStringAttributeSet, TBool aWmdrmFlag) const       
#else 
void RRtaContent::GetStringAttributeSetL (const TDesC& aUniqueId, RStringAttributeSet &aStringAttributeSet) const
#endif //SYMBIAN_ENABLE_SDP_WMDRM_SUPPORT
	{
	TInt err = KErrOverflow;
	TInt length = 0;
	TPckg <TInt> lengthPckg(length);
	
	// Write the object out to a buffer, send to client
	CBufFlat* buf = CBufFlat::NewL(50);
	CleanupStack::PushL(buf);
	
	// create write stream
	RBufWriteStream writeStream(*buf);
	CleanupClosePushL(writeStream);

	// write the directory to the stream
	aStringAttributeSet.ExternalizeL(writeStream);
	CleanupStack::PopAndDestroy(&writeStream);
	
	
	TPtr8 bufPtr = buf->Ptr(0);
	// allocate arbitary receive buffer much larger than the size of the source buffer
	length = bufPtr.Length() * 15;
	
	while(err == KErrOverflow)
		{
		HBufC8* transferBuffer = HBufC8::NewLC(length);
		transferBuffer->Des().Copy(bufPtr);
		TPtr8 transferPtr = transferBuffer->Des();
#ifdef SYMBIAN_ENABLE_SDP_WMDRM_SUPPORT
        if(aWmdrmFlag)     
            err = SendReceive(EWMDRMGetStringAttributeSet, TIpcArgs(&aUniqueId, &transferPtr));     
        else     
            err = SendReceive(EGetStringAttributeSet, TIpcArgs(&aUniqueId, &transferPtr));     
#else    
		// attempt to retrieve the attributes
		err = SendReceive(EGetStringAttributeSet, TIpcArgs(&aUniqueId, &transferPtr));	
#endif //SYMBIAN_ENABLE_SDP_WMDRM_SUPPORT
		if(err == KErrOverflow)
			{
			// Find out the length required to receive the resulting attribute set
			lengthPckg.Copy(transferPtr.Left(lengthPckg.MaxLength()));	
			}
		else if(err == KErrNone)
			{
			// read in the attribute values from the buffer
			RDesReadStream readStream(transferPtr);
			CleanupClosePushL(readStream);
			aStringAttributeSet.InternalizeL(readStream);
			CleanupStack::PopAndDestroy(&readStream);
			}
		CleanupStack::PopAndDestroy(transferBuffer);	
		}

	CleanupStack::PopAndDestroy(buf);
	}

EXPORT_C TInt RRtaContent::AgentSpecificCommand(TInt aCommand, const TDesC8& aInputBuffer, TDes8& aOutputBuffer) const
	{
	TInt err = KErrNone;
	TPckg <TInt> commandPckg(aCommand);	
	if(IsDrmCapabilityEnforced())
		{
		err = SendReceive(EContentAgentSpecificCommand, TIpcArgs(&commandPckg, &aInputBuffer, &aOutputBuffer));
		}
	else
		{
		err = SendReceive(ENoEnforceContentAgentSpecificCommand, TIpcArgs(&commandPckg, &aInputBuffer, &aOutputBuffer));
		}
	return err;	
	}

EXPORT_C void RRtaContent::NotifyStatusChangeL(ContentAccess::TEventMask aMask, const TDesC& aUniqueId) const
	{
	TPckg <TEventMask> eventMaskPckg(aMask);
	if(IsDrmCapabilityEnforced())
		{
		User::LeaveIfError(SendReceive(EContentNotifyStatusChange, TIpcArgs(&eventMaskPckg, &aUniqueId)));
		}
	else
		{
		User::LeaveIfError(SendReceive(ENoEnforceContentNotifyStatusChange, TIpcArgs(&eventMaskPckg, &aUniqueId)));
		}
	}

EXPORT_C TInt RRtaContent::CancelNotifyStatusChange(const TDesC& aUniqueId) const
	{
	TInt err = KErrNone;
	if(IsDrmCapabilityEnforced())
		{
		err = SendReceive(EContentCancelNotifyStatusChange, TIpcArgs(&aUniqueId));
		}
	else
		{
		err = SendReceive(ENoEnforceContentCancelNotifyStatusChange, TIpcArgs(&aUniqueId));
		}
	return err;
	}

EXPORT_C void RRtaContent::RequestRightsL(const TDesC& aUniqueId) const
	{	
	if(IsDrmCapabilityEnforced())
		{
		User::LeaveIfError(SendReceive(EContentRequestRights, TIpcArgs(&aUniqueId)));
		}
	else
		{
		User::LeaveIfError(SendReceive(ENoEnforceContentRequestRights, TIpcArgs(&aUniqueId)));
		}
	}

EXPORT_C TInt RRtaContent::CancelRequestRights(const TDesC& aUniqueId) const
	{
	TInt err = KErrNone;
	if(IsDrmCapabilityEnforced())
		{
		err = SendReceive(EContentCancelRequestRights, TIpcArgs(&aUniqueId));
		}
	else
		{
		err = SendReceive(ENoEnforceContentCancelRequestRights, TIpcArgs(&aUniqueId));
		}
	return err;
	}

EXPORT_C void RRtaContent::DisplayInfoL(ContentAccess::TDisplayInfo aInfo, const TDesC& aUniqueId) const
	{
	TPckg <TDisplayInfo> displayInfoPckg(aInfo);	
	if(IsDrmCapabilityEnforced())
		{
		User::LeaveIfError(SendReceive(EContentDisplayInfo, TIpcArgs(&displayInfoPckg, &aUniqueId)));
		}
	else
		{
		User::LeaveIfError(SendReceive(ENoEnforceContentDisplayInfo, TIpcArgs(&displayInfoPckg, &aUniqueId)));
		}
	}

EXPORT_C TInt RRtaContent::SetProperty(ContentAccess::TAgentProperty aProperty, TInt aValue) const		
	{
	TInt err = KErrNone;
	TPckg <TInt> valuePckg(aValue);
	TPckg <TAgentProperty> agentPropertyPckg(aProperty);
	if(IsDrmCapabilityEnforced())
		{
		err = SendReceive(EContentSetProperty, TIpcArgs(&agentPropertyPckg, &valuePckg));
		}
	else
		{
		err = SendReceive(ENoEnforceContentSetProperty, TIpcArgs(&agentPropertyPckg, &valuePckg));
		}
	return err;
	}

#ifdef SYMBIAN_ENABLE_SDP_WMDRM_SUPPORT     
      
EXPORT_C TInt RRtaContent::Open(const TDesC8& aHeaderData)     
    {     
    TInt err = RRtaClient::Connect();     
    if (err != KErrNone)     
        {     
        return err;     
        }     
         
    if(IsDrmCapabilityEnforced())     
        {     
        err = SendReceive(ECreateContentMessageHandler, TIpcArgs());     
        if(err != KErrNone)     
            {     
            return err;     
            }     
             
        err = SendReceive(EReadWMDRMHeaderData, TIpcArgs(&aHeaderData));     
        }     
             
    else     
        {     
        err = SendReceive(ENoEnforceCreateContentMessageHandler, TIpcArgs());     
        if(err != KErrNone)     
            {     
            return err;     
            }     
                 
        err = SendReceive(ENoEnforceReadWMDRMHeaderData, TIpcArgs(&aHeaderData));     
        }     
             
    return err;     
    }     
         
EXPORT_C TInt RRtaContent::GetWmdrmAttribute(const TDesC& /*aUniqueId*/, TInt aAttribute, TInt& aValue) const     
    {     
    TPckg<TInt> valuePkg(aValue);     
    TInt result = SendReceive(EWMDRMGetAttribute, TIpcArgs(aAttribute, &valuePkg));     
    return result;     
    }     
         
EXPORT_C TInt RRtaContent::GetWmdrmAttributeSet(const TDesC& aUniqueId, ContentAccess::RAttributeSet& aAttributeSet) const     
    {     
    TRAPD( err, GetAttributeSetL(aUniqueId, aAttributeSet, ETrue));     
    return err;     
    }     
         
EXPORT_C TInt RRtaContent::GetWmdrmStringAttribute(const TDesC& /*aUniqueId*/, TInt aAttribute, TDes& aValue) const     
    {     
    TIpcArgs ipcArgs(aAttribute, &aValue);     
    TInt result = SendReceive(EWMDRMGetStringAttribute, ipcArgs);     
    return result;     
    }     
      
EXPORT_C TInt RRtaContent::GetWmdrmStringAttributeSet(const TDesC& aUniqueId, ContentAccess::RStringAttributeSet& aStringAttributeSet) const     
    {     
    TRAPD( err, GetStringAttributeSetL(aUniqueId, aStringAttributeSet, ETrue));     
    return err;     
    }     
      
#endif //SYMBIAN_ENABLE_SDP_WMDRM_SUPPORT 

