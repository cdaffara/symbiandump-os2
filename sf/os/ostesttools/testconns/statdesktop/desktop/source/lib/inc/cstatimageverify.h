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




#ifndef CSTATImageVerify_H
#define CSTATImageVerify_H

#include "CSTATLogFile.h"

#define VERIFY_MAX_IMAGES	250

//class to be used to store names of reference images in time ascending order
//Verification will then take place according to current location in the array
class CSTATReferenceImages
{
	public :
		CString completefilenamepath;
		ULONGLONG lCreationTime;
};

class CSTATImageVerify
{
	public :
		CSTATImageVerify(CSTATLogFile *pLog);
		~CSTATImageVerify();

		int Initialise(const CString& path);							// sets the location of the reference images
		int CopyReferenceImages(LPTSTR refimagelocation);	// copy new ref images
		int DeleteReferenceImages();						// remove existing reference images
		int EnableVerification(int fudge);					//will load ref file names into array and sort them accroding to creation time
		int VerifyImage(CString& lastscreenshot);			//image verification function (blitting)
		int IsActive() { return iImageCount; }				// are we using verification?
		void DisableVerification() { iImageCount = 0; }		// turn it off

	private :
		bool ImagesRemaining() { return (lastrefimageloaded < iImageCount); }
		bool LoadNewImage(CString& newimage);				//image loading for reference and new images
		bool LoadRefImage();								// ...
		
		CSTATReferenceImages refimagearray[250];			// array of images to verify against
		int lastrefimageloaded;								// position of last image
		int margin;											// percentage error allowed on images
		int iImageCount;									// total images for verification
		CString referenceimagedir;							// location of reference images
		CSTATLogFile *pLogFile;								// pointer to our logging class

		CPalette m_Palette;
		unsigned char *m_pDib, *m_pDib2, *m_pDibBits, *m_pDibBits2;
		DWORD m_dwDibSize, m_dwDibSize2;
		BITMAPINFOHEADER *m_pBIH, *m_pBIH2;
		RGBQUAD *m_pPalette, *m_pPalette2;
		int m_nPaletteEntries, m_nPaletteEntries2;
};

#endif
