/*
* Copyright (c) 2003-2009 Nokia Corporation and/or its subsidiary(-ies).
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

#include <apgcli.h>
#include <apmstd.h>
#include <caf/supplieroutputfile.h>

#include <caf/caftypes.h>
#include "resolver.h"

#ifndef REMOVE_CAF1
#include <caf/attribute.h>
#include <caf/content.h>
#include <caf/bitset.h>
#endif


using namespace ContentAccess;

#ifndef REMOVE_CAF1
EXPORT_C CSupplierOutputFile* CSupplierOutputFile::NewL(const TDesC& aFileName, const TOutputType aOutputType)
	{
	return CSupplierOutputFile::NewL(aFileName, aOutputType, KNullDesC8());
	}
#endif

EXPORT_C CSupplierOutputFile* CSupplierOutputFile::NewL(const TDesC& aFileName, const TOutputType aOutputType, const TDesC8& aMimeType)
	{
	CSupplierOutputFile *self = new (ELeave) CSupplierOutputFile();
	CleanupStack::PushL(self);
	self->ConstructL(aFileName, aMimeType, aOutputType);
	CleanupStack::Pop(self);
	return self;
	}

EXPORT_C CSupplierOutputFile* CSupplierOutputFile::NewL(RReadStream& aStream)
	{
	CSupplierOutputFile *self = new (ELeave) CSupplierOutputFile();
	CleanupStack::PushL(self);
	self->InternalizeL(aStream);
	CleanupStack::Pop(self);
	return self;		
	}

CSupplierOutputFile::CSupplierOutputFile()
	{
	}
	

void CSupplierOutputFile::ConstructL(const TDesC& aFileName, const TDesC8& aMimeType, const TOutputType aOutputType)
	{
	iOutputType = aOutputType;
	
	CAgentResolver *resolver = CAgentResolver::NewL(EFalse);
	CleanupStack::PushL(resolver);
	iFileName = resolver->ConvertAgentFileNameL(aFileName);
	CleanupStack::PopAndDestroy(resolver);
	iMimeType.Copy(aMimeType);
	
	// convert mime type to lower case
	iMimeType.LowerCase();
	}


CSupplierOutputFile::~CSupplierOutputFile()
	{
	delete iFileName;
#ifndef REMOVE_CAF1
	delete iAttr;
#endif
	}


EXPORT_C TPtrC CSupplierOutputFile::FileName() const
	{
	return *iFileName;
	}

EXPORT_C TOutputType CSupplierOutputFile::OutputType() const
	{
	return iOutputType;
	}

EXPORT_C TPtrC8 CSupplierOutputFile::MimeTypeL()
	{
	TUid uid;
	TDataType dataType;
	RApaLsSession apparcSession;

	if(iMimeType.Length() == 0)
		{
		// Use the Application Architecture Server to find the Mime type of the output File
		User::LeaveIfError(apparcSession.Connect());
		CleanupClosePushL(apparcSession);
		    
   		// Use a temp FS session to preserve platsec 
		RFs tempFsSession; 
		User::LeaveIfError(tempFsSession.Connect());     
	  	CleanupClosePushL(tempFsSession);
	  	User::LeaveIfError(tempFsSession.ShareProtected());  
	  	
	  	// Open file here and send the handle because AppArc doesn't have Allfiles capability
#ifdef SYMBIAN_ENABLE_64_BIT_FILE_SERVER_API
	  	// If file size is greater than 2GB than RFile::Open will fail with KErrTooBig.
	  	// So RFile64.
	  	RFile64 tempFile; 
#else
	  	RFile tempFile;
#endif //SYMBIAN_ENABLE_64_BIT_FILE_SERVER_API
	  	User::LeaveIfError(tempFile.Open(tempFsSession, *iFileName, EFileRead|EFileShareAny)); 
	  	CleanupClosePushL(tempFile); 
   
		User::LeaveIfError(apparcSession.AppForDocument(tempFile, uid, dataType)); 

		iMimeType.Copy(dataType.Des8().Left(KMaxDataTypeLength)); 

		CleanupStack::PopAndDestroy(&tempFile); // close 
		CleanupStack::PopAndDestroy(&tempFsSession);    // close 
		CleanupStack::PopAndDestroy(&apparcSession);	// close
		}

	return iMimeType;
	}

EXPORT_C void CSupplierOutputFile::ExternalizeL(RWriteStream& aStream) const
	{
	aStream.WriteInt32L(static_cast <TInt> (iOutputType));
	aStream.WriteInt32L(iFileName->Des().Length());
	aStream.WriteL(*iFileName);
	aStream.WriteInt32L(iMimeType.Length());
	aStream.WriteL(iMimeType);
	}

void CSupplierOutputFile::InternalizeL(RReadStream& aStream)
	{
	TInt length = 0;
	iOutputType = static_cast <TOutputType> (aStream.ReadInt32L());
	length = aStream.ReadInt32L();
	iFileName = HBufC::NewL(length);
	if(length)
		{
		TPtr filename = iFileName->Des();
		aStream.ReadL(filename, length);
		}
	length = aStream.ReadInt32L();
	aStream.ReadL(iMimeType, length);
	}



#ifndef REMOVE_CAF1
EXPORT_C CAttribute &  CSupplierOutputFile::AttributesL (TBool aPreloaded) 
	{
	// retrieve the attributes, only need to do this once since this instance
	// will only ever refer to one output file 
	if(!iAttr)
		{
		// create a content consumer interface for the output file produced
		CContent* content = CContent::NewLC(FileName());	
		iAttr = content->NewAttributeL(ETrue);
		CleanupStack::PopAndDestroy(content);
		}
	else if(aPreloaded)  // update all attribute bits
		{
		CBitset &bitset = iAttr->QuerySet();
		bitset.SetAll();
		iAttr->GetL();
		}
	return *iAttr;
	}
#endif // #ifndef REMOVE_CAF1
