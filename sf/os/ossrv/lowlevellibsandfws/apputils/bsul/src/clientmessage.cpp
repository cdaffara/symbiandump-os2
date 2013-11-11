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
//

#include "clientmessagecmn.h"

using namespace BSUL;

/**
Base 64 decoding table
*/
const TInt8 AsciiToBase64[80]=
	{
	 62, -1, -1, -1, 63, 52, 53, 54, 55, 56,
	 57, 58, 59, 60, 61, -1, -1, -1, 64, -1,
	 -1, -1,  0,  1,  2,  3,  4,  5,  6,  7,
	  8,  9, 10, 11, 12, 13, 14, 15, 16, 17,
	 18, 19, 20, 21, 22, 23, 24, 25, -1, -1,
	 -1, -1, -1, -1, 26, 27, 28, 29, 30, 31,
	 32, 33, 34, 35, 36, 37, 38, 39, 40, 41,
	 42, 43, 44, 45, 46, 47, 48, 49, 50, 51
	};

/**
Base 64 encoding table
*/	
const TInt8 Base64ToAscii[65]=
	{
	 65, 66, 67, 68, 69, 70, 71, 72, 73, 74,
	 75, 76, 77, 78, 79, 80, 81, 82, 83, 84,
	 85, 86, 87, 88, 89, 90, 97, 98, 99,100,
	101,102,103,104,105,106,107,108,109,110,
	111,112,113,114,115,116,117,118,119,120,
	121,122, 48, 49, 50, 51, 52, 53, 54, 55,
	 56, 57, 43, 47, 61 
	};
	
const TInt8 KImcvLookUpStartOffset = 43;
const TUint8 KImcvConvEquals = '=';
/**
The maximum number of US ASCII characters per line that are before the CRLF line
terminator when sending emails.  RFC 2822 recommends that each line SHOULD not exceed
80 characters including the CRLF terminator, and MUST not exceed 1000.
*/
const TInt KMaxB64EncodedCharsPerLine = 60; // Could be increased to 75 characters for every encoded line if KDecodeLineLength = 675.  60 was chosen to maintain existing behaviour.

/**
Parameter factory function lookup table.  This is used to instantiate a 
CMessageParameterBase derived object based on a TParamType enum value
*/
const TMessageParameterFactoryFn KParameterFactoryFunctions[] = {NULL,
															CIntParameter::NewL,
															CDes8ReadParameter::NewL,
															CDes8Parameter::NewL,
															CPckgParameter::NewL,
															CDes16ReadParameter::NewL,
															CDes16Parameter::NewL,
															CPtrParameter::NewL};

/**
Panic string for client message framework panic
*/
#ifdef _DEBUG
_LIT(KPanicCategory,"BSUL::ClientMsg");
#endif

const TInt KMaxServerNameLength = 32;

/**
This static function is used to panic the server in case of
incorrect use of CMessageParameterBase APIs or a badly formed
message schema
@param 	aPanic The Panic value
*/
void PanicServer(TInt aPanic)
	{
	_LIT(KUnknownServer, "Unknown");
	TBuf<KMaxServerNameLength> serverName(KUnknownServer);
	
	//Get the TLS data for this thread
	TClientMessageServerData* serverData = static_cast<TClientMessageServerData*>(Dll::Tls());
	
	if(serverData != NULL)
		{
		TPtrC8 name(serverData->iServerName);
		serverName.Copy(name);
		}
	
	User::Panic(serverName, aPanic);
	}

/**
Static initialisation function for ClientMessage Framework
@param 	aServerData The initialisation data for the server using the library
@leave Any system wide error code
*/
EXPORT_C void CClientMessage::InitialiseFrameworkL(const TClientMessageServerData& aServerData)
	{
	__ASSERT_DEBUG(User::StringLength(aServerData.iServerName) <= KMaxServerNameLength, User::Invariant());
	Dll::SetTls((TAny*)&aServerData);
	}

/**
Static factory function for CClientMessage class
@param 	aMessage The message that this object encapsulates
@return Pointer to a fully constructed CClientMessage object.
@leave  KErrNotInitialised if the framework has not been initialised by a 
		call to InitialiseFrameworkL.
*/
EXPORT_C CClientMessage* CClientMessage::NewL(const RMessage2& aMessage)
	{
	const TClientMessageServerData* serverData = static_cast<const TClientMessageServerData*>(Dll::Tls());
	if(serverData == NULL)
		{
		User::Leave(KErrNotInitialised);
		}
	
	CClientMessage* self = new(ELeave) CClientMessage(aMessage,*serverData);
	CleanupStack::PushL(self);
	self->ConstructL();
	CleanupStack::Pop(self);

	return self;
	}


/**
Second phase constructor of CClientMessage Object
Finds the schema for this message and checks the caller against the security
policy defined for this message.  Traverses the array of message parameters
and instantiates a CMessageParameterBase object for each parameter and adds these
to its internal array.
@leave  KErrInvalidFunction If the message function is not found in the message table
@leave  KErrBadHandle if the message handle is Null
@leave KErrPermissionDenied if the security policy for this message is not satisfied
*/
void CClientMessage::ConstructL()
	{
	if(!iMessage.IsNull())
		{
		
		//Find the message schema for this message.
		const TClientMessageSchema* messageSchema = FindMessageSchema();
		
		if(!messageSchema)
			{
			LogBadMessageL(KErrInvalidFunction);
			User::Leave(KErrInvalidFunction);
			}
				
		//Check message against security policy
		CheckSecurityPolicyL(messageSchema->iPolicy);
				
		//iterate through message parameters and instantiate all parameter objects
		for(int index = 0;index < messageSchema->iParamCount;index++)
			{	
			CMessageParameterBase* parameter = 
					CMessageParameterBase::CreateL(messageSchema->iParams[index], index, iMessage);		
			
			//Some parameter types are defined to be ignored.  These
			//should not be added to the list of parameters
			if(parameter != NULL)
				{
				//AppendL can leave so use cleanupstack to ensure that memory is not
				//leaked if AppendL leaves.
				CleanupStack::PushL(parameter);
				iParameters.AppendL(parameter);
				CleanupStack::Pop(parameter);
				}
			}
		}
	else
		{
		LogBadMessageL(KErrBadHandle);
		User::Leave(KErrBadHandle);
		}
	}

/**
Constructor for CClientMessage Object
@param aMessage The RMessage2 to be represented by this object
@param aServerData The Initialisation data for the server creating this object
*/
EXPORT_C CClientMessage::CClientMessage(const RMessage2& aMessage, 
						const TClientMessageServerData& aServerData) 
		: iParameters(KMaxParameters), iMessage(aMessage),
		iServerData(aServerData),iFlags(aServerData.iFlags & 0xFFFF0000)

	{	
	
	}

/**
Destructor for CClientMessageObject
*/
EXPORT_C CClientMessage::~CClientMessage()
	{
	for(int i = 0; i < iParameters.Count();i++)
		{
		delete iParameters[i];
		}
	iParameters.Reset();
	}

EXPORT_C const RMessage2& CClientMessage::Message()
	{
	return iMessage;
	}

