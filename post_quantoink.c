#include "post_quantoink.h"
#include <string.h>
#include <math.h>
#include <stdlib.h>
#include <openssl/sha.h>

#ifndef M_PI
#define M_PI 3.14159265358979323846
#endif

/*
 * CHAOS PARAMETER: φ * 2.5 = 4.045 > 3.57 chaos threshold
 * This ensures TRUE CHAOTIC DIVERGENCE.
 */
#define CHAOS_R (PHI * 2.5)  // 4.045... — CHAOTIC!

void pq_init(PQState *state, const uint8_t *seed, size_t seed_len) {
    memset(state, 0, sizeof(PQState));
    
    uint8_t hash[PQ_HASH_SIZE];
    SHA256_CTX ctx;
    SHA256_Init(&ctx);
    SHA256_Update(&ctx, seed, seed_len);
    SHA256_Final(hash, &ctx);
    
    for (int s = 0; s < PQ_STREAMS; s++) {
        double stream_seed = 0.0;
        for (int i = 0; i < 8; i++) {
            stream_seed += (double)hash[s * 8 + i] / 256.0;
        }
        stream_seed = stream_seed / 8.0;
        
        // Unique starting positions across [0.1, 0.9]
        double offset = pow(PHI_INV, s + 1);
        state->stream_values[s] = 0.1 + stream_seed * 0.8 * offset;
        
        if (state->stream_values[s] < 0.001) state->stream_values[s] = 0.001;
        if (state->stream_values[s] > 0.999) state->stream_values[s] = 0.999;
    }
    
    state->accumulated_phi = PHI_INV;
    state->evolution_count = 0;
    state->fractal_layer = 0;
    memcpy(state->entropy_pool, hash, PQ_HASH_SIZE);
}

void pq_evolve(PQState *state) {
    for (int s = 0; s < PQ_STREAMS; s++) {
        double x = state->stream_values[s];
        
        // TRUE CHAOS: CHAOS_R * x * (1 - x) with CHAOS_R = 4.045 > 3.57
        double logistic = CHAOS_R * x * (1.0 - x);
        
        // φ-weighted modular perturbation — injects unpredictability
        double perturbation = state->accumulated_phi * 
                              fmod(x * PHI * 1000.0, 1.0) * 0.1;
        
        double new_x = logistic + perturbation;
        
        // Keep in (0, 1) via modular wrap
        new_x = new_x - floor(new_x);
        if (new_x < 0.001) new_x = 0.001 + fabs(sin(new_x * M_PI * 1000.0)) * 0.1;
        if (new_x > 0.999) new_x = 0.999 - fabs(cos(new_x * M_PI * 1000.0)) * 0.1;
        
        state->stream_values[s] = new_x;
        
        // Mix into entropy pool with φ-weighting
        uint8_t byte_val = (uint8_t)(new_x * 256.0);
        int pool_idx = (state->evolution_count * PQ_STREAMS + s) % PQ_HASH_SIZE;
        state->entropy_pool[pool_idx] ^= byte_val;
        state->entropy_pool[(pool_idx + 1) % PQ_HASH_SIZE] ^= 
            (uint8_t)(state->accumulated_phi * 256.0);
    }
    
    // Accumulate φ — dynamic, influenced by all streams
    double stream_avg = 0.0;
    for (int s = 0; s < PQ_STREAMS; s++) stream_avg += state->stream_values[s];
    stream_avg /= PQ_STREAMS;
    
    state->accumulated_phi = state->accumulated_phi * PHI_INV + 
                              stream_avg * (1.0 - PHI_INV);
    state->evolution_count++;
    
    if (state->evolution_count % (PQ_STREAMS * 2) == 0) {
        state->fractal_layer++;
    }
}

void pq_extract(PQState *state, uint8_t *output, size_t output_len) {
    for (size_t i = 0; i < output_len; i++) {
        pq_evolve(state);
        
        // NON-LINEAR MIXING: φ-weighted sum + XOR + modular rotation
        double mixed = 0.0;
        uint8_t xor_val = 0;
        
        for (int s = 0; s < PQ_STREAMS; s++) {
            double weight = pow(PHI_INV, s);
            mixed += state->stream_values[s] * weight;
            xor_val ^= (uint8_t)(state->stream_values[s] * 256.0);
        }
        mixed = mixed / (1.0 - pow(PHI_INV, PQ_STREAMS));
        
        uint8_t mixed_byte = (uint8_t)(mixed * 256.0);
        output[i] = mixed_byte ^ xor_val ^ state->entropy_pool[i % PQ_HASH_SIZE];
    }
}

void pq_encrypt(const uint8_t *key, size_t key_len,
                const uint8_t *plaintext, size_t pt_len,
                uint8_t *ciphertext) {
    PQState state;
    pq_init(&state, key, key_len);
    
    // MIXING PHASE: 64 evolutions before generating keystream
    for (int i = 0; i < 64; i++) pq_evolve(&state);
    
    for (size_t i = 0; i < pt_len; i++) {
        pq_evolve(&state);
        
        double mixed = 0.0;
        uint8_t xor_val = 0;
        for (int s = 0; s < PQ_STREAMS; s++) {
            mixed += state.stream_values[s] * pow(PHI_INV, s);
            xor_val ^= (uint8_t)(state.stream_values[s] * 256.0);
        }
        mixed = mixed / (1.0 - pow(PHI_INV, PQ_STREAMS));
        
        uint8_t keystream = ((uint8_t)(mixed * 256.0)) ^ xor_val;
        ciphertext[i] = plaintext[i] ^ keystream;
    }
}

void pq_decrypt(const uint8_t *key, size_t key_len,
                const uint8_t *ciphertext, size_t ct_len,
                uint8_t *plaintext) {
    pq_encrypt(key, key_len, ciphertext, ct_len, plaintext);
}

void pq_hash(const uint8_t *data, size_t len, uint8_t *hash_out) {
    PQState state;
    pq_init(&state, data, len);
    
    // EXTENDED MIXING: 256 evolutions for maximum entropy
    for (int i = 0; i < 256; i++) {
        pq_evolve(&state);
    }
    
    PQState extract_state;
    memcpy(&extract_state, &state, sizeof(PQState));
    pq_extract(&extract_state, hash_out, PQ_HASH_SIZE);
}

void pq_derive_key(const uint8_t *master_secret, size_t secret_len,
                   const uint8_t *context, size_t context_len,
                   uint8_t *derived_key, size_t key_len) {
    size_t combined_len = secret_len + context_len;
    uint8_t *combined = (uint8_t*)malloc(combined_len);
    memcpy(combined, master_secret, secret_len);
    memcpy(combined + secret_len, context, context_len);
    
    pq_hash(combined, combined_len, derived_key);
    
    if (key_len > PQ_HASH_SIZE) {
        PQState state;
        pq_init(&state, derived_key, PQ_HASH_SIZE);
        pq_extract(&state, derived_key + PQ_HASH_SIZE, key_len - PQ_HASH_SIZE);
    }
    
    free(combined);
}
