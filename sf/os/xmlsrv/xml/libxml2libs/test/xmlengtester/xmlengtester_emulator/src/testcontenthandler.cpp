// Copyright (c) 2005-2009 Nokia Corporation and/or its subsidiary(-ies).
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
//

#include "TestContentHandler.h"


CTestHandler* CTestHandler::NewL(const RFile& aFileName)
{
	CTestHandler* tester = new (ELeave) CTestHandler();
	CleanupStack::PushL(tester);
	tester->ConstructL(aFileName);
	CleanupStack::Pop(tester);
	return tester;
}


CTestHandler* CTestHandler::NewLC(const RFile& aFileName)
{
	CTestHandler* tester = new (ELeave) CTestHandler();
	CleanupStack::PushL(tester);
	tester->ConstructL(aFileName);
	return tester;
}

CTestHandler::~CTestHandler()
{
}

CTestHandler::CTestHandler()
{
}

void CTestHandler::ConstructL(const RFile & aFileName)
{
	iLog = aFileName;
}

/**
This method is a callback to indicate the start of the document.
@param				aDocParam Specifies the various parameters of the document.
					aDocParam.iCharacterSetName The character encoding of the document.
@param				aErrorCode is the error code. 
					If this is not KErrNone then special action may be required.
*/
void CTestHandler::OnStartDocumentL(const RDocumentParameters& aDocParam, TInt aErrorCode)
{
	_LIT8(KOnStartDocumentFuncName,"OnStartDocument()\r\n");
	_LIT8(KInfoOnStartDocP,"\tDocument start \tparameters: %S \r\n");
	_LIT8(KInfoOnError,"Error occurs %d \r\n");
		
	iLog.Write(KOnStartDocumentFuncName);

	if (aErrorCode == KErrNone)
	{
		TBuf8<KShortInfoSize> info;
		TBuf8<KShortInfoSize> info2;
		
		info2.Copy(aDocParam.CharacterSetName().DesC());		

		info.Format(KInfoOnStartDocP,&info2);
		iLog.Write(info);
	}
	else
	{
		TBuf8<KShortInfoSize> info;
		info.Format(KInfoOnError,aErrorCode);
		iLog.Write(info);
	}
}


/**
This method is a callback to indicate the end of the document.
@param				aErrorCode is the error code. 
					If this is not KErrNone then special action may be required.
*/
void CTestHandler::OnEndDocumentL(TInt aErrorCode)
{
	_LIT8(KOnEndDocumentFuncName,"OnEndDocument()\r\n");
	_LIT8(KInfoOnEndDoc,"\tDocument end \r\n");
	_LIT8(KInfoOnError,"Error occurs %d \r\n");

	iLog.Write(KOnEndDocumentFuncName);
	
	if (aErrorCode == KErrNone)
	{
		iLog.Write(KInfoOnEndDoc);
	}
	else
	{
		TBuf8<KShortInfoSize> info;
		info.Format(KInfoOnError,aErrorCode);
		iLog.Write(info);
	}
}


