/**********************************************************************************************
 � 2020. Triad National Security, LLC. All rights reserved.
 This program was produced under U.S. Government contract 89233218CNA000001 for Los Alamos
 National Laboratory (LANL), which is operated by Triad National Security, LLC for the U.S.
 Department of Energy/National Nuclear Security Administration. All rights in the program are
 reserved by Triad National Security, LLC, and the U.S. Department of Energy/National Nuclear
 Security Administration. The Government is granted for itself and others acting on its behalf a
 nonexclusive, paid-up, irrevocable worldwide license in this material to reproduce, prepare
 derivative works, distribute copies to the public, perform publicly and display publicly, and
 to permit others to do so.
 This program is open source under the BSD-3 License.
 Redistribution and use in source and binary forms, with or without modification, are permitted
 provided that the following conditions are met:
 1.  Redistributions of source code must retain the above copyright notice, this list of
 conditions and the following disclaimer.
 2.  Redistributions in binary form must reproduce the above copyright notice, this list of
 conditions and the following disclaimer in the documentation and/or other materials
 provided with the distribution.
 3.  Neither the name of the copyright holder nor the names of its contributors may be used
 to endorse or promote products derived from this software without specific prior
 written permission.
 THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS
 IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
 IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR
 PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR
 CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL,
 EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO,
 PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS;
 OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY,
 WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR
 OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF
 ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 **********************************************************************************************/
#include <stdio.h>
#include <iostream>
#include <matar.h>

using namespace mtr; // matar namespace

