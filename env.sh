
conda activate $(basename $PWD)

# functions

bld_secp256k1()
{
	[[ -d build_secp256k1 ]] || mkdir build_secp256k1
	(cd build_secp256k1 && \
	cmake --fresh -G Ninja \
		-D CMAKE_BUILD_TYPE=Release \
		-D SECP256K1_ENABLE_MODULE_SCHNORRSIG=ON \
		-D SECP256K1_BUILD_BENCHMARK=OFF \
		-D SECP256K1_BUILD_EXHAUSTIVE_TESTS=OFF \
		-D SECP256K1_BUILD_TESTS=OFF \
		-D SECP256K1_DISABLE_SHARED=ON \
		-D SECP256K1_INSTALL=OFF \
		-D SECP256K1_VALGRIND=OFF \
		../external/secp256k1 \
	&& ninja)
}

cnf()
{
	[[ -d build ]] || mkdir build
	(cd build && cmake --fresh -G Ninja ..)
}

bld()
{
	[[ -d build ]] || cnf
	(cd build && ninja $@)
}

tst()
{
	[[ -d build ]] || bld
	(cd build && ctest $@)
}

run()
{
	[[ -d build ]] || bld
	./build/sonos $@
}


#export CMAKE_PREFIX_PATH=$CMAKE_PREFIX_PATH:~/git/xmr/install
export CMAKE_CXX_COMPILER_LAUNCHER=ccache
