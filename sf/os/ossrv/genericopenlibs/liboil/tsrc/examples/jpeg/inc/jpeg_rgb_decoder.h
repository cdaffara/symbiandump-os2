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


#ifndef _JPEG_RGB_DECODER_H_
#define _JPEG_RGB_DECODER_H_

typedef struct jpeg_rgb_decoder_struct JpegRGBDecoder;


JpegRGBDecoder *jpeg_rgb_decoder_new(void);
void jpeg_rgb_decoder_free(JpegRGBDecoder *dec);
int jpeg_rgb_decoder_addbits(JpegRGBDecoder *dec, unsigned char *data, unsigned int len);
int jpeg_rgb_decoder_parse(JpegRGBDecoder *dec);
int jpeg_rgb_decoder_get_image(JpegRGBDecoder *dec,
	unsigned char **image, int *rowstride, int *width, int *height);



#endif

