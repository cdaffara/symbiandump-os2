// Copyright (c) 2007-2009 Nokia Corporation and/or its subsidiary(-ies).
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

#ifndef __PARSEIMPLEMENTATIONDATA_H__
#define __PARSEIMPLEMENTATIONDATA_H__

#include <e32base.h>
#include <barsread2.h> // RResourceReader
#include <ecom/implementationinformation.h>

/**
Granularity of the extended interface array
@internalComponent
*/
const TInt KExtendedInterfaceGranularity = 1;

/**
Maximum number of interfaces info structures in resource file version 3. 
@internalComponent
*/
const TInt KMaxInterfacesInResV3 = 4;

/**
Maximum number of implementations in one interface info structures in resource file version 3. 
@internalComponent
*/
const TInt KMaxImplementationsForEachInterfacesInResV3 = 8;

/** 
Max length of default data and opaque data in resource file version 3.
@internalComponent
*/
const TInt KRscDataMaxLength = 512;

/** 
Resource string max length - see the limitations of BAFLS component, RResourceReader class.
@see RResourceReader::ReadHBufCL()
@internalComponent
*/
const TInt KRscStringMaxLength = 255;

/** 
Maximum number of resource string for default_data and opaque_data in resource file version 3. 
@internalComponent
*/
const TInt KRscMaxNumberOfString = 2;

/** 
Maximum number of extended interfaces in one implementation in resource file version 3. 
@internalComponent
*/
const TInt KRscMaxExtendedInterfaces = 8;

//
// ParseImplementationData class
/**
This class facilitates parsing implementation data from the resource file.
@internalComponent
*/
class CParseImplementationData : public CBase
	{
public:
	static CParseImplementationData* NewL(TInt aResourceFormatVersion);
	static CParseImplementationData* NewLC(TInt aResourceFormatVersion);
	virtual ~CParseImplementationData();
	/**
	Parsing the implementation information structure for resource file.
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
	virtual void ParseL(RResourceReader& aReader,
				   		TInt& aInfoFormat,
				   		TUid& aImplementationUid,
				   		TInt& aVersionNo,
				   		HBufC*& aName,
				   		HBufC8*& aDefaultData,
				   		HBufC8*& aOpaqueData,
				   		RExtendedInterfacesArray*& aExtendedInterfaces,
				   		TBool& aRomOnly) = 0;
				   
protected:
	CParseImplementationData();
	
	void GetDataLC(RResourceReader& aReader,HBufC*& aData);
	
	void GetDataLC(RResourceReader& aReader,HBufC8*& aData);
	
	};

//
// CParseImplementationDataFormatVersion1 class
/**
This class facilitates parsing implementation data from the resource file version 1.
@internalComponent
*/
class CParseImplementationDataFormatVersion1 : public CParseImplementationData
{
public:
	static CParseImplementationDataFormatVersion1* NewL();
	void ParseL(RResourceReader& aReader,
		   TInt& /*aInfoFormat*/,
		   TUid& aImplementationUid,
		   TInt& aVersionNo,
		   HBufC*& aName,
		   HBufC8*& aDefaultData,
		   HBufC8*& aOpaqueData,
		   RExtendedInterfacesArray*& /*aExtendedInterfaces*/,
		   TBool& aRomOnly);
private:
	CParseImplementationDataFormatVersion1();
};

//
// CParseImplementationDataFormatVersion2 class
/**
This class facilitates parsing implementation data from the resource file version 2.
@internalComponent
*/	
class CParseImplementationDataFormatVersion2 : public CParseImplementationData
{
public:
	static CParseImplementationDataFormatVersion2* NewL();
	void ParseL(RResourceReader& aReader,
		   TInt& /*aInfoFormat*/,
		   TUid& aImplementationUid,
		   TInt& aVersionNo,
		   HBufC*& aName,
		   HBufC8*& aDefaultData,
		   HBufC8*& aOpaqueData,
		   RExtendedInterfacesArray*& /*aExtendedInterfaces*/,
		   TBool& aRomOnly);
private:
	CParseImplementationDataFormatVersion2();
	
};	

//
// CParseImplementationDataFormatVersion3 class
/**
This class facilitates parsing implementation data from the resource file version 3.
@internalComponent
*/
class CParseImplementationDataFormatVersion3 : public CParseImplementationData
{
public:
	static CParseImplementationDataFormatVersion3* NewL();
	void ParseL(RResourceReader& aReader,
		   TInt& aInfoFormat,
		   TUid& aImplementationUid,
		   TInt& aVersionNo,
		   HBufC*& aName,
		   HBufC8*& aDefaultData,
		   HBufC8*& aOpaqueData,
		   RExtendedInterfacesArray*& aExtendedInterfaces,
		   TBool& aRomOnly);
private:
	CParseImplementationDataFormatVersion3();
	
	static TInt CompareUid(const TUid& aUid1, const TUid& aUid2);
	
	void GetDataType1LC(RResourceReader& aReader,HBufC8*& aData);
	
	void GetDataType2LC(RResourceReader& aReader,HBufC8*& aData);
	
	void GetExtendedInterfacesLC(RResourceReader& aReader, RExtendedInterfacesArray*& aExtendedInterfaces);
};

#endif //__PARSEIMPLEMENTATIONDATA_H__
