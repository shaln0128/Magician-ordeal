//=============================================================================
//
// Main���� [main.cpp]
// Author :  GP11B132 36 ���c�x��
//
//=============================================================================
#include "main.h"
#include "renderer.h"
#include "camera.h"
#include "input.h"
#include "bg.h"
#include "sound.h"
#include "title.h"
#include "modeselect.h"
#include "ready.h"
#include "game.h"
#include "fade.h"
#include "result.h"
#include "tutorial.h"
//*****************************************************************************
// �}�N����`
//*****************************************************************************
#define CLASS_NAME			"AppClass"				// �E�C���h�E�̃N���X��
#define WINDOW_NAME			"Magician' ordeal"		// �E�C���h�E�̃L���v�V������

//*****************************************************************************
// �\���̒�`
//*****************************************************************************

//*****************************************************************************
// �v���g�^�C�v�錾
//*****************************************************************************
LRESULT CALLBACK WndProc(HWND, UINT, WPARAM, LPARAM);
HRESULT Init(HINSTANCE hInstance, HWND hWnd, BOOL bWindow);
void Uninit(void);
void Update(void);
void Draw(void);

//*****************************************************************************
// �O���[�o���ϐ�:
//*****************************************************************************
long g_MouseX = 0;
long g_MouseY = 0;

MODE g_Mode = MODE_TITLE;

#ifdef _DEBUG
int		g_CountFPS;							// FPS�J�E���^
char	g_DebugStr[2048] = WINDOW_NAME;		// �f�o�b�O�����\���p

#endif

											//=============================================================================
