@rem
@rem Copyright (c) 2009 Nokia Corporation and/or its subsidiary(-ies).
@rem All rights reserved.
@rem This component and the accompanying materials are made available
@rem under the terms of the License "Eclipse Public License v1.0"
@rem which accompanies this distribution, and is available
@rem at the URL "http://www.eclipse.org/legal/epl-v10.html".
@rem
@rem Initial Contributors:
@rem Nokia Corporation - initial contribution.
@rem
@rem Contributors:
@rem
@rem Description: 
@rem
copy \epoc32\wins\c\tJarDownloader\data\cacerts.dat \epoc32\wins\c\system\data\cacerts.dat
copy \epoc32\wins\c\tJarDownloader\data\certclients.dat \epoc32\wins\c\system\data\certclients.dat
copy \epoc32\winscw\c\tJarDownloader\data\cacerts.dat \epoc32\winscw\c\system\data\cacerts.dat
copy \epoc32\winscw\c\tJarDownloader\data\certclients.dat \epoc32\winscw\c\system\data\certclients.dat

copy \epoc32\wins\c\tJarDownloader\data\cacerts.dat \epoc32\wins\c\private\101f72a6\cacerts.dat
copy \epoc32\wins\c\tJarDownloader\data\certclients.dat \epoc32\wins\c\private\101f72a6\certclients.dat
copy \epoc32\winscw\c\tJarDownloader\data\cacerts.dat \epoc32\winscw\c\private\101f72a6\cacerts.dat
copy \epoc32\winscw\c\tJarDownloader\data\certclients.dat \epoc32\winscw\c\private\101f72a6\certclients.dat

REM Require for test [9.0 Install KFirenorights-no OTA response] in tJavaInstaller.txt
REM No post test replacement is required because all the source code of JarFileDecoder.dll is comipled in tJarFileDecoder.dll    
if  exist  \epoc32\RELEASE\WINSCW\UDEB\tJarFileDecoder.dll  copy /Y \epoc32\RELEASE\WINSCW\UDEB\tJarFileDecoder.dll \epoc32\RELEASE\WINSCW\UDEB\JarFileDecoder.dll
if  exist  \epoc32\RELEASE\WINSCW\UREL\tJarFileDecoder.dll  copy /Y \epoc32\RELEASE\WINSCW\UREL\tJarFileDecoder.dll \epoc32\RELEASE\WINSCW\UREL\JarFileDecoder.dll
if  exist  \epoc32\RELEASE\WINS\UDEB\tJarFileDecoder.dll  copy /Y \epoc32\RELEASE\WINS\UDEB\tJarFileDecoder.dll \epoc32\RELEASE\WINS\UDEB\JarFileDecoder.dll
if  exist  \epoc32\RELEASE\WINS\UREL\tJarFileDecoder.dll  copy /Y \epoc32\RELEASE\WINS\UREL\tJarFileDecoder.dll \epoc32\RELEASE\WINS\UREL\JarFileDecoder.dll
