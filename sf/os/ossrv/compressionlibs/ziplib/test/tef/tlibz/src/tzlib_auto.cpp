/*
* Copyright (c) 2007 Nokia Corporation and/or its subsidiary(-ies).
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


#include "tzlib.h"

/**
 * Function Name : TestAdlerinit
 * TestCase Description: Testing Adler
 * Return Value: Return KErrNone if adler32=1, else return KErrGeneral
 */
TInt CTestZlib::TestAdlerinit()
	{
	TInt res = KErrNone ;
	unsigned long j=0L;

	long adler1 = adler32(j,0, 0);
	if(adler1 == 1)
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
 * Function Name : TestAdler
 * TestCase Description: Testing Adler32
 * Return Value:  Return KErrNone if adler32>0, else return KErrGeneral
 */
TInt CTestZlib::TestAdler()
	{
	TInt res = KErrNone ;
	unsigned char  buffer[5]="1234";
	unsigned int i=4;
	unsigned long j=0L;

	long adler1 = adler32(j,0, 0);
	long adler = adler32(adler1, &buffer[0], i);
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
 * Function Name : TestDeflateReset
 * TestCase Description: Call deflateReset after deflateInit
 * Return Value: Returns KErrNone on deflateReset returning Z_OK, else KErrGeneral
 */  
TInt CTestZlib::TestDeflateReset()
	{ 
	INFO_PRINTF1(_L("DeflateReset test with valid input"));
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
		return KErrGeneral;
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
if (err != Z_OK)  
	{
	res = KErrGeneral;     			
	} 
else  
	{    	
	res = KErrNone;
	}   
err = deflateEnd(&stream);
if (err != Z_OK) 
	{
	INFO_PRINTF1(_L("Error in deflateEnd"));
	free(compr);
	free(uncompr);	
	return KErrGeneral;
	}
free(compr);
free(uncompr);
return res;
	}

/**
 * Function Name : TestDeflateReset_fail
 * TestCase Description: Call deflateReset before calling deflateInit
 * Return Value: Return KErrNone incase of deflateReset returning Z_STREAM_ERROR, else KErrGeneral
 */ 
TInt CTestZlib::TestDeflateReset_fail()
	{ 
	TInt res = KErrNone ;
	uLong len = (uLong)strlen(hello)+1;
	Byte *compr, *uncompr;
	uLong comprLen = 20*sizeof(int); 
	uLong uncomprLen = comprLen;
	int err;
	z_stream stream;
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

	stream.zalloc = (alloc_func)0;
	stream.zfree = (free_func)0;
	stream.opaque = (voidpf)0;
	err=deflateReset(&stream);
	if (err != Z_OK)  
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
 * Function Name : TestDeflateInit2_bits
 * TestCase Description: Call deflateInit2_ with window bits = 15 as argument
 * Return Value: Return KErrNone if deflateInit2_ returns Z_OK
 */
TInt CTestZlib::TestDeflateInit2_bits()
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

	err= deflateInit2_(&stream, level, 3, 15, 8, 0, zlibVersion(), sizeof(z_stream));

	if (err != Z_OK) 
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
 * Function Name : TestDeflateInit2_level
 * TestCase Description: Call deflateInit2_ with window bits = 2 as argument
 * Return Value: Returns KErrNone on deflateInit2_ returning Z_OK
 */
TInt CTestZlib::TestDeflateInit2_level()
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

	err= deflateInit2_(&stream, level, 3, 2, 8,
			0,  zlibVersion(), sizeof(z_stream));

	if (err != Z_OK) 
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
 * Function Name : TestInflateInit2_bits
 * TestCase Description: Call inflateInit2_ with window bits = 5 as argument
 * Return Value: Returns KErrNone on inflateInit2_ returning Z_OK
 */  
TInt CTestZlib::TestInflateInit2_bits()
	{
	TInt res = KErrNone ;
	z_stream d_stream; // decompression stream
	const char * version;
	d_stream.zalloc = (alloc_func)0;
	d_stream.zfree = (free_func)0;
	d_stream.opaque = (voidpf)0;

	version = zlibVersion();
	int ret =  inflateInit2_(&d_stream,5,version, sizeof(d_stream));
	if(ret==0)
		{ 
		res = KErrGeneral;    	
		}
	else
		{    
		res = KErrNone;    
		} 
	return res;       
	}

/**
 * Function Name : TestGzread
 * TestCase Description: 1. Open a gz file in read mode
 *						2. gzRead from the opened file. Reads the given number of uncompressed bytes from the compressed file.
 *						3. close the gz file
 * Return Value: Returns KErrNone on reading the gzfile successfully
 */  
TInt CTestZlib::TestGzread()
	{
	char * buf1 = (char*)malloc(1024);
	if (buf1 == NULL)
		{
		ERR_PRINTF1(_L("Heap out of memory"));
		return KErrNoMemory;
		}

	int len1;
	TInt res = KErrNone ;
	char  *file=FILETESTGZ;
	char *infile;
	gzFile in;
	infile = file;
	in = gzopen(infile, "rb");           
	if (in == Z_NULL) 
		{
		free(buf1);
		ERR_PRINTF1(_L("Could not open the file"));
		res = KErrGeneral;
		return res;
		}   
	for (;;)
		{
		len1 = gzread(in, buf1, sizeof(buf1));
		if (len1 == 0 )
			break;
		else if(len1 < 0 ) 
			{
			INFO_PRINTF1(_L("Error in reading the file"));
			free(buf1);
			return KErrGeneral;
			}
		}
	if (gzclose(in) != Z_OK)
		{    	
		ERR_PRINTF1(_L("Could not close the file"));
		res=KErrGeneral;     
		}
	free(buf1);
	return res;
	}   

/**
 * Function Name : TestGzread_fail
 * TestCase Description: 1. Open a gz file in write mode
 *						2. gzRead from the file. 
 *						3. close the file
 * Return Value: Returns KErrNone on gzread returning -1
 */   
TInt CTestZlib::TestGzread_fail()
	{
	char * buf1 = (char*)malloc(1024);
	if(buf1==NULL)
		{
		return KErrNoMemory;
		}
	int len1;
	TInt res = KErrNone ;
	gzFile in = Z_NULL;
	in = gzopen(FILETESTGZ1, "wb"); 
	res=(int)in;
	if (0 == res)
		{
		free(buf1);
		return KErrNoMemory;;
		}

	len1 = gzread(in, buf1, sizeof(buf1));
	if (len1 < 0) 
		{
		res = KErrNone;
		}
	else if (0 == len1)
		{
		res = KErrGeneral;
		}

	if (gzclose(in) != Z_OK)
		{
		free(buf1);
		INFO_PRINTF1(_L("Error encountered while closing the file"));
		return KErrGeneral;     
		}

	free(buf1);
	return res;
	}   
/**
 * Function Name : test_uncompress
 * TestCase Description: This is a global function used by many test functions
 */ 
TInt test_uncompress(Byte * compr,uLong comprLen,Byte * uncompr,uLong uncomprLen)
	{
	TInt res = KErrNone ;
	int err;
	uLong len = (uLong)strlen(hello)+1;

	err = compress(compr, &comprLen, (const Bytef*)hello, len);

	if(err == 0)
		{    	
		strcpy((char*)uncompr, "garbage");
		err = uncompress(uncompr, &uncomprLen, compr, comprLen);
		if(err < Z_OK)
			{
			res = KErrGeneral ;
			}    
		} 
	else
		{    	
		res = KErrGeneral;
		}

	return res;
	}
/**
 * Function Name : TestUncompress
 * TestCase Description: 1. Compress a text sample
 *						2. Uncompress is called with appropriate lengths 
 *						   for compressed and uncompressed data
 *	
 * Return Value: Z_OK
 */ 
TInt CTestZlib::TestUncompress()
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
	res =  test_uncompress(compr, comprLen, uncompr, uncomprLen);
	free(compr);
	free(uncompr);
	return res;
	}

/**
 * Function Name : TestUncompressfail
 * TestCase Description: 1. Uncompress is called with uncompressed length smaller than required
 *	
 * Return Value: Z_BUF_ERROR
 */   
TInt CTestZlib::TestUncompressfail()
	{
	TInt res = KErrNone ;
	Byte *compr, *uncompr;
	uLong comprLen = 1*sizeof(int); 
	uLong uncomprLen = comprLen;
	compr    = (Byte*)calloc((uInt)comprLen, 1);
	if (compr == Z_NULL)
		{
		return Z_MEM_ERROR;
		}
	uncompr  = (Byte*)calloc((uInt)uncomprLen, 1);
	if (uncompr == Z_NULL) 
		{
		free(compr);
		return KErrNoMemory;
		}
	int err =  test_uncompress(compr, comprLen, uncompr, uncomprLen);
	if(err==0)
		{
		res = KErrGeneral;
		} 
	free(compr);
	free(uncompr);
	return res;
	}

/**
 * Function Name : Test_dict_deflate
 * TestCase Description: This is a global function used by many test functions
 */ 
TInt CTestZlib::Test_dict_deflate( Byte * compr,uLong comprLen)
	{
	TInt res = KErrNone ;
	z_stream c_stream; /* compression stream */
	int err;

	c_stream.zalloc = (alloc_func)0;
	c_stream.zfree = (free_func)0;
	c_stream.opaque = (voidpf)0;

	err = deflateInit(&c_stream, Z_BEST_COMPRESSION);
	if(err<0)
		{
		res = KErrGeneral;
		return res;	
		}

	err = deflateSetDictionary(&c_stream,
			(const Bytef*)dictionary, sizeof(dictionary));
	if(err<0)
		{
		res = KErrGeneral;
		return res;
		}
	dictId = c_stream.adler;
	c_stream.next_out = compr;
	c_stream.avail_out = (uInt)comprLen;

	c_stream.next_in = (Bytef*)hello;
	c_stream.avail_in = (uInt)strlen(hello)+1;

	err = deflate(&c_stream, Z_FINISH);
	if (err != Z_STREAM_END) 
		{
		res = KErrGeneral;
		}
	err = deflateEnd(&c_stream);
	if (err != 0) 
		{
		res = KErrGeneral;
		}
	return res;
	}


/**
 * Function Name : Test_dict_inflate
 * TestCase Description: This is an utility function used by many test functions
 */
TInt CTestZlib::Test_dict_inflate(Byte * compr,uLong comprLen, Byte * uncompr,uLong uncomprLen)
	{
	TInt res = KErrNone ;
	int err;
	z_stream d_stream; /* decompression stream */

	strcpy((char*)uncompr, "garbage");

	d_stream.zalloc = (alloc_func)0;
	d_stream.zfree = (free_func)0;
	d_stream.opaque = (voidpf)0;

	d_stream.next_in  = compr;
	d_stream.avail_in = (uInt)comprLen;

	err = inflateInit(&d_stream);
	if(Z_MEM_ERROR == err)
		{
		return Z_MEM_ERROR;    	
		}
	else if(Z_VERSION_ERROR == err)
		{
		return Z_VERSION_ERROR;    	
		}

	d_stream.next_out = uncompr;
	d_stream.avail_out = (uInt)uncomprLen;

	for (;;) 
		{
		err = inflate(&d_stream, Z_NO_FLUSH);
		if (err == Z_NEED_DICT) 
			{
			if (d_stream.adler != dictId) 
				{
				break;
				}
			err = inflateSetDictionary(&d_stream, (const Bytef*)dictionary,
					sizeof(dictionary));
			if(err < Z_OK)
				{
				INFO_PRINTF1(_L("Error returned by inflateSetDictionary"));
				break;
				}
			}
		else
			{
			break;
			}
		}		// end of for

		err = inflateEnd(&d_stream);
		if (strcmp((char*)uncompr, hello)) 
			{
			INFO_PRINTF1(_L("Bad inflate with dictionary"));
			res = KErrGeneral;
			} 
		else 
			{
			INFO_PRINTF1(_L("Inflate with dictionary successful"));
			}
		return res;
	}

/**
 * Function Name : TestInflateSetDictionary
 * TestCase Description: 1.	Deflate string using an existing dictionary
 *						2.	Uncompress the compressed text using an existing dictionary
 * Return Value: Z_BUF_ERROR
 */ 
TInt CTestZlib::TestInflateSetDictionary()
	{
	TInt res = KErrNone ;
	Byte *compr, *uncompr;
	uLong comprLen = 100*sizeof(int); 
	uLong uncomprLen = comprLen;

	compr = (Byte*)calloc((uInt)comprLen, 1);
	if (compr == NULL)
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
	if(res==0)
		{
		res=Test_dict_inflate(compr, comprLen, uncompr, uncomprLen);	
		}
	else
		{
		res=KErrGeneral;
		}
	free(compr);
	free(uncompr);
	return res;
	}


/**
 * Function Name : TestInflateSetDictionary_size
 * TestCase Description: 1.	Deflate string using an existing dictionary
 *						2.	Uncompress the compresses text using a mismatching dictionary
 * Return Value: Z_DATA_ERROR
 */    
TInt CTestZlib::TestInflateSetDictionary_size()
	{ 

	TInt res=KErrNone;
	Byte *compr, *uncompr;
	uLong comprLen = 20*sizeof(int); 
	uLong uncomprLen = comprLen;
	z_stream d_stream; /* decompression stream */

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

	res = Test_dict_deflate(compr, comprLen);
	if(res<0)
		{
		res=KErrGeneral;
		free(compr);
		free(uncompr);
		return res;
		}
	strcpy((char*)uncompr, "garbage");

	d_stream.zalloc = (alloc_func)0;
	d_stream.zfree = (free_func)0;
	d_stream.opaque = (voidpf)0;

	d_stream.next_in  = compr;
	d_stream.avail_in = (uInt)comprLen;

	int err = inflateInit(&d_stream);

	d_stream.next_out = uncompr;
	d_stream.avail_out = (uInt)uncomprLen;

	for (;;) 
		{
		int err = inflate(&d_stream, Z_NO_FLUSH);
		if (err == Z_STREAM_END || err == Z_MEM_ERROR) break;
		err = inflateSetDictionary(&d_stream, (const Bytef*)dictionary,sizeof(dictionary)+2);
		if(Z_DATA_ERROR == err)
			{
			res=KErrNone; 
			break;     
			}
		else 
			{
			res=KErrGeneral; 
			break;
			}
		}
	err = inflateEnd(&d_stream);
	if(err != Z_OK)
		{
		INFO_PRINTF1(_L("InflateEnd failed"));
		free(compr);
		free(uncompr);
		return KErrGeneral;
		}

	free(compr);
	free(uncompr);
	return res;
	}

/**
 * Function Name : TestInflateSetDictionary_null
 * TestCase Description: 1.	Deflate string using an existing dictionary
 *						2.	Pass null pointer to inflateSetDictionary
 * Return Value: Z_DATA_ERROR
 */     
TInt CTestZlib::TestInflateSetDictionary_null()
	{
	TInt res=KErrNone;
	Byte *compr, *uncompr;
	uLong comprLen = 20*sizeof(int); 
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

	res= Test_dict_deflate(compr, comprLen);
	if(res == KErrGeneral)
		{
		free(compr);
		free(uncompr);
		return res;
		}
	int err = inflateSetDictionary(NULL, (const Bytef*)dictionary,sizeof(dictionary));
	if(err != Z_STREAM_ERROR)
		{
		res=KErrGeneral;
		}
	free(compr);
	free(uncompr);
	return res;
	}

/**
 * Function Name : test_compress
 * TestCase Description: This is a global function used by many test functions
 */
TInt test_compress(Byte * compr,uLong comprLen,Byte * uncompr,uLong uncomprLen)
	{
	int err = KErrNone;
	uLong len = (uLong)strlen(hello)+1;

	err = compress(compr, &comprLen, (const Bytef*)hello, len);

	if(err<0)
		{
		return err;
		}
	strcpy((char*)uncompr, "garbage");

	err = uncompress(uncompr, &uncomprLen, compr, comprLen);

	if(err<0)
		{
		return err;
		}
	if (strcmp((char*)uncompr, hello)) 
		{
		//INFO_PRINTF1(_L("Uncompressed string does not match with original string"));
		err = KErrGeneral;
		}
	else 
		{
		err=0;
		}
	return err;
	}

/**
 * Function Name : test_gzgets
 * TestCase Description: This is a global function used by many test functions
 */
int test_gzgets(const char * fname, Byte * uncompr,uLong  uncomprLen)
	{
	int err = KErrNone;
	int len = (int)strlen(hello)+1;
	gzFile file;

	file = gzopen(fname, "wb");
	if (file == NULL) 
		{
		err = KErrGeneral; 
		return err;        
		}
	gzputc(file, 'h');
	if (gzputs(file, "ello") != 4) 
		{
		err=1;
		}
	if (gzprintf(file, ", %s!", "hello") != 8) 
		{
		err=1;
		}
	err = gzseek(file, 1L, SEEK_CUR); /* add one zero byte */
	if (err < 0)
		{
		//INFO_PRINTF1(_L("Error returned by gzssek"));
		}
	gzclose(file);

	file = gzopen(fname, "rb");
	if (file == NULL) 
		{
		err = KErrGeneral; 
		return err;        
		}
	strcpy((char*)uncompr, "garbage");
	if (gzread(file, uncompr, (unsigned)uncomprLen) != len) 
		{
		}
	if (strcmp((char*)uncompr, hello))
		{
		err=0;
		}
	else 
		{
		err=1;
		}

	err = gzseek(file, -8L, SEEK_CUR);
	if (err < 0)
		{
		//INFO_PRINTF1(_L("Error returned by gzssek"));
		}
	gzgets(file, (char*)uncompr, (int)uncomprLen);

	if (strlen((char*)uncompr) != 7) // " hello!" 
		{ 
		err=0;
		}
	gzclose(file);    
	return err;
	}

/**
 * Function Name : TestGzgets
 * TestCase Description: 1. Open a gz compressed file in read mode
 *						2. Read certain valid number of bytes from the compressed file
 * Return Value: Returns read buffer
 */
TInt CTestZlib::TestGzgets()
	{ 	
	TInt res = KErrNone ;
	Byte *compr, *uncompr;
	uLong comprLen = 20*sizeof(int); 

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

	res = test_compress(compr, comprLen, uncompr, uncomprLen);
	if(res < 0)
		{
		free(compr);
		free(uncompr);
		return KErrNoMemory;
		}
	int err=test_gzgets(MYFILE,uncompr, uncomprLen);
	if(err == 0)
		{    	
		res = KErrGeneral;
		}
	free(compr);
	free(uncompr);
	return res;
	}

/**
 * Function Name : test_gzgetsfail
 * TestCase Description: This is a global function used by many test functions
 */
TInt test_gzgetsfail(const char * fname, Byte * uncompr,int  uncomprLen)
	{
	TInt res = KErrNone ;
	char* err;
	int len = (int)strlen(hello)+1;
	gzFile file;

	file = gzopen(fname, "wb");
	if (file == NULL) 
		{
		res = KErrGeneral;
		}
	err = gzgets(file, (char*)uncompr, uncomprLen);

	if(err == 0)
		{    	
		res = KErrNone ;    	
		}
	else
		{    	
		res = KErrGeneral;
		}
	int  err1=  gzclose(file);
	if (err1 != Z_OK)  
		{
		res = KErrGeneral; 
		}     
	return res;
	}


/**
 * Function Name : TestgzgetsFail
 * TestCase Description: 1. Open a gz compressed file in read mode
 *						2. Invalid number of bytes (negative number) for gzgets
 * Return Value: Z_NULL
 */
TInt CTestZlib::TestgzgetsFail()
	{
	TInt res = KErrNone ; 
	Byte *compr, *uncompr;
	uLong comprLen = 20*sizeof(int); 
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
	test_compress(compr, comprLen, uncompr, uncomprLen);
	int ret=  test_gzgetsfail(TESTFILE,uncompr, -1);
	if(ret == 0)
		{    	
		res = KErrNone ;    	
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
 * Function Name : TestgzgetsopenFail
 * TestCase Description: 1. Open a gz compressed file in read mode
 *						2. Invalid number of bytes (negative number) for gzgets
 * Return Value: KErrNone
 */   
TInt CTestZlib::TestgzgetsopenFail()
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
	res=test_compress(compr, comprLen, uncompr, uncomprLen);
	if(res < 0)
		{
		free(compr);
		free(uncompr);
		return KErrNoMemory;
		}
	int err = test_gzgetsfail(NOFILE,uncompr, uncomprLen);

	if(err != 0)
		{    	
		res = KErrNone ;    	
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
 * Function Name : Test_deflate
 * TestCase Description: 1. Call deflateInit with valid arguments
 *						2. Call deflate with necessary valid arguments
 * Return Value: Z_OK
 */ 
TInt CTestZlib::Test_deflate( Byte *compr, uLong comprLen)
	{
	TInt res = KErrNone ;
	z_stream c_stream; /* compression stream */
	int err;
	uLong len = (uLong)strlen(hello)+1;

	c_stream.zalloc = (alloc_func)0;
	c_stream.zfree = (free_func)0;
	c_stream.opaque = (voidpf)0;

	err = deflateInit(&c_stream, Z_DEFAULT_COMPRESSION);

	if(err<0)
		{
		return err;
		}
	c_stream.next_in = (Bytef*)hello;
	c_stream.next_out = compr;

	while (c_stream.total_in != len && c_stream.total_out < comprLen) 
		{
		c_stream.avail_in = c_stream.avail_out = 1; /* force small buffers */
		err = deflate(&c_stream, Z_NO_FLUSH);
		}
	/* Finish the stream, still forcing small buffers: */
	for (;;) 
		{
		c_stream.avail_out = 1;
		err = deflate(&c_stream, Z_FINISH);
		if (err == Z_STREAM_END) break;
		if(err!=Z_OK)
			{
			res = KErrGeneral;
			}    
		}
	err = deflateEnd(&c_stream);
	if(err!=Z_OK)
		{
		res = KErrGeneral;
		} 
	return res;
	}


/**
 * Function Name : TestInflate
 * TestCase Description: 1. Compress the data using deflate
 *						2. inflateInit to initialize internal stream state for decompression
 *						3. inflate with necessary valid arguments
 * Return Value: Z_OK
 */
TInt CTestZlib::TestInflate()
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
	// test_compress_positive(compr, comprLen, uncompr, uncomprLen);
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

	res = inflateInit(&d_stream);

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

		if (err == Z_STREAM_END || err == Z_MEM_ERROR) break;
		}
	//inflate() should normally be called until it returns Z_STREAM_END 

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
 * Function Name : TestInflate_fail1
 * TestCase Description: 1. Compress the data using deflate
 *						2. inflateInit to initialize internal stream state for decompression
 *						3. Set avail_in = 0 and call inflate
 * Return Value: Z_STREAM_ERROR
 */  
TInt CTestZlib::TestInflate_fail1()
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

	// test_compress_positive(compr, comprLen, uncompr, uncomprLen);
	res = Test_deflate(compr, comprLen);
	if(res<0)
		{
		free(compr);
		free(uncompr);
		return KErrNoMemory;	
		}

	int err;
	z_stream d_stream; /* decompression stream */

	strcpy((char*)uncompr, "garbage");

	d_stream.zalloc = (alloc_func)0;
	d_stream.zfree = (free_func)0;
	d_stream.opaque = (voidpf)0;

	d_stream.next_in  = compr;
	d_stream.avail_in = 0;
	d_stream.next_out = NULL;

	err = inflateInit(&d_stream);

	while (d_stream.total_out < uncomprLen && d_stream.total_in < comprLen)
		{
		d_stream.avail_in = 0;
		d_stream.avail_out = 1; /* force small buffers */
		err = inflate(&d_stream, Z_NO_FLUSH);
		if (err == Z_STREAM_ERROR) break;
		}
	//inflate() should normally be called until it returns Z_STREAM_END 

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
 * Function Name : TestInflate_fail1
 * TestCase Description: 1. Compress the sample data using deflate
 *						2. Call inflateInit to initialize internal stream state for decompression
 *						3. Pass Z_NULL to inflate
 * Return Value: Z_STREAM_ERROR
 */  
TInt CTestZlib::TestInflate_fail2()
	{
	TInt res = KErrNone ;
	Byte *compr, *uncompr;
	uLong comprLen = 20*sizeof(int); 
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

	int err;
	z_stream d_stream; /* decompression stream */

	strcpy((char*)uncompr, "garbage");

	d_stream.zalloc = (alloc_func)0;
	d_stream.zfree = (free_func)0;
	d_stream.opaque = (voidpf)0;

	d_stream.next_in  = compr;
	d_stream.avail_in = 0;
	d_stream.next_out = uncompr;

	err = inflateInit(&d_stream);

	while (d_stream.total_out < uncomprLen && d_stream.total_in < comprLen) 
		{
		d_stream.avail_in = d_stream.avail_out = 1; /* force small buffers */
		err = inflate(NULL, Z_NO_FLUSH);
		if (err == Z_STREAM_ERROR) break;      
		}
	//inflate() should normally be called until it returns Z_STREAM_END 
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
 * Function Name : TestInflate_fail3
 * TestCase Description: 1. Compress the sample data using deflate
 *						2. Call inflateInit to initialize internal stream state for decompression
 *						3. Set avail_out = 0 and call inflate with necessary valid arguments
 * Return Value: Z_STREAM_ERROR
 */    
TInt CTestZlib::TestInflate_fail3()
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

	int err;
	z_stream d_stream; /* decompression stream */

	strcpy((char*)uncompr, "garbage");

	d_stream.zalloc = (alloc_func)0;
	d_stream.zfree = (free_func)0;
	d_stream.opaque = (voidpf)0;

	d_stream.next_in  = NULL;
	d_stream.avail_in = 0;
	d_stream.next_out = uncompr;

	err = inflateInit(&d_stream);

	while (d_stream.total_out < uncomprLen && d_stream.total_in < comprLen)
		{
		d_stream.avail_in = 1;
		d_stream.avail_out = 0; /* force small buffers */
		err = inflate(&d_stream, Z_NO_FLUSH);
		if (err == Z_STREAM_ERROR) break;
		}
	//inflate() should normally be called until it returns Z_STREAM_END 

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
 * Function Name : test_compress_positive
 * TestCase Description: This is a global function used by many test functions
 */ 
TInt test_compress_positive(Byte * compr,uLong comprLen,Byte * /*uncompr*/,uLong /*uncomprLen*/)
	{
	TInt res = KErrNone ;
	int err;
	uLong len = (uLong)strlen(hello)+1;

	err = compress(compr, &comprLen, (const Bytef*)hello, len);
	if(err < 0)
		{  	
		res = KErrNoMemory;
		}

	if(err == 0)
		{  	
		res = KErrNone ;
		} 
	else
		{    	
		res = KErrGeneral;
		}   
	return res;
	}

/**
 * Function Name : test_inflateend_positive
 * TestCase Description: This is a global function used by many test functions
 */ 
TInt test_inflateend_positive(Byte * compr,uLong comprLen,Byte * uncompr,uLong uncomprLen)
	{ 
	TInt res = KErrNone ;
	int err;
	z_stream d_stream; // decompression stream

	strcpy((char*)uncompr, "garbage");

	d_stream.zalloc = (alloc_func)0;
	d_stream.zfree = (free_func)0;
	d_stream.opaque = (voidpf)0;

	d_stream.next_in  = compr;
	d_stream.avail_in = 0;
	d_stream.next_out = uncompr;

	err = inflateInit(&d_stream);
	if(err <0)
		{
		return KErrNoMemory;
		}

	while (d_stream.total_out < uncomprLen && d_stream.total_in < comprLen)
		{
		d_stream.avail_in = d_stream.avail_out = 1; // force small buffers
		err = inflate(&d_stream, Z_NO_FLUSH);
		if (err == Z_STREAM_END) break;
		if(err<0) break;
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
	return res;
	}


/**
 * Function Name : TestInflateend
 * TestCase Description: 1.	Compress the sample data using deflate
 *						2.	inflate to decompress the data
 *						3.	inflateEnd
 * Return Value: Z_OK
 */
TInt CTestZlib::TestInflateend()
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
	res= test_compress_positive(compr, comprLen, uncompr, uncomprLen);
	if(res<0)
		{
		free(compr);
		free(uncompr);
		return KErrNoMemory;	
		}
	res = Test_deflate(compr, comprLen);
	if(res<0)
		{
		free(compr);
		free(uncompr);
		return KErrNoMemory;	
		}
	res = test_inflateend_positive(compr, comprLen, uncompr, uncomprLen);
	if(res <0)    
		{
		free(compr);
		free(uncompr);
		return res;
		}
	else if(res==0)
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
 * Function Name : TestInflateend_fail
 * TestCase Description: 1.	Compress the sample data using deflate
 *						2.	inflate to decompress the data
 *						3.	Pass Z_NULL as argument to inflateEnd 
 * Return Value: Z_STREAM_ERROR
 */ 
TInt CTestZlib::TestInflateend_fail()
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
	res=test_compress(compr, comprLen, uncompr, uncomprLen);
	if(res<0)
		{
		free(compr);
		free(uncompr);
		return KErrNoMemory;	
		}
	res = Test_deflate(compr, comprLen);
	if(res<0)
		{
		free(compr);
		free(uncompr);
		return KErrNoMemory;	
		}
	int ret=  inflateEnd(NULL);
	if(ret==0)
		{
		res = KErrGeneral;    	
		}
	else
		{    	
		res = KErrNone;
		}   
	free(compr);
	free(uncompr);
	return res;
	}

/**
 * Function Name : TestInflateReset
 * TestCase Description: 1. Compress the sample data using deflate
 *						2. Call inflateInit to initialize internal stream state for decompression
 *						3. Call inflateReset with valid arguments 
 * Return Value: Z_OK
 */  
TInt CTestZlib::TestInflateReset()
	{  
	TInt res=KErrNone;
	z_stream d_stream; /* decompression stream */
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
	if(err<0)
		{
		inflateEnd(&d_stream);
		free(compr);
		free(uncompr);
		return KErrNoMemory;	
		}
	if(err!=0)
		{
		res=KErrGeneral;
		}
	free(compr);
	free(uncompr);
	res=inflateEnd(&d_stream);
	return res;
	}


/**
 * Function Name : TestInflateResetfail1
 * TestCase Description: 1. Compress the sample data using deflate
 *						2. Call inflateInit to initialize internal stream state for decompression
 *						3. Pass Z_NULL as argument to inflateReset 
 * Return Value: Z_STREAM_ERROR
 */   
TInt CTestZlib::TestInflateResetfail1()
	{
	TInt res=KErrNone;
	z_stream d_stream; /* decompression stream */
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
	inflateInit_(&d_stream,(char*)version, sizeof(d_stream));
	res=inflateReset(NULL);
	if(res!=Z_STREAM_ERROR)
		{
		res=KErrGeneral;
		}
	res=inflateEnd(&d_stream);
	if(res!=0)
		{
		res=KErrGeneral;
		}
	free(compr);
	free(uncompr);
	return res;
	}

/**
 * Function Name : TestInflateInit2_
 * TestCase Description: Test inflateInit2_
 * Return Value: Z_OK
 */     
TInt CTestZlib::TestInflateInit2_()
	{ 
	TInt res = KErrNone ;
	z_stream d_stream; // decompression stream
	const char * version;
	d_stream.zalloc = (alloc_func)0;
	d_stream.zfree = (free_func)0;
	d_stream.opaque = (voidpf)0;

	version = zlibVersion();
	TInt ret =  inflateInit2_(&d_stream,15,version, sizeof(d_stream));
	if(ret==0)
		{ 
		res = KErrNone;
		}
	else
		{     	
		return ret; 
		} 
	inflateEnd(&d_stream);
	return res;       
	}

/**
 * Function Name : TestInflateInit_
 * TestCase Description: Test inflateInit_
 * Return Value: Z_OK
 */
TInt CTestZlib::TestInflateInit_()
	{ 
	TInt res = KErrNone ;
	z_stream d_stream; 
	const char * version;
	d_stream.zalloc = (alloc_func)0;
	d_stream.zfree = (free_func)0;
	d_stream.opaque = (voidpf)0;

	Byte *compr, *uncompr;
	uLong comprLen = 20*sizeof(int); 
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
	res= test_compress_positive(compr, comprLen, uncompr, uncomprLen);
	if(res<0)
		{
		free(compr);
		free(uncompr);
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
	int ret = inflateInit_(&d_stream,(char*)version, sizeof(d_stream));
	if(ret==0)
		{ 
		res = KErrNone;
		}
	else
		{    
		res = KErrGeneral;   
		}    
	inflateEnd(&d_stream);
	free(compr);
	free(uncompr);
	return res;   
	}

/**
 * Function Name : TestInflateInit2_negative
 * TestCase Description: 1. Compress the sample data using deflate
 *						2. inflateInit2_ with stream size less than required
 * Return Value: Z_MEM_ERROR
 */  
TInt CTestZlib::TestInflateInit2_negative()
	{ 
	TInt res = KErrNone ;
	z_stream d_stream; // decompression stream
	const char * version;
	d_stream.zalloc = (alloc_func)0;
	d_stream.zfree = (free_func)0;
	d_stream.opaque = (voidpf)0;

	Byte *compr, *uncompr;
	uLong comprLen = 20*sizeof(int); // don't overflow on MSDOS
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
	int ret =  inflateInit2_(&d_stream, 15, version, sizeof(d_stream)-5);
	if(ret==0)
		{
		res = KErrGeneral;    	
		}
	else
		{    	
		res = KErrNone;
		}      
	free(compr);
	free(uncompr);
	// inflateEnd(&d_stream);
	return res;       
	}

/**
 * Function Name : TestInflateInit_negative
 * TestCase Description: 1. Compress the sample data using deflate
 *						2. Call inflateInit_ with stream size less than required
 * Return Value: Z_MEM_ERROR
 */ 
TInt CTestZlib::TestInflateInit_negative()
	{ 
	TInt res = KErrNone ;
	z_stream d_stream; // decompression stream 
	const char * version;
	d_stream.zalloc = (alloc_func)0;
	d_stream.zfree = (free_func)0;
	d_stream.opaque = (voidpf)0;

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
	int ret = inflateInit_(&d_stream,(char*)version, sizeof(d_stream) - 5);
	if(ret==0)
		{
		res = KErrGeneral;    	
		}
	else
		{    	
		res = KErrNone;
		}   
	// inflateEnd(&d_stream); 
	free(compr);
	free(uncompr);
	return res;    
	}

/**
 * Function Name : TestInflateInit2_versioncheck
 * TestCase Description: 1. Compress the sample data using deflate
 *						2. inflateInit2_ with valid zlib version as argument
 * Return Value: Z_OK
 */   
TInt CTestZlib::TestInflateInit2_versioncheck()
	{ 
	TInt res = KErrNone ;
	z_stream d_stream; // decompression stream
	const char * version;
	d_stream.zalloc = (alloc_func)0;
	d_stream.zfree = (free_func)0;
	d_stream.opaque = (voidpf)0;

	Byte *compr, *uncompr;
	uLong comprLen = 20*sizeof(int);
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
	d_stream.next_in  = compr;
	d_stream.avail_in = 0;
	d_stream.next_out = uncompr;
	version=zlibVersion();
	INFO_PRINTF2(_L("Version : %d"),version);
	int ret =  inflateInit2_(&d_stream, 15,  "1.2.4", sizeof(d_stream)-5);
	if(ret==0)
		{
		res = KErrGeneral;    	
		}
	else
		{    	
		res = KErrNone;
		}  
	free(compr);
	free(uncompr); 
	return res;      
	}

/**
 * Function Name : TestInflateInit_versioncheck
 * TestCase Description: 1. Compress the sample data using deflate
 *						2. inflateInit_ with valid zlib version as argument
 * Return Value: Z_OK
 */ 
TInt CTestZlib::TestInflateInit_versioncheck()
	{	 
	TInt res = KErrNone ;

	z_stream d_stream; // decompression stream
	const char * version;
	d_stream.zalloc = (alloc_func)0;
	d_stream.zfree = (free_func)0;
	d_stream.opaque = (voidpf)0;

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
	d_stream.next_in  = compr;
	d_stream.avail_in = 0;
	d_stream.next_out = uncompr;
	version=zlibVersion();
	INFO_PRINTF2(_L("Version : %d"),version);
	int ret =  inflateInit_(&d_stream,(char*)  "1.2.4", sizeof(d_stream) - 5);
	if(ret==0)
		{
		res = KErrGeneral;    	
		}
	else
		{    	
		res = KErrNone;
		}   
	free(compr);
	free(uncompr);
	return res;   
	}

/**
 * Function Name : TestCompress
 * TestCase Description: Test compress()
 * Return Value: Z_OK
 */  
TInt CTestZlib::TestCompress()
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
	res=test_compress_positive(compr, comprLen, uncompr, uncomprLen);
	free(compr);
	free(uncompr);
	return res;
	}

/**
 * Function Name : test_compress_negative
 * TestCase Description: This is a global function used by many test functions
 */
TInt test_compress_negative(Byte * compr,uLong comprLen,Byte * /*uncompr*/,uLong /*uncomprLen*/)
	{
	TInt res = KErrNone ;
	int err;
	uLong len = (uLong)strlen(hello)+1;
	err = compress(compr, &comprLen, (const Bytef*)hello, len);
	if(err == 0)
		{  
		res = KErrGeneral;   
		} 
	else
		{    	
		res =    KErrNone ;
		}   
	return res;
	}

/**
 * Function Name : TestCompress_negative
 * TestCase Description: Call compress with compression length less than required
 * Return Value: Z_BUF_ERROR
 */
TInt CTestZlib::TestCompress_negative()
	{
	TInt res = KErrNone ;
	Byte *compr, *uncompr;
	uLong comprLen = 1*sizeof(int);
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
	res=test_compress_negative(compr, comprLen, uncompr, uncomprLen);
	free(compr);
	free(uncompr);
	return res;
	}

/**
 * Function Name : TestCompress2_positive
 * TestCase Description: Test compress2 with valid arguments
 * Return Value: Z_OK
 */
TInt CTestZlib::TestCompress2_positive()
	{
	TInt res = KErrNone ;
	int err;
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

	err=compress2(compr, &comprLen, (const Bytef*)hello, len,Z_DEFAULT_COMPRESSION);      

	if(err == 0)
		{  	
		res = KErrNone ;
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
 * Function Name : TestCompress2_negative
 * TestCase Description: Test compress2 with compression length less than required  
 * Return Value: Z_BUF_ERROR
 */
TInt CTestZlib::TestCompress2_negative()
	{
	int err;
	TInt res = KErrNone ;
	uLong len = (uLong)strlen(hello)+1;
	Byte *compr;
	uLong comprLen = 20*sizeof(int); 
	compr    = (Byte*)calloc((uInt)comprLen, 1);
	if (compr == Z_NULL)
		{
		return KErrNoMemory;
		}
	err=compress2(compr, &comprLen, (const Bytef*)hello, len,14);   
	if(err == 0)
		{  
		res = KErrGeneral;     
		} 
	else
		{    	
		res = KErrNone ;
		}   
	free(compr);
	return res;
	}


/**
 * Function Name : test_compressbound
 * TestCase Description: This is a global function used by many test functions
 */
TInt test_compressbound(Byte * compr,uLong comprLen)
	{
	TInt res = KErrNone ;
	int err;
	uLong sourceLen = (uLong)strlen(hello)+1;
	err=compress(compr, &comprLen, (const Bytef*)hello, sourceLen);
	if(err==0)
		{    	
		int x =  compressBound(sourceLen);
		if(x > sourceLen)
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
		}
	return res;
	}

/**
 * Function Name : TestCompressbound
 * TestCase Description: 1. Call compress with appropriate arguments
 *						2. Then verify the length of the compressed buffer using compressBound  
 * Return Value: Returns an upper bound on the compressed size after compression
 *				
 */
TInt CTestZlib::TestCompressbound()
	{     
	TInt res = KErrNone ;
	Byte *compr;
	uLong comprLen =20*sizeof(int);
	compr    = (Byte*)calloc((uInt)comprLen, 1);
	if (compr == Z_NULL)
		{
		return KErrNoMemory;
		}

	res = test_compressbound(compr, comprLen);
	free(compr);
	return res;
	}

/**
 * Function Name : test_deflatebound
 * TestCase Description: This is a global function used by many test functions
 */
TInt test_deflatebound(Bytef *dest,  uLongf *destLen, const Bytef *source, uLong sourceLen, int level)
	{	 
	TInt res = KErrNone ;
	int err;
	z_stream stream;
	stream.next_in = (Bytef*)source;
	stream.avail_in = (uInt)sourceLen;
	stream.next_out = dest;
	stream.avail_out = (uInt)*destLen;
	if ((uLong)stream.avail_out != *destLen) 
		{
		res = KErrGeneral; 
		}
	stream.zalloc = (alloc_func)0;
	stream.zfree = (free_func)0;
	stream.opaque = (voidpf)0;

	err = deflateInit(&stream, level);
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
	return res;
	}

/**
 * Function Name : TestDeflatebound
 * TestCase Description: 1. Compress the sample data using deflate
 *						2. Then verify the length of the compressed buffer using deflateBound  
 * Return Value: Returns an upper bound on the compressed size after compression
 */
TInt CTestZlib::TestDeflatebound()
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
	if(uncompr == Z_NULL) 
		{
		free(compr);
		return KErrNoMemory;
		}
	res=test_deflatebound(compr, &comprLen, (const Bytef*)hello, len,Z_DEFAULT_COMPRESSION);   
	free(compr);
	free(uncompr);
	return res;
	}

/**
 * Function Name : TestDeflateparams
 * TestCase Description: 1. deflateInit to initialize the internal stream state for compression
 *						2. deflateParams with valid compression level and strategy
 *						
 * Return Value: Z_OK
 */  
TInt CTestZlib::TestDeflateparams()
	{     
	TInt res = KErrNone ;
	z_stream c_stream; /* compression stream */
	int err;
	Byte * compr;
	uLong comprLen = 20*sizeof(int); 
	compr = (Byte*)calloc((uInt)comprLen, 1);
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
	if(err != 0)
		{    	
		res = KErrGeneral;
		}
	deflateEnd(&c_stream);
	free(compr);
	return res;
	}

/**
 * Function Name : TestDeflateparamsfail1
 * TestCase Description: 1. deflateInit to initialize the internal stream state for compression
 *						2. deflateParams with invalid compression level
 * Return Value: Z_STREAM_ERROR
 */  
TInt CTestZlib::TestDeflateparamsfail1()
	{     
	TInt res = KErrNone ;
	z_stream c_stream; /* compression stream */
	int err;
	Byte * compr;
	uLong comprLen = 20*sizeof(int); 
	compr = (Byte*)calloc((uInt)comprLen, 1);
	if (compr == Z_NULL)
		{
		return KErrNoMemory;
		}

	uLong len = (uLong)strlen(hello)+1;
	c_stream.zalloc = (alloc_func)0;
	c_stream.zfree = (free_func)0;
	c_stream.opaque = (voidpf)0;

	err = deflateInit(&c_stream, Z_DEFAULT_COMPRESSION);
	if(err<0)
		{
		free(compr);
		return err;	
		}

	c_stream.next_in = (Bytef*)hello;
	c_stream.next_out = compr;

	err= deflateParams(&c_stream, 12, Z_DEFAULT_STRATEGY);
	if(err != 0)
		{    	
		res =  KErrNone;
		}
	deflateEnd(&c_stream);
	free(compr);
	return res;
	}

/**
 * Function Name : TestDeflateparamsfail2
 * TestCase Description: 1. deflateInit to initialize the internal stream state for compression
 *						2. Pass NULL stream as argument to deflateParams
 * Return Value: Z_STREAM_ERROR
 */ 
TInt CTestZlib::TestDeflateparamsfail2()
	{     
	TInt res = KErrNone ;   
	int err= deflateParams(NULL, 12, Z_DEFAULT_STRATEGY);
	if(err != 0)
		{    	
		res =  	KErrNone;
		}

	return res;
	}

/**
 * Function Name : TestCrcinit
 * TestCase Description: Test crc32 with proper arguments
 * Return Value: KErrNone
 */   
TInt CTestZlib::TestCrcinit()
	{
	TInt res = KErrNone ;
	unsigned long j=0L;
	long crc = crc32(j,0, 0);

	if(crc==0)
		{  
		res = KErrNone ;
		} 
	else
		{
		res = KErrGeneral;      	
		}   

	return res; 
	}

/**
 * Function Name : TestCrc
 * TestCase Description: 1. Call crc32 with proper arguments
 *						2. Call crc32 with with one of the arguments as an updated crc generated from previous call to crc32
 * Return Value: KErrNone
 */    
TInt CTestZlib::TestCrc()
	{
	TInt res = KErrNone ;
	unsigned char  buffer[5]="1234";
	unsigned int i=4;
	unsigned long j=0L;

	long crc1 = crc32(j,0, 0);

	long crc = crc32(crc1, &buffer[0], i);

	INFO_PRINTF2(_L("buf %x"),crc);

	if(crc==(long)2615402659LL)
		{  
		res = KErrNone ;
		} 
	else
		{
		res = KErrGeneral;      	
		}   

	return res;           
	}

/**
 * Function Name : TestGet_crc_table
 * TestCase Description: Test get_crc_table
 * Return Value: KErrNone
 */    
TInt CTestZlib::TestGet_crc_table()
	{ 
	TInt res = KErrNone ;
	const unsigned long* pcrc_32_tab;

	pcrc_32_tab = get_crc_table();

	if(pcrc_32_tab)
		{  
		res = KErrNone ;
		} 
	else
		{
		res = KErrGeneral;      	
		}   

	return res;  
	}

/**
 * Function Name : TestDeflateInit_
 * TestCase Description: deflateInit_ with all valid arguments
 * Return Value: KErrNone
 */     
TInt CTestZlib::TestDeflateInit_()
	{
	TInt res = KErrNone ;
	z_stream stream;
	int err;

	uLong sourceLen = (uLong)strlen(hello)+1;
	Byte *compr;
	uLong comprLen = 10*sizeof(int); 
	compr    = (Byte*)calloc((uInt)comprLen, 1);
	if (compr == Z_NULL)
		{
		return KErrNoMemory;
		}

	stream.zalloc = (alloc_func)0;
	stream.zfree = (free_func)0;
	stream.opaque = (voidpf)0;

	err= deflateInit_(&stream, Z_DEFAULT_COMPRESSION,  zlibVersion(), sizeof(z_stream));

	if (err != Z_OK) 
		{	 
		free(compr);
		res = KErrGeneral; 
		return res;
		} 
	else  
		{    	
		res = KErrNone;
		}   
	deflateEnd(&stream);
	free(compr);
	return res;
	}

/**
 * Function Name : TestDeflateInit_level
 * TestCase Description: deflateInit_ with invalid compression level
 * Return Value: Z_STREAM_ERROR
 */     
TInt CTestZlib::TestDeflateInit_level()
	{
	TInt res = KErrNone ;
	z_stream stream;
	int err;
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

	err= deflateInit_(&stream, 11,  zlibVersion(), sizeof(z_stream));

	if (err != Z_OK) 
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
 * Function Name : TestDeflateInit2_
 * TestCase Description: deflateInit2_ with invalid compression level
 * Return Value: Z_STREAM_ERROR
 */
TInt CTestZlib::TestDeflateInit2_()
	{
	TInt res = KErrNone ;
	z_stream stream;
	int err;

	uLong sourceLen = (uLong)strlen(hello)+1;
	Byte *compr;
	uLong comprLen = 10*sizeof(int); 
	compr    = (Byte*)calloc((uInt)comprLen, 1);
	if (compr == Z_NULL)
		{
		return KErrNoMemory;
		}

	stream.zalloc = (alloc_func)0;
	stream.zfree = (free_func)0;
	stream.opaque = (voidpf)0;

	err= deflateInit2_(&stream, 11, 8, 15, 8, 0,  zlibVersion(), sizeof(z_stream));

	if (err != Z_OK) 
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
 * Function Name : TestDeflatefail
 * TestCase Description: Set stream next_in and next_out to NULL and call deflateEnd
 * Return Value: Z_STREAM_ERROR
 */   
TInt CTestZlib::TestDeflatefail()
	{	    
	TInt res = KErrNone ;
	z_stream c_stream; /* compression stream */
	int err;
	Byte * compr;
	uLong comprLen = 20*sizeof(int); 
	compr    = (Byte*)calloc((uInt)comprLen, 1);
	if (compr == Z_NULL)
		{
		return KErrNoMemory;
		}

	uLong len = (uLong)strlen(hello)+1;

	c_stream.zalloc = (alloc_func)0;
	c_stream.zfree = (free_func)0;
	c_stream.opaque = (voidpf)0;

	res = deflateInit(&c_stream, Z_DEFAULT_COMPRESSION);
	if(res <0)
		{
		free(compr);
		return res;	
		}

	c_stream.next_in  = NULL;//(Bytef*)hello;
	c_stream.next_out = NULL;//compr;

	while (c_stream.total_in != len && c_stream.total_out < comprLen) 
		{
		c_stream.avail_in = c_stream.avail_out = 1; /* force small buffers */
		err = deflate(&c_stream, Z_NO_FLUSH);
		if(err == 0)
			{    	
			res = KErrGeneral;
			}
		else
			{
			res=KErrNone;
			break;
			} 
		}
	/* Finish the stream, still forcing small buffers: */
	res = deflateEnd(&c_stream);
	free(compr);
	return res;
	}

/**
 * Function Name : TestDeflatefail
 * TestCase Description: Set stream avail_out to NULL and call deflateEnd
 * Return Value: Z_STREAM_ERROR
 */
TInt CTestZlib::TestDeflatefail2()
	{    
	TInt res = KErrNone ;
	z_stream c_stream; /* compression stream */
	int err;
	Byte * compr;
	uLong comprLen = 20*sizeof(int); 

	compr = (Byte*)calloc((uInt)comprLen, 1);
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
	c_stream.next_in = (Bytef*)hello;
	c_stream.next_out = compr;

	while (c_stream.total_in != len && c_stream.total_out < comprLen) 
		{
		c_stream.avail_in = 1;
		c_stream.avail_out = 0; /* fail */
		err = deflate(&c_stream, Z_NO_FLUSH);
		if(err == 0)
			{    	
			res =  	 KErrGeneral;
			}
		else
			{
			res=KErrNone;
			break;
			}
		}

	err = deflateEnd(&c_stream);
	free(compr);
	return res;
	}

/**
 * Function Name : TestZlibversion
 * TestCase Description: Test Zlibversion
 * Return Value: Z_OK
 */  
TInt CTestZlib::TestZlibversion()
	{  
	TInt res = KErrNone ;
	if(strcmp(zlibVersion(), "1.2.3") != 0) 
		{
		res=KErrGeneral;
		ERR_PRINTF1(_L("using incorrect zlib version "));
		} 
	return res;
	}

/**
 * Function Name : TestGzputc
 * TestCase Description: 1. Open a gzfile in read mode
 *						2. Write a character in the file using gzputc
 *						3. Close the file
 * Return Value: KErrNone
 */    
TInt CTestZlib::TestGzputc()
	{    
	TInt res = KErrNone ;
	TInt res1 = KErrNone ;
	gzFile file;
	const char * fname = TESTFILE ;
	file = gzopen(fname, "wb");
	if (file == NULL) 
		{
		res = KErrGeneral;
		return res;
		}
	else
		{
		res1=gzputc(file, 'r');
		if(res1<0)
			{
			res = KErrGeneral;
			} 
		else  
			{    	
			res = KErrNone;
			}     	
		}
	int err=  gzclose(file);
	if (err != Z_OK)  
		{
		res = KErrGeneral; 
		} 
	else  
		{    	
		res = KErrNone;
		}   
	return res;
	}

/**
 * Function Name : TestGzopen
 * TestCase Description: 1. Open a gzfile in read mode
 *						2. Close the file
 * Return Value: KErrNone
 */    
TInt CTestZlib::TestGzopen()
	{
	TInt res = KErrNone ;
	gzFile file;
	const char * fname = TESTFILE ;
	file = gzopen(fname, "rb");
	if (file == NULL) 
		{
		res = KErrGeneral;
		return res;
		}    
	else
		{    	
		res = KErrNone;
		}
	int err=gzclose(file);
	if (err != Z_OK)  
		{
		res = KErrGeneral; 
		} 
	return res;
	}

/**
 * Function Name : TestGzopenmode
 * TestCase Description: 1.	Open a gzfile mentioning invalid mode
 * Return Value: KErrNone
 */    
TInt CTestZlib::TestGzopenmode()
	{
	TInt res = KErrNone ;
	gzFile file;
	const char * fname = TESTFILE ;
	file = gzopen(fname, "xyz");
	if (file == NULL) 
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
 * Function Name : TestGzopenfail
 * TestCase Description: Open a gzfile mentioning invalid path
 * Return Value: KErrNone
 */ 
TInt CTestZlib::TestGzopenfail()
	{
	TInt res = KErrNone ;
	gzFile file;
	const char * fname = NOFILE ;
	file = gzopen(fname, "wb");
	if (file == NULL) 
		{
		res =KErrNone;
		}    
	else
		{    	
		res = KErrGeneral;
		}
	return res;
	}

/**
 * Function Name : TestGzputcfail
 * TestCase Description: 1. Open a gzfile mentioning invalid path
 *						2. Use gzputc to write a character
 * Return Value: KErrNone
 */    
TInt CTestZlib::TestGzputcfail()
	{
	TInt res = KErrNone ;
	TInt res1 = KErrNone ;
	gzFile file;
	const char * fname = NOFILE ;
	file = gzopen(fname, "wb");   
	if (file != NULL) 
		{
		res = KErrGeneral;
		return res;
		}
	else
		{
		res1=gzputc(file, 'r');
		if(res1<0)
			{
			res = KErrNone;
			} 
		else  
			{    	
			res = KErrGeneral;
			}     	
		}

	int err=  gzclose(file);
	if (err == Z_OK)  
		{
		res = KErrGeneral; 
		} 
	else  
		{    	
		res = KErrNone;
		}   
	return res;
	}

/**
 * Function Name : TestGzputcreturn
 * TestCase Description: 1. Open a gzfile in write mode
 *						2. Write a character in the file using gzputc
 *						3. Verify the character written by comparing it with what is written
 *						4. Close the file
 * Return Value: returns the value of the character that was written 
 */    
TInt CTestZlib::TestGzputcreturn()
	{
	TInt res = KErrNone ;
	TInt res1 = KErrNone ;
	gzFile file;
	const char * fname = TESTFILE ;
	file = gzopen(fname, "wb");

	if (file == NULL) 
		{
		res = KErrGeneral;
		return res;
		}
	else
		{
		res1=gzputc(file, 'r');
		if(res1!=(int)'r')
			{
			res = KErrGeneral;
			} 
		else  
			{    	
			res = KErrNone;
			}     	
		}

	int err=  gzclose(file);
	if (err != Z_OK)  
		{
		res = KErrGeneral; 
		} 
	return res;
	}

/**
 * Function Name : TestGzputs
 * TestCase Description: 1. Open a gzfile in write mode
 *						2. Write a string into file
 *						3. Close the file
 * Return Value: KErrNone
 */    
TInt CTestZlib::TestGzputs()
	{
	TInt res = KErrNone ;
	gzFile file;
	const char * fname = TESTFILE ;
	file = gzopen(fname, "wb");
	if (file == NULL) 
		{
		res = KErrGeneral;
		return res;
		}
	if (gzputs(file, "ello") != 4) 
		{
		ERR_PRINTF1(_L("gzputs err"));
		}
	int err=  gzclose(file);
	if (err != Z_OK)  
		{
		res = KErrGeneral; 
		}       
	return res;    
	}

/**
 * Function Name : TestGzputsfail
 * TestCase Description: 1. Open a gzfile in read mode
 *						2. Write a string into file
 *						3. Close the file
 * Return Value: KErrNone
 */      
TInt CTestZlib::TestGzputsfail()
	{	
	TInt res = KErrNone ;
	gzFile file;
	const char *fname = TESTFILE ;
	file = gzopen(fname, "rb");
	if (file == NULL) 
		{
		res = KErrGeneral;
		return res;
		}
	if(gzputs(file, "ello") == 4) 
		{
		res= KErrGeneral;
		}
	int err=  gzclose(file);
	if (err != Z_OK)  
		{
		res = KErrGeneral; 
		}       
	return res;    
	}

/**
 * Function Name : TestGzprintf
 * TestCase Description: 1. Open a gzfile in write mode
 *						2. Write a string into file mentioning the valid format specifier
 *						3. Close the file
 * Return Value: KErrNone
 */     
TInt CTestZlib::TestGzprintf()
	{
	TInt res = KErrNone ;
	gzFile file;
	const char * fname = TESTFILE ;
	file = gzopen(fname, "wb");
	if(file == NULL) 
		{
		res = KErrNoMemory;
		return res;
		}   
	if(gzprintf(file, ", %s!", "hello") != 8) 
		{
		ERR_PRINTF1(_L("gzprintf err"));
		return KErrNone;
		}

	int err = gzclose(file);
	if(err == 0)
		{  	
		res = KErrNone ;
		} 
	else
		{	    	
		res =res= KErrGeneral;
		} 
	return res;
	}

/**
 * Function Name : TestGzprintf_trying
 * TestCase Description: 1. Open a gzfile in read mode
 *						2. Write a string into file mentioning the valid format specifier
 *						3. Close the file
 * Return Value: KErrNone
 */    
TInt CTestZlib::TestGzprintf_trying()
	{
	TInt res = KErrNone ;
	gzFile file;
	const char * fname = TESTFILE ;
	file = gzopen(fname, "rb");
	if (file == NULL) 
		{
		res = KErrNoMemory;
		return res;
		}   
	if (gzprintf(file, ", %s!", "hello") != 8) 
		{
		gzclose(file);
		res=KErrGeneral;
		ERR_PRINTF1(_L("gzprintf err"));
		return KErrNone;
		}
	int err = gzclose(file);
	if(err == 0)
		{  	
		res = KErrNone ;
		} 
	else
		{    	
		res = KErrGeneral;
		}   
	return res;
	}

/**
 * Function Name : TestGzprintf_trying
 * TestCase Description: 1. Open a gzfile in write mode
 *						2. Write a string into file mentioning the valid arguments for gzwrite
 *						3. Close the file
 * Return Value: KErrNone
 */
TInt CTestZlib::TestGzwrite()
	{
	TInt res = KErrNone ;
	gzFile file;
	uInt size ,len;
	const char *s="ritesh";
	len=strlen(s);
	const char * fname = TESTFILE ;
	file = gzopen(fname, "wb");
	if (file == Z_NULL)  
		{
		res = KErrGeneral; 
		return res;
		}     
	size = gzwrite(file, (char*)s, (unsigned)strlen(s));
	if(len!=size)
		{
		res = KErrGeneral;
		}
	int err=  gzclose(file);
	if (err != Z_OK)  
		{
		res = KErrGeneral; 
		}     
	return res;
	}

/**
 * Function Name : TestGzwritefail
 * TestCase Description: 1. Open a gzfile in read mode
 *						2. Write a string into file mentioning the valid arguments for gzwrite
 *						3. Close the file
 * Return Value: Z_NULL
 */
TInt CTestZlib::TestGzwritefail()
	{   
	TInt res = KErrNone ;
	gzFile file;
	uInt size ,len;
	const char *s="ritesh";
	len=strlen(s);
	const char * fname = TESTFILE ;
	file = gzopen(fname, "rb"); //read mode
	if (file == Z_NULL)  
		{
		res = KErrNoMemory; 
		return res;
		}     
	size = gzwrite(file, (char*)s, (unsigned)strlen(s));
	if(len!=size)
		{
		res = KErrNone;
		}
	int err=  gzclose(file);
	if (err != Z_OK)  
		{
		res = KErrGeneral;     	    	
		}     
	return res;
	}

TInt testgztell( const char * fname, Byte * uncompr,uLong  uncomprLen)   
	{
	int err=0;
	int len = (int)strlen(hello)+1;
	gzFile file;
	z_off_t pos;

	file = gzopen(fname, "wb");
	if (file == NULL) 
		{
		if (file == NULL) 
			{
			err = KErrGeneral; 
			return err;        
			}

		}
	gzputc(file, 'h');
	if(gzputs(file, "ello") != 4) 
		{
		//fprintf(stderr, "gzputs err: %s\n", gzerror(file, &err));
		}
	if(gzprintf(file, ", %s!", "hello") != 8) 
		{
		//fprintf(stderr, "gzprintf err: %s\n", gzerror(file, &err));
		}
	gzseek(file, 1L, SEEK_CUR); /* add one zero byte */
	gzclose(file);

	file = gzopen(fname, "rb");
	if(file == NULL) 
		{
		err = KErrGeneral; 
		return err;        
		}
	strcpy((char*)uncompr, "garbage");
	if(gzread(file, uncompr, (unsigned)uncomprLen) != len) 
		{
		//fprintf(stderr, "gzread err: %s\n", gzerror(file, &err));
		//exit(1);
		}
	if(strcmp((char*)uncompr, hello)) 
		{
		//fprintf(stderr, "bad gzread: %s\n", (char*)uncompr);
		////exit(1);
		}
	else
		{
		//printf("gzread(): %s\n", (char*)uncompr);
		}

	pos = gzseek(file, -8L, SEEK_CUR);
	if (pos != 6 || gztell(file) != pos) 
		{
		err=-1;   
		}
	err=  gzclose(file);
	return err;
	}

TInt CTestZlib::TestGztell()
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
	test_compress(compr, comprLen, uncompr, uncomprLen);
	int err = testgztell(TESTFILE,uncompr, uncomprLen);
	if(err<0)
		{
		res= KErrGeneral;	
		}
	free(compr);
	free(uncompr);
	return res;
	}

TInt CTestZlib::TestGztell1()
	{ 
	TInt res = KErrNone ;
	int err;
	gzFile file;
	file = gzopen(TESTFILE, "wb");             
	if (file == NULL) 
		{
		res = KErrGeneral; 
		return res;        
		}           
	err=gztell(file) ;
	if(err<0)
		{
		res = KErrGeneral;
		return res;	
		}
	err=  gzclose(file);
	if(err != Z_OK)  
		{
		res = KErrGeneral; 
		}
	return res;
	}

TInt CTestZlib::TestGztellfail1()
	{ 
	TInt res = KErrNone ;
	int err;
	gzFile file;
	file = gzopen(NOFILE, "wb");             
	if (file == NULL) 
		{
		res = KErrNone; 
		return res;        
		}          
	err=gztell(file) ;
	if(err>0)
		{
		res= KErrGeneral;	
		}
	return res;
	}

int  test_deflatecopy (Bytef *dest,  uLongf *destLen, const Bytef *source, uLong sourceLen, int level)
	{
	TInt res = KErrNone ;
	z_stream stream;
	z_stream stream1;
	int err;

	stream.zalloc = (alloc_func)0;
	stream.zfree = (free_func)0;
	stream.opaque = (voidpf)0;

	stream.next_in = (Bytef*)source;
	stream.avail_in = (uInt)sourceLen;

	stream.next_out = dest;
	stream.avail_out = (uInt)*destLen;
	if ((uLong)stream.avail_out != *destLen)  
		{
		res = KErrGeneral; 
		} 

	err = deflateInit(&stream, level);
	if (err != Z_OK)  
		{
		res = KErrGeneral; 
		return res;
		} 
	err=deflateCopy(&stream1 , &stream); 
	if (err != Z_OK)  
		{
		res = KErrGeneral; 
		} 
	err=deflateEnd(&stream);
	if (err != Z_OK)  
		{
		res = KErrGeneral; 
		} 
	err=deflateEnd(&stream1);
	if (err != Z_OK)  
		{
		res = KErrGeneral; 
		}
	return res;
	}

TInt CTestZlib::TestDeflatecopy()
	{
	TInt res;
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
	res= test_deflatecopy(compr, &comprLen, (const Bytef*)hello, len,Z_DEFAULT_COMPRESSION);
	free(compr);
	free(uncompr);
	return res;
	}


TInt CTestZlib::TestDeflatecopyfail()
	{
	TInt res = KErrNone ;int err;
	z_stream stream1;
	uLong len = (uLong)strlen(hello)+1;

	Byte *compr;
	uLong comprLen = 20*sizeof(int); 
	compr    = (Byte*)calloc((uInt)comprLen, 1);

	if (compr == Z_NULL)
		{
		return KErrNoMemory;
		}
	err=deflateCopy(&stream1 , NULL); 
	if (err == Z_OK)  
		{
		res = KErrNone;     	    	
		} 

	free(compr);
	return res;
	}


TInt CTestZlib::TestGzclose()
	{     
	TInt res = KErrNone ;
	int err;
	int len = (int)strlen(hello)+1;
	gzFile file;

	const char * fname = TESTFILE;
	file = gzopen(fname, "wb");
	gzputc(file, 'h');

	err= gzclose(file);

	if(err != 0)
		{    	
		res =  	 KErrGeneral;
		}
	return res;   
	}


TInt CTestZlib::TestGzclose_fail()
	{
	TInt res = KErrNone ;
	int err;
	int len = (int)strlen(hello)+1;
	gzFile file;
	const char * fname = NOFILE;

	file = gzopen(fname, "wb");

	err=gzclose(file);
	if(err == 0)
		{    	
		res =  	 KErrGeneral;
		}
	return res;   
	}

TInt CTestZlib::TestGzeof()
	{
	TInt res = KErrNone ;
	gzFile file;
	const char * fname = TESTFILE ;
	file = gzopen(fname, "wb");
	if (file == NULL) 
		{
		res = KErrGeneral;
		return res;
		}   

	if (gzputs(file, "ello") != 4) 
		{
		ERR_PRINTF1(_L("gzputs err"));
		return KErrNone;
		}
	int x=gzeof(file);
	if(x!=0)
		{
		res= KErrGeneral;	
		}

	int err=  gzclose(file);
	if (err != Z_OK)  
		{
		res = KErrGeneral; 
		} 
	return res;
	}

TInt CTestZlib::TestGzeoffail1()
	{
	TInt res = KErrNone ;
	gzFile file;
	const char * fname = TESTFILE ;
	file = gzopen(fname, "rb");
	if (file == NULL) 
		{
		res = KErrGeneral;
		return res;
		}   
	int x=gzeof(file);
	if(x!=Z_STREAM_END)
		{
		res= KErrNone;	
		}

	int err=  gzclose(file);
	if (err != Z_OK)  
		{
		res = KErrGeneral; 
		} 
	return res;
	}	

TInt CTestZlib::TestGzeoffail2()
	{
	TInt res = KErrNone ;
	gzFile file;
	const char * fname = NOFILE ;
	file = gzopen(fname, "wb");   
	int x=gzeof(file);
	if(x!=0)
		{
		res= KErrNone;	
		}    
	int err=  gzclose(file);
	if (err == Z_OK)  
		{
		res = KErrGeneral;     	    	
		}       
	return res;
	}

TInt CTestZlib::TestGzgetc()
	{
	TInt res = KErrNone ;
	gzFile file;
	const char * fname = TESTFILE ;
	file = gzopen(fname, "rb");
	if (file == Z_NULL)  
		{
		res = KErrNoMemory; 
		return res;
		} 
	int l=  gzgetc(file);
	int err=  gzclose(file);
	if (err != Z_OK)  
		{
		res = KErrGeneral; 
		}     
	return res;
	}


TInt CTestZlib::TestGzflush()
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
	int l= gzflush(file,Z_FULL_FLUSH);
	if(l != Z_OK)  
		{
		res = KErrGeneral;  
		} 

	int err=gzclose(file);
	if (err != Z_OK)  
		{
		res = KErrGeneral;     	    	
		}     
	return res;
	}


TInt CTestZlib::TestGzflushsync()
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
	int l= gzflush(file,Z_SYNC_FLUSH);
	if (l != Z_OK)  
		{
		res = KErrGeneral;  
		} 
	int err=  gzclose(file);
	if(err != Z_OK)  
		{
		res = KErrGeneral;     	    	
		}     
	return res;
	}

TInt CTestZlib::TestGzflushfail()
	{
	TInt res = KErrNone ;
	gzFile file;   
	const char * fname = TESTFILE ;
	file = gzopen(fname, "rb");
	if (file == Z_NULL)  
		{
		res = KErrGeneral; 
		return res;
		}     
	int l= gzflush(file,Z_FULL_FLUSH);
	if (l == Z_OK)  
		{
		res = KErrGeneral;  
		} 
	int err=  gzclose(file);
	if (err != Z_OK) 
		{
		res = KErrGeneral;     	    	
		} 
	return res;
	}

TInt CTestZlib::TestGzerror()
	{
	TInt res = KErrNone ;
	int err;
	int len = (int)strlen(hello)+1;
	gzFile file;
	const char * fname = TESTFILE ;
	file = gzopen(fname, "wb");
	if (file == NULL) 
		{
		res=KErrGeneral;
		}
	gzputc(file, 'h');
	if (gzputs(file, "ello") != 5) 
		{
		gzprintf(file, "gzputs err: %s\n", gzerror(file, &err));
		res=KErrGeneral;
		}
	err=  gzclose(file);
	if (err != Z_OK)  
		{
		res = KErrGeneral;     	    	
		}
	else
		{
		res = KErrNone;     	    	
		} 
	return res;
	}


TInt CTestZlib::TestGzerrorfail1()
	{
	TInt res = KErrNone ;
	int err;
	int len = (int)strlen(hello)+1;
	gzFile file;
	const char * fname = NOFILE ;
	file = gzopen(fname, "wb");
	if (file != NULL) 
		{
		res=KErrGeneral;
		return res;
		}
	gzputc(file, 'h');
	if (gzputs(file, "ello") != 5) 
		{
		gzprintf(file, "gzputs err: %s\n", gzerror(file, &err));
		res=KErrNone;
		}
	err=  gzclose(file);
	if (err == Z_OK)  
		{
		res = KErrGeneral; 
		}       
	return res;
	}


TInt CTestZlib::TestGzgetcfail()
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
	int l=  gzgetc(file);
	int err=  gzclose(file);
	if (err != Z_OK)  
		{
		res = KErrGeneral;     	    	
		} 
	return res;
	}


TInt CTestZlib::TestDeflateSetDictionary()
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

	res = test_compress(compr, comprLen, uncompr, uncomprLen);
	if(res < 0)
		{
		free(compr);
		free(uncompr);
		return KErrNoMemory;
		}
	res=Test_dict_deflate(compr, comprLen);
	free(compr);
	free(uncompr);
	return res;
	}



TInt CTestZlib::TestDeflateSetDictionary_nodict()
	{
	TInt res = KErrNone ;
	Byte *compr, *uncompr;
	uLong comprLen = 20*sizeof(int); 
	uLong uncomprLen = comprLen;
	z_stream c_stream; /* compression stream */
	int err;

	c_stream.zalloc = (alloc_func)0;
	c_stream.zfree = (free_func)0;
	c_stream.opaque = (voidpf)0;

	err = deflateInit(&c_stream, Z_BEST_COMPRESSION);                    
	if(err < 0)                 
		{
		return Z_MEM_ERROR;
		}
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
	err = deflateSetDictionary(&c_stream,NULL, sizeof(dictionary));
	if(err != 0)
		{    	
		res = KErrNone ;    	
		}
	else
		{    	
		res = KErrGeneral;
		}
	err = deflateEnd(&c_stream);
	if (err)
		{
		res =	 KErrGeneral;
		}
	free(compr);
	free(uncompr);

	return res;
	}

TInt CTestZlib::TestDeflateSetDictionary_fail()
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

	res=test_compress(compr, comprLen, uncompr, uncomprLen);

	if(res < 0)
		{
		free(compr);
		free(uncompr);
		return KErrNoMemory;
		}
	res = Test_dict_deflate(compr, comprLen);
	if(res < 0)
		{
		free(compr);
		free(uncompr);
		return KErrNoMemory;
		}
	int  err = deflateSetDictionary(NULL,(const Bytef*)dictionary, sizeof(dictionary));
	if(err != 0)
		{    	
		res = KErrNone ;    	
		}
	else
		{    	
		res = KErrGeneral;
		}
	free(compr);
	free(uncompr);
	return res;
	}


TInt CTestZlib::TestDeflateend()
	{    
	TInt res = KErrNone ;
	z_stream c_stream; /* compression stream */
	int err;
	uLong len = (uLong)strlen(hello)+1;
	Byte *compr, *uncompr;
	uLong comprLen = 20*sizeof(int); 
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

	c_stream.zalloc = (alloc_func)0;
	c_stream.zfree = (free_func)0;
	c_stream.opaque = (voidpf)0;
	err=test_compress(compr, comprLen, uncompr, uncomprLen);
	if(err <0)
		{
		free(compr);
		free(uncompr);
		return err;	
		}
	err = deflateInit(&c_stream, Z_DEFAULT_COMPRESSION);
	if(err <0)
		{
		free(compr);
		free(uncompr);
		return err;	
		}

	c_stream.next_in  = (Bytef*)hello;
	c_stream.next_out = compr;

	while (c_stream.total_in != len && c_stream.total_out < comprLen) 
		{
		c_stream.avail_in = c_stream.avail_out = 1; /* force small buffers */
		err = deflate(&c_stream, Z_NO_FLUSH);
		}
	/* Finish the stream, still forcing small buffers: */
	for (;;) 
		{
		c_stream.avail_out = 1;
		err = deflate(&c_stream, Z_FINISH);
		if (err == Z_STREAM_END) break;
		}
	err = deflateEnd(&c_stream);
	if (err)
		{
		res =	 KErrGeneral;
		}
	free(compr);
	free(uncompr);
	return res;
	}


TInt CTestZlib::TestDeflateendfail1()
	{    
	TInt res = KErrNone ;
	z_stream c_stream; /* compression stream */
	int err;
	Byte * compr;
	uLong comprLen = 20*sizeof(int); 
	compr = (Byte*)calloc((uInt)comprLen, 1);
	if (compr == Z_NULL)
		{
		return KErrNoMemory;
		}

	uLong len = (uLong)strlen(hello)+1;
	c_stream.zalloc = (alloc_func)0;
	c_stream.zfree = (free_func)0;
	c_stream.opaque = (voidpf)0;

	err = deflateInit(&c_stream, Z_DEFAULT_COMPRESSION);
	if(err<0)
		{
		free(compr);
		return KErrNoMemory;
		}
	c_stream.next_in  = (Bytef*)hello;
	c_stream.next_out = compr;

	while (c_stream.total_in != len && c_stream.total_out < comprLen) 
		{
		c_stream.avail_in = c_stream.avail_out = 1; /* force small buffers */
		err = deflate(&c_stream, Z_NO_FLUSH);
		}

	err = deflateEnd(&c_stream);
	if (!err)
		{
		res =	 KErrGeneral;
		}
	free(compr);
	return res;
	}


TInt CTestZlib::TestDeflate()
	{    
	TInt res = KErrNone ;
	z_stream c_stream; /* compression stream */
	int err;
	Byte * compr;
	uLong comprLen = 20*sizeof(int); 
	compr    = (Byte*)calloc((uInt)comprLen, 1);   
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

	while (c_stream.total_in != len && c_stream.total_out < comprLen) 
		{
		c_stream.avail_in = c_stream.avail_out = 1; /* force small buffers */
		err = deflate(&c_stream, Z_NO_FLUSH);
		if(err != 0)
			{    	
			res = KErrGeneral;
			}
		}

	err = deflateEnd(&c_stream);
	free(compr);

	return res;
	}


TInt CTestZlib::TestGzseek()
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
	int err;
	err = test_compress(compr, comprLen, uncompr, uncomprLen);
	if (err < 0)
		{
		free(compr);
		free(uncompr);
		return KErrNoMemory;
		}   
	int len = (int)strlen(hello)+1;
	gzFile file;
	file = gzopen(TESTFILE, "wb");
	if (file == NULL) 
		{
		free(compr);
		free(uncompr);
		res = KErrNoMemory; 
		return res;        
		}
	gzputc(file, 'h');
	if (gzputs(file, "ello") != 4)
		{
		res = KErrGeneral; 
		return res;
		}
	if (gzprintf(file, ", %s!", "hello") != 8) 
		{
		res = KErrGeneral; 
		return res;
		}
	err= gzseek(file, 1L, SEEK_CUR); /* add one zero byte */
	if(err<0)
		{
		res	= KErrGeneral;	
		}
	err=  gzclose(file);
	if (err != Z_OK)  
		{
		res = KErrGeneral; 
		} 

	free(compr);
	free(uncompr);
	return res;
	}


TInt CTestZlib::TestGzseekfail1()
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
	int err;
	err = test_compress(compr, comprLen, uncompr, uncomprLen);
	if (err < 0)
		{
		free(compr);
		free(uncompr);
		return KErrNoMemory;
		}   

	int len = (int)strlen(hello)+1;
	gzFile file;
	file = gzopen(NOFILE, "wb");

	err = gzseek(file, 1L, SEEK_CUR); /* add one zero byte */
	if(err>=0)
		{
		res= KErrGeneral;	
		}
	err=  gzclose(file);
	if (err == Z_OK)  
		{
		res = KErrGeneral; 
		} 

	free(compr);
	free(uncompr);
	return res;
	}

TInt CTestZlib::TestGzseekfail2()
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
	if(uncompr == Z_NULL) 
		{
		free(compr);
		return KErrNoMemory;
		}

	int err;
	err = test_compress(compr, comprLen, uncompr, uncomprLen);
	if (err < 0)
		{
		free(compr);
		free(uncompr);
		return KErrNoMemory;
		}   
	int len = (int)strlen(hello)+1;
	gzFile file;
	file = gzopen(TESTFILE, "wb");

	err = gzseek(file, -1L, SEEK_CUR); /* add one zero byte */
	if(err>=0)
		{
		res= KErrGeneral;	
		}
	err=  gzclose(file);
	if (err != Z_OK)  
		{
		res = KErrGeneral; 
		} 

	free(compr);
	free(uncompr);
	return res;
	}

