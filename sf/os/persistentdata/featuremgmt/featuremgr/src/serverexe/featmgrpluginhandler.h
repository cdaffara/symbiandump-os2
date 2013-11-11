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



#ifndef FEATMGRPLUGINHANDLER_H
#define FEATMGRPLUGINHANDLER_H

//  INCLUDES
#include <e32base.h>
#include <featmgr/featureinfoplugin.h>

// FORWARD DECLARATIONS
class CFeatMgrServer;

// CLASS DECLARATIONS
    
/**
*  Feature Manager server side plugin
*  handler implementation.
*
*/
NONSHARABLE_CLASS(CFeatMgrPluginHandler) : 
    public CBase,
    public MFeatureInfoPluginCallback
    {
    public: // Constructors and Destructor
    
        /**
        * Constructor method for instance.
        *
        * @param aImplementationUid Determines the plugin that 
        *                           the loaded plugin implements.
        * @param aServer            Reference to the Feature Manager Server
        */
        static CFeatMgrPluginHandler* NewL(TUid aImplementationUid, 
                                           CFeatMgrServer& aServer);
        
        /**
        * Destructor.
        */
        virtual ~CFeatMgrPluginHandler();
    
        /**
        * Method used to send command for plugin.
        *
        * @param aCommandId Command identifier
        */
        void SendCommandL( FeatureInfoCommand::TFeatureInfoCmd aCommandId );
    
    public: // New functions
   	
    	// MFeatureInfoPluginCallback callback methods
    	
        /**
        * Method to return data in response to a message from a Feature Manager plugin.
        *
        * @param aCommandId Command identifier
        * @param aTransId   Unique transcation identifier
        * @param aData      Data returned from call
        */
        void ProcessResponseL( FeatureInfoCommand::TFeatureInfoCmd aCommandId,
                               TUint8 aTransId, 
                               TDesC8& aData ); 
                                                        	

    private: 
    
        /**
        * C++ default constructor.
        */
        CFeatMgrPluginHandler( CFeatMgrServer& aServer );
        
        /**
        * This 2nd phase constructor.
        */
        void ConstructL( TUid aImplementationUid );
        
        /**
        * Processing of features with (supported-)value.
        */
        void ProcessFeatureInfoL( RArray<FeatureInfoCommand::TFeature>& aList, 
            FeatureInfoCommand::TFeatureInfoRespPckg aResponse );
        
        /**
        * Processing of features with flags and data.
        */
        void ProcessFeatureInfoL( RFeatureArray& aList, 
            FeatureInfoCommand::TEnhancedFeatureInfoRespPckg aResponse );
        
	private:   // data
	
	    CFeatureInfoPlugin*                 iPlugin;    // Pointer to plugin 
	    CFeatMgrServer&                     iServer;    // Server reference.
	    TUint8                              iTransId;   // Transaction id, for future use.
        FeatureInfoCommand::TFeatureInfoCmd iCommandId; // Command id	     
    };

#endif  // FEATMGRPLUGINHANDLER_H
            
// End of File
