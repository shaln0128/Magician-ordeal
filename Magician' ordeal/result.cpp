//=============================================================================
//
// ���U���g��ʏ��� [result.cpp]
// Author : GP11B132�@36�@���c�x��
//
//=============================================================================
#include "result.h"
#include "input.h"
#include "fade.h"
#include "sound.h"
#include "sprite.h"
#include "ready.h"
#include "sound.h"
#include "score.h"
//*****************************************************************************
// �}�N����`
//*****************************************************************************
#define TEXTURE_MAX					(3)				// �e�N�X�`���̐�

#define TEXTURE_PATTERN_DIVIDE_X	(5)		// �A�j���p�^�[���̃e�N�X�`�����������iX)
#define TEXTURE_PATTERN_DIVIDE_Y	(2)		// �A�j���p�^�[���̃e�N�X�`�����������iY)

#define TEXTURE_WIDTH_RESULT		(32)			// �L�����T�C�Y
#define TEXTURE_HEIGHT_RESULT		(32)

#define TEXTURE_WIDTH				(800)			// �L�����T�C�Y
#define TEXTURE_HEIGHT				(40)

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
	"data/TEXTURE/number_red.png",
	"data/TEXTURE/result01.png",
};


static bool						g_Use;						// true:�g���Ă���  false:���g�p
static float					g_w, g_h;					// ���ƍ���
static D3DXVECTOR3				g_Pos;						// �|���S���̍��W
static int						g_TexNo;					// �e�N�X�`���ԍ�

static int						g_time;						//�o�ߎ���
static int						new_score;					//�V�K�X�R�A�̓Y��
//=============================================================================
// ����������
//=============================================================================
HRESULT InitResult(void)
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


	// �v���C���[�̏�����
	
	new_score = -1;
	g_time = 0;

	int *score_rank = GetScoreRank();
	int *score_data = GetScoreData();

	for (int i = 0; i < SCORE_RANK_MAX; i++)
	{
		if (score_rank[i] == score_data[TOTALSCORE])	//�������v�_�ƃ����L���O�̐����������Ȃ�A�Y�����i�[����
		{
			new_score = i;
			break;
		}		
	}
	PlaySound(SOUND_LABEL_SE_drumroll);	//�h�������[����炷
	return S_OK;
}

//=============================================================================
// �I������
//=============================================================================
void UninitResult(void)
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
void UpdateResult(void)
{
	g_time++;

	if (g_time == 24 ||
		g_time == 68 ||
		g_time == 112 ||
		g_time == 156 ||
		g_time == 200)
	{
		PlaySound(SOUND_LABEL_SE_scoredecision);	//�X�R�A���\�����ꂫ��x�ɉ���炷
	}

	if (g_time >= 220 && 
		g_time <= 300 &&
		g_time % 20 == 0)	//�����L���O���\�������x�ɉ���炷
	{
		if ((300 - g_time) / 20 == new_score)	//�����V���ɓo�^���ꂽ�X�R�A�̃^�C�~���O�ł���΁A���ʉ���ς���
		{
			PlaySound(SOUND_LABEL_SE_rankindecision);
		}
		else
		{
			PlaySound(SOUND_LABEL_SE_rankdecision);
		}
	}

	if (GetKeyboardTrigger(DIK_RETURN))
	{
		SetFade(FADE_OUT, MODE_TITLE);
		PlaySound(SOUND_LABEL_SE_decision);
	}
	else if (GetKeyboardTrigger(DIK_R))
	{
		SetFade(FADE_OUT, MODE_GAME);
		PlaySound(SOUND_LABEL_SE_decision);
	}

#ifdef _DEBUG	// �f�o�b�O����\������
	//char *str = GetDebugStr();
	//sprintf(&str[strlen(str)], " PX:%.2f PY:%.2f", g_Pos.x, g_Pos.y);

#endif

}

