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

#include <e32def.h>

const TUid KUidPlatSecECom1Impl = {0x102026AA};
const TUid KUidPlatSecECom2Impl = {0x102026AC};
const TUid KUidPlatSecECom3Impl = {0x102026AE};
const TUid KUidPlatSecECom4Impl = {0x102026B1};

inline CPlatSecInterface1::CPlatSecInterface1() :
	CBase()
	{
	}

inline CPlatSecInterface1::~CPlatSecInterface1()
	{
	REComSession::DestroyedImplementation(iDtor_ID_Key);
	}

inline CPlatSecInterface1* CPlatSecInterface1::NewL()
	{
	TAny* p = REComSession::CreateImplementationL(KUidPlatSecECom1Impl,
												  _FOFF(CPlatSecInterface1, iDtor_ID_Key));
	return reinterpret_cast <CPlatSecInterface1*> (p);
	}

//

inline CPlatSecInterface2::CPlatSecInterface2() :
	CBase()
	{
	}

inline CPlatSecInterface2::~CPlatSecInterface2()
	{
	REComSession::DestroyedImplementation(iDtor_ID_Key);
	}

inline CPlatSecInterface2* CPlatSecInterface2::NewL()
	{
	TAny* p = REComSession::CreateImplementationL(KUidPlatSecECom2Impl,
												  _FOFF(CPlatSecInterface2, iDtor_ID_Key));
	return reinterpret_cast <CPlatSecInterface2*> (p);
	}


//

inline CPlatSecInterface3::CPlatSecInterface3() :
	CBase()
	{
	}

inline CPlatSecInterface3::~CPlatSecInterface3()
	{
	REComSession::DestroyedImplementation(iDtor_ID_Key);
	}

inline CPlatSecInterface3* CPlatSecInterface3::NewL()
	{
	TAny* p = REComSession::CreateImplementationL(KUidPlatSecECom3Impl,
												  _FOFF(CPlatSecInterface3, iDtor_ID_Key));
	return reinterpret_cast <CPlatSecInterface3*> (p);
	}
//

inline CPlatSecInterface4::CPlatSecInterface4() :
	CBase()
	{
	}

inline CPlatSecInterface4::~CPlatSecInterface4()
	{
	REComSession::DestroyedImplementation(iDtor_ID_Key);
	}

inline CPlatSecInterface4* CPlatSecInterface4::NewL()
	{
	TAny* p = REComSession::CreateImplementationL(KUidPlatSecECom4Impl,
												  _FOFF(CPlatSecInterface4, iDtor_ID_Key));
	return reinterpret_cast <CPlatSecInterface4*> (p);
	}
