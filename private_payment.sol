pragma solidity >=0.5.2 <= 0.6.2;

contract AZTEC {

    struct G1Point {
        uint x;
        uint y;
    }

    G1Point public h;
    //DEBUG
    bool public result;
    uint public _challenge;
    uint public _c;
    uint public _k1_;
    G1Point[5] public _B;
    G1Point[5] public _gmmma;
    uint[5] public _a;
    uint[5] public _k;
    uint public length;
    bytes public _hash_bytes;
    //
    constructor () public {
        h = G1Point(17566712222922113045832476702662482910568538497722999586102925295983692901801,
            10502599197598057507557403467597036081442197834105478257107790310862497666839);
    }

    function FrExp2(uint base)  view internal returns (uint) {
        uint r = 21888242871839275222246405745257275088548364400416034343698204186575808495617;
        // bytes32 r_bytes = bytes32(r);
        // bytes32 p_bytes = bytes32(uint(2));
        bytes memory input = abi.encodePacked(bytes32(uint(32)),bytes32(uint(32)),bytes32(uint(32)),bytes32(base),bytes32(uint(2)),bytes32(uint(r)));
        uint[1] memory result;
        uint gas_ = gasleft();
        bool success;
        assembly {
            success := staticcall( sub(gas_,2000), 5,  add(input,0x20), 0xc0, result, 0x20 )
        // Use "invalid" to make gas estimation work
            switch success case 0 { invalid() }
        }
        require(success);
        return result[0];
    }


    function P1() pure internal returns (G1Point memory) {
        return G1Point(1, 2);
    }


    function G1PointNegate(G1Point memory p) pure internal returns (G1Point memory) {
        // The prime q in the base field F_q for G1
        uint q = 21888242871839275222246405745257275088696311157297823662689037894645226208583;
        if (p.x == 0 && p.y == 0)
            return G1Point(0, 0);
        return G1Point(p.x, q - (p.y % q));
    }


    function G1PointAddition(G1Point memory p1, G1Point memory p2) internal returns (G1Point memory r) {
        uint[4] memory input;
        input[0] = p1.x;
        input[1] = p1.y;
        input[2] = p2.x;
        input[3] = p2.y;
        bool success;
        uint gas_ = gasleft();
        assembly {
            success := call(sub(gas_,2000), 6, 0, input, 0x80, r, 0x40)
        // Use "invalid" to make gas estimation work
            switch success case 0 { invalid() }
        }
        require(success);
    }

    function G1PointScaleMul(G1Point memory p, uint s) internal returns (G1Point memory r) {
        uint[3] memory input;
        input[0] = p.x;
        input[1] = p.y;
        input[2] = s;
        bool success;
        uint gas_ = gasleft();
        assembly {
            success := call(sub(gas_,2000), 7, 0, input, 0x60, r, 0x40)
        // Use "invalid" to make gas estimation work
            switch success case 0 { invalid() }
        }
        require (success);
    }

    function FrAddition(uint x, uint y) pure internal returns (uint) {
        uint r = 21888242871839275222246405745257275088548364400416034343698204186575808495617;
        return (x+y)%r;
    }

    // use a*b = ((a+b)**2 - (a-b)**2)/4
    function FrMultiply(uint x, uint y)  internal returns(uint) {

        if(x<y) {
            uint tmp;
            tmp = x;
            x = y;
            y=tmp;
        }

        bool cond1 = false;
        bool cond2 = false;
        if((x+y)%2!=0) {
            if(x%2==1) {
                y = y+1;
                cond1=true;
            } else {
                x = x+1;
                cond2=true;
            }
        }

        uint xPlusy = (x+y)/2;
        uint xMinusy = (x-y)/2;
        uint xPlusyPow2 = FrExp2(xPlusy);
        uint xMinusyPow2 = FrExp2(xMinusy);
        uint res = FrAddition(xPlusyPow2, FrNegate(xMinusyPow2));

        if(cond1) {
            y-=1;
            res = FrAddition(res,FrNegate(x));

        } else if(cond2) {
            x-=1;
            res = FrAddition(res,FrNegate(y));

        }

        return res;
    }

    function FrNegate(uint x) pure internal returns (uint) {
        uint r = 21888242871839275222246405745257275088548364400416034343698204186575808495617;
        return r-x;
    }

    function load32bytes(uint256 addr) pure internal returns (uint){
        uint x;
        assembly {
            x:= mload(addr)
        }
        return x;
    }

    function parseG1(bytes memory data) pure internal returns (G1Point [] memory){
        uint n = data.length / 64;
        G1Point [] memory g1 = new G1Point[](n);
        uint start = 0x20;
        for(uint i=0;i<n;i++) {
            // parse gamma
            uint hpointer;
            assembly {
                hpointer := add(data,start)
            }
            g1[i].x = load32bytes(hpointer);
            hpointer += 0x20;
            g1[i].y = load32bytes(hpointer);
            start += 0x40;
        }
        return g1;
    }

    function parseFr(bytes memory data) pure internal returns(uint []memory) {
        uint n = data.length / 32;
        uint [] memory fr = new uint[](n);
        uint start = 0x20;
        for(uint i=0;i<n;i++) {
            // parse gamma
            uint hpointer;
            assembly {
                hpointer := add(data,start)
            }
            fr[i] = load32bytes(hpointer);
            start += 0x20;
        }
        return fr;
    }

    function __verify(G1Point [] memory gamma, G1Point []memory yita, uint m, uint k_public, uint c, uint [] memory a_, uint [] memory k_, uint n)  internal returns (bool) {
        // calculate k1;
        uint k1_;
        if (m==0) {
            for(uint i=0;i<n-1;i++) {
                k1_= FrAddition( k1_, FrNegate(k_[i]));
            }
            k1_ = FrAddition(k1_, FrNegate(FrMultiply(k_public,c)) );
        } else {
            for(uint i=m-1;i<n-1;i++) {
                k1_ = FrAddition(k1_, k_[i]);
            }
            for(uint i=0; i<m-1;i++) {
                k1_ = FrAddition(k1_, FrNegate(k_[i]));
            }
            k1_ = FrAddition(k1_, FrMultiply(k_public, c));
        }
        _k1_ = k1_;

        G1Point [] memory B = new G1Point[](n);
        for(uint i=0;i<n;i++) {
            G1Point memory a_mul_h = G1PointScaleMul(h, a_[i]);
            G1Point memory c_mul_yita = G1PointScaleMul(yita[i],FrNegate(c));
            if(i==0) {
                G1Point memory k1_mul_gamma = G1PointScaleMul(gamma[i],k1_);
                B[i] = G1PointAddition(G1PointAddition(k1_mul_gamma,a_mul_h), c_mul_yita);
                // B[i] = tmp;
            } else {
                G1Point memory k_mul_gamma = G1PointScaleMul(gamma[i],k_[i-1]);
                B[i] = G1PointAddition(G1PointAddition(k_mul_gamma,a_mul_h), c_mul_yita);
            }
        }
        // //DEBUG
        for(uint i=0;i<5;i++) {
            _B[i] = G1Point(B[i].x,B[i].y);

            // = G1Point(B[i].x,B[i].y);
        }
        //DEBUG
        uint challenge =  __calculate_challenge(gamma, yita, m, B);
        _challenge  =challenge;
        _c = c;
        //
        return challenge == c;
        // return true;

    }

    // function __calculate_challenge(G1Point[] memory gamma, G1Point []memory yita, uint m, G1Point []memory B) pure internal returns (uint) {
    //     bytes memory packed_bytes;
    //     uint n = gamma.length;
    //     for(uint i=0;i<n;i++) {
    //         packed_bytes = abi.encodePacked(packed_bytes, bytes32(gamma[i].x), bytes32(gamma[i].y), bytes32(yita[i].x), bytes32(yita[i].y));
    //     }

    //     packed_bytes = abi.encodePacked(packed_bytes, bytes32(m));

    //     for(uint i=0;i<n;i++) {
    //         packed_bytes = abi.encodePacked(packed_bytes, bytes32(B[i].x), bytes32(B[i].y));
    //     }
    //     return uint(keccak256(packed_bytes));
    // }

    //  function __calculate_challenge(G1Point[] memory gamma, G1Point []memory yita, uint m, G1Point []memory B)  internal returns (uint) {
    //      bytes memory packed_bytes;
    //      uint gamma_length = 64*gamma.length;
    //      uint start = 0x20;
    //      uint gas_ = gasleft();
    //      bool success;
    //      assembly {
    //         success := call(sub(gas_,2000), 4, 0, add(gamma,0x20),gamma_length , add(packed_bytes,start), gamma_length)
    //         // Use "invalid" to make gas estimation work
    //         switch success case 0 { invalid() }
    //     }
    //     start+=gamma_length;
    //     uint yita_length = 64*gamma_length;
    //       assembly {
    //         success := call(sub(gas_,2000), 4, 0, add(yita,0x20), yita_length , add(packed_bytes,start), yita_length)
    //         // Use "invalid" to make gas estimation work
    //         switch success case 0 { invalid() }
    //     }
    //     start+=yita_length;
    //      assembly {
    //         mstore(add(packed_bytes,start),m)
    //     }
    //     start+=0x20;
    //     uint B_length = 64*B.length;
    //       assembly {
    //         success := call(sub(gas_,2000), 4, 0, add(B,0x20), B_length , add(packed_bytes,start), B_length)
    //         // Use "invalid" to make gas estimation work
    //         switch success case 0 { invalid() }
    //     }
    //     // return uint(keccak256(abi.encodePacked(packed_bytes,B)));
    //     return uint(keccak256(packed_bytes));
    //  }



    function __calculate_challenge(G1Point[] memory gamma, G1Point []memory yita, uint m, G1Point []memory B)  internal returns (uint) {
        uint challenge_size = 64*(gamma.length+yita.length+B.length) + 32;
        bytes memory packed_bytes = new bytes(challenge_size);
        uint n = gamma.length;
        uint start = 0x20;
        uint x;
        uint y;
        for(uint i=0;i<n;i++) {
            x = gamma[i].x;
            assembly {
                mstore(add(packed_bytes,start), x)
            }

            y = gamma[i].y;
            assembly {
                mstore(add(packed_bytes,add(start,0x20)),y)
            }
            x = yita[i].x;
            assembly {
                mstore(add(packed_bytes,add(start,0x40)),x)
            }
            y = yita[i].y;
            assembly {
                mstore(add(packed_bytes,add(start,0x60)),y)
            }
            start +=0x80;
        }

        assembly {
            mstore(add(packed_bytes,start),m)

        }
        start+=0x20;

        for(uint i=0;i<n;i++) {
            x= B[i].x;
            assembly {
                mstore(add(packed_bytes,start),x)
            }
            y= B[i].y;
            assembly {
                mstore(add(packed_bytes,add(start,0x20)),y)
            }
            start+=0x40;
        }

        bool success;
        uint gas_ = gasleft();
        uint[1]memory output;
        assembly {
            success := call(sub(gas_,2000), 2, 0, add(packed_bytes,0x20), challenge_size, output, 0x20)
        // Use "invalid" to make gas estimation work
            switch success case 0 { invalid() }
        }
        _hash_bytes = packed_bytes;

        uint r = 21888242871839275222246405745257275088548364400416034343698204186575808495617;
        return output[0]%r;
    }



    function verify (bytes memory gamma_byte, bytes memory yita_byte, uint m, uint k_public, uint c, bytes memory a_bytes, bytes memory k_bytes, uint n) public  {
        require(gamma_byte.length == n *64, "gamma length not qualify");
        require(yita_byte.length == n * 64 , "yita length not qualify");
        require(a_bytes.length == n*32, "a_ length not qualify");
        require(k_bytes.length == (n-1)*32, "k_ length not qualify");
        G1Point []memory gamma = parseG1(gamma_byte);
        //

        G1Point []memory yita =   parseG1(yita_byte);
        uint [] memory a_ = parseFr(a_bytes);
        uint [] memory k_ = parseFr(k_bytes);
        result = __verify(gamma, yita, m, k_public, c, a_, k_, n);

        //debug
        //  for(uint i=0;i<5;i++) {
        //     _a[i] = a_[i];
        //     if(i!=4) {_k[i] = k_[i];}
        // }

    }

    function wrapper_test() public {
        G1Point memory x = G1Point(1,2);
        G1Point memory y = G1Point(1,2);
        h =test_G1PointAddition(x,y);

        // h.x = t.x;
        // h.y = t.y;
        // h= t;
    }

    function test_G1PointAddition(G1Point memory p1, G1Point memory p2) internal  returns (G1Point memory r) {
        uint[4] memory input;
        uint[2] memory output;
        input[0] = p1.x;
        input[1] = p1.y;
        input[2] = p2.x;
        input[3] = p2.y;
        bool success;
        uint gas_ = gasleft();
        assembly {
            success := call(sub(gas_,2000), 6, 0, input, 0x80, output, 0x40)
        // Use "invalid" to make gas estimation work
            switch success case 0 { invalid() }
        }
        r.x = output[0];
        r.y =output[1];
        require(success);
    }

    function wrapper_test2() public {
        G1Point memory x = G1Point(17566712222922113045832476702662482910568538497722999586102925295983692901801,10502599197598057507557403467597036081442197834105478257107790310862497666839);
        h =test_G1PointScaleMul(x,3);

    }

    function test_G1PointScaleMul(G1Point memory p, uint s) internal returns (G1Point memory r) {
        uint[3] memory input;
        input[0] = p.x;
        input[1] = p.y;
        input[2] = s;
        bool success;
        uint gas_ = gasleft();
        assembly {
            success := call(sub(gas_,2000), 7, 0, input, 0x60, r, 0x40)
        // Use "invalid" to make gas estimation work
            switch success case 0 { invalid() }
        }
        require (success);
    }

    function test_sha256() public {
        bool success;
        uint gas_ = gasleft();
        uint [1]memory input;
        input[0] = 1;
        uint[1]memory output;
        assembly {
            success := call(sub(gas_,2000), 2, 0, input, 0x20, output, 0x20)
        // Use "invalid" to make gas estimation work
            switch success case 0 { invalid() }
        }
        _challenge = output[0];
    }

    function test_sha256_2()public {
        bytes memory input = "0x112c8fee9c14da7fbb8b65b68f8dcf1900ca162fdcd35cb088b90c0fc79a88ea11d2495f3f905e3e6028b3495f3437903d51e35033b191e3502fb93f582671e20ae1d67a9399d368c0582094fbee2f80e46a4f82529b31df034b96f6380ab48c14518bcd1a345209a80d99dfd348b3d5c2571921108d2e91020aaa93d534072c2c1dca693477a05c82560b4d84aad5df721544a1522e449af98975790ebd4af923703465ce0e98f6ccf219488a88e79b51ae79833308516564cd221f68889109015ccba50a72929fdad09a5d07f0a44c7babc1f0495c877910397af85ca48392227a74dc0bd4a50e0257801e9c7239eb1ed92afc57a701cd232482eec720da090a242749efda25ac9bf4643b357789998a8681bf030e06aa1810c9bdbb10866d14bb15543e4e72fb5b0a4077bb62f01654cdac1c104eaa3f7b2e8c80ae43a20b18c10c44f15df1facb51fe5cb815c0cf0711f7b7de4e883b04933ada8311f2152eb37b7dbaa6f61850adada07685f9e78f1f0abfe42436b60dee67094c8cc65912a7b4f4199d8888ec11dc5fb9db772a526113f70c4d1b7e8bce3cf6315f2fe7203e19ce77b5783bd121902dfd66ee786dc666166ea31e4322c9d0cee43fdd1a1e078f10edce10a351b045e5a0b6f650bfe5a4d6a30d35dd6f22e64efc5bfef40f4a1689648ac60ce0984e8913514274b63f4d05b071d9f376d13b283e3c8eba144e217e617a43d30bc70a0c74a5be0e3ebb773d4dfec2d239de6b6bae2fdd3e20aba6bfb9da9ab486a92cb43648cd0eaa99bc35042bba73d16f7698b78e70480b5164b6ff70aa2732a60c1d6e0e81eba0d449c45940addbb2d086255c8acffb1afb67ca006c0197b313673af78dce38fb6ec42b2b2cb2ddbbc979ed26892e26000000000000000000000000000000000000000000000000000000000000000222d470d4e0e1149ae69dde5359b63834c46db37a7d8dabe1155112a48c66089822f21fff6513f0212fc966093a1812d1ab288b090ad2184f6c2b3057a9bd7e701e504b1bcc3adc05cfbf5f0abcebb37bb775a9c369bddabe6837aaac64d83eea0744e14acb1c2b8e87fd812e28d3779eece006ae3e9a970b2c32301ccdcd4e9f1d3505e84f22cecc1c601a30d6615650caac0f0f57c51edb0f33c2363e0db39f0794453da785b1f64a9dbdef900ff63a9489fedf0e354dd6f47d299eef4afb1c063aeb4e805764bab1c3bd7c372371af340239acbf4ccf8c759b1ab4bdf9f5e30b2e1a6f08b69b6b755fa2fd1c1a135352ade287609450ed9b449aa91ff092951e21332644ec17a8aaccb2d0f49c0c560cb3c860b2ae8d01e043dba7187eacc31644777ecbc556dda81b27a0005ac88df457040023a2cd491fae88e429981a5c";
        uint[1]memory output;
        bool success;
        uint gas_ = gasleft();
        assembly {
            success := call(sub(gas_,2000), 2, 0, add(input,0x20), 0x3e0, output, 0x20)
        // Use "invalid" to make gas estimation work
            switch success case 0 { invalid() }
        }
        _challenge = output[0];
    }

}