TInt CTestZlib::TestGzsetparams()
	{    
	TInt res = KErrNone ;
	int len = (int)strlen(hello)+1;
	gzFile file;
	const char * fname = TESTFILE;

	file = gzopen(fname, "wb");
	gzputc(file, 'h');

	int u = gzsetparams(file, Z_BEST_SPEED, Z_DEFAULT_STRATEGY);
	if(u!=0)
		{
		res= KErrGeneral;	
		}
	int err=  gzclose(file);
	if (err != Z_OK)  
		{
		res = KErrGeneral; 
		} 

	return res;
	}


TInt CTestZlib::TestGzsetparams_fail1()
	{   
	TInt res = KErrNone ; 
	int len = (int)strlen(hello)+1;
	gzFile file;
	const char * fname = TESTFILE;
	file = gzopen(fname, "wb");
	gzputc(file, 'h');

	int u = gzsetparams(file, Z_BEST_SPEED, -2);
	if(u==0)
		{
		res= KErrGeneral;	
		}

	int err=  gzclose(file);
	if (err != Z_OK)  
		{
		res = KErrGeneral; 
		} 

	return res;
	}


TInt CTestZlib::TestGzsetparams_fail2()
	{    
	TInt res = KErrNone ;

	int len = (int)strlen(hello)+1;
	gzFile file;

	const char * fname = TESTFILE;
	file = gzopen(fname, "wb");
	gzputc(file, 'h');

	int u = gzsetparams(file, -2, Z_DEFAULT_STRATEGY);
	if(u==0)
		{
		res= KErrGeneral;	
		}
	int err=  gzclose(file);
	if (err != Z_OK)  
		{
		res = KErrGeneral; 
		} 

	return res;
	}