/**
Panics the client through the message object
set an internal flag to indicate that the RMessage reference handle is now NULL
due to the client thread being tidied up.
@param 	aServer The Panic category
@param 	aPanic The Panic value
*/
EXPORT_C void CClientMessage::PanicClient(const TDesC& aServer, TInt aPanic)
	{
	iMessage.Panic(aServer, aPanic);
	iFlags.Set(EFlagPanicClient);
	}

/**
Checks a message against the security policy defined for the server
@param aPolicy The security policy to check this message against
@leave	KErrPermissionDenied if the message does not fulfil the security policy
*/
void CClientMessage::CheckSecurityPolicyL(const TSecurityPolicy& aPolicy)
	{
	if(!(aPolicy.CheckPolicy(iMessage,
				"Client failed security policy check for this server")))
		{
		User::Leave(KErrPermissionDenied);
		}
	return;
	}
	
/**
Finds a message schema in the message table for this server.
Does a binary search on the function number to pull the correct
message from the table.  Note that this assumes that the table
is sorted.
@return	A pointer to a TClientMessageSchema object in the message table, or Null if 
		the message does not correpsond to a message in the message table
*/
const TClientMessageSchema* CClientMessage::FindMessageSchema()
	{
	//This should always be less than KNumClientMessages 
	TInt function = iMessage.Function();
	TInt beg = 0;
	TInt end = iServerData.iMessageCount - 1;
	TInt mid = 0;
	TInt midFn = 0;
	
	while(beg <= end)
		{
		mid = (end + beg)/2;
		
		midFn = iServerData.iMessageSchema[mid].iFunction;
		if(midFn > function)
			{
			end = mid - 1;
			}	
		else if(midFn < function)
			{
			beg = mid + 1;
			}	
		else
			{
			return &iServerData.iMessageSchema[mid];
			}
		}
	return NULL;
	}




/**
Validates the message parameters against the constraints provided 
in the message table
@leave	KErrBadMessage if the message fails validation against the criteria supplied in the 
		message table
@leave	Any system-wide error code
*/
EXPORT_C void CClientMessage::ValidateL()
	{
	
	for(int i = 0; i < iParameters.Count();i++)
		{
		iParameters[i]->ValidateL();
		iFlags.Set(i);
		}
	}

/**
Validates a single message argument against the constraints provided 
in the message table
@param aParam The index value identifying the argument to validate
@leave	KErrBadMessage if the message fails validation against the criteria supplied in the 
		message table for the requested argument
@leave	KErrArgument if aParam is negative or is greater than the number of parameters for
		this message
@leave	Any system-wide error code
*/
EXPORT_C void CClientMessage::ValidateL(TInt aParam)
	{
	
	if(( aParam >= 0) && (aParam < iParameters.Count()))
		{	
		iParameters[aParam]->ValidateL();
		iFlags.Set(aParam);
		}
	else
		{
		User::Leave(KErrArgument);
		}
	}

/**
Checks if a given parameter has been validated
@param aParam The index value identifying the paramater to check
@leave	KErrArgument if aParam is not a valid parameter value
@leave	KErrNotValidated if the parameter has not been validated
*/
void CClientMessage::CheckValidatedL(TInt aParam)
	{
	if((aParam < EFlagParam0Validated) || (aParam > EFlagParam3Validated))
		{
		User::Leave(KErrArgument);
		}
	
	if(!iFlags.IsSet(aParam))
		{
		User::Leave(KErrNotValidated);
		}
	}


/**
Checks if a bad messages should be logged
@return True if bad messages should be logged
*/
TBool CClientMessage::LogBadMessages()
	{
	return iFlags.IsSet(EFlagLogBadMessages);
	}

/**
Checks if a bad messages should be logged
@param aError The error code to log
*/
void CClientMessage::LogBadMessageL(TInt aError)
	{
	//Check if logging of bad messages is enabled
	if(LogBadMessages())
		{

		TUid sid = TUid(iMessage.SecureId());
		TUidName clientSid = sid.Name();
		
		TInt function = Function();
		
		TBuf<KMaxServerNameLength> serverName;
		TPtrC8 name(iServerData.iServerName);
		serverName.Copy(name);
		
		switch(aError)
			{			
	#ifdef _DEBUG	
	
				
			case KErrInvalidFunction:
				RDebug::Print(_L("%S - CClientMessage Error: %d, Function %d. Unknown function request from client %S.\n"),
								&serverName,aError,function, &clientSid);
				break;
				
			case KErrBadParameter:
				RDebug::Print(_L("%S - CClientMessage Error: %d, Function %d. Bad argument in IPC request from client %S.\n"),
								&serverName,aError,function, &clientSid);
				break;
				
			case KErrBadMessageSchema:
				RDebug::Print(_L("%S - CClientMessage Error: %d, Function %d. Message schema incotrectly defined for this function %S.\n"),
								&serverName,aError,function, &clientSid);
				break;
				
			case KErrBadDescriptor:
			case KErrOverflow:
				RDebug::Print(_L("%S - CClientMessage Error: %d, Function %d. Bad descriptor argument in IPC request from client %S.\n"),
								&serverName,aError,function, &clientSid);
				break;
				
			case KErrNotValidated:
				RDebug::Print(_L("%S - CClientMessage Error: %d, Function %d. Message parameter not validated before use %S.\n"),
								&serverName,aError,function, &clientSid);
				break;
				
			default:
				RDebug::Print(_L("%S - CClientMessage Error: %d, Function %d. Bad message received from client %S.\n"),
										&serverName,aError,function, &clientSid);
			break;
	#else	
			default:
				break;
	#endif
			}
		}
	

	}

/**
Completes the message request or Panics the client if a
message error has occured.
@param aError The error value to complete the message with
*/
EXPORT_C void CClientMessage::CompleteRequestL(TInt aError)
	{
	//If server panics client 
	//then iMessage will be NULL
	if(!iFlags.IsSet(EFlagPanicClient))
		{
		if(aError != KErrNone)
			{
			LogBadMessageL(aError);
			}
		
		switch(aError)
			{
			case KErrInvalidFunction:
			case KErrBadDescriptor:
			case KErrOverflow:
				{
				//Check if Panic is disabled
				if( iFlags.IsClear(EFlagDoNotPanicClientOnBadMessageErrors) )
					{
					TBuf<KMaxServerNameLength> serverName;
					TPtrC8 name(iServerData.iServerName);
					serverName.Copy(name);
					PanicClient(serverName, aError);
					break;
					}
				}
			default:
				{	
				iMessage.Complete(aError);
				break;
				}
			}//switch
			
		}//if
	}

/**
Gets the function number of this message
@return The function number of this message
*/
EXPORT_C TInt CClientMessage::Function()
	{
	return iMessage.Function();
	}

/**
Gets the requested message argument as an integer value
@param aParam The parameter number to retrieve
@return The Int value for the requested parameter
@leave	KErrNotValidated if the message parameter has not been validated
		KErrWrongParameterType in UREL is the parameter requested is not an integer type
		Any other system wide error code
@panic ECMPanicWrongParameterType If this function is called for a parameter 
type that is not CIntParameter.
*/
EXPORT_C TInt CClientMessage::GetIntL(TInt aParam)
	{	
	CheckValidatedL(aParam);

	return iParameters[aParam]->GetIntL();
	}

