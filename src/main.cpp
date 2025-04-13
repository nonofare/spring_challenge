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

#include <iostream>
#include <string>
#include <vector>
#include <bitset>
#include <cstdint>
#include <unordered_map>

#define DIM 3

class Board {
    int **field;

public:
    Board() {
        field = new int *[DIM];
        for (int i = 0; i < DIM; i++) {
            field[i] = new int[DIM]();
        }
    }

    Board(const Board &other, const int x, const int y, const int variant) {
        field = new int *[DIM];
        for (int i = 0; i < DIM; i++) {
            field[i] = new int[DIM]();
            for (int j = 0; j < DIM; j++) {
                field[i][j] = other.field[i][j];
            }
        }

        int current_square_value = 0;

        if (variant == 0) {
            current_square_value = 1;
        }
        if (variant & 0b0001) {
            if (int tmp = get_square_left(x, y)) {
                current_square_value += tmp;
                set_square_left(x, y, 0);
            }
        }
        if (variant & 0b0010) {
            if (int tmp = get_square_down(x, y)) {
                current_square_value += tmp;
                set_square_down(x, y, 0);
            }
        }
        if (variant & 0b0100) {
            if (int tmp = get_square_right(x, y)) {
                current_square_value += tmp;
                set_square_right(x, y, 0);
            }
        }
        if (variant & 0b1000) {
            if (int tmp = get_square_up(x, y)) {
                current_square_value += tmp;
                set_square_up(x, y, 0);
            }
        }

        set_square(x, y, current_square_value);
    }

    ~Board() {
        for (int i = 0; i < DIM; i++) {
            delete[] field[i];
        }
        delete[] field;
    }

    bool square_empty(const int x, const int y) const { return field[x][y] == 0; }

    int get_square(const int x, const int y) const { return field[x][y]; }
    int get_square_left(const int x, const int y) const { return y == 0 ? 0 : field[x][y - 1]; }
    int get_square_right(const int x, const int y) const { return y == 2 ? 0 : field[x][y + 1]; }
    int get_square_up(const int x, const int y) const { return x == 0 ? 0 : field[x - 1][y]; }
    int get_square_down(const int x, const int y) const { return x == 2 ? 0 : field[x + 1][y]; }

    void set_square(const int x, const int y, const int value) { field[x][y] = value; }
    void set_square_left(const int x, const int y, const int value) { field[x][y - 1] = value; }
    void set_square_right(const int x, const int y, const int value) { field[x][y + 1] = value; }
    void set_square_up(const int x, const int y, const int value) { field[x - 1][y] = value; }
    void set_square_down(const int x, const int y, const int value) { field[x + 1][y] = value; }

    bool has_free_square() const {
        for (int i = 0; i < DIM; i++) {
            for (int j = 0; j < DIM; j++) {
                if (field[i][j] == 0) { return true; }
            }
        }
        return false;
    }

    uint64_t get_int_repr() const {
        uint64_t num = 0;

        for (int i = 0; i < DIM; i++) {
            for (int j = 0; j < DIM; j++) {
                num = num * 10 + field[i][j];
            }
        }

        // debug
        // std::cerr << "zwracam wynik: " << num << std::endl << std::endl;
        // debug

        return num;
    }

    std::string get_str_repr() const {
        std::string str;

        for (int i = 0; i < DIM; i++) {
            for (int j = 0; j < DIM; j++) {
                str += std::to_string(field[i][j]) + " ";
            }
            str += "\n";
        }
        str += "\n";

        return str;
    }

