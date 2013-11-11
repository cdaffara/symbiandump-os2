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
// Provide the inline implementation of CImplementationInformation
// CImplementationInformation provides access to information on a particular implementation
// 
//

/**
 
 Intended Usage	:	Accessor for this implementation's human readable name
 @since			7.0
 @return			The human readable name for this implementation
 @pre 			CImplementationInformation is fully constructed and initialized
*/
const TDesC& CImplementationInformation::DisplayName() const
	{
	if(iDisplayName == NULL)
		return KNullDesC();
	else
		return *iDisplayName;
	}


/**
Intended Usage	:	Accessor for this implementation's default binary data
@since			7.0
@return			The data type which this implementation supports
@pre 			CImplementationInformation is fully constructed and initialized
*/
const TDesC8& CImplementationInformation::DataType() const
	{
	if(iData == NULL)
		return KNullDesC8();
	else
		return *iData;
	}


/**
Intended Usage	:	Accessor for this implementation's opaque binary data
@since			7.0
@return			The opaque data which is available for this implementation
@pre 			CImplementationInformation is fully constructed and initialized
*/
const TDesC8& CImplementationInformation::OpaqueData() const
	{
	if(iOpaqueData == NULL)
		return KNullDesC8();
	else
		return *iOpaqueData;
	}


/**
Intended Usage	:	Accessor for this implementation's Uid
@since			7.0
@return			The Uid of this implementation
@pre 			CImplementationInformation is fully constructed and initialized
*/
TUid CImplementationInformation::ImplementationUid() const
	{
	return iImplementationUid;
	}


/**
Intended Usage	:	Accessor for the version number of this implementation
@since			7.0
@return			The version number of this implementation
@pre 			CImplementationInformation is fully constructed and initialized
*/
TInt CImplementationInformation::Version() const
	{
	return iVersion;
	}


/**
Intended Usage	:	Accessor for whether this implementation is currently 
					disabled
@since			7.0
@return			Flag indicating whether this implementation is disabled
@pre 			CImplementationInformation is fully constructed and initialized
*/
TBool CImplementationInformation::Disabled() const
	{
	return iDisabled;
	}


/**
Intended Usage	:	Marks this implementation as disabled, or enabled.
Note that this function should not be used by any ECOM client side as it will have no effect at all
on the implementation information stored in the server side.
@since			7.0
@param			aDisabled ETrue to indicate this implementation should be disabled, EFalse for enabled.
@pre 			CImplementationInformation is fully constructed and initialized
@post			Implementation is marked as.
*/
void CImplementationInformation::SetDisabled(TBool aDisabled)
	{
	iDisabled = aDisabled;
	}

/**
Intended Usage	: Returns the drive that this implementation is installed on
Error Condition	: None
@since			7.0
@return			The drive that this implementation is on
@pre 			CImplementationInformation is fully constructed.
*/
TDriveUnit CImplementationInformation::Drive() const
	{
	return iDrive;
	}


/**
Intended Usage	: Accessor for whether this implementation is to be loaded 
				  from ROM only
@return			Flag indicating whether this implementation is to be loaded from ROM only
@pre 			CImplementationInformation is fully constructed
*/
TBool CImplementationInformation::RomOnly() const
	{
	return iRomOnly;
	}

/**
Intended Usage	: Accessor for whether this implementation is on ROM or is
				  a later version of one on ROM
@return			Flag indicating whether this implementation is on ROM or is a later version of one on ROM
@pre 			CImplementationInformation is fully constructed
*/
TBool CImplementationInformation::RomBased() const
	{
	return iRomBased;
	}
/**
Intended Usage:	Returns the VID of the plug-in that this implementation belongs to. 
				The VID is the VendorId for the plug-in's DLL
@pre			CImplementationInformation is fully constructed
@return			VendorId of the plug-in that this implementation belongs to.
*/
TVendorId CImplementationInformation::VendorId() const
	{
	return iVid;
	}
	
/**
Intended Usage:	Sets the VID of the plug-in that this implementation belongs to. 
				The VID is the VendorId for the plug-in's DLL.
@internalComponent
@pre			CImplementationInformation is fully constructed
@param 			aVid VendorId of the plug-in that this implementation belongs to.
*/
void CImplementationInformation::SetVendorId(const TVendorId aVid)
	{
	iVid = aVid;
	}

// ____________________________________________________________________________
// Provide the inline implementations of useful utilitiy functions related to
// CImplementationInformation for use in client and server sides.
//

/**
Comparison function used in ordering functions (e.g. as used with TLinearOrder) 
where ECOM UID identified structures are held in ordered RArray/RPointerArray 
objects.

@internalComponent
@param aUid1 First UID value of comparison
@param aUid2 Second UID Value of comparison
@return Returns 1 when aUid1 > aUid2; -1 when aUid1 < aUid2; 0 when they equal
*/
inline TInt CompareTUidValues(TInt aUid1, TInt aUid2)
	{
	// This has been implemented as 'return aUid1-aUid2' previously.
	// This can lead to overflow problems when comparing 2 signed integers
	// if the operands are large enough: large aUid minus large negative aUid2
	// returns a negative value due to arithmetic overflow of the result when 
	// you'd want a positive value. Hence the longer hand implementation below.
	if (aUid1 > aUid2)
		return 1;
	if (aUid1 < aUid2)
		return -1;
	return 0;
	}
