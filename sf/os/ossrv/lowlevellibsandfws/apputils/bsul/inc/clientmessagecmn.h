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
// Wrapper around RMessage2 that provides message validation and error handling
// to improve robustness of system servers under IPC attack
// 
//

#if !defined(CLIENTMESSAGECMN_H)
#define CLIENTMESSAGECMN_H

#include <e32std.h>
#include <e32base.h>
#include <e32debug.h>
#include <babitflags.h>
#include "clientmessage.h"

namespace BSUL
	{

	
	/**
	This defines the maximum number of parameters that can be stored in a  
	TClientMessageSchema structure.  This corresponds to the maximum number
	of arguments in an RMessage2
	@internalComponent
	*/
	const TInt KMaxParameters = 4;
	
	/**
	This mask is used to select the parameter type from TParameterDetails.iType
	@internalComponent
	*/	
	const TInt KParamTypeMask = 0xFFFF;
	
	/**
	This mask is used to select the validation function index from 
	TParameterDetails.iType
	@internalComponent
	*/	
	const TInt KValidationFnIndexMask = 0xFFFF0000;
	
	/**
	This is used to shift down the value masked using KValidationFnIndexMask 
	@internalComponent
	*/	
	const TInt KShift16Bit = 16;
	
	
	/**
	This enum lists the flags currently represented by the TBitFlags32
	CClientMessage::iFlags
	@internalComponent 
	**/
	enum TFlagValues
		{
		EFlagParam0Validated = 0,
		EFlagParam1Validated,
		EFlagParam2Validated,
		EFlagParam3Validated,
		/** Informs CompleteRequestL() that message not valid as client has already been panicked. */
		EFlagPanicClient,
		/** Setting this flag indicates that for bad message errors, request 
		should be completed with error code rather than panicking the client */
		EFlagDoNotPanicClientOnBadMessageErrors = 30,
		EFlagLogBadMessages = 31
		};
	
	/**
	This class implements the behaviour for a TInt parameter type.
	The ValidateL function reads the Int value from the clients message and checks 
	that the value read is between the iMax and iMin constraints defined in the 
	message schema.
	@internalComponent
	*/
	NONSHARABLE_CLASS(CIntParameter) : public CMessageParameterBase
	{
	public:
		
		static CMessageParameterBase* NewL(const TParameterDetails& aParam, TInt aParamIndex, 
				const RMessage2& aMessage, TCustomValidationFn aValidationFn);
		virtual ~CIntParameter();
		virtual void ValidateL();
		virtual TInt GetIntL();
		
	private:
		CIntParameter(const TParameterDetails& aParam,TInt aParamIndex, 
				const RMessage2& aMessage, TCustomValidationFn aValidationFn);
	
	private:	
		//This stores the TInt value read from the client message during validation
		TInt iValue;
	};
	
	/**
	This class implements the behaviour for a read only 8 bit
	descriptor parameter type.  The ValidateL function checks that the length 
	of the descriptor argument in the client’s process does not exceed iMax 
	defined in the message schema.
	@internalComponent
	*/
	NONSHARABLE_CLASS(CDes8ReadParameter) : public CMessageParameterBase
	{
	public:
		static CMessageParameterBase* NewL(const TParameterDetails& aParam, TInt aParamIndex, 
				const RMessage2& aMessage, TCustomValidationFn aValidationFn);
		virtual ~CDes8ReadParameter();
		virtual void ValidateL();
		virtual const TDesC8& GetDes8L();
		virtual TInt GetDesLengthL();
		virtual void ReadL(TDes8& aDes, TInt aOffset);	
		
	private:
		CDes8ReadParameter(const TParameterDetails& aParam, TInt aParamIndex, 
				const RMessage2& aMessage, TCustomValidationFn aValidationFn);
		
	private:	
		//On validation this descriptor is instantiated and the contents or the client 
		//descriptor are read into the local descriptor 
		HBufC8* iValue;
	};
	
	/**
	This class implements the behaviour for a read\write 8 bit
	descriptor parameter type.  The ValidateL function checks that the MaxLength 
	of the descriptor in the client’s process is not less than iMin and that the 
	length of the descriptor does not exceed iMax defined in the message schema. 
	@internalComponent
	*/
	NONSHARABLE_CLASS(CDes8Parameter) : public CMessageParameterBase
	{
	public:
		static CMessageParameterBase* NewL(const TParameterDetails& aParam, TInt aParamIndex, 
				const RMessage2& aMessage, TCustomValidationFn aValidationFn);
		virtual ~CDes8Parameter();
		virtual void ValidateL();
		virtual void WriteL(const TDesC8& aDes, TInt aOffset);
		virtual void ReadL(TDes8& aDes, TInt aOffset);
		virtual TInt GetDesLengthL();
		virtual TInt GetDesMaxLengthL();
		
	protected:
		CDes8Parameter(const TParameterDetails& aParam, TInt aParamIndex, 
				const RMessage2& aMessage, TCustomValidationFn aValidationFn);
	};
	
	/**
	This class implements the behaviour for a read only 16 bit
	descriptor parameter type.  The ValidateL function checks that the length 
	of the descriptor argument in the client’s process does not exceed iMax 
	defined in the message schema.
	@internalComponent
	*/
	NONSHARABLE_CLASS(CDes16ReadParameter) : public CMessageParameterBase
	{
	public:
		static CMessageParameterBase* NewL(const TParameterDetails& aParam, TInt aParamIndex, 
				const RMessage2& aMessage, TCustomValidationFn aValidationFn);	
		virtual ~CDes16ReadParameter();
		virtual void ValidateL();
		virtual const TDesC& GetDes16L();
		virtual TInt GetDesLengthL();
		virtual void ReadL(TDes& aDes, TInt aOffset);
		
		
	private:
		CDes16ReadParameter(const TParameterDetails& aParam, TInt aParamIndex, 
				const RMessage2& aMessage, TCustomValidationFn aValidationFn);
		
	private:
		//On validation this descriptor is instantiated and the contents or the client 
		//descriptor are read into the local descriptor 
		HBufC* iValue;
	};
	
	/**
	This class implements the behaviour for a read\write 16 bit
	descriptor parameter type.  The ValidateL function checks that the MaxLength 
	of the descriptor in the client’s process is not less than iMin and that the 
	length of the descriptor does not exceed iMax defined in the message schema. 
	@internalComponent
	*/
	NONSHARABLE_CLASS(CDes16Parameter) : public CMessageParameterBase
	{
	public:
		static CMessageParameterBase* NewL(const TParameterDetails& aParam, TInt aParamIndex, 
				const RMessage2& aMessage, TCustomValidationFn aValidationFn);
		virtual ~CDes16Parameter();
		virtual void ValidateL();
		virtual void WriteL(const TDesC& aDes, TInt aOffset);
		virtual void ReadL(TDes& aDes, TInt aOffset);
		virtual TInt GetDesLengthL();
		virtual TInt GetDesMaxLengthL();
		
	private:
		CDes16Parameter(const TParameterDetails& aParam, TInt aParamIndex, 
				const RMessage2& aMessage, TCustomValidationFn aValidationFn);
	};
	
	
	/**
	This class implements the behaviour for a generic TPckg<>
	parameter type.  The ValidateL function calls the custom validation function 
	passed in to the object on creation.
	@internalComponent
	*/
	NONSHARABLE_CLASS(CPckgParameter) : public CDes8Parameter
	{
	public:
		static CMessageParameterBase* NewL(const TParameterDetails& aParam, TInt aParamIndex, 
				const RMessage2& aMessage, TCustomValidationFn aValidationFn);
		virtual ~CPckgParameter();
		virtual void ValidateL();
		
	private:
		CPckgParameter(const TParameterDetails& aParam, TInt aParamIndex, 
				const RMessage2& aMessage, TCustomValidationFn aValidationFn );
	};
	
	
	/**
	This class implements the behaviour for a Ptr parameter type.
	The ValidateL function reads the TAny* from the clients message and stores it for
	retrieval.
	@internalComponent
	*/
	NONSHARABLE_CLASS(CPtrParameter) : public CMessageParameterBase
	{
	public:
		static CMessageParameterBase* NewL(const TParameterDetails& aParam, TInt aParamIndex, 
				const RMessage2& aMessage, TCustomValidationFn aValidationFn);
		
		/**
		Destructor for CPtrParameter class.
		*/
		virtual ~CPtrParameter(){};
		virtual void ValidateL();
		virtual const TAny* GetPtrL();
		
	private:
	
		/**
		Constructor for CPtrParameter class.
		*/
		CPtrParameter(const TParameterDetails& aParam, TInt aParamIndex, 
				const RMessage2& aMessage, TCustomValidationFn aValidationFn);
		
	private:
		//This stores the TAny* value read from the client message during validation
		const TAny* iValue;
	};
	
	/**
	This typedef is used to simplify the declaration of the message schema
	table defined by the server.
	@internalComponent
	*/
	typedef CMessageParameterBase* (*TMessageParameterFactoryFn)(const TParameterDetails& aParam, 
				TInt aParamIndex, const RMessage2& aMessage, TCustomValidationFn aValidationFn);
		
	}//namespace

#endif
