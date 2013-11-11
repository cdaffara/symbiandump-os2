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

#include <escapeutils.h>
#include <uri8.h>
#include <uri16.h>
#include <delimitedpath8.h>
#include <delimitedpath16.h>
#include <uriutilscommon.h>
#include <uriutils.h>

#include "UriUtilsInternal.h"
#include "TUriCInternal.h"
#include "CUriInternal.h"

// Panic category
//
#ifdef _DEBUG
	_LIT(KUriPanicCategory,"URI-CURI"); 
#endif

// Constants
//
_LIT(KFileUriPanicCategory,"FILEURI-CURI");




//
//
// Implementation of CUri8
//
//

/**
	Static factory constructor. Uses two phase construction and leaves nothing on the 
	CleanupStack. Creates a uri object which is a copy of the input parameter aUri.
	
	@since			6.0
	@param			aUri	A reference to a parsed uri object.
	@return			A pointer to the newly created CUri8 object. 
	@post			A fully constructed and initialized CUri8 object.
 */
EXPORT_C CUri8* CUri8::NewL(const TUriC8& aUri)
	{
	CUri8* self = CUri8::NewLC(aUri);
	CleanupStack::Pop(self);
	return self;
	}

/**	
	Static factory constructor. Uses two phase construction and leaves a pointer to 
	created object on the CleanupStack. Creates a uri object which is a copy of the 
	input parameter aUri.
	
	@since			6.0
	@param			aUri	A reference to a parsed uri object.
	@return			A pointer to the newly created CUri8 object. 
	@post			A fully constructed and initialized CUri8 object.
 */
EXPORT_C CUri8* CUri8::NewLC(const TUriC8& aUri)
	{
	CUri8* self = new (ELeave) CUri8(aUri);
	CleanupStack::PushL(self);
	self->ConstructL();
	return self;
	}
	
/**	
	Static factory constructor. Uses two phase construction and leaves nothing on the 
	CleanupStack. Creates a uri object which is empty.
	
	@since			6.0
	@return			A pointer to the newly created CUri8 object. 
	@post			A fully constructed and initialized CUri8 object.
 */
EXPORT_C CUri8* CUri8::NewL()
	{
	CUri8* self = CUri8::NewLC();
	CleanupStack::Pop(self);
	return self;
	}

/**
	Static factory constructor. Uses two phase construction and leaves a pointer to created 
	object on the CleanupStack. Creates a uri object which is empty.
	
	@since			6.0
	@return			A pointer to the newly created CUri8 object. 
	@post			A fully constructed and initialized CUri8 object.
 */
EXPORT_C CUri8* CUri8::NewLC()
	{
	CUri8* self = new (ELeave) CUri8(TUriC8());
	CleanupStack::PushL(self);
	self->ConstructL();
	return self;
	}

/**	
	Static factory constructor. This creates a CUri8 object that is an absolute uri resulting 
	from a reference uri being resolved against a base uri.
	
	@warning		Ownership of created CUri8 object is transferred to the caller.
	@leave			KUriErrBadBasePath if the base path is not an absolute path and not empty.
	@since			6.0
	@param			aBaseUri	A referece to the parsed base uri.
	@param			aRefUri		A referece to the parsed reference uri.
	@return			A pointer to the newly created CUri8 object.
	@pre 			The base uri must have an absolute or empty path, otherwise will leave
	with KUriErrBadBasePath.
	@post			A fully constructed and initialized CUri8 object.
 */
EXPORT_C CUri8* CUri8::ResolveL(const TUriC8& aBaseUri, const TUriC8& aRefUri)
	{
	// Check for a base Uri
	if( aBaseUri.UriDes().Compare(KNullDesC8) == 0 )
		{
		// Empty base Uri - resolved Uri is the reference Uri
		return NewL(aRefUri);
		}
	// See if ref has scheme and it is the same as base Uri
	if( aRefUri.IsPresent(EUriScheme) && (aRefUri.Compare(aBaseUri, EUriScheme) != 0) )
		{
		// Ref has a scheme different to base Uri's - it is an absolute Uri
		return NewL(aRefUri);
		}
	// Check for presence of components
	TBool useBaseQuery = EFalse;
	HBufC8* resolvedPath = FormResolvedPathLC<HBufC8>(aBaseUri, aRefUri, useBaseQuery);

	//Removes dot segemnts in Resolved uri as specified in RFC3986 section 5.2.
	RemoveExtraneousDotSegmentsL(resolvedPath);
	
	// Put the Uri together
	TUriC8 uri;
	FormResolvedUri(uri.iComponent, aBaseUri, aRefUri, resolvedPath, useBaseQuery);

	// Create the resolved Uri and cleanup
	CUri8* resolvedUri = NewL(uri);
	CleanupStack::PopAndDestroy(resolvedPath);

	return resolvedUri;
	}

/**	
	Destructor.
	
	@since			6.0
 */
EXPORT_C CUri8::~CUri8()
	{
	delete iUriBuf;
	}

/**	
	Provides a reference to the parsed uri. Allows access to the non-modifying API for TUriC8.
	
	@since			6.0
	@return			A const reference to the parsed uri object.
 */
EXPORT_C const TUriC8& CUri8::Uri() const
	{
	return iUri;
	}

/**	
	Intended Usage	:	Sets the specified component in the uri. The component is set to the value 
	given in the argument aData. If the specified component already exists  then it is replaced 
	with the new value.
	
	@warning		The userinfo and port components can only be set if the host component
	is present. Setting these components without a host component present will have no 
	effect on the uri.
	@since			6.0
	@param			aData		A descriptor pointer to the new value for the uri component.
	@param			aComponent	An enum specifying the component to be set.
	@pre 			Object is fully constructed.
	@post			The uri has the specified component set to the new value.
	@Leave          KErrArgument  If aComponent goes out of range.
 */
EXPORT_C void CUri8::SetComponentL(const TDesC8& aData, TUriComponent aComponent)
	{
	// Update the appropriate component table entry
	iUri.iComponent[aComponent].Set(aData);

	// Copy to the buffer by forming the uri
	FormUriL();
	}

/**	
	Removes the specified component from the uri. If the component does not exist then this function 
	does nothing.
	
	@warning		If host is removed, then userinfo and port components will also
	be removed.
	@since			6.0
	@param			aComponent	An enum specifying the component to be removed.
	@pre 			Object is fully constructed.
	@post			The uri is updated to exclude the specified component.
 */
EXPORT_C void CUri8::RemoveComponentL(TUriComponent aComponent)
	{
	if( iUri.IsPresent(aComponent) )
		{
		// Remove the component - set pointer to NULL and length to zero
		iUri.iComponent[aComponent].Set(NULL,0);

		// Re-form buffer and component table
		FormUriL();
		}
	}

