#include "h.h"

#include <iostream>
using namespace std;
volatile bool g_bEndMusicThread;		//让线程结束的标志

HANDLE hThread;

//发送信息给MCI
bool SendToMCI(std::string command)
{
	if (!mciSendString(command.c_str(), NULL, 0, 0))
		return true;
	return false;
}

//从MCI获取信息
std::string GetFromMCI(std::string command)
{
	char message[20];
	mciSendString(command.c_str(), message, 20, 0);
	std::string str(message);
	return str;
}

//音乐线程的方法
DWORD WINAPI ThreadPlayMusic(PVOID pParam)
{
	std::string filepath = "bgm.wav";
	std::string Open = "OPEN " + filepath + " ALIAS MUSIC";
	std::string Close = "CLOSE MUSIC";
	std::string Status = "status MUSIC mode";
	std::string Play = "PLAY MUSIC FROM 0";
	SendToMCI(Open);	//打开音乐文件
	while (true)
	{
		Sleep(1);
		std::string result = GetFromMCI(Status);	//获取音乐状态
		if (result == "stopped")
			SendToMCI(Play);	//如果音乐停止，重新播放
		if (g_bEndMusicThread == true)		//接收主线程发送的退出信号
			break;
	}
	SendToMCI(Close);	//关闭音乐文件
	return 0;
}

int bgmmusic()
{
	g_bEndMusicThread = false;	//初始化子线程标志位
	hThread = (HANDLE)(HANDLE)CreateThread(NULL, 0, ThreadPlayMusic, NULL, 0, NULL);	//创建线程
	return 0;
}

int WINAPI WinMain(HINSTANCE hinstance, HINSTANCE hprevinstance, LPSTR lpcmdline, int ncmdshow)
{
	MSG msg;
	hinst = hinstance;
	if (!InitWindowClass(hinstance, ncmdshow))
	{
		MessageBox(NULL, "创建窗口失败！", _T("创建窗口"), NULL);
		return 1;
	}
	//HCURSOR s = LoadCursor(hinstance, "SNOW");
	bgmmusic();

	HACCEL haccel = LoadAccelerators(hinstance, "MYMENUACCEL");//加载加速键资源

	while (GetMessage(&msg, NULL, 0, 0))
	{
		if (!TranslateAccelerator(hwnd, haccel, &msg))
		{
			TranslateMessage(&msg);
			DispatchMessage(&msg);
		}
	}
	return (int)msg.wParam;
}

