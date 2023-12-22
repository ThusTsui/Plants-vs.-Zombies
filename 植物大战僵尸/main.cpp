/*
*������־
* 1. ��������Ŀ���հ���Ŀģ�壩
* 2. �����ز�
* 3. ʵ���ʼ����Ϸ����
* 4. ʵ����Ϸ����������
* 5. ʵ�ֹ������е���Ϸ����
*/


#include<stdio.h>
#include<iostream>
#include<graphics.h>
#include"tools.h"

#define WIN_WIDTH	900
#define WIN_WEIGHT	600

enum{WAN_DOU,XIANG_RI_KUI,DA_ZUI_HUA,CARD_COUNT};

IMAGE imgBg;//��ʾ����ͼƬ
IMAGE imgBar;//�Ϸ�������ͼƬ
IMAGE imgCards[CARD_COUNT];//ֲ�￨�����飨��ö�ٸ����㣩
IMAGE* imgZhiwu[CARD_COUNT][20];//ֲ�����飨С���� ��ָ�룩

int curX, curY;//ѡ�е�ǰ��ֲ����ƶ��е�ǰ��λ��
int curZhiWu; //0:û��ѡ�У�1����ʾѡ���˵�һ��ֲ��

struct zhiwu {
	int type;		//0��ʾû��ֲ�1����һ��ֲ��
	int frameIndex;	//����֡���

};

struct zhiwu map[3][9];

struct sunshineBall
{
	int x, y;//������Ʈ������е�����λ�ã�x���䣩
	int frameIndex;//��ǰ��ʾ��ͼƬ֡���
	int destY;//Ʈ���Ŀ��λ��y����
	bool used;//�Ƿ���ʹ��
	int timer;
};

struct sunshineBall balls[10];	//��
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
	//������Ϸ����ͼƬ(�����ڴ��м��ٿ�����
	//���ַ����޸�Ϊ�����ֽ��ַ�������ֹloadimage����
	loadimage(&imgBg, "res/bg.jpg");
	loadimage(&imgBar, "res/bar5.png");

	memset(imgZhiwu, 0, sizeof(imgZhiwu));
	memset(map, 0, sizeof(map));

	//��ʼ��ֲ�￨��
	char name[64];
	for (int i = 0; i < CARD_COUNT; i++) {
		//����ֲ�￨�Ƶ��ļ���
		sprintf_s(name, sizeof(name), "res/Cards/card_%d.png", i + 1);
		loadimage(&imgCards[i], name);

		for (int j = 0; j < 20; j++) {
			sprintf_s(name, sizeof(name), "res/zhiwu/%d/%d.png",i,j+1);
			//���ж�����ļ��Ƿ����
			if (fileExist(name)) {
				//����
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

	//�����������
	srand(time(NULL));

	//������Ϸͼ�δ���
	initgraph(WIN_WIDTH, WIN_WEIGHT);
}

void updateWindow() {
	//while�ڻ���˸��ʹ��˫����
	BeginBatchDraw();//��ʼ����

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
				//���컨ר��λ��
				int x = 248 + j * 81;
				int y = 160 + i * 102;
				int zhiWuType = map[i][j].type - 1;
				int index = map[i][j].frameIndex;
				putimagePNG(x, y, imgZhiwu[zhiWuType][index]);
			}
		}
	}

	//��Ⱦ�϶���ֲ���λ��
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


	EndBatchDraw();//����˫����
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

		//���������ȥһ������ʹ�õ�
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
	createSunshine();//��������
	updateSunshine();//��������
}

void startUI() {
	IMAGE imgBGm, imgMenu1, imgMenu2;
	loadimage(&imgBGm, "res/menu.png");
	loadimage(&imgMenu1, "res/menu1.png");
	loadimage(&imgMenu2, "res/menu2.png");

	int flag = 0;//�����Ϊ0��δѡ��

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
	gameInit();//��ʼ�������ؽ���
	startUI();

	int timer = 0;
	bool flag = true;
	while (1) {
		userClick();//�û�����
		timer += getDelay();
		if (timer > 60) {
			flag = true;
			timer = 0;
		}
		if (flag) {
			flag = false;
			updateWindow();//���½���

			updateGame();//������Ϸ�������

			//Sleep(60);//����Ҫ�Ż�
		}

	}


	system("pause");
	return 0;
}