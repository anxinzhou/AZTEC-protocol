//
// Created by anxin on 2020-02-16.
//
#include <libff/algebra/curves/alt_bn128/alt_bn128_pp.hpp>
#include <libff/algebra/fields/bigint.hpp>
#include "aztec.h"
#include <vector>
#include <gmp.h>


using namespace std;
using namespace libff;

int main() {
    alt_bn128_pp::init_public_params();
//    alt_bn128_G1 h = alt_bn128_G1::random_element();
// fix h for testing
    alt_bn128_Fq h_x(bigint<alt_bn128_q_limbs>("14324065101854746342085664436165942347072198337701294035289149726922309145121"));
    alt_bn128_Fq h_y(bigint<alt_bn128_q_limbs>("5565878794651397487034759341269177878528156373705070183913823735921311218924"));
    alt_bn128_Fq h_z(bigint<alt_bn128_q_limbs>("1273135230656633615739165012850001360241518959027186091691264253859941821107"));
    alt_bn128_G1 h(h_x,h_y,h_z);
    cout<<"h:"<<endl;
    h.print();
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
//        randomness[i] = alt_bn128_Fr::random_element();
        randomness[i] = alt_bn128_Fr(alt_bn128_modulus_r)-alt_bn128_Fr(bigint<alt_bn128_r_limbs>(i*1024+212));

    }
    vector<AZTEC::commitment_source> cmts_source(n);
    for(int i=0; i<n; i++) {
        int k = value[i];
        alt_bn128_Fr & a = randomness[i];
        cmts_source[i] = AZTEC::commitment_source(k,a);
        cmts[i] = aztec.commit(k,a);
    }


    // test smart contract
//    for(int i=0;i<n;i++) {
//        cout<<"G"<<i<<endl;
//        cmts[i].first.print();
//    }

    for(int i=0;i<1;i++) {
        ProofBalance pi = aztec.proof(cmts, m, k_public, cmts_source);
        ContractVerifyContent content (pi,cmts,m,k_public);
        cout<<(content.serialize())<<endl;

        //test smart contract
//        for(int i=0;i<n-1;i++) {
//            cout<<"a_"<<i<<endl;
//            cout<<pi.k_[i]<<endl;
//        }

        bool result = aztec.verify(cmts, m, k_public, pi);
        if (result) {
//            printf("verify success %d\n",i);
        } else {
            printf("verify fail %d",i);
            exit(-1);
        }
    }

    cout<<"pass test"<<endl;
    cout<< alt_bn128_modulus_r<<endl;

    //test smart contract
    auto c =  alt_bn128_Fr(3) * aztec.h;
    c.print();
    return 0;
}