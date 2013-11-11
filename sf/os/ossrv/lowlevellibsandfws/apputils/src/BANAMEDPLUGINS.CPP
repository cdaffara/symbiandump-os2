// Copyright (c) 2000-2009 Nokia Corporation and/or its subsidiary(-ies).
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

#include <e32std.h>
#include <e32base.h>
#include <f32file.h>
#include <charconv.h>
#include <bamdesca.h>
#include <bautils.h>
#include <barsc.h>
#include <barsread.h>
#include <baflpan.h>
#include <banamedplugins.h>

// CBaNamedPlugins

EXPORT_C CBaNamedPlugins* CBaNamedPlugins::NewL(const CParameters& aParameters)
/** Allocates and constructs a new list of plug-in names. The list is populated 
using the CParameters object passed into the function. The CParameters object 
can be destroyed once the CBaNamedPlugins object has been created.

@param aParameters The parameters for the list of plug-in names. 
@return The new list of plug-in names. */
	{
	CBaNamedPlugins* const namedPlugIns=NewLC(aParameters);
	CleanupStack::Pop(namedPlugIns);
	return namedPlugIns;
	}

EXPORT_C CBaNamedPlugins* CBaNamedPlugins::NewLC(const CParameters& aParameters)
/** Allocates and constructs a new list of plug-in names. The list is populated 
using the CParameters object passed into the function. The list is left on 
the cleanup stack. The CParameters object can be destroyed once the 
CBaNamedPlugins object has been created.

@param aParameters The parameters for the list of plug-in names.
@return The new list of plug-in names. */
	{
	CBaNamedPlugins* const namedPlugIns=new(ELeave) CBaNamedPlugins(Max(aParameters.iArrayOfResourceFiles->Count(), 1));
	CleanupStack::PushL(namedPlugIns);
	namedPlugIns->ConstructL(aParameters);
	return namedPlugIns;
	}

EXPORT_C CBaNamedPlugins::~CBaNamedPlugins()
/** Destructor. Deletes all resources owned by the object prior to its 
destruction. */
	{
	for (TInt i=iArrayOfNamedPlugIns.Count()-1; i>=0; --i)
		{
		delete iArrayOfNamedPlugIns[i].iName;
		delete iArrayOfNamedPlugIns[i].iIdentifier;
		}
	iArrayOfNamedPlugIns.Close();
	}

EXPORT_C TInt CBaNamedPlugins::IndexOfUid(TUid aUid) const
/** Gets the index into the sorted list (i.e. the index into the MDesCArray) of 
the plug-in associated with the UID specified.

@param aUid A plug-in UID to search for. Its value must not be KNullUid or 
a panic occurs.
@return The index into the list of the plug-in with the UID specified, or 
KErrNotFound if no plug-in has the specified UID. */
	{
	__ASSERT_ALWAYS(aUid.iUid!=KNullUid.iUid, Panic(EBafPanicNullUid));
	for (TInt i=iArrayOfNamedPlugIns.Count()-1; i>=0; --i)
		{
		if (iArrayOfNamedPlugIns[i].iUid.iUid==aUid.iUid)
			{
			return i;
			}
		}
	return KErrNotFound;
	}

EXPORT_C TInt CBaNamedPlugins::IndexOfIdentifier(const TDesC& aIdentifier, TEquivalentIdentifiers aEquivalentIdentifiers) const
/** Gets the index into the sorted list (i.e. the index into the MDesCArray) of 
the plug-in associated with the identifier specified.

@param aIdentifier The plug-in identifier to search for.
@param aEquivalentIdentifiers A function which tests whether two plug-in 
identifiers are the same, returning true or false.
@return The index into the list of the plug-in with the identifier specified, 
or KErrNotFound if no plug-in has the specified identifier. */
	{
	for (TInt i=iArrayOfNamedPlugIns.Count()-1; i>=0; --i)
		{
		const TDesC* const identifier=iArrayOfNamedPlugIns[i].iIdentifier;
		if ((identifier!=NULL) && (*aEquivalentIdentifiers)(*identifier, aIdentifier))
			{
			return i;
			}
		}
	return KErrNotFound;
	}

