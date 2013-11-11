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
typedef int				xsd__int;

struct ns1__HtiStifResult
{
    xsd__int		caseId;
    xsd__int		caseResult;
    xsd__int		executionResult;
	xsd__string		resultDescription;
};

//gsoap ns1 service method-action: stifMessage "HtiStif"
int ns1__stifMessage(
	xsd__string		message,
    void dummy // One-Way Message Passing
);

//gsoap ns1 service method-action: stifResult "HtiStif"
int ns1__stifResult(
    struct ns1__HtiStifResult result,
    void dummy // One-Way Message Passing
);
