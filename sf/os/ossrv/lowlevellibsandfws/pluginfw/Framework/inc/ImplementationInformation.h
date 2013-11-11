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
// This file contains the definition of the CImplementationInformation API class.
// 
//

/**
 @file
 @publishedAll
 @released
*/

#ifndef __IMPLEMENTATIONINFORMATION_H__
#define __IMPLEMENTATIONINFORMATION_H__

// ____________________________________________________________________________
// INCLUDES

#include <s32std.h>
#include <f32file.h>


// ____________________________________________________________________________
// TYPES

/**
Simplifies the definition of extended interfaces array
@publishedPartner
@released
*/
// To be used internally by ECOM and by Phone Manufacturers
typedef RArray<TUid> RExtendedInterfacesArray;



// ____________________________________________________________________________
// CLASSES

/**
The CimplementationInformation class acts as the container for the registration 
data relating to a particular interface implementation.
Note that this class is used in both the ECOM client and server side hence the 
mixed API classification seen below. For the client side only the API used to 
extract the registration data should be used.
*/
NONSHARABLE_CLASS(CImplementationInformation) : public CBase
	{
public: // internalComponent APIs
    	// Internal methods used only inside ECOM , not for client use.

	static CImplementationInformation* NewLC(TBool aClientSide,
                                             RReadStream& aStream);

	static CImplementationInformation* NewL(TUid	aUid,
											TInt	aVersion,
											HBufC*  aName,
											HBufC8* aDataType,
											HBufC8* aOpaqueData,
											TDriveUnit aDrive,
											TBool aRomOnly,
											TBool aRomBased);
	static CImplementationInformation* NewL(TUid	aUid,
											TInt	aVersion,
											HBufC*  aName,
											HBufC8* aDataType,
											HBufC8* aOpaqueData,
											TDriveUnit aDrive,
											TBool aRomOnly,
											TBool aRomBased,
											RExtendedInterfacesArray* aExtendedInterfaces);
	
	~CImplementationInformation();

public: // publishedAll APIs
        // Available for use by all clients

	inline const TDesC& DisplayName() const;

	inline const TDesC8& DataType() const;
	inline const TDesC8& OpaqueData() const;

	inline TUid ImplementationUid() const;
	inline TInt Version() const;

	inline TDriveUnit Drive() const;
	
	inline TBool RomOnly() const;
	inline TBool RomBased() const;
	
	inline TVendorId VendorId() const;
	
	inline TBool Disabled() const;
	
	// This function should not be used by any ECOM client as it will have no 
    // effect at all on the implementation information stored in the server side
	inline void SetDisabled(TBool aDisabled);

	IMPORT_C void GetExtendedInterfaceListL(RExtendedInterfacesArray& aList);

public: // internalComponent APIs
    	// Internal methods used inside ECOM, not for client use.

	void ExternalizeL(TBool aClientSide,RWriteStream& aStream) const;
	void InternalizeL(TBool aClientSide,RReadStream& aStream);
	
	void SetRomBased(TBool aRomBased);
	void SetDrive(TDriveUnit aDrive);
	
 	inline void SetVendorId(const TVendorId aVid);

	void AddExtendedInterfaceL(const TUid& aExtendedInterface); 	
	RExtendedInterfacesArray* GetExtendedInterfaceList();
	
private: // internalComponent APIs
         // Internal methods used inside ECOM, not for client use.

	CImplementationInformation();

	CImplementationInformation(TUid aUid,
							   TInt aVersion,
							   HBufC*  aName,
							   HBufC8* aDataType,
							   HBufC8* aOpaqueData,
							   TDriveUnit aDrive,
							   TBool aRomOnly,
							   TBool aRomBased);
    
    CImplementationInformation(TUid aUid,
							   TInt aVersion,
							   HBufC*  aName,
							   HBufC8* aDataType,
							   HBufC8* aOpaqueData,
							   TDriveUnit aDrive,
							   TBool aRomOnly,
							   TBool aRomBased,
							   RExtendedInterfacesArray* aExtendedInterfaces);
							   	
private:
	/** The implementation UID */
	TUid		iImplementationUid;
	
	/** The implementation version */
	TInt		iVersion;
	
	/** This implementations human readable name intended for display in 
	    UI implementations */
	HBufC*		iDisplayName;
	
	/** The type of data this implementation is registered to handle :
	Used for matching against the run-time cue provided by the client
	during default interface implementation resolution.
	*/
	HBufC8*		iData;
	
	/** Data registered for this implementation which can be used by the client
	during non-default resolution operations but is ignored by the framework.
	*/
	HBufC8*		iOpaqueData;
	
	/** The flag recording if this implementation is disabled or enabled */
	TBool		iDisabled;
	
	/** The drive that this plug-in resides on */
	TDriveUnit		iDrive;
	
	/** The flag recording whether this implementation may only be loaded
	from read-only internal media */
	TBool iRomOnly;
	
	/** This flag records whether this implementation was discovered on
	read-only internal media or that it is a later version of one that was */
	TBool iRomBased;
	
	/** The Vendor ID of the plug-in which this implementation belongs to*/
	TVendorId iVid;
	
	/** Array of extended interfaces.*/
	RExtendedInterfacesArray* iExtendedInterfaceList;
	
	};	// End CImplementationInformation


// ____________________________________________________________________________
// TYPES

/**
Simplifies the definition of functions which return registry information
*/
typedef RArray<CImplementationInformation*> RImplInfoArray;

/**
Simplifies the definition of functions which return registry information
*/
typedef RPointerArray<CImplementationInformation> RImplInfoPtrArray;


// ____________________________________________________________________________
// INLINES

#include <ecom/implementationinformation.inl>


#endif		// __IMPLEMENTATIONINFORMATION_H__
