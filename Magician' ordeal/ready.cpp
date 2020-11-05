//=============================================================================
//
// ���f�B�[�E�|�[�Y�̏���[ready.cpp]
// Author : GP11B132 36 ���c�x��
//
//=============================================================================
#include "sprite.h"
#include "input.h"
#include "ready.h"
#include "fade.h"
#include "sound.h"
//*****************************************************************************
// �}�N����`
//*****************************************************************************
#define TEXTURE_WIDTH				(420)	// �L�����T�C�Y
#define TEXTURE_HEIGHT				(80) 
#define TEXTURE_MAX					(3)		// �e�N�X�`���̐�

#define PRESSENTER_WIDTH			(360)
#define PRESSENTER_HEIGHT			(40)

//*****************************************************************************
// �v���g�^�C�v�錾
//*****************************************************************************

//*****************************************************************************
// �O���[�o���ϐ�
//*****************************************************************************
static ID3D11Buffer				*g_VertexBuffer = NULL;		// ���_���
static ID3D11ShaderResourceView	*g_Texture[TEXTURE_MAX] = { NULL };	// �e�N�X�`�����

static char *g_TexturName[] = {
	"data/TEXTURE/ready.png",
	"data/TEXTURE/pause.png",
	"data/TEXTURE/press_enter.png",
};


static bool	ready;						// true:�g���Ă���  false:���g�p
static float g_ready_w;
static float g_ready_h;
static float g_x;
static float g_y;

static bool	pause;						// true:�g���Ă���  false:���g�p
static float g_pause_w;
static float g_pause_h;

static float g_pressenter_w;
static float g_pressenter_h;

static float s_y;
//=============================================================================
// ����������
//=============================================================================
HRESULT InitReady(void)
{
	ID3D11Device *pDevice = GetDevice();

	//�e�N�X�`������
	for (int i = 0; i < TEXTURE_MAX; i++)
	{
		g_Texture[i] = NULL;
		D3DX11CreateShaderResourceViewFromFile(
			GetDevice(),
			g_TexturName[i],
			NULL,
			NULL,
			&g_Texture[i],
			NULL);
	}


	// ���_�o�b�t�@����
	D3D11_BUFFER_DESC bd;
	ZeroMemory(&bd, sizeof(bd));
	bd.Usage = D3D11_USAGE_DYNAMIC;
	bd.ByteWidth = sizeof(VERTEX_3D) * 4;
	bd.BindFlags = D3D11_BIND_VERTEX_BUFFER;
	bd.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
	GetDevice()->CreateBuffer(&bd, NULL, &g_VertexBuffer);

	// �r���[�|�[�g�̐��l�ɉ����ĕ\���ʒu���X�V
	SetReadyPos();

	ready = true;
	PlaySound(SOUND_LABEL_SE_pause);
	pause = false;

	return S_OK;
}

//=============================================================================
// �I������
//=============================================================================
void UninitReady(void)
{
	if (g_VertexBuffer)
	{
		g_VertexBuffer->Release();
		g_VertexBuffer = NULL;
	}

	for (int i = 0; i < TEXTURE_MAX; i++)
	{
		if (g_Texture[i])
		{
			g_Texture[i]->Release();
			g_Texture[i] = NULL;
		}
	}

}

//=============================================================================
// �X�V����
//=============================================================================
void UpdateReady(void)
{
	SetReadyPos();	//�r���[�|�[�g�̐��l�ɉ����ĕ\���ʒu���X�V

	if (GetKeyboardTrigger(DIK_RETURN))
	{
		ready = false;
	}

	if (ready == false)
	{
		if (GetKeyboardTrigger(DIK_P))	//P�Ń|�[�Y��ʂ�ON/OFF
		{
			pause = pause == true ? false : true;
			PlaySound(SOUND_LABEL_SE_pause);
		}
	}

	if (pause == true)
	{
		//�|�[�Y���̓L�[���͂ɉ����ĉ�ʑJ��
		if (GetKeyboardTrigger(DIK_R))
		{
			SetFade(FADE_OUT, MODE_GAME);
			PlaySound(SOUND_LABEL_SE_decision);
		}
		else if (GetKeyboardTrigger(DIK_T))
		{
			SetFade(FADE_OUT, MODE_TITLE);
			PlaySound(SOUND_LABEL_SE_decision);
		}
	}


#ifdef _DEBUG	// �f�o�b�O����\������
	//char *str = GetDebugStr();
	//sprintf(&str[strlen(str)], " PX:%.2f PY:%.2f", g_Pos.x, g_Pos.y);

#endif

}

