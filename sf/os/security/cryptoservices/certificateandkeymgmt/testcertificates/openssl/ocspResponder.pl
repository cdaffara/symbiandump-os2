#
# Copyright (c) 2005-2009 Nokia Corporation and/or its subsidiary(-ies).
# All rights reserved.
# This component and the accompanying materials are made available
# under the terms of the License "Eclipse Public License v1.0"
# which accompanies this distribution, and is available
# at the URL "http://www.eclipse.org/legal/epl-v10.html".
#
# Initial Contributors:
# Nokia Corporation - initial contribution.
#
# Contributors:
#
# Description: 
#


if (fork() == 0)
	{
	if (fork() == 0)
		{
		system("openssl ocsp -index Root1\\index.txt -CA Root1\\certs\\ca.pem  -rsigner Root1\\certs\\05.pem -rkey Root1\\private\\OCSPSigner-R1.key.pem  -port 18001 -resp_key_id -ignore_err");
		}
	else
		{
		system("openssl ocsp -index Root2\\index.txt -CA Root2\\certs\\ca.pem  -rsigner Root2\\certs\\05.pem -rkey Root2\\private\\OCSPSigner-R2.key.pem  -port 18002 -resp_key_id -ignore_err");
		}
	}
else
	{
	system("openssl ocsp -index Root5\\index.txt -CA Root5\\certs\\ca.pem  -rsigner Root5\\certs\\07.pem -rkey Root5\\private\\OCSPSigner-R5.key.pem  -port 18003 -resp_key_id -ignore_err");
	}



