//BSD 3-Clause License
//
//Copyright (c) 2017, jadeblaquiere
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

#include <assert.h>
#include <ecpoint.h>
#include <ecurve.h>
#include <field.h>
#include <gmp.h>
#include <mpzurandom.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>

#define BENCH_SZ    (100)

int main(int argc, char** argv) {
    int i, j, k;
    int status;
    mpz_t n[BENCH_SZ];
    mpECP_t pt[BENCH_SZ];
    mpECP_t rpt;
    double cpu_time;
    double mul_rate;
    mpECurve_t cv;
    char **clist;
    int64_t start_time, stop_time;

    mpECurve_init(cv);
    mpECP_init(rpt);
    for (i = 0; i < BENCH_SZ; i++) {
        mpz_init(n[i]);
        mpECP_init(pt[i]);
    }

    printf("\"curve\", \"num_iter\", \"time\", \"rate\",\n");

    clist = _mpECurve_list_standard_curves();
    i = 0;
    while (clist[i] != NULL) {
        status = mpECurve_set_named(cv, clist[i]);
        assert(status == 0);
        //printf("initializing test vectors\n");
        for (j = 0; j < BENCH_SZ; j++) {
            mpECP_urandom(pt[j], cv);
            mpECP_scalar_base_mul_setup(pt[j]);
            mpz_urandom(n[j], cv->p);
        }

        start_time = clock();
        for (j = 0; j < BENCH_SZ; j++) {
            for (k = 0; k < BENCH_SZ; k++) {
                mpECP_scalar_base_mul_mpz(rpt, pt[j], n[k]);
            }
        }
        stop_time = clock();

        cpu_time = (double)(stop_time - start_time) / ((double)CLOCKS_PER_SEC);  
        mul_rate = (double)(BENCH_SZ * BENCH_SZ) / cpu_time;
        printf("\"%s\", %d, %lf, %lf,\n", clist[i], (int)(BENCH_SZ*BENCH_SZ),cpu_time, mul_rate);

        i += 1;
    }
    return 0;
}
