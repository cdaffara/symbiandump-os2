/* Portions Copyright (c) 2007-2009 Nokia Corporation and/or its subsidiary(-ies).
 * All rights reserved.
 */

/* zran.c -- example of zlib/gzip stream indexing and random access
 * Copyright (C) 2005 Mark Adler
 * For conditions of distribution and use, see copyright notice in zlib.h
   Version 1.0  29 May 2005  Mark Adler */

/* Illustrate the use of Z_BLOCK, inflatePrime(), and inflateSetDictionary()
   for random access of a compressed file.  A file containing a zlib or gzip
   stream is provided on the command line.  The compressed stream is decoded in
   its entirety, and an index built with access points about every SPAN bytes
   in the uncompressed output.  The compressed file is left open, and can then
   be read randomly, having to decompress on the average SPAN/2 uncompressed
   bytes before getting to the desired block of data.

   An access point can be created at the start of any deflate block, by saving
   the starting file offset and bit of that block, and the 32K bytes of
   uncompressed data that precede that block.  Also the uncompressed offset of
   that block is saved to provide a referece for locating a desired starting
   point in the uncompressed stream.  build_index() works by decompressing the
   input zlib or gzip stream a block at a time, and at the end of each block
   deciding if enough uncompressed data has gone by to justify the creation of
   a new access point.  If so, that point is saved in a data structure that
   grows as needed to accommodate the points.

   To use the index, an offset in the uncompressed data is provided, for which
   the latest access point at or preceding that offset is located in the index.
   The input file is positioned to the specified location in the index, and if
   necessary the first few bits of the compressed data is read from the file.
   inflate is initialized with those bits and the 32K of uncompressed data, and
   the decompression then proceeds until the desired offset in the file is
   reached.  Then the decompression continues to read the desired uncompressed
   data from the file.

   Another approach would be to generate the index on demand.  In that case,
   requests for random access reads from the compressed data would try to use
   the index, but if a read far enough past the end of the index is required,
   then further index entries would be generated and added.

   There is some fair bit of overhead to starting inflation for the random
   access, mainly copying the 32K byte dictionary.  So if small pieces of the
   file are being accessed, it would make sense to implement a cache to hold
   some lookahead and avoid many calls to extract() for small lengths.

   Another way to build an index would be to use inflateCopy().  That would
   not be constrained to have access points at block boundaries, but requires
   more memory per access point, and also cannot be saved to file due to the
   use of pointers in the state.  The approach here allows for storage of the
   index in a file.
 */

#include <e32test.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <fcntl.h>
#include <zlib.h>

_LIT(KTestTitle, "inflatePrime() Test.");

RTest test(_L("inflateprimetest.exe"));
const int numTestFiles = 2;
const char *filePath = "z:\\test\\inflateprimetest\\\0";
const char *testFile[numTestFiles] = {"gzipped.gz\0", "zipped.zip\0"};

/* Test macro and function */
void Check(TInt aValue, TInt aExpected, TInt aLine)
	{
    if (aValue != aExpected)
    	{
        test.Printf(_L("*** Expected error: %d, got: %d\r\n"), aExpected, aValue);
        test.operator()(EFalse, aLine);
        }
    }
#define test2(a, b) Check(a, b, __LINE__)

#define SPAN 1048576L       /* desired distance between access points */
#define WINSIZE 32768U      /* sliding window size */
#define CHUNK 128         /* file input buffer size */

/* access point entry */
struct point {
    off_t out;          /* corresponding offset in uncompressed data */
    off_t in;           /* offset in input file of first full byte */
    int bits;           /* number of bits (1-7) from byte at in - 1, or 0 */
    unsigned char window[WINSIZE];  /* preceding 32K of uncompressed data */
};

/* access point list */
struct access {
    int have;           /* number of list entries filled in */
    int size;           /* number of list entries allocated */
    struct point *list; /* allocated list */
};

/* Deallocate an index built by build_index() */
void free_index(struct access *index)
{
    if (index != NULL) {
        free(index->list);
        free(index);
    }
}

/* Add an entry to the access point list.  If out of memory, deallocate the
   existing list and return NULL. */
