// Copyright (c) 2007-2010 Nokia Corporation and/or its subsidiary(-ies).
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
// This file define all the common values thoughout your test project
//


/**
 @file te_uloggerostv2integsuite_defs.h
 @internalTechnology
*/

#if (!defined TE_ULOGGER_MCL_SUITEDEFS_H)
#define TE_ULOGGER_MCL_SUITEDEFS_H

#include <uloggerclient.h>
//C.A. previously:using namespace UTF;
using namespace Ulogger; //CA:added so can use old ulogger api
class CClearConfig : public CBase
{
public:
	~CClearConfig();
	CClearConfig();
	void ClearL(RULogger& loggerSession);
	void DefaultL(RULogger& loggerSession);
//private:
	
};

// Please modify below value with your project and must match with your configuration ini file which is required to be modified as well
_LIT(KTe_ULoggerSuiteString,"TheString");
_LIT(KTe_ULoggerSuiteInt,"TheInt");
_LIT(KTe_ULoggerSuiteBool,"TheBool");

// For test step panics
_LIT(KTe_ULoggerSuitePanic,"Te_ULoggerMCLSuite");

//For CSV stuff for the API call tests
_LIT8(KApiCallHeader, "Api Name, Api Id, Api Size, Size Of EVP Data, Context Id, Pc, Runtime Classification Filtering Enabled, Runtime ModuleUid Filtering Enabled, Classification Filter Matches Trace Point, ModuleUid Filter Matches Trace Point, Trace Compiled In, Minimum Execution Time, Typical Execution Time, Api Stack Usage, Api Heap Usage,");
_LIT(KApiCSVFile,"c:\\te_apitestresults.csv");
_LIT(KReferenceCsvFile, "z:\\testdata\\referencedata\\ost_baseline_h6.csv");
//_LIT(KReferenceCsvFile, "z:\\testdata\\referencedata\\ost_baseline_h4.csv");
_LIT(KReferenceWinscwCsvFile, "z:\\testdata\\referencedata\\ost_baseline_winscw.csv");
_LIT8(KCsvComma, ",");
_LIT8(KCsvNewLine, "\n"); 


#ifndef __KERNEL_MODE__
_LIT16(KName16,"A _LIT16 string with integer parameter %i !");
#endif
_LIT8(KName8,"A _LIT8 string with integer parameter %i !");

_LIT16(KAny16,"Something");

#endif
