// Copyright (c) 1997-2009 Nokia Corporation and/or its subsidiary(-ies).
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
// This file contains the implementation of
// the CImplementationInformation class.
// 
//

#include <ecom/ecom.h>
#include <ecom/ecomerrorcodes.h>
#include <ecom/ecomresolverparams.h>
#include <ecom/implementationinformation.h>
#include "RegistryData.h"
#include "ParseImplementationData.h"
#include "EComDebug.h"
#include "ecompanics.h"
// Constant for no extended interface exist in the implementation information.
// This is used in externalisation of the object.
const TInt KNoneExtendedInterface = -1;

/**
Intended Usage: This method determines the order of two extended interface.
*/
static TInt CompareUid(const TUid& aUid1, const TUid& aUid2)
	{
	return CompareTUidValues(aUid1.iUid,aUid2.iUid);
	}

/** 
Function for clean up RExtendedInterfaceArray when leave occurs
*/
void CloseAndDeleteArray(TAny* aPtr)
	{
	if (aPtr!=NULL)
		{
		(static_cast<RExtendedInterfacesArray*>(aPtr))->Close();
		delete aPtr;
		}
	}		
// ___________________________________________________________________________
//
/**
Intended Usage	:	Standardised two phase constructor which leaves the
					CImplementationInformation pointer upon the cleanup stack.
@leave  		KErrNoMemory
@since			7.0
@param			aClientSide a boolean indicating whether the streaming is to performed in client/server side
@param			aStream The stream to internalize this object from
@return			A pointer to a fully constructed CImplementationInformation
@post			Object is fully constructed and initialised
*/
CImplementationInformation* CImplementationInformation::NewLC(TBool aClientSide,RReadStream& aStream)
	{
	CImplementationInformation* self=new(ELeave) CImplementationInformation();
	CleanupStack::PushL(self);
	self->InternalizeL(aClientSide,aStream);
	return self;
	}


/**
Intended Usage	:	Standardised two phase construction which leaves nothing
					on the cleanup stack.  
@leave  		KErrNoMemory
@since			7.0
@param			aUid The unique Id of this implementation
@param			aVersion The version number of this implementation
@param			aName The display name of this implementation. This object takes ownership of aName.
@param			aDataType The data type supported by this implementation. This object takes ownership of aDataType.
@param			aOpaqueData Data for this implementation which is not used by the ECom framework.
				This object takes ownership of aOpaqueData.
@param			aDrive The drive that this implementation is on
@param			aRomOnly The flag recording whether this implementation may be loaded from ROM only
@param			aRomBased The flag recording whether this implementation is on ROM or is a later version of one on ROM
@return			A pointer to a fully constructed CImplementationInformation
@post			Object is fully constructed and initialised
*/
CImplementationInformation* CImplementationInformation::NewL(TUid aUid, 
															 TInt aVersion, 
															 HBufC*  aName,
															 HBufC8* aDataType,
															 HBufC8* aOpaqueData,
															 TDriveUnit aDrive,
															 TBool aRomOnly,
															 TBool aRomBased)

	{
	return new(ELeave) CImplementationInformation(aUid, 
												  aVersion, 
												  aName,
												  aDataType,
												  aOpaqueData,
												  aDrive,
												  aRomOnly,
												  aRomBased);
	}