/**
Gets the requested message argument as an TAny*
@param aParam The parameter number to retrieve
@return The TAny* for the requested parameter
@leave	KErrNotValidated if the message parameter has not been validated
		KErrWrongParameterType in UREL is the parameter requested is not an Ptr type
		Any other system wide error code
@panic ECMPanicWrongParameterType If this function is called for a parameter 
type that is not CPtrParameter.
*/
EXPORT_C const TAny* CClientMessage::GetPtrL(TInt aParam)
	{	
	CheckValidatedL(aParam);
	
	return iParameters[aParam]->GetPtrL();
	}

/**
Gets a reference to the local copy of the descriptor read from the message
@param aParam The parameter number of the descriptor to retrieve
@leave	KErrNotValidated if the message parameter has not been validated
		KErrWrongParameterType in UREL is the parameter requested is not a readable
		TDes8 type
		Any other system wide error code
@panic ECMPanicWrongParameterType If this function is called for a parameter 
type that is not CDes8ReadParameter.
*/
EXPORT_C const TDesC8& CClientMessage::GetDes8L(TInt aParam)
	{
	CheckValidatedL(aParam);
	
	return iParameters[aParam]->GetDes8L();
	}

/**
Gets a reference to the local copy of the descriptor read from the message
@param aParam The parameter number of the descriptor to retrieve
@leave	KErrNotValidated if the message parameter has not been validated
		KErrWrongParameterType in UREL is the parameter requested is not a readable
		TDes16 type
		Any other system wide error code
@panic ECMPanicWrongParameterType If this function is called for a parameter 
type that is not CDes16ReadParameter.
*/
EXPORT_C const TDesC& CClientMessage::GetDes16L(TInt aParam)
	{
	CheckValidatedL(aParam);
	
	return iParameters[aParam]->GetDes16L();
	}



/**
Gets a descriptor value read from the message
@param aParam The parameter number of the descriptor to retrieve
@param aDes On exit contains the descriptor value requested
@param aOffset The desired offset from which to read the descriptor
@leave	KErrNotValidated if the message parameter has not been validated
		KErrWrongParameterType in UREL is the parameter requested is not a readable
		TDes8 type
		Any other system wide error code
@panic ECMPanicWrongParameterType If this function is called for a parameter 
type that is not CDes8Parameter or CDes8ReadParameter.
*/
EXPORT_C void CClientMessage::ReadL(TInt aParam, TDes8& aDes, TInt aOffset)
	{
	CheckValidatedL(aParam);

	iParameters[aParam]->ReadL(aDes, aOffset);
	}

/**
Gets a descriptor value read from the message
@param aParam The parameter number of the descriptor to retrieve
@param aDes On exit contains the descriptor value requested
@param aOffset The desired offset from which to read the descriptor
@leave	KErrNotValidated if the message parameter has not been validated
		KErrWrongParameterType in UREL is the parameter requested is not a readable 
		TDes16 type
		Any other system wide error code
@panic ECMPanicWrongParameterType If this function is called for a parameter 
type that is not CDes16Parameter or CDes16ReadParameter.
*/
EXPORT_C void CClientMessage::ReadL(TInt aParam, TDes16& aDes, TInt aOffset)
	{
	CheckValidatedL(aParam);
	
	iParameters[aParam]->ReadL(aDes, aOffset);
	}

/**
Writes to a descriptor field in the message
@param aParam The parameter number of the descriptor to write
@param aDes The descriptor to write to the message
@param aOffset The desired offset at which to write the descriptor
@leave	KErrNotValidated if the message parameter has not been validated
		KErrWrongParameterType in UREL is the parameter requested is not a writable 
		TDes8 type
		Any other system wide error code
@panic ECMPanicWrongParameterType If this function is called for a parameter 
type that is not CDes8Parameter
*/
EXPORT_C void CClientMessage::WriteL(TInt aParam, const TDesC8& aDes, TInt aOffset)
	{
	CheckValidatedL(aParam);
	
	iParameters[aParam]->WriteL(aDes, aOffset);
	}

/**
Writes to a descriptor field in the message
@param aParam The parameter number of the descriptor to write
@param aDes The descriptor to write to the message
@param aOffset The desired offset at which to write the descriptor
@leave	KErrNotValidated if the message parameter has not been validated
		KErrWrongParameterType in UREL is the parameter requested is not a writable 
		TDes16 type
		Any other system wide error code
@panic ECMPanicWrongParameterType If this function is called for a parameter 
type that is notCDes16Parameter
*/
EXPORT_C void CClientMessage::WriteL(TInt aParam, const TDesC16& aDes, TInt aOffset)
	{	
	CheckValidatedL(aParam);
	
	iParameters[aParam]->WriteL(aDes, aOffset);
	}

/**
Gets the length of the requested descriptor message argument
@param aParam The parameter number to retrieve
@return The Length of the descriptor in the client process
@leave	KErrNotValidated if the message parameter has not been validated
		KErrWrongParameterType in UREL is the parameter requested is not a descriptor type
		Any other system wide error code
@panic ECMPanicWrongParameterType If this function is called for a parameter 
type that is not a descriptor type.
*/
EXPORT_C TInt CClientMessage::GetDesLengthL(TInt aParam)
	{
	CheckValidatedL(aParam);
	
	return iParameters[aParam]->GetDesLengthL();
	}

/**
Gets the max length of the requested descriptor message argument
@param aParam The parameter number to retrieve
@return The Max length of the descriptor in the client process
@leave	KErrNotValidated if the message parameter has not been validated
		KErrWrongParameterType in UREL is the parameter requested is not a descriptor type
		Any other system wide error code
@panic ECMPanicWrongParameterType If this function is called for a parameter type 
that is not a descriptor type.
*/
EXPORT_C TInt CClientMessage::GetDesMaxLengthL(TInt aParam)
	{
	CheckValidatedL(aParam);
	
	return iParameters[aParam]->GetDesMaxLengthL();
	}

/********************************************************************************
 * 			CMessageParameterBase and Derived Class Definitions
 *******************************************************************************/


/**
Factory function for instantiating derived Parameter classes.
Uses factory lookup table to instantiate approptiate parameter 
object based on parameter details passed in
@param aParam Parameter details object used to instantiate an appropriate
			implementation of CMessageParameterBase.
@param aParamIndex The Index of this parameter within the RMessage2 arguments
@param aMessage The RMessage2 object containing the parameter represented by 
			this object
@return A fully constructed CMessageParameterBase derived object deterimined by 
		aParam.
@leave	KErrBadMessageSchema in UREL if if the schema for this parameter is 
		incorrectly defined
@leave	Any system-wide error code.
@panic ECMPanicBadMessageSchema in UDEB if the schema for this parameter is 
		incorrectly defined
*/
CMessageParameterBase* CMessageParameterBase::CreateL(const TParameterDetails& aParam, 
			TInt aParamIndex, const RMessage2& aMessage)
	{
	
	//The parameter type is the bottom 16 bits of the param type
	TInt paramType = (aParam.iType & KParamTypeMask);
	
	__ASSERT_DEBUG((paramType > 0), PanicServer(ECMPanicBadMessageSchema));
	
	CMessageParameterBase* newParam = NULL;
	
	switch(paramType)
		{
		case EParamInt:
		case EParamDes8Read:
		case EParamDes8:
		case EParamDes16Read:
		case EParamDes16:
		case EParamPtr:
		case EParamPckg:
			{	
			//Create the new parameter object
			newParam = (KParameterFactoryFunctions[paramType])(aParam, aParamIndex, 
							aMessage, GetValidationFunctionL(aParam));
			break;
			}
			
		default:
			{
#ifdef _DEBUG
			PanicServer(ECMPanicBadMessageSchema);
#else
			User::Leave(KErrBadMessageSchema);
#endif
			}
	}

	return newParam;
	}

