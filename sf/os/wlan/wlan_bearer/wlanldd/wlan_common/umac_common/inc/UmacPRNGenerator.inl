/*
* Copyright (c) 2002-2007 Nokia Corporation and/or its subsidiary(-ies).
* All rights reserved.
* This component and the accompanying materials are made available
* under the terms of the License "Eclipse Public License v1.0"
* which accompanies this distribution, and is available
* at the URL "http://www.eclipse.org/legal/epl-v10.html".
*
* Initial Contributors:
* Nokia Corporation - initial contribution.
*
* Contributors:
*
* Description:   Implementation of WlanPRNGenerator inline methods.
*
*/

/*
* %version: 5 %
*/

extern TUint32 random( void );

inline WlanPRNGenerator::WlanPRNGenerator() :
    // to keep static code analysers happy. Real initialization below    
    iValue( 0 )
    {
    /// initiliase seed 
    iValue = random();
    }

inline TUint32 WlanPRNGenerator::Generate()
    {
    iValue = ( EMultiplier * iValue ) % EModulus;
    return iValue;
    }
