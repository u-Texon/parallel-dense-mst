# parallel-dense-mst



## Compiling

To compile the code use the following instructions:
```
  git clone --recursive https://github.com/u-Texon/parallel-dense-mst
  cd parallel-dense-mst
  cmake -B build -DCMAKE_BUILD_TYPE=Release
  cmake --build build --parallel
```

## Usage

To execute the code use the following instructions:

```sh
  mpirun -np <numProcs> ./build/MST 
```
