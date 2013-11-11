// Copyright (c) 2001-2009 Nokia Corporation and/or its subsidiary(-ies).
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

#include <authority8.h>
#include <authority16.h>
#include "CAuthorityInternal.h"
#include <uriutilscommon.h>
#include <escapeutils.h>
#include <uriutils.h>


//
//
// Implementation of CAuthority8
//
//

/**
	Static factory constructor. Uses two phase construction and leaves nothing on the 
	CleanupStack. Creates an authority object which is a copy of the input parameter 
	aAuthority.
	
	@since			6.0
	@param			aAuthority	A reference to a parsed authority object.
	@return			A pointer to the newly created CAuthority8 object. 
	@post			A fully constructed and initialized CAuthority8 object.
*/
EXPORT_C CAuthority8* CAuthority8::NewL(const TAuthorityC8& aAuthority)
	{
	CAuthority8* self = CAuthority8::NewLC(aAuthority);
	CleanupStack::Pop(self);
	return self;
	}

/**
	Static factory constructor. Uses two phase construction and leaves a pointer to created 
	object on the CleanupStack. Creates an authority object which is a copy of the input 
	parameter aAuthority.
	
	@since			6.0
	@param			aAuthority	A reference to a parsed authority object.
	@return			A pointer to the newly created CAuthority8 object. 
	@post			A fully constructed and initialized CAuthority8 object.
 */
EXPORT_C CAuthority8* CAuthority8::NewLC(const TAuthorityC8& aAuthority)
	{
	CAuthority8* self = new (ELeave) CAuthority8(aAuthority);
	CleanupStack::PushL(self);
	self->ConstructL();
	return self;
	}

/**
	Static factory constructor. Uses two phase construction and leaves nothing on the CleanupStack. 
	Creates an authority object which is empty.
	
	@since			6.0
	@return			A pointer to the newly created CAuthority8 object. 
	@post			A fully constructed and initialized CAuthority8 object.
 */
EXPORT_C CAuthority8* CAuthority8::NewL()
	{
	CAuthority8* self = CAuthority8::NewLC();
	CleanupStack::Pop(self);
	return self;
	}

/**
	Static factory constructor. Uses two phase construction and leaves a pointer to created object on 
	the CleanupStack. Creates an authority object which is empty.
	
	@since			6.0
	@return			A pointer to the newly created CAuthority8 object. 
	@post			A fully constructed and initialized CAuthority8 object.
 */
EXPORT_C CAuthority8* CAuthority8::NewLC()
	{
	CAuthority8* self = new (ELeave) CAuthority8(TAuthorityC8());
	CleanupStack::PushL(self);
	self->ConstructL();
	return self;
	}

/**
	Destructor.
	
	@since			6.0
 */
EXPORT_C CAuthority8::~CAuthority8()
	{
	delete iAuthorityBuf;
	}

/**
	Provides a reference to the parsed authority. Allows access to the non-modifying API for TAuthorityC8.
	
	@since			6.0
	@return			A const reference to the parsed authority object.
 */
EXPORT_C const TAuthorityC8& CAuthority8::Authority() const
	{
	return iAuthority;
	}

/**
	Sets the specified component in the authority. The component is set to the value given in the argument 
	aData. If the specified component already exists then it is replaced with the new value.
	
	@since			6.0
	@param			aData		A descriptor pointer to the new value for the authority component.
	@param			aComponent	An enum specifying the component to be set.
	@pre 			Object is fully constructed.
	@post			The authority has the specified component set to the new value.
	@Leave          KErrArgument  If aComponent goes out of range.
 */
EXPORT_C void CAuthority8::SetComponentL(const TDesC8& aData, TAuthorityComponent aComponent)
	{		
	// Update the appropriate component table entry
	iAuthority.iComponent[aComponent].Set(aData);

	// Copy to the buffer by forming the Authority
	FormAuthorityL();
	}

/**
	Escape encodes the component then sets the specified component in the authority. The component is set to the 
	value given in the argument aData. If the specified component already exists then it is replaced with the new 
	value.
	
	@since			6.0
	@param			aData		A descriptor pointer to the new value for the authority component.
	@param			aComponent	An enum specifying the component to be set.
	@pre 			Object is fully constructed.
	@post			The authority has the specified component set to the new value.
 */
