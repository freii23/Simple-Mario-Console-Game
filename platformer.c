#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <string.h>
#include <windows.h>

/* Константы игры */
#define MAP_WIDTH 80
#define MAP_HEIGHT 25
#define MONSTER_COUNT 8
#define CLOUD_COUNT 5
#define GOOD_COUNT 5
#define BULLET_COUNT 100

/* Булевы значения для чистого C */
#define TRUE 1
#define FALSE 0
typedef int bool;

/* Структура игрового объекта */
typedef struct {
    float x, y;
    float width, height;
    float horSpeed;
    float vertSpeed;
    bool isFly;
    char cType;
} TObject;

/* Глобальные переменные */
char map[MAP_HEIGHT][MAP_WIDTH + 1];
TObject mario;
TObject *brick = NULL;
int brickLength;
int level = 1;
int addedGood = 0;
int gettedGood = 0;
float v_sp = -1.0f;
float l_sp = 0.05f;

TObject bullet[BULLET_COUNT];
int bulletCount = 0;

TObject monster[MONSTER_COUNT];
int monstersBrick[MONSTER_COUNT] = {0, 2, 3, 3, 3, 1, 3, 2};
TObject cloud[CLOUD_COUNT];
TObject good[GOOD_COUNT];

/* Прототипы функций */
void createLevel(int lvl);
void putObjectOnMap(TObject obj);
void clearMap(void);
void showMap(void);
void setObjectPos(TObject* obj, float xPos, float yPos);
void initObject(TObject* obj, float xPos, float yPos, float oWidth, float oHeight, char inType);
bool isCollision(TObject obj1, TObject obj2);
void vertMoveObject(TObject* obj);
void horMoveObject(TObject* obj);
void shot(TObject *bullet, int dir);
void horizonMoveMap(float dx);
bool isPosInMap(int x, int y);
bool monsterOnBrick(TObject obj1, TObject obj2);
void createGood(const TObject source, TObject *obj);
void setCur(int x, int y);

/* Очистка карты */
void clearMap(void) {
    int i, j;
    
    /* Заполняем первую строку пробелами */
    for (i = 0; i < MAP_WIDTH; i++) {
        map[0][i] = ' ';
    }
    map[0][MAP_WIDTH] = '\0';
    
    /* Копируем первую строку во все остальные */
    for (j = 1; j < MAP_HEIGHT; j++) {
        strcpy(map[j], map[0]);
    }
}

/* Отображение карты */
void showMap(void) {
    int j;
    map[MAP_HEIGHT - 1][MAP_WIDTH - 1] = '\0';
    
    for (j = 0; j < MAP_HEIGHT; j++) {
        printf("%s\n", map[j]);
    }
}

/* Установка позиции объекта */
void setObjectPos(TObject* obj, float xPos, float yPos) {
    obj->x = xPos;
    obj->y = yPos;
}

/* Инициализация объекта */
void initObject(TObject* obj, float xPos, float yPos, float oWidth, float oHeight, char inType) {
    setObjectPos(obj, xPos, yPos);
    obj->width = oWidth;
    obj->height = oHeight;
    obj->vertSpeed = 0.0f;
    obj->horSpeed = 0.0f;
    obj->isFly = FALSE;
    obj->cType = inType;
}

/* Проверка коллизии между двумя объектами */
bool isCollision(TObject obj1, TObject obj2) {
    return (((obj1.x + obj1.width) > obj2.x) && 
            ((obj1.y + obj1.height) > obj2.y) && 
            ((obj2.x + obj2.width) > obj1.x) && 
            ((obj2.y + obj2.height) > obj1.y));
}

