//
// Created by anxin on 2020-02-16.
//

#include "aztec.h"
#include <libff/algebra/curves/alt_bn128/alt_bn128_pp.hpp>
#include <stdlib.h>
#include <gmp.h>
#include <stdio.h>
#include <vector>
#include <string.h>
#include <openssl/evp.h>
#include <iomanip>
#include <gmp.h>

using namespace libff;
using namespace std;

//const alt_bn128_Fr AZTEC::p = alt_bn128_modulus_r;

void AZTEC::sha256(unsigned char *digest, unsigned char *message, size_t message_len) {
    unsigned int SHALEN = 32;
    EVP_MD_CTX *mdctx;
    mdctx = EVP_MD_CTX_create();
    EVP_DigestInit_ex(mdctx, EVP_sha256(), NULL);
    EVP_DigestUpdate(mdctx, message, message_len);
    EVP_DigestFinal_ex(mdctx, digest, &SHALEN);
    EVP_MD_CTX_destroy(mdctx);

//    auto result = ethash::keccak256(message, message_len);
//    digest = result.bytes;
}

void AZTEC::encode_G1(unsigned char *packed_data, alt_bn128_G1 &target) {
    alt_bn128_G1 target_copy(target);
    target_copy.to_affine_coordinates();

    int g_size = alt_bn128_q_limbs * sizeof(mp_limb_t) * 2;
    string s_x = serializeG1(target_copy);
    int step = sizeof(unsigned int) * 2;
    for (int i = 0; i < s_x.size() / step; i += 1) {
        //clear x;
        unsigned int x;
        std::stringstream ss;
        ss << std::hex << s_x.substr(i * step, step);
        ss >> x;
//        cout<<x<<" ";
        memcpy(packed_data + i * step / 2, (unsigned char *) (&x), step / 2);
        reverse(packed_data + i * step / 2, packed_data + i * step / 2 + step / 2);
        cout << std::dec;
    }
}

void AZTEC::encode_Fr(unsigned char *packed_data, alt_bn128_Fr &target) {
//    target.char s[64];
//    gmp_sprintf(s, "%Nx", point.as_bigint().data, 4);
}


AZTEC::AZTEC() : g(alt_bn128_G1::G1_one), g2(alt_bn128_G2::G2_one) {

    alt_bn128_Fq h_x(
            bigint<alt_bn128_q_limbs>("14324065101854746342085664436165942347072198337701294035289149726922309145121"));
    alt_bn128_Fq h_y(
            bigint<alt_bn128_q_limbs>("5565878794651397487034759341269177878528156373705070183913823735921311218924"));
    alt_bn128_Fq h_z(
            bigint<alt_bn128_q_limbs>("1273135230656633615739165012850001360241518959027186091691264253859941821107"));
    h = alt_bn128_G1(h_x, h_y, h_z);
    y = 1 << 26;
    k_max = (1 << 12) - 1;
    mu = vector<alt_bn128_G1>(k_max + 1);

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
    int n = cmts.size();
    unsigned char digest[32];
    int size_of_G1 = alt_bn128_q_limbs * sizeof(mp_limb_t) * 2;
    int message_size = size_of_G1 * 2 * n + 32 + size_of_G1 * n;
//    cout << "message size: " << message_size << endl;
    unsigned char *message = new unsigned char[message_size];

    int start = 0; // record memcpy location
    // hash commitments
    for (int i = 0; i < cmts.size(); ++i) {
        alt_bn128_G1 gamma = cmts[i].first;
        encode_G1(message + start, gamma);
        alt_bn128_G1 yita = cmts[i].second;
        encode_G1(message + start + size_of_G1, yita);
        start += 2 * size_of_G1;
    }

    string padded(32 - sizeof(m), 0);
    const char *padded_cstr = padded.c_str();
    memcpy(message + start, (unsigned char *) padded_cstr, 32 - sizeof(m));
    start += 32 - sizeof(m);

    memcpy(message + start, (unsigned char *) &m, sizeof(m));
    reverse(message + start, message + start + sizeof(m));
    start += sizeof(m);

    // hash B
    for (int i = 0; i < n; ++i) {
        encode_G1(message + start, B[i]);
        start += size_of_G1;
    }


    sha256(digest, message, message_size);
    string challenge;
    for (int i = 0; i < 32; i++) {
        std::stringstream ss;
        ss << setfill('0') << setw(2) << std::hex << int(digest[i]);
        string result(ss.str());
        challenge += result;
    }
    cout << std::dec;

    mpz_t c_mpz;
    mpz_init_set_str(c_mpz, challenge.c_str(), 16);
    alt_bn128_Fr c = alt_bn128_Fr(c_mpz);
    delete[]message;
    return c;
}

Proof AZTEC::proof(vector<commitment> &cmts, int m, int k_public, vector<AZTEC::commitment_source> &cmts_source) {
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
        //use non-random for debug
        ba[i] = alt_bn128_Fr::random_element();
//        ba[i] = bigint<alt_bn128_r_limbs>(i * 1024 + 212);
        if (i == 0) continue;
        bk[i] = alt_bn128_Fr::random_element();
//        bk[i] = bigint<alt_bn128_r_limbs>(i * 1025 + 3214);
    }
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

    alt_bn128_Fr c = calculate_challenge(cmts, m, B);
    // calculate k1_ ... kn_ and a1_ ... an_
    vector<alt_bn128_Fr> k_(n - 1);
    vector<alt_bn128_Fr> a_(n);

    for (int i = 0; i < n; ++i) {
        alt_bn128_Fr &ai = cmts_source[i].second;
        a_[i] = c * ai + ba[i];
        if (i == 0) continue;
        int ki = cmts_source[i].first;
        k_[i - 1] = c * alt_bn128_Fr(ki) + bk[i];
    }


    return Proof(c, a_, k_);
}