static LRESULT CALLBACK WndProc(HWND hwnd, UINT message, WPARAM wparam, LPARAM lparam)
{
	switch (message)
	{
	case WM_COMMAND://在主菜单添加弹出式菜单
		switch (LOWORD(wparam))
		{
		case IDM_START:
			start = TRUE;
			enter = FALSE;
			InvalidateRect(hwnd, NULL, 1);
			break;
		case IDM_RSTART:
			start = TRUE;
			// 重新开始游戏 
			if ((0 == tetris.GameState) || (3 == tetris.GameState))
			{
				gameover = 0;
				//保留初始化速度 
				unsigned int temp = tetris.initalSpeed;
				//初始化游戏 
				memset((void *)Container, 0, sizeof(unsigned char)* CONTAINER_HIGH * CONTAINER_WIDTH);
				memset((void *)&tetris, 0, sizeof(struct Tetris));
				tetris.GameState = 1;
				tetris.score = 0;
				tetris.initalSpeed = temp;
				tetris.speed = 0 + tetris.initalSpeed;
				GenerateBlock();
			}


			InvalidateRect(hwnd, NULL, 1);
			break;

		case IDM_EXIT:
			SendMessage(hwnd, WM_DESTROY, 0, 0);
			break;
		case IDM_MUSIC_CLOSE:
			g_bEndMusicThread = true;	//通知子线程退出
			WaitForSingleObject(hThread, INFINITE);		//线程结束之后再释放资源
			break;
		case IDM_HELP_INSTRUCT:
			MessageBox(hwnd, "”A,S,D，W“分别表示向左移动，向下移动，向右移动，变形", "帮助", MB_OK);
			break;
		case IDM_HELP_TIPS:
			MessageBox(hwnd, "www.baidu.com", "攻略", MB_OK);
			break;
		case IDM_ABOUT:
			DialogBox(hinst, (LPCTSTR)IDD_ABOUTBOX, hwnd, (DLGPROC)About);
			break;
			break;
		}//菜单栏结束
	case WM_TIMER:
	{
					 if (3 != tetris.GameState)
					 {
						 static unsigned int timercounter = 0;
						 static unsigned int interval = 0;
						 ++timercounter;
						 interval = 1000 / (tetris.speed + 1);
						 if ((timercounter * 20) >= interval)
						 {
							 if (1 == tetris.GameState)
							 {
								 StepDown();
							 }

							 timercounter = 0;
						 }
						 InvalidateRect(hwnd, NULL, 0);
					 }
	}
		break;//定时器设置结束

	case WM_CREATE:
		tetris.GameState = 3;
		tetris.initalSpeed = 0;
		SetTimer(hwnd, tetris.timerid, 20, (TIMERPROC)NULL);

		hdc = GetDC(hwnd);
		hmemdc_end= CreateCompatibleDC(hdc);
		hmemdc_back = CreateCompatibleDC(hdc);


		h_end_bitmap = LoadBitmap(hinst, MAKEINTRESOURCE(IDB_BITMAPEND));
		SelectObject(hmemdc_end, h_end_bitmap);
		GetObject(h_end_bitmap, sizeof(BITMAP), &end_bitmap);

		h_back_bitmap = LoadBitmap(hinst, MAKEINTRESOURCE(IDB_BITMAPBACK));
		SelectObject(hmemdc_back, h_back_bitmap);
		GetObject(h_back_bitmap, sizeof(BITMAP), &back_bitmap);

		ReleaseDC(hwnd, hdc);
		break;

	case WM_PAINT:
		hdc = BeginPaint(hwnd, &ps);
		if (enter)
			start_map();
		else if (gameover)
				end_map();
		else if (start)
			OnPaint(hdc);
		EndPaint(hwnd, &ps);
		break;

	case WM_KEYDOWN:
	{
					   switch (wparam)
					   {
					   case VK_UP:
					   {
									 
									 if (tetris.GameState == 1)
									 {
										 PlaySound((LPCTSTR)IDR_TURN, NULL, SND_RESOURCE | SND_ASYNC);
										 StepRotate();
									 }
									 if (tetris.GameState == 3)
									 {
										 PlaySound((LPCTSTR)IDR_CHOOSE, NULL, SND_RESOURCE | SND_ASYNC);
										 ++tetris.initalSpeed;
										 tetris.initalSpeed = tetris.initalSpeed > 49 ? 0 : tetris.initalSpeed;
									 }
					   }
						   break;
					   case VK_LEFT:
					   {

									   if (1 == tetris.GameState)
									   {
										   StepLeft();
									   }

					   }
						   break;
					   case VK_RIGHT:
					   {
										if (1 == tetris.GameState)
										{
											StepRight();
										}
					   }
						   break;
					   case VK_DOWN:
					   {
									   
									   if (1 == tetris.GameState)
									   {
										   PlaySound((LPCTSTR)IDR_ONESTEP, NULL, SND_RESOURCE | SND_ASYNC);
										   StepDown();
									   }
									   if (3 == tetris.GameState)
									   {
										   PlaySound((LPCTSTR)IDR_CHOOSE, NULL, SND_RESOURCE | SND_ASYNC);
										   --tetris.initalSpeed;
										   //无符号整数范围0 - ~0，0往下会转为最大数
										   tetris.initalSpeed = tetris.initalSpeed > 49 ? 49 : tetris.initalSpeed;
									   }
					   }
						   break;
					   case VK_SPACE:
					   {
										if (1 == tetris.GameState)
										{
											PlaySound((LPCTSTR)IDR_FALL, NULL, SND_RESOURCE | SND_ASYNC);
											// 按Space键落地 
											while (1 == StepDown())
											{
											}
										}
					   }
						   break;
					   case 'C':
					   {
								   //调整初速度，之后会直接开始新游戏 
								   if (3 != tetris.GameState)
								   {
									   
									   tetris.GameState = 3;
								   }
					   }
						   break;
					   case 'P':
					   {
								   if (1 == tetris.GameState)
								   {
									   PlaySound((LPCTSTR)IDR_PAUSE, NULL, SND_RESOURCE | SND_ASYNC);
									   //按P键暂停游戏 
									   tetris.GameState = 2;
								   }
					   }
						   break;
					   case 'S':
					   {
								   // 按S键离开暂停状态 
								   if (2 == tetris.GameState)
								   {
									   tetris.GameState = 1;
								   }
								   // 按S键重新开始游戏 
								   if ((0 == tetris.GameState) || (3 == tetris.GameState))
								   {
									   gameover = 0;
									   //保留初始化速度 
									   unsigned int temp = tetris.initalSpeed;
									   //初始化游戏 
									   memset((void *)Container, 0, sizeof(unsigned char)* CONTAINER_HIGH * CONTAINER_WIDTH);
									   memset((void *)&tetris, 0, sizeof(struct Tetris));
									   tetris.GameState = 1;
									   tetris.score = 0;
									   tetris.initalSpeed = temp;
									   tetris.speed = 0 + tetris.initalSpeed;
									   GenerateBlock();
								   }
					   }
						   break;
					   case VK_ESCAPE:
					   {
										 // 按Esc键退出游戏 
										 PostMessage(hwnd, WM_DESTROY, (WPARAM)NULL, (LPARAM)NULL);
					   }
						   break;
						   
					   default:
					   {
								  rs = DefWindowProc(hwnd, message, wparam, lparam); // default windows proc 
					   }
						   break;
					   }
					   InvalidateRect(hwnd, NULL, 0);
	}
		break;

		case WM_DESTROY:
		{
						   KillTimer(hwnd, tetris.timerid);
						   PostQuitMessage(0);
		}
			break;
		default:
		{
				   rs = DefWindowProc(hwnd, message, wparam, lparam); /* default windows proc */
		}
			break;
  }
	return rs;

}
BOOLEAN InitWindowClass(HINSTANCE hinstance, int ncmdshow)
{
	WNDCLASSEX wcex;
	TCHAR szwindclass[] = "Snow's Tetris";
	TCHAR sztitle[] = "Snow's Tetris";
	wcex.cbSize = sizeof(WNDCLASSEX);
	wcex.style = CS_HREDRAW | CS_VREDRAW;
	wcex.lpfnWndProc = WndProc;
	wcex.cbClsExtra = 0;
	wcex.cbWndExtra = 0;
	wcex.hInstance = hinstance;
	wcex.hIcon = LoadIcon(hinstance, MAKEINTRESOURCE(IDI_ICO));
	wcex.hCursor = LoadCursor(hinstance, MAKEINTRESOURCE(IDC_CURSOR1));
	wcex.hbrBackground = (HBRUSH)GetStockObject(WHITE_BRUSH);
	wcex.lpszMenuName = "MYMENUNAME";//将资源文件中名称为MYMENUNAME的菜单加载为窗口菜单
	wcex.lpszClassName = szwindclass;
	wcex.hIconSm = LoadIcon(wcex.hInstance, MAKEINTRESOURCE(IDI_APPLICATION));
	//wcex.hIconSm = LoadIcon(wcex.hInstance, (LPCTSTR)IDI_SMALL);
	if (!RegisterClassEx(&wcex))
		return FALSE;
	hwnd = CreateWindow(szwindclass, sztitle, WS_SYSMENU, CW_USEDEFAULT, CW_USEDEFAULT, 790, 690, NULL, NULL, hinstance, NULL);
	if (!hwnd)
		return FALSE;
	ShowWindow(hwnd, ncmdshow);
	UpdateWindow(hwnd);
	return TRUE;
}

