#pragma once
#include <iostream>
#include <math.h>
#include <stdlib.h>
#include <gmp.h>
using std::string;
using std::cin;
using std::cout;

typedef unsigned int uint;

// C++ wrapper class for GMP's mpz_t
// this class ensure the same performance while making programming much easier
// of course,
class bignum
{
private:
    mpz_t value;

public:
    bignum() {
        mpz_init(value); // default 0
    }

    bignum(int val) {
        mpz_init_set_si(value, val);
    }

    bignum(const bignum &b) {
        mpz_init_set(value, b.value);
    }

    bignum(mpz_t val) {
        mpz_init_set(value, val);
    }

    bignum(const char* str, int base=0) {
        mpz_init_set_str(value, str, base);
    }

    ~bignum() {
        mpz_clear(value);
    }

    //
    explicit operator char() const {return char(mpz_get_si(value));}
    explicit operator int() const {return mpz_get_si(value);}
    explicit operator uint() const {return mpz_get_ui(value);}
    explicit operator double() const {return mpz_get_d(value);}

    /******************    ASSIGNMENT + COMPARISON OPERATORS ************/
    bignum& operator = (int val) {
        mpz_set_si(value, val);
        return (*this);
    }

    bignum& operator = (const bignum& b) {
        if (this!=&b) mpz_set(value, b.value);
        return (*this);
    }

    //Function: int mpz_cmp (const mpz_t op1, const mpz_t op2)
    //Compare op1 and op2. Return a positive value if op1 > op2, zero if op1 = op2, or a negative value if op1 < op2.
    bool operator == (const bignum& b) const {
        int cmp = mpz_cmp(value, b.value);
        return cmp==0;
    }

    bool operator != (const bignum& b) const {
        int cmp = mpz_cmp(value, b.value);
        return cmp!=0;
    }

    bool operator > (const bignum& b) const {
        int cmp = mpz_cmp(value, b.value);
        return cmp > 0;
    }

    bool operator < (const bignum& b) const {
        int cmp = mpz_cmp(value, b.value);
        return cmp < 0;
    }

    bool operator >= (const bignum& b) const {
        int cmp = mpz_cmp(value, b.value);
        return cmp >= 0;
    }

    bool operator <= (const bignum& b) const {
        int cmp = mpz_cmp(value, b.value);
        return cmp <= 0;
    }

    /******************     ARITHMETIC OPERATORS ************/

    //pre
    bignum& operator ++() {
        mpz_add_ui(value, value, 1);
        return (*this);
    }

    //post
    bignum operator ++(int nothing) {
        bignum res(*this);
        mpz_add_ui(value, value, 1);
        return res;
    }

    bignum& operator --() {
        mpz_sub_ui(value, value, 1);
        return (*this);
    }

    bignum operator --(int nothing) {
        bignum res(*this);
        mpz_sub_ui(value, value, 1);
        return res;
    }

    bignum operator - () const {
        bignum res;
        mpz_neg(res.value, value);
        return res;
    }

    bignum operator + (const bignum& b) const {
        bignum res;
        mpz_add(res.value, value, b.value);
        return res;
    }

    bignum& operator += (const bignum &b) {
        mpz_add(value, value, b.value);
        return (*this);
    }

    bignum operator - (const bignum& b) const {
        bignum res;
        mpz_sub(res.value, value, b.value);
        return res;
    }

    bignum& operator -= (const bignum &b) {
        mpz_sub(value, value, b.value);
        return (*this);
    }

    bignum operator * (const bignum& b) const {
        bignum res;
        mpz_mul(res.value, value, b.value);
        return res;
    }

    bignum& operator *= (const bignum &b) {
        mpz_mul(value, value, b.value);
        return (*this);
    }

    bignum operator / (const bignum& b) const {
        bignum res;
        mpz_tdiv_q(res.value, value, b.value);
        return res;
    }

    bignum& operator /= (const bignum &b) {
        mpz_tdiv_q(value, value, b.value);
        return (*this);
    }

    // exact division is much faster
    bignum div(const bignum& b) const {
        bignum res;
        mpz_divexact(res.value, value, b.value);
        return res;
    }

    bignum& divas(const bignum &b) {
        mpz_divexact(value, value, b.value);
        return (*this);
    }

    bignum operator % (const bignum& b) const {
        bignum res;
        mpz_tdiv_r(res.value, value, b.value);
        return res;
    }

    bignum& operator %= (const bignum& b) {
        mpz_tdiv_r(value, value, b.value);
        return (*this);
    }

    /**---------------   INTEGER FUNCTION ON  int       */
    /** Operations between mpz_t and int is much faster than between mpz_t and mpz_t   */
    /**    So we use mpz_t dedicated functions (such as mpz_add_ui) instead of mpz_add */

