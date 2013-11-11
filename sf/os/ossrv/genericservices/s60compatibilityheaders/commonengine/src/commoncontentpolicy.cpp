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



// INCLUDE FILES
#include <apfmimecontentpolicy.h>
#include "commoncontentpolicy.h"

EXPORT_C CCommonContentPolicy* CCommonContentPolicy::NewL()
	{ return reinterpret_cast<CCommonContentPolicy*>(CApfMimeContentPolicy::NewL()); }
	
EXPORT_C CCommonContentPolicy* CCommonContentPolicy::NewLC()
	{ return reinterpret_cast<CCommonContentPolicy*>(CApfMimeContentPolicy::NewLC()); }
	
EXPORT_C CCommonContentPolicy* CCommonContentPolicy::NewL(RFs& aFs)
	{ return reinterpret_cast<CCommonContentPolicy*>(CApfMimeContentPolicy::NewL(aFs)); }
	
EXPORT_C CCommonContentPolicy* CCommonContentPolicy::NewLC(RFs& aFs)
	{ return reinterpret_cast<CCommonContentPolicy*>(CApfMimeContentPolicy::NewLC(aFs)); }
	
EXPORT_C CCommonContentPolicy::~CCommonContentPolicy()
	{ delete reinterpret_cast<CApfMimeContentPolicy*>(this); }
	
EXPORT_C TBool CCommonContentPolicy::IsClosedType(const TDesC& aMimeType)
	{ return reinterpret_cast<CApfMimeContentPolicy*>(this)->IsClosedType(aMimeType); }
	
EXPORT_C TBool CCommonContentPolicy::IsClosedExtension(const TDesC& aFileExtension)
	{ return reinterpret_cast<CApfMimeContentPolicy*>(this)->IsClosedExtension(aFileExtension); }
	
EXPORT_C TBool CCommonContentPolicy::IsClosedFileL(const TDesC& aFileName)
	{ return reinterpret_cast<CApfMimeContentPolicy*>(this)->IsClosedFileL(aFileName); }
	
EXPORT_C TBool CCommonContentPolicy::IsDRMEnvelopeL(const TDesC& aFileName)
	{ return reinterpret_cast<CApfMimeContentPolicy*>(this)->IsDRMEnvelopeL(aFileName); }
	
EXPORT_C TBool CCommonContentPolicy::IsClosedFileL(RFile& aFileHandle)
	{ return reinterpret_cast<CApfMimeContentPolicy*>(this)->IsClosedFileL(aFileHandle); }
	
EXPORT_C TBool CCommonContentPolicy::IsDRMEnvelopeL(RFile& aFileHandle)
	{ return reinterpret_cast<CApfMimeContentPolicy*>(this)->IsDRMEnvelopeL(aFileHandle); }

