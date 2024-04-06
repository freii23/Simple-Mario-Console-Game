#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <windows.h>

#define mapWidth 80
#define mapHeight 25

typedef struct SObject {
	float x, y;
	float width, height;
	float horSpeed = 0;
	float vertSpeed;
	bool isFly;
	char cType;
} TObject;

char map[mapHeight][mapWidth + 1];

TObject mario;

TObject *brick = NULL;
int brickLength;
int level = 1;
int goodCount = 0;

TObject *bullet = NULL;
int bulletLength = 100;

TObject monster;
TObject monster2;

TObject cloud;
TObject good;

void createLevel(int lvl);
void putObjectOnMap(TObject obj);

void addGood() {
	goodCount += 15;
}

void clearMap() {
	for (int i = 0; i < mapWidth; i++)
		map[0][i] = ' ';
	map[0][mapWidth] = '\0';
	for (int j = 1; j < mapHeight; j++)
		sprintf(map[j], map[0]);
}

void showMap() {
	map[mapHeight - 1][mapWidth - 1] = '\0';
	for (int j = 0; j < mapHeight; j++)
		printf("%s", map[j]);
}

void setObjectPos(TObject* obj, float xPos, float yPos) {
	obj->x = xPos;
	obj->y = yPos;
}

void initObject(TObject* obj, float xPos, float yPos, float oWidth, float oHeight, char inType) {
	setObjectPos(obj, xPos, yPos);
	obj->width = oWidth;
	obj->height = oHeight;
	obj->vertSpeed = 0;
	obj->cType = inType;
}

bool isCollision(TObject obj1, TObject obj2) {
	return (((obj1.x + obj1.width) > obj2.x) && 
		((obj1.y + obj1.height) > obj2.y) && 
		((obj2.x + obj2.width) > obj1.x) && ((obj2.y + obj2.height)> obj1.y));
}

void vertMoveObject(TObject* obj) {
	obj->isFly = TRUE;
	obj->vertSpeed += 0.05;
	setObjectPos(obj, obj->x, obj->y + obj->vertSpeed);
	for (int i = 0; i < brickLength; i++)
		if (isCollision(*obj, brick[i])) {
			obj->y -= obj->vertSpeed;
			obj->vertSpeed = 0;
			obj->isFly = FALSE;
			if (brick[i].cType == '+') {
				level++;
				if (level > 2)
					level = 1;
				createLevel(level);
				Sleep(1000);
			}
			break;
		}
	// if (isCollision(*obj, cloud)) {
	// 	obj->y -= obj->vertSpeed;
	// 	obj->vertSpeed = 0;
	// 	obj->isFly = FALSE;
	// }
}

void horMoveObject(TObject* obj) {
	obj->x += obj->horSpeed;
}

void shot(TObject *bullet, int dir) {
	if (1 == dir)
		initObject(bullet, mario.x, mario.y, 1, 1, '-');
	else if (-1 == dir)
		initObject(bullet, mario.x + mario.width - 1, mario.y, 1, 1, '-');
	bullet->horSpeed = -2 * dir;
}

void horizonMoveMap(float dx) {
	mario.x -= dx;
	for (int i = 0; i < brickLength; i++)
		if (isCollision(mario, brick[i])) {
			mario.x += dx;
			return;
		}
	// if (isCollision(mario, cloud)) {
	// 	mario.x += dx;
	// 	return;
	// }	
	mario.x += dx;

	for (int i = 0; i < brickLength; i++)
		brick[i].x += dx;
	monster.x += dx;
	monster2.x += dx;
	cloud.x += dx;
	good.x += dx;
}

bool isPosInMap(int x, int y) {
	return (x >= 0) && (x < mapWidth) && (y >= 0) && (y < mapHeight);
}

bool monsterOnBrick(TObject obj1, TObject obj2) {
	return (obj1.x >= obj2.x) && (obj1.x + obj1.width <= obj2.x + obj2.width);
}

void putObjectOnMap(TObject obj) {
	int ix = (int)round(obj.x);
	int iy = (int)round(obj.y);
	int iWidth = (int)round(obj.width);
	int iHeight = (int)round(obj.height);
	for (int i = ix; i < (ix + iWidth); i++)
		for (int j = iy; j < (iy + iHeight); j++)
			if (isPosInMap(i, j))
				map[j][i] = obj.cType;
}

void createGood(const TObject source, TObject *obj) {
	initObject(obj, 0.5*(2*source.x + source.width), 
		source.y - 3, 5, 3, '$');
	obj->vertSpeed = -1;
	obj->horSpeed = 0.5;
}

