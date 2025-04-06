#include <iostream>
#include <string>
#include <vector>
#include <algorithm>
#include <bitset>

using namespace std;

// reprezentuje maksymalna wysokosc i szerokosc planszy
#define DIM 3

class Board {
    // reprezentuje plansze 2d
    int **field;

public:
    // konstruktor uzywany tylko raz w main()
    Board() {
        field = new int *[DIM];
        for (int i = 0; i < DIM; i++) {
            field[i] = new int[DIM]();
        }
    }

    // konstruktor uzywany w kazdej rekurencji simulate(), przyjmuje wartosci po poprzedniej tablicy i ustawia nowy wariant
    Board(const Board &other, const int x, const int y, const int variant) {
        // alokujemy pamiec i przepisujemy wartosci z przekazanej tablicy "other"
        field = new int *[DIM];
        for (int i = 0; i < DIM; i++) {
            field[i] = new int[DIM]();
            for (int j = 0; j < DIM; j++) {
                field[i][j] = other.field[i][j];
            }
        }

        // value reprezentuje wartosc oczek nowo dodanej kostki
        int value = 0;

        // sprawdzamy 8 bitowa zmienna "variant", dla kazdego ustawionego bitu zerujemy odpowiediego sasiada i dodamy jego wartosc do zmiennej value
        // przykladowo jezli zmienna variant == [0000 0101] to sasiad lewy i prawy zostanie pochloniety przez nowa kostke
        // jezeli variant == [0000 0000] to poprostu wstawiamy kostke z wartoscia 1
        if (variant == 0) {
            value = 1;
        }
        if (variant & 1) {
            // musimy tez sprawdzic czy sasiad istnieje, aby nie wyzerowac pola poza obrebem planszy
            if (int tmp = get_square_left(x, y)) {
                // jezeli variant obejmuje danego sasiada to dodajemy jego wartosc do zmiennej value
                value += tmp;
                // jego wartosc zostaje ustawiona na 0
                set_square_left(x, y, 0);
            }
        }
        if (variant & 2) {
            if (int tmp = get_square_down(x, y)) {
                value += tmp;
                set_square_down(x, y, 0);
            }
        }
        if (variant & 4) {
            if (int tmp = get_square_right(x, y)) {
                value += tmp;
                set_square_right(x, y, 0);
            }
        }
        if (variant & 8) {
            if (int tmp = get_square_up(x, y)) {
                value += tmp;
                set_square_up(x, y, 0);
            }
        }

        // ustawiamy wartosc nowo dodanej kostki na sume pozbieranych wartosci
        set_square(x, y, value);
    }

    ~Board() {
        for (int i = 0; i < DIM; i++) {
            delete[] field[i];
        }
        delete[] field;
    }

    bool square_empty(int x, int y) const { return field[x][y] == 0; }

    int get_square(int x, int y) const { return field[x][y]; }
    int get_square_left(int x, int y) const { return y == 0 ? 0 : field[x][y - 1]; }
    int get_square_right(int x, int y) const { return y == 2 ? 0 : field[x][y + 1]; }
    int get_square_up(int x, int y) const { return x == 0 ? 0 : field[x - 1][y]; }
    int get_square_down(int x, int y) const { return x == 2 ? 0 : field[x + 1][y]; }

    void set_square(int x, int y, int value) { field[x][y] = value; }
    void set_square_left(int x, int y, int value) { field[x][y - 1] = value; }
    void set_square_right(int x, int y, int value) { field[x][y + 1] = value; }
    void set_square_up(int x, int y, int value) { field[x - 1][y] = value; }
    void set_square_down(int x, int y, int value) { field[x + 1][y] = value; }

    long long get_board_value() const {
        long long num = 0;

        for (int i = 0; i < DIM; i++) {
            for (int j = 0; j < DIM; j++) {
                num = num * 10 + field[i][j];
            }
        }

        // debug
        cerr << "zwracam wynik: " << num << endl << endl;
        // debug

        return num;
    }

