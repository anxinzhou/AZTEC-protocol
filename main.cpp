//
// Created by anxin on 2020-02-16.
//
#include <libff/algebra/curves/alt_bn128/alt_bn128_pp.hpp>
#include <libff/algebra/fields/bigint.hpp>
#include "aztec.h"
#include <vector>

using namespace std;
using namespace libff;

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
    for(int i=0;i<100;i++) {
        Proof pi = aztec.proof(cmts, m, k_public, cmts_source);
        bool result = aztec.verify(cmts, m, k_public, pi);
        if (result) {
//            printf("verify success %d\n",i);
        } else {
            printf("verify fail %d",i);
            exit(-1);
        }
    }
    cout<<"pass test"<<endl;
    return 0;
}