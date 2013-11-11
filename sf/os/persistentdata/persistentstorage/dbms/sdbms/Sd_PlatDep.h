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
// This header file must be included in Sd_Svr.cpp. It contains platform/target dependent code.
// 
//

#ifndef __SD_PLATDEP_H__
#define __SD_PLATDEP_H__

////////////////////////////////////////////////////////////////////////////////////

typedef RProcess RDbProcess;

inline TInt CreateDbmsProcess(RDbProcess& aProcess)
	{
	return aProcess.Create(KDbsServerImg,
						   KNullDesC,
						   TUidType(KNullUid, KNullUid, KDbsServerUid3),
						   EOwnerThread);
	}

////////////////////////////////////////////////////////////////////////////////////

#ifdef __WINS__

	inline void LoadDbmsLibraryL()
		{
		_LIT(KDbmsLibraryName, "EDBMS");
		RLibrary lib;
		__LEAVE_IF_ERROR(lib.Load(KDbmsLibraryName, KNullDesC));
		}

#else//__WINS__

	#define LoadDbmsLibraryL()

#endif//__WINS__

////////////////////////////////////////////////////////////////////////////////////

#ifdef _DEBUG

	//#define DbgPrint1(aMsg, aPrm) RDebug::Print(aMsg, aPrm)
	//#define DbgPrint2(aMsg, aPrm1, aPrm2) RDebug::Print(aMsg, aPrm1, aPrm2)
	#define DbgPrint1(aMsg, aPrm)
	#define DbgPrint2(aMsg, aPrm1, aPrm2)

#else//_DEBUG

	#define DbgPrint1(aMsg, aPrm)
	#define DbgPrint2(aMsg, aPrm1, aPrm2)

#endif//_DEBUG

////////////////////////////////////////////////////////////////////////////////////

#endif//__SD_PLATDEP_H__