static void CopyToContainer(void)
{
	int i = 0;
	int j = 0;

	for (i = tetris.min_top; i <= tetris.max_bottom; ++i)
	{
		for (j = tetris.min_left; j <= tetris.max_right; ++j)
		{
			if (1 == *(tetris.current_block + BLOCK_WIDTH * i + j)
				&& 0 <= tetris.offset_top + i && 0 <= tetris.offset_left + j)
			{   /* 只复制已经出现在游戏容器里面的方块 */
				Container[tetris.offset_top + i][tetris.offset_left + j] = 1;
			}
		}
	}
}

static unsigned int queue_push(QUEUE_NODE_T **head, int data)
{
	QUEUE_NODE_T *curnode = NULL;

	/* 创建尾节点 */
	if (NULL == *head)
	{
		/* 考虑字段链表头结点未出现的情况 */
		*head = (QUEUE_NODE_T *)malloc(sizeof(QUEUE_NODE_T));
		curnode = *head;
	}
	else
	{
		curnode = *head;
		/* 寻找尾节点 */
		while (NULL != curnode->next)
		{
			curnode = (QUEUE_NODE_T *)curnode->next;
		}
		/* 创建下一节点，并转到下一节点 */
		{
			curnode->next = (QUEUE_NODE_T *)malloc(sizeof(QUEUE_NODE_T));
			curnode = (QUEUE_NODE_T *)curnode->next;
		}
	}

	/* 尾节点赋值 */
	curnode->data = data;
	curnode->next = NULL;

	return 1;
}

static unsigned int queue_pop(QUEUE_NODE_T **head, int *data)
{
	QUEUE_NODE_T *temp = NULL;

	temp = (QUEUE_NODE_T *)(*head)->next;

	*data = (*head)->data;

	if (NULL != *head)
	{
		free(*head);
		*head = NULL;
	}

	*head = temp;

	return 1;
}

static void CaculateScore(void)
{
	QUEUE_NODE_T *head = NULL;
	unsigned int award = 0;
	int count = 0;
	int i = 0;
	int i_max = 0;
	int j = 0;

	/* 记录需要消去的行, 消行一定发生在放入积木块的4行 */
	/* 只考虑已经出现在游戏容器里面的方块 */
	i = (0 < tetris.offset_top + tetris.min_top) ? tetris.offset_top + tetris.min_top : 0;
	i_max = (0 < tetris.offset_top + tetris.max_bottom) ? tetris.offset_top + tetris.max_bottom : 0;
	for (; i <= i_max; ++i)
	{
		count = 0;

		for (j = 0; j < CONTAINER_WIDTH; ++j)
		{
			count += Container[i][j];
		}

		if (count == CONTAINER_WIDTH)
		{
			queue_push(&head, i);
			++award;
		}
	}

	if (0 != award)
	{
		/* 消行 */
		while (NULL != head)
		{
			int line = 0;
			queue_pop(&head, &line);

			for (i = line - 1; i >= 0; --i) /* row high */
			{
				memcpy(&Container[i + 1][0], &Container[i][0], sizeof(unsigned char)* CONTAINER_WIDTH);
			}

			/* 最顶层不可能出现方块消行(会导致游戏结束),所以保证了倒数第二行是空白的(从倒数第一行复制)，而倒数第一行空白也是合理的，不需调整 */
		}

		/* 得分 */
		tetris.score += ScoreSets[award - 1];

		/* 速度 */
		tetris.speed = tetris.score / 10000;
		tetris.speed += tetris.initalSpeed;
		tetris.speed %= 50;
	}

}

