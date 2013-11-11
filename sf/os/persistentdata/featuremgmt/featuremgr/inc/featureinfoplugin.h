/*
* Copyright (c) 2007-2009 Nokia Corporation and/or its subsidiary(-ies).
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



#ifndef FEATUREINFOPLUGIN_H
#define FEATUREINFOPLUGIN_H

/**
@file
@publishedPartner
@released

This header file holds the definitions for the Feature Manager adaptation
interface - Feature Info Plug-in API.
It holds interface and command definitions and the related command and response
types and structures used over this interface.
*/


// INCLUDES

#include <e32base.h>
#include <featmgr/featurecmn.h>


// CONSTANTS

/**
This constant holds the ECOM Instantiation Interface UID. This interface UID
identifies plug-in implementations that derive from CFeatureInfoPlugin.
This interface is used by the Feature Manager server when it is started up.
@see CFeatureInfoPlugin
*/
const TUid KFeatureInfoPluginInterfaceUid = { 0x10205057 };


// TYPES & STRUCTURES

/**
FeatureInfoCommand namespace groups the command ID enumerations, structures
and types used over the CFeatureInfoPlugin interface.
*/
namespace FeatureInfoCommand
    {

    /**
    Command definitions
    All commands contain a command Id and a set of parameters. The command
    Ids are defined first then the parameters for each command.

    The CFeatureInfoPlug ininterface is asynchronous. A command Id is sent to
    the plugin through CFeatureInfoPlugin::ProcessCommandL(). If the command is
    supported by the plug-in it schedules an active object to do the work and
    call MFeatureInfoPluginCallback::ProcessResponseL() to complete the command.
    If the command is not supported, the plug-in should leave with
    KErrNotSupported.

	The field Input means the parameter for the command.
	The field Output means the parameter for the response.
	@see CFeatureInfoPlugin
	@see MFeatureInfoPluginCallback
    */
    enum TFeatureInfoCmd
        {
        /**
        No command. May be used for sanity checks, but
        never as an actual command ID.
        Input    None
		Output   None
        */
        ENoCommandId = 1000,

        /**
		FM uses this command to request feature information of read-only
        features, aka simple features, known to the plug-in. Implementations
        should used this command to return such features, if any are known.
        If no such features are known to the plug-in it should leave with
        KErrNotSupported, hence this command is optional.
        Features contained in a response to this command should not appear
        in ELoadEnhancedFeatureInfoCmdId responses, for efficiency.
		Input    None
		Output   TFeatureInfoRespPckg
        */
        ELoadFeatureInfoCmdId,

        /**
		FM uses this command to request feature information of
        features that have custom flags and user-data, that are known to the
        plug-in. Implementations should use this command to return such
        features, if any are known. If no such features are known to the
        plug-in it should leave with KErrNotSupported, hence this command is
        optional.
        Features contained in a response to this command should not appear
        in ELoadFeatureInfoCmdId responses, for efficiency.
        Input    None
		Output   TEnhancedFeatureInfoRespPckg
        */
        ELoadEnhancedFeatureInfoCmdId
        };

    /**
    Data structure used to list the features in data structure TFeatureInfo.
    Used in ELoadFeatureInfoCmdId commands.
    */
    struct TFeature
   	    {
   	    // UID of the feature, as found in a system header file.
   	    TUint32 iFeatureID;

   	    // Value of the 'Supported?' feature flag.
   	    // If feature has been turned OFF from a product the value is set to
   	    // EFalse and if it has been turned ON the value is set to ETrue.
        TBool iValue;
   	    };

   	/**
    Data structure used to pass data with ELoadFeatureInfoCmdId response.
    Used in ELoadFeatureInfoCmdId commands. It contains the return code for
    the command iErrorCode and is typically:
      No leave,  just returns - iList contains feature entries.
      KErrNotSuppoted - no features to report for this command.
      ...             - other  system wide error.
    */
    struct TFeatureInfo
   	    {
   	    // System wide error code.
   	    TInt iErrorCode;

   	    // List of read-only feature entries.
   	    RArray<TFeature> iList;
   	    };

    /** Data package for ELoadFeatureInfoCmdId response structure */
    typedef TPckgBuf<TFeatureInfo>	TFeatureInfoRespPckg;

   	/**
    Data structure used to pass data with ELoadEnhancedFeatureInfoCmdId
    response. Used in ELoadEnhancedFeatureInfoCmdId commands. It contains the
    return code for the command iErrorCode and is typically:
      No leave,  just returns - iList contains feature entries.
      KErrNotSuppoted - no features to report for this command.
      ...             - other  system wide error.
    @see RFeatureArray
    */
    struct TEnhancedFeatureInfo
   	    {
   	    // System wide error code.
   	    TInt iErrorCode;

   	    // List of enhanced feature records.
   	    // List consists of TFeatureEntry entries. Plugin should initialize
   	    // feature ID and feature support status flag and optionally any other
   	    // feature flags as specified in TFeatureFlags. If feature has
        // associated user-data, it should be initialized for response as well.
        // If there is no user-data value this must be set to 0.
        // For type definitions see featurecmn.h
   	    RFeatureArray iList;
   	    };

    /** Data package for ELoadEnhancedFeatureInfoCmdId response structure */
    typedef TPckgBuf<TEnhancedFeatureInfo>	TEnhancedFeatureInfoRespPckg;

    } // end namespace


