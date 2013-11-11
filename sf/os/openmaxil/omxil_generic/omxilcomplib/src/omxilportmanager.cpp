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

#include "log.h"
#include "omxilportmanager.h"
#include <openmax/il/common/omxilprocessingfunction.h>
#include "omxilcallbackmanager.h"
#include <openmax/il/common/omxilutil.h>

	/**
	   Static NewL for two-phase construction

	   @param   aProcessingFunction The component's processing function

	   @param   aCallbacks The component's callback manager

	   @param   aOmxVersion The IL Spec version in use

	   @param   aNumberOfAudioPorts Number of audio ports in the component

	   @param   aStartAudioPortNumber The start index for audio ports

	   @param   aNumberOfImagePorts Number of image ports in the component

	   @param   aStartImagePortNumber The start index for image ports

	   @param   aNumberOfVideoPorts Number of video ports in the component

	   @param   aStartVideoPortNumber The start index for video ports

	   @param   aNumberOfOtherPorts Number of other ports in the component

	   @param   aStartOtherPortNumber The start index for other ports

	   @param aImmediateReturnTimeBuffer This only applies to components with a
	   clock client port. Indicates whether the Port Manager must forward an
	   arriving clock buffer to the Callback Manager (ETrue) or to the
	   Processing Function (EFalse) . If the clock buffer is to be forwarded to
	   the Processing Function, this will happen using the BufferIndication
	   function of the component's PF. Otherwise, PF's MediaTimeIndication is
	   used instead.
	*/
COmxILPortManager*
COmxILPortManager::NewL(
	COmxILProcessingFunction& aProcessingFunction,
	MOmxILCallbackManagerIf& aCallbacks,
	const OMX_VERSIONTYPE& aOmxVersion,
	OMX_U32 aNumberOfAudioPorts,
	OMX_U32 aStartAudioPortNumber,
	OMX_U32 aNumberOfImagePorts,
	OMX_U32 aStartImagePortNumber,
	OMX_U32 aNumberOfVideoPorts,
	OMX_U32 aStartVideoPortNumber,
	OMX_U32 aNumberOfOtherPorts,
	OMX_U32 aStartOtherPortNumber,
	OMX_BOOL aImmediateReturnTimeBuffer)
	{
    DEBUG_PRINTF(_L8("COmxILPortManager::NewL"));

	COmxILPortManager* self =
		new (ELeave)COmxILPortManager(
			aProcessingFunction,
			aCallbacks);

	CleanupStack::PushL(self);
	self->ConstructL(aProcessingFunction,
					 aCallbacks,
					 aOmxVersion,
					 aNumberOfAudioPorts,
					 aStartAudioPortNumber,
					 aNumberOfImagePorts,
					 aStartImagePortNumber,
					 aNumberOfVideoPorts,
					 aStartVideoPortNumber,
					 aNumberOfOtherPorts,
					 aStartOtherPortNumber,
					 aImmediateReturnTimeBuffer);
	CleanupStack::Pop(self);
	return self;
	}

void
COmxILPortManager::ConstructL(
	COmxILProcessingFunction& /* aProcessingFunction */,
	MOmxILCallbackManagerIf& /* aCallbacks */,
	const OMX_VERSIONTYPE& aOmxVersion,
	OMX_U32 aNumberOfAudioPorts,
	OMX_U32 aStartAudioPortNumber,
	OMX_U32 aNumberOfImagePorts,
	OMX_U32 aStartImagePortNumber,
	OMX_U32 aNumberOfVideoPorts,
	OMX_U32 aStartVideoPortNumber,
	OMX_U32 aNumberOfOtherPorts,
	OMX_U32 aStartOtherPortNumber,
	OMX_BOOL aImmediateReturnTimeBuffer)
	{
    DEBUG_PRINTF(_L8("COmxILPortManager::ConstructL"));

	iImmediateReturnTimeBuffer = aImmediateReturnTimeBuffer;

	iAudioParamInit.nSize			 = sizeof(OMX_PORT_PARAM_TYPE);
	iAudioParamInit.nVersion		 = aOmxVersion;
	iAudioParamInit.nPorts			 = aNumberOfAudioPorts;
	iAudioParamInit.nStartPortNumber = aStartAudioPortNumber;

	iImageParamInit.nSize			 = sizeof(OMX_PORT_PARAM_TYPE);
	iImageParamInit.nVersion		 = aOmxVersion;
	iImageParamInit.nPorts			 = aNumberOfImagePorts;
	iImageParamInit.nStartPortNumber = aStartImagePortNumber;

	iVideoParamInit.nSize			 = sizeof(OMX_PORT_PARAM_TYPE);
	iVideoParamInit.nVersion		 = aOmxVersion;
	iVideoParamInit.nPorts			 = aNumberOfVideoPorts;
	iVideoParamInit.nStartPortNumber = aStartVideoPortNumber;

	iOtherParamInit.nSize			 = sizeof(OMX_PORT_PARAM_TYPE);
	iOtherParamInit.nVersion		 = aOmxVersion;
	iOtherParamInit.nPorts			 = aNumberOfOtherPorts;
	iOtherParamInit.nStartPortNumber = aStartOtherPortNumber;


	InsertParamIndexL(OMX_IndexParamAudioInit);
	InsertParamIndexL(OMX_IndexParamImageInit);
	InsertParamIndexL(OMX_IndexParamVideoInit);
	InsertParamIndexL(OMX_IndexParamOtherInit);


	if(iAudioParamInit.nStartPortNumber != 0)
		{
		User::Leave(KErrArgument);
		}

	if (iImageParamInit.nPorts > 0)
		{
		if (iAudioParamInit.nPorts !=
			iImageParamInit.nStartPortNumber)
			{
			User::Leave(KErrArgument);
			}
		}

	if (iVideoParamInit.nPorts > 0)
		{
		if ((iAudioParamInit.nPorts +
			 iImageParamInit.nPorts)  !=
			iVideoParamInit.nStartPortNumber)
			{
			User::Leave(KErrArgument);
			}
		}

	if (iOtherParamInit.nPorts > 0)
		{
		if ((iAudioParamInit.nPorts +
			 iImageParamInit.nPorts +
			 iVideoParamInit.nPorts)  !=
			iOtherParamInit.nStartPortNumber)
			{
			User::Leave(KErrArgument);
			}
		}

	}

COmxILPortManager::COmxILPortManager(
	COmxILProcessingFunction& aProcessingFunction,
	MOmxILCallbackManagerIf& aCallbacks)
	:
	iProcessingFunction(aProcessingFunction),
	iCallbacks(aCallbacks),
	iAllPorts()
	{
    DEBUG_PRINTF(_L8("COmxILPortManager::COmxILPortManager"));

	}

COmxILPortManager::~COmxILPortManager()
	{
    DEBUG_PRINTF(_L8("COmxILPortManager::~COmxILPortManager"));
	iAllPorts.ResetAndDestroy();
	iTimePorts.Close();
	}