bool AZTEC::__verify(vector<commitment> &cmts, int m, int k_public, Proof &pi, bool move_out) {
    //check jointsplit
    int n = cmts.size();
    //calculate challenge x
    unsigned char digest[32];
    int size_of_G1 = alt_bn128_q_limbs * sizeof(mp_limb_t) * 2;
    int message_size = (n - m) * size_of_G1 * 2;
    unsigned char *message = new unsigned char[message_size];
    int start = 0; // record memcpy location
    for (int i = m; i < n; i++) {
        alt_bn128_G1 gamma = cmts[i].first;
        encode_G1(message + start, gamma);
        alt_bn128_G1 yita = cmts[i].second;
        encode_G1(message + start + size_of_G1, yita);
        start += 2 * size_of_G1;
    }
    sha256(digest, message, message_size);
    string challenge;
    for (int i = 0; i < 32; i++) {
        std::stringstream ss;
        ss << setfill('0') << setw(2) << std::hex << int(digest[i]);
        string result(ss.str());
        challenge += result;
    }
    cout << std::dec;
    mpz_t x_mpz;
    mpz_init_set_str(x_mpz, challenge.c_str(), 16);
    alt_bn128_Fr x = alt_bn128_Fr(x_mpz);
    delete[]message;

    if(move_out) {
        if(m!=n) {  // when m==n no need to check
            // optimized pairing check
            alt_bn128_G1 assemble_gamma = cmts[m].first;
            alt_bn128_G1 assemble_yita = cmts[m].second;
            for (int i = m; i < n; i++) {
                alt_bn128_G1 gamma = cmts[i].first;
                alt_bn128_G1 yita = cmts[i].second;
                alt_bn128_Fr tmp = (x ^ i) * pi.c;
                assemble_gamma = assemble_gamma + tmp * gamma;
                assemble_yita = assemble_yita + tmp * yita;

            }
            // check one pairing
            if (alt_bn128_reduced_pairing(assemble_gamma, t2) != (alt_bn128_reduced_pairing(assemble_yita, g2))) {
                cout << "check pariing fail" << endl;
                return false;
            }

        }
    } else {
        if(m!=n) {  // when m==n no need to check
            // optimized pairing check
            alt_bn128_G1 assemble_gamma = cmts[m].first;
            alt_bn128_G1 assemble_yita = cmts[m].second;
            for (int i = 0; i < m; i++) {
                alt_bn128_G1 gamma = cmts[i].first;
                alt_bn128_G1 yita = cmts[i].second;
                alt_bn128_Fr tmp = (x ^ i) * pi.c;
                assemble_gamma = assemble_gamma + tmp * gamma;
                assemble_yita = assemble_yita + tmp * yita;

            }
            // check one pairing
            if (alt_bn128_reduced_pairing(assemble_gamma, t2) != (alt_bn128_reduced_pairing(assemble_yita, g2))) {
                cout << "check pariing fail" << endl;
                return false;
            }

        }
    }

    // check balance
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
            B[i] = pi.k_[i - 1] * gamma + pi.a_[i] * h + -pi.c * yita;
        }
    }
    alt_bn128_Fr c = calculate_challenge(cmts, m, B);
    return c == pi.c;
}

string AZTEC::serializeFr(alt_bn128_Fr &point) {
    char s[65];
    gmp_sprintf(s, "%Nx", point.as_bigint().data, 4);
    int length = strlen(s);
    if (length < 64) {
        return string(64 - length, '0') + string(s);
    }
    return string(s);
}

string AZTEC::serializeFq(alt_bn128_Fq &point) {
    char s[65];
    gmp_sprintf(s, "%Nx", point.as_bigint().data, 4);
    int length = strlen(s);
    if (length < 64) {
        return string(64 - length, '0') + string(s);
    }
    return string(s);
}

string AZTEC::serializeG1(alt_bn128_G1 &point) {
    alt_bn128_G1 p(point);
    p.to_affine_coordinates();
    return serializeFq(p.X) + serializeFq(p.Y);
}


void ContractVerifySerializeOBJ::print() const {
    cout << "gamma:" << "0x" << gamma << endl;
    cout << "yita:" << "0x" << yita << endl;
    cout << "m:" << m << endl;
    cout << "k_public:" << k_public << endl;
    cout << "c:" << c << endl;
    cout << "a_:" << "0x" << a_ << endl;
    cout << "k_:" << "0x" << k_ << endl;
}


ContractVerifySerializeOBJ ContractVerifyContent::serialize() {
    string gamma;
    string yita;
    string a_;
    string k_;
    for (int i = 0; i < cmts.size(); ++i) {
        gamma += AZTEC::serializeG1(cmts[i].first);
        yita += AZTEC::serializeG1(cmts[i].second);
        a_ += AZTEC::serializeFr(pi.a_[i]);
        if (i != cmts.size() - 1) {
            k_ += AZTEC::serializeFr(pi.k_[i]);
        }
    }
    return ContractVerifySerializeOBJ(gamma, yita, m, k_public, pi.c, a_, k_);
}