// Copyright (c) 2007-2009 Nokia Corporation and/or its subsidiary(-ies).
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
//

#include <iostream>
#include <zlib.h>

using namespace std;

unsigned in OF((void FAR *in_desc, unsigned char FAR * FAR *in_buf))
    {
    in_desc = NULL;
    in_buf = NULL;
	return 0;
    }

int out OF((void FAR *out_desc, unsigned char FAR *out_buf, unsigned len))
    {
    out_desc = NULL;
    out_buf = NULL;
    len = 0;
	return 0;
    }

/**
@SYMTestCaseID       	SYSLIB-EZLIB2-CT-4311
@SYMTestCaseDesc     	To test that the all the zlib function calls can link against libz.dll and libzlib.a.
@SYMTestPriority     	High
@SYMTestActions      	Call every exported function in zlib.h.
@SYMTestExpectedResults The code should build with no errors.
*/
void LinkTest()
    {
	z_stream stream, copiedStream;
	Bytef bytefarray[1];
	gz_header header;
	unsigned char uchararray[1];
	uLongf *ulongfp = NULL;
	const char *constcharp = NULL;
	char *charp = NULL;
	gzFile file = NULL;
	z_off_t zofft = 0;
	int *intp = NULL;
	const Bytef *constbytefp = NULL;

	zlibVersion();
	deflateInit(&stream, Z_DEFAULT_COMPRESSION);
	deflateInit2(&stream, Z_DEFAULT_COMPRESSION, Z_DEFLATED, MAX_WBITS, MAX_MEM_LEVEL, Z_DEFAULT_STRATEGY);
	deflateSetDictionary(&stream, bytefarray, 1);
	deflateCopy(&copiedStream, &stream);
	deflateReset(&stream);
	deflateParams(&stream, Z_DEFAULT_COMPRESSION, Z_DEFAULT_STRATEGY);
	deflateTune(&stream, 1, 1, 1, 1);
	deflateBound(&stream, 1);
	deflatePrime(&stream, 0, 0);
	deflateSetHeader(&stream, &header);
	deflate(&stream, Z_NO_FLUSH);
	deflateEnd(&stream);
	inflateInit(&stream);
	inflateInit2(&stream, MAX_WBITS);
	inflateSetDictionary(&stream, bytefarray, 1);
	inflateSync(&stream);
	inflateCopy(&copiedStream, &stream);
	inflateReset(&stream);
	inflatePrime(&stream, 0, 0);
	inflateGetHeader(&stream, &header);
	inflate(&stream, Z_NO_FLUSH);
	inflateEnd(&stream);
	inflateBackInit(&stream, MAX_WBITS, uchararray);
	inflateBack(&stream, in, uchararray, out, uchararray);
	inflateBackEnd(&stream);
	zlibCompileFlags();
	compress(bytefarray, ulongfp, bytefarray, 1);
	compress2(bytefarray, ulongfp, bytefarray, 1, Z_DEFAULT_COMPRESSION);
	compressBound(1);
	uncompress(bytefarray, ulongfp, bytefarray, 1);
	gzopen(constcharp, constcharp);
	gzdopen(1, constcharp);
	gzsetparams(file, Z_DEFAULT_COMPRESSION, Z_DEFAULT_STRATEGY);
	gzread(file, uchararray, 1);
	gzwrite(file, constcharp, 0);
	gzprintf(file, constcharp);
	gzputs(file, constcharp);
	gzgets(file, charp, 0);
	gzputc(file, 0);
	gzgetc(file);
	gzungetc(0, file);
	gzflush(file, Z_NO_FLUSH);
	gzseek(file, zofft, 0);
	gzrewind(file);
	gztell(file);
	gzeof(file);
	gzdirect(file);
	gzclose(file);
	gzerror(file, intp);
	gzclearerr(file);
	adler32(0, constbytefp, 0);
	adler32_combine(0, 0, zofft);
	crc32(0, constbytefp, 0);
	crc32_combine(0, 0, zofft);
    }

int main()
    {
    LinkTest();

	return 0;
    }
