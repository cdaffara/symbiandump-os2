/**
* Copyright (c) 2004-2010 Nokia Corporation and/or its subsidiary(-ies).
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
* Implements a utility class which holds the information about a USB descriptor
* 
*
*/



/**
 @file
*/

#ifndef __CPERSONALITY_H__
#define __CPERSONALITY_H__

#include <e32std.h>
#include "CUsbDevice.h"


/**
 * The CPersonalityConfigurations class keep all information of the table
 * "Configuration" of USB cenrep  
 */

NONSHARABLE_CLASS(CPersonalityConfigurations) : public CBase
    {
public:
    /*USB Classes*/
    class TUsbClasses
         {
     public:
         TUid    iClassUid;
         TInt    iFeatureId; // Indicates the class is configurable or not.
         };
public:
    void  SetPersonalityId(TInt aPersonalityId);  
    void  SetConfigId(TInt aConfigId);  
    inline const RArray<CPersonalityConfigurations::TUsbClasses>& Classes() const;
    void AppendClassesL(const TUsbClasses& aClasses );
    ~CPersonalityConfigurations();
    
private: 
    TInt                        iPersonalityId; /*Personality Id*/
    TInt                        iConfigId; /*Configuration Id*/
    RArray<TUsbClasses>         iClasses; /*Class array*/
    };

NONSHARABLE_CLASS(CPersonality) : public CBase
	{
public:
	static CPersonality* NewL();
	~CPersonality();

	const RArray<CPersonalityConfigurations::TUsbClasses>& SupportedClasses() const;
	TBool ClassSupported(TUid aClassId) const;
	static TInt Compare(const TUid&  aFirst, const TUid& aSecond);

	inline TInt Version() const;
	void SetVersion(TInt version);
	
	inline TUint32 Property() const;
    void SetProperty(TUint32 aProperty);
    
    inline TUint8 DeviceClass() const;
    void SetDeviceClass(TUint8 aDeviceClass);    
    
    inline TUint8 DeviceSubClass() const;
    void SetDeviceSubClass(TUint8 aDeviceSubClass);

    inline TUint8 DeviceProtocol() const;
    void SetDeviceProtocol(TUint8 aDeviceProtocol);    
    
    inline TUint8 NumConfigurations() const;
    void SetNumConfigurations(TUint8 aNumConfigurations);     
 
    inline TUint16 ProductId() const;
    void SetProductId(TUint16 aProductId);    
    
    inline TInt FeatureId() const;
    void SetFeatureId(TInt aFeatureId);       

    inline TUint16 BcdDevice() const;
    void SetBcdDevice(TUint16 aBcdDevice);    
    
    inline TInt PersonalityId() const;
    void SetPersonalityId(TInt aPersonalityId);  
    
    inline const TDesC* Description() const;
    void SetDescription(const TDesC* aDescription);  
    
    inline const RPointerArray<CPersonalityConfigurations>& PersonalityConfigs() const;
    void AppendPersonalityConfigsL(const CPersonalityConfigurations *aPersonalityConfig );
    
private:
	CPersonality();
	void ConstructL();
 
private:
	TInt    iVersion;
    TUint32 iProperty;	
    TUint8  iDeviceClass;	
    TUint8  iDeviceSubClass;
    TUint8  iDeviceProtocol;
    TUint8  iNumConfigurations;
    TUint16 iProductId;
    TInt    iFeatureId;
    TUint16 iBcdDevice;
    TInt    iPersonalityId;    // personality id
    HBufC*   iDescription;     // Own this Personality's description
    RPointerArray<CPersonalityConfigurations>   iPersonalityConfigs;  
	};



#include "CPersonality.inl"
	
#endif // __CPERSONALITY_H__

