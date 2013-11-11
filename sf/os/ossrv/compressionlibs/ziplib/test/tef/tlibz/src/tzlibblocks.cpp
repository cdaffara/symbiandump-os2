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

#include "tzlib.h"

//uLong dictId;

#define CHECK_ERR(err, msg) { \
	if (err != Z_OK) { \
	INFO_PRINTF2(_L("Error: %d"), err); \
	return err; \
	} \
}


TInt CTestZlib::PreDeflateInit( Byte * compr, uLong comprLen, TInt flush, TInt compression)
	{
	z_stream c_stream; // compression stream 
	int err;
	const char hello[] = "hello, hello!";
	uLong len = (uLong)strlen(hello)+1;

	c_stream.zalloc = (alloc_func)0;
	c_stream.zfree = (free_func)0;
	c_stream.opaque = (voidpf)0;

	err = deflateInit(&c_stream,compression);// Z_DEFAULT_COMPRESSION);

	if (err != Z_OK) 
		{ 
		INFO_PRINTF2(_L("deflateInit error: %d"), err); 
		return err; 
		} 

	c_stream.next_in  = (Bytef*)hello;
	c_stream.next_out = compr;

	while (c_stream.total_in != len && c_stream.total_out < comprLen) 
		{
		c_stream.avail_in = c_stream.avail_out = 1; //* force small buffers 
		err = deflate(&c_stream, flush);
		if (err != Z_OK) 
			{ 
			INFO_PRINTF2(_L("deflate return code: %d"), err); 
			deflateEnd(&c_stream);
			return err; 
			} 
		}
	// Finish the stream, still forcing small buffers: 
	for (;;) 
		{
		c_stream.avail_out = 1;
		err = deflate(&c_stream, Z_FINISH);
		if (err == Z_STREAM_END) break;
		if (err != Z_OK)
			{ 
			INFO_PRINTF2(_L("deflate error: %d"), err); 
			deflateEnd(&c_stream);
			return err; 
			} 
		}

	deflateEnd(&c_stream);
	return KErrNone;
	}

/**
 * Function Name : TestDeflateTest01
 * TestCase Description: 1. Reads flush value more than Z_FINISH [i.e, 4]
 *						2. Reads negative flush value 
 */
TInt CTestZlib::TestDeflateTest01( )
	{
	INFO_PRINTF1(_L("Zlib Test deflate"));	
	TInt flush, compression, expRet;

	Byte *comp;
	uLong compLen;
	compLen = 30;
	comp    = (Byte*)calloc((uInt)compLen, 1);

	ReadIntParam(flush);
	ReadIntParam(compression);
	ReadIntParam(expRet);

	z_stream c_stream; // compression stream 
	int err;
	const char hello[] = "hello, hello!";
	uLong len = (uLong)strlen(hello)+1;

	c_stream.zalloc = (alloc_func)0;
	c_stream.zfree = (free_func)0;
	c_stream.opaque = (voidpf)0;

	err = deflateInit(&c_stream,compression);// Z_DEFAULT_COMPRESSION);

	if (err != Z_OK) 
		{ 
		INFO_PRINTF2(_L("deflateInit error: %d"), err); 
		free(comp);
		return err; 
		}  

	c_stream.next_in  = (Bytef*)hello;
	c_stream.next_out = comp;

	while (c_stream.total_in != len && c_stream.total_out < compLen) 
		{
		c_stream.avail_in = c_stream.avail_out = 1; //* force small buffers 
		err = deflate(&c_stream, flush);
		if (err == Z_STREAM_ERROR) 
			{ 
			INFO_PRINTF1(_L("Negative test case passed"));
			deflateEnd(&c_stream);
			free(comp);
			return KErrNone; 
			}
		}

	return KErrNone;

	}

/**
 * Function Name : TestDeflatecopyDestNull
 * TestCase Description: Deflate copy with Destination as NULL
 */
TInt CTestZlib::TestDeflatecopyDestNull()
	{
	uLong len = (uLong)strlen(hello)+1;
	TInt retVal=0;
	Byte *compr, *uncompr;
	uLong comprLen = 20*sizeof(int); 
	uLong uncomprLen = comprLen;
	compr    = (Byte*)calloc((uInt)comprLen, 1);
	if (compr == Z_NULL)
		{
		return KErrNoMemory;
		}

	uncompr  = (Byte*)calloc((uInt)uncomprLen, 1);
	if (uncompr == Z_NULL) 
		{
		free(compr);
		return KErrNoMemory;
		}	


	z_stream stream;
	int err;

	stream.zalloc = (alloc_func)0;
	stream.zfree = (free_func)0;
	stream.opaque = (voidpf)0;

	stream.next_in = (Bytef*)hello;
	stream.avail_in = (uInt)len;

	stream.next_out = compr;
	stream.avail_out = (uInt)comprLen;

	err = deflateInit(&stream, Z_DEFAULT_COMPRESSION);

	err=deflateCopy(NULL , &stream); 

	if (err == Z_STREAM_ERROR )  
		{
		deflateEnd(&stream);
		free(compr);
		free(uncompr);
		retVal = KErrNone; 
		} 

	return retVal;   
	}

/**
 * Function Name : TestDeflateCopyStreamStateNull
 * TestCase Description: Deflate copy with source stream state as Z_NULL
 * Return value: KErrNone on deflateCopy retruning Z_STREAM_ERROR
 */
TInt CTestZlib::TestDeflateCopyStreamStateNull()
	{
	uLong len = (uLong)strlen(hello)+1;
	TInt retVal=0;
	Byte *compr, *uncompr;
	uLong comprLen = 20*sizeof(int); 
	uLong uncomprLen = comprLen;
	compr = (Byte*)calloc((uInt)comprLen, 1);
	if (compr == Z_NULL)
		{
		return KErrNoMemory;
		}

	uncompr = (Byte*)calloc((uInt)uncomprLen, 1);
	if (uncompr == Z_NULL) 
		{
		free(compr);
		return KErrNoMemory;
		}	    	

	z_stream stream;
	z_stream stream1;
	int err = KErrNone;

	stream.zalloc = (alloc_func)0;
	stream.zfree = (free_func)0;
	stream.opaque = (voidpf)0;

	stream.next_in = (Bytef*)hello;
	stream.avail_in = (uInt)len;

	stream.next_out = compr;
	stream.avail_out = (uInt)comprLen;

	err = deflateInit(&stream, Z_DEFAULT_COMPRESSION);

	err=deflateCopy(&stream1, &stream);
	deflateEnd(&stream);
	deflateEnd(&stream1);

	// Make stream state Z_NULL and call deflateCopy for coverage imrprovement
	stream.state = Z_NULL;
	err=deflateCopy(&stream1 , &stream); 

	if (err == Z_STREAM_ERROR )
		{
		retVal = KErrNone; 
		}
	free(compr);
	free(uncompr);
	return retVal;   
	}

/**
 * Function Name : TestDeflateInit2_WindowBits
 * TestCase Description: 1. WindowBits value more than 15
 *						2. WindowBits value less than 8
 *						3. WindowBits 8
 */
TInt CTestZlib::TestDeflateInit2_WindowBits()
	{
	TInt res = KErrNone;
	z_stream stream;
	int err;
	int level=Z_DEFAULT_COMPRESSION;
	int WindowBits=0;

	uLong sourceLen = (uLong)strlen(hello)+1;
	Byte *compr;
	uLong comprLen = 10*sizeof(int); 
	compr = (Byte*)calloc((uInt)comprLen, 1);
	if (compr == Z_NULL)
		{
		return KErrNoMemory;
		}

	ReadIntParam(WindowBits);
	stream.zalloc = (alloc_func)0;
	stream.zfree = (free_func)0;
	stream.opaque = (voidpf)0;


	err= deflateInit2_(&stream, level, 8, WindowBits, 8,
			0,  zlibVersion(), sizeof(z_stream));

	if (err == Z_STREAM_ERROR) 
		{
		res = KErrNone;
		}
	else if (err == Z_OK)
		{
		deflateEnd(&stream);
		res = KErrNone;
		}
	else  
		{    	
		res = KErrGeneral; 
		}       

	free(compr);
	return res;
	}



/**
 * Function Name : TestDeflateInit2_StreamSize
 * TestCase Description: Stream size less than sizeof(z_stream)
 */
TInt CTestZlib::TestDeflateInit2_StreamSize()
	{
	TInt res = KErrNone ;
	z_stream stream;
	int err;
	int level=Z_DEFAULT_COMPRESSION;
	int WindowBits=0;

	uLong sourceLen = (uLong)strlen(hello)+1;
	Byte *compr;
	uLong comprLen = 10*sizeof(int); 
	compr = (Byte*)calloc((uInt)comprLen, 1);
	if (compr == Z_NULL)
		{
		return KErrNoMemory;
		}

	ReadIntParam(WindowBits);
	stream.zalloc = (alloc_func)0;
	stream.zfree = (free_func)0;
	stream.opaque = (voidpf)0;

	// passing stream size < sizeof(z_stream)
	err= deflateInit2_(&stream, level, 8, 15, 8,
			0,  zlibVersion(), sizeof(z_stream)-10);

	if (err == Z_VERSION_ERROR) 
		{
		res = KErrNone;
		}
	else  
		{    	
		res = KErrGeneral; 
		}       

	free(compr);
	return res;
	}

/**
 * Function Name : TestDeflateInit2_MemLevel
 * TestCase Description: 1. MemLevel value less than 1
 *						2. MemLevel value more than MAX_MEM_LEVEL [i.e., 8]
 */
TInt CTestZlib::TestDeflateInit2_MemLevel()
	{
	TInt res = KErrNone ;
	z_stream stream;
	int err;
	int level=Z_DEFAULT_COMPRESSION;
	int MemLevel=0;

	uLong sourceLen = (uLong)strlen(hello)+1;
	Byte *compr;
	uLong comprLen = 10*sizeof(int); 
	compr    = (Byte*)calloc((uInt)comprLen, 1);
	if (compr == Z_NULL)
		{
		return KErrNoMemory;
		}

	ReadIntParam(MemLevel);
	stream.zalloc = (alloc_func)0;
	stream.zfree = (free_func)0;
	stream.opaque = (voidpf)0;


	err= deflateInit2_(&stream, level, 3, 15, MemLevel,
			0,  zlibVersion(), sizeof(z_stream));

	if (err == Z_STREAM_ERROR) 
		{
		res = KErrNone;
		} 
	else  
		{    	
		res = KErrGeneral; 
		}       

	//deflateEnd(&stream);
	free(compr);
	return res;
	}

/**
 * Function Name : TestDeflateInit2_Level
 * TestCase Description: 1. Level value less than 0
 *						2. Level value more than 9
 */
