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
* Description:  Utility library for reading simple configuration
*               property files containing Key=Value lines.
*
*/

#ifndef __HTICFG_H__
#define __HTICFG_H__

// INCLUDES
#include <e32base.h>

// CONSTANTS

// Parameter lengths
const static TInt KMaxParameterNameLength = 64;
const static TInt KMaxParameterValueLength = 64;
const static TInt KMaxParameterLength = KMaxParameterNameLength + 1 + KMaxParameterValueLength;

// Configuration file constants
#define KCfgNewLine (TChar)'\n'
#define KCfgSeparator (TChar)'='
#define KCfgComment (TChar)'#'

// FORWARD DECLARATIONS
class CDesC8ArrayFlat;

// CLASS DECLARATION
class CHtiCfg: public CBase
    {
public:

    /**
    * Create instance of configuration class.
    * @return Configuration class instance.
    */
    IMPORT_C static CHtiCfg* NewL();

    /**
    * Create instance of configuration class leaving it to cleanup stack.
    * @return Configuration class instance.
    */
    IMPORT_C static CHtiCfg* NewLC();

    /**
    * Destructor.
    */
    IMPORT_C virtual ~CHtiCfg();

    /**
    * Loads the configuration file and reads the values to iCfgParameters.
    * Drive letter is not specified. File will be searched from all drives.
    * @param aCfgFilePath absolute path (without drive letter) to the directory
    *                     from where to search the configuration file
    * @param aCfgFileName configuration filename.
    */
    IMPORT_C void LoadCfgL( const TDesC& aCfgFilePath,
                            const TDesC& aCfgFileName );

    /**
    * Reads the values from iCfgParameters and saves them to configuration file.
    * Drive letter is not specified. File will be searched from all drives and
    * if not found file will be crated to c-drive.
    * @param aCfgFilePath directory from where to search the configuration file
    * @param aCfgFileName configuration filename.
    */
    IMPORT_C void SaveCfgL( const TDesC& aCfgFilePath,
                            const TDesC& aCfgFileName );

    /**
    * Gets a parameter value as descriptor.
    * @param aName name of the parameter whose value to get
    * @return Parameter value.
    */
    IMPORT_C TPtrC8 GetParameterL( const TDesC8& aName );

    /**
    * Gets a numerical parameter value as integer.
    * @param aName name of the parameter whose value to get
    * @return Parameter value.
    */
    IMPORT_C TInt GetParameterIntL( const TDesC8& aName );

    /**
    * Sets a parameter
    * @param aName name of the parameter whose value to set
    * @param aValue parameter value to set
    * @return error code.
    */
    IMPORT_C TInt SetParameterL( const TDesC8& aName,  const TDesC8& aValue );

    /**
    * Removes a parameter
    * @param aName name of the parameter to remove
    * @return error code.
    */
    IMPORT_C TInt RemoveParameterL( const TDesC8& aName );

private:

    /**
    * Constructor.
    */
    CHtiCfg();

    /**
    * Second phase construction.
    */
    void ConstructL();


private: // data

    // array of key=value lines
    CDesC8ArrayFlat* iCfgParameters;

    };

#endif // __HTICFG_H__
