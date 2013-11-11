/*
* Copyright (c) 2002 Nokia Corporation and/or its subsidiary(-ies).
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
* Description:         This header file defines functions for conversion of pictograph.*
*/













#ifndef PICTOGRAPHOBSERVER_H

#define PICTOGRAPHOBSERVER_H

#include <e32def.h>

#include <e32std.h>

#include <convutils.h>





enum TOriginalCharset

    {

    ECharsetShiftJis,

    ECharsetEucJp,

    ECharsetOthers

    };





_LIT(KLitPictographConverterPath, "z:\\system\\libs\\");
#ifndef KFeatureIdJapanesePicto
#define KFeatureIdJapanesePicto 1079
#endif
_LIT(KLitPictographConverter, "CnvPictographShared.dll");







/**

* It sets parameters to SCharacterSet array.

* It called in ConvertFromUnicode().

* It makes possible that pictograph conversion is called from other

* conversion plug-ins.

* @since 2.6

* @param aArrayOfCharacterSets The conversion table array

*/

IMPORT_C void SetCharacterSetsForPictograph(

    RArray<CnvUtilities::SCharacterSet>& aArrayOfCharacterSets);



/**

* It sets parameters to SCharacterSet array.

* It called in ConvertFromUnicode().

* It makes possible that pictograph conversion is called from other

* conversion plug-ins.

* @since 2.6

* @param aArrayOfCharacterSets The conversion table array

* @param aOriginalCharset The character set

*/

IMPORT_C void SetCharacterSetsForPictograph(

    RArray<CnvUtilities::SCharacterSet>& aArrayOfCharacterSets,

    TOriginalCharset aOriginalCharset);



/**

* It sets parameters to SMethod array.

* It called in ConvertToUnicode().

* It makes possible that pictograph conversion is called from other

* conversion plug-ins.

* @since 2.6

* @param aArrayOfMethods The conversion table array

* @param aOriginalCharset The character set

*/

IMPORT_C void SetMethodsForPictograph(

    RArray<CnvUtilities::SMethod>& aArrayOfMethods, TOriginalCharset aOriginalCharset);



/**

* It sets parameters to SState array.

* It called in ConvertToUnicode().

* It makes possible that pictograph conversion is called from other

* conversion plug-ins.

* @since 2.6

* @param aArrayOfMethods The conversion table array

*/

IMPORT_C void SetStatesForPictograph(RArray<CnvUtilities::SState>& aArrayOfStates);



#endif // PICTOGRAPHOBSERVER_H

