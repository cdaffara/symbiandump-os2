//
// Copyright (c) 2005-2008 Nokia Corporation and/or its subsidiary(-ies).
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

#ifndef OMXILCORE_H
#define OMXILCORE_H

#include <e32base.h>
#include <openmax/il/khronos/v1_x/OMX_Core.h>

// Forward declarations
class COmxComponentLoaderIf;
class COmxILContentPipeIf;

NONSHARABLE_CLASS(COmxILCore): public CBase
	{
public:

	static const TUint8 iSpecVersionMajor	  = OMX_VERSION_MAJOR;
	static const TUint8 iSpecVersionMinor	  = OMX_VERSION_MINOR;
	static const TUint8 iSpecVersionRevision  = OMX_VERSION_REVISION;
	static const TUint8 iSpecVersionStep	  = OMX_VERSION_STEP;

public:

	COmxILCore();
	~COmxILCore();

	OMX_ERRORTYPE ListLoaders();
	void DeinitCore();
	IMPORT_C OMX_ERRORTYPE LoadComponent(const OMX_STRING aComponentName, OMX_HANDLETYPE* aHandle, OMX_PTR aAppData,OMX_CALLBACKTYPE* aCallBacks);
	IMPORT_C OMX_ERRORTYPE ComponentNameEnum(OMX_STRING aComponentName, OMX_U32 aNameLength, const OMX_U32 aIndex);
	IMPORT_C OMX_ERRORTYPE FreeHandle(OMX_HANDLETYPE aComponent) const;
	IMPORT_C static OMX_ERRORTYPE SetupTunnel(OMX_HANDLETYPE aHandleOutput, OMX_U32 aPortOutput,
											  OMX_HANDLETYPE aHandleInput, OMX_U32 aPortInput);
	IMPORT_C OMX_ERRORTYPE GetContentPipe(OMX_HANDLETYPE* aPipe,const OMX_STRING aURI);
	IMPORT_C OMX_ERRORTYPE GetComponentsOfRole ( const OMX_STRING aRole, OMX_U32* aNumComps, OMX_U8** aCompNames );
	IMPORT_C OMX_ERRORTYPE GetRolesOfComponent ( const OMX_STRING aCompName, OMX_U32* aNumRoles, OMX_U8** aRoles);

private:

	class TOmxLoaderInfo
		{
	public:
		TOmxLoaderInfo();
	public:
		TUid iUid;
		COmxComponentLoaderIf* iLoader;
		};

private:

	OMX_ERRORTYPE LoadAndInitLoader(const TUid& aUid, COmxComponentLoaderIf*& aLoader);

private:
	RArray<TOmxLoaderInfo> iLoaders;
	RPointerArray<COmxILContentPipeIf> iContentPipes;
	};

inline COmxILCore::TOmxLoaderInfo::TOmxLoaderInfo():
	iUid(TUid::Uid(0)),iLoader(NULL) 
	{
	}

/**
   Utility functions
 */
class TOmxILCoreUtils
	{
public:

	static OMX_ERRORTYPE ConvertErrorGetParameterToSetupTunnel(
		OMX_ERRORTYPE aGetParameterError);

	};

#endif /* OMXILCORE_H */
