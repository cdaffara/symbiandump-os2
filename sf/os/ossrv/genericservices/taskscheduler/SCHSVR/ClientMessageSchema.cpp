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

#include <bsul/bsul.h>
#include "CSCHCODE.H"
#include "SCHINFO.H"

using namespace BSUL;

/**
Intended Usage : This Enum lists the currently supported Custom Parameter types.  
The lower 16 bits of these enum values are used as indices to the 
Parameter Factory Function which instantiates the appropriate 
CMessageParammeterBase derived object to represent a given message parameter.  
The upper 16 bits of these values are used as the index in the array 
of custom validation functions.  If the upper 16bits are set to 0, the default
validation is used.
*/
enum TCustomParamType
	{
	EParamTScheduleSettings2 = (0x10000 | EParamPckg),
	EParamTTsTime = (0x20000 | EParamPckg),
	EParamTTaskInfo = (0x30000 | EParamPckg),
	EParamTScheduleInfo = (0x40000 | EParamPckg),
	EParamTSchedulerItemRef = (0x50000 | EParamPckg)
	};

/* These are the parameter description sets for each message to be handled by the
 * server. These descriptions provide the type of the parameter and constraints
 * used to validate messages recieved from the client.  The particular validation
 * requirements are defined in the appropriate CMessageParameterBase derived class
 * and parameter objects are instantiated via the Parameter factory Lookup table.
 */
const TInt KMaxInt32 = 	0x7FFFFFFF;
const TInt KMinInt32 = 	0x80000000;
const TInt KMaxTDes8 = 0x0FFFFFFF;
const TInt KMaxSchedules = 25000;//Max of 25000 schedules
const TInt KMaxScheduleId = (10000 * (KMaxSchedules - 1));//Schedule IDs have an offset of 10000
const TInt KMaxTasks = 9999;//Max of 9999 tasks per schedule
const TInt KMaxTaskId = (KMaxScheduleId + KMaxTasks);

const TParameterDetails KIntParam[] = {EParamInt,KMinInt32,KMaxInt32};


const TParameterDetails KRegisterClientParam[] = 	{{EParamDes16Read,0,KMaxFileName},
													{EParamInt,0,KMaxInt32}};

const TParameterDetails KCreateTimeSchedParam[] = 	{{EParamTScheduleSettings2,sizeof(TScheduleSettings2),sizeof(TScheduleSettings2)},
													{EParamDes8Read,0,KMaxTDes8},
													{EParamPckg,sizeof(TInt),sizeof(TInt)}};

const TParameterDetails KCreateCondSchedParam[] = 	{{EParamTScheduleSettings2,sizeof(TScheduleSettings2),sizeof(TScheduleSettings2)},
													{EParamDes8Read,0,KMaxTDes8},
													{EParamTTsTime, sizeof(TTsTime), sizeof(TTsTime)},
													{EParamPckg,sizeof(TInt),sizeof(TInt)}};

const TParameterDetails KScheduleTaskParam[] = 		{{EParamTTaskInfo,sizeof(TTaskInfo),sizeof(TTaskInfo)},
													{EParamInt,0,KMaxScheduleId},
													{EParamPckg,sizeof(TInt),sizeof(TInt)},
													{EParamDes16,0,KMaxTDes8}};

const TParameterDetails KScheduleIdParam[] = 		{EParamInt,0,KMaxScheduleId};

const TParameterDetails KEditTimeSchedParam[] = 	{{EParamInt,0,KMaxTasks},
													{EParamInt,0,KMaxScheduleId},
													{EParamDes8Read,0,KMaxTDes8}};

const TParameterDetails KEditCondSchedParam[] = 	{{EParamInt,0,KMaxTasks},
													{EParamInt,0,KMaxScheduleId},
													{EParamDes8Read,0,KMaxTDes8},
													{EParamTTsTime, sizeof(TTsTime), sizeof(TTsTime)}};

const TParameterDetails KGetSchedRefsParam[] = 		{{EParamInt,0,KMaxSchedules},
													{EParamInt,EAllSchedules,EPendingSchedules},
													{EParamDes8,0,KMaxTDes8}};

