/*
* Copyright (c) 2007-2010 Nokia Corporation and/or its subsidiary(-ies).
* All rights reserved.
* This component and the accompanying materials are made available
* under the terms of "Eclipse Public License v1.0"
* which accompanies this distribution, and is available
* at the URL "http://www.eclipse.org/legal/epl-v10.html".
*
* Initial Contributors:
* Nokia Corporation - initial contribution.
*
* Contributors:
*
* Description:
*
*/

/**
 @file
 @internalComponent
*/
#include <ecom/ecom.h>
#include <usbhost/internal/fdcplugin.h>
#include <usbhost/internal/fdcinterface.h>

#include "fdcproxy.h"
#include "utils.h"
#include "fdf.h"
#include "utils.h"
#include "OstTraceDefinitions.h"
#ifdef OST_TRACE_COMPILER_IN_USE
#include "fdcproxyTraces.h"
#endif


#ifdef _DEBUG
#define LOG	Log()
#define INVARIANT Invariant()
#else 
#define LOG
#define INVARIANT
#endif

_LIT(KPanicCategory, "fdcproxy");

CFdcProxy* CFdcProxy::NewL(CFdf& aFdf, CImplementationInformation& aImplInfo)
	{
    OstTraceFunctionEntry0( CFDCPROXY_NEWL_ENTRY );

	CFdcProxy* self = new(ELeave) CFdcProxy(aFdf);
	CleanupStack::PushL(self);
	self->ConstructL(aImplInfo);
#ifdef _DEBUG
	self->INVARIANT;
#endif
	CleanupStack::Pop(self);
	OstTraceFunctionExit0( CFDCPROXY_NEWL_EXIT );
	return self;
	}


void CFdcProxy::ConstructL(CImplementationInformation& aImplInfo)
	{
    OstTraceFunctionEntry0( CFDCPROXY_CONSTRUCTL_ENTRY );
    
    OstTrace1( TRACE_DUMP, CFDCPROXY_CONSTRUCTL, "FDC implementation UID: 0x%08x", aImplInfo.ImplementationUid().iUid );
    OstTraceExt1( TRACE_DUMP, CFDCPROXY_CONSTRUCTL_DUP1, "FDC display name: \"%S\"", aImplInfo.DisplayName() );
    OstTraceExt1( TRACE_DUMP, CFDCPROXY_CONSTRUCTL_DUP2, "FDC default_data: \"%s\"", aImplInfo.DataType() );
    OstTrace1( TRACE_DUMP, CFDCPROXY_CONSTRUCTL_DUP3, "FDC version: %d", aImplInfo.Version() );
    OstTrace1( TRACE_DUMP, CFDCPROXY_CONSTRUCTL_DUP4, "FDC disabled: %d", aImplInfo.Disabled());
    TDriveName drvName = aImplInfo.Drive().Name();
        
    OstTraceExt1( TRACE_DUMP, CFDCPROXY_CONSTRUCTL_DUP5, "FDC drive: %S", drvName );
    OstTrace1( TRACE_DUMP, CFDCPROXY_CONSTRUCTL_DUP6, "FDC rom only: %d", aImplInfo.RomOnly() );
    OstTrace1( TRACE_DUMP, CFDCPROXY_CONSTRUCTL_DUP7, "FDC rom based: %d", aImplInfo.RomBased() );
    OstTrace1( TRACE_DUMP, CFDCPROXY_CONSTRUCTL_DUP8, "FDC vendor ID: %08x", (TUint32)aImplInfo.VendorId() );
                        
    	
	// Before PREQ2080 a reference to the CImplementationInformation object was held. This is no longer
	// possible because as soon as REComSession::ListImplementations() is called the reference will be
	// invalid.		
	iImplementationUid = aImplInfo.ImplementationUid();
	iVersion = aImplInfo.Version();
	iDefaultData.CreateL(aImplInfo.DataType());
	iRomBased = aImplInfo.RomBased();
	OstTraceFunctionExit0( CFDCPROXY_CONSTRUCTL_EXIT );
	}