// main
int main()
{
    Kokkos::initialize();
    {
    printf("starting test of loop macros \n");

    // Kokkos::View<int *> arr("ARR", 10);
    CArrayKokkos<int> arr(10);
    FOR_ALL(i, 0, 10, {
        arr(i) = i;
    });

    // Kokkos::View<int **> arr_2D("ARR_2D", 10,10);
    CArrayKokkos<int> arr_2D(10, 10);
    FOR_ALL(i, 0, 10,
         j, 0, 10, {
        arr_2D(i, j) = j * 10 + i;
    });

    // Kokkos::View<int ***> arr_3D("ARR_3D", 10,10,10);
    CArrayKokkos<int> arr_3D(10, 10, 10);
    FOR_ALL(i, 0, 10,
            j, 0, 10,
            k, 0, 10, {
        arr_3D(i, j, k) = k * 10 * 10 + j * 10 + i;
    });

    int loc_sum = 0;
    int result  = 0;
    FOR_REDUCE_SUM(i, 0, 10,
                   loc_sum, {
        loc_sum += arr(i) * arr(i);
    }, result);
    printf("1D reduce sum: %i vs. 985960\n", result);

    loc_sum = 0;
    result  = 0;
    FOR_REDUCE_SUM(i, 0, 10,
                   j, 0, 10,
                   loc_sum, {
            loc_sum += arr_2D(i, j) * arr_2D(i, j);
    }, result);

    printf("2D reduce sum: %i vs. 9859600\n", result);

    loc_sum = 0;
    result  = 0;
    FOR_REDUCE_SUM(i, 0, 10,
                   j, 0, 10,
                   k, 0, 10,
                   loc_sum, {
                loc_sum += arr_3D(i, j, k) * arr_3D(i, j, k);
    }, result);

    printf("3D reduce: %i vs. 98596000\n", result);

    result = 0;
    int loc_max = 2000;
    FOR_REDUCE_MAX(i, 0, 10,
                   j, 0, 10,
                   k, 0, 10,
                   loc_max, {
        if (loc_max < arr_3D(i, j, k)) {
            loc_max = arr_3D(i, j, k);
        }
    },result);

    printf("3D reduce MAX %i\n", result);

    // verbose version
    int loc_max_value = 20000;
    int max_value     = 20000;
    Kokkos::parallel_reduce(
        Kokkos::MDRangePolicy<Kokkos::Rank<2>>({ 0, 0 }, { 10, 10 }),
                KOKKOS_LAMBDA(const int i, const int j, int& loc_max_value)
                {
                if (arr_2D(i, j) > loc_max_value) {
                    loc_max_value = arr_2D(i, j);
                }
    },Kokkos::Max<int>(max_value));
    printf("2D reduce MAX kokkos verbose : %i\n", max_value);

    result = 0;
    int loc_min = 2000;
    FOR_REDUCE_MIN(i, 0, 10,
                   j, 0, 10,
                   k, 0, 10,
                   loc_min, {
        if (loc_min > arr_3D(i, j, k)) {
            loc_min = arr_3D(i, j, k);
        }
    },result);

    printf("3D reduce MIN %i\n", result);

    // DO ALL

    FMatrixKokkos<int> matrix1D(10);

    // Initialize matrix2D
    DO_ALL(i, 1, 10, {
        matrix1D(i) = 1;
    }); // end parallel do

    FMatrixKokkos<int> matrix2D(10, 10);

    // Initialize matrix2D
    DO_ALL(j, 1, 10,
        i, 1, 10, {
        matrix2D(i, j) = 1;
    }); // end parallel do

    FMatrixKokkos<int> matrix3D(10, 10, 10);

    // Initialize matrix3D
    DO_ALL(k, 1, 10,
        j, 1, 10,
        i, 1, 10, {
        matrix3D(i, j, k) = 1;
    }); // end parallel do

    // Initialize matrix2D
    DO_ALL(i, 1, 1, {
        matrix1D(1)       = 10;
        matrix2D(1, 1)    = 20;
        matrix3D(1, 1, 1) = 30;

        matrix1D(10)         = -10;
        matrix2D(10, 10)     = -20;
        matrix3D(10, 10, 10) = -30;
    }); // end parallel do

    DO_REDUCE_MAX(i, 1, 10,
                  loc_max, {
        if (loc_max < matrix1D(i)) {
            loc_max = matrix1D(i);
        }
    }, result);

    printf("result max 1D matrix = %i\n", result);

    DO_REDUCE_MAX(j, 1, 10,
                  i, 1, 10,
                  loc_max, {
        if (loc_max < matrix2D(i, j)) {
            loc_max = matrix2D(i, j);
        }
    }, result);
    printf("result max 2D matrix = %i\n", result);

    DO_REDUCE_MAX(k, 1, 10,
                  j, 1, 10,
                  i, 1, 10,
                  loc_max, {
        if (loc_max < matrix3D(i, j, k)) {
            loc_max = matrix3D(i, j, k);
        }
    }, result);
    printf("result max 3D matrix = %i\n", result);

    DO_REDUCE_MIN(i, 1, 10,
                  loc_min, {
        if (loc_min > matrix1D(i)) {
            loc_min = matrix1D(i);
        }
    }, result);
    printf("result min 1D matrix = %i\n", result);

    DO_REDUCE_MIN(j, 1, 10,
                  i, 1, 10,
                  loc_min, {
        if (loc_min > matrix2D(i, j)) {
            loc_min = matrix2D(i, j);
        }
    }, result);
    printf("result min 2D matrix = %i\n", result);

    DO_REDUCE_MIN(k, 1, 10,
                  j, 1, 10,
                  i, 1, 10,
                  loc_min, {
        if (loc_min > matrix3D(i, j, k)) {
            loc_min = matrix3D(i, j, k);
        }
    }, result);

    printf("result min 3D matrix = %i\n", result);

    // testing serial FOR and DO loop macros.  These
    // serial loops work on the host or the device.
    // the serial FOR and DO macros are intended to
    // give the user a simple syntax to replace
    // the for(...){} syntax

    CArray<int> host_array1D(5);
    CArray<int> host_array2D(5, 5);
    CArray<int> host_array3D(2, 2, 2);

    FMatrix<int> host_matrix1D(3);
    FMatrix<int> host_matrix2D(3, 3);
    FMatrix<int> host_matrix3D(3, 3, 3);

    FOR_LOOP(i, 0, 5, {
        host_array1D(i) = i;
    });
    printf("value in host array1D = \n");
    FOR_LOOP(i, 0, 5, {
        printf(" %d \n", host_array1D(i));
    });

    FOR_LOOP(i, 0, 5,
         j, 0, 5, {
        host_array2D(i, j) = i * j;
    });
    printf("value in host array2D = \n");
    FOR_LOOP(i, 0, 5,
         j, 0, 5, {
        printf(" %d ", host_array2D(i, j));
        if (j == 4) {
            printf("\n");
        }
    });

    FOR_LOOP(i, 0, 2,
         j, 0, 2,
         k, 0, 2, {
        host_array3D(i, j, k) = i * j * k;
    });
    printf("value in host array3D = \n");
    FOR_LOOP(i, 0, 2,
         j, 0, 2,
         k, 0, 2, {
        printf(" %d ", host_array3D(i, j, k));
        if (k == 1) {
            printf("\n");
        }
    });

    DO_LOOP(i, 1, 3, {
        host_matrix1D(i) = i;
    });
    printf("value in host matrix1D = \n");
    DO_LOOP(i, 1, 3, {
        printf(" %d \n", host_matrix1D(i));
    });

    DO_LOOP(j, 1, 3,
        i, 1, 3, {
        host_matrix2D(i, j) = i * j;
    });
    printf("value in host matrix2D = \n");
    DO_LOOP(j, 1, 3,
        i, 1, 3, {
        printf(" %d ", host_matrix2D(i, j));
        if (i == 3) {
            printf("\n");
        }
    });

    DO_LOOP(k, 1, 3,
        j, 1, 3,
        i, 1, 3, {
        host_matrix3D(i, j, k) = i * j * k;
    });

    printf("value in host matrix3D = \n");
    DO_LOOP(k, 1, 3,
        j, 1, 3,
        i, 1, 3, {
        printf(" %d ", host_matrix3D(i, j, k));
        if (i == 3) {
            printf("\n");
        }
        if (j == 3 && i == 3) {
            printf("--\n");
        }
    });

    printf("testing for loop increments of 2 = \n");
    FOR_LOOP(i, 0, 6, 2, {
        printf(" %d \n", i);
    });
    printf("-- \n");
    FOR_LOOP(i, 0, 6, 2,
         j, 0, 6, 2, {
        printf(" %d %d \n", i, j);
    });
    printf("-- \n");
    FOR_LOOP(i, 0, 6, 2,
         j, 0, 6, 2,
         k, 0, 6, 2, {
        printf(" %d %d %d \n", i, j, k);
    });

    printf("testing do loop increments of 2 = \n");
    DO_LOOP(i, 1, 6, 2, {
        printf(" %d \n", i);
    });
    printf("-- \n");
    DO_LOOP(i, 1, 6, 2,
        j, 1, 6, 2, {
        printf(" %d %d \n", i, j);
    });
    printf("-- \n");
    DO_LOOP(i, 1, 6, 2,
        j, 1, 6, 2,
        k, 1, 6, 2, {
        printf(" %d %d %d \n", i, j, k);
    });

    printf("done\n");
    }
    Kokkos::finalize();

    return 0;
}
