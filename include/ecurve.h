//BSD 3-Clause License
//
//Copyright (c) 2018, jadeblaquiere
//All rights reserved.
//
//Redistribution and use in source and binary forms, with or without
//modification, are permitted provided that the following conditions are met:
//
//* Redistributions of source code must retain the above copyright notice, this
//  list of conditions and the following disclaimer.
//
//* Redistributions in binary form must reproduce the above copyright notice,
//  this list of conditions and the following disclaimer in the documentation
//  and/or other materials provided with the distribution.
//
//* Neither the name of the copyright holder nor the names of its
//  contributors may be used to endorse or promote products derived from
//  this software without specific prior written permission.
//
//THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
//AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
//IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
//DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE LIABLE
//FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL
//DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR
//SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER
//CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY,
//OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE
//OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.

#ifndef _EC_CURVE_H_INCLUDED_
#define _EC_CURVE_H_INCLUDED_

#include <field.h>
#include <gmp.h>

#ifdef __cplusplus
extern "C" {
#endif

// Implementation of Elliptic Curve math following GNU GMP sytle
// internal representation supports projective (e.g. Jacobian) coords

typedef enum {EQTypeNone, EQTypeUninitialized, EQTypeShortWeierstrass, EQTypeEdwards, EQTypeMontgomery, EQTypeTwistedEdwards} _mpECurve_eq_type;

// short Weierstrass curve defined as y**2 = x**3 + ax + b

typedef struct {
    mpFp_t a; // coefficient of equation
    mpFp_t b; // coefficient of equation
} _mpECurve_ws_curve_coeff_t;

// Edwards curve defined as x**2 + y**2 = c**2 * (1 + (d * x**2 * y**2))

typedef struct {
    mpFp_t c; // coefficient of equation
    mpFp_t d; // coefficient of equation
} _mpECurve_ed_curve_coeff_t;

// Montgomery curve defined as B * y**2 = x**3 + A * x**2 + x

typedef struct {
    mpFp_t B; // coefficient of equation
    mpFp_t A; // coefficient of equation
    // internal representation of Montgomery curve points is ws
    // to facilitate add/double (not differential)
    // transform is :
    // x = Bu-A/3, y = Bv
    // ws_a = (3-A^2)/(3B^2) and ws_b = (2A^3-9A)/(27B^3)
    // resulting equation:
    // v^2 = u^3 + ws_a * u + ws_b
    // reverse transform:
    // u = Binv * x + A/3, v = Binv * y
    mpFp_t ws_a; // coefficient of transformed equation
    mpFp_t ws_b; // coefficient of transformed equation
    mpFp_t Binv; // coefficient of transform
    mpFp_t Adiv3; // coefficient of transform
} _mpECurve_mo_curve_coeff_t;

// Twisted Edwards : a * x**2 + y**2 = 1 + (d * x**2 * y**2)

typedef struct {
    mpFp_t a; // coefficient of equation
    mpFp_t d; // coefficient of equation
} _mpECurve_te_curve_coeff_t;

typedef union {
    _mpECurve_ws_curve_coeff_t ws;
    _mpECurve_ed_curve_coeff_t ed;
    _mpECurve_mo_curve_coeff_t mo;
    _mpECurve_te_curve_coeff_t te;
} _mpECurve_coeff_t;

typedef struct {
    _mpECurve_eq_type type; // curve type
    //mpz_t p; // prime field of curve
    mpFp_field_ptr fp;
    _mpECurve_coeff_t coeff; // curve coefficients
    mpz_t n; // order of Generator point of curve
    mpz_t h; // cofactor of curve
    mpz_t G[2]; // x,y coordinates of Generator of EC Group
    unsigned int bits; // bit size of curve, i.e. ceil(log2(p))
} _mpECurve_t;

typedef _mpECurve_t mpECurve_t[1];
typedef _mpECurve_t *mpECurve_ptr;

void mpECurve_init(mpECurve_t c);
void mpECurve_clear(mpECurve_t c);

void mpECurve_set(mpECurve_t rop, mpECurve_t op);

int mpECurve_set_str_ws(mpECurve_t cv, char *p, char *a, char *b, char *n,
                      char *h, char *Gx, char *Gy, unsigned int bits);
int mpECurve_set_str_ed(mpECurve_t cv, char *p, char *c, char *d, char *n,
                      char *h, char *Gx, char *Gy, unsigned int bits);
int mpECurve_set_str_mo(mpECurve_t cv, char *p, char *B, char *A, char *n,
                      char *h, char *Gx, char *Gy, unsigned int bits);
int mpECurve_set_str_te(mpECurve_t cv, char *p, char *a, char *d, char *n,
                      char *h, char *Gx, char *Gy, unsigned int bits);

int mpECurve_set_mpz_ws(mpECurve_t cv, mpz_t p, mpz_t a, mpz_t b, mpz_t n,
                      mpz_t h, mpz_t Gx, mpz_t Gy, unsigned int bits);
int mpECurve_set_mpz_ed(mpECurve_t cv, mpz_t p, mpz_t c, mpz_t d, mpz_t n,
                      mpz_t h, mpz_t Gx, mpz_t Gy, unsigned int bits);
int mpECurve_set_mpz_mo(mpECurve_t cv, mpz_t p, mpz_t B, mpz_t A, mpz_t n,
                      mpz_t h, mpz_t Gx, mpz_t Gy, unsigned int bits);
int mpECurve_set_mpz_te(mpECurve_t cv, mpz_t p, mpz_t a, mpz_t d, mpz_t n,
                      mpz_t h, mpz_t Gx, mpz_t Gy, unsigned int bits);

int mpECurve_set_named(mpECurve_t c, char *name);

int mpECurve_point_check(mpECurve_t c, mpz_t Px, mpz_t Py);

int mpECurve_cmp(mpECurve_t op1, mpECurve_t op2);

/* note, _list_standard_curves allocates space for the list of curves and 
the curve names. This call returns the list, it is the responsibility of the
caller to free the individual names and then the list itself. The curve list
returned is NULL terminated. This is primarily intended for testing*/
char **_mpECurve_list_standard_curves(void);

#ifdef __cplusplus
}
#endif

#endif // _EC_CURVE_H_INCLUDED_
