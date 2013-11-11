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
#include <STATCommon.h>
#include "CSTATImageVerify.h"

//----------------------------------------------------------------------------
//standard constructor
CSTATImageVerify::CSTATImageVerify(CSTATLogFile *pLog)
: iImageCount(0), margin(0), lastrefimageloaded(0), pLogFile(pLog),
  m_pDib(NULL), m_pDib2(NULL), m_pDibBits(NULL), m_pDibBits2(NULL),
  m_pBIH(NULL), m_pBIH2(NULL), m_pPalette(NULL), m_pPalette2(NULL),
  m_dwDibSize(0), m_dwDibSize2(0), m_nPaletteEntries(0), m_nPaletteEntries2(0)
{
}

//----------------------------------------------------------------------------
//destructor
CSTATImageVerify::~CSTATImageVerify()
{
	if (m_pDib)
	{
		delete [] m_pDib;
		m_pDib = NULL;
	}

	if (m_pDib2)
	{
		delete [] m_pDib2;
		m_pDib2 = NULL;
	}
}

//----------------------------------------------------------------------------

//image verification function (loading + blitting)
int CSTATImageVerify::VerifyImage(CString& lastscreenshot)	
{
	TCHAR buffer[70];
	_stprintf(buffer, _T("User set margin of error (percentage) : %ld"), margin);

	pLogFile->Set(START_VERIFICATION, buffer);

	// make sure we've got at least 1 image left
	if(ImagesRemaining())
	{
		if(LoadRefImage())
			pLogFile->Set(REFIMAGELOAD_OK, refimagearray[lastrefimageloaded++].completefilenamepath);
		else
			return pLogFile->Set(REFIMAGELOAD_FAILURE);

		Sleep(500);

		if(LoadNewImage(lastscreenshot))
			pLogFile->Set(NEWIMAGELOAD_OK);
		else
			return pLogFile->Set(NEWIMAGELOAD_FAILURE);
	}
	else
		return pLogFile->Set(REFIMAGELOAD_FAILURE_LIMIT);	//run out of reference images

	//compare images and decide if difference is greater than margin value set earlier

	// If we have not data we can't draw.
	if (!m_pDib2)
		return pLogFile->Set(VERIFICATION_FAILURE);

	//set width and height of images
	int nWidth = m_pBIH2 -> biWidth;
	int nHeight = m_pBIH2 -> biHeight;

	HDC hdc, hMaskDC, hImageDC;
	HBITMAP hMaskBitmap, hImageBitmap;

	hdc = GetDC(NULL);

	//---------------------

	hMaskDC = CreateCompatibleDC(hdc);
	hImageDC = CreateCompatibleDC(hdc);
	
	hMaskBitmap = CreateCompatibleBitmap(hdc, nWidth, nHeight);
	
	SelectObject(hMaskDC, hMaskBitmap);
	SetTextColor(hMaskDC, RGB(0, 0, 255)); 

	//---------------------

	hImageBitmap = CreateCompatibleBitmap(hdc, nWidth, nHeight);

	// release the object now we're finished with it
	ReleaseDC(NULL, hdc);

	SelectObject(hImageDC, hImageBitmap);
	SetTextColor(hImageDC, RGB(0, 0, 255)); 

	//---------------------

	//set MaskBitmap pixel data to that of the original DIB
	SetDIBits(hMaskDC, hMaskBitmap, 0L, nHeight, m_pDibBits, (BITMAPINFO *)m_pBIH, (DWORD)DIB_RGB_COLORS);
	
	//create a new DIB using second image data
	SetDIBits(hImageDC, hImageBitmap, 0L, nHeight, m_pDibBits2, (BITMAPINFO *)m_pBIH2, (DWORD)DIB_RGB_COLORS);

	//XOR new image with original image (so new pic on top of pic 2)
	BitBlt(hMaskDC, 0, 0, nWidth, nHeight, hImageDC, 0, 0, SRCINVERT);

	//***************************************************

	//now work on percentage difference in images

	//need to use 'new' otherwise you get a stack overflow with anything big like this (1MB+)
	COLORREF (*pixelcoord) [480];
	pixelcoord = new COLORREF [640][480];
	if (!pixelcoord)
		return pLogFile->Set(VERIFICATION_FAILURE);

	int x, y, myheight, mywidth;
	float totalpixelarea;

	x = 0;
	y = 0;
	int difference = 0;
	float finalpercentage = 0;
	totalpixelarea = 0;
	myheight = 0;
	mywidth = 0;

	mywidth = m_pBIH -> biWidth;
	myheight = m_pBIH -> biHeight;

	// convert total to float for later % calculation
	totalpixelarea = (float)(mywidth * myheight);

	//initialisation
	for(y = 0; y < 480; y++)
	{
		//go across picture left to right at each line
		for(x = 0; x < 640; x++)
		{
			//get colorref value from coordinates
			pixelcoord[x][y] = 0;
		}
	}

	//check pixels - start at line 0
	for(y = 0; y < myheight; y++)
	{
		//go across picture left to right at each line
		for(x = 0; x < mywidth; x++)
		{
			//get colorref value from coordinates
			pixelcoord[x][y] = GetPixel(hMaskDC, x, y);

			//if not a  black pixel then increment counter
			if(pixelcoord[x][y] != 0)
			{
				difference++;
			}
		}
	}

	// release resources
	delete [] pixelcoord;
	DeleteDC(hMaskDC);
	DeleteDC(hImageDC);
	DeleteObject(hMaskBitmap);
	DeleteObject(hImageBitmap);

	if (m_pDib)
	{
		delete [] m_pDib;
		m_pDib = NULL;
	}

	if (m_pDib2)
	{
		delete [] m_pDib2;
		m_pDib2 = NULL;
	}

	//now calculate percentage of pic that is not black
	finalpercentage = ((difference / totalpixelarea) * 100);

	// write conversion info to file


	CString cBuffer;
	cBuffer.Format(_T("Margin %ld : Difference %f"), margin, finalpercentage);
	pLogFile->Set(cBuffer);

	if(finalpercentage > margin)
		return pLogFile->Set(VERIFICATION_FAILURE);

	return pLogFile->Set(VERIFICATION_PASS);
}


