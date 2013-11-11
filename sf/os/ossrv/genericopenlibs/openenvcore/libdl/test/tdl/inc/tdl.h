/*
* Copyright (c) 2006-2009 Nokia Corporation and/or its subsidiary(-ies).
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
* Name        : tdl.h
* 
*
*/



#ifndef TDL_H
#define TDL_H

#include <test/testexecutestepbase.h>

_LIT(KDlSymLkupWithHandle, "DlSymLkupWithHandle");
_LIT(KDlSymLkupWithRTLD_DEFAULT, "DlSymLkupWithRTLD_DEFAULT");
_LIT(KDlLoadByVar, "DlLoadByVar");
_LIT(KDlLoadWithInvalidArg, "DlLoadWithInvalidArg");
_LIT(KDlMutexOnOpenHandle, "DlMutexOnOpenHandle");
_LIT(KOpenNonStdDllOrdinalLookup, "OpenNonStdDllOrdinalLookup");
_LIT(KOpenNonStdDllSymbolicLookup, "OpenNonStdDllSymbolicLookup");
_LIT(KOpenNonStdDllSymbolicLookupWithRTLD_DEFAULT, "OpenNonStdDllSymbolicLookupWithRTLD_DEFAULT");


IMPORT_C int divide(int x, int y);

class CTestDlByName:public CTestStep
{
public:
	CTestDlByName(const TDesC& aStepName);
	virtual ~CTestDlByName();
	
	virtual TVerdict  doTestStepL(); /*test case entry*/
			
	TVerdict TestSymLkupWithHandle(); /*test the lookup functionlity using the handle returned by dlopen*/
	TVerdict TestSymLkupWithRTLD_DEFAULT(); /*test the lookup functionality using the global object handle*/
	TVerdict TestLoadByVar();/*test the file name generation form the env variable(LD_LIBRARY_PATH)*/
	TVerdict TestLoadWithInvalidArg();/*test the supported api's with invalid arguments*/
	
	TVerdict TestMutexOnOpenHandle();	
	TVerdict OpenNonStdDllOrdinalLookup();
	TVerdict OpenNonStdDllSymbolicLookup();
	TVerdict OpenNonStdDllSymbolicLookupWithRTLD_DEFAULT();

protected:		
};

#endif //TDL_H
