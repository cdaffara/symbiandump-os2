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
// Name        : tzlibcases.cpp
// 
//
#include <errno.h>
#include "tzlib.h"

#define CHECK_ERR(err, msg) { \
    if (err != Z_OK) { \
       INFO_PRINTF2(_L("Error: %d"), err); \
        return err; \
    } \
}

// -----------------------------------------------------------------------------
//Function Name :ReadStringParam
//Description:Reads string from ini file
//Param : aString is populated with the read string
// -----------------------------------------------------------------------------	
void CTestZlib::ReadStringParam(char* aString)
	{
	_LIT( Kparam, "Param%d" );
	TBuf<100> pNameBuf;
	TPtrC descriptor;
	TInt i;

	pNameBuf.Format (Kparam, ++iParamCnt);
	TBool ret = GetStringFromConfig (ConfigSection (), pNameBuf, descriptor);
	if ( descriptor == _L("\"\""))
		{
		i = 0;
		}
	else
		{
		// If the string is quoted, take only the insides
		if ( (descriptor[0] == '\"') && (descriptor[descriptor.Length()-1] == '\"'))
			{
			for (i=0; i<descriptor.Length ()-2; i++)
				{
				aString[i]=descriptor[i+1];
				}
			}
		// Otherwise,take the whole string
		else
			{
			for (i=0; i<descriptor.Length (); i++)
				{
				aString[i]=descriptor[i];
				}
			}
		}

	aString[i]='\0';
	}

// -----------------------------------------------------------------------------
//Function Name : ReadIntParam
//TestCase Description:Reads Int value from ini file
//Param : TInt to receive the read integer 
// -----------------------------------------------------------------------------
void CTestZlib::ReadIntParam(TInt &aInt)
	{
	_LIT( Kparam, "Param%d" );
	TBuf<8> pNameBuf;
	TPtrC string;
	pNameBuf.Format (Kparam, ++iParamCnt);
	TBool res = GetIntFromConfig (ConfigSection (), pNameBuf, aInt);
	}

//---------------/*COMPRESS AND UNCOMPRESS*/----------------------------------

TInt CTestZlib::sec_compress(Byte * compr, uLong comprLen, Byte * uncompr,
		uLong uncomprLen)
	{

	int err;
	const char hello[] = "hello, hello!";
	uLong len = (uLong)strlen(hello)+1;

	err = compress (compr, &comprLen, (const Bytef*)hello, len);
	CHECK_ERR(err, "compress");

	strcpy ((char*)uncompr, "garbage");

	err = uncompress (uncompr, &uncomprLen, compr, comprLen);
	CHECK_ERR(err, "uncompress");

	if ( strcmp ((char*)uncompr, hello))
		{
		printf ("Bad uncompress");
		return KErrGeneral;
		}
	else
		{
		printf ("uncompress(): %s", (char *)uncompr);
		}
	return err;
	}

//------------------------------//deflateSetDictionary//-----------------------------------

TInt CTestZlib::sec_deflateSetDictionary01(Byte * compr, uLong comprLen,
		TInt flush, TInt compression)
	{
	z_stream c_stream; // compression stream 
	int err;
	const char hello[] = "hello, hello!";
	uLong len = (uLong)strlen(hello)+1;
	const Bytef* dictionary=(const Bytef *) hello;
	c_stream.zalloc = (alloc_func)0;
	c_stream.zfree = (free_func)0;
	c_stream.opaque = (voidpf)0;

	err = deflateInit (&c_stream, compression);// Z_DEFAULT_COMPRESSION);

	if ( err != Z_OK)
		{
		INFO_PRINTF2(_L("deflateInit error: %d"), err);
		return err;
		}
	err = deflateSetDictionary (&c_stream, dictionary, 3);
	if ( err != Z_OK)
		{
		ERR_PRINTF2(_L("deflateSetDictionary error: %d"), err);
		deflateEnd (&c_stream);
		return err;
		}
	c_stream.next_in = (Bytef*)hello;
	c_stream.next_out = compr;

	while (c_stream.total_in != len && c_stream.total_out < comprLen)
		{
		c_stream.avail_in = c_stream.avail_out = 1; //* force small buffers 
		err = deflate (&c_stream, flush);
		if ( err != Z_OK)
			{
			INFO_PRINTF2(_L("deflate return code: %d"), err);
			deflateEnd (&c_stream);
			return err;
			}
		}
	// Finish the stream, still forcing small buffers: 
	for (;;)
		{
		c_stream.avail_out = 1;
		err = deflate (&c_stream, Z_FINISH);
		if ( err == Z_STREAM_END)
			break;
		if ( err != Z_OK)
			{
			INFO_PRINTF2(_L("deflate error: %d"), err);
			deflateEnd (&c_stream);
			return err;
			}
		}

	deflateEnd (&c_stream);
	return KErrNone;
	}

TInt CTestZlib::sec_deflateSetDictionary02(TInt compression)
	{
	z_stream c_stream; // compression stream 
	int err;
	const char hello[] = "hello, hello!";
	uLong len = (uLong)strlen(hello)+1;
	const Bytef* dictionary=NULL;
	c_stream.zalloc = (alloc_func)0;
	c_stream.zfree = (free_func)0;
	c_stream.opaque = (voidpf)0;

	err = deflateInit (&c_stream, compression);// Z_DEFAULT_COMPRESSION);

	if ( err != Z_OK)
		{
		INFO_PRINTF2(_L("deflateInit error: %d"), err);
		return err;
		}
	err = deflateSetDictionary (&c_stream, dictionary, 30);
	if ( err != Z_OK)
		{
		ERR_PRINTF2(_L("deflateSetDictionary error: %d"), err);
		deflateEnd (&c_stream);
		return err;
		}
	deflateEnd (&c_stream);
	return KErrNone;
	}

//------------------------------//deflateSetDictionary//-----------------------------------

TInt CTestZlib::sec_deflateSetDictionary03(Byte * compr, uLong comprLen,
		TInt flush, TInt compression)
	{
	z_stream c_stream; // compression stream 
	int err;
	const char hello[] = "hello, hello!";
	uLong len = (uLong)strlen(hello)+1;
	const Bytef* dictionary=(const Bytef *) hello;
	c_stream.zalloc = (alloc_func)0;
	c_stream.zfree = (free_func)0;
	c_stream.opaque = (voidpf)0;

	err = deflateInit (&c_stream, compression);// Z_DEFAULT_COMPRESSION);

	if ( err != Z_OK)
		{
		INFO_PRINTF2(_L("deflateInit error: %d"), err);
		return err;
		}
	c_stream.next_in = (Bytef*)hello;
	c_stream.next_out = compr;

	while (c_stream.total_in != len && c_stream.total_out < comprLen)
		{
		c_stream.avail_in = c_stream.avail_out = 1; //* force small buffers 
		err = deflate (&c_stream, flush);
		if ( err != Z_OK)
			{
			INFO_PRINTF2(_L("deflate return code: %d"), err);
			deflateEnd (&c_stream);
			return err;
			}
		}
	// Finish the stream, still forcing small buffers: 
	for (;;)
		{
		c_stream.avail_out = 1;
		err = deflate (&c_stream, Z_FINISH);
		if ( err == Z_STREAM_END)
			break;
		if ( err != Z_OK)
			{
			INFO_PRINTF2(_L("deflate error: %d"), err);
			deflateEnd (&c_stream);
			return err;
			}
		}

	deflateEnd (&c_stream);
	err = deflateSetDictionary (&c_stream, dictionary, 30);
	if ( err != Z_OK)
		{
		ERR_PRINTF2(_L("deflateSetDictionary error: %d"), err);
		return err;
		}

	return KErrNone;
	}

TInt CTestZlib::sec_deflateSetDictionary04(Byte * compr, uLong comprLen,
		TInt flush, TInt compression)
	{
	z_stream c_stream; // compression stream 
	int err;
	const char hello[] = "hello, hello!";
	uLong len = (uLong)strlen(hello)+1;
	const char dict[] = "z";

	const Bytef* dictionary=(const Bytef *) dict;
	c_stream.zalloc = (alloc_func)0;
	c_stream.zfree = (free_func)0;
	c_stream.opaque = (voidpf)0;

	err = deflateInit (&c_stream, compression);// Z_DEFAULT_COMPRESSION);

	if ( err != Z_OK)
		{
		INFO_PRINTF2(_L("deflateInit error: %d"), err);
		return err;
		}
	err = deflateSetDictionary (&c_stream, dictionary, (uLong)strlen(dict)+1);
	if ( err != Z_OK)
		{
		ERR_PRINTF2(_L("deflateSetDictionary error: %d"), err);
		deflateEnd (&c_stream);
		return err;
		}
	c_stream.next_in = (Bytef*)hello;
	c_stream.next_out = compr;

	while (c_stream.total_in != len && c_stream.total_out < comprLen)
		{
		c_stream.avail_in = c_stream.avail_out = 1; //* force small buffers 
		err = deflate (&c_stream, flush);
		if ( err != Z_OK)
			{
			INFO_PRINTF2(_L("deflate return code: %d"), err);
			deflateEnd (&c_stream);
			return err;
			}
		}
	// Finish the stream, still forcing small buffers: 
	for (;;)
		{
		c_stream.avail_out = 1;
		err = deflate (&c_stream, Z_FINISH);
		if ( err == Z_STREAM_END)
			break;
		if ( err != Z_OK)
			{
			INFO_PRINTF2(_L("deflate error: %d"), err);
			deflateEnd (&c_stream);
			return err;
			}
		}

	deflateEnd (&c_stream);
	return KErrNone;
	}

