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
//gsoap ns1 service name: HtiStif
//gsoap ns1 service namespace: urn:hti
//gsoap ns1 service style: rpc
//gsoap ns1 service encoding: literal
//gsoap ns1 service location: http://localhost:2000

typedef char*			xsd__string;
typedef wchar_t *		xsd__string_;
typedef int				xsd__int;

struct ns1__HtiStifResult
{
    xsd__int		caseId;
    xsd__int		caseResult;
    xsd__int		executionResult;
	xsd__string		resultDescription;
};

struct ArrayOfTestCases
{
    xsd__string	*__ptr;
    xsd__int	__size;
};

//gsoap ns1 service method-action: openStif "HtiStif"
int ns1__openStif(
    xsd__string		iniFile,
    struct ns1__openStifResponse{} *out  // empty response
);

//gsoap ns1 service method-action: closeStif "HtiStif"
int ns1__closeStif(
    void *_, // no in parameter
	struct ns1__closeStifResponse{} *out  // empty response
);

//gsoap ns1 service method-action: loadStifTestModule "HtiStif"
int ns1__loadStifTestModule(
    xsd__string		moduleName,
	xsd__string		iniFile,
	xsd__string		&moduleNameLoaded
);

//gsoap ns1 service method-action: unloadStifTestModule "HtiStif"
int ns1__unloadStifTestModule(
    xsd__string		moduleName,
	xsd__string		&moduleNameUnloaded
);

//gsoap ns1 service method-action: listStifTestCases "HtiStif"
int ns1__listStifTestCases(
    xsd__string		moduleName,
    struct ArrayOfTestCases *testCases
);

//gsoap ns1 service method-action: startStifTestCase "HtiStif"
int ns1__startStifTestCase(
    xsd__string		moduleName,
	xsd__int		testCaseIndex,
	xsd__int		&testCaseId
);

//gsoap ns1 service method-action: cancelStifTestCase "HtiStif"
int ns1__cancelStifTestCase(
    xsd__int		testCaseId,
	struct ns1__cancelStifTestCaseResponse{} *out  // empty response
);

//gsoap ns1 service method-action: pauseStifTestCase "HtiStif"
int ns1__pauseStifTestCase(
    xsd__int		testCaseId,
	struct ns1__pauseStifTestCaseResponse{} *out  // empty response
);

//gsoap ns1 service method-action: resumeStifTestCase "HtiStif"
int ns1__resumeStifTestCase(
    xsd__int		testCaseId,
	struct ns1__resumeStifTestCaseResponse{} *out  // empty response
);

struct queryStifTestCaseResultResponse{struct ns1__HtiStifResult _returnHtiStifResult;};
//gsoap ns1 service method-action: queryStifTestCaseResult "HtiStif"
int ns1__queryStifTestCaseResult(
    xsd__int		testCaseId,
	struct queryStifTestCaseResultResponse& r
);

struct runStifTestCaseResponse{struct ns1__HtiStifResult _returnHtiStifResult;};
//gsoap ns1 service method-action: runStifTestCase "HtiStif"
int ns1__runStifTestCase(
    xsd__string		moduleName,
	xsd__int		testCaseIndex,
	struct runStifTestCaseResponse& r
);

//gsoap ns1 service method-action: addStifTestCaseFile "HtiStif"
int ns1__addStifTestCaseFile(
    xsd__string		moduleName,
	xsd__string		testCaseFile,
    struct ns1__addStifTestCaseFileResponse{} *out  // empty response
);

//gsoap ns1 service method-action: removeStifTestCaseFile "HtiStif"
int ns1__removeStifTestCaseFile(
    xsd__string		moduleName,
	xsd__string		testCaseFile,
    struct ns1__removeStifTestCaseFileResponse{} *out  // empty response
);

//gsoap ns1 service method-action: setDeviceId "HtiStif"
int ns1__setDeviceId(
    xsd__int		deviceId,
	struct ns1__setDeviceIdResponse{} *out  // empty response
);

//gsoap ns1 service method-action: register "HtiStif"
int ns1__register(
    xsd__string		host,
    struct ns1__registerResponse{} *out  // empty response
);

//gsoap ns1 service method-action: deregister "HtiStif"
int ns1__deregister(
    xsd__string		host,
    struct ns1__deregisterResponse{} *out  // empty response
);

//gsoap ns1 service method-action: stifMessage "HtiStif"
int ns1__stifMessage(
	xsd__string		message,
    struct ns1__stifMessageResponse{} *out  // empty response
);

//gsoap ns1 service method-action: SetAttribute "HtiStif"
int ns1__setAttribute(
	xsd__string		attribute,
	xsd__string		value,
    struct ns1__SetAttributeResponse{} *out  // empty response
);