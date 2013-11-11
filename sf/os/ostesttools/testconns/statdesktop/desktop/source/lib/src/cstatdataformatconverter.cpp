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




#include "stdafx.h"
#include "afxpriv.h"
#include "CSTATDataFormatConverter.h"

//-------------------------------------------------------------------------------
//standard functions for new variant class
CSTATDataFormatConverter::CSTATDataFormatConverter(CSTATLogFile *pLog)
: bWriteToFile(true)
{
	vt = VT_BLOB;
	    
	BLOB* pBody = new BLOB;
	byref = pBody;
	pBody->pBlobData = m_index = new BYTE [16];	//16 is the default buffer size
	pBody->cbSize = m_bufsize = 16;

	// set up logging
	pLogFile = pLog;

	// reset bitmap structures
	memset(&iPbmHeader, 0xff, sizeof(iPbmHeader));
	memset(&fileheader, 0xff, sizeof(fileheader));
	memset(&bmpHeader, 0xff, sizeof(bmpHeader));
	pPbmBits = NULL;
	bmpBits = NULL;
	lDataSize = 0;
}

//-------------------------------------------------------------------------------

//Destructor
CSTATDataFormatConverter::~CSTATDataFormatConverter()
{
	BLOB* pBody = (BLOB*)byref;
	if (pBody)
	{
		if (pBody->pBlobData) 
			delete [] pBody->pBlobData;
	
		delete pBody;
	}

	if (pPbmBits)
	{
		delete [] pPbmBits;
		pPbmBits = NULL;
	}

	if (bmpBits)
	{
		delete [] bmpBits;
		bmpBits = NULL;
	}
}

//----------------------------------------------------------------------------

bool CSTATDataFormatConverter::ConvertStringToUnicode(CString& data)
{
	// clear the existing buffer
	((BLOB*)byref)->cbSize = 0;

	// length IN BYTES of the UNICODE result (not the TCHAR input)
	UINT len = (data.GetLength() + 1) * 2;

	// check that the buffer can hold the contents -- otherwise enlarge
	EnsureBufSize( len );

	// convert the input string to unicode
	USES_CONVERSION;
	LPWSTR wstr = T2W(data.GetBuffer(0));

	// copy data into the buffer and set the size
	memcpy( m_index, wstr, len);
	((BLOB*)byref)->cbSize = len;

	// done
	return true;
}

//----------------------------------------------------------------------------

void CSTATDataFormatConverter::EnsureBufSize(int size)
{
	if (size > m_bufsize)
	{
		BLOB* pBody = (BLOB*)byref;
		BYTE* newbuf = new BYTE [size];
		delete[] pBody->pBlobData;
		m_index = pBody->pBlobData = newbuf;
		m_bufsize = size;
	}
}

//----------------------------------------------------------------------------
//Converts from .mbm to .bmp
int
CSTATDataFormatConverter::ConvertScreenShot(CString &mbmdirectory, CString &destdir)
{
	CFileFind imagefinder;
	CString searchpattern = mbmdirectory;
	if (searchpattern.Right(1) != _T('\\'))
		searchpattern += _T("\\");
	searchpattern += "*.mbm";

	// there will only ever be one file to be converted at a time
	if (imagefinder.FindFile(searchpattern, 0))
	{
		imagefinder.FindNextFile();

		// get the source filename (in quotes)
		CString imagefilename = _T("\"");
		imagefilename += imagefinder.GetFilePath();
		imagefilename += _T("\"");

		// get the destination folder (in quotes)
		CString newimagefilename = _T("\"");
		newimagefilename += destdir;

		// add a backslash if we need one
		if (newimagefilename.Right(1) != _T("\\"))
			newimagefilename += _T("\\");

		// add the filename
		newimagefilename += imagefinder.GetFileName();
		newimagefilename.Replace(_T(".mbm"), _T(".bmp"));
		newimagefilename += _T("\"");
		imagefinder.Close();

		// remove dest file if it already exists
		if(imagefinder.FindFile(newimagefilename, 0))
		{
			imagefinder.Close();
			DeleteFile(newimagefilename);
		}

		imagefilename.Remove('\"');
		newimagefilename.Remove('\"');

		if (bWriteToFile)
		{
			pLogFile->Set(CONVERTINGIMAGE, imagefilename.GetBuffer(0));
			pLogFile->Set(TO, newimagefilename.GetBuffer(0));
		}
		else
			pLogFile->Set(READINGIMAGE, imagefilename.GetBuffer(0));

		// convert the bitmap
		int errorcode = LoadBitmap(imagefilename);
		if (errorcode != ITS_OK)
			return pLogFile->Set(E_BITMAPLOADFAILED, pLogFile->Text(errorcode));

		if ((errorcode = SaveBitmap(newimagefilename)) != ITS_OK)
			return pLogFile->Set(E_BITMAPSAVEFAILED, pLogFile->Text(errorcode));

		DeleteFile(imagefilename);	//delete original .mbm image

		// success
		mbmdirectory = newimagefilename;

		// log a message that desktop STAT can use to display bitmap
		pLogFile->Set(SCREENSHOT_CONVERSION_OK, mbmdirectory, false, true);

		return ITS_OK;
	}

	return pLogFile->Set(E_NOIMAGESTOCONVERT, searchpattern);
}