CFdcProxy::CFdcProxy(CFdf& aFdf)
:	iFdf(aFdf),
	i0thInterface(-1) // -1 means unassigned
	{
    OstTraceFunctionEntry0( CFDCPROXY_CFDCPROXY_CONS_ENTRY );
	}


CFdcProxy::~CFdcProxy()
	{
    OstTraceFunctionEntry0( CFDCPROXY_CFDCPROXY_DES_ENTRY );
    INVARIANT;

	// Only executed when the FDF is finally shutting down.
	// By this time detachment of all devices should have been signalled to
	// all FDCs and the FDC plugins should have been cleaned up.
	// If is safe to assert this because iPlugin and iDeviceIds are not
	// allocated on construction so this doesn't have to safe against partial
	// construction.
	
    if(iPlugin)
        {
        OstTrace0( TRACE_FATAL, CFDCPROXY_CFDCPROXY_DUP2, "Plugin should empty" );
        __ASSERT_DEBUG(EFalse,User::Panic(KPanicCategory,__LINE__));
        }
    if(!(iDeviceIds.Count() == 0))
        {
        OstTrace0( TRACE_FATAL, CFDCPROXY_CFDCPROXY_DUP3, "DeviceCount should be 0" );
        __ASSERT_DEBUG(EFalse,User::Panic(KPanicCategory,__LINE__));
        }
    iDeviceIds.Close();
	iDefaultData.Close();

	INVARIANT;
	OstTraceFunctionExit0( CFDCPROXY_CFDCPROXY_DES_EXIT );
	}


TInt CFdcProxy::NewFunction(TUint aDeviceId,
		const RArray<TUint>& aInterfaces,
		const TUsbDeviceDescriptor& aDeviceDescriptor,
		const TUsbConfigurationDescriptor& aConfigurationDescriptor)
	{
	OstTraceFunctionEntry0( CFDCPROXY_NEWFUNCTION_ENTRY );

	OstTrace1( TRACE_NORMAL, CFDCPROXY_NEWFUNCTION, "aDeviceId = %d", aDeviceId );
	INVARIANT;

	// Create a plugin object if required, call Mfi1NewFunction on it, and
	// update our iDeviceIds.
	TRAPD(err, NewFunctionL(aDeviceId, aInterfaces, aDeviceDescriptor, aConfigurationDescriptor));
	INVARIANT;
	OstTrace1( TRACE_NORMAL, CFDCPROXY_NEWFUNCTION_DUP1, "err = %d", err );
	OstTraceFunctionExit0( CFDCPROXY_NEWFUNCTION_EXIT );
	return err;
	}


