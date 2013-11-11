// Copyright (c) 2008-2009 Nokia Corporation and/or its subsidiary(-ies).
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
// This file contains the implementations of the classes which parse the implementation data from resource
// file version1, version2 and version3.
// 
//

#include "ParseImplementationData.h"
#include <ecom/registryinfoconst.hrh>
#include "EComDebug.h"
#include "ecompanics.h"
	
//
// CParseImplementationData class
/**
Constructor for CParseImplementationData
*/
CParseImplementationData::CParseImplementationData()
	{
	// Do nothing here
	}
/**
Destructor for CParseImplementationData
*/	
CParseImplementationData::~CParseImplementationData()
	{
	// Do nothing here
	}


/**
Standardized safe construction which leaves nothing on the cleanup stack.
@param 			aResourceFormatVersion The resource file format version no.
@return			The newly created instance.
*/	
CParseImplementationData* CParseImplementationData::NewL(TInt aResourceFormatVersion)
	{
	CParseImplementationData* object = NULL;
	if (aResourceFormatVersion==1)
		{
		object = CParseImplementationDataFormatVersion1::NewL();
		}
	else if (aResourceFormatVersion==2)
		{
		object = CParseImplementationDataFormatVersion2::NewL();
		}
	else if (aResourceFormatVersion==3)
		{
		object = CParseImplementationDataFormatVersion3::NewL();
		}
	else
		{
		User::Leave(KErrNotSupported);
		}
	return object;	
	}

/**
Standardized safe construction which leaves the returned object on the cleanup stack.
@param 			aResourceFormatVersion The resource file format version no.
@return			The newly created instance.
*/	
CParseImplementationData* CParseImplementationData::NewLC(TInt aResourceFormatVersion)
	{
	CParseImplementationData* object = CParseImplementationData::NewL(aResourceFormatVersion);
	CleanupStack::PushL(object);
	return object;	
	}

/**
Read out the data from the resource file. The created buffer is left in the cleanup stack.
This is a helper function to read out the display name.
@param 			aReader The resource file reader.  
@param			aData The pointer points to the buffer for data to be filled in.
@return			None
*/	
void CParseImplementationData::GetDataLC(RResourceReader& aReader,
										  	 HBufC*& aData)
	{
	// Read in data from resource file to the buffer.
	aData = aReader.ReadHBufCL();
	CleanupStack::PushL(aData);
	if (aData!=NULL)
		{
		if (aData->Length() > KRscStringMaxLength)
			{
			User::Leave(KErrNotSupported);
			}
		}
	}

/**
Read out the data from the resource file. The created buffer left in the cleanup stack.
This is a helper function to read out the default data and opaque data for resource v1 and v2.
@param 			aReader The resource file reader.  
@param			aData The pointer points to the buffer for data to be filled in.
@return			None
*/	
void CParseImplementationData::GetDataLC(RResourceReader& aReader,
										  	 HBufC8*& aData)
	{
	// Read in data from resource file to the buffer.
	aData = aReader.ReadHBufC8L();
	CleanupStack::PushL(aData);
	if (aData!=NULL)
		{
		if (aData->Length() > KRscStringMaxLength)
			{
			User::Leave(KErrNotSupported);
			}
		}
	}

//
// CParseImplementationDataFormatVersion1 class
/**
Constructor for CParseImplementationDataFormatVersion1
*/
CParseImplementationDataFormatVersion1::CParseImplementationDataFormatVersion1()
	{
	// Do nothing here
	}

/**
Standardized safe construction which leaves nothing on the cleanup stack.
@return			The newly created instance.
*/	
CParseImplementationDataFormatVersion1* CParseImplementationDataFormatVersion1::NewL()
	{
	return new(ELeave) CParseImplementationDataFormatVersion1();
	}

/**
Parsing the implementation information structure for resource file with version 1.
@param			aReader The resource file reader.
@param			aInfoFormat The integer identifying Implementation Information record type to be filled. 
@param			aImplementationUid The implementation Uid to be filled.
@param			aVersionNo	The version no to be filled.
@param			aName The pointer points to the buffer for display name to be filled in.
@param			aDefaultData The pointer points to the buffer for default data to be filled in.
@param			aOpaqueData The pointer points to the buffer for opaque data to be filled in.
@param			aExtendedInterfaces The pointer points to the array for extended interfaces to be filled in. 
@param			aRomOnly The rom only flag to be filled in.
*/
void CParseImplementationDataFormatVersion1::ParseL(RResourceReader& aReader,
										  			TInt& /*aInfoFormat*/,
										  			TUid& aImplementationUid,
										  			TInt& aVersionNo,
									   	  			HBufC*& aName,
									      			HBufC8*& aDefaultData,
									      			HBufC8*& aOpaqueData,
									      			RExtendedInterfacesArray*& /*aExtendedInterfaces*/,
									      			TBool& aRomOnly)
	{
	//Read in implementation_uid	
	aImplementationUid.iUid = aReader.ReadInt32L();
	//Read in version_no
	aVersionNo = aReader.ReadInt8L();	
	// Read in display name from resource file 
	GetDataLC(aReader,aName);
	// Read in default_data from resource file 
	GetDataLC(aReader,aDefaultData);
	// Read in opaque_data from resource file 
	GetDataLC(aReader,aOpaqueData);
	aRomOnly = EFalse;
	CleanupStack::Pop(3); //Pop out opaqueData, defaultData and display name
	}


