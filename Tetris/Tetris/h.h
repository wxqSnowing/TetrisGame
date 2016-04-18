#pragma once

#include <windows.h>
#include <stdlib.h>
#include <tchar.h>
#include "resource.h"
#include <time.h>
#include <windef.h>
#include <mmsystem.h>  
#include <process.h>


/* ����ϸ��֮��Ĳ��� */
#define CELLS_WIDTH (30)
#define CELLS_DISTANCE (1)



/* ��������� */
#define BLOCK_KINDS (7)
#define BLOCK_ROTATES (4)
#define BLOCK_WIDTH (4) /* ����߳�4 */
#define BLOCK_SIZE (16) /* ����ռ�þ����С4x4 BLOCK_WIDTH x BLOCK_WIDTH */

/* ���ɷ���Ŀռ� �߼�����ϵ ������width ���ϵ���high */
#define CONTAINER_WIDTH (20)
#define CONTAINER_HIGH (20)

/* ���ڴ�С */
/* ���ڿ�� = ��϶ + ������߽� + ����CONTAINER_WIDTH * (��϶ + CELLS_WIDTH) + ��϶ + �����ұ߽� + ��϶ + ��һ��������߽� + BLOCK_WIDTH * (��϶ + CELLS_WIDTH) + ��϶ + ��һ�������ұ߽� + ��϶ + ��������ռ�ò��� */
#define WINDOW_WIDTH (CELLS_DISTANCE + CELLS_DISTANCE + CONTAINER_WIDTH * (CELLS_WIDTH + 1) + CELLS_DISTANCE + CELLS_DISTANCE + CELLS_DISTANCE + CELLS_DISTANCE + BLOCK_WIDTH * (CELLS_WIDTH + 1) + CELLS_DISTANCE + CELLS_DISTANCE + CELLS_DISTANCE + 7) /* 7 Ϊ���ڱ�ռ�� */
/* ���ڸ߶� = ��϶ + �����ϱ߽� + ����CONTAINER_HIGH * (��϶ + CELLS_WIDTH) + ��϶ + �����±߽� + ��϶ + ������������ռ�ò��� */
#define WINDOW_HIGH (CELLS_DISTANCE + CELLS_DISTANCE + CONTAINER_HIGH * (CELLS_WIDTH + 1) + CELLS_DISTANCE + CELLS_DISTANCE + CELLS_DISTANCE + 33) /* 33 Ϊ�������߶� */

struct QUEUE_NODE_T
{
	int data;/* �ֶ����� */
	struct QUEUE_NODE_T *next;/* ��һ�ֶ����� */
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
	unsigned int GameState; /* ��Ϸ״̬��0����Ϸ���У� 1����Ϸ��ʼ��2����Ϸ��ͣ */
	unsigned int initalSpeed; /* ���ٶ� */
	unsigned int speed; /* �ٶ� */
	unsigned int score; /* �÷� */

	/* ��һ������ */
	const unsigned char *next_block;
	unsigned int next_kind;
	unsigned int next_rotate;

	/* ��ǰ���� */
	const unsigned char *current_block;
	unsigned int kind;
	unsigned int rotate;
	int offset_left; /* �߼����� left, ������Χ -min_left ~ CONTAINER_WIDTH - max_right */
	int offset_top; /* �߼����� top, ������Χ -min_top ~ CONTAINER_HIGH - max_bottom */
	int offset_top_destination; /* ����������ǰ����ֱ������λ�� */

	int min_left;
	int max_right;
	int min_top;
	int max_bottom;
} Tetris;
Tetris tetris = { 0 };

/* ������������ */
RECT ContainerRect = { CELLS_DISTANCE, CELLS_DISTANCE,CELLS_DISTANCE + CELLS_DISTANCE + CONTAINER_WIDTH * (CELLS_WIDTH + 1) + CELLS_DISTANCE + CELLS_DISTANCE,CELLS_DISTANCE + CELLS_DISTANCE + CONTAINER_HIGH * (CELLS_WIDTH + 1) + CELLS_DISTANCE + CELLS_DISTANCE };