/**	
	Constructor. First phase of two-phase construction method. Does	non-allocating construction.
	
	@since			6.0
	@param			aUri	The parsed uri component information from which to create 
	the uri.
*/
CUri8::CUri8(const TUriC8& aUri)
: CBase(), iUri(aUri)
	{
	}

/**	
	Second phase of two-phase construction method. Does any allocations required to fully construct 
	the object.
	
	@since			6.0
	@pre 			First phase of construction is complete.
	@post			The object is fully constructed and initialized.
 */
void CUri8::ConstructL()
	{
	// Create the HBufC
	FormUriL();
	}

/**	
	Forms the uri from the parsed uri information. A copy of the parsed uri is created. The parsed uri 
	is changed to refer to the copy.
	
	@since			6.0
	@pre 			The parsed uri information is set.
	@post			The uri buffer is updated with the parsed uri information.
 */
void CUri8::FormUriL()
	{
	TBool isIPv6Host;

	// Calculate length of of the Uri
	TInt length = CalculateUriLength(iUri.iComponent, isIPv6Host);

	// Create a temporary buffer and descriptor pointer to it
	HBufC8* buf = HBufC8::NewL(length);
	TPtr8 uri = buf->Des();

	// Create the uri, updating the internal uri object
	DoFormUri(uri, iUri.iComponent, isIPv6Host);

	// Update the internal buffer and descriptor pointer
	delete iUriBuf;
	iUriBuf = buf;
	iUri.iUriDes.Set(iUriBuf->Des());
	}

//
//
// Implementation of CUri16
//
//

/**	
	Static factory constructor. Uses two phase construction and leaves nothing on the CleanupStack. 
	Creates a uri object which is a copy of the input parameter aUri.
	
	@deprecated Deprecated in 9.1
	@since			6.0
	@param			aUri	A reference to a parsed uri object.
	@return			A pointer to the newly created CUri16 object. 
	@post			A fully constructed and initialized CUri16 object.
 */
EXPORT_C CUri16* CUri16::NewL(const TUriC16& aUri)
	{
	CUri16* self = CUri16::NewLC(aUri);
	CleanupStack::Pop(self);
	return self;
	}

/**	
	Static factory constructor. Uses two phase construction and leaves a pointer to created object on 
	the CleanupStack. Creates a uri object which is a copy of the input parameter aUri.
	
	@since			6.0
	@deprecated Deprecated in 9.1
	@param			aUri	A reference to a parsed uri object.
	@return			A pointer to the newly created CUri16 object. 
	@post			A fully constructed and initialized CUri16 object.
 */
EXPORT_C CUri16* CUri16::NewLC(const TUriC16& aUri)
	{
	CUri16* self = new (ELeave) CUri16(aUri);
	CleanupStack::PushL(self);
	self->ConstructL();
	return self;
	}

/**
	Static factory constructor. Uses two phase construction and leaves nothing on the CleanupStack. 
	Creates a uri object which is empty.
	
	@since			6.0
	@deprecated Deprecated in 9.1
	@return			A pointer to the newly created CUri16 object. 
	@post			A fully constructed and initialized CUri16 object.
 */
EXPORT_C CUri16* CUri16::NewL()
	{
	CUri16* self = CUri16::NewLC();
	CleanupStack::Pop(self);
	return self;
	}

/**
	Static factory constructor. Uses two phase construction and leaves a pointer to created object on 
	the CleanupStack. Creates a uri object which is empty.
	
	@since			6.0
	@deprecated Deprecated in 9.1
	@return			A pointer to the newly created CUri16 object. 
	@post			A fully constructed and initialized CUri16 object.
 */
EXPORT_C CUri16* CUri16::NewLC()
	{
	CUri16* self = new (ELeave) CUri16(TUriC16());
	CleanupStack::PushL(self);
	self->ConstructL();
	return self;
	}

/**	
	Static factory constructor. This creates a CUri16 object that is an absolute uri resulting from a 
	reference uri being resolved against a base uri.
	
	@warning		Ownership of created CUri16 object is transferred to caller.
	@leave			KUriErrBadBasePath if the base path is not an absolute path and not empty.
	@since			6.0
	@deprecated Deprecated in 9.1
	@param			aBaseUri	A referece to the parsed base uri.
	@param			aRefUri		A referece to the parsed reference uri.
	@return			A pointer to the newly created CUri16 object.
	@pre 			The base uri must have an absolute or empty path, otherwise will leave
	with KUriErrBadBasePath.
	@post			A fully constructed and initialized CUri16 object.
 */
EXPORT_C CUri16* CUri16::ResolveL(const TUriC16& aBaseUri, const TUriC16& aRefUri)
	{
	// Check for a base Uri
	if( aBaseUri.UriDes().Compare(KNullDesC16) == 0 )
		{
		// Empty base Uri - resolved Uri is the reference Uri
		return NewL(aRefUri);
		}
	// See if ref has scheme and it is the same as base Uri
	if( aRefUri.IsPresent(EUriScheme) && aRefUri.Compare(aBaseUri, EUriScheme) != 0 )
		{
		// Ref has a scheme different to base Uri's - it is an absolute Uri
		return NewL(aRefUri);
		}
	// Check for presence of components
	TBool useBaseQuery = EFalse;
	HBufC16* resolvedPath = FormResolvedPathLC<HBufC16>(aBaseUri, aRefUri, useBaseQuery);

	// Put the Uri together
	TUriC16 uri;
	FormResolvedUri(uri.iComponent, aBaseUri, aRefUri, resolvedPath, useBaseQuery);

	// Create the resolved Uri and cleanup
	CUri16* resolvedUri = NewL(uri);
	CleanupStack::PopAndDestroy(resolvedPath);

	return resolvedUri;
	}

/**	
	Destructor.
	
	@since			6.0
	@deprecated Deprecated in 9.1
 */
EXPORT_C CUri16::~CUri16()
	{
	delete iUriBuf;
	}

/**	
	Provides a reference to the parsed uri. Allows access to the non-modifying API for TUriC16.
	
	@since			6.0
	@deprecated Deprecated in 9.1
	@return			A const reference to the parsed uri object.
 */
EXPORT_C const TUriC16& CUri16::Uri() const
	{
	return iUri;
	}

/**
	Sets the specified component in the uri. The component is set to the value given in the argument 
	aData. If the specified component already exists then it is replaced with the new value.
	
	@warning		The userinfo and port components can only be set if the host component
	is present. Setting these components without a host component present will have no 
	effect on the uri.
	@since			6.0
	@deprecated Deprecated in 9.1
	@param			aData		A descriptor pointer to the new value for the uri component.
	@param			aComponent	An enum specifying the component to be set.
	@pre 			Object is fully constructed.
	@post			The uri has the specified component set to the new value.
	@Leave          KErrArgument  If aComponent goes out of range.
 */
