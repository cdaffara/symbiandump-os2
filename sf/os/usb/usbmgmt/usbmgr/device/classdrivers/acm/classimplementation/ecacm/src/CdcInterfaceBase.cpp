/*
* Copyright (c) 1997-2010 Nokia Corporation and/or its subsidiary(-ies).
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

#include <e32std.h>
#include "CdcInterfaceBase.h"
#include "OstTraceDefinitions.h"
#ifdef OST_TRACE_COMPILER_IN_USE
#include "CdcInterfaceBaseTraces.h"
#endif


CCdcInterfaceBase::CCdcInterfaceBase(const TDesC16& aIfcName)
/**
 * Constructor.
 *
 * @param aIfcName The name of the interface.
 */
	{
	OstTraceFunctionEntry0( CCDCINTERFACEBASE_CCDCINTERFACEBASE_CONS_ENTRY );
	iIfcName.Set(aIfcName);
	OstTraceFunctionExit0( CCDCINTERFACEBASE_CCDCINTERFACEBASE_CONS_EXIT );
	}

void CCdcInterfaceBase::BaseConstructL()
/**
 * Construct the object
 * This call registers the object with the USB device driver
 */
	{
	OstTraceFunctionEntry0( CCDCINTERFACEBASE_BASECONSTRUCTL_ENTRY );
	OstTrace0( TRACE_NORMAL, CCDCINTERFACEBASE_BASECONSTRUCTL, "CCdcInterfaceBase::BaseConstructL;\tcalling RDevUsbcClient::Open" );
	// 0 is assumed to mean ep0
	TInt ret = iLdd.Open(0); 
	if ( ret )
		{		
		OstTrace1( TRACE_NORMAL, CCDCINTERFACEBASE_BASECONSTRUCTL_DUP1, 
					"CCdcInterfaceBase::BaseConstructL;\tRDevUsbcClient::Open = %d", ret );
		if (ret < 0)
			{
			User::Leave(ret);
			}
		}

	ret = SetUpInterface();
	if ( ret )
		{
		OstTrace1( TRACE_NORMAL, CCDCINTERFACEBASE_BASECONSTRUCTL_DUP2, 
					"CCdcInterfaceBase::BaseConstructL;\tSetUpInterface = %d", ret );
		if (ret < 0)
			{
			User::Leave(ret);
			}
		}
	OstTraceFunctionExit0( CCDCINTERFACEBASE_BASECONSTRUCTL_EXIT );
	}

CCdcInterfaceBase::~CCdcInterfaceBase()
/**
 * Destructor.
 */
	{
	OstTraceFunctionEntry0( CCDCINTERFACEBASE_CCDCINTERFACEBASE_DES_ENTRY );
	if ( iLdd.Handle() )
		{
		OstTrace0( TRACE_NORMAL, CCDCINTERFACEBASE_CCDCINTERFACEBASE_DES, 
				"CCdcInterfaceBase::~CCdcInterfaceBase;\tLDD handle exists" );
		// Don't bother calling ReleaseInterface- the base driver spec says 
		// that Close does it for us.
		OstTrace0( TRACE_NORMAL, CCDCINTERFACEBASE_CCDCINTERFACEBASE_DES_DUP1, 
				"CCdcInterfaceBase::~CCdcInterfaceBase;\tclosing LDD session" );
		iLdd.Close();
		}
	OstTraceFunctionExit0( CCDCINTERFACEBASE_CCDCINTERFACEBASE_DES_EXIT );
	}

TInt CCdcInterfaceBase::GetInterfaceNumber(TUint8& aIntNumber)
/**
 * Get my interface number
 *
 * @param aIntNumber My interface number
 * @return Error.
 */
	{
	OstTraceFunctionEntry0( CCDCINTERFACEBASE_GETINTERFACENUMBER_ENTRY );
	TInt interfaceSize = 0;
	// 0 means the main interface in the LDD API
	TInt res = iLdd.GetInterfaceDescriptorSize(0, interfaceSize);

	if ( res )
		{
		OstTrace1( TRACE_NORMAL, CCDCINTERFACEBASE_GETINTERFACENUMBER, 
				"CCdcInterfaceBase::GetInterfaceNumber;\t***GetInterfaceDescriptorSize()=%d", res );
		OstTraceFunctionExit0( CCDCINTERFACEBASE_GETINTERFACENUMBER_EXIT );
		return res;
		}

	HBufC8* interfaceBuf = HBufC8::New(interfaceSize);
	if ( !interfaceBuf )
		{
		OstTrace0( TRACE_NORMAL, CCDCINTERFACEBASE_GETINTERFACENUMBER_DUP1, 
				"CCdcInterfaceBase::GetInterfaceNumber;\t***failed to create interfaceBuf-returning KErrNoMemory" );
		OstTraceFunctionExit0( CCDCINTERFACEBASE_GETINTERFACENUMBER_EXIT_DUP1 );
		return KErrNoMemory;
		}

	TPtr8 interfacePtr = interfaceBuf->Des();
	interfacePtr.SetLength(0);
	// 0 means the main interface in the LDD API
	res = iLdd.GetInterfaceDescriptor(0, interfacePtr); 

	if ( res )
		{
		delete interfaceBuf;
		OstTrace1( TRACE_NORMAL, CCDCINTERFACEBASE_GETINTERFACENUMBER_DUP2, "CCdcInterfaceBase::GetInterfaceNumber;res=%d", res );
		OstTraceFunctionExit0( CCDCINTERFACEBASE_GETINTERFACENUMBER_EXIT_DUP2 );
		return res;
		}

	OstTrace1( TRACE_NORMAL, CCDCINTERFACEBASE_GETINTERFACENUMBER_DUP3, 
			"CCdcInterfaceBase::GetInterfaceNumber;\t***interface length = %d", interfacePtr.Length() );
	for ( TInt i = 0 ; i < interfacePtr.Length() ; i++ )
		{
		OstTraceExt1( TRACE_NORMAL, CCDCINTERFACEBASE_GETINTERFACENUMBER_DUP4, 
				"\t***** %hhx", interfacePtr[i] );
		}

	const TUint8* buffer = reinterpret_cast<const TUint8*>(interfacePtr.Ptr());
	// 2 is where the interface number is, according to the LDD API
	aIntNumber = buffer[2]; 
	OstTrace1( TRACE_NORMAL, CCDCINTERFACEBASE_GETINTERFACENUMBER_DUP5, 
			"CCdcInterfaceBase::GetInterfaceNumber;\tinterface number = %d", (TInt)aIntNumber );
	
	delete interfaceBuf;
	OstTraceFunctionExit0( CCDCINTERFACEBASE_GETINTERFACENUMBER_EXIT_DUP3 );
	return KErrNone;
	}

//
// End of file
