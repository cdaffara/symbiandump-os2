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



 
TBool CEditableText::HasChanged()const
/** Tests whether the editable text object has been changed by an operation on 
it, as set by SetHasChanged().

@return ETrue if a change has occurred to the text object. EFalse if there 
has been no change. */
	{return iHasChanged;}


TBool CPlainText::FieldSetPresent()const
	{return (iFieldSet.IsPtr() && iFieldSet.AsPtr()!=NULL);}


void CPlainText::ConsumeAdornmentL(RReadStream& aStream)
	{
	TStreamId id;
	aStream>> id;
	}


TUid CPlainText::UidFromStreamL(RReadStream& aStream)
	{
	TUid uid;
	aStream>> uid;
	return uid;
	}
