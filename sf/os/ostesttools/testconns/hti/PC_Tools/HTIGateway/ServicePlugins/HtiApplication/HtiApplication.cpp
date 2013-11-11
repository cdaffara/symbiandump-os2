/*
* Copyright (c) 2009 Nokia Corporation and/or its subsidiary(-ies). 
* All rights reserved.
* This component and the accompanying materials are made available
* under the terms of "Eclipse Public License v1.0"
* which accompanies this distribution, and is available
* at the URL "http://www.eclipse.org/legal/epl-v10.html".
*
* Initial Contributors:
* Nokia Corporation - initial contribution.
*
* Contributors:
* 
* Description:
*/
#include "HtiApplicationH.h"
#include "HtiPlugin.h"
#include "HtiSoapHandlerInterface.h"
#include "hticommon.h"

// Application control commands
// NOTE: Only unicode versions of commands are used.
//const int CMD_HTI_START_PROCESS         = 0x02;
const int CMD_HTI_START_PROCESS_U         = 0x03;
//const int CMD_HTI_STATUS_PROCESS        = 0x04;
const int CMD_HTI_STATUS_PROCESS_U        = 0x05;
const int CMD_HTI_STATUS_PROCESSID        = 0x06;
const int CMD_HTI_START_PROCESS_RET_VAL_U = 0x07;
//const int CMD_HTI_STOP_PROCESS          = 0x08;
const int CMD_HTI_STOP_PROCESS_U          = 0x09;
const int CMD_HTI_STOP_PROCESSID          = 0x0A;
const int CMD_HTI_GET_PROCESS_EXIT_CODE   = 0x0B;
//const int CMD_HTI_LIST_PROCESSES        = 0x0C;
const int CMD_HTI_LIST_PROCESSES_U        = 0x0D;
//const int CMD_HTI_START_PROCESS_RET_VAL = 0x0E;
//const int CMD_HTI_START_APP             = 0x10;
const int CMD_HTI_START_APP_U             = 0x11;
//const int CMD_HTI_START_APP_UID         = 0x12;
const int CMD_HTI_START_APP_UID_U         = 0x13;
//const int CMD_HTI_START_DOC             = 0x14;
const int CMD_HTI_START_DOC_U             = 0x15;
//const int CMD_HTI_STATUS_APP            = 0x16;
const int CMD_HTI_STATUS_APP_U            = 0x17;
//const int CMD_HTI_STATUS_DOC            = 0x18;
const int CMD_HTI_STATUS_DOC_U            = 0x19;
const int CMD_HTI_STATUS_APP_UID          = 0x1A;
//const int CMD_HTI_STOP_APP              = 0x1C;
const int CMD_HTI_STOP_APP_U              = 0x1D;
//const int CMD_HTI_STOP_DOC              = 0x1E;
const int CMD_HTI_STOP_DOC_U              = 0x1F;
const int CMD_HTI_STOP_APP_UID            = 0x20;
//const int CMD_HTI_LIST_APPS               = 0x24;
const int CMD_HTI_LIST_APPS_U             = 0x25;

// Application install commands
//const int CMD_HTI_INSTALL_SOFTWARE        = 0x30;
const int CMD_HTI_INSTALL_SOFTWARE_U        = 0x31;
const int CMD_HTI_UNINSTALL_SOFTWARE        = 0x32;
const int CMD_HTI_UNINSTALL_SOFTWARE_NAME_U = 0x33;
//const int CMD_HTI_UNINSTALL_SOFTWARE_NAME   = 0x34;

// Application control response codes
const int CMD_RESP_OK               = 0xF0;
const int CMD_RESP_NOT_FOUND        = 0xF1;
const int CMD_RESP_ALREADY_RUNNING  = 0xF2;
const int CMD_RESP_ALREADY_STOPPED  = 0xF3;
const int CMD_RESP_RUNNING          = 0xF4;
const int CMD_RESP_KILLED           = 0xF5;
const int CMD_RESP_PANIC            = 0xF6;

//**********************************************************************************
// HELPER FUNCTIONS
//
//**********************************************************************************

