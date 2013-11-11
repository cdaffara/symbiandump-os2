// Copyright (c) 2001-2009 Nokia Corporation and/or its subsidiary(-ies).
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
// The API definitions required for the use of the ECOM framework by a client 
// are contained / defined here.
// 
//

/**
 @file
 @publishedAll
 @released 
*/

#ifndef __ECOM_H__
#define __ECOM_H__

// ____________________________________________________________________________
// INCLUDES

#include <e32base.h>
#include <s32std.h>
#include <f32file.h>

#include <ecom/ecomerrorcodes.h>
#include <ecom/ecomresolverparams.h>
#include <ecom/implementationinformation.h>


// ____________________________________________________________________________
// CONSTANTS

class CGlobalData;

/** 
The UID identifying the ROM-Only CResolver Interface Implementation. 
This resolver only returns implementations present on R/O internal media
(built-in) or upgrades of such implementations on R/W media. It does not 
return implementations that solely reside on R/W media. 
*/
const TUid KRomOnlyResolverUid = {0x10009D92};


// ____________________________________________________________________________
// CLASSES

/**
The REComSession singleton class provides an API to identify (resolution), 
instantiate, and destroy interface class implementations requested by 
ECOM clients. It manages the session connection to the ECOM Server itself so 
that typically only one connection is made to the ECOM Server per thread.
It provides an un-initialise function where it releases memory and handles
of unused plug-ins, see the FinalClose() function. It also ensures that all
necessary clean-up is performed in the event of a Leave by any of the
ListImplementationsL() or CreateImplementationL() methods.

Note: Although a public default constructor is provided in this class
it is strongly recommended this object is never created by users of this
API on the stack or heap by value. To access the non-static methods of this
API use the static OpenL() method to obtain a pointer or a 
reference to the single reference counted session in use. Close() must be 
called when finished with.

Special care must be taken when using aInstanceKey. It is a 32-bit instance key
that is returned from a call to one of the CreateImplementationL() methods. 
This instance key must only be used with DestroyedImplementation() to identify
the implementation instance for destruction, or GetImplementationUidL() to
retrieve the Implementation Uid of the instance it refers to.
No meaning must be attached to this instance key by ECOM clients 
and it must not be used for any other purpose.


@see REComSession::FinalClose()
*/
class REComSession  : public RSessionBase
	{
	friend class CGlobalData;
	
public: // publishedAll APIs
        // Available for use by all clients

	IMPORT_C REComSession();
	
	IMPORT_C static REComSession& OpenL();
	IMPORT_C void Close();

	// Get Implementation Uid from an Instance Key
	IMPORT_C static TUid GetImplementationUidL(
							TUid aInstanceKey);

    // Destroy Implementation API declaration											
	IMPORT_C static void DestroyedImplementation(
                            TUid aInstanceKey);

	// See developer documentation for use of FinalClose.
	IMPORT_C static void FinalClose();
	

	// Notification API declarations
    //
     
	IMPORT_C void NotifyOnChange(TRequestStatus& aStatus);
	IMPORT_C void CancelNotifyOnChange(TRequestStatus& aStatus);
	
	
	// List implementations API declarations
	//
	// Use LI1 to list all implementations, unfiltered.
	// Use LI2 when default resolving required on the returned list.
	// Use LI3 when custom resolving required on the returned list.
		
	// ListImplementationsL - LI1
	IMPORT_C static void ListImplementationsL(
                            TUid aInterfaceUid,
							RImplInfoPtrArray& aImplInfoArray);
							
	// ListImplementationsL - LI2
	IMPORT_C static void ListImplementationsL(
                            TUid aInterfaceUid, 
							const TEComResolverParams& aResolutionParameters,
							RImplInfoPtrArray& aImplInfoArray);
							
	// ListImplementationsL - LI3
	IMPORT_C static void ListImplementationsL(
                            TUid aInterfaceUid, 
							const TEComResolverParams& aResolutionParameters, 
							TUid aResolverUid,
							RImplInfoPtrArray& aImplInfoArray);
							
	// Use LI7 to list all implementations, unfiltered and with no capability chaeck.
	// Use LI8 when default resolving required on the returned list and with no capability chaeck.
	// Use LI9 when custom resolving required on the returned list and with no capability chaeck.
		
	// ListImplementationsL - LI7
	IMPORT_C static void ListImplementationsL(
                            TUid aInterfaceUid,
							RImplInfoPtrArray& aImplInfoArray,TBool aCapabilityCheck);
							
	// ListImplementationsL - LI8
	IMPORT_C static void ListImplementationsL(
                            TUid aInterfaceUid, 
							const TEComResolverParams& aResolutionParameters,
							RImplInfoPtrArray& aImplInfoArray,TBool aCapabilityCheck);
							
	// ListImplementationsL - LI9
	IMPORT_C static void ListImplementationsL(
                            TUid aInterfaceUid, 
							const TEComResolverParams& aResolutionParameters, 
							TUid aResolverUid,
							RImplInfoPtrArray& aImplInfoArray,TBool aCapabilityCheck);						


	// Create implementations API declarations
    //
    // When Implementation UID is known: 
    // Use CI1, CI2. 
    // Use CI3, CI4 when you have construction parameters.
    //
    // When the Interface UID is known and default resolving is required:
    // Use CI5, CI6. 
    // Use CI7, CI8 when you have construction parameters.
    // 
    // When the Interface UID is known and custom resolving is required:
    // Use CI9, CI10.
    // Use CI11, CI12 when you have construction parameters.
    	
	// CreateImplementationL - CI1
	IMPORT_C static TAny* CreateImplementationL(
                            TUid aImplementationUid, 
                            TUid& aInstanceKey);
                            
	// CreateImplementationL - CI2
	IMPORT_C static TAny* CreateImplementationL(
                            TUid aImplementationUid, 
							TInt32 aKeyOffset);
							
	// CreateImplementationL - CI3
	IMPORT_C static TAny* CreateImplementationL(
                            TUid aImplementationUid, 
							TUid& aInstanceKey, 
							TAny* aConstructionParameters);
							
	// CreateImplementationL - CI4
	IMPORT_C static TAny* CreateImplementationL(
                            TUid aImplementationUid, 
							TInt32 aKeyOffset,
							TAny* aConstructionParameters);
							
	// CreateImplementationL - CI5
	IMPORT_C static TAny* CreateImplementationL(
                            TUid aInterfaceUid, 
							TUid& aInstanceKey,
							const TEComResolverParams& aResolutionParameters);
							
	// CreateImplementationL - CI6
	IMPORT_C static TAny* CreateImplementationL(
                            TUid aInterfaceUid, 
							TInt32 aKeyOffset, 
							const TEComResolverParams& aResolutionParameters);
							
	// CreateImplementationL - CI7
	IMPORT_C static TAny* CreateImplementationL(
                            TUid aInterfaceUid, 
							TUid& aInstanceKey, 
							TAny* aConstructionParameters, 
							const TEComResolverParams& aResolutionParameters);
							
	// CreateImplementationL - CI8
	IMPORT_C static TAny* CreateImplementationL(
                            TUid aInterfaceUid, 
							TInt32 aKeyOffset, 
							TAny* aConstructionParameters, 
							const TEComResolverParams& aResolutionParameters);
							
	// CreateImplementationL - CI9
	IMPORT_C static TAny* CreateImplementationL(
                            TUid aInterfaceUid, 
							TUid& aInstanceKey, 
							const TEComResolverParams& aResolutionParameters,
							TUid aResolverUid);
												
	// CreateImplementationL - CI10
	IMPORT_C static TAny* CreateImplementationL(
                            TUid aInterfaceUid, 
							TInt32 aKeyOffset,
							const TEComResolverParams& aResolutionParameters, 
							TUid aResolverUid);
												
	// CreateImplementationL - CI11
	IMPORT_C static TAny* CreateImplementationL(
                            TUid aInterfaceUid, 
							TUid& aInstanceKey, 
							TAny* aConstructionParameters,
							const TEComResolverParams& aResolutionParameters, 
							TUid aResolverUid);
												
	// CreateImplementationL - CI12
	IMPORT_C static TAny* CreateImplementationL(
                            TUid aInterfaceUid, 
							TInt32 aKeyOffset, 
							TAny* aConstructionParameters,
							const TEComResolverParams& aResolutionParameters,
							TUid aResolverUid);
	

public: // publishedPartner APIs
        // Extended interfaces API declarations available for use by 
        // all system software i.e by phone manufacturers and internally (i.e. not supported for use by 3rd parties)
        // See Symbian OS Developer Library and ECOM How To/FAQ documentation.
				
                			
	// List implementations API declarations
	//
	// Use L14 to list all implementations which satisfy interface, extended 
    //      interfaces
	// Use L15 to list all implementations which satisfy interface, extended 
    //      interfaces when default resolving required on the list returned
	// Use L16 to list all implementations which satisfy interface, extended
    //      interfaces when custom resolving required on the list returned
		
	// ListImplementationsL - LI4
	IMPORT_C static void ListImplementationsL(
                            TUid aInterfaceUid,
                            RExtendedInterfacesArray& aExtendedInterfaces,
							RImplInfoPtrArray& aImplInfoArray);
							
	// ListImplementationsL - LI5
	IMPORT_C static void ListImplementationsL(
                            TUid aInterfaceUid, 
                            RExtendedInterfacesArray& aExtendedInterfaces,
							const TEComResolverParams& aResolutionParameters,
							RImplInfoPtrArray& aImplInfoArray);
							
	// ListImplementationsL - LI6
	IMPORT_C static void ListImplementationsL(
                            TUid aInterfaceUid, 
                            RExtendedInterfacesArray& aExtendedInterfaces,
							const TEComResolverParams& aResolutionParameters, 
							TUid aResolverUid,
							RImplInfoPtrArray& aImplInfoArray);
			
			
	// Create implementations API declarations
    //
    // When the interface UID and  the extended interface UID's are known 
	//     and default resolving is required. The construction parameters may 
    //     optionally be provided.
    //     Use CI13: Instance key is returned
 	//     Use CI14: An offset to aInstanceKey returned by the ECom framework 
    //         to identify this instance to the framework
	// When the interface UID and  the extended interface UID's are known 
	//     and custom resolving is required.  The construction parameters may 
    //     optionally be provided.
    //     Use CI15: Instance key is returned
    //     Use CI16: An offset to aInstanceKey returned by the ECom framework 
    //         to identify this instance to the framework
    	
	// CreateImplementationL - CI13
	IMPORT_C static TAny* CreateImplementationL(
                            TUid aInterfaceUid, 
                            const RExtendedInterfacesArray& aExtendedInterfaces,
							TUid& aInstanceKey, 
							const TEComResolverParams& aResolutionParameters,
							TAny* aConstructionParameters = NULL);
												
	// CreateImplementationL - CI14
	IMPORT_C static TAny* CreateImplementationL(
                            TUid aInterfaceUid, 
                            const RExtendedInterfacesArray& aExtendedInterfaces,
							TInt32 aKeyOffset, 
							const TEComResolverParams& aResolutionParameters,
							TAny* aConstructionParameters = NULL);			

	// CreateImplementationL - CI15
	IMPORT_C static TAny* CreateImplementationL(
                            TUid aInterfaceUid, 
                            const RExtendedInterfacesArray& aExtendedInterfaces,
							TUid& aInstanceKey, 
							const TEComResolverParams& aResolutionParameters, 
							TUid aResolverUid,
							TAny* aConstructionParameters = NULL);
												
	// CreateImplementationL - CI16
	IMPORT_C static TAny* CreateImplementationL(
                            TUid aInterfaceUid, 
                            const RExtendedInterfacesArray& aExtendedInterfaces,
							TInt32 aKeyOffset, 
							const TEComResolverParams& aResolutionParameters,
							TUid aResolverUid,
							TAny* aConstructionParameters = NULL);	
           		
                   	
	// Extended Interface query and management API declarations
	//
							
	IMPORT_C static TAny* GetExtendedInterfaceL(
                            const TUid& aInstanceKey, 
                            const TUid& aExtendedInterfaceUid);
	
	IMPORT_C static void ListExtendedInterfacesL(
                            const TUid& aImplementationUid, 
                            RExtendedInterfacesArray& aIfInfo);
	
	IMPORT_C static void ManuallyReleaseExtendedInterfaceL(
                            const TUid& aInstanceKey, 
                            const TUid& aExtendedInterfaceUid);
	

private: // @internalComponent APIs
         // This API is for internal testing purposes.Should not be used externally by clients.

	static REComSession& OpenLC();
	
	void ConstructL();
	void ReallyClose();
	
	void ListImplementationsL(
            TInt aServiceId, 
			TUid aInterfaceUid, 
			const RExtendedInterfacesArray& aExtendedInterfaces,
			const TEComResolverParams& aResolutionParameters, 
			TUid aResolverUid,
			RImplInfoPtrArray& aImplInfoArray,
			TBool aCapabilityCheck = ETrue);
			
	TAny* ResolveAndCreateImplL(
            TUid aImplementationUid, 
			TUid& aInstanceKey, 
			TAny* aCreationParameters, 
            TBool aCreationParamsFlag);
                                     
	TAny* ResolveAndCreateImplL(
            TUid aInterfaceUid, 
			const TEComResolverParams& aResolutionParameters, 
			TUid& aInstanceKey, 
			TAny* aCreationParameters, 
            TBool aCreationParamsFlag);
                                     
	TAny* ResolveAndCreateImplL(
            TUid aInterfaceUid, 
			const TEComResolverParams& aResolutionParameters, 
			TUid aResolverUid, 
			TUid& aInstanceKey, 
			TAny* aCreationParameters, 
            TBool aCreationParamsFlag);
	
	TAny* ResolveAndCreateImplL(
            TUid aInterfaceUid, 
            const RExtendedInterfacesArray& aExtendedInterfaces, 
			const TEComResolverParams& aResolutionParameters, 
			TUid& aInstanceKey, 
			TAny* aCreationParameters, 
            TBool aCreationParamsFlag);
                                     
	TAny* ResolveAndCreateImplL(
            TUid aInterfaceUid, 
            const RExtendedInterfacesArray& aExtendedInterfaces, 
			const TEComResolverParams& aResolutionParameters, 
			TUid aResolverUid, 
			TUid& aInstanceKey, 
			TAny* aCreationParameters, 
            TBool aCreationParamsFlag);

	TAny* ResolveAndCreateImplL(
            TInt aServiceId,
			TUid aInterfaceUid,
			const RExtendedInterfacesArray& aExtendedInterfaces, 
			const TEComResolverParams& aResolutionParameters, 
			TUid aResolverUid, 
			TUid& aInstanceKey, 
			TAny* aCreationParameters, 
            TBool aCreationParamsFlag);
	
	void ProcessListExtendedInterfacesL(
            const TUid& aImplementationUid,
			RExtendedInterfacesArray& aIfInfo);   
	
	void PackMatchStrAndExtendedInterfacesL(
            const TEComResolverParams& aResolutionParameters, 
			const RExtendedInterfacesArray& aExtendedInterfaces, 
			RBuf8& aMatchStrExtInfBuf);
                             
                                     
public: // @internalComponent APIs
    	// Internal methods used inside ECOM, not for client use.

    // This API is for internal testing purposes.
	IMPORT_C static void SetGetParametersL(const TIpcArgs &aArgs);
			
            									
private:
    /** Not used */
	TInt iReserved1;

	/** not used */
	TInt iReserved3;
    /** not used */
	TInt iReserved2;
	};

#endif	// __ECOM_H__