TInt CTestZlib::TestDeflateInit2_Level()
	{
	TInt res = KErrNone ;
	z_stream stream;
	int err;
	int level=0;

	uLong sourceLen = (uLong)strlen(hello)+1;
	Byte *compr;
	uLong comprLen = 10*sizeof(int); 
	compr    = (Byte*)calloc((uInt)comprLen, 1);
	if (compr == Z_NULL)
		{
		return KErrNoMemory;
		}

	ReadIntParam(level);
	stream.zalloc = (alloc_func)0;
	stream.zfree = (free_func)0;
	stream.opaque = (voidpf)0;


	err= deflateInit2_(&stream, level, 8, 15, 8,
			0,  zlibVersion(), sizeof(z_stream));

	if (err == Z_STREAM_ERROR) 
		{
		res = KErrNone;
		} 
	else  
		{    	
		res = KErrGeneral; 
		}       

	//deflateEnd(&stream);
	free(compr);
	return res;
	}

/**
 * Function Name : TestDeflateInit2_Strategy
 * TestCase Description: 1. Strategy value less than 0
 *						2. Strategy value more than Z_FIXED
 */
TInt CTestZlib::TestDeflateInit2_Strategy()
	{
	TInt res = KErrNone ;
	z_stream stream;
	int err;
	int level=Z_DEFAULT_COMPRESSION;
	int strategy=0;

	uLong sourceLen = (uLong)strlen(hello)+1;
	Byte *compr;
	uLong comprLen = 10*sizeof(int); 
	compr = (Byte*)calloc((uInt)comprLen, 1);
	if (compr == Z_NULL)
		{
		return KErrNoMemory;
		}

	ReadIntParam(strategy);
	stream.zalloc = (alloc_func)0;
	stream.zfree = (free_func)0;
	stream.opaque = (voidpf)0;


	err= deflateInit2_(&stream, level, 8, 15, 8,
			strategy, zlibVersion(), sizeof(z_stream));

	if (err == Z_STREAM_ERROR) 
		{
		res = KErrNone;
		} 
	else  
		{    	
		res = KErrGeneral; 
		}       

	free(compr);
	return res;
	}

/**
 * Function Name : TestDeflateInit2_Version
 * TestCase Description: Invalid version value
 * Return Value: returns Z_VERSION_ERROR
 */
TInt CTestZlib::TestDeflateInit2_Version()
	{
	TInt res = KErrNone ;
	z_stream stream;
	int err;
	int level=Z_DEFAULT_COMPRESSION;
	char version[5];

	uLong sourceLen = (uLong)strlen(hello)+1;
	Byte *compr;
	uLong comprLen = 10*sizeof(int); 
	compr = (Byte*)calloc((uInt)comprLen, 1);
	if (compr == Z_NULL)
		{
		return KErrNoMemory;
		}

	ReadStringParam(version);
	stream.zalloc = (alloc_func)0;
	stream.zfree = (free_func)0;
	stream.opaque = (voidpf)0;


	err= deflateInit2_(&stream, level, 8, 15, 8,
			0, version, sizeof(z_stream));

	if (err == Z_VERSION_ERROR) 
		{
		res = KErrNone;
		} 
	else  
		{
		res = KErrGeneral; 
		}       

	free(compr);
	return res;
	}

/**
 * Function Name : TestDeflateInit2_VersionNull
 * TestCase Description: version value is Z_NULL
 * Return Value: Z_VERSION_ERROR
 */
TInt CTestZlib::TestDeflateInit2_VersionNull()
	{
	TInt res = KErrNone ;
	z_stream stream;
	int err;
	int level=Z_DEFAULT_COMPRESSION;

	uLong sourceLen = (uLong)strlen(hello)+1;
	Byte *compr;
	uLong comprLen = 10*sizeof(int); 
	compr = (Byte*)calloc((uInt)comprLen, 1);
	if (compr == Z_NULL)
		{
		return KErrNoMemory;
		}

	stream.zalloc = (alloc_func)0;
	stream.zfree = (free_func)0;
	stream.opaque = (voidpf)0;


	err= deflateInit2_(&stream, level, 8, 15, 8,
			0, Z_NULL, sizeof(z_stream));

	if (err == Z_VERSION_ERROR) 
		{
		res = KErrNone;
		} 
	else  
		{	
		res = KErrGeneral; 
		}       

	free(compr);
	return res;
	}


/**
 * Function Name : TestDeflateInit2_StreamNull
 * TestCase Description: Pass Z_NULL stream
 */
TInt CTestZlib::TestDeflateInit2_StreamNull()
	{
	TInt res = KErrNone ;
	//z_stream stream;
	int err;	
	int level=Z_DEFAULT_COMPRESSION;

	uLong sourceLen = (uLong)strlen(hello)+1;
	Byte *compr;
	uLong comprLen = 10*sizeof(int); 
	compr = (Byte*)calloc((uInt)comprLen, 1);
	if (compr == Z_NULL)
		{
		return KErrNoMemory;
		}

	//stream.zalloc = (alloc_func)0;
	//stream.zfree = (free_func)0;
	//stream.opaque = (voidpf)0;

	err= deflateInit2_(Z_NULL, level, 8, 15, 8,
			0,  zlibVersion(), sizeof(z_stream));

	if (err == Z_STREAM_ERROR) 
		{
		res = KErrNone;
		}
	else  
		{    	
		res = KErrGeneral; 
		}       

	free(compr);
	return res;
	}

/**
 * Function Name : TestInflateInit2_Version
 * TestCase Description: Invalid version value
 * Return Value: returns Z_VERSION_ERROR
 */
TInt CTestZlib::TestInflateInit2_Version()
	{ 
	TInt res = KErrNone ;

	z_stream d_stream; // decompression stream
	char version[5];
	d_stream.zalloc = (alloc_func)0;
	d_stream.zfree = (free_func)0;
	d_stream.opaque = (voidpf)0;

	Byte *compr, *uncompr;
	uLong comprLen = 20*sizeof(int); // don't overflow on MSDOS
	uLong uncomprLen = comprLen;
	compr    = (Byte*)calloc((uInt)comprLen, 1);
	if (compr == Z_NULL)
		{
		return KErrNoMemory;
		}
	uncompr  = (Byte*)calloc((uInt)uncomprLen, 1);
	if (uncompr == Z_NULL) 
		{
		free(compr);
		return KErrNoMemory;
		}
	res = Test_deflate(compr, comprLen);
	if(res<0)
		{
		free(compr);
		free(uncompr);
		return KErrNoMemory;	
		}
	d_stream.next_in  = compr;
	d_stream.avail_in = 0;
	d_stream.next_out = uncompr;

	//Reading invalid version from ini file
	ReadStringParam(version);	

	int ret =  inflateInit2_(&d_stream, 15, version, sizeof(d_stream));
	if(ret == Z_VERSION_ERROR)
		{
		res = KErrNone;    	
		}
	else
		{    	
		res = KErrGeneral;
		}   
	free(compr);
	free(uncompr);
	return res;   
	}

/**
 * Function Name : TestInflateInit2_VersionNull
 * TestCase Description: version value is Z_NULL
 * Return Value: Z_VERSION_ERROR
 */
TInt CTestZlib::TestInflateInit2_VersionNull()
	{ 
	TInt res = KErrNone ;

	z_stream d_stream; // decompression stream
	d_stream.zalloc = (alloc_func)0;
	d_stream.zfree = (free_func)0;
	d_stream.opaque = (voidpf)0;

	Byte *compr, *uncompr;
	uLong comprLen = 20*sizeof(int); // don't overflow on MSDOS
	uLong uncomprLen = comprLen;
	compr    = (Byte*)calloc((uInt)comprLen, 1);
	if (compr == Z_NULL)
		{
		return KErrNoMemory;
		}
	uncompr  = (Byte*)calloc((uInt)uncomprLen, 1);
	if (uncompr == Z_NULL) 
		{
		free(compr);
		return KErrNoMemory;
		}
	res = Test_deflate(compr, comprLen);
	if(res<0)
		{
		free(compr);
		free(uncompr);
		return KErrNoMemory;	
		}
	d_stream.next_in  = compr;
	d_stream.avail_in = 0;
	d_stream.next_out = uncompr;

	int ret =  inflateInit2_(&d_stream, 15, Z_NULL, sizeof(d_stream));
	if(ret == Z_VERSION_ERROR)
		{
		res = KErrNone;    	
		}
	else
		{    	
		res = KErrGeneral;
		}   
	free(compr);
	free(uncompr);
	return res;   
	}

/**
 * Function Name : TestInflateInit2_WindowBits
 * TestCase Description: Window bits more than 48 to increase the conditional coverage
 * Return Value: returns Z_STREAM_ERROR
 */
TInt CTestZlib::TestInflateInit2_WindowBits()
	{ 
	TInt res = KErrNone ;

	z_stream d_stream; // decompression stream
	int WindowBits = 0;
	d_stream.zalloc = (alloc_func)0;
	d_stream.zfree = (free_func)0;
	d_stream.opaque = (voidpf)0;

	Byte *compr, *uncompr;
	uLong comprLen = 20*sizeof(int); // don't overflow on MSDOS
	uLong uncomprLen = comprLen;
	compr    = (Byte*)calloc((uInt)comprLen, 1);
	if (compr == Z_NULL)
		{
		return KErrNoMemory;
		}
	uncompr  = (Byte*)calloc((uInt)uncomprLen, 1);
	if (uncompr == Z_NULL) 
		{
		free(compr);
		return KErrNoMemory;
		}
	res = Test_deflate(compr, comprLen);
	if(res<0)
		{
		free(compr);
		free(uncompr);
		return KErrNoMemory;	
		}
	d_stream.next_in  = compr;
	d_stream.avail_in = 0;
	d_stream.next_out = uncompr;

	//Reading window bits from ini file
	ReadIntParam(WindowBits);	

	int ret =  inflateInit2_(&d_stream, WindowBits, "1.2.4", sizeof(d_stream));
	if(ret == Z_STREAM_ERROR)
		{
		res = KErrNone;    	
		}
	else
		{    	
		res = KErrGeneral;
		}   
	free(compr);
	free(uncompr);
	return res;   
	}

/**
 * Function Name : TestInflateInit2_StreamNull
 * TestCase Description: Stream NULL
 * Return Value: returns Z_STREAM_ERROR
 */
TInt CTestZlib::TestInflateInit2_StreamNull()
	{ 
	TInt res = KErrNone ;

	z_stream d_stream; // decompression stream
	//d_stream.zalloc = (alloc_func)0;
	//d_stream.zfree = (free_func)0;
	//d_stream.opaque = (voidpf)0;

	Byte *compr, *uncompr;
	uLong comprLen = 20*sizeof(int); // don't overflow on MSDOS
	uLong uncomprLen = comprLen;
	compr = (Byte*)calloc((uInt)comprLen, 1);
	if (compr == Z_NULL)
		{
		return KErrNoMemory;
		}
	uncompr  = (Byte*)calloc((uInt)uncomprLen, 1);
	if (uncompr == Z_NULL) 
		{
		free(compr);
		return KErrNoMemory;
		}
	res = Test_deflate(compr, comprLen);
	if(res<0)
		{
		free(compr);
		free(uncompr);
		return KErrNoMemory;	
		}
	//d_stream.next_in  = compr;
	//d_stream.avail_in = 0;
	//d_stream.next_out = uncompr;

	int ret =  inflateInit2_(Z_NULL, 15, "1.2.4", sizeof(d_stream));
	if(ret == Z_STREAM_ERROR)
		{
		res = KErrNone;    	
		}
	else
		{    	
		res = KErrGeneral;
		}   
	free(compr);
	free(uncompr);
	return res;   
	}

