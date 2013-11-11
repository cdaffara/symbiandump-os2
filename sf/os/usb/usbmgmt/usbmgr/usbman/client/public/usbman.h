/*
* Copyright (c) 1997-2009 Nokia Corporation and/or its subsidiary(-ies).
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
* RUsb Client side header
* Implements the Symbian OS USB Management API RUsb
*
*/

/**
 @file
*/

#ifndef __USBMAN_H__
#define __USBMAN_H__

#include <e32std.h>
#include <usberrors.h>
#include <usbstates.h>
#include <usb/usbshared.h>

// The propery of a personality is a bitmap, and bit 0 is used to identify
// whether a personality is hidden.
const TUint32 KUsbPersonalityPropertyHidden = 0x00000001;

NONSHARABLE_CLASS(RUsb) : public RSessionBase
/**
The RUsb class implements the Symbian OS USB Management API RUsb

@publishedPartner
@released
*/
	{
public:

	// Request types, the interest of which can be cancelled by clients
	enum TUsbReqType
		{
		EStart,
		EStop,
		ETryStart,
		ETryStop
		};
		
	/**
	Constructor

	
	@since	7.0

	@publishedPartner
	@released
	 */
	IMPORT_C RUsb();

	/**
	Destructor

	
	@since	7.0

	@publishedPartner
	@released
	*/
	IMPORT_C ~RUsb();

	/**
	Extract the version of the server providing the RUsb API

	
	@since	7.0

	@return	Version of the server

	@publishedPartner
	@released
	*/
	IMPORT_C TVersion Version() const;

	/**
	Connect the Handle to the Server
	Must be called before all other methods except Version()

	
	@since	7.0

	@return	KErrNone if successful, otherwise the error that occurred

	@publishedPartner
	@released
	*/
	IMPORT_C TInt Connect();

	/**
	Start the device's USB service. Should not be called if the USB services
	have already been started
	Note: Asynchonous Version, outcome returned when the status is completed
	
	
	@since	7.0
	@param	aStatus		Status to complete once the start operation has completed
	@capability NetworkControl

	@publishedPartner
	@released
	*/
	IMPORT_C void Start(TRequestStatus& aStatus);

	/**
	Cancels the pending start operation of the device's USB service.

	
	@since	7.0
	@capability NetworkControl

	@publishedPartner
	@released
	*/
	IMPORT_C void StartCancel();

	/**
	Stops the device's USB service. Should not be called if the USB services
	have not been started. This is the synchronous variant of this function.
	This function is deprecated- use the asynchronous version.
	
	
	@since	7.0
	@capability NetworkControl

	@publishedPartner
	@released
	*/
	IMPORT_C void Stop();

	/**
	Stops the device's USB service. Should not be called if the USB services
	have not been started. This is the asynchronous variant of this function.
	
	
	@since	7.0s
	@param	aStatus		Status to complete once the stop operation has completed
	@capability NetworkControl

	@publishedPartner
	@released
	*/
	IMPORT_C void Stop(TRequestStatus& aStatus);

	/**
	Cancels the pending stop operation of the device's USB service.

	
	@since	7.0s
	@capability NetworkControl

	@publishedPartner
	@released
	*/
	IMPORT_C void StopCancel();

	/**
	Get the current state of the device's USB service.

	
	@since	7.0s
	@param	aState	Set by the method to the current state of the USB service

	@return	KErrNone if successful, otherwise the error that occurred

	@publishedPartner
	@released
	*/
	IMPORT_C TInt GetServiceState(TUsbServiceState& aState);

	/**
	Request to be notified of a change in service state of the USB device. The
	request only completes when the service state changes.

	
	@since	7.0s
	@param	aState		State variable to be written to upon completion of the request
	@param	aStatus		Status to complete when required state change occurs

	@publishedPartner
	@released
	*/
	IMPORT_C void ServiceStateNotification(
		TUsbServiceState& aState,
		TRequestStatus& aStatus
	);

	/**
	Cancel the outstanding service state notification request.

	
	@since 7.0s

	@publishedPartner
	@released
	*/
	IMPORT_C void ServiceStateNotificationCancel();

	/**
	Gets the current device state (eg. powered, configured...).

	
	@since	7.0s
	@param	aState	Set by the method to the current state of the USB device
	@return	KErrNone if successful, otherwise the error that occurred

	@publishedPartner
	@released
	*/
	IMPORT_C TInt GetDeviceState(TUsbDeviceState& aState);

	/**
	Request to be notified of a change in state of the USB device.

	
	@since	7.0s
	@param	aStateMask	State mask of the states the client is interested in	
	@param	aState		State variable to be written to upon completion of the request
	@param	aStatus		Status to complete when required state change occurs

	@publishedPartner
	@released
	*/
	IMPORT_C void DeviceStateNotification(
		TUint aStateMask,
		TUsbDeviceState& aState,
		TRequestStatus& aStatus
	);

	/**
	Cancel the outstanding device state notification request.

	
	@since 7.0s

	@publishedPartner
	@released
	*/
	IMPORT_C void DeviceStateNotificationCancel();
	
	/**
	Try to start the device's USB service. It starts the current personality
	only if the service is in the idle state. Calling this API while the server
	is in any other states has no any effect on the service state.
	 
	Note: Asynchonous version, outcome returned when the status is completed
	
	@param aPersonalityId 	a personality id
	@param aStatus			Status to complete once the start operation has completed.
							It may be one of the following:	
								KErrNotSupported
								KErrAccessDenied
								KErrServerBusy
								KErrAbort
								KErrNone
	@capability NetworkControl
	@publishedPartner
	@released
	*/
	IMPORT_C void TryStart(TInt aPersonalityId, TRequestStatus& aStatus);

	/**
	Try to stop the device's USB service. It stops the service only if the serice
	is in the started state. Calling this API while the server is in the other states 
	has no any effect on the service state.

	Note: Asynchonous version, outcome returned when the status is completed
	
	@param	aStatus		Status to complete once the stop operation has completed.
						It may be one of the following:
								KErrNotSupported
								KErrAccessDenied
								KErrServerBusy
								KErrNone
	@capability NetworkControl
	@publishedPartner
	@released
	*/
	IMPORT_C void TryStop(TRequestStatus& aStatus);

	/**
	Cancels the interest of the pending operation of the device's USB service, 
	either starting or stopping. The pending request will run to the completion.
	The caller of this function receives a status of KErrCancel.
	
	@param  aMessageId	a message id to identify the request to be cancelled
	@publishedPartner
	@released
	*/
	IMPORT_C TInt CancelInterest(TUsbReqType aMessageId);

	/**
	Gets the textual description of the personality identified by the aPersonalityId.
	Caller is repsonsible for freeing up memories allocated to 
	aLocalizedPersonalityDescriptor.

	@param	aPersonalityId a personality id
	@param  aLocalizedPersonalityDescriptor a localize text string
	@return	KErrNone if successful, otherwise the error that occurred
	@publishedPartner
	@released
	*/
	IMPORT_C TInt GetDescription(TInt aPersonalityId, HBufC*& aLocalizedPersonalityDescriptor);

	/**
	Gets the current personality id of the device's USb service
	
	@param	aPersonalityId set to the current personality of USB device
	@return	KErrNone if successful, otherwise the error that occurred
	@publishedPartner
	@released
	*/
	IMPORT_C TInt GetCurrentPersonalityId(TInt& aPersonalityId);
	
	/**
	Gets supported classes by the given personality identified by the aPersonalityId

	@param aPersonalityId a personality id 
	@param aClassUids an array of class uids
	@return	KErrNone if successful, otherwise the error that occurred
	@publishedPartner
	@released
	*/
	IMPORT_C TInt GetSupportedClasses(TInt aPersonalityId, RArray<TUid>& aClassUids);

	/** 
	Queries the USB manager to determine if a given class is supported
	
	@param aPersonalityId a personality id
	@param aClassUid a class uid
	@param aSupported set upon return
	@return	KErrNone if successful, otherwise the error that occurred
	@publishedPartner
	@released
	*/
	IMPORT_C TInt ClassSupported(TInt aPersonalityId, TUid aClassUid, TBool& aSupported);
	
	/**
	Gets all supported personality ids of the device's USB service.

	@param	aPersonalityIds populated with all supported personality ids of the USB device
	@return	KErrNone if successful, otherwise the error that occurred
	@publishedPartner
	@released
	*/
	IMPORT_C TInt GetPersonalityIds(RArray<TInt>& aPersonalityIds);

	/**
	Marks the start of heap cell checking for the USB Manager. This function is only defined
	in debug builds.

	@return	KErrNone if successful, otherwise the error that occurred

	@publishedPartner
	@released
	*/
	IMPORT_C TInt __DbgMarkHeap();

	/**
	Checks that the number of allocated cells on the USB Manager's heap is correct. The USB
	Manager will be panicked if it is not. This function is only defined in debug builds.
	
	@param	aCount	The expected number of heap cells allocated
	@return	KErrNone if successful, otherwise the error that occurred

	@publishedPartner
	@released
	*/
	IMPORT_C TInt __DbgCheckHeap(TInt aCount);

	/**
	Marks the end of heap cell checking for the USB Manager. Checks that the number of heap
	cells allocated since the last __DbgMarkHeap() is aCount; the most common value to pass
	here is zero. This function is only defined in debug builds.

	@param	aCount	The expected number of heap cells allocated
	@return	KErrNone if successful, otherwise the error that occurred

	@publishedPartner
	@released
	*/
	IMPORT_C TInt __DbgMarkEnd(TInt aCount);

	/**
	Simulates memory allocation failure in the USB Manager. This function is only defined in
	debug builds.

	@param	aCount	The number of allocations after which memory allocation should fail
	@return	KErrNone if successful, otherwise the error that occurred

	@publishedPartner
	@released
	*/
	IMPORT_C TInt __DbgFailNext(TInt aCount);
	
	/**
	Functions below this point are deprecated and should not be used.
	*/

	/**
	Get the current state of the device's USB service. This function is deprecated and has been
	replaced by the GetServiceState function from version 7.0s onwards.

	
	@since	7.0
	@param	aState	Set by the method to the current state of the USB service

	@return	KErrNone if successful, otherwise the error that occurred

	@publishedPartner
	@deprecated
	*/
	IMPORT_C TInt GetCurrentState(TUsbServiceState& aState);

	/**
	Request to be notified of a change in state of the USB device. This function is deprecated
	and has been replaced by the DeviceStateNotification function from version 7.0s onwards.

	
	@since	7.0
	@param	aStateMask	State mask of the states the client is interested in	
	@param	aState		State variable to be written to upon completion of the request
	@param	aStatus		Status to complete when required state change occurs

	@publishedPartner
	@deprecated
	*/
	IMPORT_C void StateNotification(
		TUint aStateMask,
		TUsbDeviceState& aState,
		TRequestStatus& aStatus
	);

	/**
	Cancel the outstanding device state notification request. This function is deprecated and
	has been replaced by the DeviceStateNotificationCancel function from version 7.0s onwards.

	
	@since 7.0

	@publishedPartner
	@deprecated
	*/
	IMPORT_C void StateNotificationCancel();

	/**
	* Set or reset the mode in which current RUsb session operates.
	* Recent implementation assumes that there is the one and only application
	* which controls USBMAN, all other clients act as listeners to notification 
	* requests. Only application which was previously granted 'control' mode is 
	* allowed to reset it at later stage. Any calls from other applications will
	* generate an error.
	*
	* @param	aValue		True to inform USBMAN that Application wants to 'control' 
	*						USBMAN
	*						False otherwise
	*
	* @capability NetworkControl
	* @return	KErrNone		 if successful
	*			KErrAccessDenied otherwise
	*/
	IMPORT_C TInt SetCtlSessionMode(TBool aValue);

	/**
	* Cable watcher wants to assert Bus_request.
	* If ID-Pin is present this is an A-Device and this will result in an attempt 
	* to raise VBus. Second attempt to raise VBus causes KErrUsbOtgVbusAlreadyRaised
	* error.
	* If ID-Pin is absent this is a B-Device and this will result in an attempt to 
	* use SRP. Recently does nothing
	* 
	* When BusRequest() returns an error, VBus remains low until the errors are cleared by
	* BusDrop() or BusClearErr() calls 
	* 
	* @capability NetworkControl
	* @return KErrNone if successful, otherwise an error code returned by OTGDI
	*/
	IMPORT_C TInt BusRequest();

	/**
	* Cable watcher wants to assert Bus_request after SRP.
	* If ID-Pin is present this is an A-Device and this will result in an attempt 
	* to raise VBus
	* 
	* @capability NetworkControl
	* @return KErrNone if successful, otherwise an error code returned by OTGDI
	*/
	IMPORT_C TInt BusRespondSrp();

	/**
	* Cable watcher wants to clear the Bus Error after A_VBUS_ERR
	* Only works if ID-Pin is present (this is an A-Device) and there
	* has already been a bus erorr.
	* This will not result in any attempt to raise or drop VBus
	* 
	* @capability NetworkControl
	* @return KErrNone if successful, otherwise an error code returned by OTGDI
	*/
	IMPORT_C TInt BusClearError();

	/**
	* Cable watcher wants to drop VBus.
	* If ID-Pin is present this is an A-Device and this will result in stopping VBus 
	* power-up
	* 
	* @capability NetworkControl
	* @return KErrNone if successful, otherwise an error code returned by OTGDI
	*/
	IMPORT_C TInt BusDrop();
	
	/**
	* Register for Messages notifications
	* The request only completes when the new message arrives.
	* Calling this function the first time initializes Messages queue
	* 
	*
	* @param	aMessage	UI Message variable to be written to upon completion 
	* 						of the request
	* @param	aStatus		Status to complete when required state change occurs
	*			KErrNone	- if successful
	*			KErrInUse	- if there is another outstanding nofitication request 
	* 						  for the same session
	*			otherwise an error code returned by OTGDI or Host
	*/
	IMPORT_C void MessageNotification(TRequestStatus& aStatus, TInt& aMessage);

	/**
	* Cancel the outstanding Messages notification request.
	*/
	IMPORT_C void MessageNotificationCancel();

	/**
	* Register for Host Device Event notifications.
	* The request only completes when the host event occurs.
	* Calling this function the first time initializes Host Events queue
	*
	* @param	aStatus		Status to complete when required event occurs
	* 
	*			KErrNone	- if successful
	*			KErrInUse	- if there is another outstanding nofitication 
	*						  request for the same session
	*			otherwise an error code returned by FDF
	* @param	aDeviceInformation	device info to be written to upon completion 
	*								of the request
	*/
	IMPORT_C void HostEventNotification(TRequestStatus& aStatus,
										TDeviceEventInformation& aDeviceInformation);
	/**
	* Cancel the outstanding FDF Device Event notification request.
	*/
	IMPORT_C void HostEventNotificationCancel();

	/**
	* Enable Function Driver Loading.
	*
	* @capability NetworkControl
	* @return	KErrNone		 - if successful
	*			KErrNotSupported - if FDF is not included in current configuration
	*			otherwise an error code returned by FDF
	*/
	IMPORT_C TInt EnableFunctionDriverLoading();

	/**
	* Disable Function Driver Loading.
	*
	* @capability NetworkControl
	*/
	IMPORT_C void DisableFunctionDriverLoading();

	/**
	* Get Supported Languages from USB Device
	*
	* @param	aDeviceId	DeviceID of given device
	* @param	aLangIds	an array of language IDs supported by given device.
	*						These language IDs are supplied by USB-IF and are 
	*						different from standard Symbian TLanguage enumeration
	*
	* @return	KErrNone		 - if successful
	*			otherwise an error code returned by FDF
	*/
	IMPORT_C TInt GetSupportedLanguages(TUint aDeviceId, RArray<TUint>& aLangIds);

	/**
	* Get Manufacturer Descriptor
	*
	* @param	aDeviceId	DeviceID of given device
	* @param	aLangId		required language ID which is supplied by USB-IF and is 
	*						different from standard Symbian TLanguage enumeration
	* @param	aString		manufacturer descriptor value at output
	*
	* @return	KErrNone		 - if successful
	*			otherwise an error code returned by FDF
	*/
	IMPORT_C TInt GetManufacturerStringDescriptor(TUint aDeviceId, TUint aLangId, TName& aString);

	/**
	* Get Product Descriptor
	*
	* @param	aDeviceId	DeviceID of given device
	* @param	aLangId		required language ID which is supplied by USB-IF and is 
	*						different from standard Symbian TLanguage enumeration
	* @param	aString		product descriptor value at output
	*
	* @return	KErrNone		 - if successful
	*			otherwise an error code returned by FDF
	*/
	IMPORT_C TInt GetProductStringDescriptor(TUint aDeviceId, TUint aLangId, TName& aString);
		
	/**
	* Retrieve Otg Descriptor for device which has given device Id.
	* Currently TOtgDescriptor has following fields:
	*  - HNP supported
	*  - SRP supported
	* An OTG device should support them both. 
	*
	* @param	aDeviceId	DeviceID of given device
	* @param	aDescriptor OTG descriptor value at output
	*
	* @return	KErrNone		 - if successful
	*			otherwise an error code returned by FDF
	*/
	IMPORT_C TInt GetOtgDescriptor(TUint aDeviceId, TOtgDescriptor& aDescriptor);
	
	/**
	Simulates memory allocation in the USB Manager. This function is only defined in
	debug builds.

	@return	KErrNone if successful, otherwise the error that occurred

	@publishedPartner
	@released
	*/
	IMPORT_C TInt __DbgAlloc();
	
	/**
	Informs USB Manager that the client would like to initialte USB session.
	On A-Device, it results in sending a notification to 'controller' application
	On B-Device, it may trigger either SRP or HNP sequence depending on the state of VBus 

	@return	KErrNone if successful, otherwise the error that occurred

	@publishedPartner
	@released
	*/
	IMPORT_C TInt RequestSession();
	

	/**
	Gets the property of the personality identified by the aPersonalityId.

	@param  aPersonalityId a personality id
	@return the personality property
	@publishedPartner
	@released
	*/
	IMPORT_C TInt GetPersonalityProperty(TInt aPersonalityId, TUint32& aProperty);

	/**
	Gets the detailed textual description of the personality identified by the aPersonalityId.
	Caller is repsonsible for freeing up memories allocated to 
	aLocalizedPersonalityDescriptor.

	@param  aPersonalityId a personality id
	@param  aLocalizedPersonalityDescriptor a localize text string
	@return KErrNone if successful, otherwise the error that occurred
	@publishedPartner
	@released
	*/
	IMPORT_C TInt GetDetailedDescription(TInt aPersonalityId, HBufC*& aLocalizedPersonalityDescriptor);
	
private:
	/** 
	Used to register device state notifications.
	*/
	TPckg<TUint32> iDeviceStatePkg;

	/** 
	Used to register service state notifications.
	*/
	TPckg<TUint32> iServiceStatePkg;

	/**
	Used to register OTG/Host message notifications.
	*/
	TPckg<TUint32> iMessagePkg;

	/**
	Used to register Host state notifications.
	*/
	TPckg<TDeviceEventInformation> iHostPkg;
	};

#endif //__USBMAN_H__
