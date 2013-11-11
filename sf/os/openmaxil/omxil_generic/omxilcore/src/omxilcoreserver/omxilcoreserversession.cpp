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

#include "log.h"
#include "omxilcore.h"
#include "omxilcoreserver.h"
#include "omxilcoreserversession.h"


_LIT(KOmxILCoreServerPanic, "OmxILCoreServer Panic");

/**
 * By default Symbian 2nd phase constructor is private.
 */
COmxILCoreServerSession::COmxILCoreServerSession(COmxILCore& aCore, COmxILCoreServer& aCoreServer)
	:
	iCore(aCore),
	iCoreServer(aCoreServer)
	{
    DEBUG_PRINTF(_L8("COmxILCoreServerSession::COmxILCoreServerSession"));

	// Inform the server that this session is opening
	aCoreServer.SessionOpened();

	}

/**
 * Destructor
 */
COmxILCoreServerSession::~COmxILCoreServerSession()
	{
    DEBUG_PRINTF(_L8("COmxILCoreServerSession::~COmxILCoreServerSession"));

	// Inform the server that this session is closed
	iCoreServer.SessionClosed();

	}

/**
 * Constructs, and returns a pointer to, a new COmxILCoreServerSession object.
 * Leaves on failure.
 * @return COmxILCoreServerSession* A pointer to newly created utlitly object.
 */
COmxILCoreServerSession* COmxILCoreServerSession::NewL(COmxILCore& aCore, COmxILCoreServer& aCoreServer)
	{
    DEBUG_PRINTF(_L8("COmxILCoreServerSession::NewL"));

	COmxILCoreServerSession* self = new(ELeave) COmxILCoreServerSession(aCore, aCoreServer);
	return self;
	}

/**
 * from CSession2
 * @param aMessage - Function and data for the session
 */
void COmxILCoreServerSession::ServiceL(const RMessage2& aMessage)
	{
    DEBUG_PRINTF(_L8("COmxILCoreServerSession::ServiceL"));

	TRAPD( errL, DispatchMessageL(aMessage) );

	if( errL != KErrNone )
		{
		ASSERT(0);
		}
	}

/**
 * Dispatch the message received from ServeL
 * @param aMessage - Function and data for the session
 * @return error code
 * @leave in case of writeL leave or request leave
 */
