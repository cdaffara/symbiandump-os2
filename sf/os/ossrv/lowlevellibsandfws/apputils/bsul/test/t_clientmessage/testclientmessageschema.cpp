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

#include <e32capability.h>
#include "t_clientmessagetestserver.h"

using namespace BSUL;

enum TCustomParamType
	{
	EParamCustom1 = (0x10000 | EParamPckg),
	EParamCustom2 = (0x20000 | EParamPckg),
	EParamCustom3 = (0x30000 | EParamInt),
	EParamCustom4 = (0x40000 | EParamDes16Read),
	EParamCustom5 = (0x90000 | EParamPckg)
	};


/* These are the parameter description sets for each message to be handled by the
 * server. These descriptions provide the type of the parameter and constraints
 * used to validate messages recieved from the client.  The particular validation
 * requirements are defined in the appropriate CMessageParameterBase derived class
 * and parameter objects are instantiated via the Parameter factory Lookup table.
 */
//Integer parameter set
const TParameterDetails KMessage0Params[2] = {{EParamInt,-10,100},{EParamInt,0,200}};

//These parameter sets are incorrectly defined. Attempts to call this message should
//panic the server.
const TParameterDetails KMessage2Params[1] = {{EParamInt,100,10}};
const TParameterDetails KMessage3Params[1] = {{EParamInt,-10,-20}};

//Des*Read parameter sets
const TParameterDetails KMessage4Params[2] = {{EParamDes8Read,2,16},{EParamDes16Read,0,32}};

//These parameter sets are incorrectly defined. Attempts to call this message should
//panic the server.
const TParameterDetails KMessage8Params[1] = {{EParamDes8Read,0,-16}};

//Des* parameter sets
const TParameterDetails KMessage9Params[4] = {{EParamDes8,64,64},
												{EParamInt,0,64},
												{EParamDes16,128,128},
												{EParamInt,0,128}};

const TParameterDetails KMessage11Params[4] = {{EParamDes8,64,64},
												{EParamInt,0,64},
												{EParamDes16,128,128},
												{EParamInt,0,128}};

//These parameter sets are incorrectly defined. Attempts to call this message should
//panic the server.
const TParameterDetails KMessage12Params[1] = {{EParamDes8,-64,64}};
const TParameterDetails KMessage13Params[1] = {{EParamDes8,64,-64}};

//Des parameter sets
const TParameterDetails KMessage14Params[3] = {{EParamDes8Read,0,16},
												{EParamInt,0,16},
												{EParamDes8,16,16}};

const TParameterDetails KMessage15Params[3] = {{EParamDes16Read,0,64},
												{EParamInt,0,64},
												{EParamDes16,64,64}};

//Ptr parameter sets
const TParameterDetails KMessage16Params[1] = {{EParamPtr,0,0}};


//Custom parameter sets
const TParameterDetails KMessage18Params[1] = {{EParamCustom1,sizeof(CMTestStruct2),sizeof(CMTestStruct2)}};
const TParameterDetails KMessage19Params[1] = {{EParamCustom2,sizeof(CMTestStruct1),sizeof(CMTestStruct1)}};

//Invalid parameter type
const TParameterDetails KMessage27Params[1] = {{453,0,0}};

//standard param types with custom validation functions
//Custom parameter sets
const TParameterDetails KMessage28Params[1] = {{EParamCustom3,0,0}};
const TParameterDetails KMessage29Params[1] = {{EParamCustom4,0,32}};

//Pkg parameter
const TParameterDetails KMessage30Params[1] = {{EParamPckg,sizeof(CMTestStruct1),sizeof(CMTestStruct1)}};

const TParameterDetails KMessage31Params[2] = {{EParamInt,EGetIntL,EWrite16L},{EParamPtr,0,0}};

//Custom parameter type. EParamCustom5 is incorrectly defined as the validation function
//index is outside the range of the supplied validation fuction array
const TParameterDetails KMessage32Params[1] = {{EParamCustom5,sizeof(CMTestStruct1),sizeof(CMTestStruct1)}};