//=============================================================================
// �`�揈��
//=============================================================================
void DrawResult(void)
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
	
	int *score_rank = GetScoreRank();
	int *score_data = GetScoreData();

	//�X�R�A�̕\��
	for (int i = 0; i < TYPEMAX; i++)
	{
		int number = score_data[i];

		number = number >= 0 ? number : 0;

		// �e�N�X�`���ݒ�
		GetDeviceContext()->PSSetShaderResources(0, 1, &g_Texture[0]);

		for (int j = 0; j < SCORE_DIGIT_MAX; j++)
		{
			int x = 0;

			x = g_time < 4 * (i * SCORE_DIGIT_MAX + j) + i * 20 ? rand() % 10 : number % 10;	//�\�����Ԃ�����܂ł͗����ŕ\��

			number /= 10;

			// �v���C���[�̈ʒu��e�N�X�`���[���W�𔽉f
			float px = 460.0f - (TEXTURE_WIDTH_RESULT)* float(j);	// �v���C���[�̕\���ʒuX
			float py = 250.0f + 44.0f * i;	// �v���C���[�̕\���ʒuY
			float pw = TEXTURE_WIDTH_RESULT;		// �v���C���[�̕\����
			float ph = TEXTURE_HEIGHT_RESULT;		// �v���C���[�̕\������

			float tw = 1.0f / TEXTURE_PATTERN_DIVIDE_X;	// �e�N�X�`���̕�
			float th = 1.0f / TEXTURE_PATTERN_DIVIDE_Y;	// �e�N�X�`���̍���
			float tx = tw * (x % TEXTURE_PATTERN_DIVIDE_X);	// �e�N�X�`���̍���X���W
			float ty = th * (x / TEXTURE_PATTERN_DIVIDE_X);	// �e�N�X�`���̍���Y���W

			// �P���̃|���S���̒��_�ƃe�N�X�`�����W��ݒ�
			SetSprite(g_VertexBuffer, px, py, pw, ph, tx, ty, tw, th);

			// �|���S���`��
			GetDeviceContext()->Draw(4, 0);

		}
	}

	//�����L���O�̕`��
	for (int i = 0; i < SCORE_RANK_MAX; i++)
	{
		int number = score_rank[i];

		number = number >= 0 ? number: 0;
		
		int texNo;

		texNo = new_score == i ? 1 : 0;

		float sx = float(200 + (5 - i) * 20 - g_time);
		sx = sx > 0 ? sx * 6.0f : 0.0f;	//�\�����Ԃ�����܂ł͉�ʊO�ɕ`��@�E���痬��Ă���悤��

		// �e�N�X�`���ݒ�
		GetDeviceContext()->PSSetShaderResources(0, 1, &g_Texture[texNo]);

		for (int j = 0; j < SCORE_DIGIT_MAX; j++)
		{
			int x = 0;

			x = number % 10;

			number /= 10;

			// �v���C���[�̈ʒu��e�N�X�`���[���W�𔽉f
			float px = 810.0f - (TEXTURE_WIDTH_RESULT)* float(j);	// �X�R�A�̕\���ʒuX
			float py = 250.0f + 44.0f * i;							// �X�R�A�̕\���ʒuY
			float pw = TEXTURE_WIDTH_RESULT;						// �X�R�A�̕\����
			float ph = TEXTURE_HEIGHT_RESULT;						// �X�R�A�̕\������

			float tw = 1.0f / TEXTURE_PATTERN_DIVIDE_X;				// �e�N�X�`���̕�
			float th = 1.0f / TEXTURE_PATTERN_DIVIDE_Y;				// �e�N�X�`���̍���
			float tx = tw * (x % TEXTURE_PATTERN_DIVIDE_X);			// �e�N�X�`���̍���X���W
			float ty = th * (x / TEXTURE_PATTERN_DIVIDE_X);			// �e�N�X�`���̍���Y���W

			// �P���̃|���S���̒��_�ƃe�N�X�`�����W��ݒ�
			SetSprite(g_VertexBuffer, px + sx, py, pw, ph, tx, ty, tw, th);

			// �|���S���`��
			GetDeviceContext()->Draw(4, 0);

		}
	}
	if (g_time > 420)	//�\������������i�r��\��
	{
		GetDeviceContext()->PSSetShaderResources(0, 1, &g_Texture[2]);

		float px = 480.0f;				// �i�r�̕\���ʒuX
		float py = 480.0f;				// �i�r�̕\���ʒuY
		float pw = TEXTURE_WIDTH;		// �i�r�̕\����
		float ph = TEXTURE_HEIGHT;		// �i�r�̕\������

		float tw = 1.0f;	// �e�N�X�`���̕�
		float th = 1.0f;	// �e�N�X�`���̍���
		float tx = 0.0f;	// �e�N�X�`���̍���X���W
		float ty = 0.0f;	// �e�N�X�`���̍���Y���W

		// �P���̃|���S���̒��_�ƃe�N�X�`�����W��ݒ�
		SetSprite(g_VertexBuffer, px, py, pw, ph, tx, ty, tw, th);

		// �|���S���`��
		GetDeviceContext()->Draw(4, 0);

	}
}