//----------------------------------------------------------------------------
int CSTATDataFormatConverter::LoadBitmap(CString &file)
{
	// reset structures
	memset(&iPbmHeader, 0xff, sizeof(iPbmHeader));
	if (pPbmBits)
	{
		delete [] pPbmBits;
		pPbmBits = NULL;
	}

	// open file
	HANDLE infile;
	if (INVALID_HANDLE_VALUE != (infile = CreateFile(file, 
										   GENERIC_READ,
										   FILE_SHARE_READ, 
										   NULL, 
										   OPEN_EXISTING,
										   FILE_ATTRIBUTE_NORMAL,
										   0)))
	{
		long int iBuffer;
		DWORD dwBytesRead = 0;

		// read the header to ensure it is a valid file
		if (!ReadFile(infile, (LPVOID *)&iBuffer, 4, &dwBytesRead, NULL) ||
			dwBytesRead != 4 || iBuffer != KWriteOnceFileStoreUid)
		{
			CloseHandle(infile);
			return E_BADFORMAT;
		}

		if (!ReadFile(infile, (LPVOID *)&iBuffer, 4, &dwBytesRead, NULL) ||
			dwBytesRead != 4 || iBuffer != KMultiBitmapFileImageUid)
		{
			CloseHandle(infile);
			return E_BADFORMAT;
		}

		if (!ReadFile(infile, (LPVOID *)&iBuffer, 4, &dwBytesRead, NULL) ||
			dwBytesRead != 4 || iBuffer != 0)
		{
			CloseHandle(infile);
			return E_BADFORMAT;
		}

		if (!ReadFile(infile, (LPVOID *)&iBuffer, 4, &dwBytesRead, NULL) ||
			dwBytesRead != 4 || iBuffer != KMultiBitmapFileImageChecksum)
		{
			CloseHandle(infile);
			return E_BADFORMAT;
		}

		if (!ReadFile(infile, (LPVOID *)&iBuffer, 4, &dwBytesRead, NULL) ||
			dwBytesRead != 4)
		{
			CloseHandle(infile);
			return E_BADFORMAT;
		}

		// go back to offset from the beginning
		if (SetFilePointer(infile, iBuffer, NULL, FILE_BEGIN) == 0xFFFFFFFF)
		{
			CloseHandle(infile);
			return E_BADFORMAT;
		}

		// read the number of sources (in our case it will always be one)
		if (!ReadFile(infile, (LPVOID *)&iBuffer, 4, &dwBytesRead, NULL) ||
			dwBytesRead != 4)
		{
			CloseHandle(infile);
			return E_BADFORMAT;
		}

		// read offset from the beginning of this bitmap
		if (!ReadFile(infile, (LPVOID *)&iBuffer, 4, &dwBytesRead, NULL) ||
			dwBytesRead != 4)
		{
			CloseHandle(infile);
			return E_BADFORMAT;
		}

		// go back to offset from the beginning
		if (SetFilePointer(infile, iBuffer, NULL, FILE_BEGIN) == 0xFFFFFFFF)
		{
			CloseHandle(infile);
			return E_BADFORMAT;
		}

		// read in the header
		DWORD dwSize = sizeof(SEpocBitmapHeader);
		if (!ReadFile(infile, (LPVOID *)&iPbmHeader, dwSize, &dwBytesRead, NULL) || dwBytesRead != dwSize)
		{
			CloseHandle(infile);
			return E_BADFORMAT;
		}

		dwSize = iPbmHeader.iBitmapSize - iPbmHeader.iStructSize;

		// now read in the bitmap matrix
		pPbmBits = new char[dwSize];
		if (pPbmBits)
		{
			memset(pPbmBits, 0xff, dwSize);
			if (!ReadFile(infile, (LPVOID *)pPbmBits, dwSize, &dwBytesRead, NULL) || dwBytesRead != dwSize)
			{
				CloseHandle(infile);
				return E_BADFORMAT;
			}

			if (iPbmHeader.iCompression != ENoBitmapCompression)
			{
				int byteWidth = ByteWidth(iPbmHeader.iWidthInPixels, iPbmHeader.iBitsPerPixel);
				int expandedsize = byteWidth * iPbmHeader.iHeightInPixels;
				char* newbits = new char[expandedsize];
				if (newbits)
				{
					memset(newbits,0xff,expandedsize);
					int ret = ITS_OK;

					switch (iPbmHeader.iCompression)
					{
						case EByteRLECompression:
							ret = ExpandByteRLEData(newbits,expandedsize,pPbmBits, dwSize);
							break;
						case ETwelveBitRLECompression:
							ret = ExpandTwelveBitRLEData(newbits,expandedsize,pPbmBits, dwSize);
							break;
						case ESixteenBitRLECompression:
							ret = ExpandSixteenBitRLEData(newbits,expandedsize,pPbmBits, dwSize);
							break;
						case ETwentyFourBitRLECompression:
							ret = ExpandTwentyFourBitRLEData(newbits,expandedsize,pPbmBits, dwSize);
							break;
						default:
							delete [] pPbmBits;
							return E_BADCOMPRESSION;
							break;
					}

					delete [] pPbmBits;
					pPbmBits = newbits;
					iPbmHeader.iCompression = ENoBitmapCompression;
					iPbmHeader.iBitmapSize += expandedsize - dwSize;
				}
				else
				{
					CloseHandle(infile);
					return E_OUTOFMEM;
				}
			}

			CloseHandle(infile);
		}
		else
		{
			CloseHandle(infile);
			return E_OUTOFMEM;
		}
	}
	else
		return E_FILE_OPEN_READ_FAILED;

	return ITS_OK;
}

