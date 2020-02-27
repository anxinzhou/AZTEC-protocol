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
    int k_public = 5;
    int m = 2;
    int n = 5;
    vector<int> value{7, 5, 1, 2, 4};
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


    t1 = std::chrono::high_resolution_clock::now();
    for (int i = 0; i < 100; i++) {
        Proof pi = aztec.proof(cmts, m, k_public, cmts_source);
        ContractVerifyContent content(pi, cmts, m, k_public);
//        cout << (content.serialize()) << endl;

        bool result = aztec.verify(cmts, m, k_public, pi);
        if (result) {
//            printf("verify success %d\n",i);
        } else {
            printf("verify fail %d", i);
            exit(-1);
        }
    }
    t2 = std::chrono::high_resolution_clock::now();

    duration = std::chrono::duration_cast<std::chrono::microseconds>(t2 - t1).count();

    std::cout << duration / 1000.0 / 1000 << "s" << endl;


    cout << "pass test" << endl;

    return 0;
}