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
* Implements part of UsbMan USB Class Framework.
* All USB classes to be managed by UsbMan must derive
* from this class.
*
*/

/**
 @file
*/

#ifndef __CUSBCLASSCONTROLLERBASE_H__
#define __CUSBCLASSCONTROLLERBASE_H__

#include <e32base.h>
#include <usbstates.h>

class MUsbClassControllerNotify;
class TUsbDescriptor;

class CUsbClassControllerBase : public CActive
/** Base class for Class Controllers.

  @publishedPartner
  @released 
 */
	{
public:
	/** Owner
		@return The owner of the class controller.
		*/
	IMPORT_C MUsbClassControllerNotify& Owner() const;

	/** State
		@return The service state.
		*/
	IMPORT_C TUsbServiceState State() const;

	/** StartupPriority
		@return The relative priority of this class controller.
		*/
	IMPORT_C TInt StartupPriority() const;

	/** Compare
		Static function to compare two class controllers on the basis of their 
		startup priorities.
		@param aFirst First class controller.
		@param aSecond Second class controller.
		@return Result of comparison of two class controllers.
		*/
	IMPORT_C static TInt Compare(const CUsbClassControllerBase&  aFirst, 
		const CUsbClassControllerBase& aSecond);
		
public: // Functions derived from CActive to be implemented by derived classes.
	/** RunL
		Framework function. Class controllers are Active Objects, and may if 
		relevant use that to register interfaces asynchronously.
		*/
	virtual void RunL() = 0;
	
	/** DoCancel
		Framework function. 
		*/
	virtual void DoCancel() = 0;
	
	/** RunError
		@param aError The error with which RunL left.
		@return Error. 
		*/
	virtual TInt RunError(TInt aError) = 0;

public:
	/** Destructor
	*/
	IMPORT_C virtual ~CUsbClassControllerBase();

public:
	/** Start
		Called by the server to get this class controller to register its 
		interfaces.
		@param aStatus TRequestStatus to signal completion of the request.
		*/
	virtual void Start(TRequestStatus& aStatus) = 0;

	/** Stop
		Called by the server to get this class controller to deregister its 
		interfaces.
		@param aStatus TRequestStatus to signal completion of the request.
		*/
	virtual void Stop(TRequestStatus& aStatus) = 0;

	/** GetDescriptorInfo
		Returns information on the class controller's descriptors.
		@param aDescriptorInfo Structure to return information in.
		*/
	virtual void GetDescriptorInfo(TUsbDescriptor& aDescriptorInfo) const = 0;

protected:
	/** Constructor
		@param aOwner Owner.
		@param aStartupPriority Relative startup priority of this class controller.
		*/
	IMPORT_C CUsbClassControllerBase(MUsbClassControllerNotify& aOwner, TInt aStartupPriority);

protected:
	/**
		The relative priority of this class controller. The class controllers 
		are sorted using their priorities to determine what order to start 
		them in. 
		*/
	const TInt iStartupPriority;

	/**
		The current service state. This must be kept up-to-date depending on 
		whether this class controller's interfaces are registered or not.
		*/
	TUsbServiceState iState;

	/**
		Owner.
		*/
	MUsbClassControllerNotify& iOwner;
	};

#endif //__CUSBCLASSCONTROLLERBASE_H__

