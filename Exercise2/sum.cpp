#include <mpi.h>
#include <cstdio>
#include <cstdint>

int main(int argc,char** argv) {
 MPI_Init(&argc,&argv); int rank,size; MPI_Comm_rank(MPI_COMM_WORLD,&rank); MPI_Comm_size(MPI_COMM_WORLD,&size);
 const long long n=10000000; long long local=0,total=0;
 MPI_Barrier(MPI_COMM_WORLD); double start=MPI_Wtime();
 // Balanced contiguous ranges; every integer is visited exactly once.
 for(long long i=n*rank/size+1;i<=n*(rank+1)/size;++i) local+=i;
 MPI_Reduce(&local,&total,1,MPI_LONG_LONG_INT,MPI_SUM,0,MPI_COMM_WORLD);
 double elapsed=MPI_Wtime()-start,maximum;
 MPI_Reduce(&elapsed,&maximum,1,MPI_DOUBLE,MPI_MAX,0,MPI_COMM_WORLD);
 int ok=1; if(rank==0) {ok=(total==n*(n+1)/2); printf("sum,%d,%.9f,%lld\n",size,maximum,total);}
 MPI_Bcast(&ok,1,MPI_INT,0,MPI_COMM_WORLD); MPI_Finalize(); return ok?0:1;
}