TInt CTestZlib::sec_deflateSetDictionary05(Byte * compr, uLong comprLen,
		TInt flush, TInt compression)
	{
	z_stream c_stream; // compression stream 
	int err;
	const char hello[] = "hello, hello!";
	uLong len = (uLong)strlen(hello)+1;
	const char
			dict[] = "abcdefghijklmnopqrstuvwxyz \
    abcdefghijklmnopqrstuvwxyz \
    abcdefghijklmnopqrstuvwxyz \
    abcdefghijklmnopqrstuvwxyz \
    abcdefghijklmnopqrstuvwxyz \
    abcdefghijklmnopqrstuvwxyz \
    abcdefghijklmnopqrstuvwxyz \
    abcdefghijklmnopqrstuvwxyz \
    abcdefghijklmnopqrstuvwxyz \
    abcdefghijklmnopqrstuvwxyz";

	const Bytef* dictionary=(const Bytef *) dict;
	c_stream.zalloc = (alloc_func)0;
	c_stream.zfree = (free_func)0;
	c_stream.opaque = (voidpf)0;

	err = deflateInit (&c_stream, compression);// Z_DEFAULT_COMPRESSION);

	if ( err != Z_OK)
		{
		INFO_PRINTF2(_L("deflateInit error: %d"), err);
		return err;
		}
	err = deflateSetDictionary (&c_stream, dictionary, (uLong)strlen(dict)+1);
	if ( err != Z_OK)
		{
		ERR_PRINTF2(_L("deflateSetDictionary error: %d"), err);
		deflateEnd (&c_stream);
		return err;
		}
	c_stream.next_in = (Bytef*)hello;
	c_stream.next_out = compr;

	while (c_stream.total_in != len && c_stream.total_out < comprLen)
		{
		c_stream.avail_in = c_stream.avail_out = 1; //* force small buffers 
		err = deflate (&c_stream, flush);
		if ( err != Z_OK)
			{
			INFO_PRINTF2(_L("deflate return code: %d"), err);
			deflateEnd (&c_stream);
			return err;
			}
		}
	// Finish the stream, still forcing small buffers: 
	for (;;)
		{
		c_stream.avail_out = 1;
		err = deflate (&c_stream, Z_FINISH);
		if ( err == Z_STREAM_END)
			break;
		if ( err != Z_OK)
			{
			INFO_PRINTF2(_L("deflate error: %d"), err);
			deflateEnd (&c_stream);
			return err;
			}
		}

	deflateEnd (&c_stream);
	return KErrNone;
	}
//------------------------------//DEFLATE//-----------------------------------

TInt CTestZlib::sec_deflate01( Byte * compr, uLong comprLen, TInt flush,
		TInt compression)
	{
	z_stream c_stream; // compression stream 
	int err;
	const char hello[] = "hello, hello!";
	uLong len = (uLong)strlen(hello)+1;

	c_stream.zalloc = (alloc_func)0;
	c_stream.zfree = (free_func)0;
	c_stream.opaque = (voidpf)0;

	err = deflateInit (&c_stream, compression);// Z_DEFAULT_COMPRESSION);

	if ( err != Z_OK)
		{
		INFO_PRINTF2(_L("deflateInit error: %d"), err);
		return err;
		}

	c_stream.next_in = (Bytef*)hello;
	c_stream.next_out = compr;

	while (c_stream.total_in != len && c_stream.total_out < comprLen)
		{
		c_stream.avail_in = c_stream.avail_out = 1; //* force small buffers 
		err = deflate (&c_stream, flush);
		if ( err != Z_OK)
			{
			INFO_PRINTF2(_L("deflate return code: %d"), err);
			deflateEnd (&c_stream);
			return err;
			}
		}
	// Finish the stream, still forcing small buffers: 
	for (;;)
		{
		c_stream.avail_out = 1;
		err = deflate (&c_stream, Z_FINISH);
		if ( err == Z_STREAM_END)
			break;
		if ( err != Z_OK)
			{
			INFO_PRINTF2(_L("deflate error: %d"), err);
			deflateEnd (&c_stream);
			return err;
			}
		}

	deflateEnd (&c_stream);
	return KErrNone;
	}

TInt CTestZlib::sec_deflate02( Byte * compr, uLong comprLen, TInt flush,
		TInt compression)
	{
	z_stream c_stream; // compression stream 
	int err;
	const char hello[] = "hello, hello!";
	uLong len = (uLong)strlen(hello)+1;

	c_stream.zalloc = (alloc_func)0;
	c_stream.zfree = (free_func)0;
	c_stream.opaque = (voidpf)0;

	err = deflateInit (&c_stream, compression);// Z_DEFAULT_COMPRESSION);

	if ( err != Z_OK)
		{
		INFO_PRINTF2(_L("deflateInit error: %d"), err);
		return err;
		}

	c_stream.next_in = (Bytef*)hello;
	c_stream.next_out = compr;

	while (c_stream.total_in != len && c_stream.total_out < comprLen)
		{
		c_stream.avail_in = c_stream.avail_out = 1; //* force small buffers 
		err = deflate (&c_stream, Z_NO_FLUSH);
		if ( err != Z_OK)
			{
			INFO_PRINTF2(_L("deflate return code: %d"), err);
			deflateEnd (&c_stream);
			return err;
			}
		}
	// Finish the stream, still forcing small buffers: 
	for (;;)
		{
		c_stream.avail_out = 1;
		err = deflate (&c_stream, Z_FINISH);
		if ( err == Z_STREAM_END)
			break;
		if ( err != Z_OK)
			{
			INFO_PRINTF2(_L("deflate error: %d"), err);
			deflateEnd (&c_stream);
			return err;
			}
		}

	//deflate call after a finish
	err = deflate (&c_stream, flush);
	deflateEnd (&c_stream);
	return err;
	}
//-------------------------------/INFLATE/----------------------------------

TInt CTestZlib::sec_inflate( Byte * compr, uLong comprLen, Byte * uncompr,
		uLong uncomprLen, TInt flush)
	{
	int err;
	const char hello[] = "hello, hello!";
	z_stream d_stream; // decompression stream 

	//strcpy((char*)uncompr, "garbage");

	d_stream.zalloc = (alloc_func)0;
	d_stream.zfree = (free_func)0;
	d_stream.opaque = (voidpf)0;

	d_stream.next_in = compr;
	d_stream.avail_in = 0;
	d_stream.next_out = uncompr;

	err = inflateInit (&d_stream);
	CHECK_ERR(err, "inflateInit");

	while (d_stream.total_out < uncomprLen && d_stream.total_in < comprLen)
		{
		d_stream.avail_in = d_stream.avail_out = 1; // force small buffers 
		err = inflate (&d_stream, flush);
		if ( err == Z_STREAM_END)
			break;
		if ( err != Z_OK)
			{
			inflateEnd (&d_stream);
			return err;
			}
		}

	err = inflateEnd (&d_stream);
	CHECK_ERR(err, "inflateEnd");

	if ( strcmp ((char*)uncompr, hello))
		{
		ERR_PRINTF1(_L("Bad inflate"));
		return KErrGeneral;
		}
	else
		{
		INFO_PRINTF1(_L("inflate success"));
		}
	return 0;
	}

//256K buffer size to hold streams
TInt CTestZlib::sec_inflate_large_buf( Byte * compr, uLong comprLen,
		Byte * uncompr, uLong uncomprLen, TInt flush)
	{
	int err;
	const char hello[] = "hello, hello!";
	z_stream d_stream; // decompression stream 

	//strcpy((char*)uncompr, "garbage");

	d_stream.zalloc = (alloc_func)0;
	d_stream.zfree = (free_func)0;
	d_stream.opaque = (voidpf)0;

	d_stream.next_in = compr;
	d_stream.avail_in = 0;
	d_stream.next_out = uncompr;

	err = inflateInit (&d_stream);
	CHECK_ERR(err, "inflateInit");

	while (d_stream.total_out < uncomprLen && d_stream.total_in < comprLen)
		{
		d_stream.avail_in = d_stream.avail_out = 262144; // 256K
		err = inflate (&d_stream, flush);
		if ( err == Z_STREAM_END)
			break;
		if ( err != Z_OK)
			{
			inflateEnd (&d_stream);
			return err;
			}
		}

	err = inflateEnd (&d_stream);
	CHECK_ERR(err, "inflateEnd");

	if ( strcmp ((char*)uncompr, hello))
		{
		ERR_PRINTF1(_L("Bad inflate"));
		return KErrGeneral;
		}
	else
		{
		INFO_PRINTF1(_L("inflate success"));
		}
	return 0;
	}

//----------------------------//GZ OPERATION//--------------------------------

