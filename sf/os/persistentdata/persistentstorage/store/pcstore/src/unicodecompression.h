// Copyright (c) 2006-2009 Nokia Corporation and/or its subsidiary(-ies).
// All rights reserved.
// This component and the accompanying materials are made available
// under the terms of "Eclipse Public License v1.0"
// which accompanies this distribution, and is available
// at the URL "http://www.eclipse.org/legal/epl-v10.html".
//
// Initial Contributors:
// Nokia Corporation - initial contribution.
//
// Contributors:
//
// Description:
// Classes defined in this file are used for Unicode compression and decompression.
// Their code is borrowed from Symbian, only with some changes such as the "Panic" function 
// is changed to exit the program. The Symbian coding standard will be kept in the code.
// 
//


#if !defined(__UNICODECOMPRESSION_H__)
#define __UNICODECOMPRESSION_H__

#include <stdio.h>
#include <pcstore/pcstoredef.h>
#include <pcstore/storereadstream.h>
#include <pcstore/storewritestream.h>
#include "pcstoreconst.h"

namespace PCStore
{
/**
@internalComponent
*/
class TUnicodeCompressionState
    {
    public:
    TUnicodeCompressionState();
    void Reset();
    static TInt StaticWindowIndex(TUint16 aCode);
    static TInt DynamicWindowOffsetIndex(TUint16 aCode);
    static TUint32 DynamicWindowBase(TInt aOffsetIndex);
    static TBool EncodeAsIs(TUint16 aCode);

    enum TPanic
        {
        EUnhandledByte,         // expander code fails to handle all possible byte codes
        ENotUnicode,            // expander can't handle Unicode values outside range 0x0..0x10FFFF;
                                // that is, 16-bit codes plus 32-bit codes that can be expressed using
                                // 16-bit surrogates
        EOutputBufferOverflow   // output buffer is not big enough
        };

    static void Panic(TPanic aPanic);

    protected:

    enum
        {
        EStaticWindows = 8,
        EDynamicWindows = 8,
        ESpecialBases = 7
        };

    TBool iUnicodeMode;                                 // TRUE if in Unicode mode as opposed to single-byte mode
    TUint32 iActiveWindowBase;                          // base of the active window - bases are 32-bit because they
                                                        // can be set to the surrogate area, which represents codes
                                                        // from 0x00010000 to 0x0010FFFF - planes 1-16 of ISO-10646.
    static const TUint32 iStaticWindow[EStaticWindows]; // bases of the static windows
    static const TUint32 iDynamicWindowDefault[EDynamicWindows];    // default bases of the dynamic windows
    static const TUint16 iSpecialBase[ESpecialBases];   // bases for window offsets F9..FF

    TUint32 iDynamicWindow[EDynamicWindows];            // bases of the dynamic windows
    TInt iUnicodeWords;                                 // Unicode words processed; read by compressor, written by expander
    TInt iMaxUnicodeWords;                              // maximum number of Unicode words to read or write
    TInt iCompressedBytes;                              // compressed bytes processed: read by expander, written by compressor
    TInt iMaxCompressedBytes;                           // maximum number of compressed bytes to read or write
    };

/**
@internalComponent
*/
class MUnicodeSource
    {
    public:
    virtual TUint16 ReadUnicodeValueL() = 0;
    };

/**
@internalComponent

A class to read Unicode values directly from memory.
*/
class TMemoryUnicodeSource: public MUnicodeSource
    {
    public:
    inline TMemoryUnicodeSource(const TUint16* aPtr);
    inline TUint16 ReadUnicodeValueL();

    private:
    const TUint16* iPtr;
    };

/**
@internalComponent
 
A class to read Unicode values from a stream built on a memory object.
*/
class TMemoryStreamUnicodeSource: public MUnicodeSource
    {
    public:
    inline TMemoryStreamUnicodeSource(CStoreReadStream& aStream);
    inline TUint16 ReadUnicodeValueL();

    private:
    CStoreReadStream& iStream;
    };

/**
@internalComponent
 
*/
class MUnicodeSink
    {
    public:
    virtual void WriteUnicodeValueL(TUint16 aValue) = 0;
    };

/**
@internalComponent

A class to write Unicode values directly to memory.
*/
class TMemoryUnicodeSink: public MUnicodeSink
    {
    public:
    inline TMemoryUnicodeSink(TUint16* aPtr);
    inline void WriteUnicodeValueL(TUint16 aValue);