//**********************************************************************************
// ResponseCode2String
//**********************************************************************************
char* ResponseCode2String(struct soap* soap, BYTE code)
{
    char tmp[50]; // 50 should be more than enough

    switch(code)
    {
    case CMD_RESP_OK:               sprintf(tmp, "ok"); break;
    case CMD_RESP_NOT_FOUND:        sprintf(tmp, "not found"); break;
    case CMD_RESP_ALREADY_RUNNING:  sprintf(tmp, "already running"); break;
    case CMD_RESP_ALREADY_STOPPED:  sprintf(tmp, "already stopped"); break;
    case CMD_RESP_RUNNING:          sprintf(tmp, "running"); break;
    case CMD_RESP_KILLED:           sprintf(tmp, "killed"); break;
    case CMD_RESP_PANIC:            sprintf(tmp, "panic"); break;
    default:                        sprintf(tmp, "unknown response code 0x%02x", code);
    }

    char* text = (char*) soap_malloc(soap, strlen(tmp)+1); // +1 is for null character
    memcpy(text, tmp, strlen(tmp)+1);
    return text;
}

//**********************************************************************************
// ExitTypeCode2String
//**********************************************************************************
char* ExitTypeCode2String(struct soap* soap, BYTE code)
{
    char tmp[50]; // 50 should be more than enough

    switch(code)
    {
    case 0:  sprintf(tmp, "killed"); break;
    case 1:  sprintf(tmp, "terminated"); break;
    case 2:  sprintf(tmp, "panic"); break;
    case 3:  sprintf(tmp, "running"); break;
    default: sprintf(tmp, "unknown exit type code 0x%02x", code);
    }

    char* text = (char*) soap_malloc(soap, strlen(tmp)+1); // +1 is for null character
    memcpy(text, tmp, strlen(tmp)+1);
    return text;
}

//**********************************************************************************
// SOAP FUNCTIONS
//
//**********************************************************************************
//**********************************************************************************
// ns1__startProcess()
//**********************************************************************************
int ns1__startProcess(struct soap* soap,
                      wchar_t *fileName,
                      wchar_t *args,
                      struct startProcessResponse &r)
{
    // Check parameters
    if ( check_mandatory_wcstring_parameter( soap, fileName, "fileName" ) )
        return SOAP_FAULT;

    // Construct & send HTI message
    HtiMsgHelper msg( soap, HTI_APPLICATION_UID, CMD_HTI_START_PROCESS_U );
    msg.AddWCStringWithLengthByte( fileName );
    msg.AddWCStringWithLengthByte( args );
    if ( msg.SendReceiveMsg( HTIMSG_TIMEOUT_30_SECONDS ) )
        return SOAP_FAULT;

    // Check response
    if ( msg.CheckMsgMinLen( 1 ) )
        return SOAP_FAULT;

    r._returnstartProcessResult.status = ResponseCode2String( soap, msg.GetByte(0) );

    if( msg.GetByte( 0 ) == CMD_RESP_OK )
    {
        // pid only returned if found
        if ( msg.CheckMsgExactLen( 5 ) )
            return SOAP_FAULT;
        r._returnstartProcessResult.pid = msg.GetInt(1);
    }

    return SOAP_OK;
}

//**********************************************************************************
// ns1__stopProcess()
//**********************************************************************************
int ns1__stopProcess(struct soap* soap,
                     wchar_t *matchPattern,
                     char *&result)
{
    // Check parameters
    if ( check_mandatory_wcstring_parameter( soap, matchPattern, "matchPattern" ) )
        return SOAP_FAULT;

    // Construct & send HTI message
    HtiMsgHelper msg( soap, HTI_APPLICATION_UID, CMD_HTI_STOP_PROCESS_U );
    msg.AddWCStringWithLengthByte( matchPattern );
    if ( msg.SendReceiveMsg( HTIMSG_TIMEOUT_10_SECONDS ) )
        return SOAP_FAULT;

    // Check response
    if ( msg.CheckMsgExactLen( 1 ) )
        return SOAP_FAULT;

    result = ResponseCode2String( soap, msg.GetByte(0) );

    return SOAP_OK;
}

//**********************************************************************************
// ns1__stopProcessById()
//**********************************************************************************
int ns1__stopProcessById(struct soap* soap,
                         int pid,
                         char *&result)
{
    // Construct & send HTI message
    HtiMsgHelper msg( soap, HTI_APPLICATION_UID, CMD_HTI_STOP_PROCESSID );
    msg.AddInt( pid );
    if ( msg.SendReceiveMsg( HTIMSG_TIMEOUT_10_SECONDS ) )
        return SOAP_FAULT;

    // Check response
    if ( msg.CheckMsgExactLen( 1 ) )
        return SOAP_FAULT;

    result = ResponseCode2String( soap, msg.GetByte(0) );

    return SOAP_OK;
}

