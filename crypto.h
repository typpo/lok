#ifndef CRYPTO_H
#define CRYPTO_H
int crypto_e(char *key, char *text);

int crypto_d(char *key, char *text);

int crypto_hash(char *key);
#endif
