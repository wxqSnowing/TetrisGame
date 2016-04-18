#pragma once

#include <windows.h>
#include <stdlib.h>
#include <tchar.h>
#include "resource.h"
#include <time.h>
#include <windef.h>
#include <mmsystem.h>  
#include <process.h>


/* 方块细胞之间的参数 */
#define CELLS_WIDTH (30)
#define CELLS_DISTANCE (1)



/* 方块的种类 */
#define BLOCK_KINDS (7)
#define BLOCK_ROTATES (4)
#define BLOCK_WIDTH (4) /* 方块边长4 */
#define BLOCK_SIZE (16) /* 方块占用矩阵大小4x4 BLOCK_WIDTH x BLOCK_WIDTH */

/* 容纳方块的空间 逻辑坐标系 从左到右width 从上到下high */
#define CONTAINER_WIDTH (20)
#define CONTAINER_HIGH (20)

/* 窗口大小 */
/* 窗口宽度 = 空隙 + 容器左边界 + 方块CONTAINER_WIDTH * (空隙 + CELLS_WIDTH) + 空隙 + 容器右边界 + 空隙 + 下一个容器左边界 + BLOCK_WIDTH * (空隙 + CELLS_WIDTH) + 空隙 + 下一个容器右边界 + 空隙 + 窗口两边占用部分 */
#define WINDOW_WIDTH (CELLS_DISTANCE + CELLS_DISTANCE + CONTAINER_WIDTH * (CELLS_WIDTH + 1) + CELLS_DISTANCE + CELLS_DISTANCE + CELLS_DISTANCE + CELLS_DISTANCE + BLOCK_WIDTH * (CELLS_WIDTH + 1) + CELLS_DISTANCE + CELLS_DISTANCE + CELLS_DISTANCE + 7) /* 7 为窗口边占用 */
/* 窗口高度 = 空隙 + 容器上边界 + 方块CONTAINER_HIGH * (空隙 + CELLS_WIDTH) + 空隙 + 容器下边界 + 空隙 + 窗口上下两边占用部分 */
#define WINDOW_HIGH (CELLS_DISTANCE + CELLS_DISTANCE + CONTAINER_HIGH * (CELLS_WIDTH + 1) + CELLS_DISTANCE + CELLS_DISTANCE + CELLS_DISTANCE + 33) /* 33 为标题栏高度 */

struct QUEUE_NODE_T
{
	int data;/* 字段内容 */
	struct QUEUE_NODE_T *next;/* 下一字段内容 */
};
typedef struct QUEUE_NODE_T QUEUE_NODE_T;