void
COmxILPortManager::AppendPortL(const COmxILPort* aPort)
	{
    DEBUG_PRINTF(_L8("COmxILPortManager::AppendPort"));

	const TInt portCount = iAllPorts.Count();
	OMX_PORTDOMAINTYPE portDomain = aPort->Domain();
	OMX_U32 startPortNumber = 0;
	TBool timePort = EFalse;
	
	switch(portDomain)
		{
	case OMX_PortDomainAudio:
		{
		__ASSERT_ALWAYS(portCount >= 0,
						User::Panic(KOmxILPortManagerPanicCategory, 1));
		__ASSERT_ALWAYS(portCount < iAudioParamInit.nPorts,
						User::Panic(KOmxILPortManagerPanicCategory, 1));

		startPortNumber = iAudioParamInit.nStartPortNumber;

		}
		break;

	case OMX_PortDomainImage:
		{
		__ASSERT_ALWAYS(portCount >= iAudioParamInit.nPorts,
						User::Panic(KOmxILPortManagerPanicCategory, 1));
		__ASSERT_ALWAYS(portCount <
						iAudioParamInit.nPorts +
						iImageParamInit.nPorts,
						User::Panic(KOmxILPortManagerPanicCategory, 1));

		startPortNumber = iImageParamInit.nStartPortNumber;

		}
		break;

	case OMX_PortDomainVideo:
		{
		__ASSERT_ALWAYS(portCount >=
						iAudioParamInit.nPorts +
						iImageParamInit.nPorts,
						User::Panic(KOmxILPortManagerPanicCategory, 1));
		__ASSERT_ALWAYS(portCount <
						iAudioParamInit.nPorts +
						iImageParamInit.nPorts +
						iVideoParamInit.nPorts,
						User::Panic(KOmxILPortManagerPanicCategory, 1));

		startPortNumber = iVideoParamInit.nStartPortNumber;

		}
		break;


	case OMX_PortDomainOther:
		{
		__ASSERT_ALWAYS(portCount >=
						iAudioParamInit.nPorts +
						iImageParamInit.nPorts +
						iVideoParamInit.nPorts,
						User::Panic(KOmxILPortManagerPanicCategory, 1));
		__ASSERT_ALWAYS(portCount <
						iAudioParamInit.nPorts +
						iImageParamInit.nPorts +
						iVideoParamInit.nPorts +
						iOtherParamInit.nPorts,
						User::Panic(KOmxILPortManagerPanicCategory, 1));

		startPortNumber = iOtherParamInit.nStartPortNumber;
		
		OMX_OTHER_PARAM_PORTFORMATTYPE paramFormat;
		paramFormat.nSize = sizeof(OMX_OTHER_PARAM_PORTFORMATTYPE);
		paramFormat.nVersion = TOmxILSpecVersion();
		paramFormat.nPortIndex = aPort->Index();
		paramFormat.nIndex = 0;

		if ((aPort->GetParameter(OMX_IndexParamOtherPortFormat, &paramFormat) == OMX_ErrorNone) &&
			paramFormat.eFormat == OMX_OTHER_FormatTime)
			{
			timePort = ETrue;	
			}
		}
		
		break;

	default:
		{
		User::Panic(KOmxILPortManagerPanicCategory, 1);
		}
		};

	if(portCount ==  startPortNumber)
		{
		iAllPorts.AppendL(aPort);
		}
	else
		{
		for (TInt i=startPortNumber; i<portCount; ++i)
			{
			if (iAllPorts[i]->Index() >= aPort->Index())
				{
				User::Panic(KOmxILPortManagerPanicCategory, 1);
				}
			}
		iAllPorts.AppendL(aPort);
		}

	
    TRAPD(err, iTimePorts.AppendL(timePort));
    if (KErrNone != err)
        {
        iAllPorts.Remove(iAllPorts.Count()-1);
        User::Leave(err);
        }
 	}
 
void COmxILPortManager::RemoveLastAppendedPort()
	{
	if (iAllPorts.Count()>0 && iTimePorts.Count()>0)
		{
		iAllPorts.Remove(iAllPorts.Count()-1);
		iTimePorts.Remove(iTimePorts.Count()-1);	
		}
	}


/**
  This method is used at component's construction time, i.e., in a factory
  method of the component. The main component object uses this method to add
  the component's ports to its port manager instance.

   @param aPort The port instance to be added.

   @param aDirection The direction of the port being added.

   @ return A Symbian error code indicating if the function call was
   successful.  KErrNone on success, otherwise another of the system-wide error
   codes.
*/
TInt
COmxILPortManager::AddPort(const COmxILPort* aPort,
						   OMX_DIRTYPE aDirection)
	{
    DEBUG_PRINTF(_L8("COmxILPortManager::AddPort"));

	__ASSERT_ALWAYS(aPort, User::Panic(KOmxILPortManagerPanicCategory, 1));

	if (iAllPorts.Count() >= (iAudioParamInit.nPorts +
							  iVideoParamInit.nPorts +
							  iImageParamInit.nPorts +
							  iOtherParamInit.nPorts))
		{
		return KErrArgument;
		}

	if (aPort->Direction() != aDirection)
		{
		return KErrArgument;
		}

	if (iAllPorts.Find(aPort) != KErrNotFound)
		{
		return KErrArgument;
		}

	OMX_ERRORTYPE omxRetValue =
		aPort->GetLocalOmxParamIndexes(ManagedParamIndexes());
	if (OMX_ErrorNone == omxRetValue)
		{
		omxRetValue = aPort->GetLocalOmxConfigIndexes(ManagedConfigIndexes());
		}

	if (OMX_ErrorNone != omxRetValue)
		{
		if (OMX_ErrorInsufficientResources == omxRetValue)
			{
			return KErrNoMemory;
			}
		else
			{
			return KErrGeneral;
			}
		}
		
	TRAPD(err, AppendPortL(aPort));
	if (KErrNone != err)
		{
		return err;
		}
		
	// Here, let's register this port into the call back manager so buffer
	// marks can be propagated to the right port...
	const OMX_U32 propagationPortIndex = aPort->BufferMarkPropagationPort();
	if (propagationPortIndex != COmxILPort::KBufferMarkPropagationPortNotNeeded)
		{
		omxRetValue = iCallbacks.RegisterBufferMarkPropagationPort(aPort->Index(),
													 propagationPortIndex);
		}

	err = KErrNone;
	if (OMX_ErrorNone != omxRetValue)
		{
		RemoveLastAppendedPort();
		switch (omxRetValue)
				{
			case OMX_ErrorInsufficientResources:
				{
				err = KErrNoMemory;
				}
				break;
			default:
				{
				err = KErrGeneral;
				}
				};
		}

	return err;

	}

TBool
COmxILPortManager::RemoveBuffersFromPfOrCm(
	COmxILPort* apPort, OMX_BOOL aRemoveFromPfOnly /* = OMX_FALSE */) const
	{
	__ASSERT_DEBUG(apPort, User::Panic(KOmxILPortManagerPanicCategory, 1));
    DEBUG_PRINTF3(_L8("COmxILPortManager::RemoveBuffersFromPfOrCm: PORT[%d] aRemoveFromPfOnly[%s] "),
				  apPort->Index(), aRemoveFromPfOnly ? "TRUE" : "FALSE");

	const TInt headerCount = apPort->Count();
	OMX_BUFFERHEADERTYPE* pHeader = 0;
	TBool allHeadersRemovedFromPf = ETrue;
	for (TInt j=0; j<headerCount; ++j)
		{
		pHeader = (*apPort)[j];
		if (!apPort->IsBufferAtHome(pHeader))
			{
			// Tell the PF to remove this header from its queues...
			if (!iProcessingFunction.BufferRemovalIndication(
					pHeader,
					apPort->Direction()))
				{
				if (OMX_FALSE == aRemoveFromPfOnly)
					{
					if (!iCallbacks.BufferRemovalIndication(
							pHeader,
							apPort->Direction()))
						{
						allHeadersRemovedFromPf = EFalse;
						}
					else
						{
						apPort->SetBufferReturned(pHeader);
						// Make sure the buffer contents are cleared
						TOmxILUtil::ClearBufferContents(pHeader);
						}
					}
				}
			else
				{
				apPort->SetBufferReturned(pHeader);
				// Make sure the buffer contents are cleared
				TOmxILUtil::ClearBufferContents(pHeader);
				}
			}
		}

	return allHeadersRemovedFromPf;

	}

OMX_ERRORTYPE
COmxILPortManager::GetParameter(OMX_INDEXTYPE aParamIndex,
								TAny* apComponentParameterStructure) const
	{
    DEBUG_PRINTF(_L8("COmxILPortManager::GetParameter"));

	TInt index = FindParamIndex(aParamIndex);
	if (KErrNotFound == index)
		{
		return OMX_ErrorUnsupportedIndex;
		}

	OMX_ERRORTYPE omxRetValue = OMX_ErrorNone;
	switch(aParamIndex)
		{
	case OMX_IndexParamAudioInit:
	case OMX_IndexParamImageInit:
	case OMX_IndexParamVideoInit:
	case OMX_IndexParamOtherInit:
		{
		if (OMX_ErrorNone !=
			(omxRetValue =
			 TOmxILUtil::CheckOmxStructSizeAndVersion(
				 const_cast<OMX_PTR>(apComponentParameterStructure),
				 sizeof(OMX_PORT_PARAM_TYPE))))
			{
			return omxRetValue;
			}

		OMX_PORT_PARAM_TYPE*
			pPortParamType
			= static_cast<OMX_PORT_PARAM_TYPE*>(
				apComponentParameterStructure);

		switch(aParamIndex)
			{
		case OMX_IndexParamAudioInit:
			*pPortParamType = iAudioParamInit;
			break;
		case OMX_IndexParamImageInit:
			*pPortParamType = iImageParamInit;
			break;
		case OMX_IndexParamVideoInit:
			*pPortParamType = iVideoParamInit;
			break;
		case OMX_IndexParamOtherInit:
			*pPortParamType = iOtherParamInit;
			break;
			}

		}
		break;

	default:
		{
		// Obtain the port index
		OMX_U32 portIndex;
		if (OMX_ErrorNone != GetPortIndexFromOmxStruct(
				apComponentParameterStructure,
				portIndex))
			{
			return OMX_ErrorBadPortIndex;
			}

		// Now delegate to the specific port
		return iAllPorts[portIndex]->GetParameter(
			aParamIndex,
			apComponentParameterStructure);
		}
		};

	return OMX_ErrorNone;

	}