/**
Constructor for CMessageParameterBase object
@param aParamIndex The Index of this parameter within the RMessage2 arguments
@param aMessage The RMessage2 object containing the parameter represented by 
			this object
*/
CMessageParameterBase::CMessageParameterBase(const TParameterDetails& aParam, 
		TInt aParamIndex,const RMessage2& aMessage, TCustomValidationFn aValidationFn)
	: iIndex(aParamIndex), iMessage(aMessage), iParamDetails(aParam), iValidationFn(aValidationFn)
	{

	}

/**
Gets the validation function for this parameter from the 
TClientMessageServerData structure
@param aParam Parameter object used to find the validation function
@return The validation function for this parameter type
@leave	KErrBadMessageSchema in UREL if if the schema for this parameter is 
		incorrectly defined
@leave	Any other system wide error code
@panic ECMPanicBadMessageSchema in UDEB if the schema for this parameter is 
		incorrectly defined
*/
TCustomValidationFn CMessageParameterBase::GetValidationFunctionL(const TParameterDetails& aParam)
	{
	//Get the TLS data for this thread - this will never be null at this point 
	//as it is checked in CClientMessage::NewL
	TClientMessageServerData* serverData = static_cast<TClientMessageServerData*>(Dll::Tls());
	
	//The index of the validation function for this parameter is held in 
	//the upper 16 bits of aParam.iType. Mask this out and shift down to 
	//get the index.
	TInt fnIndex = (aParam.iType & KValidationFnIndexMask) >> KShift16Bit;
	
	
	if(fnIndex >= serverData->iValidationFnCount)
		{
#ifdef _DEBUG
			PanicServer(ECMPanicBadMessageSchema);
#else
			User::Leave(KErrBadMessageSchema);
#endif
		}
	
	//Return the validation function
	return serverData->iCustomValidationFns[fnIndex];
	}

/**
Default implementation of GetIntL for CMessageParameterBase object.
This is only called if this API is not defined for the given parameter type.
@return KErrNone - A Dummy return value
@leave KErrWrongParameterType in UREL if this function is not defined for the 
		given parameter type
@panic ECMPanicWrongParameterType in UDEB if this function is not defined for the 
		given parameter type
*/
TInt CMessageParameterBase::GetIntL()
	{
#ifdef _DEBUG
	User::Panic(KPanicCategory,ECMPanicWrongParameterType);
#else
	User::Leave(KErrWrongParameterType);
#endif
	return KErrNone;
	}

/**
Default implementation of GetPtrL for CMessageParameterBase object.
This is only called if this API is not defined for the given parameter type.
@return NULL - A Dummy return value
@leave KErrWrongParameterType in UREL if this function is not defined for the 
		given parameter type
@panic ECMPanicWrongParameterType in UDEB if this function is not defined for the 
		given parameter type
*/
const TAny* CMessageParameterBase::GetPtrL()
	{
#ifdef _DEBUG
	User::Panic(KPanicCategory,ECMPanicWrongParameterType);
#else
	User::Leave(KErrWrongParameterType);
#endif
	return NULL;
	}

/**
Default implementation of WriteL for CMessageParameterBase object.
This is only called if this API is not defined for the given parameter type.
@leave KErrWrongParameterType in UREL if this function is not defined for the 
		given parameter type
@panic ECMPanicWrongParameterType in UDEB if this function is not defined for the 
		given parameter type
*/
void CMessageParameterBase::WriteL(const TDesC8& /*aDes*/, TInt /*aOffset*/)
	{
#ifdef _DEBUG
	User::Panic(KPanicCategory,ECMPanicWrongParameterType);
#else
	User::Leave(KErrWrongParameterType);
#endif
	}

/**
Default implementation of WriteL for CMessageParameterBase object.
This is only called if this API is not defined for the given parameter type.
@leave KErrWrongParameterType in UREL if this function is not defined for the 
		given parameter type
@panic ECMPanicWrongParameterType in UDEB if this function is not defined for the 
		given parameter type
*/
void CMessageParameterBase::WriteL(const TDesC& /*aDes*/, TInt /*aOffset*/)
	{
#ifdef _DEBUG
	User::Panic(KPanicCategory,ECMPanicWrongParameterType);
#else
	User::Leave(KErrWrongParameterType);
#endif
	}

/**
Default implementation of ReadL for CMessageParameterBase object.
This is only called if this API is not defined for the given parameter type.
@leave KErrWrongParameterType in UREL if this function is not defined for the 
		given parameter type
@panic ECMPanicWrongParameterType in UDEB if this function is not defined for the 
		given parameter type
*/
void CMessageParameterBase::ReadL(TDes8& /*aDes*/,TInt /*aOffset*/)
	{
#ifdef _DEBUG
	User::Panic(KPanicCategory,ECMPanicWrongParameterType);
#else
	User::Leave(KErrWrongParameterType);
#endif
	}

/**
Default implementation of ReadL for CMessageParameterBase object.
This is only called if this API is not defined for the given parameter type.
@leave KErrWrongParameterType in UREL if this function is not defined for the 
		given parameter type
@panic ECMPanicWrongParameterType in UDEB if this function is not defined for the 
		given parameter type
*/
void CMessageParameterBase::ReadL(TDes& /*aDes*/, TInt /*aOffset*/)
	{
#ifdef _DEBUG
	User::Panic(KPanicCategory,ECMPanicWrongParameterType);
#else
	User::Leave(KErrWrongParameterType);
#endif
	}

/**
Default implementation of GetDesLengthL for CMessageParameterBase object.
This is only called if this API is not defined for the given parameter type.
@return KErrNone - A Dummy return
@leave KErrWrongParameterType in UREL if this function is not defined for the 
		given parameter type
@panic ECMPanicWrongParameterType in UDEB if this function is not defined for the 
		given parameter type
*/
TInt CMessageParameterBase::GetDesLengthL()
	{
#ifdef _DEBUG
	User::Panic(KPanicCategory,ECMPanicWrongParameterType);
#else
	User::Leave(KErrWrongParameterType);
#endif
	return KErrNone;
	}

/**
Default implementation of GetDesMaxLengthL for CMessageParameterBase object.
This is only called if this API is not defined for the given parameter type.
@return KErrNone - A Dummy return
@leave KErrWrongParameterType in UREL if this function is not defined for the 
		given parameter type
@panic ECMPanicWrongParameterType in UDEB if this function is not defined for the 
		given parameter type
*/
TInt CMessageParameterBase::GetDesMaxLengthL()
	{
#ifdef _DEBUG
	User::Panic(KPanicCategory,ECMPanicWrongParameterType);
#else
	User::Leave(KErrWrongParameterType);
#endif
	return KErrNone;
	}

/**
Default implementation of GetDes8L for CMessageParameterBase object.
This is only called if this API is not defined for the given parameter type.
@return KErrNone - A Dummy return
@leave KErrWrongParameterType in UREL if this function is not defined for the 
		given parameter type
@panic ECMPanicWrongParameterType in UDEB if this function is not defined for the 
		given parameter type
*/

