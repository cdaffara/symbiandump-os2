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
// Implementation of CMatchData class
// 
//

/**
 @file 
 @publishedAll
 @released
*/

#include <xml/matchdata.h>
#include <xml/xmlframeworkerrors.h>

using namespace Xml;

/**
The maximum length of variant field

@internalComponent
*/
const TInt KXmlMaxVariantLength = 32;

/**
The maximum length of mime type field

@internalComponent
*/
const TInt KXmlMaxMimeTypeLength = 255;

/**
Fields separator for data and opaque data in resource information file

@internalComponent
*/
_LIT8(KXmlDataSeparator, "||");

/**
Set flag macro to simplify bitmap management

@internalComponent
*/
#define SYMBIAN_XML_FLAG_SET(bitmap, flag, setting) \
			bitmap = setting?(bitmap|flag):(bitmap&(~flag))
			
/**
Rom only flag
To request for rom based parsers only
Default value is FALSE

@internalComponent
*/ 
const TInt32 KXmlRomOnlyFlag = 0x00000001;

/**
Case Sensitivity flag
To specify how case sensitivity should be applied, while performing strings matching
Default value is TRUE

@internalComponent
*/
const TInt32 KXmlCaseSensitivityFlag = 0x00000002;

/**
Leave on many flag
To leave when the query is narrowed down to more than one parser.
If not set, the XML framework will choose a parser.
Default value is FALSE

@internalComponent
*/
const TInt32 KXmlLeaveOnManyFlag = 0x00000004;

/**
Reserved, unused flags in iAddInfo attribute

@internalComponent
*/
const TInt32 KXmlAddInfoReservedFlags = 0xFFFFFFF8;

/**  
Object constructor. Initializes internal state to default values 

@internalComponent
*/
CMatchData::CMatchData(): 
			iMimeType(NULL),
			iVariant(NULL),
			iAddInfo(KXmlCaseSensitivityFlag)
	{
	
	}

/** 
Object destructor. Deletes allocated heap based memory. 

@internalComponent
*/
CMatchData::~CMatchData()
	{
	if (iMimeType != NULL)
		{
		delete iMimeType;
		iMimeType = NULL;
		}
	
	if (iVariant != NULL)
		{
		delete iVariant;
		iVariant = NULL;
		}
	}

/**
Creates CMatchData object with its default values. 

@leave KErrNoMemory 	If there is not enough memory to create an object.

@return A pointer to the newly created CMatchData object.

*/
EXPORT_C CMatchData* CMatchData::NewL()
	{
	CMatchData* me = CMatchData::NewLC();
	CleanupStack::Pop(me); 
	return me;
	}
	
/** 
Creates CMatchData object using heap based stream of externalized object's data. 

@param aPackage Descriptor to the heap based stream of externalized object 

@leave KErrNoMemory 	If there is not enough memory to create an object. 
@leave KErrArgument 	If the argument passed to an object is incorrect.

@return A pointer to the newly created CMatchData object.

@internalComponent
*/
EXPORT_C CMatchData* CMatchData::NewL(const TDesC8& aPackage)
	{
	CMatchData* me = CMatchData::NewLC(aPackage);
	CleanupStack::Pop(me);
	return me;
	}
	

/**
Creates CMatchData object with its default values. 
Leaves an obeject pointer on a cleanup stack. 

@leave KErrNoMemory If there is not enough memory

@return A pointer to the newly created CMatchData object.

*/
EXPORT_C CMatchData* CMatchData::NewLC()
	{
	CMatchData* me = new (ELeave) CMatchData();
	CleanupStack::PushL(me);
       return me;	
	}
	

/**
Creates CMatchData object using heap based stream of externalized object's data. 
Leaves an obeject pointer on a cleanup stack. 

@param aPackage Descriptor to the heap based stream of externalized object 

@leave KErrNoMemory 	If there is not enough memory to create an object.
@leave KErrArgument 	If the argument passed to an object is incorrect.

@return A pointer to the newly created CMatchData object.

@internalComponent
*/
EXPORT_C CMatchData* CMatchData::NewLC(const TDesC8& aPackage)
	{
	CMatchData* me = new (ELeave) CMatchData();
	CleanupStack::PushL(me);
	me->ConstructL(aPackage);
	return me;
	}

