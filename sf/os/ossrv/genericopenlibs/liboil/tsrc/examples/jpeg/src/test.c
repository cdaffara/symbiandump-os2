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

#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>
#include <stdlib.h>
#include <stdio.h>
#include "jpeg.h"

#define LOG_FILE "c:\\logs\\examples_jpeg_log1.txt"
#include "std_log_result.h"
#define LOG_FILENAME_LINE __FILE__, __LINE__

void create_xml(int result)
{
    if(result)
        assert_failed = 1;
    
    testResultXml("examples_jpeg");
    close_log_file();
}

/* getfile */

void *getfile (char *path, int *n_bytes);
static void dump_pgm (unsigned char *ptr, int rowstride, int width, int height);


int
main (int argc, char *argv[])
{
  unsigned char *data;
  int len;
  JpegDecoder *dec;
  char *fn = "c:\\data\\liboil\\test.jpg";
  unsigned char *ptr;
  int rowstride;
  int width;
  int height;

  /*if (argc < 2) {
        printf("jpeg_test <file.jpg>\n");
        exit(1);
  }*/
  if (argc > 1)
        fn = argv[1];
  
  std_log(LOG_FILENAME_LINE, "Test Started examples_jpeg");
  dec = jpeg_decoder_new ();    //to create decoder instance
  
  data = getfile (fn, &len);

  jpeg_decoder_addbits (dec, data, len);
  jpeg_decoder_decode (dec);

  jpeg_decoder_get_component_ptr (dec, 1, &ptr, &rowstride);
  jpeg_decoder_get_component_size (dec, 1, &width, &height);

  dump_pgm (ptr, rowstride, width, height);
  
  std_log(LOG_FILENAME_LINE, "Test Successful");
  create_xml(0);
  return 0;
}


/* getfile */

void *
getfile (char *path, int *n_bytes)
{
  int fd;
  struct stat st;
  void *ptr = NULL;
  int ret;

  fd = open (path, O_RDONLY);
  if (!fd)
    return NULL;

  ret = fstat (fd, &st);
  if (ret < 0) {
    close (fd);
    return NULL;
  }

  ptr = malloc (st.st_size);
  if (!ptr) {
    close (fd);
    return NULL;
  }

  ret = read (fd, ptr, st.st_size);
  if (ret != st.st_size) {
    free (ptr);
    close (fd);
    return NULL;
  }

  if (n_bytes)
    *n_bytes = st.st_size;

  close (fd);
  return ptr;
}

static void
dump_pgm (unsigned char *ptr, int rowstride, int width, int height)
{
  int x, y;

  printf ("P2\n");
  printf ("%d %d\n", width, height);
  printf ("255\n");

  for (y = 0; y < height; y++) {
    for (x = 0; x < width; x++) {
      printf ("%d ", ptr[x]);
      if ((x & 15) == 15) {
        printf ("\n");
      }
    }
    printf ("\n");
    ptr += rowstride;
  }
}
