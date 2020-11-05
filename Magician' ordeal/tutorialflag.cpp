//=============================================================================
//
//�`���[�g���A���t���O���� [tutorialflag.cpp]
// Author : GP11B132 36 ���c�x��
//
//=============================================================================
#include "sprite.h"
#include "input.h"
#include "tutorialflag.h"
#include "sound.h"
//*****************************************************************************
// �}�N����`
//*****************************************************************************
#define TEXTURE_WIDTH				(960)	// �L�����T�C�YTutorialFlag
#define TEXTURE_HEIGHT				(540)	
#define TEXTURE_MAX					(7)		// �e�N�X�`���̐�

#define JUDG_WIDTH					(250)
#define JUDG_HEIGHT					(70)

//*****************************************************************************
// �v���g�^�C�v�錾
//*****************************************************************************

//*****************************************************************************
// �O���[�o���ϐ�
//*****************************************************************************
static ID3D11Buffer				*g_VertexBuffer = NULL;		// ���_���
static ID3D11ShaderResourceView	*g_Texture[TEXTURE_MAX] = { NULL };	// �e�N�X�`�����

static char *g_TexturName[] = {
	"data/TEXTURE/success.png",
	"data/TEXTURE/failure.png",
	"data/TEXTURE/tutorial01.png",
	"data/TEXTURE/tutorial02.png",
	"data/TEXTURE/tutorial03.png",
	"data/TEXTURE/tutorial04.png",
	"data/TEXTURE/tutorial05.png",
};

//�o�ߎ���
static int g_time;

//������o���ʒu�ƕ�
static float g_judg_w;
static float g_judg_h;
static float g_tutorialflag_w;
static float g_tutorialflag_h;
static D3DXVECTOR3 g_tutorialflag_pos;
static int g_tutorialflag_texno;

//�e��t���O
static bool g_tutorialflag_success;
static TUTORIALSTATE tutorialstate;
static TUTORIALSTATE next_tutorialstate;
//=============================================================================
// ����������
//=============================================================================
HRESULT InitTutorialFlag(void)
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

	//�r���[�|�[�g�ɉ����Ĉʒu�ƕ����X�V
	SetTutorialFlagPos();
	g_time = 0;
	g_tutorialflag_texno = 0;
	tutorialstate = WASD_CAMERA;
	

	return S_OK;
}

//=============================================================================
// �I������
//=============================================================================
void UninitTutorialFlag(void)
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
void UpdateTutorialFlag(void)
{
	//�r���[�|�[�g�ɉ����Ĉʒu�ƕ����X�V
	SetTutorialFlagPos();

	g_time++;

	if (tutorialstate == CONVERSION)	//�ڍs��
	{
		if (g_time >= 120)	//2�b��Ɏ��̃`���[�g���A����
		{
			tutorialstate = next_tutorialstate;
			g_time = 0;
		}
	}
#ifdef _DEBUG	// �f�o�b�O����\������
	char *str = GetDebugStr();
	sprintf(&str[strlen(str)], " state = %d", tutorialstate);

#endif

}

//=============================================================================
// �`�揈��
//=============================================================================
void DrawTutorialFlag(void)
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

	//����̕`��
	if (tutorialstate == CONVERSION)
	{
		// �e�N�X�`���ݒ�
		GetDeviceContext()->PSSetShaderResources(0, 1, &g_Texture[g_tutorialflag_texno]);

		// �X�R�A�̈ʒu��e�N�X�`���[���W�𔽉f
		float px = g_tutorialflag_pos.x;		// ����̕\���ʒuX
		float py = g_tutorialflag_pos.y;		// ����̕\���ʒuY
		float pw = g_judg_w;			// ����̕\����
		float ph = g_judg_h;			// ����̕\������

		float tw = 1.0f;		// �e�N�X�`���̕�
		float th = 1.0f;		// �e�N�X�`���̍���
		float tx = 0.0f;		// �e�N�X�`���̍���X���W
		float ty = 0.0f;		// �e�N�X�`���̍���Y���W

		float alpha = g_time < 90 ? 1.0f : (120 - g_time) / 30.0f;		// �e�N�X�`���̍���Y���W
		// �P���̃|���S���̒��_�ƃe�N�X�`�����W��ݒ�
		SetSpriteColor(g_VertexBuffer, px, py, pw, ph, tx, ty, tw, th, D3DXCOLOR(1.0f,1.0f,1.0f,alpha),true);

		// �|���S���`��
		GetDeviceContext()->Draw(4, 0);
	}
	else
	{
		// �e�N�X�`���ݒ�
		GetDeviceContext()->PSSetShaderResources(0, 1, &g_Texture[tutorialstate + 2]);

		// �X�R�A�̈ʒu��e�N�X�`���[���W�𔽉f
		float px = g_tutorialflag_pos.x;		// �`���[�g���A���摜�̕\���ʒuX
		float py = g_tutorialflag_pos.y;		// �`���[�g���A���摜�̕\���ʒuY
		float pw = g_tutorialflag_w;			// �`���[�g���A���摜�̕\����
		float ph = g_tutorialflag_h;			// �`���[�g���A���摜�̕\������

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

void SetTutorialFlagPos(void)	//�r���[�|�[�g�ɉ����Ĉʒu�ƕ����X�V
{
	VIEWPORT *viewport = GetViewports();
	float mag = viewport->mag;
	float tl_x = viewport->topleft.x;
	float tl_y = viewport->topleft.y;

	g_judg_w = JUDG_WIDTH / mag;
	g_judg_h = JUDG_HEIGHT / mag;
	g_tutorialflag_w = TEXTURE_WIDTH / mag;
	g_tutorialflag_h = TEXTURE_HEIGHT / mag;
	g_tutorialflag_pos.x = (-tl_x + SCREEN_CENTER_X) / mag;
	g_tutorialflag_pos.y = (-tl_y + SCREEN_CENTER_Y) / mag;
}

TUTORIALSTATE GetTutorialState(void)	//���݂̃`���[�g���A���̏�Ԃ�Ԃ�
{
	return tutorialstate;
}

void ConversionTutorial(TUTORIALSTATE state, bool flag)	//��Ԃ̈ڍs
{
	g_time = 0;
	tutorialstate = CONVERSION;
	next_tutorialstate = state;
	g_tutorialflag_success = flag;
	g_tutorialflag_texno = flag ? 0 : 1;
	if (flag == true)	//�t���O�ɉ����Č��ʉ���炷
	{
		PlaySound(SOUND_LABEL_SE_success);
	}
	else
	{
		PlaySound(SOUND_LABEL_SE_failure);
	}
}

int GetTutorialTime(void)	//�o�ߎ��Ԃ�Ԃ�
{
	return g_time;
}

void SkipTime(void)	//�ڍs���X�L�b�v
{
	g_time = 120;
}