// Copyright (c) 2007-2009 Nokia Corporation and/or its subsidiary(-ies).
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
// This class provides a wrapper around the launching of a seperate process to
// create and modify publish and subscribe variables.  The calls below map directly to 
// the corresponding RProperty functions.  Properties are defined with the following
// security policies ReadPolicy = EAlwaysPass, WritePolicy = SID(0X10204FC5) (SysAgt2Svr SID).
// The maximum size of Text properties has been restricted to 128 bytes for convenience.
// 
//


#ifndef __TPROPERTYMANAGER_H__
#define __TPROPERTYMANAGER_H__

#include <e32test.h>
#include <e32property.h>

class PropertyManager
	{
public:

enum TOperation {	EDefineProperty,
					EDeleteProperty,
					ESetPropertyInt,
					ESetPropertyDes8,
					ESetPropertyDes16
					};

	IMPORT_C static TInt DefineProperty(TUid aCategory, TUint aKey, TInt aAttr,TInt aPreallocated = 0);
	IMPORT_C static TInt DeleteProperty(TUid aCategory, TUint aKey);
	IMPORT_C static TInt SetProperty(TUid aCategory, TUint aKey, TInt aValue);
	IMPORT_C static TInt SetProperty(TUid aCategory, TUint aKey, const TDesC8 &aValue);
	IMPORT_C static TInt SetProperty(TUid aCategory, TUint aKey, const TDesC16 &aValue);
	
private:
	LOCAL_C TInt LaunchHelperProcess(TOperation aOperation,TDesC& aArgs);
	
	};

#endif

