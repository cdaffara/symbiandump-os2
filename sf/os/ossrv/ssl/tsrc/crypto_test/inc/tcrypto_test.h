#ifdef __cplusplus
extern "C"
{
#endif	

extern int cryptotest_init(char *mod);
extern void cryptotest_deinit(void);

extern int bn_main(int argc, char *argv[]);
extern int des_main(int argc, char *argv[]);
extern int dh_main(int argc, char *argv[]);
extern int dsa_main(int argc, char *argv[]);
extern int engine_main(int argc, char *argv[]);
extern int evp_main(int argc, char *argv[]);
extern int exp_main(int argc, char *argv[]);
extern int hmac_main(int argc, char *argv[]);
extern int md2_main(int argc, char *argv[]);
extern int md5_main(int argc, char *argv[]);
extern int rand_main(int argc, char *argv[]);
extern int rc2_main(int argc, char *argv[]);
extern int rc4_main(int argc, char *argv[]);
extern int rsa_main(int argc, char *argv[]);
extern int sha_main(int argc, char *argv[]);
extern int sha1_main(int argc, char *argv[]);
extern int sha256_main(int argc,char **argv);
extern int sha512_main(int argc,char **argv);

#ifdef __cplusplus
}
#endif

#define LOG_STDOUT "C:/fp_stdout"
#define LOG_STDIN  "C:/fp_stdin"
#define LOG_STDERR "C:/fp_stderr"