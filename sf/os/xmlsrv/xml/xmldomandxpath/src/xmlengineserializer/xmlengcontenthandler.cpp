// Copyright (c) 2006-2009 Nokia Corporation and/or its subsidiary(-ies).
// All rights reserved.
// This component and the accompanying materials are made available
// under the terms of "Eclipse Public License v1.0"
// which accompanies this distribution, and is available
// at the URL "http://www.eclipse.org/legal/epl-v10.html".
//
// Initial Contributors:
// Nokia Corporation - initial contribution.
//
// Contributors:
//
// Description:
// Content handler implementation
//

#include "xmlengcontenthandler.h"
#include <xml/dom/xmlengerrors.h>
#include <xml/dom/xmlengdeserializer.h>
#include "xmlengdeserializerxop.h"
#include <xml/dom/xmlengextendedcontenthandler.h>
#include <xml/parserfeature.h>

#include <xml/dom/xmlengserializeerrors.h>
#include <bsul/clientmessage.h>

CXmlEngContentHandler* CXmlEngContentHandler::NewL( CXmlEngDeserializer* aDeserializer,
											   		MContentHandler* aContentHandler )
	{
	CXmlEngContentHandler* self = new (ELeave) CXmlEngContentHandler(aDeserializer, aContentHandler);
	CleanupStack::PushL(self);
	self->ConstructL();
	CleanupStack::Pop(self); //self
	return self;
	}

CXmlEngContentHandler::~CXmlEngContentHandler()
	{
	iCachedPrefixes.ResetAndDestroy();
	iCachedUris.ResetAndDestroy();
	iCachedErrors.Reset();
	iStrPool.Close();
	}

CXmlEngContentHandler::CXmlEngContentHandler(CXmlEngDeserializer* aDeserializer, 
									   		 MContentHandler* aContentHandler)
: iDeserializer(aDeserializer),
  iContentHandler(aContentHandler)
	{
	iSkipIncludeElement = 0;
  	iSkipPrefixMapping = 0;
	}

void CXmlEngContentHandler::ConstructL()
	{
	iStrPool.OpenL();
	}

/**
This method is a callback to indicate the start of the document.
@param				aDocParam Specifies the various parameters of the document.
@param				aDocParam.iCharacterSetName The character encoding of the document.
@param				aErrorCode is the error code. 
					If this is not KErrNone then special action may be required.
*/
void CXmlEngContentHandler::OnStartDocumentL(const RDocumentParameters& aDocParam, TInt aErrorCode)
{
	iContentHandler->OnStartDocumentL(aDocParam, aErrorCode);
}


/**
This method is a callback to indicate the end of the document.
@param				aErrorCode is the error code. 
					If this is not KErrNone then special action may be required.
*/
void CXmlEngContentHandler::OnEndDocumentL(TInt aErrorCode)
{
	iContentHandler->OnEndDocumentL(aErrorCode);
}


