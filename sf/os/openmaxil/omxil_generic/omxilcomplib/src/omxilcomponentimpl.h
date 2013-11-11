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

#ifndef OMXILCOMPONENTIMPL_H
#define OMXILCOMPONENTIMPL_H

#include <e32base.h>
#include <openmax/il/khronos/v1_x/OMX_Component.h>

// forward declarations
class COmxILFsm;
class COmxILConfigManager;
class MOmxILCallbackManagerIf;
class MOmxILPortManagerIf;
class MOmxILCallbackNotificationIf;
class COmxILProcessingFunction;
class COmxILPort;

class COmxILComponentImpl : public CBase
	{
public:
	static COmxILComponentImpl* NewL(COmxILComponent* aComponent, OMX_HANDLETYPE aHandle);
	virtual ~COmxILComponentImpl();

	void InitComponentL();
	void ConstructL(OMX_HANDLETYPE aComponent);
	MOmxILCallbackNotificationIf* CreateCallbackManagerL(COmxILComponent::TCallbackManagerType aCallbackManagerType);
	void CreatePortManagerL(COmxILComponent::TPortManagerType aType,
							const OMX_VERSIONTYPE& aOmxVersion,
							OMX_U32 aNumberOfAudioPorts,
							OMX_U32 aStartAudioPortNumber,
							OMX_U32 aNumberOfImagePorts,
							OMX_U32 aStartImagePortNumber,
							OMX_U32 aNumberOfVideoPorts,
							OMX_U32 aStartVideoPortNumber,
							OMX_U32 aNumberOfOtherPorts,
							OMX_U32 aStartOtherPortNumber,
							OMX_BOOL aImmediateReturnTimeBuffer);
	TInt AddPort(const COmxILPort* aPort, OMX_DIRTYPE aDirection);
	void RegisterProcessingFunction(const COmxILProcessingFunction* aProcessingFunction);
	void RegisterConfigurationManager(const COmxILConfigManager* aConfigManager);
	//Getters
	OMX_COMPONENTTYPE* GetHandle() const;
	OMX_PTR GetAppData() const;
	OMX_CALLBACKTYPE* GetCallbacks() const;
	COmxILConfigManager* GetConfigManager() const;
	COmxILProcessingFunction* GetProcessingFunction() const;
	MOmxILCallbackNotificationIf* GetCallbackNotificationIf() const;
	//Setters
	void SetPortManagerForConfigManager();
	
private:
	COmxILComponentImpl(COmxILComponent* aComponent, OMX_HANDLETYPE aHandle);
	void ConstructL();
	static OMX_ERRORTYPE GetComponentVersion(
		OMX_HANDLETYPE aComponent,
		OMX_STRING aComponentName,
		OMX_VERSIONTYPE* apComponentVersion,
		OMX_VERSIONTYPE* apSpecVersion,
		OMX_UUIDTYPE* apComponentUUID);

    static OMX_ERRORTYPE SendCommand(
		OMX_HANDLETYPE aComponent,
		OMX_COMMANDTYPE aCmd,
		OMX_U32 aParam1,
		OMX_PTR aCmdData);

	static OMX_ERRORTYPE GetParameter(
		OMX_HANDLETYPE aComponent,
		OMX_INDEXTYPE aParamIndex,
		OMX_PTR aComponentParameterStructure);

	static OMX_ERRORTYPE SetParameter(
		OMX_HANDLETYPE aComponent,
		OMX_INDEXTYPE aIndex,
		OMX_PTR aComponentParameterStructure);

	static OMX_ERRORTYPE GetConfig(
		OMX_HANDLETYPE aComponent,
		OMX_INDEXTYPE aIndex,
		OMX_PTR aComponentConfigStructure);

	static OMX_ERRORTYPE SetConfig(
		OMX_HANDLETYPE aComponent,
		OMX_INDEXTYPE aIndex,
		OMX_PTR aComponentConfigStructure);

	static OMX_ERRORTYPE GetExtensionIndex(
		OMX_HANDLETYPE aComponent,
		OMX_STRING aParameterName,
		OMX_INDEXTYPE* aIndexType);

	static OMX_ERRORTYPE GetState(
		OMX_HANDLETYPE aComponent,
		OMX_STATETYPE* aState);

	static OMX_ERRORTYPE ComponentTunnelRequest(
		OMX_HANDLETYPE aComp,
		OMX_U32 aPort,
		OMX_HANDLETYPE aTunneledComp,
		OMX_U32 aTunneledPort,
		OMX_TUNNELSETUPTYPE* aTunnelSetup);

	static OMX_ERRORTYPE UseBuffer(
		OMX_HANDLETYPE aComponent,
		OMX_BUFFERHEADERTYPE** appBufferHdr,
		OMX_U32 aPortIndex,
		OMX_PTR apAppPrivate,
		OMX_U32 aSizeBytes,
		OMX_U8* aBuffer);

	static OMX_ERRORTYPE AllocateBuffer(
		OMX_HANDLETYPE aComponent,
		OMX_BUFFERHEADERTYPE** apBuffer,
		OMX_U32 aPortIndex,
		OMX_PTR aAppData,
		OMX_U32 aSizeBytes);

	static OMX_ERRORTYPE FreeBuffer(
		OMX_HANDLETYPE aComponent,
		OMX_U32 aPortIndex,
		OMX_BUFFERHEADERTYPE* aBuffer);

	static OMX_ERRORTYPE EmptyThisBuffer(
		OMX_HANDLETYPE aComponent,
		OMX_BUFFERHEADERTYPE* aBuffer);

	static OMX_ERRORTYPE FillThisBuffer(
		OMX_HANDLETYPE aComponent,
		OMX_BUFFERHEADERTYPE* aBuffer);

	static OMX_ERRORTYPE SetCallbacks(
		OMX_HANDLETYPE aComponent,
		OMX_CALLBACKTYPE* aCallbacks,
		OMX_PTR aAppData);

	static OMX_ERRORTYPE ComponentDeInit(
		OMX_HANDLETYPE aComponent);

    static OMX_ERRORTYPE UseEGLImage(
		OMX_HANDLETYPE aComponent,
		OMX_BUFFERHEADERTYPE** appBufferHdr,
		OMX_U32 aPortIndex,
		OMX_PTR aAppPrivate,
		void* eglImage);

    static OMX_ERRORTYPE ComponentRoleEnum(
        OMX_HANDLETYPE aComponent,
		OMX_U8* aRole,
		OMX_U32 aIndex);
	
private:
	COmxILComponent* ipComponent;
	RBuf8 iComponentName;
	OMX_COMPONENTTYPE* ipHandle;
	OMX_PTR ipAppData;
	OMX_CALLBACKTYPE* ipCallbacks;
	COmxILFsm* ipFsm;
	COmxILConfigManager* ipConfigManager;
	MOmxILCallbackManagerIf* ipCallbackManager;
	MOmxILPortManagerIf* ipPortManager;
	COmxILProcessingFunction* ipProcessingFunction;
	};

#endif // OMXILCOMPONENTIMPL_H
