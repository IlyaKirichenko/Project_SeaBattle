#define _CRT_SECURE_NO_WARNINGS
#include <iostream>
#include <string>
#include <cmath>
#include <cstdlib>
#include <ctime>
#include <windows.h>
#include <fcntl.h>
#include <io.h>

using namespace std;

void SetColor(int color) {
    HANDLE hConsole = GetStdHandle(STD_OUTPUT_HANDLE);
    SetConsoleTextAttribute(hConsole, color);
}

class Ship {
public:
    int size;
    int hits;
    bool isAlive() const { return hits < size; }
    Ship(int s) : size(s), hits(0) {}
};

class SeabattleGame {
private:
    Ship* field[10][10];
    Ship* ships[10];
    int shipCount;
    int shipLimits[5];
    int shipPlaced[5];

public:
    int score;

    SeabattleGame() {
        for (int y = 0; y < 10; y++)
            for (int x = 0; x < 10; x++)
                field[x][y] = nullptr;

        shipLimits[0] = 0; shipLimits[1] = 4; shipLimits[2] = 3; shipLimits[3] = 2; shipLimits[4] = 1;

        for (int i = 0; i < 5; i++) shipPlaced[i] = 0;
        for (int i = 0; i < 10; i++) ships[i] = nullptr;

        shipCount = 0;
        score = 0;
    }

    SeabattleGame(const SeabattleGame&) = delete;
    SeabattleGame& operator=(const SeabattleGame&) = delete;

    ~SeabattleGame() {
        for (int i = 0; i < shipCount; i++) delete ships[i];
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
        if (x1 == x2) actualSize = abs(y2 - y1) + 1;
        else actualSize = abs(x2 - x1) + 1;
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
                if (field[x][y] != nullptr) return "Клетка уже занята";
        return "";
    }

    void PlaceShip(int x1, int y1, int x2, int y2, int size) {
        Ship* newShip = new Ship(size);
        for (int x = min(x1, x2); x <= max(x1, x2); x++)
            for (int y = min(y1, y2); y <= max(y1, y2); y++)
                field[x][y] = newShip;
        ships[shipCount] = newShip;
        shipCount++;
    }

