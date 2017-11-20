#include <iostream>
#include <cstddef>

#include "Field.h"

const int FIELD_TILE_NUMBER = 12;
const int FIELD_TILE_SIDE = 32;
const int MAX_PLAYER_SKILL = 15;
const int MAX_RUNNING_PLAYERS = 3;
const int MAX_ROUND_NUMBER = 1350;
const int TEAM_SIZE = 11;
const int FIELD_LENGTH = 128;
const int FIELD_WIDTH = 96;
const int SEED = 3210;

MPI_Datatype POSITION;
MPI_Datatype STAT;
Position invalidPosition = {INT32_MIN, INT32_MIN};
Position goalPosition = {INT32_MAX, INT32_MAX};

int main(int argc, char *argv[]) {
    int rank, size, roundCount = 0;

    const int commFieldTileTeamAColor[TEAM_SIZE] = {0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10};
    const int commFieldTileTeamBColor[TEAM_SIZE] = {11, 12, 13, 14, 15, 16, 17, 18, 19, 20, 21};
    const int commFieldColor = 22, commTeamAColor = 23, commTeamBColor = 24;

    MPI_Comm commField, commTeamA, commTeamB;
    MPI_Comm commFieldTileTeamA[FIELD_TILE_NUMBER], commFieldTileTeamB[FIELD_TILE_NUMBER];

    const int POSITION_ENTRY_COUNT = 2;
    int blockLength[POSITION_ENTRY_COUNT] = {1, 1};
    MPI_Aint displacement[POSITION_ENTRY_COUNT] = {offsetof(Position, x), offsetof(Position, y)};
    MPI_Datatype type[POSITION_ENTRY_COUNT] = {MPI_INT, MPI_INT};

    const int STAT_ENTRY_COUNT = 12;
    int blockLength1[STAT_ENTRY_COUNT] = {1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1};
    MPI_Datatype type1[STAT_ENTRY_COUNT] = {MPI_INT, MPI_INT, MPI_INT, MPI_INT,
        MPI_C_BOOL, MPI_C_BOOL, MPI_INT, MPI_INT, MPI_INT, MPI_INT, MPI_INT, MPI_INT};
    MPI_Aint displacement1[STAT_ENTRY_COUNT] = {offsetof(Stat, distanceTraveledSinceTheBeginning),
        offsetof(Stat, ballWasReachedSinceTheBeginningCount), offsetof(Stat, ballWasWonSinceTheBeginningCount),
            offsetof(Stat, ballChallenge), offsetof(Stat, ballWasReached), offsetof(Stat, ballWasWon),
                offsetof(Stat, ballPositionX), offsetof(Stat, ballPositionY), offsetof(Stat, initialPositionX),
                    offsetof(Stat, initialPositionY), offsetof(Stat, finalPositionX), offsetof(Stat, finalPositionY)};

    MPI_Init(&argc, &argv);

    MPI_Type_create_struct(STAT_ENTRY_COUNT, blockLength1, displacement1, type1, &STAT);
    MPI_Type_commit(&STAT);

    MPI_Type_create_struct(POSITION_ENTRY_COUNT, blockLength, displacement, type, &POSITION);
    MPI_Type_commit(&POSITION);

    MPI_Comm_size(MPI_COMM_WORLD, &size);
    MPI_Comm_rank(MPI_COMM_WORLD, &rank);

    srand((unsigned int) SEED + rank);


    if (0 <= rank && rank < FIELD_TILE_NUMBER)
    {
        MPI_Comm_split(MPI_COMM_WORLD, commFieldColor, rank, &commField);
        MPI_Comm_split(MPI_COMM_WORLD, MPI_UNDEFINED, rank, &commTeamA);
        MPI_Comm_split(MPI_COMM_WORLD, MPI_UNDEFINED, rank, &commTeamB);

        for (int i = 0; i < FIELD_TILE_NUMBER; ++i) {
            if (i == rank)
            {
                MPI_Comm_split(MPI_COMM_WORLD, commFieldTileTeamAColor[i], rank, &commFieldTileTeamA[i]);
            } else
            {
                MPI_Comm_split(MPI_COMM_WORLD, MPI_UNDEFINED, rank, &commFieldTileTeamA[i]);

            }
        }

        for (int i = 0; i < FIELD_TILE_NUMBER; ++i) {
            if (i == rank)
            {
                MPI_Comm_split(MPI_COMM_WORLD, commFieldTileTeamBColor[i], rank, &commFieldTileTeamB[i]);
            } else
            {
                MPI_Comm_split(MPI_COMM_WORLD, MPI_UNDEFINED, rank, &commFieldTileTeamB[i]);

            }
        }

        Position *positions;
        Stat *statTeamA, *statTeamB;

        Field field(rank, &commField, &commFieldTileTeamA[rank], &commFieldTileTeamB[rank]);

        positions = field.ReceiveTeamPositions(&commFieldTileTeamA[rank]);
        field.UpdateTeamPosition(positions, field.GetTeamA());

        positions = field.ReceiveTeamPositions(&commFieldTileTeamB[rank]);
        field.UpdateTeamPosition(positions, field.GetTeamB());

        for (int i = 0; i < 2 * MAX_ROUND_NUMBER; ++i) {
            if (i == MAX_ROUND_NUMBER)
            {
                field.Reset();

                positions = field.ReceiveTeamPositions(&commFieldTileTeamA[rank]);
                field.UpdateTeamPosition(positions, field.GetTeamA());

                positions = field.ReceiveTeamPositions(&commFieldTileTeamB[rank]);
                field.UpdateTeamPosition(positions, field.GetTeamB());
            }

            field.PlayRound();


            if (rank == 0)
            {
                statTeamA = field.ReceiveStatistics(&commFieldTileTeamA[rank]);
                statTeamB = field.ReceiveStatistics(&commFieldTileTeamB[rank]);

                field.PrintStatistics(i + 1, statTeamA, statTeamB);

                delete[] statTeamA;
                delete[] statTeamB;
            }
        }
    } else if (FIELD_TILE_NUMBER <= rank && rank < FIELD_TILE_NUMBER + TEAM_SIZE)
    {
        MPI_Comm_split(MPI_COMM_WORLD, MPI_UNDEFINED, rank, &commField);
        MPI_Comm_split(MPI_COMM_WORLD, commTeamAColor, rank, &commTeamA);
        MPI_Comm_split(MPI_COMM_WORLD, MPI_UNDEFINED, rank, &commTeamB);

        for (int i = 0; i < FIELD_TILE_NUMBER; ++i) {
            MPI_Comm_split(MPI_COMM_WORLD, commFieldTileTeamAColor[i], rank, &commFieldTileTeamA[i]);
        }

        for (int i = 0; i < FIELD_TILE_NUMBER; ++i) {
            MPI_Comm_split(MPI_COMM_WORLD, MPI_UNDEFINED, rank, &commFieldTileTeamB[i]);
        }

        Player player(rank - FIELD_TILE_NUMBER, rank - FIELD_TILE_NUMBER, 'A', &commTeamA, commFieldTileTeamA);

        player.SetGameTactics(true);
        player.JumpToDefaultPosition();
        player.SendPosition(player.GetPosition());

        for (int i = 0; i < MAX_ROUND_NUMBER; ++i)
        {
            player.PlayRound();
            player.SendStatistics();
        }

        player.SetGameTactics(false);
        player.JumpToDefaultPosition();
        player.SendPosition(player.GetPosition());

        for (int i = 0; i < MAX_ROUND_NUMBER; ++i)
        {
            player.PlayRound();
            player.SendStatistics();
        }
    } else if (FIELD_TILE_NUMBER + TEAM_SIZE <= rank && rank < FIELD_TILE_NUMBER + 2 * TEAM_SIZE)
    {
        MPI_Comm_split(MPI_COMM_WORLD, MPI_UNDEFINED, rank, &commField);
        MPI_Comm_split(MPI_COMM_WORLD, MPI_UNDEFINED, rank, &commTeamA);
        MPI_Comm_split(MPI_COMM_WORLD, commTeamBColor, rank, &commTeamB);

        for (int i = 0; i < FIELD_TILE_NUMBER; ++i) {
            MPI_Comm_split(MPI_COMM_WORLD, MPI_UNDEFINED, rank, &commFieldTileTeamA[i]);
        }

        for (int i = 0; i < FIELD_TILE_NUMBER; ++i) {
            MPI_Comm_split(MPI_COMM_WORLD, commFieldTileTeamBColor[i], rank, &commFieldTileTeamB[i]);
        }

        Player player(rank - FIELD_TILE_NUMBER - TEAM_SIZE, rank - FIELD_TILE_NUMBER, 'B', &commTeamB, commFieldTileTeamB);

        player.SetGameTactics(false);
        player.JumpToDefaultPosition();
        player.SendPosition(player.GetPosition());

        for (int i = 0; i < MAX_ROUND_NUMBER; ++i)
        {
            player.PlayRound();
            player.SendStatistics();
        }

        player.SetGameTactics(true);
        player.JumpToDefaultPosition();
        player.SendPosition(player.GetPosition());

        for (int i = 0; i < MAX_ROUND_NUMBER; ++i)
        {
            player.PlayRound();
            player.SendStatistics();
        }
    }

    if (commField != MPI_COMM_NULL) MPI_Comm_free(&commField);
    if (commTeamA != MPI_COMM_NULL) MPI_Comm_free(&commTeamA);
    if (commTeamB != MPI_COMM_NULL) MPI_Comm_free(&commTeamB);

    for (int i = 0; i < TEAM_SIZE; ++i) {
        if (commFieldTileTeamA[i] != MPI_COMM_NULL) MPI_Comm_free(&commFieldTileTeamA[i]);
        if (commFieldTileTeamB[i] != MPI_COMM_NULL) MPI_Comm_free(&commFieldTileTeamB[i]);
    }

    MPI_Finalize();
}
