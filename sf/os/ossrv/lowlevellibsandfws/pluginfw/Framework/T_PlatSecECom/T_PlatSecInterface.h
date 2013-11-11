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
//

#ifndef __T_PLATSECINTERFACE_H__
#define __T_PLATSECINTERFACE_H__

#include <e32base.h>
#include <ecom/ecom.h>

/**
Test interface used in PlatSec ECOM tests
@internalComponent
*/
class CPlatSecInterface1 : public CBase
{
public:
	static CPlatSecInterface1* NewL();
	virtual ~CPlatSecInterface1();
	virtual TUid ImplId() = 0;

protected:
	inline CPlatSecInterface1();

private:
	TUid iDtor_ID_Key;
};

/**
Test interface used in PlatSec ECOM tests
@internalComponent
*/
class CPlatSecInterface2: public CBase
{
public:
	static CPlatSecInterface2* NewL();
	virtual ~CPlatSecInterface2();
	virtual TUid ImplId() = 0;

protected:
	inline CPlatSecInterface2();

private:
	TUid iDtor_ID_Key;
};

/**
Test interface used in PlatSec ECOM tests
@internalComponent
*/
class CPlatSecInterface3: public CBase
{
public:
	static CPlatSecInterface3* NewL();
	virtual ~CPlatSecInterface3();
	virtual TUid ImplId() = 0;

protected:
	inline CPlatSecInterface3();

private:
	TUid iDtor_ID_Key;
};

/**
Test interface used in PlatSec ECOM tests
@internalComponent
*/
class CPlatSecInterface4: public CBase
{
public:
	static CPlatSecInterface4* NewL();
	virtual ~CPlatSecInterface4();
	virtual TUid ImplId() = 0;

protected:
	inline CPlatSecInterface4();

private:
	TUid iDtor_ID_Key;
};
#include "T_PlatSecInterface.inl"

#endif//__T_PLATSECINTERFACE_H__