TInt CTestZlib::TestGzsetparams_fail3()
	{   
	TInt res = KErrNone ; 
	int len = (int)strlen(hello)+1;
	gzFile file;
	const char * fname = TESTFILE;

	file = gzopen(fname, "rb");
	if (file == NULL) 
		{
		res = KErrGeneral; 
		return res;        
		}
	gzputc(file, 'h');

	int u = gzsetparams(file, Z_BEST_SPEED, Z_DEFAULT_STRATEGY);
	if(u==0)
		{
		res	= KErrGeneral;	
		}

	int err=  gzclose(file);
	if (err != Z_OK)  
		{
		res = KErrGeneral; 
		} 
	return res;
	}


TInt CTestZlib::TestGzrewind()
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
	gzputc(file, 'h');
	err = gzrewind(file);
	if(err == 0)
		{    	
		res = KErrNone ;    	
		}
	else
		{    	
		res = KErrGeneral;
		}
	err = gzclose(file);
	if (err != Z_OK)  
		{
		res = KErrGeneral; 
		} 
	return res;
	}



TInt CTestZlib::TestGzrewindfail()
	{    
	TInt res = KErrNone ;
	int err;
	int len = (int)strlen(hello)+1;
	gzFile file;

	const char * fname = TESTFILE;
	file = gzopen(fname, "wb");
	if (file == NULL) 
		{
		res = KErrGeneral; 
		}
	gzputc(file, 'h');
	err = gzrewind(file);
	if(err == 0)
		{    	
		res =  	 KErrGeneral;
		}
	else
		{    	
		res = KErrNone ;  
		}
	err=  gzclose(file);
	if (err != Z_OK)  
		{
		res = KErrGeneral; 
		} 

	return res;
	}

