#ifndef GLOBAL_H
#define GLOBAL_H

#include <mpi.h>
#include <cstdlib>

struct Position
{
    int x;
    int y;
};

extern const int FIELD_PROCESS_RANK;
extern const int MAX_ROUND_NUMBER;
extern const int MAX_STEP_NUMBER;
extern const int FIELD_LENGTH;
extern const int FIELD_WIDTH;
extern const int SEED;
extern MPI_Datatype POSITION;
#endif //GLOBAL_H