EXPORT_C void CAuthority8::SetAndEscapeComponentL(const TDesC8& aData, TAuthorityComponent aComponent)
	{
	// Create escaped version of component
	HBufC8* escaped = EscapeUtils::EscapeEncodeL(aData, EscapeUtils::EEscapeAuth);
	CleanupStack::PushL(escaped);

	// Set the component	
	if(aComponent<EAuthorityMaxComponents && aComponent >=EAuthorityUserinfo )
	   {
	    SetComponentL(*escaped, aComponent);
	   }
	else
	   {
		User::Leave(KErrArgument);	
	   }

	// Cleanup
	CleanupStack::PopAndDestroy(escaped);
	}
	
/**
	Removes the specified component from the authority. If the component does not exist then this function does 
	nothing.
	
	@since			6.0
	@param			aComponent	An enum specifying the component to be removed.
	@pre 			Object is fully constructed.
	@post			The authority is updated to exclude the specified component.
 */
EXPORT_C void CAuthority8::RemoveComponentL(TAuthorityComponent aComponent)
	{
	if( iAuthority.IsPresent(aComponent) )
		{
		// Remove the component - set pointer to NULL and length to zero
		iAuthority.iComponent[aComponent].Set(NULL,0);

		// Re-form buffer and component table
		FormAuthorityL();
		}
	}

/**
	Constructor. First phase of two-phase construction method. Does non-allocating construction.
	
	@param			aNewAuthority	The parsed authority component information from which 
	to create the authority.
	@since			6.0
 */
CAuthority8::CAuthority8(const TAuthorityC8& aNewAuthority)
: CBase(), iAuthority(aNewAuthority)
	{
	}

/**
	Second phase of two-phase construction method. Does any allocations required to fully construct 
	the object.
	
	@since			6.0
	@pre 			First phase of construction is complete.
	@post			The object is fully constructed and initialized.
 */
void CAuthority8::ConstructL()
	{
	// Create the HBufC8
	FormAuthorityL();
	}

/**
	Forms the authority from the parsed authority information. A copy of the parsed authority is created. 
	The parsed authority is changed to refer to the copy.
	
	@since			6.0
	@pre 			The parsed authority information is set.
	@post			The authority buffer is a copy of the parsed authority, and the 
	parsed authority now refers to the copy.
 */
void CAuthority8::FormAuthorityL()
	{
	TBool isIPv6Host;

	// Calculate length of of the Authority
	TInt length = CalculateAuthorityLength(iAuthority.iComponent, isIPv6Host);

	// Create a temporary buffer and descriptor pointer to it
	HBufC8* buf = HBufC8::NewL(length);
	TPtr8 authority = buf->Des();

	// Create the authority, updating the internal authority object
	DoFormAuthority(authority, iAuthority.iComponent, isIPv6Host);

	// Update the internal buffer and descriptor pointer for the authority object
	delete iAuthorityBuf;
	iAuthorityBuf = buf;
	iAuthority.iAuthorityDes.Set(iAuthorityBuf->Des());
	}

//
//
// Implementation of CAuthority16
//
//

/**
	Static factory constructor. Uses two phase construction and leaves nothing on the CleanupStack. 
	Creates an authority object which is a copy of the input parameter aAuthority.
	
	@since			6.0
	@deprecated 	Deprecated in 9.1
	@param			aAuthority	A reference to a parsed authority object.
	@return			A pointer to the newly created CAuthority16 object. 
	@post			A fully constructed and initialized CAuthority16 object.
 */
EXPORT_C CAuthority16* CAuthority16::NewL(const TAuthorityC16& aAuthority)
	{
	CAuthority16* self = CAuthority16::NewLC(aAuthority);
	CleanupStack::Pop(self);
	return self;
	}

/**
	Static factory constructor. Uses two phase construction and leaves a pointer to created object on 
	the CleanupStack. Creates an authority object which is a copy of the input parameter aAuthority.
	
	@since			6.0
	@deprecated 	Deprecated in 9.1
	@param			aAuthority	A reference to a parsed authority object.
	@return			A pointer to the newly created CAuthority16 object. 
	@post			A fully constructed and initialized CAuthority16 object.
 */
EXPORT_C CAuthority16* CAuthority16::NewLC(const TAuthorityC16& aAuthority)
	{
	CAuthority16* self = new (ELeave) CAuthority16(aAuthority);
	CleanupStack::PushL(self);
	self->ConstructL();
	return self;
	}

