rem
rem Copyright (c) 2009 Nokia Corporation and/or its subsidiary(-ies). All rights reserved.

rem Redistribution and use in source and binary forms, with or without 
rem modification, are permitted provided that the following conditions are met:

rem Redistributions of source code must retain the above copyright notice, this 
rem list of conditions and the following disclaimer.
rem Redistributions in binary form must reproduce the above copyright notice, 
rem this list of conditions and the following disclaimer in the documentation 
rem and/or other materials provided with the distribution.
rem Neither the name of Nokia Corporation nor the names of its contributors 
rem may be used to endorse or promote products derived from this software 
rem without specific prior written permission.

rem THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS" 
rem AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE 
rem IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE 
rem DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE LIABLE 
rem FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL 
rem DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR 
rem SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER 
rem CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, 
rem OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE 
rem OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
rem
rem Description:      
rem

ssltest.exe ssltest(3_19_2_6669) -test_cipherlist
ssltest.exe ssltest(3_19_2_6670) -key C:/keyU.ss -cert C:/certU.ss -c_key C:/keyU.ss -c_cert C:/certU.ss -ssl2
ssltest.exe ssltest(3_19_2_6671) -key C:/keyU.ss -cert C:/certU.ss -c_key C:/keyU.ss -c_cert C:/certU.ss -ssl2 -server_auth -CAfile C:/certCA.ss
ssltest.exe ssltest(3_19_2_6672) -key C:/keyU.ss -cert C:/certU.ss -c_key C:/keyU.ss -c_cert C:/certU.ss -ssl2 -client_auth -CAfile C:/certCA.ss
ssltest.exe ssltest(3_19_2_6673) -key C:/keyU.ss -cert C:/certU.ss -c_key C:/keyU.ss -c_cert C:/certU.ss -ssl2 -server_auth -client_auth -CAfile C:/certCA.ss
ssltest.exe ssltest(3_19_2_6674) -key C:/keyU.ss -cert C:/certU.ss -c_key C:/keyU.ss -c_cert C:/certU.ss -ssl2
ssltest.exe ssltest(3_19_2_6675) -key C:/keyU.ss -cert C:/certU.ss -c_key C:/keyU.ss -c_cert C:/certU.ss -ssl2 -server_auth -CAfile C:/certCA.ss
ssltest.exe ssltest(3_19_2_6676) -key C:/keyU.ss -cert C:/certU.ss -c_key C:/keyU.ss -c_cert C:/certU.ss -ssl2 -client_auth -CAfile C:/certCA.ss
ssltest.exe ssltest(3_19_2_6677) -key C:/keyU.ss -cert C:/certU.ss -c_key C:/keyU.ss -c_cert C:/certU.ss -ssl2 -server_auth -client_auth -CAfile C:/certCA.ss
ssltest.exe ssltest(3_19_2_6678) -key C:/keyU.ss -cert C:/certU.ss -c_key C:/keyU.ss -c_cert C:/certU.ss
ssltest.exe ssltest(3_19_2_6679) -key C:/keyU.ss -cert C:/certU.ss -c_key C:/keyU.ss -c_cert C:/certU.ss -server_auth -CAfile C:/certCA.ss
ssltest.exe ssltest(3_19_2_6680) -key C:/keyU.ss -cert C:/certU.ss -c_key C:/keyU.ss -c_cert C:/certU.ss -client_auth -CAfile C:/certCA.ss
ssltest.exe ssltest(3_19_2_6681) -key C:/keyU.ss -cert C:/certU.ss -c_key C:/keyU.ss -c_cert C:/certU.ss -server_auth -client_auth -CAfile C:/certCA.ss
ssltest.exe ssltest(3_19_2_6682) -key C:/keyU.ss -cert C:/certU.ss -c_key C:/keyU.ss -c_cert C:/certU.ss -bio_pair -ssl2
ssltest.exe ssltest(3_19_2_6683) -key C:/keyU.ss -cert C:/certU.ss -c_key C:/keyU.ss -c_cert C:/certU.ss -bio_pair -ssl2 -server_auth -CAfile C:/certCA.ss
ssltest.exe ssltest(3_19_2_6684) -key C:/keyU.ss -cert C:/certU.ss -c_key C:/keyU.ss -c_cert C:/certU.ss -bio_pair -ssl2 -client_auth -CAfile C:/certCA.ss
ssltest.exe ssltest(3_19_2_6685) -key C:/keyU.ss -cert C:/certU.ss -c_key C:/keyU.ss -c_cert C:/certU.ss -bio_pair -ssl2 -server_auth -client_auth -CAfile C:/certCA.ss
ssltest.exe ssltest(3_19_2_6686) -key C:/keyU.ss -cert C:/certU.ss -c_key C:/keyU.ss -c_cert C:/certU.ss -bio_pair -ssl2
ssltest.exe ssltest(3_19_2_6687) -key C:/keyU.ss -cert C:/certU.ss -c_key C:/keyU.ss -c_cert C:/certU.ss -bio_pair -ssl2
ssltest.exe ssltest(3_19_2_6688) -key C:/keyU.ss -cert C:/certU.ss -key C:/keyU.ss -cert C:/certU.ss -c_key C:/keyU.ss -c_cert C:/certU.ss -bio_pair -ssl2
ssltest.exe ssltest(3_19_2_6689) -key C:/keyU.ss -cert C:/certU.ss -c_key C:/keyU.ss -c_cert C:/certU.ss -bio_pair -ssl2
ssltest.exe ssltest(3_19_2_6690) -key C:/keyU.ss -cert C:/certU.ss -c_key C:/keyU.ss -c_cert C:/certU.ss
ssltest.exe ssltest(3_19_2_6691) -key C:/keyU.ss -cert C:/certU.ss -c_key C:/keyU.ss -c_cert C:/certU.ss -bio_pair -no_dhe
ssltest.exe ssltest(3_19_2_6692) -key C:/keyU.ss -cert C:/certU.ss -c_key C:/keyU.ss -c_cert C:/certU.ss -bio_pair -dhe1024dsa -v
ssltest.exe ssltest(3_19_2_6693) -key C:/keyU.ss -cert C:/certU.ss -c_key C:/keyU.ss -c_cert C:/certU.ss -bio_pair -server_auth -CAfile C:/certCA.ss
ssltest.exe ssltest(3_19_2_6694) -key C:/keyU.ss -cert C:/certU.ss -c_key C:/keyU.ss -c_cert C:/certU.ss -bio_pair -client_auth -CAfile C:/certCA.ss
ssltest.exe ssltest(3_19_2_6695) -key C:/keyU.ss -cert C:/certU.ss -c_key C:/keyU.ss -c_cert C:/certU.ss -bio_pair -server_auth -client_auth -CAfile C:/certCA.ss
ssltest.exe ssltest(3_19_2_6696) -key C:/keyU.ss -cert C:/certU.ss -c_key C:/keyU.ss -c_cert C:/certU.ss -bio_pair -server_auth -client_auth -app_verify -CAfile C:/certCA.ss
ssltest.exe ssltest(3_19_2_6697) -key C:/keyU.ss -cert C:/certU.ss -c_key C:/keyU.ss -c_cert C:/certU.ss -v -bio_pair -tls1 -cipher ADH -dhe1024dsa -num 10 -f -time
ssltest.exe ssltest(3_19_2_6698) -v -bio_pair -tls1 -cert C:/server2.pem -no_dhe -num 10-f -time
ssltest.exe ssltest(3_19_2_6699) -v -bio_pair -tls1 -cert c:/server2.pem -dhe1024dsa -num 10 -f -time
ssltest.exe ssltest(3_19_2_6773) -symstore

copy c:\spd_logs\xml\*.xml e:\bctest\results\ssl\