EXPORT_C TUid CBaNamedPlugins::UidAtIndex(TInt aIndex) const
/** Gets the UID of the plug-in at the specified index into the MDesCArray.

@param aIndex The index into the MDesCArray. Must be within the bounds of 
the array, or a panic occurs.
@return The UID of the plug-in at the specified index. */
	{
	return iArrayOfNamedPlugIns[aIndex].iUid;
	}

EXPORT_C const TDesC* CBaNamedPlugins::IdentifierAtIndex(TInt aIndex) const
/** Gets the identifier of the plug-in at the specified index into the MDesCArray.

@param aIndex The index into the MDesCArray. Must be within the bounds of 
the array, or a panic occurs.
@return The identifier of the plug-in at the specified index. */
	{
	return iArrayOfNamedPlugIns[aIndex].iIdentifier;
	}

EXPORT_C TInt CBaNamedPlugins::MdcaCount() const
/** Gets the number of plug-ins in the list.

@return The number of plug-ins in the list. */
	{
	return iArrayOfNamedPlugIns.Count();
	}

EXPORT_C TPtrC CBaNamedPlugins::MdcaPoint(TInt aIndex) const
/** Returns a TPtrC for the name of the plug-in at the given index.

@param aIndex The index into the list. Must be within the bounds of the array, 
or a panic occurs.
@return The name of the plug-in at the given index. */
	{
	return *iArrayOfNamedPlugIns[aIndex].iName;
	}

CBaNamedPlugins::CBaNamedPlugins(TInt aGranularity)
	:iArrayOfNamedPlugIns(aGranularity)
	{
	}

void CBaNamedPlugins::ConstructL(const CParameters& aParameters)
	{
	TFileName* const fileName=new(ELeave) TFileName;
	CleanupStack::PushL(fileName);
	RResourceFile resourceFile;
	CleanupClosePushL(resourceFile);
	const TCompareNames compareNames=(aParameters.iCompareNames!=NULL)? aParameters.iCompareNames: DefaultAlgorithmToCompareNames;
	for (TInt i=aParameters.iArrayOfResourceFiles->Count()-1; i>=0; --i)
		{
		const TResourceFile& resourceFileData=(*aParameters.iArrayOfResourceFiles)[i];
		*fileName=*resourceFileData.iFullFileName;
		BaflUtils::NearestLanguageFile(aParameters.iFileServerSession, *fileName);
		TNamedPlugIn namedPlugIn;
		namedPlugIn.iIdentifier=(resourceFileData.iIdentifier==NULL)? NULL: resourceFileData.iIdentifier->AllocLC();
		namedPlugIn.iUid=resourceFileData.iUid;
		namedPlugIn.iCompareNames=compareNames;
		if (!BaflUtils::FileExists(aParameters.iFileServerSession, *fileName))
			{
			if (aParameters.iFallBackName==NULL)
				{
				namedPlugIn.iName=TParsePtrC(*resourceFileData.iFullFileName).Name().AllocLC();
				}
			else
				{
				namedPlugIn.iName=aParameters.iFallBackName->FallBackNameL(*resourceFileData.iFullFileName);
				CleanupStack::PushL(namedPlugIn.iName);
				}
			User::LeaveIfError(iArrayOfNamedPlugIns.Append(namedPlugIn));
			CleanupStack::Pop(namedPlugIn.iName);
			}
		else
			{
			resourceFile.Close();
			resourceFile.OpenL(aParameters.iFileServerSession, *fileName);
			HBufC8* const resource=resourceFile.AllocReadLC(resourceFile.Offset()+2); // read the first resource after the RSS_SIGNATURE resource
			switch (resourceFileData.iFormat)
				{
			case TResourceFile::EFormatTbuf:
				{
				const TPtrC name(REINTERPRET_CAST(const TText*, resource->Ptr()), resource->Length()/sizeof(TText));
				if (name.Length()>0)
					{
					namedPlugIn.iName=name.AllocLC();
					User::LeaveIfError(iArrayOfNamedPlugIns.Append(namedPlugIn));
					CleanupStack::Pop(namedPlugIn.iName);
					}
				}
				break;
			case TResourceFile::EFormatArrayOfUidNamePairs:
				{
				TResourceReader resourceReader;
				resourceReader.SetBuffer(resource);
				for (TInt j=resourceReader.ReadUint16()-1; j>=0; --j)
					{
					namedPlugIn.iUid=TUid::Uid(resourceReader.ReadUint32());
					const TPtrC name(resourceReader.ReadTPtrC());
					if (name.Length()>0)
						{
						namedPlugIn.iName=name.AllocLC();
						User::LeaveIfError(iArrayOfNamedPlugIns.Append(namedPlugIn));
						CleanupStack::Pop(namedPlugIn.iName);
						}
					}
				}
				break;
			default:
				Panic(EBafPanicBadResourceFileFormat);
				break;
				}
			CleanupStack::PopAndDestroy(resource);
			}
		if (namedPlugIn.iIdentifier!=NULL)
			{
			CleanupStack::Pop(namedPlugIn.iIdentifier);
			}
		}
	CleanupStack::PopAndDestroy(2, fileName);
	iArrayOfNamedPlugIns.Sort(TLinearOrder<TNamedPlugIn>(CompareNamedPlugIns));
	if (aParameters.iTextForNone!=NULL)
		{
		TNamedPlugIn namedPlugIn;
		namedPlugIn.iName=aParameters.iTextForNone->AllocLC();
		namedPlugIn.iIdentifier=NULL;
		namedPlugIn.iUid=KNullUid;
		namedPlugIn.iCompareNames=NULL;
		switch (aParameters.iArrayPositionOfTextForNone)
			{
		case EArrayPositionFirst:
			User::LeaveIfError(iArrayOfNamedPlugIns.Insert(namedPlugIn, 0));
			break;
		case EArrayPositionLast:
			User::LeaveIfError(iArrayOfNamedPlugIns.Append(namedPlugIn));
			break;
		default:
			Panic(EBafPanicBadArrayPosition);
			break;
			}
		CleanupStack::Pop(namedPlugIn.iName);
		}
	}