TInt CTestZlib::sec_gzio( const char *fname, Byte * uncompr, uLong uncomprLen)
	{
#ifdef NO_GZCOMPRESS
	ERR_PRINTF1(_L("NO_GZCOMPRESS -- gz* functions cannot compress"));
#else
	const char hello[] = "hello, hello!";
	int len = (int)strlen(hello)+1;
	gzFile file;
	z_off_t pos;

	file = gzopen (fname, "wb");
	if ( file == NULL)
		{
		ERR_PRINTF1(_L("gzopen error"));
		return KErrGeneral;
		}
	gzputc (file, 'h');
	if ( gzputs (file, "ello")!= 4)
		{
		gzclose (file);
		ERR_PRINTF1(_L("gzputs err"));
		return KErrGeneral;
		}
	if ( gzprintf (file, ", %s!", "hello")!= 8)
		{
		gzclose (file);
		ERR_PRINTF1(_L("gzprintf err:"));
		return KErrGeneral;
		}

	gzseek (file, 1L, SEEK_CUR); // add one zero byte 
	gzclose (file);

	file = gzopen (fname, "rb");
	if ( file == NULL)
		{
		ERR_PRINTF1(_L("gzopen error"));
		return KErrGeneral;
		}
	strcpy ((char*)uncompr, "garbage");

	if ( gzread (file, uncompr, (unsigned)uncomprLen)!= len)
		{
		gzclose (file);
		ERR_PRINTF1(_L("gzread err"));
		return KErrGeneral;
		}

	if ( strcmp ((char*)uncompr, hello))
		{
		gzclose (file);
		ERR_PRINTF1(_L("bad gzread"));
		return KErrGeneral;
		}

	pos = gzseek (file, -8L, SEEK_CUR);
	if ( pos != 6 || gztell (file)!= pos)
		{
		gzclose (file);
		ERR_PRINTF3(_L("gzseek error, pos=%ld, gztell=%ld"),(long)pos, (long)gztell(file));
		return KErrGeneral;
		}

	if ( gzgetc (file)!= ' ')
		{
		gzclose (file);
		ERR_PRINTF1(_L("gzgetc error"));
		return KErrGeneral;
		}

	if ( gzungetc (' ', file)!= ' ')
		{
		gzclose (file);
		ERR_PRINTF1(_L("gzungetc error"));
		return KErrGeneral;
		}

	gzgets (file, (char*)uncompr, (int)uncomprLen);

	if ( strlen ((char*)uncompr)!= 7)
		{
		gzclose (file);
		// " hello!" 
		ERR_PRINTF1(_L("gzgets err after gzseek"));
		return KErrGeneral;
		}

	if ( strcmp ((char*)uncompr, hello + 6))
		{
		gzclose (file);
		ERR_PRINTF1(_L("bad gzgets after gzseek"));
		return KErrGeneral;
		}

	gzclose (file);
#endif
	return KErrNone;
	}

TInt CTestZlib::Test_zlibVersion()
	{
	INFO_PRINTF1(_L("Zlib Test zlibVersion"));
	int retVal = 0;

	const char *version = zlibVersion ();
	if ( strcmp (ZLIB_VERSION, version)== 0)
		{
		INFO_PRINTF1(_L("Returned version matches!"));
		retVal=KErrNone;
		}

	else
		{
		ERR_PRINTF1(_L("Return version mismatch!"));
		retVal=KErrGeneral;
		}
	return retVal;
	}

TInt CTestZlib::Test_compress01()
	{
	INFO_PRINTF1(_L("Zlib Test compress"));
	int retVal = 0;

	Byte *comp, *uncomp;
	uLong compLen, uncompLen;
	compLen = uncompLen = 30;

	comp = (Byte*)calloc((uInt)compLen, 1);
	if ( comp == NULL)
		{
		INFO_PRINTF1(_L("Could not allocate memory for comp."));
		return KErrNoMemory;
		}
	uncomp = (Byte*)calloc((uInt)uncompLen, 1);
	if ( uncomp == NULL)
		{
		INFO_PRINTF1(_L("Could not allocate memory for uncomp."));
		free (comp);
		return KErrNoMemory;
		}

	retVal = sec_compress (comp, compLen, uncomp, uncompLen);
	free (comp);
	free (uncomp);
	return retVal;
	}

// Test deflate - normal flow
TInt CTestZlib::Test_deflate01()
	{
	INFO_PRINTF1(_L("Zlib Test deflate"));
	int retVal = KErrGeneral;
	TInt flush, compression, expRet;

	Byte *comp;
	uLong compLen;
	compLen = 30;
	comp = (Byte*)calloc((uInt)compLen, 1);

	ReadIntParam (flush);
	ReadIntParam (compression);
	ReadIntParam (expRet);

	retVal = sec_deflate01 (comp, compLen, flush, compression);
	free (comp);
	if ( retVal != expRet)
		{
		ERR_PRINTF3(_L("Expected %d, returned %d"),expRet,retVal);
		retVal= KErrGeneral;
		}
	else
		{
		INFO_PRINTF1(_L("Test passes!"));
		retVal= KErrNone;
		}
	return retVal;
	}

//Negative test - 
TInt CTestZlib::Test_deflate02()
	{
	INFO_PRINTF1(_L("Zlib Test deflate"));
	int retVal = KErrGeneral;
	TInt flush, compression, expRet;

	Byte *comp;
	uLong compLen;
	compLen = 30;
	comp = (Byte*)calloc((uInt)compLen, 1);

	ReadIntParam (flush);
	ReadIntParam (compression);
	ReadIntParam (expRet);

	retVal = sec_deflate02 (comp, compLen, flush, compression);
	free (comp);
	if ( retVal != expRet)
		{
		ERR_PRINTF3(_L("Expected %d, returned %d"),expRet,retVal);
		retVal= KErrGeneral;
		}
	else
		{
		INFO_PRINTF1(_L("Test passes!"));
		retVal= KErrNone;
		}
	return retVal;
	}

// Test deflate small output buffer
TInt CTestZlib::Test_deflateEnd()
	{
	INFO_PRINTF1(_L("Zlib Test deflateEnd"));

	Byte *comp;
	uLong compLen;
	compLen = 30;

	comp = (Byte*)calloc((uInt)compLen, 1);

	z_stream c_stream; // compression stream 
	int err;
	const char hello[] = "hello, hello!";
	uLong len = (uLong)strlen(hello)+1;

	c_stream.zalloc = (alloc_func)0;
	c_stream.zfree = (free_func)0;
	c_stream.opaque = (voidpf)0;

	err = deflateInit (&c_stream, Z_DEFAULT_COMPRESSION);
	if ( err != Z_OK)
		{
		INFO_PRINTF2(_L("deflateInit failed: %d"), err);
		free (comp);
		return err;
		}

	c_stream.next_in = (Bytef*)hello;
	c_stream.next_out = comp;
	err = deflateEnd (&c_stream);
	if ( err != Z_OK)
		{
		ERR_PRINTF2(_L("deflateEnd failed: %d"), err);
		free (comp);
		return err;
		}
	if ( c_stream.state != NULL)
		{
		ERR_PRINTF1(_L("Stream state expected NULL"));
		free (comp);
		return err;
		}
	free (comp);
	return KErrNone;
	}

TInt CTestZlib::Test_inflate01()
	{
	INFO_PRINTF1(_L("Zlib Test inflate. Positive test"));
	int retVal = KErrGeneral;
	TInt flush, compression, expRet;
	Byte *comp, *uncomp;
	uLong compLen, uncompLen;
	compLen = uncompLen = 60;

	comp = (Byte*)calloc((uInt)compLen, 1);
	//comp    = (Byte*)malloc(compLen);
	uncomp = (Byte*)calloc((uInt)uncompLen, 1);
	//uncomp  = (Byte*)malloc(uncompLen);

	ReadIntParam (flush);
	ReadIntParam (compression);
	ReadIntParam (expRet);

	retVal = sec_deflate01 (comp, compLen, flush, compression);
	if ( retVal != KErrNone)
		{
		ERR_PRINTF3(_L("Expected %d, returned %d"),KErrNone,retVal);
		retVal= KErrGeneral;
		}
	retVal = sec_inflate (comp, compLen, uncomp, uncompLen, flush);
	if ( retVal != expRet)
		{
		ERR_PRINTF3(_L("Expected %d, returned %d"),expRet,retVal);
		retVal= KErrGeneral;
		}
	else
		{
		INFO_PRINTF1(_L("Test passes!"));
		retVal= KErrNone;
		}
	free (comp);
	free (uncomp);
	return retVal;
	}

//Negative test - invalid deflate data - return Z_DATA_ERROR
TInt CTestZlib::Test_inflate02()
	{
	INFO_PRINTF1(_L("Zlib Test inflate with invalide deflate data"));
	int retVal = KErrGeneral;
	TInt flush, compression, expRet;
	Byte *comp, *uncomp;
	uLong compLen, uncompLen;
	compLen = uncompLen = 30;

	comp = (Byte*)calloc((uInt)compLen, 1);
	//comp    = (Byte*)malloc(compLen);
	uncomp = (Byte*)calloc((uInt)uncompLen, 1);
	//uncomp  = (Byte*)malloc(uncompLen);

	ReadIntParam (flush);
	ReadIntParam (compression);
	ReadIntParam (expRet);

	retVal = sec_inflate (comp, compLen, uncomp, uncompLen, flush);
	if ( retVal != expRet)
		{
		ERR_PRINTF3(_L("Expected %d, returned %d"),expRet,retVal);
		retVal= KErrGeneral;
		}
	else
		{
		INFO_PRINTF1(_L("Test passes!"));
		retVal= KErrNone;
		}
	free (comp);
	free (uncomp);
	return retVal;
	}

