//
// Created by anxin on 2020-02-16.
//

#ifndef ANONYMOUS_PAYMENT_AZTEC_H
#define ANONYMOUS_PAYMENT_AZTEC_H

#include <libff/algebra/curves/alt_bn128/alt_bn128_pp.hpp>
#include <libff/algebra/fields/bigint.hpp>
#include <vector>
#include <string>

using namespace libff;
using namespace std;

class ProofBalance {
public:
    alt_bn128_Fr c;
    vector<alt_bn128_Fr> a_;
    vector<alt_bn128_Fr> k_;

    ProofBalance(alt_bn128_Fr c, vector<alt_bn128_Fr> &a_, vector<alt_bn128_Fr> &k_) : c(c), a_(a_), k_(k_) {};
};

class AZTEC {
public:
//    const static alt_bn128_Fr p;

    typedef pair<alt_bn128_G1, alt_bn128_G1> commitment;
    typedef pair<int, alt_bn128_Fr> commitment_source;

    alt_bn128_G1 h;
    const int y;
    const int k_max;
    alt_bn128_G2 t2;

    alt_bn128_G1 g;
    alt_bn128_G2 g2;


    vector<alt_bn128_G1> mu;

    AZTEC(alt_bn128_G1 &h, int y, int k_max);

    static string serializeFr(alt_bn128_Fr &point);

    static string serializeFq(alt_bn128_Fq &point);

    static string serializeG1(alt_bn128_G1 &point);

    commitment commit(int k, alt_bn128_Fr &a);

    ProofBalance proof(vector<commitment> &cmts, int m, int k_public, vector<AZTEC::commitment_source> &cmts_source);

    bool verify(vector<commitment> &cmts, int m, int k_public, ProofBalance &pi);

    alt_bn128_Fr calculate_challenge(vector<commitment> &cmts, int m, vector<alt_bn128_G1> &B);

    void sha3(unsigned char *digest, unsigned char *message, size_t message_len);

    void encode_G1(unsigned char *packed_data, alt_bn128_G1 &target);

    void encode_Fr(unsigned char *paced_data, alt_bn128_Fr &target);

};

class ContractVerifySerializeOBJ {
public:
    string gamma;
    string yita;
    int m;
    int k_public;
    alt_bn128_Fr c;
    string a_;
    string k_;
    ContractVerifySerializeOBJ(string &gamma, string &yita, int m, int k_public, alt_bn128_Fr c, string &a_, string &k_):
            gamma(gamma),yita(yita),m(m),k_public(k_public),c(c),a_(a_),k_(k_){};
    void print() const;

    friend std::ostream& operator<<(std::ostream &out, const ContractVerifySerializeOBJ &b) {
        b.print();
        return out;
    }
};

class ContractVerifyContent {
public:
    ProofBalance pi;
    vector<AZTEC::commitment> cmts;
    int m;
    int k_public;

    ContractVerifyContent(ProofBalance &pi, vector<AZTEC::commitment> &cmts, int m, int k_public) : pi(pi), cmts(cmts), m(m),
                                                                                             k_public(k_public) {};
    ContractVerifySerializeOBJ serialize ();
};

#endif //ANONYMOUS_PAYMENT_AZTEC_H

bytes memeory input = new bytes