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

#include <acminterface.h>
#include "AcmPortFactory.h"
#include "AcmUtils.h"
#include "AcmPort.h"
#include "AcmPanic.h"
#include "acmserver.h"
#include "CdcAcmClass.h"
#include "c32comm_internal.h"
#include "OstTraceDefinitions.h"
#ifdef OST_TRACE_COMPILER_IN_USE
#include "AcmPortFactoryTraces.h"
#endif


// Do not move this into a header file. It must be kept private to the CSY. It 
// is the secret information that enables the old (registration port) 
// mechanism allowing the USB Manager to set up ACM interfaces.
const TUint KRegistrationPortUnit = 666;

CAcmPortFactory* CAcmPortFactory::NewL()
/**
 * Make a new CAcmPortFactory
 *
 * @return Ownership of a newly created CAcmPortFactory object
 */
	{
	OstTraceFunctionEntry0( CACMPORTFACTORY_NEWL_ENTRY );
	CAcmPortFactory* self = new(ELeave) CAcmPortFactory;
	CleanupClosePushL(*self);
	self->ConstructL();
	CleanupStack::Pop();
	OstTraceFunctionExit0( CACMPORTFACTORY_NEWL_EXIT );
	return self;
	}
	
CAcmPortFactory::CAcmPortFactory()
/**
 * Constructor.
 */
	{
	OstTraceFunctionEntry0( CACMPORTFACTORY_CACMPORTFACTORY_ENTRY );
	iVersion = TVersion(
		KEC32MajorVersionNumber, 
		KEC32MinorVersionNumber, 
		KEC32BuildVersionNumber);
	iConfigBuf().iAcmConfigVersion = 1;
	iOwned = EFalse;
	OstTraceFunctionExit0( CACMPORTFACTORY_CACMPORTFACTORY_EXIT );
	}

void CAcmPortFactory::ConstructL()
/**
 * Second phase constructor.
 */
	{
	OstTraceFunctionEntry0( CACMPORTFACTORY_CONSTRUCTL_ENTRY );
	TInt	aReason	= SetName(&KAcmSerialName);
	if (aReason < 0)
		{
		OstTrace1( TRACE_NORMAL, CACMPORTFACTORY_CONSTRUCTL, "CAcmPortFactory::ConstructL;aReason=%d", aReason );
		User::Leave(aReason);
		}
	
	iAcmServer = CAcmServer::NewL(*this);
	
	TInt err = RProperty::Define(KUidSystemCategory, KAcmKey, RProperty::EByteArray, TPublishedAcmConfigs::KAcmMaxFunctions);
	if(err == KErrAlreadyExists)
		{	
		aReason = iAcmProperty.Attach(KUidSystemCategory, KAcmKey, EOwnerThread);
		if (aReason < 0)
			{
			OstTrace1( TRACE_NORMAL, CACMPORTFACTORY_CONSTRUCTL_DUP1, "CAcmPortFactory::ConstructL;aReason=%d", aReason );
			User::Leave(aReason);
			}
	
		//Since the P&S data already exists we need to retrieve it
		aReason = iAcmProperty.Get(iConfigBuf);
		if (aReason < 0)
			{
			OstTrace1( TRACE_NORMAL, CACMPORTFACTORY_CONSTRUCTL_DUP2, "CAcmPortFactory::ConstructL;aReason=%d", aReason );
			User::Leave(aReason);
			}
		}
	else if(err == KErrNone)
		{
		//A blank iConfigBuf already exists at this point to we don't need to do anything to it
		//before publishing the P&S data
		aReason = iAcmProperty.Attach(KUidSystemCategory, KAcmKey, EOwnerThread);
		if (aReason < 0)
			{
			OstTrace1( TRACE_NORMAL, CACMPORTFACTORY_CONSTRUCTL_DUP3, "CAcmPortFactory::ConstructL;aReason=%d", aReason );
			User::Leave(aReason);
			}
		PublishAcmConfig();
		iOwned = ETrue;
		}
	else
		{
		if (err < 0)
			{
			OstTrace1( TRACE_NORMAL, CACMPORTFACTORY_CONSTRUCTL_DUP4, "CAcmPortFactory::ConstructL;err=%d", err );
			User::Leave(err);
			}
		//Here will always leave, but a log will be created at least.	
		}
		
	OstTraceFunctionExit0( CACMPORTFACTORY_CONSTRUCTL_EXIT );
	}

/**
 * Utility function for publishing the TPublishedAcmConfigs data
 * @pre Requires iAcmProperty to be attached before it is called
 */
