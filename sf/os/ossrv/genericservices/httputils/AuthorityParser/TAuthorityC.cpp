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
#include <uriutilscommon.h>
#include <escapeutils.h>

// Panic category
//
_LIT(KAuthorityPanicCategory,"AUTHORITY"); 

//
//
// Implementation of TAuthorityC8
//
//

/**
	Constructor.
	
	@since			6.0
 */
EXPORT_C TAuthorityC8::TAuthorityC8()
	{
	// Reset the component table and the Authority
	Reset();
	}

/**
	Retrieves the specified component in the authority.
	
	@since			6.0
	@param			aComponent	The enum specifying the component.
	@return			A constant reference to a descriptor pointer to the specified component.
 */
EXPORT_C const TDesC8& TAuthorityC8::Extract(TAuthorityComponent aComponent) const
	{
	__ASSERT_ALWAYS(aComponent<EAuthorityMaxComponents && aComponent >EAuthorityComplete, User::Panic(KAuthorityPanicCategory(), KUriUtilsErrBadComponentIndex));

	return iComponent[aComponent];
	}

/**
	Indicates whether the specified component is present in the authority.
	
	@since			6.0
	@param			aComponent	The enum specifying the component.
	@return			A boolean value of ETrue if the desired component is present, 
	or EFalse if the desired component is not present.
 */
EXPORT_C TBool TAuthorityC8::IsPresent(TAuthorityComponent aComponent) const
	{
	__ASSERT_ALWAYS(aComponent<EAuthorityMaxComponents && aComponent >EAuthorityComplete, User::Panic(KAuthorityPanicCategory(), KUriUtilsErrBadComponentIndex));

	TBool present = TBool(iComponent[aComponent].Ptr());
	return present;
	}

/**
	Compares the specified component against the one in the authority passed in.
	
	@since			6.0
	@param			aAuthority	The authority to compare components against.
	@param			aComponent	The enum specifying the component to compare.
	@return			An integer value of zero if the components are the same, any other
	value if the components are not the same.
 */
EXPORT_C TInt TAuthorityC8::Compare(const TAuthorityC8& aAuthority, TAuthorityComponent aComponent) const
	{
	__ASSERT_ALWAYS(aComponent<EAuthorityMaxComponents && aComponent >EAuthorityComplete, User::Panic(KAuthorityPanicCategory(), KUriUtilsErrBadComponentIndex));

	// Does the component exist in both the Authoritys
	if( IsPresent(aComponent) && aAuthority.IsPresent(aComponent) )
		{
		if( aComponent == EAuthorityHost )
			{
			// Do case insensitive compare for host
			return (iComponent[aComponent].CompareF(aAuthority.iComponent[aComponent]));
			}
		else
			{
			// Do case sensitive compare for all other components
			return (iComponent[aComponent].Compare(aAuthority.iComponent[aComponent]));
			}
		}
	else
		return KErrNotFound;
	}

/**
	Retrieves the descriptor for the entire authority.
	
	@since			6.0
	@return			A const reference to a descriptor pointer to the authority.
 */
EXPORT_C const TDesC8& TAuthorityC8::AuthorityDes() const
	{
	return iAuthorityDes;
	}

/** 
	Create a new HBufC descriptor containing the desired component or the full Authority.
	
	@param	aComponent  The component to convert into Unicode (EAuthorityScheme - EAuthorityFragment)
	or the full authority (EAuthorityComplete -- the default)).
	@return the descriptor containing the desired component. 
 */
EXPORT_C HBufC* TAuthorityC8::DisplayFormL(TAuthorityComponent aComponent ) const
	{
	TPtrC8 component;
	
	if ( aComponent == EAuthorityComplete) // extract the full authority
		{
		component.Set(iAuthorityDes);
		}
	else
		{ // extract the specified component, will Panic if invalid
		component.Set( Extract(aComponent) );
		}
	// convert descriptor from UTF-8 into Unicode
	return EscapeUtils::ConvertToUnicodeFromUtf8L(component);
	}


/**
	@internalComponent
	Resets the descriptor pointers for the authority components and the authority.
	
	@since			6.0
	@post			All authority component information is removed and the authority 
	descriptor is set to NULL.
 */
void TAuthorityC8::Reset()
	{
	// Set descriptor pointers to NULL and lengths to zero
	for( TInt i=0; i<EAuthorityMaxComponents; ++i )
		{
		iComponent[i].Set(NULL,0);
		}
	iAuthorityDes.Set(NULL,0);
	}

