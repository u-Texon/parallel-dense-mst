# parallel-dense-mst



## Compiling

To compile the code use the following instructions:
```
  git clone https://github.com/u-Texon/parallel-dense-mst
  cd parallel-dense-mst
  mkdir build && cd build
  cmake -DCMAKE_BUILD_TYPE=Release [-DUSE_EXPLICIT_INSTANTIATION=ON] ..
  make
```

## Usage

To execute the code use the following instructions:

```sh
  mpirun -np <numProcs> ./MST 
```
