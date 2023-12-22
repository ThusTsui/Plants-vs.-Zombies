/*
*开发日志
* 1. 创建新项目（空白项目模板）
* 2. 导入素材
* 3. 实现最开始的游戏场景
* 4. 实现游戏顶部工具栏
* 5. 实现工具栏中的游戏卡牌
*/


#include<stdio.h>
#include<iostream>
#include<graphics.h>
#include"tools.h"

#define WIN_WIDTH	900
#define WIN_WEIGHT	600

enum{WAN_DOU,XIANG_RI_KUI,DA_ZUI_HUA,CARD_COUNT};

IMAGE imgBg;//表示背景图片
IMAGE imgBar;//上方工具栏图片
IMAGE imgCards[CARD_COUNT];//植物卡牌数组（用枚举更方便）
IMAGE* imgZhiwu[CARD_COUNT][20];//植物数组（小技巧 放指针）

int curX, curY;//选中当前的植物，在移动中当前的位置
int curZhiWu; //0:没有选中；1：表示选择了第一种植物

struct zhiwu {
	int type;		//0表示没有植物；1：第一种植物
	int frameIndex;	//序列帧序号

};

struct zhiwu map[3][9];

struct sunshineBall
{
	int x, y;//阳光在飘落过程中的坐标位置（x不变）
	int frameIndex;//当前显示的图片帧序号
	int destY;//飘落的目标位置y坐标
	bool used;//是否在使用
	int timer;
};

struct sunshineBall balls[10];	//池
IMAGE imgSunshineBall[29];

bool fileExist(const char* name) {
	FILE* fp = fopen(name, "r");
	if (fp == NULL) {
		return false;
	}
	else {
		fclose(fp);
		return true;
	}
}

void gameInit() {
	//加载游戏背景图片(放在内存中减少开销）
	//把字符集修改为“多字节字符集”防止loadimage报错
	loadimage(&imgBg, "res/bg.jpg");
	loadimage(&imgBar, "res/bar5.png");

	memset(imgZhiwu, 0, sizeof(imgZhiwu));
	memset(map, 0, sizeof(map));

	//初始化植物卡牌
	char name[64];
	for (int i = 0; i < CARD_COUNT; i++) {
		//生成植物卡牌的文件名
		sprintf_s(name, sizeof(name), "res/Cards/card_%d.png", i + 1);
		loadimage(&imgCards[i], name);

		for (int j = 0; j < 20; j++) {
			sprintf_s(name, sizeof(name), "res/zhiwu/%d/%d.png",i,j+1);
			//先判断这个文件是否存在
			if (fileExist(name)) {
				//加载
				imgZhiwu[i][j] = new IMAGE;
				loadimage(imgZhiwu[i][j], name);
			}
			else {
				break;
			}
		}
	}
	curZhiWu = 0;




	memset(balls, 0, sizeof(balls));
	for (int i = 0; i < 29; i++) {
		sprintf_s(name, sizeof(name), "res/sunshine/%d.png", i + 1);
		loadimage(&imgSunshineBall[i], name);
	}

	//配置随机种子
	srand(time(NULL));

	//创建游戏图形窗口
	initgraph(WIN_WIDTH, WIN_WEIGHT);
}