// uncompressed data buffer inititalized to NULL
TInt CTestZlib::Test_inflate03()
	{
	INFO_PRINTF1(_L("Zlib Test inflate with NULL uncompressed data buffer"));
	int retVal = KErrGeneral;
	TInt flush, compression, expRet;
	Byte *comp=NULL, *uncomp=NULL;
	uLong compLen, uncompLen;
	compLen = uncompLen = 30;

	comp = (Byte*)calloc((uInt)compLen, 1);
	//uncomp  = (Byte*)calloc((uInt)uncompLen, 1); //Do not alloc for uncompressed data

	ReadIntParam (flush);
	ReadIntParam (compression);
	ReadIntParam (expRet);

	retVal = sec_deflate01 (comp, compLen, flush, compression);
	if ( retVal != KErrNone)
		{
		ERR_PRINTF3(_L("Expected %d, returned %d"),KErrNone,retVal);
		retVal= KErrGeneral;
		}
	retVal = sec_inflate (comp, compLen, uncomp, uncompLen, flush);
	if ( retVal != expRet)
		{
		ERR_PRINTF3(_L("Expected %d, returned %d"),expRet,retVal);
		retVal= KErrGeneral;
		}
	else
		{
		INFO_PRINTF1(_L("Test passes!"));
		retVal= KErrNone;
		}
	free (comp); //free(uncomp);
	return retVal;
	}

//Not enough buffer size for uncompresed data
TInt CTestZlib::Test_inflate04()
	{
	INFO_PRINTF1(_L("Zlib Test inflatewith not enough buffer size for uncompressed data"));
	int retVal = KErrGeneral;
	TInt flush, compression, expRet;
	Byte *comp=NULL, *uncomp=NULL;
	uLong compLen, uncompLen;
	compLen = 30;
	uncompLen = 5;

	comp = (Byte*)calloc((uInt)compLen, 1);
	uncomp = (Byte*)calloc((uInt)uncompLen, 1); //Do not alloc for uncompressed data

	ReadIntParam (flush);
	ReadIntParam (compression);
	ReadIntParam (expRet);

	retVal = sec_deflate01 (comp, compLen, flush, compression);
	if ( retVal != KErrNone)
		{
		ERR_PRINTF3(_L("Expected %d, returned %d"),KErrNone,retVal);
		retVal= KErrGeneral;
		}
	retVal = sec_inflate (comp, compLen, uncomp, uncompLen, flush);
	if ( retVal != expRet)
		{
		ERR_PRINTF3(_L("Expected %d, returned %d"),expRet,retVal);
		retVal= KErrGeneral;
		}
	else
		{
		INFO_PRINTF1(_L("Test passes!"));
		retVal= KErrNone;
		}
	free (comp);
	free (uncomp);
	return retVal;
	}

//Use 256K sized chunks for inflating
TInt CTestZlib::Test_inflate05()
	{
	INFO_PRINTF1(_L("Zlib Test inflate with 256K sized chunks"));
	int retVal = KErrGeneral;
	TInt flush, compression, expRet;
	Byte *comp=NULL, *uncomp=NULL;
	uLong compLen, uncompLen;
	compLen = 30;
	uncompLen = 30;

	comp = (Byte*)calloc((uInt)compLen, 1);
	uncomp = (Byte*)calloc((uInt)uncompLen, 1); //Do not alloc for uncompressed data

	ReadIntParam (flush);
	ReadIntParam (compression);
	ReadIntParam (expRet);

	retVal = sec_deflate01 (comp, compLen, flush, compression);
	if ( retVal != KErrNone)
		{
		ERR_PRINTF3(_L("Expected %d, returned %d"),KErrNone,retVal);
		retVal= KErrGeneral;
		}
	retVal = sec_inflate_large_buf (comp, compLen, uncomp, uncompLen, flush);
	if ( retVal != expRet)
		{
		ERR_PRINTF3(_L("Expected %d, returned %d"),expRet,retVal);
		retVal= KErrGeneral;
		}
	else
		{
		INFO_PRINTF1(_L("Test passes!"));
		retVal= KErrNone;
		}
	free (comp);
	free (uncomp);
	return retVal;
	}

TInt CTestZlib::Test_inflate06()
	{
	INFO_PRINTF1(_L("Zlib Test inflate with invalid data"));
	int retVal = KErrGeneral;
	TInt flush, compression, expRet;
	Byte *comp, *uncomp;
	uLong compLen, uncompLen;
	compLen = uncompLen = 60;

	comp = (Byte*)calloc((uInt)compLen, 1);
	//comp    = (Byte*)malloc(compLen);
	uncomp = (Byte*)calloc((uInt)uncompLen, 1);
	//uncomp  = (Byte*)malloc(uncompLen);

	ReadIntParam (flush);
	ReadIntParam (compression);
	ReadIntParam (expRet);

	retVal = sec_deflate01 (comp, compLen, flush, compression);
	if ( retVal != KErrNone)
		{
		ERR_PRINTF3(_L("Expected %d, returned %d"),KErrNone,retVal);
		retVal= KErrGeneral;
		}
	
	// Corrupt the compressed data
	comp[0] = 'a';
	comp[1] = 'a';
	
	retVal = sec_inflate (comp, compLen, uncomp, uncompLen, flush);
	if ( retVal != expRet)
		{
		ERR_PRINTF3(_L("Expected %d, returned %d"),expRet,retVal);
		retVal= KErrGeneral;
		}
	else
		{
		INFO_PRINTF1(_L("Test passes!"));
		retVal= KErrNone;
		}
	free (comp);
	free (uncomp);
	return retVal;
	}

TInt CTestZlib::Test_inflateEnd()
	{
	INFO_PRINTF1(_L("Zlib Test inflateEnd"));
	Byte *comp, *uncomp;
	uLong compLen, uncompLen;
	compLen = uncompLen = 30;

	comp = (Byte*)calloc((uInt)compLen, 1);
	uncomp = (Byte*)calloc((uInt)uncompLen, 1);

	int err;
	z_stream d_stream; // decompression stream 

	//strcpy((char*)uncompr, "garbage");

	d_stream.zalloc = (alloc_func)0;
	d_stream.zfree = (free_func)0;
	d_stream.opaque = (voidpf)0;

	d_stream.next_in = comp;
	d_stream.avail_in = 0;
	d_stream.next_out = uncomp;

	err = inflateInit (&d_stream);
	if ( err != Z_OK)
		{
		INFO_PRINTF2(_L("inflateInit error: %d"), err);
		free (comp);
		free (uncomp);
		return err;
		}
	//Not inflating

	err = inflateEnd (&d_stream);
	if ( err != Z_OK)
		{
		INFO_PRINTF2(_L("inflateEnd error: %d"), err);
		free (comp);
		free (uncomp);
		return err;
		}
	if ( d_stream.state != NULL)
		{
		INFO_PRINTF2(_L("inflateEnd error: %d"), err);
		free (comp);
		free (uncomp);
		return err;
		}

	free (comp);
	free (uncomp);
	return KErrNone;
	}

// Test deflateSetDictionary - normal flow
TInt CTestZlib::Test_deflateSetDictionary01()
	{
	INFO_PRINTF1(_L("Zlib Test test_deflateSetDictionary - positive test"));
	int retVal = KErrGeneral;
	TInt flush, compression, expRet;

	Byte *comp;
	uLong compLen;
	compLen = 30;
	comp = (Byte*)calloc((uInt)compLen, 1);

	ReadIntParam (flush);
	ReadIntParam (compression);
	ReadIntParam (expRet);

	retVal = sec_deflateSetDictionary01 (comp, compLen, flush, compression);
	free (comp);
	if ( retVal != expRet)
		{
		ERR_PRINTF3(_L("Expected %d, returned %d"),expRet,retVal);
		retVal= KErrGeneral;
		}
	else
		{
		INFO_PRINTF1(_L("Test passes!"));
		retVal= KErrNone;
		}
	return retVal;
	}

// Test deflateSetDictionary - dictionary NULL
TInt CTestZlib::Test_deflateSetDictionary02()
	{
	INFO_PRINTF1(_L("Zlib Test test_deflateSetDictionary with dictionary NULL"));
	int retVal = KErrGeneral;

	TInt compression, expRet;

	ReadIntParam (compression);
	ReadIntParam (expRet);
	retVal = sec_deflateSetDictionary02 (compression);

	if ( retVal != expRet)
		{
		ERR_PRINTF3(_L("Expected %d, returned %d"),expRet,retVal);
		retVal= KErrGeneral;
		}
	else
		{
		INFO_PRINTF1(_L("Test passes!"));
		retVal= KErrNone;
		}
	return retVal;
	}

// Test deflateSetDictionary - set dictionary after deflate, deflateEnd
TInt CTestZlib::Test_deflateSetDictionary03()
	{
	INFO_PRINTF1(_L("Zlib Test test_deflateSetDictionary - set dictionary after deflating"));
	int retVal = KErrGeneral;
	TInt flush, compression, expRet;

	Byte *comp;
	uLong compLen;
	compLen = 30;
	comp = (Byte*)calloc((uInt)compLen, 1);

	ReadIntParam (flush);
	ReadIntParam (compression);
	ReadIntParam (expRet);

	retVal = sec_deflateSetDictionary03 (comp, compLen, flush, compression);
	free (comp);
	if ( retVal != expRet)
		{
		ERR_PRINTF3(_L("Expected %d, returned %d"),expRet,retVal);
		retVal= KErrGeneral;
		}
	else
		{
		INFO_PRINTF1(_L("Test passes!"));
		retVal= KErrNone;
		}
	return retVal;
	}
