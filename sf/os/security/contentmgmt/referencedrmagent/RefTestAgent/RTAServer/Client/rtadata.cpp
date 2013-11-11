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
#include "rtadata.h"

using namespace ReferenceTestAgent;
using namespace ContentAccess;


EXPORT_C RRtaData::RRtaData()
	{
	
	}

EXPORT_C TInt RRtaData::Open(const TDesC& aFileName, const TDesC& aUniqueId, TUint aMode)
	{
	TPckg <TUint> fileModePckg(aMode);
	TInt err = RRtaClient::Connect();
	
	if(err == KErrNone)
		{
		if(IsDrmCapabilityEnforced())
			{
			err = SendReceive(ECreateDataMessageHandler, TIpcArgs());
			}
		else
			{
			err = SendReceive(ENoEnforceCreateDataMessageHandler, TIpcArgs());
			}
		}

	if(err == KErrNone)
		{
		ShareAuto();
		if(IsDrmCapabilityEnforced())
			{
			err = SendReceive(EOpenFile, TIpcArgs(&aFileName, &aUniqueId, &fileModePckg));
			}
		else
			{
			err = SendReceive(ENoEnforceOpenFile, TIpcArgs(&aFileName, &aUniqueId, &fileModePckg));
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
	
EXPORT_C TInt RRtaData::Open(RFile& aFile, const TDesC& aUniqueId)
	{
	TIpcArgs ipcArgs;
	aFile.TransferToServer(ipcArgs, 0, 1);
	ipcArgs.Set(2, &aUniqueId);
	
	TInt err = RRtaClient::Connect();
	if(err == KErrNone)
		{
		if(IsDrmCapabilityEnforced())
			{
			err = SendReceive(ECreateDataMessageHandler, TIpcArgs());
			}
		else
			{
			err = SendReceive(ENoEnforceCreateDataMessageHandler, TIpcArgs());
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

EXPORT_C TInt RRtaData::Read(TDes8& aBuffer, TInt aLength)
	{
	TPckg <TInt> lengthPckg(aLength);
	if(IsDrmCapabilityEnforced())
		{
		return SendReceive(EReadData, TIpcArgs(&aBuffer, &lengthPckg));	
		}
	else
		{
		return SendReceive(ENoEnforceReadData, TIpcArgs(&aBuffer, &lengthPckg));	
		}
	}

#ifdef SYMBIAN_ENABLE_64_BIT_FILE_SERVER_API
EXPORT_C TInt RRtaData::Read(TInt64 aPos, TDes8& aBuffer, TInt aLength)
#else
EXPORT_C TInt RRtaData::Read(TInt aPos, TDes8& aBuffer, TInt aLength)
#endif // SYMBIAN_ENABLE_64_BIT_FILE_SERVER_API
	{
#ifdef SYMBIAN_ENABLE_64_BIT_FILE_SERVER_API
	TPckg <TInt64> posPckg(aPos);
#else
	TPckg <TInt> posPckg(aPos);
#endif // SYMBIAN_ENABLE_64_BIT_FILE_SERVER_API
	TPckg <TInt> lengthPckg(aLength);
	if(IsDrmCapabilityEnforced())
		{
		return SendReceive(EReadOffsetData, TIpcArgs(&posPckg, &aBuffer, &lengthPckg));	
		}
	else
		{
		return SendReceive(ENoEnforceReadOffsetData, TIpcArgs(&posPckg, &aBuffer, &lengthPckg));	
		}
	}
	
#ifdef SYMBIAN_ENABLE_64_BIT_FILE_SERVER_API
EXPORT_C TInt RRtaData::DataSize(TInt64 &aSize) 
#else
EXPORT_C TInt RRtaData::DataSize(TInt &aSize) 
#endif // SYMBIAN_ENABLE_64_BIT_FILE_SERVER_API
	{
#ifdef SYMBIAN_ENABLE_64_BIT_FILE_SERVER_API
	TPckg <TInt64> sizePckg(aSize);
#else
	TPckg <TInt> sizePckg(aSize);
#endif // SYMBIAN_ENABLE_64_BIT_FILE_SERVER_API
	if(IsDrmCapabilityEnforced())
		{
		return SendReceive(EDataSize, TIpcArgs(&sizePckg));	
		}
	else
		{
		return SendReceive(ENoEnforceDataSize, TIpcArgs(&sizePckg));		
		}
	}
	
#ifdef SYMBIAN_ENABLE_64_BIT_FILE_SERVER_API
EXPORT_C TInt RRtaData::Seek(TSeek aMode, TInt64 &aPos) const 
#else
EXPORT_C TInt RRtaData::Seek(TSeek aMode, TInt &aPos) const 
#endif // SYMBIAN_ENABLE_64_BIT_FILE_SERVER_API
	{
	TPckg <TSeek> modePckg(aMode);		
#ifdef SYMBIAN_ENABLE_64_BIT_FILE_SERVER_API
	TPckg <TInt64> posPckg(aPos);
#else
	TPckg <TInt> posPckg(aPos);
#endif // SYMBIAN_ENABLE_64_BIT_FILE_SERVER_API
	
	if(IsDrmCapabilityEnforced())
		{
		return SendReceive(ESeek, TIpcArgs(&modePckg, &posPckg));	
		}
	else
		{
		return SendReceive(ENoEnforceSeek, TIpcArgs(&modePckg, &posPckg));	
		}
	}


EXPORT_C void RRtaData::EvaluateIntentL(TIntent aIntent)
	{
	TPckg <TIntent> intentPckg(aIntent);
	if(IsDrmCapabilityEnforced())
		{
		User::LeaveIfError(SendReceive(EEvaluateIntent, TIpcArgs(&intentPckg)));
		}
	else
		{
		User::LeaveIfError(SendReceive(ENoEnforceEvaluateIntent, TIpcArgs(&intentPckg)));	
		}
	}

EXPORT_C void RRtaData::ExecuteIntentL(TIntent aIntent)
	{
	TPckg <TIntent> intentPckg(aIntent);
	if(IsDrmCapabilityEnforced())
		{
		User::LeaveIfError(SendReceive(EExecuteIntent, TIpcArgs(&intentPckg)));
		}
	else
		{
		User::LeaveIfError(SendReceive(ENoEnforceEvaluateIntent, TIpcArgs(&intentPckg)));	
		}
	}

EXPORT_C TInt RRtaData::GetAttribute (TInt aAttribute, TInt &aValue) const
	{
	TPckg <TInt> attributePckg(aAttribute);
	TPckg <TInt> valuePckg(aValue);			
	return SendReceive(EGetAttribute, TIpcArgs(&attributePckg, &valuePckg));	
	}
	
EXPORT_C TInt RRtaData::GetAttributeSet (RAttributeSet &aAttributeSet) const
	{
	TRAPD( err, GetAttributeSetL(aAttributeSet) );
	return err;
	}

#ifdef SYMBIAN_ENABLE_SDP_WMDRM_SUPPORT
void RRtaData::GetAttributeSetL (RAttributeSet &aAttributeSet, TBool aWmdrmFlag) const     
#else 
void RRtaData::GetAttributeSetL (RAttributeSet &aAttributeSet) const
#endif // SYMBIAN_ENABLE_SDP_WMDRM_SUPPORT
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
            err = SendReceive(EWMDRMGetAttributeSet, TIpcArgs(&transferPtr));     
        else     
            err = SendReceive(EGetAttributeSet, TIpcArgs(&transferPtr));         
#else 
		// attempt to retrieve the attributes
		err = SendReceive(EGetAttributeSet, TIpcArgs(&transferPtr));	
#endif // SYMBIAN_ENABLE_SDP_WMDRM_SUPPORT
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
		
EXPORT_C TInt RRtaData::GetStringAttribute (TInt aAttribute, TDes &aValue) const
	{
	TPckg <TInt> attributePckg(aAttribute);
	return SendReceive(EGetStringAttribute, TIpcArgs(&attributePckg, &aValue));
	}

EXPORT_C TInt RRtaData::GetStringAttributeSet (RStringAttributeSet &aStringAttributeSet) const
	{
	TRAPD( err, GetStringAttributeSetL(aStringAttributeSet) );
	return err;
	}

#ifdef SYMBIAN_ENABLE_SDP_WMDRM_SUPPORT
void RRtaData::GetStringAttributeSetL (RStringAttributeSet &aStringAttributeSet, TBool aWmdrmFlag) const     
#else 
void RRtaData::GetStringAttributeSetL (RStringAttributeSet &aStringAttributeSet) const
#endif // SYMBIAN_ENABLE_SDP_WMDRM_SUPPORT
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
            err = SendReceive(EWMDRMGetStringAttributeSet, TIpcArgs(&transferPtr));     
        else     
            err = SendReceive(EGetStringAttributeSet, TIpcArgs(&transferPtr));       
#else 
		// attempt to retrieve the attributes
		err = SendReceive(EGetStringAttributeSet, TIpcArgs(&transferPtr));	
#endif // SYMBIAN_ENABLE_SDP_WMDRM_SUPPORT
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

EXPORT_C TInt RRtaData::SetProperty(ContentAccess::TAgentProperty aProperty, TInt aValue) const
	{
	TInt err = KErrNone;
	TPckg <TInt> valuePckg(aValue);
	TPckg <TAgentProperty> agentPropertyPckg(aProperty);
	if(IsDrmCapabilityEnforced())
		{
		err = SendReceive(EDataSetProperty, TIpcArgs(&agentPropertyPckg, &valuePckg));
		}
	else
		{
		err = SendReceive(ENoEnforceDataSetProperty, TIpcArgs(&agentPropertyPckg, &valuePckg));
		}
	return err;
	}

#ifdef SYMBIAN_ENABLE_SDP_WMDRM_SUPPORT     
      
EXPORT_C TInt RRtaData::Open(const TDesC8& aHeaderData)     
    {     
    TInt err = RRtaClient::Connect();     
    if(err != KErrNone)     
        {     
        return err;     
        }     
         
    if(IsDrmCapabilityEnforced())     
        {     
        err = SendReceive(ECreateDataMessageHandler, TIpcArgs());     
        if(err != KErrNone)     
            return err;     
             
        err = SendReceive(EReadWMDRMHeaderData, TIpcArgs(&aHeaderData));     
        }     
             
    else     
        {     
        err = SendReceive(ENoEnforceCreateDataMessageHandler, TIpcArgs());     
        if(err != KErrNone)     
            return err;     
             
        err = SendReceive(ENoEnforceReadWMDRMHeaderData, TIpcArgs(&aHeaderData));     
        }     
      
    return err;     
    }     
         
EXPORT_C TInt RRtaData::Read(const TDesC8& aEncryptedInputPacket, TDes8& aDecryptedOutputPacket)     
    {     
    TInt err = 0;     
    if(IsDrmCapabilityEnforced())     
        {     
        err =  SendReceive(EReadWMDRMDataPacket, TIpcArgs(&aEncryptedInputPacket, &aDecryptedOutputPacket));         
        }     
    else     
        {     
        err =  SendReceive(ENoEnforceReadWMDRMDataPacket, TIpcArgs(&aEncryptedInputPacket, &aDecryptedOutputPacket));        
        }     
         
    return err;     
    }     
         
      
EXPORT_C TInt RRtaData::GetWmdrmAttribute(TInt aAttribute, TInt& aValue) const     
    {     
    TPckg<TInt> valuePkg(aValue);     
    TInt err = SendReceive(EWMDRMGetAttribute, TIpcArgs(aAttribute, &valuePkg));     
    return err;     
    }     
         
EXPORT_C TInt RRtaData::GetWmdrmAttributeSet(ContentAccess::RAttributeSet& aAttributeSet) const     
    {     
    TRAPD( err, GetAttributeSetL(aAttributeSet, ETrue));     
    return err;     
    }     
         
EXPORT_C TInt RRtaData::GetWmdrmStringAttribute(TInt aAttribute, TDes& aValue) const     
    {     
    return SendReceive(EWMDRMGetStringAttribute, TIpcArgs(aAttribute, &aValue));     
    }     
      
EXPORT_C TInt RRtaData::GetWmdrmStringAttributeSet(ContentAccess::RStringAttributeSet& aStringAttributeSet) const     
    {     
    TRAPD( err, GetStringAttributeSetL(aStringAttributeSet, ETrue));     
    return err;     
    }     
      
#endif //SYMBIAN_ENABLE_SDP_WMDRM_SUPPORT 


