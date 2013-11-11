
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
// Name        : ulibz_test.cpp
// 
//

 
#include "ulibz.h"
#include <zlib.h>


// ============================ MEMBER FUNCTIONS ===============================


/*
-------------------------------------------------------------------------------
Function Name		: CTestlibz::libzcomp_decomp()
API Tested			: Compress() and Decompress()

-------------------------------------------------------------------------------
*/

	
TInt CTestlibz::libzcomp_decomp()
	{
	__UHEAP_MARK;
    int err = 0;
	Byte compr[20];
	Byte uncompr[20];
	uLong comprLen;
	uLong uncomprLen;

	const char hello[] = "hello, hello!";

	comprLen = sizeof(compr);
	uncomprLen = sizeof(uncompr);
	
    uLong len = (uLong)strlen(hello)+1;

	INFO_PRINTF1(_L("compress()\n"));
    err = compress(compr, &comprLen, (const Bytef*)hello, len);
   	if(err != Z_OK)
		{
			INFO_PRINTF1(_L("compress failed\n"));
			return KErrGeneral;
		}

    strcpy((char*)uncompr, "garbage");

	INFO_PRINTF1(_L("uncompress()\n"));
    err = uncompress(uncompr, &uncomprLen, compr, comprLen);
    	if(err != Z_OK)
		{
			INFO_PRINTF1(_L("uncompress failed\n"));
			return KErrGeneral;
		}
    if (strcmp((char*)uncompr, hello)) 
    {
        INFO_PRINTF1(_L("Bad uncompress\n"));
       	return KErrGeneral;
    } 
 	__UHEAP_MARKEND;
	return KErrNone;
	}

	
/*
-------------------------------------------------------------------------------
Function Name		: CTestlibz::libzdefl_Infl()
API Tested			: deflate(), deflateInit(), deflateEnd().

-------------------------------------------------------------------------------
*/


TInt CTestlibz::libzdefl_Infl()
	{
	__UHEAP_MARK;
	int ret;
	Byte* compr;
	Byte* uncompr;
	
	uLong comprLen = 30;
	uLong uncomprLen = 30;
	
	compr = (Byte*)calloc((uInt)comprLen, 1);
	if(compr == NULL)
		{
		INFO_PRINTF1(_L("Could not allocate memory for compr."));
		return KErrNoMemory;
		}
	
	uncompr = (Byte*)calloc((uInt)uncomprLen, 1);	
	if(uncompr == NULL)
		{
		INFO_PRINTF1(_L("Could not allocate memory for uncompr."));
		free(compr);
		return KErrNoMemory;
		}
	
	ret = libzdeflate(compr, comprLen);
	if(ret == KErrGeneral)
	{
		free(compr);
		free(uncompr);
		return KErrGeneral;
	}
	ret = libzinflate(compr, comprLen, uncompr, uncomprLen);
	free(compr);
	free(uncompr);
	__UHEAP_MARKEND;
	return ret;

	}

/*
-------------------------------------------------------------------------------
Function Name		: CTestlibz::libzdeflate()
API Tested			: deflate(), deflateInit(), deflateEnd().

-------------------------------------------------------------------------------
*/

TInt CTestlibz::libzdeflate(Byte * compr, uLong comprLen)
	{
    z_stream c_stream; 			/* compression stream */

	int err;

	const char hello[] = "hello, hello!";

    uLong len = (uLong)strlen(hello)+1;

    c_stream.zalloc = (alloc_func)0;
    c_stream.zfree = (free_func)0;
    c_stream.opaque = (voidpf)0;

    err = deflateInit(&c_stream, Z_DEFAULT_COMPRESSION);
   	if(err != Z_OK)
		{
			INFO_PRINTF2(_L("deflateInit failed with err %d"), err);
			return KErrGeneral;
		}
    c_stream.next_in  = (Bytef*)hello;
    c_stream.next_out = compr;

    while (c_stream.total_in != len && c_stream.total_out < comprLen) 
    {
        c_stream.avail_in = c_stream.avail_out = 1; /* force small buffers */
        err = deflate(&c_stream, Z_NO_FLUSH);
   		if(err != Z_OK)
		{
			INFO_PRINTF2(_L("deflate failed with err %d"), err);
			return KErrGeneral;
		}
    }

    /* Finish the stream, still forcing small buffers: */
    for (;;) 
	{
        c_stream.avail_out = 1;
        err = deflate(&c_stream, Z_FINISH);
        if (err == Z_STREAM_END) 
			break;
     	if(err != Z_OK)
		{
			INFO_PRINTF2(_L("deflate failed with err %d"), err);
			return KErrGeneral;
		}
    }
	INFO_PRINTF1(_L("deflateEnd()\n"));
    err = deflateEnd(&c_stream);
    if(err != Z_OK)
		{
			INFO_PRINTF2(_L("deflateEnd failed with err %d"), err);
			return KErrGeneral;
		}
	return KErrNone;
	}
	

