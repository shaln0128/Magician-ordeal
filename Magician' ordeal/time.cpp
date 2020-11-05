//////=============================================================================
//
// �^�C���Ȃǂ�UI���� [time.cpp]
// Author : GP11B132 36 ���c�x��
//
//=============================================================================
#include "sprite.h"
#include "score.h"
#include "time.h"
#include "fade.h"
#include "ready.h"
#include "fire.h"
//*****************************************************************************
// �}�N����`
//***************************************************************************** 
#define TEXTURE_MAX					(10)		// �e�N�X�`���̐�

#define TEXTURE_PATTERN_DIVIDE_X	(5)			// �A�j���p�^�[���̃e�N�X�`�����������iX)
#define TEXTURE_PATTERN_DIVIDE_Y	(2)			// �A�j���p�^�[���̃e�N�X�`�����������iY)

#define TEXTURE_WIDTH				(25)		// �L�����T�C�Y
#define TEXTURE_HEIGHT				(50)

#define SECOND_WIDTH				(8)			// �L�����T�C�Y
#define SECOND_HEIGHT				(2)
#define SECOND_REACH				(45)
	
#define FIRE_WIDTH					(40)			// �L�����T�C�Y
#define FIRE_HEIGHT					(40)			// �L�����T�C�Y

#define FIREBAR_WIDTH				(200)			// �L�����T�C�Y
#define FIREBAR_HEIGHT				(FIREBAR_WIDTH / 10)

#define LASERICON_WIDTH				(40)			// �L�����T�C�Y
#define LASERICON_HEIGHT			(20)			// �L�����T�C�Y

#define COVER_WIDTH					(250)			// �L�����T�C�Y
#define COVER_HEIGHT				(70)			// �L�����T�C�Y


#define NAVI_WIDTH					(540)			// �L�����T�C�Y
#define NAVI_HEIGHT					(40)			// �L�����T�C�Y

#define TEXTURE_POS_X				(TEXTURE_WIDTH + TEXTURE_WIDTH / 2)
#define TEXTURE_POS_Y				(TEXTURE_HEIGHT / 2)

#define pi							float(3.14159265)	//�ς�
//*****************************************************************************
// �v���g�^�C�v�錾
//*****************************************************************************


//*****************************************************************************
// �O���[�o���ϐ�
//*****************************************************************************
static ID3D11Buffer				*g_VertexBuffer = NULL;		// ���_���
static ID3D11ShaderResourceView	*g_Texture[TEXTURE_MAX] = { NULL };	// �e�N�X�`�����

static char *g_TexturName[] = {
	"data/TEXTURE/number_black.png",
	"data/TEXTURE/black.png",
	"data/TEXTURE/white.png",
	"data/TEXTURE/red.png",
	"data/TEXTURE/chip.png",
	"data/TEXTURE/fire_icon.png",
	"data/TEXTURE/fire_bar.png",
	"data/TEXTURE/laser_icon.png",
	"data/TEXTURE/laser_stock.png",
	"data/TEXTURE/navi.png",
};

static int g_time;	//�o�ߎ���

//���ꂼ��̃e�N�X�`���̕��ƈʒu
static float g_texture_w;
static float g_texture_h;
static float g_texture_x;
static float g_texture_y;

static float g_second_w;
static float g_second_h;
static float g_second_x;
static float g_second_y;
static float g_second_reach;

static float g_fire_w;
static float g_fire_h;
static float g_fire_x;
static float g_fire_y;

static float g_firebar_w;
static float g_firebar_h;
static float g_firebar_x;
static float g_firebar_y;

static float g_lasericon_w;
static float g_lasericon_h;
static float g_lasericon_x;
static float g_lasericon_y;

static float g_cover_w;
static float g_cover_h;
static float g_cover_x;
static float g_cover_y;

static float g_navi_w;
static float g_navi_h;
static float g_navi_x;
static float g_navi_y;

