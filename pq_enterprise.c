#define _GNU_SOURCE
#include "pq_enterprise.h"
#include <string.h>
#include <stdlib.h>
#include <math.h>
#include <pthread.h>

#ifdef _WIN32
#include <windows.h>
#else
#include <sys/mman.h>
#include <unistd.h>
#endif

int pq_aead_encrypt(const uint8_t *key, size_t key_len,
                    const uint8_t *nonce, size_t nonce_len,
                    const uint8_t *plaintext, size_t pt_len,
                    const uint8_t *aad, size_t aad_len,
                    uint8_t *ciphertext,
                    uint8_t *tag, size_t tag_len) {
    uint8_t enc_key[PQ_AEAD_KEY_SIZE];
    uint8_t mac_key[PQ_AEAD_KEY_SIZE];
    
    pq_derive_key(key, key_len, nonce, nonce_len, enc_key, PQ_AEAD_KEY_SIZE);
    
    uint8_t mac_nonce[16] = "mac-derivation";
    pq_derive_key(key, key_len, mac_nonce, 16, mac_key, PQ_AEAD_KEY_SIZE);
    
    pq_encrypt(enc_key, PQ_AEAD_KEY_SIZE, plaintext, pt_len, ciphertext);
    
    size_t mac_input_len = pt_len + aad_len;
    uint8_t *mac_input = (uint8_t*)malloc(mac_input_len);
    memcpy(mac_input, ciphertext, pt_len);
    memcpy(mac_input + pt_len, aad, aad_len);
    
    pq_derive_key(mac_key, PQ_AEAD_KEY_SIZE, mac_input, mac_input_len, 
                  tag, tag_len);
    
    free(mac_input);
    pq_secure_zero(enc_key, PQ_AEAD_KEY_SIZE);
    pq_secure_zero(mac_key, PQ_AEAD_KEY_SIZE);
    
    return 0;
}

int pq_aead_decrypt(const uint8_t *key, size_t key_len,
                    const uint8_t *nonce, size_t nonce_len,
                    const uint8_t *ciphertext, size_t ct_len,
                    const uint8_t *aad, size_t aad_len,
                    const uint8_t *tag, size_t tag_len,
                    uint8_t *plaintext) {
    uint8_t enc_key[PQ_AEAD_KEY_SIZE];
    uint8_t mac_key[PQ_AEAD_KEY_SIZE];
    
    pq_derive_key(key, key_len, nonce, nonce_len, enc_key, PQ_AEAD_KEY_SIZE);
    
    uint8_t mac_nonce[16] = "mac-derivation";
    pq_derive_key(key, key_len, mac_nonce, 16, mac_key, PQ_AEAD_KEY_SIZE);
    
    uint8_t expected_tag[PQ_AEAD_TAG_SIZE];
    size_t mac_input_len = ct_len + aad_len;
    uint8_t *mac_input = (uint8_t*)malloc(mac_input_len);
    memcpy(mac_input, ciphertext, ct_len);
    memcpy(mac_input + ct_len, aad, aad_len);
    
    pq_derive_key(mac_key, PQ_AEAD_KEY_SIZE, mac_input, mac_input_len,
                  expected_tag, tag_len);
    
    if (memcmp(expected_tag, tag, tag_len) != 0) {
        free(mac_input);
        pq_secure_zero(enc_key, PQ_AEAD_KEY_SIZE);
        pq_secure_zero(mac_key, PQ_AEAD_KEY_SIZE);
        return -1;
    }
    
    free(mac_input);
    pq_decrypt(enc_key, PQ_AEAD_KEY_SIZE, ciphertext, ct_len, plaintext);
    
    pq_secure_zero(enc_key, PQ_AEAD_KEY_SIZE);
    pq_secure_zero(mac_key, PQ_AEAD_KEY_SIZE);
    
    return 0;
}

void pq_secure_zero(void *ptr, size_t len) {
    volatile uint8_t *p = (volatile uint8_t*)ptr;
    while (len--) *p++ = 0;
}

void pq_secure_alloc(void **ptr, size_t len) {
#ifdef _WIN32
    *ptr = VirtualAlloc(NULL, len, MEM_COMMIT | MEM_RESERVE, PAGE_READWRITE);
    if (*ptr) VirtualLock(*ptr, len);
#else
    long page_size = sysconf(_SC_PAGESIZE);
    size_t alloc_size = ((len + page_size - 1) / page_size) * page_size + 2 * page_size;
    
    void *mem = mmap(NULL, alloc_size, PROT_READ | PROT_WRITE,
                     MAP_PRIVATE | MAP_ANONYMOUS, -1, 0);
    
    if (mem != MAP_FAILED) {
        mprotect(mem, page_size, PROT_NONE);
        mprotect(mem + alloc_size - page_size, page_size, PROT_NONE);
        mlock(mem + page_size, len);
        *ptr = mem + page_size;
    } else {
        *ptr = NULL;
    }
#endif
}