TInt CTestZlib::TestGzdopen()
	{
	TInt res = KErrNone ;
	int len = (int)strlen(hello)+1;
	gzFile file;
	const char * fname = TESTFILE ;
	FILE*  fp = fopen(fname, "rb");
	file = gzdopen(fileno(fp), "rb");
	if (file == NULL)
		{
		res = Z_MEM_ERROR;
		fclose(fp);
		return res;
		}
	int err=  gzclose(file);
	if (err != Z_OK)  
		{
		res = KErrGeneral; 
		}
	fclose(fp);
	return res;
	}


TInt CTestZlib::TestGzdopen_fail()
	{
	TInt res = KErrNone ;
	int len = (int)strlen(hello)+1;
	gzFile file;
	const char * fname = TESTFILE ;
	FILE*  fp = fopen(fname, "rb");
	file = gzdopen(fileno(fp), "xyz");
	if (file == NULL) 
		{
		res=KErrNone; 
		fclose(fp);
		return res;
		}
	int err=  gzclose(file);
	if (err == Z_OK)  
		{
		res = KErrGeneral; 
		}  
	fclose(fp);
	return res;
	}


TInt CTestZlib::TestGzdopen_fail2()
	{
	TInt res = KErrNone ;
	int len = (int)strlen(hello)+1;
	gzFile file;
	const char * fname = TESTFILE ;
	FILE*  fp = fopen(fname, "rb");
	file = gzdopen(-1, "xyz");
	if (file == NULL) 
		{ 
		fclose(fp);
		res=KErrNone;
		return res;
		}
	int err=  gzclose(file);
	if (err == Z_OK)  
		{
		res = KErrGeneral; 
		}
	fclose(fp);
	return res;
	}