void createLevel(int lvl) {
	initObject(&mario, 39, 10, 3, 3, '@');

	if (1 == lvl) {
		brickLength = 6;
		brick = (TObject*)realloc(brick, sizeof(*brick) * brickLength);
		initObject(brick+0, 20, 20, 55, 5, '#');
		initObject(&cloud, 30, 10, 8, 3, '?');
		initObject(brick+1, 80, 15, 22, 10, '#');
		initObject(brick+2, 120, 20, 60, 5, '#');
		initObject(brick+3, 180, 15, 30, 10, '#');
		initObject(brick+4, 210, 20, 40, 5, '#');
		initObject(brick+5, 270, 11, 3, 1, '+');
		initObject(&monster, brick[1].x, brick[2].y - 2, 3, 2, '$');
		initObject(&monster2, brick[2].x, brick[2].y - 3, 3, 3, '$');

		monster.horSpeed = 0.5;	
		monster2.horSpeed = 0.5;			
	}
	if (2 == lvl) {
		brickLength = 4;
		brick = (TObject*)realloc(brick, sizeof(*brick) * brickLength);
		initObject(brick+0, 20, 20, 40, 5, '#');
		initObject(brick+1, 80, 15, 15, 13, '#');
		initObject(brick+2, 130, 15, 20, 7, '#');
		initObject(brick+3, 190, 15, 20, 20, '+');
	}
}

void setCur(int x, int y) {
	COORD coord;
	coord.X = x;
	coord.Y = y;
	SetConsoleCursorPosition(GetStdHandle(STD_OUTPUT_HANDLE), coord);
}

int main() {
	createLevel(level);
	system("color 9F");
	bullet = (TObject*)realloc(bullet, sizeof(*bullet) * bulletLength);
	int bulCount = 0;

	do {
		clearMap();

		if ((FALSE == mario.isFly) && (GetKeyState(VK_SPACE) < 0))
			mario.vertSpeed = -1;
		if (GetKeyState('A') < 0) horizonMoveMap(1);
		if (GetKeyState('D') < 0) horizonMoveMap(-1); 
		if (GetKeyState('Q') < 0) {shot(bullet+bulCount, 1); bulCount++;}
		if (GetKeyState('E') < 0) {shot(bullet+bulCount, -1); bulCount++;}

		if (mario.y > mapHeight)
			createLevel(level);

		horMoveObject(&monster);
		putObjectOnMap(monster);
		if (!(monsterOnBrick(monster, brick[1])))
			monster.horSpeed *= -1;

		horMoveObject(&monster2);
		putObjectOnMap(monster2);
		if (!(monsterOnBrick(monster2, brick[2])))
			monster2.horSpeed *= -1;

		for (int i = 0; i < bulCount; i++)
			horMoveObject(bullet+i);
		for (int i = 0; i < bulCount - 1; i++)
			putObjectOnMap(bullet[i]);
		
		for (int i = 0; i < brickLength; i++) {
			putObjectOnMap(brick[i]);
		}
		putObjectOnMap(cloud);

		vertMoveObject(&mario);		
		putObjectOnMap(mario);

		for (int i = 0; i < bulCount; i++) {
			if (!(isPosInMap(bullet[i].x, bullet[i].y))) {		
				bulCount--;
			}
			if (isCollision(bullet[i], monster2)) {
				bulCount--;
				bullet[i].horSpeed = 0;
				monster2.y = 80;
			}
			for (int j = 0; j < brickLength; j++)
				if (isCollision(bullet[i], brick[j])) {
					bulCount--;
					bullet[i].horSpeed = 0;
				}
		}
		if (isCollision(mario, monster))
			createLevel(level);
		if (isCollision(mario, monster2))
			createLevel(level);	
		if (isCollision(mario, cloud) && '?' == cloud.cType) {
			cloud.cType = '-';
			createGood(cloud, &good);
		}	

		if (good.x < cloud.x + cloud.width + 1)
			horMoveObject(&good);
		vertMoveObject(&good);
		putObjectOnMap(good);
		if (isCollision(mario, good)) {
			addGood();
			good.y += 20;
		}

		if (bulCount < 0)	
			bulCount = 0;

		sprintf(map[0], "Count of Goods: %d\n", goodCount);

		setCur(0, 0);
		showMap();

		Sleep(10);

	} while (GetKeyState(VK_ESCAPE) >= 0);
	system("color 0F");
	system("cls");

	return 0;
}