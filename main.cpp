#include <iostream>
#include <string>
#include <vector>
#include <algorithm>
#include <cstdint>

using namespace std;
#define DIMENSION 3

int **get_board(int **copy_board = nullptr) {
    int **board = new int *[DIMENSION];
    for (int i = 0; i < DIMENSION; i++) {
        board[i] = new int[DIMENSION];
    }

    if (copy_board) {
        for (int i = 0; i < DIMENSION; i++) {
            for (int j = 0; j < DIMENSION; j++) {
                board[i][j] = copy_board[i][j];
            }
        }
    }

    return board;
}

void delete_board(int **board) {
    for (int i = 0; i < DIMENSION; i++) {
        delete[] board[i];
    }
    delete[] board;
}


int get_left_neighbour(int **board, const int x, const int y) {
    return y == 0 ? 0 : board[x][y - 1];
}

int get_right_neighbour(int **board, const int x, const int y) {
    return y == 2 ? 0 : board[x][y + 1];
}

int get_up_neighbour(int **board, const int x, const int y) {
    return x == 0 ? 0 : board[x - 1][y];
}

int get_down_neighbour(int **board, const int x, const int y) {
    return x == 2 ? 0 : board[x + 1][y];
}

void remove_left_neighbour(int **board, const int x, const int y) {
    board[x][y - 1] = 0;
}

void remove_right_neighbour(int **board, const int x, const int y) {
    board[x][y + 1] = 0;
}

void remove_up_neighbour(int **board, const int x, const int y) {
    board[x - 1][y] = 0;
}

void remove_down_neighbour(int **board, const int x, const int y) {
    board[x + 1][y] = 0;
}

std::vector<uint8_t> get_capture_possibilities(int **board, const int x, const int y) {
    // up + right + down + left - 1111 <-> 15
    // up + right + down        - 1110 <-> 14
    // up + right + left        - 1101 <-> 13
    // up + down + left         - 1011 <-> 11
    // right + down + left      - 0111 <-> 7
    // up + right               - 1100 <-> 12
    // up + down                - 1010 <-> 10
    // right + down             - 0110 <-> 6
    // up + left                - 1001 <-> 9
    // right + left             - 0101 <-> 5
    // down + left              - 0011 <-> 3
    // none                     - 0000 <-> 0

    std::vector<uint8_t> possibilities = std::vector<uint8_t>();

    const int left_neighbour = get_left_neighbour(board, x, y);
    const int right_neighbour = get_right_neighbour(board, x, y);
    const int up_neighbour = get_up_neighbour(board, x, y);
    const int down_neighbour = get_down_neighbour(board, x, y);

    if (up_neighbour + right_neighbour + down_neighbour + left_neighbour < 6 && up_neighbour + right_neighbour +
        down_neighbour + left_neighbour > 4) {
        possibilities.push_back(15);
    }
    if (up_neighbour + right_neighbour + down_neighbour < 6 && up_neighbour + right_neighbour + down_neighbour > 3) {
        possibilities.push_back(14);
    }
    if (up_neighbour + right_neighbour + left_neighbour < 6 && up_neighbour + right_neighbour + left_neighbour > 3) {
        possibilities.push_back(13);
    }
    if (up_neighbour + down_neighbour + left_neighbour < 6 && up_neighbour + down_neighbour + left_neighbour > 3) {
        possibilities.push_back(11);
    }
    if (right_neighbour + down_neighbour + left_neighbour < 6 && right_neighbour + down_neighbour + left_neighbour >
        3) {
        possibilities.push_back(7);
    }
    if (up_neighbour + right_neighbour < 6 && up_neighbour + right_neighbour > 2) {
        possibilities.push_back(12);
    }
    if (up_neighbour + down_neighbour < 6 && up_neighbour + down_neighbour > 2) {
        possibilities.push_back(10);
    }
    if (right_neighbour + down_neighbour < 6 && right_neighbour + down_neighbour > 2) {
        possibilities.push_back(6);
    }
    if (up_neighbour + left_neighbour < 6 && up_neighbour + left_neighbour > 2) {
        possibilities.push_back(9);
    }
    if (right_neighbour + left_neighbour < 6 && right_neighbour + left_neighbour > 2) {
        possibilities.push_back(5);
    }
    if (down_neighbour + left_neighbour < 6 && down_neighbour + left_neighbour > 2) {
        possibilities.push_back(3);
    }
    if (possibilities.empty()) {
        possibilities.push_back(0);
    }

    return possibilities;
}

int **get_captured_board(int **board, const int x, const int y, const uint8_t variant) {
    int **new_board = get_board(board);
    int value = 0;

    if (variant == 0) {
        value = 1;
    }
    if (variant & 1) {
        // 0001 -> left
        value += get_left_neighbour(new_board, x, y);
        remove_left_neighbour(new_board, x, y);
    }
    if (variant & 2) {
        // 0010 -> down
        value += get_down_neighbour(new_board, x, y);
        remove_down_neighbour(new_board, x, y);
    }
    if (variant & 4) {
        // 0100 -> right
        value += get_right_neighbour(new_board, x, y);
        remove_right_neighbour(new_board, x, y);
    }
    if (variant & 8) {
        // 1000 -> up
        value += get_up_neighbour(new_board, x, y);
        remove_up_neighbour(new_board, x, y);
    }

    new_board[x][y] = value;

    return new_board;
}

int simulate(int **board, const int max_depth, int current_depth, int variant_count) {
    if (current_depth >= max_depth) { return variant_count; }
    current_depth++;

    for (int x = 0; x < DIMENSION; x++) {
        for (int y = 0; y < DIMENSION; y++) {
            if (board[x][y] == 0) {
                variant_count++;

                std::vector<uint8_t> possibilities = get_capture_possibilities(board, x, y);

                for (int i = 0; i < possibilities.size(); i++) {
                    int **new_board = get_captured_board(board, x, y, possibilities[i]);
                    variant_count += simulate(new_board, max_depth, current_depth, variant_count) % (2 << 30);
                    delete_board(new_board);
                }
            }
        }
    }

    return variant_count;
}

int main() {
    int depth;
    cin >> depth;
    cin.ignore();
    cerr << "depth: " << depth << endl;

    int **board = get_board();

    for (int i = 0; i < DIMENSION; i++) {
        for (int j = 0; j < DIMENSION; j++) {
            int value;
            cin >> value;
            cin.ignore();

            board[i][j] = value;

            cerr << board[i][j] << " ";
        }
        cerr << endl;
    }

    int result = simulate(board, depth, 0, 0);
    delete_board(board);

    cout << result << endl;
}