//----------------------------------------------------------------------------
//image loading for current new image
bool CSTATImageVerify::LoadNewImage(CString& newimage)
{
	CFile cf;

	// Attempt to open the Dib file for reading.
	if(!cf.Open(newimage, CFile::modeRead))
		return false;

	// Get the size of the file and store
	// in a local variable. Subtract the
	// size of the BITMAPFILEHEADER structure
	// since we won't keep that in memory.
	DWORD dwDibSize;
	dwDibSize = cf.GetLength() - sizeof( BITMAPFILEHEADER );

	// Attempt to allocate the Dib memory.
	unsigned char *pDib;
	pDib = new unsigned char [dwDibSize];
	if(!pDib)
	{
		cf.Close();
		return false;
	}

	BITMAPFILEHEADER BFH;

	// Read in the Dib header and data.
	try
	{
		// Did we read in the entire BITMAPFILEHEADER?
		if( cf.Read( &BFH, sizeof( BITMAPFILEHEADER ) )
			!= sizeof( BITMAPFILEHEADER ) ||

			// Is the type 'MB'?
			BFH.bfType != 'MB' ||

			// Did we read in the remaining data?
			cf.Read( pDib, dwDibSize ) != dwDibSize )
		{

			// Delete the memory if we had any
			// errors and return FALSE.
			delete [] pDib;
			cf.Close();
			return false;
			}
		}

	// If we catch an exception, delete the
	// exception, the temporary Dib memory,
	// and return FALSE.
	catch( CFileException *e )
	{
		e->Delete();
		delete [] pDib;
		cf.Close();
		return( FALSE );
	}
	
	// If we got to this point, the Dib has been
	// loaded. If a Dib was already loaded into
	// this class, we must now delete it.
	if (m_pDib2)
	{
		delete [] m_pDib2;
		m_pDib2 = NULL;
	}

	// Store the local Dib data pointer and
	// Dib size variables in the class member
	// variables.
	m_pDib2 = pDib;
	m_dwDibSize2 = dwDibSize;

	// Pointer our BITMAPINFOHEADER and RGBQUAD
	// variables to the correct place in the Dib data.
	m_pBIH2 = (BITMAPINFOHEADER *) m_pDib2;
	m_pPalette2 =
		(RGBQUAD *) &m_pDib2[sizeof(BITMAPINFOHEADER)];

	// Calculate the number of palette entries.
	m_nPaletteEntries = 1 << m_pBIH2->biBitCount;
	if( m_pBIH2->biBitCount > 8 )
		m_nPaletteEntries = 0;
	else if( m_pBIH2->biClrUsed != 0 )
		m_nPaletteEntries = m_pBIH2->biClrUsed;

	// Point m_pDib2Bits to the actual Dib bits data.
	m_pDibBits2 =
		&m_pDib2[sizeof(BITMAPINFOHEADER)+
			m_nPaletteEntries*sizeof(RGBQUAD)];

	// If we have a valid palette, delete it.
	if( m_Palette.GetSafeHandle() != NULL )
		m_Palette.DeleteObject();

	// If there are palette entries, we'll need
	// to create a LOGPALETTE then create the
	// CPalette palette.
	if( m_nPaletteEntries != 0 )
	{
		// Allocate the LOGPALETTE structure.
		LOGPALETTE *pLogPal = (LOGPALETTE *) new char [sizeof(LOGPALETTE) + m_nPaletteEntries*sizeof(PALETTEENTRY)];
		if (pLogPal)
		{
			// Set the LOGPALETTE to version 0x300
			// and store the number of palette
			// entries.
			pLogPal->palVersion = 0x300;
			pLogPal->palNumEntries = (WORD)m_nPaletteEntries;

			// Store the RGB values into each
			// PALETTEENTRY element.
			for( int i=0; i<m_nPaletteEntries; i++ )
			{
				pLogPal->palPalEntry[i].peRed =
					m_pPalette2[i].rgbRed;
				pLogPal->palPalEntry[i].peGreen =
					m_pPalette2[i].rgbGreen;
				pLogPal->palPalEntry[i].peBlue =
					m_pPalette2[i].rgbBlue;
			}

			// Create the CPalette object and
			// delete the LOGPALETTE memory.
			m_Palette.CreatePalette( pLogPal );
			delete [] pLogPal;
		}
		else
			return false;
	}

	cf.Close();

	return true;
}

