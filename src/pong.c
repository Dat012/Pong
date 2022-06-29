#include<stdio.h>
#include <termios.h>
#include <unistd.h>

void turn_off_canon(void) {
    struct termios uncanon_s;
    tcgetattr(0, &uncanon_s);
    uncanon_s.c_lflag &= ~(ECHO | ICANON | IEXTEN);
    uncanon_s.c_cc[VMIN] = 0;
    uncanon_s.c_cc[VTIME] = 0;
    tcsetattr(0, TCSAFLUSH, &uncanon_s);
}

//  1 аргумент - левая тарелка; 2 аргумент - правая тарелка; 3 аргумент y мяча; 4 аргумент x мяча
void draw(int yt1, int yt2, int ym, int xm, int c1, int c2);

int main(void) {
    turn_off_canon();
    char step;  // первый ход любого игрока

    int c1 = 0;  // счет первого игрока
    int c2 = 0;  // счет второго игрока

    int yt1 = 12;  // x левой тарелки изначально в центре поля
    int yt2 = 12;  // x правой тарелки изначально в центре поля
    int xm = 39;  // x мяча изначально в центре поля
    int ym = 12;  // y мяча изначально в центре поля

    int W = 2;  // Количество символов проходимое мячом по оси X за 1 ход
    int H = 1;  // Количество символов проходимое мячом по оси Y за 1 ход

    int mvx = W;  // Переменная задает движение мяча по оси X
    int mvy = -H;  // Переменная задает движение мяча по оси Y

    draw(yt1, yt2, ym, xm, c1, c2);

    xm += mvx;  // Первое движение мяча - это 1 шаг под 45 градусов влево, поэтому делаем -1 x
    ym += mvy;  // И -1 y

    while (1) {
        read(0, &step, 1);
        // в зависимости от нажатой кнопки смещаем тарелку; а - левая тарелка вверх
        if (step == 'a' && yt1 >= 3) {
            yt1 -= 1;
        } else if (step == 'z' && yt1 <= 21) {  // z - левая тарелка вниз
            yt1 += 1;
        } else if (step == 'k' && yt2 >= 3) {  // k - правая тарелка вверх
            yt2 -= 1;
        } else if (step == 'm' && yt2 <= 21) {  // m - правая тарелка вниз
            yt2 += 1;
        }
        // Обработка столкновений мяча
        // Стены
        if (ym - 1 <= 0 && mvx < 0 && mvy < 0) {  // 1 отражение от вверхней стены вниз
            mvx = W * -1;
            mvy = H;
        } else if (ym + 1 >= 24 && mvx > 0 && mvy > 0) {  // 5 отражение от нижней стены вверх
            mvx = W;
            mvy = H * -1;
        } else if (ym - 1 <= 0 && mvx > 0 && mvy < 0) {  // 6
            mvx = W;
            mvy = H;
        } else if (ym + 1 >= 24 && mvx < 0 && mvy > 0) {  // 7
            mvx = W * -1;
            mvy = H * -1;
        } else if (xm - 1 <= 3 && (ym == yt1 - 1 || ym == yt1 - 2)) {
            // Тарелка левая // 2 отражение вправо вверх
            mvx = W;
            mvy = H * -1;
        } else if (xm - 1 <= 3 && ym == yt1) {  // 3 отражение в центр вправо
            mvx = W;
            mvy = 0;
        } else if (xm - 1 <= 3 && (ym == yt1 + 1 || ym == yt1 + 2)) {  // 4 отражение вправо вниз
            mvx = W;
            mvy = H;
        } else if (xm + 1 >= 76 && (ym == yt2 - 1 || ym == yt2 - 2)) {
            // Тарелка правая // 8 отражение влево вверх
            mvx = W * -1;
            mvy = H * -1;
        } else if (xm + 1 >= 76 && ym == yt2) {  // отражение влево в центр
            mvx = W * -1;
            mvy = 0;
        } else if (xm + 1 >= 76 && (ym == yt2 + 1 || ym == yt2 + 2)) {  // отражение влево вниз
            mvx = W * -1;
            mvy = H;
        } else if (xm + 1 >= 78) {  // 2 отражение вправо вверх
            c1++;
            yt1 = 12;
            yt2 = 12;
            xm = 39;
            ym = 12;
            mvx *= -1;
            mvy *= -1;
        } else if (xm - 1 <= 0) {  // 2 отражение вправо вверх
            c2++;
            yt1 = 12;
            yt2 = 12;
            xm = 39;
            ym = 12;
            mvx *= -1;
            mvy *= -1;
        }

        printf("\e[H\e[2J\e[3J");  // стираем поле

        usleep(50000);
        draw(yt1, yt2, ym, xm, c1, c2);  // и сразу рисуем новое

        xm += mvx;
        ym += mvy;

        if (c1 == 21) {
            printf("Поздравляем! Победил игрок слева\n");
            break;
        } else if (c2 == 21) {
            printf("Поздравляем! Победил игрок справа\n");
            break;
        }
    }

    return 0;
}

void draw(int yt1, int yt2, int ym, int xm, int c1, int c2) {
    for (int y = 0; y < 25; y++) {
        for (int x = 0; x < 80; x++) {
            if (y == 0 || y == 24) {  // Верхняя / нижняя граница поля
                printf("=");
            } else if (x == 0 || x == 79) {  // Левая / правая граница поля
                printf("|");
            } else if (x == 2 && (y == yt1 - 1 || y == yt1 || y == yt1 + 1)) {  // Левая тарелка
                printf("\u2588");
            } else if (x == 77 && (y == yt2 - 1 || y == yt2 || y == yt2 + 1)) {  // Правая тарелка
                printf("\u2588");
            } else if (y == ym && x == xm) {
                printf("\u2B24");
            } else if (x == 39) {  // Центр поля из точек
                if (y == ym && xm == 39) {
                    // Если координата x отрисовки совпадает с координатой мяча и он находится в центре,
                    // рисуем его вместо точки
                    printf("\u2B24");
                } else {  // Если мяч все же не в центре, рисуем точку в центре
                    printf(".");
                }
            } else {
                printf(" ");  // Пустое простанство в незанятых координатах
            }
        }
        printf("\n");
    }
    printf("Player1: %d                               Player2: %d\n", c1, c2);
}
