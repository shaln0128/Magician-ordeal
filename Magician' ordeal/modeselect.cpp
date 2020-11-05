//=============================================================================
//
// ���[�h�Z���N�g����[modeselect.cpp]
// Author : GP11B132 36 ���c�x��
//
//=============================================================================
#include "title.h"
#include "modeselect.h"
#include "sprite.h"
#include "input.h"
#include "bg.h"
#include "fade.h"
#include "sound.h"
//*****************************************************************************
// �}�N����`
//*****************************************************************************
#define MODE_TEXTURE_WIDTH				(600)	// �L�����T�C�Y
#define MODE_TEXTURE_HEIGHT				(80)
#define TEXTURE_MAX						(2)		// �e�N�X�`���̐�


//*****************************************************************************
// �v���g�^�C�v�錾
//*****************************************************************************

typedef enum	//���Ɉڍs���郂�[�h�̃X�e�[�g
{
	SELECTMODE_TUTORIAL,
	SELECTMODE_PLAYGAME,
	SELECTMODE_NONE,
}SELECTMODE;

//*****************************************************************************
// �O���[�o���ϐ�
//*****************************************************************************
static ID3D11Buffer				*g_VertexBuffer = NULL;		// ���_���
static ID3D11ShaderResourceView	*g_Texture[TEXTURE_MAX] = { NULL };	// �e�N�X�`�����

static char *g_TexturName[] = {
	"data/TEXTURE/tutorial.png",
	"data/TEXTURE/playgame.png",
};

static SELECTMODE g_flag;		//���݂̏��
CURSORPOP mode[TEXTURE_MAX];	//���[�h�摜�̍\����
static int g_time;	//�o�ߎ���
//=============================================================================
// ����������
//=============================================================================
HRESULT InitModeSelect(void)
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


	g_flag = SELECTMODE_NONE;
	g_time = 0;

	//���[�h�摜�̍\���̂̏�����
	for (int i = 0; i < TEXTURE_MAX; i++)
	{
		mode[i].use = true;
		mode[i].w = MODE_TEXTURE_WIDTH;
		mode[i].h = MODE_TEXTURE_HEIGHT;
		mode[i].pos = D3DXVECTOR3(480.0f, 280.0f + 120.0f * i, 0.0f);
		mode[i].texNo = i;
	}

	return S_OK;
}

//=============================================================================
// �I������
//=============================================================================
void UninitModeSelect(void)
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
void UpdateModeSelect(void)
{
	if (g_flag == SELECTMODE_NONE)	//�����I������Ă��Ȃ����
	{
		//���͂��ꂽ�L�[�ɉ����ď�Ԃ�ω�
		if (GetKeyboardTrigger(DIK_T))
		{
			g_flag = SELECTMODE_TUTORIAL;
			PlaySound(SOUND_LABEL_SE_decision);
		}
		else if (GetKeyboardTrigger(DIK_G))
		{
			g_flag = SELECTMODE_PLAYGAME;
			PlaySound(SOUND_LABEL_SE_decision);
		}
	}
	else
	{
		g_time++;
		if (g_time >= 100)	//100�t���[���ォ��FADE�����Ɉڍs
		{
			if (g_flag == SELECTMODE_TUTORIAL)
			{
				SetFade(FADE_OUT, MODE_TUTORIAL);
			}
			else if (g_flag == SELECTMODE_PLAYGAME)
			{
				SetFade(FADE_OUT, MODE_GAME);
			}
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
void DrawModeSelect(void)
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

	
	for (int i = 0; i < TEXTURE_MAX; i++)
	{
		GetDeviceContext()->PSSetShaderResources(0, 1, &g_Texture[mode[i].texNo]);

		// �X�R�A�̈ʒu��e�N�X�`���[���W�𔽉f
		float px = mode[i].pos.x;		// ���[�h�摜�̕\���ʒuX
		float py = mode[i].pos.y;		// ���[�h�摜�̕\���ʒuY
		float pw = mode[i].w;			// ���[�h�摜�̕\����
		float ph = mode[i].h;			// ���[�h�摜�̕\������

		float tw = 1.0f;		// �e�N�X�`���̕�
		float th = 1.0f;		// �e�N�X�`���̍���
		float tx = 0.0f;		// �e�N�X�`���̍���X���W
		float ty = 0.0f;		// �e�N�X�`���̍���Y���W

		float alpha = g_flag == mode[i].texNo && g_time / 8 % 2 == 1 ? 0.2f : 1.0f;	//�ڍs���͓_��
		// �P���̃|���S���̒��_�ƃe�N�X�`�����W��ݒ�
		SetSpriteColor(g_VertexBuffer, px, py, pw, ph, tx, ty, tw, th,D3DXCOLOR(1.0f,1.0f,1.0f,alpha),true);

		// �|���S���`��
		GetDeviceContext()->Draw(4, 0);

	}

}