//----------------------------------------------------------------------------
//image loading for current reference image
bool CSTATImageVerify::LoadRefImage()
{
	CFile cf;
	if( !cf.Open(refimagearray[lastrefimageloaded].completefilenamepath, CFile::modeRead ) )
		return false;

	// Get the size of the file and store
	// in a local variable. Subtract the
	// size of the BITMAPFILEHEADER structure
	// since we won't keep that in memory.
	DWORD dwDibSize;
	dwDibSize = cf.GetLength() - sizeof( BITMAPFILEHEADER );

	// Attempt to allocate the Dib memory.
	unsigned char *pDib;
	pDib = new unsigned char [dwDibSize];
	if(!pDib)
	{
		cf.Close();
		return false;
	}

	BITMAPFILEHEADER BFH;

	// Read in the Dib header and data.
	try
	{

		// Did we read in the entire BITMAPFILEHEADER?
		if( cf.Read( &BFH, sizeof( BITMAPFILEHEADER ) )
			!= sizeof( BITMAPFILEHEADER ) ||

			// Is the type 'MB'?
			BFH.bfType != 'MB' ||

			// Did we read in the remaining data?
			cf.Read( pDib, dwDibSize ) != dwDibSize )
		{

			// Delete the memory if we had any
			// errors and return FALSE.
			delete [] pDib;
			cf.Close();
			return false;
		}
	}

	// If we catch an exception, delete the
	// exception, the temporary Dib memory,
	// and return FALSE.
	catch( CFileException *e )
	{
		e->Delete();
		delete [] pDib;
		cf.Close();
		return false;
	}
	
	// If we got to this point, the Dib has been
	// loaded. If a Dib was already loaded into
	// this class, we must now delete it.
	if(m_pDib)
	{
		delete m_pDib;
		m_pDib = NULL;
	}

	// Store the local Dib data pointer and
	// Dib size variables in the class member
	// variables.
	m_pDib = pDib;
	m_dwDibSize = dwDibSize;

	// Pointer our BITMAPINFOHEADER and RGBQUAD
	// variables to the correct place in the Dib data.
	m_pBIH = (BITMAPINFOHEADER *) m_pDib;
	m_pPalette =
		(RGBQUAD *) &m_pDib[sizeof(BITMAPINFOHEADER)];

	// Calculate the number of palette entries.
	m_nPaletteEntries = 1 << m_pBIH->biBitCount;
	if( m_pBIH->biBitCount > 8 )
		m_nPaletteEntries = 0;
	else if( m_pBIH->biClrUsed != 0 )
		m_nPaletteEntries = m_pBIH->biClrUsed;

	// Point m_pDibBits to the actual Dib bits data.
	m_pDibBits =
		&m_pDib[sizeof(BITMAPINFOHEADER)+
			m_nPaletteEntries*sizeof(RGBQUAD)];

	// If we have a valid palette, delete it.
	if (m_Palette.GetSafeHandle())
		m_Palette.DeleteObject();

	// If there are palette entries, we'll need
	// to create a LOGPALETTE then create the
	// CPalette palette.
	if( m_nPaletteEntries != 0 )
	{
		// Allocate the LOGPALETTE structure.
		LOGPALETTE *pLogPal = (LOGPALETTE *) new char
				[sizeof(LOGPALETTE)+
				m_nPaletteEntries*sizeof(PALETTEENTRY)];

		if(pLogPal)
		{

			// Set the LOGPALETTE to version 0x300
			// and store the number of palette
			// entries.
			pLogPal->palVersion = 0x300;
			pLogPal->palNumEntries = (WORD)m_nPaletteEntries;

			// Store the RGB values into each
			// PALETTEENTRY element.
			for( int i=0; i<m_nPaletteEntries; i++ )
			{
				pLogPal->palPalEntry[i].peRed =
					m_pPalette[i].rgbRed;
				pLogPal->palPalEntry[i].peGreen =
					m_pPalette[i].rgbGreen;
				pLogPal->palPalEntry[i].peBlue =
					m_pPalette[i].rgbBlue;
			}

			// Create the CPalette object and
			// delete the LOGPALETTE memory.
			m_Palette.CreatePalette( pLogPal );
			delete [] pLogPal;
		}
		else
			return false;
	}

	cf.Close();

	return true;
}

