#include <math.h>
#include <stdio.h>
#include <string.h>
#include <stdint.h>
#include "post_quantoink.h"

int main() {
    printf("╔═══════════════════════════════════════╗\n");
    printf("║  POST-QUANTOINK ALGORITHM             ║\n");
    printf("║  Chaotic Divergence + φ-Harmonics     ║\n");
    printf("╚═══════════════════════════════════════╝\n\n");

    // Test 1: State Evolution (Chaotic Divergence)
    printf("=== Test 1: Chaotic Divergence ===\n");
    const uint8_t seed1[] = "post-quantoink seed";
    PQState state;
    pq_init(&state, seed1, sizeof(seed1));
    
    printf("Initial streams (DIFFERENT positions):\n");
    for (int s = 0; s < PQ_STREAMS; s++) {
        printf("  Stream %d: %.10f\n", s, state.stream_values[s]);
    }
    
    // Evolve 10 times
    for (int i = 0; i < 10; i++) pq_evolve(&state);
    
    printf("\nAfter 10 evolutions (DIVERGED):\n");
    for (int s = 0; s < PQ_STREAMS; s++) {
        printf("  Stream %d: %.10f\n", s, state.stream_values[s]);
    }
    
    // Check if streams are DIFFERENT (diverged)
    int unique = 0;
    for (int s1 = 0; s1 < PQ_STREAMS; s1++) {
        for (int s2 = s1 + 1; s2 < PQ_STREAMS; s2++) {
            if (fabs(state.stream_values[s1] - state.stream_values[s2]) > 0.01) unique++;
        }
    }
    printf("  Divergence check: %d/6 pairs different %s\n\n",
           unique, unique >= 4 ? "✅ CHAOTIC" : "⚠️ CONVERGING");

    // Test 2: Encryption/Decryption
    printf("=== Test 2: Encrypt/Decrypt ===\n");
    const uint8_t key[] = "post-quantoink secret key";
    const uint8_t pt[] = "Post-Quantoink is ALIVE!";
    uint8_t ct[sizeof(pt)], dt[sizeof(pt)];
    
    pq_encrypt(key, sizeof(key), pt, sizeof(pt), ct);
    pq_decrypt(key, sizeof(key), ct, sizeof(pt), dt);
    
    printf("  Plaintext:  %s\n", pt);
    printf("  Ciphertext: ");
    for (size_t i = 0; i < sizeof(pt); i++) printf("%02x ", ct[i]);
    printf("\n  Decrypted:  %s\n", dt);
    printf("  Match: %s\n\n", memcmp(pt, dt, sizeof(pt)) == 0 ? "✅ YES" : "❌ NO");

    // Test 3: Wrong Key
    printf("=== Test 3: Wrong Key ===\n");
    const uint8_t wrong_key[] = "wrong secret key here!!";
    uint8_t wrong[sizeof(pt)];
    pq_decrypt(wrong_key, sizeof(wrong_key), ct, sizeof(pt), wrong);
    printf("  Wrong decrypt: %s\n", wrong);
    printf("  Match: %s (should be NO)\n\n", 
           memcmp(pt, wrong, sizeof(pt)) == 0 ? "⚠️ YES" : "✅ NO");

    // Test 4: Hashing
    printf("=== Test 4: Hash Function ===\n");
    const uint8_t msg[] = "Hash this with Post-Quantoink";
    uint8_t hash1[PQ_HASH_SIZE], hash2[PQ_HASH_SIZE];
    pq_hash(msg, sizeof(msg), hash1);
    pq_hash(msg, sizeof(msg), hash2);
    
    printf("  Hash 1: ");
    for (int i = 0; i < 16; i++) printf("%02x", hash1[i]);
    printf("\n  Hash 2: ");
    for (int i = 0; i < 16; i++) printf("%02x", hash2[i]);
    printf("\n  Deterministic: %s\n\n", 
           memcmp(hash1, hash2, PQ_HASH_SIZE) == 0 ? "✅ YES" : "❌ NO");

    // Test 5: Avalanche Effect
    printf("=== Test 5: Avalanche Effect ===\n");
    const uint8_t msg2[] = "Hash this with Post-Quantoinl";  // One bit different ('k' → 'l')
    uint8_t hash3[PQ_HASH_SIZE];
    pq_hash(msg2, sizeof(msg2), hash3);
    
    int diff_bits = 0;
    for (int i = 0; i < PQ_HASH_SIZE; i++) {
        uint8_t diff = hash1[i] ^ hash3[i];
        for (int b = 0; b < 8; b++) {
            if (diff & (1 << b)) diff_bits++;
        }
    }
    printf("  Changed bits: %d/256 (%.1f%%)\n", diff_bits, diff_bits * 100.0 / 256.0);
    printf("  Avalanche: %s\n\n",
           (diff_bits > 100 && diff_bits < 156) ? "✅ EXCELLENT" : 
           (diff_bits > 50) ? "⚠️ GOOD (improving)" : "❌ NEEDS WORK");

    // Test 6: Key Derivation
    printf("=== Test 6: Key Derivation ===\n");
    const uint8_t master[] = "master secret key";
    const uint8_t context1[] = "encryption-key";
    const uint8_t context2[] = "authentication-key";
    uint8_t key1[PQ_KEY_SIZE], key2[PQ_KEY_SIZE];
    
    pq_derive_key(master, sizeof(master), context1, sizeof(context1), key1, PQ_KEY_SIZE);
    pq_derive_key(master, sizeof(master), context2, sizeof(context2), key2, PQ_KEY_SIZE);
    
    printf("  Key 1 (encryption): ");
    for (int i = 0; i < 8; i++) printf("%02x", key1[i]);
    printf("\n  Key 2 (auth):       ");
    for (int i = 0; i < 8; i++) printf("%02x", key2[i]);
    
    int key_diff = 0;
    for (int i = 0; i < PQ_KEY_SIZE; i++) {
        if (key1[i] != key2[i]) key_diff++;
    }
    printf("\n  Different bytes: %d/%d %s\n\n",
           key_diff, PQ_KEY_SIZE, key_diff > 20 ? "✅ GOOD" : "⚠️ SIMILAR");

    printf("╔═══════════════════════════════════════╗\n");
    printf("║  POST-QUANTOINK — ALGORITHM ACTIVE   ║\n");
    printf("║  Encrypt + Decrypt + Hash + KDF       ║\n");
    printf("║  ΦΩ0 — I AM THAT I AM                ║\n");
    printf("╚═══════════════════════════════════════╝\n");

    return 0;
}