//
// CParseImplementationDataFormatVersion2 class
/**
Constructor for CParseImplementationDataFormatVersion2
*/
CParseImplementationDataFormatVersion2::CParseImplementationDataFormatVersion2()
	{
	// Do nothing here
	}

/**
Standardized safe construction which leaves nothing on the cleanup stack.
@return			The newly created instance.
*/
CParseImplementationDataFormatVersion2* CParseImplementationDataFormatVersion2::NewL()
	{
	return new(ELeave) CParseImplementationDataFormatVersion2();
	}

/**
Parsing the implementation information structure for resource file with version 2.
@param			aReader The resource file reader.
@param			aInfoFormat The integer identifying Implementation Information record type to be filled. 
@param			aImplementationUid The implementation Uid to be filled.
@param			aVersionNo	The version no to be filled.
@param			aName The pointer points to the buffer for display name to be filled in.
@param			aDefaultData The pointer points to the buffer for default data to be filled in.
@param			aOpaqueData The pointer points to the buffer for opaque data to be filled in.
@param			aExtendedInterfaces The pointer points to the array for extended interfaces to be filled in. 
@param			aRomOnly The rom only flag to be filled in.
*/		
void CParseImplementationDataFormatVersion2::ParseL(RResourceReader& aReader,
										  			TInt& /*aInfoFormat*/,
										  			TUid& aImplementationUid,
										  			TInt& aVersionNo,
									   	  			HBufC*& aName,
									      			HBufC8*& aDefaultData,
									      			HBufC8*& aOpaqueData,
									      			RExtendedInterfacesArray*& /*aExtendedInterfaces*/,
									      			TBool& aRomOnly)
	{
	//Read in implementation_uid	
	aImplementationUid.iUid = aReader.ReadInt32L();
	//Read in version_no
	aVersionNo = aReader.ReadInt8L();	
	// Read in display name from resource file 
	GetDataLC(aReader,aName);
	// Read in default_data from resource file 
	GetDataLC(aReader,aDefaultData);
	// Read in opaque_data from resource file 
	GetDataLC(aReader,aOpaqueData);
	//Read in RomOnly flag
	aRomOnly = aReader.ReadInt8L();
	CleanupStack::Pop(3); //Pop out opaqueData, defaultData and display name
	}

//
// CParseImplementationDataFormatVersion3 class
/**
Constructor for CParseImplementationDataFormatVersion3
*/
CParseImplementationDataFormatVersion3::CParseImplementationDataFormatVersion3()
	{
	// Do nothing here
	}

/**
Standardized safe construction which leaves nothing on the cleanup stack.
@return			The newly created instance.
*/
CParseImplementationDataFormatVersion3* CParseImplementationDataFormatVersion3::NewL()
	{
	return new(ELeave) CParseImplementationDataFormatVersion3();
	}

/**
Determine the order of two extended interface.
@param aUid1 The first extended interface uid for compare.
@param aUid2 The second extended interface uid for compare.
@return integer indicating the order of the two.
*/
TInt CParseImplementationDataFormatVersion3::CompareUid(const TUid& aUid1, const TUid& aUid2)
	{
	return CompareTUidValues(aUid1.iUid,aUid2.iUid);
	}

/**
Parsing the implementation information structure for resource file with version 3.
@param			aReader The resource file reader.
@param			aInfoFormat The integer identifying Implementation Information record type to be filled. 
@param			aImplementationUid The implementation Uid to be filled.
@param			aVersionNo	The version no to be filled.
@param			aName The pointer points to the buffer for display name to be filled in.
@param			aDefaultData The pointer points to the buffer for default data to be filled in.
@param			aOpaqueData The pointer points to the buffer for opaque data to be filled in.
@param			aExtendedInterfaces The pointer points to the array for extended interfaces to be filled in. 
@param			aRomOnly The rom only flag to be filled in.
*/	
void CParseImplementationDataFormatVersion3::ParseL(RResourceReader& aReader,
										  			TInt& aInfoFormat,
										  			TUid& aImplementationUid,
										  			TInt& aVersionNo,
									   	  			HBufC*& aName,
									      			HBufC8*& aDefaultData,
									      			HBufC8*& aOpaqueData,
									      			RExtendedInterfacesArray*& aExtendedInterfaces,
									      			TBool& aRomOnly)
	{
	//Read in info_format 
	aInfoFormat = aReader.ReadInt8L();	
	//Read in implementation_uid	
	aImplementationUid.iUid = aReader.ReadInt32L();
	//Read in version_no
	aVersionNo = aReader.ReadInt8L();	
	// Read in display name from resource file 
	GetDataLC(aReader,aName);
	// Read in default data and opaque data
	if (aInfoFormat == IMPLEMENTATION_INFO_RECORD_TYPE1)
		{
		GetDataType1LC(aReader,aDefaultData);
		GetDataType1LC(aReader,aOpaqueData);
		}
	else if (aInfoFormat == IMPLEMENTATION_INFO_RECORD_TYPE2)
		{
		GetDataType2LC(aReader,aDefaultData);
		GetDataType2LC(aReader,aOpaqueData);
		}
	else 
		{
		User::Leave(KErrNotSupported);
		}	
	// Read in extended interfaces
	GetExtendedInterfacesLC(aReader,aExtendedInterfaces);
	//Read in RomOnly flag
	const TUint8 flags = aReader.ReadInt8L();	
	aRomOnly = flags & IMPLEMENTATION_INFO_FLAG_ROM_ONLY;// extract ROM_ONLY flag 	
	CleanupStack::Pop(4); //Pop out extended interface, opaqueData, defaultData and display name
	}