/* ===========================================================================
 * Test deflate() with full flush
 */
TInt CTestZlib::Test_flush(   Byte * compr,uLong * comprLen)
	{
	TInt res = KErrNone ;
	z_stream c_stream; /* compression stream */
	int err;
	uInt len = (uInt)strlen(hello)+1;

	c_stream.zalloc = (alloc_func)0;
	c_stream.zfree = (free_func)0;
	c_stream.opaque = (voidpf)0;

	err = deflateInit(&c_stream, Z_DEFAULT_COMPRESSION);
	if(err<0)
		{
		return Z_MEM_ERROR;    	
		}

	c_stream.next_in  = (Bytef*)hello;
	c_stream.next_out = compr;
	c_stream.avail_in = 3;
	c_stream.avail_out = (uInt)*comprLen;
	err = deflate(&c_stream, Z_FULL_FLUSH);

	int h=inflateSyncPoint(&c_stream);
	compr[3]++; /* force an error in first compressed block */
	c_stream.avail_in = len - 3;

	err = deflate(&c_stream, Z_FINISH);
	if (err != Z_STREAM_END)
		{
		res = KErrGeneral;
		}
	err = deflateEnd(&c_stream);  
	if (err != 0) 
		{
		res = KErrGeneral;
		}
	*comprLen = c_stream.total_out;
	return res;
	}