// Test deflateSetDictionary - set tiny dictionary < MIN_MATCH
TInt CTestZlib::Test_deflateSetDictionary04()
	{
	INFO_PRINTF1(_L("Zlib Test test_deflateSetDictionary - positive test"));
	int retVal = KErrGeneral;
	TInt flush, compression, expRet;

	Byte *comp;
	uLong compLen;
	compLen = 30;
	comp = (Byte*)calloc((uInt)compLen, 1);

	ReadIntParam (flush);
	ReadIntParam (compression);
	ReadIntParam (expRet);

	retVal = sec_deflateSetDictionary04 (comp, compLen, flush, compression);
	free (comp);
	if ( retVal != expRet)
		{
		ERR_PRINTF3(_L("Expected %d, returned %d"),expRet,retVal);
		retVal= KErrGeneral;
		}
	else
		{
		INFO_PRINTF1(_L("Test passes!"));
		retVal= KErrNone;
		}
	return retVal;
	}

// Test deflateSetDictionary - set large dictionary > MAX_MATCH
TInt CTestZlib::Test_deflateSetDictionary05()
	{
	INFO_PRINTF1(_L("Zlib Test test_deflateSetDictionary - set large dictionary > MAX_MATCH"));
	int retVal = KErrGeneral;
	TInt flush, compression, expRet;

	Byte *comp;
	uLong compLen;
	compLen = 30;
	comp = (Byte*)calloc((uInt)compLen, 1);

	ReadIntParam (flush);
	ReadIntParam (compression);
	ReadIntParam (expRet);

	retVal = sec_deflateSetDictionary05 (comp, compLen, flush, compression);
	free (comp);
	if ( retVal != expRet)
		{
		ERR_PRINTF3(_L("Expected %d, returned %d"),expRet,retVal);
		retVal= KErrGeneral;
		}
	else
		{
		INFO_PRINTF1(_L("Test passes!"));
		retVal= KErrNone;
		}
	return retVal;
	}

TInt CTestZlib::Test_gzio()
	{
	INFO_PRINTF1(_L("Zlib Test gzio"));
	int retVal = 0;

	Byte *comp, *uncomp;
	uLong compLen, uncompLen;
	compLen = uncompLen = 30;

	comp = (Byte*)calloc((uInt)compLen, 1);
	if ( comp == NULL)
		{
		INFO_PRINTF1(_L("Could not allocate memory for comp."));
		return KErrNoMemory;
		}

	uncomp = (Byte*)calloc((uInt)uncompLen, 1);
	if ( uncomp == NULL)
		{
		INFO_PRINTF1(_L("Could not allocate memory for uncomp."));
		free (comp);
		return KErrNoMemory;
		}

	retVal = sec_gzio ("C:\\bye.gz", uncomp, uncompLen);
	free (comp);
	free (uncomp);
	return retVal;
	}

TInt CTestZlib::Test_gzdirect()
	{
	INFO_PRINTF1(_L("gzdirect test: read gz file"));
	TInt res = KErrNone;
	char mode[3];
	gzFile file;
	const char * fname=  TESTFILE;
	ReadStringParam (mode);
	file = gzopen (fname, mode);
	if ( file == NULL)
		{
		res = KErrGeneral;
		return res;
		}
	res = gzdirect (file); //0=success
	gzclose (file);
	return res;
	}

TInt CTestZlib::Test_gzdirectnull()
	{
	INFO_PRINTF1(_L("gzdirect test: read NULL stream"));
	TInt res = KErrNone;
	gzFile file=NULL;
	res = gzdirect (file); //0=success
	gzclose (file);
	return res;
	}

TInt CTestZlib::Test_gzclearerr_null()
	{
	TInt res = KErrNone;
	int err;
	gzFile file;
	const char *fname=  NULL;
	file = gzopen (fname, "wb");
	if ( file == NULL)
		{
		res=KErrGeneral;
		}
	gzputc (file, 'h');
	if ( gzputs (file, "ello")!= 5)
		{
		gzprintf (file, "gzputs err");
		res=KErrGeneral;
		}
	err= gzclose (file);
	if ( err != Z_OK)
		{
		res = KErrGeneral;
		}
	gzclearerr (file);
	z_stream *s = (z_stream*)file;
	if ( s == NULL)
		res = KErrNone;
	else
		{
		ERR_PRINTF1(_L("Expected NULL stream. Returned nonNULL"));
		res = KErrGeneral;
		}
	return res;
	}

TInt CTestZlib::Test_gzclearerr()
	{

	Byte *compr, *uncompr;
	uLong comprLen = 20*sizeof(int);

	uLong uncomprLen = comprLen;
	compr = (Byte*)calloc((uInt)comprLen, 1);
	if ( compr == Z_NULL)
		{
		return KErrNoMemory;
		}
	uncompr = (Byte*)calloc((uInt)uncomprLen, 1);
	if ( uncompr == Z_NULL)
		{
		free (compr);
		return KErrNoMemory;
		}

	INFO_PRINTF1(_L("gzclearerr test"));
	int err;
	int len = (int)strlen(hello)+1;
	gzFile file;

	file = gzopen (MYFILE, "wb");
	if ( file == NULL)
		{
		err = KErrGeneral;
		free (compr);
		free (uncompr);
		return err;
		}
	gzputc (file, 'h');
	if ( gzputs (file, "ello")!= 4)
		{
		err=KErrGeneral;
		}
	if ( gzprintf (file, ", %s!", "hello")!= 8)
		{
		err=KErrGeneral;
		}
	gzseek (file, 1L, SEEK_CUR); // add one zero byte 
	gzclose (file);

	file = gzopen (MYFILE, "rb");
	if ( file == NULL)
		{
		err = KErrGeneral;
		free (compr);
		free (uncompr);
		return err;
		}
	strcpy ((char*)uncompr, "garbage");

	if ( gzread (file, uncompr, (unsigned)uncomprLen)!= len)
		{
		}
	if ( strcmp ((char*)uncompr, hello))
		{
		err=KErrGeneral;
		}
	else
		{
		err=KErrNone;
		}

	gzseek (file, -8L, SEEK_CUR);
	gzclearerr (file);
	gzgets (file, (char*)uncompr, (int)uncomprLen);

	if ( strlen ((char*)uncompr)!= 7)
		{
		//1 " hello!" 
		ERR_PRINTF1(_L("gzegets gets wrong string"));
		err=KErrGeneral;
		}
	gzclose (file);

	free (compr);
	free (uncompr);
	return err;
	}

TInt CTestZlib::Test_gzerror_streamend()
	{
#ifdef NO_GZCOMPRESS
	ERR_PRINTF1(_L("NO_GZCOMPRESS -- gz* functions cannot compress"));
#else
	const char *msgptr;
	char fname[] = "C:\\bye.gz";

	Byte *compr, *uncompr;
	uLong comprLen, uncomprLen;
	comprLen = uncomprLen = 30;

	compr = (Byte*)calloc((uInt)comprLen, 1);
	if ( compr == NULL)
		{
		INFO_PRINTF1(_L("Could not allocate memory for compr."));
		return KErrNoMemory;
		}

	uncompr = (Byte*)calloc((uInt)uncomprLen, 1);
	if ( uncompr == NULL)
		{
		INFO_PRINTF1(_L("Could not allocate memory for uncompr."));
		free (compr);
		return KErrNoMemory;
		}

	int err;
	const char hello[] = "hello, hello!";
	int len = (int)strlen(hello)+1;
	gzFile file;

	file = gzopen (fname, "wb");
	if ( file == NULL)
		{
		ERR_PRINTF1(_L("gzopen error"));
		free (compr);
		free (uncompr);
		return KErrGeneral;
		}
	gzputc (file, 'h');
	if ( gzputs (file, "ello")!= 4)
		{
		ERR_PRINTF1(_L("gzputs err"));
		free (compr);
		free (uncompr);
		return KErrGeneral;
		}
	if ( gzprintf (file, ", %s!", "hello")!= 8)
		{
		ERR_PRINTF1(_L("gzprintf err="));
		free (compr);
		free (uncompr);
		return KErrGeneral;
		}

	gzseek (file, 1L, SEEK_CUR); // add one zero byte 
	gzclose (file);

	file = gzopen (fname, "rb");
	if ( file == NULL)
		{
		ERR_PRINTF1(_L("gzopen error"));
		free (compr);
		free (uncompr);
		return KErrGeneral;
		}
	strcpy ((char*)uncompr, "garbage");

	if ( gzread (file, uncompr, (unsigned)uncomprLen)!= len)
		{
		ERR_PRINTF1(_L("gzread err"));
		free (compr);
		free (uncompr);
		return KErrGeneral;
		}

	if ( strcmp ((char*)uncompr, hello))
		{
		ERR_PRINTF1(_L("bad gzread"));
		free (compr);
		free (uncompr);
		return KErrGeneral;
		}

	INFO_PRINTF2(_L("Reached file position %ld"),gzseek(file, -8L, SEEK_CUR));

	gzgets (file, (char*)uncompr, (int)uncomprLen);
	msgptr = gzerror (file, &err);
	if ( strcmp (msgptr, "C:\\bye.gz: stream end")!= 0)
		{
		// " hello!" 
		ERR_PRINTF1(_L("gzerror err on streamend"));
		free (compr);
		free (uncompr);
		return KErrGeneral;
		}

	gzclose (file);
	free (compr);
	free (uncompr);

	if ( err != Z_STREAM_END)
		{
		return KErrGeneral;
		}
	return KErrNone;
#endif
	}