struct access *addpoint(struct access *index, int bits,
    off_t in, off_t out, unsigned left, unsigned char *window)
{
    struct point *next;

    // if list is empty, create it (start with eight points)
    if (index == NULL) {
        index = (struct access *)malloc(sizeof(struct access));
        if (index == NULL) return NULL;
        index->list = (struct point *)malloc(sizeof(struct point) << 3);
        if (index->list == NULL) {
            free(index);
            return NULL;
        }
        index->size = 8;
        index->have = 0;
    }

    // if list is full, make it bigger
    else if (index->have == index->size) {
        index->size <<= 1;
        next = (struct point *)realloc(index->list, sizeof(struct point) * index->size);
        if (next == NULL) {
            free_index(index);
            return NULL;
        }
        index->list = next;
    }

    // fill in entry and increment how many we have
    next = index->list + index->have;
    next->bits = bits;
    next->in = in;
    next->out = out;
    if (left)
        memcpy(next->window, window + WINSIZE - left, left);
    if (left < WINSIZE)
        memcpy(next->window + left, window, WINSIZE - left);
    index->have++;

    /* return list, possibly reallocated */
    return index;
}

/* Make one entire pass through the compressed stream and build an index, with
   access points about every span bytes of uncompressed output -- span is
   chosen to balance the speed of random access against the memory requirements
   of the list, about 32K bytes per access point.  Note that data after the end
   of the first zlib or gzip stream in the file is ignored.  build_index()
   returns the number of access points on success (>= 1), Z_MEM_ERROR for out
   of memory, Z_DATA_ERROR for an error in the input file, or Z_ERRNO for a
   file read error.  On success, *built points to the resulting index. */
int build_index(FILE *in, off_t span, struct access **built)
{
    int ret;
    off_t totin, totout;        /* our own total counters to avoid 4GB limit */
    off_t last;                 /* totout value of last access point */
    struct access *index;       /* access points being generated */
    z_stream strm;
    unsigned char input[CHUNK];
    unsigned char window[WINSIZE];
	struct point *next = NULL;

    /* initialize inflate */
    strm.zalloc = Z_NULL;
    strm.zfree = Z_NULL;
    strm.opaque = Z_NULL;
    strm.avail_in = 0;
    strm.next_in = Z_NULL;
    ret = inflateInit2(&strm, 47);      /* automatic zlib or gzip decoding */
    if (ret != Z_OK)
        return ret;

    /* inflate the input, maintain a sliding window, and build an index -- this
       also validates the integrity of the compressed data using the check
       information at the end of the gzip or zlib stream */
    totin = totout = last = 0;
    index = NULL;               /* will be allocated by first addpoint() */
    strm.avail_out = 0;
    do {
        /* get some compressed data from input file */
        strm.avail_in = fread(input, 1, CHUNK, in);
        if (ferror(in)) {
            ret = Z_ERRNO;
            goto build_index_error;
        }
        if (strm.avail_in == 0) {
            ret = Z_DATA_ERROR;
            goto build_index_error;
        }
        strm.next_in = input;

        /* process all of that, or until end of stream */
        do {
            /* reset sliding window if necessary */
            if (strm.avail_out == 0) {
                strm.avail_out = WINSIZE;
                strm.next_out = window;
            }

            /* inflate until out of input, output, or at end of block --
               update the total input and output counters */
            totin += strm.avail_in;
            totout += strm.avail_out;
            ret = inflate(&strm, Z_BLOCK);      /* return at end of block */
            totin -= strm.avail_in;
            totout -= strm.avail_out;
            if (ret == Z_NEED_DICT)
                ret = Z_DATA_ERROR;
            if (ret == Z_MEM_ERROR || ret == Z_DATA_ERROR)
                goto build_index_error;
            if (ret == Z_STREAM_END)
                break;

            /* if at end of block, consider adding an index entry (note that if
               data_type indicates an end-of-block, then all of the
               uncompressed data from that block has been delivered, and none
               of the compressed data after that block has been consumed,
               except for up to seven bits) -- the totout == 0 provides an
               entry point after the zlib or gzip header, and assures that the
               index always has at least one access point; we avoid creating an
               access point after the last block by checking bit 6 of data_type
             */
            if ((strm.data_type & 128) && !(strm.data_type & 64) &&
                (totout == 0 || totout - last > span)) {
                index = addpoint(index, strm.data_type & 7, totin,
                                 totout, strm.avail_out, window);
                if (index == NULL) {
                    ret = Z_MEM_ERROR;
                    goto build_index_error;
                }
                last = totout;
            }
        } while (strm.avail_in != 0);
    } while (ret != Z_STREAM_END);

    /* clean up and return index (release unused entries in list) */
    (void)inflateEnd(&strm);
    
    next = (struct point *)realloc(index->list, sizeof(struct point) * index->have);
    if (next == NULL) {
        free_index(index);
        return Z_MEM_ERROR;
    }
    index->list = next;
    index->size = index->have;
    *built = index;
    return index->size;

    /* return error */
  build_index_error:
    (void)inflateEnd(&strm);
    if (index != NULL)
        free_index(index);
    return ret;
}

