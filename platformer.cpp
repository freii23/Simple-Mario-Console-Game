#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <windows.h>

#define mapWidth 80
#define mapHeight 25
#define monCount 8
#define cloudCount 5
#define goodCount 5


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
int addedGood = 0;
int gettedGood = 0;
float v_sp = -1;
float l_sp = 0.05;

TObject *bullet = NULL;
int bulletLength = 100;

TObject monster[monCount];
int monstersBrick[monCount] = {0, 2, 3, 3, 3, 1, 3, 2};
TObject cloud[cloudCount];
TObject good[goodCount];

void createLevel(int lvl);
void putObjectOnMap(TObject obj);

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
	obj->vertSpeed += l_sp;
	setObjectPos(obj, obj->x, obj->y + obj->vertSpeed);
	for (int i = 0; i < brickLength; i++)
		if (isCollision(*obj, brick[i])) {
			obj->y -= obj->vertSpeed;
			obj->vertSpeed = 0;
			obj->isFly = FALSE;
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
	mario.x += dx;

	for (int i = 0; i < brickLength; i++)
		brick[i].x += dx;
	for (int i = 0; i < monCount; i++)
		monster[i].x += dx;
	for (int i = 0; i < cloudCount; i++)
		cloud[i].x += dx;
	for (int i = 0; i < goodCount; i++)
		good[i].x += dx;
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
		v_sp = -1;
		l_sp = 0.05;
		brickLength = 6;
		brick = (TObject*)realloc(brick, sizeof(*brick) * brickLength);
		initObject(brick+0, 20, 20, 55, 5, '#');
		initObject(brick+1, 80, 15, 22, 10, '#');
		initObject(brick+2, 120, 20, 60, 5, '#');
		initObject(brick+3, 180, 15, 30, 10, '#');
		initObject(brick+4, 210, 20, 40, 5, '#');
		initObject(brick+5, 270, 11, 3, 1, '+');

		initObject(monster+0, brick[monstersBrick[0]].x, brick[monstersBrick[0]].y - 2, 3, 2, '$');
		initObject(monster+1, brick[monstersBrick[1]].x, brick[monstersBrick[1]].y - 3, 3, 3, '$');
		initObject(monster+2, brick[monstersBrick[2]].x, brick[monstersBrick[2]].y - 2, 3, 2, '$');

		initObject(cloud+0, 30, 10, 8, 3, '?');
		initObject(cloud+1, 200, 8, 8, 3, '?');

		monster[0].horSpeed = 0.5;	
		monster[1].horSpeed = 0.5;
		monster[2].horSpeed = 0.5;
	}
	if (2 == lvl) {
		v_sp = -1;		
		l_sp = 0.05;		
		brickLength = 4;
		brick = (TObject*)realloc(brick, sizeof(*brick) * brickLength);
		initObject(brick+0, 20, 20, 40, 5, '#');
		initObject(brick+1, 80, 15, 15, 13, '#');
		initObject(brick+2, 130, 15, 20, 7, '#');
		initObject(brick+3, 190, 15, 20, 20, '+');
		
		initObject(monster+3, brick[monstersBrick[3]].x, brick[monstersBrick[3]].y - 2, 3, 2, '$');
		initObject(monster+4, brick[monstersBrick[4]].x, brick[monstersBrick[4]].y - 3, 3, 3, '$');
		initObject(monster+5, brick[monstersBrick[5]].x, brick[monstersBrick[5]].y - 2, 3, 2, '$');

		initObject(cloud+2, 20, 10, 8, 3, '?');

		monster[3].horSpeed = 0.5;	
		monster[4].horSpeed = 0.5;
		monster[5].horSpeed = 0.5;		
	}
	if (3 == lvl) {
		initObject(&mario, 5, 4, 1, 1, '@');
		v_sp = -0.71;	
		l_sp = 0.032;			
		brickLength = 16;
		brick = (TObject*)realloc(brick, sizeof(*brick) * brickLength);
		initObject(brick+0, 5, 5, 2, 30, '#');
		initObject(brick+1, 20, 7, 2, 30, '#');
		initObject(brick+2, 35, 9, 1, 30, '#');
		initObject(brick+3, 46, 11, 1, 40, '#');
		initObject(brick+4, 60, 13, 3, 30, '#');
		initObject(brick+5, 80, 20, 4, 50, '#');
		initObject(brick+6, 80, 1, 4, 13, '#');
		initObject(brick+7, 100, 15, 3, 30, '#');
		initObject(brick+8, 100, 3, 3, 8, '#');
		initObject(brick+9, 125, 20, 3, 30, '#');
		initObject(brick+10, 125, 3, 3, 14, '#');
		initObject(brick+11, 142, 24, 10, 20, '#');
		initObject(brick+12, 142, 1, 30, 20, '#');
		initObject(brick+13, 163, 24, 40, 20, '#');
		initObject(brick+14, 210, 18, 15, 30, '#');
		initObject(brick+15, 226, 18, 1, 1, '+');						
	}
	if (4 == lvl) {
		v_sp = -1;
		l_sp = 0.05;				
		printf("-1");
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
			mario.vertSpeed = v_sp;
		if (GetKeyState('A') < 0) horizonMoveMap(1);
		if (GetKeyState('D') < 0) horizonMoveMap(-1); 
		if (GetKeyState('Q') < 0) {shot(bullet+bulCount, 1); bulCount++;}
		if (GetKeyState('E') < 0) {shot(bullet+bulCount, -1); bulCount++;}

		if (mario.y > mapHeight) {
			createLevel(level);
			addedGood = 0;
		}

		for (int i = 0; i < monCount; i++) {
			horMoveObject(monster+i);
			putObjectOnMap(monster[i]);
			if (!(monsterOnBrick(monster[i], brick[monstersBrick[i]])))
				monster[i].horSpeed *= -1;
		}			

		for (int i = 0; i < bulCount; i++)
			horMoveObject(bullet+i);
		for (int i = 0; i < bulCount - 1; i++)
			putObjectOnMap(bullet[i]);
		
		for (int i = 0; i < brickLength; i++) {
			putObjectOnMap(brick[i]);
		}
		for (int i = 0; i < cloudCount; i++) {
			putObjectOnMap(cloud[i]);
		}

		vertMoveObject(&mario);		
		putObjectOnMap(mario);

		for (int i = 0; i < bulCount; i++) {
			if (!(isPosInMap(bullet[i].x, bullet[i].y))) {		
				bulCount--;
			}
			for (int j = 0; j < monCount; j++)
				if (isCollision(bullet[i], monster[j])) {
					bulCount--;
					bullet[i].horSpeed = 0;
					monster[j].y = 80;				
				}
			for (int j = 0; j < brickLength; j++)
				if (isCollision(bullet[i], brick[j])) {
					bulCount--;
					bullet[i].horSpeed = 0;
				}
		}
		for (int i = 0; i < monCount; i++)
			if (isCollision(mario, monster[i])) {
				createLevel(level);
				addedGood = 0;
			}
		for (int i = 0; i < cloudCount; i++)
			if (isCollision(mario, cloud[i]) && '?' == cloud[i].cType) {
				cloud[i].cType = '-';
				createGood(cloud[i], good+i);
			}	
		for (int i = 0; i < cloudCount; i++) {
			if (good[i].x < cloud[i].x + cloud[i].width + 1)
				horMoveObject(good+i);
			vertMoveObject(good+i);
			putObjectOnMap(good[i]);
			if (isCollision(mario, good[i])) {
				addedGood += 15;
				good[i].y += 20;
			}
		}

		if (bulCount < 0)	
			bulCount = 0;

		sprintf(map[0], "Count of Goods: %d\n", (gettedGood + addedGood));

		setCur(0, 0);
		showMap();

		Sleep(10);

	} while (GetKeyState(VK_ESCAPE) >= 0);
	system("color 0F");
	system("cls");

	return 0;
}