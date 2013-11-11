/*
* Copyright (c) 2006-2009 Nokia Corporation and/or its subsidiary(-ies).
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
* crypto parameters interface
*
*/


/**
 @file
 @publishedAll
 @released 
*/

#ifndef __CRYPTOPARAMS_H__
#define __CRYPTOPARAMS_H__

#include <bigint.h>

namespace CryptoSpi
	{
	/**
	Abstract base class for generic crypto parameters to enable plug-in 
	specific parameters and the externalisation of plug-ins. The type
	of the sub-class is identified by the GetType method.

	All sub-class contain copies of (instead of references to) the 
	supplied values.
	*/
	NONSHARABLE_CLASS(CCryptoParam) : public CBase
		{
	public:

		/**
		The definition of the data type of the embedded value. 
		Other data types may be added in future so applications 
		should not panic if the type is not recognised.
		*/
		enum TParamType
			{
			/** 
			RCryptoIntParam 
			*/
			EInt = 1,
			/** 
			RCryptoBigIntParam 
			*/
			EBigInt = 2,
			/** 
			RCryptoDesC8Param 
			*/
			EDesC8 = 3,
			/** 
			RCryptoDesC16Param 
			*/
			EDesC16 = 4,
			};
				
		/**
		Returns the data type of the crypto parameter
		@return The data type of the crypto parameter
		*/
		IMPORT_C TInt Type() const;
		
		/**
		Returns the Uid of the crypto parameter
		@return The Uid of the crypto parameter
		*/
		IMPORT_C TUid Uid() const;
		
		/** 
		Destructor 
		*/
		IMPORT_C ~CCryptoParam();
		
	protected:
		/**
		 * @internalComponent
		 * 
		 * Constructor 
		 * @param aType	The data type identifier for the sub-class.
		 * @param aUid	The Uid of the cryptoparam
		 */
		CCryptoParam(TParamType aType, TUid aUid);

	protected:
		/**
		The data type of the embedded value
		*/
		TParamType iType;
		
		/**
		The Uid of the crypto parameter
		*/
		TUid iUid;
		};

	/**
	CryptoParam class that wraps a TInt
	*/
	NONSHARABLE_CLASS(CCryptoIntParam) : public CCryptoParam
		{
	public: 
		/**
		Factory method for allocating a new CCryptoIntParam object
		that contains a copy of the supplied TInt
		@param aValue The TInt to be wrapped (copied)
		@param aUid The UID of the CryptoParam
		@return A pointer to a CCryptoIntParam instance
		*/
		IMPORT_C static CCryptoIntParam* NewL(TInt aValue, TUid aUid);

		/**
		Factory method for allocating a new CCryptoIntParam object
		that contains a copy of the supplied TInt
		Leaves the pointer of the CryptoParam on the cleanup stack
		@param aValue The TInt to be wrapped (copied)
		@param aUid The UID of the CryptoParam
		@return A pointer to a CCryptoIntParam instance
		*/
		IMPORT_C static CCryptoIntParam* NewLC(TInt aValue, TUid aUid);

		/**
		Returns the embedded value.
		@return The embedded integer
		*/
		IMPORT_C TInt Value() const;

		/**
		Destructor
		*/
		IMPORT_C ~CCryptoIntParam();
		
	private:
		/**
		Constructor
		@param aValue The integer to wrap
		@param aUid	The UID of the crypto parameter
		*/		
		CCryptoIntParam(TInt aValue, TUid aUid);

	private:
		/** 
		The embedded value 
		*/
		TInt iValue;
		};

	/**
	Crypto param class the wraps an RInteger
	 */
	NONSHARABLE_CLASS(CCryptoBigIntParam) : public CCryptoParam
		{
	public: 
		/**
		Factory method for allocating a new CCryptoBigIntParam object
		that contains a copy of the supplied TInteger object.
		@param aValue	The TInteger to be wrapped (copied)
		@param aUid	The UID of the CryptoParam
		@return A pointer to a CCryptoBigIntParam instance
		*/
		IMPORT_C static CCryptoBigIntParam* NewL(const TInteger& aValue, TUid aUid);
		
		/**
		Factory method for allocating a new CCryptoBigIntParam object
		that contains a copy of the supplied TInteger object.
		Leaves the pointer of the CryptoParam onto the cleanup stack.
		@param aValue	The TInteger to be wrapped (copied)
		@param aUid	The UID of the CryptoParam
		@return A pointer to a CCryptoBigIntParam instance
		*/		
		IMPORT_C static CCryptoBigIntParam* NewLC(const TInteger& aValue, TUid aUid);

		/**
		Returns the embedded value.
		@return A reference to the embedded TInteger
		*/
		IMPORT_C const TInteger& Value() const;

		/** 
		Destructor 
		*/
		IMPORT_C ~CCryptoBigIntParam();
		
	private:
		/**
		Constructor
		*/
		CCryptoBigIntParam();
		
		/**
		Constructor
		@param aUid	The UID of the crypto parameter
		*/				
		CCryptoBigIntParam(TUid aUid);
		
		/**
		Second Phase Constructor
		@param aValue	The TInteger to wrap
		*/		
		void ConstructL(const TInteger& aValue);
	
	private:
		/**
		The copied RInteger
		*/
		RInteger iValue;
		};
	
	/**
	Crypto param class that wraps an 8-bit constant descriptor
	*/
	NONSHARABLE_CLASS(CCryptoDesC8Param) : public CCryptoParam
		{
	public:
		/**
		Factory method for allocating a new CCryptoDesC8Param object
		that contains a copy of the supplied RInteger object.
		@param aValue The TDesC* to be wrapped (copied)
		@param aUid	The Uid of the CryptoParam
		@return A pointer to a CCryptoDesC8Param instance
		*/
		IMPORT_C static CCryptoDesC8Param* NewL(const TDesC8& aValue, TUid aUid);

		/**
		Factory method for allocating a new CCryptoDesC8Param object
		that contains a copy of the supplied RInteger object.
		Leaves the pointer of the CryptoParam on the cleanup stack
		@param aValue The TDesC* to be wrapped (copied)
		@param aUid	The Uid of the CryptoParam
		@return A pointer to a CCryptoDesC8Param instance
		*/
		IMPORT_C static CCryptoDesC8Param* NewLC(const TDesC8& aValue, TUid aUid);

		/**
		Returns the embedded value.
		@return A reference to the embedded TDesC8
		*/
		IMPORT_C const TDesC8& Value() const;

		/**
		Destructor
		*/
		IMPORT_C ~CCryptoDesC8Param();
		
	private:
		/**
		Constructor
		*/
		CCryptoDesC8Param();
		
		/**
		Constructor
		@param aUid	The UID of the crypto parameter
		*/						
		CCryptoDesC8Param(TUid aUid);

		/**
		Second Phase Constructor
		@param aValue The DesC8 to wrap
		*/				
		void ConstructL(const TDesC8& aValue);
	
	private:
		/**
		The copied descriptor
		*/		
		HBufC8* iValue;
		};

	/**
	Crypto param class that wraps an 16-bit constant descriptor
	*/
	NONSHARABLE_CLASS(CCryptoDesC16Param) : public CCryptoParam
		{
	public:
		/**
		Factory method for allocating a new CCryptoDesC8Param object
		that contains a copy of the supplied RInteger object.
		@param aValue The TDesC* to be wrapped (copied)
		@param aUid	The Uid of the CryptoParam
		@return A pointer to a CCryptoDesC8Param instance
		*/
		IMPORT_C static CCryptoDesC16Param* NewL(const TDesC16& aValue, TUid aUid);

		/**
		Factory method for allocating a new CCryptoDesC16Param object
		that contains a copy of the supplied RInteger object.
		Leaves the pointer of the CryptoParam on the cleanup stack
		@param aValue The TDesC* to be wrapped (copied)
		@param aUid	The Uid of the CryptoParam
		@return A pointer to a CCryptoDesC16Param instance
		*/
		IMPORT_C static CCryptoDesC16Param* NewLC(const TDesC16& aValue, TUid aUid);

		/**
		Returns the embedded value.
		@return A reference to the embedded TDesC16
		*/
		IMPORT_C const TDesC16& Value() const;

		/**
		Destructor
		*/
		IMPORT_C ~CCryptoDesC16Param();
		
	private:
		/**
		Constructor
		*/
		CCryptoDesC16Param();
		
		/**
		Constructor
		@param aUid	The UID of the crypto parameter
		*/						
		CCryptoDesC16Param(TUid aUid);

		/**
		Second Phase Constructor
		@param aValue The DesC16 to wrap
		*/				
		void ConstructL(const TDesC16& aValue);
	
	private:
		/**
		The copied descriptor
		*/		
		HBufC16* iValue;
		};
		
	NONSHARABLE_CLASS(CCryptoParams) : public CBase
		{
	public:
		IMPORT_C static CCryptoParams* NewL(void);
		IMPORT_C static CCryptoParams* NewLC(void);

		/**
		 * Various adding methods (CCryptoParams takes a copy)
		 */		
		IMPORT_C void AddL(const TInteger& aParam, TUid aUid);
		IMPORT_C void AddL(const TInt aParam, TUid aUid);
		IMPORT_C void AddL(const TDesC8& aParam, TUid aUid);
		IMPORT_C void AddL(const TDesC16& aParam, TUid aUid);

		/**
		 * Various retrieving methods
		 */
		IMPORT_C const TInteger& GetBigIntL(TUid aUid) const;
		IMPORT_C TInt GetTIntL(TUid aUid) const;
		IMPORT_C const TDesC8& GetTDesC8L(TUid aUid) const;
		IMPORT_C const TDesC16& GetTDesC16L(TUid aUid) const;
		IMPORT_C const RPointerArray<CCryptoParam>& GetParams() const;

		/// Queries if a parameter with the specified uid is present
		IMPORT_C TBool IsPresent(TUid aUid) const;

		/// Return the count of parameters present
		IMPORT_C TInt Count(void) const;

		/// Copy the passed CCryptoParams
		IMPORT_C CCryptoParams& CopyL(const CCryptoParams& aParams);

		/// Destructor
		IMPORT_C virtual ~CCryptoParams();

	protected:
		/** @internalComponent */
		CCryptoParams();

		/** @internalComponent */
		void ConstructL(void);
		/** @internalComponent */
		CCryptoParam* GetCryptoParam(TUid aUid) const;
		/** @internalComponent */ 
		CCryptoParam* GetCryptoParamL(TUid aUid) const;

	private:
		RPointerArray<CCryptoParam> iParams;
		};
	} //End of namespace

#endif // __CRYPTOPARAMS_H__