/**
Standard symbian 2-phase construction method.
It reads a stream stored on a heap and internalize the object. 

@param aPackage Descriptor to the heap based stream of externalized object 

@leave KErrNoMemory 	If there is not enough memory to create an object.
@leave KErrArgument 	If the argument passed to an object is incorrect.

@return A pointer to the newly created CMatchData object.

@internalComponent
*/		
void CMatchData::ConstructL(const TDesC8& aPackage)
	{
	RDesReadStream stream(aPackage);
	InternalizeL(stream);
	stream.Close();
	}

/** 
Externalizes the object into RWriteStrem.

@param aStream Stream to write to

@internalComponent
*/		
void CMatchData::ExternalizeL(RWriteStream& aStream) const
	{
	aStream.WriteInt16L(iMimeType? iMimeType->Length():0);
	if(iMimeType && iMimeType->Length() > 0)
		{
		aStream.WriteL(*iMimeType, iMimeType->Length());
		}
	
	aStream.WriteInt16L(iVariant?iVariant->Length():0);
	if (iVariant && iVariant->Length() > 0 )
		{
		aStream.WriteL(*iVariant, iVariant->Length());
		}
	
	aStream.WriteUint32L(iAddInfo);
	}
	
/** 
Internalizes the object out of RReadStream.

@param aStream Stream to read from 

@leave KErrNoMemory 	If there is not enough memory for new string allocation.
@leave KErrArgument 	If passed descriptor doesn't comply with length constraints.  

@internalComponent
*/	
void CMatchData::InternalizeL(RReadStream& aStream)
	{
	TInt16 temp;
	
	// read mime type and create the heap buffer for it. 
	// verify constraints
	temp = aStream.ReadInt16L();
	if (temp <=0 || temp > KXmlMaxMimeTypeLength)
		{
		// must not be greater then max
		User::LeaveIfError(KErrArgument);
		}
	
	// delete current value as it is no longer valid
	if (iMimeType != NULL)
		{
		delete iMimeType;
		iMimeType = NULL;
		}
	
	// create buffer and read new data if available
	if (temp > 0)
		{
		iMimeType = HBufC8::NewL(temp);
		TPtr8 ptr = iMimeType->Des();
		aStream.ReadL(ptr, temp);
		}
	
	// read variant and create the heap buffer for it. 
	// verify constarints
	temp = aStream.ReadInt16L();
	if (temp <0 || temp > KXmlMaxVariantLength)
		{
		// must not be greater then max, zero possible. 
		User::LeaveIfError(KErrArgument);
		}
	 	
	// delete current value as it is no longer valid
	if (iVariant != NULL)
		{
		delete iVariant;
		iVariant = NULL;
		}
	
	// create buffer and read new data if available
	if (temp > 0)
		{
		iVariant = HBufC8::NewL(temp);
		TPtr8 ptr = iVariant->Des();
		aStream.ReadL(ptr, temp);	
		}
	
	// read additional info 
	iAddInfo = aStream.ReadUint32L();
	// check for correctness of the iAddInfo bitmap
	if (iAddInfo & KXmlAddInfoReservedFlags > 0 )
		{
		// Leave as unexpected flags are set. 
		User::LeaveIfError(KErrArgument);
		}	
}

/**
Sets up a mime type attribute to a string given as an aData descriptor. 

@param aData String descriptor

@leave KErrNoMemory 	If there is not enough memory for new string allocation.
@leave KErrArgument 	If passed descriptor doesn't comply with length constraints. 

*/		
EXPORT_C void CMatchData::SetMimeTypeL(const TDesC8& aData)
	{
	// verify constraints
	if (aData.Length() == 0 || aData.Length() > KXmlMaxMimeTypeLength)
		{
		// must not be greater than max
		User::Leave(KErrArgument);
		}

	// delete current value as it is no longer valid
	if (iMimeType != NULL)
		{
		delete iMimeType;
		iMimeType = NULL;
		}
		
	// allocate required buffer and copy new data
	iMimeType = aData.AllocL();
	
	}