const TDesC8& CMessageParameterBase::GetDes8L()
	{
#ifdef _DEBUG
	User::Panic(KPanicCategory,ECMPanicWrongParameterType);
#else
	User::Leave(KErrWrongParameterType);
#endif	
	_LIT8(KDummy,"");
	return KDummy;
	}

/**
Default implementation of GetDes16L for CMessageParameterBase object.
This is only called if this API is not defined for the given parameter type.
@return KErrNone - A Dummy return
@leave KErrWrongParameterType in UREL if this function is not defined for the 
		given parameter type
@panic ECMPanicWrongParameterType in UDEB if this function is not defined for the 
		given parameter type
*/
const TDesC& CMessageParameterBase::GetDes16L()
	{
#ifdef _DEBUG
	User::Panic(KPanicCategory,ECMPanicWrongParameterType);
#else
	User::Leave(KErrWrongParameterType);
#endif	
	
	_LIT(KDummy,"");
	return KDummy;
	}

/**
Returns the value of iMin defined in the schema for this parameter
@return The Min constraint for this parameter
*/
TInt CMessageParameterBase::Min()
	{
	return iParamDetails.iMin;
	}

/**
Returns the value of iMax defined in the schema for this parameter
@return The max constraint for this parameter
*/
TInt CMessageParameterBase::Max()
	{
	return iParamDetails.iMax;
	}


/**
Factory function for instantiating CIntParameter objects
@param aParam Parameter details object used to construct object.
@param aParamIndex The Index of this parameter within the RMessage2 arguments
@param aMessage The RMessage2 object containing the parameter represented by 
@return A fully constructed CIntParameter object.
@leave	Any system-wide error code.
*/
CMessageParameterBase* CIntParameter::NewL(const TParameterDetails& aParam, 
				TInt aParamIndex, const RMessage2& aMessage, TCustomValidationFn aValidationFn)
	{
	CIntParameter* self = new(ELeave) CIntParameter(aParam, aParamIndex, aMessage, aValidationFn);
	return self;
	}

/**
Constructor for CIntParameter class.
@param aParam Parameter details to be encapsulated by object
@param aParamIndex The Index of this parameter within the RMessage2 arguments
@param aMessage The RMessage2 object containing the parameter to be represented
@panic ECMPanicBadMessageSchema in UDEB if the schema for this parameter is 
		incorrectly defined
*/
CIntParameter::CIntParameter(const TParameterDetails& aParam, TInt aParamIndex,
			const RMessage2& aMessage, TCustomValidationFn aValidationFn)
		: CMessageParameterBase(aParam, aParamIndex, aMessage, aValidationFn)
		
	{	
	__ASSERT_DEBUG((iParamDetails.iMax >= iParamDetails.iMin), 
			PanicServer(ECMPanicBadMessageSchema));
	}

/**
Destructor for CIntParameter class.
*/
CIntParameter::~CIntParameter()
	{

	}

/**
Validates given message parameter agains constraints
represented by this object. Stores the Int value from the message
 to allow for simple retrieval when required.
@leave	KErrBadParameter if the message parameter does not conform
		to the constraints represented by this object
@leave	Any system-wide error code
*/
void CIntParameter::ValidateL()
	{
	
	switch(iIndex)
		{
	
		case 0:
			iValue = iMessage.Int0();
			break;
		
		case 1:
			iValue = iMessage.Int1();
			break;
		
		case 2:
			iValue = iMessage.Int2();
			break;
			
		case 3:
			iValue = iMessage.Int3();
			break;
		
		default:
			User::Leave(KErrArgument);
			break;
		}
	
	if(iValidationFn != NULL)
		{
		iValidationFn(this);	
		}
	
	else
		{	
		if((iValue < iParamDetails.iMin)||(iValue > iParamDetails.iMax))
			{
			User::Leave(KErrBadParameter);
			}
		}
	}

/**
Retrieves the TInt value read from the clients message during validation
@return The TInt value read from the client message
*/
TInt CIntParameter::GetIntL()
	{
	return iValue;
	}

/**
Factory function for instantiating CDes8ReadParameter objects
@param aParam Parameter details object used to construct object.
@param aParamIndex The Index of this parameter within the RMessage2 arguments
@param aMessage The RMessage2 object containing the parameter to be represented
@return A fully constructed CDes8ReadParameter object.
@leave	Any system-wide error code.
*/
CMessageParameterBase* CDes8ReadParameter::NewL(const TParameterDetails& aParam, 
		TInt aParamIndex, const RMessage2& aMessage, TCustomValidationFn aValidationFn)
	{
	CDes8ReadParameter* self = 
		new(ELeave) CDes8ReadParameter(aParam, aParamIndex, aMessage, aValidationFn);
	
	return self;
	}

/**
Constructor for CDes8ReadParameter class.
@param aParam Parameter details to be encapsulated by object
@param aParamIndex The Index of this parameter within the RMessage2 arguments
@param aMessage The RMessage2 object containing the parameter to be represented
@panic ECMPanicBadMessageSchema in UDEB if the schema for this parameter is 
		incorrectly defined
*/
CDes8ReadParameter::CDes8ReadParameter(const TParameterDetails& aParam, TInt aParamIndex,
		const RMessage2& aMessage, TCustomValidationFn aValidationFn)
	: CMessageParameterBase(aParam, aParamIndex, aMessage, aValidationFn)
	{	
	__ASSERT_DEBUG((iParamDetails.iMin >= 0), 
			PanicServer(ECMPanicBadMessageSchema));
	
	__ASSERT_DEBUG((iParamDetails.iMax > 0), 
			PanicServer(ECMPanicBadMessageSchema));
	
	__ASSERT_DEBUG((iParamDetails.iMax >= iParamDetails.iMin), 
			PanicServer(ECMPanicBadMessageSchema));
	}

/**
Destructor for CDes8ReadParameter class.
*/
CDes8ReadParameter::~CDes8ReadParameter()
	{
	delete iValue;
	}

/**
Validates given message argument against constraints
represented by this object. Reads in the descriptor from the 
clients message to enable simple retrival when required.
@leave	KErrBadDescriptor if the message parameter does not conform
		to the constraints represented by this object
@leave	Any system-wide error code
*/
void CDes8ReadParameter::ValidateL()
	{
	TInt length = iMessage.GetDesLengthL(iIndex);
	
	//if there is a supplied custom validation function, call that now
	if(iValidationFn != NULL)
		{
		iValidationFn(this);
		}
	
	else
		{		
		if((length < iParamDetails.iMin) || (length > iParamDetails.iMax))
			{
			User::Leave(KErrBadDescriptor);
			}
		}
	
	iValue = HBufC8::NewL(length);
	TPtr8 ptr = iValue->Des();
	ReadL(ptr,0);
	}

/**
Gets the descriptor read from the clients message during validation
@return	const reference to the local descriptor copy
*/
const TDesC8& CDes8ReadParameter::GetDes8L()
	{
	return *iValue;
	}

/**
Gets the length of the descriptor in the client message
@return	The length of the descriptor
@leave	KErrBadDescriptor if the message argument is not a descriptor type
		Any other system wide error code
*/
TInt CDes8ReadParameter::GetDesLengthL()
	{
	return iMessage.GetDesLengthL(iIndex);
	}

