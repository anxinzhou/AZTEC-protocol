//
// Created by anxin on 2020-02-16.
//

#include "aztec.h"
#include <libff/algebra/curves/alt_bn128/alt_bn128_pp.hpp>
#include <stdlib.h>
#include <gmp.h>
#include <vector>
#include <openssl/evp.h>

using namespace libff;
using namespace std;

const alt_bn128_G1 AZTEC::g = alt_bn128_G1::G1_one;
const alt_bn128_G2 AZTEC::g2 = alt_bn128_G2::G2_one;
//const alt_bn128_Fr AZTEC::p = alt_bn128_modulus_r;

void AZTEC::sha3(unsigned char *digest, const unsigned char *message, size_t message_len) {
    unsigned int SHALEN = 32;
    EVP_MD_CTX *mdctx;
    mdctx = EVP_MD_CTX_create();
    EVP_DigestInit_ex(mdctx, EVP_sha3_256(), NULL);
    EVP_DigestUpdate(mdctx, message, message_len);
    EVP_DigestFinal_ex(mdctx, digest, &SHALEN);
    EVP_MD_CTX_destroy(mdctx);
}

void AZTEC::encode_G1(unsigned char *packed_data, alt_bn128_G1 &target) {
    bigint<alt_bn128_q_limbs> x = target.X.as_bigint();
    bigint<alt_bn128_q_limbs> y = target.Y.as_bigint();
    bigint<alt_bn128_q_limbs> z = target.Z.as_bigint();
    int q_size = alt_bn128_q_limbs * sizeof(mp_limb_t);
    memcpy(packed_data, (unsigned char *) (x.data), q_size);
    memcpy(packed_data + q_size, (unsigned char *) (y.data), q_size);
    memcpy(packed_data + 2 * q_size, (unsigned char *) (z.data), q_size);
}

AZTEC::AZTEC(alt_bn128_G1 &h, int y, int k_max) : h(h), y(y), k_max(k_max), mu(vector<alt_bn128_G1>(k_max + 1)) {
    if (y <= k_max) {
        printf("y should be larger than k_max");
        exit(-1);
    }

    //generate mu
    for (int i = 0; i <= k_max; i++) {
        alt_bn128_Fr tmp(y - i);
        tmp = tmp.inverse();
        mu[i] = tmp * h;
    }

    //set t2;

    t2 = bigint<alt_bn128_r_limbs>(y) * g2;
}


AZTEC::commitment AZTEC::commit(int k, alt_bn128_Fr &a) {
    alt_bn128_G1 gamma = a * mu[k];
    alt_bn128_G1 sigma = (alt_bn128_Fr(k) * a) * mu[k] + a * h;
    return commitment(gamma, sigma);
}

alt_bn128_Fr AZTEC::calculate_challenge(vector<commitment> &cmts, int m, vector<alt_bn128_G1> &B) {
    // calculate challenge
    unsigned char digest[32];
    int n = cmts.size();
    int size_of_G1 = alt_bn128_q_limbs * sizeof(mp_limb_t) * 3;
    int message_size = size_of_G1 * 2 * n + sizeof(m) + size_of_G1 * n;
    unsigned char *message = new unsigned char[message_size];
    int start = 0; // record memcpy location
    // fill message
    unsigned char *g1_packed_data = new unsigned char[size_of_G1];
    // hash commitments
    printf("hash commitments\n");
    for (int i = 0; i < cmts.size(); ++i) {
        alt_bn128_G1 gamma = cmts[i].first;
        alt_bn128_G1 yita = cmts[i].second;
        encode_G1(g1_packed_data, gamma);
        memcpy(g1_packed_data, message + start,  size_of_G1);
        encode_G1(g1_packed_data, yita);
        memcpy(g1_packed_data, message + start + size_of_G1,  size_of_G1);
        start = i * 2 * size_of_G1;
    }
    // hash m
    printf("hash m\n");
    memcpy(message + start, (unsigned char *) &m, sizeof(m));
    start += sizeof(m);
    // hash B
    printf("hash b_\n");
    for (int i = 0; i < n; ++i) {
        encode_G1(g1_packed_data, B[i]);
        memcpy(g1_packed_data, message + start,  size_of_G1);
        start += size_of_G1;
    }
    printf("calculate hash\n");
    sha3(digest, message, message_size);
    bigint<alt_bn128_r_limbs> tmp_c;

    for(int i=0; i<alt_bn128_r_limbs; ++i) {
        unsigned long int value = 0;
        memcpy((unsigned char*)&value, digest+i*sizeof(unsigned long int), sizeof(unsigned long int));
        tmp_c.data[i] = value;
    }

    alt_bn128_Fr c = alt_bn128_Fr(tmp_c);
    delete[]g1_packed_data;
    delete[]message;
    return c;
}

