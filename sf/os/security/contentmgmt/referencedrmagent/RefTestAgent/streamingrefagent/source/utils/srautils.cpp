/*
* Copyright (c) 2007-2009 Nokia Corporation and/or its subsidiary(-ies).
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
* Implements some utility functions for reference agent.
*
*/


#include "srautils.h"
#include <s32file.h>
#include "sdpfmtattributefield.h"
#include "sdpcodecstringpool.h"
#include "sdpattributefield.h"

HBufC8* ExtractKmsIdLC(const TDesC8& aValue)
/**
 	Searches the given format attribute value for KMS ID.
 	@param The format attribute value.
 	@return The value of KMS ID, if it exists. Otherwise, NULL.
 */
	{
	HBufC8* kmsid(0);
	_LIT8(KKmsIdParameter, "IPDCKMSId=");
	
	TLex8 parser(aValue);
	while(!parser.Eos())
		{
		TPtrC8 token(parser.NextToken());
		if(token.Find(KKmsIdParameter) == KErrNotFound) continue;
		TLex8 subParser(token);
		while(!subParser.Eos())
			{
			if(subParser.Get() != '=') continue;
			TPtrC8 value(subParser.NextToken());
			if(value.Length() == 0) break;
			//Copy the kmsid value to a buffer
			kmsid = HBufC8::NewLC(value.Length()+1);
			TPtr8 ptrBuf(kmsid->Des());
			ptrBuf.Copy(value);
			//Copy the separator if it does not exist
			_LIT8(KSeparator,";");
			if(ptrBuf.Find(KSeparator) == KErrNotFound)
				{
				ptrBuf.Append(KSeparator);
				}
			break;	
			}
		break;
		}
	return kmsid;
	}


EXPORT_C TBool CheckKeyStreamSupportedL(const CSdpMediaField& aSdpKeyStream, const TDesC8& aSupportedKmsIds)
/**
 	Function to determine whether the test stream agent supports the key management scheme 
 	specified in the SDP key stream definition
 	@param aSdpKeyStream Contains the metadata for the SDP key management scheme of the stream
 	@param aSupportedKmsIds The list of supported KMS Ids. The delimeter of the list is semi-colon.
 	@return ETrue if the test stream agent recognizes the SDP format and the KMS Id is supported.
	@return EFalse if the test stream agent fails to recognize the SDP format or is unable to decode the key stream.
 */
	{
	TBool supported = EFalse;
	//FormatAttributeFields method is not constant. So create a new instance.
	CSdpMediaField* sdp = aSdpKeyStream.CloneL();
	CleanupStack::PushL(sdp);
	
	TInt attributesCount = sdp->FormatAttributeFields().Count();
	if (attributesCount <= 0)
		{
		CleanupStack::PopAndDestroy(sdp);
		return EFalse;
		}

	HBufC8* kmsid(0);
	
	for(TInt i=0; i<attributesCount && !supported; ++i)
		{
		if(sdp->FormatAttributeFields()[i]->Format().Compare(_L8("ipdc-ksm"))) continue;
		
		kmsid = ExtractKmsIdLC(sdp->FormatAttributeFields()[i]->Value());
		if(!kmsid) continue;
		
		if(aSupportedKmsIds.Find(*kmsid) != KErrNotFound) 
			{
			supported = ETrue;
			}
		CleanupStack::PopAndDestroy(kmsid);
		}
	CleanupStack::PopAndDestroy(sdp);
	return supported;
	}



