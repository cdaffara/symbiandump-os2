/*
* Copyright (c) 2005-2009 Nokia Corporation and/or its subsidiary(-ies).
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




#ifndef CSTATDataFormatConverter_H
#define CSTATDataFormatConverter_H

#include <oaidl.h>
#include <statcommon.h>
#include "CSTATLogfile.h"

const long int KWriteOnceFileStoreUid=268435511;
const long int KMultiBitmapFileImageUid=268435522;
const long int KMultiBitmapFileImageChecksum=1194943545;

enum TBitmapColor
{
	EMonochromeBitmap,
	EColorBitmap
};

enum TBitmapfileCompression
{
	ENoBitmapCompression=0,
	EByteRLECompression,
	ETwelveBitRLECompression,
	ESixteenBitRLECompression,
	ETwentyFourBitRLECompression
};

struct SEpocBitmapHeader
{
	long iBitmapSize;
	long iStructSize;
	long iWidthInPixels;
	long iHeightInPixels;
	long iWidthInTwips;
	long iHeightInTwips;
	long iBitsPerPixel;
	TBitmapColor iColor;
	long iPaletteEntries;	// always 0 in current implementations
	TBitmapfileCompression iCompression;
};

#include <pshpack2.h>
struct TRgbTriple
{
	unsigned char rgbtBlue;
	unsigned char rgbtGreen;
	unsigned char rgbtRed;
};
#include <poppack.h>

struct TRgbQuad
{
	unsigned char iBlue;
	unsigned char iGreen;
	unsigned char iRed;
	unsigned char iReserved;
};

class TRgb
{
public:
	TRgb();
	TRgb(long unsigned int val);
	TRgb(int aRed,int aGreen,int aBlue);
	TRgb &operator=(const TRgb& aColor);
	int operator==(const TRgb& aColor);
	int Difference(const TRgb& col) const;
	int Gray2() const;
	int Gray4() const;
	int Gray16() const;
	int Gray256() const;
	int Color16() const;
	int Color256() const;
	int Color4K() const;
	int Color64K() const;
	long int Color16M() const;
	static TRgb Gray2(int aGray2);
	static TRgb Gray4(int aGray4);
	static TRgb Gray16(int aGray16);
	static TRgb Gray256(int aGray256);
	static TRgb Color16(int aColor16);
	static TRgb Color256(int aColor256);
	static TRgb Color4K(int aColor64K);
	static TRgb Color64K(int aColor64K);
	static TRgb Color16M(long int aColor16M);
public:
	unsigned char iRed;
	unsigned char iGreen;
	unsigned char iBlue;
	unsigned char iSpare;
};

class CSTATDataFormatConverter : public VARIANT
{
	public:
		CSTATDataFormatConverter(CSTATLogFile *pLog);
		virtual ~CSTATDataFormatConverter();
		bool ConvertStringToUnicode(CString& data);
		int ConvertScreenShot(CString &mbmdirectory, CString &destptr);
		void GetScreenshotData(TBitmapFileHeader **ppFHeader, TBitmapInfoHeader **ppBMPHeader, char **ppBits, unsigned long *pSize)
		{
			*ppFHeader = &fileheader;
			*ppBMPHeader = &bmpHeader;
			*ppBits = bmpBits;
			*pSize = lDataSize;
		}

		bool bWriteToFile;						// flag to write bitmap out to disk or not

	private:
		void EnsureBufSize(int size);
		int LoadBitmap(CString &file);
		int SaveBitmap(CString &file);
		bool ExpandByteRLEData(char* aDest,int aDestSize,char* aSrce,int aSrceSize);
		bool ExpandTwelveBitRLEData(char* aDest,int aDestSizeInBytes,char* aSrce,int aSrceSizeInBytes);
		bool ExpandSixteenBitRLEData(char* aDest,int aDestSizeInBytes,char* aSrce,int aSrceSizeInBytes);
		bool ExpandTwentyFourBitRLEData(char* aDest,int aDestSizeInBytes,char* aSrce,int aSrceSizeInBytes);
		TRgb GetPixel(int aXCoord,int aYCoord);
		int ByteWidth(int aPixelWidth,int aBitsPerPixel);

		CSTATLogFile *pLogFile;					// pointer to our logging class
		BYTE* m_index;							// command buffer access
		int m_bufsize;							// command buffer size

		SEpocBitmapHeader iPbmHeader;			// EPOC screenshot header info
		char* pPbmBits;							// EPOC screenshot data
		TBitmapFileHeader fileheader;			// converted screenshot header info
		TBitmapInfoHeader bmpHeader;			// converted screenshot bitmap info
		char* bmpBits;							// converted screenshot data
		unsigned long lDataSize;
};

#endif