EXPORT_C void CUri16::SetComponentL(const TDesC16& aData, TUriComponent aComponent)
	{
	// Update the appropriate component table entry
	iUri.iComponent[aComponent].Set(aData);

	// Copy to the buffer by forming the uri
	FormUriL();
	}

/**
	Removes the specified component from the uri. If the component does not exist then this function 
	does nothing.
	
	@warning		If host is removed, then userinfo and port components will also
	be removed.
	@since			6.0
	@deprecated Deprecated in 9.1
	@param			aComponent	An enum specifying the component to be removed.
	@pre 			Object is fully constructed.
	@post			The uri is updated to exclude the specified component.
 */
EXPORT_C void CUri16::RemoveComponentL(TUriComponent aComponent)
	{
	if( iUri.IsPresent(aComponent) )
		{
		// Remove the component - set pointer to NULL and length to zero
		iUri.iComponent[aComponent].Set(NULL,0);

		// Re-form buffer and component table
		FormUriL();
		}
	}

/**
	Constructor. First phase of two-phase construction method. Does	non-allocating construction.
	
	@since			6.0
	@param			aUri	The parsed uri component information from which to create
	the uri.
 */

CUri16::CUri16(const TUriC16& aUri)
: CBase(), iUri(aUri)
	{
	}

/**
	Second phase of two-phase construction method. Does any allocations required to fully construct 
	the object.
	
	@since			6.0
	@pre 			First phase of construction is complete.
	@post			The object is fully constructed and initialized.
 */
void CUri16::ConstructL()
	{
	// Create the HBufC
	FormUriL();
	}

/**
	Forms the uri from the parsed uri information. A copy of the parsed uri is created. The parsed uri 
	is changed to refer to the copy.
	
	@since			6.0
	@pre 			The parsed uri information is set.
	@post			The uri buffer is updated with the parsed uri information.
 */
void CUri16::FormUriL()
	{
	TBool isIPv6Host;

	// Calculate length of of the Uri
	TInt length = CalculateUriLength(iUri.iComponent, isIPv6Host);

	// Create a temporary buffer and descriptor pointer to it
	HBufC16* buf = HBufC16::NewL(length);
	TPtr16 uri = buf->Des();

	// Create the uri, updating the internal uri object
	DoFormUri(uri, iUri.iComponent, isIPv6Host);

	// Update the internal buffer and descriptor pointer
	delete iUriBuf;
	iUriBuf = buf;
	iUri.iUriDes.Set(iUriBuf->Des());
	}

//
//
// Implementation of templated LOCAL functions
//
//

/**
	Calculates the length of the uri from a list of the components.
	
	@since			6.0
	@param			aComponent	The array of descriptor pointers to the uri 
					components.
	@param			aIsIPv6Host	ETrue if an IPv6 format host is used
	@return			The length of the uri including the required delimiters.	
*/
template<class TPtrCType>
TInt CalculateUriLength(const TPtrCType aComponent[], TBool& aIsIPv6Host)
	{
	TBool noAuthority = ETrue;
	TInt length=0;
	aIsIPv6Host=EFalse;
	for( TInt i=0; i<EUriMaxComponents; ++i )
		{
		if( aComponent[i].Ptr() )
			{
			length += aComponent[i].Length();
			if( noAuthority && (i==EUriUserinfo || i==EUriHost || i==EUriPort) )
				{
				// There's an authority part...
				noAuthority = EFalse;
				}
			// Need to make space for a delimiter if not path or host
			if( i!=EUriHost && i!=EUriPath )
				++length;		
			
			// If it's an IPv6 hostname, need extra space for []
			if(i==EUriHost && (UriUtils::HostType(aComponent[i])==UriUtils::EIPv6Host))
				{
				length+=2;				
				aIsIPv6Host=ETrue;
				}
			}
		}
	if( !noAuthority && IsNetworkScheme(aComponent[EUriScheme]))
		{
		// Make space for authority delimiter
		length += KUriNetworkAuthorityDelimiterLength;
		}
	return length;
	}

/**
	Templated function to form a uri. The output argument aUri points to a descriptor 
	buffer large enough to hold the uri. The new uri component information is given by 
	the input/output argument aComponent. For each uri component that exists in aComponent,
	that component and its appropriate delimiters are appended to aUri. Then the components 
	in aComponent are updated to refer to the copies versions in aUri.
						
	@since			6.0
	@param			aUri		The descriptor pointer to buffer to be appended.
	@param			aComponent	The array of descriptor pointers to be copied and 
					then updated.
	@param			aIsIPv6Host	ETrue if an IPv6 format host is used
	@pre 			The buffer pointed to by aUri should be large enough to have the uri 
					components given in aNewComponent copied into it, as well as the required delimiters.
					This can be obtained using CalculateUriLength().
	@post			The uri buffer pointed to by aUri will have a copy of the uri defined 
					in aComponent, and then aComponent will refer to the copies of these components in aUri.
*/
template<class TPtrType, class TPtrCType>
void DoFormUri(TPtrType& aUri, TPtrCType aComponent[], TBool& aIsIPv6Host)
	{
	TBool isNetworkScheme = ETrue;
	if( aComponent[EUriScheme].Ptr() )
		{
		// Update the scheme
		SetScheme(aUri, aComponent[EUriScheme]);
		isNetworkScheme = IsNetworkScheme(aComponent[EUriScheme]);
		}
	if( aComponent[EUriHost].Ptr() )
		{
		// Update the authority - only needed if there is a host; update userinfo, host and port
		SetAuthority(aUri, aComponent[EUriUserinfo], aComponent[EUriHost], aComponent[EUriPort], aIsIPv6Host, isNetworkScheme);
		}
	else
		{
		// Ensure that there is no userinfo or port components if there is no host 
		// - set pointer to NULL and length to zero
		aComponent[EUriUserinfo].Set(NULL,0);
		aComponent[EUriPort].Set(NULL,0);
		}
	if( aComponent[EUriPath].Ptr() )
		{
		// Update the path
		SetPath(aUri, aComponent[EUriPath]);
		}
	if( aComponent[EUriQuery].Ptr() )
		{
		// Update the query
		SetQuery(aUri, aComponent[EUriQuery]);
		}
	if( aComponent[EUriFragment].Ptr() )
		{
		// Update the fragment
		SetFragment(aUri, aComponent[EUriFragment]);
		}
	}

