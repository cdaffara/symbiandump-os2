/*
* Copyright (c) 2002-2004 Nokia Corporation and/or its subsidiary(-ies).
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
* Description:         This class is a utitlity class for conversion of Vodafone
*                pictograph.
*
*/










#ifndef CNVPICTOGRAPHSHARED_H
#define CNVPICTOGRAPHSHARED_H

//  INCLUDES
#include <e32std.h>
#include <charconv.h>
#include <convutils.h>
#include "PictographObserver.h"


// CONSTANTS
// Count of Vodafone pictograph's pages
const TInt KCountOfPictoPages(6);


// CLASS DECLARATION

/**
*  This class is a utitlity class for conversion of Vodafone pictograph.
*  The conversion data are in each CnvPctograph1 - 6 dll.
*  CnvPictographShared only to be used by ISO2022Jp.cpl, SHIFTJIS_SHARED.dll or
*  something. For example, when CCnvCharacterSetConverter::ConvertFromUnicode()
*  is called, and ConvertFromUnicode() of ISO2022Jp.cpl is called. After that,
*  ISO2022Jp.cpl sets some parameters for the conversion. At that time,
*  CnvPictographShared::SetCharacterSetsForPictograph() is called.
*  In a nut shell, CnvPictographShared is chaind from Japanese conversion
*  plug-ins.
*
*  @lib CnvPictograph1.dll
*  @lib CnvPictograph2.dll
*  @lib CnvPictograph3.dll
*  @lib CnvPictograph4.dll
*  @lib CnvPictograph5.dll
*  @lib CnvPictograph6.dll
*  @since 2.6
*/
class CnvPictographShared
    {
    public:
        /**
        * It sets parameters to SCharacterSet array.
        * It called in ConvertFromUnicode().
        * It makes possible that pictograph conversion is called from other
        * conversion plug-ins.
        * @since 2.6
        * @param aArrayOfCharacterSets The conversion table array
        */
        static void SetCharacterSetsForPictograph(
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
        static void SetCharacterSetsForPictograph(
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
        static void SetMethodsForPictograph(
            RArray<CnvUtilities::SMethod>& aArrayOfMethods,
            TOriginalCharset aOriginalCharset);
        /**
        * It sets parameters to SState array.
        * It called in ConvertToUnicode().
        * It makes possible that pictograph conversion is called from other
        * conversion plug-ins.
        * @since 2.6
        * @param aArrayOfMethods The conversion table array
        */
        static void SetStatesForPictograph(
            RArray<CnvUtilities::SState>& aArrayOfStates);

    private:
        /**
        * It converts from an intermediate buffer to a pictograph code.
        * After that, it adds shift-in code(0x0F).
        * @since 2.6
        * @param aStartPositionInDescriptor The start postion for checking
        * @param aDescriptor The descripter has intermidiate buffer.
        * @param aNumberOfCharactersThatDroppedOut The number of characters
        *                                          that it's dropped out
        */
        static void ConvertToPictograph1InPlace(
            TInt aStartPositionInDescriptor,
            TDes8& aDescriptor, TInt& aNumberOfCharactersThatDroppedOut);
        /**
        * It converts from an intermediate buffer to a pictograph code.
        * After that, it adds shift-in code(0x0F).
        * @since 2.6
        * @param aStartPositionInDescriptor The start postion for checking
        * @param aDescriptor The descripter has intermidiate buffer.
        * @param aNumberOfCharactersThatDroppedOut The number of characters
        *                                          that it's dropped out
        */
        static void ConvertToPictograph2InPlace(
            TInt aStartPositionInDescriptor,
            TDes8& aDescriptor, TInt& aNumberOfCharactersThatDroppedOut);
        /**
        * It converts from an intermediate buffer to a pictograph code.
        * After that, it adds shift-in code(0x0F).
        * @since 2.6
        * @param aStartPositionInDescriptor The start postion for checking
        * @param aDescriptor The descripter has intermidiate buffer.
        * @param aNumberOfCharactersThatDroppedOut The number of characters
        *                                          that it's dropped out
        */
        static void ConvertToPictograph3InPlace(
            TInt aStartPositionInDescriptor,
            TDes8& aDescriptor, TInt& aNumberOfCharactersThatDroppedOut);
        /**
        * It converts from an intermediate buffer to a pictograph code.
        * After that, it adds shift-in code(0x0F).
        * @since 2.6
        * @param aStartPositionInDescriptor The start postion for checking
        * @param aDescriptor The descripter has intermidiate buffer.
        * @param aNumberOfCharactersThatDroppedOut The number of characters
        *                                          that it's dropped out
        */
        static void ConvertToPictograph4InPlace(
            TInt aStartPositionInDescriptor,
            TDes8& aDescriptor, TInt& aNumberOfCharactersThatDroppedOut);
        /**
        * It converts from an intermediate buffer to a pictograph code.
        * After that, it adds shift-in code(0x0F).
        * @since 2.6
        * @param aStartPositionInDescriptor The start postion for checking
        * @param aDescriptor The descripter has intermidiate buffer.
        * @param aNumberOfCharactersThatDroppedOut The number of characters
        *                                          that it's dropped out
        */
        static void ConvertToPictograph5InPlace(
            TInt aStartPositionInDescriptor,
            TDes8& aDescriptor, TInt& aNumberOfCharactersThatDroppedOut);
        /**
        * It converts from an intermediate buffer to a pictograph code.
        * After that, it adds shift-in code(0x0F).
        * @since 2.6
        * @param aStartPositionInDescriptor The start postion for checking
        * @param aDescriptor The descripter has intermidiate buffer.
        * @param aNumberOfCharactersThatDroppedOut The number of characters
        *                                          that it's dropped out
        */
        static void ConvertToPictograph6InPlace(
            TInt aStartPositionInDescriptor,
            TDes8& aDescriptor, TInt& aNumberOfCharactersThatDroppedOut);

        /**
        * It returns a number of bytes that the conversion to pictograph is
        * possible in the descriptor.
        * @since 2.6
        * @param aDescriptor A descriptor has any character code.
        * @return Number of bytes that the conversion is possible
        */
        static TInt NumberOfBytesAbleToConvertSJisDirectMap(const TDesC8& aDescriptor);

        /**
        * It returns a number of bytes that the conversion to pictograph is
        * possible in the descriptor.
        * @since 2.6
        * @param aDescriptor A descriptor has any character code.
        * @return Number of bytes that the conversion is possible
        */
        static TInt NumberOfBytesAbleToConvertToEucJpDirectMap1(const TDesC8& aDescriptor);

        /**
        * It returns a number of bytes that the conversion to pictograph is
        * possible in the descriptor.
        * @since 2.6
        * @param aDescriptor A descriptor has any character code.
        * @return Number of bytes that the conversion is possible
        */
        static TInt NumberOfBytesAbleToConvertToEucJpDirectMap2(const TDesC8& aDescriptor);

        /**
        * It returns a number of bytes that the conversion to pictograph is
        * possible in the descriptor.
        * @since 2.6
        * @param aDescriptor A descriptor has any character code.
        * @return Number of bytes that the conversion is possible
        */
        static TInt NumberOfBytesAbleToConvertToPicto1(
            const TDesC8& aDescriptor);
        /**
        * It returns a number of bytes that the conversion to pictograph is
        * possible in the descriptor.
        * @since 2.6
        * @param aDescriptor A descriptor has any character code.
        * @return Number of bytes that the conversion is possible
        */
        static TInt NumberOfBytesAbleToConvertToPicto2(
            const TDesC8& aDescriptor);
        /**
        * It returns a number of bytes that the conversion to pictograph is
        * possible in the descriptor.
        * @since 2.6
        * @param aDescriptor A descriptor has any character code.
        * @return Number of bytes that the conversion is possible
        */
        static TInt NumberOfBytesAbleToConvertToPicto3(
            const TDesC8& aDescriptor);
        /**
        * It returns a number of bytes that the conversion to pictograph is
        * possible in the descriptor.
        * @since 2.6
        * @param aDescriptor A descriptor has any character code.
        * @return Number of bytes that the conversion is possible
        */
        static TInt NumberOfBytesAbleToConvertToPicto4(
            const TDesC8& aDescriptor);
        /**
        * It returns a number of bytes that the conversion to pictograph is
        * possible in the descriptor.
        * @since 2.6
        * @param aDescriptor A descriptor has any character code.
        * @return Number of bytes that the conversion is possible
        */
        static TInt NumberOfBytesAbleToConvertToPicto5(
            const TDesC8& aDescriptor);
        /**
        * It returns a number of bytes that the conversion to pictograph is
        * possible in the descriptor.
        * @since 2.6
        * @param aDescriptor A descriptor has any character code.
        * @return Number of bytes that the conversion is possible
        */
        static TInt NumberOfBytesAbleToConvertToPicto6(
            const TDesC8& aDescriptor);
        /**
        * It's a common function for checking the number of bytes that
        * the conversion to pictograph
        * @since 2.6
        * @param aDescriptor A descriptor has any character code.
        * @param aIntermidiateCode An intermidiate buffer
        * @return Number of bytes that the conversion is possible
        */
        static TInt NumberOfBytesAbleToConvertToPictograph(
            const TDesC8& aDescriptor, TInt aIntermidiateCode);
        /**
        * It converts the string which includes a pictograph's escape sequence
        * to unicode.
        * @since 2.6
        * @param aDescriptor It includes a pictograph's escape sequence
        */
        static void ConvertToPictographFromUnicode(TDes8& aDescriptor);

        /**
        * It converts the string which has Unicode with pictograph to EUC-jp
        * with direct mapped pictograph
        * @since 2.6
        * @param aDescriptor It includes a pictograph's escape sequence
        */
        static void ConvertToPictographFromEucJpPackedInPlace(TDes8& aDescriptor);
        /**
        * It appends a shift-in code to the descriptor.
        * @since 2.6
        * @param aDescriptor It's append a shift-in code.
        * @param aNumberOfCharactersThatDroppedOut The number of characters
        *                                          that it's dropped out
        */
        static void AppendShiftIn(TDes8& aDescriptor,
            TInt& aNumberOfCharactersThatDroppedOut);
    };

#endif // CNVPICTOGRAPHSHARED_H

// End of File