/**
 * Function Name : TestDeflate_HuffmanStrategy
 * TestCase Description: Strategy is HUFFMAN
 * Return Value: Z_OK
 */
TInt CTestZlib::TestDeflate_HuffmanStrategy()
	{
	TInt res = KErrNone ;
	z_stream stream;
	int err;
	int level=Z_DEFAULT_COMPRESSION;

	uLong sourceLen = (uLong)strlen(hello)+1;
	Byte *compr;
	uLong comprLen = 20*sizeof(int); 
	compr = (Byte*)calloc((uInt)comprLen, 1);
	if (compr == Z_NULL)
		{
		return KErrNoMemory;
		}

	stream.zalloc = (alloc_func)0;
	stream.zfree = (free_func)0;
	stream.opaque = (voidpf)0;


	err= deflateInit2_(&stream, level, 8, 15, 8,
			2, zlibVersion(), sizeof(z_stream));

	if (err == Z_OK) 
		{
		res = KErrNone;
		} 
	else  
		{	
		res = KErrGeneral; 
		}

	stream.next_in  = (Bytef*)hello;
	stream.next_out = compr;

	while (stream.total_in != sourceLen && stream.total_out < comprLen) 
		{
		stream.avail_in = stream.avail_out = 1; /* force small buffers */
		err = deflate(&stream, Z_NO_FLUSH);
		}
	/* Finish the stream, still forcing small buffers: */
	for (;;) 
		{
		stream.avail_out = 1;
		err = deflate(&stream, Z_FINISH);
		if (err == Z_STREAM_END) break;
		if(err!=Z_OK)
			{
			res = KErrGeneral;
			}    
		}

	err = deflateEnd(&stream);
	if(err!=Z_OK)
		{
		res = KErrGeneral;
		}     

	free(compr);
	return res;
	}

/**
 * Function Name : TestDeflate_AvailInZero
 * TestCase Description: avail_in value is set to 0
 * Return Value: Z_STREAM_ERROR
 */
TInt CTestZlib::TestDeflate_AvailInZero()
	{
	TInt res = KErrNone ;
	z_stream stream;
	int err;
	int level=Z_DEFAULT_COMPRESSION;

	uLong sourceLen = (uLong)strlen(hello)+1;
	Byte *compr;
	uLong comprLen = 20*sizeof(int); 
	compr = (Byte*)calloc((uInt)comprLen, 1);
	if (compr == Z_NULL)
		{
		return KErrNoMemory;
		}

	stream.zalloc = (alloc_func)0;
	stream.zfree = (free_func)0;
	stream.opaque = (voidpf)0;


	err= deflateInit2_(&stream, level, 8, 15, 8,
			0, zlibVersion(), sizeof(z_stream));

	if (err == Z_OK) 
		{
		res = KErrNone;
		} 
	else  
		{	
		res = KErrGeneral; 
		}

	stream.next_in  = Z_NULL;
	stream.next_out = Z_NULL;    

	while (stream.total_in != sourceLen && stream.total_out < comprLen) 
		{
		stream.avail_in = 0;	// Setting avail_in to zero
		stream.avail_out = 1; 	// force small buffers 
		err = deflate(&stream, Z_NO_FLUSH);
		if (err != Z_OK)
			break;
		}

	if (err == Z_STREAM_ERROR) 
		{
		res = KErrNone;
		deflateEnd(&stream);
		free(compr);
		return res;
		} 
	else  
		{	
		res = KErrGeneral; 
		free(compr);
		}	   

	return res;
	}

/**
 * Function Name : TestGzsetparamDefaultCompression
 * TestCase Description: This test case is intended to cover the condition in
 *						deflateParams_r. Z_DEFAULT_COMPRESSION and strategy>4 is passed
 * Return Value: Z_STREAM_ERROR
 */
TInt CTestZlib::TestGzsetparamDefaultCompression()
	{
	TInt res = KErrNone ;

	int len = (int)strlen(hello)+1;
	gzFile file;

	const char * fname = TESTFILE;

	file = gzopen(fname, "wb");
	gzputc(file, 'h');

	int u = gzsetparams(file, Z_DEFAULT_COMPRESSION, 8);
	if(u == Z_STREAM_ERROR)
		{
		res=KErrNone;	
		}
	int err=  gzclose(file);
	if (err != Z_OK)  
		{
		res = KErrGeneral; 
		} 

	return res;
	}

/**
 * Function Name : TestDeflateResetNullStream
 * TestCase Description: Pass NULL stream
 * Return Value: Z_STREAM_ERROR
 */  	
TInt CTestZlib::TestDeflateResetNullStream()
	{ 
	INFO_PRINTF1(_L("DeflateReset test with NULL input"));
	TInt res = KErrNone ;
	int level = Z_DEFAULT_COMPRESSION ;
	uLong len = (uLong)strlen(hello)+1;
	Byte *compr, *uncompr;
	uLong comprLen = 20*sizeof(int); 
	uLong uncomprLen = comprLen;
	int err;
	z_stream stream;
	compr    = (Byte*)calloc((uInt)comprLen, 1);
	if (compr == Z_NULL)
		{
		return KErrNoMemory;
		}
	uncompr  = (Byte*)calloc((uInt)uncomprLen, 1);
	if (uncompr == Z_NULL) 
		{
		free(compr);
		return KErrNoMemory;
		}

	stream.zalloc = (alloc_func)0;
	stream.zfree = (free_func)0;
	stream.opaque = (voidpf)0;

	err = deflateInit(&stream, level);
	if (err != Z_OK) 
		{
		free(compr);
		free(uncompr);
		return err;
		}

	Bytef *dest = compr ;
	uLongf *destLen = &comprLen;
	const Bytef *source = (const Bytef*)hello;
	uLong sourceLen = len;  

	stream.next_in = (Bytef*)source;
	stream.avail_in = (uInt)sourceLen;

#ifdef MAXSEG_64K
	/* Check for source > 64K on 16-bit machine: */
	if ((uLong)stream.avail_in != sourceLen)  
		{
		res = KErrGeneral; 
		}
#endif

	stream.next_out = dest;
	stream.avail_out = (uInt)*destLen;

	if ((uLong)stream.avail_out != *destLen)  
		{
		res = KErrGeneral; 
		}

	err=deflateReset(Z_NULL/*&stream*/);
	if (err == Z_STREAM_ERROR)  
		{
		res = KErrNone;     			
		} 
	else  
		{    	
		res = KErrGeneral;
		}   
	deflateEnd(&stream);
	free(compr);
	free(uncompr);
	return res;
	}

/**
 * Function Name : TestDeflateResetStreamStateNull
 * TestCase Description: Pass NULL stream state
 * Return Value: Z_STREAM_ERROR
 */  	
TInt CTestZlib::TestDeflateResetStreamStateNull()
	{ 
	TInt res = KErrNone ;
	int level = Z_DEFAULT_COMPRESSION ;
	uLong len = (uLong)strlen(hello)+1;
	Byte *compr, *uncompr;
	uLong comprLen = 20*sizeof(int); 
	uLong uncomprLen = comprLen;
	int err;
	z_stream stream;
	compr    = (Byte*)calloc((uInt)comprLen, 1);
	if (compr == Z_NULL)
		{
		return KErrNoMemory;
		}
	uncompr  = (Byte*)calloc((uInt)uncomprLen, 1);
	if (uncompr == Z_NULL) 
		{
		free(compr);
		return KErrNoMemory;
		}

	stream.zalloc = (alloc_func)0;
	stream.zfree = (free_func)0;
	stream.opaque = (voidpf)0;

	err = deflateInit(&stream, level);
	if (err != Z_OK) 
		{
		free(compr);
		free(uncompr);
		return err;
		}

	Bytef *dest = compr ;
	uLongf *destLen = &comprLen;
	const Bytef *source = (const Bytef*)hello;
	uLong sourceLen = len;  

	stream.next_in = (Bytef*)source;
	stream.avail_in = (uInt)sourceLen;

#ifdef MAXSEG_64K
	/* Check for source > 64K on 16-bit machine: */
	if ((uLong)stream.avail_in != sourceLen)  
		{
		res = KErrGeneral; 
		}
#endif

	stream.next_out = dest;
	stream.avail_out = (uInt)*destLen;

	if ((uLong)stream.avail_out != *destLen)  
		{
		res = KErrGeneral; 
		}

	err=deflateReset(&stream);
	deflateEnd(&stream);

	//equate stream state to Z_NULL for coverage improvement
	stream.state = Z_NULL;
	err=deflateReset(&stream);

	if (err == Z_STREAM_ERROR)  
		{
		res = KErrNone;     			
		} 
	else  
		{    	
		res = KErrGeneral;
		}   
	free(compr);
	free(uncompr);
	return res;
	}

/**
 * Function Name : TestDeflate_Scenarios
 * TestCase Description: 1. Compression level is Z_DEFAULT_COMPRESSION, strategy is Z_RLE
 *						2. Compression level is Z_BEST_SPEED, strategy is Z_HUFFMAN_ONLY
 *						3. Compression level is Z_BEST_COMPRESSION, strategy is Z_FILTERED
 *						4. Compression level is Z_BEST_COMPRESSION, strategy is Z_HUFFMAN_ONLY
 *						5. Compression level is Z_BEST_COMPRESSION, strategy is Z_FIXED
 * Return Value: Z_OK
 */
TInt CTestZlib::TestDeflate_Scenarios()
	{
	TInt res = KErrNone ;
	z_stream stream;
	int err;
	int level=0;
	int strategy=0;

	uLong sourceLen = (uLong)strlen(hello)+1;
	Byte *compr;
	uLong comprLen = 20*sizeof(int); 
	compr = (Byte*)calloc((uInt)comprLen, 1);
	if (compr == Z_NULL)
		{
		return KErrNoMemory;
		}

	stream.zalloc = (alloc_func)0;
	stream.zfree = (free_func)0;
	stream.opaque = (voidpf)0;

	ReadIntParam(level);
	ReadIntParam(strategy);  	
	err= deflateInit2_(&stream, level, 8, 15, 8,
			strategy, zlibVersion(), sizeof(z_stream));

	if (err == Z_OK) 
		{
		res = KErrNone;
		} 
	else  
		{	
		res = KErrGeneral; 
		}

	stream.next_in  = (Bytef*)hello;
	stream.next_out = compr;

	while (stream.total_in != sourceLen && stream.total_out < comprLen) 
		{
		stream.avail_in = stream.avail_out = 1; /* force small buffers */
		err = deflate(&stream, Z_NO_FLUSH);
		}
	/* Finish the stream, still forcing small buffers: */
	for (;;) 
		{
		stream.avail_out = 1;
		err = deflate(&stream, Z_FINISH);
		if (err == Z_STREAM_END) break;
		if(err!=Z_OK)
			{
			res = KErrGeneral;
			}    
		}

	err = deflateEnd(&stream);
	if(err!=Z_OK)
		{
		res = KErrGeneral;
		}     

	free(compr);
	return res;
	}

