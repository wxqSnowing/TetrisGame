#include "h.h"

#include <iostream>
using namespace std;
volatile bool g_bEndMusicThread;		//���߳̽����ı�־

HANDLE hThread;

//������Ϣ��MCI
bool SendToMCI(std::string command)
{
	if (!mciSendString(command.c_str(), NULL, 0, 0))
		return true;
	return false;
}

//��MCI��ȡ��Ϣ
std::string GetFromMCI(std::string command)
{
	char message[20];
	mciSendString(command.c_str(), message, 20, 0);
	std::string str(message);
	return str;
}

//�����̵߳ķ���
DWORD WINAPI ThreadPlayMusic(PVOID pParam)
{
	std::string filepath = "bgm.wav";
	std::string Open = "OPEN " + filepath + " ALIAS MUSIC";
	std::string Close = "CLOSE MUSIC";
	std::string Status = "status MUSIC mode";
	std::string Play = "PLAY MUSIC FROM 0";
	SendToMCI(Open);	//�������ļ�
	while (true)
	{
		Sleep(1);
		std::string result = GetFromMCI(Status);	//��ȡ����״̬
		if (result == "stopped")
			SendToMCI(Play);	//�������ֹͣ�����²���
		if (g_bEndMusicThread == true)		//�������̷߳��͵��˳��ź�
			break;
	}
	SendToMCI(Close);	//�ر������ļ�
	return 0;
}

int bgmmusic()
{
	g_bEndMusicThread = false;	//��ʼ�����̱߳�־λ
	hThread = (HANDLE)(HANDLE)CreateThread(NULL, 0, ThreadPlayMusic, NULL, 0, NULL);	//�����߳�
	return 0;
}

int WINAPI WinMain(HINSTANCE hinstance, HINSTANCE hprevinstance, LPSTR lpcmdline, int ncmdshow)
{
	MSG msg;
	hinst = hinstance;
	if (!InitWindowClass(hinstance, ncmdshow))
	{
		MessageBox(NULL, "��������ʧ�ܣ�", _T("��������"), NULL);
		return 1;
	}
	//HCURSOR s = LoadCursor(hinstance, "SNOW");
	bgmmusic();

	HACCEL haccel = LoadAccelerators(hinstance, "MYMENUACCEL");//���ؼ��ټ���Դ

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
	case WM_COMMAND://�����˵���ӵ���ʽ�˵�
		switch (LOWORD(wparam))
		{
		case IDM_START:
			start = TRUE;
			enter = FALSE;
			InvalidateRect(hwnd, NULL, 1);
			break;
		case IDM_RSTART:
			start = TRUE;
			// ���¿�ʼ��Ϸ 
			if ((0 == tetris.GameState) || (3 == tetris.GameState))
			{
				gameover = 0;
				//������ʼ���ٶ� 
				unsigned int temp = tetris.initalSpeed;
				//��ʼ����Ϸ 
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
			g_bEndMusicThread = true;	//֪ͨ���߳��˳�
			WaitForSingleObject(hThread, INFINITE);		//�߳̽���֮�����ͷ���Դ
			break;
		case IDM_HELP_INSTRUCT:
			MessageBox(hwnd, "��A,S,D��W���ֱ��ʾ�����ƶ��������ƶ��������ƶ�������", "����", MB_OK);
			break;
		case IDM_HELP_TIPS:
			MessageBox(hwnd, "www.baidu.com", "����", MB_OK);
			break;
		case IDM_ABOUT:
			DialogBox(hinst, (LPCTSTR)IDD_ABOUTBOX, hwnd, (DLGPROC)About);
			break;
			break;
		}//�˵�������
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
		break;//��ʱ�����ý���

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
										   //�޷���������Χ0 - ~0��0���»�תΪ�����
										   tetris.initalSpeed = tetris.initalSpeed > 49 ? 49 : tetris.initalSpeed;
									   }
					   }
						   break;
					   case VK_SPACE:
					   {
										if (1 == tetris.GameState)
										{
											PlaySound((LPCTSTR)IDR_FALL, NULL, SND_RESOURCE | SND_ASYNC);
											// ��Space����� 
											while (1 == StepDown())
											{
											}
										}
					   }
						   break;
					   case 'C':
					   {
								   //�������ٶȣ�֮���ֱ�ӿ�ʼ����Ϸ 
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
									   //��P����ͣ��Ϸ 
									   tetris.GameState = 2;
								   }
					   }
						   break;
					   case 'S':
					   {
								   // ��S���뿪��ͣ״̬ 
								   if (2 == tetris.GameState)
								   {
									   tetris.GameState = 1;
								   }
								   // ��S�����¿�ʼ��Ϸ 
								   if ((0 == tetris.GameState) || (3 == tetris.GameState))
								   {
									   gameover = 0;
									   //������ʼ���ٶ� 
									   unsigned int temp = tetris.initalSpeed;
									   //��ʼ����Ϸ 
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
										 // ��Esc���˳���Ϸ 
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
	wcex.lpszMenuName = "MYMENUNAME";//����Դ�ļ�������ΪMYMENUNAME�Ĳ˵�����Ϊ���ڲ˵�
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
			{   /* ֻ�����Ѿ���������Ϸ��������ķ��� */
				Container[tetris.offset_top + i][tetris.offset_left + j] = 1;
			}
		}
	}
}

