// Copyright (c) 2006-2009 Nokia Corporation and/or its subsidiary(-ies).
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

const TUid KUidMultipeImageImpl1  = {0x10282308};
const TUid KUidMultipeImageImpl2  = {0x1028230A};
const TUid KUidMultipeImageImpl3  = {0x1028230C};
const TUid KUidMultipeImageImpl4  = {0x1028230E};
const TUid KUidMultipeImageImpl5  = {0x10282310};
const TUid KUidMultipeImageImpl6  = {0x10282312};
const TUid KUidMultipeImageImpl7  = {0x10282314};
const TUid KUidMultipeImageImpl8  = {0x10282316};
const TUid KUidMultipeImageImpl9  = {0x10282318};
const TUid KUidMultipeImageImpl10 = {0x1028231A};
const TUid KUidMultipeImageImpl11 = {0x1028231C};
const TUid KUidMultipeImageImpl12 = {0x1028231E};
const TUid KUidMultipeImageImpl13 = {0x10282320};
const TUid KUidMultipeImageImpl14 = {0x10282322};
const TUid KUidMultipeImageImpl15 = {0x10282324};
const TUid KUidMultipeImageImpl16 = {0x10282326};
const TUid KUidMultipeImageImpl17 = {0x10282328};
const TUid KUidMultipeImageImpl18 = {0x1028232A};
const TUid KUidMultipeImageImpl19 = {0x1028232C};
const TUid KUidMultipeImageImpl20 = {0x1028232E};

inline CMultipleImageInterface1::CMultipleImageInterface1() :
	CBase()
	{
	}

inline CMultipleImageInterface1::~CMultipleImageInterface1()
	{
	REComSession::DestroyedImplementation(iDtor_ID_Key);
	}

inline CMultipleImageInterface1* CMultipleImageInterface1::NewL()
	{
	TAny* p = REComSession::CreateImplementationL(KUidMultipeImageImpl1,
												  _FOFF(CMultipleImageInterface1, iDtor_ID_Key));
	return reinterpret_cast <CMultipleImageInterface1*> (p);
	}