const TParameterDetails KCountSchedulesParam[] = 	{{EParamPckg,sizeof(TInt),sizeof(TInt)},
													{EParamInt,EAllSchedules,EPendingSchedules}};

const TParameterDetails KGetSchedInfoParam[] = 		{{EParamInt,0,KMaxScheduleId},
													{EParamDes8,sizeof(TScheduleInfo), sizeof(TScheduleInfo)},
													{EParamDes8, 0, sizeof(TTsTime)}};

const TParameterDetails KGetTimeSchedDataParam[] = 	{{EParamInt,0,KMaxScheduleId},
													{EParamTScheduleInfo,sizeof(TScheduleInfo), sizeof(TScheduleInfo)},
													{EParamDes8,0,KMaxTDes8}};

const TParameterDetails KGetCondSchedDataParam[] = 	{{EParamInt,0,KMaxScheduleId},
													{EParamTScheduleInfo,sizeof(TScheduleInfo), sizeof(TScheduleInfo)},
													{EParamDes8,0,KMaxTDes8},
													{EParamDes8, 0, sizeof(TTsTime)}};

const TParameterDetails KGetTaskDataParam[] = 		{{EParamInt,0,KMaxScheduleId},
													{EParamTScheduleInfo,sizeof(TScheduleInfo), sizeof(TScheduleInfo)},
													{EParamDes8,0,KMaxTDes8}};

const TParameterDetails KGetTaskRefsParam[] = 		{{EParamInt,0,KMaxTasks},
													{EParamInt,EAllSchedules,EPendingSchedules},
													{EParamInt,EAllTasks,EMyTasks},
													{EParamDes8,0,KMaxTDes8}};

const TParameterDetails KCountTasksParam[] = 		{{EParamPckg,sizeof(TInt),sizeof(TInt)},
													{EParamInt,EAllSchedules,EPendingSchedules},
													{EParamInt,EAllTasks,EMyTasks}};

const TParameterDetails KGetTaskParam[] = 			{{EParamInt,KMinInt32,KMaxInt32},
													{EParamDes8,0,sizeof(TTaskInfo)},
													{EParamInt,KMinInt32,KMaxInt32},
													{EParamDes16,0,KMaxTDes8}};

const TParameterDetails KGetScheduledItemRefParam[] = {{EParamInt,0,KMaxTaskId},
													{EParamDes8,0, sizeof(TSchedulerItemRef)},
													{EParamDes8,0, sizeof(TTsTime)}};

const TParameterDetails KDeleteTaskParam[] = 		{EParamInt,KMinInt32,KMaxInt32};

const TParameterDetails KGetTaskDataSizeParam[] = 	{{EParamInt,KMinInt32,KMaxInt32},
													{EParamPckg,sizeof(TInt),sizeof(TInt)}};

const TParameterDetails KGetScheduleTypeParam[] = 	{{EParamInt,0,KMaxScheduleId},
													{EParamPckg,sizeof(TInt),sizeof(TInt)}};


/* This Security Policy defines the requirements for a client to interact
 * with the server. If a client sends a request to the server and doesnt
 * meet the security policy, the request is denied with KErrAccessDenied.
 * For the bakup server no policy is required so a default always pass
 * policy is used.
 */
_LIT_SECURITY_POLICY_PASS(KDefaultPolicy);

/* This is the array of messages that will be accepted by the server.  
 * The messages are defined as follows:
 *  	{Function Number, Security Policy, Number of Parameters, Parameter Descriptions}
 * A Binary search is used to locate the messages in the table therefore
 * the messages MUST appear in the list below sorted by function number.
 */