/**
This method is a callback to indicate an element has been parsed.
@param				aElement is a handle to the element's details.
@param				aAttributes contains the attributes for the element.
@param				aErrorCode is the error code.
					If this is not KErrNone then special action may be required.
*/
void CXmlEngContentHandler::OnStartElementL(const RTagInfo& aElement,
								 const RAttributeArray& aAttributes, 
								 TInt aErrorCode)
{
	_LIT8(KInclude, "Include");
	_LIT8(KXopIncludeUri, "http://www.w3.org/2004/08/xop/include");
	_LIT8(KHref, "href");
	_LIT8(KHrefUriEmpty, ""); 
	
	TBool xopInclude = EFalse;
	
	if(iSkipIncludeElement)
		{
		if( aElement.LocalName().DesC() == KInclude
	    	&& aElement.Uri().DesC() == KXopIncludeUri)
			{
			xopInclude = ETrue;
			++iSkipIncludeElement;				
			}		
		return;
		}

	MXmlEngExtendedHandler* ext = (MXmlEngExtendedHandler*)iContentHandler->GetExtendedInterface(MXmlEngExtendedHandler::EExtInterfaceUid);

	// determine if this element is xop:Include
	if( xopInclude ||
		(aElement.LocalName().DesC() == KInclude &&
	    aElement.Uri().DesC() == KXopIncludeUri))
		{
		for ( TInt i = 0; i < aAttributes.Count(); i++)
			{
			if( aAttributes[i].Attribute().LocalName().DesC() == KHref
				&& aAttributes[i].Attribute().Uri().DesC() == KHrefUriEmpty)
				{
				TPtrC8 cidValue;
				TPtrC8 data;
				cidValue.Set(CidValue(aAttributes[i].Value().DesC()));
				if( (CXmlEngDeserializer*)iDeserializer->ExternalData() 
					 && iDeserializer->GetDataContainer(cidValue) )
					{
					++iSkipIncludeElement;
					iSkipPrefixMapping = ETrue;
					if(ext)
						ext->OnDataContainerL(*iDeserializer->GetDataContainer(cidValue), KErrNone);	
					}
				else if(((CXmlEngDeserializerXOP*)iDeserializer)->GetData(cidValue, data) == KErrNone)					
					{
					++iSkipIncludeElement;
					iSkipPrefixMapping = ETrue;
					if(ext)
						{
						if(iDeserializer->ParsingOptions().iOptions & TXmlEngParsingOptions::KOptionEncodeToBase64)
							{
							//TImCodecB64 base64Codec; 
							// The encoded length of base64 is about twice (use 3x) decoded length
							HBufC8* data64 = HBufC8::NewLC(3*data.Size());  
							TPtr8 data64Ptr = data64->Des();
							// Encode to the base64 Content-Transfer-Encoding
							//base64Codec.Encode(data, data64Ptr);
							using namespace BSUL;
							Base64Codec::Encode(data, data64Ptr);
							if(data64Ptr.Length() == 0)
								{
								User::Leave(KErrGeneral);
								}
							ext->OnBinaryContentL(data64Ptr,cidValue,aErrorCode);
							CleanupStack::PopAndDestroy(); //data64							
							}
						else
							{
							ext->OnBinaryContentL(data,cidValue,aErrorCode);			
							}
						}
						
					}
				else
					{
					User::Leave(KXmlEngErrBinDataNotFound);
					//iContentHandler->OnStartElementL(aElement, aAttributes, aErrorCode);
					}						
				}
			} //for
		}

	if(!iSkipIncludeElement)
		{
		if(iCachedUris.Count())
			{
			RString prefix;
			RString uri;
			for(TInt i = 0; i < iCachedPrefixes.Count(); i++)
				{
				prefix = iStrPool.OpenStringL(iCachedPrefixes[i]->Des());
				CleanupClosePushL(prefix);
				uri = iStrPool.OpenStringL(iCachedUris[i]->Des());
				CleanupClosePushL(uri);
				iContentHandler->OnStartPrefixMappingL(prefix, uri, iCachedErrors[i]);
				CleanupStack::PopAndDestroy(&uri);
				CleanupStack::PopAndDestroy(&prefix);
				}						
			}
		if(iDeserializer->IsFeatureEnabled(EConvertTagsToLowerCase))
			{
			RTagInfo elementTag = ConvertNameToLowerCaseL(aElement);
			CleanupClosePushL(elementTag);
			iContentHandler->OnStartElementL(elementTag, aAttributes, aErrorCode);
			CleanupStack::PopAndDestroy(); //elementTag
			}
		else
			{
			iContentHandler->OnStartElementL(aElement, aAttributes, aErrorCode);				
			}			
		}
	iCachedUris.ResetAndDestroy();
	iCachedPrefixes.ResetAndDestroy();
	iCachedErrors.Reset();
}

	
/**
This method is a callback to indicate the end of the element has been reached.
@param				aElement is a handle to the element's details.
@param				aErrorCode is the error code.
					If this is not KErrNone then special action may be required.
*/
void CXmlEngContentHandler::OnEndElementL(const RTagInfo& aElement, TInt aErrorCode)
{
	_LIT8(KInclude, "Include");
	_LIT8(KXopIncludeUri, "http://www.w3.org/2004/08/xop/include");
	
	if(iSkipIncludeElement)
		{
		if( aElement.LocalName().DesC() == KInclude
	    	&& aElement.Uri().DesC() == KXopIncludeUri)
			{
			--iSkipIncludeElement;				
			}
		return;
		}
	else
	    {
	    iSkipPrefixMapping = EFalse;
	    }

	if(iDeserializer->IsFeatureEnabled(EConvertTagsToLowerCase))
		{
		RTagInfo elementTag = ConvertNameToLowerCaseL(aElement);
		CleanupClosePushL(elementTag);
		iContentHandler->OnEndElementL(elementTag, aErrorCode);
		CleanupStack::PopAndDestroy(&elementTag); //elementTag
		}
	else
		{
		iContentHandler->OnEndElementL(aElement, aErrorCode);	
		}
}

/**
This method is a callback that sends the content of the element.
Not all the content may be returned in one go. The data may be sent in chunks.
When an OnEndElementL is received this means there is no more content to be sent.
@param				aBytes is the raw content data for the element. 
					The client is responsible for converting the data to the 
					required character set if necessary.
					In some instances the content may be binary and must not be converted.
@param				aErrorCode is the error code.
					If this is not KErrNone then special action may be required.
*/
void CXmlEngContentHandler::OnContentL(const TDesC8& aBytes, TInt aErrorCode)
{
	if(iSkipIncludeElement)
		{
		return;
		}
	iContentHandler->OnContentL(aBytes, aErrorCode);
}
	