static float rad = pi / 180;	//���W�A���ϊ��p
//=============================================================================
// ����������
//=============================================================================
HRESULT InitTime(void)
{
	ID3D11Device *pDevice = GetDevice();

	//�e�N�X�`������
	for (int i = 0; i < TEXTURE_MAX; i++)
	{
		g_Texture[i] = NULL;
		D3DX11CreateShaderResourceViewFromFile(GetDevice(),
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

	g_time = 3600; 	//�������Ԃ�60�b

	// �r���[�|�[�g�̐��l�ɉ����ĕ\���ʒu���X�V
	SetTimePos();

	return S_OK;
}

//=============================================================================
// �I������
//=============================================================================
void UninitTime(void)
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
void UpdateTime(void)
{
	if (GetMode() == MODE_GAME)	//�Q�[�����[�h�̏ꍇ
	{
		g_time -= 1;

		if (g_time < 0)
		{
			g_time = 0;
		}
	}
	
	// �r���[�|�[�g�̐��l�ɉ����ĕ\���ʒu���X�V
	SetTimePos();
#ifdef _DEBUG	// �f�o�b�O����\������
	char *str = GetDebugStr();
	sprintf(&str[strlen(str)], " TIME:%d", g_time);

#endif
}

//=============================================================================
// �`�揈��
//=============================================================================
void DrawTime(void)
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


	{
		GetDeviceContext()->PSSetShaderResources(0, 1, &g_Texture[4]);

		// ���v�Ղ̈ʒu��e�N�X�`���[���W�𔽉f
		float px = g_second_x;	// ���v�Ղ̕\���ʒuX
		float py = g_second_y;	// ���v�Ղ̕\���ʒuY
		float size = g_second_reach * 2.5f;

		// �P���̃|���S���̒��_�ƃe�N�X�`�����W��ݒ�
		SetSpriteColor(g_VertexBuffer, px, py, size, size, 0.0f, 0.0f, 1.0f, 1.0f, D3DXCOLOR(0.8f, 0.8f, 0.8f, 0.3f), true);

		// �|���S���`��
		GetDeviceContext()->Draw(4, 0);
	}

	int number = g_time;

	int x, y;

	number /= 60;
	x = number / 10;
	y = number % 10;

	{
		// �e�N�X�`���ݒ�
		GetDeviceContext()->PSSetShaderResources(0, 1, &g_Texture[0]);

		// ���v�̐����̈ʒu��e�N�X�`���[���W�𔽉f
		float px = g_texture_x - g_texture_w;	// ���v�̐����̕\���ʒuX
		float py = g_texture_y;	// ���v�̐����̕\���ʒuY
		float pw = g_texture_w;		// ���v�̐����̕\����
		float ph = g_texture_h;		// ���v�̐����̕\������

		float tw = 1.0f / TEXTURE_PATTERN_DIVIDE_X;	// �e�N�X�`���̕�
		float th = 1.0f / TEXTURE_PATTERN_DIVIDE_Y;	// �e�N�X�`���̍���
		float tx = tw * float(x % 5);	// �e�N�X�`���̍���X���W
		float ty = th * float(x / 5);	// �e�N�X�`���̍���Y���W

		// �P���̃|���S���̒��_�ƃe�N�X�`�����W��ݒ�
		SetSprite(g_VertexBuffer, px, py, pw, ph, tx, ty, tw, th);

		GetDeviceContext()->Draw(4, 0);
	}

	{
		GetDeviceContext()->PSSetShaderResources(0, 1, &g_Texture[0]);

		// ���v�̐����̈ʒu��e�N�X�`���[���W�𔽉f
		float px = g_texture_x;	// ���v�̐����̕\���ʒuX
		float py = g_texture_y;	// ���v�̐����̕\���ʒuY
		float pw = g_texture_w;		// ���v�̐����̕\����
		float ph = g_texture_h;		// ���v�̐����̕\������

		float tw = 1.0f / TEXTURE_PATTERN_DIVIDE_X;	// �e�N�X�`���̕�
		float th = 1.0f / TEXTURE_PATTERN_DIVIDE_Y;	// �e�N�X�`���̍���
		float tx = tw * float(y % 5);	// �e�N�X�`���̍���X���W
		float ty = th * float(y / 5);	// �e�N�X�`���̍���Y���W

		// �P���̃|���S���̒��_�ƃe�N�X�`�����W��ݒ�
		SetSprite(g_VertexBuffer, px, py, pw, ph, tx, ty, tw, th);

		// �|���S���`��
		GetDeviceContext()->Draw(4, 0);
	}
	
	for (int i = 0; i < 60; i++)
	{
		int texNo = i < g_time / 60 ? 1 : 2;

		texNo = ((texNo == 1) && (i % 5 == 0)) ? 3 : texNo;

		GetDeviceContext()->PSSetShaderResources(0, 1, &g_Texture[texNo]);

		// ���v�̐j�̈ʒu��e�N�X�`���[���W�𔽉f
		float px = g_second_x;	// ���v�̐j�̕\���ʒuX
		float py = g_second_y;	// ���v�̐j�̕\���ʒuY
		float pw = i % 5 == 0 ? g_second_w * 1.8f : g_second_w;		// ���v�̐j�̕\����
		float ph = g_second_h;		// ���v�̐j�̕\������
		float angle = (-6 * i - 90) * rad;
		float sx = cosf(angle) * g_second_reach;	// ���v�̐j�̕\���ʒuX
		float sy = sinf(angle) * g_second_reach;	// ���v�̐j�̕\���ʒuY

		// �P���̃|���S���̒��_�ƃe�N�X�`�����W��ݒ�
		SetSpriteColorRotation(g_VertexBuffer, px + sx, py + sy, pw, ph, 1.0f, 1.0f, 0.0f, 0.0f, D3DXCOLOR(1.0f, 1.0f, 1.0f, 1.0f), angle);

		// �|���S���`��
		GetDeviceContext()->Draw(4, 0);
	}


	{
		GetDeviceContext()->PSSetShaderResources(0, 1, &g_Texture[2]);

		// �J�o�[�̈ʒu��e�N�X�`���[���W�𔽉f
		float px = g_cover_x;	// �J�o�[�̕\���ʒuX
		float py = g_cover_y;	// �J�o�[�̕\���ʒuY
		float pw = g_cover_w;
		float ph = g_cover_h;

		// �P���̃|���S���̒��_�ƃe�N�X�`�����W��ݒ�
		SetSpriteColor(g_VertexBuffer, px, py, pw, ph, 0.0f, 0.0f, 1.0f, 1.0f, D3DXCOLOR(0.8f, 0.8f, 0.8f, 0.3f), true);

		// �|���S���`��
		GetDeviceContext()->Draw(4, 0);
	}

	{
		GetDeviceContext()->PSSetShaderResources(0, 1, &g_Texture[5]);

		// ���A�C�R���̈ʒu��e�N�X�`���[���W�𔽉f
		float px = g_fire_x;	// ���A�C�R���̕\���ʒuX
		float py = g_fire_y;	// ���A�C�R���̕\���ʒuY
		float pw = g_fire_w;
		float ph = g_fire_h;
		// �P���̃|���S���̒��_�ƃe�N�X�`�����W��ݒ�
		SetSpriteColor(g_VertexBuffer, px, py, pw, ph, 0.0f, 0.0f, 1.0f, 1.0f, D3DXCOLOR(1.0f, 1.0f, 1.0f, 1.0f), true);

		// �|���S���`��
		GetDeviceContext()->Draw(4, 0);
	}

	{
		int *breath_time = GetBreathTime();

		GetDeviceContext()->PSSetShaderResources(0, 1, &g_Texture[3]);

		float hp_p = *breath_time / float(BREATH_TIME_MAX);
		float px = g_firebar_x;
		float py = g_firebar_y;
		float dw = -g_firebar_w / 2;
		float dh = -g_firebar_h / 2;
			
		// �P���̃|���S���̒��_�ƃe�N�X�`�����W��ݒ�
		SetSpriteHpbar(g_VertexBuffer, hp_p, px, py, dw, dh);

		GetDeviceContext()->Draw(4, 0);
	}

	{
		GetDeviceContext()->PSSetShaderResources(0, 1, &g_Texture[6]);

		// ���Q�[�W�̈ʒu��e�N�X�`���[���W�𔽉f
		float px = g_firebar_x;	// ���Q�[�W�̕\���ʒuX
		float py = g_firebar_y;	// ���Q�[�W�̕\���ʒuY
		float pw = g_firebar_w;
		float ph = g_firebar_h;
		// �P���̃|���S���̒��_�ƃe�N�X�`�����W��ݒ�
		SetSpriteColor(g_VertexBuffer, px, py, pw, ph, 0.0f, 0.0f, 1.0f, 1.0f, D3DXCOLOR(1.0f, 1.0f, 1.0f, 1.0f), true);

		// �|���S���`��
		GetDeviceContext()->Draw(4, 0);
	}

	{
		GetDeviceContext()->PSSetShaderResources(0, 1, &g_Texture[8]);

		// ���[�U�[�A�C�R���̈ʒu��e�N�X�`���[���W�𔽉f
		float px = g_lasericon_x;	// ���[�U�[�A�C�R���̕\���ʒuX
		float py = g_lasericon_y;	// ���[�U�[�A�C�R���̕\���ʒuY
		float pw = g_lasericon_w;
		float ph = g_lasericon_h;
		// �P���̃|���S���̒��_�ƃe�N�X�`�����W��ݒ�
		SetSpriteColor(g_VertexBuffer, px, py, pw, ph, 0.0f, 0.0f, 1.0f, 1.0f, D3DXCOLOR(1.0f, 1.0f, 1.0f, 1.0f), true);

		// �|���S���`��
		GetDeviceContext()->Draw(4, 0);
	}

	if (GetMode() == MODE_GAME)
	{
		GetDeviceContext()->PSSetShaderResources(0, 1, &g_Texture[9]);

		// �i�r�̈ʒu��e�N�X�`���[���W�𔽉f
		float px = g_navi_x;	// �i�r�̕\���ʒuX
		float py = g_navi_y;	// �i�r�̕\���ʒuY
		float pw = g_navi_w;
		float ph = g_navi_h;
		// �P���̃|���S���̒��_�ƃe�N�X�`�����W��ݒ�
		SetSpriteColor(g_VertexBuffer, px, py, pw, ph, 0.0f, 0.0f, 1.0f, 1.0f, D3DXCOLOR(1.0f, 1.0f, 1.0f, 1.0f), true);

		// �|���S���`��
		GetDeviceContext()->Draw(4, 0);
	}

	PLAYER *player = GetPlayer();

	for (int i = 1; i <= player[0].laser_stock; i++)
	{
		GetDeviceContext()->PSSetShaderResources(0, 1, &g_Texture[7]);
		// ���[�U�[�A�C�R���̈ʒu��e�N�X�`���[���W�𔽉f
		float px = g_lasericon_x + g_lasericon_w * 1.25f * i;	// ���[�U�[�A�C�R���̕\���ʒuX
		float py = g_lasericon_y;	// ���[�U�[�A�C�R���̕\���ʒuY
		float pw = g_lasericon_w; 
		float ph = g_lasericon_h;
		// �P���̃|���S���̒��_�ƃe�N�X�`�����W��ݒ�
		SetSpriteColor(g_VertexBuffer, px, py, pw, ph, 0.0f, 0.0f, 1.0f, 1.0f, D3DXCOLOR(1.0f, 1.0f, 1.0f, 1.0f), true);

		// �|���S���`��
		GetDeviceContext()->Draw(4, 0);
	}
}

int GetTime(void)	//�o�ߎ��Ԃ�Ԃ�
{
	return g_time;
}

// �r���[�|�[�g�̐��l�ɉ����ĕ\���ʒu���X�V
void SetTimePos(void)
{
	VIEWPORT *viewport = GetViewports();
	float mag = viewport->mag;	//�{��
	float tl_x = viewport->topleft.x;	//������W
	float tl_y = viewport->topleft.y;

	g_texture_w = TEXTURE_WIDTH / mag;
	g_texture_h = TEXTURE_HEIGHT / mag;
	g_texture_x = (-tl_x + 60.0f) / mag + g_texture_w * 0.5f;
	g_texture_y = (-tl_y + 60.0f) / mag;
	
	g_second_w = SECOND_WIDTH / mag;
	g_second_h = SECOND_HEIGHT / mag;
	g_second_x = (-tl_x + 60.0f) / mag;
	g_second_y = (-tl_y + 60.0f) / mag;
	g_second_reach = SECOND_REACH / mag;

	g_fire_w = FIRE_WIDTH / mag;
	g_fire_h = FIRE_HEIGHT / mag;
	g_fire_x = (-tl_x + 145.0f) / mag;
	g_fire_y = (-tl_y + 40.0f) / mag;

	g_firebar_w = FIREBAR_WIDTH / mag;
	g_firebar_h = FIREBAR_HEIGHT / mag;
	g_firebar_x = (-tl_x + 268.0f) / mag;
	g_firebar_y = g_fire_y;

	g_lasericon_w = LASERICON_WIDTH / mag;
	g_lasericon_h = LASERICON_HEIGHT / mag;
	g_lasericon_x = (-tl_x + 145.0f) / mag;
	g_lasericon_y = (-tl_y + 80.0f) / mag;

	g_cover_w = COVER_WIDTH / mag;
	g_cover_h = COVER_HEIGHT / mag;
	g_cover_x = (-tl_x + 250.0f) / mag;
	g_cover_y = (-tl_y + 60.0f) / mag;

	g_navi_w = NAVI_WIDTH / mag;
	g_navi_h = NAVI_HEIGHT / mag;
	g_navi_x = (-tl_x + 640.0f) / mag;
	g_navi_y = (-tl_y + 510.0f) / mag;
}