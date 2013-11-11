/*
* Copyright (c) 2000-2009 Nokia Corporation and/or its subsidiary(-ies).
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


#include <e32std.h>
#include <e32base.h>
#include <f32file.h>
#include <banamedplugins.h>
#include <charconv.h>
#include <convnames.h>


_LIT(KLitNameOfResourceFileForBuiltInCharacterSets, "z:\\resource\\charconv\\BUILTIN.RSC");
_LIT(KLitRscExtension, ".RSC");


// CCnvCharacterSetNames

EXPORT_C CCnvCharacterSetNames* CCnvCharacterSetNames::NewL(RFs& aFileServerSession, const TArray<CCnvCharacterSetConverter::SCharacterSet>& aArrayOfCharacterSetsAvailable)
/** Allocates and constructs a localised list of character set encoding names.

@param aFileServerSession A connected session with the file server.
@param aArrayOfCharacterSetsAvailable The array of character sets available 
on the phone. This can be created using CCnvCharacterSetConverter::CreateArrayOfCharacterSetsAvailableL() 
or CreateArrayOfCharacterSetsAvailableLC().
@return Pointer to the newly created object. */
	{
	CCnvCharacterSetNames* arrayOfNamedCharacterSets=NewLC(aFileServerSession, aArrayOfCharacterSetsAvailable);
	CleanupStack::Pop(arrayOfNamedCharacterSets);
	return arrayOfNamedCharacterSets;
	}


EXPORT_C CCnvCharacterSetNames* CCnvCharacterSetNames::NewLC(RFs& aFileServerSession, const TArray<CCnvCharacterSetConverter::SCharacterSet>& aArrayOfCharacterSetsAvailable)
/** Allocates and constructs a localised list of character set encoding names. 
Puts a pointer to the object on the cleanup stack.

@param aFileServerSession A connected session with the file server.
@param aArrayOfCharacterSetsAvailable The array of character sets available 
on the phone. This can be created using CCnvCharacterSetConverter::CreateArrayOfCharacterSetsAvailableL() 
or CreateArrayOfCharacterSetsAvailableLC().
@return Pointer to the newly created object. */
	{
	CCnvCharacterSetNames* arrayOfNamedCharacterSets=new(ELeave) CCnvCharacterSetNames;
	CleanupStack::PushL(arrayOfNamedCharacterSets);
	arrayOfNamedCharacterSets->ConstructL(aFileServerSession, aArrayOfCharacterSetsAvailable);
	return arrayOfNamedCharacterSets;
	}

EXPORT_C CCnvCharacterSetNames::~CCnvCharacterSetNames()
/** Deletes the array of named plug-ins owned by the object prior to its destruction. */
	{
	delete iNamedPlugins;
	}


EXPORT_C TInt CCnvCharacterSetNames::IndexOfIdentifier(TUint aIdentifier) const
/** Gets the index into the sorted list of the character set identifier specified. 

@param aIdentifier The character set identifier. This must not be zero, or 
a panic occurs.
@return The index into the list of the character set identifier, or KErrNotFound 
if the identifier does not exist in the list. */
	{
	return iNamedPlugins->IndexOfUid(TUid::Uid(aIdentifier));
	}


EXPORT_C TUint CCnvCharacterSetNames::IdentifierAtIndex(TInt aIndex) const
/** Gets the character set identifier at the specified index into the list.

@param aIndex The index into the list. Must be within the bounds of the array, 
or a panic occurs.
@return The character set identifier at the specified index. */
	{
	return iNamedPlugins->UidAtIndex(aIndex).iUid;
	}

CCnvCharacterSetNames::CCnvCharacterSetNames()
	:iNamedPlugins(NULL)
	{
	}

void CCnvCharacterSetNames::DestroyResourceFileArray(TAny* aArrayOfCharconvResourceFiles)
	{
	RArray<CBaNamedPlugins::TResourceFile>& arrayOfCharconvResourceFiles=*STATIC_CAST(RArray<CBaNamedPlugins::TResourceFile>*, aArrayOfCharconvResourceFiles);
	for (TInt i=arrayOfCharconvResourceFiles.Count()-1; i>=0; --i)
		{
		const CBaNamedPlugins::TResourceFile& resourceFile=arrayOfCharconvResourceFiles[i];
		delete resourceFile.iFullFileName;
		delete resourceFile.iIdentifier;
		}
	arrayOfCharconvResourceFiles.Close();
	}

void CCnvCharacterSetNames::ConstructL(RFs& aFileServerSession, const TArray<CCnvCharacterSetConverter::SCharacterSet>& aArrayOfCharacterSetsAvailable)
	{
	RArray<CBaNamedPlugins::TResourceFile> arrayOfCharconvResourceFiles;
	CleanupStack::PushL(TCleanupItem(DestroyResourceFileArray, &arrayOfCharconvResourceFiles));
	TParse* parser=new(ELeave) TParse;
	CleanupStack::PushL(parser);
	CBaNamedPlugins::TResourceFile resourceFile;
	resourceFile.iFullFileName=KLitNameOfResourceFileForBuiltInCharacterSets().AllocLC();
	resourceFile.iIdentifier=NULL;
	resourceFile.iUid=KNullUid;
	resourceFile.iFormat=CBaNamedPlugins::TResourceFile::EFormatArrayOfUidNamePairs;
	User::LeaveIfError(arrayOfCharconvResourceFiles.Append(resourceFile));
	CleanupStack::Pop(resourceFile.iFullFileName);
	for (TInt i=aArrayOfCharacterSetsAvailable.Count()-1; i>=0; --i)
		{
		const CCnvCharacterSetConverter::SCharacterSet& characterSet=aArrayOfCharacterSetsAvailable[i];
		if (characterSet.NameIsFileName())
			{
			const TPtrC fileNameOfCharacterSetPlugIn(characterSet.Name());
			User::LeaveIfError(parser->SetNoWild(KLitRscExtension, &fileNameOfCharacterSetPlugIn, NULL));
			CBaNamedPlugins::TResourceFile resourceFile;
			resourceFile.iFullFileName=parser->FullName().AllocLC();
			resourceFile.iIdentifier=NULL;
			resourceFile.iUid=TUid::Uid(characterSet.Identifier());
			resourceFile.iFormat=CBaNamedPlugins::TResourceFile::EFormatTbuf;
			User::LeaveIfError(arrayOfCharconvResourceFiles.Append(resourceFile));
			CleanupStack::Pop(resourceFile.iFullFileName);
			}
		}
	CleanupStack::PopAndDestroy(parser);
	CBaNamedPlugins::CParameters* const parameters=CBaNamedPlugins::CParameters::NewLC(aFileServerSession, arrayOfCharconvResourceFiles.Array());
	iNamedPlugins=CBaNamedPlugins::NewL(*parameters);
	CleanupStack::PopAndDestroy(2, &arrayOfCharconvResourceFiles);
	}

TInt CCnvCharacterSetNames::MdcaCount() const
	{
	return iNamedPlugins->MdcaCount();
	}

TPtrC CCnvCharacterSetNames::MdcaPoint(TInt aIndex) const
	{
	return iNamedPlugins->MdcaPoint(aIndex);
	}

