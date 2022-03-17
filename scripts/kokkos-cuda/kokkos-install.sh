#!/bin/bash -e

rm -rf ${KOKKOS_BUILD_DIR} ${KOKKOS_INSTALL_DIR}
mkdir -p ${KOKKOS_BUILD_DIR}
cd ${KOKKOS_BUILD_DIR}

OPTIONS=(
-D CMAKE_BUILD_TYPE=Release
-D CMAKE_INSTALL_PREFIX="${KOKKOS_INSTALL_DIR}"
-D CMAKE_CXX_STANDARD=17
-D Kokkos_ENABLE_SERIAL=ON
-D Kokkos_ENABLE_CUDA=ON
#-D Kokkos_ARCH_POWER9=ON
-D Kokkos_ARCH_VOLTA70=ON
-D Kokkos_ENABLE_CUDA_CONSTEXPR=ON
-D Kokkos_ENABLE_CUDA_LAMBDA=ON
-D Kokkos_ENABLE_CUDA_RELOCATABLE_DEVICE_CODE=ON
-D Kokkos_ENABLE_TESTS=OFF
-D BUILD_TESTING=OFF
)
cmake "${OPTIONS[@]}" "${KOKKOS_SOURCE_DIR:-../}"
make -j -l32
make install

cd $scriptdir