void pq_secure_free(void *ptr, size_t len) {
    if (!ptr) return;
    pq_secure_zero(ptr, len);
#ifdef _WIN32
    VirtualUnlock(ptr, len);
    VirtualFree(ptr, 0, MEM_RELEASE);
#else
    long page_size = sysconf(_SC_PAGESIZE);
    void *base = (uint8_t*)ptr - page_size;
    size_t alloc_size = ((len + page_size - 1) / page_size) * page_size + 2 * page_size;
    munlock(ptr, len);
    munmap(base, alloc_size);
#endif
}

void pq_strengthen_key(const uint8_t *password, size_t password_len,
                       const uint8_t *salt, size_t salt_len,
                       uint8_t *key, size_t key_len,
                       int iterations) {
    uint8_t temp[PQ_HASH_SIZE];
    
    size_t combined_len = password_len + salt_len;
    uint8_t *combined = (uint8_t*)malloc(combined_len);
    memcpy(combined, password, password_len);
    memcpy(combined + password_len, salt, salt_len);
    
    pq_hash(combined, combined_len, temp);
    free(combined);
    
    for (int i = 1; i < iterations; i++) {
        uint8_t iter_input[PQ_HASH_SIZE + 4];
        memcpy(iter_input, temp, PQ_HASH_SIZE);
        iter_input[PQ_HASH_SIZE] = (i >> 24) & 0xFF;
        iter_input[PQ_HASH_SIZE+1] = (i >> 16) & 0xFF;
        iter_input[PQ_HASH_SIZE+2] = (i >> 8) & 0xFF;
        iter_input[PQ_HASH_SIZE+3] = i & 0xFF;
        
        pq_hash(iter_input, PQ_HASH_SIZE + 4, temp);
    }
    
    memcpy(key, temp, key_len < PQ_HASH_SIZE ? key_len : PQ_HASH_SIZE);
    pq_secure_zero(temp, PQ_HASH_SIZE);
}

typedef struct {
    PQState *state;
    uint8_t *output;
    size_t start;
    size_t len;
} PQThreadData;

void* pq_extract_worker(void *arg) {
    PQThreadData *data = (PQThreadData*)arg;
    PQState local_state;
    memcpy(&local_state, data->state, sizeof(PQState));
    
    for (size_t i = 0; i < data->start * 4; i++) {
        pq_evolve(&local_state);
    }
    
    for (size_t i = 0; i < data->len; i++) {
        pq_evolve(&local_state);
        
        double mixed = 0.0;
        uint8_t xor_val = 0;
        for (int s = 0; s < PQ_STREAMS; s++) {
            mixed += local_state.stream_values[s] * pow(PHI_INV, s);
            xor_val ^= (uint8_t)(local_state.stream_values[s] * 256.0);
        }
        mixed = mixed / (1.0 - pow(PHI_INV, PQ_STREAMS));
        
        data->output[data->start + i] = ((uint8_t)(mixed * 256.0)) ^ xor_val;
    }
    
    return NULL;
}

void pq_extract_parallel(PQState *state, uint8_t *output, size_t output_len,
                         int num_threads) {
    if (num_threads <= 1 || output_len < (size_t)num_threads) {
        pq_extract(state, output, output_len);
        return;
    }
    
    pthread_t *threads = (pthread_t*)malloc(num_threads * sizeof(pthread_t));
    PQThreadData *thread_data = (PQThreadData*)malloc(num_threads * sizeof(PQThreadData));
    
    size_t chunk_size = output_len / num_threads;
    
    for (int t = 0; t < num_threads; t++) {
        thread_data[t].state = state;
        thread_data[t].output = output;
        thread_data[t].start = t * chunk_size;
        thread_data[t].len = (t == num_threads - 1) ? 
                             output_len - thread_data[t].start : chunk_size;
        
        pthread_create(&threads[t], NULL, pq_extract_worker, &thread_data[t]);
    }
    
    for (int t = 0; t < num_threads; t++) {
        pthread_join(threads[t], NULL);
    }
    
    free(threads);
    free(thread_data);
}
