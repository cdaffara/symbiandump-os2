/*
* Copyright (c) 1997-2009 Nokia Corporation and/or its subsidiary(-ies).
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
*
*/


#ifndef __FLDINFO_H__
#define __FLDINFO_H__

#include <e32std.h>



class TFindFieldInfo
/** 
information returned from a "Find the fields in this range" request
Gets information about the fields found in a range of characters by CPlainText::FindFields().

This information is the number of fields fully or partially within the range 
and the start position and length of the first field found. 
@publishedAll
@released
*/
	{
public:
	IMPORT_C TBool operator==(const TFindFieldInfo& aInfo)const;
	IMPORT_C TBool operator!=(const TFindFieldInfo& aInfo)const;
public:
	/** The number of fields found in the specified range. */
	TInt iFieldCountInRange; // number of fields found in the specified range
	/** The document position of the start of the first field in the range. */
	TInt iFirstFieldPos; // character position of the start of the first field in the range
	/** The length of the first field in the range. */
	TInt iFirstFieldLen; // length of the first field in the range
	};


#endif