/*
-------------------------------------------------------------------------------
Function Name		: CTestlibz::libzinflate()
API Tested			: inflate(), inflateInit(),inflateEnd(). 

-------------------------------------------------------------------------------
*/

TInt CTestlibz::libzinflate(Byte * compr,uLong  comprLen, Byte *  uncompr, uLong uncomprLen)
	{
    z_stream d_stream; /* decompression stream */
	int err;
	const char hello[] = "hello, hello!";

    strcpy((char*)uncompr, "garbage");

    d_stream.zalloc = (alloc_func)0;
    d_stream.zfree = (free_func)0;
    d_stream.opaque = (voidpf)0;

    d_stream.next_in  = compr;
    d_stream.avail_in = 0;
    d_stream.next_out = uncompr;

    err = inflateInit(&d_stream);
   	if(err != Z_OK)
		{
			INFO_PRINTF2(_L("inflateInit failed with err %d"), err);
			return KErrGeneral;
		}

    while (d_stream.total_out < uncomprLen && d_stream.total_in < comprLen) {
        d_stream.avail_in = d_stream.avail_out = 1; /* force small buffers */
        err = inflate(&d_stream, Z_NO_FLUSH);
        if (err == Z_STREAM_END) 
			break;
	   	if(err != Z_OK)
		{
			INFO_PRINTF2(_L("inflate failed with err %d"), err);
			return KErrGeneral;
		}
    }

    err = inflateEnd(&d_stream);
   	if(err != Z_OK)
	{
		INFO_PRINTF2(_L("inflateEnd failed with err %d"), err);
		return KErrGeneral;
	}
    if (strcmp((char*)uncompr, hello)) 
	{
		INFO_PRINTF1(_L("Bad Inflate\n"));
		return KErrGeneral;
    } 
    return KErrNone;
	}
	
/*
-------------------------------------------------------------------------------
Function Name		: CTestlibz::libzgzio()
API Tested			: gzopen(), gzputc(), gzprintf(), gzseek(), gzclose(), 
					  gzread(), gzgetc(), gzungetc(), gzgets(), gzputs(), 
					  gzerror(), gztell().

-------------------------------------------------------------------------------
*/