//----------------------------------------------------------------------------
int CSTATImageVerify::EnableVerification(int fudge)
{
	// temp variable
	FILETIME creationtime;
	creationtime.dwLowDateTime = 0;
	creationtime.dwHighDateTime = 0;

	// initialise settings
	iImageCount = 0;
	lastrefimageloaded = 0;
	margin = fudge;

	// check limits
	if (margin < 0)
		margin = 0;
	if (margin > 100)
		margin = 100;

	// populate our array with images
	CFileFind refimagefinder;
	if (refimagefinder.FindFile(referenceimagedir + _T("\\*.bmp"), 0))
	{
		int i = 0;
		int iMoreFiles = true;
		for (i=0;i<VERIFY_MAX_IMAGES;i++)
		{
			if (iMoreFiles)
			{
				iMoreFiles = refimagefinder.FindNextFile();
  				refimagearray[i].completefilenamepath = refimagefinder.GetFilePath();	//store filename into array string variable
				refimagefinder.GetLastWriteTime(&creationtime);		//store corresponding modification time
				refimagearray[i].lCreationTime = (((ULONGLONG) creationtime.dwHighDateTime) << 32) + creationtime.dwLowDateTime;
				iImageCount++;
			}
			else
			{
				refimagearray[i].completefilenamepath = _T("");
				refimagearray[i].lCreationTime = 0;
			}
		}

		refimagefinder.Close();
	}
	
	// now sort into date/time order
	if (iImageCount)
	{
		CSTATReferenceImages temp;
		bool bNotFinished = true;
		int i = 0;
		while (bNotFinished)
		{
			bNotFinished = false;
			for (i=0;i<iImageCount;i++)
			{
				if ((i + 1) < iImageCount &&
					refimagearray[i+1].lCreationTime < refimagearray[i].lCreationTime)
				{
					temp = refimagearray[i];
					refimagearray[i]= refimagearray[i+1];
					refimagearray[i+1]= temp;
					bNotFinished = true;
				}
			}
		}
	}

	return iImageCount;
}
//----------------------------------------------------------------------------