/**
Sets up a variant attribute to a string given as an aVariant descriptor. 
If this is not set OR is set to a string of length 0 the Variant ID is
not used during parser resolution and hence the variant IDs of plug-in 
parsers are ignored.

@param aData String descriptor

@leave KErrNoMemory 	If there is not enough memory for new string allocation.
@leave KErrArgument 	If passed descriptor doesn't comply with length constraints. 

*/			
EXPORT_C void CMatchData::SetVariantL(const TDesC8& aVariant)
	{

	// verify constraints
	if (aVariant.Length() > KXmlMaxVariantLength)
		{
		// must not be greater than max
		User::Leave(KErrArgument);
		}

	// delete current value as it is no longer valid
	if (iVariant != NULL)
		{
		delete iVariant;
		iVariant = NULL;
		}
	
	// allocate required buffer and copy new data	
	iVariant = aVariant.AllocL();
	}

/**
Returns a pointer to the mime type string.  

@return A pointer to the mime type string. 
		Pointer to KNullDesC8 is returned if not set up yet.

*/		
EXPORT_C const TPtrC8 CMatchData::MimeType() const
	{
	TPtrC8 result;

	if(iMimeType)
		result.Set(*iMimeType);
	else
		result.Set(KNullDesC8);

	return result;
	}
	
/**
Returns a pointer to the variant string.  

@return A pointer to the variant string. 
		Pointer to KNullDesC8 is returned if not set up yet.

*/	
EXPORT_C const TPtrC8 CMatchData::Variant() const
	{
	TPtrC8 result;

	if(iVariant)
		result.Set(*iVariant);
	else
		result.Set(KNullDesC8);

	return result;
	}

/**
Returns a heap based buffer descriptor of an externalized CMatchData object stream. 
The newly created object's ownership is shifted to the caller. 

@leave KErrNoMemory If there is not enough memory to create an heap based buffer. 

@return Heap based buffer descriptor of an externalized object stream.

@internalComponent
*/		
HBufC8* CMatchData::PackToBufferL() const
	{
	//Create new Heap Descriptor with the size of externalized CMatchData
	HBufC8* data = HBufC8::NewLC(	(iMimeType?iMimeType->Size():0)
								+ (iVariant?iVariant->Size():0) 
								+ 2*sizeof(TInt16) 		// length of mime type and variant strings
								+ sizeof(TUint32));		// size of addinfo variable
	
	// create the stream using allocated buffer
	TPtr8   dataDes = data->Des();
	RDesWriteStream stream(dataDes);

	// pack object into the allocated buffer
	ExternalizeL(stream);
	
	//cleanup
	stream.Close();
	CleanupStack::Pop(data);
	return data;
	}


/**
Sets the LeaveOnManyFlag flag. 
If set, it notifies customized resolver it should leave when the query is resolved to more than one parser. 
By default this flag is not set, so the framework chooses a parser in this case. 

@param aSetting The setting value. 

*/		
EXPORT_C void CMatchData::SetLeaveOnMany(TBool aSetting) 
	{
	SYMBIAN_XML_FLAG_SET(iAddInfo, KXmlLeaveOnManyFlag, aSetting);
	}
	
/**
Sets the Rom Only flag. 
If set, it notifies customized resolver the request is for ROM-based parsers only. 
By default this flag is not set, so the framework searches for rom and non-rom based parsers. 

@param aSetting The setting value. 

*/
EXPORT_C void CMatchData::SetRomOnly(TBool aSetting) 
	{
	SYMBIAN_XML_FLAG_SET(iAddInfo, KXmlRomOnlyFlag, aSetting);
	}