//**********************************************************************************
// ns1__getProcessStatus()
//**********************************************************************************
int ns1__getProcessStatus(struct soap* soap,
                          wchar_t *matchPattern,
                          char* &status)
{
    // Check parameters
    if ( check_mandatory_wcstring_parameter( soap, matchPattern, "matchPattern" ) )
        return SOAP_FAULT;

    // Construct & send HTI message
    HtiMsgHelper msg( soap, HTI_APPLICATION_UID, CMD_HTI_STATUS_PROCESS_U );
    msg.AddWCStringWithLengthByte( matchPattern );
    if ( msg.SendReceiveMsg( HTIMSG_TIMEOUT_10_SECONDS ) )
        return SOAP_FAULT;

    // Check response
    if ( msg.CheckMsgExactLen( 1 ) )
        return SOAP_FAULT;

    status = ResponseCode2String( soap, msg.GetByte(0) );

    return SOAP_OK;
}

//**********************************************************************************
// ns1__getProcessStatusById()
//**********************************************************************************
int ns1__getProcessStatusById(struct soap* soap,
                              int pid,
                              char* &status)
{
    // Construct & send HTI message
    HtiMsgHelper msg( soap, HTI_APPLICATION_UID, CMD_HTI_STATUS_PROCESSID );
    msg.AddInt( pid );
    if ( msg.SendReceiveMsg( HTIMSG_TIMEOUT_10_SECONDS ) )
        return SOAP_FAULT;

    // Check response
    if ( msg.CheckMsgExactLen( 1 ) )
        return SOAP_FAULT;

    status = ResponseCode2String( soap, msg.GetByte(0) );

    return SOAP_OK;
}

//**********************************************************************************
// ns1__startApp()
//**********************************************************************************
int ns1__startApp(struct soap* soap,
                  wchar_t *appName,
                  wchar_t *docName,
                  struct startAppResponse &r)
{
    // Check parameters
    if ( check_mandatory_wcstring_parameter( soap, appName, "appName" ) )
        return SOAP_FAULT;

    // Construct & send HTI message
    HtiMsgHelper msg( soap, HTI_APPLICATION_UID, CMD_HTI_START_APP_U );
    msg.AddWCStringWithLengthByte( appName );
    msg.AddWCStringWithLengthByte( docName );
    if ( msg.SendReceiveMsg( HTIMSG_TIMEOUT_10_SECONDS ) )
        return SOAP_FAULT;

    // Check response
    if ( msg.CheckMsgMinLen( 1 ) )
        return SOAP_FAULT;

    r._returnstartAppResult.status = ResponseCode2String( soap, msg.GetByte(0) );

    if( msg.GetByte( 0 ) == CMD_RESP_OK )
    {
        // threadId only returned if found
        if ( msg.CheckMsgExactLen( 5 ) )
            return SOAP_FAULT;
        r._returnstartAppResult.threadId = msg.GetInt(1);
    }

    return SOAP_OK;
}

//**********************************************************************************
// ns1__startAppByUid()
//**********************************************************************************
int ns1__startAppByUid(struct soap* soap,
                       int uid,
                       wchar_t *docName,
                       struct startAppByUidResponse &r)
{
    // Construct & send HTI message
    HtiMsgHelper msg( soap, HTI_APPLICATION_UID, CMD_HTI_START_APP_UID_U );
    msg.AddInt( uid );
    msg.AddWCStringWithLengthByte( docName );
    if ( msg.SendReceiveMsg( HTIMSG_TIMEOUT_10_SECONDS ) )
        return SOAP_FAULT;

    // Check response
    if ( msg.CheckMsgMinLen( 1 ) )
        return SOAP_FAULT;

    r._returnstartAppByUidResult.status = ResponseCode2String( soap, msg.GetByte(0) );

    if( msg.GetByte( 0 ) == CMD_RESP_OK )
    {
        // threadId only returned if found
        if ( msg.CheckMsgExactLen( 5 ) )
            return SOAP_FAULT;
        r._returnstartAppByUidResult.threadId = msg.GetInt(1);
    }

    return SOAP_OK;
}