void CFdcProxy::NewFunctionL(TUint aDeviceId,
		const RArray<TUint>& aInterfaces,
		const TUsbDeviceDescriptor& aDeviceDescriptor,
		const TUsbConfigurationDescriptor& aConfigurationDescriptor)
	{
	OstTraceFunctionEntry0( CFDCPROXY_NEWFUNCTIONL_ENTRY );

	// We may already have aDeviceId in our collection of device IDs, if the
	// device is offering multiple Functions of the same type. In this case we
	// don't want to add the device ID again.
	// If we already know about this device, then we should definitely have
	// already made iPlugin- this is checked in the invariant.
	// However, if we don't know this device, we may still already have made
	// iPlugin, to handle some other device. So we have to do some logic
	// around creating the objects we need.

	TBool alreadyKnowThisDevice = EFalse;
	const TUint count = iDeviceIds.Count();
	for ( TUint ii = 0 ; ii < count ; ++ii )
		{
		if ( iDeviceIds[ii] == aDeviceId )
			{
			alreadyKnowThisDevice = ETrue;
			break;
			}
		}
	
	OstTrace1( TRACE_NORMAL, CFDCPROXY_NEWFUNCTIONL, "alreadyKnowThisDevice = %d", alreadyKnowThisDevice );
	
	TArrayRemove arrayRemove(iDeviceIds, aDeviceId);
	if ( !alreadyKnowThisDevice )
		{
		// We add the device ID to our array first because it's failable.
		// Logically, it should be done *after* we call Mfi1NewFunction on the
		// plugin, but we can't have the failable step of adding the device ID
		// to the array after telling the FDC.
        TInt error=iDeviceIds.Append(aDeviceId);
    	LEAVEIFERRORL(error,OstTrace0( TRACE_NORMAL, CFDCPROXY_NEWFUNCTIONL_DUT1, "iDeviceIds append fails" ););
         
		// This cleanup item removes aDeviceId from iDeviceIds on a leave.
		CleanupRemovePushL(arrayRemove);
		}

	TBool neededToMakePlugin = EFalse;
	CFdcPlugin* plugin = iPlugin;
	MFdcInterfaceV1* iface = iInterface;
	if ( !plugin )
		{
		neededToMakePlugin = ETrue;
		OstTrace1( TRACE_NORMAL, CFDCPROXY_NEWFUNCTIONL_DUP2, "FDC implementation UID: 0x%08x", iImplementationUid.iUid );
		plugin = CFdcPlugin::NewL(iImplementationUid, *this);
		CleanupStack::PushL(plugin);
		iface = reinterpret_cast<MFdcInterfaceV1*>(plugin->GetInterface(TUid::Uid(KFdcInterfaceV1)));
		}
	
	if(!iface)
	    {
        OstTrace0( TRACE_FATAL, CFDCPROXY_NEWFUNCTIONL_DUP3, "Empty interface" );
        __ASSERT_DEBUG(EFalse,User::Panic(KPanicCategory,__LINE__));
	    }

	
	TInt err = KErrNone;

	// Log the interfaces they're being offered.
	const TUint ifCount = aInterfaces.Count();
	OstTrace1( TRACE_NORMAL, CFDCPROXY_NEWFUNCTIONL_DUP4, "offering %d interfaces:", ifCount );
	for ( TUint ii = 0 ; ii < ifCount ; ++ii )
		{
        OstTrace1( TRACE_NORMAL, CFDCPROXY_NEWFUNCTIONL_DUP5, "interface %d", aInterfaces[ii] );
		}

	iInMfi1NewFunction = ETrue;
	// Check that the FDC always claims the 0th interface.
	if(!(i0thInterface == -1))
	    {
        OstTrace0( TRACE_FATAL, CFDCPROXY_NEWFUNCTIONL_DUP6, "the FDC not claims the 0th interface" );
        __ASSERT_DEBUG(EFalse,User::Panic(KPanicCategory,__LINE__));
	    }

	i0thInterface = aInterfaces[0];
	err = iface->Mfi1NewFunction(   aDeviceId,
									aInterfaces.Array(), // actually pass them a TArray for const access
									aDeviceDescriptor,
									aConfigurationDescriptor);
	OstTrace1( TRACE_NORMAL, CFDCPROXY_NEWFUNCTIONL_DUP11, "err = %d", err);
	iInMfi1NewFunction = EFalse;
	// The implementation of Mfi1NewFunction may not leave.
//	ASSERT_ALWAYS(leave_err == KErrNone);
	// This is set back to -1 when the FDC claims the 0th interface.
	if(!(i0thInterface == -1))
	    {
        OstTrace0( TRACE_FATAL, CFDCPROXY_NEWFUNCTIONL_DUP7, "the FDC not claims the 0th interface" );
        __ASSERT_DEBUG(EFalse,User::Panic(KPanicCategory,__LINE__));
	    }
	
	// If this leaves, then:
	// (a) aDeviceId will be removed from iDeviceIds (if we needed to add it).
	// (b) the FDF will get the leave code.
	// If this doesn't leave, then iPlugin, iInterface and iDeviceIds are
	// populated OK and the FDF will get KErrNone.
	LEAVEIFERRORL(err,OstTrace0( TRACE_NORMAL, CFDCPROXY_NEWFUNCTIONL_DUP8,"iface->Mfi1NewFunction function fails"););

	if ( neededToMakePlugin )
		{
		CLEANUPSTACK_POP1(plugin);
		// Now everything failable has been done we can assign iPlugin and
		// iInterface.
		if(!plugin)
		    {
            OstTrace0( TRACE_FATAL, CFDCPROXY_NEWFUNCTIONL_DUP9,"Empty plugin");
            __ASSERT_DEBUG(EFalse,User::Panic(KPanicCategory,__LINE__));
		    }
		if(!iface)
		    {
            OstTrace0( TRACE_FATAL, CFDCPROXY_NEWFUNCTIONL_DUP10,"Empty iface");
            __ASSERT_DEBUG(EFalse,User::Panic(KPanicCategory,__LINE__));
		    }
		iPlugin = plugin;
		iInterface = iface;
		}
	if ( !alreadyKnowThisDevice )
		{
		CLEANUPSTACK_POP1(&arrayRemove);
		}
	OstTraceFunctionExit0( CFDCPROXY_NEWFUNCTIONL_EXIT );
	}