/* This Security Policy defines the requirements for a client to interact
 * with the server. If a client sends a request to the server and doesnt
 * meet the security policy, the request is denied with KErrAccessDenied.
 * 
 */
_LIT_SECURITY_POLICY_PASS(KDefaultPolicy);
_LIT_SECURITY_POLICY_FAIL(KFailPolicy);
_LIT_SECURITY_POLICY_S0(KSidPassPolicy,0x10285B57);
_LIT_SECURITY_POLICY_S0(KSidFailPolicy,0x12345678);
_LIT_SECURITY_POLICY_C2(KCapFailPolicy,ECapabilityReadUserData,ECapabilityPowerMgmt);
_LIT_SECURITY_POLICY_C2(KCapPassPolicy,ECapabilityReadUserData,ECapabilityWriteUserData);

/* This is the array of messages that will be accepted by the server.  
 * The messages are defined as follows:
 *  	{Function Number, Number of Parameters, Parameter Descriptions}
 * When loading the message objects, this table is traversed and so the most 
 * frequently called messages should be first in the list to ensure the most
 * efficient operation
 */
const TClientMessageSchema KClientMessages[] = 
	{
	MESSAGE_SCHEMA(ETestMessage0,KDefaultPolicy,KMessage0Params),
	MESSAGE_SCHEMA(ETestMessage1,KDefaultPolicy,KMessage0Params),
	MESSAGE_SCHEMA(ETestMessage2,KDefaultPolicy,KMessage2Params),
	MESSAGE_SCHEMA(ETestMessage3,KDefaultPolicy,KMessage3Params),
	MESSAGE_SCHEMA(ETestMessage4,KDefaultPolicy,KMessage4Params),
	MESSAGE_SCHEMA(ETestMessage5,KDefaultPolicy,KMessage4Params),
	MESSAGE_SCHEMA(ETestMessage6,KDefaultPolicy,KMessage4Params),
	MESSAGE_SCHEMA(ETestMessage7,KDefaultPolicy,KMessage4Params),
	MESSAGE_SCHEMA(ETestMessage8,KDefaultPolicy,KMessage8Params),											 
	MESSAGE_SCHEMA(ETestMessage9,KDefaultPolicy,KMessage9Params),
	MESSAGE_SCHEMA(ETestMessage10,KDefaultPolicy,KMessage9Params),
	MESSAGE_SCHEMA(ETestMessage11,KDefaultPolicy,KMessage11Params),
	MESSAGE_SCHEMA(ETestMessage12,KDefaultPolicy,KMessage12Params),
	MESSAGE_SCHEMA(ETestMessage13,KDefaultPolicy,KMessage13Params),
	MESSAGE_SCHEMA(ETestMessage14,KDefaultPolicy,KMessage14Params),
	MESSAGE_SCHEMA(ETestMessage15,KDefaultPolicy,KMessage15Params),
	MESSAGE_SCHEMA(ETestMessage16,KDefaultPolicy,KMessage16Params),
	MESSAGE_SCHEMA(ETestMessage17,KDefaultPolicy,KMessage16Params),
	MESSAGE_SCHEMA(ETestMessage18,KDefaultPolicy,KMessage18Params),
	MESSAGE_SCHEMA(ETestMessage19,KDefaultPolicy,KMessage19Params),
	MESSAGE_SCHEMA(ETestMessage20,KFailPolicy,KMessage0Params),
	MESSAGE_SCHEMA(ETestMessage21,KSidPassPolicy,KMessage0Params),
	MESSAGE_SCHEMA(ETestMessage22,KSidFailPolicy,KMessage0Params),
	MESSAGE_SCHEMA(ETestMessage23,KCapPassPolicy,KMessage0Params),
	MESSAGE_SCHEMA(ETestMessage24,KCapFailPolicy,KMessage0Params),
	MESSAGE_SCHEMA_NO_PARAMS(ETestMessage25,KDefaultPolicy),
	MESSAGE_SCHEMA(ETestMessage26,KDefaultPolicy,KNoParams),
	MESSAGE_SCHEMA(ETestMessage27,KDefaultPolicy,KMessage27Params),
	MESSAGE_SCHEMA(ETestMessage28,KDefaultPolicy,KMessage28Params),
	MESSAGE_SCHEMA(ETestMessage29,KDefaultPolicy,KMessage29Params),
	MESSAGE_SCHEMA(ETestMessage30,KDefaultPolicy,KMessage30Params),
	MESSAGE_SCHEMA(ETestMessage31,KDefaultPolicy,KMessage31Params),
	MESSAGE_SCHEMA(ETestMessage32,KDefaultPolicy,KMessage32Params)
	};
														 