/**
This method is a notification of the beginning of the scope of a prefix-URI Namespace mapping.
This method is always called before the corresponding OnStartElementL method.
@param				aPrefix is the Namespace prefix being declared.
@param				aUri is the Namespace URI the prefix is mapped to.
@param				aErrorCode is the error code.
					If this is not KErrNone then special action may be required.
*/
void CXmlEngContentHandler::OnStartPrefixMappingL(const RString& aPrefix, const RString& aUri, 
									   TInt aErrorCode)
{
	if(iSkipIncludeElement)
		{
		return;
		}
	iSkipPrefixMapping = EFalse;
	HBufC8* pref = aPrefix.DesC().AllocL();
	HBufC8* uri = aUri.DesC().AllocL();
	iCachedPrefixes.AppendL(pref);
	iCachedUris.AppendL(uri);
	User::LeaveIfError(iCachedErrors.Append(aErrorCode));
}

/**
This method is a notification of the end of the scope of a prefix-URI mapping.
This method is called after the corresponding DoEndElementL method.
@param				aPrefix is the Namespace prefix that was mapped.
@param				aErrorCode is the error code.
					If this is not KErrNone then special action may be required.
*/
void CXmlEngContentHandler::OnEndPrefixMappingL(const RString& aPrefix, TInt aErrorCode)
{
	if(iSkipPrefixMapping)
		{
		return;
		}
	iContentHandler->OnEndPrefixMappingL(aPrefix, aErrorCode);
}

/**
This method is a notification of ignorable whitespace in element content.
@param				aBytes are the ignored bytes from the document being parsed.
@param				aErrorCode is the error code.
					If this is not KErrNone then special action may be required.
*/
void CXmlEngContentHandler::OnIgnorableWhiteSpaceL(const TDesC8& aBytes, TInt aErrorCode)
{
	if(iSkipIncludeElement)
		{
		return;
		}
	iContentHandler->OnIgnorableWhiteSpaceL(aBytes, aErrorCode);
}

/**
This method is a notification of a skipped entity. If the parser encounters an 
external entity it does not need to expand it - it can return the entity as aName 
for the client to deal with.
@param				aName is the name of the skipped entity.
@param				aErrorCode is the error code.
					If this is not KErrNone then special action may be required.
*/
void CXmlEngContentHandler::OnSkippedEntityL(const RString& aName, TInt aErrorCode)
{
	if(iSkipIncludeElement)
		{
		return;
		}
	iContentHandler->OnSkippedEntityL(aName, aErrorCode);
}

/**
This method is a receive notification of a processing instruction.
@param				aTarget is the processing instruction target.
@param				aData is the processing instruction data. If empty none was supplied.
@param				aErrorCode is the error code.
					If this is not KErrNone then special action may be required.
*/
void CXmlEngContentHandler::OnProcessingInstructionL(const TDesC8& aTarget,
										  const TDesC8& aData, 
										  TInt aErrorCode)
{
	if(iSkipIncludeElement)
		{
		return;
		}
	iContentHandler->OnProcessingInstructionL(aTarget, aData, aErrorCode);
}

/**
This method indicates an error has occurred.
@param				aError is the error code
*/
void CXmlEngContentHandler::OnError(TInt aErrorCode)
{
	iContentHandler->OnError(aErrorCode);	
}

/**
This method obtains the interface matching the specified uid.
@return				0 if no interface matching the uid is found.
					Otherwise, the this pointer cast to that interface.
@param				aUid the uid identifying the required interface.
*/
TAny* CXmlEngContentHandler::GetExtendedInterface(const TInt32 /*aUid*/)
{
	return 0;	
}

/**
This method strips CID string from "cid:" prefix to fetch the pure CID value
@return				CID value
@param				aCid CID string
*/
TPtrC8 CXmlEngContentHandler::CidValue(const TDesC8& aCid)
	{
	_LIT8(KCidPrefix, "cid:*");
	TInt position = aCid.Match(KCidPrefix);
	if(position < 0)
		{
		position = 0;
		}
	return aCid.Mid( position + KCidPrefix().Size() - 1); // minus star character (*)
	}

/**
This method converts element name to lower case and creates new tag info
@return				New tag info with lower case element name
@param				aTagInfo Original tag info
*/
RTagInfo CXmlEngContentHandler::ConvertNameToLowerCaseL(const RTagInfo& aElement)
	{
	// convert name to lower case, create new RTagInfo, invoke callback
	HBufC8* uriBuf = aElement.Uri().DesC().AllocLC();
	HBufC8* prefBuf = aElement.Prefix().DesC().AllocLC();
	HBufC8* nameBuf = aElement.LocalName().DesC().AllocLC();
	nameBuf->Des().LowerCase();
	
	RString uri = iStrPool.OpenStringL(uriBuf->Des());
	CleanupClosePushL(uri);
	RString pref = iStrPool.OpenStringL(prefBuf->Des());
	CleanupClosePushL(pref);
	RString name = iStrPool.OpenStringL(nameBuf->Des());
	CleanupStack::Pop(&pref);
	CleanupStack::Pop(&uri);
	CleanupStack::PopAndDestroy(nameBuf);
	CleanupStack::PopAndDestroy(prefBuf);
	CleanupStack::PopAndDestroy(uriBuf);
	RTagInfo elementTag;
	elementTag.Open(uri, pref, name);
	TPtrC8 a = elementTag.LocalName().DesC();
	return elementTag;	
	}