    private:
    TUint16* iPtr;
    };

/**
@internalComponent

A class to write Unicode values to a stream built on a memory object.
*/
class TMemoryStreamUnicodeSink: public MUnicodeSink
    {
    public:
    inline TMemoryStreamUnicodeSink(CStoreWriteStream& aStream);
    inline void WriteUnicodeValueL(TUint16 aValue);

    private:
    CStoreWriteStream& iStream;
    };

/**
@internalComponent
 
A class to hold functions to compress text using the Standard Compression Scheme for Unicode.
*/
class TUnicodeCompressor: public TUnicodeCompressionState
    {
    public:
    TUnicodeCompressor();
    void CompressL(CStoreWriteStream& aOutput,MUnicodeSource& aInput,
                            TInt aMaxOutputBytes = KMaxTInt,TInt aMaxInputWords = KMaxTInt,
                            TInt* aOutputBytes = static_cast<TInt*>(NULL),TInt* aInputWords = static_cast<TInt*>(NULL));
    void CompressL(TUint8* aOutput,MUnicodeSource& aInput,
                            TInt aMaxOutputBytes = KMaxTInt,TInt aMaxInputWords = KMaxTInt,
                            TInt* aOutputBytes = static_cast<TInt*>(NULL),TInt* aInputWords = static_cast<TInt*>(NULL));
    TInt FlushL(CStoreWriteStream& aOutput,TInt aMaxOutputBytes,TInt& aOutputBytes);
    TInt FlushL(TUint8* aOutput,TInt aMaxOutputBytes,TInt& aOutputBytes);
    static TInt CompressedSizeL(MUnicodeSource& aInput,TInt aInputWords);

    private:

     // A structure to store a character and its treatment code
    struct TAction
        {
        // Treatment codes: static and dynamic window numbers, plain ASCII or plain Unicode
        enum
            {
            EPlainUnicode = -2, // character cannot be expressed as ASCII or using static or dynamic windows
            EPlainASCII = -1,   // character can be emitted as an ASCII code
            EFirstDynamic = 0,  // values 0..255 are for dynamic windows with offsets at these places in the offset table
            ELastDynamic = 255,
            EFirstStatic = 256, // values 256..263 are for static windows 0..7
            ELastStatic = 263
            };

        inline TAction();
        TAction(TUint16 aCode);

        TUint16 iCode;      // Unicode value of the character
        TInt iTreatment;    // treatment code: see above
        };

    void DoCompressL(CStoreWriteStream* aOutputStream,TUint8* aOutputPointer,MUnicodeSource* aInput,
                     TInt aMaxCompressedBytes,TInt aMaxUnicodeWords,
                     TInt* aCompressedBytes,TInt* aUnicodeWords);
    void FlushInputBufferL();
    void FlushOutputBufferL();
    void WriteRunL();
    void WriteCharacter(const TAction& aAction);
    void WriteSCharacter(const TAction& aAction);
    void WriteUCharacter(TUint16 aCode);
    void WriteByte(TUint aByte);
    void WriteCharacterFromBuffer();
    void SelectTreatment(TInt aTreatment);

    enum
        {
        EMaxInputBufferSize = 4,
        EMaxOutputBufferSize = EMaxInputBufferSize * 3  // no Unicode character can be encoded as more than three bytes
        };
    TAction iInputBuffer[EMaxInputBufferSize];          // circular buffer; queue of Unicode characters to be processed
    TInt iInputBufferStart;                             // position of first Unicode character to be processed
    TInt iInputBufferSize;                              // characters in the input buffer
    TUint8 iOutputBuffer[EMaxOutputBufferSize];         // circular buffer; queue of compressed bytes to be output
    TInt iOutputBufferStart;                            // position of first compressed byte to be output
    TInt iOutputBufferSize;                             // characters in the output buffer
    TInt iDynamicWindowIndex;                           // index of the current dynamic window
    CStoreWriteStream* iOutputStream;                        // if non-null, output is to this stream
    TUint8* iOutputPointer;                             // if non-null, output is to memory
    MUnicodeSource* iInput;                             // input object
    };

/**
@internalComponent

A class to hold functions to expand text using the Standard Compression Scheme for Unicode.
*/
class TUnicodeExpander: public TUnicodeCompressionState
    {
    public:
    TUnicodeExpander();
    void ExpandL(MUnicodeSink& aOutput,CStoreReadStream& aInput,
                          TInt aMaxOutputWords = KMaxTInt,TInt aMaxInputBytes = KMaxTInt,
                          TInt* aOutputWords = static_cast<TInt*>(NULL),TInt* aInputBytes = static_cast<TInt*>(NULL));
    void ExpandL(MUnicodeSink& aOutput,const TUint8* aInput,
                          TInt aMaxOutputWords = KMaxTInt,TInt aMaxInputBytes = KMaxTInt,
                          TInt* aOutputWords = static_cast<TInt*>(NULL),TInt* aInputBytes = static_cast<TInt*>(NULL));
    TInt FlushL(MUnicodeSink& aOutput,TInt aMaxOutputWords,TInt& aOutputWords);
    static TInt ExpandedSizeL(CStoreReadStream& aInput,TInt aInputBytes);
    static TInt ExpandedSizeL(const TUint8* aInput,TInt aInputBytes);

