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
* Description:  Declaration of the class TATDriveInfo.
*
*/



#ifndef ATDRIVEINFO_H
#define ATDRIVEINFO_H

#include <f32file.h>

// The path of the storage file
#ifdef __WINS__
_LIT( KATDataFilePath, ":\\logs\\analyzetool\\" );
#else
_LIT( KATDataFilePath, ":\\analyzetool\\" );
_LIT( KATDataFilePath2, ":\\data\\analyzetool\\" );
#endif

/**
*  Check what drives exists and creates file full path.
*/
class TATDriveInfo
    {    
    public: // Constructors
        
        /**
        * C++ default constructor.
        */
        TATDriveInfo();

    public: // New functions

        /**
        * Create the file full path.
        * @param aPath Full path.
        * @param aFileName Filename.
        * @param aFs A handle to a file server.
        * @return KErrNone or KErrAlreadyExists, if successful; 
        *   Otherwise one of the other system wide error codes.
        */
        static TInt CreatePath( TDes& aPath, const TDesC& aFileName, const TDesC& aFilePath, RFs& aFs );   
    
    private: // New functions
    
        /**
        * Get the available drive character.
        * @param aDrive The drive letter.
        * @param aDriveNumber The drive number.
        * @param aFs A handle to a file server.
        * @param aDriveType Drive type.
        * @return KErrNone, if successful; otherwise KErrNotFound
        */
        static TInt GetDrive( TChar& aDrive, TInt& aDriveNumber, 
                RFs& aFs, const TUint aDriveType );  
        
    };

#endif // ATDRIVEINFO_H

// End of File

