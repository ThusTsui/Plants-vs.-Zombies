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

enum{WAN_DOU,XIANG_RI_KUI,MOU_GU,CARD_COUNT};

IMAGE imgBg;//��ʾ����ͼƬ
IMAGE imgBar;//�Ϸ�������ͼƬ
IMAGE imgCards[CARD_COUNT];//ֲ�￨�����飨��ö�ٸ����㣩
IMAGE* imgZhiwu[CARD_COUNT][20];//ֲ�����飨С���� ��ָ�룩
int curX, curY;//ѡ�е�ǰ��ֲ����ƶ��е�ǰ��λ��
int curZhiWu; //0:û��ѡ�У�1��ѡ��
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

	//��Ⱦ�϶���ֲ���λ��
	if (curZhiWu) {
		IMAGE* img = imgZhiwu[curZhiWu - 1][0];
		putimagePNG(curX-img->getwidth()/2, curY-img->getheight()/2,img );
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
	gameInit();//��ʼ�������ؽ���

	while (1) {
		userClick();//�û�����
		updateWindow();//���½���
	}




	system("pause");
	return 0;
}