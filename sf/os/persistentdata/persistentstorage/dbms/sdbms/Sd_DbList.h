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
// CDbNamesFactory class
// 
//

#ifndef __SDDBLIST_H__
#define __SDDBLIST_H__

#include <f32file.h> //RFs & TDriveNumber, includes <e32base.h> for CBase.

//Forward declarations 
class RDbPropsFactory;

//Global function
void CheckDriveL(RFs& aFs, TDriveNumber aDriveNumber);

/**
CDbNamesFactory class describes an object which can be used to get a list of names of 
secure shared databases, which have the same format UID.
CDbNamesFactory::DbNamesLC() is the acquisition method, which can be used for that.
The class also offers a static ExtractArgs() method, which can be used for extracting 
the drive number and constructing a TDbPolicyRequest object from RMessage parameter
(EDbsDatabaseList related message).
@internalComponent
*/
NONSHARABLE_CLASS(CDbNamesFactory) : public CBase
	{
public:
	static void ExtractArgs(const RMessage2& aMessage, TDriveNumber& aDriveNumber, 
							TDbPolicyRequest& aDbPolicyRequest);
	static CDbNamesFactory* NewLC();
	virtual ~CDbNamesFactory();
	CDbDatabaseNames* DbNamesLC(TDriveNumber& aDriveNumber, 
								const TDbPolicyRequest& aDbPolicyRequest, 
								const RDbPropsFactory& aDbPropsFactory, 
								RFs& aFs);

private:
	CDbNamesFactory();
	void ConstructSearchPattern(const TDbPolicyRequest& aDbPolicyRequest);
	void SearchL(RFs& aFs, CDbDatabaseNames* aDbNames);

private:
	TBuf<KMaxFileName>	iDbPath;
	TBuf<KMaxFileName>	iDbNameCmnPart;

	};

#endif//__SDDBLIST_H__