static void CaculateBlockBoundary(void)
{
	int i = 0;
	int j = 0;
	int isFounded = 0;

	/* CaculateMinLeft */
	isFounded = 0;
	for (j = 0; j < BLOCK_WIDTH && 0 == isFounded; ++j)
	{
		for (i = 0; i < BLOCK_WIDTH && 0 == isFounded; ++i)
		{
			if (1 == *(tetris.current_block + BLOCK_WIDTH * i + j))
			{
				tetris.min_left = j;
				isFounded = 1;
			}
		}
	}

	/* CaculateMaxRight */
	isFounded = 0;
	for (j = BLOCK_WIDTH - 1; j >= 0 && 0 == isFounded; --j)
	{
		for (i = 0; i < BLOCK_WIDTH && 0 == isFounded; ++i)
		{
			if (1 == *(tetris.current_block + BLOCK_WIDTH * i + j))
			{
				tetris.max_right = j;
				isFounded = 1;
			}
		}
	}

	/* CaculateMinTop */
	isFounded = 0;
	for (i = 0; i < BLOCK_WIDTH && 0 == isFounded; ++i)
	{
		for (j = 0; j < BLOCK_WIDTH && 0 == isFounded; ++j)
		{
			if (1 == *(tetris.current_block + BLOCK_WIDTH * i + j))
			{
				tetris.min_top = i;
				isFounded = 1;
			}
		}
	}

	/* CaculateMaxBottom */
	isFounded = 0;
	for (i = BLOCK_WIDTH - 1; i >= 0 && 0 == isFounded; --i)
	{
		for (j = 0; j < BLOCK_WIDTH && 0 == isFounded; ++j)
		{
			if (1 == *(tetris.current_block + BLOCK_WIDTH * i + j))
			{
				tetris.max_bottom = i;
				isFounded = 1;
			}
		}
	}
}

static void CheckGameOver(void)
{
	int i = 0;
	int j = 0;
	int lineblockflags = 0; /* 0, 不存在方块， 1， 存在方块 */
	int gameoverflags = 1; /* 0, 不结束， 1， 结束 */

	for (i = 0; i < CONTAINER_HIGH && 1 == gameoverflags; ++i)
	{
		lineblockflags = 0;
		for (j = 0; j < CONTAINER_WIDTH && 0 == lineblockflags; ++j)
		{
			if (1 == Container[i][j])
			{
				lineblockflags = 1;
			}
		}
		if (0 == lineblockflags)
		{
			gameoverflags = 0;
		}
	}
	tetris.GameState = (0 == gameoverflags) ? 1 : 0;
}
static void GenerateBlock(void)
{
	/* 检查游戏是否已经结束 */
	CheckGameOver();
	if (0 == tetris.GameState)
	{
		start = 0;
		gameover = 1;
		PlaySound((LPCTSTR)IDR_GAMEOVERBGM, NULL, SND_RESOURCE | SND_ASYNC);
		return;
	}

	if (NULL == tetris.current_block)
	{
		/* 第一次 */
		srand((unsigned int)time(0));
		tetris.kind = rand() % BLOCK_KINDS;
		tetris.rotate = rand() % BLOCK_ROTATES;
		tetris.current_block = BlockSets + tetris.kind * BLOCK_SIZE * BLOCK_ROTATES + tetris.rotate * BLOCK_SIZE;

		tetris.next_kind = rand() % BLOCK_KINDS;
		tetris.next_rotate = rand() % BLOCK_ROTATES;
	}
	else
	{
		tetris.kind = tetris.next_kind;
		tetris.rotate = tetris.next_rotate;
		tetris.current_block = tetris.next_block;

		srand((unsigned int)time(0));
		tetris.next_kind = rand() % BLOCK_KINDS;
		tetris.next_rotate = rand() % BLOCK_ROTATES;
	}
	tetris.next_block = BlockSets + tetris.next_kind * BLOCK_SIZE * BLOCK_ROTATES + tetris.next_rotate * BLOCK_SIZE;

	CaculateBlockBoundary();

	/* 新方块从中间落下，一行行下落 */
	tetris.offset_left = (CONTAINER_WIDTH - BLOCK_WIDTH) / 2 - 1 + 1; /* 因原始数据偏向左边，所以补偿1 */
	tetris.offset_top = -tetris.max_bottom;
}

