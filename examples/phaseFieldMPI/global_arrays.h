#pragma once
#include "matar.h"

struct GlobalArrays
{
    CArray<double> comp_all;
    DCArrayKokkos<double> comp;
    DCArrayKokkos<double> dfdc;

    GlobalArrays(const std::array<int,3> & nn_all, const std::array<int,3> & nn);
};