/**
	Templated function to set the scheme in a uri. The output argument aUri points to the descriptor 
	buffer into which aScheme will be copied.The argument aScheme is then updated to point to the 
	copied version in aUri.
						
	@warning		This function will panic with KUriErrBufferOverflow if there is not
					enough space in the descriptor to append the scheme and the required delimiter.
	@since			6.0
	@param			aUri	The descriptor pointer to buffer to be appended.
	@param			aScheme	The descriptor pointer to the scheme component to be copied 
					and then updated.
	@pre 			The buffer pointed to by aUri should be large enough to have aNewScheme
					appended to it with the required delimiter. This can be obtained using CalculateUriLength().
	@post			The uri buffer now includes a copy of aScheme and aScheme points to the 
					copy of the scheme component in aUri.
*/
template<class TPtrType, class TPtrCType>
void SetScheme(TPtrType& aUri, TPtrCType& aScheme)
	{
	__ASSERT_DEBUG(aUri.Length() + aScheme.Length() + 1 <= aUri.MaxLength(), User::Panic(KUriPanicCategory, KUriUtilsErrBufferOverflow));

	// Append the scheme and delimiter
	aUri.Append(aScheme);
	aUri.Append(KSchemeDelimiter);

	// Update the component table to use the copy
	aScheme.Set(aUri.Left(aScheme.Length()));
	}
	
/**
	Templated function to set the authority in a uri. The output argument aUri points to the descriptor 
	buffer into which aUserinfo, aHost and aPort will be copied. The arguments aUserinfo, aHost and aPort 
	are updated to point to the copied versions in aUri.
						
	@warning		This function will panic with KUriErrBufferOverflow if there 
					is not enough space in the descriptor to append the components and any required 
					delimiters. 
	@since			6.0
	@param			aUri		The descriptor pointer to buffer to be appended.
	@param			aUserinfo	The descriptor pointer to the userinfo component to 
					be copied and then updated.
	@param			aHost		The descriptor pointer to the host component to 
					be copied and then updated.
	@param			aPort		The descriptor pointer to the port component to 
					be copied and then updated.
	@param			aIsIPv6Host	ETrue if an IPv6 format host is used
	@param			aUseNetworkDelimiter EFalse if this is a SIP scheme otherwise ETrue
	@pre 			The buffer pointed to by aUri should be large enough to have 
					aUserinfo, aHost and aPort appended to it with the required delimiters.
					This can be obtained using CalculateUriLength().
	@post			The uri buffer now includes a copy of aUserinfo, aHost and
					aPort, and aUserinfo, aHost and aPort will refer to the copies versions in aUri.
*/
template<class TPtrType, class TPtrCType>
void SetAuthority(TPtrType& aUri, TPtrCType& aUserinfo, TPtrCType& aHost, TPtrCType& aPort, TBool& aIsIPv6Host, TBool aUseNetworkDelimiter)
	{
	__ASSERT_DEBUG(aUri.Length() + aHost.Length() + (aUseNetworkDelimiter ? KUriNetworkAuthorityDelimiterLength:0) <= aUri.MaxLength(), User::Panic(KUriPanicCategory, KUriUtilsErrBufferOverflow));
	
	if (aUseNetworkDelimiter)
		{
		// If a network scheme append authority delimiter (TWO slash delimiters!)
		aUri.Append(KSlashDelimiter);
		aUri.Append(KSlashDelimiter);
		}

	// Check for userinfo
	if( aUserinfo.Ptr() )
		{
		__ASSERT_DEBUG(aUri.Length() + aUserinfo.Length() + aHost.Length() + 1 <= aUri.MaxLength(), User::Panic(KUriPanicCategory, KUriUtilsErrBufferOverflow));

		// Append userinfo and update the component table to use copy
		aUri.Append(aUserinfo);
		aUserinfo.Set(aUri.Right(aUserinfo.Length()));

		// Append delimiter
		aUri.Append(KUserinfoDelimiter);
		}
	// There's always a host - append and update the component table to use the copy
	
	// Check if it's an IPv6 address
	if ( aIsIPv6Host )
		{
		aUri.Append(KIPv6UriOpenBrace); 
		aUri.Append(aHost);
		aUri.Append(KIPv6UriCloseBrace);
		// Dont include the braces in the host
		// Position = (length of uri - length of host) - length of end brace
		aHost.Set( aUri.Mid((aUri.Length()-aHost.Length())-1, aHost.Length()) );
		}
	else
		{
		aUri.Append(aHost);
		aHost.Set(aUri.Right(aHost.Length()));
		}
	
	// Check for a port
	if( aPort.Ptr() )
		{
		__ASSERT_DEBUG(aUri.Length() + aPort.Length() + 1 <= aUri.MaxLength(), User::Panic(KUriPanicCategory, KUriUtilsErrBufferOverflow));
				   
		// Append delimiter
		aUri.Append(KPortDelimiter);
	
		// Append port and update the component table to use copy
		aUri.Append(aPort);
		aPort.Set(aUri.Right(aPort.Length()));
		}
	}
/**
	Templated function to set the path in a uri. The output argument aUri points to the descriptor 
	buffer into which aPath will be copied.The argument aPath is then updated to point to the copied 
	version in aUri.
						
	@warning		This function will panic with KUriErrBufferOverflow if there 
					is not enough space in the descriptor to append the path.
	@since			6.0
	@param			aUri	The descriptor pointer to buffer to be appended.
	@param			aPath	The descriptor pointer to the path component to be copied 
					and then updated.
	@pre 			The buffer pointed to by aUri should be large enough to have 
					aPath appended to it. This can be obtained using CalculateUriLength().
	@post			The uri buffer now includes a copy of aPath and aPath points to the 
					copy of the path component in aUri.
*/
template<class TPtrType, class TPtrCType>
void SetPath(TPtrType& aUri, TPtrCType& aPath)
	{
	__ASSERT_DEBUG(aUri.Length() + aPath.Length() <= aUri.MaxLength(), User::Panic(KUriPanicCategory, KUriUtilsErrBufferOverflow));

	// Append the path
	aUri.Append(aPath);

	// Update the component table
	aPath.Set(aUri.Right(aPath.Length()));
	}

/**
	Templated function to set the query in a uri. The output argument aUri points to the descriptor 
	buffer into which aQuery will be copied. The argument aQuery is then updated to point to the copied 
	version in aUri.
						
	@warning		This function will panic with KUriErrBufferOverflow if there 
					is not enough space in the descriptor to append the query and the delimiter.
	@since			6.0
	@param			aUri	The descriptor pointer to buffer to be appended.
	@param			aQuery	The descriptor pointer to the query component to be copied 
					and then updated.
	@pre 			The buffer pointed to by aUri should be large enough to have 
					aQuery appended to it. This can be obtained using CalculateUriLength().
	@post			The uri buffer now includes a copy of aQuery and aQuery points to the 
					copy of the query component in aUri.
*/
template<class TPtrType, class TPtrCType>
void SetQuery(TPtrType& aUri, TPtrCType& aQuery)
	{
	__ASSERT_DEBUG(aUri.Length() + aQuery.Length() + 1 <= aUri.MaxLength(), User::Panic(KUriPanicCategory, KUriUtilsErrBufferOverflow));

	// Append delimiter and the query
	aUri.Append(KQueryDelimiter);
	aUri.Append(aQuery);

	// Update the component table
	aQuery.Set(aUri.Right(aQuery.Length()));
	}