void CAcmPortFactory::PublishAcmConfig()
	{
	// Update the publish and subscribe info
	TInt err = iAcmProperty.Set(iConfigBuf);
	if (err != KErrNone)
		{	
		OstTraceExt3( TRACE_NORMAL, CACMPORTFACTORY_PUBLISHACMCONFIG, 
				"CAcmPortFactory::PublishAcmConfig;code=%d;name=%S;line = %d", 
				(TInt)EPanicInternalError, KAcmPanicCat, (TInt)__LINE__);
		__ASSERT_DEBUG( EFalse, User::Panic(KAcmPanicCat, EPanicInternalError) );
		}
	
	(void)err;
	}

TSecurityPolicy CAcmPortFactory::PortPlatSecCapability (TUint aPort) const
/**
 * Called by C32 when it needs to check the capabilities of a client against the 
 * capabilites required to perform C32 defered operations on this port, aPort. 
 *
 * @param aPort The number of the port.
 * @return a security policy
 */
	//return the security policy for the given port number, aPort.  
	{
	OstTraceFunctionEntry0( CACMPORTFACTORY_PORTPLATSECCAPABILITY_ENTRY );
	TSecurityPolicy securityPolicy; 
	if ( aPort == KRegistrationPortUnit ) 
		{
		securityPolicy = TSecurityPolicy(ECapabilityNetworkControl);
		}
	else
		{
		securityPolicy = TSecurityPolicy(ECapabilityLocalServices);	
		}
	OstTraceFunctionExit0( CACMPORTFACTORY_PORTPLATSECCAPABILITY_EXIT );
	return securityPolicy;	 	
	}

void CAcmPortFactory::AcmPortClosed(const TUint aUnit)
/**
 * Called by an ACM port when it is closed. 
 *
 * @param aUnit The port number of the closing port.
 */
	{
	OstTraceFunctionEntry0( CACMPORTFACTORY_ACMPORTCLOSED_ENTRY );
	OstTrace1( TRACE_NORMAL, CACMPORTFACTORY_ACMPORTCLOSED, "CAcmPortFactory::AcmPortClosed;aUnit=%d", aUnit );
	// I would assert that the calling port is stored in our array, but if we 
	// ran out of memory during CAcmPort::NewL, this function would be called 
	// from the port's destructor, but the slot in the port array would still 
	// be NULL. 

	// Reset the slot in our array of ports. 
	const TUint index = aUnit - KAcmLowUnit;
	iAcmPortArray[index] = NULL;

#ifdef _DEBUG
	LogPortsAndFunctions();
#endif
	OstTraceFunctionExit0( CACMPORTFACTORY_ACMPORTCLOSED_EXIT );
	}

CAcmPortFactory::~CAcmPortFactory()
/**
 * Destructor.
 */
	{
	OstTraceFunctionEntry0( DUP1_CACMPORTFACTORY_CACMPORTFACTORY_ENTRY );
	// Delete ACM instances. We could assert that the ACM Class Controller has 
	// caused them all to be destroyed, but if we do that, and USBSVR panics 
	// while it's Started, it will result in C32 panicking too, which is 
	// undesirable. TODO: I'm not sure about this philosophy. 
	iAcmClassArray.ResetAndDestroy();

	// We don't need to clean this array up because C32 will not shut us down 
	// while ports are still open on us.
	iAcmPortArray.Close();
	
	// Detach the local handles
	iAcmProperty.Close();

	// Remove the RProperty entries if they are owned by this instance of the PortFactory
	if(iOwned)
		{
		RProperty::Delete(KUidSystemCategory, KAcmKey);		
		}
	
	delete iAcmServer;
	OstTraceFunctionExit0( DUP1_CACMPORTFACTORY_CACMPORTFACTORY_EXIT );
	}

