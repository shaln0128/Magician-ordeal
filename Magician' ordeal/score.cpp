//=============================================================================
//
// �X�R�A���� [score.cpp]
// Author : GP11B132 36 ���c�x��
//
//=============================================================================
#include "sprite.h"
#include "score.h"
//*****************************************************************************
// �}�N����`
//***************************************************************************** 
#define TEXTURE_MAX					(2)		// �e�N�X�`���̐�

#define TEXTURE_PATTERN_DIVIDE_X	(5)		// �A�j���p�^�[���̃e�N�X�`�����������iX)
#define TEXTURE_PATTERN_DIVIDE_Y	(2)		// �A�j���p�^�[���̃e�N�X�`�����������iY)

#define TEXTURE_WIDTH				(25)	// �L�����T�C�Y
#define TEXTURE_HEIGHT				(50)

#define TEXTURE_POS_X				(SCREEN_WIDTH - TEXTURE_WIDTH / 2)
#define TEXTURE_POS_Y				(SCREEN_HEIGHT - TEXTURE_HEIGHT / 2)

#define SCORE_MAX					(999999)	//�X�R�A�̍ő�l
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
	"data/TEXTURE/white.png",
};

static int score_data[TYPEMAX] = {0,0,0,0,0};					//�������Ȃǂ̃f�[�^���i�[
static int score_rank[SCORE_RANK_MAX] = { -1,-1,-1,-1,-1 };	//�����L���O���i�[
static float g_texture_w;	//�\���ʒu�Ȃ�
static float g_texture_h;
static float g_texture_x;
static float g_texture_y;
//=============================================================================
// ����������
//=============================================================================
HRESULT InitScore(void)
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


	// �X�R�A�z��̏�����

	for (int i = 0; i < TYPEMAX; i++)
	{
		score_data[i] = 0;
	}

	//�r���[�|�[�g�̐��l�ɉ����ĕ\���ʒu���X�V
	SetScorePos();

	return S_OK;
}

//=============================================================================
// �I������
//=============================================================================
void UninitScore(void)
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
void UpdateScore(void)
{
	//�r���[�|�[�g�̐��l�ɉ����ĕ\���ʒu���X�V
	SetScorePos();

#ifdef _DEBUG	// �f�o�b�O����\������
	char *str = GetDebugStr();
	sprintf(&str[strlen(str)], " SCORE:%d", score_data[3]);

#endif
}

//=============================================================================
// �`�揈��
//=============================================================================
void DrawScore(void)
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
		GetDeviceContext()->PSSetShaderResources(0, 1, &g_Texture[1]);

		// �J�o�[�̈ʒu��e�N�X�`���[���W�𔽉f
		float px = g_texture_x - g_texture_w * 2.5f;
		float py = g_texture_y;
		float pw = g_texture_w * 6.5f;
		float ph = g_texture_h * 1.1f;

		// �P���̃|���S���̒��_�ƃe�N�X�`�����W��ݒ�
		SetSpriteColor(g_VertexBuffer, px, py, pw, ph, 0.0f, 0.0f, 1.0f, 1.0f, D3DXCOLOR(0.8f, 0.8f, 0.8f, 0.3f), true);

		// �|���S���`��
		GetDeviceContext()->Draw(4, 0);
	}

	//���݂̍��v�X�R�A���i�[
	int number = score_data[TOTALSCORE];

	if (number < 0)
	{
		number = 0;
	}
	else if (number > SCORE_MAX)	//�ő�l�𒴂������Ȃ�����
	{
		number = SCORE_MAX;
	}

	//�X�R�A�̕`��
	for (int i = 0; i < SCORE_DIGIT_MAX; i++)
	{					
		int x = 0;

		x = number % 10;

		number /= 10;

		// �e�N�X�`���ݒ�
		GetDeviceContext()->PSSetShaderResources(0, 1, &g_Texture[0]);

		// �X�R�A�̈ʒu��e�N�X�`���[���W�𔽉f
		float px = g_texture_x - g_texture_w *float(i);		// �X�R�A�̕\���ʒuX
		float py = g_texture_y;								// �X�R�A�̕\���ʒuY
		float pw = g_texture_w;								// �X�R�A�̕\����
		float ph = g_texture_h;								// �X�R�A�̕\������

		float tw = 1.0f / TEXTURE_PATTERN_DIVIDE_X;	// �e�N�X�`���̕�
		float th = 1.0f / TEXTURE_PATTERN_DIVIDE_Y;	// �e�N�X�`���̍���
		float tx = tw * float(x % 5);	// �e�N�X�`���̍���X���W
		float ty = th * float(x / 5);	// �e�N�X�`���̍���Y���W

		// �P���̃|���S���̒��_�ƃe�N�X�`�����W��ݒ�
		SetSprite(g_VertexBuffer, px, py, pw, ph, tx, ty, tw, th);

		// �|���S���`��
		GetDeviceContext()->Draw(4, 0);

	}
}


void AddScore(int n, SCORETYPE type)	//�X�R�A�����Z�@��������_���[�W�Ȃǂ��ꍇ����
{
	score_data[TOTALSCORE] += n;

	if (type == DAMAGE)
	{
		score_data[type] += n;
	}
	else
	{
		score_data[type]++;
	}
}

void RecordScore(void)	//�X�R�A�������L���O�z��ւƊi�[
{
	int min = score_data[TOTALSCORE];
	int temp = 0;

	//�����L���O�z��̃\�[�g
	for (int i = 0; i < SCORE_RANK_MAX; i++)
	{
		if (min >= score_rank[i])
		{
			temp = score_rank[i];
			score_rank[i] = min;
			min = temp;
		}
	}
}

int *GetScoreRank(void)	//�����L���O�ւ̃A�h���X
{
	return score_rank;
}

int *GetScoreData(void)	//�X�R�A�f�[�^�ւ̃A�h���X
{
	return score_data;
}

void SetScorePos(void)	//�r���[�|�[�g�̐��l�ɉ����ĕ\���ʒu���X�V
{
	VIEWPORT *viewport = GetViewports();
	float mag = viewport->mag;	//�{��
	float tl_x = viewport->topleft.x;	//������W
	float tl_y = viewport->topleft.y;

	g_texture_w = TEXTURE_WIDTH / mag;
	g_texture_h = TEXTURE_HEIGHT / mag;
	g_texture_x = (-tl_x + SCREEN_WIDTH) / mag - g_texture_w / 2.0f;
	g_texture_y = (-tl_y)/ mag + g_texture_h / 2.0f;
}