TInt test_sync(Byte * compr,uLong comprLen, Byte * uncompr,uLong uncomprLen)
	{
	TInt res = KErrNone ;
	int err;
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

	err=  inflate(&d_stream, Z_NO_FLUSH);
	if(err!=0)
		{
		res=KErrGeneral;
		}
	d_stream.avail_in = (uInt)comprLen-2;   /* read all compressed data */
	err = inflateSync(&d_stream);           /* but skip the damaged part */
	if(err!=0)
		{
		res=KErrGeneral;
		}
	err = inflate(&d_stream, Z_FINISH);
	if(err==0)
		{
		res=KErrGeneral;
		}
	err = inflateEnd(&d_stream);
	if(err!=0)
		{
		res=KErrGeneral;
		}
	return res;
	}


TInt CTestZlib::TestInflateSync()
	{   
	TInt res = KErrNone ;
	uLong len = (uLong)strlen(hello)+1;

	Byte *compr, *uncompr;
	uLong comprLen = 20*sizeof(int); 
	uLong uncomprLen = comprLen;
	compr = (Byte*)calloc((uInt)comprLen, 1);
	if (compr == Z_NULL)
		{
		return KErrNoMemory;
		}
	uncompr  = (Byte*)calloc((uInt)uncomprLen, 1);
	if(uncompr == Z_NULL) 
		{
		free(compr);
		return KErrNoMemory;
		}
	res= test_compress_positive(compr, comprLen, uncompr, uncomprLen);
	if(res<0)
		{
		free(compr);
		free(uncompr);
		return KErrNoMemory;	
		}
	Test_flush(compr, &comprLen);
	res = test_sync(compr, comprLen, uncompr, uncomprLen);
	free(compr);
	free(uncompr);
	return res;
	}