OMX_ERRORTYPE
COmxILPortManager::SetParameter(OMX_INDEXTYPE aParamIndex,
								const TAny* apComponentParameterStructure,
								OMX_BOOL aPortIsDisabled /* = OMX_FALSE */ )
	{
    DEBUG_PRINTF(_L8("COmxILPortManager::SetParameter"));

	TInt index = FindParamIndex(aParamIndex);
	if (KErrNotFound == index)
		{
		return OMX_ErrorUnsupportedIndex;
		}

	// Obtain the port index
	OMX_U32 portIndex;
	if (OMX_ErrorNone != GetPortIndexFromOmxStruct(
			apComponentParameterStructure,
			portIndex))
		{
		return OMX_ErrorBadPortIndex;
		}

    DEBUG_PRINTF2(_L8("COmxILPortManager::SetParameter : PORT[%u]"), portIndex);

	// Grab the port here...
	COmxILPort* pPort = iAllPorts[portIndex];

	if (OMX_TRUE == aPortIsDisabled &&
		pPort->IsEnabled() &&
		!pPort->IsTransitioningToEnabled())
		{
		// There is an indication from the FSM that the port must be disabled,
		// otherwise, this is not allowed in the current state.
		return OMX_ErrorIncorrectStateOperation;
		}

	OMX_ERRORTYPE omxRetValue = OMX_ErrorNone;
	switch(aParamIndex)
		{
	case OMX_IndexParamAudioInit:
	case OMX_IndexParamVideoInit:
	case OMX_IndexParamImageInit:
	case OMX_IndexParamOtherInit:
		{
		// Don't allow changes in the OMX_PORT_PARAM_TYPE structure
		return OMX_ErrorUnsupportedIndex;
		}
	default:
		{
		TBool updateProcessingFunction = EFalse;
		omxRetValue =
			pPort->SetParameter(
				aParamIndex,
				apComponentParameterStructure,
				updateProcessingFunction);

		if (OMX_ErrorNone == omxRetValue)
			{
			if (updateProcessingFunction)
				{
				omxRetValue = iProcessingFunction.ParamIndication(
					aParamIndex,
					apComponentParameterStructure);
				}
			}

		}
		};

	return omxRetValue;

	}

OMX_ERRORTYPE
COmxILPortManager::GetConfig(OMX_INDEXTYPE aConfigIndex,
							 TAny* apComponentConfigStructure) const
	{
    DEBUG_PRINTF(_L8("COmxILPortManager::GetConfig"));

	TInt index = FindConfigIndex(aConfigIndex);
	if (KErrNotFound == index)
		{
		return OMX_ErrorUnsupportedIndex;
		}

	// Obtain the port index
	OMX_U32 portIndex;
	if (OMX_ErrorNone != GetPortIndexFromOmxStruct(apComponentConfigStructure,
												   portIndex))
		{
		return OMX_ErrorBadPortIndex;
		}

	// Now delegate to the specific port
	return iAllPorts[portIndex]->GetConfig(
		aConfigIndex,
		apComponentConfigStructure);

	}

OMX_ERRORTYPE
COmxILPortManager::SetConfig(OMX_INDEXTYPE aConfigIndex,
							 const TAny* apComponentConfigStructure)
	{
	DEBUG_PRINTF(_L8("COmxILPortManager::SetConfig"));

#ifdef _OMXIL_COMMON_IL516C_ON

	if (OMX_IndexConfigPortBufferReturnRequest == aConfigIndex)
		{
		OMX_U32 portIndex;
		if (OMX_ErrorNone != GetPortIndexFromOmxStruct(
				apComponentConfigStructure,
				portIndex))
			{
			return OMX_ErrorBadPortIndex;
			}
		DEBUG_PRINTF2(_L8("COmxILPortManager::SetConfig : PORT[%u] OMX_IndexConfigPortBufferReturnRequest"), portIndex);

		// Check the index of the port..
		if ((OMX_ALL != portIndex) &&
			(CheckPortIndex(portIndex) != OMX_ErrorNone))
			{
			return OMX_ErrorBadPortIndex;
			}

		// This error will be ignored...
		OMX_ERRORTYPE omxRetValue = OMX_ErrorNone;
		if (portIndex != OMX_ALL)
			{
			omxRetValue = BufferEjectIndication(portIndex);
			}
		else
			{
			const TInt portCount = iAllPorts.Count();

			for (TUint i = 0; i< portCount; ++i)
				{
				omxRetValue = BufferEjectIndication(iAllPorts[i]->Index());
				}
			}

		return OMX_ErrorNone;

		}
#endif

	TInt index = FindConfigIndex(aConfigIndex);
	if (KErrNotFound == index)
		{
		return OMX_ErrorUnsupportedIndex;
		}

	// Obtain the port index
	OMX_U32 portIndex;
	if (OMX_ErrorNone != GetPortIndexFromOmxStruct(apComponentConfigStructure,
												   portIndex))
		{
		return OMX_ErrorBadPortIndex;
		}

    DEBUG_PRINTF2(_L8("COmxILPortManager::SetConfig : PORT[%u]"), portIndex);

	TBool updateProcessingFunction = EFalse;
	OMX_ERRORTYPE omxRetValue =
		iAllPorts[portIndex]->SetConfig(
			aConfigIndex,
			apComponentConfigStructure,
			updateProcessingFunction);

	if (OMX_ErrorNone == omxRetValue)
		{
		if (updateProcessingFunction)
			{
			omxRetValue = iProcessingFunction.ConfigIndication(
				aConfigIndex,
				apComponentConfigStructure);
			}
		}

	return omxRetValue;

	}

OMX_ERRORTYPE
COmxILPortManager::GetExtensionIndex(
	OMX_STRING aParameterName,
	OMX_INDEXTYPE* apIndexType) const
	{
    DEBUG_PRINTF(_L8("COmxILPortManager::GetExtensionIndex"));

	// See if the extension index is supported by any of the ports...
	const TInt portCount = iAllPorts.Count();
	OMX_ERRORTYPE retValue = OMX_ErrorNone;
	for (TUint i = 0; i< portCount; ++i)
		{
		retValue = iAllPorts[i]->GetExtensionIndex(aParameterName,
												   apIndexType);
		if (retValue != OMX_ErrorUnsupportedIndex)
			{
			return retValue;
			}
		}

	return OMX_ErrorUnsupportedIndex;

	}

OMX_ERRORTYPE
COmxILPortManager::PopulateBuffer(OMX_BUFFERHEADERTYPE** appBufferHdr,
								  OMX_U32 aPortIndex,
								  OMX_PTR apAppPrivate,
								  OMX_U32 aSizeBytes,
								  OMX_U8* apBuffer,
								  TBool& portPopulationCompleted,
								  OMX_BOOL aPortIsDisabled /* = OMX_FALSE */ )
	{
    DEBUG_PRINTF(_L8("COmxILPortManager::PopulateBuffer"));

	// Check the index of the port..
	if (CheckPortIndex(aPortIndex) != OMX_ErrorNone)
		{
		return OMX_ErrorBadPortIndex;
		}

	// Grab the port here...
	COmxILPort* pPort = iAllPorts[aPortIndex];

#ifdef _OMXIL_COMMON_IL516C_ON
	if (!apBuffer)
		{
		// Basically, if OMX_UseBuffer is used, the port must not be populated
		// at this point, otherwise it is an error...
		if (pPort->IsPopulated())
			{
			return OMX_ErrorIncorrectStateOperation;
			}
		}
	else
		{
		// ... AllocateBuffer... this is only allowed if the the IL Client has
		// issued the command to idle already..
#endif

	if (OMX_TRUE == aPortIsDisabled &&
		pPort->IsEnabled() &&
		!pPort->IsTransitioningToEnabled())
		{
		// There is an indication from the FSM that the port must be disabled,
		// otherwise, the buffer allocation is not allowed in the current
		// state. Note that a port may be transitioning to enabled and in that
		// case the buffer population must be allowed...
		return OMX_ErrorIncorrectStateOperation;
		}

#ifdef _OMXIL_COMMON_IL516C_ON
		}
#endif

	// Check that in case of tunnelling, this port is not buffer supplier...
	if (pPort->IsTunnelledAndBufferSupplier())
		{
		return OMX_ErrorBadPortIndex;
		}

	// Now delegate to the port...
	OMX_ERRORTYPE omxRetValue;
	if (OMX_ErrorNone != (omxRetValue = pPort->PopulateBuffer(
							  appBufferHdr,
							  apAppPrivate,
							  aSizeBytes,
							  apBuffer,
							  portPopulationCompleted)))
		{
		return omxRetValue;
		}

	if (portPopulationCompleted && pPort->IsTransitioningToEnabled())
		{
		// This is a case of port being enabled...  Inform the port that it
		// has been enabled..
		pPort->SetTransitionToEnabledCompleted();

		// For each enabled port, the IL Client must be notified with an
		// enabled completion event...
		omxRetValue = iCallbacks.CommandCompleteNotification(
			OMX_CommandPortEnable, pPort->Index());
		}

	return omxRetValue;

	}