/**
Standardised two phase construction which leaves nothing on the cleanup stack.  
@leave  		KErrNoMemory
@param			aUid The unique Id of this implementation
@param			aVersion The version number of this implementation
@param			aName The display name of this implementation. This object takes ownership of aName.
@param			aDataType The data type supported by this implementation. This object takes ownership of aDataType.
@param			aOpaqueData Data for this implementation which is not used by the ECom framework.
				This object takes ownership of aOpaqueData.
@param			aDrive The drive that this implementation is on
@param			aRomOnly The flag recording whether this implementation may be loaded from ROM only
@param			aRomBased The flag recording whether this implementation is on ROM or is a later version of one on ROM
@param			aExtendedInterfaces The pointer to the array recording the extended interfaces supported by this implementation.
				This object takes ownership of aExtendedInterfaces. NULL is available for PLUGIN without extended interfaces support.
@return			A pointer to a fully constructed CImplementationInformation
@post			Object is fully constructed and initialised
*/	
CImplementationInformation* CImplementationInformation::NewL(TUid aUid, 
															 TInt aVersion, 
															 HBufC*  aName,
															 HBufC8* aDataType,
															 HBufC8* aOpaqueData,
															 TDriveUnit aDrive,
															 TBool aRomOnly,
															 TBool aRomBased,
															 RExtendedInterfacesArray* aExtendedInterfaces)

	{
	return new(ELeave) CImplementationInformation(aUid, 
												  aVersion, 
												  aName,
												  aDataType,
												  aOpaqueData,
												  aDrive,
												  aRomOnly,
												  aRomBased,
												  aExtendedInterfaces);
	}	
	
/**
Intended Usage	: D'tor	
@since			7.0
@pre 			CImplementationInformation is fully constructed.
@post			CImplementationInformation is completely destroyed.
*/
CImplementationInformation::~CImplementationInformation()
	{
	// d'tor 
	if (iDisplayName!=NULL)
		{
		delete iDisplayName;
		}
	if (iData!=NULL)
		{
		delete iData;
		}
	if (iOpaqueData!=NULL)
		{
		delete iOpaqueData;
		}
	if (iExtendedInterfaceList!=NULL)
		{
		iExtendedInterfaceList->Close();
		delete iExtendedInterfaceList;
		}
	}

/**
Intended Usage	: Default c'tor
@since			7.0
@pre 			None
@post			CImplementationInformation is fully constructed.
*/
CImplementationInformation::CImplementationInformation() : CBase()
	{
	// Do nothing here
	}

/**
Intended Usage	:	Constructor with parameters.  This object takes ownership of
					aName, aDataType and aOpaqueData.
@param			aUid The unique Id of this implementation
@param			aVersion The version number of this implementation
@param			aName The display name of this implementation
@param			aDataType The data type supported by this implementation
@param			aOpaqueData Data for this implementation which is not used by the ECom framework
@param			aDrive The drive which this implementation is on
@param			aRomOnly The flag recording whether this implementation may be loaded from ROM only
@param			aRomBased The flag recording whether this implementation is on ROM or is a later version of one on ROM
@since			7.0
@pre 			None
@post			CImplementationInformation is fully constructed.
*/
CImplementationInformation::CImplementationInformation(TUid aUid, 
													   TInt aVersion, 
													   HBufC*  aName,
													   HBufC8* aDataType,
													   HBufC8* aOpaqueData,
													   TDriveUnit aDrive,
													   TBool aRomOnly,
													   TBool aRomBased)
: CBase(),
iImplementationUid(aUid),
iVersion(aVersion),
iDisplayName(aName),
iData(aDataType),
iOpaqueData(aOpaqueData),
iDrive(aDrive),
iRomOnly(aRomOnly),
iRomBased(aRomBased)
	{
	// Do nothing here
	}

/**
Constructor with parameters.  This object takes ownership of aName, aDataType, aOpaqueData and aExtendedInterfaces.
@param			aUid The unique Id of this implementation
@param			aVersion The version number of this implementation
@param			aName The display name of this implementation
@param			aDataType The data type supported by this implementation
@param			aOpaqueData Data for this implementation which is not used by the ECom framework
@param			aDrive The drive which this implementation is on
@param			aRomOnly The flag recording whether this implementation may be loaded from ROM only
@param			aRomBased The flag recording whether this implementation is on ROM or is a later version of one on ROM
@param			aExtendedInterfaces The array recording the extended interfaces supported by this implementation.
				NULL is available for PLUGIN without extended interfaces support.
@pre 			None
@post			CImplementationInformation is fully constructed.
*/
CImplementationInformation::CImplementationInformation(TUid aUid, 
													   TInt aVersion, 
													   HBufC*  aName,
													   HBufC8* aDataType,
													   HBufC8* aOpaqueData,
													   TDriveUnit aDrive,
													   TBool aRomOnly,
													   TBool aRomBased,
													   RExtendedInterfacesArray* aExtendedInterfaces)