// CLASS DECLARATION

/**
This M-class is an interface class implemented by FM server to receive
feature information responses from adaptation plug-ins i.e. they call the
server back with the data. Reference supplied to the plugin implementation
at construction and available in iResponseCallback.

@see CFeatureInfoPlugin
*/
class MFeatureInfoPluginCallback
    {
    public:
        /**
        Method to return data in response to a message from
        a Feature Info plugin. The related ProcessCommandL call
        must return before this method can be called.

        @param aCommandId Command ID for which the response comes
        @param aTransId   Transcation identifier of
                          the original command
        @param aData      Data returned from call.
                          Data package contents are defined by command.
                          Can be deleted right after
                          ProcessResponseL has returned.
        */
        virtual void ProcessResponseL(
                        const FeatureInfoCommand::TFeatureInfoCmd aCommandId,
                        const TUint8 aTransId,
                        TDesC8& aData ) = 0;
    };


/**
Feature Manager ECOM Plugin interface class to be implemented by adaptation.
As it is an adaptation interface implementations are only loaded from ROM.

Implementations of this interface are created by the Feature Manager server
during start up in it's main thread where an Active Scheduler is installed.
Implementations must perform minmial work in ProcessCommandL() and return.
Plugin work and calls to ProcessResponseL() must be performed asynchronsly in
an active object callback running in the server's main thread.

For the command Ids and types used with this interface see definitions in the
FeatureInfoCommand namespace.

@see MFeatureInfoPluginCallback
@see KFeatureInfoPluginInterfaceUid
*/
class CFeatureInfoPlugin : public CBase
    {
    public:  // construction and destruction

        /**
        Constructor method for instance.
        Uses ECom to find correct instance.

        @param aImplementationUid UID of the interface
                                  implementation to instantiate.
        @param aResponseCallback  Reference to plugin callback handler.
        */
        inline static CFeatureInfoPlugin* NewL(TUid aImplementationUid,
                                MFeatureInfoPluginCallback& aResponseCallback);

        /**
        Destructor
        */
        inline virtual ~CFeatureInfoPlugin();

    public:
        /**
        Method to invoke a particular command in the plugin.
        Response to method is returned via separate ProcessResponseL
        call. Call to ProcessResponseL is done after the call to
        ProcessCommandL returns.
        Leaves with error code KErrNotSupported if command ID is not
        supported by the plug-in. If ProcessCommandL leaves, no corresponding
        ProcessResponseL is expected.

        @param aCommandId Command ID
        @param aTransId   Transaction ID
        @param aData      Data associated with command.
                          Data package contents are defined by command.
                          Some commands require no data and pass
                          empty buffer as aData.
        @leave KErrNotSupported aCommandId not supported by plug-in.
        */
        virtual void ProcessCommandL(
                        const FeatureInfoCommand::TFeatureInfoCmd aCommandId,
                        const TUint8 aTransId,
                        TDesC8& aData ) = 0;


   protected:
        /**
        Callback pointer to be used with responses to commands.
        This pointer is not owned by this class.
        */
        MFeatureInfoPluginCallback* iResponseCallback;  // not owned

   private:
	    /** Destructor identifier to be used with ECom framework. */
        TUid iDestructorIDKey;
    };

#include <featmgr/featureinfoplugin.inl>

#endif      // FEATUREINFOPLUGIN_H

// End of File