OMX_ERRORTYPE
COmxILPortManager::FreeBuffer(OMX_U32 aPortIndex,
							  OMX_BUFFERHEADERTYPE* apBufferHeader,
							  TBool& portDepopulationCompleted,
							  OMX_BOOL aPortIsDisabled /* = OMX_FALSE */)
	{
    DEBUG_PRINTF3(_L8("COmxILPortManager::FreeBuffer : PORT[%u] BUFFER [%X]"), aPortIndex, apBufferHeader);

	// Check the index of the port..
	if (CheckPortIndex(aPortIndex) != OMX_ErrorNone)
		{
		return OMX_ErrorBadPortIndex;
		}

	// Grab the port here...
	COmxILPort* pPort = iAllPorts[aPortIndex];

	// Check that in case of tunnelling, this port is not buffer supplier...
	if (pPort->IsTunnelledAndBufferSupplier())
		{
		return OMX_ErrorBadPortIndex;
		}

	TBool errorPortUnpopulated = EFalse;
	if (OMX_TRUE == aPortIsDisabled &&
		pPort->IsEnabled())
		{
		// There is an indication from the FSM that the port should be
		// disabled. If that's not the case, the buffer deallocation will raise
		// an OMX_ErrorPortUnpopulated error in the current state.

		if (!pPort->IsBufferAtHome(apBufferHeader))
			{
			// FreeBuffer will normally be called in a situation where we are
			// not suppliers and the supplier already got the buffer. So the
			// buffer won't be on our side almost never.

			if (!iTimePorts[aPortIndex] || !iImmediateReturnTimeBuffer)
				{
				// We'll tell the PF to remove this
				// header from its queues, in case this is called in some strange
				// situation (should not happen if the tunnelled component is well
				// behaved)...
				iProcessingFunction.BufferRemovalIndication(
					apBufferHeader,
					pPort->Direction());
				}

			// Set the returned flag as this buffer will not count as "away"
			// anymore...
			pPort->SetBufferReturned(apBufferHeader);
			}

		// We have to send the Port Unpopulated Error...
		errorPortUnpopulated = ETrue;
		}

	OMX_ERRORTYPE omxRetValue = OMX_ErrorNone;
	// Now delegate to the port...
	if (OMX_ErrorNone != (omxRetValue = pPort->FreeBuffer(
							  apBufferHeader,
							  portDepopulationCompleted)))
		{
		return omxRetValue;
		}

	if (errorPortUnpopulated)
		{
		if (OMX_ErrorNone !=
			(omxRetValue =
			 iCallbacks.ErrorEventNotification(OMX_ErrorPortUnpopulated)))
			{
			return omxRetValue;
			}
		}


	if (portDepopulationCompleted)
		{
		if (pPort->IsTransitioningToDisabled())
			{
			// Here we must complete the OMX_CommandPortDisable command

			// Set the state of the port to  disabled as the command has already
			// completed...
			pPort->SetTransitionToDisabledCompleted();

			// For each disabled port, the IL Client must be notified
			// with a disabled completion event...
			omxRetValue = iCallbacks.CommandCompleteNotification(
				OMX_CommandPortDisable, aPortIndex);

			// Clear this flag here. Otherwise, the FSM would inform the client
			// of a successful transition to OMX_StateIdle which is not the
			// case...
			portDepopulationCompleted = EFalse;
			}
		}

	return omxRetValue;

	}

OMX_ERRORTYPE
COmxILPortManager::TunnelRequest(OMX_U32 aPortIndex,
								 OMX_HANDLETYPE aTunneledComp,
								 OMX_U32 aTunneledPort,
								 OMX_TUNNELSETUPTYPE* apTunnelSetup,
								 OMX_BOOL aPortIsDisabled /* = OMX_FALSE */)
	{
    DEBUG_PRINTF3(_L8("COmxILPortManager::TunnelRequest : aTunneledComp [%X] aTunneledPort [%d]"), aTunneledComp, aTunneledPort);

	// Check the index of the port..
	if (CheckPortIndex(aPortIndex) != OMX_ErrorNone)
		{
		return OMX_ErrorBadPortIndex;
		}

	// Grab the port here...
	COmxILPort* pPort = iAllPorts[aPortIndex];

	if (OMX_TRUE == aPortIsDisabled &&
		pPort->IsEnabled())
		{
		// There is an indication from the FSM that the port must be disabled,
		// otherwise, the tunnel request is not allowed in the current state.
		return OMX_ErrorIncorrectStateOperation;
		}

	OMX_ERRORTYPE omxRetValue = OMX_ErrorNone;
	// Check whether the tunnel is being torn down...
	if (!aTunneledComp)
		{
		// Tell the port...
		if (OMX_ErrorNone !=
			(omxRetValue = pPort->TunnelRequest(
				aTunneledComp,
				aTunneledPort,
				apTunnelSetup)))
			{
			return omxRetValue;
			}

		if (OMX_ErrorNone !=
			(omxRetValue = iCallbacks.DeregisterTunnelCallback(aPortIndex)))
			{
			// This is serious enough...
			return OMX_ErrorInvalidComponent;
			}

		// We are done here...
		return OMX_ErrorNone;
		}

	// Check whether the port is being re-tunnelled...
	if (pPort->IsTunnelled())
		{
		// Only two valid options here:
		// 1.- The port is completely disabled...
		// or...
		// 2.- The port is enabled AND component in OMX_StateLoaded
		if ((!pPort->IsEnabled() &&
			 !pPort->IsTransitioningToEnabled() &&
			 !pPort->IsTransitioningToDisabled())
			||
			(pPort->IsEnabled() && !aPortIsDisabled))
			{
			if (OMX_ErrorNone !=
				(omxRetValue = iCallbacks.DeregisterTunnelCallback(aPortIndex)))
				{
				return OMX_ErrorInvalidComponent;
				}
			}
		else
			{
			return OMX_ErrorIncorrectStateOperation;
			}
		}

	// Delegate to the port...
	if (OMX_ErrorNone !=
		(omxRetValue = pPort->TunnelRequest(
			aTunneledComp,
			aTunneledPort,
			apTunnelSetup)))
		{
		return omxRetValue;
		}

	// From this point on, the port will assume that a tunnel has been
	// successfully created. If there is a problem at other end, the IL Client
	// will clear the tunnel on this side using ComponentTunnelRequest with
	// NULL handle parameter

	// Register the existence of a tunnel within the callback manager...
	if (OMX_ErrorNone !=
		(omxRetValue = iCallbacks.RegisterTunnelCallback(aPortIndex,
													  pPort->Direction(),
													  aTunneledComp,
													  aTunneledPort)))
		{
		// This is serious enough...
		return OMX_ErrorInvalidComponent;
		}

	return OMX_ErrorNone;

	}


