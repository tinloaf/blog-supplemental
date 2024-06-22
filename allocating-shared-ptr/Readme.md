This is the benchmark code for [my blog article on efficiently allocating std::shared_ptr](https://www.lukas-barth.net/blog/emacs-wsl-copy-clipboard/)

Building
==========

This project requires the boost libraries to be installed, and a C++17-capable compiler.

This is a CMake-based project. Building it should be as easy as creating a `build` directory and running `cmake` and `make` in it

```
mkdir build
cd build
cmake -DCMAKE_BUILD_TYPE=Release ..
make
```

If everything went well, this should give you a `benchmark` executable.

If you want to explicitly select the compiler to build with, you should set it before running CMake like this:

```
export CXX=/usr/bin/clang++-17
cmake -DCMAKE_BUILD_TYPE=Release ..
```

Running
=========

In the most simple case, you can just run the `benchmark` binary, which should first print the sizes of the benchmark data structures, and then print how low each scenario took. Note that the first run is a cache-warmup run and should be discarded. As an example:

```
> ./benchmark
Size 4: 80 bytes.
Size 3: 64 bytes.
Size 2: 48 bytes.
Size 1: 32 bytes.
Size 0: 16 bytes.
Iterations for boost::fast_pool_allocator took 34.4578 seconds
Iterations for std::make_shared took 38.6706 seconds
Iterations for boost::fast_pool_allocator took 34.8455 seconds
Iterations for new took 70.1021 seconds
```

So the actual measurements would be `38.6706` seconds for `make_shared`, `34.8455` seconds for `fast_pool_allocator` and `70.1021` seconds for `new`.

If you want to run the benchmarks with `jemalloc` or `tcmalloc`, you should load them using `LD_PRELOAD`. As an example, if your `tcmalloc` library is installed to `/usr/lib/x86_64-linux-gnu/libtcmalloc_minimal.so.4` (which should be the case if you installed `tcmalloc` in Ubuntu 22.04), just run:

```
LD_PRELOAD=/usr/lib/x86_64-linux-gnu/libtcmalloc_minimal.so.4 ./benchmark
```