    std::vector<int> get_capture_possibilities(const int x, const int y) const {
        std::vector<int> possibilities = std::vector<int>();

        int u = get_square_up(x, y);
        int r = get_square_right(x, y);
        int d = get_square_down(x, y);
        int l = get_square_left(x, y);

        if (u < 1) { u = 1000; }
        if (r < 1) { r = 1000; }
        if (d < 1) { d = 1000; }
        if (l < 1) { l = 1000; }

        if (u + r + d + l <= 6) { possibilities.push_back(0b1111); }
        if (u + r + d + 0 <= 6) { possibilities.push_back(0b1110); }
        if (u + r + 0 + l <= 6) { possibilities.push_back(0b1101); }
        if (u + 0 + d + l <= 6) { possibilities.push_back(0b1011); }
        if (0 + r + d + l <= 6) { possibilities.push_back(0b0111); }
        if (u + r + 0 + 0 <= 6) { possibilities.push_back(0b1100); }
        if (u + 0 + d + 0 <= 6) { possibilities.push_back(0b1010); }
        if (0 + r + d + 0 <= 6) { possibilities.push_back(0b0110); }
        if (u + 0 + 0 + l <= 6) { possibilities.push_back(0b1001); }
        if (0 + r + 0 + l <= 6) { possibilities.push_back(0b0101); }
        if (0 + 0 + d + l <= 6) { possibilities.push_back(0b0011); }
        if (possibilities.empty()) { possibilities.push_back(0b000); }

        return possibilities;
    }
};

template<typename Hash>
uint64_t simulate(const Board *board, const int max_depth, int current_depth,
                  std::unordered_map<std::pair<uint64_t, int>, uint64_t, Hash> &cache) {
    if (current_depth >= max_depth) {
        // debug
        // std::cerr << "zbyt gleboko" << std::endl;
        // debug
        return board->get_int_repr();
    }

    std::pair<uint64_t, int> key = {board->get_int_repr(), current_depth};
    if (cache.find(key) != cache.end()) { return cache[key]; }

    current_depth++;

    // debug
    // std::cerr << "poziom: " << current_depth - 1 << "/" << max_depth << std::endl;
    // std::cerr << board->get_str_repr() << std::endl;
    // debug

    if (board->has_free_square() == false) {
        //debug
        // std::cerr << "brak wolnych miejsc" << std::endl;
        // debug
        return board->get_int_repr();
    }

    uint64_t board_values_sum = 0;

    for (int i = 0; i < DIM; i++) {
        for (int j = 0; j < DIM; j++) {
            if (board->square_empty(i, j)) {
                std::vector<int> possibilities = board->get_capture_possibilities(i, j);

                // debug
                // std::cerr << "nastepne rekurencje dla: [" << i << ", " << j << "] -> ";
                // for (int k = 0; k < possibilities.size(); k++) {
                //     std::cerr << std::bitset<4>(possibilities[k]) << ", ";
                // }
                // std::cerr << std::endl << std::endl;
                // debug

                for (int k = 0; k < possibilities.size(); k++) {
                    //debug
                    // std::cerr << "wykonuje: " << std::bitset<4>(possibilities[k]) << std::endl;
                    //debug
                    Board *new_possible_board = new Board(*board, i, j, possibilities[k]);
                    board_values_sum =
                            board_values_sum + simulate(new_possible_board, max_depth, current_depth, cache) &
                            0x3FFFFFFF;
                    delete new_possible_board;
                }
            }
        }
    }

    cache[key] = board_values_sum;
    return board_values_sum;
}

int main() {
    int depth;
    std::cin >> depth;
    std::cin.ignore();

    Board *board = new Board();

    for (int i = 0; i < DIM; i++) {
        for (int j = 0; j < DIM; j++) {
            int value;
            std::cin >> value;
            std::cin.ignore();

            board->set_square(i, j, value);
        }
    }

    auto pair_hash = [](const std::pair<uint64_t, int> &p) -> std::size_t {
        return std::hash<uint64_t>()(p.first) ^ std::hash<int>()(p.second);
    };

    // mapa<
    //     para<
    //         liczbowa reprezentacja planszy
    //         glebokosc
    //         >
    //     suma koncowych wartosci plansz
    //     >
    std::unordered_map<std::pair<uint64_t, int>, uint64_t, decltype(pair_hash)> cache(0, pair_hash);
    cache.reserve(1000000);

    std::cout << simulate(board, depth, 0, cache) << std::endl;

    delete board;
}