static unsigned int DetectCollision(const unsigned char *block, int offset_left, int offset_top)
{
	/* return 2， 发生碰撞(新积木与容器内积木碰撞);1, 发生碰撞(超出左、右、下边框); 0 没有碰撞 */
	unsigned int state = 0;

	if (0 <= offset_left + tetris.min_left /* 左边界检测 */
		&& offset_left + tetris.max_right <= CONTAINER_WIDTH - 1 /* 右边界检测 */
		/* 新方块可以允许上边界交叉 */
		/* && 0 <= offset_top + tetris.min_top 上边界检测 */
		&& offset_top + tetris.max_bottom <= CONTAINER_HIGH - 1) /* 下边界检测 */
	{
		int i = 0;
		int j = 0;

		for (i = tetris.min_top; i <= tetris.max_bottom && 0 == state; ++i)
		{
			for (j = tetris.min_left; j <= tetris.max_right && 0 == state; ++j)
			{
				if (1 == *(block + BLOCK_WIDTH * i + j)
					&& 1 == Container[offset_top + i][offset_left + j])
				{
					/* 新方块可以允许上边界交叉，但不允许和下方容器内积木碰撞(重叠) */
					/* 只考虑方块与容器内积木碰撞，忽略超出上边界部分 */
					if (0 <= offset_top + tetris.min_top)
					{
						state = 2; /* 2， 发生碰撞(新积木与容器内积木碰撞) */
					}
				}
			}
		}
		/* state = 0; 如果一直没有碰撞，此时state值仍为0 */
	}
	else
	{
		state = 1; /* 1, 发生碰撞(超出左、右、下边框) */
	}

	return state;
}

static void StepLeft(void)
{
	--tetris.offset_left;
	if (DetectCollision(tetris.current_block, tetris.offset_left, tetris.offset_top))
	{
		++tetris.offset_left;
	}
}

static void StepRight(void)
{
	++tetris.offset_left;
	if (DetectCollision(tetris.current_block, tetris.offset_left, tetris.offset_top))
	{
		--tetris.offset_left;
	}
}

static int StepDown(void)
{
	/* return 0,触底 1 正常下移*/
	++tetris.offset_top;
	if (DetectCollision(tetris.current_block, tetris.offset_left, tetris.offset_top))
	{
		--tetris.offset_top;
		CopyToContainer();
		CaculateScore();
		GenerateBlock();

		return 0;
	}

	return 1;
}

static void UpdateCurrBlockDestinationPosition(void)
{
	tetris.offset_top_destination = tetris.offset_top;
	for (;;)
	{
		if (DetectCollision(tetris.current_block, tetris.offset_left, tetris.offset_top_destination))
		{
			--tetris.offset_top_destination;

			break;
		}

		++tetris.offset_top_destination;
	}
}

static void StepRotate(void)
{
	++tetris.rotate;
	tetris.rotate %= BLOCK_ROTATES;
	tetris.current_block = BlockSets + tetris.kind * BLOCK_SIZE * BLOCK_ROTATES + tetris.rotate * BLOCK_SIZE;

	/* 计算新边界 */
	CaculateBlockBoundary();

	/* 在边界处旋转需要调整offset_left,offset_top */
	/* 右边界 */
	if (tetris.offset_left + tetris.max_right > (CONTAINER_WIDTH - 1))
	{
		tetris.offset_left = (CONTAINER_WIDTH - 1) - tetris.max_right;
	}
	/* 左边界 */
	if (tetris.offset_left + tetris.min_left < 0)
	{
		tetris.offset_left = -tetris.min_left;
	}
	/* 下边界 */
	if (tetris.offset_top + tetris.max_bottom > CONTAINER_HIGH - 1)
	{
		tetris.offset_top = CONTAINER_HIGH - 1 - tetris.max_bottom;
	}

	if (DetectCollision(tetris.current_block, tetris.offset_left, tetris.offset_top))
	{
		--tetris.rotate;
		tetris.rotate %= BLOCK_ROTATES;
		tetris.current_block = BlockSets + tetris.kind * BLOCK_SIZE * BLOCK_ROTATES + tetris.rotate * BLOCK_SIZE;

		/* 恢复旋转前的边界 */
		CaculateBlockBoundary();
	}
}


static void OnDrawFrame(HDC hdc_mem)
{
	HPEN hPen = { 0 };
	HPEN OldPen = { 0 };
	HBRUSH hBrush = { 0 };
	HBRUSH OldhBrush = { 0 };

	gameover = 0;
	/* 创建一个虚线画笔 */
	hPen = CreatePen(PS_SOLID, 1, RGB(0xC0, 0xC0, 0xC0));

	/* 将虚线画笔分配给DC */
	OldPen = (HPEN)SelectObject(hdc_mem, hPen);

	//创建一个画刷
	hBrush = CreateSolidBrush(RGB(255, 192, 203));
	OldhBrush = (HBRUSH)SelectObject(hdc_mem, hBrush);//选择画刷)

	/* 当前方块容器边界 */
	Rectangle(hdc_mem, ContainerRect.left, ContainerRect.top, ContainerRect.right, ContainerRect.bottom);


	hBrush = CreateSolidBrush(RGB(189, 255, 127));//随机出现方块的位置的方框的颜色
	OldhBrush = (HBRUSH)SelectObject(hdc_mem, hBrush);//选择画刷)

	/* 下一个方块容器边界 */
	Rectangle(hdc_mem, NextBlockRect.left, NextBlockRect.top, NextBlockRect.right, NextBlockRect.bottom);

	/* 恢复设置 */
	SelectObject(hdc_mem, OldPen);
	DeleteObject(hPen);

	//删除画刷
	SelectObject(hdc_mem,OldhBrush);
	DeleteObject(hBrush);

	return;
}

