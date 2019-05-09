#include "platform.h"
#include "printf.h"
#include "string.h"
#include "test.h"


int perform_aes_basic_test(aes_test_case *test, STRUCT_AES *aes_ctx){
	int ret_err;
	unsigned char ret;
	
	unsigned char *r_aes = NULL, *r_key = NULL;
	if(test->random_aes_len == 19){
		r_aes = test->random_aes;
	}
	if(test->random_key_len == 19){
		r_key = test->random_key;
	}

	ret = aes(test->mode, aes_ctx, test->key, test->msg, test->out, r_aes, r_key);
	/* Checks */
	if(test->to_check & CHECK_RET){
		if(ret != test->exp_ret){
			printf("[AES tests] Error: test \"%s\" failed, bad expected ret!\n", test->name); 
			ret_err = 1;
			goto err;
		}
	}
	if(test->to_check & CHECK_OUTPUT){
		if(local_memcmp(test->out, test->exp_out, 16) != 0){
			printf("[AES tests] Error: test \"%s\" failed, bad expected output!\n", test->name); 
			ret_err = 2;
			goto err;
		}
	}
	if(test->to_check & CHECK_KEY_CTR){
		if(aes_ctx->ctr_key != test->exp_ctr_key){
			printf("[AES tests] Error: test \"%s\" failed, bad expected key counter!\n", test->name); 
			ret_err = 3;
			goto err;
		}
	}
	if(test->to_check & CHECK_AES_CTR){
		if(aes_ctx->ctr_aes != test->exp_ctr_aes){
			printf("[AES tests] Error: test \"%s\" failed, bad expected AES counter!\n", test->name); 
			ret_err = 4;
			goto err;
		}
	}
	if(test->to_check & CHECK_AES_CTX){
		if(test->exp_aes_ctx != NULL){
			if(local_memcmp(test->exp_aes_ctx, aes_ctx, sizeof(STRUCT_AES)) != 0){
				printf("[AES tests] Error: test \"%s\" failed, bad expected AES context!\n", test->name); 
				ret_err = 5;
				goto err;
			}
		}
	}
	if(test->to_check & CHECK_STATE){
		if(aes_ctx->state != test->exp_state){
			printf("[AES tests] Error: test \"%s\" failed, bad expected AES state!\n", test->name); 
			ret_err = 6;
			goto err;
		}
	}

	return 0;
err:
	return ret_err;
}

extern unsigned int static_tests_size;
int get_buffer(char *dest, const char **test_string, unsigned int size){
	if(test_string == NULL){
		/* Get the test from the comm line */
		while(_comm_getbuffer(dest, size)){};
	}
	else{
		/* Get the test from the internal buffer */
		if(static_tests_size < size){
			goto err;
		}
		local_memcpy(dest, *test_string, size);
		*test_string += size;
		static_tests_size -= size;
	}

	return 0;
err:
	return -1;	
}

/* Receive a bunch of tests and perform them */
int perform_aes_tests(const char **test_string, cb_aes_test_t callback){
	char c;
	STRUCT_AES aes_ctx;
	aes_test_case test;

	if(test_string == NULL){
		printf("[Tests] Welcome to dynamic AES tests ... Waiting on the line!\n");
	}
	else{
		printf("[Tests] Welcome to static AES tests\n");
	}
	while(1){
		if(get_buffer(&c, test_string, 1)){
			goto err;
		}
		switch (c) {
			/* Begin a test session */
			case 'b':{
				printf("[Tests] Begin received!\n");
				local_memset(&aes_ctx, 0, sizeof(aes_ctx));
				local_memset(&test, 0, sizeof(test));
				/* Call the optional callback */
				if(callback != NULL){
					callback();
				}
				break;
			}
			/* End a test session */
			case 'e':{
				printf("[Tests] End received!\n");
				break;
			}
			/* We have received a test case */
			case 't':{
				local_memset(&test, 0, sizeof(test));
				printf("[Tests] Basic test received!\n");
				/* Receive the serialized test */
				/* Name */
				if(get_buffer((char*)&test.name, test_string, 32)){ goto err; }
				/* Mode */
				if(get_buffer((char*)&test.mode, test_string, 1)){ goto err; }
				/* To check */
				if(get_buffer((char*)&test.to_check, test_string, 1)){ goto err; }
				/* Message */
				if(get_buffer((char*)&test.msg_len, test_string, 1)){ goto err; }
				if(get_buffer((char*)&test.msg, test_string, 16)){ goto err; }
				/* Key */
				if(get_buffer((char*)&test.key_len, test_string, 1)){ goto err; }
				if(get_buffer((char*)&test.key, test_string, 32)){ goto err; }
				/* Random for key schedule */
				if(get_buffer((char*)&test.random_key_len, test_string, 1)){ goto err; }
				if(get_buffer((char*)&test.random_key, test_string, 19)){ goto err; }
				/* Random for AES */
				if(get_buffer((char*)&test.random_aes_len, test_string, 1)){ goto err; }
				if(get_buffer((char*)&test.random_aes, test_string, 19)){ goto err; }
				/* Expected output */
				if(get_buffer((char*)&test.exp_out_len, test_string, 1)){ goto err; }
				if(get_buffer((char*)&test.exp_out, test_string, 16)){ goto err; }
				/* Expected ret */
				if(get_buffer((char*)&test.exp_ret, test_string, 4)){ goto err; }
				/* Expected state */
				if(get_buffer((char*)&test.exp_state, test_string, 1)){ goto err; }
				/* Expected key counter */
				if(get_buffer((char*)&test.exp_ctr_key, test_string, 4)){ goto err; }
				/* Expected AES counter */
				if(get_buffer((char*)&test.exp_ctr_aes, test_string, 4)){ goto err; }
				/* Line to output return elements on */
				if(get_buffer((char*)&test.comm_line_output, test_string, 1)){ goto err; }
				test.exp_aes_ctx = NULL;
#ifdef MEASURE_PERFORMANCE
				unsigned int cycles = 0;
				cortex_m4_cycles_reset();
#endif
				/* Perform the test */
				int ret = perform_aes_basic_test(&test, &aes_ctx);
#ifdef MEASURE_PERFORMANCE
				cycles = get_cortex_m4_cycles();
#endif
				if(ret){
					printf("[X] !!!! Test \"%s\" not OK :-(. Return = %d\n", test.name, ret);
				}
				else{
#ifdef MEASURE_PERFORMANCE
					printf("[+] OK!: test \"%s\", cycles = %d\n", test.name, cycles);
#else
					printf("[+] OK!: test \"%s\"\n", test.name);
#endif
				}
				if(test.comm_line_output){
					unsigned int i;
					/* Return value */
					_comm_putchar('r');
					_comm_putchar((char)ret & 0xff);
					for(i = 0; i < 16; i++){
						_comm_putchar(test.out[i]);
					}
				}
				break;
			}
			default:
				break;
		}
	}

	return 0;
err:
	return -1;
}
