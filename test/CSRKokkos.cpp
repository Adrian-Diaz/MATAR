#include <stdio.h>
#include <math.h>
#include <matar.h>
#include <limits.h>

    

int main(int argc, char* argv[]){
    
    Kokkos::initialize(); { 
    size_t nnz = 6 ;
    size_t dim1 = 3;
    size_t dim2 = 10;
    CArrayKokkos<size_t> starts(dim1 + 1);
    CArrayKokkos<size_t> columns(nnz);
    CArrayKokkos<int> array(nnz); 
    RUN ({
        for(int i =0; i < 4; i++){
            starts(i) = 2*i;
            for(int j = 0; j < 2; j++){
                columns(2*i + j) = i + j;
                array(2*i + j) = 2*i + j ;
            }
         }
    });

    /*
    |1 2 0 0 0 0 0 0 0 0|
    |0 0 3 4 0 0 0 0 0 0|
    |0 0 0 0 5 6 0 0 0 0|
    */
   
    //auto B = CArrayKokkos<int>(3, 10); 
    //B(0,0) = 1;
    //B(0,1) = 2;
    //B(1,2) = 3;
    //B(1,3) = 4;
    //B(2,4) = 5;
    //B(2,5) = 6;



    
    const std::string s = "hello";   
    auto pre_A = CSRArrayKokkos<int>(starts, array, columns, dim1, dim2, s);
    auto A = pre_A;
    //auto A = CSRArrayKokkos(B, 3, 10);
    int* res = A.pointer();
    auto a_start = A.get_starts();
    int total = 0;
    int loc_total = 0;  
   
    RUN ({
         printf("nnz : %d\n", A.nnz());
    });

    REDUCE_SUM(i, 0, nnz,
                loc_total, {
                    loc_total += res[i];
                    }, total);    
    printf("Sum of nnz from pointer method %d\n", total);
    total = 0;
    REDUCE_SUM(i, 0, nnz,
                loc_total, {
                    loc_total += a_start[i];
                    }, total);    
    printf("Sum of start indices form .get_starts() %d\n", total); 
   total = 0;
    REDUCE_SUM(i, 0, dim1,
               j, 0, dim2,
                loc_total, {
                    loc_total += A(i,j);
                    }, total);    
    printf("Sum of nnz in array notation %d\n", total);
    auto ss = A.begin(0);
    
    } Kokkos::finalize();
    return 0; 

    
}