static unsigned int queue_push(QUEUE_NODE_T **head, int data)
{
	QUEUE_NODE_T *curnode = NULL;

	/* ����β�ڵ� */
	if (NULL == *head)
	{
		/* �����ֶ�����ͷ���δ���ֵ���� */
		*head = (QUEUE_NODE_T *)malloc(sizeof(QUEUE_NODE_T));
		curnode = *head;
	}
	else
	{
		curnode = *head;
		/* Ѱ��β�ڵ� */
		while (NULL != curnode->next)
		{
			curnode = (QUEUE_NODE_T *)curnode->next;
		}
		/* ������һ�ڵ㣬��ת����һ�ڵ� */
		{
			curnode->next = (QUEUE_NODE_T *)malloc(sizeof(QUEUE_NODE_T));
			curnode = (QUEUE_NODE_T *)curnode->next;
		}
	}

	/* β�ڵ㸳ֵ */
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

	/* ��¼��Ҫ��ȥ����, ����һ�������ڷ����ľ���4�� */
	/* ֻ�����Ѿ���������Ϸ��������ķ��� */
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
		/* ���� */
		while (NULL != head)
		{
			int line = 0;
			queue_pop(&head, &line);

			for (i = line - 1; i >= 0; --i) /* row high */
			{
				memcpy(&Container[i + 1][0], &Container[i][0], sizeof(unsigned char)* CONTAINER_WIDTH);
			}

			/* ��㲻���ܳ��ַ�������(�ᵼ����Ϸ����),���Ա�֤�˵����ڶ����ǿհ׵�(�ӵ�����һ�и���)����������һ�пհ�Ҳ�Ǻ���ģ�������� */
		}

		/* �÷� */
		tetris.score += ScoreSets[award - 1];

		/* �ٶ� */
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
	int lineblockflags = 0; /* 0, �����ڷ��飬 1�� ���ڷ��� */
	int gameoverflags = 1; /* 0, �������� 1�� ���� */

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
	/* �����Ϸ�Ƿ��Ѿ����� */
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
		/* ��һ�� */
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

	/* �·�����м����£�һ�������� */
	tetris.offset_left = (CONTAINER_WIDTH - BLOCK_WIDTH) / 2 - 1 + 1; /* ��ԭʼ����ƫ����ߣ����Բ���1 */
	tetris.offset_top = -tetris.max_bottom;
}

