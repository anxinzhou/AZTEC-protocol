//
// Created by anxin on 2020-02-16.
//
#include <libff/algebra/curves/alt_bn128/alt_bn128_pp.hpp>
#include <libff/algebra/fields/bigint.hpp>
#include "aztec.h"
#include <vector>
#include <chrono>
#include <gmp.h>


using namespace std;
using namespace libff;

int main() {
    alt_bn128_pp::init_public_params();
//
    cout<<"setup time"<<endl;
    auto t1 = std::chrono::high_resolution_clock::now();
    AZTEC aztec;
    auto t2 = std::chrono::high_resolution_clock::now();

    auto duration = std::chrono::duration_cast<std::chrono::microseconds>(t2 - t1).count();

    std::cout << duration / 1000.0 / 1000 << "s" << endl;


    // test
    int k_public = 0;
    int n= 32;
    int m = n;
    vector<int>value;
    for(int i=0;i<n;i++) {
        value.push_back(n-i);
        k_public+= n-i;
    }
    for(int i=m;i<n;i++) {
        k_public -=2*(n-i);
    }
//    vector<int> value{198, 195, 197, 213, 197};
    vector<AZTEC::commitment> cmts(n);
    vector<alt_bn128_Fr> randomness(n);
    for (int i = 0; i < n; i++) {
        randomness[i] = alt_bn128_Fr::random_element();
        // use randomness for debug
//        randomness[i] = alt_bn128_Fr(alt_bn128_modulus_r)-alt_bn128_Fr(bigint<alt_bn128_r_limbs>(i*1024+212));
    }


    vector<AZTEC::commitment_source> cmts_source(n);
    for (int i = 0; i < n; i++) {
        int k = value[i];
        alt_bn128_Fr &a = randomness[i];
        cmts_source[i] = AZTEC::commitment_source(k, a);
        cmts[i] = aztec.commit(k, a);
    }



    for (int i = 0; i < 1; i++) {
        t1 = std::chrono::high_resolution_clock::now();
        Proof pi = aztec.proof(cmts, m, k_public, cmts_source);
        t2 = std::chrono::high_resolution_clock::now();
        duration = std::chrono::duration_cast<std::chrono::microseconds>(t2 - t1).count();

        std::cout << duration / 1000.0 / 1000 << "s" << endl;
        bool result = aztec.verify_move_in(cmts, m, k_public, pi);
        if (result) {
//            printf("verify success %d\n",i);
        } else {
            printf("verify fail %d", i);
            exit(-1);
        }


        ContractVerifyContent content(pi, cmts, m, k_public,n);
        cout << (content.serialize()) << endl;

    }




    cout << "pass test" << endl;

    return 0;
}