/* Вертикальное движение объекта */
void vertMoveObject(TObject* obj) {
    int i;
    obj->isFly = TRUE;
    obj->vertSpeed += l_sp;
    setObjectPos(obj, obj->x, obj->y + obj->vertSpeed);
    
    /* Проверка коллизии с блоками */
    for (i = 0; i < brickLength; i++) {
        if (isCollision(*obj, brick[i])) {
            obj->y -= obj->vertSpeed;
            obj->vertSpeed = 0.0f;
            obj->isFly = FALSE;
            
            /* Проверка на финишный блок */
            if (brick[i].cType == '+') {
                gettedGood += addedGood;
                addedGood = 0;
                level++;
                if (level > 3) {
                    level = 1;
                    gettedGood = 0;
                }
                createLevel(level);
                Sleep(1000);
            }
            break;
        }
    }
}

/* Горизонтальное движение объекта */
void horMoveObject(TObject* obj) {
    obj->x += obj->horSpeed;
}

/* Выстрел */
void shot(TObject *bullet, int dir) {
    if (dir == 1) {
        initObject(bullet, mario.x, mario.y, 1, 1, '-');
    } else if (dir == -1) {
        initObject(bullet, mario.x + mario.width - 1, mario.y, 1, 1, '-');
    }
    bullet->horSpeed = -2.0f * dir;
}

/* Движение карты по горизонтали */
void horizonMoveMap(float dx) {
    int i;
    
    /* Проверяем коллизию с блоками */
    mario.x -= dx;
    for (i = 0; i < brickLength; i++) {
        if (isCollision(mario, brick[i])) {
            mario.x += dx;
            return;
        }
    }
    mario.x += dx;

    /* Двигаем все объекты */
    for (i = 0; i < brickLength; i++) {
        brick[i].x += dx;
    }
    for (i = 0; i < MONSTER_COUNT; i++) {
        monster[i].x += dx;
    }
    for (i = 0; i < CLOUD_COUNT; i++) {
        cloud[i].x += dx;
    }
    for (i = 0; i < GOOD_COUNT; i++) {
        good[i].x += dx;
    }
}

/* Проверка, находится ли позиция в пределах карты */
bool isPosInMap(int x, int y) {
    return (x >= 0) && (x < MAP_WIDTH) && (y >= 0) && (y < MAP_HEIGHT);
}

/* Проверка, находится ли монстр на блоке */
bool monsterOnBrick(TObject obj1, TObject obj2) {
    return (obj1.x >= obj2.x) && (obj1.x + obj1.width <= obj2.x + obj2.width);
}

/* Размещение объекта на карте */
void putObjectOnMap(TObject obj) {
    int ix = (int)round(obj.x);
    int iy = (int)round(obj.y);
    int iWidth = (int)round(obj.width);
    int iHeight = (int)round(obj.height);
    int i, j;
    
    for (i = ix; i < (ix + iWidth); i++) {
        for (j = iy; j < (iy + iHeight); j++) {
            if (isPosInMap(i, j)) {
                map[j][i] = obj.cType;
            }
        }
    }
}

/* Создание бонуса */
void createGood(const TObject source, TObject *obj) {
    initObject(obj, 0.5f * (2 * source.x + source.width), 
               source.y - 3, 5, 3, '$');
    obj->vertSpeed = -1.0f;
    obj->horSpeed = 0.5f;
}

