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


#ifndef __FLDNUMS_H__
#define __FLDNUMS_H__

#include <e32std.h>

class TDeneryToCharBase
/**
@internalComponent
*/
	{
public:
	IMPORT_C TInt DeneryToChar(TPtr& aBuf,TInt aDenery)const;
protected:
	virtual TInt NumChars(TInt aDenery)const=0; 
	virtual void DoDeneryToChar(TPtr& aBuf,TInt aDenery)const=0;
	};


class TRomanNumeral : public TDeneryToCharBase
/**
@internalComponent
*/
	{
public:

protected:
	IMPORT_C TInt NumChars(TInt aDenery)const; 
	IMPORT_C void DoDeneryToChar(TPtr& aBuf,TInt aDenery)const;
	};


class TArabicNumeral : public TDeneryToCharBase
/**
@internalComponent
*/
	{
protected:
	IMPORT_C TInt NumChars(TInt aDenery)const;
	IMPORT_C void DoDeneryToChar(TPtr& aBuf,TInt aDenery)const;
	};


class TAlphabeticNumeral : public TDeneryToCharBase
/**
@internalComponent
*/
	{
public:

protected:
	IMPORT_C TInt NumChars(TInt aDenery)const;
	IMPORT_C void DoDeneryToChar(TPtr& aBuf,TInt aDenery)const;
private:
	void PropendDigitAsChar(TPtr& aValueText,TInt aDigit)const;
	};


#endif