/**
Retrieves the descriptor value read from the clients 
message during validation
@param aDes The target descriptor.
@param aOffset The offset from the start of the clients descriptor
@leave KErrArgument if iIndex has a value outside the valid range, or if aOffset is negative.
@panic ECMPanicBadDescriptor in UDEB if the supplied descriptor is too small. 
if the schema for this parameter is incorrectly defined
*/
void CDes8ReadParameter::ReadL(TDes8& aDes, TInt aOffset)
	{
	__ASSERT_DEBUG((aDes.MaxLength() >= (iMessage.GetDesLengthL(iIndex) - aOffset)), 
			PanicServer(ECMPanicBadDescriptor));
	
	iMessage.ReadL(iIndex,aDes,aOffset);
	}

/**
Factory function for instantiating CDes8WriteParameter objects
@param aParam Parameter details object used to construct object.
@param aParamIndex The Index of this parameter within the RMessage2 arguments
@param aMessage The RMessage2 object containing the parameter to be represented
@return A fully constructed CDes8WriteParameter object.
@leave	Any system-wide error code.
*/
CMessageParameterBase* CDes8Parameter::NewL(const TParameterDetails& aParam, 
		TInt aParamIndex, const RMessage2& aMessage, TCustomValidationFn aValidationFn)
	{
	CDes8Parameter* self = 
		new(ELeave) CDes8Parameter(aParam, aParamIndex, aMessage, aValidationFn);
	
	return self;
	}

/**
Constructor for CDes8WriteParameter class.
@param aParam Parameter details to be encapsulated by object
@param aParamIndex The Index of this parameter within the RMessage2 arguments
@param aMessage The RMessage2 object containing the parameter to be represented
@panic ECMPanicBadMessageSchema in UDEB if the schema for this parameter is 
		incorrectly defined
*/
CDes8Parameter::CDes8Parameter(const TParameterDetails& aParam, TInt aParamIndex,
		const RMessage2& aMessage, TCustomValidationFn aValidationFn)
	: CMessageParameterBase(aParam, aParamIndex, aMessage, aValidationFn)
		
	{
	__ASSERT_DEBUG((iParamDetails.iMin >= 0), 
			PanicServer(ECMPanicBadMessageSchema));
	
	__ASSERT_DEBUG((iParamDetails.iMax >= 0), 
			PanicServer(ECMPanicBadMessageSchema));
	}

/**
Destructor for CDes8WriteParameter class.
*/
CDes8Parameter::~CDes8Parameter()
	{
	}

/**
Validates given message argument against constraints
represented by this object.
@leave	KErrBadDescriptor if the message parameter does not conform
		to the constraints represented by this object
@leave	Any system-wide error code
*/
void CDes8Parameter::ValidateL()
	{
	
	//if there is a supplied custom validation function, call that now
	if(iValidationFn != NULL)
		{
		iValidationFn(this);
		}
	
	else
		{
		TInt length = iMessage.GetDesLengthL(iIndex);
		TInt maxLength = iMessage.GetDesMaxLengthL(iIndex);
		
		if((maxLength < iParamDetails.iMin)||(length > iParamDetails.iMax))
			{
			User::Leave(KErrBadDescriptor);
			}
		}
	}

/**
Gets the length of the descriptor in the client message
@return	The length of the descriptor
@leave	KErrBadDescriptor if the message argument is not a descriptor type
		Any other system wide error code
*/
TInt CDes8Parameter::GetDesLengthL()
	{
	return iMessage.GetDesLengthL(iIndex);
	}

/**
Gets the max length of the descriptor in the client message
@return	The max length of the descriptor
@leave	KErrBadDescriptor if the message argument is not a descriptor type
		Any other system wide error code
*/
TInt CDes8Parameter::GetDesMaxLengthL()
	{
	return iMessage.GetDesMaxLengthL(iIndex);
	}


/**
Reads a descriptor from the requested message argument
@param aDes The target descriptor.
@param aOffset The offset from the start of the clients descriptor
@leave	Any system wide error code.
@panic ECMPanicBadDescriptor in UDEB if the supplied descriptor is too small. 
*/
void CDes8Parameter::ReadL(TDes8& aDes, TInt aOffset)
	{
	__ASSERT_DEBUG((aDes.MaxLength() >= (iMessage.GetDesLengthL(iIndex) - aOffset)), 
			PanicServer(ECMPanicBadDescriptor));
	
	iMessage.ReadL(iIndex,aDes,aOffset);
	}

/**
Validates and writes a descriptor to the requested 
message argument
@param aDes The source descriptor containing the data to be written.
@param aOffset The offset from the start of the clients descriptor
@leave	   Any system wide error code.
*/
void CDes8Parameter::WriteL(const TDesC8& aDes, TInt aOffset)
	{
	iMessage.WriteL(iIndex,aDes,aOffset);
	}

/**
Factory function for instantiating CIntParameter objects
@param aParam Parameter details object used to construct object.
@param aParamIndex The Index of this parameter within the RMessage2 arguments
@param aMessage The RMessage2 object containing the parameter to be represented
@return A fully constructed CIntParameter object.
@leave	Any system-wide error code.
*/
CMessageParameterBase* CDes16ReadParameter::NewL(const TParameterDetails& aParam, 
		TInt aParamIndex, const RMessage2& aMessage, TCustomValidationFn aValidationFn)
	{
	CDes16ReadParameter* self = 
		new(ELeave) CDes16ReadParameter(aParam, aParamIndex, aMessage, aValidationFn);
	
	return self;
	}

/**
Constructor for CDes8ReadParameter class.
@param aParam Parameter details to be encapsulated by object
@param aParamIndex The Index of this parameter within the RMessage2 arguments
@param aMessage The RMessage2 object containing the parameter to be represented
@panic ECMPanicBadMessageSchema in UDEB if the schema for this parameter is 
		incorrectly defined
*/
CDes16ReadParameter::CDes16ReadParameter(const TParameterDetails& aParam, TInt aParamIndex,
		const RMessage2& aMessage, TCustomValidationFn aValidationFn)
	: CMessageParameterBase(aParam, aParamIndex, aMessage, aValidationFn)
	{
	__ASSERT_DEBUG((iParamDetails.iMin >= 0), 
			PanicServer(ECMPanicBadMessageSchema));
	
	__ASSERT_DEBUG((iParamDetails.iMax > 0), 
			PanicServer(ECMPanicBadMessageSchema));
	
	__ASSERT_DEBUG((iParamDetails.iMax >= iParamDetails.iMin), 
			PanicServer(ECMPanicBadMessageSchema));
	}

/**
Destructor for CDes16ReadParameter class.
*/
CDes16ReadParameter::~CDes16ReadParameter()
	{
	delete iValue;
	}

/**
Validates given message argument against constraints
represented by this object. Reads in the descriptor from the 
clients message to enable simple retrival when required.
@leave	KErrBadDescriptor if the message parameter does not conform
		to the constraints represented by this object
@leave	Any system-wide error code
*/
void CDes16ReadParameter::ValidateL()
	{
	TInt length = iMessage.GetDesLengthL(iIndex);
	
	//if there is a supplied custom validation function, call that now
	if(iValidationFn != NULL)
		{
		iValidationFn(this);
		}
	
	else
		{		
		if((length < iParamDetails.iMin) || (length > iParamDetails.iMax))
			{
			User::Leave(KErrBadDescriptor);
			}
		}

	iValue = HBufC::NewL(length);
	TPtr ptr = iValue->Des();
	ReadL(ptr,0);
	}

