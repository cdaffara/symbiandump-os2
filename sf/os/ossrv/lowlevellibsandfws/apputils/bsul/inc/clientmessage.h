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

#if !defined(CLIENTMESSAGE_H)
#define CLIENTMESSAGE_H

#include <e32std.h>
#include <e32base.h>
#include <e32debug.h>
#include <babitflags.h>

namespace BSUL
	{
	
	//Version number of the client server framework code
	const TInt KClientMessageVersion = 0x1;
	
	//Forward Declarations
	class CMessageParameterBase;	
	
	//Const Declarations

	/**
	Returned to caller to indicate that the requested message number is not
	supported by the current server
	@publishedPartner
	@released
	*/
	const TInt KErrInvalidFunction 		= -1004; 
	
	/**
	Returned to caller if a message is passed in containing a non-descriptor
	parameter which doesn't meet the constraints specified in the schema
	@publishedPartner
	@released
	*/
	const TInt KErrBadParameter 		= -1005; 
	
	/**
	Returned to caller in UREL mode when the server attempts to call a 
	CMessageParameterBase function which is not defined for the given
	parameter type. E.g. calling GetIntL() on a Descriptor parameter.
	@publishedPartner
	@released
	*/
	const TInt KErrWrongParameterType 		= -1006; 
	
	/**
	Returned to caller in UREL mode when a message is found to be
	incorrectly defined in the message schema. 
	@publishedPartner
	@released
	*/
	const TInt KErrBadMessageSchema			= -1007; 
	
	
	/**
	Returned to caller to indicate that the requested parameter has not yet
	been validated
	@publishedPartner
	@released
	*/
	const TInt KErrNotValidated			= -1008; //Server internal error
	
	/**
	Returned to caller to indicate that the ClientMessage framework has
	not yet been initialised
	@publishedPartner
	@released
	*/
	const TInt KErrNotInitialised		= -1009; //Server internal error
	
	
	//Enum Declarations
	
	/**
	This is the list of Panic codes used by the ClientMessage framework
	@publishedPartner
	@released
	*/
	enum
		{
		/**This panic is raised in UDEB mode when the server attempts to call a 
		CMessageParameterBase function which is not defined for the given
		parameter type. E.g. calling GetIntL() on a Descriptor parameter.
		*/
		ECMPanicWrongParameterType = 0,
		
		/**This panic is raised when the server attempts to read from a
		descriptor in the clients process and supplies a target descriptor
		that is smaller than the client descriptor.
		*/
		ECMPanicBadDescriptor,
		
		/**This panic is raised when a TParameterDetails structure for a 
		given parameter is incorrectly defined in the message schema
		*/
		ECMPanicBadMessageSchema
		};	
	
	/**
	This Enum lists the currently supported Parameter types.  
	These enum values are used as indices to the Parameter Factory Function
	which instantiates the appropriate CMessageParammeterBase derived
	object to represent a given message parameter.
	@publishedPartner
	@released
	*/
	enum TParamType
		{
		EParamNull = 0,
		EParamInt,
		EParamDes8Read,
		EParamDes8,
		EParamPckg,
		EParamDes16Read,
		EParamDes16,
		EParamPtr
		};
	
	/**
	This enum lists the currently supported flags that can be passed to the server
	in TClientMessageServerData::iFlags.  Note that internally this value is stored
	in a TBitFlags32 type but this type cannot be statically initialised in a dll 
	so the value is passed to the framework in a TInt. The bottom 16 bit flags are 
	reserved for the framework's internal use.

	@publishedPartner
	@released
	*/
	enum TServerFlags
		{
		/** Default empty flag */
		ESrvFlagNone = 0,
		/** Setting this flag indicates that for bad message errors, request 
		should be completed with error code rather than panicking the client */
		ESrvFlagDoNotPanicClientOnBadMessageErrors = 0x40000000,
		/** Indicates that bad messages should be logged in UDEB */
		ESrvFlagLogBadMessages = 0x80000000
		};
	
	//Typedef Declarations
	
	/**
	This typedef is used to simplify the declaration of custom
	parameter validation functions
	@publishedPartner
	@released
	*/
	typedef void (*TCustomValidationFn)(CMessageParameterBase* aParameter);

	
	
	//Struct Declarations
		
	/** 
	This structure defines the format for storing information for an 
	individual message parameter. 
	 
	 iType - This defines the parameter type and is used by the CMessageParameterBase
	 			class to instantiate the appropriate derived class via the 
	 			KParameterFactoryFunctions factory lookup table.
	 
	 iMin, iMax - These are two constraints that are used in the ValidateL 
	 			method of a class derived from CMessageParameter to validate the message
	 			as required by that parameter.  
	 			The  constraints that are used depend on the type as follows:
	 			EParamInt:
	 				-iMin = Minimum int value accepted
	 				-iMax = Maximum int value accepted. This must be > iMin
	 				
	 				NOTE:	Currently unsigned integers are not explicitly supported,
	 						however 16 bit unsigned integers are implicitly supported
	 						by the EParamInt type.  Note that the constraint above
	 						(iMax > iMin) means that the maximum supported value for an 
	 						unsigned int is 0x7FFFFFFF.
	 							 				
	 			EParamDes8Read:
	 			EParamDes16Read:
	 				-iMin = Minimum length of descriptor in  clients address space.
	 						If no minimum restriction exists this should be set to 0 in
	 						the schema. This must be >= 0
	 				-iMax = Maximum length of descriptor in clients address space.  
	 						The descriptor supplied by the client must not have a 
	 						length that exceeds this value. This must be >= iMin
	 			EParamDes8:
	 			EParamDes16:
	 				-iMin =  Minimum MaxLength of descriptor in clients address space.
	 						 The descriptor supplied by the client must have a MaxLength
	 						 that is at least as big as this value. This must be >= 0
	 				-iMax =  Maximum length of descriptor in clients address space.  
	 						The descriptor supplied by the client must not have a 
	 						length that exceeds this value. This must be >= 0.
	 
	 				NOTE: 	As the EParamDes* types use MaxLength() as a restriction
	 						They cannot be used if the client intends to pass a TDesC 
	 						derived descriptor.  If the client intends read only use 
	 						then the explicit EParamDes*Read types should be used instead.
	 						
	 			EParamPckg:
	 				-iMin = Minimum length of descriptor in  clients address space.
	 						If no minimum restriction exists this should be set to 0 in
	 						the schema. This must be >= 0
	 				-iMax = Maximum length of descriptor in clients address space.  
	 						The descriptor supplied by the client must not have a 
	 						length that exceeds this value. This must be >= iMin
	 
	 			EParamPtr:
	  				-iMin = Unused
	 				-iMax = Unused
	@publishedPartner
	@released
	*/
	struct TParameterDetails
		{
		const TInt iType;
		const TInt iMin;
		const TInt iMax;	
		};
	
	/**
	This struct is used to represent a single message from the client.
	It defines the function number corresponding to the message, the security 
	policy for the message, the number of paramaters that are passed to the 
	function and the type and constraint information for each of the parameters 
	as described above. The server must define an array of these objects to be 
	used by CClientMessage to validate all incoming messages.
	
	NOTE: It is assumed that the message parameters are contiguous. I.E. if there is one
	parameter it is assumed to be located at index 0, if there are 2 parameters they 
	are located at index 0,1 etc.
	
	@publishedPartner
	@released
	*/
	struct TClientMessageSchema
		{
		
		//Version number of the framework
		const TInt iVersion;
		
		//The message number described by this schema
		const TInt iFunction;
		
		//The security policy for this message. The CClientMessage framework 
		//uses this security policy to police incoming messages.
		const TStaticSecurityPolicy& iPolicy;
		
		//The number of parameters expected by this message
		const TInt iParamCount;	
		
		//The array of TParameterDetails structs describing the 
		//parameter types and constraints for this message
		const TParameterDetails* iParams;
		
		//Reserved values for future proofing
		const TInt iReserved1;
		const TInt iReserved2;
		};	
	
	/**
	This struct is used to initialise the ClientMessage Framework.
	This struct provides the framework with the message schema and
	the custom validation functions for the server as well as the server 
	name and flags.
	The struct should be populated by the server using the SERVER_DATA 
	macro defined below.
	@publishedPartner
	@released
	*/
	struct TClientMessageServerData
		{
		//Version number of this structure
		const TInt iVersion;
		
		//The number of messages accepted by this server
		const TInt iMessageCount;
		
		//The array of TClientMessageSchema structs defining the schema for
		//each message accpeted by this server
		const TClientMessageSchema* iMessageSchema;
		
		//The number of custom validation fucntions suplpied by this server
		const TInt iValidationFnCount;
		
		//The array of custom validation functions supplied by this server.
		//These validation functions are called from ValidateL to provide 
		//customised validation of any parameter type.
		const TCustomValidationFn* iCustomValidationFns;
		
		//The name of the server using the framework.  This string is used both
		//in logging bad messages and in panicing the client or the server.
		//In line with Panic category strings, this value should be 16 characters or less.
		const TUint8* iServerName;
		
		//Flags used to pass settings info into the framework. The supported values are
		//defined in the TServerFlags enum above. 
		const TInt iFlags;
		
		//Reserved values for future proofing
		const TInt iReserved1;
		const TInt iReserved2;
		};
	
	//Macro Definitions
	
	/**
	This macro is used to simplify declaration of TClientMessageSchema entries.
	The function number, security policy and pointer to array of TParameterDetails
	structures is converted into the expected format for the declaration of a
	TClientMessageSchema structure.
	@publishedPartner
	@released
	*/	
	#define MESSAGE_SCHEMA(Function,Policy,Params) {KClientMessageVersion,Function,Policy,sizeof(Params)/sizeof(TParameterDetails),Params,0,0}

	
	/**
	This TParameterDetails structure is used to represent a message with no 
	parameters.  This is required as the schema expects a const TParameterDetails*.
	@publishedPartner
	@released
	*/
	const TParameterDetails KNoParams[1] = {{EParamNull,0,0}};	
		
	/**
	This macro is used to simplify declaration of TClientMessageSchema entries
	for functions with no parameters. Because the number of parameters is set 
	to 0, a CMessageParameterBase derived object is not instantiated for this 
	parameter type.
	@publishedPartner
	@released
	*/
	#define MESSAGE_SCHEMA_NO_PARAMS(Function,Policy) {KClientMessageVersion,Function,Policy,0,KNoParams,0,0}
		
	
	/**
	This macro is used to simplify declaration of the TClientMessageServerData structure.
	The pointer to the array of TClientMessageSchemas, pointer to array of 
	TCustomValidationFns, server name and server flags are converted into the 
	expected format for the declaration of a TClientMessageServerData structure.
	@publishedPartner
	@released
	*/	
	#define SERVER_DATA(ClientMessages,ValidationFns,ServerName,Flags){KClientMessageVersion,sizeof(ClientMessages)/sizeof(TClientMessageSchema),ClientMessages,sizeof(ValidationFns)/sizeof(TCustomValidationFn),ValidationFns,(TUint8*)ServerName,Flags,0,0}
	
		
	//Class Declarations
	
	/**
	This is the abstract base class for all message parameter classes. 
	The class defines one pure virtual function, ValidateL, which all derived classes 
	must implement.  This class also defines default implementation for the access
	methods which should be properly defined in all derived classes.
	This class is not intended for external derivation.
	@publishedPartner
	@released
	*/
	NONSHARABLE_CLASS(CMessageParameterBase) : public CBase
	{
	public:
		static CMessageParameterBase* CreateL(const TParameterDetails& aParam, TInt aParamIndex,
							const RMessage2& aMessage);
		virtual void ValidateL() = 0;
		virtual ~CMessageParameterBase(){};
		
		virtual TInt GetIntL();
		virtual const TAny* GetPtrL();
		virtual const TDesC8& GetDes8L();
		virtual const TDesC& GetDes16L();
		virtual TInt GetDesLengthL();
		virtual TInt GetDesMaxLengthL();
		virtual void ReadL(TDes8& aDes, TInt aOffset);
		virtual void ReadL(TDes& aDes, TInt aOffset);
		virtual void WriteL(const TDesC8& aDes, TInt aOffset);
		virtual void WriteL(const TDesC& aDes, TInt aOffset);
		
		virtual TInt Min();
		virtual TInt Max();
		
	protected:
		CMessageParameterBase(const TParameterDetails& aParam, TInt aParamIndex,
					const RMessage2& aMessage, TCustomValidationFn aValidationFn);
		
	private:
	static TCustomValidationFn GetValidationFunctionL(const TParameterDetails& aParam);
	
	protected:	
		//The index of this parameter within the corresponding RMessage2 object
		TInt iIndex;
		
		//A reference to the RMessage2 object that contains this parameter
		const RMessage2& iMessage;
		
		//A reference to the structure that defines the expected type
		//and constraints for this parameter
		const TParameterDetails& iParamDetails; 
		
		//A custom validation function to allow user defined validation
		//this function is called from ValidateL
		TCustomValidationFn iValidationFn;
		
	};

	/**
	This class is used by a server to sanitise incoming messages.  
	The class provides a wrapper around an RMessage2 object and provides a method 
	for validation of the message against the constraints defined in the message schema.  
	This class also provides methods for safely accessing the message arguments, 
	and for error handling and logging of bad messages.
	This class is not intended for derivation.
	@publishedPartner
	@released
	*/
	NONSHARABLE_CLASS(CClientMessage) : public CBase
	{
	public:
		IMPORT_C static void InitialiseFrameworkL(const TClientMessageServerData& aServerData);
		IMPORT_C static CClientMessage* NewL(const RMessage2& aMessage);
		IMPORT_C virtual ~CClientMessage();
		
		IMPORT_C TInt Function();
		IMPORT_C const RMessage2& Message();
		IMPORT_C virtual void ValidateL();
		IMPORT_C virtual void ValidateL(TInt aParam);
		IMPORT_C virtual void CompleteRequestL(TInt aError);
		IMPORT_C virtual void PanicClient(const TDesC& aServer, TInt aPanic);
		
		IMPORT_C virtual TInt GetIntL(TInt aParam);		
		IMPORT_C virtual const TAny* GetPtrL(TInt aParam);		
		IMPORT_C virtual const TDesC8& GetDes8L(TInt aParam);
		IMPORT_C virtual const TDesC& GetDes16L(TInt aParam);
		IMPORT_C virtual TInt GetDesLengthL(TInt aParam);
		IMPORT_C virtual TInt GetDesMaxLengthL(TInt aParam);	
		IMPORT_C virtual void ReadL(TInt aParam, TDes8& aDes, TInt aOffset = 0);
		IMPORT_C virtual void ReadL(TInt aParam, TDes16& aDes, TInt aOffset = 0);
		IMPORT_C virtual void WriteL(TInt aParam, const TDesC8& aDes, TInt aOffset = 0);
		IMPORT_C virtual void WriteL(TInt aParam, const TDesC16& aDes, TInt aOffset = 0);		
	
	private:
		virtual void LogBadMessageL(TInt aError);
		virtual void CheckSecurityPolicyL(const TSecurityPolicy& aPolicy);	
		CClientMessage(const RMessage2& aMessage,const TClientMessageServerData& aServerData);
		void ConstructL();
		const TClientMessageSchema* FindMessageSchema();
		void CheckValidatedL(TInt aParam);
		TBool LogBadMessages();
		
	private:
		//Array of pointers to CMessageParameterBase Derived parameter objects used
		// to validate and access the individual message arguments
		RPointerArray <CMessageParameterBase> iParameters;
		
		//Reference to the underlying RMessage2 wrapped by this object
		const RMessage2& iMessage;
		
		//Reference to TClientMessageServerData structure stored in TLS for
		//this thread.  This structure is passed in to the framework by the server
		//on initialisation.
		const TClientMessageServerData& iServerData;
		
		//Bit flags for use internally within CClientMessage.
		TBitFlags32 iFlags;
	};
	/**
	This class is used for Base64 based encoding and decoding .
	This class provides a method  for encoding and decoding
	@publishedPartner
	@released
	*/
	class Base64Codec 
	{
		// base64 and UU coding defines.
		/**
		This is the list of Bitmask used for encoding and decoding
		@publishedPartner
		@released
		*/
		enum EMaskValues
			{ 
			ESixBitMask = 0x3F, 
			EEightBitMask = 0xFF 
			};
		
		/**
		This is the list of shift values used for encoding and decoding
		@publishedPartner
		@released
		*/
		enum EMaskShiftValues
			{ 
			ESix = 6, 
			EFour = 4, 
			ETwo = 2, 
			EZero = 0
			};
		
		/**
		This enum is used as padding charcter
		@publishedPartner
		@released
		*/
		enum
		{ 
		EPadChar = 64 
		};
	public:
		IMPORT_C static TInt Encode(const TDesC8& aSrcString, TDes8& aDestString);
		IMPORT_C static TInt Decode(const TDesC8& aSrcString, TDes8& aDestString);
	};
	}//namespace

#endif
