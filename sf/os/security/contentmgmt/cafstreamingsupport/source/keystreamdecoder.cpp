// Copyright (c) 2007-2009 Nokia Corporation and/or its subsidiary(-ies).
// All rights reserved.
// This component and the accompanying materials are made available
// under the terms of the License "Eclipse Public License v1.0"
// which accompanies this distribution, and is available
// at the URL "http://www.eclipse.org/legal/epl-v10.html".
//
// Initial Contributors:
// Nokia Corporation - initial contribution.
//
// Contributors:
//
// Description:
//

#include <caf/streaming/keystreamdecoder.h>
#include <caf/streaming/streamagentfactory.h>
#include "streamagentresolver.h"
#include <caf/streaming/protectedstreamdesc.h>
#include "scaflog.h"
#include <caf/supplier.h>
#include <caf/metadataarray.h>

using namespace StreamAccess;
 
EXPORT_C CKeyStreamDecoder* CKeyStreamDecoder::NewL(const CProtectedStreamDesc& aProtectedStreamDesc, const CSdpMediaField& aSdpKeyStream, const CSdpDocument& aSdpDoc)
	{
	CKeyStreamDecoder* self = CKeyStreamDecoder::NewLC(aProtectedStreamDesc,aSdpKeyStream,aSdpDoc);
	CleanupStack::Pop(self);
	return self;
	}

EXPORT_C CKeyStreamDecoder* CKeyStreamDecoder::NewLC(const CProtectedStreamDesc& aProtectedStreamDesc, const CSdpMediaField& aSdpKeyStream, const CSdpDocument& aSdpDoc)
	{
	CKeyStreamDecoder* self = new(ELeave) CKeyStreamDecoder();
	CleanupStack::PushL(self);
	self->ConstructL(aProtectedStreamDesc,aSdpKeyStream,aSdpDoc);
	return self;
	}

CKeyStreamDecoder::CKeyStreamDecoder()
	{
	}

CKeyStreamDecoder::~CKeyStreamDecoder()
	{
	delete iAgentKeyStreamDecoder;
	delete iStreamAgentFactory;
	
	REComSession::DestroyedImplementation(iEcomKey);
	REComSession::FinalClose();	
	}

void CKeyStreamDecoder::ConstructL(const CProtectedStreamDesc& aProtectedStreamDesc, const CSdpMediaField& aSdpKeyStream, const CSdpDocument& aSdpDoc)
	{
	DEBUG_PRINTF(_L("Creating a key stream decoder object."));
	
	// Create an instance of the Stream Agent Resolver
	CStreamAgentResolver* resolver = CStreamAgentResolver::NewLC();
	
	/* Retrieve a reference to the CStreamAgentInfo object, associated to the resolved stream agent plugin 
	 * capable of supporting the supplied SDP media description 
	 */
	CStreamAgentInfo& agentInfo = resolver->ResolveSdpKeyStreamL(aSdpKeyStream);
	
	/* Create a handle to the factory object (CStreamAgentFactory) associated to the
	 * resolved stream agent, so that it can be successfully destroyed and freed having been used 
	 */
	iStreamAgentFactory = static_cast<CStreamAgentFactory *>(REComSession::CreateImplementationL(agentInfo.ImplementationUid(), iEcomKey));
	
	// Instantiate the necessary implementation of key stream sink
	CKeyStreamSink* keyStreamSink = aProtectedStreamDesc.CreateKeyStreamSinkLC();
	
	// Retrieve a reference to the stream agents factory
	CStreamAgentFactory& agentFactory = agentInfo.StreamAgentFactory();
	
	/* The factory is then used to generate an instance of the CAgentKeyStreamDecoder, using the instantiated key
	 * stream sink and the SDP media description
	 */
	iAgentKeyStreamDecoder = agentFactory.GetKeyStreamDecoderL(*keyStreamSink,aSdpKeyStream,aSdpDoc);
	
	CleanupStack::PopAndDestroy(2,resolver); //< keyStreamSink, resolver
	
	DEBUG_PRINTF(_L("The key stream decoder object has been created successfully."));
	}
	
EXPORT_C void CKeyStreamDecoder::GetAttributeL(const TAttribute& aAttribute, TBool& aValue) const
	{
	iAgentKeyStreamDecoder->GetAttributeL(aAttribute, aValue);
	}

EXPORT_C HBufC* CKeyStreamDecoder::GetStringAttributeLC(const TStringAttribute& aAttribute) const
	{
	return iAgentKeyStreamDecoder->GetStringAttributeLC(aAttribute);
	}


EXPORT_C ContentAccess::CImportFile* CKeyStreamDecoder::CreateImportSessionLC(const TDesC8& aContentMimeType, const TDesC& aSuggestedName, const TDesC& aOutputDirectory) const
	{
	return DoCreateImportSessionLC(aContentMimeType, aSuggestedName, aOutputDirectory);
	}

