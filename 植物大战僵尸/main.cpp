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

enum{WAN_DOU,XIANG_RI_KUI,MOU_GU,CARD_COUNT};

IMAGE imgBg;//表示背景图片
IMAGE imgBar;//上方工具栏图片
IMAGE imgCards[CARD_COUNT];//植物卡牌数组（用枚举更方便）
IMAGE* imgZhiwu[CARD_COUNT][20];//植物数组（小技巧 放指针）
int curX, curY;//选中当前的植物，在移动中当前的位置
int curZhiWu; //0:没有选中；1：选中
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

	//渲染拖动中植物的位置
	if (curZhiWu) {
		IMAGE* img = imgZhiwu[curZhiWu - 1][0];
		putimagePNG(curX-img->getwidth()/2, curY-img->getheight()/2,img );
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
		}
		else if (msg.message == WM_MOUSEMOVE&& status==1) {
			curX = msg.x;
			curY = msg.y;

		}
		else if (msg.message == WM_LBUTTONUP) {

		}
	}
}
int main(void) {
	gameInit();//初始化、加载界面

	while (1) {
		userClick();//用户操作
		updateWindow();//更新界面
	}




	system("pause");
	return 0;
}