CPort* CAcmPortFactory::NewPortL(const TUint aUnit)
/**
 * Downcall from C32. Create a new port for the supplied unit number.
 *
 * @param aUnit Port unit number
 */
	{
	OstTraceFunctionEntry0( CACMPORTFACTORY_NEWPORTL_ENTRY );
	OstTrace1( TRACE_NORMAL, CACMPORTFACTORY_NEWPORTL, "CAcmPortFactory::NewPortL;aUnit=%d", aUnit );
	CPort* port = NULL;
	TUint lowerLimit = KAcmLowUnit; // This non-const TUint avoids compiler remarks (low-level warnings) for the following comparisons..
	// ACM ports
	if ( (aUnit >= lowerLimit) && aUnit < static_cast<TUint>( iAcmClassArray.Count()) + KAcmLowUnit)
		{
		// Can only create an ACM port if the corresponding ACM interface 
		// itself has been created. We keep the slots in the  iAcmClassArray array 
		// up-to-date with how many ACM interface instances have been created.
		const TUint index = aUnit - KAcmLowUnit;
		if ( iAcmPortArray[index] )
			{
			if (KErrInUse < 0)
				{
				OstTrace1( TRACE_ERROR, CACMPORTFACTORY_NEWPORTL_DUP2, "CAcmPortFactory::NewPortL;KErrInUse=%d", KErrInUse );
				User::Leave(KErrInUse);
				}
				// TODO: is this ever executed?
			}						   
		iAcmPortArray[index] = CAcmPort::NewL(aUnit, *this);
		iAcmPortArray[index]->SetAcm( iAcmClassArray[index]);
		port = iAcmPortArray[index];		
		}
	// Registration port
	else if ( aUnit == KRegistrationPortUnit )
		{
		port = CRegistrationPort::NewL(*this, KRegistrationPortUnit);
		}
	else 
		{
		if (KErrAccessDenied < 0)
			{
			OstTrace1( TRACE_ERROR, CACMPORTFACTORY_NEWPORTL_DUP3, "CAcmPortFactory::NewPortL;KErrAccessDenied=%d", KErrAccessDenied );
			User::Leave(KErrAccessDenied);
			}
		}

#ifdef _DEBUG
	LogPortsAndFunctions();
#endif
	OstTrace1( TRACE_NORMAL, CACMPORTFACTORY_NEWPORTL_DUP1, "CAcmPortFactory::NewPortL;port=0x%08x", port );
	OstTraceFunctionExit0( CACMPORTFACTORY_NEWPORTL_EXIT );
	return port;
	}

void CAcmPortFactory::DestroyFunctions(const TUint aNoAcms)
/**
 * Utility to delete ACM functions in the array. Deletes ACM functions and 
 * resizes the ACM array down. Also tells each of the affected ACM ports that 
 * its function has gone down.
 *
 * @param aNoAcms Number of ACM interfaces to destroy.
 */
	{
	OstTraceFunctionEntry0( CACMPORTFACTORY_DESTROYFUNCTIONS_ENTRY );
	OstTrace1( TRACE_NORMAL, CACMPORTFACTORY_DESTROYFUNCTIONS, "CAcmPortFactory::DestroyFunctions;aNoAcms=%d", aNoAcms );
#ifdef _DEBUG
	CheckAcmArray();
#endif

	for ( TUint ii = 0 ; ii < aNoAcms ; ii++ )
		{
		const TUint index =  iAcmClassArray.Count() - 1;
		
		// Inform the relevant ACM ports, if any, so they can complete 
		// outstanding requests. NB The ACM port array should have been padded 
		// up adequately, but not necessarily filled with ports.
		if ( iAcmPortArray[index] )
			{
			iAcmPortArray[index]->SetAcm(NULL);
			// Don't remove slots from the ACM port array, because higher-
			// indexed ports might still be open.
			}

		// Destroy ACM interface.
		delete iAcmClassArray[index];
		iAcmClassArray.Remove(index);	
		}
		
	//decrement the published configurations counter
	iConfigBuf().iAcmCount -= aNoAcms;
	PublishAcmConfig();
	
#ifdef _DEBUG
	CheckAcmArray();
	LogPortsAndFunctions();
#endif
	OstTrace0( TRACE_NORMAL, CACMPORTFACTORY_DESTROYFUNCTIONS_DUP1, "CAcmPortFactory::DestroyFunctions;<<CAcmPortFactory::DestroyFunctions" );
	OstTraceFunctionExit0( CACMPORTFACTORY_DESTROYFUNCTIONS_EXIT );
	}

void CAcmPortFactory::CheckAcmArray()
/**
 * Utility to check that each slot in the ACM interface array points to 
 * something valid. NB It is the ACM port array which may contain empty slots.
 */
	{
	OstTraceFunctionEntry0( CACMPORTFACTORY_CHECKACMARRAY_ENTRY );
	for ( TUint ii  = 0; ii < static_cast<TUint>( iAcmClassArray.Count()) ; ii++ )
		{
		if (!iAcmClassArray[ii])
			{			
			OstTraceExt2( TRACE_NORMAL, CACMPORTFACTORY_CHECKACMARRAY, "CAcmPortFactory::CheckAcmArray;code=%d;name=%S", (TInt)EPanicInternalError, KAcmPanicCat);
			__ASSERT_DEBUG( EFalse, User::Panic(KAcmPanicCat, EPanicInternalError) );
			}
		}
	OstTraceFunctionExit0( CACMPORTFACTORY_CHECKACMARRAY_EXIT );
	}