const TClientMessageSchema KClientMessages[] = 
				{
				MESSAGE_SCHEMA(ERegisterClient,KDefaultPolicy,KRegisterClientParam),
				MESSAGE_SCHEMA(ECreateTimeSchedule,KDefaultPolicy,KCreateTimeSchedParam),
				MESSAGE_SCHEMA(ECreateConditionSchedule,KDefaultPolicy,KCreateCondSchedParam),
				MESSAGE_SCHEMA(EScheduleTask,KDefaultPolicy,KScheduleTaskParam),
				MESSAGE_SCHEMA(EDisableSchedule,KDefaultPolicy,KScheduleIdParam),
				MESSAGE_SCHEMA(EEnableSchedule,KDefaultPolicy,KScheduleIdParam),
				MESSAGE_SCHEMA(EDeleteSchedule,KDefaultPolicy,KScheduleIdParam),
				MESSAGE_SCHEMA(EEditTimeSchedule,KDefaultPolicy,KEditTimeSchedParam),
				MESSAGE_SCHEMA(EEditConditionSchedule,KDefaultPolicy,KEditCondSchedParam),
				MESSAGE_SCHEMA(EGetScheduleInfo,KDefaultPolicy,KGetSchedInfoParam),
				MESSAGE_SCHEMA(EGetTimeScheduleData,KDefaultPolicy,KGetTimeSchedDataParam),
				MESSAGE_SCHEMA(EGetConditionScheduleData,KDefaultPolicy,KGetCondSchedDataParam),
				MESSAGE_SCHEMA(EGetTaskData,KDefaultPolicy,KGetTaskDataParam),
				MESSAGE_SCHEMA(ECountSchedules,KDefaultPolicy,KCountSchedulesParam),
				MESSAGE_SCHEMA(ECountTasks,KDefaultPolicy,KCountTasksParam),
				MESSAGE_SCHEMA(EGetTaskRefs,KDefaultPolicy,KGetTaskRefsParam),
				MESSAGE_SCHEMA(EGetTask,KDefaultPolicy,KGetTaskParam),
				MESSAGE_SCHEMA(EGetTaskDataSize,KDefaultPolicy,KGetTaskDataSizeParam),
				MESSAGE_SCHEMA(EGetScheduleRefs,KDefaultPolicy,KGetSchedRefsParam),
				MESSAGE_SCHEMA(EDeleteTask,KDefaultPolicy,KDeleteTaskParam),
				MESSAGE_SCHEMA(EGetSchedulerItemRefAndNextDueTime,KDefaultPolicy,KGetScheduledItemRefParam),
				MESSAGE_SCHEMA(EGetScheduleType,KDefaultPolicy,KGetScheduleTypeParam),
				MESSAGE_SCHEMA_NO_PARAMS(ESchDbgMarkHeap,KDefaultPolicy),
				MESSAGE_SCHEMA(ESchDbgCheckHeap,KDefaultPolicy,KIntParam),
				MESSAGE_SCHEMA(ESchDbgMarkEnd,KDefaultPolicy,KIntParam),
				MESSAGE_SCHEMA(ESchDbgFailNext,KDefaultPolicy,KIntParam),
				MESSAGE_SCHEMA_NO_PARAMS(ESchDbgResetHeap,KDefaultPolicy),
				MESSAGE_SCHEMA_NO_PARAMS(ESchFaultServer,KDefaultPolicy)
				};


static void ValidateTTsTimeL(CMessageParameterBase* aParameter);
static void ValidateTTaskInfoL(CMessageParameterBase* aParameter);
static void ValidateTScheduleInfoL(CMessageParameterBase* aParameter);
static void ValidateTSchedulerItemRefL(CMessageParameterBase* aParameter);
static void ValidateTScheduleSettingsL(CMessageParameterBase* aParameter);

/* This is the array of custom validation functions that are used to validate
 * parameters based on CPckgParameter parameter types
 */
const TCustomValidationFn KCustomValidationFunctions[] = {NULL,
															&ValidateTScheduleSettingsL,
															&ValidateTTsTimeL,
															&ValidateTTaskInfoL,
															&ValidateTScheduleInfoL,
															&ValidateTSchedulerItemRefL
															};

/*This struct defines the server data to be passed to the message framework on
 * initialisation
 */
extern const TClientMessageServerData KServerData = SERVER_DATA(KClientMessages,
																KCustomValidationFunctions,
																"Task Scheduler",
																ESrvFlagLogBadMessages);
	 
