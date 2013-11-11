## Makefile for vector generation code.

all:		rsa_test dsa_test gen_random gen_rsakey gen_dsakey

rsa_test:	rsa_test.c keys.c keys.h utils.c utils.h
			gcc rsa_test.c keys.c utils.c -o rsa_test -lcrypto

dsa_test:	dsa_test.c utils.c utils.h
			gcc dsa_test.c utils.c -o dsa_test -lcrypto

gen_random:	gen_random.c
			gcc gen_random.c -o gen_random -lcrypto

gen_rsakey: gen_rsakey.c utils.c utils.h
			gcc gen_rsakey.c utils.c -o gen_rsakey -lcrypto

gen_dsakey: gen_dsakey.c utils.c utils.h
			gcc gen_dsakey.c utils.c -o gen_dsakey -lcrypto

clean:
			rm -f rsa_test dsa_test gen_random gen_dsakey