
# functions

cnf()
{
	[[ -d build ]] || mkdir build
	(cd build && cmake --fresh -G Ninja "$@" ..)
}

bld()
{
	[[ -d build ]] || cnf
	(cd build && ninja "$@")
}

tst()
{
	[[ -d build ]] || bld
	(cd build && ctest "$@")
}

run()
{
	[[ -d build ]] || bld
	./build/sonos "$@"
}


export CMAKE_CXX_COMPILER_LAUNCHER=ccache
