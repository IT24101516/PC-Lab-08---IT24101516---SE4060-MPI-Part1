#include <mpi.h>
#include <cstdio>
#include <cstdint>

int main(int argc,char** argv) {
 MPI_Init(&argc,&argv);int rank,size;MPI_Comm_rank(MPI_COMM_WORLD,&rank);MPI_Comm_size(MPI_COMM_WORLD,&size);
 if(size<4) {if(rank==0) fprintf(stderr,"Use at least 4 processes.\n");MPI_Finalize();return 1;}
 int x=30,y;
 if(rank==1) {printf("Rank 1 sends to rank 3; rank 3 expects rank 2.\n");fflush(stdout);MPI_Ssend(&x,1,MPI_INT,3,0,MPI_COMM_WORLD);}
 if(rank==3) MPI_Recv(&y,1,MPI_INT,2,0,MPI_COMM_WORLD,MPI_STATUS_IGNORE);
 MPI_Finalize();return 0;
}