TInt CTestlibz::libzgzio()
	{
	__UHEAP_MARK;
#ifdef NO_GZCOMPRESS
    INFO_PRINT1(_L("NO_GZCOMPRESS -- gz* functions cannot compress"));
	return KErrNone;
#else
	const char Buffer[] = "Symbian Libz!";
	Byte uncompr[15];
	char fname[] = "C:\\Libz\\Test1\\bye.gz";
	int err = 0;
	int len;
    gzFile file;
    z_off_t pos;

	err = iRfile.Create(iRfs, KGZFILE, EFileShareAny);
	if( err != KErrNone && err != KErrAlreadyExists )
		{
		INFO_PRINTF1(_L("File create successfully\n"));
		return KErrGeneral;
		}

	len = (int)strlen(Buffer)+1;

	//-------------gzopen()------------------
	INFO_PRINTF1(_L("gzopen()\n"));
	file = gzopen(fname, "wb");
    if (file == NULL) {
        INFO_PRINTF1(_L("gzopen error"));
        return KErrGeneral;
    }

	//-------------gzputc() ------------
	INFO_PRINTF1(_L("gputc()\n"));
    gzputc(file, 'S');
    if (gzputs(file, "ymbian") != 6) {
        INFO_PRINTF2(_L("gzputs err: %s\n"), gzerror(file, &err));
        return KErrGeneral;
    }
	//-------------gzprintf() ------------
	INFO_PRINTF1(_L("gzprintf()\n"));
    if (gzprintf(file, " %s!", "Libz") != 6) {
        INFO_PRINTF2(_L("gzprintf err: %s\n"), gzerror(file, &err));
        return KErrGeneral;
    }
	//-------------gzseek() ------------
	INFO_PRINTF1(_L("gzseek()\n"));
    if(gzseek(file, 1L, SEEK_CUR) != 14){ /* add one zero byte */
		INFO_PRINTF2(_L("gzseek err: %s\n"), gzerror(file, &err));
		return KErrGeneral;
	}
	//-------------gzclose() ------------
	INFO_PRINTF1(_L("gzclose()\n"));
    if(gzclose(file) == Z_ERRNO){
		INFO_PRINTF2(_L("gzclose err: %s\n"), gzerror(file, &err));
		return KErrGeneral;
	}

	//-------------gzopen()------------------
	INFO_PRINTF1(_L("gzopen()\n"));
	file = gzopen(fname, "rb");
    if (file == NULL) {
        INFO_PRINTF1(_L("gzopen error\n"));
        return KErrGeneral;
    }
	strcpy((char*)uncompr, "garbage");

	//-------------gzread()------------------
	INFO_PRINTF1(_L("gzread()\n"));
    if (gzread(file, uncompr, sizeof(uncompr)) != len) {
        INFO_PRINTF2(_L("gzread err: %s\n"), gzerror(file, &err));
        return KErrGeneral;
    }

    if (strcmp((char*)uncompr, Buffer)) 
    {
        INFO_PRINTF2(_L("bad gzread: %s\n"), (char*)uncompr);
 		return KErrGeneral;
    }

	//-------------gzseek() & gztell()-----------------
	INFO_PRINTF1(_L("gzseek & gztell()\n"));
    pos = gzseek(file, -7L, SEEK_CUR);
    if (gztell(file) != pos || pos != 7) 
    {
       INFO_PRINTF3(_L("gzseek error, pos=%ld, gztell=%ld\n"), (long)pos, (long)gztell(file));
       return KErrGeneral;
    }

	//-------------gzgetc()------------------
	INFO_PRINTF1(_L("gzgetc()\n"));
    if (gzgetc(file) != ' ') 
    {
        INFO_PRINTF1(_L("gzgetc error"));
        return KErrGeneral;
      
    }
    
	//-------------gzungetc()------------------
	INFO_PRINTF1(_L("gzungetc\n"));
    if (gzungetc(' ', file) != ' ') 
    {
        INFO_PRINTF1(_L("gzungetc error\n"));
        return KErrGeneral;
    }

	//-------------gzgets()------------------
	INFO_PRINTF1(_L("gzgets()\n"));
    gzgets(file, (char*)uncompr, sizeof(uncompr)); 
    if (strlen((char*)uncompr) != 6) 
    { 
    	/* " Libz!" */
        INFO_PRINTF2(_L("gzgets err after gzseek: %s\n"), gzerror(file, &err));
        return KErrGeneral;
    }
    
    if (strcmp((char*)uncompr, Buffer + 7)) 
    {
        INFO_PRINTF1(_L("bad gzgets after gzseek\n"));
        return KErrGeneral;
    }

	//-------------gzclose() ------------
    if(gzclose(file) == Z_ERRNO)
    {
		INFO_PRINTF2(_L("gzclose err: %s\n"), gzerror(file, &err));
		return KErrGeneral;
	}
#endif
	__UHEAP_MARKEND;
	return KErrNone;
	}
	


/*
-------------------------------------------------------------------------------
Function Name		: CTestlibz::libzversion()
API Tested			: zlibversion()

-------------------------------------------------------------------------------
*/

TInt CTestlibz::libzversion()
	{
	__UHEAP_MARK;
	char buf[]="1.2.3";
	INFO_PRINTF1(_L("zlibVersion()\n"));
	if( (strcmp( buf, zlibVersion() ) != 0) ){
		INFO_PRINTF1(_L("zlibversion failed\n"));
		return KErrGeneral;
		}
	__UHEAP_MARKEND;
	return KErrNone;
	}
	
	
	
	