    private:
    void DoExpandL(MUnicodeSink* aOutput,CStoreReadStream* aInputStream,const TUint8* aInputPointer,
                   TInt aMaxOutputWords,TInt aMaxInputBytes,
                   TInt* aOutputWords,TInt* aInputBytes);
    void HandleByteL();
    void FlushOutputBufferL();
    TBool HandleSByteL(TUint8 aByte);
    TBool HandleUByteL(TUint8 aByte);
    TBool ReadByteL(TUint8& aByte);
    TBool QuoteUnicodeL();
    TBool DefineWindowL(TInt aIndex);
    TBool DefineExpansionWindowL();
    void WriteChar(TText aChar);
    void WriteChar32(TUint aChar);

    enum
        {
        EMaxInputBufferSize = 3,                        // no Unicode character can be encoded as more than 3 bytes
        EMaxOutputBufferSize = 2                        // no byte can be expanded into more than 2 Unicode characters
        };
    TUint8 iInputBuffer[EMaxInputBufferSize];           // buffer containing a group of compressed bytes representing
                                                        // a single operation; when an input source ends in the
                                                        // middle of an operation, this buffer enables the next
                                                        // expansion to start in the correct state
    TInt iInputBufferStart;                             // next read position in the input buffer
    TInt iInputBufferSize;                              // bytes in the input buffer
    TUint16 iOutputBuffer[EMaxOutputBufferSize];        // circular buffer; queue of Unicode characters to be output
    TInt iOutputBufferStart;                            // position of first Unicode character to be output
    TInt iOutputBufferSize;                             // characters in the output buffer
    MUnicodeSink* iOutput;                              // output object
    CStoreReadStream* iInputStream;                          // if non-null, input is from this stream
    const TUint8* iInputPointer;                        // if non-null, input is from memory
    };

// inline functions start here

inline TMemoryUnicodeSource::TMemoryUnicodeSource(const TUint16* aPtr):
    iPtr(aPtr)
    {
    }

inline TUint16 TMemoryUnicodeSource::ReadUnicodeValueL()
    {
    return *iPtr++;
    }

inline TMemoryStreamUnicodeSource::TMemoryStreamUnicodeSource(CStoreReadStream& aStream):
    iStream(aStream)
    {
    }

inline TUint16 TMemoryStreamUnicodeSource::ReadUnicodeValueL()
    {
    TUint16 x;
    iStream.Read(reinterpret_cast<TUint8*>(&x),sizeof(TUint16));
    return x;
    }

inline TMemoryUnicodeSink::TMemoryUnicodeSink(TUint16* aPtr):
    iPtr(aPtr)
    {
    }

inline void TMemoryUnicodeSink::WriteUnicodeValueL(TUint16 aValue)
    {
    *iPtr++ = aValue;
    }

inline TMemoryStreamUnicodeSink::TMemoryStreamUnicodeSink(CStoreWriteStream& aStream):
    iStream(aStream)
    {
    }

inline void TMemoryStreamUnicodeSink::WriteUnicodeValueL(TUint16 aValue)
    {
    iStream.Write(reinterpret_cast<TUint8*>(&aValue),sizeof(TUint16));
    }

inline TUnicodeCompressor::TAction::TAction():
    iCode(0),
    iTreatment(EPlainUnicode)
    {
    }
}
#endif // !defined(__UNICODECOMPRESSION_H__)
