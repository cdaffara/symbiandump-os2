/*
* Copyright (c) 2005-2009 Nokia Corporation and/or its subsidiary(-ies).
* All rights reserved.
* This component and the accompanying materials are made available
* under the terms of the License "Eclipse Public License v1.0"
* which accompanies this distribution, and is available
* at the URL "http://www.eclipse.org/legal/epl-v10.html".
*
* Initial Contributors:
* Nokia Corporation - initial contribution.
*
* Contributors:
*
* Description: 
* TestParameter.h: interface for the CTestParameter class.
*
*/




/**
 @file
*/

#ifndef __TESTPARAMETER_H__
#define __TESTPARAMETER_H__

#include <e32base.h>

const TInt KMaxValueSize = 100;

class CTestParameter : public CBase  
{
public:
	enum TType
	{
		EInt,
		EString,
		EIntRange,
		ERandom
	};
public:
	virtual ~CTestParameter(){};

	virtual TType GetType(void) = 0;
	TBool iValid;
protected:
	CTestParameter();
	TInt ConvertHex(const TDes& aValue, TInt &aIntValue);
};

class CIntTestParameter : public CTestParameter  
{
public:
	static CIntTestParameter* NewL(TDes& aValue);
	static CIntTestParameter* NewLC(TDes& aValue);
	virtual ~CIntTestParameter(){};

	inline TType GetType(void) { return(EInt);};
	inline TInt Value(void) { return(iValue);}
protected:
	CIntTestParameter(){}
private:
	void Construct(TDes& aValue);
private:
	TInt iValue;
};

class CIntRangeTestParameter : public CTestParameter  
{
public:
	static CIntRangeTestParameter* NewL(TDes& aValue);
	static CIntRangeTestParameter* NewLC(TDes& aValue);
	virtual ~CIntRangeTestParameter(){};

	inline TType GetType(void) { return(EIntRange);};
	inline TInt Start(void) { return(iStart);};
	inline TInt Finish(void) { return(iFinish);};
	inline TInt Range(void) { return((iFinish - iStart)+1);};
protected:
	CIntRangeTestParameter(){}
private:
	void Construct(TDes& aValue);
private:
	TInt iStart;
	TInt iFinish;
};

class CRandomTestParameter : public CTestParameter  
{
public:
	static CRandomTestParameter* NewL(TDes& aValue);
	static CRandomTestParameter* NewLC(TDes& aValue);
	virtual ~CRandomTestParameter(){};

	inline TType GetType(void) { return(ERandom);};
	inline TInt Interations(void) { return(iInterations);};
protected:
	CRandomTestParameter(){}
private:
	void Construct(TDes& aValue);
private:
	TInt iInterations;
};

class CStringTestParameter : public CTestParameter  
{
public:
	static CStringTestParameter* NewL(TDes& aValue);
	static CStringTestParameter* NewLC(TDes& aValue);
	virtual ~CStringTestParameter(){};

	inline TType GetType(void) { return(EString);};
	inline void GetValue(TDes& aValue) { aValue.Copy(iValue);};
protected:
	CStringTestParameter(){}
private:
	void Construct(TDes& aValue);
private:
	TBuf<KMaxValueSize> iValue;
};

#endif // !defined(AFX_TESTPARAMETER_H__95894347_8529_11D6_AB96_00080214A261__INCLUDED_)