TInt CBaNamedPlugins::CompareNamedPlugIns(const TNamedPlugIn& aNamedPlugIn1, const TNamedPlugIn& aNamedPlugIn2)
	{
	__ASSERT_DEBUG((aNamedPlugIn1.iCompareNames!=NULL) && (aNamedPlugIn1.iCompareNames==aNamedPlugIn2.iCompareNames), Panic(EBafPanicBadCompareNames));
	return (*aNamedPlugIn1.iCompareNames)(*aNamedPlugIn1.iName, *aNamedPlugIn2.iName);
	}

TInt CBaNamedPlugins::DefaultAlgorithmToCompareNames(const TDesC& aName1, const TDesC& aName2)
	{
	return aName1.CompareC(aName2);
	}

// CBaNamedPlugins::MFallBackName
/**
@internalAll
*/
EXPORT_C void CBaNamedPlugins::MFallBackName::MFallBackName_Reserved_1()
	{
	}
/**
@internalAll
*/
EXPORT_C void CBaNamedPlugins::MFallBackName::MFallBackName_Reserved_2()
	{
	}

// CBaNamedPlugins::CParameters

EXPORT_C CBaNamedPlugins::CParameters* CBaNamedPlugins::CParameters::NewL(RFs& aFileServerSession, const TArray<TResourceFile>& aArrayOfResourceFiles)
/** Allocates and constructs a new parameters object. 
	
@param aFileServerSession A connected session with the file server. This is 
required to search the file sytem for the localised resource files, and to 
open them for reading.
@param aArrayOfResourceFiles Array of TResourceFile objects. Each object 
contains information about a single plug-in, if its iFormat attribute is set 
to EFormatTbuf or potentially multiple plug-ins if its iFormat attribute is 
set to EFormatArrayOfUidNamePairs. This information includes the filename of 
its resource file. The CParameters object takes a copy of this array.
@return The new parameters object. */
	{
	CParameters* const parameters=NewLC(aFileServerSession, aArrayOfResourceFiles);
	CleanupStack::Pop(parameters);
	return parameters;
	}

