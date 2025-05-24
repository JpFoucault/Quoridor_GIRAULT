/*
    Copyright (c) Arnaud BANNIER, Nicolas BODIN and Matthieu LE BERRE.
    Licensed under the MIT License.
    See LICENSE.md in the project root for license information.
*/

#include "core/quoridor_core.h"
#include "core/utils.h"
//#define min(a,b) ((a)<(b))?(a):(b)

void QuoridorCore_updateValidMoves_bis(QuoridorCore* self)
{
    const int gridSize = self->gridSize;
    const int currI = self->positions[self->playerID].i;
    const int currJ = self->positions[self->playerID].j;
    const int otherI = self->positions[self->playerID ^ 1].i;
    const int otherJ = self->positions[self->playerID ^ 1].j;

    // TODO

    for (int i = 0; i < self->gridSize; i++)
    {
        for (int j = 0; j < self->gridSize; j++)
            self->isValid[i][j] = false;
    }

    if (currI - 1 >= 0 && !QuoridorCore_hasWallAbove(self, currI, currJ) && (currI - 1 != otherI || currJ != otherJ))
        self->isValid[currI - 1][currJ] = true;
    if (currJ + 1 < self->gridSize && !QuoridorCore_hasWallRight(self, currI, currJ) && (currI != otherI || currJ + 1 != otherJ))
        self->isValid[currI][currJ + 1] = true;
    if (currI + 1 < self->gridSize && !QuoridorCore_hasWallBelow(self, currI, currJ) && (currI + 1 != otherI || currJ != otherJ))
        self->isValid[currI + 1][currJ] = true;
    if (currJ - 1 >= 0 && !QuoridorCore_hasWallLeft(self, currI, currJ) && (currI != otherI || currJ - 1 != otherJ))
        self->isValid[currI][currJ - 1] = true;

    if (currI - 1 == otherI && currJ == otherJ && !QuoridorCore_hasWallAbove(self, currI, currJ))
    {
        if (QuoridorCore_hasWallAbove(self, otherI, otherJ) || otherI == 0)
        {
            if (otherJ - 1 >= 0 && !QuoridorCore_hasWallLeft(self, otherI, otherJ))
                self->isValid[otherI][otherJ - 1] = true;
            if (otherJ + 1 < self->gridSize && !QuoridorCore_hasWallRight(self, otherI, otherJ))
                self->isValid[otherI][otherJ + 1] = true;
        }
        else
        {
            if (otherI - 1 >= 0)
                self->isValid[otherI - 1][otherJ] = true;
        }
    }
    if (currI == otherI && currJ + 1 == otherJ && !QuoridorCore_hasWallRight(self, currI, currJ))
    {
        if (QuoridorCore_hasWallRight(self, otherI, otherJ) || otherJ == self->gridSize - 1)
        {
            if (otherI - 1 >= 0 && !QuoridorCore_hasWallAbove(self, otherI, otherJ))
                self->isValid[otherI - 1][otherJ] = true;
            if (otherI + 1 < self->gridSize && !QuoridorCore_hasWallBelow(self, otherI, otherJ))
                self->isValid[otherI + 1][otherJ] = true;
        }
        else
        {
            if (otherJ + 1 < self->gridSize)
                self->isValid[otherI][otherJ + 1] = true;
        }
    }
    if (currI + 1 == otherI && currJ == otherJ && !QuoridorCore_hasWallBelow(self, currI, currJ))
    {
        if (QuoridorCore_hasWallBelow(self, otherI, otherJ) || otherI == self->gridSize - 1)
        {
            if (otherJ - 1 >= 0 && !QuoridorCore_hasWallLeft(self, otherI, otherJ))
                self->isValid[otherI][otherJ - 1] = true;
            if (otherJ + 1 < self->gridSize && !QuoridorCore_hasWallRight(self, otherI, otherJ))
                self->isValid[otherI][otherJ + 1] = true;
        }
        else
        {
            if (otherI + 1 < self->gridSize)
                self->isValid[otherI + 1][otherJ] = true;
        }
    }
    if (currI == otherI && currJ - 1 == otherJ && !QuoridorCore_hasWallLeft(self, currI, currJ))
    {
        if (QuoridorCore_hasWallLeft(self, otherI, otherJ) || otherJ == 0)
        {
            if (otherI - 1 >= 0 && !QuoridorCore_hasWallAbove(self, otherI, otherJ))
                self->isValid[otherI - 1][otherJ] = true;
            if (otherI + 1 < self->gridSize && !QuoridorCore_hasWallBelow(self, otherI, otherJ))
                self->isValid[otherI + 1][otherJ] = true;
        }
        else
        {
            if (otherJ - 1 >= 0)
                self->isValid[otherI][otherJ - 1] = true;
        }
    }

}