/**
	Static factory constructor. Uses two phase construction and leaves nothing on the CleanupStack. 
	Creates an authority object which is empty.
	
	@since			6.0
	@deprecated 	Deprecated in 9.1
	@return			A pointer to the newly created CAuthority16 object. 
	@post			A fully constructed and initialized CAuthority16 object.
 */
EXPORT_C CAuthority16* CAuthority16::NewL()
	{
	CAuthority16* self = CAuthority16::NewLC();
	CleanupStack::Pop(self);
	return self;
	}

/**
	Static factory constructor. Uses two phase construction and leaves a pointer to created object on the 
	CleanupStack. Creates an authority object which is empty.
	
	@since			6.0
	@deprecated 	Deprecated in 9.1
	@return			A pointer to the newly created CAuthority16 object. 
	@post			A fully constructed and initialized CAuthority16 object.
 */
EXPORT_C CAuthority16* CAuthority16::NewLC()
	{
	CAuthority16* self = new (ELeave) CAuthority16(TAuthorityC16());
	CleanupStack::PushL(self);
	self->ConstructL();
	return self;
	}

/**
	Destructor.
	
	@since			6.0
	@deprecated 	Deprecated in 9.1
 */
EXPORT_C CAuthority16::~CAuthority16()
	{
	delete iAuthorityBuf;
	}

/**
	Provides a reference to the parsed authority. Allows access to the non-modifying API for TAuthorityC.
	
	@since			6.0
	@deprecated 	Deprecated in 9.1
	@return			A const reference to the parsed authority object.
 */
EXPORT_C const TAuthorityC16& CAuthority16::Authority() const
	{
	return iAuthority;
	}

/**
	Sets the specified component in the authority. The component is set to the value given in the argument aData. 
	If the specified component already exists then it is replaced with the new value.
	
	@since			6.0
	@deprecated 	Deprecated in 9.1
	@param			aData		A descriptor pointer to the new value for the authority component.
	@param			aComponent	An enum specifying the component to be set.
	@pre 			Object is fully constructed.
	@post			The authority has the specified component set to the new value.
	@Leave          KErrArgument  If aComponent goes out of range.
 */
EXPORT_C void CAuthority16::SetComponentL(const TDesC16& aData, TAuthorityComponent aComponent)
	{
	// Update the appropriate component table entry
	iAuthority.iComponent[aComponent].Set(aData);

	// Copy to the buffer by forming the Authority
	FormAuthorityL();
	}

/**
	Escape encodes the component then sets the specified component in the authority. The component is set to the 
	value given in the argument aData. If the specified component already exists then it is replaced with the new 
	value.
	
	@since			6.0
	@deprecated 	Deprecated in 9.1
	@param			aData		A descriptor pointer to the new value for the authority component.
	@param			aComponent	An enum specifying the component to be set.
	@pre 			Object is fully constructed.
	@post			The authority has the specified component set to the new value.
 */
EXPORT_C void CAuthority16::SetAndEscapeComponentL(const TDesC16& aData, TAuthorityComponent aComponent)
	{
	// Need to convert to utf8 first
	HBufC8* utf8 = EscapeUtils::ConvertFromUnicodeToUtf8L(aData);
	CleanupStack::PushL(utf8);

	// Create escaped version of component
	HBufC8* escaped = EscapeUtils::EscapeEncodeL(*utf8, EscapeUtils::EEscapeAuth);
	CleanupStack::PushL(escaped);

	// Convert back to 16-bits
	HBufC16* converted = HBufC16::NewLC(escaped->Length());
	converted->Des().Copy(*escaped);

	// Set the component
	if(aComponent<EAuthorityMaxComponents && aComponent >=EAuthorityUserinfo )
	   {
		SetComponentL(*converted, aComponent);
	   }
    else
	   {
		User::Leave(KErrArgument);	
	   }	

	// Cleanup
	CleanupStack::PopAndDestroy(3, utf8);	// utf8, escaped, converted
	}

/**
	Removes the specified component from the authority. If the component does not exist then this 
	function does nothing.
	
	@since			6.0
	@deprecated 	Deprecated in 9.1
	@param			aComponent	An enum specifying the component to be removed.
	@pre 			Object is fully constructed.
	@post			The authority is updated to exclude the specified component.
 */