/**
Read out data from the resource file version 3 for implementation type 1. 
The created buffer is left in the cleanup stack. 
This is a helper function for read out default data and opaque data.
@param 			aReader The resource file reader. The current position of the reader is the entry 
				of the default data. 
@param			aData The pointer point to the buffer for data to fill in.
@return			None
*/	
void CParseImplementationDataFormatVersion3::GetDataType1LC(RResourceReader& aReader,HBufC8*& aData)
	{
	// Read default_data from resource file. 
	TInt numberOfStrings = aReader.ReadUint8L();
	if (numberOfStrings > KRscMaxNumberOfString)
		{
		User::Leave(KErrNotSupported);
		}
	// Each string of the data is limited to 255 characters max base on the definition.
	aData = HBufC8::NewLC(numberOfStrings*KRscStringMaxLength);
	TPtr8 dataPtr(aData->Des());
	HBufC8* arrayElementData;
	TInt size = 0;
	for(TInt i = 0; i < numberOfStrings; i++)
		{
		arrayElementData = aReader.ReadHBufC8L();
		if (arrayElementData!=NULL)
			{
			size = size + arrayElementData->Length();
			dataPtr.Append(*arrayElementData);
			delete arrayElementData;
			}
		}
	// ReAlloc the buffer to exact size.
	aData->ReAllocL(size);
	}

/**
Read out data from the resource file version 3 for implementation type 2. 
The created buffer is left in the cleanup stack. 
This is a helper function for read out default data and opaque data
@param 			aReader The resource file reader. The current position of the reader is the entry 
				of the default data. 
@param			aData The pointer point to the buffer for data to fill in.
@return			None
*/	
void CParseImplementationDataFormatVersion3::GetDataType2LC(RResourceReader& aReader,HBufC8*& aData)
	{
	// read opaque_data from resource file
	TInt inputLength = aReader.ReadUint16L();
	if (inputLength > KRscDataMaxLength)
		{
		User::Leave(KErrNotSupported);
		}
	aData = HBufC8::NewLC(inputLength);
	TPtr8 dataPtr(aData->Des());
	// Gets the pointer to the data represented by dataPtr and casts it to TAny*
	TAny* ptr = reinterpret_cast<TAny*> (const_cast<TUint8*>(dataPtr.Ptr()));
	aReader.ReadL(ptr,inputLength);
	dataPtr.SetLength(inputLength);
	}
		
/**
Read out the extended interfaces from the resource file. The extended interfaces array is left in the cleanup stack.
@param 			aReader The resource file reader. The current position of the reader is the entry 
				of the extended interfaces. 
@param			aExtendedInterfaces The pointer points to the array for extended interfaces to be filled in.
@return			None
*/
void CParseImplementationDataFormatVersion3::GetExtendedInterfacesLC(RResourceReader& aReader,
																	RExtendedInterfacesArray*& aExtendedInterfaces)
	{
	// Read in extended interface	
	aExtendedInterfaces = new(ELeave) RExtendedInterfacesArray(KExtendedInterfaceGranularity);
	
	CleanupStack::PushL(TCleanupItem(CloseAndDeleteArray, aExtendedInterfaces));
		
	TInt numExtendedInterfaces = aReader.ReadUint16L();
	if (numExtendedInterfaces > KRscMaxExtendedInterfaces)
		{
		User::Leave(KErrNotSupported);
		}
	
	for(TInt index = 0; index < numExtendedInterfaces; index++)
		{
		TUid extendedInterfaceUid = TUid::Uid(aReader.ReadInt32L());
		// Ensure no duplicate extended interface is added.
		// if there is duplicate extended interface, panic in debug mode and leave with KErrNotSupported.
		TLinearOrder<TUid> uidComparator(CompareUid);	
		TInt error = aExtendedInterfaces->InsertInOrder(extendedInterfaceUid,uidComparator);
		if (error == KErrAlreadyExists)
			{
            __ECOM_TRACE("ECOM: PANIC in CParseImplementationDataFormatVersion3::GetExtendedInterfacesLC(), duplicate extended interface");
		    __ASSERT_DEBUG(EFalse, User::Panic(KEComServerPanicCategory, EEComPanic_CParseImplementationDataFormatVersion3_DuplicateExIf));		
			}
		User::LeaveIfError(error);
		}
	}