void* AIData_create(QuoridorCore* core)
{
    // Cette fonction n'est utile que si vous avez besoin de mémoriser des informations sur les coups précédents de l'IA.

    return NULL;
}

void AIData_destroy(void* self)
{
    if (!self) return;
}

void AIData_reset(void* self)
{
}

void QuoridorCore_getShortestPath(QuoridorCore* self, int playerID, QuoridorPos* path, int* size)
{
    bool visited[MAX_GRID_SIZE][MAX_GRID_SIZE] = { false };
    QuoridorPos pred[MAX_GRID_SIZE][MAX_GRID_SIZE];
    QuoridorPos queue[MAX_GRID_SIZE * MAX_GRID_SIZE];
    int front = 0, rear = 0;

    QuoridorPos start = self->positions[playerID];
    queue[rear++] = start;
    visited[start.i][start.j] = true;

    bool found = false;
    QuoridorPos end;

    while (front < rear && !found)
    {
        QuoridorPos current = queue[front++];
        int i = current.i;
        int j = current.j;


        if ((playerID == 0 && j == self->gridSize - 1) || (playerID == 1 && j == 0))
        {
            end = current;
            found = true;
            break;
        }


        const int di[] = { -1, 1, 0, 0 };
        const int dj[] = { 0, 0, -1, 1 };
        bool wallCheck[4] = { QuoridorCore_hasWallAbove(self, i, j), QuoridorCore_hasWallBelow(self, i, j), QuoridorCore_hasWallLeft(self, i, j), QuoridorCore_hasWallRight(self, i, j) };

        for (int dir = 0; dir < 4; dir++)
        {
            int ni = i + di[dir];
            int nj = j + dj[dir];

            if (ni < 0 || ni >= self->gridSize || nj < 0 || nj >= self->gridSize)
                continue;

            if (!wallCheck[dir] && !visited[ni][nj])
            {
                visited[ni][nj] = true;
                pred[ni][nj] = current;
                queue[rear++] = (QuoridorPos){ ni, nj };
            }
        }
    }

    if (found)
    {
        QuoridorPos tmp = end;
        while (tmp.i != start.i || tmp.j != start.j)
        {
            path[(*size)++] = tmp;
            tmp = pred[tmp.i][tmp.j];
        }
        path[(*size)++] = start;
    }
}

