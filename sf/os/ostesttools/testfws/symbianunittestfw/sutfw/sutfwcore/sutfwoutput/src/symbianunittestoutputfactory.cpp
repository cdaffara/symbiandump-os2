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

#include "symbianunittestoutputfactory.h"
#include "symbianunittestoutputastext.h"
#include "symbianunittestoutputasxml.h"
#include "symbianunittestoutputashtml.h"
    
// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
//
CSymbianUnitTestOutputFormatter* SymbianUnitTestOutputFactory::CreateOutputLC(
    const TDesC& aFileNameWithoutExtension,
    const TDesC& aFileExtension )
    {    
    CSymbianUnitTestOutputFormatter* outputFormatter = NULL;

    if ( aFileExtension.CompareF( KTextOutput ) == 0 )
        {
        outputFormatter = 
            CSymbianUnitTestOutputAsText::NewL( aFileNameWithoutExtension );
        }
    else if ( aFileExtension.CompareF( KXmlOutput ) == 0 )
        {
        outputFormatter = 
            CSymbianUnitTestOutputAsXml::NewL( aFileNameWithoutExtension );
        }
    else if ( aFileExtension.CompareF( KHtmlOutput ) == 0 ) 
        {
        outputFormatter = 
            CSymbianUnitTestOutputAsHtml::NewL( aFileNameWithoutExtension );
        }
    else
        {
        User::Leave( KErrNotSupported );
        }
    
    CleanupStack::PushL( outputFormatter );
    
    return outputFormatter;
    }