//**********************************************************************************
// ns1__startDoc()
//**********************************************************************************
int ns1__startDoc(struct soap* soap,
                  wchar_t *docName,
                  struct startDocResponse &r)
{
    // Check parameters
    if ( check_mandatory_wcstring_parameter( soap, docName, "docName" ) )
        return SOAP_FAULT;

    // Construct & send HTI message
    HtiMsgHelper msg( soap, HTI_APPLICATION_UID, CMD_HTI_START_DOC_U );
    msg.AddWCStringWithLengthByte( docName );
    if ( msg.SendReceiveMsg( HTIMSG_TIMEOUT_10_SECONDS ) )
        return SOAP_FAULT;

    // Check response
    if ( msg.CheckMsgMinLen( 1 ) )
        return SOAP_FAULT;

    r._returnstartDocResult.status = ResponseCode2String( soap, msg.GetByte(0) );

    if( msg.GetByte( 0 ) == CMD_RESP_OK )
    {
        // threadId only returned if found
        if ( msg.CheckMsgExactLen( 5 ) )
            return SOAP_FAULT;
        r._returnstartDocResult.threadId = msg.GetInt(1);
    }

    return SOAP_OK;
}

//**********************************************************************************
// ns1__getAppStatus()
//**********************************************************************************
int ns1__getAppStatus(struct soap* soap,
                      wchar_t *appName,
                      char* &status)
{
    // Check parameters
    if ( check_mandatory_wcstring_parameter( soap, appName, "appName" ) )
        return SOAP_FAULT;

    // Construct & send HTI message
    HtiMsgHelper msg( soap, HTI_APPLICATION_UID, CMD_HTI_STATUS_APP_U );
    msg.AddWCStringWithLengthByte( appName );
    if ( msg.SendReceiveMsg( HTIMSG_TIMEOUT_10_SECONDS ) )
        return SOAP_FAULT;

    // Check response
    if ( msg.CheckMsgExactLen( 1 ) )
        return SOAP_FAULT;

    status = ResponseCode2String( soap, msg.GetByte(0) );

    return SOAP_OK;
}

//**********************************************************************************
// ns1__getAppStatusByUid()
//**********************************************************************************
int ns1__getAppStatusByUid(struct soap* soap,
                           int uid,
                           char* &status)
{
    // Construct & send HTI message
    HtiMsgHelper msg( soap, HTI_APPLICATION_UID, CMD_HTI_STATUS_APP_UID );
    msg.AddInt( uid );
    if ( msg.SendReceiveMsg( HTIMSG_TIMEOUT_10_SECONDS ) )
        return SOAP_FAULT;

    // Check response
    if ( msg.CheckMsgExactLen( 1 ) )
        return SOAP_FAULT;

    status = ResponseCode2String( soap, msg.GetByte(0) );

    return SOAP_OK;
}

//**********************************************************************************
// ns1__getAppStatusByDoc()
//**********************************************************************************
int ns1__getAppStatusByDoc(struct soap* soap,
                           wchar_t *docName,
                           char* &status)
{
    // Check parameters
    if ( check_mandatory_wcstring_parameter( soap, docName, "docName" ) )
        return SOAP_FAULT;

    // Construct & send HTI message
    HtiMsgHelper msg( soap, HTI_APPLICATION_UID, CMD_HTI_STATUS_DOC_U );
    msg.AddWCStringWithLengthByte( docName );
    if ( msg.SendReceiveMsg( HTIMSG_TIMEOUT_10_SECONDS ) )
        return SOAP_FAULT;

    // Check response
    if ( msg.CheckMsgExactLen( 1 ) )
        return SOAP_FAULT;

    status = ResponseCode2String( soap, msg.GetByte(0) );

    return SOAP_OK;
}

//**********************************************************************************
// ns1__stopApp()
//**********************************************************************************
int ns1__stopApp(struct soap* soap,
                 wchar_t *appName,
                 char *&result)
{
    // Check parameters
    if ( check_mandatory_wcstring_parameter( soap, appName, "appName" ) )
        return SOAP_FAULT;

    // Construct & send HTI message
    HtiMsgHelper msg( soap, HTI_APPLICATION_UID, CMD_HTI_STOP_APP_U );
    msg.AddWCStringWithLengthByte( appName );
    if ( msg.SendReceiveMsg( HTIMSG_TIMEOUT_10_SECONDS ) )
        return SOAP_FAULT;

    // Check response
    if ( msg.CheckMsgExactLen( 1 ) )
        return SOAP_FAULT;

    result = ResponseCode2String( soap, msg.GetByte(0) );

    return SOAP_OK;
}