/**
 * Function Name : TestDeflate_NullStream
 * TestCase Description: NULL stream is passed for deflate
 * Return Value: Z_STREAM_ERROR
 */
TInt CTestZlib::TestDeflate_NullStream()
	{
	TInt res = KErrNone ;
	z_stream stream;
	int err;
	int level=Z_DEFAULT_COMPRESSION;

	uLong sourceLen = (uLong)strlen(hello)+1;
	Byte *compr;
	uLong comprLen = 20*sizeof(int); 
	compr = (Byte*)calloc((uInt)comprLen, 1);
	if (compr == Z_NULL)
		{
		return KErrNoMemory;
		}

	stream.zalloc = (alloc_func)0;
	stream.zfree = (free_func)0;
	stream.opaque = (voidpf)0;


	err= deflateInit2_(&stream, level, 8, 15, 8,
			0, zlibVersion(), sizeof(z_stream));

	if (err == Z_OK) 
		{
		res = KErrNone;
		} 
	else  
		{	
		res = KErrGeneral; 
		}

	stream.next_in  = (Bytef*)hello;
	stream.next_out = compr;    

	while (stream.total_in != sourceLen && stream.total_out < comprLen) 
		{
		stream.avail_in = stream.avail_out = 1; 	// force small buffers 
		err = deflate(Z_NULL, Z_NO_FLUSH);
		if (err != Z_OK)
			break;
		}

	if (err == Z_STREAM_ERROR) 
		{
		res = KErrNone;
		deflateEnd(&stream);
		free(compr);
		return res;
		} 
	else  
		{	
		res = KErrGeneral; 
		}	

	return res;
	}

/**
 * Function Name : TestDeflate_StreamStateNull
 * TestCase Description: stream state is made Z_NULL before calling deflate
 * Return Value: Z_STREAM_ERROR
 */
TInt CTestZlib::TestDeflate_StreamStateNull()
	{
	TInt res = KErrNone ;
	z_stream stream;
	int err;
	int level=Z_DEFAULT_COMPRESSION;

	uLong sourceLen = (uLong)strlen(hello)+1;
	Byte *compr;
	uLong comprLen = 20*sizeof(int); 
	compr = (Byte*)calloc((uInt)comprLen, 1);
	if (compr == Z_NULL)
		{
		return KErrNoMemory;
		}

	stream.zalloc = (alloc_func)0;
	stream.zfree = (free_func)0;
	stream.opaque = (voidpf)0;

	err= deflateInit2_(&stream, level, 8, 15, 8,
			0, zlibVersion(), sizeof(z_stream));

	if (err == Z_OK) 
		{
		res = KErrNone;
		} 
	else  
		{	
		res = KErrGeneral; 
		}

	stream.next_in  = (Bytef*)hello;
	stream.next_out = compr;

	while (stream.total_in != sourceLen && stream.total_out < comprLen) 
		{
		stream.avail_in = stream.avail_out = 1; 	// force small buffers 
		err = deflate(&stream, Z_NO_FLUSH);
		}
	deflateEnd(&stream);

	//equate stream state to Z_NULL for voverage improvement
	stream.state = Z_NULL;       
	err = deflate(&stream, Z_NO_FLUSH);

	if (err == Z_STREAM_ERROR) 
		{
		res = KErrNone;
		} 
	else  
		{	
		res = KErrGeneral;
		}
	free(compr);
	return res;
	}

/**
 * Function Name : TestDeflateEndNull
 * TestCase Description: Pass Z_NULL to deflateEnd
 * Return Value: Z_STREAM_ERROR
 */
TInt CTestZlib::TestDeflateEndNull()
	{
	TInt res = KErrNone ;
	z_stream stream;
	int err;
	int level=Z_DEFAULT_COMPRESSION;
	int strategy=Z_DEFAULT_STRATEGY;

	uLong sourceLen = (uLong)strlen(hello)+1;
	Byte *compr;
	uLong comprLen = 20*sizeof(int); 
	compr = (Byte*)calloc((uInt)comprLen, 1);
	if (compr == Z_NULL)
		{
		return KErrNoMemory;
		}

	stream.zalloc = (alloc_func)0;
	stream.zfree = (free_func)0;
	stream.opaque = (voidpf)0;

	err= deflateInit2_(&stream, level, 8, 15, 8,
			strategy, zlibVersion(), sizeof(z_stream));

	if (err == Z_OK) 
		{
		res = KErrNone;
		} 
	else  
		{	
		res = KErrGeneral; 
		}

	stream.next_in  = (Bytef*)hello;
	stream.next_out = compr;

	while (stream.total_in != sourceLen && stream.total_out < comprLen) 
		{
		stream.avail_in = stream.avail_out = 1; /* force small buffers */
		err = deflate(&stream, Z_NO_FLUSH);
		}
	/* Finish the stream, still forcing small buffers: */
	for (;;) 
		{
		stream.avail_out = 1;
		err = deflate(&stream, Z_FINISH);
		if (err == Z_STREAM_END) break;
		if(err!=Z_OK)
			{
			res = KErrGeneral;
			}    
		}

	// Pass Z_NULL
	err = deflateEnd(&stream);
	err = deflateEnd(Z_NULL); // for coverage improvement
	if(err == Z_STREAM_ERROR)
		{
		res = KErrNone;
		}
	else
		{
		res = KErrGeneral;
		}

	free(compr);
	return res;
	}

/**
 * Function Name : TestDeflateEndStreamStateNull
 * TestCase Description: Make stream.state = Z_NULL
 * Return Value: Z_STREAM_ERROR
 */
TInt CTestZlib::TestDeflateEndStreamStateNull()
	{
	TInt res = KErrNone ;
	z_stream stream;
	int err;
	int level=Z_DEFAULT_COMPRESSION;
	int strategy=Z_DEFAULT_STRATEGY;

	uLong sourceLen = (uLong)strlen(hello)+1;
	Byte *compr;
	uLong comprLen = 20*sizeof(int); 
	compr = (Byte*)calloc((uInt)comprLen, 1);
	if (compr == Z_NULL)
		{
		return KErrNoMemory;
		}

	stream.zalloc = (alloc_func)0;
	stream.zfree = (free_func)0;
	stream.opaque = (voidpf)0;

	err= deflateInit2_(&stream, level, 8, 15, 8,
			strategy, zlibVersion(), sizeof(z_stream));

	if (err == Z_OK) 
		{
		res = KErrNone;
		} 
	else  
		{	
		res = KErrGeneral; 
		}

	stream.next_in  = (Bytef*)hello;
	stream.next_out = compr;

	while (stream.total_in != sourceLen && stream.total_out < comprLen) 
		{
		stream.avail_in = stream.avail_out = 1; /* force small buffers */
		err = deflate(&stream, Z_NO_FLUSH);
		}
	/* Finish the stream, still forcing small buffers: */
	for (;;) 
		{
		stream.avail_out = 1;
		err = deflate(&stream, Z_FINISH);
		if (err == Z_STREAM_END) break;
		if(err!=Z_OK)
			{
			res = KErrGeneral;
			}    
		}
	err = deflateEnd(&stream);

	// Make state Z_NULL
	stream.state = Z_NULL;
	err = deflateEnd(&stream);

	if(err == Z_STREAM_ERROR)
		{
		res = KErrNone;
		}
	else
		{
		res = KErrGeneral;
		}

	free(compr);
	return res;
	}

/**
 * Function Name : TestDeflate_WindowBits
 * TestCase Description: Window bits more than 15 is supplied for deflate init
 * Return Value: Z_OK
 */
TInt CTestZlib::TestDeflate_WindowBits()
	{
	TInt res = KErrNone ;
	z_stream stream;
	int err;
	int level=Z_BEST_COMPRESSION;

	uLong sourceLen = (uLong)strlen(hello)+1;
	Byte *compr;
	uLong comprLen = 20*sizeof(int); 
	compr = (Byte*)calloc((uInt)comprLen, 1);
	if (compr == Z_NULL)
		{
		return KErrNoMemory;
		}

	stream.zalloc = (alloc_func)0;
	stream.zfree = (free_func)0;
	stream.opaque = (voidpf)0;


	err= deflateInit2_(&stream, level, 8, 25, 8,
			0, zlibVersion(), sizeof(z_stream));

	if (err == Z_OK) 
		{
		res = KErrNone;
		} 
	else  
		{	
		res = KErrGeneral; 
		}

	stream.next_in  = (Bytef*)hello;
	stream.next_out = compr;

	while (stream.total_in != sourceLen && stream.total_out < comprLen) 
		{
		stream.avail_in = stream.avail_out = 1; /* force small buffers */
		err = deflate(&stream, Z_NO_FLUSH);
		}
	/* Finish the stream, still forcing small buffers: */
	for (;;) 
		{
		stream.avail_out = 1;
		err = deflate(&stream, Z_FINISH);
		if (err == Z_STREAM_END) break;
		if(err!=Z_OK)
			{
			res = KErrGeneral;
			}    
		}

	err = deflateEnd(&stream);
	if(err!=Z_OK)
		{
		res = KErrGeneral;
		}     

	free(compr);
	return res;
	}

/**
 * Function Name : TestDeflateBoundStreamNotNull
 * TestCase Description: Pass valid stream to deflatebound with memlevel=7, which 
 *						in turn covers the decision for (s->hash_bits != 8 + 7) 
 */
TInt CTestZlib::TestDeflateBoundStreamNotNull()
	{
	TInt res = KErrNone;
	z_stream stream;
	int err;
	int level=Z_DEFAULT_COMPRESSION;

	uLong sourceLen = (uLong)strlen(hello)+1;
	Byte *compr;
	uLong comprLen = 20*sizeof(int); 
	compr = (Byte*)calloc((uInt)comprLen, 1);
	if (compr == Z_NULL)
		{
		return KErrNoMemory;
		}

	stream.next_in  = (Bytef*)hello;
	stream.next_out = compr;
	stream.avail_in = (uInt)sourceLen;
	stream.avail_out = (uInt)comprLen;
	if ((uLong)stream.avail_out != comprLen) 
		{
		res = KErrGeneral; 
		}

	stream.zalloc = (alloc_func)0;
	stream.zfree = (free_func)0;
	stream.opaque = (voidpf)0;

	err= deflateInit2_(&stream, level, 8, 14, 7,
			0,  zlibVersion(), sizeof(z_stream));

	if (err == Z_OK) 
		{
		int y= deflateBound(&stream, sourceLen);
		if(y > sourceLen)
			{  
			res = KErrNone ;
			} 
		else
			{   	
			res = KErrGeneral;      	
			}   
		}
	else
		{
		res = KErrGeneral;      
		return res;   
		}       

	err=deflateEnd(&stream);
	if (err != Z_OK) 
		{	
		res = KErrGeneral;      
		}
	free(compr);
	return res;
	}