    bignum operator + (int val) const {
        bignum res;
        if (val >= 0) mpz_add_ui(res.value, value, val);
        else mpz_sub_ui(res.value, value, -val);
        return res;
    }

    bignum& operator += (int val) {
        if (val >= 0) mpz_add_ui(value, value, val);
        else mpz_sub_ui(value, value, -val);
        return (*this);
    }

    bignum operator - (int val) const {
        bignum res;
        if (val >= 0) mpz_sub_ui(res.value, value, val);
        else mpz_add_ui(res.value, value, -val);
        return res;
    }

    bignum& operator -= (int val) {
        if (val >= 0) mpz_sub_ui(value, value, val);
        else mpz_add_ui(value, value, -val);
        return (*this);
    }

    bignum operator * (int val) const {
        bignum res;
        mpz_mul_si(res.value, value, val);
        return res;
    }

    bignum& operator *= (int val) {
        mpz_mul_si(value, value, val);
        return (*this);
    }

    bignum operator / (int val) const {
        bignum res;
        mpz_tdiv_q_ui(res.value, value, abs(val));
        if (val < 0) mpz_neg(res.value, res.value);
        return res;
    }

    bignum& operator /= (int val) {
        mpz_tdiv_q_ui(value, value, abs(val));
        if (val < 0) mpz_neg(value, value);
        return (*this);
    }

    // exact division is much faster
    bignum div(int val) const {
        bignum res;
        mpz_divexact_ui(res.value, value, abs(val));
        if (val < 0) mpz_neg(res.value, res.value);
        return res;
    }

    bignum& divas(int val) {
        mpz_divexact_ui(value, value, abs(val));
        if (val < 0) mpz_neg(value, value);
        return (*this);
    }

    bignum operator % (int val) const {
        bignum res;
        mpz_tdiv_r_ui(res.value, value, abs(val)); // in C++14, x % m = x % abs(m)
        return res;
    }

    bignum& operator %= (int val) {
        mpz_tdiv_r_ui(value, value, abs(val));
        return (*this);
    }

    /******************     utility ************/
    size_t getSize(int base) {
        size_t bit_size = mpz_sizeinbase(value, 2);
        if (base==2) return bit_size;
        else return ceil(bit_size / (log(base) / log(2)));
    }

    string toString(int base) {
        int nbits = getSize(base);
        char *buffer = new char[nbits+2];
        mpz_get_str(buffer, base, value);
        string res = string(buffer);
        delete buffer;
        return res;
    }

    const mpz_t* const valuePtr() const {
        return &value;
    }

    /******************     FRIEND OPERATORS ************/
    friend bool operator == (int a, const bignum &b);
    friend bool operator != (int a, const bignum &b);
    friend bool operator < (int a, const bignum &b);
    friend bool operator > (int a, const bignum &b);
    friend bool operator <= (int a, const bignum &b);
    friend bool operator >= (int a, const bignum &b);

    friend bignum operator + (int a, const bignum &b);
    friend bignum operator - (int a, const bignum &b);
    friend bignum operator * (int a, const bignum &b);
    friend bignum operator / (int a, const bignum &b);
    friend bignum operator % (int a, const bignum &b);

    /******************     ISTREAM/OSTREAM OPERATORS ************/
    // note: only use with cin/cout
    friend std::ostream & operator << (std::ostream &out, const bignum &b);
    friend std::istream & operator >> (std::istream &in, bignum &b);
};

/******************     ISTREAM/OSTREAM OPERATORS ************/

std::ostream& operator << (std::ostream &out, const bignum& b) {
    size_t nbDigits = mpz_sizeinbase(b.value, 10);
    char *buffer = new char[nbDigits+2];
    gmp_sprintf(buffer, "%Zu", b.value);
    out << buffer;
    delete buffer;
    return out;
}

std::istream & operator >> (std::istream &in, bignum &b) {
    string tmp;
    in >> tmp;
    b = bignum(tmp.c_str(), 0);
    return in;
}

/********************     FRIEND OPERATORS ***************/
bool operator == (int a, const bignum &b) {
    return b == a;
}

bool operator != (int a, const bignum &b) {
    return b != a;
}

bool operator < (int a, const bignum &b) {
    return b > a;
}

bool operator > (int a, const bignum &b) {
    return b < a;
}

bool operator <= (int a, const bignum &b) {
    return b >= a;
}

bool operator >= (int a, const bignum &b) {
    return b <= a;
}

bignum operator + (int a, const bignum &b) {
    return b + a;
}

bignum operator - (int a, const bignum &b) {
    return -b + a;
}

bignum operator * (int a, const bignum &b) {
    return b * a;
}

bignum operator / (int a, const bignum &b) {
    return bignum(a) / b;
}

bignum operator % (int a, const bignum &b) {
    return bignum(a) % b;
}