TInt CTestZlib::Test_gzungetcnegative()
	{
	gzFile file=NULL;
	int ret = gzungetc (' ', file); //NULL stream
	if ( ret != EOF)
		{
		ERR_PRINTF2(_L("gzungetc NULL stream test: Expected EOF but returned %d"),ret);
		return KErrGeneral;
		}

	const char hello[] = "hello, hello!";
	int len = (int)strlen(hello)+1;

	file = gzopen ("C:\\bye.gz", "wb");
	if ( file == NULL)
		{
		ERR_PRINTF1(_L("gzopen error"));
		return KErrGeneral;
		}
	gzputc (file, 'h');
	if ( gzputs (file, "ello")!= 4)
		{
		ERR_PRINTF1(_L("gzputs err"));
		gzclose (file);
		return KErrGeneral;
		}

	ret = gzungetc (' ', file); //non-read mode
	if ( ret != EOF)
		{
		ERR_PRINTF2(_L("gzungetc non-read mode test: Expected EOF but returned %d"),ret);
		gzclose (file);
		return KErrGeneral;
		}

	gzseek (file, 1L, SEEK_CUR); // add one zero byte 
	gzclose (file);

	file = gzopen ("C:\\bye.gz", "rb");
	if ( file == NULL)
		{
		ERR_PRINTF1(_L("gzopen error"));
		return KErrGeneral;
		}

	if ( gzungetc (EOF, file)!= EOF) //ungetc EOF
		{
		gzclose (file);
		ERR_PRINTF1(_L("gzungetc error"));
		return KErrGeneral;
		}
	gzclose (file);

	return KErrNone;
	}

TInt CTestZlib::Test_gzseeknegative()
	{

	int err=0;

	int len = (int)strlen(hello)+1;
	gzFile file;

	file = gzopen (TESTFILE, "wb");
	if ( file == NULL)
		{
		return KErrNoMemory;
		}
	gzputc (file, 'h');

	err= gzseek (file, 1L, SEEK_END); /* add one zero byte */
	if ( err != -1)
		{
		gzclose (file);
		return KErrGeneral;
		}

	err= gzclose (file);
	file = gzopen (TESTFILE, "rb");
	if ( gzgetc (file)!= 'h')
		{
		gzclose (file);
		ERR_PRINTF1(_L("gzgetc error"));
		return KErrGeneral;
		}
	if ( gzgetc (file)!= EOF)
		{
		gzclose (file);
		ERR_PRINTF1(_L("gzgetc error"));
		return KErrGeneral;
		}
	err= gzseek (file, 1L, SEEK_CUR);
	if ( gzgetc (file)!= -1L)
		{
		gzclose (file);
		ERR_PRINTF1(_L("gzseek error"));
		return KErrGeneral;
		}
	gzclose (file);
	return KErrNone;
	}

/*TInt CTestZlib::TestGzopenRw()
 {
 TInt res = KErrNone ;
 gzFile file;
 char c;
 const char *fname = "c:\\Bytes.gz";
 file = gzopen(fname, "wr");
 if (file == NULL) 
 {
 res = KErrGeneral;
 return res;
 }
 else
 {
 res=gzputc(file, 'r');
 if(res<0)
 {
 res = KErrGeneral;
 } 
 else  
 {    	
 res = KErrNone;
 //gzclearerr(file);
 gzseek(file,0, SEEK_SET);
 c = gzgetc(file);
 if(c == 'r')
 {
 res = KErrNone;
 }
 else
 {
 ERR_PRINTF1(_L("gzgetc error in rw mode. Expected r"));
 res=KErrGeneral;		        	
 }
 }     	
 }
 gzclose(file);
 return res;
 }
 */
/**
 * Function Name : Test_gzdirecttxt
 * TestCase Description: 1. Checks whether a normal txt file gives a compressed stream or not
 */
TInt CTestZlib::Test_gzdirecttxt()
	{
	gzFile file;
	int ret=KErrGeneral;
	char fname[] = "C:\\gzdirecttest.txt";
	FILE *fp=NULL;
	fp=fopen (fname, "w");
	fputc ('\n', fp);
	fclose (fp);
	file = gzopen (fname, "rb");
	ret = gzdirect (file);
	if ( ret)
		{
		INFO_PRINTF1(_L("Reading a Non GzFile"));
		ret=KErrNone;
		}
	else
		{
		ERR_PRINTF1(_L("Error in gzdirect. Expeceted 1, returned 0"));
		ret=KErrGeneral;
		}
	gzclose (file);
	return ret;
	}

/**
 * Function Name : TestGzungetcChain
 * TestCase Description: 1. Checks whether gzungetc ungets the read chars using getc
 */
TInt CTestZlib::TestGzungetcChain()
	{
	gzFile file;
	char tmp;
	char fname[] = "C:\\Hello.gz";
	file = gzopen (fname, "wb");
	gzputc (file, 'h');
	if ( gzputs (file, "ello World")!= 10)
		{
		gzclose (file);
		printf ("Error: gzputs\n");
		}
	gzclose (file);
	file = gzopen (fname, "rb");
	while (' ' != ( tmp = gzgetc(file) ))
		{
		gzungetc (tmp, file);
		gzseek (file, 1L, SEEK_CUR);
		}
	gzclose (file);
	return KErrNone;
	}

/**
 * Function Name : TestGzseekBack
 * TestCase Description: 1. Checks whether gzseek returns -1
 * for backward seeks in files opened in write mode.
 */
TInt CTestZlib::TestGzseekBack()
	{
	int err;
	int len = (int)strlen(hello)+1;
	gzFile file;

	const char * fname=  TESTFILE;

	file = gzopen (fname, "wb");
	if ( file == NULL)
		{
		err = KErrGeneral;
		return err;
		}
	gzputc (file, 'h');
	if ( gzputs (file, "ello")!= 4)
		{
		err=1;
		}
	if ( gzprintf (file, ", %s!", "hello")!= 8)
		{
		err=1;
		}
	err = (int) gzseek(file,0, SEEK_SET); /* to beg */
	gzclose (file);
	if ( err == -1)
		{
		return KErrNone;
		}
	else
		{
		ERR_PRINTF2(_L("Expected -1, returned %d"),err);
		return KErrGeneral;
		}
	}

/**
 * Function Name : TestGzseekAppend
 * TestCase Description: 
 * 1. Writes a text file, closes.
 * 2. Open using gzopen in append mode
 * 3. Writes another character.
 * 4. Seek one down from current position
 * 5. Checks whether gzseek returns 2
 */
TInt CTestZlib::TestGzseekAppend()
	{
	const char hello[] = "hello, hello!";
	int len = (int)strlen(hello)+1;
	int err;
	gzFile file;

	FILE *fp = fopen ("c:\\file.txt", "wb");
	fputc ('h', fp);
	fclose (fp);
	file = gzopen ("c:\\file.txt", "a");
	if ( file == NULL)
		{
		ERR_PRINTF1(_L("gzopen error"));
		return KErrGeneral;
		}
	gzputc (file, 'h');
	err = (int) gzseek(file,1L, SEEK_CUR); /* to next pos */
	gzclose (file);
	if ( err == 2)
		return KErrNone;
	else
		{
		ERR_PRINTF2(_L("Expected 2, returned %d"),err);
		return KErrGeneral;
		}
	}

/**
 * Function Name : TestGzseekHugeOffset
 * TestCase Description: 
 * 1. Writes a text file, closes.
 * 2. Open using gzopen in append mode
 * 3. Writes another character.
 * 4. Seek 4097 up from current position
 * 5. Checks whether gzseek returns 16386 or not.
 */
TInt CTestZlib::TestGzseekHugeOffset()
	{
	const char hello[] = "hello, hello!";
	int len = (int)strlen(hello)+1;
	int err;
	gzFile file;

	FILE *fp = fopen ("c:\\file.txt", "wb");
	fputc ('h', fp);
	fclose (fp);
	file = gzopen ("c:\\file.txt", "a");
	if ( file == NULL)
		{
		ERR_PRINTF1(_L("gzopen error"));
		return KErrGeneral;
		}
	gzputc (file, 'h');
	err = (int) gzseek(file,16385L, SEEK_CUR); /* advance pos by 16385*/
	gzclose (file);
	if ( err == 16386)
		return KErrNone;
	else
		{
		ERR_PRINTF2(_L("Expected 2, returned %d"),err);
		return KErrGeneral;
		}
	}

/**
 * Function Name : TestGzseekNoSize
 * TestCase Description: 
 * 1. Seeks a zero sized file
 * 2. Checks whether it returns -1 or not
 */
