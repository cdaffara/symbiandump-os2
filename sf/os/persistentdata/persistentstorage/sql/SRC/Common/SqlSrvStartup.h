// Copyright (c) 2005-2010 Nokia Corporation and/or its subsidiary(-ies).
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

#ifndef __SQLSRVSTARTUP_H__
#define __SQLSRVSTARTUP_H__

#include <e32std.h>	//TInt, TVersion

//SqlDb server name
_LIT(KSqlSrvName, "!SQL Server");

//SQL server image name
_LIT(KSqlSrvImg, "SqlSrv");

//SQL server: major version number, minor version number, build number constants.
const TInt KSqlMajorVer = 1;
const TInt KSqlMinorVer = 1;
const TInt KSqlBuildVer = 0;

const TUid KSqlSrvUid3 = {0x10281E17};//The same UID is in SqlSrv.mmp file

/**
@return SQL server version.

@internalComponent
*/	
inline TVersion SqlSrvVersion()
	{
	return TVersion(KSqlMajorVer, KSqlMinorVer, KSqlBuildVer);
	}

TInt StartSqlServer();

#endif //__SQLSRVSTARTUP_H__