/**
This method is a callback to indicate an element has been parsed.
@param				aElement is a handle to the element's details.
@param				aAttributes contains the attributes for the element.
@param				aErrorCode is the error code.
					If this is not KErrNone then special action may be required.
*/
void CTestHandler::OnStartElementL(const RTagInfo& aElement, const RAttributeArray& aAttributes, 
								 TInt aErrorCode)
{
	_LIT8(KOnStartElementFuncName,"OnStartElement()\r\n");
	_LIT8(KInfoOnStartElePU,"\tElement start namespace: %S \t prefix: %S \tname: %S\r\n");
	_LIT8(KInfoOnAttributePU,"\t\tAttribute namaspace: %S \t prefix: %S \tname: %S \t value: %S\r\n");
	_LIT8(KInfoOnError,"Error occurs %d \r\n");

	iLog.Write(KOnStartElementFuncName);
	
	if (aErrorCode == KErrNone)
	{
		
		TBuf8<KShortInfoSize> info;
		TBuf8<KShortInfoSize> info2;
		TBuf8<KShortInfoSize> info3;
		TBuf8<KShortInfoSize> info4;
		TBuf8<KShortInfoSize> info5;

		info2.Copy(aElement.LocalName().DesC());		
		info3.Copy(aElement.Uri().DesC());		
		info4.Copy(aElement.Prefix().DesC());
		
		info.Format(KInfoOnStartElePU,&info3,&info4,&info2);	
		iLog.Write(info);
		
		RArray <RAttribute> array = aAttributes;
		TInt size = array.Count();
		
		RAttribute attr;
		
		if ( size > 0 )
		{
			for ( TInt i = 0; i < size; i++)
			{
				attr = array[i];
				
				info2.Copy(attr.Attribute().LocalName().DesC());
				info3.Copy(attr.Attribute().Uri().DesC());		
				info4.Copy(attr.Attribute().Prefix().DesC());		
				info5.Copy(attr.Value().DesC());		
				
				info.Format(KInfoOnAttributePU,&info3,&info4,&info2,&info5);
				iLog.Write(info);
			}
		}
	}
	else
	{
		TBuf8<KShortInfoSize> info;
		info.Format(KInfoOnError,aErrorCode);
		iLog.Write(info);
	}
}

	
/**
This method is a callback to indicate the end of the element has been reached.
@param				aElement is a handle to the element's details.
@param				aErrorCode is the error code.
					If this is not KErrNone then special action may be required.
*/
void CTestHandler::OnEndElementL(const RTagInfo& aElement, TInt aErrorCode)
{
	_LIT8(KOnEndElementFuncName,"OnEndElement()\r\n");
	_LIT8(KInfoOnEndElePU,"\tElement end namespace: %S \t prefix: %S \tname: %S\r\n");
	_LIT8(KInfoOnError,"Error occurs %d \r\n");

	TBuf8<KShortInfoSize> info;
	TBuf8<KShortInfoSize> info2;
	TBuf8<KShortInfoSize> info3;
	TBuf8<KShortInfoSize> info4;
	
	iLog.Write(KOnEndElementFuncName);
	if (aErrorCode == KErrNone)
	{
		info2.Copy(aElement.LocalName().DesC());		
		info3.Copy(aElement.Uri().DesC());		
		info4.Copy(aElement.Prefix().DesC());
		
		info.Format(KInfoOnEndElePU,&info3,&info4,&info2);	
		iLog.Write(info);
	}
	else
	{
		info.Format(KInfoOnError,aErrorCode);
		iLog.Write(info);
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
void CTestHandler::OnContentL(const TDesC8& aBytes, TInt aErrorCode)
	 {
	_LIT8(KOnContentFuncName,"OnContent()\r\n");
	_LIT8(KInfoOnContent,"\tContent of element: %S \r\n");
	_LIT8(KInfoOnError,"Error occurs %d \r\n");
	TBuf8<KShortInfoSize> info;
	TBuf8<KShortInfoSize> info2;
	iLog.Write(KOnContentFuncName);
	if (aErrorCode == KErrNone)
		{
		if(aBytes.Length() >= KShortInfoSize)
			{ 
			TPtrC8 bytes = aBytes.Ptr();
			HBufC8* buf1 = HBufC8::NewL(aBytes.Length() + 100);
			buf1->Des().Format(KInfoOnContent,&bytes);
			iLog.Write(*buf1);
			}
		info2.Copy(aBytes);
		info2.Trim();
		if (info2.Length())
			{
			info.Format(KInfoOnContent,&info2);
			iLog.Write(info);
			}
		}
		else
			{
			TBuf8<KShortInfoSize> info;
			info.Format(KInfoOnError,aErrorCode);
			iLog.Write(info);
			}
	  }


/**
This method is a callback that sends the binary content of the element.
@param				aBytes is the raw binary content data for the element. 
@param				aErrorCode is the error code.
					If this is not KErrNone then special action may be required.
*/
void CTestHandler::OnBinaryContentL(const TDesC8& aBytes, const TDesC8& aCid, TInt aErrorCode)
{
/*
	//use this part of the code if you want to save binary data to MSWord format
	RFs fs;
	CleanupClosePushL(fs);
	User::LeaveIfError(fs.Connect());
	RFile f;
	CleanupClosePushL(f);
	TInt err = f.Replace(fs, _L("c:\\xmlengine\\deserializer\\output\\bin.doc"), EFileWrite);
	err = f.Write(aBytes);
	CleanupStack::PopAndDestroy(2); //f fs
*/
	aCid.Size(); // eliminate compilation warning
	_LIT8(KOnContentFuncName,"OnBinaryContent()\r\n");
	_LIT8(KInfoOnContent,"\tBinary content of element: %S \r\n");
	_LIT8(KInfoOnError,"Error occurs %d \r\n");
	
	HBufC8* info = HBufC8::NewL(aBytes.Size() + 50);
	TPtr8 infoPtr = info->Des();

	iLog.Write(KOnContentFuncName);

	if (aErrorCode == KErrNone)
	{
		infoPtr.Format(KInfoOnContent, &aBytes);
		iLog.Write(infoPtr);
	}
	else
	{
		TBuf8<KShortInfoSize> info;
		info.Format(KInfoOnError,aErrorCode);
		iLog.Write(info);
	}
	delete info;
}

/**
This method is a callback that sends data container reference
@param				aContainer Container encapsulating binary data
@param				aErrorCode is the error code.
					If this is not KErrNone then special action may be required.
*/
void CTestHandler::OnDataContainerL(const TXmlEngDataContainer& aContainer, TInt aErrorCode)
{
	_LIT8(KOnContentFuncName,"OnDataContainer()\r\n");
	_LIT8(KInfoOnContent,"\tData container CID: %S \r\n");
	_LIT8(KInfoOnError,"Error occurs %d \r\n");
	
	HBufC8* info = HBufC8::NewL(aContainer.Cid().Size() + 50);
	TPtr8 infoPtr = info->Des();

	iLog.Write(KOnContentFuncName);

	if (aErrorCode == KErrNone)
	{   TPtrC8 cid = aContainer.Cid();
		infoPtr.Format(KInfoOnContent, &cid);
		iLog.Write(infoPtr);
	}
	else
	{
		TBuf8<KShortInfoSize> info;
		info.Format(KInfoOnError,aErrorCode);
		iLog.Write(info);
	}
	delete info;	
}
	
/**
This method is a notification of the beginning of the scope of a prefix-URI Namespace mapping.
This method is always called before the corresponding OnStartElementL method.
@param				aPrefix is the Namespace prefix being declared.
@param				aUri is the Namespace URI the prefix is mapped to.
@param				aErrorCode is the error code.
					If this is not KErrNone then special action may be required.
*/
void CTestHandler::OnStartPrefixMappingL(const RString& aPrefix, const RString& aUri, 
									   TInt aErrorCode)
{
	_LIT8(KOnStartPrefMapFuncName,"OnStartPrefixMapping()\r\n");
	_LIT8(KInfoOnStartPref,"\tPrefix mapping start prefix: %S \t uri: %S \r\n");
	_LIT8(KInfoOnError,"Error occurs %d \r\n");

	iLog.Write(KOnStartPrefMapFuncName);
	
	if (aErrorCode == KErrNone)
	{
		TBuf8<KShortInfoSize> info;
		TBuf8<KShortInfoSize> info2;
		TBuf8<KShortInfoSize> info3;
		
		info2.Copy(aPrefix.DesC());		
		info3.Copy(aUri.DesC());		
		
		info.Format(KInfoOnStartPref,&info2,&info3);
	
		iLog.Write(info);
	}
	else
	{
		TBuf8<KShortInfoSize> info;
		info.Format(KInfoOnError,aErrorCode);
		iLog.Write(info);
	}
}

/**
This method is a notification of the end of the scope of a prefix-URI mapping.
This method is called after the corresponding DoEndElementL method.
@param				aPrefix is the Namespace prefix that was mapped.
@param				aErrorCode is the error code.
					If this is not KErrNone then special action may be required.
*/
void CTestHandler::OnEndPrefixMappingL(const RString& aPrefix, TInt aErrorCode)
{
	_LIT8(KOnEndPrefMapFuncName,"OnEndPrefixMapping()\r\n");
	_LIT8(KInfoOnEndPref,"\tPrefix mapping end prefix: %S \r\n");
	_LIT8(KInfoOnError,"Error occurs %d \r\n");
	
	iLog.Write(KOnEndPrefMapFuncName);
	
	if (aErrorCode == KErrNone)
	{
		TBuf8<KShortInfoSize> info;
		TBuf8<KShortInfoSize> info2;
		
		info2.Copy(aPrefix.DesC());		
		
		info.Format(KInfoOnEndPref,&info2);
	
		iLog.Write(info);
	}
	else
	{
		TBuf8<KShortInfoSize> info;
		info.Format(KInfoOnError,aErrorCode);
		iLog.Write(info);
	}
}

/**
This method is a notification of ignorable whitespace in element content.
@param				aBytes are the ignored bytes from the document being parsed.
@param				aErrorCode is the error code.
					If this is not KErrNone then special action may be required.
*/
void CTestHandler::OnIgnorableWhiteSpaceL(const TDesC8& aBytes, TInt aErrorCode)
{
	_LIT8(KOnIgnorWhiteFuncName,"OnIgnorableWhiteSpace()\r\n");	
	_LIT8(KInfoOnIgnorWhiteSpace,"\tIgnoring white space: %S length: %d \r\n");
	_LIT8(KInfoOnError,"Error occurs %d \r\n");
	
	TBuf8<KShortInfoSize> info;
	TBuf8<KShortInfoSize> info2;
	
	iLog.Write(KOnIgnorWhiteFuncName);
	
	if (aErrorCode == KErrNone)
	{
		info2.Copy(aBytes);
		info2.Trim();
		
		if (info2.Length())
		{
			info.Format(KInfoOnIgnorWhiteSpace,&info2,info2.Length());
			iLog.Write(info);
		}
	}
	else
	{
		TBuf8<KShortInfoSize> info;
		info.Format(KInfoOnError,aErrorCode);
		iLog.Write(info);
	}
}

/**
This method is a notification of a skipped entity. If the parser encounters an 
external entity it does not need to expand it - it can return the entity as aName 
for the client to deal with.
@param				aName is the name of the skipped entity.
@param				aErrorCode is the error code.
					If this is not KErrNone then special action may be required.
*/
void CTestHandler::OnSkippedEntityL(const RString& aName, TInt aErrorCode)
{
	_LIT8(KOnSkipEntFuncName,"OnSkippedEntity()\r\n");	
	_LIT8(KInfoOnEndPref,"\tSkipped entity: %S \r\n");
	_LIT8(KInfoOnError,"Error occurs %d \r\n");
	
	iLog.Write(KOnSkipEntFuncName);

	if (aErrorCode == KErrNone)
	{
		TBuf8<KShortInfoSize> info;
		TBuf8<KShortInfoSize> info2;
		
		info2.Copy(aName.DesC());		
		
		info.Format(KInfoOnEndPref,&info2);
	
		iLog.Write(info);
	}
	else
	{
		TBuf8<KShortInfoSize> info;
		info.Format(KInfoOnError,aErrorCode);
		iLog.Write(info);
	}	
}

/**
This method is a receive notification of a processing instruction.
@param				aTarget is the processing instruction target.
@param				aData is the processing instruction data. If empty none was supplied.
@param				aErrorCode is the error code.
					If this is not KErrNone then special action may be required.
*/
void CTestHandler::OnProcessingInstructionL(const TDesC8& aTarget, const TDesC8& aData, 
										  TInt aErrorCode)
{
	_LIT8(KOnProcInstFuncName,"OnProcessingInstruction()\r\n");
	_LIT8(KInfoOnEndEle,"\tProcessing instruction target: %S \t data: %S\r\n");
	_LIT8(KInfoOnError,"Error occurs %d \r\n");

	TBuf8<KShortInfoSize> info;
	
	iLog.Write(KOnProcInstFuncName);
	
	if (aErrorCode == KErrNone)
	{
		info.Format(KInfoOnEndEle,&aTarget,&aData);
		iLog.Write(info);
	}
	else
	{
		info.Format(KInfoOnError,aErrorCode);
		iLog.Write(info);
	}
}

/**
This method indicates an error has occurred.
@param				aError is the error code
*/
void CTestHandler::OnError(TInt aErrorCode)
{
	_LIT8(KOnErrorFuncName,"OnError()\r\n");
	_LIT8(KInfoOnErrorFunc,"\tError function - error occurs %d \r\n");
	
	iLog.Write(KOnErrorFuncName);	
	TBuf8<KShortInfoSize> info;
	info.Format(KInfoOnErrorFunc,aErrorCode);
	iLog.Write(info);
}

/**
This method obtains the interface matching the specified uid.
@return				0 if no interface matching the uid is found.
					Otherwise, the this pointer cast to that interface.
@param				aUid the uid identifying the required interface.
*/
TAny* CTestHandler::GetExtendedInterface(const TInt32 aUid)
	{
	if (aUid == MXmlEngExtendedHandler::EExtInterfaceUid)
		{
		return static_cast<MXmlEngExtendedHandler*>(this);
		}
	else return 0;
	}