TInt CAcmPortFactory::CreateFunctions(const TUint aNoAcms, const TUint8 aProtocolNum, const TDesC16& aAcmControlIfcName, const TDesC16& aAcmDataIfcName)
/**
 * Tries to create the ACM functions.
 * 
 * @param aNoAcms Number of ACM functions to create.
 * @param aProtocolNum Protocol setting to use for these ACM functions.
 * @param aAcmControlIfcName Control Interface Name or a null descriptor
 * @param aAcmDataIfcName Data Interface Name or a null descriptor
 */
	{
	OstTraceFunctionEntry0( CACMPORTFACTORY_CREATEFUNCTIONS_ENTRY );
	OstTraceExt4( TRACE_NORMAL, CACMPORTFACTORY_CREATEFUNCTIONS, "CAcmPortFactory::CreateFunctions;aNoAcms=%d;aProtocolNum=%d;aAcmControlIfcName=%S;aAcmDataIfcName=%S", (TInt)aNoAcms, (TInt)aProtocolNum, aAcmControlIfcName, aAcmDataIfcName );

#ifdef _DEBUG
	CheckAcmArray();
#endif

	TInt ret = KErrNone;

	// Create the ACM class instances.
	for ( TUint ii = 0 ; ii < aNoAcms ; ii++ )
		{
		OstTrace1( TRACE_NORMAL, CACMPORTFACTORY_CREATEFUNCTIONS_DUP1, "CAcmPortFactory::CreateFunctions;\tabout to create ACM instance %d", ii );
		TRAP(ret, CreateFunctionL(aProtocolNum, aAcmControlIfcName, aAcmDataIfcName));
		if ( ret != KErrNone )
			{
			// Destroy the most recent ACMs that _did_ get created.
			DestroyFunctions(ii);
			break;
			}
		}

	// all the ACM Functions should now have been created. publish the data
	PublishAcmConfig();

#ifdef _DEBUG
	CheckAcmArray();
	LogPortsAndFunctions();
#endif

	OstTrace1( TRACE_NORMAL, CACMPORTFACTORY_CREATEFUNCTIONS_DUP2, "CAcmPortFactory::CreateFunctions;ret=%d", ret );
	OstTraceFunctionExit0( CACMPORTFACTORY_CREATEFUNCTIONS_EXIT );
	return ret;
	}

void CAcmPortFactory::CreateFunctionL(const TUint8 aProtocolNum, const TDesC16& aAcmControlIfcName, const TDesC16& aAcmDataIfcName)
/**
 * Creates a single ACM function, appending it to the  iAcmClassArray array.
 */
	{
	OstTraceFunctionEntry0( CACMPORTFACTORY_CREATEFUNCTIONL_ENTRY );
	OstTraceExt2( TRACE_NORMAL, CACMPORTFACTORY_CREATEFUNCTIONL, 
			"CAcmPortFactory::CreateFunctionL;\tiAcmPortArray.Count()=%d;iAcmClassArray.Count()=%d", 
			iAcmPortArray.Count(), iAcmClassArray.Count() );
	
	OstTraceExt3( TRACE_NORMAL, CACMPORTFACTORY_CREATEFUNCTIONL_DUP1, 
			"CAcmPortFactory::CreateFunctionL;aProtocolNum=%d;aAcmControlIfcName=%S;aAcmDataIfcName=%S", 
			aProtocolNum, aAcmControlIfcName, aAcmDataIfcName );

	CCdcAcmClass* acm = CCdcAcmClass::NewL(aProtocolNum, aAcmControlIfcName, aAcmDataIfcName);
	CleanupStack::PushL(acm);

	// If there isn't already a slot in the ACM port array corresponding to 
	// this ACM interface instance, create one. 
	TInt	err;
	if ( iAcmPortArray.Count() <  iAcmClassArray.Count() + 1 )
		{
		OstTrace0( TRACE_NORMAL, CACMPORTFACTORY_CREATEFUNCTIONL_DUP2, 
				"CAcmPortFactory::CreateFunctionL;\tappending a slot to the ACM port array" );
		err = iAcmPortArray.Append(NULL);
		if (err < 0)
			{
			OstTrace1( TRACE_ERROR, CACMPORTFACTORY_CREATEFUNCTIONL_DUP4, "CAcmPortFactory::CreateFunctionL;err=%d", err );
			User::Leave(err);
			}
		}

	err = iAcmClassArray.Append(acm);
	if (err < 0)
		{
		OstTrace1( TRACE_ERROR, CACMPORTFACTORY_CREATEFUNCTIONL_DUP5, "CAcmPortFactory::CreateFunctionL;err=%d", err );
		User::Leave(err);
		}

	CleanupStack::Pop(acm);
	
	// If there's an ACM port at the relevant index (held open from when USB 
	// was previously Started, perhaps) then tell it about its new ACM 
	// interface.
	if ( iAcmPortArray[iAcmClassArray.Count() - 1] )
		{
		OstTraceExt2( TRACE_NORMAL, CACMPORTFACTORY_CREATEFUNCTIONL_DUP3, 
				"CAcmPortFactory::CreateFunctionL;\tinforming CAcmPort instance %d of acm 0x%p", 
				iAcmClassArray.Count() - 1, acm );
		iAcmPortArray[iAcmClassArray.Count() - 1]->SetAcm(acm);
		}
 
	// update the TPublishedAcmConfig with the current details
	iConfigBuf().iAcmConfig[iConfigBuf().iAcmCount].iProtocol = aProtocolNum;
	iConfigBuf().iAcmCount++;
	//don't update the p&s data here, do it in CreateFunctions after the construction of 
	//all the requested functions
	OstTraceFunctionExit0( CACMPORTFACTORY_CREATEFUNCTIONL_EXIT );
	}

