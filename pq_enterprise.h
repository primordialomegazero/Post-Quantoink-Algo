#ifndef PQ_ENTERPRISE_H
#define PQ_ENTERPRISE_H

#include "post_quantoink.h"
#include <stdint.h>
#include <stddef.h>

/*
 * POST-QUANTOINK ENTERPRISE
 * 
 * Production-grade cryptographic operations:
 * - AEAD (Authenticated Encryption with Associated Data)
 * - Secure memory handling
 * - Key strengthening
 * - Multi-threaded extraction
 */

#define PQ_AEAD_KEY_SIZE 32
#define PQ_AEAD_NONCE_SIZE 12
#define PQ_AEAD_TAG_SIZE 16

/*
 * AEAD Encrypt: ciphertext + authentication tag
 * 
 * encrypt(m, key, nonce, aad) → (c, tag)
 * decrypt(c, tag, key, nonce, aad) → m or FAIL
 */
int pq_aead_encrypt(const uint8_t *key, size_t key_len,
                    const uint8_t *nonce, size_t nonce_len,
                    const uint8_t *plaintext, size_t pt_len,
                    const uint8_t *aad, size_t aad_len,
                    uint8_t *ciphertext,
                    uint8_t *tag, size_t tag_len);

int pq_aead_decrypt(const uint8_t *key, size_t key_len,
                    const uint8_t *nonce, size_t nonce_len,
                    const uint8_t *ciphertext, size_t ct_len,
                    const uint8_t *aad, size_t aad_len,
                    const uint8_t *tag, size_t tag_len,
                    uint8_t *plaintext);

/*
 * Secure Memory Operations
 */
void pq_secure_zero(void *ptr, size_t len);
void pq_secure_alloc(void **ptr, size_t len);
void pq_secure_free(void *ptr, size_t len);

/*
 * Key Strengthening
 */
void pq_strengthen_key(const uint8_t *password, size_t password_len,
                       const uint8_t *salt, size_t salt_len,
                       uint8_t *key, size_t key_len,
                       int iterations);

/*
 * Multi-Threaded Extraction
 */
void pq_extract_parallel(PQState *state, uint8_t *output, size_t output_len,
                         int num_threads);

#endif