static void OnDrawSpeedSelect(HDC hdc_mem)
{
	TCHAR buffer[64] = { 0 };
	int size = 0;

	size = wsprintf(buffer, TEXT("初始速度增加 = ↑"));
	TextOut(hdc_mem, ContainerRect.left + (CONTAINER_WIDTH / 3) * CELLS_WIDTH, ContainerRect.top + (CONTAINER_HIGH / 2) * CELLS_WIDTH + 1 * CELLS_WIDTH, buffer, size);

	size = wsprintf(buffer, TEXT("初始速度减少 = ↓"));
	TextOut(hdc_mem, ContainerRect.left + (CONTAINER_WIDTH / 3) * CELLS_WIDTH, ContainerRect.top + (CONTAINER_HIGH / 2) * CELLS_WIDTH + 2 * CELLS_WIDTH, buffer, size);

	size = wsprintf(buffer, TEXT("设置结束开始游戏 = S"));
	TextOut(hdc_mem, ContainerRect.left + (CONTAINER_WIDTH / 3) * CELLS_WIDTH, ContainerRect.top + (CONTAINER_HIGH / 2) * CELLS_WIDTH + 3 * CELLS_WIDTH, buffer, size);

	size = wsprintf(buffer, TEXT("当前初始速度为: %d"), tetris.initalSpeed + 1);
	TextOut(hdc_mem, ContainerRect.left + (CONTAINER_WIDTH / 3) * CELLS_WIDTH, ContainerRect.top + (CONTAINER_HIGH / 2) * CELLS_WIDTH + 4 * CELLS_WIDTH, buffer, size);


	return;
}

static void OnDrawBlock(HDC hdc_mem)
{
	HBRUSH hBrushPrompt = { 0 };

	HBRUSH hBrushBlue = { 0 };
	HBRUSH hOldBrush = { 0 };

	/* 创建一个实心刷子,系统用来绘制要填充图形的内部区域的位图 */
	hBrushBlue = CreateSolidBrush(RGB(106, 90, 205)); /* 蓝色 */

	hBrushPrompt = CreateSolidBrush(RGB(0, 0, 0)); /* 黑色 */
	
	
	//DrawCurrentBlockDestinationPosition
	{
		int i = 0;
		int j = 0;

		for (i = 0; i < BLOCK_WIDTH; ++i)
		{
			for (j = 0; j < BLOCK_WIDTH; ++j)
			{
				/* 只画出现在容器内的方块 */
				if (1 == *(tetris.current_block + BLOCK_WIDTH * i + j)
					&& 0 <= tetris.offset_left + j && 0 <= tetris.offset_top_destination + i)
				{
					Rectangle(hdc_mem,
						ContainerRect.left + CELLS_DISTANCE + CELLS_DISTANCE + (tetris.offset_left + j) * (CELLS_WIDTH + 1),
						ContainerRect.top + CELLS_DISTANCE + CELLS_DISTANCE + (tetris.offset_top_destination + i) * (CELLS_WIDTH + 1),
						ContainerRect.left + CELLS_DISTANCE + CELLS_DISTANCE + (tetris.offset_left + j) * (CELLS_WIDTH + 1) + CELLS_WIDTH,
						ContainerRect.top + CELLS_DISTANCE + CELLS_DISTANCE + (tetris.offset_top_destination + i) * (CELLS_WIDTH + 1) + CELLS_WIDTH);
				}
			}
		}
	}

	/* 将实心刷子分配给内存DC */
	hOldBrush = (HBRUSH)SelectObject(hdc_mem, hBrushBlue);
	DeleteObject(hBrushPrompt);

	//DrawCurrentBlock
	{
		int i = 0;
		int j = 0;

		for (i = 0; i < BLOCK_WIDTH; ++i)
		{
			for (j = 0; j < BLOCK_WIDTH; ++j)
			{
				/* 只画出现在容器内的方块 */
				if (1 == *(tetris.current_block + BLOCK_WIDTH * i + j)
					&& 0 <= tetris.offset_left + j && 0 <= tetris.offset_top + i)
				{
					Rectangle(hdc_mem,
						ContainerRect.left + CELLS_DISTANCE + CELLS_DISTANCE + (tetris.offset_left + j) * (CELLS_WIDTH + 1),
						ContainerRect.top + CELLS_DISTANCE + CELLS_DISTANCE + (tetris.offset_top + i) * (CELLS_WIDTH + 1),
						ContainerRect.left + CELLS_DISTANCE + CELLS_DISTANCE + (tetris.offset_left + j) * (CELLS_WIDTH + 1) + CELLS_WIDTH,
						ContainerRect.top + CELLS_DISTANCE + CELLS_DISTANCE + (tetris.offset_top + i) * (CELLS_WIDTH + 1) + CELLS_WIDTH);
				}
			}
		}
	}

	
	//DrawContainer
	{
		int i = 0;
		int j = 0;
		for (i = 0; i < CONTAINER_HIGH; ++i)
		{
			for (j = 0; j < CONTAINER_WIDTH; ++j)
			{
				if (1 == Container[i][j])
				{
					Rectangle(hdc_mem,
						ContainerRect.left + CELLS_DISTANCE + CELLS_DISTANCE + j * (CELLS_WIDTH + 1),
						ContainerRect.top + CELLS_DISTANCE + CELLS_DISTANCE + i * (CELLS_WIDTH + 1),
						ContainerRect.left + CELLS_DISTANCE + CELLS_DISTANCE + j * (CELLS_WIDTH + 1) + CELLS_WIDTH,
						ContainerRect.top + CELLS_DISTANCE + CELLS_DISTANCE + i * (CELLS_WIDTH + 1) + CELLS_WIDTH);
				}
			}
		}
	}


	//DrawNextBlock
	{
		int i = 0;
		int j = 0;

		for (i = 0; i < BLOCK_WIDTH; ++i)
		{
			for (j = 0; j < BLOCK_WIDTH; ++j)
			{
				if (1 == *(tetris.next_block + BLOCK_WIDTH * i + j))
				{
					Rectangle(hdc_mem,
						NextBlockRect.left + CELLS_DISTANCE + CELLS_DISTANCE + j * (CELLS_WIDTH + 1),
						NextBlockRect.top + CELLS_DISTANCE + CELLS_DISTANCE + i * (CELLS_WIDTH + 1),
						NextBlockRect.left + CELLS_DISTANCE + CELLS_DISTANCE + j * (CELLS_WIDTH + 1) + CELLS_WIDTH,
						NextBlockRect.top + CELLS_DISTANCE + CELLS_DISTANCE + i * (CELLS_WIDTH + 1) + CELLS_WIDTH);
				}
			}
		}
	}

	/* 恢复设置 */
	SelectObject(hdc_mem, hOldBrush);
	DeleteObject(hBrushBlue);
	return;
}