void CAcmPortFactory::Info(TSerialInfo& aSerialInfo)
/**
 * Get info about this CSY, fill in the supplied structure.
 *
 * @param aSerialInfo where info will be written to
 */
	{
	OstTraceFunctionEntry0( CACMPORTFACTORY_INFO_ENTRY );
	// NB Our TSerialInfo does not advertise the existence of the registration 
	// port.
	
	_LIT(KSerialDescription, "USB Serial Port Emulation via ACM");	
	aSerialInfo.iDescription = KSerialDescription;
	aSerialInfo.iName = KAcmSerialName;
	aSerialInfo.iLowUnit = KAcmLowUnit;
	aSerialInfo.iHighUnit = (iAcmPortArray.Count()==0) ? 0 : (iAcmPortArray.Count()-1);
	// See comments in AcmInterface.h 
	OstTraceFunctionExit0( CACMPORTFACTORY_INFO_EXIT );
	}

void CAcmPortFactory::LogPortsAndFunctions()
/**
 * Utility to log in a tabular form all the ACM function instances and any 
 * associated ports. 
 * The number of ACM functions listed is the number we have been asked to 
 * make which we have made successfully. All the ACM 
 * functions listed should be pointing somewhere (enforced by CheckAcmArray). 
 * Any or all of the ACM ports may be NULL, indicating simply that the client 
 * side hasn't opened an RComm on that ACM port yet. 
 * It may also be the case that there are more ACM port slots than there are 
 * ACM functions slots. This just means that USB is in a more 'Stopped' state 
 * than it has been in the past- the ports in these slots may still be open, 
 * so the ACM port array must retain slots for them.
 * All this is just so the logging gives a clear picture of the functions and 
 * port open/closed history of the CSY.
 */
	{
	OstTraceFunctionEntry0( CACMPORTFACTORY_LOGPORTSANDFUNCTIONS_ENTRY );
	TUint ii;

	// Log ACM functions and corresponding ports.
	for ( ii = 0 ; ii < static_cast<TUint>( iAcmClassArray.Count()) ; ii++ )
		{
		OstTraceExt4( TRACE_NORMAL, CACMPORTFACTORY_LOGPORTSANDFUNCTIONS, 
				"CAcmPortFactory::LogPortsAndFunctions;\t iAcmClassArray[%d] = %p, iAcmPortArray[%d] = %p", 
				(TInt)ii, iAcmClassArray[ii], (TInt)ii, iAcmPortArray[ii] );	
		}
	// Log any ports extending beyond where we currently have ACM interfaces.
	for ( ; ii < static_cast<TUint>(iAcmPortArray.Count()) ; ii++ )
		{
		OstTraceExt3( TRACE_NORMAL, CACMPORTFACTORY_LOGPORTSANDFUNCTIONS_DUP1, 
				"CAcmPortFactory::LogPortsAndFunctions;\t iAcmClassArray[%d] = <no slot>, iAcmPortArray[%d] = %p", 
				(TInt)ii, (TInt)ii, iAcmPortArray[ii] );
		}
	OstTraceFunctionExit0( CACMPORTFACTORY_LOGPORTSANDFUNCTIONS_EXIT );
	}

//
// End of file