static const unsigned char BlockSets[BLOCK_KINDS * BLOCK_ROTATES * BLOCK_SIZE] =
{
	0, 1, 0, 0,
	0, 1, 0, 0,
	0, 1, 0, 0,
	0, 1, 0, 0,

	0, 0, 0, 0,
	0, 0, 0, 0,
	0, 0, 0, 0,
	1, 1, 1, 1,

	0, 1, 0, 0,
	0, 1, 0, 0,
	0, 1, 0, 0,
	0, 1, 0, 0,

	0, 0, 0, 0,
	0, 0, 0, 0,
	0, 0, 0, 0,
	1, 1, 1, 1,

	0, 0, 0, 0,
	0, 0, 1, 0,
	0, 0, 1, 0,
	0, 1, 1, 0,

	0, 0, 0, 0,
	0, 0, 0, 0,
	0, 1, 0, 0,
	0, 1, 1, 1,

	0, 0, 0, 0,
	0, 1, 1, 0,
	0, 1, 0, 0,
	0, 1, 0, 0,

	0, 0, 0, 0,
	0, 0, 0, 0,
	1, 1, 1, 0,
	0, 0, 1, 0,

	0, 0, 0, 0,
	0, 1, 0, 0,
	0, 1, 0, 0,
	0, 1, 1, 0,

	0, 0, 0, 0,
	0, 0, 0, 0,
	0, 1, 1, 1,
	0, 1, 0, 0,

	0, 0, 0, 0,
	0, 1, 1, 0,
	0, 0, 1, 0,
	0, 0, 1, 0,

	0, 0, 0, 0,
	0, 0, 0, 0,
	0, 0, 1, 0,
	1, 1, 1, 0,

	0, 0, 0, 0,
	0, 0, 0, 0,
	0, 1, 1, 0,
	0, 1, 1, 0,

	0, 0, 0, 0,
	0, 0, 0, 0,
	0, 1, 1, 0,
	0, 1, 1, 0,

	0, 0, 0, 0,
	0, 0, 0, 0,
	0, 1, 1, 0,
	0, 1, 1, 0,

	0, 0, 0, 0,
	0, 0, 0, 0,
	0, 1, 1, 0,
	0, 1, 1, 0,
	
	0, 0, 0, 0,
	0, 0, 0, 0,
	0, 1, 1, 0,
	1, 1, 0, 0,

	0, 0, 0, 0,
	0, 1, 0, 0,
	0, 1, 1, 0,
	0, 0, 1, 0,

	0, 0, 0, 0,
	0, 0, 0, 0,
	0, 1, 1, 0,
	1, 1, 0, 0,

	0, 0, 0, 0,
	0, 1, 0, 0,
	0, 1, 1, 0,
	0, 0, 1, 0,
	
	0, 0, 0, 0,
	0, 0, 0, 0,
	1, 1, 1, 0,
	0, 1, 0, 0,

	0, 0, 0, 0,
	0, 1, 0, 0,
	1, 1, 0, 0,
	0, 1, 0, 0,

	0, 0, 0, 0,
	0, 0, 0, 0,
	0, 1, 0, 0,
	1, 1, 1, 0,

	0, 0, 0, 0,
	0, 1, 0, 0,
	0, 1, 1, 0,
	0, 1, 0, 0,

	0, 0, 0, 0,
	0, 0, 0, 0,
	1, 1, 0, 0,
	0, 1, 1, 0,

	0, 0, 0, 0,
	0, 0, 1, 0,
	0, 1, 1, 0,
	0, 1, 0, 0,

	0, 0, 0, 0,
	0, 0, 0, 0,
	1, 1, 0, 0,
	0, 1, 1, 0,

	0, 0, 0, 0,
	0, 0, 1, 0,
	0, 1, 1, 0,
	0, 1, 0, 0
};


typedef struct Tetris
{
	unsigned int timerid;
	unsigned int GameState; /* 游戏状态，0，游戏运行； 1，游戏开始；2，游戏暂停 */
	unsigned int initalSpeed; /* 初速度 */
	unsigned int speed; /* 速度 */
	unsigned int score; /* 得分 */

	/* 下一个方块 */
	const unsigned char *next_block;
	unsigned int next_kind;
	unsigned int next_rotate;

	/* 当前方块 */
	const unsigned char *current_block;
	unsigned int kind;
	unsigned int rotate;
	int offset_left; /* 逻辑坐标 left, 正常范围 -min_left ~ CONTAINER_WIDTH - max_right */
	int offset_top; /* 逻辑坐标 top, 正常范围 -min_top ~ CONTAINER_HIGH - max_bottom */
	int offset_top_destination; /* 用来表明当前方块直线下落位置 */

	int min_left;
	int max_right;
	int min_top;
	int max_bottom;
} Tetris;
Tetris tetris = { 0 };

/* 容器矩形坐标 */
RECT ContainerRect = { CELLS_DISTANCE, CELLS_DISTANCE,CELLS_DISTANCE + CELLS_DISTANCE + CONTAINER_WIDTH * (CELLS_WIDTH + 1) + CELLS_DISTANCE + CELLS_DISTANCE,CELLS_DISTANCE + CELLS_DISTANCE + CONTAINER_HIGH * (CELLS_WIDTH + 1) + CELLS_DISTANCE + CELLS_DISTANCE };