static void OnDrawResult(HDC hdc_mem)
{
	TCHAR buffer[64] = { 0 };
	int size = 0;

	size = wsprintf(buffer, TEXT("%d"), tetris.score);
	TextOut(hdc_mem, NextBlockRect.left + CELLS_DISTANCE, NextBlockRect.bottom + 3 * CELLS_WIDTH, TEXT("Score:"), 6);
	TextOut(hdc_mem, NextBlockRect.left + CELLS_DISTANCE, NextBlockRect.bottom + 4* CELLS_WIDTH, buffer, size);

	size = wsprintf(buffer, TEXT("%d"), tetris.speed + 1);
	TextOut(hdc_mem, NextBlockRect.left + CELLS_DISTANCE, NextBlockRect.bottom + 5 * CELLS_WIDTH, TEXT("Speed:"), 6);
	TextOut(hdc_mem, NextBlockRect.left + CELLS_DISTANCE, NextBlockRect.bottom + 6 * CELLS_WIDTH, buffer, size);

	switch (tetris.GameState)
	{
	case 0:
	{
			  TextOut(hdc_mem, NextBlockRect.left + CELLS_DISTANCE, NextBlockRect.bottom + 7 * CELLS_WIDTH, TEXT("Game Over!!!"), 12);

	}
		break;
	case 1:
	{
	}
		break;
	case 2:
	{
			  //NextBlockRect.left + CELLS_DISTANCE
			  TextOut(hdc_mem, NextBlockRect.left/2, NextBlockRect.bottom + 2 * CELLS_WIDTH, TEXT("Game Pause!!"), 12);
	}
		break;
	default:
	{
	}
		break;
	}
	size = wsprintf(buffer, TEXT("旋转= ↑"));
	TextOut(hdc_mem, NextBlockRect.left + CELLS_DISTANCE, NextBlockRect.bottom + 9 * CELLS_WIDTH, buffer, size);

	size = wsprintf(buffer, TEXT("左移= ←"));
	TextOut(hdc_mem, NextBlockRect.left + CELLS_DISTANCE, NextBlockRect.bottom + 10 * CELLS_WIDTH, buffer, size);

	size = wsprintf(buffer, TEXT("右移= →"));
	TextOut(hdc_mem, NextBlockRect.left + CELLS_DISTANCE, NextBlockRect.bottom + 11 * CELLS_WIDTH, buffer, size);

	size = wsprintf(buffer, TEXT("下移= ↓"));
	TextOut(hdc_mem, NextBlockRect.left + CELLS_DISTANCE, NextBlockRect.bottom + 12 * CELLS_WIDTH, buffer, size);

	size = wsprintf(buffer, TEXT("落地= Space"));
	TextOut(hdc_mem, NextBlockRect.left + CELLS_DISTANCE, NextBlockRect.bottom + 13 * CELLS_WIDTH, buffer, size);

	size = wsprintf(buffer, TEXT("初速度= C"));
	TextOut(hdc_mem, NextBlockRect.left + CELLS_DISTANCE, NextBlockRect.bottom + 14 * CELLS_WIDTH, buffer, size);
	size = wsprintf(buffer, TEXT("暂停= P"));
	TextOut(hdc_mem, NextBlockRect.left + CELLS_DISTANCE, NextBlockRect.bottom + 15 * CELLS_WIDTH, buffer, size);

	return;
}

