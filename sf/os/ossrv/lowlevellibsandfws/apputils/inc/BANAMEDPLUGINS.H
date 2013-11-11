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

#if !defined(__BANAMEDPLUGINS_H__)
#define __BANAMEDPLUGINS_H__

#if !defined(__E32STD_H__)
#include <e32std.h>
#endif

#if !defined(__E32BASE_H__)
#include <e32base.h>
#endif

#if !defined(__BAMDESCA_H__)
#include <bamdesca.h>
#endif

class RFs;


class CBaNamedPlugins : public CBase, public MDesCArray
/** 
A localised list of the names of the plug-ins available on the phone for a 
particular plug-in framework.

This class should be used by applications that display lists of plug-ins. 
It is provided so that the plug-in names displayed to users:

are not filenames

are localisable, i.e. for a multi-language ROM device, plug-in names must 
be translated into the correct language and sorted according to the locale's 
collation rules

can be filtered depending on the current locale, i.e. the user will not necessarily 
see the names of all plug-ins for a given framework for every language of 
a multi-language ROM.

Note that the class MDesC16Array is shown in the derivation tree. The class 
definition for CBaNamedPlugins, however, uses the typedef MDesCArray. In 6.1 
builds, the symbol MDesCArray always resolves to MDesC16Array.

@see MDesCArray
@publishedAll
@released
*/
	{
public:
/** The prototype for a function that compares two plug-in names, aName1 and aName2 
for sorting.

The plug-in names list is sorted using this algorithm after it has been populated. 
Implementing this function is optional. If implemented, it is passed as a 
parameter to CParameters::SetCompareNames(). If not implemented, a default 
algorithm is used.

The function should return a positive value if aName1 is to occur after aName2 
or negative if aName1 is to occur before aName2. Zero should be returned if 
both descriptors are equivalent. */
	typedef TInt (*TCompareNames)(const TDesC& aName1, const TDesC& aName2);

/** The prototype for a function that compares two plug-in identifiers, aIdentifier1 
and aIdentifier2 to find out if they are the same.

Implementing this function is optional. If implemented, it is passed to CBaNamedPlugins::IndexOfIdentifier(), 
which uses the function to compare a specified plug-in identifier with each 
identifier in turn in the list of named plug-ins. The function should return 
ETrue if they are the same, EFalse if not. TDesC::CompareC() could be used to 
do the comparison.

@see TDesC16::CompareC()
@see TDesC8::CompareC() */
	typedef TBool (*TEquivalentIdentifiers)(const TDesC& aIdentifier1, const TDesC& aIdentifier2);

/** The position in the list of plug-in names for the text string which represents 
the choice of no plug-in.

Passed as an argument to SetTextForNoneL() and SetTextForNone(). */
	enum TArrayPosition
		{
	/** The string is inserted at the start of the list (array position zero). */
		EArrayPositionFirst,

	/** The string is appended to the list. */
		EArrayPositionLast
		};

	class TResourceFile
/** Information about a resource file containing the names of one or more named 
plug-ins. 

The information is the full filename of the resource file (the language-specific 
version of this file provides the localised names, for display to users), 
the format for the contents of the resource file and a unique identifier for 
the plug-in, which is either a UID or an arbitrary textual identifier (this 
is not for display).

An array of TResourceFile objects is passed to CParameters::NewL() and NewLC(). */
		{
	public:
/** The format of the contents of a resource file.

The EFormatArrayOfUidNamePairs format enables the names of multiple plug-ins 
to be held in a single resource file. This could be used in situations where 
a fixed set of plug-ins are provided in a single package, e.g. the plug-ins 
provided in a ROM (helping to reduce ROM space). In other cases, the EFormatTbuf 
format should be used (here a single resource file provides the name of a 
single plug-in). */
		enum TFormat
			{
	
	/** The resource file contains a single TBUF (text string) resource, to hold the 
	plug-in name. */
			EFormatTbuf, // the resource is a TBUF

			
	/** The resource file contains an ARRAY resource, whose items are UID_NAME_PAIR 
	structs. These contain plug-in UID and name pairs. */
			EFormatArrayOfUidNamePairs // the resource is an ARRAY of UID_NAME_PAIR
			};
	public:
	/** The full filename of the resource file, with a language-independent extension 
	(i.e. .rsc rather than, for instance, .r12). The language-specific version 
	of this file (with the correct language extension) contains the localised 
	name of the plug-in. The BaflUtils class is used internally to create the 
	correct language extension for iFullFileName.
	
	@see BaflUtils::NearestLanguageFile() */
		HBufC* iFullFileName;
	/** Optional unique identifier for the plug-in , for instance the filename of the 
	plug in's DLL. If not applicable, it may be NULL. */
		HBufC* iIdentifier; // this may be NULL
	/** Optional plug-in UID. If not applicable, it may have a value of KNullUid. */
		TUid iUid; // this may be KNullUid
	/** The format of the resource file's contents. */
		TFormat iFormat;
		};

	class MFallBackName
/** 
Interface class with a single pure virtual function that generates a fallback 
name for plug-ins.

The FallBackNameL() function is called during construction of the CBaNamedPlugins 
object for any plug-ins for which no resource file could be found with the 
correct language extension.

Use of this interface is optional. To use it, pass an instance of a class 
which implements the interface to CParameters::SetFallBackName(). If this 
is not done, then by default the fallback name used for plug-ins is simply 
the filename of the resource file without the drive, directory path or extension. 
@publishedAll
@released
*/
		{
	public:
	/** 
	Generates and returns a fallback name for plug-ins for which no resource is 
	available. The fallback name can be generated using the filename of the plug-in's 
	resource file, which is passed as an argument.
	
	@param aFullResourceFileName The full filename of the resource file. This 
	is the same as TResourceFile::iFullFileName.
	@return The fallback name for a plug-in. 
	*/
		virtual HBufC* FallBackNameL(const TDesC& aFullResourceFileName) const=0;
	private:
		IMPORT_C virtual void MFallBackName_Reserved_1();
		IMPORT_C virtual void MFallBackName_Reserved_2();
		};

		class CParameters : public CBase
/** 
The parameters for a localised list of plug-in names.

An object of this class is passed to CBaNamedPlugins::NewL() and NewLC(). 
The parameters are as follows - minimally, the first two must be provided:

An array of TResourceFile objects. Each object contains information about 
a single plug-in, or multiple plug-ins, including the filename of the corresponding 
resource file. Versions of these resource files with the correct filename 
extensions for the required languages provide the name of one or more plug-in, 
translated appropriately.

A connected session with the file server. This is required to search the file 
sytem for the localised resource files, then to open them for reading.

An optional object that generates a fallback name for plug-ins, if no resource 
file could be found. If no such function is provided, then the fallback name 
used for plug-ins is simply the filename of the resource file without the 
drive, directory path or extension.

An optional function that compares two plug-in names for sorting. The list 
is sorted after it has been fully populated, using this algorithm. If not 
specified, sorting is done by using the system-wide (locale-dependent) collation 
rules.

An optional descriptor which, if provided, adds an additional item whose meaning 
is "none" (i.e. "no plug-in") to the MDesCArray, and the array position (either 
the start or the end of the array) at which to insert it. 
@publishedAll
@released
*/
		{
	public:
	IMPORT_C static CParameters* NewL(RFs& aFileServerSession, const TArray<TResourceFile>& aArrayOfResourceFiles);
	IMPORT_C static CParameters* NewLC(RFs& aFileServerSession, const TArray<TResourceFile>& aArrayOfResourceFiles);
	IMPORT_C virtual ~CParameters();
	IMPORT_C void SetFallBackName(const MFallBackName& aFallBackName); // fall-back name is used if the resource file does not exist - by default the name of the file without drive, directories or extension is used
	IMPORT_C void SetCompareNames(TCompareNames aCompareNames);
		// SetTextForNoneL and SetTextForNone both add an extra item to the array (i.e. to the MDesCArray interface)
	IMPORT_C void SetTextForNoneL(const TDesC& aTextForNone, TArrayPosition aArrayPositionOfTextForNone);
	IMPORT_C void SetTextForNone(HBufC* aTextForNone, TArrayPosition aArrayPositionOfTextForNone); // passes ownership of aTextForNone in to the CParameters object
	private:
		CParameters(RFs& aFileServerSession);
		void ConstructL(const TArray<TResourceFile>& aArrayOfResourceFiles);
	private:
		RFs& iFileServerSession;
		TArray<TResourceFile>* iArrayOfResourceFiles; // a shallow copy
		const MFallBackName* iFallBackName;
		TCompareNames iCompareNames;
		HBufC* iTextForNone;
		TArrayPosition iArrayPositionOfTextForNone; // this is undefined if iTextForNone is NULL
	private:
		friend class CBaNamedPlugins;
		};
public:
	IMPORT_C static CBaNamedPlugins* NewL(const CParameters& aParameters);
	IMPORT_C static CBaNamedPlugins* NewLC(const CParameters& aParameters);
	IMPORT_C virtual ~CBaNamedPlugins();
	IMPORT_C TInt IndexOfUid(TUid aUid) const;
	IMPORT_C TInt IndexOfIdentifier(const TDesC& aIdentifier, TEquivalentIdentifiers aEquivalentIdentifiers) const;
	IMPORT_C TUid UidAtIndex(TInt aIndex) const;
	IMPORT_C const TDesC* IdentifierAtIndex(TInt aIndex) const;
	// from MDesCArray
	IMPORT_C virtual TInt MdcaCount() const;
	IMPORT_C virtual TPtrC MdcaPoint(TInt aIndex) const;
private:
	class TNamedPlugIn
/** This class is internal and is not intended for use. */
		{
	public:
		HBufC* iName;
		HBufC* iIdentifier; // this may be NULL
		TUid iUid;
		TCompareNames iCompareNames; // unfortunately the only decent way of passing this function pointer to CompareNamedPlugIns is by storing it in each TNamedPlugIn (an alternative would be to use Dll::Tls but that would be a lot more clumsy)
		};
private:
	CBaNamedPlugins(TInt aGranularity);
	void ConstructL(const CParameters& aParameters);
	static TInt CompareNamedPlugIns(const TNamedPlugIn& aNamedPlugIn1, const TNamedPlugIn& aNamedPlugIn2);
	static TInt DefaultAlgorithmToCompareNames(const TDesC& aName1, const TDesC& aName2);
private:
	RArray<TNamedPlugIn> iArrayOfNamedPlugIns;
	};

#endif

