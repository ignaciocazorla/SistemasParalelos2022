/*
** Sending simple, point-to-point messages.
*/

#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include "mpi.h" 

#define MASTER 0

int main(int argc, char* argv[])
{
  int myrank;
  int size;
  int dest;              /* destination rank for message */
  int source;            /* source rank of a message */
  int tag = 0;           /* scope for adding extra information to a message */
  MPI_Status status;     /* struct used by MPI_Recv */
  char message[BUFSIZ];

  /* MPI_Init returns once it has started up processes */
  MPI_Init( &argc, &argv );

  /* size and rank will become ubiquitous */ 
  MPI_Comm_size( MPI_COMM_WORLD, &size );
  MPI_Comm_rank( MPI_COMM_WORLD, &myrank );

  /* 
  ** SPMD - conditionals based upon rank
  ** will also become ubiquitous
  */

  if(myrank == 1){
    sprintf(message, "Hola Mundo! Soy el proceso %d!", myrank);
    dest = myrank + 1;
    MPI_Send(message,strlen(message)+1, MPI_CHAR, dest, tag, MPI_COMM_WORLD);
    MPI_Recv(message, BUFSIZ, MPI_CHAR, 0, tag, MPI_COMM_WORLD, &status);
    printf("Mensaje recibido en proceso %d: %s\n", myrank, message);
  }else{
    MPI_Recv(message, BUFSIZ, MPI_CHAR, myrank - 1, tag, MPI_COMM_WORLD, &status);
    printf("Mensaje recibido en proceso %d: %s\n", myrank, message);
    sprintf(message, "Hola Mundo! Soy el proceso %d!", myrank);
    if (myrank < (size - 1)){
      dest = myrank + 1;
    }else{
      dest = MASTER;
    }
    MPI_Send(message,strlen(message)+1, MPI_CHAR, dest, tag, MPI_COMM_WORLD);
  }

  /* don't forget to tidy up when we're done */
  MPI_Finalize();

  /* and exit the program */
  return EXIT_SUCCESS;
}