TInt CTestZlib::TestGzseekNoSize()
	{
	TInt res = KErrNone;
	gzFile file;
	uInt size, len;
	const char *s="\0";
	len=strlen (s);
	const char * fname=  TESTFILE;
	file = gzopen (fname, "wb");
	if ( file == Z_NULL)
		{
		ERR_PRINTF1(_L("gzopen error"));
		return KErrGeneral;
		}
	size = gzwrite (file, (char*)s, (unsigned)strlen(s));
	if ( len!=size)
		{
		ERR_PRINTF1(_L("gzwrite error"));
		return KErrGeneral;
		}

	res = (int) gzseek(file,1L, SEEK_CUR); /* to next pos */
	gzclose (file);
	if ( res == 1)
		{
		return KErrNone;
		}
	else
		{
		ERR_PRINTF2(_L("Expected -1, returned %d"), res);
		return KErrGeneral;
		}
	}

/**
 * Function Name : TestGzopenLongPath
 * TestCase Description: 
 * 1. Seeks a file with long name
 * 2. Checks whether gzopen returns NULL or not
 */
TInt CTestZlib::TestGzopenLongPath01()
	{
	gzFile file;
	const char
			* fname = "c:\\fffff\
    fffffffffffffffffffffffffffffff\
    fffffffffffffffffffffffffffffff\
    fffffffffffffffffffffffffffffff\
    fffffffffffffffffffffffffffffff\
    fffffffffffffffffffffffffffffff\
    fffffffffffffffffffffffffffffff\
    fffffffffffffffffffffffffffffff\
    fffffffffffffffffffffffffffffff\
    fffffffffffffffffffffffffffffff\
    fffffffffffffffffffffffffffffff\
    fffffffffffffffffffffffffffffff.txt";
	file = gzopen (fname, "wb");
	if ( file == Z_NULL)
		{
		INFO_PRINTF1(_L("Returned NULL"));
		return KErrNone;
		}
	else
		{
		ERR_PRINTF2(_L("Expected NULL, returned %d"), file);
		return KErrGeneral;
		}
	}

/**
 * Function Name : TestGzseekLongPath
 * TestCase Description: 
 * 1. Seeks a acceptable long file name
 * 2. Checks whether it returns 1 or not
 */
TInt CTestZlib::TestGzseekLongPath01()
	{
	TInt res = KErrNone;
	gzFile file;
	uInt size, len;
	const char *s="\0";
	len=strlen (s);
	const char
			* fname = "c:\\fffff\
    fffffffffffffffffffffffffffffff\
    fffffffffffffffffffffffffffffff\
    fffffffffffffffffffffffffffffff\
    fffffffffffffffffffffffffffffff\
    fffffffffffffffffffffffffffffff\
    fffffffffffffffffffffffffffffff.txt";
	file = gzopen (fname, "wb");
	if ( file == Z_NULL)
		{
		ERR_PRINTF1(_L("gzopen error"));
		return KErrGeneral;
		}
	size = gzwrite (file, (char*)s, (unsigned)strlen(s));
	if ( len!=size)
		{
		ERR_PRINTF1(_L("gzwrite error"));
		return KErrGeneral;
		}

	res = (int) gzseek(file,1L, SEEK_CUR); /* to next pos */
	gzclose (file);
	if ( res == 1)
		{
		return KErrNone;
		}
	else
		{
		ERR_PRINTF2(_L("Expected -1, returned %d"), res);
		return KErrGeneral;
		}
	}

/**
 * Function Name : TestGzopenLongPath
 * TestCase Description: 
 * 1. Seeks a long pathed file
 * 2. Checks whether it returns NULL or not
 */
TInt CTestZlib::TestGzopenLongPath02()
	{
	gzFile file;
	int ret = KErrNone;
	const char
			* fname = "C:\\fffff\
    fffffffffffffffffffffffffffffff\
    fffffffffffffffffffffffffffffff\
    fffffffffffffffffffffffffffffff\
    fffffffffffffffffffffffffffffff\
    fffffffffffffffffffffffffffffff\
    fffffffffffffffffffffffffffffff";

	file = gzopen (fname, "wb");
	if ( file == Z_NULL)
		{
		ERR_PRINTF1(_L("gzopen error- File NULL"));
		return KErrGeneral;
		}
	else
		{
		INFO_PRINTF1(_L("Expected file pointer, returned Success"));
		}
	gzclose (file);
	return ret;
	}

/**
 * Function Name : TestGzseekMixedFile01
 * TestCase Description: 
 * 1. Open using gzopen in write mode
 * 2. gzputs a string.
 * 3. fopen it, writes a text, close.
 * 4. Seek one down from current position
 * 5. Checks whether gzseek returns 1 for offset 1L, 
 *    1000L for offset 1000L, -1 for -1L, -1 for -1000L
 */
TInt CTestZlib::TestGzseekMixedFile01()
	{
	const char hello[] = "hello, hello!";
	int len = (int)strlen(hello)+1;
	int err;
	gzFile file;

	TInt offset, expRes;

	ReadIntParam (offset);
	ReadIntParam (expRes);

	file = gzopen ("c:\\file.txt", "wb");
	gzputs (file, hello);
	gzclose (file);
	FILE *fp = fopen ("c:\\file.txt", "w+");
	fputc ('h', fp);
	fclose (fp);
	file = gzopen ("c:\\file.txt", "rb");
	if ( file == NULL)
		{
		ERR_PRINTF1(_L("gzopen error"));
		return KErrGeneral;
		}
	err = (int) gzseek(file,offset, SEEK_CUR); /* to next pos */
	gzclose (file);
	if ( err == expRes)
		return KErrNone;
	else
		{
		ERR_PRINTF3(_L("Expected %d, returned %d"),expRes, err);
		return KErrGeneral;
		}
	}

/**
 * Function Name : TestGzopenNoMode
 * TestCase Description: 
 * 1. gzopen a file with NULL mode
 * 2. Checks whether gzopen returns NULL or not
 */
TInt CTestZlib::TestGzopenNoMode()
	{
	gzFile file;
	const char * fname = "c:\\file.txt";
	file = gzopen (fname, NULL);
	if ( file == Z_NULL)
		{
		INFO_PRINTF1(_L("Returned NULL"));
		return KErrNone;
		}
	else
		{
		ERR_PRINTF2(_L("Expected NULL, returned %d"), file);
		return KErrGeneral;
		}
	}

/**
 * Function Name : TestGzopenNoPath
 * TestCase Description: 
 * 1. gzopen a file with NULL path
 * 2. Checks whether gzopen returns NULL or not
 */
TInt CTestZlib::TestGzopenNoPath()
	{
	gzFile file;
	file = gzopen (NULL, "wb");
	if ( file == Z_NULL)
		{
		INFO_PRINTF1(_L("Returned NULL"));
		return KErrNone;
		}
	else
		{
		ERR_PRINTF2(_L("Expected NULL, returned %d"), file);
		return KErrGeneral;
		}
	}

/**
 * Function Name : TestGzopenNoPath
 * TestCase Description: 
 * 1. gzopen a file with path,mode empty string, 
 * 2. Checks whether gzopen returns NULL or not
 */
TInt CTestZlib::TestGzopenNoPathMode()
	{
	gzFile file;
	file = gzopen ("", "");
	if ( file == Z_NULL)
		{
		INFO_PRINTF1(_L("Returned NULL"));
		return KErrNone;
		}
	else
		{
		ERR_PRINTF2(_L("Expected NULL, returned %d"), file);
		return KErrGeneral;
		}
	}
/**
 * Function Name : TestGzseekConcatedFile01
 * TestCase Description: 
 * 1. Open a manually concatinated gz file using gzopen in read mode
 * 2. Seek one down from current position
 * 3. Checks whether gzseek returns 1 for offset 1L, 
 *    -1 for offset 1000L, -1 for -1L, -1 for -1000L
 */
TInt CTestZlib::TestGzseekConcatedFile01()
	{
	int err;
	gzFile file;

	TInt offset, expRes;
	ReadIntParam (offset);
	ReadIntParam (expRes);

	file = gzopen (FILETESTGZCONCAT, "rb");
	if ( file == NULL)
		{
		ERR_PRINTF1(_L("gzopen error"));
		return KErrGeneral;
		}
	err = (int) gzseek(file,offset, SEEK_CUR); // to next pos 
	gzclose (file);
	if ( err == expRes)
		{
		return KErrNone;
		}
	else
		{
		ERR_PRINTF3(_L("Expected %d, returned %d"),expRes, err);
		return KErrGeneral;
		}
	}

/**
 * Function Name : TestGzopenNoPath
 * TestCase Description: 
 * 1. gzopen a file with valid path, mode: 9, f, h, R, 9, fb, hb, Rb, 
 *    and wwwwwwwwwwwwwwwwwwwwwwwwwwwwwwwwwwwwwwwwwwwwwwwwwwwwwwwwwww
 * 2. Checks whether gzopen returns NULL all cases, except the last one.
 */
TInt CTestZlib::TestGzopenDiffMode()
	{
	TInt res = KErrNone;
	gzFile file=Z_NULL;
	const char * fname=  TESTFILE;
	char mode[100];
	TInt expRes;
	ReadStringParam (mode);
	ReadIntParam (expRes);
	file = gzopen (fname, "wb");
	res = gzputc (file, 'h');
	res = gzclose (file);
	file = gzopen (fname, mode);
	if ( (file == Z_NULL && expRes == 0) || (file != Z_NULL && expRes == 1))
		{
		res = KErrNone;
		}
	else
		{
		res = KErrGeneral;
		ERR_PRINTF1(_L("Expected NULL, returned nonNULL"));
		}
	if ( file)
		{
		gzclose (file);
		}
	return res;
	}