bool
CSTATDataFormatConverter::ExpandByteRLEData(char* aDest,int aDestSize,char* aSrce,int aSrceSize)
{
	char* srcelimit=aSrce+aSrceSize;
	char* destlimit=aDest+aDestSize;
	while(aSrce<srcelimit && aDest<destlimit)
		{
		char count=*aSrce++;
		if (count<0)
			{
			int runLength=-count;
			memcpy(aDest,aSrce,runLength);
			aSrce+=runLength;
			aDest+=runLength;
			}
		else
			{
			char value=*aSrce++;
			while(count>=0)
				{
				*aDest++=value;
				count--;
				}
			}
		}
	if (aSrce!=srcelimit || aDest!=destlimit)
		return false;

	return true;
}

bool
CSTATDataFormatConverter::ExpandTwelveBitRLEData(char* aDest,int aDestSizeInBytes,char* aSrce,int aSrceSizeInBytes)
{
	unsigned short* srcePtr = (unsigned short*)aSrce;
	unsigned short* destPtr = (unsigned short*)aDest;
	unsigned short* srcePtrLimit = srcePtr + (aSrceSizeInBytes / 2);
	unsigned short* destPtrLimit = destPtr + (aDestSizeInBytes / 2);

	while (srcePtr < srcePtrLimit && destPtr < destPtrLimit)
		{
		unsigned short value = *srcePtr++;
		int runLength = (value >> 12) + 1;
		value &= 0x0fff;

		for (;runLength > 0;runLength--)
			*destPtr++ = value;
		}

	if (srcePtr != srcePtrLimit || destPtr != destPtrLimit)
		return false;

	return true;
}

