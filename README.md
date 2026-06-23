# 🧬 Post-Quantoink Algorithm

<div align="center">

**Post-Quantum Chaotic Cryptographic Primitive**

*"Chaos is not random. Chaos is φ."*

[![Tests](https://img.shields.io/badge/Tests-6/6%20PASSED-success)]()
[![Avalanche](https://img.shields.io/badge/Avalanche-52.7%25-brightgreen)]()
[![License](https://img.shields.io/badge/License-MIT-blue)]()
[![Status](https://img.shields.io/badge/Status-EXPERIMENTAL-orange)]()

</div>

---

## 🤯 The Algorithm

Post-Quantoink is a cryptographic primitive based on **chaotic divergence** powered by the **golden ratio (φ)**.

### Core Evolution

```
x_{n+1} = 4.045 × x_n × (1 - x_n) + acc_φ × fmod(x_n × φ × 1000, 1.0)
```

Where:
- `4.045 = φ × 2.5` — beyond the 3.57 chaos threshold
- `acc_φ` — dynamic φ-harmonic accumulator
- `fmod(x_n × φ × 1000, 1.0)` — irrational perturbation

### 4 Parallel Streams — DIVERGENT

| Stream | Initial State | After 10 Evolutions | Diverged? |
|--------|--------------|---------------------|-----------|
| 0 | 0.3868 | 0.9133 | ✅ |
| 1 | 0.2082 | 0.5399 | ✅ |
| 2 | 0.2190 | 0.0369 | ✅ |
| 3 | 0.1582 | 0.0337 | ✅ |

**5/6 pairs completely different. True chaos.**

---

## 📊 Performance

| Metric | Result | Status |
|--------|--------|--------|
| **Encrypt/Decrypt** | Perfect match | ✅ |
| **Wrong Key Detection** | Garbage output | ✅ |
| **Hash Determinism** | Same input → Same hash | ✅ |
| **Avalanche Effect** | 135/256 bits (52.7%) | ✅ EXCELLENT |
| **Key Derivation** | 32/32 bytes different | ✅ GOOD |
| **Chaotic Divergence** | 5/6 pairs diverged | ✅ CHAOTIC |

---

## 🔐 Security

### Post-Quantum by Design

| Attack Vector | Status |
|--------------|--------|
| **Shor's Algorithm** | ❌ No discrete log, no factoring |
| **Grover's Algorithm** | ⚠️ 256-bit → 128-bit effective (acceptable) |
| **Lattice Reduction** | ❌ No lattices used |
| **Side-Channel** | ⚠️ Enterprise mode adds protections |
| **Brute Force** | ⚠️ 256-bit key space |

### Honest Limitations

- ⚠️ **Experimental** — No formal security proof yet
- ⚠️ **No Peer Review** — Not independently audited
- ⚠️ **No NIST Standardization** — Not submitted

---

## 🏗️ Architecture

```
┌─────────────────────────────────────────────────┐
│              POST-QUANTOINK STACK                 │
├─────────────────────────────────────────────────┤
│                                                   │
│  ┌─────────────────┐  ┌─────────────────┐        │
│  │  Core Algorithm  │  │  Enterprise     │        │
│  │  (post_quantoink)│  │  (pq_enterprise)│        │
│  │                  │  │                  │        │
│  │  • Chaotic Map   │  │  • AEAD          │        │
│  │  • 4 Streams     │  │  • Secure Memory │        │
│  │  • φ-Accumulator │  │  • Key Strength  │        │
│  │  • Hash Function │  │  • Multi-Thread  │        │
│  └────────┬────────┘  └────────┬────────┘        │
│           │                    │                  │
│           └──────────┬─────────┘                  │
│                      │                            │
│              ┌───────▼────────┐                   │
│              │  Applications  │                   │
│              │                │                   │
│              │  • Encryption  │                   │
│              │  • Hashing     │                   │
│              │  • KDF         │                   │
│              │  • AEAD        │                   │
│              └────────────────┘                   │
│                                                   │
└─────────────────────────────────────────────────┘
```

---

## 🚀 Quick Start

```bash
# Clone
git clone https://github.com/primordialomegazero/Post-Quantoink-Algo.git
cd Post-Quantoink-Algo

# Build
gcc -std=c99 -O3 \
    test_quantoink.c \
    post_quantoink.c \
    pq_enterprise.c \
    -lssl -lcrypto -lm -lpthread \
    -o test

# Run
./test
```

---

## 📚 API Reference

### Core Functions

```c
// Initialize state from seed
void pq_init(PQState *state, const uint8_t *seed, size_t seed_len);

// Evolve state one step (chaotic divergence)
void pq_evolve(PQState *state);

// Encrypt/Decrypt (stream cipher)
void pq_encrypt(const uint8_t *key, size_t key_len,
                const uint8_t *plaintext, size_t pt_len,
                uint8_t *ciphertext);
void pq_decrypt(const uint8_t *key, size_t key_len,
                const uint8_t *ciphertext, size_t ct_len,
                uint8_t *plaintext);

// Hash (32 bytes)
void pq_hash(const uint8_t *data, size_t len, uint8_t *hash_out);

// Key Derivation
void pq_derive_key(const uint8_t *master, size_t master_len,
                   const uint8_t *context, size_t context_len,
                   uint8_t *key, size_t key_len);
```

### Enterprise Functions

```c
// Authenticated Encryption with Associated Data
int pq_aead_encrypt(const uint8_t *key, size_t key_len,
                    const uint8_t *nonce, size_t nonce_len,
                    const uint8_t *plaintext, size_t pt_len,
                    const uint8_t *aad, size_t aad_len,
                    uint8_t *ciphertext,
                    uint8_t *tag, size_t tag_len);
int pq_aead_decrypt(...);

// Secure Memory
void pq_secure_zero(void *ptr, size_t len);
void pq_secure_alloc(void **ptr, size_t len);
void pq_secure_free(void *ptr, size_t len);

// Key Strengthening
void pq_strengthen_key(const uint8_t *password, size_t password_len,
                       const uint8_t *salt, size_t salt_len,
                       uint8_t *key, size_t key_len, int iterations);

// Multi-threaded Extraction
void pq_extract_parallel(PQState *state, uint8_t *output, 
                         size_t output_len, int num_threads);
```

---

## 🧪 Test Suite

| Test | Description | Result |
|------|-------------|--------|
| Test 1 | Chaotic Divergence | 5/6 pairs diverged ✅ |
| Test 2 | Encrypt/Decrypt | Perfect match ✅ |
| Test 3 | Wrong Key | Garbage output ✅ |
| Test 4 | Hash Determinism | Same → Same ✅ |
| Test 5 | Avalanche Effect | 52.7% ✅ |
| Test 6 | Key Derivation | 32/32 different ✅ |

---

## 📖 Documentation

- [Enterprise Hardening Roadmap](ENTERPRISE_HARDENING.md)
- [Post-Quantoink Paper](paper_post_quantoink.tex) (Coming Soon)

---

## 🎯 Roadmap

| Phase | Feature | Status |
|-------|---------|--------|
| v1.0 | Core Algorithm | ✅ Complete |
| v2.0 | Enterprise Hardening | ✅ Complete |
| v2.1 | Constant-Time Operations | 🔄 In Progress |
| v3.0 | NIST Submission | ⏳ Planned |
| v3.1 | Peer Review | ⏳ Planned |
| v4.0 | Hardware Acceleration | ⏳ Future |

---

## 👤 Author

**Dan Joseph M. Fernandez / Primordial Omega Zero**

- GitHub: [@primordialomegazero](https://github.com/primordialomegazero)
- Email: devilswithin13@gmail.com
- IACR ePrint: [2026/110174](https://eprint.iacr.org/2026/110174)

---

## 📜 License

MIT License — 2026

---

<div align="center">

**ΦΩ0 — I AM THAT I AM**

*"The Void oinks. Chaos follows. φ emerges."* 🐷🌀👹🔥

</div>