/**
 * Function Name : TestDeflateBoundStreamNull
 * TestCase Description: Pass Z_NULL to deflatebound
 */
TInt CTestZlib::TestDeflateBoundStreamNull()
	{
	TInt res = KErrNone;
	z_stream stream;
	int err;
	int level=Z_DEFAULT_COMPRESSION;

	uLong sourceLen = (uLong)strlen(hello)+1;
	Byte *compr;
	uLong comprLen = 20*sizeof(int); 
	compr = (Byte*)calloc((uInt)comprLen, 1);
	if (compr == Z_NULL)
		{
		return KErrNoMemory;
		}

	stream.next_in  = (Bytef*)hello;
	stream.next_out = compr;
	stream.avail_in = (uInt)sourceLen;
	stream.avail_out = (uInt)comprLen;
	if ((uLong)stream.avail_out != comprLen) 
		{
		res = KErrGeneral; 
		}

	stream.zalloc = (alloc_func)0;
	stream.zfree = (free_func)0;
	stream.opaque = (voidpf)0;

	err= deflateInit2_(&stream, level, 8, 15, 7,
			0,  zlibVersion(), sizeof(z_stream));

	if (err == Z_OK) 
		{
		int y= deflateBound(Z_NULL, sourceLen);
		if(y > sourceLen)
			{  
			res = KErrNone ;
			} 
		else
			{   	
			res = KErrGeneral;      	
			}   
		}
	else
		{
		res = KErrGeneral;      
		return res;   
		}       

	err=deflateEnd(&stream);
	if (err != Z_OK) 
		{	
		res = KErrGeneral;      
		}
	free(compr);
	return res;
	}

/**
 * Function Name : TestDeflateBoundStreamStateNull
 * TestCase Description: Stream state is equated to Z_NULL 
 *						and passed to deflatebound
 */
TInt CTestZlib::TestDeflateBoundStreamStateNull()
	{
	TInt res = KErrNone;
	z_stream stream;
	int err;
	int level=Z_DEFAULT_COMPRESSION;

	uLong sourceLen = (uLong)strlen(hello)+1;
	Byte *compr;
	uLong comprLen = 20*sizeof(int); 
	compr = (Byte*)calloc((uInt)comprLen, 1);
	if (compr == Z_NULL)
		{
		return KErrNoMemory;
		}

	stream.next_in  = (Bytef*)hello;
	stream.next_out = compr;
	stream.avail_in = (uInt)sourceLen;
	stream.avail_out = (uInt)comprLen;
	if ((uLong)stream.avail_out != comprLen) 
		{
		res = KErrGeneral; 
		}

	stream.zalloc = (alloc_func)0;
	stream.zfree = (free_func)0;
	stream.opaque = (voidpf)0;

	err= deflateInit2_(&stream, level, 8, 15, 7,
			0,  zlibVersion(), sizeof(z_stream));

	if (err == Z_OK) 
		{
		//stream.state = Z_NULL;
		int y= deflateBound(&stream, sourceLen);
		if(y > sourceLen)
			{  
			res = KErrNone ;
			} 
		else
			{   	
			res = KErrGeneral;      	
			}
		err=deflateEnd(&stream);
		if (err != Z_OK) 
			{	
			INFO_PRINTF1(_L("Error encountered in deflateEnd"));
			free(compr);
			return KErrNone;      
			}

		// for coverage improvement
		stream.state = Z_NULL;
		y = deflateBound(&stream, sourceLen);
		}			// end of if
	else
		{
		res = KErrGeneral;      
		return res;   
		}       

	free(compr);
	return res;
	}

/**
 * Function Name : TestDeflateSetDictionaryWrap
 * TestCase Description: 1. Pass window bits > 15 to set the stream->wrap = 2
 *						2. Pass window bits < 0 to set the stream->wrap = 0
 * Return Value: Z_STREAM_ERROR
 */	
TInt CTestZlib::TestDeflateSetDictionaryWrap()
	{
	int level=Z_DEFAULT_COMPRESSION;
	int WindowBits = 0;

	Byte *comp;
	uLong compLen;
	compLen = 30;
	comp = (Byte*)calloc((uInt)compLen, 1);

	ReadIntParam(WindowBits);

	z_stream stream; // compression stream 
	int err;
	const char hello[] = "hello, hello!";
	uLong len = (uLong)strlen(hello)+1;
	const Bytef* dictionary=(const Bytef *) hello;
	stream.zalloc = (alloc_func)0;
	stream.zfree = (free_func)0;
	stream.opaque = (voidpf)0;

	err= deflateInit2_(&stream, level, 8, WindowBits, 8, 0,  zlibVersion(), sizeof(z_stream));

	if (err != Z_OK) 
		{ 
		INFO_PRINTF2(_L("deflateInit error: %d"), err); 
		free(comp);
		return err; 
		} 
	err = deflateSetDictionary(&stream,dictionary,3);
	if(err == Z_STREAM_ERROR)
		{
		free(comp);
		deflateEnd(&stream);
		return KErrNone;
		}
	stream.next_in  = (Bytef*)hello;
	stream.next_out = comp;

	while (stream.total_in != len && stream.total_out < compLen) 
		{
		stream.avail_in = stream.avail_out = 1; //* force small buffers 
		err = deflate(&stream, Z_NO_FLUSH);
		if (err != Z_OK) 
			{ 
			INFO_PRINTF2(_L("deflate return code: %d"), err); 
			deflateEnd(&stream);
			free(comp);
			return err; 
			} 
		}
	// Finish the stream, still forcing small buffers: 
	for (;;) 
		{
		stream.avail_out = 1;
		err = deflate(&stream, Z_FINISH);
		if (err == Z_STREAM_END) break;
		if (err != Z_OK)
			{ 
			INFO_PRINTF2(_L("deflate error: %d"), err); 
			deflateEnd(&stream);
			free(comp);
			return err; 
			} 
		}

	deflateEnd(&stream);
	free(comp);
	return KErrNone;

	}

/**
 * Function Name : TestDeflateSetDictionaryLen
 * TestCase Description: Supply dictLength > MAX_DIST(s)
 * Return Value: Z_OK
 */	
TInt CTestZlib::TestDeflateSetDictionaryLen()
	{
	int level=Z_DEFAULT_COMPRESSION;

	Byte *comp;
	uLong compLen;
	compLen = 30;
	comp = (Byte*)calloc((uInt)compLen, 1);

	z_stream stream; // compression stream 
	int err;
	const char hello[] = "hello, hello!";
	uLong len = (uLong)strlen(hello)+1;
	const Bytef* dictionary=(const Bytef *) hello;
	stream.zalloc = (alloc_func)0;
	stream.zfree = (free_func)0;
	stream.opaque = (voidpf)0;

	err= deflateInit2_(&stream, level, 8, 9, 8, 0,  zlibVersion(), sizeof(z_stream));

	if (err != Z_OK) 
		{ 
		INFO_PRINTF2(_L("deflateInit error: %d"), err); 
		free(comp);
		return err; 
		} 

	// Pass dictLength=251 which is > MAX_DIST(s) for window bits=9
	err = deflateSetDictionary(&stream,dictionary,251);
	if(err != Z_OK)
		{
		free(comp);
		deflateEnd(&stream);
		return err;
		}
	stream.next_in  = (Bytef*)hello;
	stream.next_out = comp;

	while (stream.total_in != len && stream.total_out < compLen) 
		{
		stream.avail_in = stream.avail_out = 1; //* force small buffers 
		err = deflate(&stream, Z_NO_FLUSH);
		if (err != Z_OK) 
			{ 
			INFO_PRINTF2(_L("deflate return code: %d"), err); 
			deflateEnd(&stream);
			free(comp);
			return err; 
			} 
		}
	// Finish the stream, still forcing small buffers: 
	for (;;) 
		{
		stream.avail_out = 1;
		err = deflate(&stream, Z_FINISH);
		if (err == Z_STREAM_END) break;
		if (err != Z_OK)
			{ 
			INFO_PRINTF2(_L("deflate error: %d"), err); 
			deflateEnd(&stream);
			free(comp);
			return err; 
			} 
		}

	deflateEnd(&stream);
	free(comp);
	return KErrNone;

	}

/**
 * Function Name : TestInflateSetDictionaryBadMode
 * TestCase Description: Supply window bits=9 making state->mode = BAD
 * Return Value: Z_STREAM_ERROR
 */
TInt CTestZlib::TestInflateSetDictionaryBadMode()
	{
	TInt res = KErrNone ;
	Byte *compr, *uncompr;
	uLong comprLen = 100*sizeof(int); 
	uLong uncomprLen = comprLen;

	compr    = (Byte*)calloc((uInt)comprLen, 1);
	if (compr == Z_NULL)
		{
		return KErrNoMemory;
		}
	uncompr  = (Byte*)calloc((uInt)uncomprLen, 1);
	if (uncompr == Z_NULL) 
		{
		free(compr);
		return KErrNoMemory;
		}
	res = Test_dict_deflate(compr, comprLen);

	if(res < 0)
		{
		free(compr);
		free(uncompr);
		return res;
		}
	if(res == 0)
		{

		int err;
		z_stream d_stream; // decompression stream 

		strcpy((char*)uncompr, "garbage");

		d_stream.zalloc = (alloc_func)0;
		d_stream.zfree = (free_func)0;
		d_stream.opaque = (voidpf)0;

		d_stream.next_in  = compr;
		d_stream.avail_in = (uInt)comprLen;

		err	= inflateInit2_(&d_stream, 9, "1.2.4", sizeof(d_stream));
		if(err < 0 )
			{
			free(compr);
			free(uncompr);
			return Z_MEM_ERROR;    	
			}

		d_stream.next_out = uncompr;
		d_stream.avail_out = (uInt)uncomprLen;

		for (;;) 
			{
			err = inflate(&d_stream, Z_NO_FLUSH);
			err = inflateSetDictionary(&d_stream, (const Bytef*)dictionary,sizeof(dictionary));
			if(err == Z_STREAM_ERROR)
				{
				res = KErrNone;
				break;
				}
			}		// end of for

		err = inflateEnd(&d_stream);
		}			// end of outer if
	else
		{
		res=KErrGeneral;
		}

	free(compr);
	free(uncompr);
	return res; 
	}

/**
 * Function Name : TestInflateSetDictionaryStreamStateNull
 * TestCase Description: Make stream->state = Z_NULL
 * Return Value: Z_STREAM_ERROR
 */    
