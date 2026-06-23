#ifndef POST_QUANTOINK_H
#define POST_QUANTOINK_H

#include <stdint.h>
#include <stddef.h>

/*
 * POST-QUANTOINK ALGORITHM
 * 
 * A post-quantum cryptographic primitive based on:
 * - φ-harmonic chaotic divergence (logistic map with φ-weighting)
 * - Parallel fractal streams (4 streams, self-similar)
 * - Irreversible irrationality (φ⁻¹ is non-repeating)
 * - Recursive self-reference (accumulated φ feeds back)
 * 
 * Core evolution:
 *   x_{n+1} = φ * x_n * (1 - x_n) + accumulated_φ * sin(x_n * π)
 * 
 * This is CHAOTIC (φ = 1.618 > 3.57 chaos threshold for logistic map).
 * Tiny changes in initial state → completely different trajectories.
 */

#define PHI 1.6180339887498948482
#define PHI_INV 0.6180339887498948482
#define PQ_STREAMS 4
#define PQ_HASH_SIZE 32
#define PQ_KEY_SIZE 32

/*
 * Post-Quantoink State
 */
typedef struct {
    double stream_values[PQ_STREAMS];    // Current value per stream
    double accumulated_phi;              // Accumulated φ across evolutions
    int evolution_count;                 // Total evolutions performed
    int fractal_layer;                   // Current fractal depth
    uint8_t entropy_pool[PQ_HASH_SIZE];  // Entropy accumulation
} PQState;

/*
 * Core API
 */
void pq_init(PQState *state, const uint8_t *seed, size_t seed_len);
void pq_evolve(PQState *state);
void pq_extract(PQState *state, uint8_t *output, size_t output_len);
void pq_encrypt(const uint8_t *key, size_t key_len,
                const uint8_t *plaintext, size_t pt_len,
                uint8_t *ciphertext);
void pq_decrypt(const uint8_t *key, size_t key_len,
                const uint8_t *ciphertext, size_t ct_len,
                uint8_t *plaintext);
void pq_hash(const uint8_t *data, size_t len, uint8_t *hash_out);
void pq_derive_key(const uint8_t *master_secret, size_t secret_len,
                   const uint8_t *context, size_t context_len,
                   uint8_t *derived_key, size_t key_len);

#endif