// Called by the FDF whenever a device is detached.
// We check if the device is relevant to us. If it is, we signal its
// detachment to the plugin.
void CFdcProxy::DeviceDetached(TUint aDeviceId)
	{
    OstTraceFunctionEntry0( CFDCPROXY_DEVICEDETACHED_ENTRY );
    OstTrace1( TRACE_NORMAL, CFDCPROXY_DEVICEDETACHED, "aDeviceId = %d", aDeviceId );

	INVARIANT;
	
	const TUint count = iDeviceIds.Count();
	for ( TUint ii = 0 ; ii < count ; ++ii )
		{
		if ( iDeviceIds[ii] == aDeviceId )
			{
            OstTrace0( TRACE_FATAL, CFDCPROXY_DEVICEDETACHED_DUP1, "matching device id- calling Mfi1DeviceDetached!");
            if(!iInterface)
                {
                OstTrace0( TRACE_NORMAL, CFDCPROXY_DEVICEDETACHED_DUP2, "Empty iInterface" );
                __ASSERT_DEBUG(EFalse,User::Panic(KPanicCategory,__LINE__));
                }
 
			iInterface->Mfi1DeviceDetached(aDeviceId);
			// The implementation of Mfi1DeviceDetached may not leave.
//			ASSERT_ALWAYS(err == KErrNone);
			iDeviceIds.Remove(ii);
			break;
			}
		}
	OstTrace1( TRACE_NORMAL, CFDCPROXY_DEVICEDETACHED_DUP3, "iDeviceIds.Count() = %d", iDeviceIds.Count() );
	
	if ( iDeviceIds.Count() == 0 )
		{
		delete iPlugin;
		iPlugin = NULL;
		iInterface = NULL;
		
		// If an FDC was loaded and then unloaded and an upgrade of the FDC installed then when the FDC is
		// loaded again ECom will load the original version and not the new version unless we do a FinalClose()
		// to release cached handles
#pragma message("ECom defect DEF122443 raised")		
		REComSession::FinalClose(); 
		}

	INVARIANT;
	OstTraceFunctionExit0( CFDCPROXY_DEVICEDETACHED_EXIT );
	}


void CFdcProxy::Invariant() const
	{
	// If the class invariant fails hopefully it will be clear why from
	// inspection of this object dump.
	LOG;

	// Either these are all 0 or none of them are:
	// iDeviceIds.Count, iPlugin, iInterface
	
	if(!(
					(
						iDeviceIds.Count() != 0 && iPlugin && iInterface
					)
				||
					(
						iDeviceIds.Count() == 0 && !iPlugin && !iInterface
					)
		))
	    {
        OstTrace0( TRACE_FATAL, CFDCPROXY_INVARIANT, "iDeviceIds count error" );
        __ASSERT_DEBUG(EFalse,User::Panic(KPanicCategory,__LINE__));
	    }


	// Each device ID appears only once in the device ID array.
	const TUint count = iDeviceIds.Count();
	for ( TUint ii = 0 ; ii < count ; ++ii )
		{
		for ( TUint jj = ii+1 ; jj < count ; ++jj )
			{
			if(!(iDeviceIds[ii] != iDeviceIds[jj]))
			    {
                OstTrace0( TRACE_FATAL, CFDCPROXY_INVARIANT_DUP1, "Repeated iDeviceIDs" );
                __ASSERT_DEBUG(EFalse,User::Panic(KPanicCategory,__LINE__));
			    }

			}
		}
	}