/**
	Templated function to set the fragment in a uri. The output argument aUri points to the descriptor 
	buffer into which aFragment will be copied. The argument aFragment is then updated to point to the 
	copied version in aUri.
						
	@warning		This function will panic with KUriErrBufferOverflow if there 
					is not enough space in the descriptor to append the fragment and the delimiter.
	@since			6.0
	@param			aUri		The descriptor pointer to buffer to be appended.
	@param			aFragment	The descriptor pointer to the fragment component
					to be copied and then updated.
	@pre 			The buffer pointed to by aUri should be large enough to have 
					aFragment appended to it. This can be obtained using CalculateUriLength().
	@post			The uri buffer now includes a copy of aFragment and aFragment points 
					to the copy of the fragment component in aUri.
*/
template<class TPtrType, class TPtrCType>
void SetFragment(TPtrType& aUri, TPtrCType& aFragment)
	{
	__ASSERT_DEBUG(aUri.Length() + aFragment.Length() + 1 <= aUri.MaxLength(), User::Panic(KUriPanicCategory, KUriUtilsErrBufferOverflow));

	// Append delimiter and the fragment
	aUri.Append(KFragmentDelimiter);
	aUri.Append(aFragment);

	// Update the component table
	aFragment.Set(aUri.Right(aFragment.Length()));
	}

/**
	Forms the resolved path. Checks to see if the base query needs to be used in the resolved uri. 
	The pointer to the resolved path is left on the cleanup stack.
						
	@since			6.0
	@param			aBaseUri		The base uri.
	@param			aRefUri			The reference uri.
	@param			aUseBaseQuery	An output argument specifying whether the base
					query should be used in the resolved uri.
	@return			A pointer to a buffer that contains the resolved path.
*/
template<class HBufCType, class TUriCType>
HBufCType* FormResolvedPathLC(const TUriCType& aBaseUri, const TUriCType& aRefUri, TBool& aUseBaseQuery)
	{
	HBufCType* resolvedPath = NULL;
	if( !aRefUri.IsPresent(EUriScheme) && !aRefUri.IsPresent(EUriHost) && !aRefUri.Extract(EUriPath).Length() && !aRefUri.IsPresent(EUriQuery) )
		{
		// Ref is just a fragment
		aUseBaseQuery = ETrue;
		resolvedPath = aBaseUri.Extract(EUriPath).AllocLC();
		}
	else if( aRefUri.IsPresent(EUriHost) )
		{
		// Ref is a network path
		resolvedPath = aRefUri.Extract(EUriPath).AllocLC();
		}
	else
		{
		// Need to some path resolving...
		resolvedPath = ResolvePathsL(aBaseUri.Extract(EUriPath), aRefUri.Extract(EUriPath));
		CleanupStack::PushL(resolvedPath);
		}
	return resolvedPath;
	}

/**
	Cleans up a resolved path. This deals with occurences of '.' and '..' where these are complete 
	path segments.
						
	@since			6.0
	@param			aResolvedPath	The delimited data object that contains the 
					resolved path.
	@pre 			The input/output argument contains the path to be cleaned.
	@post			The resolved path has had all the occurences of '.' and '..' 
					processed and has been updated to contain the cleaned path.
 */
template<class TPtrCType, class CDelimitedDataBaseType>
void CleanResolvedPathL(CDelimitedDataBaseType* aResolvedPath)
	{
	// Create a modifiable path object for resolved path
	aResolvedPath->Parse();

	TBool done = EFalse;
	while( !done )
   		{
   		// Get the next segment
   		TPtrCType segment;
   		TInt more = aResolvedPath->Parser().Peek(segment);

   		if( more == KErrNotFound )
   			{
   			// No more segments - done
   			done = ETrue;
   			}
   		else if( IsParentDir(segment) )
   			{
   			// Found a '..' - remove '..' from path, and remove previous segment
			aResolvedPath->RemoveCurrentL();
			if( aResolvedPath->Parser().Dec() == KErrNotFound )
				{
				// No previous directory - put back '..' and stop
				InsertParentDirL(aResolvedPath);
				done = ETrue;
				}
			else
				{
				// Remove the parent directory
				aResolvedPath->RemoveCurrentL();
				if( aResolvedPath->Parser().Eos() )
					{
					// '..' is the last segment - add a '/' to the path (add empty segment)
					aResolvedPath->AddBackDelimiterL();
					done = ETrue;
					}
				}
			}
   		else if( IsSameDir(segment) )
   			{
   			// Found a '.' - remove -.- from the path
			aResolvedPath->RemoveCurrentL();
			if( aResolvedPath->Parser().Eos() )
				{
				// '..' is the last segment - add a '/' to the path (add empty segment)
				aResolvedPath->AddBackDelimiterL();
				done = ETrue;
				}
			}
   		else
   			{
   			// Segment wasn't '.' or '..' - parse to next segment
   			aResolvedPath->Parser().Inc();
   			}
   		}
	}

/**
	Forms the resolved uri. Sets the components for the resolved uri from those in the base uri and 
	the reference uri. The resolved path is given by the input argument aResolvedPath
						
	@since			6.0
	@param			aComponent		The array of components to be set for the resolved uri.
	@param			aBaseUri		The base uri.
	@param			aRefUri			The reference uri.
	@param			aResolvedPath	The resolved path.
	@param			aUseBaseQuery	A boolean indicating whether the base query 
					should be used.
*/
template<class TPtrCType, class TUriCType, class HBufCType>
void FormResolvedUri(TPtrCType aComponent[], const TUriCType& aBaseUri, const TUriCType& aRefUri, const HBufCType* aResolvedPath, TBool aUseBaseQuery)
	{
	// Scheme...
	if( aBaseUri.IsPresent(EUriScheme) )
		{
		// Use the base scheme
		aComponent[EUriScheme].Set(aBaseUri.Extract(EUriScheme));
		}
	// Authority
	if( aRefUri.IsPresent(EUriHost) )
		{
		// Use the ref host, userinfo and port - must set host first
		aComponent[EUriHost].Set(aRefUri.Extract(EUriHost));
		aComponent[EUriUserinfo].Set(aRefUri.Extract(EUriUserinfo));
		aComponent[EUriPort].Set(aRefUri.Extract(EUriPort));
		}
	else if( aBaseUri.IsPresent(EUriHost) )
		{
		// Use the base host, userinfo and port - must set host first
		aComponent[EUriHost].Set(aBaseUri.Extract(EUriHost));
		aComponent[EUriUserinfo].Set(aBaseUri.Extract(EUriUserinfo));
		aComponent[EUriPort].Set(aBaseUri.Extract(EUriPort));
		}
	// Path...
	aComponent[EUriPath].Set(*aResolvedPath);

	// Query...
	if( aUseBaseQuery && aBaseUri.IsPresent(EUriQuery) )
		{
		// Use the query from the base
		aComponent[EUriQuery].Set(aBaseUri.Extract(EUriQuery));
		}
	else if( aRefUri.IsPresent(EUriQuery) )
		{
		// Use the query from the ref
		aComponent[EUriQuery].Set(aRefUri.Extract(EUriQuery));
		}
	// Fragment
	if( aRefUri.IsPresent(EUriFragment) )
		{
		// Use the fragment from the ref
		aComponent[EUriFragment].Set(aRefUri.Extract(EUriFragment));
		}
	}

