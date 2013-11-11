Current state of filecertstore tests

Scripts

UNIFIEDCERTSTORE1-CONF1.txt - All tests pass 

UNIFIEDCERTSTORE2-CONF1.txt - All tests pass (use techview mode)

UNIFIEDCERTSTORE3-CONF1.txt - Most tests pass (expect 41 to fail)
Failure occurs because the tests attempt to add some certificates for which private keys do not exist yet AND because the test tries to do filtered listing on user certificates for a number of different keyId and key usage combinations.

To resolve these issues we need to 
(a) generate a couple more certificates with keys that have restricted usages, and import the keys at the beginning of the script (with the appropriate usage)

(b) Rewrite the filtering tests to use the appropriate combinations

So far, I've added 3 certificates based on 3 different RSA keys with all usage (rsacert1, rsacert2 and rsacert3) and 1 certificate based on a NR key with NR usage (nrcert)