void CFdcProxy::Log() const
	{
    OstTrace1( TRACE_DUMP, CFDCPROXY_LOG, "Logging CFdcProxy 0x%08x:", this );
	const TUint count = iDeviceIds.Count();
	OstTrace1( TRACE_DUMP, CFDCPROXY_LOG_DUP1, "iDeviceIds.Count() = %d", count );

	for ( TUint i = 0 ; i < count ; ++i )
		{
        OstTraceExt2( TRACE_DUMP, CFDCPROXY_LOG_DUP2, "iDeviceIds[%u] = %u", i, iDeviceIds[i] );
		}
	OstTrace1( TRACE_DUMP, CFDCPROXY_LOG_DUP3, "iPlugin = 0x%08x", iPlugin );
	OstTrace1( TRACE_DUMP, CFDCPROXY_LOG_DUP4, "iInterface = 0x%08x", iInterface );
	}


const TDesC8& CFdcProxy::DefaultDataField() const
	{
	return iDefaultData;
	}

TUid CFdcProxy::ImplUid() const
	{
	return iImplementationUid;
	}
	
TInt CFdcProxy::Version() const
	{
	return iVersion;
	}	
	
	
TInt CFdcProxy::DeviceCount() const
	{
	return iDeviceIds.Count();
	}
	
	
// If a FD has been uninstalled from the device then its proxy needs to be deleted from the proxy list
// maintained by the FDF. However if the FD is in use by an attached peripheral it can't be deleted 
// until that device is removed. Hence this function marks it for deletion upon device detachment.
// This situation will also occur if a FD upgrade is installed onto the device and the original FD (the one that
// is being upgraded) is in use.	
void CFdcProxy::MarkForDeletion()
	{
	iMarkedForDeletion = ETrue;
	}	
	
	
	
// If a FD is installed and a device attached which uses it, then if while the device is still attached that FD is 
// uninstalled then the FD proxy is marked for deletion when the device detaches. However in the situation where the
// FD is re-installed while the device still remains attached then the FD proxy should not be deleted when the device
// eventually detaches. Hence this function is to undo the mark for deletion that was placed upon the proxy when the FD
// was uninstalled.	
void CFdcProxy::UnmarkForDeletion()
	{
	iMarkedForDeletion = EFalse;
	}	
	
	
TBool CFdcProxy::MarkedForDeletion() const
	{
	return iMarkedForDeletion;
	}
	
	
TBool CFdcProxy::RomBased() const
	{
	return iRomBased;
	}	
	
	
TUint32 CFdcProxy::MfpoTokenForInterface(TUint8 aInterface)
	{
    OstTraceFunctionEntry0( CFDCPROXY_MFPOTOKENFORINTERFACE_ENTRY );

	// This function must only be called from an implementation of
	// Mfi1NewInterface.
	if(!iInMfi1NewFunction)
	    {
        OstTrace0( TRACE_FATAL, CFDCPROXY_MFPOTOKENFORINTERFACE, "Empty iInMfi1NewFunction" );
        User::Panic(KPanicCategory,__LINE__);
	    }

	// Support our check that the FDC claims the 0th interface.
	if ( aInterface == i0thInterface )
		{
		i0thInterface = -1;
		}

	return iFdf.TokenForInterface(aInterface);
	}


const RArray<TUint>& CFdcProxy::MfpoGetSupportedLanguagesL(TUint aDeviceId)
	{
	OstTraceFunctionEntry0( CFDCPROXY_MFPOGETSUPPORTEDLANGUAGESL_ENTRY );
	
	CheckDeviceIdL(aDeviceId);

	return iFdf.GetSupportedLanguagesL(aDeviceId);
	}


