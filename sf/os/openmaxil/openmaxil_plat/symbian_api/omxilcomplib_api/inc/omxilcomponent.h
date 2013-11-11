// Copyright (c) 2008-2009 Nokia Corporation and/or its subsidiary(-ies).
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


/**
 @file
 @internalComponent
*/

#ifndef OMXILCOMPONENT_H
#define OMXILCOMPONENT_H

#include <e32base.h>
#include <openmax/il/khronos/v1_x/OMX_Component.h>

// forward declarations
class COmxILConfigManager;
class COmxILProcessingFunction;
class COmxILPort;
class COmxILComponentImpl;
class MOmxILCallbackNotificationIf;

/**
   Component Panic category
*/
_LIT(KOmxILComponentPanicCategory, "OmxILComponent");

class COmxILComponent : public CBase
	{
public:
	enum TCallbackManagerType
		{
		EOutofContext,
		EInContext,
		ECallbackManagerTypeMax =0xff
		};
	enum TPortManagerType
		{
		ENonBufferSharingPortManager,
		EBufferSharingPortManager,
		EPortManagerTypeMax =0xff
		};
		
public:
	IMPORT_C virtual ~COmxILComponent();		
	IMPORT_C static OMX_ERRORTYPE SymbianErrorToGetHandleError(TInt aSymbianError);
	
protected:
	IMPORT_C COmxILComponent();
	IMPORT_C void InitComponentL();
	IMPORT_C virtual void ConstructL(OMX_HANDLETYPE aComponent);	
	IMPORT_C MOmxILCallbackNotificationIf* CreateCallbackManagerL(TCallbackManagerType aCallbackManagerType);
	IMPORT_C void CreatePortManagerL(TPortManagerType aType,
									const OMX_VERSIONTYPE& aOmxVersion,
									OMX_U32 aNumberOfAudioPorts,
									OMX_U32 aStartAudioPortNumber,
									OMX_U32 aNumberOfImagePorts,
									OMX_U32 aStartImagePortNumber,
									OMX_U32 aNumberOfVideoPorts,
									OMX_U32 aStartVideoPortNumber,
									OMX_U32 aNumberOfOtherPorts,
									OMX_U32 aStartOtherPortNumber,
									OMX_BOOL aImmediateReturnTimeBuffer = OMX_TRUE);
	IMPORT_C TInt AddPort(const COmxILPort* aPort, OMX_DIRTYPE aDirection);
	IMPORT_C void RegisterProcessingFunction(const COmxILProcessingFunction* aProcessingFunction);
	IMPORT_C void RegisterConfigurationManager(const COmxILConfigManager* aConfigManager);
	
	//getters and setters
	IMPORT_C OMX_COMPONENTTYPE* GetHandle() const;
	IMPORT_C OMX_PTR GetAppData() const;
	IMPORT_C OMX_CALLBACKTYPE* GetCallbacks() const;
	IMPORT_C COmxILConfigManager* GetConfigManager() const;
	IMPORT_C COmxILProcessingFunction* GetProcessingFunction() const;
	
	//From CBase
	IMPORT_C virtual TInt Extension_(TUint aExtensionId, TAny *&a0, TAny *a1);

private:
	COmxILComponentImpl* ipImpl;
	};

#endif // OMXILCOMPONENT_H