//
//
// Implemetation of LOCAL functions
//
//

/**
	Function used to resolve a base path (aBasePath) against a reference path (aRefPath),
	as described by RFC2396.
						
	@since			6.0
	@param			aBasePath	A descriptor reference to the base path.
	@param			aRefPath	A descriptor reference to the reference path.
	@return			A pointer to a buffer containing the resolve path.
	@leave			KUriErrBadBasePath if the base path is not an absolute path and not empty.
*/
HBufC8* ResolvePathsL(const TDesC8& aBasePath, const TDesC8& aRefPath)
	{
	TInt refLength = aRefPath.Length();
	if( refLength && aRefPath[0] == KSlashDelimiter )
		{
		// Relative path is absolute - that is the resolved path
		return aRefPath.AllocL();
		}
	// Ok got work to do - base path must be absolute (check 1st char) or empty
	if( aBasePath.Length() && aBasePath[0] != KSlashDelimiter )
		{
		// Base path not empty and not abosolute - bad base path
		User::Leave(KUriUtilsErrBadBasePath);
		}
	// Create a modifiable path object for resolved path
	CDelimitedPath8* resolvedPath = CDelimitedPath8::NewLC(aBasePath);

	// Check for empty ref path - use all of base path if empty
	if( refLength )
		{
		// Not empty - ensure that base path's last segment is removed and add reference
		resolvedPath->PopBackL();
		resolvedPath->PushBackL(aRefPath);
		}
	// Clean up the path to resolve occurences of '..' and '.' - parser path first
	CleanResolvedPathL<TPtrC8>(resolvedPath);

	// Return pointer to HBufC with path
	HBufC8* path = resolvedPath->Parser().Des().AllocL();
	CleanupStack::PopAndDestroy(resolvedPath);
	return path;
	}

/**
	Function used to resolve a base path (aBasePath) against a reference path (aRefPath), 
	as described by RFC2396.
						
	@since			6.0
	@param			aBasePath	A descriptor reference to the base path.
	@param			aRefPath	A descriptor reference to the reference path.
	@return			A pointer to a buffer containing the resolve path.
	@leave			KUriErrBadBasePath if the base path is not an absolute path and not empty.
*/
HBufC16* ResolvePathsL(const TDesC16& aBasePath, const TDesC16& aRefPath)
	{
	TInt refLength = aRefPath.Length();
	if( refLength && aRefPath[0] == KSlashDelimiter )
		{
		// Relative path is absolute - that is the resolved path
		return aRefPath.AllocL();
		}
	// Ok got work to do - base path must be absolute (check 1st char) or empty
	if( aBasePath.Length() && aBasePath[0] != KSlashDelimiter )
		{
		// Base path not empty and not abosolute - bad base path
		User::Leave(KUriUtilsErrBadBasePath);
		}
	// Create a modifiable path object for resolved path
	CDelimitedPath16* resolvedPath = CDelimitedPath16::NewLC(aBasePath);

	// Check for empty ref path - use all of base path if empty
	if( refLength )
		{
		// Not empty - ensure that base path's last segment is removed and add reference
		resolvedPath->PopBackL();
		resolvedPath->PushBackL(aRefPath);
		}
	// Clean up the path to resolve occurences of '..' and '.' - parser path first
	CleanResolvedPathL<TPtrC16>(resolvedPath);

	// Return pointer to HBufC with path
	HBufC16* path = resolvedPath->Parser().Des().AllocL();
	CleanupStack::PopAndDestroy(resolvedPath);
	return path;
	}
	
/**
	Checks if the segment is '.' (8-bit version).
	
	@since			6.0
	@param			aSegment	A descriptor with the segment to check.
	@return			A boolean value of ETrue if the segment is '.', EFalse if not.
*/
TBool IsSameDir(const TDesC8& aSegment)
	{
	_LIT8(KSameDir, ".");
	return (aSegment.Compare(KSameDir) == 0);
	}
	
/**
	Checks if the segment is '.' (16-bit version).
	
	@since			6.0
	@param			aSegment	A descriptor with the segment to check.
	@return			A boolean value of ETrue if the segment is '.', EFalse if not.
*/
TBool IsSameDir(const TDesC16& aSegment)
	{
	_LIT16(KSameDir, ".");
	return (aSegment.Compare(KSameDir) == 0);
	}
	
/**
	Checks if the segment is '..' (8-bit version).
	
	@since			6.0
	@param			aSegment	A descriptor with the segment to check.
	@return			A boolean value of ETrue if the segment is '..', EFalse if not.
*/
TBool IsParentDir(const TDesC8& aSegment)
	{
	_LIT8(KParentDir, "..");
	return (aSegment.Compare(KParentDir) == 0);
	}

/**
	Checks if the segment is '..' (16-bit version).
	
	@since			6.0
	@param			aSegment	A descriptor with the segment to check.
	@return			A boolean value of ETrue if the segment is '..', EFalse if not.
*/
TBool IsParentDir(const TDesC16& aSegment)
	{
	_LIT16(KParentDir, "..");
	return (aSegment.Compare(KParentDir) == 0);
	}

/**
	Inserts the segment '..' at the current parsed position (8-bit version).
						
	@since			6.0
	@param			aResolvedPath	The delimited data object to have the segment
					inserted.
	@pre			The delimited data object must be parsed to the position where 
					the segment is to be inserted.
	@post			The segment '..' has been inserted at the current position.
*/
void InsertParentDirL(CDelimitedDataBase8* aResolvedPath)
	{
	_LIT8(KParentDir, "..");
	aResolvedPath->InsertCurrentL(KParentDir);
	}
	
/**
	Inserts the segment '..' at the current parsed position (16-bit version).
						
	@since			6.0
	@param			aResolvedPath	The delimited data object to have the segment
					inserted.
	@pre			The delimited data object must be parsed to the position where 
					the segment is to be inserted.
	@post			The segment '..' has been inserted at the current position.
*/
void InsertParentDirL(CDelimitedDataBase16* aResolvedPath)
	{
	_LIT16(KParentDir, "..");
	aResolvedPath->InsertCurrentL(KParentDir);
	}
	