TInt CTestZlib::TestInflateSetDictionaryStreamStateNull()
	{
	TInt res = KErrNone ;
	Byte *compr, *uncompr;
	uLong comprLen = 100*sizeof(int); 
	uLong uncomprLen = comprLen;

	compr    = (Byte*)calloc((uInt)comprLen, 1);
	if (compr == Z_NULL)
		{
		return KErrNoMemory;
		}
	uncompr  = (Byte*)calloc((uInt)uncomprLen, 1);
	if (uncompr == Z_NULL) 
		{
		free(compr);
		return KErrNoMemory;
		}
	res = Test_dict_deflate(compr, comprLen);

	if(res < 0)
		{
		free(compr);
		free(uncompr);
		return res;
		}
	if(res == 0)
		{

		int err;
		z_stream d_stream; // decompression stream 

		strcpy((char*)uncompr, "garbage");

		d_stream.zalloc = (alloc_func)0;
		d_stream.zfree = (free_func)0;
		d_stream.opaque = (voidpf)0;

		d_stream.next_in  = compr;
		d_stream.avail_in = (uInt)comprLen;

		err	= inflateInit2_(&d_stream, 15, "1.2.4", sizeof(d_stream));
		if(err < 0 )
			{
			free(compr);
			free(uncompr);
			return Z_MEM_ERROR;    	
			}

		d_stream.next_out = uncompr;
		d_stream.avail_out = (uInt)uncomprLen;

		for (;;) 
			{
			err = inflate(&d_stream, Z_NO_FLUSH);
			if (err < Z_OK)
				{
				INFO_PRINTF1(_L("Inflate failed"));
				res = KErrGeneral;
				break;
				}
			err = inflateSetDictionary(&d_stream, (const Bytef*)dictionary,sizeof(dictionary));
			if(err == Z_STREAM_ERROR)
				{
				res = KErrNone;
				break;
				}
			}
		err = inflateEnd(&d_stream);

		//for coverage improvement
		d_stream.state = Z_NULL;
		err = inflateSetDictionary(&d_stream, (const Bytef*)dictionary,sizeof(dictionary));
		if(err == Z_STREAM_ERROR)
			{
			res = KErrNone;
			}
		}		// end of outer if
	else
		{
		res=KErrGeneral;
		}

	free(compr);
	free(uncompr);
	return res; 
	}

/**
 * Function Name : TestDeflateParamsStreamStateNull
 * TestCase Description: Make stream state Z_NULL 
 * Return Value: Z_STREAM_ERROR
 */	
TInt CTestZlib::TestDeflateParamsStreamStateNull()
	{     
	TInt res = KErrNone ;
	z_stream c_stream; 		// compression stream 
	int err;
	Byte * compr;

	uLong comprLen = 20*sizeof(int); 

	compr=(Byte*)calloc((uInt)comprLen, 1);
	if (compr == Z_NULL)
		{
		return KErrNoMemory;
		}

	uLong len = (uLong)strlen(hello)+1;

	c_stream.zalloc = (alloc_func)0;
	c_stream.zfree = (free_func)0;
	c_stream.opaque = (voidpf)0;

	err = deflateInit(&c_stream, Z_DEFAULT_COMPRESSION);
	if(err <0)
		{
		free(compr);
		return err;	
		}

	c_stream.next_in  = (Bytef*)hello;
	c_stream.next_out = compr;

	err= deflateParams(&c_stream, Z_NO_COMPRESSION, Z_DEFAULT_STRATEGY);
	deflateEnd(&c_stream);

	// Equate the stream state to Z_NULL for coverage improvement
	c_stream.state = Z_NULL;
	err= deflateParams(&c_stream, Z_NO_COMPRESSION, Z_DEFAULT_STRATEGY);

	if(err == Z_STREAM_ERROR)
		{    	
		res=KErrNone;
		}
	else
		{
		res=KErrGeneral;
		}

	free(compr);
	return res;
	}

/**
 * Function Name : TestInflateSyncAvailInNull
 * TestCase Description: Make avail in Z_NULL 
 * Return Value: Z_BUF_ERROR
 */  
TInt CTestZlib::TestInflateSyncAvailInNull()
	{   
	TInt res = KErrNone ;
	uLong len = (uLong)strlen(hello)+1;

	Byte *compr, *uncompr;
	uLong comprLen = 20*sizeof(int); 
	uLong uncomprLen = comprLen;
	compr    = (Byte*)calloc((uInt)comprLen, 1);
	if (compr == Z_NULL)
		{
		return KErrNoMemory;
		}
	uncompr  = (Byte*)calloc((uInt)uncomprLen, 1);
	if (uncompr == Z_NULL) 
		{
		free(compr);
		return KErrNoMemory;
		}

	int err;
	err = compress(compr, &comprLen, (const Bytef*)hello, len);
	if(err < 0)
		{  	
		res = KErrNoMemory;
		}
	else if(err == 0)
		{  	
		res = KErrNone ;
		} 
	else
		{    	
		res = KErrGeneral;
		}   

	if(res<0)
		{
		free(compr);
		free(uncompr);
		return KErrNoMemory;	
		}

	Test_flush(compr, &comprLen);

	z_stream d_stream; /* decompression stream */

	strcpy((char*)uncompr, "garbage");

	d_stream.zalloc = (alloc_func)0;
	d_stream.zfree = (free_func)0;
	d_stream.opaque = (voidpf)0;

	d_stream.next_in  = compr;
	d_stream.avail_in = 2; /* just read the zlib header */

	err = inflateInit(&d_stream);
	if(err!=0)
		{
		res=KErrGeneral;
		}


	d_stream.next_out = uncompr;
	d_stream.avail_out = (uInt)uncomprLen;

	err=inflate(&d_stream, Z_NO_FLUSH);
	if(err!=0)
		{
		res=KErrGeneral;
		}

	// Make avail_in Z_NULL
	d_stream.avail_in = Z_NULL;
	err = inflateSync(&d_stream);           /* but skip the damaged part */
	if(err == Z_BUF_ERROR)
		{
		res=KErrNone;
		}

	free(compr);
	free(uncompr);
	err = inflateEnd(&d_stream);
	if(Z_OK != err)
		{
		INFO_PRINTF1(_L("Error encountered in inflateEnd"));
		return KErrGeneral;
		}
	return res;
	}

/**
 * Function Name : TestInflateSyncStreamNull
 * TestCase Description: Pass NULL stream to InflateSync
 * Return Value: Z_STREAM_ERROR
 */  
TInt CTestZlib::TestInflateSyncStreamNull()
	{   
	TInt res = KErrNone ;
	uLong len = (uLong)strlen(hello)+1;

	Byte *compr, *uncompr;
	uLong comprLen = 20*sizeof(int); 
	uLong uncomprLen = comprLen;
	compr    = (Byte*)calloc((uInt)comprLen, 1);
	if (compr == Z_NULL)
		{
		return KErrNoMemory;
		}
	uncompr  = (Byte*)calloc((uInt)uncomprLen, 1);
	if (uncompr == Z_NULL) 
		{
		free(compr);
		return KErrNoMemory;
		}

	int err;
	err = compress(compr, &comprLen, (const Bytef*)hello, len);
	if(err < 0)
		{  	
		res = KErrNoMemory;
		}
	else if(err == 0)
		{  	
		res = KErrNone ;
		} 
	else
		{    	
		res = KErrGeneral;
		}   

	if(res<0)
		{
		free(compr);
		free(uncompr);
		return KErrNoMemory;	
		}

	Test_flush(compr, &comprLen);

	z_stream d_stream; /* decompression stream */

	strcpy((char*)uncompr, "garbage");

	d_stream.zalloc = (alloc_func)0;
	d_stream.zfree = (free_func)0;
	d_stream.opaque = (voidpf)0;

	d_stream.next_in  = compr;
	d_stream.avail_in = 2; /* just read the zlib header */

	err = inflateInit(&d_stream);
	if(err!=0)
		{
		res=KErrGeneral;
		}

	d_stream.next_out = uncompr;
	d_stream.avail_out = (uInt)uncomprLen;

	err=inflate(&d_stream, Z_NO_FLUSH);
	if(err!=0)
		{
		res=KErrGeneral;
		}

	d_stream.avail_in = (uInt)comprLen-2;

	// Make stream Z_NULL
	err = inflateSync(Z_NULL);           
	if(err == Z_STREAM_ERROR)
		{
		res=KErrNone;
		}
	err = inflateEnd(&d_stream);
	if(Z_OK != err)
		{
		INFO_PRINTF1(_L("Error encountered in inflateEnd"));
		return KErrGeneral;
		}
	free(compr);
	free(uncompr);
	return res;
	}

/**
 * Function Name : TestInflateSyncStreamStateNull
 * TestCase Description: Make stream state = Z_NULL 
 * Return Value: Z_STREAM_ERROR
 */  
TInt CTestZlib::TestInflateSyncStreamStateNull()
	{   
	TInt res = KErrNone ;
	uLong len = (uLong)strlen(hello)+1;

	Byte *compr, *uncompr;
	uLong comprLen = 20*sizeof(int); 
	uLong uncomprLen = comprLen;
	compr    = (Byte*)calloc((uInt)comprLen, 1);
	if (compr == Z_NULL)
		{
		return KErrNoMemory;
		}
	uncompr  = (Byte*)calloc((uInt)uncomprLen, 1);
	if (uncompr == Z_NULL) 
		{
		free(compr);
		return KErrNoMemory;
		}

	int err;
	err = compress(compr, &comprLen, (const Bytef*)hello, len);
	if(err < 0)
		{  	
		res = KErrNoMemory;
		}
	else if(err == 0)
		{  	
		res = KErrNone ;
		} 
	else
		{    	
		res = KErrGeneral;
		}   

	if(res<0)
		{
		free(compr);
		free(uncompr);
		return KErrNoMemory;	
		}

	Test_flush(compr, &comprLen);

	z_stream d_stream; /* decompression stream */

	strcpy((char*)uncompr, "garbage");

	d_stream.zalloc = (alloc_func)0;
	d_stream.zfree = (free_func)0;
	d_stream.opaque = (voidpf)0;

	d_stream.next_in  = compr;
	d_stream.avail_in = 2; /* just read the zlib header */

	err = inflateInit(&d_stream);
	if(err!=0)
		{
		res=KErrGeneral;
		}
	d_stream.next_out = uncompr;
	d_stream.avail_out = (uInt)uncomprLen;
	err=inflate(&d_stream, Z_NO_FLUSH);
	if(err!=0)
		{
		res=KErrGeneral;
		}

	d_stream.avail_in = (uInt)comprLen-2;
	err = inflateSync(&d_stream);
	err = inflateEnd(&d_stream);
	if(Z_OK != err)
		{
		INFO_PRINTF1(_L("Error encountered in inflateEnd"));
		return KErrGeneral;
		}

	// for coverage improvement	
	d_stream.state = Z_NULL;
	err = inflateSync(&d_stream);

	if(err == Z_STREAM_ERROR)
		{
		res=KErrNone;
		}
	free(compr);
	free(uncompr);
	return res;
	}

/**
 * Function Name : TestInflateSyncPointStreamStateNull
 * TestCase Description: Make stream state = Z_NULL 
 * Return Value: Z_STREAM_ERROR
 */ 
