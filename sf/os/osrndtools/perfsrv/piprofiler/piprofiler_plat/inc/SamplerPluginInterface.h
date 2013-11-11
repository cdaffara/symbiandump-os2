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


#ifndef __SAMPLER_PLUGIN_INTERFACE__
#define __SAMPLER_PLUGIN_INTERFACE__

#include <e32base.h>
#include <ecom/ecom.h>
#include <badesca.h>
#include <piprofiler/ProfilerAttributes.h>	// internal settings format presentations 


// Constant for indexing (iOrder):
const TInt KSamplerPluginNotIndexed      = -1;

/**
 * Constant:    KSamplerPluginInterfaceUid
 *
 * Description: UID of this ECOM interface. It should be unique in the system.
 *              It is used to identify this specific custom interface.
 *              Implementations of this interface will use this ID, when they
 *              publish the implementation. Clients use this UID to search for
 *              implementations for this interface (the
 *              EcomInterfaceDefinition.inl does this).
 */
const TUid KSamplerPluginInterfaceUid = {0x2001E5BC};

/**
 *
 * Description: UID of this ECOM interface. It should be unique in the system.
 *
 */
enum TGenericSettingTypes
{
	EEnablePlugin,
	EOutputSettings,
	ESaveDrive,
	EFilePrefix,
	ETracingMode
};


enum TSamplerCaptionTypes
	{
	ECaptionLengthShort,
	ECaptionLengthMedium,
	ECaptionLengthLong,
    ESettingsCaption
	};


/**
* Used by GetValue(). These are the keys for retrieving a specific
* value. This enum can be extended to provide other values as well as
* long as the original keys are not changed.
*/
enum TSamplerPluginValueKeys
    {

    ESamplerPluginKeySettingsItemValueString = 1,
    ESamplerPluginSettings,
    ESamplerPluginEnabled,
    ESamplerPluginDisabled,
    ESamplerPluginType,
    ESamplerPluginVersion
    };
 

/**
 * Class:       CSamplerInterfaceDefinition
 *
 * Description: Custom ECOM interface definition. This interface is used by
 *              clients to find specific instance and do corresponding
 *              calculation operation for given too numbers. Plugin
 *              implementations implement the Calculate function.
 */
class TBapBuf;
class CProfilerSampleStream;

class CSamplerPluginInterface : public CBase 
{

    // CSamplerPluginLoader accesses iOrder which should not be accessed outside.
    friend class CSamplerPluginLoader;

public: 
	// Wrapper functions to handle ECOM "connectivity".
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
    static CSamplerPluginInterface* NewL(const TUid aImplementationUid, TAny* aInitParams);

    /**
     * Function:   ~CSamplerPluginInterface
     *
     * Description: Wraps ECom object destruction. Notifies the ECOM
     *              framework that specific instance is being destroyed.
     *              See EcomInterfaceDefinition.inl for details.
     */
    virtual ~CSamplerPluginInterface();
protected: // New

	/**
	* C++ constructor.
	*/
	CSamplerPluginInterface();
        
public: 
	// Public pure virtual functions, which are implemented by
    // interface implementations (See ..\plugin)
    
    /**
     * Method for initializing and starting of profiling in single plugin implementation
     * @param aStream is a data stream to store the gathered data, provided by engine
     * @return TInt if no error KErrNone, else any of system-wide errors
     */
    virtual TInt	ResetAndActivateL(CProfilerSampleStream& aStream) = 0;

	/**
     * Method for stopping of profiling in single plugin implementation
     * @return TInt if no error KErrNone, else any of system-wide errors
     */
    virtual TInt	StopSampling() = 0;
	
    /**
     * Method for checking if plugin is enabled
     * @return TBool if enabled return ETrue else EFalse
     */
    virtual TBool   Enabled() = 0;
 	
    /**
    * Method for getting an array of sampler attributes, size of an array: 1...n
    * @return array of settings of one or several sampler plugins
    */
    virtual void GetAttributesL(CArrayFixFlat<TSamplerAttributes>* aAttributeArray) = 0;	
    
    /**
    * Method for setting configurations of single sampler attributes
    * @param aAttributes contains settings of a single sampler plugin 
    */
    virtual TInt SetAttributesL(TSamplerAttributes& aAttributes) = 0; 
    
    /**
    * Method for parsing text formatted settings block and converting
    * it to TSamplerAttributes data structure
    * @param aSingleSettingArray containing setting lines of a single sampler
    * @return status of conversion, if success KErrNone, else KErrGeneral
    */
    virtual TInt ConvertRawSettingsToAttributes(CDesC8ArrayFlat* aSingleSettingArray) = 0;
 
    /**
    * Method for getting UID of this plugin.
    * @param aSubId the implementation id of sub sampler
    * @returns uid of sampler or sub sampler, if aSubId -1 uid of sampler, else uid of sub sampler
    */
    virtual TUid 	Id(TInt aSubId) const = 0;
    
    /**
    * Method for getting locally defined sub ID value inside a specific plug-in.
    * @param aUid of a specific sampler
    * @returns local ID of sampler or sub sampler
    */
    virtual TInt 	SubId(TUid aUid) const = 0;
    
    /**
    * Method for getting sampler type.
    * @returns PROFILER_USER_MODE_SAMPLER, PROFILER_KERNEL_MODE_SAMPLER or PROFILER_DUMMY_MODE_SAMPLER
    */
    virtual TInt 	GetSamplerType() = 0;
	 
	 
	 // some internal inline methods, used by engine
    inline TInt     Flush();
    inline TInt     AddSample(TUint8* sample, TUint32 length, TInt limitSize);  
    inline void     SetOrder( TInt aOrder );
    inline static void ListAllImplementationsL(RImplInfoPtrArray& aImplInfoArray);

    /**
    * Static methods for getting setting value out of descriptor
    * 
    * @param aBuf buffer where to convert the value
    * @param aValue parameter where to store the requested type of value
    */
    inline static void     Str2Bool(const TDesC8& aBuf, TBool& aValue);
    inline static void     Str2Int(const TDesC8& aBuf, TInt& aValue);
    inline static void     Str2Int(const TDesC8& aBuf, TUint32& aValue);
     
    /** iDtor_ID_Key Instance identifier key. When instance of an
     *               implementation is created by ECOM framework, the
     *               framework will assign UID for it. The UID is used in
     *               destructor to notify framework that this instance is
     *               being destroyed and resources can be released.
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
	TInt 					iSamplerType;
	
	// this variable must be defined by the extending classes!!
	TInt					iSamplerId;
	
	CProfilerSampleStream*	iStream;
    TBool                   iEnabled;
		
private:
	TBapBuf*				iBuffer;
};

#include <piprofiler/ProfilerGenericClassesUsr.h>
#include <piprofiler/SamplerPluginInterface.inl>


#endif // __SAMPLER_PLUGIN_INTERFACE__
