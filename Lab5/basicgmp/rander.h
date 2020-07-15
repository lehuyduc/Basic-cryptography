#pragma once
#include <iostream>
#include <math.h>
#include <time.h>
#include <stdlib.h>
#include <gmp.h>
#include <bignum.h>

class Rander
{
private:
    gmp_randstate_t state;

public:
    Rander() {
        gmp_randinit_default(state);
        gmp_randseed_ui(state, 1);
    }

    Rander(unsigned int seed) {
        gmp_randinit_default(state);
        gmp_randseed_ui(state, seed);
    }

    Rander(const Rander &r) {
        gmp_randinit_set(state, r.state);
    }

    ~Rander() {
        gmp_randclear(state);
    }

    Rander& operator = (const Rander& r) {
        if (this!=&r) {
            gmp_randclear(state);
            gmp_randinit_set(state, r.state);
        }
        return (*this);
    }

    //-------
    void initSeed(unsigned int seed) {
        gmp_randseed_ui(state, seed);
    }

    // generate from 0->n-1
    bignum randomm(bignum maxValue) {
        mpz_t res, range;
        mpz_init(res);
        mpz_init_set(range, *maxValue.valuePtr());
        mpz_urandomm(res, state, range);

        bignum ret = bignum(res);
        mpz_clear(res);
        mpz_clear(range);
        return ret;
    }

    // random 0->2^n-1
    bignum randomb(bignum numberBit) {
        mpz_t res;
        mp_bitcnt_t nbit = uint(numberBit);
        mpz_init(res);
        mpz_urandomb(res, state, nbit);

        bignum ret = bignum(res);
        mpz_clear(res);
        return ret;
    }
};
