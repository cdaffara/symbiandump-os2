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
// 8 and 16 bit ini file parser
// 
//

/**
 @file
 @internalAll
*/

#include "IniParserImpl.h"
#include "inifile.h"
#include "s32file.h"

namespace BSUL
{
/**
Creates a 8 bit section content iterator
this iterator is used to navigate through the key value pairs
within the section.Useful when the number of keys within the 
section is unknown.
@param aSectionName the name of the section to iterate
@param aIniDocument the document object containing the section
@return A pointer to a newly created CIniSecIter8 object
@leave	KErrNoMemory if not enough memory
		KErrArgument if aIniDocument is NULL
*/
EXPORT_C CIniSecIter8* CIniSecIter8::NewL(const TDesC8& aSectionName,const CIniDocument8* aIniDocument)
{
	CIniSecIter8* self=new (ELeave)CIniSecIter8();
	CleanupStack::PushL(self);
	self->iImpl=CIniSecIter8Impl::NewL(aSectionName,aIniDocument);
	CleanupStack::Pop();
	return self;	
}

/**
Return the next key value pair within the section
@param aKey a pointer to contain the key name
@param aValue a pointer to contain the key value
@return ETrue if there are keyvalue pairs to return
	    EFalse if it is already end of section
@post the iterator points to the next available keyvalue pair
	  the aKeyName points to  the key name
	  the aKeyValue points to the key value
*/	
EXPORT_C TBool CIniSecIter8::Next(TPtrC8& aKey,TPtrC8& aValue)
{
	return iImpl->Next(aKey,aValue);
}

/**
Look ahead in the section to check whether there is 
still any keyvalue pair in the section to be read
@return ETrue if it is already end of section
	    EFalse indicating the next keyvalue pair exists
*/
EXPORT_C TBool CIniSecIter8::End()
{
	return iImpl->End();
}

/**
Reset the iterator to point to the first keypair value within
the section.
@post the iterator now points to first keypair in the section
*/
EXPORT_C void CIniSecIter8::Reset()
{
	iImpl->Reset();
}

/**
Destructor
*/
EXPORT_C CIniSecIter8::~CIniSecIter8()
{
	delete iImpl;
}

//Default constructor
CIniSecIter8::CIniSecIter8()
{
}

//

/** 
Opening 8 bit ini file for reading.If the supplied file name is a valid
ini file, it will instantiate the object and read the content of 
the ini file.If file not found it will simply instantiate the object.
The file is opened for read only and close directly after the construction
of this object is complete.
User will need to explicitly call Externalise(aFileName) to write the content back to ini file.
@param aFs the handle to the file session
@param aFileName the ini file name to read from
@return A pointer to a newly created CIniDocument8 object
*/
EXPORT_C CIniDocument8* CIniDocument8::NewL(RFs& aFs,const TDesC& aFileName)
	{
	CIniDocument8* self=new (ELeave)CIniDocument8();
	CleanupStack::PushL(self);
	self->iImpl=CIniDocument8Impl::NewL(aFs,aFileName);
	CleanupStack::Pop();
	return self;	
	}

/**
Externalise the buffered content to an output file name
This will first write into a temporary file in the same directory and path
as the supplied aFileName.It will then replace the existing file or 
create a new file if does not exist yet.
@param aFileName the output file name to write to
@return KErrNone if no error
		Other System Wide errors
*/
EXPORT_C TInt CIniDocument8::Externalise(const TDesC& aFileName)
{
	TRAPD(r,iImpl->FlushL(aFileName));
	return r;
}
/**
Compare this document against another for differences.
@param aDoc to compare against.
@return True if same
		Otherwise false
*/
EXPORT_C TBool CIniDocument8::CompareDocs(CIniDocument8& aDoc) 
	{
	return (iImpl->CompareDocs( *(aDoc.iImpl) ));
	}	

/** 
Get an array of the section name present in the ini document object
Note that any items inside this array will be cleared and the array will
be populated with the sections' names from this document object.
@param aSectionList an array of descriptor to contain the section name
@return KErrNone if successful, otherwise another of the system-wide error codes
@post aSectionList contains all the section name in the document object
*/
EXPORT_C TInt CIniDocument8::GetSectionList(RArray<TPtrC8>& aSectionList) const
	{
	return iImpl->GetSectionList(aSectionList);
	}

/**
Get the value of a key within a section
@param aSectionName the section where the key resides
@param aKey the key name
@param aValue pointer to the key value
@return
	KErrNotFound if either section or keyname not found
	KErrNone if successful
@post aKeyValue now points to the key value
*/
EXPORT_C TInt CIniDocument8::GetKeyValue(const TDesC8& aSectionName,const TDesC8& aKey,TPtrC8& aValue) const
	{
	TRAPD(ret,iImpl->GetKeyValueL(aSectionName,aKey,aValue));
	return ret;
	}

/**
Add a section to the ini document object
@param aSectionName the name of the section to be added
@return
	KErrNone if successful
	KErrAlreadyExists if the section with that name already exists
	Any other system wide error code
@post a section with that name is created and added to the document object
*/
EXPORT_C TInt CIniDocument8::AddSection(const TDesC8& aSectionName)
	{
	TRAPD(ret,iImpl->AddSectionL(aSectionName));
	return ret;	
	}

/**
Remove an existing section from the ini document object
@param aSectionName the name of the section to be removed
@return KErrNone if successful
	    KErrNotFound if section does not exist
	    Any other system wide error code
@post if exist that section is removed from the document object	
*/	
EXPORT_C TInt CIniDocument8::RemoveSection(const TDesC8& aSectionName)
	{
	TRAPD(ret,iImpl->RemoveSectionL(aSectionName));
	return ret;
	}

/**
Set the value of a key within a section.
This API offers the following flexibility:
-If section does not exist,create a section and key and value
-If section exists but key does not exist, create the key and value
-Else replace the existing key value with the new value
@param aSectionName the name of the section
@param aKey the name of the key
@param aValue the new value for this key
@return
	KErrNone if successful,any other system wide error code
*/
EXPORT_C TInt CIniDocument8::SetKey(const TDesC8& aSectionName,const TDesC8& aKey,const TDesC8& aValue)
	{
	TRAPD(ret,iImpl->SetKeyL(aSectionName,aKey,aValue));	
	return ret;
	}

/**
Remove an existing key within a section
if supplied section or key name does not exist, it does nothing
@param aSectionName the name of the section where the key resides
@param aKey the name of the key to be removed
@post if exist that key is removed from the section
*/
EXPORT_C TInt CIniDocument8::RemoveKey(const TDesC8& aSectionName,const TDesC8& aKey)
	{
	TRAPD(ret,iImpl->RemoveKeyL(aSectionName,aKey));
	return ret;	
	}

/**
Destructor
*/	
EXPORT_C CIniDocument8::~CIniDocument8()
	{
	delete iImpl;
	}

CIniDocument8::CIniDocument8()
{
}

//
/**
Creates a 8 bit light weight parser
@param aFs the handle to the file session
@param aFileName the ini file name to open
@return A pointer to a newly created CIniFile8 object
@leave	KErrNoMemory if not enough memory
		KErrNotFound if the supplied file does not exist
*/
EXPORT_C CIniFile8* CIniFile8::NewL(RFs& aFs,const TDesC& aFileName)
	{
	CIniFile8* self=new (ELeave)CIniFile8;
	CleanupStack::PushL(self);
	self->iImpl=CIniFile8Impl::NewL(aFs,aFileName);
	CleanupStack::Pop();
	return self;
	}

/**
Public destructor
*/	
EXPORT_C CIniFile8::~CIniFile8()
	{
	delete iImpl;	
	}

/**
Get the value of a key within a section
@param aSectionName the section where the key resides
@param aKeyName the key name
@param aValue pointer to the key value
@return
	KErrNotFound if either section or keyname not found
	KErrNone if successful
*/	
EXPORT_C TInt CIniFile8::FindVar(const TDesC8& aSectionName,const TDesC8& aKeyName,TPtrC8& aValue)	const
	{
	return iImpl->FindVar(aSectionName,aKeyName,aValue);
	}
	
CIniFile8::CIniFile8(){}

//
/**
Creates a 16 bit section content iterator
this iterator is used to navigate through the key value pairs
within the section.Useful when the number of keys within the 
section is unknown.
@param aSectionName the name of the section to iterate
@param aIniDocument the document object containing the section
@return A pointer to a newly created CIniSecIter16 object
@leave	KErrNoMemory if not enough memory
		KErrArgument if aIniDocument is NULL
*/
EXPORT_C CIniSecIter16* CIniSecIter16::NewL(const TDesC16& aSectionName,const CIniDocument16* aIniDocument)
{
	CIniSecIter16* self=new (ELeave)CIniSecIter16();
	CleanupStack::PushL(self);
	self->iImpl=CIniSecIter16Impl::NewL(aSectionName,aIniDocument);
	CleanupStack::Pop();
	return self;	
}

/**
Return the next key value pair within the section
@param aKey a pointer to contain the key name
@param aValue a pointer to contain the key value
@return ETrue if there are keyvalue pairs to return
	    EFalse if it is already end of section
@post the iterator points to the next available keyvalue pair
	  the aKeyName points to  the key name
	  the aKeyValue points to the key value
*/	
EXPORT_C TBool CIniSecIter16::Next(TPtrC16& aKey,TPtrC16& aValue)
{
	return iImpl->Next(aKey,aValue);
}

/**
Look ahead in the section to check whether there is 
still any keyvalue pair in the section to be read
@return ETrue if it is already end of section
	    EFalse indicating the next keyvalue pair exists
*/
EXPORT_C TBool CIniSecIter16::End()
{
	return iImpl->End();
}

/**
Reset the iterator to point to the first keypair value within
the section.
@post the iterator now points to first keypair in the section
*/
EXPORT_C void CIniSecIter16::Reset()
{
	iImpl->Reset();
}

/**
Destructor
*/
EXPORT_C CIniSecIter16::~CIniSecIter16()
{
	delete iImpl;
}

//Default constructor
CIniSecIter16::CIniSecIter16()
{
}

//

/** 
Opening 16 bit ini file for reading.If the supplied file name is a valid
ini file, it will instantiate the object and read the content of 
the ini file.If file not found it will simply instantiate the object.
The file is opened for read only and close directly after the construction
of this object is complete.
User will need to explicitly call Externalise(aFileName) to write the content back to ini file.
@param aFs the handle to the file session
@param aFileName the ini file name to read from
@return A pointer to a newly created CIniDocument16 object
*/
EXPORT_C CIniDocument16* CIniDocument16::NewL(RFs& aFs,const TDesC& aFileName)
	{
	CIniDocument16* self=new (ELeave)CIniDocument16();
	CleanupStack::PushL(self);
	self->iImpl=CIniDocument16Impl::NewL(aFs,aFileName);
	CleanupStack::Pop();
	return self;	
	}

/**
Flush the buffered content to an output file name
This will first write into a temporary file in the same directory and path
as the supplied aFileName.It will then replace the existing file or 
create a new file if does not exist yet.
@param aFileName the output file name to write to
@return KErrNone if no error
		Other System Wide errors
*/
EXPORT_C TInt CIniDocument16::Externalise(const TDesC& aFileName)
{
	TRAPD(r,iImpl->FlushL(aFileName));
	return r;
}
	
/** 
Get an array of the section name present in the ini document object
Note that any items inside this array will be cleared and the array will
be populated with the sections' names from this document object.
@param aSectionList an array of descriptor to contain the section name
@return KErrNone if successful, otherwise another of the system-wide error codes
@post aSectionList contains all the section name in the document object
*/
EXPORT_C TInt CIniDocument16::GetSectionList(RArray<TPtrC16>& aSectionList) const
	{
	return iImpl->GetSectionList(aSectionList);
	}

/**
Get the value of a key within a section
@param aSectionName the section where the key resides
@param aKey the key name
@param aValue pointer to the key value
@return
	KErrNotFound if either section or keyname not found
	KErrNone if successful
@post aKeyValue now points to the key value
*/
EXPORT_C TInt CIniDocument16::GetKeyValue(const TDesC16& aSectionName,const TDesC16& aKey,TPtrC16& aValue) const
	{
	TRAPD(ret,iImpl->GetKeyValueL(aSectionName,aKey,aValue));
	return ret;
	}

/**
Add a section to the ini document object
@param aSectionName the name of the section to be added
@return
	KErrNone if successful
	KErrAlreadyExists if the section with that name already exists
	Any other system wide error code
@post a section with that name is created and added to the document object
*/
EXPORT_C TInt CIniDocument16::AddSection(const TDesC16& aSectionName)
	{
	TRAPD(ret,iImpl->AddSectionL(aSectionName));
	return ret;	
	}

/**
Remove an existing section from the ini document object
@param aSectionName the name of the section to be removed
@return KErrNone if successful
	    KErrNotFound if section does not exist
	    Any other system wide error code
@post if exist that section is removed from the document object	
*/	
EXPORT_C TInt CIniDocument16::RemoveSection(const TDesC16& aSectionName)
	{
	TRAPD(ret,iImpl->RemoveSectionL(aSectionName));
	return ret;
	}

/**
Set the value of a key within a section.
This API offers the following flexibility:
-If section does not exist,create a section and key and value
-If section exists but key does not exist, create the key and value
-Else replace the existing key value with the new value
@param aSectionName the name of the section
@param aKey the name of the key
@param aValue the new value for this key
@return
	KErrNone if successful,any other system wide error code
*/
EXPORT_C TInt CIniDocument16::SetKey(const TDesC16& aSectionName,const TDesC16& aKey,const TDesC16& aValue)
	{
	TRAPD(ret,iImpl->SetKeyL(aSectionName,aKey,aValue));	
	return ret;
	}

/**
Remove an existing key within a section
if supplied section or key name does not exist, it does nothing
@param aSectionName the name of the section where the key resides
@param aKey the name of the key to be removed
@post if exist that key is removed from the section
*/
EXPORT_C TInt CIniDocument16::RemoveKey(const TDesC16& aSectionName,const TDesC16& aKey)
	{
	TRAPD(ret,iImpl->RemoveKeyL(aSectionName,aKey));
	return ret;	
	}
/**
Compare two document objects. If names, keys or values differ, a false is returned,
else a true is returned.
@param aDoc name of the document to compare against this object.
*/
EXPORT_C TBool CIniDocument16::CompareDocs(CIniDocument16& aDoc)	
	{
	return (iImpl->CompareDocs( *(aDoc.iImpl) ));
	}	
/**
Destructor
*/	
EXPORT_C CIniDocument16::~CIniDocument16()
	{
	delete iImpl;
	}

CIniDocument16::CIniDocument16(){}

//
/**
Creates a 16 bit light weight parser
@param aFs the handle to the file session
@param aFileName the ini file name to open
@return A pointer to a newly created CIniFile16 object
@leave	KErrNoMemory if not enough memory
		KErrNotFound if the supplied file does not exist
*/
EXPORT_C CIniFile16* CIniFile16::NewL(RFs& aFs,const TDesC& aFileName)
	{
	return CIniFile16::NewL(aFs,aFileName,EFalse);
	}

/**
Creates a 16 bit light weight parser
@param aFs the handle to the file session
@param aFileName the ini file name to open
@param aConvert8To16 upconvert 8 bit files otherwise leave with KErrCorrupt
@return A pointer to a newly created CIniFile16 object
@leave	KErrNoMemory if not enough memory
		KErrNotFound if the supplied file does not exist
*/
EXPORT_C CIniFile16* CIniFile16::NewL(RFs& aFs,const TDesC& aFileName,TBool aConvert8To16)
	{
	CIniFile16* self=new (ELeave)CIniFile16;
	CleanupStack::PushL(self);
	self->iImpl=CIniFile16Impl::NewL(aFs,aFileName,aConvert8To16);
	CleanupStack::Pop();
	return self;
	}

/**
Public Destructor
*/	
EXPORT_C CIniFile16::~CIniFile16()
	{
	delete iImpl;	
	}

/**
Get the value of a key within a section
@param aSectionName the section where the key resides
@param aKeyName the key name
@param aValue pointer to the key value
@return
	KErrNotFound if either section or keyname not found
	KErrNone if successful
*/		
EXPORT_C TInt CIniFile16::FindVar(const TDesC16& aSectionName,const TDesC16& aKeyName,TPtrC16& aValue)const
	{
	return iImpl->FindVar(aSectionName,aKeyName,aValue);
	}
	
CIniFile16::CIniFile16(){}

//

CIniDocument8Impl* CIniDocument8Impl::NewL(RFs& aFs,const TDesC& aFileName)
{
	CIniDocument8Impl* self= new (ELeave) CIniDocument8Impl();
	CleanupStack::PushL(self);
	self->ConstructL(aFs,aFileName);
	CleanupStack::Pop();
	return self;
}

// This method will panic if, when reading the input configuration file, a key
// is attempted to be processed without a valid section name being already defined.
void CIniDocument8Impl::ConstructL(RFs& aFs,const TDesC& aFileName)
{
	TInt    filesize=0;
	TInt    startOfLine=0;
	CIniSection8* section = NULL;
	HBufC8* fileBuffer=NULL;
	iTempImpl=new (ELeave)CIniDocumentTmpl8(aFs,ETrue);
	
	TRAPD(ret,GetBufferL(aFs,aFileName,fileBuffer));
	//if the file is not found, assume we are creating a new file.
	if (ret==KErrNotFound)
		{
		return;
		}
	User::LeaveIfError(ret);
	if (!fileBuffer)
		{	
		return;	
		}	
	CleanupStack::PushL(fileBuffer);
	filesize = fileBuffer->Length();
	TPtrC8 bufferDescriptor = fileBuffer->Des();
	while (startOfLine < filesize)
		{
		TPtrC8 myBuffer = ExtractLineFromBuffer<HBufC8, TPtrC8>(bufferDescriptor, startOfLine);
		CIniLine8* line = CIniLine8::NewLC(myBuffer);
		startOfLine += (line->LineBuffer()).Length();
		
		switch(line->LineType())
			{
			case ESection:
			 	section = CIniSection8::NewLC(line);
			 	iTempImpl->AddSectionL(section);
			 	CleanupStack::Pop(section);
			 	break;
			 	
			case EKeyValue:
				{
				CIniKey8* key = CIniKey8::NewLC(line);
				if (section == NULL)
					{
					User::Leave(KErrCorrupt);	//Unnamed sections within the file are not allowed but not preventable, hence leave if found. 
					}
			 	section->InsertKeyL(key);
			 	CleanupStack::Pop(key);
			 	break;
				}
				
			case EComment:
				break;
				
			default:
				User::Panic(_L("Invalid LineType"), KErrCorrupt);	// programming error.
			}
		iTempImpl->AppendIntoQueue(line);
		CleanupStack::Pop(line);
		}
	CleanupStack::PopAndDestroy(fileBuffer);
}

void CIniDocument8Impl::FlushL(const TDesC& aFileName)
{
	iTempImpl->FlushL(aFileName);
}

TInt CIniDocument8Impl::GetSectionList(RArray<TPtrC8>& aSectionList) const
{
	return iTempImpl->GetSectionList(aSectionList);
}

void CIniDocument8Impl::GetKeyValueL(const TDesC8& aSectionName,const TDesC8& aKeyName,TPtrC8& aKeyValue) const
{
	iTempImpl->GetKeyValueL(aSectionName,aKeyName,aKeyValue);
}

void CIniDocument8Impl::AddSectionL(const TDesC8& aSectionName)
{
	iTempImpl->AddSectionL(aSectionName);
}

void CIniDocument8Impl::RemoveSectionL(const TDesC8& aSectionName)
{
	iTempImpl->RemoveSectionL(aSectionName);
}

void CIniDocument8Impl::SetKeyL(const TDesC8& aSectionName,const TDesC8& aKeyName,const TDesC8& aKeyValue)
{
	iTempImpl->SetKeyL(aSectionName,aKeyName,aKeyValue);
}

void CIniDocument8Impl::RemoveKeyL(const TDesC8& aSectionName,const TDesC8& aKeyName)
{
	iTempImpl->RemoveKeyL(aSectionName,aKeyName);
}

CIniDocument8Impl::~CIniDocument8Impl()
{
	delete iTempImpl;
}

CIniSection8* CIniDocument8Impl::SectionL(const TDesC8& aSectionName) const
{
	return iTempImpl->SectionL(aSectionName);
}

TBool CIniDocument8Impl::CompareDocs(CIniDocument8Impl& aDocImpl) 
{
	return(iTempImpl->CompareDocs(*aDocImpl.iTempImpl));
}

//
CIniDocument16Impl* CIniDocument16Impl::NewL(RFs& aFs,const TDesC& aFileName)
{
	CIniDocument16Impl* self= new (ELeave) CIniDocument16Impl();
	CleanupStack::PushL(self);
	self->ConstructL(aFs,aFileName);
	CleanupStack::Pop();
	return self;
}

void CIniDocument16Impl::ConstructL(RFs& aFs,const TDesC& aFileName)
{
	TInt    filesize=0;
	TInt    startOfLine=0;
	CIniSection16* section = NULL;
	HBufC8* fileBuffer=NULL;
	iTempImpl=new (ELeave)CIniDocumentTmpl16(aFs,EFalse);
	
	TRAPD(ret,GetBufferL(aFs,aFileName,fileBuffer));
	//if the file is not found, assume we are creating a new file.
	if (ret==KErrNotFound)
		{
		return;
		}
	User::LeaveIfError(ret);
	if (!fileBuffer)
		{	
		return;	
		}	
	CleanupStack::PushL(fileBuffer);
	filesize = fileBuffer->Length()/2;
	
	// process the filemark at the start of the file.
	const TUint16* rawptr16=reinterpret_cast<const TUint16*>(fileBuffer->Ptr());
	TPtrC16 bufferPtr;
	
	//must always start with the byte ordering characters
	bufferPtr.Set(rawptr16,1);
	if (bufferPtr.Compare(_L("\xFEFF"))!=0)
		User::Leave(KErrCorrupt);
	//skip the byte ordering character(FEFF) assuming little endian
	bufferPtr.Set(rawptr16+1,(filesize - 1));	
	
	while (startOfLine < (filesize-1))
		{
		TPtrC16 myBuffer = ExtractLineFromBuffer<HBufC16, TPtrC16>(bufferPtr, startOfLine);
		CIniLine16* line = CIniLine16::NewLC(myBuffer);
		startOfLine += (line->LineBuffer()).Length();
		
		TLineType lineType = line->LineType();
		switch (lineType)
			{
			case ESection:
				section = CIniSection16::NewLC(line);
				iTempImpl->AddSectionL(section);
			 	CleanupStack::Pop(section);
			 	break;
			 	
			case EKeyValue:
				{
				CIniKey16* key = CIniKey16::NewLC(line);
				
				if (section == NULL)
					{
					User::Leave(KErrCorrupt);	//Unnamed sections are not allowed hence leave if found.
					}
				section->InsertKeyL(key);
				CleanupStack::Pop(key);
			 	break;
				}
				
			case EComment:
				break;
				
			default:
				User::Panic(_L("Invalid LineType"), KErrCorrupt);
			}
		iTempImpl->AppendIntoQueue(line);
		CleanupStack::Pop(line);
	}	
	CleanupStack::PopAndDestroy(fileBuffer);
}

void CIniDocument16Impl::FlushL(const TDesC& aFileName)
{
	iTempImpl->FlushL(aFileName);
}

TInt CIniDocument16Impl::GetSectionList(RArray<TPtrC16>& aSectionList) const
{
	return iTempImpl->GetSectionList(aSectionList);
}

void CIniDocument16Impl::GetKeyValueL(const TDesC16& aSectionName,const TDesC16& aKeyName,TPtrC16& aKeyValue) const
{
	iTempImpl->GetKeyValueL(aSectionName,aKeyName,aKeyValue);
}

void CIniDocument16Impl::AddSectionL(const TDesC16& aSectionName)
{
	iTempImpl->AddSectionL(aSectionName);
}

void CIniDocument16Impl::RemoveSectionL(const TDesC16& aSectionName)
{
	iTempImpl->RemoveSectionL(aSectionName);
}

void CIniDocument16Impl::SetKeyL(const TDesC16& aSectionName,const TDesC16& aKeyName,const TDesC16& aKeyValue)
{
	iTempImpl->SetKeyL(aSectionName,aKeyName,aKeyValue);
}

void CIniDocument16Impl::RemoveKeyL(const TDesC16& aSectionName,const TDesC16& aKeyName)
{
	iTempImpl->RemoveKeyL(aSectionName,aKeyName);
}

CIniDocument16Impl::~CIniDocument16Impl()
	{
	delete iTempImpl;
	}

CIniSection16* CIniDocument16Impl::SectionL(const TDesC16& aSectionName) const
{
	return iTempImpl->SectionL(aSectionName);
}

TBool CIniDocument16Impl::CompareDocs(CIniDocument16Impl& aDocImpl) 
{
	return(iTempImpl->CompareDocs(*(aDocImpl.iTempImpl)));
}

//	

CIniSecIter8Impl* CIniSecIter8Impl::NewL(const TDesC8& aSectionName,const CIniDocument8* aIniDocument) 
	{
	CIniSecIter8Impl* self=new (ELeave)CIniSecIter8Impl();
	CleanupStack::PushL(self);
	self->ConstructL(aSectionName,aIniDocument);
	CleanupStack::Pop();
	return self;
	}

void CIniSecIter8Impl::ConstructL(const TDesC8& aSectionName,const CIniDocument8* aIniDocument)
	{
	iTempImpl=new (ELeave)CIniSecIterTmpl8();
	if (!aIniDocument)
		User::Leave(KErrArgument);
	iTempImpl->iSection=aIniDocument->iImpl->SectionL(aSectionName);
	}
	
TBool CIniSecIter8Impl::Next(TPtrC8& aKeyName,TPtrC8& aKeyValue)	
	{
	return iTempImpl->Next(aKeyName,aKeyValue);
	}
	
void CIniSecIter8Impl::Reset()
	{
	iTempImpl->Reset();	
	}

//
CIniSecIter16Impl* CIniSecIter16Impl::NewL(const TDesC16& aSectionName,const CIniDocument16* aIniDocument) 
	{
	CIniSecIter16Impl* self=new (ELeave)CIniSecIter16Impl();
	CleanupStack::PushL(self);
	self->ConstructL(aSectionName,aIniDocument);
	CleanupStack::Pop();
	return self;
	}

void CIniSecIter16Impl::ConstructL(const TDesC16& aSectionName,const CIniDocument16* aIniDocument)
	{
	iTempImpl=new (ELeave)CIniSecIterTmpl16();
	if (!aIniDocument)
		User::Leave(KErrArgument);	
	iTempImpl->iSection=aIniDocument->iImpl->SectionL(aSectionName);
	}
	
TBool CIniSecIter16Impl::Next(TPtrC16& aKeyName,TPtrC16& aKeyValue)	
	{
	return iTempImpl->Next(aKeyName,aKeyValue);
	}
	
void CIniSecIter16Impl::Reset()
	{
	iTempImpl->Reset();
	}

//
CIniFile8Impl* CIniFile8Impl::NewL(RFs& aFs,const TDesC& aFileName)
	{
	CIniFile8Impl* self=new (ELeave)CIniFile8Impl;
	CleanupStack::PushL(self);
	self->ConstructL(aFs,aFileName);
	CleanupStack::Pop();
	return self;
	}

void CIniFile8Impl::ConstructL(RFs& aFs,const TDesC& aFileName)
	{
	iTempImpl=new (ELeave)CIniFileTmpl8();
	HBufC8* tempBuffer=NULL;
	//read the file
	GetBufferL(aFs,aFileName,tempBuffer);
	if (tempBuffer)
		{
		CleanupStack::PushL(tempBuffer);
		iTempImpl->ProcessBufferL(*tempBuffer);
		CleanupStack::Pop();
		delete tempBuffer;
		}
	}

TInt CIniFile8Impl::FindVar(const TDesC8& aSection,const TDesC8& aKey,TPtrC8& aValue)
	{
	return iTempImpl->FindVar(aSection,aKey,aValue);	
	}

//
CIniFile16Impl* CIniFile16Impl::NewL(RFs& aFs,const TDesC& aFileName,TBool aConvert8To16)
	{
	CIniFile16Impl* self=new (ELeave)CIniFile16Impl;
	CleanupStack::PushL(self);
	self->ConstructL(aFs,aFileName,aConvert8To16);
	CleanupStack::Pop();
	return self;
	}

void CIniFile16Impl::ConstructL(RFs& aFs,const TDesC& aFileName,TBool aConvert8To16)
	{
	iTempImpl=new (ELeave)CIniFileTmpl16();
	HBufC8* tempBuffer=NULL;
	//read the file
	GetBufferL(aFs,aFileName,tempBuffer);
	if (tempBuffer)
		{
		CleanupStack::PushL(tempBuffer);
		TPtrC16 bufferPtr;
		const TUint16* rawptr16=reinterpret_cast<const TUint16*>(tempBuffer->Ptr());
		//must always start with the byte ordering characters
		bufferPtr.Set(rawptr16,1);
		if (bufferPtr.Compare(_L("\xFEFF"))==0)
			{
			//skip the byte ordering character(FEFF) assuming little endian
			bufferPtr.Set(rawptr16+1,(tempBuffer->Length()/2)-1);
			}
		else
			{
			//byte ordering characters not found, so leave, unless we should upconvert
			if (!aConvert8To16)
				{			
				User::Leave(KErrCorrupt);
				}
			//treat as an 8-bit file and upconvert to 16
			HBufC16* tempBuffer16=HBufC16::NewL(tempBuffer->Length());
			tempBuffer16->Des().Copy(*tempBuffer);
			CleanupStack::PopAndDestroy(tempBuffer);
			CleanupStack::PushL(tempBuffer16);	
			bufferPtr.Set(*tempBuffer16);
			}	
		iTempImpl->ProcessBufferL(bufferPtr);
		CleanupStack::PopAndDestroy();
		}
	}

TInt CIniFile16Impl::FindVar(const TDesC16& aSection,const TDesC16& aKey,TPtrC16& aValue)
	{
	return iTempImpl->FindVar(aSection,aKey,aValue);	
	}
	
}//namespace