//**********************************************************************************
// ns1__stopAppByUid()
//**********************************************************************************
int ns1__stopAppByUid(struct soap* soap,
                      int uid,
                      char *&result)
{
    // Construct & send HTI message
    HtiMsgHelper msg( soap, HTI_APPLICATION_UID, CMD_HTI_STOP_APP_UID );
    msg.AddInt( uid );
    if ( msg.SendReceiveMsg( HTIMSG_TIMEOUT_10_SECONDS ) )
        return SOAP_FAULT;

    // Check response
    if ( msg.CheckMsgExactLen( 1 ) )
        return SOAP_FAULT;

    result = ResponseCode2String( soap, msg.GetByte(0) );

    return SOAP_OK;
}

//**********************************************************************************
// ns1__stopAppByDoc()
//**********************************************************************************
int ns1__stopAppByDoc(struct soap* soap,
                      wchar_t *docName,
                      char *&result)
{
    // Check parameters
    if ( check_mandatory_wcstring_parameter( soap, docName, "docName" ) )
        return SOAP_FAULT;

    // Construct & send HTI message
    HtiMsgHelper msg( soap, HTI_APPLICATION_UID, CMD_HTI_STOP_DOC_U );
    msg.AddWCStringWithLengthByte( docName );
    if ( msg.SendReceiveMsg( HTIMSG_TIMEOUT_10_SECONDS ) )
        return SOAP_FAULT;

    // Check response
    if ( msg.CheckMsgExactLen( 1 ) )
        return SOAP_FAULT;

    result = ResponseCode2String( soap, msg.GetByte(0) );

    return SOAP_OK;
}

//**********************************************************************************
// ns1__listProcesses()
//**********************************************************************************
int ns1__listProcesses(struct soap* soap,
                       wchar_t *matchPattern,
                       ArrayOfHtiProcess *HtiProcesses)
{
    // Construct & send HTI message
    HtiMsgHelper msg( soap, HTI_APPLICATION_UID, CMD_HTI_LIST_PROCESSES_U );
    msg.AddWCStringWithLengthByte( matchPattern );
    if ( msg.SendReceiveMsg( HTIMSG_TIMEOUT_10_SECONDS ) )
        return SOAP_FAULT;

    // Check response
    if ( msg.CheckMsgMinLen( 1 ) )
        return SOAP_FAULT;

    if(msg.GetByte(0) != CMD_RESP_OK)
    {
        // This sould happen only if message is corrupted somehow...
        soap->error = soap_receiver_fault_format(soap, "HtiError",
            "response code error (0x%x)", msg.GetByte(0));
        return SOAP_FAULT;
    }

    // Fill & alloc soap stuff
    HtiProcesses->__size = (int) msg.GetWord(1);
    HtiProcesses->__ptr = soap_new_ns1__HtiProcess(soap, HtiProcesses->__size);

    // Go through processes
    int offset = 3;
    ns1__HtiProcess *process = HtiProcesses->__ptr;
    for(int i = 0; i < HtiProcesses->__size; i++)
    {
        process->processId = msg.GetInt(offset);//pid;
        process->processStatus = ResponseCode2String(soap, msg.GetByte(offset+4));

        // Get processname
        char* tmp = (char*) msg.GetMsgBody();
        tmp += offset+6;
        int processNameLen = msg.GetByte(offset+5);
        process->processName = (char*) soap_malloc(soap, processNameLen+1);
        memcpy(process->processName, tmp, processNameLen);
        process->processName[processNameLen] = 0x0; //  add null

        // for the next iteration
        offset += 4+1+1+processNameLen;
        process++;
    }

    return SOAP_OK;
}

//**********************************************************************************
// ns1__startProcessRetVal()
//**********************************************************************************
int ns1__startProcessRetVal(struct soap* soap,
                            wchar_t *fileName,
                            wchar_t *args,
                            struct startProcessResponse &r)
{
    // Check parameters
    if ( check_mandatory_wcstring_parameter( soap, fileName, "fileName" ) )
        return SOAP_FAULT;

    // Construct & send HTI message
    HtiMsgHelper msg( soap, HTI_APPLICATION_UID, CMD_HTI_START_PROCESS_RET_VAL_U );
    msg.AddWCStringWithLengthByte( fileName );
    msg.AddWCStringWithLengthByte( args );
    if ( msg.SendReceiveMsg( HTIMSG_TIMEOUT_30_SECONDS ) )
        return SOAP_FAULT;