    void DrawField(bool showShips) {
        HANDLE hConsole = GetStdHandle(STD_OUTPUT_HANDLE);
        cout << "   0 1 2 3 4 5 6 7 8 9" << endl;
        for (int y = 0; y < 10; y++) {
            cout << y << "  ";
            for (int x = 0; x < 10; x++) {
                if (field[x][y] == nullptr) {
                    SetConsoleTextAttribute(hConsole, 1);
                    cout << "~ ";
                    SetConsoleTextAttribute(hConsole, 7);
                }
                else {
                    Ship* s = field[x][y];
                    if (s->hits == s->size) {
                        SetConsoleTextAttribute(hConsole, 12);
                        cout << "X ";
                        SetConsoleTextAttribute(hConsole, 7);
                    }
                    else if (showShips) {
                        cout << "0 ";
                    }
                    else {
                        SetConsoleTextAttribute(hConsole, 1);
                        cout << "~ ";
                        SetConsoleTextAttribute(hConsole, 7);
                    }
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

    int Shoot(int x, int y, bool attacked[10][10]) {
        if (attacked[x][y]) return 2;
        attacked[x][y] = true;
        if (field[x][y] != nullptr) {
            field[x][y]->hits++;
            return 1;
        }
        return 0;
    }

    bool AllShipsDead() {
        for (int i = 0; i < shipCount; i++)
            if (ships[i]->isAlive()) return false;
        return true;
    }

    void ShipBorder(int x, int y, bool attacked[10][10]) {
        Ship* s = field[x][y];
        int minX = x, maxX = x;
        int minY = y, maxY = y;

        for (int sy = 0; sy < 10; sy++) {
            for (int sx = 0; sx < 10; sx++) {
                if (field[sx][sy] == s) {
                    if (sx < minX) { minX = sx; }
                    if (sx > maxX) { maxX = sx; }
                    if (sy < minY) { minY = sy; }
                    if (sy > maxY) { maxY = sy; }
                }
            }
        }
        for (int sx = minX - 1; sx <= maxX + 1; sx++) {
            for (int sy = minY - 1; sy <= maxY + 1; sy++) {
                if (sx >= 0 && sx <= 9 && sy >= 0 && sy <= 9) {
                    attacked[sx][sy] = true;
                }
            }
        }
    }

    bool ShipDead(int x, int y) {
        if (field[x][y] == nullptr) return false;
        return !field[x][y]->isAlive();
    }

    void PlaceShipsRandom() {
        int sizes[] = { 4, 3, 3, 2, 2, 2, 1, 1, 1, 1 };
        for (int i = 0; i < 10; i++) {
            bool placed = false;
            while (!placed) {
                int dir = rand() % 2;
                int x1 = rand() % 10;
                int y1 = rand() % 10;
                int x2, y2;
                if (dir == 0) { x2 = x1 + sizes[i] - 1; y2 = y1; }
                else { x2 = x1; y2 = y1 + sizes[i] - 1; }

                if (x2 > 9 || y2 > 9) continue;
                if (CheckCoordinates(x1, y1) != "") continue;
                if (CheckCoordinates(x2, y2) != "") continue;
                if (CheckBusy(x1, y1, x2, y2) != "") continue;

                PlaceShip(x1, y1, x2, y2, sizes[i]);
                shipPlaced[sizes[i]]++;
                placed = true;
            }
        }
    }

    void DrawFieldWithAttacks(bool showShips, bool attacked[10][10]) {
        HANDLE hConsole = GetStdHandle(STD_OUTPUT_HANDLE);
        cout << "  0 1 2 3 4 5 6 7 8 9" << endl;
        char array[] = { 'A','B','C','D', 'E','F', 'G','H','I','J' };
        for (int y = 0; y < 10; y++) {
            SetColor(2);
            cout << array[y] << " ";
            for (int x = 0; x < 10; x++) {
                SetColor(2);
                if (field[x][y] == nullptr) {
                    if (attacked[x][y]) {
                        SetConsoleTextAttribute(hConsole, 8);
                        cout << "* ";
                        SetConsoleTextAttribute(hConsole, 7);
                    }
                    else {
                        SetConsoleTextAttribute(hConsole, 1);
                        cout << "~ ";
                        SetConsoleTextAttribute(hConsole, 7);
                    }
                }
                else {
                    Ship* s = field[x][y];
                    if (s->hits == s->size) {
                        SetConsoleTextAttribute(hConsole, 12);
                        cout << "X ";
                        SetConsoleTextAttribute(hConsole, 7);
                    }
                    else if (attacked[x][y]) {
                        SetConsoleTextAttribute(hConsole, 12);
                        cout << "* ";
                        SetConsoleTextAttribute(hConsole, 7);
                    }
                    else if (showShips) {
                        SetConsoleTextAttribute(hConsole, 7);
                        cout << "O ";
                    }
                    else {
                        SetConsoleTextAttribute(hConsole, 1);
                        cout << "~ ";
                        SetConsoleTextAttribute(hConsole, 7);
                    }
                }
            }
            cout << endl;
        }
    }

    int SwitchLetterToDigit(char coorLetter) {
        char array[] = { 'A','B','C','D', 'E','F', 'G','H','I','J', 'a','b','c','d','e','f','g','h','i','j' };
        for (int i = 0; i < 20; i++) {
            if (coorLetter == array[i]) {
                if (i >= 10) return i - 10;
                else return i;
            }
        }
        return -1;
    }
};

SeabattleGame player;
SeabattleGame bot;

bool playerAttacked[10][10] = {}; // куда игрок уже стрелял (по боту)
bool botAttacked[10][10] = {};    // куда бот уже стрелял (по игроку)

HANDLE hPlayerTurn; // Ивент: очередь Игрока
HANDLE hBotTurn;    // Ивент: очередь Бота

// ПОТОК ИГРОКА
DWORD WINAPI PlayerThread(LPVOID lpParam) {
    while (true) {
        // Ждем сигнала, что наступил наш ход
        WaitForSingleObject(hPlayerTurn, INFINITE);

        // Если проснулись, а игра уже окончена (победил бот) - закрываем поток
        if (player.AllShipsDead() || bot.AllShipsDead()) return 0;

        system("cls");
        SetColor(14);
        cout << "<---- ПОЛЕ БОТА ---->" << endl;
        SetColor(2);
        bot.DrawFieldWithAttacks(false, playerAttacked);
        SetColor(14);
        cout << "\n<---- ВАШЕ ПОЛЕ ---->" << endl;
        SetColor(2);
        player.DrawFieldWithAttacks(true, botAttacked);

        SetColor(2);

        cout << "\nВаш ход! Введите координаты выстрела (пример - A 0):" << endl;
        int x, y;
        char coorLetter;
        cin >> coorLetter >> x;

        y = player.SwitchLetterToDigit(coorLetter);

        if (cin.fail()) {
            cin.clear();
            cin.ignore(10000, '\n');
            cout << "Введите числа от 0 до 9" << endl;
            Sleep(500);
            SetEvent(hPlayerTurn); // Перезапускаем свой же ход
            continue;
        }

        if (x < 0 || x > 9 || y < 0 || y > 9) {
            cout << "Координаты вне поля" << endl;
            Sleep(500);
            SetEvent(hPlayerTurn);
            continue;
        }

        int result = bot.Shoot(x, y, playerAttacked);
        if (result == 2) {
            cout << "Вы уже стреляли сюда!" << endl;
            Sleep(500);
            SetEvent(hPlayerTurn);
            continue;
        }
        else if (result == 1) {
            SetColor(12);
            cout << "ПОПАЛ!" << endl;
            cout << "Стреляй ещё" << endl;
            if (bot.ShipDead(x, y)) {
                SetColor(12);
                cout << "КОРАБЛЬ УНИЧТОЖЕН!!!" << endl;
                bot.ShipBorder(x, y, playerAttacked);
                SetColor(2);
            }
            SetColor(2);
            Sleep(800);

            // Проверка победы игрока
            if (bot.AllShipsDead()) {
                system("cls");
                SetColor(10);
                cout << "<---- ВЫ ПОБЕДИЛИ! ---->" << endl;
                SetColor(2);

                SetEvent(hBotTurn); // Будим бота, чтобы он увидел конец игры и тихо завершился
                return 0; // Завершаем работу потока игрока
            }
            SetEvent(hPlayerTurn); // Ходим ещё раз
            continue;
        }
        else {
            SetColor(8);
            cout << "Мимо." << endl;
            SetColor(2);
            Sleep(1500);
            SetEvent(hBotTurn); // Сигнализируем, что теперь очередь Бота
        }
    }
    return 0;
}

// ПОТОК БОТА
DWORD WINAPI BotThread(LPVOID lpParam) {
    int botLastHitX = -1, botLastHitY = -1;
    char array[] = { 'A','B','C','D', 'E','F', 'G','H','I','J' };

    while (true) {
        // Ждем сигнала, что наступил наш ход
        WaitForSingleObject(hBotTurn, INFINITE);

        // Если проснулись, а игра уже окончена (победил игрок) - закрываем поток
        if (player.AllShipsDead() || bot.AllShipsDead()) return 0;

        while (true) { // Внутренний цикл добивания
            int bx, by;
            if (botLastHitX != -1) {
                if (!botAttacked[botLastHitX - 1][botLastHitY] && botLastHitX - 1 >= 0) { bx = botLastHitX - 1; by = botLastHitY; } //влево
                else if (!botAttacked[botLastHitX + 1][botLastHitY] && botLastHitX + 1 <= 9) { bx = botLastHitX + 1; by = botLastHitY; } //вправо
                else if (!botAttacked[botLastHitX][botLastHitY - 1] && botLastHitY - 1 >= 0) { bx = botLastHitX;     by = botLastHitY - 1; } //вверх
                else if (!botAttacked[botLastHitX][botLastHitY + 1] && botLastHitY + 1 <= 9) { bx = botLastHitX;     by = botLastHitY + 1; } //вниз
                else { botLastHitX = -1; botLastHitY = -1; }
            }
            if (botLastHitX == -1) {
                do {
                    bx = rand() % 10;
                    by = rand() % 10;
                } while (botAttacked[bx][by]);
            }

            int botResult = player.Shoot(bx, by, botAttacked);
            if (botResult == 1) {
                botLastHitX = bx; botLastHitY = by; //запоминаем попадание
                SetColor(12);
                cout << "\nБот выстрелил в (" << array[by] << ", " << bx << ") - ПОПАЛ!" << endl;
                if (player.ShipDead(bx, by)) {
                    botLastHitX = -1; botLastHitY = -1; //корабль потоплен
                    cout << "Бот потопил корабль!" << endl;
                    player.ShipBorder(bx, by, botAttacked);
                }
                SetColor(2);
                Sleep(1000);

                // Проверяем победу бота
                if (player.AllShipsDead()) {
                    system("cls");
                    SetColor(12);
                    cout << "<---- БОТ ПОБЕДИЛ! ---->" << endl;
                    SetColor(2);

                    SetEvent(hPlayerTurn); // Будим игрока, чтобы он тоже завершился
                    return 0; // Завершаем работу потока бота
                }
                continue; //попал - ходит ещё раз 
            }
            else {
                cout << "\nБот выстрелил в (" << array[by] << ", " << bx << ") - мимо." << endl;
                Sleep(1500);
                SetEvent(hPlayerTurn); // Сигнализируем, что теперь очередь Игрока
                break;
            }
        }
    }
    return 0;
}


int main() {
    setlocale(LC_ALL, "Russian");
    srand(time(0));
    time_t startTime = time(nullptr);

    // ... РАССТАНОВКА КОРАБЛЕЙ В ОСНОВНОМ ПОТОКЕ ...
    system("cls");
    SetColor(14);
    cout << "<---- ВАШЕ ПОЛЕ ---->" << endl;
    SetColor(2);

    bool dummy[10][10] = {};
    player.DrawFieldWithAttacks(true, dummy);

    SetColor(14);
    cout << "\nВыберите тип корабля:" << endl;
    SetColor(2);
    cout << "1 - Расставить корабли в ручную" << endl;
    cout << "2 - Расставить всё рандомно" << endl;
    cout << "0 - Выйти" << endl;
    cout << "Введите число:" << endl;
    int size, menu;
    cin >> menu;

    if (menu == 0) return 0;

    if (menu == 1) {
        while (true) {
            system("cls");
            SetColor(14);
            cout << "<---- ВАШЕ ПОЛЕ ---->" << endl;
            SetColor(2);
            bool dummy[10][10] = {};
            player.DrawFieldWithAttacks(true, dummy);
            SetColor(14);
            cout << "\nВыберите тип корабля:" << endl;
            SetColor(2);

            cout << "1 - Однопалубный" << endl;
            cout << "2 - Двухпалубный" << endl;
            cout << "3 - Трёхпалубный" << endl;
            cout << "4 - Четырёхпалубный" << endl;
            cout << "0 - Выйти" << endl;
            cout << "Введите число:" << endl;

            cin >> size;

            if (cin.fail()) {
                cin.clear();
                cin.ignore(10000, '\n');
                if (size == (char)size) { cout << "Должно быть числом" << endl; Sleep(500); continue; }
                cout << "Введите число от 1 до 5" << endl;
                cin.get(); continue;
            }

            if (size == 0) return 0;

            if (size < 1 || size > 4) { cout << "Число должно быть от 1 до 4" << endl; cin.get(); Sleep(500); continue; }

            string msg = player.CheckShipLimit(size);
            if (msg != "") { cout << msg << endl; cin.get(); continue; }

            if (size == 1) {
                cout << "Введите координаты (A 0):" << endl;
                int x, y; char coortLetter;
                cin >> coortLetter >> x;

                y = player.SwitchLetterToDigit(coortLetter);

                if (cin.fail()) {
                    cin.clear(); cin.ignore(10000, '\n');
                    cout << "Координаты должны быть числами от 0 до 9" << endl;
                    Sleep(500); continue;
                }

                msg = player.CheckCoordinates(x, y);
                if (msg != "") { cout << msg << endl;  Sleep(500); continue; }

                msg = player.CheckBusy(x, y, x, y);
                if (msg != "") { cout << msg << endl; Sleep(500); continue; }

                player.PlaceShip(x, y, x, y, size);
                player.IncrementShipCount(size);
                cout << "Однопалубный корабль добавлен" << endl; Sleep(500);
                continue;
            }

            cout << "Введите координаты начала (пример - A 4):" << endl;
            int x1, y1, x2, y2; char wasd, coorLetter;
            cin >> coorLetter >> x1;
            cout << "Выберете направлене корабля:" << endl;
            cin >> wasd;

            if (cin.fail()) {
                cin.clear(); cin.ignore(10000, '\n');
                cout << "Координаты должны таккого формата (пример - A 4)" << endl;
                Sleep(500); continue;
            }

            y1 = player.SwitchLetterToDigit(coorLetter);

            switch (wasd) {
            case 'w': case 'W': y2 = y1 - size + 1; x2 = x1; break;
            case 's': case 'S': y2 = y1 + size - 1; x2 = x1; break;
            case 'a': case 'A': x2 = x1 - size + 1; y2 = y1; break;
            case 'd': case 'D': x2 = x1 + size - 1; y2 = y1; break;
            default: cout << "Неверное направление!" << endl; continue;
            }

            msg = player.CheckCoordinates(x1, y1); if (msg != "") { cout << msg << endl;  Sleep(500); continue; }
            msg = player.CheckCoordinates(x2, y2); if (msg != "") { cout << msg << endl;  Sleep(500); continue; }
            msg = player.CheckDirection(x1, y1, x2, y2); if (msg != "") { cout << msg << endl;  Sleep(500); continue; }
            msg = player.CheckLength(x1, y1, x2, y2, size); if (msg != "") { cout << msg << endl; Sleep(500); continue; }
            msg = player.CheckBusy(x1, y1, x2, y2); if (msg != "") { cout << msg << endl; Sleep(500); continue; }

            player.PlaceShip(x1, y1, x2, y2, size);
            player.IncrementShipCount(size);

            string shipNames[] = { "", "Однопалубный", "Двухпалубный", "Трёхпалубный", "Четырёхпалубный" };
            cout << shipNames[size] << " корабль добавлен" << endl; Sleep(500);

            if (player.AllShipsPlaced()) {
                cout << "Все корабли расставлены! Начинаем бой!" << endl;
                Sleep(500); break;
            }
        }
    }
    if (menu == 2) {
        player.PlaceShipsRandom();
        cout << "Корабли расставлены рандомно!" << endl; Sleep(500);
    }

    bot.PlaceShipsRandom();

    hPlayerTurn = CreateEventA(NULL, FALSE, TRUE, NULL);  // Игрок ходит первым (TRUE)
    hBotTurn = CreateEventA(NULL, FALSE, FALSE, NULL);    // Бот ждет (FALSE)

    HANDLE hThreads[2];
    DWORD idThread[2];

    hThreads[0] = CreateThread(NULL, 0, PlayerThread, NULL, 0, &idThread[0]);
    hThreads[1] = CreateThread(NULL, 0, BotThread, NULL, 0, &idThread[1]);

    WaitForMultipleObjects(2, hThreads, TRUE, INFINITE);

    time_t endTime = time (nullptr);
    int gameTime = difftime(endTime, startTime);

    int minutes = gameTime / 60;
    int second = gameTime % 60;

    CloseHandle(hPlayerTurn);
    CloseHandle(hBotTurn);
    for (int i = 0; i < 2; i++) {
        CloseHandle(hThreads[i]);
    }
    cout << endl;
    cout << "Время игры: " << minutes <<" минут "<< second <<" секунд" << endl;
    cout << "\nИгра окончена. Нажмите Enter для выхода...";
    cin.get(); cin.get();

    return 0;
}