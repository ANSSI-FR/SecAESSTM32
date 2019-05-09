#ifndef __TEST_H__
#define __TEST_H__

#include "aes.h"

#define CHECK_OUTPUT	(0x1 << 0)
#define CHECK_RET	(0x1 << 1)
#define CHECK_STATE     (0x1 << 2)
#define CHECK_KEY_CTR   (0x1 << 3)
#define CHECK_AES_CTR   (0x1 << 4)
#define CHECK_AES_CTX   (0x1 << 5)


/* This file contains our test vector format */
typedef struct {
	/* Test case name */
	char name[33];
	/* The mode */
	unsigned char mode;
	/* What do we want to check? */
	unsigned char to_check;
	/* Message */
	unsigned char msg_len;
	unsigned char msg[16];
	/* Real output */
	unsigned char out[16];
	/* Key */
	unsigned char key_len;
	unsigned char key[32];
	/* Randoms */
	unsigned char random_aes_len;
	unsigned char random_aes[19];
	unsigned char random_key_len;
	unsigned char random_key[19];
	/* Expected output */
	unsigned char exp_out_len;
	unsigned char exp_out[16];
	/* Expected Error */
	unsigned int exp_ret;
	/* Expected state */
	unsigned char exp_state;
	/* Expected key counter */
	unsigned int exp_ctr_key;
	/* Expected aes counter */
	unsigned int exp_ctr_aes;
	/* Optional context to check */
	STRUCT_AES *exp_aes_ctx;
	/* Output line */
	unsigned char comm_line_output;
} aes_test_case;

/* AES test callback */
typedef void (*cb_aes_test_t)(void);

int perform_aes_basic_test(aes_test_case *test, STRUCT_AES *aes_ctx);

int perform_aes_tests(const char **test_string, cb_aes_test_t callback);

#endif /* __TEST_H__ */
