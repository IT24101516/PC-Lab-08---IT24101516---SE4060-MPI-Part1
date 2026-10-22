#include <mpi.h>
#include <cstdio>
#include <cstdint>

// Stateless SplitMix64: the same sample indices give the same points at every process count.
static uint64_t mix(uint64_t z) {z+=0x9e3779b97f4a7c15ULL;z=(z^(z>>30))*0xbf58476d1ce4e5b9ULL;z=(z^(z>>27))*0x94d049bb133111ebULL;return z^(z>>31);}
static double uniform(uint64_t index) {return (mix(index)>>11)*0x1.0p-53;}
int main(int argc,char** argv) {
 MPI_Init(&argc,&argv);int rank,size;MPI_Comm_rank(MPI_COMM_WORLD,&rank);MPI_Comm_size(MPI_COMM_WORLD,&size);
 const long long n=10000000;long long local=0,total=0;
 MPI_Barrier(MPI_COMM_WORLD);double start=MPI_Wtime();
 for(long long i=n*rank/size;i<n*(rank+1)/size;++i) {double x=uniform(2*i),y=uniform(2*i+1);if(x*x+y*y<=1.0) ++local;}
 if(rank==0) {total=local;for(int source=1;source<size;++source) {long long value;MPI_Recv(&value,1,MPI_LONG_LONG_INT,source,17,MPI_COMM_WORLD,MPI_STATUS_IGNORE);total+=value;}}
 else MPI_Send(&local,1,MPI_LONG_LONG_INT,0,17,MPI_COMM_WORLD);
 double elapsed=MPI_Wtime()-start,maximum;MPI_Reduce(&elapsed,&maximum,1,MPI_DOUBLE,MPI_MAX,0,MPI_COMM_WORLD);
 if(rank==0) printf("pi,%d,%.9f,%lld,%.10f\n",size,maximum,total,4.0*total/n);
 MPI_Finalize();return 0;
}