static void ValidationFn1L(CMessageParameterBase* aParameter);
static void ValidationFn2L(CMessageParameterBase* aParameter);
static void ValidationFn3L(CMessageParameterBase* aParameter);
static void ValidationFn4L(CMessageParameterBase* aParameter);

const TCustomValidationFn KCustomValidationFunctions[] = {NULL,
														&ValidationFn1L,
														&ValidationFn2L,
														&ValidationFn3L,
														&ValidationFn4L};

extern const TClientMessageServerData KServerData = SERVER_DATA(KClientMessages,
																KCustomValidationFunctions,
																"CMTestServer",
																ESrvFlagLogBadMessages);

void ValidationFn1L(CMessageParameterBase* aParameter)
	{
	CMTestStruct2 struct1;

	TPckg<CMTestStruct2> pData(struct1);
	
	if(aParameter->GetDesLengthL() != sizeof(CMTestStruct2))
		{
		User::Leave(KErrBadDescriptor);
		}
	
	aParameter->ReadL(pData,0);
	
	if((struct1.iEnum < ETestVal0)||(struct1.iEnum > ETestVal3))
		{
		User::Leave(KErrBadParameter);
		}
	
	if(struct1.iDes.Length() <= 0)
		{
		User::Leave(KErrBadParameter);
		}
	
	if(struct1.iCount < 0)
		{
		User::Leave(KErrBadParameter);
		}	
	}

void ValidationFn2L(CMessageParameterBase* aParameter)
	{
	CMTestStruct1 struct1;
	TPckg<CMTestStruct1> pData(struct1);
	
	aParameter->ReadL(pData,0);
	
	if(struct1.iInt <= 0)
		{
		User::Leave(KErrBadParameter);
		}
	}

void ValidationFn3L(CMessageParameterBase* aParameter)
	{
	
	TInt value = aParameter->GetIntL();
	
	if(value%4 != 0)
		{
		User::Leave(KErrBadParameter);
		}

	}

void ValidationFn4L(CMessageParameterBase* aParameter)
	{
	TBuf<32> data;
	
	aParameter->ReadL(data,0);
	
	if(data != _L("Expected Data"))
		{
		User::Leave(KErrBadParameter);
		}
	}


//parameter sets for server2
const TParameterDetails KMessage0Params2[2] = {{EParamInt,0,10},{EParamInt,-100,100}};

/* Descriptor parameter for server2 */
const TParameterDetails KMessage1Params2[1] = {{EParamDes8Read,0,16}};

/* This is the array of messages that will be accepted by the server.  
 * The messages are defined as follows:
 *  	{Function Number, Number of Parameters, Parameter Descriptions}
 */
const TClientMessageSchema KClientMessages2[] = {MESSAGE_SCHEMA(ETestMessage0,KDefaultPolicy,KMessage0Params2),
												MESSAGE_SCHEMA(ETestMessage1,KDefaultPolicy,KMessage1Params2)};

const TCustomValidationFn KCustomValidationFunctions2[] = {NULL};

extern const TClientMessageServerData KServer2Data = SERVER_DATA(KClientMessages2,
																KCustomValidationFunctions2,
																"CMTestServer2",
																(ESrvFlagLogBadMessages | ESrvFlagDoNotPanicClientOnBadMessageErrors));
