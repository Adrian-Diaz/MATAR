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
    const std::string s = "hello";   
    auto pre_A = CSRArrayKokkos<int>(starts, array, columns, dim1, dim2, s);
    auto A = pre_A;
  
    
    int* res = A.pointer();
    auto a_start = A.get_starts();
    int total = 0;
    int loc_total = 0;  
    loc_total += 0; //Get rid of warning 

    RUN ({
        printf("A is %ld x %ld \n", A.dim1(), A.dim2());
        printf("And has %ld non zero elements\n", A.nnz());
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