: CBase(),
iImplementationUid(aUid),
iVersion(aVersion),
iDisplayName(aName),
iData(aDataType),
iOpaqueData(aOpaqueData),
iDrive(aDrive),
iRomOnly(aRomOnly),
iRomBased(aRomBased),
iExtendedInterfaceList(aExtendedInterfaces)
	{
	// Do nothing here
	}
/**
Intended Usage	: Stream out the internal state to aStream.	

Error Condition	: Leave with the error code.
@leave  		KErrNoMemory.
@leave  		@see RWriteStream.
@since			7.0
@param			aClientSide a boolean indicating whether the streaming is to performed in client/server side
@param			aStream The stream to store the data in.
@pre 			CImplementationInformation is fully constructed and initialized
*/
void CImplementationInformation::ExternalizeL(TBool aClientSide,RWriteStream& aStream) const
	{
	aStream.WriteInt32L(iImplementationUid.iUid);
	aStream.WriteInt32L(iVersion);
	if(iDisplayName)
		{
		TPtr outputBuf = iDisplayName->Des();
		aStream.WriteInt32L(outputBuf.Length());
		aStream.WriteL(outputBuf);
		}
	else
		{
		aStream.WriteInt32L(0);
		}

	if(iData)
		{
		TPtr8 outputBuf = iData->Des();
		aStream.WriteInt32L(outputBuf.Length());
		aStream.WriteL(outputBuf);
		}
	else
		{
		aStream.WriteInt32L(0);
		}

	if(iOpaqueData)
		{
		TPtr8 outputBuf = iOpaqueData->Des();
		aStream.WriteInt32L(outputBuf.Length());
		aStream.WriteL(outputBuf);
		}
	else
		{
		aStream.WriteInt32L(0);
		}
	if (aClientSide)
		{
		aStream.WriteInt32L(iDrive);		
		aStream.WriteInt32L(iVid.iId);		
		}
	TInt additionalImplInfo=iRomOnly;
	additionalImplInfo|=iRomBased<<1;
	additionalImplInfo|=iDisabled<<2;
	aStream.WriteInt8L(additionalImplInfo);
	
	if (iExtendedInterfaceList != NULL)
		{
		TInt count = iExtendedInterfaceList->Count();
		aStream.WriteInt32L(count);
		for(TInt i = 0; i < count; ++i)
			{
	   		aStream.WriteInt32L((*iExtendedInterfaceList)[i].iUid);
			}
		}
	else
		{
		aStream.WriteInt32L(KNoneExtendedInterface);
		}
	}
	


