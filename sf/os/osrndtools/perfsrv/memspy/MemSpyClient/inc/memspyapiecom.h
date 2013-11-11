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
* Contains the declaration of  CMemSpyProcess class 
*/

#ifndef MEMSPYAPIECOM_H
#define MEMSPYAPIECOM_H

// System includes
#include <e32base.h>
#include <f32file.h>

class TMemSpyEComCategoryData;
class TMemSpyEComInterfaceData;
class TMemSpyEComImplementationData;

NONSHARABLE_CLASS( CMemSpyApiEComCategory ) : public CBase
	{
public:
	IMPORT_C ~CMemSpyApiEComCategory();
	
	static CMemSpyApiEComCategory* NewL(const TMemSpyEComCategoryData& aData);

	static CMemSpyApiEComCategory* NewLC(const TMemSpyEComCategoryData& aData);
	
public:
	IMPORT_C TUid Id() const;
	
	IMPORT_C const TDesC& Name() const;
	
	IMPORT_C TInt InterfaceCount() const;
	
private:
	void ConstructL(const TMemSpyEComCategoryData& aData);
		
private:
	TMemSpyEComCategoryData *iData;
};

NONSHARABLE_CLASS( CMemSpyApiEComInterface ) : public CBase
    {
public:
    IMPORT_C ~CMemSpyApiEComInterface();
    
    static CMemSpyApiEComInterface* NewL(const TMemSpyEComInterfaceData& aData);

    static CMemSpyApiEComInterface* NewLC(const TMemSpyEComInterfaceData& aData);
    
public:
    IMPORT_C TUid Id() const;
    
    IMPORT_C TUid CategoryId() const;
    
    IMPORT_C const TDesC& Name() const;
    
    IMPORT_C TInt ImplementationCount() const;
    
private:
    void ConstructL(const TMemSpyEComInterfaceData& aData);
        
private:
    TMemSpyEComInterfaceData *iData;
};

NONSHARABLE_CLASS( CMemSpyApiEComImplementation ) : public CBase 
    {
public:
    IMPORT_C ~CMemSpyApiEComImplementation();
    
    static CMemSpyApiEComImplementation* NewL(const TMemSpyEComImplementationData& aData);

    static CMemSpyApiEComImplementation* NewLC(const TMemSpyEComImplementationData& aData);
    
public:
    IMPORT_C TUid ImplementationUid() const;
        
    IMPORT_C const TDesC& Name() const;
    
    IMPORT_C TInt Version() const;
    
    IMPORT_C const TDesC& DataType() const;
        
    IMPORT_C const TDesC& OpaqueData() const;
    
    IMPORT_C TDriveUnit Drive() const;
        
    IMPORT_C TBool RomOnly() const;
        
    IMPORT_C TBool RomBased() const;
        
    IMPORT_C TVendorId VendorId() const;
        
    IMPORT_C TBool Disabled() const;
    
private:
    
    void ConstructL(const TMemSpyEComImplementationData& aData);
        
private:
    TMemSpyEComImplementationData *iData;
};


#endif // MEMSPYAPIECOM_H
