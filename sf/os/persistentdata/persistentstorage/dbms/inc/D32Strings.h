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
// DBMS - Common strings
// 
//

#ifndef __D32STRINGS_H__
#define __D32STRINGS_H__

#ifdef __D32STRINGS__
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

CONST_LIT(KSecure,			"SECURE");
CONST_LIT(KDbsPrefix,		"DBS");
CONST_LIT(KSecurityPolicyDir,"POLICY\\");
CONST_LIT(KPolicyFileExtMask,"*.SPD");

#endif//__D32STRINGS_H__