EXPORT_C void CAuthority16::RemoveComponentL(TAuthorityComponent aComponent)
	{
	if( iAuthority.IsPresent(aComponent) )
		{
		// Remove the component - set pointer to NULL and length to zero
		iAuthority.iComponent[aComponent].Set(NULL,0);

		// Re-form buffer and component table
		FormAuthorityL();
		}
	}

/**
	Constructor. First phase of two-phase construction method. Does non-allocating construction.
	
	@since			6.0
	@param			aNewAuthority	The parsed authority component information from which 
	to create the authority.
 */
CAuthority16::CAuthority16(const TAuthorityC16& aNewAuthority)
: CBase(), iAuthority(aNewAuthority)
	{
	}

/**
	Second phase of two-phase construction method. Does any allocations required to fully construct 
	the object.
	
	@since			6.0
	@pre 			First phase of construction is complete.
	@post			The object is fully constructed and initialized.
 */
void CAuthority16::ConstructL()
	{
	// Create the HBufC16
	FormAuthorityL();
	}

/**
	Forms the authority from the parsed authority information. A copy of the parsed authority is created.
	The parsed authority is changed to refer to the copy.
	
	@since			6.0
	@pre 			The parsed authority information is set.
	@post			The authority buffer is a copy of the parsed authority, and the 
					parsed authority now refers to the copy.
 */
void CAuthority16::FormAuthorityL()
	{
	TBool isIPv6Host;

	// Calculate length of of the Authority
	TInt length = CalculateAuthorityLength(iAuthority.iComponent, isIPv6Host);

	// Create a temporary buffer and descriptor pointer to it
	HBufC16* buf = HBufC16::NewL(length);
	TPtr16 authority = buf->Des();

	// Create the authority, updating the internal authority object
	DoFormAuthority(authority, iAuthority.iComponent, isIPv6Host);

	// Update the internal buffer and descriptor pointer for the authority object
	delete iAuthorityBuf;
	iAuthorityBuf = buf;
	iAuthority.iAuthorityDes.Set(iAuthorityBuf->Des());
	}

//
//
// Implementation of templated LOCAL functions
//
//

/**
	Calculates the length of the authority from a list of the components.
					  
	@since			6.0
	@param			aComponent	The array of descriptor pointers to the authority 
	components.
	@param			aIsIPv6Host	Specifies ETrue or EFalse.
	@return			The length of the authority including the required delimiters.	
*/
template<class TPtrCType>
TInt CalculateAuthorityLength(const TPtrCType aComponent[], TBool& aIsIPv6Host)
	{
	aIsIPv6Host = EFalse;
	TInt length=0;
	for( TInt i=0; i<EAuthorityMaxComponents; ++i )
		{
		if( aComponent[i].Ptr() )
			{
			length += aComponent[i].Length();
			// Need to make space for a delimiter if not a host
			if( i != EAuthorityHost )
				++length;
			else
				// Check the host type
				if (UriUtils::HostType(aComponent[i]) == UriUtils::EIPv6Host)
					{
					// If its an IPv6 format host, need to increase the length for the []
					aIsIPv6Host = ETrue;
					length+=2;
					}
			}
		}
	return length;
	}

/**
	Templated function to form an authority. The new authority component information is given by 
	the input/output argument aComponent. For each authority component that exists in aComponent, 
	that component and its appropriate delimiters are appended to aAuthority. Then the components 
	in aComponent are updated to refer to the copied versions in aUri.
	
	@since			6.0
	@param			aAuthority	The descriptor pointer to buffer to be appended.
	@param			aComponent	The array of descriptor pointers to be copied and 
	then updated.
	@param			aIsIPv6Host	Specifies ETrue or EFalse.
	@pre 			The buffer pointed to by aAuthority should be large enough 
	to have the authority components given in aNewComponent copied into it, as well as 
	the required delimiters.
	@post			The authority buffer will have a copy of the authority defined in
	aNewComponent, and aOldComponent will refer to copies of these components in aAuthority.
*/
template<class TPtrType, class TPtrCType>
void DoFormAuthority(TPtrType& aAuthority, TPtrCType aComponent[], const TBool& aIsIPv6Host)
	{
	if( aComponent[EAuthorityUserinfo].Ptr() )
		{
		// Update the userinfo
		SetUserinfo(aAuthority, aComponent[EAuthorityUserinfo]);
		}
	if( aComponent[EAuthorityHost].Ptr() )
		{
		// Update the host
		SetHost(aAuthority, aComponent[EAuthorityHost], aIsIPv6Host);
		}
	if( aComponent[EAuthorityPort].Ptr() )
		{
		// Update the port
		SetPort(aAuthority, aComponent[EAuthorityPort]);
		}
	}

