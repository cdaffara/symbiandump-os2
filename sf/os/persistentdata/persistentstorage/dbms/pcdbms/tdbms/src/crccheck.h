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

#ifndef __T2_CRCCHECK_H__
#define __T2_CRCCHECK_H__

#include <f32file.h>
#include <e32test.h>

typedef struct
	{
	TInt filesz;
	TUint32 crc;
	} TestCheckInfo;

class TDBMS_CRCChecks
	{
public:
	TDBMS_CRCChecks()
   		{ 
		User::LeaveIfError(ifs.Connect());
		};
	~TDBMS_CRCChecks() { ifs.Close(); iarray.Close(); };
	TInt FileCrcL(const RFile&, TUint32 &);
	const TInt GenerateCrcL(const TPtrC);
	const TInt RecordCount();
	const TestCheckInfo operator[](const TInt);
	const TInt DumpCrcRecordsL(const TDesC&);
	const TInt ValidateCrcRecordsL(const TDesC&);
	const void ErrorReportL(const TInt err, TPtrC&);
	const TInt SetSessionPath(const TDesC&);
	
	enum TCrcCheckErrs 	{
						ECrcCheckOk=1,
						ECrcCheckMismatch,
						ECrcCheckFewerRecords,
						ECrcCheckMoreRecords,
						};

private:
	RFs ifs;
	TPtrC8 itestname, ilogfile;
	RArray<TestCheckInfo> iarray;
	};


#endif  //  __T2_CRCCHECK_H__