// ���C���֐�
//=============================================================================
int APIENTRY WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPSTR lpCmdLine, int nCmdShow)
{
	//UNREFERENCED_PARAMETER(hPrevInstance);	// �����Ă��ǂ����ǁA�x�����o��i���g�p�錾�j
	//UNREFERENCED_PARAMETER(lpCmdLine);		// �����Ă��ǂ����ǁA�x�����o��i���g�p�錾�j

	// ���Ԍv���p
	DWORD dwExecLastTime;
	DWORD dwFPSLastTime;
	DWORD dwCurrentTime;
	DWORD dwFrameCount;
	
	WNDCLASSEX	wcex = {
		sizeof(WNDCLASSEX),
		CS_CLASSDC,
		WndProc,
		0,
		0,
		hInstance,
		NULL,
		LoadCursor(NULL, IDC_ARROW),
		(HBRUSH)(COLOR_WINDOW+1),
		NULL,
		CLASS_NAME,
		NULL
	};
	HWND		hWnd;
	MSG			msg;
	
	// �E�B���h�E�N���X�̓o�^
	RegisterClassEx(&wcex);

	// �E�B���h�E�̍쐬
	hWnd = CreateWindow(CLASS_NAME,
						WINDOW_NAME,
						WS_OVERLAPPEDWINDOW,
						CW_USEDEFAULT,																		// �E�B���h�E�̍����W
						CW_USEDEFAULT,																		// �E�B���h�E�̏���W
						SCREEN_WIDTH + GetSystemMetrics(SM_CXDLGFRAME)*2,									// �E�B���h�E����
						SCREEN_HEIGHT + GetSystemMetrics(SM_CXDLGFRAME)*2+GetSystemMetrics(SM_CYCAPTION),	// �E�B���h�E�c��
						NULL,
						NULL,
						hInstance,
						NULL);

	bool mode = true;

	int id = MessageBox(NULL, "Window���[�h�Ńv���C���܂����H", "�N�����[�h", MB_YESNOCANCEL | MB_ICONQUESTION);
	switch (id)
	{
	case IDYES:		// Yes�Ȃ�Window���[�h�ŋN��
		mode = true;
		break;
	case IDNO:		// No�Ȃ�t���X�N���[�����[�h�ŋN��
		mode = false;
		break;
	case IDCANCEL:	// CANCEL�Ȃ�I��
	default:
		return -1;
		break;
	}

	// DirectX�̏�����(�E�B���h�E���쐬���Ă���s��)
	if(FAILED(Init(hInstance, hWnd, true)))
	{
		return -1;
	}

	// �t���[���J�E���g������
	timeBeginPeriod(1);	// ����\��ݒ�
	dwExecLastTime = dwFPSLastTime = timeGetTime();	// �V�X�e���������~���b�P�ʂŎ擾
	dwCurrentTime = dwFrameCount = 0;
	
	// �E�C���h�E�̕\��(Init()�̌�ɌĂ΂Ȃ��Ƒʖ�)
	ShowWindow(hWnd, nCmdShow);
	UpdateWindow(hWnd);
	
	// ���b�Z�[�W���[�v
	while(1)
	{
		if(PeekMessage(&msg, NULL, 0, 0, PM_REMOVE))
		{
			if(msg.message == WM_QUIT)
			{// PostQuitMessage()���Ă΂ꂽ�烋�[�v�I��
				break;
			}
			else
			{
				// ���b�Z�[�W�̖|��ƃf�B�X�p�b�`
				TranslateMessage(&msg);
				DispatchMessage(&msg);
			}
		}
		else
		{
			dwCurrentTime = timeGetTime();					// �V�X�e���������擾

			if ((dwCurrentTime - dwFPSLastTime) >= 1000)	// 1�b���ƂɎ��s
			{
#ifdef _DEBUG
				g_CountFPS = dwFrameCount;
#endif
				dwFPSLastTime = dwCurrentTime;				// FPS�𑪒肵��������ۑ�
				dwFrameCount = 0;							// �J�E���g���N���A
			}

			if ((dwCurrentTime - dwExecLastTime) >= (1000 / 60))	// 1/60�b���ƂɎ��s
			{
				dwExecLastTime = dwCurrentTime;	// ��������������ۑ�

#ifdef _DEBUG	// �f�o�b�O�ł̎�����FPS��\������
				wsprintf(g_DebugStr, WINDOW_NAME);
				wsprintf(&g_DebugStr[strlen(g_DebugStr)], " FPS:%d", g_CountFPS);
#endif

				Update();			// �X�V����
				Draw();				// �`�揈��

#ifdef _DEBUG	// �f�o�b�O�ł̎������\������
				wsprintf(&g_DebugStr[strlen(g_DebugStr)], " MX:%d MY:%d", GetMousePosX(), GetMousePosY());
				SetWindowText(hWnd, g_DebugStr);
#endif

				dwFrameCount++;		// �����񐔂̃J�E���g�����Z
			}
		}
	}
	
	timeEndPeriod(1);				// ����\��߂�

	// �E�B���h�E�N���X�̓o�^������
	UnregisterClass(CLASS_NAME, wcex.hInstance);

	// �I������
	Uninit();

	return (int)msg.wParam;
}

//=============================================================================
// �v���V�[�W��
//=============================================================================
LRESULT CALLBACK WndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
{
	switch( message )
	{
	case WM_DESTROY:
		PostQuitMessage(0);
		break;

	case WM_KEYDOWN:
		switch(wParam)
		{
		case VK_ESCAPE:					// [ESC]�L�[�������ꂽ
			DestroyWindow(hWnd);		// �E�B���h�E��j������悤�w������
			break;
		}
		break;

	case WM_MOUSEMOVE:
		g_MouseX = LOWORD(lParam);
		g_MouseY = HIWORD(lParam);
		break;

	default:
		return DefWindowProc(hWnd, message, wParam, lParam);
	}

	return 0;
}

//=============================================================================
// ����������
//=============================================================================
HRESULT Init(HINSTANCE hInstance, HWND hWnd, BOOL bWindow)
{
	// �����_�����O�����̏�����
	InitRenderer(hInstance, hWnd, bWindow);

	InitFade();
	// �J���������̏�����
	InitCamera();

	InitBg();

	//���͂̏�����
	InitInput(hInstance, hWnd);

	InitSound(hWnd);
	
	SetMode(g_Mode);

	return S_OK;
}


