// Copyright (c) 2004-2009 Nokia Corporation and/or its subsidiary(-ies).
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
// RDbPropsFactory class
// 
//

#ifndef __SD_DBPROPS_H__
#define __SD_DBPROPS_H__

#include "D32DBMS.H"
#include "D32Security.h"

using namespace DBSC;

/**
This structure describes an object, which conatains 
database properties, which can be collected from a database path and format string.
@internalComponent
*/
struct TDbProps
	{
	TDbPolicyRequest	iDbPolicyRequest;
	TDriveNumber		iDriveNumber;
	TFileName			iPath;
	TDbName				iFormatStr;
	};

/**
RDbPropsFactory class is used to parse the supplied database path and format string and 
collect from them some database properties, such as requested access type, drive number, 
security policy uid, path, format string
@internalComponent
*/
class RDbPropsFactory
	{
public:
	static void StripCommonPart(TDes& aDbName);
	static void ConstructCommonPart(TUid aPolicyUid, TDes& aRes);
	RDbPropsFactory(RFs&);
	void OpenL();
	void Close();
	TDbProps* ExtractLC(const TDesC& aPath, const TDesC& aFormatStr);
	TDbProps* ExtractLC(const TDesC& aPath, TUid aPolicyUid);
	void GetPrivatePathL(TDriveNumber aDriveNumber, TDes& aRes) const;

private:
	void ExtractSecureL(const TDesC& aFormatStr, TDbProps& aDbProps);
	void ExtractNonSecureL(const TDesC& aPath, const TDesC& aFormatStr, TDbProps& aDbProps);
	void ConstructFullDbPathL(TDbProps& aDbProps);
	void ConstructFormatString(TDbProps& aDbProps, const TDesC& aFormatStr);

private:
	RFs&		iFs;
	TParse*		iFileNameParser;
	HBufC*		iPrivateDataPath; 

	};

#endif//__SD_DBPROPS_H__
