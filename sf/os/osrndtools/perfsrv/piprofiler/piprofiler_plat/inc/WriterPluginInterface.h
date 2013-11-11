/*
* Copyright (c) 2009 Nokia Corporation and/or its subsidiary(-ies). 
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


#ifndef __WRITERPLUGIN_INTERFACE__
#define __WRITERPLUGIN_INTERFACE__

#include <e32base.h>
#include <ecom/ecom.h>
#include <badesca.h>


// Constant for indexing (iOrder):
const TInt KWriterPluginNotIndexed      = -1;

/**
 * Constant:    KWriterPluginInterfaceUid
 *
 * Description: UID of this ECOM interface. It should be unique in the system.
 *              It is used to identify this specific custom interface.
 *              Implementations of this interface will use this ID, when they
 *              publish the implementation. Clients use this UID to search for
 *              implementations for this interface (the
 *              EcomInterfaceDefinition.inl does this).
 */
const TUid KWriterPluginInterfaceUid = {0x2001E5BD};

/**
* Used by GetValue(). These are the keys for retrieving a specific
* value. This enum can be extended to provide other values as well as
* long as the original keys are not changed.
*/
enum TWriterPluginValueKeys
    {
    EWriterPluginKeySettingsItemValueString = 1,
    EWriterPluginSettings,
    EWriterPluginEnabled,
    EWriterPluginDisabled,
    EWriterPluginType,
    EWriterPluginVersion
    };
/**
 *
 * Description: UID of this ECOM interface. It should be unique in the system.
 *
 */
    
/**
 * Class:       CWriterInterfaceDefinition
 *
 * Description: Custom ECOM interface definition. This interface is used by
 *              clients to find specific instance and do corresponding
 *              calculation operation for given too numbers. Plugin
 *              implementations implement the Calculate function.
 */

class CProfilerSampleStream;

class CWriterPluginInterface : public CBase
    {

    // CSamplerPluginLoader accesses iOrder which should not be accessed outside.
    friend class CWriterPluginLoader;

public: // Wrapper functions to handle ECOM "connectivity".
        // These are implemented in EComInterfaceDefinition.inl.
        // These functions are used only by the client.
    /**
     * Function:   NewL
     *
     * Description: Wraps ECom object instantitation. Will search for
     *              interface implementation, which matches to given
     *              aOperationName.
     *
     * Param:       aOperationName name of requested implementation.
     *              Implementations advertise their "name" as specified
     *              in their resource file field
     *                 IMPLEMENTATION_INFO::default_data.
     *              For details, see EcomInterfaceDefinition.inl comments.
     *              In this example, the allowed values are "sum" and
     *              "multiply".
     *
     * Note:        This is not a "normal" NewL method, since normally NewL
     *              methods are only defined for concrete classes.
     *              Note that also implementations of this interface provide
     *              NewL methods. They are the familiar NewL's, which create
     *              instance of classes.
     */
    static CWriterPluginInterface* NewL(const TUid aImplementationUid, TAny* aInitParams);

    /**
     * Function:   ~CWriterPluginInterface
     *
     * Description: Wraps ECom object destruction. Notifies the ECOM
     *              framework that specific instance is being destroyed.
     *              See EcomInterfaceDefinition.inl for details.
     */
    virtual ~CWriterPluginInterface();
protected: // New

        /**
        * C++ constructor.
        */
        CWriterPluginInterface();
       
public: 
     /**
      * Method for getting caption of this plugin. This should be the
      * localized name of the settings view to be shown in parent view.
      *
      * @param aCaption pointer to Caption variable
      */
	 virtual TInt 	Start() = 0;
	 
	 virtual void 	Stop() = 0;

	 virtual TUid 	Id() const = 0;
	 
	 virtual void 	GetValue( const TWriterPluginValueKeys aKey, TDes& aValue ) = 0;
	 
	 virtual void 	SetValue( const TWriterPluginValueKeys aKey, TDes& aValue ) = 0;
	
	 virtual void 	GetWriterVersion(TDes* aDes) = 0;

	 virtual TUint32 GetWriterType() = 0;

	 virtual TBool GetEnabled() = 0;

     virtual void   WriteData() = 0;
     virtual void   SetStream( CProfilerSampleStream& aStream ) = 0;
     
	 // internal inline functions
	 inline static void ListAllImplementationsL( RImplInfoPtrArray& aImplInfoArray );
     inline void    SetOrder( TInt aOrder );
     
private:

    /** iDtor_ID_Key Instance identifier key. When instance of an
    * implementation is created by ECOM framework, the
    * framework will assign UID for it. The UID is used in
    * destructor to notify framework that this instance is
    * being destroyed and resources can be released.
    */
    TUid iDtor_ID_Key;
    
    /**
    * Index of the plugin in listbox. Used for CSamplerPluginLoader. Default
    * value is KSamplerPluginNotIndexed which means not ordered. This value is
    * read, if defined, from the opaque_data field of the plugin's resource
    * definition. Index starts from 0.
    */
    TInt iOrder;
public:
	TInt	                      iAdditionalSettings;	
	TBool                         isEnabled;
    };

#include <piprofiler/WriterPluginInterface.inl>

#endif // __WRITERPLUGIN_INTERFACE__