//----------------------------------------------------------------------------
// Get the local reference image directory from the registry
int CSTATImageVerify::Initialise(const CString& path)
{
	int ret = ERROR_REGISTRY;
	TCHAR szFullPath[256 + 1];


	_tcsncpy(szFullPath, path.operator LPCTSTR(), path.GetLength() + 1);
	
	//append ref images directory to the stat installation directory as found in registry
	_tcscat(szFullPath, _T("\\Reference Images"));
	referenceimagedir = szFullPath;

	// try to create in case it doesn't exist
	CreateDirectory(referenceimagedir, NULL);

	// there may be old files to delete...
	_tcscat(szFullPath, _T("\\*.*"));
	CFileFind finder;
	if (finder.FindFile(szFullPath, 0))
	{
		int iWorking = 1;
		int filecount = 0;
		while (iWorking)
		{
			iWorking = finder.FindNextFile();

			// skip . and .. files
			if (finder.IsDots())
				continue;

			filecount++;
		}

		finder.Close();

		if (filecount)
			ret = REFDIR_FOUND;
		else
			ret = ITS_OK;
	}
	else
		ret = LOG_DIR_CREATE_FAILURE;
	
	
	
	return ret;
}

//----------------------------------------------------------------------------
//reference images copied over to local machine
int CSTATImageVerify::CopyReferenceImages(LPTSTR refimagelocation)
{
	int ret = GENERAL_FAILURE;

	// need to copy into null terminated string to have 2 nulls at end as required
	TCHAR szFrom[MAX_PATH + 1] = {0};
	_tcscpy(szFrom, refimagelocation);
	_tcscat(szFrom, _T("\\*.bmp"));

	// need to copy into null terminated string to have 2 nulls at end as required
	TCHAR szTo[MAX_PATH + 1] = {0};
	_tcscpy(szTo, referenceimagedir);

	//new file structure for copying files
	SHFILEOPSTRUCT fo = {0};
	fo.wFunc = FO_COPY;
	fo.pFrom = szFrom;
	fo.pTo = szTo;
	fo.fFlags = FOF_NOCONFIRMATION | FOF_SILENT;

	//copy new images across
	if(::SHFileOperation(&fo) != 0) //if fail file operation
		return ERR_FILE_COPY_FAILED;

	// check that at least 1 bitmap image got copied across
	_tcscat(szTo, _T("\\*.bmp"));
	CFileFind finder;
	ret = finder.FindFile(szTo, 0);
	finder.Close();
	if (!ret)
		return NO_BITMAPS;

	return ITS_OK;
}

//----------------------------------------------------------------------------
//local reference images deleted from local machine
int CSTATImageVerify::DeleteReferenceImages()
{
	// need to copy into null terminated string to have 2 nulls at end as required
	TCHAR szImages[MAX_PATH + 1] = {0};
	_tcscpy(szImages, referenceimagedir);
	_tcscat(szImages, _T("\\*.*"));
	
	//new file structure for deleting files
	SHFILEOPSTRUCT fo2 = {0};
	fo2.wFunc = FO_DELETE;
	fo2.pFrom = szImages;
	fo2.fFlags = FOF_NOCONFIRMATION | FOF_SILENT | FOF_FILESONLY | FOF_NOERRORUI;

	//delete
	if(::SHFileOperation(&fo2) != 0) //if fail file deletion
		return DELETEIMAGES_FAILURE;

	return ITS_OK;
}