/**
Gets the descriptor read from the clients message during validation
@return	const reference to the local descriptor copy
*/
const TDesC& CDes16ReadParameter::GetDes16L()
	{
	return *iValue;
	}

/**
Gets the length of the descriptor in the client message
@return	The length of the descriptor
@leave	KErrBadDescriptor if the message argument is not a descriptor type
		Any other system wide error code
*/
TInt CDes16ReadParameter::GetDesLengthL()
	{
	return iMessage.GetDesLengthL(iIndex);
	}

/**
Retrieves the descriptor value read from the clients 
message during validation
@param aDes The target descriptor.
@param aOffset The offset from the start of the clients descriptor
@leave	KErrArgument if the suplied descriptor is too small or an invalid
		offset is supplied
@panic ECMPanicBadDescriptor in UDEB if the supplied descriptor is too small. 
*/
void CDes16ReadParameter::ReadL(TDes& aDes, TInt aOffset)
	{
	__ASSERT_DEBUG((aDes.MaxLength() >= (iMessage.GetDesLengthL(iIndex) - aOffset)), 
			PanicServer(ECMPanicBadDescriptor));
	
	iMessage.ReadL(iIndex,aDes,aOffset);
	}

/**
Factory function for instantiating CDes16WriteParameter objects
@param aParam Parameter details object used to construct object.
@param aParamIndex The Index of this parameter within the RMessage2 arguments
@param aMessage The RMessage2 object containing the parameter to be represented
@return A fully constructed CDes16WriteParameter object.
@leave	Any system-wide error code.
*/
CMessageParameterBase* CDes16Parameter::NewL(const TParameterDetails& aParam,
		TInt aParamIndex, const RMessage2& aMessage, TCustomValidationFn aValidationFn)
	{
	CDes16Parameter* self = 
		new(ELeave) CDes16Parameter(aParam, aParamIndex, aMessage, aValidationFn);
	
	return self;
	}

/**
Constructor for CDes16WriteParameter class.
@param aParam Parameter details to be encapsulated by object
@param aParamIndex The Index of this parameter within the RMessage2 arguments
@param aMessage The RMessage2 object containing the parameter to be represented
@panic ECMPanicBadMessageSchema in UDEB if the schema for this parameter is 
		incorrectly defined
*/
CDes16Parameter::CDes16Parameter(const TParameterDetails& aParam, TInt aParamIndex,
		const RMessage2& aMessage, TCustomValidationFn aValidationFn)
	: CMessageParameterBase(aParam, aParamIndex, aMessage, aValidationFn)
	{
	__ASSERT_DEBUG((iParamDetails.iMin >= 0), 
			PanicServer(ECMPanicBadMessageSchema));
	__ASSERT_DEBUG((iParamDetails.iMax >= 0), 
			PanicServer(ECMPanicBadMessageSchema));
	}

/**
Destructor for CDes16WriteParameter class.
*/
CDes16Parameter::~CDes16Parameter()
	{
	
	}

/**
Validates given message argument against constraints
represented by this object.
@leave	KErrBadDescriptor if the message parameter does not conform
		to the constraints represented by this object
@leave	Any system-wide error code
*/
void CDes16Parameter::ValidateL()
	{	
	
	//if there is a supplied custom validation function, call that now
	if(iValidationFn != NULL)
		{
		iValidationFn(this);
		}	
	else
		{		
		TInt length = iMessage.GetDesLengthL(iIndex);
		TInt maxLength = iMessage.GetDesMaxLengthL(iIndex);
		
		if((maxLength < iParamDetails.iMin)||(length > iParamDetails.iMax))
			{
			User::Leave(KErrBadDescriptor);
			}
		}
	}

/**
Gets the length of the descriptor in the client message
@return	The length of the descriptor
@leave	KErrBadDescriptor if the message argument is not a descriptor type
		Any other system wide error code
*/
TInt CDes16Parameter::GetDesLengthL()
	{
	return iMessage.GetDesLengthL(iIndex);
	}

/**
Gets the max length of the descriptor in the client message
@return	The max length of the descriptor
@leave	KErrBadDescriptor if the message argument is not a descriptor type
		Any other system wide error code
*/
TInt CDes16Parameter::GetDesMaxLengthL()
	{
	return iMessage.GetDesMaxLengthL(iIndex);
	}

/**
Reads a descriptor from the requested message argument
@param aDes The target descriptor.
@param aOffset The offset from the start of the clients descriptor
@leave	Any system wide error code.
@panic ECMPanicBadDescriptor in UDEB if the supplied descriptor is too small. 
*/
void CDes16Parameter::ReadL(TDes& aDes,	TInt aOffset)
	{
	__ASSERT_DEBUG((aDes.MaxLength() >= (iMessage.GetDesLengthL(iIndex) - aOffset)), 
			PanicServer(ECMPanicBadDescriptor));
	
	iMessage.ReadL(iIndex,aDes,aOffset);
	}

/**
Writes a descriptor to the requested message argument
@param aDes The source descriptor containing the data to be written.
@param aOffset The offset from the start of the clients descriptor
@leave	Any system wide error code.
*/
void CDes16Parameter::WriteL(const TDesC& aDes,	TInt aOffset)
	{
	iMessage.WriteL(iIndex,aDes,aOffset);
	}

/**
Factory function for instantiating CPckgParameter objects
@param aParam Parameter details object used to construct object.
@param aParamIndex The Index of this parameter within the RMessage2 arguments
@param aMessage The RMessage2 object containing the parameter to be represented
@return A fully constructed CPckgParameter object.
@leave	Any system-wide error code.
*/
CMessageParameterBase* CPckgParameter::NewL(const TParameterDetails& aParam, 
		TInt aParamIndex, const RMessage2& aMessage, TCustomValidationFn aValidationFn )
	{
	CPckgParameter* self = 
		new(ELeave) CPckgParameter(aParam, aParamIndex, aMessage, aValidationFn);
	
	return self;
	}

/**
Constructor for CPckgParameter class.
@param aParam Parameter details to be encapsulated by object
@param aParamIndex The Index of this parameter within the RMessage2 arguments
@param aMessage The RMessage2 object containing the parameter to be represented
*/
CPckgParameter::CPckgParameter(const TParameterDetails& aParam, TInt aParamIndex,
		const RMessage2& aMessage, TCustomValidationFn aValidationFn)
	: CDes8Parameter(aParam, aParamIndex, aMessage, aValidationFn)
		
	{

	}

/**
Destructor for CPckgParameter class.
*/
CPckgParameter::~CPckgParameter()
	{
	}

/**
Validates given message argument against constraints
represented by this object.
@leave	KErrBadDescriptor if the message parameter does not conform
		to the constraints represented by this object
@leave	Any system-wide error code
*/
void CPckgParameter::ValidateL()
	{
	
	//if there is a supplied custom validation function, call that now
	if(iValidationFn != NULL)
		{
		iValidationFn(this);
		}
	
	else
		{
		TInt length = iMessage.GetDesLengthL(iIndex);
		
		if((length < iParamDetails.iMin)||(length > iParamDetails.iMax))
			{
			User::Leave(KErrBadDescriptor);
			}
		}
	}