TInt CTestZlib::TestinflateSyncfail()
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

	res= test_compress_positive(compr, comprLen, uncompr, uncomprLen);
	if(res<0)
		{
		free(compr);
		free(uncompr);
		return KErrNoMemory;	
		}
	int err = test_sync(compr, comprLen, uncompr, uncomprLen);
	if(err!=0)
		{
		res = KErrNone ;
		}    
	else
		{
		res = KErrGeneral;
		}    
	free(compr);
	free(uncompr);
	return res;
	}


int  test_syncpoint (Bytef *dest,  uLongf *destLen, const Bytef *source, uLong sourceLen, int level)
	{
	TInt res = KErrNone ;
	z_stream stream;
	int err;

	stream.next_in = (Bytef*)source;
	stream.avail_in = (uInt)sourceLen;
	stream.next_out = dest;
	stream.avail_out = (uInt)*destLen;

	stream.zalloc = (alloc_func)0;
	stream.zfree = (free_func)0;
	stream.opaque = (voidpf)0;

	err = deflateInit(&stream, level);
	if (err != Z_OK) return err;    

	err = deflate(&stream, Z_FINISH);
	if (err != Z_STREAM_END) 
		{
		deflateEnd(&stream);
		return err == Z_OK ? Z_BUF_ERROR : err;
		}
	*destLen = stream.total_out;    
	int h=inflateSyncPoint(&stream);
	if(h!=0)
		{
		res=KErrGeneral;
		}
	deflateEnd(&stream);
	return res;
	}