/* Создание уровня */
void createLevel(int lvl) {
    int i;
    
    /* Инициализация Марио */
    initObject(&mario, 39, 10, 3, 3, '@');
    
    /* Обнуляем монстров */
    for (i = 0; i < MONSTER_COUNT; i++) {
        initObject(&monster[i], 0, 0, 0, 0, ' ');
    }
    
    switch (lvl) {
        case 1:
            v_sp = -1.0f;
            l_sp = 0.05f;
            brickLength = 6;
            brick = (TObject*)realloc(brick, sizeof(*brick) * brickLength);
            
            /* Создание блоков */
            initObject(brick + 0, 20, 20, 55, 5, '#');
            initObject(brick + 1, 80, 15, 22, 10, '#');
            initObject(brick + 2, 120, 20, 60, 5, '#');
            initObject(brick + 3, 180, 15, 30, 10, '#');
            initObject(brick + 4, 210, 20, 40, 5, '#');
            initObject(brick + 5, 270, 11, 3, 1, '+');

            /* Создание монстров */
            initObject(monster + 0, brick[monstersBrick[0]].x, brick[monstersBrick[0]].y - 2, 3, 2, '$');
            initObject(monster + 1, brick[monstersBrick[1]].x, brick[monstersBrick[1]].y - 3, 3, 3, '$');
            initObject(monster + 2, brick[monstersBrick[2]].x, brick[monstersBrick[2]].y - 2, 3, 2, '$');

            /* Создание облаков */
            initObject(cloud + 0, 30, 10, 8, 3, '?');
            initObject(cloud + 1, 200, 8, 8, 3, '?');

            /* Установка скорости монстров */
            monster[0].horSpeed = 0.5f;
            monster[1].horSpeed = 0.5f;
            monster[2].horSpeed = 0.5f;
            break;

        case 2:
            v_sp = -1.0f;
            l_sp = 0.05f;
            brickLength = 4;
            brick = (TObject*)realloc(brick, sizeof(*brick) * brickLength);
            
            initObject(brick + 0, 20, 20, 40, 5, '#');
            initObject(brick + 1, 80, 15, 15, 13, '#');
            initObject(brick + 2, 130, 15, 20, 7, '#');
            initObject(brick + 3, 190, 15, 20, 20, '+');
            
            initObject(monster + 3, brick[monstersBrick[3]].x, brick[monstersBrick[3]].y - 2, 3, 2, '$');
            initObject(monster + 4, brick[monstersBrick[4]].x, brick[monstersBrick[4]].y - 3, 3, 3, '$');
            initObject(monster + 5, brick[monstersBrick[5]].x, brick[monstersBrick[5]].y - 2, 3, 2, '$');

            initObject(cloud + 2, 20, 10, 8, 3, '?');

            monster[3].horSpeed = 0.5f;
            monster[4].horSpeed = 0.5f;
            monster[5].horSpeed = 0.5f;
            break;

        case 3:
            initObject(&mario, 5, 4, 1, 1, '@');
            v_sp = -0.71f;
            l_sp = 0.032f;
            brickLength = 16;
            brick = (TObject*)realloc(brick, sizeof(*brick) * brickLength);
            
            initObject(brick + 0, 5, 5, 2, 30, '#');
            initObject(brick + 1, 20, 7, 2, 30, '#');
            initObject(brick + 2, 35, 9, 1, 30, '#');
            initObject(brick + 3, 46, 11, 1, 40, '#');
            initObject(brick + 4, 60, 13, 3, 30, '#');
            initObject(brick + 5, 80, 20, 4, 50, '#');
            initObject(brick + 6, 80, 1, 4, 13, '#');
            initObject(brick + 7, 100, 15, 3, 30, '#');
            initObject(brick + 8, 100, 3, 3, 8, '#');
            initObject(brick + 9, 125, 20, 3, 30, '#');
            initObject(brick + 10, 125, 3, 3, 14, '#');
            initObject(brick + 11, 142, 24, 10, 20, '#');
            initObject(brick + 12, 142, 1, 30, 20, '#');
            initObject(brick + 13, 163, 24, 40, 20, '#');
            initObject(brick + 14, 210, 18, 15, 30, '#');
            initObject(brick + 15, 226, 18, 1, 1, '+');
            break;

        default:
            v_sp = -1.0f;
            l_sp = 0.05f;
            printf("Уровень завершен!\n");
            break;
    }
}

/* Установка курсора в консоли */
void setCur(int x, int y) {
    COORD coord;
    coord.X = x;
    coord.Y = y;
    SetConsoleCursorPosition(GetStdHandle(STD_OUTPUT_HANDLE), coord);
}