EXPORT_C ContentAccess::CImportFile* CKeyStreamDecoder::CreateImportSessionLC(const TDesC8& aContentMimeType) const
	{
	return DoCreateImportSessionLC(aContentMimeType, KNullDesC(), KNullDesC());
	}

ContentAccess::CImportFile* CKeyStreamDecoder::DoCreateImportSessionLC(const TDesC8& aContentMimeType, const TDesC& aSuggestedName, const TDesC& aOutputDirectory) const
	{
	DEBUG_PRINTF(_L("Creating an import session for post-acquisition content."));
	
	// Handle to the post-acquisition rights object file owned by the agent
	RFile   fPostAcq;
	// The content Id of the pos-acquisition rights object.
	RBuf8 contentId;
	contentId.CreateL(KMimeTypesMaxLength);
	contentId.CleanupClosePushL();
	// The mime type which will be used to import the rights object
	RBuf8 mimeTypeRights; 
	mimeTypeRights.CreateL(KMimeTypesMaxLength);
	mimeTypeRights.CleanupClosePushL();
	// The mime type which will be used to import the content file.
	RBuf8 mimeTypeContent;
	mimeTypeContent.CreateL(KMimeTypesMaxLength);
	mimeTypeContent.CleanupClosePushL();
	
	iAgentKeyStreamDecoder->GetPostDeliveryRightsL(fPostAcq, contentId, mimeTypeRights, mimeTypeContent);
	CleanupClosePushL(fPostAcq);
	
	DEBUG_PRINTF2(_L8("Post-acquisition RO's Mime Type: (%S)"), &mimeTypeRights);
	DEBUG_PRINTF2(_L8("Post-acquisition Content's Mime Type: (%S)"), &mimeTypeContent);
	
	// Create a supplier
	ContentAccess::CSupplier* supplier = ContentAccess::CSupplier::NewLC();
	// Check if there is any CAF agent supporting postacquisition rights object import
	if(!supplier->IsImportSupported(mimeTypeRights))
		{
		DEBUG_PRINTF2(_L8("A CAF agent capable of importing (%S) mime type can not be found!"), &mimeTypeRights);
		User::Leave(KErrNotSupported);
		}
	// if everything went successfully, create a file import object and import the rights object
	// Create meta-data array
	ContentAccess::CMetaDataArray* metaDataArray = ContentAccess::CMetaDataArray::NewLC();

	// The content Id is provided as suggested name to allow the agent to generate the output file
	HBufC* roName = HBufC::NewLC(contentId.Length());
	TPtr roPtr (roName->Des());
	roPtr.Copy(contentId);
	// Create the import object for the rights object file
	ContentAccess::CImportFile* importRo =  supplier->ImportFileL(mimeTypeRights, *metaDataArray, *roName);
	CleanupStack::PopAndDestroy(roName);
	CleanupStack::PushL(importRo);
	
	TBuf8<128> buf;
	TInt dataLen = 0;
	// Start importing the content of the post-acquisition RO file
	do
		{
		User::LeaveIfError(fPostAcq.Read(buf));
		dataLen = buf.Length();
		User::LeaveIfError(importRo->WriteData(buf));
		} while(dataLen > 0);
	// Post-acquisition rigths object import is being completed
	User::LeaveIfError(importRo->WriteDataComplete());
	CleanupStack::PopAndDestroy(importRo);
	
	//The meta data field of the content Id. A content Id is passed to an agent through this meta-data field.
	_LIT8(KMetaDataFieldCid, "cid");
	// Add the (postacq) RO's content id used to match the content with the RO
	metaDataArray->AddL(KMetaDataFieldCid(), contentId);
	
	//The meta data field of the mime type. A mime type is passed to an agent through this meta-data field.
	_LIT8(KMetaDataFieldMimeType, "mimetype");
	// Add the mime type of the protected streamed data (e.g. video/3gpp)
	metaDataArray->AddL(KMetaDataFieldMimeType(), aContentMimeType);
	
	// Create the import object for the protected streamed content
	ContentAccess::CImportFile* importContent(0);
	if((aSuggestedName.Length() > 0) && (aOutputDirectory.Length() > 0))
		{
		// A file name is suggested. The client wants the agent to generate the output files.
		DEBUG_PRINTF2(_L("Output filename (%S) is suggested."), &aSuggestedName);
		supplier->SetOutputDirectoryL(aOutputDirectory);
		importContent =  supplier->ImportFileL(mimeTypeContent, *metaDataArray, aSuggestedName);
		}
	else
		{
		// No suggested file name is passed to the agent, the client will provide output files for the agent to use
		DEBUG_PRINTF(_L("Output filename is not suggested."));
		importContent =  supplier->ImportFileL(mimeTypeContent, *metaDataArray);
		}
	
	CleanupStack::PopAndDestroy(6, &contentId);
	CleanupStack::PushL(importContent);
	
	DEBUG_PRINTF(_L("Import session has been created successfully."));
	
	return importContent;
	}