    // Check response
    if ( msg.CheckMsgMinLen( 1 ) )
        return SOAP_FAULT;

    r._returnstartProcessResult.status = ResponseCode2String( soap, msg.GetByte(0) );

    if( msg.GetByte( 0 ) == CMD_RESP_OK )
    {
        // pid only returned if found
        if ( msg.CheckMsgExactLen( 5 ) )
            return SOAP_FAULT;
        r._returnstartProcessResult.pid = msg.GetInt(1);
    }

    return SOAP_OK;
}

//**********************************************************************************
// ns1__getProcessExitCode()
//**********************************************************************************
int ns1__getProcessExitCode(struct soap* soap,
                            int pid,
                            struct getProcessExitCodeResponse &r)
{
    // Construct & send HTI message
    HtiMsgHelper msg( soap, HTI_APPLICATION_UID, CMD_HTI_GET_PROCESS_EXIT_CODE );
    msg.AddInt( pid );
    if ( msg.SendReceiveMsg( HTIMSG_TIMEOUT_10_SECONDS ) )
        return SOAP_FAULT;

    // Check response
    if ( msg.CheckMsgMinLen( 7 ) )
        return SOAP_FAULT;
    if ( msg.GetByte( 0 ) != CMD_RESP_OK )
        return SOAP_FAULT;

    r._returngetProcessExitCodeResult.exitType = ExitTypeCode2String( soap, msg.GetByte( 1 ) );
    r._returngetProcessExitCodeResult.exitReason = msg.GetInt( 2 );
    r._returngetProcessExitCodeResult.exitCategory = msg.GetSoapString( 7, (int) msg.GetByte( 6 ) );

    return SOAP_OK;
}

//**********************************************************************************
// ns1__installSoftware()
//**********************************************************************************
int ns1__installSoftware(struct soap* soap,
                         wchar_t *installPackagePath,
                         bool upgrade,
                         bool optionalItems,
                         bool ocsp,
                         bool ignoreOCSPWarnings,
                         bool installUntrusted,
                         bool allowPackageInfo,
                         bool grantUserCapabilities,
                         bool killApp,
                         bool overwrite,
                         bool download,
                         wchar_t *username,
                         wchar_t *password,
                         char *drive,
                         unsigned char language,
                         bool usePhoneLanguage,
                         bool upgradeData,
                         struct ns1__installSoftwareResponse *out)
{
    // Check parameters
    if ( check_mandatory_wcstring_parameter(
             soap, installPackagePath, "installPackagePath" ) )
        return SOAP_FAULT;
    if ( check_mandatory_string_parameter( soap, drive, "drive" ) )
        return SOAP_FAULT;

    // Construct HTI message
    HtiMsgHelper msg( soap, HTI_APPLICATION_UID, CMD_HTI_INSTALL_SOFTWARE_U );
    msg.AddWCStringWithLengthByte( installPackagePath );
    msg.AddByte( upgrade );
    msg.AddByte( optionalItems );
    msg.AddByte( ocsp );
    msg.AddByte( ignoreOCSPWarnings );
    msg.AddByte( installUntrusted );
    msg.AddByte( allowPackageInfo );
    msg.AddByte( grantUserCapabilities );
    msg.AddByte( killApp );
    msg.AddByte( overwrite );
    msg.AddByte( download );
    msg.AddWCStringWithLengthByteZero( username );
    msg.AddWCStringWithLengthByteZero( password );
    msg.AddString( drive );
    msg.AddByte( language );
    msg.AddByte( usePhoneLanguage );
    msg.AddByte( upgradeData );

    // Send message
    if ( msg.SendReceiveMsg( HTIMSG_TIMEOUT_60_SECONDS ) )
        return SOAP_FAULT;

    // Check response
    if ( msg.CheckMsgExactLen( 1 ) != SOAP_OK )
        return SOAP_FAULT;
    if ( msg.GetByte( 0 ) != CMD_RESP_OK )
        return SOAP_FAULT;

    return SOAP_OK;
}