/* 下一个方块容器矩形坐标 */
RECT NextBlockRect = { CELLS_DISTANCE + CELLS_DISTANCE + CONTAINER_WIDTH * (CELLS_WIDTH + 1) + CELLS_DISTANCE + CELLS_DISTANCE + CELLS_DISTANCE + CELLS_DISTANCE,CELLS_DISTANCE,CELLS_DISTANCE + CELLS_DISTANCE + CONTAINER_WIDTH * (CELLS_WIDTH + 1) + CELLS_DISTANCE + CELLS_DISTANCE + CELLS_DISTANCE + CELLS_DISTANCE + CELLS_DISTANCE + BLOCK_WIDTH * (CELLS_WIDTH + 1) + CELLS_DISTANCE + CELLS_DISTANCE,CELLS_DISTANCE + CELLS_DISTANCE + BLOCK_WIDTH * (CELLS_WIDTH + 1) + CELLS_DISTANCE + CELLS_DISTANCE };



long WINAPI WndProc(HWND hWnd, UINT iMessage, UINT wParam, LONG lParam);
BOOLEAN InitWindowClass(HINSTANCE hinstance, int ncmdshow);
LRESULT CALLBACK WndProc(HWND hwnd, UINT message, WPARAM wparam, LPARAM lparam);
int WINAPI WinMain(HINSTANCE hinstance, HINSTANCE hprevinstance, LPSTR lpcmdline, int ncmdshow);
LRESULT CALLBACK About(HWND hDlg, UINT message, WPARAM wParam, LPARAM lParam);


#pragma comment(lib,"winmm.lib")//导入声音头文件库

HWND hwnd;
HINSTANCE hinst;

static BOOL start = FALSE, rstart = FALSE, music = TRUE, gameover = FALSE, enter = TRUE;
HDC hdc = { 0 };
PAINTSTRUCT ps = { 0 };
LRESULT rs = 0;

//图片加载的定义
static HDC  hmemdc_end, hmemdc_back;
static HBITMAP h_end_bitmap, h_back_bitmap;
static BITMAP end_bitmap, back_bitmap;
static bool fullclient = false;
RECT clientRect;

HMENU hmenu1, hmenu1add, hmenu2, hmenu2add, hadmenu;


/* 消多行分数奖励 */
static const int ScoreSets[BLOCK_WIDTH] = { 100, 400, 800, 1600 };
static unsigned char Container[CONTAINER_HIGH][CONTAINER_WIDTH] = { 0 };

//FUCTION
//游戏结束判断
static void CheckGameOver(void);   //1结束判断
static void GenerateBlock(void);//7检查游戏是否已经结束


static void CopyToContainer(void);   //2复制容器中的方块

//3，4记录容器中已存在方块
static unsigned int queue_push(QUEUE_NODE_T **head, int data);    
static unsigned int queue_pop(QUEUE_NODE_T **head, int *data);


static void CaculateScore(void);   //5 计分
static void CaculateBlockBoundary(void);//6 计算方块范围

static unsigned int DetectCollision(const unsigned char *block, int offset_left, int offset_top);//8碰撞
static void StepLeft(void);//9左移
static void StepRight(void);//10右移
static int StepDown(void);//11向下运动


static void UpdateCurrBlockDestinationPosition(void);//12更新当前方块目的的位置
static void StepRotate(void);//13步骤
static void OnDrawFrame(HDC hdc_mem);//14画图

static void OnDrawSpeedSelect(HDC hdc_mem);//15速度选择
static void OnDrawBlock(HDC hdc_mem);//16画方块
static void OnDrawResult(HDC hdc_mem);//17画结果
static void OnPaint(HDC hdc);//画


void start_map();
void geme_backgound_bmp(HDC hdc);
void end_map();

int bgmmusic();