/**
	Templated function to set the userinfo in an authority. The output argument aAuthority points to 
	the descriptor buffer into which aNewUserInfo will be copied. The argument aOldUserInfo is updated 
	to point to the copied version of aNewUserInfo in aAuthority.
						
	@warning		This function will panic with KAuthorityErrBufferOverflow if there is not
	enough space in the descriptor to append the component and any required delimiters.
	@since			6.0
	@param			aAuthority	The descriptor pointer to buffer to be appended.
	@param			aUserinfo	The descriptor pointer to the userinfo component to be copied 
	and then updated.
	@pre 			The buffer pointed to by aAuthority should be large enough to have 
	aNewUserInfo appended to it with the required delimiter.
	@post			The authority buffer now includes aNewUserInfo and aOldUserInfo points
	to the copy of the userinfo component in aAuthority.
*/
template<class TPtrType, class TPtrCType> 
void SetUserinfo(TPtrType& aAuthority, TPtrCType& aUserinfo)
	{
	// Append the userinfo and delimiter
	aAuthority.Append(aUserinfo);
	aAuthority.Append(KUserinfoDelimiter);

	// Update the component table
	aUserinfo.Set(aAuthority.Left(aUserinfo.Length()));
	}

/**
	Templated function to set the host in an authority. The output argument aAuthority points to the 
	descriptor buffer into which aNewHost will be copied. The argument aOldHost is updated to point to 
	the copied version of aNewHost in aAuthority.
						
	@warning		This function will panic with KAuthorityErrBufferOverflow if there is not
	enough space in the descriptor to append the component and any required delimiters.
	@since			6.0
	@param			aAuthority	The descriptor pointer to buffer to appended.
	@param			aHost		The descriptor pointer to the host component to be copied 
	and then updated.
	@param			aIsIPv6Host	Specifies ETrue or EFalse.
	@pre 			The buffer pointed to by aAuthority should be large enough to have 
	aNewHost appended to it with the required delimiter.
	@post			The authority buffer now includes aNewHost and aOldHost points to the 
	copy of the host component in aAuthority.
*/
template<class TPtrType, class TPtrCType> 
void SetHost(TPtrType& aAuthority, TPtrCType& aHost, const TBool& aIsIPv6Host)
	{
	if (aIsIPv6Host)
		{
		aAuthority.Append(KIPv6UriOpenBrace);
		aAuthority.Append(aHost);
		aAuthority.Append(KIPv6UriCloseBrace);

		// Position = (length of uri - length of host) - length of end brace
		aHost.Set( aAuthority.Mid((aAuthority.Length()-aHost.Length())-1, aHost.Length()) );
		}
	else
		{
		// Append the host
		aAuthority.Append(aHost);

		// Update the component table
		aHost.Set(aAuthority.Right(aHost.Length()));
		}
	}

/**
	Templated function to set the port in an authority. The output argument aAuthority points to the 
	descriptor buffer into which aNewPort will be copied. The argument aOldPort is updated to point to 
	the copied version of aNewPort in aAuthority.
						
	@warning		This function will panic with KAuthorityErrBufferOverflow if there is not
	enough space in the descriptor to append the component and any required delimiters.
	@since			6.0
	@param			aAuthority	The descriptor pointer to buffer to appended.
	@param			aPort		The descriptor pointer to the port component to be copied 
	and then updated.
	@pre 			The buffer pointed to by aAuthority should be large enough to have aNewPort
	appended to it with the required delimiter.
	@post			The authority buffer now includes aNewPort and aOldPort points to the 
	copy of the port component in aAuthority.
*/
template<class TPtrType, class TPtrCType> 
void SetPort(TPtrType& aAuthority, TPtrCType& aPort)
	{
	// Append delimiter and the port
	aAuthority.Append(KPortDelimiter);
	aAuthority.Append(aPort);

	// Update the component table
	aPort.Set(aAuthority.Right(aPort.Length()));
	}