/**
Sets the Case Sensitivity flag. 
Customized resolver uses this setting to turn on or off case sensitivity for strings matching.

@param aSetting The setting value. 

*/
EXPORT_C void CMatchData::SetCaseSensitivity(TBool aSetting)
	{
	SYMBIAN_XML_FLAG_SET(iAddInfo, KXmlCaseSensitivityFlag, aSetting);
	}

/**
Static function to sort an array of CImplementationInformation objects, 
in ascending order of their Uid values.
The function is used in CMatchData::ResolveL().

@param aImpInfo1	pointer of first CImplementationInformation object
@param aImpInfo2	pointer of second CImplementationInformation object

@return			zero, if the two objects have equal Uids
@return			a negative value, if the first object's Uid is less than the second object's Uid.
@return			a positive value, if the first object's Uid is greater than the second object's Uid.

@internalComponent

*/	
TInt CMatchData::SortOrder(const CImplementationInformation &aImpInfo1, 
							const CImplementationInformation &aImpInfo2)
	{
    if (aImpInfo1.ImplementationUid().iUid > aImpInfo2.ImplementationUid().iUid)
		{
		return 1;
		}
	if (aImpInfo1.ImplementationUid().iUid < aImpInfo2.ImplementationUid().iUid)
		{
		return -1;
		}
	return 0;		
	}

/**
Performs a parser resolution. 
Following criteria are considered:
 - Case sensitivity for string matching is applied according to the Case Sensitivity flag.
 - Only ROM-based parsers are considered when ROM only flag is set. 
 - Mime type is mandatory as it must match the data field in resource information file.
 - Variant is optional. If present it must match first entry in the opaque data field. 
 - If the query is narrowed down to more than one parser the behaviour is determined in 
    SelectSingleParserL function. 

@param 	aImplList The list of available parsers to choose from. 

@leave 	KErrNoMemory
@leave 	KErrXmlMoreThanOneParserMatched

@return 	Parser's Uid or KNullUid if parser isn't found. 

@see SelectSingleParserL

@internalComponent

*/
EXPORT_C TUid CMatchData::ResolveL(RImplInfoArray& aImplList) const
	{
	TUid matchUid;
	
	// Create an array to hold multiple matching parser uids
	// RImplInfoPtrArray is used here for the usage of TLinearOrder<> pattern
	RImplInfoPtrArray parserArray;
	
	CleanupClosePushL(parserArray);
	
	// go through the list of implementations for the 
	for (TInt i = 0 ; i < aImplList.Count(); i++)
		{
		::CImplementationInformation* impData = aImplList[i];
		
		if (RomOnly() && !(impData->RomBased()))
			{
			// Request for rom based parser only. 
			// This one isn't rom based, so continue with the next one
			continue;
			}
			
		if ((MimeTypeMatch(impData->DataType()) == TRUE) 
			&& (VariantMatch(impData->OpaqueData()) == TRUE) )
			{
			// store the matching implementation occurance 
			// if there is only one occurance, this Uid will be returned
			// otherwise, the list will contain items sorted by Uid
			// (see CMatchData::SortOrder())
			parserArray.InsertInOrderL(impData, 
							TLinearOrder<CImplementationInformation>(CMatchData::SortOrder));			
			}
		}
	
	// check if multiple match found
	if (parserArray.Count()==1)
		{
		// list filtered down to one parser.
		matchUid = parserArray[0]->ImplementationUid();
		}
	else if (parserArray.Count()>1)
		{
		// multiple matches found. we need to make a decision 
		matchUid = SelectSingleParserL(parserArray);
		}
	else
		{
		// no match
		matchUid = KNullUid;
		}
		
	CleanupStack::PopAndDestroy(&parserArray);
	return matchUid;
	}
	