TInt CFdcProxy::MfpoGetManufacturerStringDescriptor(TUint aDeviceId, TUint aLangId, TName& aString)
	{
	OstTraceFunctionEntry0( CFDCPROXY_MFPOGETMANUFACTURERSTRINGDESCRIPTOR_ENTRY );

	TRAPD(err,
		CheckDeviceIdL(aDeviceId);
		iFdf.GetManufacturerStringDescriptorL(aDeviceId, aLangId, aString)
		);

	if ( !err )
		{
        OstTraceExt1( TRACE_NORMAL, CFDCPROXY_MFPOGETMANUFACTURERSTRINGDESCRIPTOR, "aString = \"%S\"", aString );
 		}

	OstTrace1( TRACE_NORMAL, CFDCPROXY_MFPOGETMANUFACTURERSTRINGDESCRIPTOR_DUP1, "err = %d", err );
	
	OstTraceFunctionExit0( CFDCPROXY_MFPOGETMANUFACTURERSTRINGDESCRIPTOR_EXIT );
	return err;
	}


TInt CFdcProxy::MfpoGetProductStringDescriptor(TUint aDeviceId, TUint aLangId, TName& aString)
	{
	OstTraceFunctionEntry0( CFDCPROXY_MFPOGETPRODUCTSTRINGDESCRIPTOR_ENTRY );
	
	TRAPD(err,
		CheckDeviceIdL(aDeviceId);
		iFdf.GetProductStringDescriptorL(aDeviceId, aLangId, aString)
		);

	if ( !err )
		{
		OstTraceExt1( TRACE_NORMAL, CFDCPROXY_MFPOGETPRODUCTSTRINGDESCRIPTOR, "aString = \"%S\"", aString );
		}

	OstTrace1( TRACE_NORMAL, CFDCPROXY_MFPOGETPRODUCTSTRINGDESCRIPTOR_DUP1, "err = %d", err );
	OstTraceFunctionExit0( CFDCPROXY_MFPOGETPRODUCTSTRINGDESCRIPTOR_EXIT );
	return err;
	}


TInt CFdcProxy::MfpoGetSerialNumberStringDescriptor(TUint aDeviceId, TUint aLangId, TName& aString)
	{
    OstTraceFunctionEntry0( CFDCPROXY_MFPOGETSERIALNUMBERSTRINGDESCRIPTOR_ENTRY );

	TRAPD(err,
		CheckDeviceIdL(aDeviceId);
		iFdf.GetSerialNumberStringDescriptorL(aDeviceId, aLangId, aString)
		);

	if ( !err )
		{
        OstTraceExt1( TRACE_NORMAL, CFDCPROXY_MFPOGETSERIALNUMBERSTRINGDESCRIPTOR, "aString = \"%S\"", aString );
 		}

	OstTrace1( TRACE_NORMAL, CFDCPROXY_MFPOGETSERIALNUMBERSTRINGDESCRIPTOR_DUP1, "err = %d", err );
	OstTraceFunctionExit0( CFDCPROXY_MFPOGETSERIALNUMBERSTRINGDESCRIPTOR_EXIT );
	return err;
	}

/**
Leaves with KErrNotFound if aDeviceId is not on our array of device IDs.
Used to ensure that FDCs can only request strings etc from devices that are
'their business'.
*/
void CFdcProxy::CheckDeviceIdL(TUint aDeviceId) const
	{
	OstTraceFunctionEntry0( CFDCPROXY_CHECKDEVICEIDL_ENTRY );
	OstTrace1( TRACE_NORMAL, CFDCPROXY_CHECKDEVICEIDL, "aDeviceId = %d", aDeviceId );
	
	TBool found = EFalse;
	const TUint count = iDeviceIds.Count();
	for ( TUint i = 0 ; i < count ; ++i )
		{
		if ( iDeviceIds[i] == aDeviceId )
			{
			found = ETrue;
			break;
			}
		}
	if ( !found )
		{
        OstTrace1( TRACE_NORMAL, CFDCPROXY_CHECKDEVICEIDL_DUP1, "DeviceId=%d not found", aDeviceId);
        User::Leave(KErrNotFound);
		}
	OstTraceFunctionExit0( CFDCPROXY_CHECKDEVICEIDL_EXIT );
	}


