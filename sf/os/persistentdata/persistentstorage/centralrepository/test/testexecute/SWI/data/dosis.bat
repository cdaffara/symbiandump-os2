@echo off
rem
rem Copyright (c) 2009-2010 Nokia Corporation and/or its subsidiary(-ies).
rem All rights reserved.
rem This component and the accompanying materials are made available
rem under the terms of "Eclipse Public License v1.0"
rem which accompanies this distribution, and is available
rem at the URL "http://www.eclipse.org/legal/epl-v10.html".
rem
rem Initial Contributors:
rem Nokia Corporation - initial contribution.
rem
rem Contributors:
rem
rem Description:
rem
@echo on

@if not exist "%1" md "%1"
@if not exist "%1" goto :InvalidFolder
@if not exist "%2" goto :MissingCert
@if not exist "%3" goto :MissingKey

rem The stub..
makesis -s centrepswiteststub.pkg

rem Make and sign the KS12 package. This modifies the KS1.2 keyspace, which
rem is a user mod on the ROM keyspace K1.
makesis KS12.pkg KS12.sis
signsis -S KS12.sis KS12s.sis %2 %3

rem Make and sign the KS14 package.
makesis KS14.pkg KS14.sis
signsis -S KS14.sis KS14s.sis %2 %3

rem Make and sign the KS2 package. This creates the KS2 keyspace, which
rem is new.
makesis KS2.pkg KS2.sis
signsis -S KS2.sis KS2s.sis %2 %3

rem Make and sign the KS22 package. This creates the KS22 keyspace, which
rem upgrades KS21 (user modified KS2).
makesis KS22.pkg KS22.sis
signsis -S KS22.sis KS22s.sis %2 %3

rem Make and sign the KS3 package. This package will fail to install
rem because the referenced keyspace file is not in the ROM stub.
makesis KS3.pkg KS3.sis
signsis -S KS3.sis KS3s.sis %2 %3

rem Make and sign the KS4 package. This keyspace contains various capabilities
rem which we'll test don't change on a further install.
makesis KS4.pkg KS4.sis
signsis -S KS4.sis KS4s.sis %2 %3

rem Installs on top of the above, you might think the capabilities in the
rem above would be lost, but no - they are persisted before the caps in this
rem package can splat them.
makesis KS41.pkg KS41.sis
signsis -S KS41.sis KS41s.sis %2 %3

rem Make and sign the KP4 package. This keyspace contains various capabilities
rem which we'll test don't change on a further install.
makesis KP4.pkg KP4.sis
signsis -S KP4.sis KP4s.sis %2 %3

rem Installs on top of the above, you might think the capabilities in the
rem above would be lost, but no - they are persisted before the caps in this
rem package can splat them.
makesis KP41.pkg KP41.sis
signsis -S KP41.sis KP41s.sis %2 %3

rem SP filenull for uninstalling KP4.
makesis KPS4.pkg KPS4.sis
signsis -S KPS4.sis KPS4s.sis %2 %3

rem Installs on top of the ROM (51551554)
makesis KS51.pkg KS51.sis
signsis -S KS51.sis KS51s.sis %2 %3

rem Make and sign the KP2 package. This creates the KP2 keyspace, which
rem is new.
makesis KP2.pkg KP2.sis
signsis -S KP2.sis KP2s.sis %2 %3

rem Make and sign the KP22 package. This creates the KP22 keyspace, which
rem upgrades KP21 (user modified KP2).
makesis KP22.pkg KP22.sis
signsis -S KP22.sis KP22s.sis %2 %3

rem Make and sign the KP23 package.
makesis KP23.pkg KP23.sis
signsis -S KP23.sis KP23s.sis %2 %3

rem Make and sign the KP12 package.
makesis KP12.pkg KP12.sis
signsis -S KP12.sis KP12s.sis %2 %3

rem Make and sign the KP14 package.
makesis KP14.pkg KP14.sis
signsis -S KP14.sis KP14s.sis %2 %3

rem Make and sign the KPS14 standard patch, this SP is installed with a
rem file-null so the 51551551 keyspace file can be removed.
makesis KPS14.pkg KPS14.sis
signsis -S KPS14.sis KPS14s.sis %2 %3

rem Make and sign the KP3 package. This package will fail to install
rem because the referenced keyspace file is not in the ROM stub.
makesis KP3.pkg KP3.sis
signsis -S KP3.sis KP3s.sis %2 %3

rem Make and sign the KP62 package.
makesis KP62.pkg KP62.sis
signsis -S KP62.sis KP62s.sis %2 %3

rem Make and sign the KP62 package.
makesis KS62.pkg KS62.sis
signsis -S KS62.sis KS62s.sis %2 %3

rem Make and sign the KPS64 package.
makesis KPS64.pkg KPS64.sis
signsis -S KPS64.sis KPS64s.sis %2 %3

rem Make and sign the KSX package. This is the embedded sis that goes in AppX.sis
makesis KSX.pkg KSX.sis
signsis -S KSX.sis KSXs.sis %2 %3

rem Make the AppX package for the application. This is the sis which contains the 
rem embedded sis that has the application repository as a Centrep patch. This sis
rem is self-signed and conditonally installs the exe file depending on the environment
makesis APPX.pkg APPX.sis
signsis -S APPX.sis APPXs.sis certstore\appx.cer certstore\appx.key

rem Make and sign the KSD package. This contains the corrupt KSD keyspace.
makesis KSD.pkg KSD.sis
signsis -S KSD.sis KSDs.sis %2 %3

rem Make the AppY,1,2 packages for the application. These sis files contain the 
rem embedded sis that has the application repository as a Centrep patch. This sis
rem is self-signed and conditonally installs the exe file depending on the environment
rem These SIS files are used to test a standard installation

rem The following packages are used for the APP-SP test

rem Make and sign the KSY package. This is the embedded PU sis that goes in AppY.sis
makesis KSY.pkg KSY.sis
signsis -S KSY.sis KSYs.sis %2 %3

rem Make and sign the APPY package. This package will install an APP exe
rem a keyspace in an embedded PU package and an SP package to be used for the uninstall
makesis APPY.pkg APPY.sis
signsis -S APPY.sis APPYs.sis certstore\appx.cer certstore\appx.key


rem Make and sign the KSY1 package. This is the embedded PU sis that goes in AppY1.sis
makesis KSY1.pkg KSY1.sis
signsis -S KSY1.sis KSY1s.sis %2 %3

rem Make and sign the APPY1 package. This package will install the APP exe
rem a new version of the keyspace in an embedded PU package and an SP package 
rem to be used for the uninstall
makesis APPY1.pkg APPY1.sis
signsis -S APPY1.sis APPY1s.sis certstore\appx.cer certstore\appx.key


rem Make and sign the KSY2 package. This is the embedded PU sis that goes in AppY2.sis
makesis KSY2.pkg KSY2.sis
signsis -S KSY2.sis KSY2s.sis %2 %3

rem Make and sign the APPY2 package. This package will install the APP exe
rem a new version of the keyspace in an embedded PU package and an SP package 
rem to be used for the uninstall
makesis APPY2.pkg APPY2.sis
signsis -S APPY2.sis APPY2s.sis certstore\appx.cer certstore\appx.key

echo f | XCOPY /fry *.sis %1\
del /f *.sis
@goto :EOF

:InvalidFolder
@echo ERROR: Can't create folder %1
@goto :EOF

:MissingCert
@echo ERROR: Missing certificate %2
@goto :EOF

:MissingKey
@echo ERROR: Missing certificate key %3
@goto :EOF