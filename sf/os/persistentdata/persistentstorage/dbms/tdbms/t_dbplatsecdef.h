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
// t_dbplatsecdef - Common declarations
// 
//

#ifndef T_DBPLATSECDEF_H
#define T_DBPLATSECDEF_H

#include <e32test.h>
#include <d32dbms.h>

#ifdef __T_DBPLATSECDEF__
	#define CONST_LIT8(var, val) extern const TLitC8<sizeof(val)> var = {sizeof(val) - 1, val}
	#define CONST_LIT16(var, val) extern const TLitC16<sizeof(L##val)/2> var = {sizeof(L##val)/2 - 1, L##val}
	#define CONST_TTEXT(var, val) extern const TText var = val
#else
	#define CONST_LIT8(var, val) extern const TLitC8<sizeof(val)> var
	#define CONST_LIT16(var, val) extern const TLitC16<sizeof(L##val)/2> var
	#define CONST_TTEXT(var, val) extern const TText var
#endif

#ifdef _UNICODE
	#define CONST_LIT(var, val) CONST_LIT16(var, val)
#else
	#define CONST_LIT(var, val) CONST_LIT8(var, val)
#endif

extern RTest 				TheTest;
extern RDbs 				TheDbs;
extern RDbNamedDatabase 	TheDb1;
extern RDbNamedDatabase 	TheDb2;
extern RDbTable				TheTbl1;
extern RDbTable				TheTbl2;

const TUid KSecureDbUid = {0x11335577};
const TUid KSecureDbUid2 = {0x11335578};//Not enough rights for database operations here

const TInt KDatabaseNamePos = 2;

CONST_LIT(KSecure,	"SECURE");
CONST_LIT(KDb1Name, "c:A.dB");
CONST_LIT(KDb2Name, "C:b.Db");
CONST_LIT(KDb3Name, "c:C.db");
CONST_LIT(KTableName1, "ATBL");
CONST_LIT(KTableName2, "BTbL");
CONST_LIT(KTableName3, "ctBl");
CONST_LIT(KTableName4, "dTbL");
CONST_LIT(KProtDb1ZName, "z:A.db");
CONST_LIT(KProtDb1CName, "c:a.db");
CONST_LIT(KProtDb2CName, "C:B.DB");
CONST_LIT(KProtDb3CName, "c:z.DB");

CONST_LIT(KCol1, "ID");
CONST_LIT(KCol2, "DATA1");
CONST_LIT(KCol3, "DATA2");

#endif//T_DBPLATSECDEF_H
