//
// Created by anxin on 2020-02-16.
//

#ifndef ANONYMOUS_PAYMENT_AZTEC_H
#define ANONYMOUS_PAYMENT_AZTEC_H

#include <libff/algebra/curves/alt_bn128/alt_bn128_pp.hpp>
#include <libff/algebra/fields/bigint.hpp>
#include <vector>

using namespace libff;
using namespace std;

class Proof {
public:
    alt_bn128_Fr c;
    vector <alt_bn128_Fr> a_;
    vector <alt_bn128_Fr> k_;

    Proof(alt_bn128_Fr c, vector <alt_bn128_Fr> &a_, vector <alt_bn128_Fr> &k_) : c(c), a_(a_), k_(k_) {};


};

class AZTEC {
public:
    const static alt_bn128_G1 g;
    const static alt_bn128_G2 g2;
//    const static alt_bn128_Fr p;

    typedef pair <alt_bn128_G1, alt_bn128_G1> commitment;
    typedef pair<int, alt_bn128_Fr> commitment_source;

    alt_bn128_G1 h;
    const int y;
    const int k_max;
    alt_bn128_G2 t2;
    vector <alt_bn128_G1> mu;

    AZTEC(alt_bn128_G1 &h, int y, int k_max);

    commitment commit(int k, alt_bn128_Fr &a);

    Proof proof(vector <commitment> &cmts, int m,int  k_public, vector <AZTEC::commitment_source> &cmts_source);

    bool verify(vector <commitment> &cmts, int m, int k_public, Proof &pi);

    alt_bn128_Fr calculate_challenge(vector <commitment> &cmts, int m, vector <alt_bn128_G1> &B);

    void sha3(unsigned char *digest, const unsigned char *message, size_t message_len);

    void encode_G1(unsigned char *packed_data, alt_bn128_G1 &target);

};


#endif //ANONYMOUS_PAYMENT_AZTEC_H