//
//
// File URI Implementation - CUri8
//
//

/**
	Initializes the file URI components (scheme, empty hostname and path).	 
	
	It uses GenerateFileUriPathL() to generate a file Uri path using the filename and drive. 
	
	@since			9.1
	@param			aFileName	A reference to a filename 
	@param			aDrive		A drive number. This is a TFileUriFlags value.
	@param			aFlags		A flag to indicate if the file exists on a fixed drive or removable media drive.
					This is a TFileUriFlags value.
	@pre 			Object fully constructed
	@post			The object is initialized with file URI components.
 */
void CUri8::InitializeFileUriComponentsL(const TDesC& aFileName, TDriveNumber aDrive, TUint aFlags)
	{
	HBufC* uriPath16 = GenerateFileUriPathL(aFileName, aDrive, aFlags);
	CleanupStack::PushL(uriPath16);
	HBufC8* uriPath = EscapeUtils::ConvertFromUnicodeToUtf8L(*uriPath16);
	CleanupStack::PopAndDestroy(uriPath16);
	CleanupStack::PushL(uriPath);
	HBufC8* escpedUriPath = EscapeUtils::EscapeEncodeL(*uriPath, EscapeUtils::EEscapeNormal);
	CleanupStack::PopAndDestroy(uriPath);
	CleanupStack::PushL(escpedUriPath);
	
	//SetComponent is not used in order to increase efficiency, by avoiding overhead of length calculation, 
	//tmp buffer allocation and updation of internal uri object, internal buffer & descriptor pointer 
	//for each SetComponent call     
	iUri.iComponent[EUriPath].Set(*escpedUriPath);
	iUri.iComponent[EUriHost].Set(KNullDesC8);
	iUri.iComponent[EUriScheme].Set(KFileUriScheme8);
	FormUriL();
	
	CleanupStack::PopAndDestroy(escpedUriPath);
	}

/**
	Allocates and constructs a file URI object for a specified file.
	 
	- If the file exists on a fixed drive, then the file URI takes the form: 'file://\<drive-letter\>/\<filepath including filename\>'.	
	- If the file exists on a removable media drive, then the file URI takes the form: 'file://ext-media/\<filepath including filename\>'.
	
	@since			9.1
	@param			aFullFileName	A reference to a fully qualified filename 
	@param			aFlags			A flag to indicate if the file exists on a fixed drive or removable media drive.
									This is a TFileUriFlags value.
	@return			A pointer to the newly created file URI (CUri8) object. 
	@post			A fully constructed and initialized file URI (CUri8) object.
 */
EXPORT_C CUri8* CUri8::CreateFileUriL(const TDesC& aFullFileName, TUint aFlags)
	{
	//It should be called to construct a file URI for a public file stored on a fix drive 
	// or on a removable media drive only
	__ASSERT_ALWAYS( ((aFlags == 0) || (aFlags & EExtMedia)), User::Panic(KFileUriPanicCategory, KUriUtilsCannotConvert) );
		
	CUri8* self = CUri8::NewLC();
	self->InitializeFileUriComponentsL(aFullFileName, EDriveA, aFlags);
	CleanupStack::Pop(self);
	return self;
	}

/**
	Allocates and constructs a file URI object for a file that is private to the application.

	- If the file exists on a fixed drive, then the file URI takes the form 'file://private/\<drive-letter\>/<filepath including filename\>'.
	- If the file exists on a removable media drive, then the file URI takes the form 'file://private/ext-media/\<filepath including filename\>'.
	
	@since			9.1
	@param			aRelativeFileName	A reference to the filename relative to the application's private directory.
	@param			aDrive				Drive number, if the private file stored on fixed drive, otherwise not used
										This is a TDriveNumber value
	@param			aFlags				A flag to indicate if the file exists on a fixed drive or removable media drive.
										This is a TFileUriFlags value.
	@return			A pointer to the newly created file URI (CUri8) object. 
	@post			A fully constructed and initialized file URI (CUri8) object.
*/	


EXPORT_C CUri8* CUri8::CreatePrivateFileUriL(const TDesC& aRelativeFileName, TDriveNumber aDrive, TInt aFlags) 
	{
	//It should be called to construct a file URI for the application's private file stored on a fix drive 
	// or on a removable media drive only
	__ASSERT_ALWAYS( (((aFlags == 0) || (aFlags & EExtMedia)) && (aDrive >= EDriveA && aDrive <= EDriveZ)), User::Panic(KFileUriPanicCategory, KUriUtilsCannotConvert) );

	CUri8* self = CUri8::NewLC();
	self->InitializeFileUriComponentsL(aRelativeFileName, aDrive, aFlags|EPrivate);
	CleanupStack::Pop(self);
	return self;
	}


//
//
// File URI Implementation - CUri16
//
//

/**
	Initializes the file URI components (scheme, empty hostname and path).	 
	
	It uses GenerateFileUriPathL() to generate a file Uri path using the filename and drive. 
	
	@since			9.1
	@param			aFileName	A reference to a filename 
	@param			aDrive		A drive number. This is a TFileUriFlags value.
	@param			aFlags		A flag to indicate if the file exists on a fixed drive or removable media drive.
					This is a TFileUriFlags value.
	@pre 			Object fully constructed
	@post			The object is initialized with file URI components.
 */
void CUri16::InitializeFileUriComponentsL(const TDesC& aFileName, TDriveNumber aDrive, TUint aFlags)
	{	
	HBufC* uriPath = GenerateFileUriPathL(aFileName, aDrive, aFlags);
	CleanupStack::PushL(uriPath);

	HBufC8* uriPath8 = EscapeUtils::ConvertFromUnicodeToUtf8L(*uriPath);
	CleanupStack::PopAndDestroy(uriPath);
	CleanupStack::PushL(uriPath8);

	HBufC8* escapedUriPath8 = EscapeUtils::EscapeEncodeL(*uriPath8, EscapeUtils::EEscapeNormal);
	CleanupStack::PopAndDestroy(uriPath8);
	CleanupStack::PushL(escapedUriPath8);

	HBufC* escapedUriPath = HBufC::NewLC(escapedUriPath8->Length());
	escapedUriPath->Des().Copy(*escapedUriPath8); 
	
	//SetComponent is not used in order to increase efficiency, by avoiding overhead of length calculation, 
	//tmp buffer allocation and updation of internal uri object, internal buffer & descriptor pointer 
	//for each SetComponent call     
	iUri.iComponent[EUriPath].Set(*escapedUriPath);
	iUri.iComponent[EUriHost].Set(KNullDesC16);
	iUri.iComponent[EUriScheme].Set(KFileUriScheme16);
	FormUriL();
	
	CleanupStack::PopAndDestroy(escapedUriPath);
	CleanupStack::PopAndDestroy(escapedUriPath8);
	}