/* Основная функция */
int main(void) {
    int i, j;
    
    /* Инициализация игры */
    createLevel(level);
    system("color 9F");
    bulletCount = 0;

    /* Основной игровой цикл */
    do {
        clearMap();

        /* Обработка ввода */
        if ((mario.isFly == FALSE) && (GetKeyState(VK_SPACE) < 0)) {
            mario.vertSpeed = v_sp;
        }
        if (GetKeyState('A') < 0) horizonMoveMap(1);
        if (GetKeyState('D') < 0) horizonMoveMap(-1);
        if (GetKeyState('Q') < 0 && bulletCount < BULLET_COUNT - 1) {
            shot(&bullet[bulletCount], 1);
            bulletCount++;
        }
        if (GetKeyState('E') < 0 && bulletCount < BULLET_COUNT - 1) {
            shot(&bullet[bulletCount], -1);
            bulletCount++;
        }

        /* Проверка падения Марио */
        if (mario.y > MAP_HEIGHT) {
            createLevel(level);
            addedGood = 0;
        }

        /* Обновление монстров */
        for (i = 0; i < MONSTER_COUNT; i++) {
            if (monster[i].cType != ' ') {
                horMoveObject(&monster[i]);
                putObjectOnMap(monster[i]);
                if (!monsterOnBrick(monster[i], brick[monstersBrick[i]])) {
                    monster[i].horSpeed *= -1;
                }
            }
        }

        /* Обновление пуль */
        for (i = 0; i < bulletCount; i++) {
            horMoveObject(&bullet[i]);
            putObjectOnMap(bullet[i]);
        }

        /* Размещение блоков */
        for (i = 0; i < brickLength; i++) {
            putObjectOnMap(brick[i]);
        }

        /* Размещение облаков */
        for (i = 0; i < CLOUD_COUNT; i++) {
            putObjectOnMap(cloud[i]);
        }

        /* Обновление Марио */
        vertMoveObject(&mario);
        putObjectOnMap(mario);

        /* Проверка коллизий пуль */
        for (i = bulletCount - 1; i >= 0; i--) {
            bool removeBullet = FALSE;
            
            /* Проверка границ карты */
            if (!isPosInMap((int)bullet[i].x, (int)bullet[i].y)) {
                removeBullet = TRUE;
            }
            
            /* Проверка коллизии с монстрами */
            for (j = 0; j < MONSTER_COUNT && !removeBullet; j++) {
                if (isCollision(bullet[i], monster[j])) {
                    removeBullet = TRUE;
                    monster[j].y = 80;
                }
            }
            
            /* Проверка коллизии с блоками */
            for (j = 0; j < brickLength && !removeBullet; j++) {
                if (isCollision(bullet[i], brick[j])) {
                    removeBullet = TRUE;
                }
            }
            
            /* Удаление пули */
            if (removeBullet) {
                for (j = i; j < bulletCount - 1; j++) {
                    bullet[j] = bullet[j + 1];
                }
                bulletCount--;
            }
        }

        /* Проверка коллизий Марио с монстрами */
        for (i = 0; i < MONSTER_COUNT; i++) {
            if (isCollision(mario, monster[i])) {
                createLevel(level);
                addedGood = 0;
            }
        }

        /* Проверка коллизий с облаками и бонусами */
        for (i = 0; i < CLOUD_COUNT; i++) {
            if (isCollision(mario, cloud[i]) && cloud[i].cType == '?') {
                cloud[i].cType = '-';
                createGood(cloud[i], &good[i]);
            }
        }

        /* Обновление бонусов */
        for (i = 0; i < CLOUD_COUNT; i++) {
            if (good[i].x < cloud[i].x + cloud[i].width + 1) {
                horMoveObject(&good[i]);
            }
            vertMoveObject(&good[i]);
            putObjectOnMap(good[i]);
            
            if (isCollision(mario, good[i])) {
                addedGood += 15;
                good[i].y += 20;
            }
        }

        /* Ограничение количества пуль */
        if (bulletCount < 0) {
            bulletCount = 0;
        }

        /* Отображение счета */
        sprintf(map[0], "Count of Goods: %d", (gettedGood + addedGood));

        /* Отображение игры */
        setCur(0, 0);
        showMap();

        Sleep(10);

    } while (GetKeyState(VK_ESCAPE) >= 0);

    /* Восстановление цветов консоли */
    system("color 0F");
    system("cls");

    /* Освобождение памяти */
    if (brick != NULL) {
        free(brick);
    }

    return 0;
}