OMX_ERRORTYPE
COmxILPortManager::TunnellingBufferAllocation(TBool& aComponentPopulationCompleted,
											  TUint32 aPortIndex /* = OMX_ALL */)
	{
    DEBUG_PRINTF(_L8("COmxILPortManager::TunnellingBufferAllocation"));

	aComponentPopulationCompleted = EFalse;

	// Check the index of the port..
	if ((OMX_ALL != aPortIndex) && (CheckPortIndex(aPortIndex) != OMX_ErrorNone))
		{
		return OMX_ErrorBadPortIndex;
		}

	const TInt portCount = iAllPorts.Count();
	COmxILPort* pPort = 0;
	OMX_U32 portIndex = 0;
	TInt i=0;
	OMX_ERRORTYPE omxRetValue = OMX_ErrorNone;
	do
		{
		if (aPortIndex != OMX_ALL)
			{
			// Grab the port here...
			pPort = iAllPorts[aPortIndex];
			portIndex = aPortIndex;
			}
		else
			{
			pPort = iAllPorts[i];
			portIndex = pPort->Index();
			}


		if (pPort->IsEnabled() &&
			pPort->IsTunnelledAndBufferSupplier() &&
			!pPort->IsPopulated())
			{
			TBool portPopulationCompleted = EFalse;
			if (OMX_ErrorNone !=
				(omxRetValue = pPort->PopulateTunnel(portPopulationCompleted)))
				{
				// TODO: Check case of ports being enabled (error callback needed...)
				return omxRetValue;
				}

			if (portPopulationCompleted && pPort->IsTransitioningToEnabled())
				{
				// This is a case of port being enabled...  Inform the port that it
				// has been enabled..
				pPort->SetTransitionToEnabledCompleted();

				// For each enabled port, the IL Client must be notified with an
				// enabled completion event...
				if (OMX_ErrorNone != (
						omxRetValue = iCallbacks.CommandCompleteNotification(
							OMX_CommandPortEnable, portIndex)))
					{
					return omxRetValue;
					}
				}

			}

		// Increment loop counter
		++i;
		}
	while (OMX_ALL == aPortIndex && i < portCount);

	if (AllPortsPopulated())
		{
		DEBUG_PRINTF(_L8("COmxILPortManager::TunnellingBufferAllocation : AllPortsPopulated : [TRUE]"));
		aComponentPopulationCompleted = ETrue;
		}

	return OMX_ErrorNone;

	}

OMX_ERRORTYPE
COmxILPortManager::TunnellingBufferDeallocation(
	TBool& aComponentDepopulationCompleted)
	{
    DEBUG_PRINTF(_L8("COmxILPortManager::TunnellingBufferDeallocation"));

	aComponentDepopulationCompleted = EFalse;

	const TInt portCount = iAllPorts.Count();
	COmxILPort* pPort = 0;
	TInt portDepopulationCounter = 0;
	OMX_ERRORTYPE omxRetValue = OMX_ErrorNone;
	TBool portDepopulationCompleted = EFalse;
	for (TInt i=0; i<portCount; ++i)
		{
		pPort = iAllPorts[i];
		if (pPort->IsEnabled() && pPort->IsTunnelledAndBufferSupplier())
			{
			// TODO: Check that at this point, the ProcessingFunction must not
			// hold any buffer header...
			portDepopulationCompleted = EFalse;
			if (OMX_ErrorNone !=
				(omxRetValue = pPort->FreeTunnel(portDepopulationCompleted)))
				{
				return omxRetValue;
				}

			if (pPort->IsDePopulated())
				{
				portDepopulationCounter++;
				}
			}
		}

	if (AllPortsDePopulated())
		{
		DEBUG_PRINTF(_L8("COmxILPortManager::TunnellingBufferDeallocation : AllPortsDepopulated : [TRUE]"));
		aComponentDepopulationCompleted = ETrue;
		}

	return OMX_ErrorNone;

	}

OMX_ERRORTYPE
COmxILPortManager::InitiateTunnellingDataFlow(
	OMX_U32 aPortIndex /* = OMX_ALL */,
	OMX_BOOL aSuppliersAndNonSuppliers /* = OMX_FALSE */)
	{
    DEBUG_PRINTF2(_L8("COmxILPortManager::InitiateTunnellingDataFlow : PORT[%d]"), aPortIndex);

	// Check the index of the port..
	if ((OMX_ALL != aPortIndex) && (CheckPortIndex(aPortIndex) != OMX_ErrorNone))
		{
		return OMX_ErrorBadPortIndex;
		}

	// This is an indication that the component is ready to start exchanging
	// buffers... Supplier tunnelled ports must initiate the buffer exchange in
	// the tunnel...
	const TInt portCount = iAllPorts.Count();
	COmxILPort* pPort = 0;
	OMX_U32 portIndex = 0;
	TInt i=0;
	do
		{
		if (aPortIndex != OMX_ALL)
			{
			// Grab the port here...
			pPort = iAllPorts[aPortIndex];
			portIndex = aPortIndex;
			}
		else
			{
			pPort = iAllPorts[i];
			portIndex = pPort->Index();
			}

		if (pPort->IsEnabled() &&
			((OMX_TRUE == aSuppliersAndNonSuppliers) ||
			 pPort->IsTunnelledAndBufferSupplier()))
			{
			const TInt headerCount = pPort->Count();
			OMX_BUFFERHEADERTYPE* pHeader = 0;
			OMX_DIRTYPE portDir = OMX_DirMax;
			OMX_ERRORTYPE omxRetValue = OMX_ErrorNone;
			for (TInt j=0; j<headerCount; ++j)
				{
				// Here, there are two options:
				//
				// 1. -The port is an input port, in which case, the buffer is
				// directly sent to the callback manager which in turn sends it
				// to the tunnelled component in charge of filling it
				// (OMX_FillThisBuffer)
				//
				// 2.- The port is an output port, and the buffer must be sent
				// to the processing function to be filled before the buffer
				// can be sent to the tunnelled component. After that, the proc
				// function will send the buffer to the call back manager to be
				// sent to the tunnelled component (OMX_EmptyThisBuffer)...
				pHeader = (*pPort)[j];

				// Check that we actually have the buffer in our side...
				if (!pPort->IsBufferAtHome(pHeader))
					{
					DEBUG_PRINTF2(_L8("COmxILPortManager::InitiateTunnellingDataFlow : BUFFER HEADER[%X] is not at home"),
								  pHeader);
					continue;
					}

				portDir = pPort->Direction();
				__ASSERT_DEBUG((OMX_DirInput == portDir ||
								OMX_DirOutput == portDir),
							   User::Panic(KOmxILPortManagerPanicCategory, 1));

				if (OMX_DirInput == portDir)
					{
					// Input port -> Send buffer to callback manager...
					omxRetValue =
						iCallbacks.BufferDoneNotification(pHeader,
														  portIndex,
														  portDir);
					}
				else
					{
					// Output port -> Send buffer to processing function...
					omxRetValue =
						iProcessingFunction.BufferIndication(pHeader,
															 portDir);
					}

				if (omxRetValue != OMX_ErrorNone)
					{
					return omxRetValue;
					}

				// Inform the port that one of its buffers has been sent
				// away...
				TBool bufferMarkedWithOwnMark = EFalse;
				if (!pPort->SetBufferSent(pHeader, bufferMarkedWithOwnMark))
					{
					// The buffer header was not found...
					return OMX_ErrorBadParameter;
					}

				// For each MarkBuffer command processed, the IL Client must be notified
				// with an OMX_EventCommandComplete event...
				if (bufferMarkedWithOwnMark)
					{
					if (OMX_ErrorNone !=
						(omxRetValue = iCallbacks.CommandCompleteNotification(
							OMX_CommandMarkBuffer, portIndex)))
						{
						return omxRetValue;
						}
					}

				} // for (TInt j=0; j<headerCount; j++)
			} // if (pPort->IsEnabled() && pPort->IsTunnelledAndBufferSupplier())

		// Increment loop counter
		++i;
		}
	while(OMX_ALL == aPortIndex && i < portCount);

	return OMX_ErrorNone;

	}

