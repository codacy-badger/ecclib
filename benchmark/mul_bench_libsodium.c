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
#include <gmp.h>
#include <mpzurandom.h>
#include <sodium.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>

#define BENCH_SZ    (50)

// standard size libsodium vector - privkeys, pubkeys follow this format
typedef unsigned char    svector[32];

int main(int argc, char** argv) {
    int i, j, k;
    int status;
    svector n[BENCH_SZ];
    svector pt[BENCH_SZ];
    svector rpt;
    double cpu_time;
    double mul_rate;
    int64_t start_time, stop_time;

    printf("\"curve\", \"num_iter\", \"time\", \"rate\",\n");

    for (i = 0; i < BENCH_SZ; i++) {
        svector t;
        randombytes_buf(n[i], 32);
        randombytes_buf(t, 32);
        crypto_core_ed25519_from_uniform(pt[i], t);
    }

    start_time = clock();
    for (j = 0; j < BENCH_SZ; j++) {
        for (k = 0; k < BENCH_SZ; k++) {
            status = crypto_scalarmult_ed25519(rpt, n[k], pt[j]);
            assert(status == 0);
        }
    }
    stop_time = clock();

    cpu_time = (double)(stop_time - start_time) / ((double)CLOCKS_PER_SEC);  
    mul_rate = (double)(BENCH_SZ * BENCH_SZ) / cpu_time;
    printf("\"ed25519\", %d, %lf, %lf,\n", (int)(BENCH_SZ*BENCH_SZ), cpu_time, mul_rate);

    return 0;
}
