/*
* Copyright (c) 2003-2009 Nokia Corporation and/or its subsidiary(-ies).
* All rights reserved.
* This component and the accompanying materials are made available
* under the terms of the License "Eclipse Public License v1.0"
* which accompanies this distribution, and is available
* at the URL "http://www.eclipse.org/legal/epl-v10.html".
*
* Initial Contributors:
* Nokia Corporation - initial contribution.
*
* Contributors:
*
* Description: 
*
*/


/**
 @file
 @publishedPartner
 @deprecated
*/

#ifndef __BITSET_H__
#define __BITSET_H__

#ifndef REMOVE_CAF1

#include <e32base.h>
#include <caf/caftypes.h>

class RWriteStream;
class RReadStream;

namespace ContentAccess
{
	
	/** @deprecated */
	enum TCafUtilsPanics
		{
		EInvalidBit
		};

	_LIT(KCafUtilsPanic, "CafUtils");

	/**
	 * A container of bits. 
	 * 
	 * The bits may be retrieved, set, and unset.
	 *
	 * @publishedPartner
	 * @deprecated 
	 */
	class CBitset : public CBase
		{
	public:
		/**
		 * Constructs a new bitset object with a maximum number of bits.
		 * 
		 * @param aMaxBits	The maximum number of bits this object may contain.
		 */
		IMPORT_C static CBitset* NewL(TInt aMaxBits);

		/** @see CBitset::NewL(TInt aMaxBits) */
		IMPORT_C static CBitset* NewLC(TInt aMaxBits);
		
		/** 
		 * Copy constructor
		 *
		 * @param aSource	The object to be copied from.
		 */
		IMPORT_C static CBitset* NewL(const CBitset& aSource);

		/** @see CBitset::NewL(const CBitset& aSource) */
		IMPORT_C static CBitset* NewLC(const CBitset& aSource);

		virtual ~CBitset();

	public:
		/** Resets the entire bitset to zero. */
		IMPORT_C void Reset();

		/** Gets the maximum number of settable/gettable bits. */
		IMPORT_C TInt MaxBits() const;

		/** Sets all bits in the set to one. */
		IMPORT_C void SetAll();

		/** Inverts all bits in the set. */
		IMPORT_C void Invert();
		
		/** Sets the 'aBit'th bit of the set. 
		* @param aBit the bit to set 
		* @panic CafUtils 0 if aBit is out of range
		*/
		IMPORT_C void Set(TInt aBit);

		/** Clears the 'aBit'th bit of the set. 
		* @param aBit the bit to clear
		* @panic CafUtils 0 if aBit is out of range
		*/
		IMPORT_C void Unset(TInt aBit);
		
		/** 
		 * Queries the single bit of the set.
		 * 
		 * @param aBit	The bit that requires testing.
		 * @return		ETrue if the bit was set, EFalse otherwise.
 		 * @panic CafUtils 0 if aBit is out of range
		 * 
		 */
		IMPORT_C TBool IsSet(TInt aBit) const;

		/** 
		 * Assignment of a bitset. 
		 * 
		 * Note that an assignment of a longer bitset to a
		 * shorter bitset will result in the shorter bitset
		 * being expanded. This function may leave.
		 *
		 * @param aSource	The bitset ......
		 * @return			A bitset.
		 */
		IMPORT_C CBitset& operator=(const CBitset& aSource);

		/**
		 * Allows comparisons of CBitsets. 
		 * 
		 * Note that it is possible to compare a longer
		 * bitset with a shorter one. However, only the 
		 * relevant (low-order) bits are compared.
		 *
		 * @param aSource	The bitset .....
		 * @return			ETrue if the ....., EFalse otherwise.
		 */
		IMPORT_C TBool operator==(const CBitset& aSource) const;
		inline TBool operator!=(const CBitset& aSource) const;

		/** 
		 * Externalisation function that allows this object to be
		 * marshalled prior to being shipped over a client-server
		 * interface by related glue-code.
		 *
		 * @param aStream	On return, the .....
		 */
		IMPORT_C void ExternalizeL(RWriteStream& aStream) const;
		
		/**
		 * Internalisation function that allows this object to to be
		 * unmarshalled after transfer across a client-server
		 * interface. 
		 *
		 * @param aStream	On return, the .....
		 */
		IMPORT_C void InternalizeL(RReadStream& aStream);

		/**
		 * Allows a caller to set multiple bits in the bitset
		 * by allowing a variable-length parameter list. 
		 * 
		 * @param aItems	The number of items following in the
		 * 					parameter list.
		 */
		IMPORT_C void SetListL(TInt aItems, ...);

		/**
		 * Allows a caller to query multiple bits in the bitset.
		 * 
		 * @param aItems	The number of items following in the
		 * 					parameter list.
		 * @return			ETrue if all of the bits specified were set. EFalse if
		 * 					one or more were unset.
		 */
		IMPORT_C TBool IsSetList(TInt aItems, ...) const;

	private:
		void ConstructL(TInt aMaxId);
		void ConstructL(const CBitset& aSource);
		CBitset();
		CBitset(const CBitset& aSource);

		/**
		 * Converts from a bit ID to a byte and 
		 * bit within one of the bit-field sets.
		 * 
		 * @param aBitId		The bit ID being mapped.
		 * @param aByte			Modified to contain the resultant byte number.
		 * @param aBitInByte	Modified to contain the resultant bit number
		 * 						within the byte.
		 */
		void IdentifyBit(TInt aBitId, TInt& aByte, TInt& aBitInByte) const;
		
		/** 
		* Tests whether the bit value is valid for this bitset.
		*
		* @param aAttributeId	The .....
		* @return				ETrue if the bit value is valid for this bitset, EFalse otherwise.
		*/
		inline TBool ValidBit(TInt aAttributeId) const;
		
	private:
		/** The number of bytes required to allow for maxbits */
		TInt iWidth; 

		/** The maximum number of bits in this bitset */
		TInt iMaxBits;

		/** The actual set of bits */
		TUint8* iBitSet;
		};

}

inline TBool ContentAccess::CBitset::operator!=(const CBitset& aSource) const
	{
	return !operator==(aSource);
	}

inline TBool ContentAccess::CBitset::ValidBit(TInt aBit) const
	{
	return (aBit >= 0) && (aBit < iMaxBits);
	}

#endif // REMOVE_CAF1

#endif // __BITSET_H__
