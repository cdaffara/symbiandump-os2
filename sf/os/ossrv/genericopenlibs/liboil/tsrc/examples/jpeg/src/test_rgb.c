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

#include <liboil/liboil.h>
#include "liboil/liboilcolorspace.h"
#include "jpeg.h"

#define LOG_FILE "c:\\logs\\examples_jpeg_rgb_log1.txt"
#include "std_log_result.h"
#define LOG_FILENAME_LINE __FILE__, __LINE__

void create_xml(int result)
{
    if(result)
        assert_failed = 1;
    
    testResultXml("examples_jpeg_rgb");
    close_log_file();
}

/* getfile */

void *getfile (char *path, int *n_bytes);
static void dump_pnm (uint32_t *ptr, int rowstride, int width, int height);

int
main (int argc, char *argv[])
{
  unsigned char *data;
  int len;
  char *fn = "c:\\data\\liboil\\test.jpg";
  uint32_t *image = NULL;
  int width;
  int height;
  int ret;
  std_log(LOG_FILENAME_LINE, "Test Started examples_jpeg_rgb");

  /*if (argc < 2) {
    printf("jpeg_rgb_test <file.jpg>\n");
    std_log(LOG_FILENAME_LINE,"jpeg_rgb_test <file.jpg>");
    exit(1);
  }*/
  if(argc > 1){
      fn = argv[1];
  }
  std_log(LOG_FILENAME_LINE,"fn = %s",fn);
  data = getfile (fn, &len);

  if (data == NULL) {
    printf("cannot read file %s\n", fn);
    exit(1);
  }

  ret = jpeg_decode_argb (data, len, &image, &width, &height);
  if (ret) {
    dump_pnm (image, width*4, width, height);
  }

  if (image) free (image);

  free (data);

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
  std_log(LOG_FILENAME_LINE, "getfile ENTER");
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
  std_log(LOG_FILENAME_LINE, "getfile EXIT");
  return ptr;
}

static void
dump_pnm (uint32_t *ptr, int rowstride, int width, int height)
{
  int x, y;

  printf ("P3\n");
  printf ("%d %d\n", width, height);
  printf ("255\n");
  std_log(LOG_FILENAME_LINE, "P3");
  std_log(LOG_FILENAME_LINE, "%d %d",width, height);
  std_log(LOG_FILENAME_LINE, "255");

  for (y = 0; y < height; y++) {
    for (x = 0; x < width; x++) {
      printf ("%d ", oil_argb_R(ptr[x]));
      printf ("%d ", oil_argb_G(ptr[x]));   //Extracts the green component from color.Evaluates to the green component
      printf ("%d ", oil_argb_B(ptr[x]));
      std_log(LOG_FILENAME_LINE, "%d ",oil_argb_R(ptr[x]));
      std_log(LOG_FILENAME_LINE, "%d ",oil_argb_G(ptr[x]));
      std_log(LOG_FILENAME_LINE, "%d ",oil_argb_B(ptr[x]));
      if ((x & 15) == 15) {
        printf ("\n");
      }
    }
    printf ("\n");
    ptr += rowstride/4;
  }
}
