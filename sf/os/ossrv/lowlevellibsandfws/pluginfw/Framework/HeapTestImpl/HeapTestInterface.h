// Copyright (c) 2003-2009 Nokia Corporation and/or its subsidiary(-ies).
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

#ifndef __HEAPTESTINTERFACE_H_
#define __HEAPTESTINTERFACE_H_

#include <e32base.h>
#include <ecom/ecom.h>

class CHeapTestInterface : public CBase
{
public:
	static CHeapTestInterface* NewL();
	virtual ~CHeapTestInterface();
	virtual TUid ImplId() = 0;
	virtual void DisplayMessage()=0;


protected:
	inline CHeapTestInterface();

private:
	TUid iDtor_ID_Key;

};

const TUid KCHeapTestInterfaceUid = {0x101FE392};
const TUid KCHeapTestImplementationUid = {0x101FE392};

inline CHeapTestInterface::CHeapTestInterface() :
	CBase()
	{
	}

inline CHeapTestInterface::~CHeapTestInterface()
	{
	REComSession::DestroyedImplementation(iDtor_ID_Key);
	}

inline CHeapTestInterface* CHeapTestInterface::NewL()
	{
	TUid implUid = {0x101FE393};
	return REINTERPRET_CAST(CHeapTestInterface*, 
		REComSession::CreateImplementationL(implUid, 
											_FOFF(CHeapTestInterface, iDtor_ID_Key)));
	}


#endif //__HEAPTESTINTERFACE_H_