//=============================================================================
// �I������
//=============================================================================
void Uninit(void)
{
	SetMode(MODE_MAX);
	//���͂̏I������
	UninitInput();

	UninitFade();

	UninitReady();

	// �J�����̏I������
	UninitCamera();

	UninitBg();

	// �����_�����O�̏I������
	UninitRenderer();

	UninitSound();
}

//=============================================================================
// �X�V����
//=============================================================================
void Update(void)
{
	//���͂̍X�V����
	UpdateInput();
	// �J�����̍X�V����
	UpdateCamera();

	UpdateBg();

	if (GetFade() != FADE_OUT)
	{
		switch (g_Mode)
		{
		case MODE_TITLE:
			UpdateTitle();
			break;

		case MODE_SELECT:
			UpdateModeSelect();
			break;

		case MODE_TUTORIAL:
			UpdateTutorial();
			break;

		case MODE_GAME:
			if (AreYouReady())
			{
				UpdateReady();
			}
			else
			{
				UpdateReady();
				UpdateGame();
			}

			break;

		case MODE_RESULT:
			UpdateResult();
			break;

		}

	}
	
	UpdateFade();

}

//=============================================================================
// �`�揈��
//=============================================================================
void Draw(void)
{
	// �o�b�N�o�b�t�@�N���A
	Clear();

	// �J�������Z�b�g
	SetCamera();

	// 2D�`��Ȃ̂Ő[�x����
	SetDepthEnable(false);

	DrawBg();

	switch (g_Mode)
	{
	case MODE_TITLE:
		DrawTitle();
		break;

	case MODE_SELECT:
		DrawModeSelect();
		break;

	case MODE_TUTORIAL:
		DrawTutorial();
		break;

	case MODE_GAME:
		DrawGame();
		DrawReady();
		break;

	case MODE_RESULT:
		DrawResult();
		break;
	}

	

	DrawFade();
	// �o�b�N�o�b�t�@�A�t�����g�o�b�t�@����ւ�
	Present();

}


long GetMousePosX(void)
{
	return g_MouseX;
}


long GetMousePosY(void)
{
	return g_MouseY;
}


#ifdef _DEBUG
char *GetDebugStr(void)
{
	return g_DebugStr;
}
#endif


void SetMode(MODE mode)
{
	// ���[�h��ς���O�ɑS������������������Ⴄ

	// �^�C�g����ʂ̏I������
	UninitTitle();

	UninitModeSelect();

	// �Q�[����ʂ̏I������
	UninitGame();

	// ���U���g��ʂ̏I������
	UninitResult();

	UninitTutorial();

	ResetViewports();

	g_Mode = mode;	// ���̃��[�h���Z�b�g���Ă���

	switch (g_Mode)
	{
	case MODE_TITLE:
		// �^�C�g����ʂ̏�����
		InitTitle();
		PlaySound(SOUND_LABEL_BGM_title);
		break;

	case MODE_SELECT:
		InitModeSelect();
		PlaySound(SOUND_LABEL_BGM_title);
		break;

	case MODE_TUTORIAL:
		InitTutorial();
		PlaySound(SOUND_LABEL_BGM_tutorial);
		break;

	case MODE_GAME:
		// �Q�[����ʂ̏�����
		InitGame();
		InitReady();
		PlaySound(SOUND_LABEL_BGM_game);
		break;

	case MODE_RESULT:
		// ���U���g��ʂ̏�����
		InitResult();
		PlaySound(SOUND_LABEL_BGM_title);
		break;

		// �Q�[���I�����̏���
	case MODE_MAX:
		break;
	}

	SetBg(g_Mode);
}

//=============================================================================
// ���[�h�̎擾
//=============================================================================
MODE GetMode(void)
{
	return g_Mode;
}