/// @brief Calcule une heuristique d'évaluation de l'état du jeu pour un joueur donné.
/// Cette fonction est utilisée dans l'algorithme Min-Max pour estimer la qualité d'une position.
/// Elle retourne une valeur représentant l'avantage du joueur playerID.
/// Une valeur positive indique un avantage pour ce joueur, une valeur négative indique un avantage pour l'adversaire.
/// @param self Instance du jeu Quoridor.
/// @param playerID Indice du joueur à évaluer (0 ou 1).
/// @return Une estimation numérique de l'avantage du joueur playerID.
static float QuoridorCore_computeScore(QuoridorCore* self, int playerID, int currDepth)
{
    int playerA = playerID;
    int playerB = playerID ^ 1;

    // TODO

    QuoridorPos pathA[MAX_PATH_LEN] = { {-1, -1} };
    QuoridorPos pathB[MAX_PATH_LEN] = { {-1, -1} };


    int sizeA = 0;
    int sizeB = 0;
    int sizebtw = 0;

    QuoridorCore_getShortestPath(self, playerA, pathA, &sizeA);
    QuoridorCore_getShortestPath(self, playerB, pathB, &sizeB);

    // difference entre les 2 chemins
    float score = (float)(10 * (sizeB - sizeA));

    // plus le coup cherché est loin, moins il vaut pour favoriser une victoire rapide
    score -= 0.5f * currDepth;

    // favorise le fait d'avoir des murs
    score += 3.f * (self->wallCounts[playerA] - self->wallCounts[playerB]);

    // favorise la proimité avec l'arrivée
    if (playerID == 0)
    {
        score += (float)(2 * self->positions[playerID].j);
    }
    else
    {
        score += (float)(2 * (8 - self->positions[playerID].j));
    }

    //prise du centre
    int center = self->gridSize / 2;
    float dcenter = (float)(abs(self->positions[playerA].j - center));
    score -= 0.5f * dcenter;

    // nombre de mouvements
    int movA = 0;
    int movB = 0;
    QuoridorCore gameCopy = *self;
    gameCopy.playerID = playerB;
    QuoridorCore_updateValidMoves_bis(&gameCopy);
    for (int i = 0; i < self->gridSize; i++)
    {
        for (int j = 0; j < self->gridSize; j++)
        {
            if (self->isValid[i][j])
                movA++;
            if (gameCopy.isValid[i][j])
                movB++;
        }
    }
    score += (float)(movA - movB);

    return score + Float_randAB(-0.1f, 0.1f);
}

bool QuoridorCore_isWalltested(QuoridorTurn* potential, int size, QuoridorAction action, int wallI, int wallJ)
{
    for (int index = 0; index < size; index++)
    {
        if (potential[index].action == action && potential[index].i == wallI && potential[index].j == wallJ)
            return true;
    }
    return false;
}


/// @brief Applique l'algorithme Min-Max (avec élagage alpha-bêta) pour déterminer le coup joué par l'IA.
/// Cette fonction explore récursivement une partie de l'arbre des coups possibles jusqu'à une profondeur maximale donnée.
/// @param self Instance du jeu Quoridor.
/// @param playerID Identifiant du joueur pour lequel on cherche le meilleur coup.
/// @param currDepth Profondeur actuelle dans l'arbre de recherche.
/// @param maxDepth Profondeur maximale à atteindre dans l'arbre.
/// @param alpha Meilleure valeur actuellement garantie pour le joueur maximisant.
/// @param beta Meilleure valeur actuellement garantie pour le joueur minimisant.
/// @param turn Pointeur vers une variable où sera enregistré le meilleur coup trouvé (à la racine).
/// @return L'évaluation numérique de la position courante, selon la fonction heuristique.