TInt CTestZlib::TestInflateSyncPointStreamStateNull()
	{
	TInt res = KErrNone ;
	uLong len = (uLong)strlen(hello)+1;

	Byte *compr, *uncompr;
	uLong comprLen = 20*sizeof(int); 
	uLong uncomprLen = comprLen;
	compr    = (Byte*)calloc((uInt)comprLen, 1);
	if (compr == Z_NULL)
		{
		return KErrNoMemory;
		}
	uncompr  = (Byte*)calloc((uInt)uncomprLen, 1);
	if (uncompr == Z_NULL) 
		{
		free(compr);
		return KErrNoMemory;
		}

	z_stream stream;
	int err;

	stream.next_in = (Bytef*)hello;
	stream.avail_in = (uInt)len;

	stream.next_out = compr;
	stream.avail_out = (uInt)comprLen;

	stream.zalloc = (alloc_func)0;
	stream.zfree = (free_func)0;
	stream.opaque = (voidpf)0;

	err = deflateInit(&stream, Z_DEFAULT_COMPRESSION);
	if (err != Z_OK) 
		{
		free(compr);
		free(uncompr);
		return err;    
		}

	err = deflate(&stream, Z_FINISH);
	if (err != Z_STREAM_END) 
		{
		deflateEnd(&stream);
		free(compr);
		free(uncompr);
		return err == Z_OK ? Z_BUF_ERROR : err;
		}
	comprLen = stream.total_out;    

	err = inflateSyncPoint(&stream);
	err = deflateEnd(&stream);
	if(err == Z_STREAM_ERROR)
		{
		INFO_PRINTF1(_L("Error encountered in inflateEnd"));
		free(compr);
		free(uncompr);
		return KErrNone;
		}

	//Make stream state NULL
	stream.state = Z_NULL;
	err = inflateSyncPoint(&stream);
	//err = inflateEnd(&stream);  	  	    
	if(err == Z_STREAM_ERROR)
		{
		res=KErrNone;
		}

	free(compr);
	free(uncompr);
	return res;
	}


/**
 * Function Name : TestAdlerScenarios
 * TestCase Description: 1. len = 1, adler > BASE
 *						2. len < 16, adler > BASE
 *						3. len > NMAX
 * Return Value: Adler value
 */
TInt CTestZlib::TestAdlerScenarios()
	{
	TInt res = KErrNone ;
	//unsigned char  buffer[5]="1234";
	unsigned char buffer[5552];
	int len=0;
	int adler1=0;

	_LIT(KParam1, "Param1");
	TBool ret = GetIntFromConfig(ConfigSection(), KParam1, adler1);
	if(!ret)
		{
		INFO_PRINTF1(_L("Failed to read the values from ini file"));
		}

	_LIT(KParam2, "Param2");
	ret = GetIntFromConfig(ConfigSection(), KParam2, len);
	if(!ret)
		{
		INFO_PRINTF1(_L("Failed to read the values from ini file"));
		}

	long long adler = adler32((long)adler1, &buffer[0], (unsigned int)len);
	INFO_PRINTF2(_L("buf %x"),adler);

	if(adler > 0)
		{    	
		res=KErrNone ;    	
		}
	else
		{    	
		res = KErrGeneral;
		}
	return res;
	}

/**
 * Function Name : TestGzsetparamsFileNull
 * TestCase Description: Pass NULL pointer to gzsetparams
 * Return Value: Z_STREAM_ERROR
 */
TInt CTestZlib::TestGzsetparamsFileNull()
	{    
	TInt res = KErrNone ;

	int len = (int)strlen(hello)+1;
	gzFile file;

	const char * fname = TESTFILE;

	file = gzopen(fname, "wb");
	gzputc(file, 'h');

	int err = gzsetparams(NULL, Z_BEST_SPEED, Z_DEFAULT_STRATEGY);
	if(err == Z_STREAM_ERROR)
		{
		gzclose(file);
		res = KErrNone;
		}

	return res;
	}

/**
 * Function Name : TestGzopenWrite
 * TestCase Description: Open a gz file in write mode, close it and
 *						and then open again in read mode
 */
TInt CTestZlib::TestGzopenWrite()
	{
	gzFile file;
	const char * fname = "c:\\file.txt";
	file = gzopen(fname, "wb");

	gzputs(file, "Coverage Improvement");

	gzclose(file);

	file = gzopen(fname, "rb");

	if (file == NULL) 
		{
		ERR_PRINTF1(_L("gzopen error"));
		return KErrGeneral;
		}

	gzclose(file);

	return KErrNone;
	}

/**
 * Function Name : TestGzreadLargeFile
 * TestCase Description: Open a large gz file in read mode and read from it
 */
TInt CTestZlib::TestGzreadLargeFile()
	{
	gzFile file;
	const char *s="\0";
	uInt len = strlen(s);

	char *buf1 = (char*)malloc(1024);
	if (buf1 == Z_NULL)
		{
		return KErrNoMemory;
		}

	file = gzopen(FILETESTGZLARGE, "rb");    
	if (file == NULL) 
		{
		ERR_PRINTF1(_L("gzopen error"));
		free(buf1);
		return KErrGeneral;
		}

	for (;;)
		{
		len = gzread(file, buf1, sizeof(buf1));
		if(len<=0) break;
		}

	gzclose(file);
	free(buf1);
	return KErrNone;
	}

/**
 * Function Name : TestGzopenWriteNoPath
 * TestCase Description: Open a gz file in write mode, 
 *						close it and then open again in read mode
 */
TInt CTestZlib::TestGzopenWriteNoPath()
	{
	gzFile file;
	const char * fname = "c:\\file.txt";
	file = gzopen(fname, "wb");

	gzputs(file, "Coverage Improvement");

	gzclose(file);

	file = gzopen(fname, "rb");

	if (file == NULL) 
		{
		ERR_PRINTF1(_L("gzopen error"));
		return KErrGeneral;
		}

	gzclose(file);
	return KErrNone;
	} 
/**
 * Function Name : TestGzreadLenZero
 * TestCase Description: 1. Read from a gz file specifying read buffer length=0
 */ 	
TInt CTestZlib::TestGzreadLenZero()
	{
	char * buf1 = (char*)malloc(1024);
	if (buf1 == Z_NULL)
		{
		ERR_PRINTF1(_L("Heap out of memory"));
		return KErrNoMemory;
		}

	TInt res = KErrNone ;
	gzFile file;
	uInt len=0;
	const char * fname = "c:\\file.txt";

	// Write some text in gz file and close it
	file = gzopen(fname, "wb");
	if (file == NULL) 
		{
		free(buf1);
		ERR_PRINTF1(_L("Could not open the file"));
		res = KErrGeneral;
		return res;
		}
	gzputs(file, "Coverage Improvement");
	gzclose(file);

	file = gzopen(fname, "rb");         
	if (file == NULL) 
		{
		free(buf1);
		ERR_PRINTF1(_L("Could not open the file"));
		res = KErrGeneral;
		return res;
		}   

	for (;;)
		{
		len = gzread(file, buf1, len);
		if(len<=0) break;
		}

	if (gzclose(file) != Z_OK)
		{    	
		ERR_PRINTF1(_L("Could not close the file"));
		res=KErrGeneral;     
		}
	free(buf1);
	return res;
	}


/**
 * Function Name : TestGzreadBufZero
 * TestCase Description: 1. Read from a gz file passing a NULL for read buffer
 */ 	
TInt CTestZlib::TestGzreadBufZero()
	{
	char * buf1 = (char*)malloc(1024);
	if (buf1 == Z_NULL)
		{
		ERR_PRINTF1(_L("Heap out of memory"));
		return KErrNoMemory;
		}

	TInt res = KErrNone ;
	gzFile file;
	uInt len=0;
	const char * fname = "c:\\file.txt";

	// Write some text in gz file and close it
	file = gzopen(fname, "wb");
	if (file == NULL) 
		{
		free(buf1);
		ERR_PRINTF1(_L("Could not open the file"));
		res = KErrGeneral;
		return res;
		}
	gzputs(file, "Coverage Improvement");
	gzclose(file);

	file = gzopen(fname, "rb");         
	if (file == NULL) 
		{
		free(buf1);
		ERR_PRINTF1(_L("Could not open the file"));
		res = KErrGeneral;
		return res;
		}   

	for (;;)
		{
		len = gzread(file, NULL, sizeof(buf1));
		if(len<=0) break;
		}

	if (gzclose(file) == Z_STREAM_ERROR)
		{    	
		res=KErrNone;     
		}
	free(buf1);
	return res;

	}

/**
 * Function Name : TestGzreadNonGzFile
 * TestCase Description: 1. Read from a non gz file 
 */ 
TInt CTestZlib::TestGzreadNonGzFile()
	{
	char *buf1 = (char*)malloc(1024);
	if (buf1 == Z_NULL)
		{
		ERR_PRINTF1(_L("Heap out of memory"));
		return KErrNoMemory;
		}

	TInt res = KErrNone ;
	gzFile file;
	uInt len=0;
	const char * fname = "c:\\file.txt";

	// Write in txt file and close it
	FILE *fp = fopen(fname,"w");
	if(fp == NULL)
		{
		free(buf1);
		ERR_PRINTF1(_L("Could not open the output file."));
		res = KErrGeneral;
		return res;
		}
	
	fputc('\n',fp);
	fclose(fp);

	file = gzopen(fname, "rb");         
	if (file == NULL) 
		{
		free(buf1);
		ERR_PRINTF1(_L("Could not open the file"));
		res = KErrGeneral;
		return res;
		}   

	for (;;)
		{
		len = gzread(file, buf1, sizeof(buf1));
		if(len<=0) break;
		}

	if (gzclose(file) == Z_STREAM_ERROR)
		{    	
		res=KErrNone;     
		}
	free(buf1);
	return res;
	}

/**
 * Function Name : TestGzrewindNonGzFile
 * TestCase Description: 1. Rewind in a non gz file 
 */ 
TInt CTestZlib::TestGzrewindNonGzFile()
	{    
	TInt res = KErrNone ;
	int err;
	int len = (int)strlen(hello)+1;
	gzFile file;
	const char * fname = "c:\\file.txt";

	FILE *fp=NULL;
	fp=fopen(fname,"w");
	fputc('\n',fp);
	fclose(fp);

	file = gzopen(fname, "rb");
	if (file == NULL) 
		{
		res = KErrGeneral;
		}
	err = gzrewind(file);

	if(err == 0)
		{    	
		res = KErrNone ;    	
		}
	else
		{    	
		res = KErrGeneral;
		}
	err=  gzclose(file);
	if (err != Z_OK)  
		{
		res = KErrGeneral; 
		} 

	return res;
	}

/**
 * Function Name : TestGzrewindFileNull
 * TestCase Description: Pass NULL to gzrewind
 */
TInt CTestZlib::TestGzrewindFileNull()
	{    
	TInt res = KErrNone ;
	int err;
	int len = (int)strlen(hello)+1;
	gzFile file;

	const char * fname = TESTFILE;
	file = gzopen(fname, "rb");
	if (file == NULL) 
		{
		res = KErrGeneral;
		}
	err = gzrewind(NULL);

	if(err == Z_ERRNO)
		{    	
		res = KErrNone ;    	
		}
	else
		{    	
		res = KErrGeneral;
		}
	err=  gzclose(file);
	if (err != Z_OK)  
		{
		res = KErrGeneral; 
		}

	return res;
	}

/**
 * Function Name : TestGzflushWithZFinish
 * TestCase Description: Flush with flush = Z_FINISH
 */