/* Use the index to read len bytes from offset into buf, return bytes read or
   negative for error (Z_DATA_ERROR or Z_MEM_ERROR).  If data is requested past
   the end of the uncompressed data, then extract() will return a value less
   than len, indicating how much as actually read into buf.  This function
   should not return a data error unless the file was modified since the index
   was generated.  extract() may also return Z_ERRNO if there is an error on
   reading or seeking the input file. */
int extract(FILE *in, struct access *index, off_t offset,
                  unsigned char *buf, int len)
{
    int ret, skip, value;
    z_stream strm;
    struct point *here;
    unsigned char input[CHUNK];
    //unsigned char discard[WINSIZE]; /* No longer required. See comments below. */

    /* proceed only if something reasonable to do */
    if (len < 0)
        return 0;

    /* find where in stream to start */
    here = index->list;
    ret = index->have;
    while (--ret && here[1].out <= offset)
        here++;

    /* initialize file and inflate state to start there */
    strm.zalloc = Z_NULL;
    strm.zfree = Z_NULL;
    strm.opaque = Z_NULL;
    strm.avail_in = 0;
    strm.next_in = Z_NULL;
    ret = inflateInit2(&strm, -15);         /* raw inflate */
    if (ret != Z_OK)
        return ret;
    ret = fseek(in, here->in - (here->bits ? 1 : 0), SEEK_SET);
    if (ret == -1)
        goto extract_ret;
    
    ret = getc(in);
    if (ret == -1) {
        ret = ferror(in) ? Z_ERRNO : Z_DATA_ERROR;
        goto extract_ret;
    }
    
    // If bits is > 0 set the value as done in the original zran.c
    // else set the value to the next byte to prove that inflatePrime
    // is not adding anything to the start of the stream when bits is
    // set to 0. It is then necessary to unget the byte.
	if(here->bits) {	
	    value = ret >> (8 - here->bits);
	}
	else {
		value = ret;
		ungetc(ret, in);	
	}	
	
	ret = inflatePrime(&strm, here->bits, value);
	if(ret != Z_OK) {
		goto extract_ret;
	}
	test.Printf(_L("zran: bits = %d\n"), here->bits);
    test.Printf(_L("zran: value = %d\n"), value); 
    
    (void)inflateSetDictionary(&strm, here->window, WINSIZE);

	/* No longer required. See comment below.
	 *
     * skip uncompressed bytes until offset reached, then satisfy request
    offset -= here->out;
     */
    strm.avail_in = 0;
    skip = 1;                               /* while skipping to offset */
    do {
        /* define where to put uncompressed data, and how much */
        if (skip) {          /* at offset now */
            strm.avail_out = len;
            strm.next_out = buf;
            skip = 0;                       /* only do this once */
        }
        
        /* This code is not required in this test as it is used
         * to discard decompressed data between the current
         * access point and the offset(place in the file from
         * which we wish to decompress data).
         * 
        if (offset > WINSIZE) {             // skip WINSIZE bytes
            strm.avail_out = WINSIZE;
            strm.next_out = discard;
            offset -= WINSIZE;
        }
        else if (offset != 0) {             // last skip
            strm.avail_out = (unsigned)offset;
            strm.next_out = discard;
            offset = 0;
        }
		*/
		
        /* uncompress until avail_out filled, or end of stream */
        do {
            if (strm.avail_in == 0) {
                strm.avail_in = fread(input, 1, CHUNK, in);
                if (ferror(in)) {
                    ret = Z_ERRNO;
                    goto extract_ret;
                }
                if (strm.avail_in == 0) {
                    ret = Z_DATA_ERROR;
                    goto extract_ret;
                }
                strm.next_in = input;
            }
            ret = inflate(&strm, Z_NO_FLUSH);       /* normal inflate */
            if (ret == Z_NEED_DICT)
                ret = Z_DATA_ERROR;
            if (ret == Z_MEM_ERROR || ret == Z_DATA_ERROR)
                goto extract_ret;
            if (ret == Z_STREAM_END)
                break;
        } while (strm.avail_out != 0);

        /* if reach end of stream, then don't keep trying to get more */
        if (ret == Z_STREAM_END)
            break;

        /* do until offset reached and requested data read, or stream ends */
    } while (skip);

    /* compute number of uncompressed bytes read after offset */
    ret = skip ? 0 : len - strm.avail_out;

    /* clean up and return bytes read or error */
  extract_ret:
    (void)inflateEnd(&strm);
    return ret;
}