/* ��һ������������������ */
RECT NextBlockRect = { CELLS_DISTANCE + CELLS_DISTANCE + CONTAINER_WIDTH * (CELLS_WIDTH + 1) + CELLS_DISTANCE + CELLS_DISTANCE + CELLS_DISTANCE + CELLS_DISTANCE,CELLS_DISTANCE,CELLS_DISTANCE + CELLS_DISTANCE + CONTAINER_WIDTH * (CELLS_WIDTH + 1) + CELLS_DISTANCE + CELLS_DISTANCE + CELLS_DISTANCE + CELLS_DISTANCE + CELLS_DISTANCE + BLOCK_WIDTH * (CELLS_WIDTH + 1) + CELLS_DISTANCE + CELLS_DISTANCE,CELLS_DISTANCE + CELLS_DISTANCE + BLOCK_WIDTH * (CELLS_WIDTH + 1) + CELLS_DISTANCE + CELLS_DISTANCE };



long WINAPI WndProc(HWND hWnd, UINT iMessage, UINT wParam, LONG lParam);
BOOLEAN InitWindowClass(HINSTANCE hinstance, int ncmdshow);
LRESULT CALLBACK WndProc(HWND hwnd, UINT message, WPARAM wparam, LPARAM lparam);
int WINAPI WinMain(HINSTANCE hinstance, HINSTANCE hprevinstance, LPSTR lpcmdline, int ncmdshow);
LRESULT CALLBACK About(HWND hDlg, UINT message, WPARAM wParam, LPARAM lParam);


#pragma comment(lib,"winmm.lib")//��������ͷ�ļ���

HWND hwnd;
HINSTANCE hinst;

static BOOL start = FALSE, rstart = FALSE, music = TRUE, gameover = FALSE, enter = TRUE;
HDC hdc = { 0 };
PAINTSTRUCT ps = { 0 };
LRESULT rs = 0;

//ͼƬ���صĶ���
static HDC  hmemdc_end, hmemdc_back;
static HBITMAP h_end_bitmap, h_back_bitmap;
static BITMAP end_bitmap, back_bitmap;
static bool fullclient = false;
RECT clientRect;

HMENU hmenu1, hmenu1add, hmenu2, hmenu2add, hadmenu;


/* �����з������� */
static const int ScoreSets[BLOCK_WIDTH] = { 100, 400, 800, 1600 };
static unsigned char Container[CONTAINER_HIGH][CONTAINER_WIDTH] = { 0 };

//FUCTION
//��Ϸ�����ж�
static void CheckGameOver(void);   //1�����ж�
static void GenerateBlock(void);//7�����Ϸ�Ƿ��Ѿ�����


static void CopyToContainer(void);   //2���������еķ���

//3��4��¼�������Ѵ��ڷ���
static unsigned int queue_push(QUEUE_NODE_T **head, int data);    
static unsigned int queue_pop(QUEUE_NODE_T **head, int *data);


static void CaculateScore(void);   //5 �Ʒ�
static void CaculateBlockBoundary(void);//6 ���㷽�鷶Χ

static unsigned int DetectCollision(const unsigned char *block, int offset_left, int offset_top);//8��ײ
static void StepLeft(void);//9����
static void StepRight(void);//10����
static int StepDown(void);//11�����˶�


static void UpdateCurrBlockDestinationPosition(void);//12���µ�ǰ����Ŀ�ĵ�λ��
static void StepRotate(void);//13����
static void OnDrawFrame(HDC hdc_mem);//14��ͼ

static void OnDrawSpeedSelect(HDC hdc_mem);//15�ٶ�ѡ��
static void OnDrawBlock(HDC hdc_mem);//16������
static void OnDrawResult(HDC hdc_mem);//17�����
static void OnPaint(HDC hdc);//��


void start_map();
void geme_backgound_bmp(HDC hdc);
void end_map();

int bgmmusic();
