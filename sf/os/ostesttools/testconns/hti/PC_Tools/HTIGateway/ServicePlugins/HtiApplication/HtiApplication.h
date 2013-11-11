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
// HTI service functions
//gsoap ns1 service name: HtiApplication
//gsoap ns1 service namespace: urn:hti
//gsoap ns1 service style: rpc
//gsoap ns1 service encoding: literal
//gsoap ns1 service location: http://localhost:2000

typedef char*		    xsd__string;
typedef wchar_t *	    xsd__string_;
typedef int			    xsd__int;
typedef bool            xsd__boolean;
typedef unsigned char	xsd__unsignedByte;

enum ns1__mimeType { sisx, sis, pip, jad, jar, java, jarx };

struct ns1__startProcessResult
{
	xsd__string		status;
	xsd__int		pid;
};

struct ns1__startAppByUidResult
{
	xsd__string		status;
	xsd__int 		threadId;
};

struct ns1__startDocResult
{
	xsd__string		status;
	xsd__int 		threadId;
};

struct ns1__startAppResult
{
	xsd__string		status;
	xsd__int 		threadId;
};

struct ns1__getProcessExitCodeResult
{
    xsd__string     exitType;
    xsd__int        exitReason;
    xsd__string     exitCategory;
};

class ns1__HtiProcess
{
public:
	xsd__int		processId;
	xsd__string		processName;
	xsd__string		processStatus;
};

class ArrayOfHtiProcess
{
public:
    ns1__HtiProcess *__ptr;
    int				__size;
};

class ns1__HtiRunningApp
{
public:
    xsd__int        uid;
    xsd__string_    caption;
    xsd__string_    document;
    xsd__boolean    hidden;
    xsd__boolean    system;
    xsd__boolean    ready;
    xsd__boolean    busy;
    xsd__boolean    closable;
};

class ArrayOfHtiRunningApp
{
public:
    ns1__HtiRunningApp     *__ptr;
    int             __size;
};

struct startProcessResponse{struct ns1__startProcessResult _returnstartProcessResult;};
//gsoap ns1 service method-action: startProcess "HtiApplication"
int ns1__startProcess(
	xsd__string_	fileName,
	xsd__string_	args,
	struct startProcessResponse &r
);

//gsoap ns1 service method-action: stopProcess "HtiApplication"
int ns1__stopProcess(
	xsd__string_	matchPattern,
	xsd__string		&result
);

//gsoap ns1 service method-action: stopProcessById "HtiApplication"
int ns1__stopProcessById(
	xsd__int		pid,
	xsd__string		&result
);

//gsoap ns1 service method-action: getProcessStatus "HtiApplication"
int ns1__getProcessStatus(
	xsd__string_	matchPattern,
	xsd__string		&status
);

//gsoap ns1 service method-action: getProcessStatusById "HtiApplication"
int ns1__getProcessStatusById(
	xsd__int		pid,
	xsd__string		&status
);

struct startAppResponse{struct ns1__startAppResult _returnstartAppResult;};
//gsoap ns1 service method-action: startApp "HtiApplication"
int ns1__startApp(
	xsd__string_	appName,
	xsd__string_	docName,
	struct startAppResponse& r
);

struct startAppByUidResponse{struct ns1__startAppByUidResult _returnstartAppByUidResult;};
//gsoap ns1 service method-action: startAppByUid "HtiApplication"
int ns1__startAppByUid(
	xsd__int		uid,
	xsd__string_	docName,
	struct startAppByUidResponse& r
);

struct startDocResponse{struct ns1__startDocResult _returnstartDocResult;};
//gsoap ns1 service method-action: startDoc "HtiApplication"
int ns1__startDoc(
	xsd__string_	docName,
	struct startDocResponse& r
);

//gsoap ns1 service method-action: getAppStatus "HtiApplication"
int ns1__getAppStatus(
	xsd__string_	appName,
	xsd__string		&status
);

//gsoap ns1 service method-action: getAppStatusByUid "HtiApplication"
int ns1__getAppStatusByUid(
	xsd__int		uid,
	xsd__string		&status
);

//gsoap ns1 service method-action: getAppStatusByDoc "HtiApplication"
int ns1__getAppStatusByDoc(
	xsd__string_	docName,
	xsd__string		&status
);

//gsoap ns1 service method-action: stopApp "HtiApplication"
int ns1__stopApp(
	xsd__string_	appName,
	xsd__string		&result
);

//gsoap ns1 service method-action: stopAppByUid "HtiApplication"
int ns1__stopAppByUid(
	xsd__int		uid,
	xsd__string		&result
);

//gsoap ns1 service method-action: stopAppByDoc "HtiApplication"
int ns1__stopAppByDoc(
	xsd__string_	docName,
	xsd__string		&result
);

//gsoap ns1 service method-action: listProcesses "HtiApplication"
int ns1__listProcesses(
	xsd__string_	matchPattern,
	ArrayOfHtiProcess *HtiProcesses
);

//gsoap ns1 service method-action: startProcessRetVal "HtiApplication"
int ns1__startProcessRetVal(
	xsd__string_	fileName,
	xsd__string_	args,
	struct startProcessResponse &r
);

struct getProcessExitCodeResponse{struct ns1__getProcessExitCodeResult _returngetProcessExitCodeResult;};
//gsoap ns1 service method-action: getProcessExitCode "HtiApplication"
int ns1__getProcessExitCode(
	xsd__int        pid,
	struct getProcessExitCodeResponse &r
);

//gsoap ns1 service method-action: installSoftware "HtiApplication"
int ns1__installSoftware(
    xsd__string_    installPackagePath,
    xsd__boolean    upgrade,
    xsd__boolean    optionalItems,
    xsd__boolean    ocsp,
    xsd__boolean    ignoreOCSPWarnings,
    xsd__boolean    installUntrusted,
    xsd__boolean    allowPackageInfo,
    xsd__boolean    grantUserCapabilities,
    xsd__boolean    killApp,
    xsd__boolean    overwrite,
    xsd__boolean    download,
    xsd__string_    username,
    xsd__string_    password,
    xsd__string     drive,
    xsd__unsignedByte language,
    xsd__boolean    usePhoneLanguage,
    xsd__boolean    upgradeData,
    struct ns1__installSoftwareResponse{} *out  //empty response
);

//gsoap ns1 service method-action: uninstallSoftware "HtiApplication"
int ns1__uninstallSoftware(
    xsd__int            uid,
    xsd__boolean        killApp,
    xsd__boolean        breakDependency,
    enum ns1__mimeType  mimeType,
    struct ns1__uninstallSoftwareResponse{} *out  //empty response
);

//gsoap ns1 service method-action: uninstallSoftwareByName "HtiApplication"
int ns1__uninstallSoftwareByName(
    xsd__string_        packageName,
    xsd__boolean        killApp,
    xsd__boolean        breakDependency,
    enum ns1__mimeType  mimeType,
    struct ns1__uninstallSoftwareByNameResponse{} *out  //empty response
);

//gsoap ns1 service method-action: listRunningApps "HtiApplication"
int ns1__listRunningApps(
	xsd__boolean           includeHidden,
    xsd__boolean           includeSystem,
	ArrayOfHtiRunningApp   *htiApps
);