static void OnPaint(HDC hdc)
{
	HDC hdc_mem = { 0 };
	HBITMAP hBitmap = { 0 };
	HBITMAP hOldBitmap = { 0 };

	//创建内存DC 
	hdc_mem = CreateCompatibleDC(hdc);
	//创建一个bmp内存空间 
	//hBitmap = CreateCompatibleBitmap(hdc, WINDOW_WIDTH, WINDOW_HIGH);
	hBitmap = CreateCompatibleBitmap(hdc, GetSystemMetrics(SM_CXSCREEN), GetSystemMetrics(SM_CYSCREEN));
	//将bmp内存空间分配给内存DC 
	hOldBitmap = (HBITMAP)SelectObject(hdc_mem, hBitmap);
	//先用背景色将位图清除干净，这里用的是白色作为背景 
	//PatBlt(hdc_mem, 0, 0, WINDOW_WIDTH, WINDOW_HIGH, WHITENESS);
	PatBlt(hdc_mem, 0, 0, GetSystemMetrics(SM_CXSCREEN), GetSystemMetrics(SM_CYSCREEN), WHITENESS);
	
	geme_backgound_bmp(hdc_mem);//加载游戏背景图片
	OnDrawFrame(hdc_mem);

	if (3 == tetris.GameState)
	{
		/* 速度选择 */
		OnDrawSpeedSelect(hdc_mem);
	}
	else
	{
		/* 正常游戏 */
		UpdateCurrBlockDestinationPosition();
		OnDrawBlock(hdc_mem);
	}

	OnDrawResult(hdc_mem);

	/* 将内存DC的内容复制到屏幕显示DC中,完成显示 */
	BitBlt(hdc, 0, 0, GetSystemMetrics(SM_CXSCREEN), GetSystemMetrics(SM_CYSCREEN), hdc_mem, 0, 0, SRCCOPY);

	SelectObject(hdc_mem, hOldBitmap);
	DeleteObject(hBitmap);
	DeleteDC(hdc_mem);
}

void start_map()
{
	HDC hmemdc_load; 
	HBITMAP h_load_bitmap; 
	BITMAP load_bitmap; 
	bool fullclient = false; 
	hdc = GetDC(hwnd);
	hmemdc_load = CreateCompatibleDC(hdc); 
	h_load_bitmap = LoadBitmap(hinst, MAKEINTRESOURCE(IDB_BITMAPLOADING));
	SelectObject(hmemdc_load, h_load_bitmap);
	GetObject(h_load_bitmap, sizeof(BITMAP), &load_bitmap);
	if (!fullclient)
	{
		GetClientRect(hwnd, &clientRect);
		StretchBlt(hdc, 0, 0, clientRect.right - clientRect.left, clientRect.bottom - clientRect.top, hmemdc_load, 0, 0, load_bitmap.bmWidth, load_bitmap.bmHeight, SRCCOPY);
	}
	else
		BitBlt(hdc, 0, 0, load_bitmap.bmWidth, load_bitmap.bmHeight, hmemdc_load, 0, 0, SRCCOPY);

}
void geme_backgound_bmp(HDC hdc)
{
	if (!fullclient)
	{
		GetClientRect(hwnd, &clientRect);
		StretchBlt(hdc,500, 0, 900, clientRect.bottom, hmemdc_back, 0, 0, 3*back_bitmap.bmWidth, back_bitmap.bmHeight, SRCCOPY);
	}
	else
		BitBlt(hdc, 0, 0, back_bitmap.bmWidth,back_bitmap.bmHeight, hmemdc_back, 0, 0, SRCCOPY);
}
void end_map()
{
	//显示结束界面
	if (!fullclient)
	{
		GetClientRect(hwnd, &clientRect);
		StretchBlt(hdc, 0, 0, clientRect.right - clientRect.left, clientRect.bottom - clientRect.top, hmemdc_end, 0, 0, end_bitmap.bmWidth, end_bitmap.bmHeight, SRCCOPY);
	}
	else
		BitBlt(hdc, 0, 0, end_bitmap.bmWidth, end_bitmap.bmHeight, hmemdc_end, 0, 0, SRCCOPY);

}
LRESULT CALLBACK About(HWND hDlg, UINT message, WPARAM wParam, LPARAM lParam)
{
	switch (message)
	{
	case WM_INITDIALOG:
		return TRUE;

	case WM_COMMAND:
		if (LOWORD(wParam) == IDOK || LOWORD(wParam) == IDCANCEL)
		{
			EndDialog(hDlg, LOWORD(wParam));
			return TRUE;
		}
		break;
	}
	return FALSE;
}