OMX_ERRORTYPE
COmxILPortManager::BufferIndication(
	OMX_BUFFERHEADERTYPE* apBufferHeader,
	OMX_DIRTYPE aDirection,
	OMX_BOOL aPortIsDisabled /* = OMX_FALSE */)
	{

	OMX_U32 portIndex = aDirection == OMX_DirInput ?
		apBufferHeader->nInputPortIndex : apBufferHeader->nOutputPortIndex;

    DEBUG_PRINTF3(_L8("COmxILPortManager::BufferIndication : PORT[%u] BUFFER [%X] "),
				  portIndex, apBufferHeader);

	// Check the index of the port..
	if (CheckPortIndex(portIndex) != OMX_ErrorNone)
		{
		return OMX_ErrorBadPortIndex;
		}

	// Grab the port here...
	COmxILPort* pPort = iAllPorts[portIndex];

	// Check that port direction is the correct one...
	if (pPort->Direction() != aDirection)
		{
		return OMX_ErrorBadPortIndex;
		}

#ifdef _OMXIL_COMMON_IL516C_ON
	(void)aPortIsDisabled;
	// Only restriction here is that the port must be populated
	if (!pPort->IsPopulated())
		{
		return OMX_ErrorIncorrectStateOperation;
		}
#endif

#ifndef _OMXIL_COMMON_IL516C_ON
	if (!pPort->IsEnabled() &&
		!pPort->IsTransitioningToDisabled() &&
		!pPort->IsTransitioningToEnabled())
		{
		return OMX_ErrorIncorrectStateOperation;
		}

	// Check port enabled property...
	if (OMX_TRUE == aPortIsDisabled &&
		pPort->IsEnabled())
		{
		// There is an indication from the FSM that the port must be disabled,
		// otherwise, the buffer indication is not allowed in the current
		// state.
		return OMX_ErrorIncorrectStateOperation;
		}

#endif

	OMX_ERRORTYPE omxRetValue = OMX_ErrorNone;
	// Check whether this port is a buffer supplier...
	if (pPort->IsTunnelledAndBufferSupplier() &&
		pPort->IsTransitioningToDisabled())
		{
		// Set the state of the port to disabled as the command has already
		// completed...
		if (!pPort->SetBufferReturned(apBufferHeader))
			{
			// The buffer header was not found...
			return OMX_ErrorBadParameter;
			}

		if (pPort->HasAllBuffersAtHome())
			{
			// All buffers at home.. we can initiate here the
			// depopulation of the tunnel...
			TBool portDepopulationCompleted = EFalse;
			if (OMX_ErrorNone !=
				(omxRetValue = pPort->FreeTunnel(portDepopulationCompleted)))
				{
				return omxRetValue;
				}

			// Inform the port that the disabled command has
			// completed...
			pPort->SetTransitionToDisabledCompleted();

			// For each disabled port, the IL Client must be notified
			// with a port disabled completion event...
			if (OMX_ErrorNone !=
				(omxRetValue = iCallbacks.CommandCompleteNotification(
					OMX_CommandPortDisable, portIndex)))
				{
				return omxRetValue;
				}
			}

		// Make sure the buffer contents are cleared...
		TOmxILUtil::ClearBufferContents(apBufferHeader);

		// ... we are done.. no need to send the buffer to the
		// processing function...
		return OMX_ErrorNone;

		} // if ((pPort->IsTunnelledAndBufferSupplier() && pPort->IsTransitioningToDisabled())
#ifdef _OMXIL_COMMON_IL516C_ON
	else if (pPort->IsTunnelled() && pPort->IsTransitioningToDisabled())
		{
		// We get here if the port is tunnelled, non-supplier and is currently
		// in the process of transitioning to disabled...  To avoid Race
		// condition #3, we need to check that we've received the request from
		// the supplier to return its buffers... however, we don't check it for
		// now, it is just easier to return the buffer now...
		DEBUG_PRINTF3(_L8("COmxILPortManager::BufferIndication : PORT [%u] BUFFER [%X] : "
						  "WARNING This port is being disabled, "
						  "the buffer id being returned to the tunnelled component"),
						  portIndex, apBufferHeader);
		omxRetValue =
			iCallbacks.BufferDoneNotification(apBufferHeader,
											  portIndex,
											  aDirection);
		// ... we are done..
		return OMX_ErrorNone;
		}
#endif


	// Inform the port that one of its buffers is going to be sent to the
	// processing function (exception applies to OMX_PortDomainOther ports) ... 
	// The port will also mark this buffer if the port
	// has pending marks to be signalled... The buffer marks are finally
	// processed/propagated by the callback manager once the buffer has been
	// consumed by the processing function...
	TBool bufferMarkedWithOwnMark = EFalse;
	if (!pPort->SetBufferSent(apBufferHeader, bufferMarkedWithOwnMark))
		{
		return OMX_ErrorBadParameter;
		}

	// For each MarkBuffer command processed, the IL Client must be notified
	// with an OMX_EventCommandComplete event...
	if (bufferMarkedWithOwnMark)
		{
		if (OMX_ErrorNone !=
			(omxRetValue = iCallbacks.CommandCompleteNotification(
				OMX_CommandMarkBuffer, portIndex)))
			{
			return omxRetValue;
			}
		}

	if (iImmediateReturnTimeBuffer && iTimePorts[portIndex])
		{
		// OMX_OTHER_FormatTime ports (such as COmxILClientClockPort) needs
		// to return the buffer sooner to the buffer supplier component
		OMX_TIME_MEDIATIMETYPE* pMediaTime =
			reinterpret_cast<OMX_TIME_MEDIATIMETYPE*>(apBufferHeader->pBuffer);

		OMX_TIME_MEDIATIMETYPE timeInfo;
		if (pMediaTime)
			{
			timeInfo = *pMediaTime;
			}

		// Return the buffer (send the Buffer Done notification) via callback now.
		apBufferHeader->nFilledLen = 0;

		OMX_ERRORTYPE err = iCallbacks.ClockBufferDoneNotification(
			apBufferHeader, portIndex, aDirection);

		if (err != OMX_ErrorNone)
			{
			return err;
			}

		if (pMediaTime)
			{
			// Send time update to the processing function
			err = iProcessingFunction.MediaTimeIndication(timeInfo);
			}

		__ASSERT_DEBUG(err != OMX_ErrorNotImplemented,
					   User::Panic(KOmxILPortManagerPanicCategory, 1));

		return err;
		}
	
	return iProcessingFunction.BufferIndication(apBufferHeader,
												aDirection);
	}


OMX_ERRORTYPE
COmxILPortManager::BufferReturnIndication(
	OMX_BUFFERHEADERTYPE* apBufferHeader,
	OMX_DIRTYPE aDirection,
	TBool& aAllBuffersReturned)
	{

	aAllBuffersReturned = EFalse;

	OMX_U32 portIndex = aDirection == OMX_DirInput ?
		apBufferHeader->nInputPortIndex : apBufferHeader->nOutputPortIndex;

    DEBUG_PRINTF3(_L8("COmxILPortManager::BufferReturnIndication : PORT[%u] BUFFER [%X]"),
				  portIndex, apBufferHeader);

	// Check the index of the port..
	if (CheckPortIndex(portIndex) != OMX_ErrorNone)
		{
		return OMX_ErrorBadPortIndex;
		}

	// Grab the port here...
	COmxILPort* pPort = iAllPorts[portIndex];

	// Check that port direction is the correct one...
	if (pPort->Direction() != aDirection)
		{
		return OMX_ErrorBadPortIndex;
		}


	if (!pPort->IsEnabled())
		{
		return OMX_ErrorBadPortIndex;
		}

	// Check that this port must be buffer supplier...
	if (!pPort->IsTunnelledAndBufferSupplier())
		{
		return OMX_ErrorBadPortIndex;
		}

	// Inform the port that a buffer has returned...
	if (!pPort->SetBufferReturned(apBufferHeader))
		{
		// The buffer header was not found...
		return OMX_ErrorBadParameter;
		}

	// Make sure the buffer contents are cleared...
	TOmxILUtil::ClearBufferContents(apBufferHeader);

	if (pPort->HasAllBuffersAtHome())
		{
		aAllBuffersReturned = ETrue;
		}

	return OMX_ErrorNone;

	}