bool
CSTATDataFormatConverter::ExpandSixteenBitRLEData(char* aDest,int aDestSizeInBytes,char* aSrce,int aSrceSizeInBytes)
{
	char* srcePtrLimit = aSrce + aSrceSizeInBytes;
	unsigned short* destPtr = (unsigned short*)aDest;
	unsigned short* destPtrLimit = destPtr + (aDestSizeInBytes / 2);

	while (aSrce < srcePtrLimit && destPtr < destPtrLimit)
		{
		int runLength = *aSrce++;

		if (runLength >= 0)
			{
			unsigned short value = *((unsigned short*)(aSrce));
			aSrce += 2;
			for (runLength++; runLength > 0; runLength--)
				*destPtr++ = value;
			}
		else
			{
			runLength = -runLength;
			int byteLength = runLength * 2;
			memcpy(destPtr,aSrce,byteLength);
			aSrce += byteLength;
			destPtr += runLength;
			}
		}

	if (aSrce != srcePtrLimit || destPtr != destPtrLimit)
		return false;

	return true;
}

bool
CSTATDataFormatConverter::ExpandTwentyFourBitRLEData(char* aDest,int aDestSizeInBytes,char* aSrce,int aSrceSizeInBytes)
{
	char* srcePtrLimit = aSrce + aSrceSizeInBytes;
	char* destPtrLimit = aDest + aDestSizeInBytes;

	while (aSrce < srcePtrLimit && aDest < destPtrLimit)
		{
		int runLength = *aSrce++;

		if (runLength >= 0)
			{
			char component1 = *aSrce++;
			char component2 = *aSrce++;
			char component3 = *aSrce++;
			for (runLength++; runLength > 0; runLength--)
				{
				*aDest++ = component1;
				*aDest++ = component2;
				*aDest++ = component3;
				}
			}
		else
			{
			runLength = -runLength;
			int byteLength = runLength * 3;
			memcpy(aDest,aSrce,byteLength);
			aSrce += byteLength;
			aDest += byteLength;
			}
		}

	if (aSrce != srcePtrLimit || aDest != destPtrLimit)
		return false;

	return true;
}

int
CSTATDataFormatConverter::SaveBitmap(CString &newfilename)
{
	// reset structures
	memset(&fileheader, 0xff, sizeof(fileheader));
	memset(&bmpHeader, 0xff, sizeof(bmpHeader));
	if (bmpBits)
	{
		delete [] bmpBits;
		bmpBits = NULL;
	}
	lDataSize = 0;

	// copy header info into BMP structures
	bmpHeader.biSize = sizeof(TBitmapInfoHeader);
	bmpHeader.biWidth = iPbmHeader.iWidthInPixels;
	bmpHeader.biHeight = iPbmHeader.iHeightInPixels;
	bmpHeader.biPlanes = 1;
	bmpHeader.biBitCount = 24;
	bmpHeader.biCompression = 0;
	bmpHeader.biSizeImage = 0;
	bmpHeader.biXPelsPerMeter = 0;
	bmpHeader.biYPelsPerMeter = 0;
	bmpHeader.biClrUsed = 0;
	bmpHeader.biClrImportant = 0;

	long byteWidth = ((bmpHeader.biWidth * 3) + 3) & ~3;
	lDataSize = bmpHeader.biHeight * byteWidth;

	fileheader.bfType = 'B'+('M'<<8);
	fileheader.bfSize = sizeof(TBitmapFileHeader)+sizeof(TBitmapInfoHeader) + lDataSize;
	fileheader.bfReserved1 = 0;
	fileheader.bfReserved2 = 0;
	fileheader.bfOffBits = sizeof(TBitmapFileHeader)+sizeof(TBitmapInfoHeader);

	// copy BMP data
	bmpBits = new char[lDataSize];
	if (!bmpBits)
		return E_OUTOFMEM;

	memset(bmpBits,0xff,lDataSize);

	for(long y=0;y<bmpHeader.biHeight;y++)
	{
		char* dest=&bmpBits[(bmpHeader.biHeight-y-1)*byteWidth];
		for(long x=0;x<bmpHeader.biWidth;x++)
		{
			TRgb pixel=GetPixel(x,y);
			*dest++=pixel.iBlue;
			*dest++=pixel.iGreen;
			*dest++=pixel.iRed;
		}
	}

	int ret = ITS_OK;
	if (bWriteToFile)
	{
		// write the whole lot out to file
		HANDLE infile;
		if (INVALID_HANDLE_VALUE != (infile = CreateFile(newfilename, 
											   GENERIC_WRITE,
											   0, 
											   NULL, 
											   CREATE_ALWAYS,
											   FILE_ATTRIBUTE_NORMAL,
											   0)))
		{
			DWORD dwBytesWritten = 0;

			// read the header to ensure it is a valid file
			if (WriteFile(infile, (LPVOID *)&fileheader, sizeof(TBitmapFileHeader), &dwBytesWritten, NULL) &&
				dwBytesWritten == sizeof(TBitmapFileHeader))
			{
				// read the header to ensure it is a valid file
				if (WriteFile(infile, (LPVOID *)&bmpHeader, sizeof(TBitmapInfoHeader), &dwBytesWritten, NULL) &&
					dwBytesWritten == sizeof(TBitmapInfoHeader))
				{
					if (WriteFile(infile, (LPVOID *)bmpBits, lDataSize, &dwBytesWritten, NULL) &&
						dwBytesWritten == lDataSize)
					{
						ret = ITS_OK;
					}
					else
						ret = E_BADWRITE;
				}
				else
					ret = E_BADWRITE;
			}
			else
				ret = E_BADWRITE;

			CloseHandle(infile);
		}
		else
			ret = E_FILE_OPEN_WRITE_FAILED;
	}

	return ret;
}