static float QuoridorCore_minMax(
    QuoridorCore* self, int playerID, int currDepth, int maxDepth,
    float alpha, float beta, QuoridorTurn* turn)
{
    int playerA = playerID;
    int playerB = playerID ^ 1;

    if (self->state != QUORIDOR_STATE_IN_PROGRESS)
    {
        if ((playerA == 0 && self->state == QUORIDOR_STATE_P0_WON) || (playerA == 1 && self->state == QUORIDOR_STATE_P1_WON))
        {
            return 10000.f + (10 - currDepth);
        }
        else if ((playerB == 0 && self->state == QUORIDOR_STATE_P0_WON) || (playerB == 1 && self->state == QUORIDOR_STATE_P1_WON))
        {
            return -10000.f - (10 - currDepth);
        }
    }
    else if (currDepth >= maxDepth)
    {
        return QuoridorCore_computeScore(self, playerID, currDepth);
    }

    const int gridSize = self->gridSize;
    const int currI = self->positions[self->playerID].i;
    const int currJ = self->positions[self->playerID].j;
    QuoridorTurn childTurn = { 0 };

    const bool maximizing = (currDepth % 2) == 0;
    float value = maximizing ? -INFINITY : INFINITY;
    int bestMoveIndex = -1;

    // TODO

    QuoridorPos path1[MAX_GRID_SIZE * MAX_GRID_SIZE];
    QuoridorPos path2[MAX_GRID_SIZE * MAX_GRID_SIZE];
    for (int i = 0; i < MAX_GRID_SIZE * MAX_GRID_SIZE; i++)
    {
        path1[i].i = -1;
        path1[i].j = -1;

        path2[i].i = -1;
        path2[i].j = -1;
    }
    int size1 = 0;

    QuoridorCore_getShortestPath(self, playerB, path1, &size1);

    for (int i = 0; i < size1 / 2; i++)
    {
        QuoridorPos tmp = path1[i];
        path1[i] = path1[size1 - (i + 1)];
        path1[size1 - (i + 1)] = tmp;
    }
    QuoridorPos next1 = path1[1];

    QuoridorCore gCopy = *self;
    gCopy.isValid[next1.i][next1.j] = false;

    int size2 = 0;
    QuoridorCore_getShortestPath(&gCopy, playerB, path2, &size2);
    if (path2[0].i == -1 || path2[0].j == -1)
    {
        gCopy.isValid[next1.i][next1.j] = true;
        int rIndex = Int_randAB(1, size1 - 1);
        next1 = path1[rIndex];
        gCopy.isValid[next1.i][next1.j] = false;
        QuoridorCore_getShortestPath(&gCopy, playerB, path2, &size2);
    }


    QuoridorTurn potential[2 * MAX_PATH_LEN];
    AssertNew(potential);

    float scores[2 * MAX_PATH_LEN] = { -1000000 };
    int cpt = 0;

    for (int i = 0; i < self->gridSize; i++)
    {
        for (int j = 0; j < self->gridSize; j++)
        {
            if (QuoridorCore_canMoveTo(self, i, j))
            {
                potential[cpt].action = QUORIDOR_MOVE_TO;
                potential[cpt].i = i;
                potential[cpt].j = j;

                QuoridorCore gameCopy = *self;
                QuoridorTurn turnCopy;
                QuoridorCore_playTurn(&gameCopy, (potential[cpt]));

                scores[cpt] = QuoridorCore_minMax(&gameCopy, playerA, currDepth + 1, maxDepth, alpha, beta, &turnCopy);
                if (maximizing)
                {
                    if (scores[cpt] > value)
                    {
                        value = scores[cpt];
                        bestMoveIndex = cpt;
                        alpha = (float)(fmax(alpha, value));
                    }
                }
                else
                {
                    if (scores[cpt] < value)
                    {
                        value = scores[cpt];
                        bestMoveIndex = cpt;
                        beta = (float)(fmin(beta, value));
                    }
                }

                cpt++;

                if (beta <= alpha)
                    goto pruning;

            }
        }
    }
    int count = 0;
    if (currDepth < 4)
    {
        QuoridorPos current = path1[count++];
        QuoridorPos next = path1[count++];
        for (int i = 0; i < size1; i++)
        {
            if (next.i == -1 || next.j == -1)
                continue;

            int dI = current.i - next.i;
            int dJ = current.j - next.j;

            if (dI != 0)
            {
                if (QuoridorCore_canPlayWall(self, WALL_TYPE_HORIZONTAL, min(current.i, next.i), current.j))
                {
                    potential[cpt].action = QUORIDOR_PLAY_HORIZONTAL_WALL;
                    potential[cpt].i = min(current.i, next.i);
                    potential[cpt].j = current.j;

                    QuoridorCore gameCopy = *self;
                    QuoridorTurn turnCopy;
                    QuoridorCore_playTurn(&gameCopy, (potential[cpt]));

                    scores[cpt] = QuoridorCore_minMax(&gameCopy, playerA, currDepth + 1, maxDepth, alpha, beta, &turnCopy);
                    if (maximizing)
                    {
                        if (scores[cpt] > value)
                        {
                            value = scores[cpt];
                            bestMoveIndex = cpt;
                            alpha = (float)(fmax(alpha, value));
                        }
                    }
                    else
                    {
                        if (scores[cpt] < value)
                        {
                            value = scores[cpt];
                            bestMoveIndex = cpt;
                            beta = (float)(fmin(beta, value));
                        }
                    }

                    cpt++;

                    if (beta <= alpha)
                        goto pruning;

                }
                if (QuoridorCore_canPlayWall(self, WALL_TYPE_HORIZONTAL, min(current.i, next.i), current.j - 1))
                {
                    potential[cpt].action = QUORIDOR_PLAY_HORIZONTAL_WALL;
                    potential[cpt].i = min(current.i, next.i);
                    potential[cpt].j = current.j - 1;

                    QuoridorCore gameCopy = *self;
                    QuoridorTurn turnCopy;
                    QuoridorCore_playTurn(&gameCopy, (potential[cpt]));

                    scores[cpt] = QuoridorCore_minMax(&gameCopy, playerA, currDepth + 1, maxDepth, alpha, beta, &turnCopy);
                    if (maximizing)
                    {
                        if (scores[cpt] > value)
                        {
                            value = scores[cpt];
                            bestMoveIndex = cpt;
                            alpha = (float)(fmax(alpha, value));
                        }
                    }
                    else
                    {
                        if (scores[cpt] < value)
                        {
                            value = scores[cpt];
                            bestMoveIndex = cpt;
                            beta = (float)(fmin(beta, value));
                        }
                    }

                    cpt++;

                    if (beta <= alpha)
                        goto pruning;

                }
            }
            else if (dJ != 0)
            {
                if (QuoridorCore_canPlayWall(self, WALL_TYPE_VERTICAL, current.i, min(current.j, next.j)))
                {
                    potential[cpt].action = QUORIDOR_PLAY_VERTICAL_WALL;
                    potential[cpt].i = current.i;
                    potential[cpt].j = min(current.j, next.j);

                    QuoridorCore gameCopy = *self;
                    QuoridorTurn turnCopy;
                    QuoridorCore_playTurn(&gameCopy, (potential[cpt]));

                    scores[cpt] = QuoridorCore_minMax(&gameCopy, playerA, currDepth + 1, maxDepth, alpha, beta, &turnCopy);
                    if (maximizing)
                    {
                        if (scores[cpt] > value)
                        {
                            value = scores[cpt];
                            bestMoveIndex = cpt;
                            alpha = (float)(fmax(alpha, value));
                        }
                    }
                    else
                    {
                        if (scores[cpt] < value)
                        {
                            value = scores[cpt];
                            bestMoveIndex = cpt;
                            beta = (float)(fmin(beta, value));
                        }
                    }

                    cpt++;

                    if (beta <= alpha)
                        goto pruning;

                }
                if (QuoridorCore_canPlayWall(self, WALL_TYPE_VERTICAL, current.i - 1, min(current.j, next.j)))
                {
                    potential[cpt].action = QUORIDOR_PLAY_VERTICAL_WALL;
                    potential[cpt].i = current.i - 1;
                    potential[cpt].j = min(current.j, next.j);

                    QuoridorCore gameCopy = *self;
                    QuoridorTurn turnCopy;
                    QuoridorCore_playTurn(&gameCopy, (potential[cpt]));

                    scores[cpt] = QuoridorCore_minMax(&gameCopy, playerA, currDepth + 1, maxDepth, alpha, beta, &turnCopy);
                    if (maximizing)
                    {
                        if (scores[cpt] > value)
                        {
                            value = scores[cpt];
                            bestMoveIndex = cpt;
                            alpha = (float)(fmax(alpha, value));
                        }
                    }
                    else
                    {
                        if (scores[cpt] < value)
                        {
                            value = scores[cpt];
                            bestMoveIndex = cpt;
                            beta = (float)(fmin(beta, value));
                        }
                    }

                    cpt++;

                    if (beta <= alpha)
                        goto pruning;

                }
            }

            current = next;
            next = path1[count++];
        }

        if (path2[0].i != -1 && path2[0].j != -1)
        {
            count = 0;
            current = path2[count++];
            next = path2[count++];
            for (int i = 0; i < size2; i++)
            {
                if (next.i == -1 || next.j == -1)
                    continue;

                int dI = current.i - next.i;
                int dJ = current.j - next.j;

                if (dI != 0)
                {
                    if (QuoridorCore_canPlayWall(self, WALL_TYPE_HORIZONTAL, min(current.i, next.i), current.j) &&
                        !QuoridorCore_isWalltested(potential, cpt, QUORIDOR_PLAY_HORIZONTAL_WALL, min(current.i, next.i), current.j))
                    {
                        potential[cpt].action = QUORIDOR_PLAY_HORIZONTAL_WALL;
                        potential[cpt].i = min(current.i, next.i);
                        potential[cpt].j = current.j;

                        QuoridorCore gameCopy = *self;
                        QuoridorTurn turnCopy;
                        QuoridorCore_playTurn(&gameCopy, (potential[cpt]));

                        scores[cpt] = QuoridorCore_minMax(&gameCopy, playerA, currDepth + 1, maxDepth, alpha, beta, &turnCopy);
                        if (maximizing)
                        {
                            if (scores[cpt] > value)
                            {
                                value = scores[cpt];
                                bestMoveIndex = cpt;
                                alpha = (float)(fmax(alpha, value));
                            }
                        }
                        else
                        {
                            if (scores[cpt] < value)
                            {
                                value = scores[cpt];
                                bestMoveIndex = cpt;
                                beta = (float)(fmin(beta, value));
                            }
                        }

                        cpt++;

                        if (beta <= alpha)
                            goto pruning;

                    }
                    if (QuoridorCore_canPlayWall(self, WALL_TYPE_HORIZONTAL, min(current.i, next.i), current.j - 1) &&
                        !QuoridorCore_isWalltested(potential, cpt, QUORIDOR_PLAY_HORIZONTAL_WALL, min(current.i, next.i), current.j - 1))
                    {
                        potential[cpt].action = QUORIDOR_PLAY_HORIZONTAL_WALL;
                        potential[cpt].i = min(current.i, next.i);
                        potential[cpt].j = current.j - 1;

                        QuoridorCore gameCopy = *self;
                        QuoridorTurn turnCopy;
                        QuoridorCore_playTurn(&gameCopy, (potential[cpt]));

                        scores[cpt] = QuoridorCore_minMax(&gameCopy, playerA, currDepth + 1, maxDepth, alpha, beta, &turnCopy);
                        if (maximizing)
                        {
                            if (scores[cpt] > value)
                            {
                                value = scores[cpt];
                                bestMoveIndex = cpt;
                                alpha = (float)(fmax(alpha, value));
                            }
                        }
                        else
                        {
                            if (scores[cpt] < value)
                            {
                                value = scores[cpt];
                                bestMoveIndex = cpt;
                                beta = (float)(fmin(beta, value));
                            }
                        }

                        cpt++;

                        if (beta <= alpha)
                            goto pruning;

                    }
                }
                else if (dJ != 0)
                {
                    if (QuoridorCore_canPlayWall(self, WALL_TYPE_VERTICAL, current.i, min(current.j, next.j)) &&
                        !QuoridorCore_isWalltested(potential, cpt, QUORIDOR_PLAY_VERTICAL_WALL, current.i, min(current.j, next.j)))
                    {
                        potential[cpt].action = QUORIDOR_PLAY_VERTICAL_WALL;
                        potential[cpt].i = current.i;
                        potential[cpt].j = min(current.j, next.j);

                        QuoridorCore gameCopy = *self;
                        QuoridorTurn turnCopy;
                        QuoridorCore_playTurn(&gameCopy, (potential[cpt]));

                        scores[cpt] = QuoridorCore_minMax(&gameCopy, playerA, currDepth + 1, maxDepth, alpha, beta, &turnCopy);
                        if (maximizing)
                        {
                            if (scores[cpt] > value)
                            {
                                value = scores[cpt];
                                bestMoveIndex = cpt;
                                alpha = (float)(fmax(alpha, value));
                            }
                        }
                        else
                        {
                            if (scores[cpt] < value)
                            {
                                value = scores[cpt];
                                bestMoveIndex = cpt;
                                beta = (float)(fmin(beta, value));
                            }
                        }

                        cpt++;

                        if (beta <= alpha)
                            goto pruning;

                    }
                    if (QuoridorCore_canPlayWall(self, WALL_TYPE_VERTICAL, current.i - 1, min(current.j, next.j)) &&
                        !QuoridorCore_isWalltested(potential, cpt, QUORIDOR_PLAY_VERTICAL_WALL, current.i - 1, min(current.j, next.j)))
                    {
                        potential[cpt].action = QUORIDOR_PLAY_VERTICAL_WALL;
                        potential[cpt].i = current.i - 1;
                        potential[cpt].j = min(current.j, next.j);

                        QuoridorCore gameCopy = *self;
                        QuoridorTurn turnCopy;
                        QuoridorCore_playTurn(&gameCopy, (potential[cpt]));

                        scores[cpt] = QuoridorCore_minMax(&gameCopy, playerA, currDepth + 1, maxDepth, alpha, beta, &turnCopy);
                        if (maximizing)
                        {
                            if (scores[cpt] > value)
                            {
                                value = scores[cpt];
                                bestMoveIndex = cpt;
                                alpha = (float)(fmax(alpha, value));
                            }
                        }
                        else
                        {
                            if (scores[cpt] < value)
                            {
                                value = scores[cpt];
                                bestMoveIndex = cpt;
                                beta = (float)(fmin(beta, value));
                            }
                        }

                        cpt++;

                        if (beta <= alpha)
                            goto pruning;

                    }
                }

                current = next;
                next = path2[count++];
            }
        }

    }
    else
    {
        QuoridorPos current = path1[count++];
        QuoridorPos next = path1[count++];

        int dI = current.i - next.i;
        int dJ = current.j - next.j;

        if (dI != 0)
        {
            if (QuoridorCore_canPlayWall(self, WALL_TYPE_HORIZONTAL, min(current.i, next.i), current.j))
            {
                potential[cpt].action = QUORIDOR_PLAY_HORIZONTAL_WALL;
                potential[cpt].i = min(current.i, next.i);
                potential[cpt].j = current.j;

                QuoridorCore gameCopy = *self;
                QuoridorTurn turnCopy;
                QuoridorCore_playTurn(&gameCopy, (potential[cpt]));

                scores[cpt] = QuoridorCore_minMax(&gameCopy, playerA, currDepth + 1, maxDepth, alpha, beta, &turnCopy);
                if (maximizing)
                {
                    if (scores[cpt] > value)
                    {
                        value = scores[cpt];
                        bestMoveIndex = cpt;
                        alpha = (float)(fmax(alpha, value));
                    }
                }
                else
                {
                    if (scores[cpt] < value)
                    {
                        value = scores[cpt];
                        bestMoveIndex = cpt;
                        beta = (float)(fmin(beta, value));
                    }
                }

                cpt++;

                if (beta <= alpha)
                    goto pruning;

            }
            if (QuoridorCore_canPlayWall(self, WALL_TYPE_HORIZONTAL, min(current.i, next.i), current.j - 1))
            {
                potential[cpt].action = QUORIDOR_PLAY_HORIZONTAL_WALL;
                potential[cpt].i = min(current.i, next.i);
                potential[cpt].j = current.j - 1;

                QuoridorCore gameCopy = *self;
                QuoridorTurn turnCopy;
                QuoridorCore_playTurn(&gameCopy, (potential[cpt]));

                scores[cpt] = QuoridorCore_minMax(&gameCopy, playerA, currDepth + 1, maxDepth, alpha, beta, &turnCopy);
                if (maximizing)
                {
                    if (scores[cpt] > value)
                    {
                        value = scores[cpt];
                        bestMoveIndex = cpt;
                        alpha = (float)(fmax(alpha, value));
                    }
                }
                else
                {
                    if (scores[cpt] < value)
                    {
                        value = scores[cpt];
                        bestMoveIndex = cpt;
                        beta = (float)(fmin(beta, value));
                    }
                }

                cpt++;

                if (beta <= alpha)
                    goto pruning;

            }
        }
        else if (dJ != 0)
        {
            if (QuoridorCore_canPlayWall(self, WALL_TYPE_VERTICAL, current.i, min(current.j, next.j)))
            {
                potential[cpt].action = QUORIDOR_PLAY_VERTICAL_WALL;
                potential[cpt].i = current.i;
                potential[cpt].j = min(current.j, next.j);

                QuoridorCore gameCopy = *self;
                QuoridorTurn turnCopy;
                QuoridorCore_playTurn(&gameCopy, (potential[cpt]));

                scores[cpt] = QuoridorCore_minMax(&gameCopy, playerA, currDepth + 1, maxDepth, alpha, beta, &turnCopy);
                if (maximizing)
                {
                    if (scores[cpt] > value)
                    {
                        value = scores[cpt];
                        bestMoveIndex = cpt;
                        alpha = (float)(fmax(alpha, value));
                    }
                }
                else
                {
                    if (scores[cpt] < value)
                    {
                        value = scores[cpt];
                        bestMoveIndex = cpt;
                        beta = (float)(fmin(beta, value));
                    }
                }

                cpt++;

                if (beta <= alpha)
                    goto pruning;

            }
            if (QuoridorCore_canPlayWall(self, WALL_TYPE_VERTICAL, current.i - 1, min(current.j, next.j)))
            {
                potential[cpt].action = QUORIDOR_PLAY_VERTICAL_WALL;
                potential[cpt].i = current.i - 1;
                potential[cpt].j = min(current.j, next.j);

                QuoridorCore gameCopy = *self;
                QuoridorTurn turnCopy;
                QuoridorCore_playTurn(&gameCopy, (potential[cpt]));

                scores[cpt] = QuoridorCore_minMax(&gameCopy, playerA, currDepth + 1, maxDepth, alpha, beta, &turnCopy);
                if (maximizing)
                {
                    if (scores[cpt] > value)
                    {
                        value = scores[cpt];
                        bestMoveIndex = cpt;
                        alpha = (float)(fmax(alpha, value));
                    }
                }
                else
                {
                    if (scores[cpt] < value)
                    {
                        value = scores[cpt];
                        bestMoveIndex = cpt;
                        beta = (float)(fmin(beta, value));
                    }
                }

                cpt++;

                if (beta <= alpha)
                    goto pruning;

            }
        }

    }


    int flag = -1;
    if (maximizing)
    {
        for (int i = 0; i < cpt; i++)
        {
            if (scores[i] > value)
            {
                flag = i;
                value = scores[i];
            }
        }
    }
    else
    {
        for (int i = 0; i < cpt; i++)
        {
            if (scores[i] < value)
            {
                flag = i;
                value = scores[i];
            }
        }
    }

    *turn = potential[flag];

pruning:
    if (bestMoveIndex != -1)
        *turn = potential[bestMoveIndex];


    return value;   
}

QuoridorTurn QuoridorCore_computeTurn(QuoridorCore* self, int depth, void* aiData)
{
    QuoridorTurn childTurn = { 0 };

    if (self->state != QUORIDOR_STATE_IN_PROGRESS) return childTurn;

    const float alpha = -INFINITY;
    const float beta = INFINITY;
    float childValue = QuoridorCore_minMax(self, self->playerID, 0, depth, alpha, beta, &childTurn);
    return childTurn;
}