//=============================================================================
// �`�揈��
//=============================================================================
void DrawReady(void)
{
	// ���_�o�b�t�@�ݒ�
	UINT stride = sizeof(VERTEX_3D);
	UINT offset = 0;
	GetDeviceContext()->IASetVertexBuffers(0, 1, &g_VertexBuffer, &stride, &offset);

	// �}�g���N�X�ݒ�
	SetWorldViewProjection2D();

	// �v���~�e�B�u�g�|���W�ݒ�
	GetDeviceContext()->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLESTRIP);

	// �}�e���A���ݒ�
	MATERIAL material;
	ZeroMemory(&material, sizeof(material));
	material.Diffuse = D3DXCOLOR(1.0f, 1.0f, 1.0f, 1.0f);
	SetMaterial(material);

	if (ready == true)
	{
		// �e�N�X�`���ݒ�
		GetDeviceContext()->PSSetShaderResources(0, 1, &g_Texture[0]);

		// �X�R�A�̈ʒu��e�N�X�`���[���W�𔽉f
		float px = g_x;			// ���f�B�[�̕\���ʒuX
		float py = g_y;			// ���f�B�[�̕\���ʒuY
		float pw = g_ready_w;	// ���f�B�[�̕\����
		float ph = g_ready_h;	// ���f�B�[�̕\������

		float tw = 1.0f;		// �e�N�X�`���̕�
		float th = 1.0f;		// �e�N�X�`���̍���
		float tx = 0.0f;		// �e�N�X�`���̍���X���W
		float ty = 0.0f;		// �e�N�X�`���̍���Y���W

		// �P���̃|���S���̒��_�ƃe�N�X�`�����W��ݒ�
		SetSprite(g_VertexBuffer, px, py, pw, ph, tx, ty, tw, th);

		// �|���S���`��
		GetDeviceContext()->Draw(4, 0);

		// �e�N�X�`���ݒ�
		GetDeviceContext()->PSSetShaderResources(0, 1, &g_Texture[2]);

		pw = g_pressenter_w;	// �v���X�G���^�[�̕\����
		ph = g_pressenter_h;	// �v���X�G���^�[�̕\������

		// �P���̃|���S���̒��_�ƃe�N�X�`�����W��ݒ�
		SetSprite(g_VertexBuffer, px, py + s_y, pw, ph, tx, ty, tw, th);

		// �|���S���`��
		GetDeviceContext()->Draw(4, 0);
	}
	else if (pause == true)	//�|�[�Y��ʂ̕`��
	{
		// �e�N�X�`���ݒ�
		GetDeviceContext()->PSSetShaderResources(0, 1, &g_Texture[1]);

		// �X�R�A�̈ʒu��e�N�X�`���[���W�𔽉f
		float px = g_x;			// �|�[�Y��ʂ̕\���ʒuX
		float py = g_y;			// �|�[�Y��ʂ̕\���ʒuY
		float pw = g_pause_w;	// �|�[�Y��ʂ̕\����
		float ph = g_pause_h;	// �|�[�Y��ʂ̕\������

		float tw = 1.0f;		// �e�N�X�`���̕�
		float th = 1.0f;		// �e�N�X�`���̍���
		float tx = 0.0f;		// �e�N�X�`���̍���X���W
		float ty = 0.0f;		// �e�N�X�`���̍���Y���W

		// �P���̃|���S���̒��_�ƃe�N�X�`�����W��ݒ�
		SetSprite(g_VertexBuffer, px, py, pw, ph, tx, ty, tw, th);

		// �|���S���`��
		GetDeviceContext()->Draw(4, 0);
	}
}

bool AreYouReady(void)	//���f�B�[��ʂ��|�[�Y��ʂȂ�true��Ԃ�
{
	bool ans = ready == true || pause == true ? true : false;
	return ans;
}

void SetReadyPos(void)	//�r���[�|�[�g�̐��l�ɉ����ĕ\���ʒu���X�V
{
	VIEWPORT *viewport = GetViewports();
	float mag = viewport->mag;	//�{��
	float tl_x = viewport->topleft.x;	//������W
	float tl_y = viewport->topleft.y;

	g_ready_w = TEXTURE_WIDTH / mag;
	g_ready_h = TEXTURE_HEIGHT / mag;
	g_x = (-tl_x + SCREEN_CENTER_X) / mag;
	g_y = (-tl_y + SCREEN_CENTER_Y) / mag;

	g_pause_w = SCREEN_WIDTH / mag;
	g_pause_h = SCREEN_HEIGHT / mag;

	g_pressenter_w = PRESSENTER_WIDTH / mag;
	g_pressenter_h = PRESSENTER_HEIGHT / mag;
	s_y = 50.0f / mag;
}