/**
Helps parser resolution when multiple matches are found
Following criteria are considered on the list filtered by ResolveL:
- The code might either leave with an error if LeaveOnMany flag is set, or 
  select a parser from the multiple-item list. The default behaviour is to select
  the parser with lowest uid. To preserve backwards compatibility, in the case of 
  no variant a Symbian supplied parser will be returned if present. If there are 
  multiple matching Symbian parsers, the one with the lowest Uid will be returned.

@param 	aImplList The filtered list of parsers to choose from. 

@leave 	KErrNoMemory
@leave 	KErrXmlMoreThanOneParserMatched

@return 	Parser's Uid

@internalComponent
*/
TUid CMatchData::SelectSingleParserL(RImplInfoPtrArray& aImplList) const	
	{
	TInt arrayLen = aImplList.Count();
	
	// check if multiple match found
	if ( LeaveOnMany() )
		{
		// At this point more than one parser was found matching resolution criteria,
		// the user is requesting to leave in such a case
		User::Leave(KErrXmlMoreThanOneParserMatched);					
		}
	
	if (IsInvalidVariant()) 
		{
		// multiple matches in list and variant is invalid means we should look for Symbian plugin
		// we go through the (ascending) sorted list of implementations
		for (TInt j = 0 ; j <= arrayLen-1; j++)
			{
			::CImplementationInformation* impData = aImplList[j];
		
			// we check every matching parser variant for Symbian string
			if ((CaseSensitivity() && (impData->OpaqueData().Compare(KXmlSymbianPluginVariant) == 0))
				||(!CaseSensitivity() && (impData->OpaqueData().CompareF(KXmlSymbianPluginVariant) == 0))  )
				{
				// this match is the lowest Uid Symbian parser, return it
				return impData->ImplementationUid();
				}
			}
		// if this for loop finishes without returning, this means there are multiple
		// matching parsers none of which are Symbian. We defer their processing to the
		// common 'return lowest uid' part of the algorithm
		}
		
	// return the lowest uid, which should be the first item in the list
	return aImplList[0]->ImplementationUid();
	}
	
/**
Performs string matching for all the entries in an aField string separated by aSeparator. 

@param aField		string with several entries separated by aSeparator. 
@param aMatchString string to match against entries in aField
@param aSeparator   string which separates entries in aField

@return Match result

@internalComponent
*/
TBool CMatchData::MatchField(const TDesC8& aField, const TDesC8& aMatchString, const TDesC8& aSeparator) const
	{
	// verify input data
	if (aField.Length() == 0 || aMatchString.Length() == 0)
		{
		return EFalse;
		}
	
	// Check if aField and aMatchString are not the same.
	// For most cases aField is a one-entry string, so for better performance it is worth to check it up front. 
	if (	(CaseSensitivity() && (aField.Compare(aMatchString) == 0))
		|| (!CaseSensitivity() && (aField.CompareF(aMatchString) == 0) )  )
		{
		return ETrue;
		}
	
	// Find the first separator position
	TInt separatorPos = aField.Find(aSeparator);
	if (separatorPos == KErrNotFound)
		{
		// No separators in the string. 
		// Only one entry in the aField string, which was already verified as non-matching. 
		// No matches found then. 
		return EFalse;
		}
		
	TInt separatorLength =  aSeparator.Length();
	
	// call again the method with the "first field" of the string
	if (MatchField(aField.Left(separatorPos), aMatchString, aSeparator) == TRUE)
		{
		return ETrue;
		}
	
	// call the method with the remaining sections of the string
	if (MatchField(aField.Mid(separatorPos + separatorLength), aMatchString, aSeparator) == TRUE)
		{
		return ETrue;
		}
	
	//All recursive calls returned false, so the match wasn't found
	return EFalse;
	}
	
/**
Checks for mime type entry in aDataField string.

@param aDataFiled string with several mime type entries separated by "||" string. 

@return Match result

@internalComponent
*/	
TBool CMatchData::MimeTypeMatch(const TDesC8& aDataField) const
	{
	return MatchField(aDataField, *iMimeType, KXmlDataSeparator);
	}