//
//
// Implementation of TAuthorityC16
//
//

/**
	Constructor.
	
	@since			6.0
	@deprecated Deprecated in 9.1
*/
EXPORT_C TAuthorityC16::TAuthorityC16()
	{
	// Reset the component table and the Authority
	Reset();
	}

/**
	Retrieves the specified component in the authority.
	
	@since			6.0
	@deprecated Deprecated in 9.1
	@param			aComponent	The enum specifying the component.
	@return			A constant reference to a descriptor pointer to the specified component.
 */
EXPORT_C const TDesC16& TAuthorityC16::Extract(TAuthorityComponent aComponent) const
	{
	__ASSERT_ALWAYS(aComponent<EAuthorityMaxComponents && aComponent >EAuthorityComplete, User::Panic(KAuthorityPanicCategory(), KUriUtilsErrBadComponentIndex));

	return iComponent[aComponent];
	}

/**
	Indicates whether the specified component is present in the authority.
	
	@since			6.0
	@deprecated Deprecated in 9.1
	@param			aComponent	The enum specifying the component.
	@return			A boolean value of ETrue if the desired component is present, 
	or EFalse if the desired component is not present.
 */
EXPORT_C TBool TAuthorityC16::IsPresent(TAuthorityComponent aComponent) const
	{
	__ASSERT_ALWAYS(aComponent<EAuthorityMaxComponents && aComponent >EAuthorityComplete, User::Panic(KAuthorityPanicCategory(), KUriUtilsErrBadComponentIndex));

	TBool present = iComponent[aComponent].Ptr() ? ETrue : EFalse;
	return present;
	}

/**
	Compares the specified component against the one in the authority passed in.
	
	@since			6.0
	@deprecated Deprecated in 9.1
	@param			aAuthority	The authority to compare components against.
	@param			aComponent	The enum specifying the component to compare.
	@return			An integer value of zero if the components are the same, any other
	value if the components are not the same.
 */
EXPORT_C TInt TAuthorityC16::Compare(const TAuthorityC16& aAuthority, TAuthorityComponent aComponent) const
	{
	__ASSERT_ALWAYS(aComponent<EAuthorityMaxComponents && aComponent >EAuthorityComplete, User::Panic(KAuthorityPanicCategory(), KUriUtilsErrBadComponentIndex));

	// Does the component exist in both the Authoritys
	if( IsPresent(aComponent) && aAuthority.IsPresent(aComponent) )
		{
		if( aComponent == EAuthorityHost )
			{
			// Do case insensitive compare for host
			return (iComponent[aComponent].CompareF(aAuthority.iComponent[aComponent]));
			}
		else
			{
			// Do case sensitive compare for all other components
			return (iComponent[aComponent].Compare(aAuthority.iComponent[aComponent]));
			}
		}
	else
		return KErrNotFound;
	}

/**
	Retrieves the descriptor for the entire authority.
	
	@since			6.0
	@deprecated Deprecated in 9.1
	@return			A const reference to a descriptor pointer to the authority.
 */
EXPORT_C const TDesC16& TAuthorityC16::AuthorityDes() const
	{
	return iAuthorityDes;
	}
	
/** 
	Create a new HBufC descriptor containing the desired component or the full authority.
	
	@param   aComponent   The component to convert into Unicode (EAuthorityScheme - EAuthorityFragment), 
				the full authority (EAuthorityComplete -- the default), or the final segment of the path (EAuthorityTail).
	@return     the descriptor containing the desired component. 
	@deprecated     Deprecated in 9.1. Provided for compatibility.
 */
EXPORT_C HBufC* TAuthorityC16::DisplayFormL(TAuthorityComponent aComponent) const
	{
	TPtrC component;
	if ( aComponent == EAuthorityComplete) // extract the full authority
		{
		component.Set(iAuthorityDes);
		}
	else
		{ // extract the specified component, will Panic if invalid
		component.Set( Extract(aComponent) );
	}

	return component.AllocL();
	}

/**
	@internalComponent
	Resets the descriptor pointers for the authority components and the authority.
	
	@since			6.0
	@deprecated Deprecated in 9.1
	@post			All authority component information is removed and the authority 
	descriptor is set to NULL.
 */
void TAuthorityC16::Reset()
	{
	// Set descriptor pointers to NULL and lengths to zero
	for( TInt i=0; i<EAuthorityMaxComponents; ++i )
		{
		iComponent[i].Set(NULL,0);
		}
	iAuthorityDes.Set(NULL,0);
	}