/**
Factory function for instantiating CPtrParameter objects
@param aParam Parameter details object used to construct object.
@param aParamIndex The Index of this parameter within the RMessage2 arguments
@param aMessage The RMessage2 object containing the parameter to be represented
@return A fully constructed CPtrParameter object.
@leave	Any system-wide error code.
*/
CMessageParameterBase* CPtrParameter::NewL(const TParameterDetails& aParam, 
		TInt aParamIndex, const RMessage2& aMessage, TCustomValidationFn aValidationFn)
	{
	CPtrParameter* self = new(ELeave) CPtrParameter(aParam, aParamIndex, aMessage, aValidationFn);
	return self;
	}

/**
Constructor for CPtrParameter class.
@param aParamIndex The Index of this parameter within the RMessage2 arguments
@param aMessage The RMessage2 object containing the parameter to be represented
*/
CPtrParameter::CPtrParameter(const TParameterDetails& aParam, TInt aParamIndex,
		const RMessage2& aMessage, TCustomValidationFn aValidationFn)
	: CMessageParameterBase(aParam, aParamIndex, aMessage, aValidationFn)
	{
	
	}

/**
Validates given message argument against constraints
represented by this object. Stores the TAny* from the 
clients message to enable simple retrival when required.
@leave	KErrArgument if the argument index is invalid
@leave	Any system-wide error code
*/
void CPtrParameter::ValidateL()
	{

	switch(iIndex)
		{
	
		case 0:
			iValue = iMessage.Ptr0();
			break;
		
		case 1:
			iValue = iMessage.Ptr1();
			break;
		
		case 2:
			iValue = iMessage.Ptr2();
			break;
			
		case 3:
			iValue = iMessage.Ptr3();
			break;
		
		default:
			User::Leave(KErrArgument);
			break;
		}
	
	//if there is a supplied custom validation function, call that now
	if(iValidationFn != NULL)
		{
		iValidationFn(this);
		}
	}

/**
Retrieves the TAny pointer read from the clients message during validation
@return The TAny pointer read from the client message
*/
const TAny* CPtrParameter::GetPtrL()
	{
	return iValue;
	}

/**
Decode the string
@param aSrcString Source string
@param rDestString Destination string
@return  1 if aSrcString is not long enough to decode fully, resulting in the storage of
	the last character and requiring another aSrcString (poss 0 length) to be passed to it to 
	clear this character. 
@return 0 if the line was decoded OK or the end of the encoded file is reached ie "="
*/

EXPORT_C  TInt Base64Codec::Decode(const TDesC8& aSrcString, TDes8& rDestString)
	{
	TInt shiftStored = 0;
	TInt maskShiftStored = ESix;
	
	TInt decodedInt=0;
	TInt8 offsetChar=0;
	TUint8 decodedChar=0;
	 
	// Clears the destination string
	rDestString.Zero();

	// Initialise variables
	const TUint8* srcStringPtr=aSrcString.Ptr();
	const TUint8* srcStringEnd=aSrcString.Length()+srcStringPtr;
	TUint8* destStringPtr=(TUint8*)rDestString.Ptr();
	TUint8* destStringPtrBase=destStringPtr;

	TInt maskShift=maskShiftStored;
	TInt shiftStorage=shiftStored;
	
	// Main character process loop
	while(srcStringPtr<srcStringEnd)	
		{
		offsetChar=(TInt8)(*srcStringPtr-KImcvLookUpStartOffset);
		srcStringPtr++;

		// Check for valid B64 character		
		if((offsetChar>=0)&&(offsetChar<80))
			{
			// Read in next character and B64 decode
			decodedInt=AsciiToBase64[offsetChar];

			// Exits when a PAD char is reached
			if(decodedInt==EPadChar)
				{
				rDestString.SetLength((TInt)(destStringPtr-destStringPtrBase));
				return EFalse;
				}

			// Ensures the first 2 chars of 4 are received before processing
			if(maskShift==ESix)
				maskShift=EFour;
			else
				{
				shiftStorage=shiftStorage<<ESix;
				shiftStorage=shiftStorage|decodedInt;
				decodedChar=(TUint8)((shiftStorage>>maskShift)&EEightBitMask);
				
				if((maskShift-=ETwo)<EZero)
					maskShift=ESix; 
				
				*destStringPtr++=decodedChar;
				}
			shiftStorage=decodedInt;
			}
		}
	shiftStored=shiftStorage;
	maskShiftStored=maskShift;
	
	rDestString.SetLength((TInt)(destStringPtr-destStringPtrBase));
	
	return maskShift<ESix;
	}

/**
Encode the string
@param aSrcString Source string
@param rDestString Destination string
@return  1 if aSrcString is not long enough to encode fully
@return 0 if the line was encoded OK
*/
EXPORT_C TInt Base64Codec::Encode(const TDesC8& aSrcString, TDes8& rDestString)
	{
	// Clears the destination string
	rDestString.Zero();
	
	// Initialise variables
	const TUint8* srcStringPtr=aSrcString.Ptr();
	const TUint8* srcStringEnd=aSrcString.Length()+srcStringPtr;
	TUint8* destStringPtr=(TUint8*)rDestString.Ptr();
	TUint8* destStringPtrBase=destStringPtr;

	TInt character=0;
	TUint8 encodedChar=0;
	TInt charStorage=0;
 	TInt maskShift=EZero;
	TInt destStringCharNum = 0;

	while(srcStringPtr<=srcStringEnd)
		{
		// maskShift is used as a char read counter
		if(maskShift==ESix)
			{
			// If the 3rd char read is also the last char then the while loop
			// is broken on the next check.
			if(srcStringPtr==srcStringEnd)
				srcStringPtr++;
			maskShift=EZero;
			character=0;   
			}
		else
			{
			if(srcStringPtr==srcStringEnd)
				character=0;
			else
				character=*srcStringPtr;

			srcStringPtr++;
			// Shifts charStorage ready for the next char
			charStorage=charStorage<<8;
			maskShift+=ETwo;
			}
		charStorage=charStorage|character;
		// Shifts the mask to the correct bit location
		// Masks (AND's) the valid bits from charStorage
		// Shifts the valid bits into the low order 8bits
		// Converts to BASE64 char, Casts the result to an unsigned char (which it should be ?....I hope)
		encodedChar=(TUint8)Base64ToAscii[((charStorage>>maskShift)&ESixBitMask)];

		*destStringPtr++=encodedChar;
		destStringCharNum++;

		// Add a CRLF every KMaxB64EncodedCharsPerLine characters so as not to exceed the line length
		// limitation specified in RFC 2822.
		if (destStringCharNum == KMaxB64EncodedCharsPerLine)
			{
			destStringCharNum = 0;
			*destStringPtr++ = '\r';
			*destStringPtr++ = '\n';
			}
		}
	
	// Check for not enough chars and pad if required
	if (maskShift==EFour)
		{
		*destStringPtr++=KImcvConvEquals;
		*destStringPtr++=KImcvConvEquals;
		}
	else
		if(maskShift==ESix)
			*destStringPtr++=KImcvConvEquals;	
			
	rDestString.SetLength((TInt)(destStringPtr-destStringPtrBase));
	return ((TInt)(srcStringPtr-srcStringEnd));
	} 
