//
// Created by anxin on 2020-02-16.
//
#include <libff/algebra/curves/alt_bn128/alt_bn128_pp.hpp>
#include <libff/algebra/fields/bigint.hpp>
#include "aztec.h"
#include <vector>

using namespace std;
using namespace libff;
//template<typename ppT>
//void pairing_test()
//{
//    GT<ppT> GT_one = GT<ppT>::one();
//
//    printf("Running bilinearity tests:\n");
//    G1<ppT> P = (Fr<ppT>::random_element()) * G1<ppT>::one();
//    //G1<ppT> P = Fr<ppT>("2") * G1<ppT>::one();
//    G2<ppT> Q = (Fr<ppT>::random_element()) * G2<ppT>::one();
//    //G2<ppT> Q = Fr<ppT>("3") * G2<ppT>::one();
//
//    printf("P:\n");
//    P.print();
//    P.print_coordinates();
//    printf("Q:\n");
//    Q.print();
//    Q.print_coordinates();
//    printf("\n\n");
//
//    Fr<ppT> s = Fr<ppT>::random_element();
//    s.print();
//    //Fr<ppT> s = Fr<ppT>("2");
//    G1<ppT> sP = s * P;
//    G2<ppT> sQ = s * Q;
//
//    printf("Pairing bilinearity tests (three must match):\n");
//    GT<ppT> ans1 = ppT::reduced_pairing(sP, Q);
//    GT<ppT> ans2 = ppT::reduced_pairing(P, sQ);
//    GT<ppT> ans3 = ppT::reduced_pairing(P, Q)^s;
//    ans1.print();
//    ans2.print();
//    ans3.print();
//    assert(ans1 == ans2);
//    assert(ans2 == ans3);
//
//    assert(ans1 != GT_one);
//    assert((ans1^Fr<ppT>::field_char()) == GT_one);
//    printf("\n\n");
//}

int main() {
    alt_bn128_pp::init_public_params();
    alt_bn128_G1 h = alt_bn128_G1::random_element();
    int y = 1<<13;
    int k_max = (1<<12) - 1;
    AZTEC aztec(h,y,k_max);
    // test
    int k_public = 5;
    int m = 2;
    int n = 5;
    vector<int> value  {7,5,1,2,4};
    vector<AZTEC::commitment> cmts(n);
    vector<alt_bn128_Fr> randomness(n);
    for(int i=0; i<n; i++) {
        randomness[i] = alt_bn128_Fr::random_element();
    }
    vector<AZTEC::commitment_source> cmts_source(n);
    for(int i=0; i<n; i++) {
        int k = value[i];
        alt_bn128_Fr & a = randomness[i];
        cmts_source[i] = AZTEC::commitment_source(k,a);
        cmts[i] = aztec.commit(k,a);
    }
    printf("generate proof\n");
    Proof pi = aztec.proof(cmts, m, k_public, cmts_source);
    printf("verify proof\n");
    bool result = aztec.verify(cmts, m, k_public, pi);
    if (result) {
        printf("verify success");
    } else {
        printf("verify fail");
    }
    return 0;
}