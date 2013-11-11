May 16th 2007
-------------

PREQ57 (CryptoSPI) has now been delivered into the MCL. This
replaces this component ('crypto') for 9.5 onwards. CryptoSPI
is binary compatible with crypto, and provides header files in
the following locations:

   cryptospi/inc        -- old 'crypto' header files
   cryptospi/inc/spi    -- new 'cryptospi' header files

A number of other teams' MMP files continue to use hard-coded
paths of 'common/generic/security/crypto/inc' within the MMP
file. They do not use /epoc32/include for export-control
reasons. When 9.4 has been branched from the MCL, these MMP
files should be rewritten to use the location 'cryptospi/inc'
instead. These MMP files are:

   common/generic/j2me/utils/JavaMIDletInstaller/group/JavaHelper.mmp
   common/generic/networking/ipsec/ipsec_itest/group/ts_ipsec.mmp
   common/generic/networking/ipsec/pkiservice/group/pkiservicenew.mmp
   common/generic/networking/ipsec/pkiservice/group/pkiservicesymb.mmp
   common/generic/networking/ipsec/utlpkcs1/group/utlpkcs1new.mmp
   common/generic/networking/ipsec/utlpkcs5/group/utlpkcs5new.mmp
   common/generic/networking/ppp/group/PPP.MMP
   common/generic/networking/ppp/te_ppp/group/t_ppp_lib.mmp
   common/generic/syslibs/store/BMAKE/ESTOR.MMP
   common/generic/syslibs/store/TCRYPT/T_CRYPT.MMP
   common/generic/syslibs/store/TPAGE/T_SPAGE.MMP
   common/generic/wap-stack/wapstack/group/wapstksrv.mmp

Once these MMP files have been modified, the 'crypto' component tree
can be deleted from Perforce and a TBAS build completed to confirm
that there are no regressions. This will enable the old 'crypto' code
to be removed once 9.4 has branched from the MCL.