TInt CTestZlib::TestInflateSyncPoint()
	{
	TInt res = KErrNone ;
	uLong len = (uLong)strlen(hello)+1;
	Byte *compr, *uncompr;
	uLong comprLen = 20*sizeof(int); 
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
	res= test_syncpoint(compr, &comprLen, (const Bytef*)hello, len,Z_DEFAULT_COMPRESSION);
	free(compr);
	free(uncompr);
	return res;
	}

TInt CTestZlib::TestInflateSyncPoint_null()
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
	int h=inflateSyncPoint(NULL);
	if(h==0)
		{
		res=KErrGeneral;
		}
	free(compr);
	free(uncompr);
	return res;
	}


TInt CTestZlib::TestZerror()
	{
	TInt res = KErrNone ;
	const char * s = zError(1);
	if(strcmp(s,"stream end"))
		{
		res=KErrGeneral;
		}
	return res;
	}

TInt CTestZlib::TestZerror1()
	{
	TInt res = KErrNone ;
	const char * s = zError(-3);
	if(strcmp(s,"data error"))
		{
		res=KErrGeneral;
		}

	return res;
	} 

TInt CTestZlib::TestZerror2()
	{
	TInt res = KErrNone ;
	const char * s = zError(0);
	if(strcmp(s,""))
		{
		res=KErrGeneral;
		}

	return res;
	}