/*
Custom validation function for EParamTTsTime parameters.
Checks that the TPckg passed in to the server is exactly the size 
defined in the schema. Reads the value from the clients message to 
ensure the data is valid.
@param aParameter The parameter object to be validated
@leave KErrBadDescriptor if the TPckg passed in to the server is not 
		the expected size	
@leave Any other system-wide error code.
*/
void ValidateTTsTimeL(CMessageParameterBase* aParameter)
	{
	TTsTime time;
	TPckg<TTsTime> pTime(time);
	
	TInt length = aParameter->GetDesLengthL();
	
	if((length < aParameter->Min())||(length > aParameter->Max()))
		{
		User::Leave(KErrBadDescriptor);
		}
	
	aParameter->ReadL(pTime,0);
		
	}

/*
Custom validation function for EParamTTaskInfo parameters.
Checks that the TPckg passed in to the server is exactly the size 
defined in the schema. Reads the value from the clients message to 
ensure the data is valid.
@param aParameter The parameter object to be validated
@leave KErrBadDescriptor if the TPckg passed in to the server is not 
		the expected size	
@leave Any other system-wide error code.
*/
void ValidateTTaskInfoL(CMessageParameterBase* aParameter)
	{
	TTaskInfo taskInfo;
	TPckg<TTaskInfo> pTaskInfo(taskInfo);
	
	TInt length = aParameter->GetDesLengthL();
	
	if((length < aParameter->Min())||(length > aParameter->Max()))
		{
		User::Leave(KErrBadDescriptor);
		}
	
	aParameter->ReadL(pTaskInfo,0);
	}

/*
Custom validation function for EParamTScheduleInfo parameters.
Checks that the TPckg passed in to the server is exactly the size 
defined in the schema. Reads the value from the clients message to 
ensure the data is valid.
@param aParameter The parameter object to be validated
@leave KErrBadDescriptor if the TPckg passed in to the server is not 
		the expected size	
@leave Any other system-wide error code.
*/
void ValidateTScheduleInfoL(CMessageParameterBase* aParameter)
	{
	TScheduleInfo scheduleInfo;
	TPckg<TScheduleInfo> pScheduleInfo(scheduleInfo);
	
	TInt length = aParameter->GetDesLengthL();
	
	if((length < aParameter->Min())||(length > aParameter->Max()))
		{
		User::Leave(KErrBadDescriptor);
		}
	
	aParameter->ReadL(pScheduleInfo,0);
	}

/*
Custom validation function for EParamTSchedulerItemRef parameters.
Checks that the TPckg passed in to the server is exactly the size 
defined in the schema. Reads the value from the clients message to 
ensure the data is valid.
@param aParameter The parameter object to be validated
@leave KErrBadDescriptor if the TPckg passed in to the server is not 
		the expected size	
@leave Any other system-wide error code.
*/
void ValidateTSchedulerItemRefL(CMessageParameterBase* aParameter)
	{
	TSchedulerItemRef itemRef;
	TPckg<TSchedulerItemRef> pItemRef(itemRef);
	
	TInt length = aParameter->GetDesLengthL();
	
	if((length < aParameter->Min())||(length > aParameter->Max()))
		{
		User::Leave(KErrBadDescriptor);
		}
	
	aParameter->ReadL(pItemRef,0);
	}

/*
Custom validation function for EParamTScheduleSettings2 parameters.
Checks that the TPckg passed in to the server is exactly the size 
defined in the schema. Reads the value from the clients message to 
ensure the data is valid. Checks iEntryCount is > 0.
@param aParameter The parameter object to be validated
@leave KErrBadDescriptor if the TPckg passed in to the server is not 
		the expected size	
@leave Any other system-wide error code.
*/
void ValidateTScheduleSettingsL(CMessageParameterBase* aParameter)
	{
	TScheduleSettings2 settings;
	TPckg<TScheduleSettings2> pSettings(settings);
	
	TInt length = aParameter->GetDesLengthL();
	
	if((length < aParameter->Min())||(length > aParameter->Max()))
		{
		User::Leave(KErrBadDescriptor);
		}
	
	aParameter->ReadL(pSettings,0);
	
	if(settings.iEntryCount < 0)
		{
		User::Leave(KErrBadParameter);
		}

	}