TInt CTestZlib::TestGzflushWithZFinish()
	{
	TInt res = KErrNone ;
	gzFile file;

	const char * fname = TESTFILE ;
	file = gzopen(fname, "wb");
	if (file == Z_NULL)  
		{
		res = KErrNoMemory; 
		return res;
		}   
	int l= gzflush(file,Z_FINISH);
	if(l != Z_OK)  
		{
		res = KErrGeneral;  
		}

	l = gzclose(file);
	if (l != Z_OK)  
		{
		res = KErrGeneral;     	    	
		}
	return res;
	}

/**
 * Function Name : TestUncompressLenSmall
 * TestCase Description: Supply uncompress length smaller than compress length
 * Return value:	Z_BUF_ERROR
 */ 	
TInt CTestZlib::TestUncompressLenSmall()
	{
	TInt res = KErrNone;
	TInt err=0;
	Byte *compr, *uncompr;
	uLong len = (uLong)strlen(hello)+1;

	uLong comprLen = 20*sizeof(int); 
	uLong uncomprLen = 3*sizeof(int);
	compr = (Byte*)calloc((uInt)comprLen, 1);
	if (compr == Z_NULL)
		{
		return KErrNoMemory;
		}

	uncompr  = (Byte*)calloc((uInt)uncomprLen, 1);
	if (uncompr == Z_NULL) 
		{
		free(compr);
		return KErrNoMemory;
		}

	err = compress(compr, &comprLen, (const Bytef*)hello, len);
	if(err == 0)
		{    	
		strcpy((char*)uncompr, "garbage");
		err = uncompress(uncompr, &uncomprLen, compr, comprLen);
		if(err == Z_BUF_ERROR)
			{
			res = KErrNone ;
			}    
		else
			{
			res = KErrGeneral;
			}       
		} 

	free(compr);
	free(uncompr);
	return res;
	} 

/**
 * Function Name : TestUncompressBufNull
 * TestCase Description: Supply NULL string to uncompress
 * Return value:	Z_STREAM_ERROR
 */ 	
TInt CTestZlib::TestUncompressBufNull()
	{
	TInt res = KErrNone;
	TInt err=0;
	Byte *compr, *uncompr;
	uLong len = (uLong)strlen(hello)+1;

	uLong comprLen = 20*sizeof(int); 
	uLong uncomprLen = 3*sizeof(int);
	compr = (Byte*)calloc((uInt)comprLen, 1);
	if (compr == Z_NULL)
		{
		return KErrNoMemory;
		}

	uncompr  = (Byte*)calloc((uInt)uncomprLen, 1);
	if (uncompr == Z_NULL) 
		{
		free(compr);
		return KErrNoMemory;
		}

	err = compress(compr, &comprLen, (const Bytef*)hello, len);
	if(err == 0)
		{    	
		strcpy((char*)uncompr, "garbage");
		err = uncompress(uncompr, &uncomprLen, Z_NULL, comprLen);
		if(err == Z_STREAM_ERROR)
			{
			res = KErrNone ;
			}    
		else
			{
			res = KErrGeneral;
			}

		} 

	free(compr);
	free(uncompr);
	return res;
	} 	

/**
 * Function Name : TestUncompressLenNull
 * TestCase Description: Supply uncompress length=0
 * Return value:	Z_DATA_ERROR
 */ 	
TInt CTestZlib::TestUncompressLenNull()
	{
	TInt res = KErrNone;
	TInt err=0;
	Byte *compr, *uncompr;
	uLong len = (uLong)strlen(hello)+1;

	uLong comprLen = 20*sizeof(int); 
	uLong uncomprLen = 3*sizeof(int);
	compr = (Byte*)calloc((uInt)comprLen, 1);
	if (compr == Z_NULL)
		{
		return KErrNoMemory;
		}

	uncompr  = (Byte*)calloc((uInt)uncomprLen, 1);
	if (uncompr == Z_NULL) 
		{
		free(compr);
		return KErrNoMemory;
		}

	err = compress(compr, &comprLen, (const Bytef*)hello, len);
	if(err == 0)
		{    	
		strcpy((char*)uncompr, "garbage");
		err = uncompress(uncompr, &uncomprLen, compr, 0);
		if(err == Z_DATA_ERROR)
			{
			res = KErrNone ;
			}    
		else
			{
			res = KErrGeneral;
			}
		} 

	free(compr);
	free(uncompr);
	return res;
	}


/**
 * Function Name : TestInflateScenarios
 * TestCase Description: 1. Pass Windowbits = 15 for inflateInit2_, Z_SYNC_FLUSH for inflate
 *						2. Pass Windowbits = -15 for inflateInit2_, Z_SYNC_FLUSH for inflate
 *						3. Pass Windowbits = 29 for inflateInit2_, Z_BLOCK for inflate
 * Return Value: Z_OK
 */
TInt CTestZlib::TestInflateScenarios()
	{
	TInt res = KErrNone ;
	TInt WindowBits = 0;
	TInt flush=0;
	Byte *compr, *uncompr;
	uLong comprLen = 20*sizeof(int); 
	uLong uncomprLen = comprLen;
	compr    = (Byte*)calloc((uInt)comprLen, 1);
	if (compr == Z_NULL)
		{
		return KErrNoMemory;
		}
	uncompr  = (Byte*)calloc((uInt)uncomprLen, 1);
	if (uncompr == Z_NULL) 
		{
		free(compr);
		return KErrNoMemory;
		}
	res = Test_deflate(compr, comprLen);
	if(res<0)
		{
		free(compr);
		free(uncompr);
		return KErrNoMemory;	
		}

	int err=0;
	z_stream d_stream; /* decompression stream */

	strcpy((char*)uncompr, "garbage");

	d_stream.zalloc = (alloc_func)0;
	d_stream.zfree = (free_func)0;
	d_stream.opaque = (voidpf)0;

	d_stream.next_in  = compr;
	d_stream.avail_in = 0;
	d_stream.next_out = uncompr;

	ReadIntParam(WindowBits);
	res = inflateInit2_(&d_stream, WindowBits, "1.2.4", sizeof(d_stream));

	if(res<0)
		{
		free(compr);
		free(uncompr);
		return KErrNoMemory;	
		}

	ReadIntParam(flush);
	while (d_stream.total_out < uncomprLen && d_stream.total_in < comprLen) 
		{
		d_stream.avail_in = d_stream.avail_out = 1; /* force small buffers */
		err = inflate(&d_stream, flush);

		if (err == Z_STREAM_END || err == Z_MEM_ERROR || err == Z_DATA_ERROR) break;
		}
	if(err == Z_MEM_ERROR)
		{
		err = inflateEnd(&d_stream);
		free(compr);
		free(uncompr);

		return err;
		}

	err = inflateEnd(&d_stream);
	if (err != Z_OK)  
		{
		res = KErrGeneral;     			
		} 
	else
		{
		res=KErrNone ;
		}
	free(compr);
	free(uncompr);
	return res;

	}


/**
 * Function Name : TestInflateStreamStateNull
 * TestCase Description: Make stream.state = NULL and call inflate
 * Return Value: Z_STREAM_ERROR
 */
TInt CTestZlib::TestInflateStreamStateNull()
	{
	TInt res = KErrNone ;
	Byte *compr, *uncompr;
	uLong comprLen = 20*sizeof(int); 
	uLong uncomprLen = comprLen;
	compr    = (Byte*)calloc((uInt)comprLen, 1);
	if (compr == Z_NULL)
		{
		return KErrNoMemory;
		}
	uncompr  = (Byte*)calloc((uInt)uncomprLen, 1);
	if (uncompr == Z_NULL) 
		{
		free(compr);
		return KErrNoMemory;
		}
	res = Test_deflate(compr, comprLen);
	if(res<0)
		{
		free(compr);
		free(uncompr);
		return KErrNoMemory;	
		}

	int err=0;
	z_stream d_stream; /* decompression stream */

	strcpy((char*)uncompr, "garbage");

	d_stream.zalloc = (alloc_func)0;
	d_stream.zfree = (free_func)0;
	d_stream.opaque = (voidpf)0;

	d_stream.next_in  = compr;
	d_stream.avail_in = 0;
	d_stream.next_out = uncompr;

	res = inflateInit2_(&d_stream, 15, "1.2.4", sizeof(d_stream));

	if(res<0)
		{
		free(compr);
		free(uncompr);
		return KErrNoMemory;	
		}

	while (d_stream.total_out < uncomprLen && d_stream.total_in < comprLen) 
		{
		d_stream.avail_in = d_stream.avail_out = 1; /* force small buffers */
		err = inflate(&d_stream, Z_SYNC_FLUSH);

		if (err == Z_STREAM_END || err == Z_STREAM_ERROR || err == Z_MEM_ERROR) break;
		}

	if(err == Z_STREAM_ERROR)
		{
		res=KErrNone;
		}
	err = inflateEnd(&d_stream);
	if (err == Z_STREAM_ERROR)  
		{
		res=KErrNone;
		} 

	d_stream.state = Z_NULL;
	err = inflate(&d_stream, Z_SYNC_FLUSH);
	if (err == Z_STREAM_ERROR)
		{
		res = KErrNone;
		}

	free(compr);
	free(uncompr);
	return res;    
	}

/**
 * Function Name : TestInflateResetStreamStateNull
 * TestCase Description: Make stream.state = NULL and call inflateReset
 * Return Value: Z_STREAM_ERROR
 */
TInt CTestZlib::TestInflateResetStreamStateNull()
	{  
	TInt res=KErrNone;
	z_stream d_stream; 		/* decompression stream */
	const char * version;
	d_stream.zalloc = (alloc_func)0;
	d_stream.zfree = (free_func)0;
	d_stream.opaque = (voidpf)0;

	Byte *compr, *uncompr;
	uLong comprLen = 20*sizeof(int); 
	uLong uncomprLen = comprLen;
	compr    = (Byte*)calloc((uInt)comprLen, 1);
	if (compr == Z_NULL)
		{
		return KErrNoMemory;
		}
	uncompr  = (Byte*)calloc((uInt)uncomprLen, 1);
	if (uncompr == Z_NULL) 
		{
		free(compr);
		return KErrNoMemory;
		}

	res = Test_deflate(compr, comprLen);
	if(res<0)
		{
		free(compr);
		free(uncompr);
		return KErrNoMemory;	
		}
	d_stream.next_in  = compr;
	d_stream.avail_in = 0;
	d_stream.next_out = uncompr;
	version=zlibVersion();
	int err;
	err = inflateInit_(&d_stream,(char*)version, sizeof(d_stream));

	if(err<0)
		{
		free(compr);
		free(uncompr);
		return KErrNoMemory;	
		}

	err=inflateReset(&d_stream);
	err=inflateEnd(&d_stream);
	free(compr);
	free(uncompr);

	// for coverage improvement
	d_stream.state = Z_NULL;
	err=inflateReset(&d_stream); 

	if(err == Z_STREAM_ERROR)
		{
		res = KErrNone;
		}
	else
		{
		res = KErrGeneral;
		}
	return res;
	}
