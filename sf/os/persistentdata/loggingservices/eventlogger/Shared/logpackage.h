// Copyright (c) 2002-2009 Nokia Corporation and/or its subsidiary(-ies).
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

#ifndef __LOGPACKAGE_H__
#define __LOGPACKAGE_H__

#include <s32mem.h>
#include <logcli.h>

class CLogPackage : public CBase
/**
@internalComponent
*/
	{
public:
	IMPORT_C static CLogPackage* NewL();
	IMPORT_C ~CLogPackage();
	//
	IMPORT_C void SetLogEventL(const CLogEvent& aEvent);
	IMPORT_C void GetLogEventL(CLogEvent& aEvent) const;
	//
	IMPORT_C void SetLogEventTypeL(const CLogEventType& aType);
	IMPORT_C void GetLogEventTypeL(CLogEventType& aType) const;
	//
	IMPORT_C void SetLogConfigL(const TLogConfig& aConfig);
	IMPORT_C void GetLogConfigL(TLogConfig& aConfig) const;
	//
	IMPORT_C void SetLogFilterListL(const CLogFilterList& aFilterList);
	IMPORT_C void GetLogFilterListL(CLogFilterList& aFilterList) const;
	//
	inline TPtr8& Ptr();
	inline void ResizeL(TInt aSize);
	//
private:
	CLogPackage();
	void ConstructL();
	//
private:
	CBufFlat* iBuffer;
	TPtr8 iPtr;
	};

inline TPtr8& CLogPackage::Ptr()
	{
	iPtr.Set(iBuffer->Ptr(0));
	return iPtr;
	}

inline void CLogPackage::ResizeL(TInt aSize)
	{
	iBuffer->ResizeL(aSize);
	}

#endif

