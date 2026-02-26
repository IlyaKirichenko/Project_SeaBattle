#include <iostream>
#include <string>
#include <cmath>
#include <windows.h>

using namespace std;

void SetColor(int color) {
    HANDLE hConsole = GetStdHandle(STD_OUTPUT_HANDLE);
    SetConsoleTextAttribute(hConsole, color);
}

class SeabattleGame {
private:
    char field[10][10];
    int shipLimits[5];
    int shipPlaced[5];

public:
    SeabattleGame() {
        for (int y = 0; y < 10; y++)
            for (int x = 0; x < 10; x++)
                field[x][y] = '~';

        shipLimits[0] = 0; shipLimits[1] = 4; shipLimits[2] = 3; shipLimits[3] = 2; shipLimits[4] = 1;
        for (int i = 0; i < 5; i++) shipPlaced[i] = 0;
    }

    string CheckShipSize(int size) {
        if (size < 1 || size > 4) return "Введите число от 1 до 4";
        return "";
    }

    string CheckShipLimit(int size) {
        if (shipPlaced[size] >= shipLimits[size]) return "Достигнуто максимальное количество кораблей этого типа";
        return "";
    }

    string CheckCoordinates(int x, int y) {
        if (x < 0 || x > 9 || y < 0 || y > 9) return "Координаты вне поля";
        return "";
    }

    string CheckDirection(int x1, int y1, int x2, int y2) {
        if (x1 != x2 && y1 != y2) return "Корабль должен быть по горизонтали или вертикали";
        return "";
    }

    string CheckLength(int x1, int y1, int x2, int y2, int expectedSize) {
        int actualSize;
        if (x1 == x2) {
            actualSize = abs(y2 - y1) + 1;
        }
        else {
            actualSize = abs(x2 - x1) + 1;
        }
        if (actualSize != expectedSize)
            return "Длина корабля должна быть " + to_string(expectedSize) + " клетки";
        return "";
    }

    string CheckBusy(int x1, int y1, int x2, int y2) {
        int startX = max(0, min(x1, x2) - 1);
        int endX = min(9, max(x1, x2) + 1);
        int startY = max(0, min(y1, y2) - 1);
        int endY = min(9, max(y1, y2) + 1);

        for (int x = startX; x <= endX; x++)
            for (int y = startY; y <= endY; y++)
                if (field[x][y] != '~')
                    return "Клетка уже занята";
        return "";
    }

    void PlaceShip(int x1, int y1, int x2, int y2) {
        for (int x = min(x1, x2); x <= max(x1, x2); x++)
            for (int y = min(y1, y2); y <= max(y1, y2); y++)
                field[x][y] = '0';
    }

    void DrawField() {
        HANDLE hConsole = GetStdHandle(STD_OUTPUT_HANDLE); 

        cout << "   0 1 2 3 4 5 6 7 8 9" << endl;
        for (int y = 0; y < 10; y++) {
            cout << y << "  ";
            for (int x = 0; x < 10; x++) {
                if (field[x][y] == '~') {
                    SetConsoleTextAttribute(hConsole, 1); 
                    cout << '~' << " ";
                    SetConsoleTextAttribute(hConsole, 7); 
                }
                else {
                    cout << field[x][y] << " ";
                }
            }
            cout << endl;
        }
    }

    bool AllShipsPlaced() {
        for (int i = 1; i <= 4; i++)
            if (shipPlaced[i] != shipLimits[i]) return false;
        return true;
    }

    void IncrementShipCount(int size) {
        shipPlaced[size]++;
    }
};

int main() {
    SeabattleGame game;
    setlocale(LC_ALL, "Russian");
    while (true) {
        system("cls");
        SetColor(7);
        game.DrawField();
        SetColor(2);
        cout << "\nВыберите тип корабля:" << endl;
        cout << "1 - Однопалубный" << endl;
        cout << "2 - Двухпалубный" << endl;
        cout << "3 - Трёхпалубный" << endl;
        cout << "4 - Четырёхпалубный" << endl;
        cout << "0 - Выйти" << endl;
        cout << "Введите число:" << endl;
        int size;
        if (!(cin >> size)) {
            cin.clear();
            cin.ignore(10000, '\n');
            cout << "Введите число от 1 до 4" << endl;
            cin.get();
            continue;
        }

        if (size == 0) break;

        string msg = game.CheckShipSize(size);
        if (msg != "") {
            cout << msg << endl;
            cin.get(); cin.get();
            continue;
        }

        msg = game.CheckShipLimit(size);
        if (msg != "") {
            cout << msg << endl;
            cin.get(); cin.get();
            continue;
        }

        if (size == 1) {
            cout << "Введите координаты (x y):" << endl;
            int x, y;
            cin >> x >> y;

            if (cin.fail()) {
                cin.clear();
                cin.ignore(10000, '\n');
                cout << "Координаты должны быть числами от 0 до 9" << endl;
                cin.get(); cin.get();
                continue;
            }

            msg = game.CheckCoordinates(x, y);
            if (msg != "") {
                cout << msg << endl;
                cin.get(); cin.get();
                continue;
            }

            msg = game.CheckBusy(x, y, x, y);
            if (msg != "") {
                cout << msg << endl;
                cin.get(); cin.get();
                continue;
            }

            game.PlaceShip(x, y, x, y);
            game.IncrementShipCount(size);
            cout << "Однопалубный корабль добавлен" << endl;
            cin.get(); cin.get();
            continue;
        }

        cout << "Введите координаты начала (x1 y1):" << endl;
        int x1, y1, x2, y2;
        cin >> x1 >> y1;

        cout << "Введите координаты конца (x2 y2):" << endl;
        cin >> x2 >> y2;

        if (cin.fail()) {
            cin.clear();
            cin.ignore(10000, '\n');
            cout << "Координаты должны быть числами от 0 до 9" << endl;
            cin.get(); cin.get();
            continue;
        }

        msg = game.CheckCoordinates(x1, y1);
        if (msg != "") { cout << msg << endl; cin.get(); cin.get(); continue; }
        msg = game.CheckCoordinates(x2, y2);
        if (msg != "") { cout << msg << endl; cin.get(); cin.get(); continue; }
        msg = game.CheckDirection(x1, y1, x2, y2);
        if (msg != "") { cout << msg << endl; cin.get(); cin.get(); continue; }
        msg = game.CheckLength(x1, y1, x2, y2, size);
        if (msg != "") { cout << msg << endl; cin.get(); cin.get(); continue; }
        msg = game.CheckBusy(x1, y1, x2, y2);
        if (msg != "") { cout << msg << endl; cin.get(); cin.get(); continue; }

        game.PlaceShip(x1, y1, x2, y2);
        game.IncrementShipCount(size);

        string shipNames[] = { "", "Однопалубный", "Двухпалубный", "Трёхпалубный", "Четырёхпалубный" };
        cout << shipNames[size] << " корабль добавлен" << endl;
        cin.get(); cin.get();

        if (game.AllShipsPlaced()) {
            cout << "Игра завершена" << endl;
            cin.get();
            break;
        }
    }

    return 0;
}