    void print() const {
        for (int i = 0; i < DIM; i++) {
            for (int j = 0; j < DIM; j++) {
                cerr << field[i][j] << " ";
            }
            cerr << endl;
        }
        cerr << endl;
    }

    // wyznacza wektor mozliwych przejec, jakie kostka wstawiona nadane pole jest w stanie wykonac
    std::vector<int> get_capture_possibilities(const int x, const int y) const {
        // kazda mozliwosc jest reprezentowana jako liczba 4 bitowa, kazdy bit reprezentuje jeden kierunek
        // 1 jezeli kostka w danym kierunku moze zostac przejeta, 0 jezeli nie

        std::vector<int> possibilities = std::vector<int>();

        int u = get_square_up(x, y);
        int r = get_square_right(x, y);
        int d = get_square_down(x, y);
        int l = get_square_left(x, y);

        // zabezpiecznie przed zajmowaniem pol na ktorych nie ma kostek
        if (u < 1) { u = 1000; }
        if (r < 1) { r = 1000; }
        if (d < 1) { d = 1000; }
        if (l < 1) { l = 1000; }

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
        if (u + r + d + l <= 6) { possibilities.push_back(15); }
        if (u + r + d + 0 <= 6) { possibilities.push_back(14); }
        if (u + r + 0 + l <= 6) { possibilities.push_back(13); }
        if (u + 0 + d + l <= 6) { possibilities.push_back(11); }
        if (0 + r + d + l <= 6) { possibilities.push_back(7); }
        if (u + r + 0 + 0 <= 6) { possibilities.push_back(12); }
        if (u + 0 + d + 0 <= 6) { possibilities.push_back(10); }
        if (0 + r + d + 0 <= 6) { possibilities.push_back(6); }
        if (u + 0 + 0 + l <= 6) { possibilities.push_back(9); }
        if (0 + r + 0 + l <= 6) { possibilities.push_back(5); }
        if (0 + 0 + d + l <= 6) { possibilities.push_back(3); }
        if (possibilities.empty()) { possibilities.push_back(0); }

        return possibilities;
    }
};

long long simulate(const Board *board, const int max_depth, int current_depth) {
    if (current_depth > max_depth) {
        // debug
        cerr << "zbyt gleboko" << endl;
        // debug
        return board->get_board_value();
    }
    current_depth++;

    // debug
    cerr << "poziom: " << current_depth - 1 << "/" << max_depth << endl;
    board->print();
    // debug

    long long board_values_sum = 0;
    int free_squares = 0;

    for (int x = 0; x < DIM; x++) {
        for (int y = 0; y < DIM; y++) {
            if (board->square_empty(x, y)) {
                free_squares++;
                std::vector<int> possibilities = board->get_capture_possibilities(x, y);

                // debug
                cerr << "nastepne rekurencje dla: [" << x << ", " << y << "] -> ";
                for (int i = 0; i < possibilities.size(); i++) {
                    cerr << bitset<4>(possibilities[i]) << ", ";
                }
                cerr << endl << endl;
                // debug

                for (int i = 0; i < possibilities.size(); i++) {
                    cerr << "wykonuje: " << bitset<4>(possibilities[i]) << endl;
                    Board *new_possible_board = new Board(*board, x, y, possibilities[i]);
                    board_values_sum += simulate(new_possible_board, max_depth, current_depth);
                    board_values_sum = board_values_sum % (2 << 30);
                    delete new_possible_board;
                }
            }
        }
    }

    if (free_squares == 0) {
        // debug
        cerr << "brak wolnych miejsc" << endl;
        // debug
        return board->get_board_value();
    }

    return board_values_sum;
}

int main() {
    int depth;
    cin >> depth;
    cin.ignore();

    Board *board = new Board();

    for (int i = 0; i < DIM; i++) {
        for (int j = 0; j < DIM; j++) {
            int value;
            cin >> value;
            cin.ignore();

            board->set_square(i, j, value);
        }
    }

    int result = simulate(board, depth, 0);
    cout << result << endl;

    delete board;
}
