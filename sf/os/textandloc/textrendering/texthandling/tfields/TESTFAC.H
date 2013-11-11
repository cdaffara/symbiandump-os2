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


#ifndef __TESTFAC_H__
#define __TESTFAC_H__

#include <fldbase.h>
#include <fldbltin.h>

// classes:
class TTestFieldInfo;
class TTestFieldFactory;
//


class TTestFieldInfo : public MFieldFileNameInfo, public MFieldPageNumInfo, public MFieldNumPagesInfo
	{
public:
	// from MFieldFileNameInfo
	virtual TInt UpdateFieldFileName(TPtr& aValueText)const;
	// from MFieldPageNumInfo
	virtual TInt UpdateFieldPageNum()const;
	// from MFieldNumPagesInfo
	virtual TInt UpdateFieldNumPages()const;
	};


class TTestFieldFactory : public MTextFieldFactory
	{
public:
	// from MTextFieldFactory
	virtual CTextField* NewFieldL(TUid aFieldType); 
	// Creates a field of the type specified
	// Returns NULL if it does not recognise/support the field type
private:
	TTestFieldInfo iInfo;
	};


#endif