OMX_ERRORTYPE
COmxILPortManager::BufferFlushIndicationFlushCommand(
	TUint32 aPortIndex, TBool aEjectBuffers /* = ETrue */)
	{
    DEBUG_PRINTF2(_L8("COmxILPortManager::BufferFlushIndicationFlushCommand : PORT[%u]"), aPortIndex);

	// Check the index of the port..
	if ((OMX_ALL != aPortIndex) && (CheckPortIndex(aPortIndex) != OMX_ErrorNone))
		{
		return OMX_ErrorBadPortIndex;
		}

	OMX_ERRORTYPE omxRetValue = OMX_ErrorNone;
	RPointerArray<COmxILPort> portsToFlush;
	RPointerArray<COmxILPort> portsToNotify;
	const TInt portCount = iAllPorts.Count();
	COmxILPort* pPort = 0;
	TInt i=0;
	TBool flushSuccessful;
	do
		{
		flushSuccessful = ETrue;
		if (aPortIndex != OMX_ALL)
			{
			// Grab the port here...
			pPort = iAllPorts[aPortIndex];
			}
		else
			{
			pPort = iAllPorts[i];
			}

		if (pPort->IsEnabled() && pPort->Count())
			{
			// If port is tunnelled:
			//
			// - Output ports buffers are sent to the processing function when
			// flushed
			//
			// - Input ports buffers are sent upstream.
			//
			// If port is non-tunnelled:
			// - Buffers are ejected always...
			if (pPort->IsTunnelled())
				{
				// Remove buffers from PF only...
				RemoveBuffersFromPfOrCm(pPort, OMX_TRUE);

				if (aEjectBuffers)
					{
					// Now we need to send input buffers upstream and ouput
					// buffers to the PF... this is done by
					// InitiateTunnellingDataFlow
					if ((omxRetValue = InitiateTunnellingDataFlow(
							 pPort->Index(), OMX_TRUE)) != OMX_ErrorNone)
						{
						// The flush has failed, we need to notify the IL Cient
						// via EventHandler...
						iCallbacks.ErrorEventNotification(omxRetValue);
						flushSuccessful = EFalse;
						}
					}
				}
			else
				{
				// All other ports are simply flushed...
				portsToFlush.Append(pPort);
				}
			}
		// Notify flushing completed, even if there was nothing to
		// flush...
		if (flushSuccessful)
			{
			portsToNotify.Append(pPort);
			}

		// Increment loop counter
		++i;
		}
	while(OMX_ALL == aPortIndex && i < portCount);

	const TInt flushCount = portsToFlush.Count();
	const TInt notifyCount = portsToNotify.Count();


	if (iAllPorts.Count() == flushCount)
		{
		omxRetValue = iProcessingFunction.BufferFlushingIndication(
			OMX_ALL,
			OMX_DirMax);
		}
	else
		{
		for (i=0; i<flushCount && OMX_ErrorNone == omxRetValue; ++i)
			{
			pPort = portsToFlush[i];
			omxRetValue = iProcessingFunction.BufferFlushingIndication(
				pPort->Index(),
				pPort->Direction());
			}
		}

	for (i=0; i<notifyCount && OMX_ErrorNone == omxRetValue; ++i)
		{
		pPort = portsToNotify[i];
		omxRetValue = iCallbacks.CommandCompleteNotification(OMX_CommandFlush,
											   portsToNotify[i]->Index());
		}

	portsToFlush.Close();
	portsToNotify.Close();

	return omxRetValue;

	}

OMX_ERRORTYPE
COmxILPortManager::BufferFlushIndicationPauseOrExeToIdleCommand(
	TBool& aAllBuffersReturnedToSuppliers)
	{
    DEBUG_PRINTF(_L8("COmxILPortManager::BufferFlushIndicationPauseOrExeToIdleCommand"));

	aAllBuffersReturnedToSuppliers = EFalse;

	const TInt portCount = iAllPorts.Count();
	TBool foundBufferSupplierThatNeedsToWait = EFalse;
	COmxILPort* pPort = 0;
	OMX_ERRORTYPE omxRetValue = OMX_ErrorNone;
	for (TInt i=0; i<portCount; ++i)
		{
		pPort = iAllPorts[i];

		if (pPort->Count())
			{
			if (pPort->IsTunnelledAndBufferSupplier() &&
				!pPort->HasAllBuffersAtHome())
				{
				if (!RemoveBuffersFromPfOrCm(pPort))
					{
					// This port will have to wait to get some of its buffers
					// returned by the tunnelled port...
					foundBufferSupplierThatNeedsToWait = ETrue;
#ifdef _OMXIL_COMMON_IL516C_ON
					// Request buffer ejection from the tunnelled component
					iCallbacks.EjectBuffersRequest(pPort->Index());
#endif
					}
				continue;
				}

#ifdef _OMXIL_COMMON_IL516C_ON
			if (!pPort->IsTunnelled())
				{
#endif

			if (OMX_ErrorNone !=
				(omxRetValue = iProcessingFunction.BufferFlushingIndication(
					pPort->Index(),
					pPort->Direction())))
				{
				return omxRetValue;
				}

#ifdef _OMXIL_COMMON_IL516C_ON
				}
#endif

			}
		}

	if (!foundBufferSupplierThatNeedsToWait)
		{
		aAllBuffersReturnedToSuppliers = ETrue;
		}

	return OMX_ErrorNone;

	}

OMX_ERRORTYPE
COmxILPortManager::PortEnableIndication(
	TUint32 aPortIndex,
	TBool aIndicationIsFinal)
	{
    DEBUG_PRINTF3(_L8("COmxILPortManager::PortEnableIndication: PORT[%u] TRANSITIONISFINAL[%d]"), aPortIndex, aIndicationIsFinal);

	// Check the index of the port..
	if ((OMX_ALL != aPortIndex) && (CheckPortIndex(aPortIndex) != OMX_ErrorNone))
		{
		return OMX_ErrorBadPortIndex;
		}

	const TInt portCount = iAllPorts.Count();
	COmxILPort* pPort = 0;
	OMX_U32 portIndex = 0;
	TInt i=0;
	OMX_ERRORTYPE omxRetValue = OMX_ErrorNone;
	do
		{
		// We do this so we loop or not depending on the needs...
		if (aPortIndex != OMX_ALL)
			{
			// Grab the port here...
			pPort = iAllPorts[aPortIndex];
			portIndex = aPortIndex;
			}
		else
			{
			pPort = iAllPorts[i];
			portIndex = pPort->Index();
			}

		// If port is already enabled, just indicate that the enable command has
		// completed successfully
		if (pPort->IsEnabled())
			{
			if (OMX_ErrorNone !=
				(omxRetValue =
				 iCallbacks.CommandCompleteNotification(
					 OMX_CommandPortEnable, portIndex)))
				{
				return omxRetValue;
				}

			++i;
			continue;
			}

		// First check that no-one port is currently transitioning to
		// Enabled...
		if (pPort->IsTransitioningToEnabled() ||
			pPort->IsTransitioningToDisabled())
			{
			// Send an error event...  The spec mandates that the nData2 and
			// the pEventData are 0 and NULL respectively, but they could be
			// used here to hand some information like the index of the port
			// that has failed...
			if (OMX_ErrorNone !=
				(omxRetValue =
				 iCallbacks.ErrorEventNotification(OMX_ErrorPortUnresponsiveDuringAllocation)))
				{
				return omxRetValue;
				}

			if (OMX_ALL == aPortIndex)
				{
				++i;
				continue;
				}
			else
				{
				return OMX_ErrorUndefined;
				}
			}

		if (aIndicationIsFinal)
			{
			// Inform the port that it has been enabled..
			pPort->SetTransitionToEnabledCompleted();

			// For each enabled port, the IL Client must be notified
			// with an enabled completion event...
			if (OMX_ErrorNone !=
				(omxRetValue =
				 iCallbacks.CommandCompleteNotification(
					 OMX_CommandPortEnable, portIndex)))
				{
				return omxRetValue;
				}

			}
		else
			{
			// Inform the port that it is being enabled..
			pPort->SetTransitionToEnabled();
			}

		// Increment loop counter
		++i;
		}
	while(OMX_ALL == aPortIndex && i < portCount);

	return OMX_ErrorNone;

	}