/**
Intended Usage	: Restore the internal state from aStream.	

Error Condition	: Leave with the error code.
@leave  		KErrNoMemory.
@leave  		@see RReadStream.
@since			7.0
@param			aClientSide a boolean indicating whether the streaming is to performed in client/server side
@param			aStream The stream to read the data from.
@pre 			CImplementationInformation is fully constructed.
@post			CImplementationInformation is restored to the state specified by
				the contents of aStream.
*/
void CImplementationInformation::InternalizeL(TBool aClientSide,RReadStream& aStream)
	{
	delete iDisplayName;
	iDisplayName = NULL;
	delete iData;
	iData = NULL;
	delete iOpaqueData;
	iOpaqueData = NULL;

	iImplementationUid.iUid = aStream.ReadInt32L();
	iVersion = aStream.ReadInt32L();
	TInt inputLength = aStream.ReadInt32L();
	if(inputLength > 0)
  		{
  		// read in the iName string
  		iDisplayName = HBufC::NewL(inputLength);
  		TPtr inputBuf = iDisplayName->Des();
  		aStream.ReadL(inputBuf,inputLength);
  		}
  	inputLength = aStream.ReadInt32L();
	if(inputLength > 0) 
  		{
  		// read in the iData string
  		iData = HBufC8::NewL(inputLength);
  		TPtr8 inputBuf = iData->Des();
  		aStream.ReadL(inputBuf,inputLength);
  		}
	inputLength = aStream.ReadInt32L();
	if(inputLength > 0)
  		{
  		// read in the iOpaqueData string
  		iOpaqueData = HBufC8::NewL(inputLength);
 		TPtr8 inputBuf = iOpaqueData->Des();
  		aStream.ReadL(inputBuf,inputLength);
  		}
	if (aClientSide)
		{
		TUint checkDrive = aStream.ReadInt32L();
		if (checkDrive > (TUint) KMaxDrives)
			User::Leave(KErrCorrupt);
		iDrive = checkDrive;	
		iVid.iId = aStream.ReadInt32L();		
		}
	TInt8 additionalImplInfo=aStream.ReadInt8L();
	iRomOnly=additionalImplInfo&1;
	iRomBased=(additionalImplInfo&2)>>1;
	iDisabled=(additionalImplInfo&4)>>2;
	TInt count = aStream.ReadInt32L();
	if (count != KNoneExtendedInterface)
		{
		for(TInt i = 0; i < count; i++)
	   		{
	   		AddExtendedInterfaceL(TUid::Uid(aStream.ReadInt32L()));
			}
		}
	}

/**
Intended Usage	: Set whether this implementation is on ROM or is
				  a later version of one on ROM
@pre 			CImplementationInformation is fully constructed
*/
void CImplementationInformation::SetRomBased(TBool aRomBased)
	{
	iRomBased = aRomBased;
	}

void CImplementationInformation::SetDrive(TDriveUnit aDrive)
	{
	iDrive=aDrive;
	}

/**
Intended Usage	: Add extended interface to list of extended interfaces
@pre 			CImplementationInformation is fully constructed
*/
void CImplementationInformation::AddExtendedInterfaceL(const TUid& aExtendedInterface)
	{
	// Allocates extended interfaces list in case it is NULL
	if (iExtendedInterfaceList == NULL)
		{
		iExtendedInterfaceList = new(ELeave) RExtendedInterfacesArray(KExtendedInterfaceGranularity);
		}

	// Ensure no duplicate extended interface is added.
	// if there is duplicate extended interface, ignore this extended interface and panic in debug mode.
	TLinearOrder<TUid> uidComparator(CompareUid);		
	TInt error = iExtendedInterfaceList->InsertInOrder(aExtendedInterface,uidComparator);
	if (error == KErrAlreadyExists)
		{
        __ECOM_TRACE1("ECOM: PANIC in CImplementationInformation::AddExtendedInterfaceL(), duplicate extended interface %x", aExtendedInterface.iUid);
		__ASSERT_DEBUG(EFalse, User::Panic(KEComServerPanicCategory,EEComPanic_CImlpementationInfromation_DuplicateExIf));		
		}
	else
		{
		User::LeaveIfError(error);
		}
	}	
/**
Intended Usage	: Get extended interface list.
@pre 			CImplementationInformation is fully constructed
*/
RExtendedInterfacesArray* CImplementationInformation::GetExtendedInterfaceList()
	{
	return iExtendedInterfaceList;
	}


/**
Intended Usage	: Get extended interface list.
@pre 	CImplementationInformation is fully constructed
@param 	aList The array of the extended interfaces which are supported by this implementation.  
@publishedPartner
@released
*/
EXPORT_C void CImplementationInformation::GetExtendedInterfaceListL(RExtendedInterfacesArray& aList)
	{
	aList.Reset();
	if (iExtendedInterfaceList != NULL)
		{
		for (TInt i = 0;i < iExtendedInterfaceList->Count(); i++)
			{
			aList.AppendL((*iExtendedInterfaceList)[i]);		
			}
		}
	}
