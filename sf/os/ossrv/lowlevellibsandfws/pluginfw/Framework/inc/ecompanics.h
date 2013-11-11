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

#ifndef ECOMPANICS_H
#define ECOMPANICS_H

/**
The following enumaration defines the ECOM panic codes used in the ecom.dll and 
ecomserver.exe binaries. They are provided for debug purposes only to help 
diagnose problems.
Server panics have the category - "ECOMServer" and client DLL panics have the
category - "ECOMClientDLL". ECOM will log a message to RDebug::Print prior to 
panics to give further information.
@internalComponent
*/
enum TEComPanic
{
    EEComPanicUndefined = 0,
    
    //------------------------------------------------------------------------
    
    EEComPanicClientDllBase = -16900,
    
    /** ECOM API used before when there is no open session to ECOM Server.
    Raised on: UREL & UDEB builds */
    EEComPanic_LibraryNotInitialised                                = -16900,
    
    /** Plug-in implementation claims to support an extended object that
    requires allocation/deallocation but supplies no release function.
    Raised on: UDEB builds */
    EEComPanic_CInstanceInfoExtended_CreateExtObjectL_NoReleaseFunc = -16901,
    
    /** Plug-in implementation claims to support an extended object that
    requires allocation/deallocation but failed to supply a release object.
    Raised on: UDEB builds */
    EEComPanic_CInstanceInfoExtended_CreateExtObjectL_NoReleaseObj  = -16902,
    
    /** Plug-in implementation claims to support an extended object that
    requires allocation/deallocation but failed to supply a release object.
    Raised on: UDEB builds */
    EEComPanic_CInstanceInfoExtended_DestroyExtObject_NoReleaseObj  = -16903,

    /** Client supplied an implementation instance key which is not known to
    the ECOM API i.e. it was not returned from a previous call.
    Raised on: UDEB builds */
    EEComPanic_InvalidImplementationInstanceKey                     = -16904,
    
    EEComPanicClientDllEnd = -16949,  
      
    //------------------------------------------------------------------------

    EEComPanicServerBase = -16950,

    /** Fatal leave occurred in the CServerStartupMgr class during startup
    Raised on: UREL & UDEB builds */
    EEComPanic_CServerStartupMgr_RunError                           = -16950,
    
    /** Fatal leave occurred in the CServerStartupMgr class during 
    plug-in (re)scanning and registration. e.g. Out of memory  
    Raised on: UREL & UDEB builds */
    EEComPanic_CDiscoverer_CIdleScanningTimer_RunError              = -16951,
     
    /** Fatal leave occurred in the CIdleScanningTimer class during 
    plug-in (re)scanning and registration. e.g. Out of memory  
    Raised on: UREL & UDEB builds */
    EEComPanic_CDiscoverer_CDirChangeNotifier_RunError              = -16952,
    
    /** Input stream used to construct CImplementationInformation has 
    duplicate extended interfaces UIDs.    
    Raised on: UDEB builds */
    EEComPanic_CImlpementationInfromation_DuplicateExIf             = -16953,
    
    /** Plug-in resource file contains duplciate extended interface UIDs.
    Raised on: UDEB builds */
    EEComPanic_CParseImplementationDataFormatVersion3_DuplicateExIf = -16954,
    
    /** Server invariant test. CDllData object previously found in the registry
    was found to be missing during an update operation following a re-scan.
    Raised on: UDEB builds */
    EEComPanic_CRegistryData_UpdateDllDataL_DllRegistrationMissing  = -16955,
    
    EEComPanicServerEnd  = -16999    
    
    //------------------------------------------------------------------------
};

// ____________________________________________________________________________
// FUNCTIONS

/** 
Function for clean up RExtendedInterfaceArray when leave occurs
@internalComponent
*/
extern void CloseAndDeleteArray(TAny* aPtr);



#endif // ECOMPANICS_H