/* Demonstrate the use of build_index() and extract() by processing the file
   provided and then extracting CHUNK bytes at each access point. */
int TestInflatePrime(char *file)
	{
    int len;
    FILE *in;
    struct access *index;
    unsigned char buf[CHUNK];

    in = fopen(file, "rb");
    if (in == NULL) 
    	{
        return KErrPathNotFound;
    	}

    // build index
    len = build_index(in, SPAN, &index);
    if (len < 0) 
    	{
        fclose(in);
        test.Printf(_L("error: %d\n"), len);
        return KErrGeneral;
    	}
    test.Printf(_L("zran: built index with %d access points\n"), len);

	// Extract some data at the start of each access point. This is done
	// so that we can try extracting some data that does not necessarily 
	// start at a byte boundary ie it might start mid byte.
    for(int i = 0; i < index->have; i++)
	    {
	    len = extract(in, index, index->list[i].out, buf, CHUNK);
	    if (len < 0)
	    	{
	    	test.Printf(_L("zran: extraction failed: "));

	    	if(len == Z_MEM_ERROR)
                {
                test.Printf(_L("out of memory error\n"));
                }
            else
                {
                test.Printf(_L("input corrupted error\n"));
                }
            }
	    else 
	    	{
	        test.Printf(_L("zran: extracted %d bytes at %Lu\n"), len, index->list[i].out);
	    	}	
	    }    

    // clean up and exit
    free_index(index);
    fclose(in);
    
    return KErrNone;
	}

/**
@SYMTestCaseID       	SYSLIB-EZLIB2-UT-4273
@SYMTestCaseDesc     	To check that data can be decompressed at various points in a 
                        compressed file (i.e. decompression may start part of the way 
                        through a byte) via the use of inflatePrime().
@SYMTestPriority     	Low
@SYMTestActions      	1.	Open a compressed file for reading.
                        2.	Create an inflate stream and initialise it using inflateInit2(), 
                            setting windowBits to 47 (automatic gzip/zip header detection).
                        3.	Inflate the data in the file using inflate(). During inflation 
                            create access points using structure Point which maps points 
                            in the uncompressed data with points in the compressed data. 
                            The first access point should be at the start of the data 
                            i.e. after the header.
                            
                            Structure  Point consist of : 
                            •	UPoint(in bytes) – this is the point in the uncompressed data 
                            •	CPoint(in bytes) – this is the point in the compressed data
                            •	bits(in bits) – this is the point in the compressed data
                        4.	Cleanup the inflate stream using inflateEnd().
                        5.	For each access point do the following:
                            a.	Initialise the inflate stream using inflateInit2(), 
                                setting windowBits to -15.
                            b.	Move the file pointer to CPoint - 1 in the input file.
                            c.	Calculate the value which will be passed to inflatePrime(). 
                                The algorithm used to calculate value can be seen in the 
                                attached diagram (in the test spec).
                            d.	Call inflatePrime() with the bits and value.
                            e.	Inflate a small section of in the input file using inflate().
                            f.	Cleanup the inflate stream using inflateEnd().
                        6.	Close the compressed file and cleanup any allocated memory.
                        
                        Note: This test should be completed using a zlib file and a gzip 
                              file. These files should be 500 – 1000KB in size.
@SYMTestExpectedResults inflatePrime() should return Z_OK and the data should be 
                        decompressed with no errors.
@SYMDEF                 REQ7362
*/
void RunTestL()
	{
	test.Next(_L(" @SYMTestCaseID:SYSLIB-EZLIB2-UT-4273 "));
	int err;	
	char file[KMaxFileName];
	
	for(int i = 0; i < numTestFiles; i++)
		{
		TBuf<40> testName(_L("inflatePrime test using file "));
		testName.AppendNum(i);
		test.Next(testName);
		
		strcpy(file, filePath);
		strcat(file, testFile[i]);
			
		err = TestInflatePrime(file);
			
		if(err == KErrPathNotFound)
			{
			test.Printf(_L("zran: could not open file number %d for reading\n"), i);
			User::Leave(err);
			}
		else if(err != KErrNone)
			{
			User::Leave(err);
			}
			
		test.Printf(_L("\n"));		
		}
	}

TInt E32Main()
	{
	__UHEAP_MARK;

	test.Printf(_L("\n"));
	test.Title();
	test.Start(KTestTitle);

	CTrapCleanup* cleanup = CTrapCleanup::New();

	TRAPD(err, RunTestL());
	test2(err, KErrNone);
	
	test.End();
	test.Close();
	delete cleanup;

	__UHEAP_MARKEND;
	return KErrNone;
	}
