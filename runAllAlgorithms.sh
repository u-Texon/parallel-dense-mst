i=1
until [ $i -gt 64 ]
do
  mpirun -np $i ./build/MST algo=filter
  mpirun -np $i ./build/MST algo=kruskal
  mpirun -np $i ./build/MST algo=boruvka
  mpirun -np $i ./build/MST algo=merge
  mpirun -np $i ./build/MST algo=mixedMerge
  mpirun -np $i ./build/MST algo=boruvkaMerge
  ((i=i*2))
done