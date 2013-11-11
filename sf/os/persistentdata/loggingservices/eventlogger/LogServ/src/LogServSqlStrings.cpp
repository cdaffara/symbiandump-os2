// Copyright (c) 2002-2009 Nokia Corporation and/or its subsidiary(-ies).
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

//__LOGSRV_STRINGS__ macro must be defined only here, in order to get the string constants from
//the "LogServSqlStrings.h" header defined only once. For all other source files where "LogServSqlStrings.h"
//is included the constants will be seen as external declarations.
#define __LOGSRV_STRINGS__

#include "LogServSqlStrings.h"

/**
Generic 16-bit buffer, used for construction/formatting of the SQL statements used in the server source files.
 
@internalComponent. 
*/
TBuf<KMaxSqlStringLength> TheSql;
