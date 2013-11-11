/* Portion Copyright © 2008-09 Nokia Corporation and/or its subsidiary(-ies). All rights reserved.*/

#undef G_DISABLE_ASSERT
#undef G_LOG_DOMAIN

#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <glib.h>
#include <string.h>
#include <stdio.h>
#include <stdlib.h>

#ifdef SYMBIAN
#include "mrt2_glib2_test.h"
#endif /*SYMBIAN*/


#define BUFFER_SIZE 1024

gint main (gint argc, gchar * argv[])
{
    GIOChannel *gio_r, *gio_w ;
    GError *gerr = NULL;
    GString *buffer;
    char *filename;
    char *srcdir = getenv ("srcdir");
    gint rlength = 0;
    glong wlength = 0;
    gsize length_out;
    const gchar encoding[] = "ISO-8859-5";
    GIOStatus status;
    GIOFlags flags;

  #ifdef SYMBIAN
  g_log_set_handler (NULL,  G_LOG_FLAG_FATAL| G_LOG_FLAG_RECURSION | G_LOG_LEVEL_CRITICAL | G_LOG_LEVEL_WARNING | G_LOG_LEVEL_MESSAGE | G_LOG_LEVEL_INFO | G_LOG_LEVEL_DEBUG, &mrtLogHandler, NULL);
  g_set_print_handler(mrtPrintHandler);
  #endif /*SYMBIAN*/
	  

    if (!srcdir)
      srcdir = "c:";
    filename = g_strconcat (srcdir, G_DIR_SEPARATOR_S, "iochannel-test-infile", NULL);
  
    setbuf (stdout, NULL); /* For debugging */

    gio_r = g_io_channel_new_file (filename, "r", &gerr);
    if (gerr)
      {
        g_warning ("Unable to open file %s: %s", filename, gerr->message);
        g_error_free (gerr);
        
        g_assert(FALSE && "iochannel-test failed");
        
        #if SYMBIAN
  		testResultXml("iochannel-test");
  		#endif /* EMULATOR */
        
        return 1;
      }
    gio_w = g_io_channel_new_file ("c:\\iochannel-test-outfile", "w", &gerr);
    if (gerr)
      {
        g_warning ("Unable to open file %s: %s", "iochannel-test-outfile", gerr->message);
        g_error_free (gerr);
        
        g_assert(FALSE && "iochannel-test failed");
        
        #if SYMBIAN
  		testResultXml("iochannel-test");
  		#endif /* EMULATOR */
        
        return 1;
      }

    g_io_channel_set_encoding (gio_r, encoding, &gerr);
    if (gerr)
      {
        g_warning (gerr->message);
        g_error_free (gerr);
        
        g_assert(FALSE && "iochannel-test failed");
        
        #if SYMBIAN
  		testResultXml("iochannel-test");
  		#endif /* EMULATOR */
        
        return 1;
      }
    
    g_io_channel_set_buffer_size (gio_r, BUFFER_SIZE);

    status = g_io_channel_set_flags (gio_r, G_IO_FLAG_NONBLOCK, &gerr);
    if (status == G_IO_STATUS_ERROR)
      {
        g_warning (gerr->message);
        g_assert(FALSE && "iochannel-test failed");
        g_error_free (gerr);
        gerr = NULL;
      }
    flags = g_io_channel_get_flags (gio_r);
    buffer = g_string_sized_new (BUFFER_SIZE);

    while (TRUE)
    {
        do
          status = g_io_channel_read_line_string (gio_r, buffer, NULL, &gerr);
        while (status == G_IO_STATUS_AGAIN);
        if (status != G_IO_STATUS_NORMAL)
          break;

        rlength += buffer->len;

        do
          status = g_io_channel_write_chars (gio_w, buffer->str, buffer->len,
            &length_out, &gerr);
        while (status == G_IO_STATUS_AGAIN);
        if (status != G_IO_STATUS_NORMAL)
          break;

        wlength += length_out;

        if (length_out < buffer->len)
        {
        	g_warning ("Only wrote part of the line.");
        	g_assert(FALSE && "iochannel-test failed");
        }
          

#ifdef VERBOSE
        g_print ("%s", buffer->str);
#endif
        g_string_truncate (buffer, 0);
    }

    switch (status)
      {
        case G_IO_STATUS_EOF:
          break;
        case G_IO_STATUS_ERROR:
          g_warning (gerr->message);
          g_error_free (gerr);
          gerr = NULL;
          break;
        default:
          g_warning ("Abnormal exit from write loop.");
          g_assert(FALSE && "iochannel-test failed");
          break;
      }

    do
      status = g_io_channel_flush (gio_w, &gerr);
    while (status == G_IO_STATUS_AGAIN);

    if (status == G_IO_STATUS_ERROR)
      {
        g_warning (gerr->message);
        g_assert(FALSE && "iochannel-test failed");
        g_error_free (gerr);
        gerr = NULL;
      }

#ifdef VERBOSE
    g_print ("read %d bytes, wrote %ld bytes\n", rlength, wlength);
#endif

    g_io_channel_unref(gio_r);
    g_io_channel_unref(gio_w);
    
   	#if SYMBIAN
  	testResultXml("iochannel-test");
  	#endif /* EMULATOR */
  	    
    return 0;
}