static unsigned int DetectCollision(const unsigned char *block, int offset_left, int offset_top)
{
	/* return 2�� ������ײ(�»�ľ�������ڻ�ľ��ײ);1, ������ײ(�������ҡ��±߿�); 0 û����ײ */
	unsigned int state = 0;

	if (0 <= offset_left + tetris.min_left /* ��߽��� */
		&& offset_left + tetris.max_right <= CONTAINER_WIDTH - 1 /* �ұ߽��� */
		/* �·�����������ϱ߽罻�� */
		/* && 0 <= offset_top + tetris.min_top �ϱ߽��� */
		&& offset_top + tetris.max_bottom <= CONTAINER_HIGH - 1) /* �±߽��� */
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
					/* �·�����������ϱ߽罻�棬����������·������ڻ�ľ��ײ(�ص�) */
					/* ֻ���Ƿ����������ڻ�ľ��ײ�����Գ����ϱ߽粿�� */
					if (0 <= offset_top + tetris.min_top)
					{
						state = 2; /* 2�� ������ײ(�»�ľ�������ڻ�ľ��ײ) */
					}
				}
			}
		}
		/* state = 0; ���һֱû����ײ����ʱstateֵ��Ϊ0 */
	}
	else
	{
		state = 1; /* 1, ������ײ(�������ҡ��±߿�) */
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
	/* return 0,���� 1 ��������*/
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

	/* �����±߽� */
	CaculateBlockBoundary();

	/* �ڱ߽紦��ת��Ҫ����offset_left,offset_top */
	/* �ұ߽� */
	if (tetris.offset_left + tetris.max_right > (CONTAINER_WIDTH - 1))
	{
		tetris.offset_left = (CONTAINER_WIDTH - 1) - tetris.max_right;
	}
	/* ��߽� */
	if (tetris.offset_left + tetris.min_left < 0)
	{
		tetris.offset_left = -tetris.min_left;
	}
	/* �±߽� */
	if (tetris.offset_top + tetris.max_bottom > CONTAINER_HIGH - 1)
	{
		tetris.offset_top = CONTAINER_HIGH - 1 - tetris.max_bottom;
	}

	if (DetectCollision(tetris.current_block, tetris.offset_left, tetris.offset_top))
	{
		--tetris.rotate;
		tetris.rotate %= BLOCK_ROTATES;
		tetris.current_block = BlockSets + tetris.kind * BLOCK_SIZE * BLOCK_ROTATES + tetris.rotate * BLOCK_SIZE;

		/* �ָ���תǰ�ı߽� */
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
	/* ����һ�����߻��� */
	hPen = CreatePen(PS_SOLID, 1, RGB(0xC0, 0xC0, 0xC0));

	/* �����߻��ʷ����DC */
	OldPen = (HPEN)SelectObject(hdc_mem, hPen);

	//����һ����ˢ
	hBrush = CreateSolidBrush(RGB(255, 192, 203));
	OldhBrush = (HBRUSH)SelectObject(hdc_mem, hBrush);//ѡ��ˢ)

	/* ��ǰ���������߽� */
	Rectangle(hdc_mem, ContainerRect.left, ContainerRect.top, ContainerRect.right, ContainerRect.bottom);


	hBrush = CreateSolidBrush(RGB(189, 255, 127));//������ַ����λ�õķ������ɫ
	OldhBrush = (HBRUSH)SelectObject(hdc_mem, hBrush);//ѡ��ˢ)

	/* ��һ�����������߽� */
	Rectangle(hdc_mem, NextBlockRect.left, NextBlockRect.top, NextBlockRect.right, NextBlockRect.bottom);

	/* �ָ����� */
	SelectObject(hdc_mem, OldPen);
	DeleteObject(hPen);

	//ɾ����ˢ
	SelectObject(hdc_mem,OldhBrush);
	DeleteObject(hBrush);

	return;
}

static void OnDrawSpeedSelect(HDC hdc_mem)
{
	TCHAR buffer[64] = { 0 };
	int size = 0;

	size = wsprintf(buffer, TEXT("��ʼ�ٶ����� = ��"));
	TextOut(hdc_mem, ContainerRect.left + (CONTAINER_WIDTH / 3) * CELLS_WIDTH, ContainerRect.top + (CONTAINER_HIGH / 2) * CELLS_WIDTH + 1 * CELLS_WIDTH, buffer, size);

	size = wsprintf(buffer, TEXT("��ʼ�ٶȼ��� = ��"));
	TextOut(hdc_mem, ContainerRect.left + (CONTAINER_WIDTH / 3) * CELLS_WIDTH, ContainerRect.top + (CONTAINER_HIGH / 2) * CELLS_WIDTH + 2 * CELLS_WIDTH, buffer, size);

	size = wsprintf(buffer, TEXT("���ý�����ʼ��Ϸ = S"));
	TextOut(hdc_mem, ContainerRect.left + (CONTAINER_WIDTH / 3) * CELLS_WIDTH, ContainerRect.top + (CONTAINER_HIGH / 2) * CELLS_WIDTH + 3 * CELLS_WIDTH, buffer, size);

	size = wsprintf(buffer, TEXT("��ǰ��ʼ�ٶ�Ϊ: %d"), tetris.initalSpeed + 1);
	TextOut(hdc_mem, ContainerRect.left + (CONTAINER_WIDTH / 3) * CELLS_WIDTH, ContainerRect.top + (CONTAINER_HIGH / 2) * CELLS_WIDTH + 4 * CELLS_WIDTH, buffer, size);


	return;
}

static void OnDrawBlock(HDC hdc_mem)
{
	HBRUSH hBrushPrompt = { 0 };

	HBRUSH hBrushBlue = { 0 };
	HBRUSH hOldBrush = { 0 };

	/* ����һ��ʵ��ˢ��,ϵͳ��������Ҫ���ͼ�ε��ڲ������λͼ */
	hBrushBlue = CreateSolidBrush(RGB(106, 90, 205)); /* ��ɫ */

	hBrushPrompt = CreateSolidBrush(RGB(0, 0, 0)); /* ��ɫ */
	
	
	//DrawCurrentBlockDestinationPosition
	{
		int i = 0;
		int j = 0;

		for (i = 0; i < BLOCK_WIDTH; ++i)
		{
			for (j = 0; j < BLOCK_WIDTH; ++j)
			{
				/* ֻ�������������ڵķ��� */
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

	/* ��ʵ��ˢ�ӷ�����ڴ�DC */
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
				/* ֻ�������������ڵķ��� */
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

	/* �ָ����� */
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
	size = wsprintf(buffer, TEXT("��ת= ��"));
	TextOut(hdc_mem, NextBlockRect.left + CELLS_DISTANCE, NextBlockRect.bottom + 9 * CELLS_WIDTH, buffer, size);

	size = wsprintf(buffer, TEXT("����= ��"));
	TextOut(hdc_mem, NextBlockRect.left + CELLS_DISTANCE, NextBlockRect.bottom + 10 * CELLS_WIDTH, buffer, size);

	size = wsprintf(buffer, TEXT("����= ��"));
	TextOut(hdc_mem, NextBlockRect.left + CELLS_DISTANCE, NextBlockRect.bottom + 11 * CELLS_WIDTH, buffer, size);

	size = wsprintf(buffer, TEXT("����= ��"));
	TextOut(hdc_mem, NextBlockRect.left + CELLS_DISTANCE, NextBlockRect.bottom + 12 * CELLS_WIDTH, buffer, size);

	size = wsprintf(buffer, TEXT("���= Space"));
	TextOut(hdc_mem, NextBlockRect.left + CELLS_DISTANCE, NextBlockRect.bottom + 13 * CELLS_WIDTH, buffer, size);

	size = wsprintf(buffer, TEXT("���ٶ�= C"));
	TextOut(hdc_mem, NextBlockRect.left + CELLS_DISTANCE, NextBlockRect.bottom + 14 * CELLS_WIDTH, buffer, size);
	size = wsprintf(buffer, TEXT("��ͣ= P"));
	TextOut(hdc_mem, NextBlockRect.left + CELLS_DISTANCE, NextBlockRect.bottom + 15 * CELLS_WIDTH, buffer, size);

	return;
}

static void OnPaint(HDC hdc)
{
	HDC hdc_mem = { 0 };
	HBITMAP hBitmap = { 0 };
	HBITMAP hOldBitmap = { 0 };

	//�����ڴ�DC 
	hdc_mem = CreateCompatibleDC(hdc);
	//����һ��bmp�ڴ�ռ� 
	//hBitmap = CreateCompatibleBitmap(hdc, WINDOW_WIDTH, WINDOW_HIGH);
	hBitmap = CreateCompatibleBitmap(hdc, GetSystemMetrics(SM_CXSCREEN), GetSystemMetrics(SM_CYSCREEN));
	//��bmp�ڴ�ռ������ڴ�DC 
	hOldBitmap = (HBITMAP)SelectObject(hdc_mem, hBitmap);
	//���ñ���ɫ��λͼ����ɾ��������õ��ǰ�ɫ��Ϊ���� 
	//PatBlt(hdc_mem, 0, 0, WINDOW_WIDTH, WINDOW_HIGH, WHITENESS);
	PatBlt(hdc_mem, 0, 0, GetSystemMetrics(SM_CXSCREEN), GetSystemMetrics(SM_CYSCREEN), WHITENESS);
	
	geme_backgound_bmp(hdc_mem);//������Ϸ����ͼƬ
	OnDrawFrame(hdc_mem);

	if (3 == tetris.GameState)
	{
		/* �ٶ�ѡ�� */
		OnDrawSpeedSelect(hdc_mem);
	}
	else
	{
		/* ������Ϸ */
		UpdateCurrBlockDestinationPosition();
		OnDrawBlock(hdc_mem);
	}

	OnDrawResult(hdc_mem);

	/* ���ڴ�DC�����ݸ��Ƶ���Ļ��ʾDC��,�����ʾ */
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
	//��ʾ��������
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