/**
	Allocates and constructs a file URI object for a specified file.
	 
	- If the file exists on a fixed drive, then the file URI takes the form: 'file://\<drive-letter\>/\<filepath including filename\>'.	
	- If the file exists on a removable media drive, then the file URI takes the form: 'file://ext-media/\<filepath including filename\>'.
	
	@since			9.1
	@param			aFullFileName	A reference to a fully qualified filename 
	@param			aFlags	A flag to indicate if the file exists on a fixed drive or removable media drive.
					This is a TFileUriFlags value.
	@return			A pointer to the newly created file URI (CUri16) object. 
	@post			A fully constructed and initialized file URI (CUri16) object.
 */
EXPORT_C CUri16* CUri16::CreateFileUriL(const TDesC& aFullFileName, TUint aFlags)
	{
	//It should be called to construct a file URI for a public file stored on a fix drive 
	// or on a removable media drive only
	__ASSERT_ALWAYS( ((aFlags == 0) || (aFlags & EExtMedia)), User::Panic(KFileUriPanicCategory, KUriUtilsCannotConvert) );
		
	CUri16* self = CUri16::NewLC();
	self->InitializeFileUriComponentsL(aFullFileName, EDriveA, aFlags);
	CleanupStack::Pop(self);
	return self;
	}
	
/**
	Allocates and constructs a file URI object for a file that is private to the application.

	- If the file exists on a fixed drive, then the file URI takes the form 'file://private/\<drive-letter\>/<filepath including filename\>'.
	- If the file exists on a removable media drive, then the file URI takes the form 'file://private/ext-media/\<filepath including filename\>'.
	
	@since			9.1
	@param			aRelativeFileName	A reference to the filename relative to the application's private directory.
	@param			aDrive				Drive number, if the private file stored on fixed drive, otherwise not used
										This is a TDriveNumber value
	@param			aFlags				A flag to indicate if the file exists on a fixed drive or removable media drive.
										This is a TFileUriFlags value.
	@return			A pointer to the newly created file URI (CUri16) object. 
	@post			A fully constructed and initialized file URI (CUri16) object.
*/


EXPORT_C CUri16* CUri16::CreatePrivateFileUriL(const TDesC& aRelativeFileName, TDriveNumber aDrive, TInt aFlags) 
	{
	//It should be called to construct a file URI for the application's private file stored on a fix drive 
	// or on a removable media drive only
	__ASSERT_ALWAYS( (((aFlags == 0) || (aFlags & EExtMedia)) && (aDrive >= EDriveA && aDrive <= EDriveZ)), User::Panic(KFileUriPanicCategory, KUriUtilsCannotConvert) );

	CUri16* self = CUri16::NewLC();
	self->InitializeFileUriComponentsL(aRelativeFileName, aDrive, aFlags|EPrivate);
	CleanupStack::Pop(self);
	return self;
	}


//
//
// Implemetation of LOCAL functions for the File URI
//
//
	
/**
	Function used to generate 16bit file uri using 1st parameter aFileName and 
	2nd parameter aDrive for the application's private or a public file.
	
	This is called by API CreateFileUri() and CreatePrivateFileUri()to 
	generate a filename.
	
	Note: The space allocated for the returned descriptor will likely be larger 
		  than the length of the descriptor
	
	@leave			KErrBadName	A provided Drivename or filename is not valid 
	@since			9.1
	@param			aFileName	A descriptor reference to the filename.
	@param			aDrive		A descriptor reference to drive letter.
	@param			aFlags		A flag to indicate the private or a public file exists on removable media or a fixed drive.
	@return			A pointer to a buffer containing the resolved fully qualified filename.
 */
HBufC* GenerateFileUriPathL(const TDesC& aFileName, TDriveNumber aDrive,  TUint aFlags)
	{
	TInt origLength = aFileName.Length();

	//Leaves with KErrBadName if filename length is out of range 
	if (origLength == 0 || origLength > KMaxFileName)
		{
		User::Leave(KErrBadName);	
		}
		
	TPtrC filename(aFileName);
	
	//extract drive letter and remove drive "x:" from filename
	TUint drive = filename[0];
	
	 // hasDrive means it begins with a drive, e.g. "c:"
	 const TBool hasDrive =  ((drive >= 'A' && drive <= 'Z') || (drive >= 'a' && drive <= 'z')) && (filename[1] == KDriveSeparator);
	 // hasTopPath means it begins with a \ (possibly after the drive)
	 const TBool hasTopPath = (hasDrive && (filename[2] == KFilePathSeparator)) || (!hasDrive && (drive == TUint(KFilePathSeparator) ));
 
	 TInt skip = KDefaultPath().Length(); //  skip leading "<drive>:\" by default
	 if(aFlags & TUint(EPrivate)) 
		{
		skip = (hasDrive ? (KDefaultPath().Length() - 1) : 0) + (hasTopPath ? 1 : 0) ;
		}
	 else
		{
		// if not private then it should have valid drive i.e. "<drive>:\"
		if (!(hasDrive && hasTopPath)) 
			{
			User::Leave(KErrBadName);
			}
		}
	
	if(skip)
		  {
		  filename.Set(aFileName.Right(origLength - skip));
		  } 	

	TInt uriLen = aFileName.Length() + KExtMedia().Length() + KPrivate().Length() + 1 /* for drive letter */;
	
	HBufC* fileUri = HBufC::NewLC(uriLen);
	TPtr fileUriPtr = fileUri->Des(); 
	fileUriPtr.Append(KUriPathSeparator);
	
	if (aFlags & TUint(EPrivate))
		{
		fileUriPtr.Append(KPrivate);
		drive = TInt16('A' + aDrive);
		}
		
	if (aFlags & EExtMedia)
		{
		fileUriPtr.Append(KExtMedia);			
		}
	else
		{
		fileUriPtr.Append(drive);
		fileUriPtr.Append(KUriPathSeparator);	
		}
		
	fileUriPtr.Append(filename);
		
	//Convert "\" to "/"
	ChangePathSeparator(fileUriPtr, KFilePathSeparator, KUriPathSeparator);

	//Handling "./" and "../" in the file URI path  or resolving the URI path 
	CDelimitedPath16* resolvedPath = CDelimitedPath16::NewLC(fileUriPtr);
 	// Clean up the path to resolve occurences of '..' and '.'
 	CleanResolvedPathL<TPtrC>(resolvedPath);
 	fileUriPtr.Copy(resolvedPath->Parser().Des()); // new path will always be shorter than old one
 	CleanupStack::PopAndDestroy(resolvedPath);
 		
	CleanupStack::Pop(fileUri);
	return fileUri;
	}