void FindAndLoadRightsObjectL(RFs& aFs, const TDesC8& aCid, const TDesC& aPrivateFolder, CSraRightsObject*& aRo)
/**
 	The reference agent keeps rights object files in its own private folder.
 	This function scans the private directory and reads RO files in turn.
 	If the RO with the given Content ID is found, it's assigned as the session's rights object.
 	
 	A real-life agent should pre-calculate the map of all supported Rights Object on construction 
 	and import to avoid costly searches on each opened stream.
 	
 	@param aFs The file server handle.
 	@param aCid The searched Content Id.
 	@param aPrivateFolder The private folder of the agent.
 	@param aRo Rights Object which is found in the private folder of the agent.
 */
	{
	ASSERT(!aRo);
	TFileName privFolder(aPrivateFolder);
	privFolder[0] = aFs.GetSystemDriveChar();
	privFolder.Append(_L("*.dr"));
	
	CDir *entryList;
	User::LeaveIfError(aFs.GetDir(privFolder,KEntryAttNormal,ESortByName|EDescending|EDirsLast,entryList));
	CleanupStack::PushL(entryList);
	privFolder.SetLength(privFolder.Length()-4);
	
	TInt count = entryList->Count();
	TInt i = 0;
	TFileName filePath;
	RFile fRo;
	RFileReadStream stream;
	CSraRightsObject* ro = NULL;
	for(i=0; i<count; ++i)
		{
		if((*entryList)[i].IsDir()) continue;

		filePath.Copy(privFolder);
		filePath.Append((*entryList)[i].iName);
		User::LeaveIfError(fRo.Open(aFs, filePath, EFileRead | EFileShareReadersOnly));
		CleanupClosePushL(fRo);
		stream.Attach(fRo);
		CleanupClosePushL(stream);
		ro = CSraRightsObject::NewL(stream);
		CleanupStack::PushL(ro);
		if(aCid.Compare(ro->ContentId()->Des()) == 0)
			{
			CleanupStack::Pop(ro);
			aRo = ro;
			CleanupStack::PopAndDestroy(2, &fRo);
			break;
			}
		else
			{
			CleanupStack::PopAndDestroy(3, &fRo);		
			}//if
		}//for
	CleanupStack::PopAndDestroy(entryList);
	}

EXPORT_C void DoSetRightsObjectL(RFs& aFs, CSdpMediaField& aSdp, CSraRightsObject*& aRo, const TDesC& aPrivateFolder)
/**
 	Finds and loads the Rights Object specified in the SDP given.
 */
	{
	// Find the rights object if it is defined in the SDP
	CSdpAttributeField* roAttrField = NULL;
	TInt count = aSdp.AttributeFields().Count();
	if(count <= 0)
		{
		// No rights object is defined in the SDP. 
		// The agent cannot service without any rights object.
		User::Leave(KErrNotSupported);
		}

	// The content Id of the rights object is defined with the following string.
	_LIT8(KSdpAttributeRightsObject,"baseCID");
	RStringPool pool = SdpCodecStringPool::StringPoolL();
	// Create a string for the content id by using the current string pool.
	RStringF roAttribute = pool.OpenFStringL(KSdpAttributeRightsObject);
	CleanupClosePushL(roAttribute);
		
	TInt i = 0;
	for(i=0; i<count; ++i)
		{
		if((aSdp.AttributeFields()[i]->Attribute()) == roAttribute)
			{
			// The attribute of the content id has been found.
			roAttrField = aSdp.AttributeFields()[i];
			break;
			}//if
		}//for
	CleanupStack::PopAndDestroy(&roAttribute);
	
	if(roAttrField) 
		{//the rights object is defined in the SDP
		FindAndLoadRightsObjectL(aFs, roAttrField->Value(), aPrivateFolder, aRo);
		}
	}

EXPORT_C void DoSetSdpMediaFieldL(RFs& aFs, CSdpMediaField*& aSdp, CSraRightsObject*& aRo, const TDesC8& aSdpData, const TDesC& aPrivateFolder)
/**
 	Generates a Service Description Protocol (SDP) object from the given SDP data.
 	
 	@param aFs The file server handle.
 	@param aSdp The session description protocol object generated by this function.
 	@param aRo Rights Object which is found in the private folder of the agent.
 	@param aSdpData The SDP data sent by the client.
 	@param aPrivateFolder The private folder of the agent.
  */
	{
	ASSERT(!aSdp);
	// Decode the received message into an SDP object
	aSdp = CSdpMediaField::DecodeL(aSdpData,ETrue);
	// Set the specified RO
	DoSetRightsObjectL(aFs, *aSdp, aRo, aPrivateFolder);
	}