//**********************************************************************************
// ns1__uninstallSoftware()
//**********************************************************************************
int ns1__uninstallSoftware(struct soap* soap,
                           int uid,
                           bool killApp,
                           bool breakDependency,
                           enum ns1__mimeType  mimeType,
                           struct ns1__uninstallSoftwareResponse *out)
{
    // Construct HTI message
    HtiMsgHelper msg( soap, HTI_APPLICATION_UID, CMD_HTI_UNINSTALL_SOFTWARE );
    msg.AddInt( uid );
    msg.AddByte( killApp );
    msg.AddByte( breakDependency );
    msg.AddByte( mimeType );

    // Send message
    if ( msg.SendReceiveMsg( HTIMSG_TIMEOUT_60_SECONDS ) )
        return SOAP_FAULT;

    // Check response
    if ( msg.CheckMsgExactLen( 1 ) != SOAP_OK )
        return SOAP_FAULT;
    if ( msg.GetByte( 0 ) != CMD_RESP_OK )
        return SOAP_FAULT;

    return SOAP_OK;
}

//**********************************************************************************
// ns1__uninstallSoftwareByName()
//**********************************************************************************
int ns1__uninstallSoftwareByName(struct soap* soap,
                                 wchar_t *packageName,
                                 bool killApp,
                                 bool breakDependency,
                                 enum ns1__mimeType  mimeType,
                                 struct ns1__uninstallSoftwareByNameResponse *out)
{
    if ( check_mandatory_wcstring_parameter(
             soap, packageName, "packageName" ) )
        return SOAP_FAULT;

    // Construct HTI message
    HtiMsgHelper msg( soap, HTI_APPLICATION_UID, CMD_HTI_UNINSTALL_SOFTWARE_NAME_U );
    msg.AddWCStringWithLengthByte( packageName );
    msg.AddByte( killApp );
    msg.AddByte( breakDependency );
    msg.AddByte( mimeType );

    // Send message
    if ( msg.SendReceiveMsg( HTIMSG_TIMEOUT_60_SECONDS ) )
        return SOAP_FAULT;

    // Check response
    if ( msg.CheckMsgExactLen( 1 ) != SOAP_OK )
        return SOAP_FAULT;
    if ( msg.GetByte( 0 ) != CMD_RESP_OK )
        return SOAP_FAULT;

    return SOAP_OK;
}

//**********************************************************************************
// ns1__listRunningApps()
//**********************************************************************************
int ns1__listRunningApps(struct soap* soap,
                         bool includeHidden,
                         bool includeSystem,
                         ArrayOfHtiRunningApp *htiApps)
{
    // Construct & send HTI message
    HtiMsgHelper msg( soap, HTI_APPLICATION_UID, CMD_HTI_LIST_APPS_U );
    msg.AddByte( includeHidden );
    msg.AddByte( includeSystem );
    if ( msg.SendReceiveMsg( HTIMSG_TIMEOUT_10_SECONDS ) )
        return SOAP_FAULT;

    // Check response
    if ( msg.CheckMsgMinLen( 3 ) )
        return SOAP_FAULT;

    if ( msg.GetByte(0) != CMD_RESP_OK )
    {
        // This sould happen only if message is corrupted somehow...
        soap->error = soap_receiver_fault_format(soap, "HtiError",
            "response code error (0x%x)", msg.GetByte(0));
        return SOAP_FAULT;
    }

    // Fill & alloc soap stuff
    htiApps->__size = (int) msg.GetWord( 1 );
    htiApps->__ptr = soap_new_ns1__HtiRunningApp( soap, htiApps->__size );

    // Go through apps
    int offset = 3;
    ns1__HtiRunningApp *app = htiApps->__ptr;
    for ( int i = 0; i < htiApps->__size; i++ )
    {
        // App UID
        app->uid = msg.GetInt( offset );
        offset += 4;

        // Caption
        int captionLen = msg.GetByte( offset );
        offset++;
        app->caption = msg.GetSoapWCString( offset, captionLen );
        offset += captionLen * 2;

        // Document name
        int docLen = msg.GetByte( offset );
        offset++;
        app->document = msg.GetSoapWCString( offset, docLen );
        offset += docLen * 2;

        // Flags
        app->hidden = msg.GetByte( offset );
        offset++;
        app->system = msg.GetByte( offset );
        offset++;
        app->ready = msg.GetByte( offset );
        offset++;
        app->busy = msg.GetByte( offset );
        offset++;
        app->closable = msg.GetByte( offset );
        offset++;   

        // Advance the array pointer for the next iteration
        app++;
    }

    return SOAP_OK;
}