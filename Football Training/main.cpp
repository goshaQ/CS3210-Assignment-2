#include <iostream>
#include <cstddef>

#include "Field.h"

const int FIELD_PROCESS_RANK = 0;
const int MAX_ROUND_NUMBER = 900;
const int MAX_STEP_NUMBER = 10;
const int FIELD_LENGTH = 128;
const int FIELD_WIDTH = 64;
const int SEED = 3210;

MPI_Datatype POSITION;

int main(int argc, char *argv[]) {
    int rank, size, roundCount = 0;

    const int POSITION_ENTRY_COUNT = 2;
    int blockLength[POSITION_ENTRY_COUNT] = {1, 1};
    MPI_Aint displacement[POSITION_ENTRY_COUNT] = {offsetof(Position, x), offsetof(Position, y)};
    MPI_Datatype type[POSITION_ENTRY_COUNT] = {MPI_INT, MPI_INT};

    MPI_Init(&argc, &argv);

    MPI_Type_create_struct(POSITION_ENTRY_COUNT, blockLength, displacement, type, &POSITION);
    MPI_Type_commit(&POSITION);

    MPI_Comm_size(MPI_COMM_WORLD, &size);
    MPI_Comm_rank(MPI_COMM_WORLD, &rank);

    srand((unsigned int) SEED + rank);

    switch (rank)
    {
        case FIELD_PROCESS_RANK:
        {
            Position **positions;
            Field field;

            positions = field.ReceivePlayerPositions();
            field.UpdatePlayerPositions(positions);

            while (roundCount++ < MAX_ROUND_NUMBER)
            {
                field.PlayRound();
                field.PrintStatistics(roundCount);
            }

            break;
        }
        default:
            int id = rank - 1;

            Player player(id, rank);
            player.SendPosition(player.GetPosition());

            while (roundCount++ < MAX_ROUND_NUMBER)
            {
                player.PlayRound();
            }
    }

    MPI_Finalize();
}