OMX_ERRORTYPE
COmxILPortManager::PortDisableIndication(
	TUint32 aPortIndex)
	{
    DEBUG_PRINTF2(_L8("COmxILPortManager::PortDisableIndication: PORT[%u] "), aPortIndex);

	// Check the index of the port..
	if ((OMX_ALL != aPortIndex) && (CheckPortIndex(aPortIndex) != OMX_ErrorNone))
		{
		return OMX_ErrorBadPortIndex;
		}

	const TInt portCount = iAllPorts.Count();
	COmxILPort* pPort = 0;
	OMX_U32 portIndex = 0;
	TInt i=0;
	OMX_ERRORTYPE omxRetValue = OMX_ErrorNone;
	do
		{
		if (aPortIndex != OMX_ALL)
			{
			// Grab the port here...
			pPort = iAllPorts[aPortIndex];
			portIndex = aPortIndex;
			}
		else
			{
			pPort = iAllPorts[i];
			portIndex = pPort->Index();
			}

		// If port is already disabled, just indicate that the disable command has
		// completed successfully
		if (!pPort->IsEnabled())
			{
			if (OMX_ErrorNone !=
				(omxRetValue =
				 iCallbacks.CommandCompleteNotification(
					 OMX_CommandPortDisable, portIndex)))
				{
				return omxRetValue;
				}

			++i;
			continue;
			}

		// First check that no-one port is currently transitioning to
		// Disabled...
		if (pPort->IsTransitioningToDisabled() ||
			pPort->IsTransitioningToEnabled())
			{
			// Send an error event...  The spec mandates that the nData2 and
			// the pEventData are 0 and NULL respectively, but they could be
			// used here to hand some information like the index of the port
			// that has failed...
			if (OMX_ErrorNone !=
				(omxRetValue =
				 iCallbacks.ErrorEventNotification(OMX_ErrorPortUnresponsiveDuringAllocation)))
				{
				return omxRetValue;
				}

			if (OMX_ALL == aPortIndex)
				{
				++i;
				continue;
				}
			else
				{
				return OMX_ErrorUndefined;
				}

			}

		if (pPort->IsTunnelledAndBufferSupplier())
			{
			if (!pPort->HasAllBuffersAtHome() && !RemoveBuffersFromPfOrCm(pPort))
				{
				// Inform the port that it is being disabled
				pPort->SetTransitionToDisabled();
				// This port will have to wait to get all its buffers
				// returned by the tunnelled port...
#ifdef _OMXIL_COMMON_IL516C_ON
				//... but request the ejection of the buffers first...
				iCallbacks.EjectBuffersRequest(pPort->Index());
#endif
				}
			else
				{
				// Buffer supplier with all buffers at home.. we can
				// initiate here the depopulation of the tunnel...

				// This boolean is not used here ...
				TBool portDepopulationCompleted = EFalse;

				// Better to ignore here any possible error in
				// FreeTunnel... nothing we can do about it...
				if (OMX_ErrorNone !=
					(omxRetValue =
					 pPort->FreeTunnel(portDepopulationCompleted)))
					{
					return omxRetValue;
					}

				// Inform the port that the disabled command has
				// completed...
				pPort->SetTransitionToDisabledCompleted();

				// For each disabled port, the IL Client must be notified
				// with a disabled completion event...
				if (OMX_ErrorNone !=
					(omxRetValue =
					 iCallbacks.CommandCompleteNotification(
						 OMX_CommandPortDisable, portIndex)))
					{
					return omxRetValue;
					}

				} // else <- if (!pPort->HasAllBuffersAtHome())

			} // if (pPort->IsTunnelledAndBufferSupplier())
		else
			{
			if (pPort->Count() > 0)
				{

#ifdef _OMXIL_COMMON_IL516C_ON
			if (!pPort->IsTunnelled())
				{
#endif

				if (OMX_ErrorNone !=
					(omxRetValue =
					 iProcessingFunction.BufferFlushingIndication(
					portIndex,
					pPort->Direction())))
					{
					return omxRetValue;
					}

#ifdef _OMXIL_COMMON_IL516C_ON
				}
#endif

				// Inform the port that it is being disabled
				pPort->SetTransitionToDisabled();
				}
			else
				{
				// Inform the port that the disabled command has
				// completed...
				pPort->SetTransitionToDisabledCompleted();

				// For each disabled port, the IL Client must be notified
				// with a disabled completion event...
				if (OMX_ErrorNone !=
					(omxRetValue =
					 iCallbacks.CommandCompleteNotification(
						 OMX_CommandPortDisable, portIndex)))
					{
					return omxRetValue;
					}
				}

			}

		// Increment loop counter
		++i;
		}
	while(OMX_ALL == aPortIndex && i < portCount);

	return OMX_ErrorNone;

	}

OMX_ERRORTYPE
COmxILPortManager::BufferMarkIndication(
	TUint32 aPortIndex,
	OMX_PTR ipMarkData)
	{
    DEBUG_PRINTF2(_L8("COmxILPortManager::BufferMarkIndication: PORT[%u] "), aPortIndex);

	// Check the index of the port..
	if (CheckPortIndex(aPortIndex) != OMX_ErrorNone)
		{
		return OMX_ErrorBadPortIndex;
		}

	const OMX_MARKTYPE* pMark = static_cast<OMX_MARKTYPE*>(ipMarkData);

	if (!pMark->hMarkTargetComponent)
		{
		return OMX_ErrorBadParameter;
		}

	// Let's tell the port to store the mark so it can mark the next incoming
	// buffer...
	return iAllPorts[aPortIndex]->StoreBufferMark(pMark);

	}

#ifdef _OMXIL_COMMON_IL516C_ON
OMX_ERRORTYPE
COmxILPortManager::BufferEjectIndication(
	TUint32 aPortIndex)
	{
    DEBUG_PRINTF2(_L8("COmxILPortManager::BufferEjectIndication: PORT[%u] "), aPortIndex);

	// Check the index of the port..
	if (CheckPortIndex(aPortIndex) != OMX_ErrorNone)
		{
		return OMX_ErrorBadPortIndex;
		}

	OMX_ERRORTYPE omxRetValue = OMX_ErrorNone;
	COmxILPort* pPort = iAllPorts[aPortIndex];

	if (pPort->Count())
		{
		omxRetValue = iProcessingFunction.BufferFlushingIndication(
			pPort->Index(),
			pPort->Direction());
		}

	return omxRetValue;

	}
#endif

OMX_ERRORTYPE
COmxILPortManager::ComponentRoleIndication(TUint aComponentRoleIndex)
	{
    DEBUG_PRINTF2(_L8("COmxILPortManager::ComponentRoleIndication : aComponentRoleIndex[%u]"), aComponentRoleIndex);

	// At this point, the IL Client wants to set the default role that the
	// standard component is assuming. Therefore, the role defaults need to be
	// reloaded into all ports and the processing function object.
	OMX_ERRORTYPE omxRetValue = OMX_ErrorNone;
	const TInt portCount = iAllPorts.Count();
	for (TUint i = 0; i< portCount; ++i)
		{
		if (OMX_ErrorNone !=
			(omxRetValue =
			 iAllPorts[i]->SetComponentRoleDefaults(aComponentRoleIndex)))
			{
			return omxRetValue;
			}
		}

	return iProcessingFunction.ComponentRoleIndication(aComponentRoleIndex);

	}

OMX_ERRORTYPE
COmxILPortManager::PortSettingsChangeIndication(OMX_U32 aPortIndex,
												TUint aPortSettingsIndex,
												const TDesC8& aPortSettings,
												OMX_EVENTTYPE& aEventForILClient)
	{
    DEBUG_PRINTF2(_L8("COmxILPortManager::PortSettingsChangeIndication: PORT[%u] "), aPortIndex);

	// Check the index of the port..
	if (CheckPortIndex(aPortIndex) != OMX_ErrorNone)
		{
		return OMX_ErrorBadPortIndex;
		}

	return iAllPorts[aPortIndex]->DoPortReconfiguration(
		aPortSettingsIndex, aPortSettings, aEventForILClient);

	}


TBool
COmxILPortManager::AllPortsPopulated() const
	{

	const TInt portCount = iAllPorts.Count();
	COmxILPort* pPort = 0;
	for (TInt i=0; i<portCount; ++i)
		{
		pPort = iAllPorts[i];
		if (!pPort->IsEnabled())
			{
			continue;
			}

		if (!pPort->IsPopulated())
			{
			DEBUG_PRINTF(_L8("COmxILPortManager::AllPortsPopulated : [FALSE]"));
			return EFalse;
			}
		}

	DEBUG_PRINTF(_L8("COmxILPortManager::AllPortsPopulated : [TRUE]"));
	return ETrue;

	}

TBool
COmxILPortManager::AllPortsDePopulated() const
	{

	const TInt portCount = iAllPorts.Count();
	for (TInt i=0; i<portCount; ++i)
		{
		if (!iAllPorts[i]->IsDePopulated())
			{
			DEBUG_PRINTF(_L8("COmxILPortManager::AllPortsDePopulated : [FALSE]"));
			return EFalse;
			}
		}

	DEBUG_PRINTF(_L8("COmxILPortManager::AllPortsDePopulated : [TRUE]"));
	return ETrue;

	}

TBool
COmxILPortManager::AllBuffersAtHome() const
	{

	const TInt portCount = iAllPorts.Count();
	for (TInt i=0; i<portCount; ++i)
		{
		if (iAllPorts[i]->IsTunnelledAndBufferSupplier())
			{
			if (!iAllPorts[i]->HasAllBuffersAtHome())
				{
				DEBUG_PRINTF(_L8("COmxILPortManager::AllBuffersAtHome : [FALSE]"));
				return EFalse;
				}
			}
		}

	DEBUG_PRINTF(_L8("COmxILPortManager::AllBuffersAtHome : [TRUE]"));
	return ETrue;

	}
