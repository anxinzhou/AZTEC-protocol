pragma solidity >=0.5.2 <= 0.6.2;

contract AZTEC {

    struct G1Point {
        uint x;
        uint y;
    }

    G1Point public h;

    constructor () public {
        h = G1Point(17566712222922113045832476702662482910568538497722999586102925295983692901801,
            10502599197598057507557403467597036081442197834105478257107790310862497666839);
    }

    function FrExp2(uint base)  internal returns (uint) {
        uint r = 21888242871839275222246405745257275088548364400416034343698204186575808495617;
        // bytes32 r_bytes = bytes32(r);
        // bytes32 p_bytes = bytes32(uint(2));
        bytes memory input = abi.encodePacked(bytes32(uint(32)),bytes32(uint(32)),bytes32(uint(32)),bytes32(base),bytes32(uint(2)),bytes32(uint(r)));
        uint[1] memory result;
        uint gas_ = gasleft();
        bool success;
        assembly {
            success := call( sub(gas_,2000), 5, 0,  add(input,0x20), 0xc0, result, 0x20 )
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

        uint xPlusy = FrAddition(x,y);
        uint xMinusy = FrAddition(x,FrNegate(y));

        uint xPlusyPow2 = FrExp2(xPlusy);

        uint xMinusyPow2 = FrExp2(xMinusy);

        return FrAddition(xPlusyPow2, FrNegate(xMinusyPow2)) / 4;
    }

    function FrNegate(uint x) pure internal returns (uint) {
        uint r = 21888242871839275222246405745257275088548364400416034343698204186575808495617;
        return -x % r;
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

        G1Point [] memory B = new G1Point[](n);
        for(uint i=0;i<n;i++) {
            G1Point memory a_mul_h = G1PointScaleMul(h, a_[i]);
            G1Point memory c_mul_yita = G1PointScaleMul(yita[i],c);
            if(i==0) {
                G1Point memory k1_mul_gamma = G1PointScaleMul(gamma[i],k1_);
                B[i] = G1PointAddition(G1PointAddition(k1_mul_gamma,a_mul_h), G1PointNegate(c_mul_yita));
            } else {
                G1Point memory k_mul_gamma = G1PointScaleMul(gamma[i],k_[i]);
                B[i] = G1PointAddition(G1PointAddition(k_mul_gamma,a_mul_h), G1PointNegate(c_mul_yita));
            }
        }

        uint challenge =  __calculate_challenge(gamma, yita, m, B);
        return challenge == c;

    }

    function __calculate_challenge(G1Point[] memory gamma, G1Point []memory yita, uint m, G1Point []memory B) pure internal returns (uint) {
        bytes memory packed_bytes;
        uint n = gamma.length;
        for(uint i=0;i<n;i++) {
            packed_bytes = abi.encodePacked(packed_bytes, bytes32(gamma[i].x), bytes32(gamma[i].y), bytes32(yita[i].x), bytes32(yita[i].y));
        }

        packed_bytes = abi.encodePacked(packed_bytes, bytes32(m));

        for(uint i=0;i<n;i++) {
            packed_bytes = abi.encodePacked(packed_bytes, bytes32(B[i].x), bytes32(B[i].y));
        }
        return uint(keccak256(packed_bytes));
    }



    function verify (bytes memory gamma_byte, bytes memory yita_byte, uint m, uint k_public, uint c, bytes memory a_bytes, bytes memory k_bytes, uint n) public returns (bool) {
        require(gamma_byte.length == n *64, "gamma length not qualify");
        require(yita_byte.length == n * 64 , "yita length not qualify");
        require(a_bytes.length == n*32, "a_ length not qualify");
        require(k_bytes.length == (n-1)*32, "k_ length not qualify");
        G1Point []memory gamma = parseG1(gamma_byte);
        G1Point []memory yita =   parseG1(yita_byte);
        uint [] memory a_ = parseFr(a_bytes);
        uint [] memory k_ = parseFr(k_bytes);
        return __verify(gamma, yita, m, k_public, c, a_, k_, n);
    }
}