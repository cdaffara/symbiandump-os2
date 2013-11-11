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


#include <flddef.h>
#include <fldbltin.h>
#include <conpics.h>
#include "TESTFAC.H"

/////////////////////////////////
// TTestFieldFactory
/////////////////////////////////

CTextField* TTestFieldFactory::NewFieldL(TUid aFieldType)
// Creates a field (in aHeader) of the type specified in aHeader
// 
	{
	CTextField* field=NULL;

	if (aFieldType==KDummyFieldUid)
		field = (CTextField*)new(ELeave) CDummyField();

	else if (aFieldType==KDateTimeFieldUid)
		field = (CTextField*)new(ELeave) CDateTimeField();

	else if (aFieldType==KPageNumberFieldUid)
		{
		field = (CTextField*)new(ELeave) CPageNumField();
		((CPageNumField*)field)->SetPageNumInfo(&iInfo);
		((CPageNumField*)field)->SetNumberStyle(CPageNumField::ERomanLower);
		}

	else if (aFieldType==KNumPagesFieldUid)
		{
		field = (CTextField*)new(ELeave) CNumPagesField();
		((CNumPagesField*)field)->SetNumPagesInfo(&iInfo);
		((CNumPagesField*)field)->SetNumberStyle(CPageNumField::EAlphabeticUpper);
		}

	else if (aFieldType==KFileNameFieldUid)
		{
		field = (CTextField*)new(ELeave) CFileNameField();
		((CFileNameField*)field)->SetFileNameInfo(&iInfo);
		}

	return field;
	}


/////////////////////////////////
// TTestFieldInfo
/////////////////////////////////

TInt TTestFieldInfo::UpdateFieldFileName(TPtr& aValueText)const
	{
	TBuf<12> name(_L("FileName.doc"));
	if (aValueText.MaxLength() < name.Length())
		return name.Length();
	else
		{
		aValueText = name;
		return 0;
		}
	}


TInt TTestFieldInfo::UpdateFieldNumPages()const
	{
	return 2;
	}


TInt TTestFieldInfo::UpdateFieldPageNum()const
	{
	return 1;
	}
