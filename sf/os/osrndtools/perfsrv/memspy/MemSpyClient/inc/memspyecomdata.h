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
* Contains the declaration of  TMemSpyProcessData class
*/

#ifndef MEMSPYECOMDATA_H
#define MEMSPYECOMDATA_H

// TMemSpyProcess data class holds data to be sent to the UI
class TMemSpyEComCategoryData 
	{	
public:
	//constructor & destructor
	inline TMemSpyEComCategoryData ()
		: iInterfaceCount(0)
		{
		}
	
public:
    TUid iId;        
    TFullName iName;
    TInt iInterfaceCount;
	};

class TMemSpyEComInterfaceData 
    {   
public:
    //constructor & destructor
    inline TMemSpyEComInterfaceData ()
        : iImplementationCount(0)
        {
        }
    
public:
    TUid iId;
    TUid iCategoryId;
    TFullName iName;
    TInt iImplementationCount;
    };

class TMemSpyEComImplementationData 
    {   
public:
    TUid iImplementationUid;
    TFullName iName;
    TInt iVersion;
    TFullName iDataType;
    TFullName iOpaqueData;
    TDriveUnit iDrive;
    TBool iRomOnly;
    TBool iRomBased;
    TVendorId iVendorId;    
    TBool iDisabled;
    };

#endif // MEMSPYECOMDATA_H