TRgb
CSTATDataFormatConverter::GetPixel(int aXCoord,int aYCoord)
{
	unsigned char col;
	aXCoord%=iPbmHeader.iWidthInPixels;
	aYCoord%=iPbmHeader.iHeightInPixels;
	int byteWidth = ByteWidth(iPbmHeader.iWidthInPixels,iPbmHeader.iBitsPerPixel);
	int yOffset = aYCoord * byteWidth;

	switch(iPbmHeader.iBitsPerPixel)
	{
		case 1:
			col = pPbmBits[yOffset + (aXCoord / 8)];
			col >>= (aXCoord&7);
			return TRgb::Gray2(col & 1);
		case 2:
			col = pPbmBits[yOffset + (aXCoord / 4)];
			col = (unsigned char)(col>>(2*(aXCoord%4)));
			return TRgb::Gray4(col & 3);
		case 4:
			col = pPbmBits[yOffset + (aXCoord / 2)];
			if (aXCoord & 1)
				col >>= 4;
			col &= 0xf;
			if (iPbmHeader.iColor==EColorBitmap)
				return TRgb::Color16(col);
			else
				return TRgb::Gray16(col);
		case 8:
			col=pPbmBits[yOffset + aXCoord];
			if (iPbmHeader.iColor==EColorBitmap)
				return TRgb::Color256(col);
			else
				return TRgb::Gray256(col);
		case 12:
		case 16:
		{
			unsigned short* shortPtr = (unsigned short*)&pPbmBits[yOffset + (aXCoord * 2)];
			if (iPbmHeader.iBitsPerPixel == 12)
				return TRgb::Color4K(*shortPtr);
			else
				return TRgb::Color64K(*shortPtr);
		}
		case 24:
		{
			char* pixelPtr = pPbmBits + yOffset + (aXCoord * 3);
			TRgb pixelColor;
			pixelColor.iBlue = *pixelPtr++;
			pixelColor.iGreen = *pixelPtr++;
			pixelColor.iRed = *pixelPtr;
			return pixelColor;
		}
		default:
			return TRgb(0);
	}
}

int
CSTATDataFormatConverter::ByteWidth(int aPixelWidth,int aBitsPerPixel)
{
	int wordWidth = 0;

	switch (aBitsPerPixel)
	{
	case 1:
		wordWidth = (aPixelWidth + 31) / 32;
		break;
	case 2:
		wordWidth = (aPixelWidth + 15) / 16;
		break;
	case 4:
		wordWidth = (aPixelWidth + 7) / 8;
		break;
	case 8:
		wordWidth = (aPixelWidth + 3) / 4;
		break;
	case 12:
	case 16:
		wordWidth = (aPixelWidth + 1) / 2;
		break;
	case 24:
		wordWidth = (((aPixelWidth * 3) + 11) / 12) * 3;
		break;
	default:
		break;
	};

	return wordWidth * 4;
}

