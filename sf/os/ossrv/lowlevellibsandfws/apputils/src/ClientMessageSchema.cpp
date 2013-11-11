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

#include <bsul/clientmessage.h>
#include "babackup.h"
#include "backup_std.h"

using namespace BSUL;


/* These are the parameter description sets for each message to be handled by the
 * server. These descriptions provide the type of the parameter and constraints
 * used to validate messages recieved from the client.  The particular validation
 * requirements are defined in the appropriate CMessageParameterBase derived class
 * and parameter objects are instantiated via the Parameter factory Lookup table.
 */

//EBakOpCodeGetEvent
const TParameterDetails KMessage21Param[1] = {EParamDes16,KMaxFileName + 1,KMaxFileName + 1};
//EBakOpCodeCloseAllFiles
const TParameterDetails KMessage22Param[1] = {EParamInt,0,MBackupObserver::EReleaseLockNoAccess};
//EBakOpCodeCloseFile
const TParameterDetails KMessage24Param[3] = {{EParamInt,0,KMaxFileName},
											{EParamDes16Read,1,KMaxFileName},
												{EParamInt,0,MBackupObserver::EReleaseLockNoAccess}};
//EBakOpCodeRestartFile
const TParameterDetails KMessage25Param[2] = {{EParamInt,0,KMaxFileName},
											{EParamDes16Read,1,KMaxFileName}};
//EBakOpCodeNotifyLockChange
const TParameterDetails KMessage26Param[2] = {{EParamInt,0,KMaxFileName},
											{EParamDes16Read,1,KMaxFileName}};
//EBakOpCodeNotifyLockChangeCancel
const TParameterDetails KMessage27Param[2] = {{EParamInt,0,KMaxFileName},
											{EParamDes16Read,1,KMaxFileName}};

//EBakOpCodeNotifyBackupOperation 
const TParameterDetails KMessage29Param[1] = {EParamDes8Read,sizeof(TBackupOperationAttributes),sizeof(TBackupOperationAttributes)};

//EBakOpCodeGetBackupOperationState
const TParameterDetails KMessage31Param[1] = {EParamDes8,sizeof(TBool),sizeof(TBool)};

//EBakOpCodeGetBackupOperationEvent
const TParameterDetails KMessage33Param[1] = {EParamDes8,sizeof(TBackupOperationAttributes),sizeof(TBackupOperationAttributes)};

//EBakOpCodeSetBackupOperationObserverIsPresent
const TParameterDetails KMessage34Param[1] = {EParamInt,0,1};

/* This Security Policy defines the requirements for a client to interact
 * with the server. If a client sends a request to the server and doesnt
 * meet the security policy, the request is denied with KErrAccessDenied.
 * For the bakup server no policy is required so a default always pass
 * policy is used.
 */
_LIT_SECURITY_POLICY_PASS(KDefaultPolicy);

_LIT_SECURITY_POLICY_C1(KWriteDeviceDataPolicy, ECapabilityWriteDeviceData); 

/* This is the array of messages that will be accepted by the server.  
 * The messages are defined as follows:
 *  	{Function Number, Number of Parameters, Parameter Descriptions}
 * When loading the message objects, this table is traversed and so the most 
 * frequently called messages should be first in the list to ensure the most
 * efficient operation
 */


const TClientMessageSchema KClientMessages[] = {MESSAGE_SCHEMA_NO_PARAMS(EBakOpCodeEventReady, KDefaultPolicy),
												MESSAGE_SCHEMA(EBakOpCodeGetEvent, KDefaultPolicy, KMessage21Param),
												MESSAGE_SCHEMA(EBakOpCodeCloseAllFiles, KWriteDeviceDataPolicy, KMessage22Param),													
												MESSAGE_SCHEMA_NO_PARAMS(EBakOpCodeRestartAll, KWriteDeviceDataPolicy),
												MESSAGE_SCHEMA(EBakOpCodeCloseFile, KWriteDeviceDataPolicy, KMessage24Param),
												MESSAGE_SCHEMA(EBakOpCodeRestartFile, KWriteDeviceDataPolicy, KMessage25Param),
												MESSAGE_SCHEMA(EBakOpCodeNotifyLockChange, KDefaultPolicy, KMessage26Param),
												MESSAGE_SCHEMA(EBakOpCodeNotifyLockChangeCancel, KDefaultPolicy, KMessage27Param),
												MESSAGE_SCHEMA_NO_PARAMS(EBakOpCodeCloseServer, KDefaultPolicy),												
												MESSAGE_SCHEMA(EBakOpCodeNotifyBackupOperation, KWriteDeviceDataPolicy, KMessage29Param),
												MESSAGE_SCHEMA_NO_PARAMS(EBakOpCodeCancelOutstandingBackupOperationEvent, KDefaultPolicy),
												MESSAGE_SCHEMA(EBakOpCodeGetBackupOperationState, KDefaultPolicy, KMessage31Param),
												MESSAGE_SCHEMA_NO_PARAMS(EBakOpCodeBackupOperationEventReady, KDefaultPolicy),
												MESSAGE_SCHEMA(EBakOpCodeGetBackupOperationEvent, KDefaultPolicy, KMessage33Param),
												MESSAGE_SCHEMA(EBakOpCodeSetBackupOperationObserverIsPresent, KDefaultPolicy, KMessage34Param),
												MESSAGE_SCHEMA_NO_PARAMS(EBakOpCodeStopNotifications, KDefaultPolicy)
												};

const TCustomValidationFn KCustomValidationFunctions[] = {NULL};

extern const TClientMessageServerData KServerData = SERVER_DATA(KClientMessages,
																KCustomValidationFunctions,
																"Backup Server",
																ESrvFlagNone);