TInt COmxILCoreServerSession::DispatchMessageL(const RMessage2& aMessage)
	{

	TInt error = KErrNone;
	OMX_ERRORTYPE omxError = OMX_ErrorNone;

	TOmxILCoreClientToServerMessages messageFunction =
		static_cast<TOmxILCoreClientToServerMessages>(aMessage.Function());
	__ASSERT_ALWAYS(messageFunction <= EOmxILCoreGetRolesOfComponent,
					User::Panic(KOmxILCoreServerPanic, 0));

	switch(messageFunction)
		{
	case EOmxILCoreListLoaders:
		{
		DEBUG_PRINTF(_L8("COmxILCoreServerSession::DispatchMessageL : EOmxILCoreListLoaders"));
		omxError = iCore.ListLoaders();
		}
		break;

	case EOmxILCoreDeinit:
		{
		DEBUG_PRINTF(_L8("COmxILCoreServerSession::DispatchMessageL : EOmxILCoreDeinit"));

		// Flag the deinitialization ...
		iCoreServer.SetDeinitInProgress();
		iCore.DeinitCore();

		// Write server thread id
		RThread thisThread;
		TPckgBuf<TUint64> p(thisThread.Id().Id());
		aMessage.WriteL(1,p);
		thisThread.Close();
		}
		break;

	case EOmxILCoreGetILCore:
		{
		DEBUG_PRINTF(_L8("COmxILCoreServerSession::DispatchMessageL : EOmxILCoreGetILCore"));

		// Write IL Core reference...
		TPckgBuf<COmxILCore*> p(&iCore);
		aMessage.WriteL(1,p);

		}
		break;

	case EOmxILCoreComponentNameEnum:
		{
		DEBUG_PRINTF(_L8("COmxILCoreServerSession::DispatchMessageL : EOmxILCoreComponentNameEnum"));

		TComponentNameEnum* pArg =
			static_cast<TComponentNameEnum*>(
				const_cast<TAny*>(aMessage.Ptr1()));
		__ASSERT_DEBUG(pArg, User::Panic(KOmxILCoreServerPanic, 0));

		omxError = iCore.ComponentNameEnum(
			pArg->cComponentName,
			pArg->nNameLength,
			pArg->nIndex);
		}
		break;

	case EOmxILCoreGetHandle:
		{
		DEBUG_PRINTF(_L8("COmxILCoreServerSession::DispatchMessageL : EOmxILCoreGetHandle"));

		TGetHandle* pArg =
			static_cast<TGetHandle*>(
				const_cast<TAny*>(aMessage.Ptr1()));
		__ASSERT_DEBUG(pArg, User::Panic(KOmxILCoreServerPanic, 0));

		omxError = iCore.LoadComponent(
			pArg->cComponentName,
			pArg->pHandle,
			pArg->pAppData,
			pArg->pCallBacks);

		}
		break;

	case EOmxILCoreFreeHandle:
		{
		DEBUG_PRINTF(_L8("COmxILCoreServerSession::DispatchMessageL : EOmxILCoreFreeHandle"));

		OMX_HANDLETYPE* hComponent =
			static_cast<OMX_HANDLETYPE*>(
				const_cast<TAny*>(aMessage.Ptr1()));

		omxError = iCore.FreeHandle(hComponent);

		}
		break;

	case EOmxILCoreSetupTunnel:
		{
		DEBUG_PRINTF(_L8("COmxILCoreServerSession::DispatchMessageL : EOmxILCoreSetupTunnel"));

		TSetupTunnel* pArg =
			static_cast<TSetupTunnel*>(
				const_cast<TAny*>(aMessage.Ptr1()));
		__ASSERT_DEBUG(pArg, User::Panic(KOmxILCoreServerPanic, 0));

		omxError = COmxILCore::SetupTunnel(
			pArg->hOutput,
			pArg->nPortOutput,
			pArg->hInput,
			pArg->nPortInput);

		}
		break;

	case EOmxILCoreGetContentPipe:
		{
		DEBUG_PRINTF(_L8("COmxILCoreServerSession::DispatchMessageL : EOmxILCoreGetContentPipe"));

		TGetContentPipe* pArg =
			static_cast<TGetContentPipe*>(
				const_cast<TAny*>(aMessage.Ptr1()));
		__ASSERT_DEBUG(pArg, User::Panic(KOmxILCoreServerPanic, 0));

		omxError = iCore.GetContentPipe(
			pArg->hPipe,
			pArg->szURI);

		}
		break;

	case EOmxILCoreGetComponentsOfRole:
		{
		DEBUG_PRINTF(_L8("COmxILCoreServerSession::DispatchMessageL : EOmxILCoreGetComponentsOfRole"));

		TGetComponentsOfRole* pArg =
			static_cast<TGetComponentsOfRole*>(
				const_cast<TAny*>(aMessage.Ptr1()));
		__ASSERT_DEBUG(pArg, User::Panic(KOmxILCoreServerPanic, 0));

		omxError = iCore.GetComponentsOfRole(
			pArg->role,
			pArg->pNumComps,
			pArg->compNames);

		}
		break;

	case EOmxILCoreGetRolesOfComponent:
		{
		DEBUG_PRINTF(_L8("COmxILCoreServerSession::DispatchMessageL : EOmxILCoreGetRolesOfComponent"));

		TGetRolesOfComponent* pArg =
			static_cast<TGetRolesOfComponent*>(
				const_cast<TAny*>(aMessage.Ptr1()));
		__ASSERT_DEBUG(pArg, User::Panic(KOmxILCoreServerPanic, 0));

		omxError = iCore.GetRolesOfComponent(
			pArg->compName,
			pArg->pNumRoles,
			pArg->roles);

		}
		break;

	default:
		{
		// Unknown Message
		error = KErrNotSupported;
		}
		break;
		};

	// Write OMX result
	TPckgBuf<OMX_ERRORTYPE> p(omxError);
    aMessage.WriteL(0,p);

	aMessage.Complete(error);

	return error;
	}
