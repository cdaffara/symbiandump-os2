This directory contains several test programs

ezfile: which can be used to compress and then uncompress a single file.
ezlibtest: contains a number of tests designed to test many aspects of the ezlib interface.  This is 
not example code.
example: This test is taken from the zlib distribution and it tests the c interface.  The bits of
example that test the gzip io stuff have been removed.
gzip: A little program written to test the limited gzip functionality provided by ezlib.  It can be 
used to compress and decompress files using the gzip file format.
ezexample: This test is the copy of example test which has been made automated.
gziptest: contains unit tests designed to test many aspects of the gzip interface (like compression - decompression of gzip files)
inflateprimetest: This test is designed to test the inflatePrime function which is part of libz.dll.
decompresstest: This test is designed to check for invalid return codes, panics and memory leaks 
when decompressing invalid Zip and GZip files.