/**
Checks for variant string entry in first field of aOpaqueField string.

@param aOpaqueField string with several entries separated by "||" string. 

@return Match result

@internalComponent
*/	
TBool CMatchData::VariantMatch(const TDesC8& aOpaqueField) const
	{
	TPtrC8 matchString;
	
	if (IsInvalidVariant())
		{
		// optional variant is not set
		// return true in all cases
		return ETrue;
		}
		
	TInt separatorPos = aOpaqueField.Find(KXmlDataSeparator);
	
	// set the first entry in the opague data to match against
	if (separatorPos == KErrNotFound)
		{
		matchString.Set(aOpaqueField);
		}
	else
		{
		matchString.Set(aOpaqueField.Left(separatorPos));
		}
		
	// perform string matching
	if (	(CaseSensitivity() && (iVariant->Compare(matchString) == 0))
		||(!CaseSensitivity() && (iVariant->CompareF(matchString) == 0))  )
		{
		return ETrue;
		}
		
	return EFalse;
	}
/**
Returns Case Sensitivity flag value

@return Flag state

@internalComponent
*/
TBool CMatchData::CaseSensitivity() const
	{
	return (iAddInfo & KXmlCaseSensitivityFlag)?ETrue:EFalse;
	}

/**
Returns LeaveOnMany flag value

@return Flag state

@internalComponent
*/	
TBool CMatchData::LeaveOnMany() const
	{
	return (iAddInfo & KXmlLeaveOnManyFlag)?ETrue:EFalse;
	}
	
/**
Returns Rom-Only flag value 

@return Flag state

@internalComponent
*/
TBool CMatchData::RomOnly() const
	{
	return (iAddInfo & KXmlRomOnlyFlag)?ETrue:EFalse;
	}
	
/**
Assignement operator

@param aMatchData CMatchData object to assign from

@internalComponent
*/
CMatchData& CMatchData::operator=(const CMatchData & aMatchData)
	{

	/*If pointing to same. Then return itself*/
	 if ( &aMatchData ==  this )
	 {
		 return *this;
	 }


	 /*
	  * SYMBIAN DEF132492 FIX : Added TRAP statements to handle	the case wherein the functions
	  * called here leave.
	  */
     TRAPD( err, SetMimeTypeL(aMatchData.MimeType()) ) ; 
     if( err != KErrNone )
	  {     
		 return *this; 
	 }	  	   

	 /*
	  * SYMBIAN DEF132492 FIX : Added TRAP statements to handle	the case wherein the functions
	  * called here leave.
	  */	 
	TRAP( err, SetVariantL(aMatchData.Variant()));
    if( err != KErrNone )
	  {     
	     return *this; 
	 }	  	   


	iAddInfo = aMatchData.iAddInfo;
	return *this;
	}
/**
Comparison operator.

@param aMatchData CMatchData object to compare with

@internalComponent
*/
TBool CMatchData::operator==(const CMatchData & aMatchData) const
	{
	
	// verify iAddInfo first as the fastest comparision
	if (aMatchData.iAddInfo != iAddInfo)
		{
		return EFalse;
		}
	
	// verfiy mime type 
	if (iMimeType == NULL 
		|| (CaseSensitivity() && iMimeType->Compare(aMatchData.MimeType()) != 0)
		|| (!CaseSensitivity() && iMimeType->CompareF(aMatchData.MimeType()) != 0) ) 		
		{
		// iMimeType is NULL or strings doesn't match
		// verify the case where both might be NULL. 
		if ( (iMimeType != NULL) || (aMatchData.iMimeType != NULL ) )
			{
			// mime type doesn't match
			return EFalse;
			}
		}
	
	// verfiy variant
	if (iVariant == NULL 
		|| (CaseSensitivity() && iVariant->Compare(aMatchData.Variant()) != 0)
		|| (!CaseSensitivity() && iVariant->CompareF(aMatchData.Variant()) != 0) ) 		
		{
		// iVariant is NULL or strings doesn't match
		// verify the case where both might be NULL. 
		if ( (iVariant != NULL) || (aMatchData.iVariant != NULL) )
			{
			// variant doesn't match
			return EFalse;
			}
		}
	return ETrue;
	}
	
/**
Checks if iVariant is not set or empty

@return Variant string validness

@internalComponent
*/
TBool CMatchData::IsInvalidVariant() const
	{
	return (iVariant == NULL || (iVariant && iVariant->Length() == 0));	
	}