/**
 * Function Name : TestGzseekConcatedFile02
 * TestCase Description: 
 * 1. Open a programmatically concatinated gz file using gzopen 
 *    in read mode
 * 2. Seek one down from current position
 * 3. Checks whether gzseek returns 1 for offset 1L, 
 *    -1 for offset 1000L, -1 for -1L, -1 for -1000L
 */
TInt CTestZlib::TestGzseekConcatedFile02()
	{
	int err;
	gzFile file;

	TInt offset, expRes;

	ReadIntParam (offset);
	ReadIntParam (expRes);
	char fname1[13]="c:\\first.gz";
	char fname2[14]="c:\\second.gz";

	//create 2 gz files
	file = gzopen (fname1, "w");
	if ( file == NULL)
		{
		ERR_PRINTF1(_L("gzopen error"));
		return KErrGeneral;
		}
	gzputc (file, 'h');
	gzclose (file);
	file = gzopen (fname2, "w");
	if ( file == NULL)
		{
		unlink (fname1);
		ERR_PRINTF1(_L("gzopen error"));
		return KErrGeneral;
		}
	gzputc (file, 'e');
	gzclose (file);

	//concatenate the two
	FILE *fpFirst=NULL, *fpSecond=NULL;
	fpFirst = fopen (fname1, "a");
	fpSecond = fopen (fname2, "r");
	char c;
	for (; !feof(fpSecond);)
		{
		c=fgetc (fpSecond);
		fputc (c, fpFirst);
		}
	fclose (fpFirst);
	fclose (fpSecond);

	//Now seek
	file = gzopen (fname1, "r");
	err = (int) gzseek(file,offset, SEEK_CUR); // to next pos 
	gzclose (file);
	if ( err == expRes)
		{
		unlink (fname1);
		unlink (fname2);
		return KErrNone;
		}
	else
		{
		unlink (fname1);
		unlink (fname2);
		ERR_PRINTF3(_L("Expected %d, returned %d"),expRes, err);
		return KErrGeneral;
		}
	}

/**
 * Function Name : TestGzprintf01
 * TestCase Description: 
 * 1. Prints an empty string
 * 2. Checks whether  returns 0 
 */
TInt CTestZlib::TestGzprintf01()
	{
	TInt res = KErrGeneral;
	gzFile file;
	const char * fname=  TESTFILE;
	file = gzopen (fname, "wb");
	if ( file == NULL)
		{
		res = KErrNoMemory;
		return res;
		}
	res = gzprintf (file, "");
	if ( res != 0)
		{
		ERR_PRINTF1(_L("gzprintf err"));
		}
	else
		{
		res = KErrNone;
		}
	gzclose (file);
	return res;
	}
/**
 * Function Name : TestGzprintf02
 * TestCase Description: 
 * 1. Prints an large string of length 4097, 4096
 * 2. Checks whether  returns 0, 4095 
 */
TInt CTestZlib::TestGzprintf02()
	{
	TInt res = KErrGeneral;
	gzFile file;
	const char * fname=  TESTFILE;

	char largeStr[4098];
	TInt strLength, expRes;
	ReadIntParam (strLength);
	ReadIntParam (expRes);

	//create alarge string
	for (int i=0; i<strLength;i++)
		{
		largeStr[i]='a';
		}
	largeStr[strLength]='\0';
	file = gzopen (fname, "wb");
	if ( file == NULL)
		{
		res = KErrNoMemory;
		return res;
		}
	res = gzprintf (file, largeStr);
	if ( res != expRes)
		{
		ERR_PRINTF1(_L("gzprintf err"));
		}
	else
		{
		res = KErrNone;
		}
	gzclose (file);
	unlink (TESTFILE);
	return res;
	}

/**
 * Function Name : TestGzflushNull
 * TestCase Description: 
 * 1. Flushes a NULL stream
 * 2. Checks whether  returns Z_STREAM_ERROR(-2)
 */
TInt CTestZlib::TestGzflushNull()
	{
	TInt res = KErrNone;
	gzFile file=  NULL;
	int l= gzflush (file, Z_FULL_FLUSH);
	if ( l != Z_STREAM_ERROR)
		{
		res = KErrGeneral;
		}
	return res;
	}

/**
 * Function Name : TestGzflushRepeat
 * TestCase Description: 
 * 1. Flushes a valid stream twice
 * 2. Checks whether  returns 0
 */
TInt CTestZlib::TestGzflushRepeat()
	{
	TInt res = KErrNone;
	gzFile file;

	const char * fname=  TESTFILE;
	file = gzopen (fname, "wb");
	if ( file == Z_NULL)
		{
		res = KErrNoMemory;
		return res;
		}
	int l= gzflush (file, Z_FULL_FLUSH);
	if ( l != Z_OK)
		{
		res = KErrGeneral;
		}
	l= gzflush (file, Z_SYNC_FLUSH);
	if ( l != Z_OK)
		{
		res = KErrGeneral;
		}
	int err= gzclose (file);
	if ( err != Z_OK)
		{
		res = KErrGeneral;
		}
	return res;
	}

/**
 * Function Name : TestGzflushHugeBuf
 * TestCase Description: 
 * 1. Flushes a valid stream 
 * 2. Checks whether  returns 0
 */
TInt CTestZlib::TestGzflushHugeBuf()
	{
	TInt res = KErrNone;
	gzFile file;

	const char * fname=  TESTFILE;
	file = gzopen (fname, "wb");
	if ( file == Z_NULL)
		{
		res = KErrNoMemory;
		return res;
		}
	for (int i=0; i<16385;i++)
		{
		gzputc (file, 'a');
		}

	int l= gzflush (file, Z_FULL_FLUSH);
	if ( l != Z_OK)
		{
		res = KErrGeneral;
		}
	int err= gzclose (file);
	if ( err != Z_OK)
		{
		res = KErrGeneral;
		}
	return res;
	}

/**
 * Function Name : TestGzrewindNull
 * TestCase Description: 
 * 1. Rewinds a NULL stream 
 * 2. Checks whether  returns -1
 */
TInt CTestZlib::TestGzrewindNull()
	{
	TInt res = KErrNone;
	res = gzrewind (NULL);
	if ( res == -1)
		{
		res = KErrNone;
		}
	else
		{
		res = KErrGeneral;
		}
	return res;
	}

/**
 * Function Name : TestGzrewindTransparent
 * TestCase Description: 
 * 1. Rewinds a non-gz file stream 
 * 2. Checks whether  returns 1 or not
 */
TInt CTestZlib::TestGzrewindTransparent()
	{

	gzFile file;
	int ret=KErrGeneral;
	char fname[] = "C:\\gzdirecttest.txt";
	FILE *fp=NULL;
	fp=fopen (fname, "w");
	fputc ('\n', fp);
	fclose (fp);
	file = gzopen (fname, "rb");
	ret = gzdirect (file);
	if ( ret)
		{
		INFO_PRINTF1(_L("Reading a Non GzFile"));
		ret = gzrewind (file);
		if ( ret)
			{
			ret=KErrGeneral;
			}
		else
			{
			ret = KErrNone;
			}
		}
	else
		{
		ERR_PRINTF1(_L("Error in gzdirect. Expeceted 1, returned 0"));
		ret=KErrGeneral;
		}
	gzclose (file);
	return ret;
	}

/**
 * Function Name : TestGzgetsBufNull
 * TestCase Description: 
 * 1. Gets from file into a NULL buffer
 * 2. Checks whether  returns NULL or not
 */
TInt CTestZlib::TestGzgetsBufNull()
	{

	const char hello[] = "hello, hello!";
	int len = (int)strlen(hello)+1;
	gzFile file;
	char *buf=NULL;
	file = gzopen ("c:\\file.gz", "w");
	if ( file == NULL)
		{
		ERR_PRINTF1(_L("gzopen error"));
		return KErrGeneral;
		}
	gzputs (file, hello);
	gzclose (file);
	file = gzopen ("c:\\file.gz", "r");
	buf = gzgets (file, buf, len);
	gzclose (file);
	if ( buf == Z_NULL)
		return KErrNone;
	else
		return KErrGeneral;
	}

/**
 * Function Name : TestGzgetsSmallBuf
 * TestCase Description: 
 * 1. Gets from file into a small buffer
 * 2. Checks whether  returns the string correctly or not
 */
TInt CTestZlib::TestGzgetsSmallBuf()
	{
	const char hello[] = "hello, hello!\n";
	int len;
	char expBuf[100];
	ReadIntParam (len);
	ReadStringParam (expBuf);
	gzFile file;
	char *buf=(char *)malloc(strlen(hello));
	file = gzopen ("c:\\file.gz", "w");
	if ( file == NULL)
		{
		ERR_PRINTF1(_L("gzopen error"));
		free (buf);
		return KErrGeneral;
		}
	gzputs (file, hello);
	gzclose (file);
	file = gzopen ("c:\\file.gz", "r");
	buf = gzgets (file, (char *)buf, len);
	gzclose (file);

	if ( !strcmp(buf,expBuf))
		{
		free (buf);
		return KErrNone;
		}
	else
		{
		free (buf);
		return KErrGeneral;
		}
	}