void updateWindow() {
	//while内会闪烁，使用双缓冲
	BeginBatchDraw();//开始缓冲

	putimage(0, 0, &imgBg);
	//putimage(250,0,&imgBar);
	putimagePNG(250, 0, &imgBar);

	for (int i = 0; i < CARD_COUNT; i++) {
		int x = 338 + i * 65;
		int y = 5;
		putimagePNG(x, y, &imgCards[i]);
	}



	for (int i = 0; i < 3; i++) {
		for (int j = 0; j < 9; j++) {
			if (map[i][j].type > 0&& map[i][j].type!=3) {
				int x = 256 + j * 81;
				int y = 185 + i * 102;
				int zhiWuType = map[i][j].type-1;
				int index = map[i][j].frameIndex;
				putimagePNG(x, y, imgZhiwu[zhiWuType][index]);
			}
			else if (map[i][j].type == 3) {
				//大嘴花专属位置
				int x = 248 + j * 81;
				int y = 160 + i * 102;
				int zhiWuType = map[i][j].type - 1;
				int index = map[i][j].frameIndex;
				putimagePNG(x, y, imgZhiwu[zhiWuType][index]);
			}
		}
	}

	//渲染拖动中植物的位置
	if (curZhiWu) {
		IMAGE* img = imgZhiwu[curZhiWu - 1][0];
		putimagePNG(curX - img->getwidth() / 2, curY - img->getheight() / 2, img);
	}

	int ballMax = sizeof(balls) / sizeof(balls[0]);
	for (int i = 0; i < ballMax; i++) {
		if (balls[i].used) {
			IMAGE* img = &imgSunshineBall[balls[i].frameIndex];
			putimagePNG(balls[i].x, balls[i].y, img);
		}
		
	}


	EndBatchDraw();//结束双缓冲
}
void userClick() {
	ExMessage msg;
	static int status = 0;
	if (peekmessage(&msg)) {
		if (msg.message == WM_LBUTTONDOWN) {
			if (msg.x > 338 && msg.x < 338 + 65 * CARD_COUNT && msg.y < 96) {
				int index = (msg.x - 338) / 65;
				//printf("%d\n", index);
				status = 1;
				curZhiWu = index + 1;
			}
			else if (msg.x > 256 && msg.y > 179 && msg.y < 489 && status == 1) {
				int row = (msg.y - 179) / 102;
				int col = (msg.x - 256) / 81;
				//printf("%d,%d\n", row, col);

				if (map[row][col].type == 0) {
					map[row][col].type = curZhiWu;
					map[row][col].frameIndex = 0;
				}
				curZhiWu = 0;
				status = 0;
			}


		}
		else if (msg.message == WM_MOUSEMOVE&& status==1) {
			curX = msg.x;
			curY = msg.y;

		}
/*		else if (msg.message == WM_LBUTTONUP) {
			if (msg.x > 256 && msg.y > 179 && msg.y < 489) {
				int row = (msg.y - 179) / 102;
				int col = (msg.x - 256) / 81;
				//printf("%d,%d\n", row, col);

				if(map[row][col].type==0){
					map[row][col].type = curZhiWu;
					map[row][col].frameIndex = 0;
				}

			}
						
			curZhiWu = 0;
			status = 0;

		}
		*/
	}
}

void createSunshine() {
	static int count = 0;
	static int fre = 200;
	count++;
	if (count >= fre) {
		fre = 100 + rand() % 150;
		count = 0;

		//从阳光池中去一个可以使用的
		int ballMax = sizeof(balls) / sizeof(balls[0]);
		int i;
		for (i = 0; i < ballMax && balls[i].used; i++);
		if (i >= ballMax)return;

		balls[i].used = true;
		balls[i].frameIndex = 0;
		balls[i].x = 240 + rand() % (800 - 260);
		balls[i].y = 60;
		balls[i].destY = 250 + (rand() % 4) * 90;
	}

}

void updateSunshine() {
	int ballMax = sizeof(balls) / sizeof(balls[0]);
	for (int i = 0; i < ballMax; i++) {
		if (balls[i].used) {
			balls[i].frameIndex = (balls[i].frameIndex + 1) % 29;
			if (balls[i].timer == 0) { 
				balls[i].y += 3; 
			}
			
			if (balls[i].y >= balls[i].destY) {
				//balls[i].used = false;
				balls[i].timer++;

				if (balls[i].timer > 100) {
					balls[i].used = false;
				}
			}
		}
	}
}

void updateGame() {
	for (int i = 0; i < 3; i++) {
		for (int j = 0; j < 9; j++) {
			if (map[i][j].type > 0) {
				map[i][j].frameIndex++;
				int zhiWuType = map[i][j].type - 1;
				int index = map[i][j].frameIndex;
				if (imgZhiwu[zhiWuType][index] == NULL) {
					map[i][j].frameIndex = 0;
				}
			}
		}
	}
	createSunshine();//创建阳光
	updateSunshine();//更新阳光
}

void startUI() {
	IMAGE imgBGm, imgMenu1, imgMenu2;
	loadimage(&imgBGm, "res/menu.png");
	loadimage(&imgMenu1, "res/menu1.png");
	loadimage(&imgMenu2, "res/menu2.png");

	int flag = 0;//标记设为0，未选中

	while (1) {
		BeginBatchDraw();
		putimage(0, 0, &imgBGm);
		putimagePNG(474, 75, flag ? &imgMenu2 : &imgMenu1);
		

		ExMessage msg;
		if (peekmessage(&msg)) {
			if (msg.message == WM_MOUSEMOVE)					 
				if(msg.x > 474 && msg.x < 474 + 300 && msg.y > 75 && msg.y < 75 + 140){
				flag = 1;				
				}
				else {
					flag = 0;
				}
			else if (msg.message == WM_LBUTTONUP && flag==1) {
				return;
			}
		}
		EndBatchDraw();
	}
}

int main(void) {
	gameInit();//初始化、加载界面
	startUI();

	int timer = 0;
	bool flag = true;
	while (1) {
		userClick();//用户操作
		timer += getDelay();
		if (timer > 60) {
			flag = true;
			timer = 0;
		}
		if (flag) {
			flag = false;
			updateWindow();//更新界面

			updateGame();//更新游戏相关数据

			//Sleep(60);//还我要优化
		}

	}


	system("pause");
	return 0;
}