EXPORT_C CBaNamedPlugins::CParameters* CBaNamedPlugins::CParameters::NewLC(RFs& aFileServerSession, const TArray<TResourceFile>& aArrayOfResourceFiles)
/** Allocates and constructs a new parameters object. The object is left on the 
cleanup stack. 

@param aFileServerSession A connected session with the file server. This is 
required to search the file sytem for the localised resource files and to 
open them for reading.
@param aArrayOfResourceFiles Array of TResourceFile objects. Each object 
contains information about a single plug-in, if its iFormat attribute is set 
to EFormatTbuf or potentially multiple plug-ins if its iFormat attribute is 
set to EFormatArrayOfUidNamePairs. This information includes the filename of 
its resource file. The CParameters object takes a copy of this array.
@return The new parameters object. */
	{
	CParameters* const parameters=new(ELeave) CParameters(aFileServerSession);
	CleanupStack::PushL(parameters);
	parameters->ConstructL(aArrayOfResourceFiles);
	return parameters;
	}


EXPORT_C CBaNamedPlugins::CParameters::~CParameters()
/** Destructor. Deletes all resources owned by the object. */
	{
	delete iArrayOfResourceFiles;
	delete iTextForNone;
	}

EXPORT_C void CBaNamedPlugins::CParameters::SetFallBackName(const MFallBackName& aFallBackName)
/** Sets a function that generates a fallback name for plug-ins for which no 
resource file could be found. If SetFallBackName() is not called, then by 
default the fallback name used for plug-ins is simply the filename of the 
resource file without the drive, directory path or extension. 

@param aFallBackName An instance of an MFallBackName-derived class. This should 
implement a function which creates a name for plug-ins for which there is 
no resource available (the "fallback" name). */
	{
	iFallBackName=&aFallBackName;
	}

EXPORT_C void CBaNamedPlugins::CParameters::SetCompareNames(TCompareNames aCompareNames)
/** Sets a function that compares two plug-in names for sorting. The plug-in 
names list is sorted after it has been fully populated, using this algorithm. 
If SetCompareNames() is not called, collation takes place by default using 
TDesC::CompareC().
	
@param aCompareNames A function that compares two plug-in names for sorting. */
	{
	iCompareNames=aCompareNames;
	}

EXPORT_C void CBaNamedPlugins::CParameters::SetTextForNoneL(const TDesC& aTextForNone, TArrayPosition aArrayPositionOfTextForNone)
/** Sets a text string, representing the choice of no plug-in and the array 
position at which to insert it. This function increases the length of the 
plug-in names list by one because it creates and adds an item to the array 
which is empty except for the text string specified.

@param aTextForNone The string whose meaning is "none", i.e. no plug-in. It 
is assumed that this descriptor has already been localised.
@param aArrayPositionOfTextForNone Whether the string should be inserted at 
the start or appended to the end of the array. */
	{
	HBufC* const textForNone=aTextForNone.AllocL();
	delete iTextForNone;
	iTextForNone=textForNone;
	iArrayPositionOfTextForNone=aArrayPositionOfTextForNone;
	}


EXPORT_C void CBaNamedPlugins::CParameters::SetTextForNone(HBufC* aTextForNone, TArrayPosition aArrayPositionOfTextForNone)
/** Sets a text string, representing the choice of no plug-in and the array 
position at which to insert it. This function increases the length of the 
plug-in names list by one because it creates and adds an item to the array 
which is empty except for the text string specified. The function cannot 
leave because nothing is allocated ownership of aTextForNone is passed to 
the CParameters object. 

@param aTextForNone The string whose meaning is "none", i.e. no plug-in. It 
is assumed that this descriptor has already been localised.
@param aArrayPositionOfTextForNone Whether the string should be inserted at 
the start or appended to the end of the array. */
	{
	delete iTextForNone;
	iTextForNone=aTextForNone;
	iArrayPositionOfTextForNone=aArrayPositionOfTextForNone;
	}

CBaNamedPlugins::CParameters::CParameters(RFs& aFileServerSession)
	:iFileServerSession(aFileServerSession),
	 iArrayOfResourceFiles(NULL),
	 iFallBackName(NULL),
	 iCompareNames(NULL),
	 iTextForNone(NULL),
	 iArrayPositionOfTextForNone(STATIC_CAST(TArrayPosition, -1))
	{
	}

void CBaNamedPlugins::CParameters::ConstructL(const TArray<TResourceFile>& aArrayOfResourceFiles)
	{
	iArrayOfResourceFiles=new(ELeave) TArray<TResourceFile>(aArrayOfResourceFiles);
	}