Proof AZTEC::proof(vector<commitment> &cmts, int m,int  k_public, vector<AZTEC::commitment_source> &cmts_source) {
    // check validity of R balance
    int n = cmts.size();
    if (m > n || n == 0) {
        printf("invalid m or n");
        exit(-1);
    }

    if (cmts.size() != cmts_source.size()) {
        printf("size shoudl be equal");
        exit(-1);
    }

    int input = k_public;
    for (int i = m; i < cmts_source.size(); i++) {
        input += cmts_source[i].first;
    }
    int output = 0;
    for (int i = 0; i < m; i++) {
        output += cmts_source[i].first;
    }
    if (input != output) {
        printf("input should be equal to output");
    }
    // pick ba1,...ban and bk2.... bkn
    vector<alt_bn128_Fr> ba(n);
    vector<alt_bn128_Fr> bk(n);

    for (int i = 0; i < n; ++i) {
        ba[i] = alt_bn128_Fr::random_element();
        if (i == 0) continue;
        bk[i] = alt_bn128_Fr::random_element();
    }
    printf("calculate bk1\n");
    // calculate bk1
    alt_bn128_Fr left_part(0);
    alt_bn128_Fr right_part(0);
    for (int i = m; i < n; i++) {
        left_part += bk[i];
    }
    for (int i = 1; i < m; i++) {
        right_part += bk[i];
    }

    bk[0] = left_part - right_part;
    // calculate B1 ... Bn
    vector<alt_bn128_G1> B(n);
    for (int i = 0; i < n; ++i) {
        B[i] = bk[i] * cmts[i].first + ba[i] * h;
    }

    printf("calculate challenge\n");
    alt_bn128_Fr c = calculate_challenge(cmts, m, B);
    // calculate k1_ ... kn_ and a1_ ... an_
    printf("calculate k_ a_\n");
    vector<alt_bn128_Fr> k_(n - 1);
    vector<alt_bn128_Fr> a_(n);

    for (int i = 0; i < n; ++i) {
        alt_bn128_Fr &ai = cmts_source[i].second;
        a_[i] = c * ai + ba[i];
        if (i == 0) continue;
        int ki = cmts_source[i].first;
        k_[i] = c * alt_bn128_Fr(ki) + bk[i];
    }


    return Proof(c, a_, k_);
}

bool AZTEC::verify(vector<commitment> &cmts, int m, int k_public, Proof &pi) {
    int n = cmts.size();
    if (!(m >= 0 && m <= n)) {
        printf("wrong size of m");
        return 0;
    }
    alt_bn128_Fr k1_(0);
    if (m == 0) {
        for (int i = 0; i < n - 1; i++) {
            k1_ -= pi.k_[i];
        }
        k1_ -= alt_bn128_Fr(k_public) * pi.c;
    } else {
        for (int i = m - 1; i < n - 1; i++) {
            k1_ += pi.k_[i];
        }
        for (int i = 0; i < m - 1; i++) {
            k1_ -= pi.k_[i];
        }
        k1_ += alt_bn128_Fr(k_public) * pi.c;
    }
    vector<alt_bn128_G1> B(n);
    for (int i = 0; i < n; ++i) {
        alt_bn128_G1 &gamma = cmts[i].first;
        alt_bn128_G1 &yita = cmts[i].second;
        if (i == 0) {
            B[i] = k1_ * gamma + pi.a_[i] * h + -pi.c * yita;
        } else {
            B[i] = pi.k_[i] * gamma + pi.a_[i] * h + -pi.c * yita;
        }
    }
    alt_bn128_Fr c = calculate_challenge(cmts, m, B);
    return c == pi.c;
}

