//=============================================================================
//
// �^�C�g������[title.cpp]
// Author : GP11B132 36 ���c�x��
//
//=============================================================================
#include "title.h"
#include "sprite.h"
#include "input.h"
#include "bg.h"
#include "fade.h"
#include "sound.h"
//*****************************************************************************
// �}�N����`
//*****************************************************************************
#define TEXTURE_WIDTH				(500)	// �L�����T�C�Y
#define TEXTURE_HEIGHT				(300)	 
#define TEXTURE_MAX					(3)		// �e�N�X�`���̐�

#define TEXTURE_DIVIDE_X	(120)		// �A�j���p�^�[���̃e�N�X�`�����������iX)
#define TEXTURE_DIVIDE_Y	(60)		// �A�j���p�^�[���̃e�N�X�`�����������iY)

#define PRESSENTER_WIDTH			(360)
#define PRESSENTER_HEIGHT			(40)

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
	"data/TEXTURE/title_new.png",
	"data/TEXTURE/press_enter.png",
	"data/TEXTURE/white.png",
};


CURSORPOP title;	//�^�C�g���̍\����

static int x_curve_frequency;						//���ɃJ�[�u����Ԋu
static int y_curve_frequency;						//�c�ɃJ�[�u����Ԋu
static float x_curve_size;							//���̃J�[�u�̕�
static float y_curve_size;							//�c�̃J�[�u�̕�
static float x_rad;
static float y_rad;
static float add_x_rad;
static float add_y_rad;
static float rad = pi / 180.0f;
static float g_mag;									//�^�C�g���̕\���{��
static int g_time;									//�o�ߎ���
static float addcolor = 1.0f / 20.0f;				//�F�𑫂��x����
//=============================================================================
// ����������
//=============================================================================
HRESULT InitTitle(void)
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


	// �^�C�g���̏�����
	title.use = true;
	title.w = TEXTURE_WIDTH;
	title.h = TEXTURE_HEIGHT;
	title.pos = D3DXVECTOR3(SCREEN_WIDTH / 2, SCREEN_HEIGHT / 2, 0.0f);
	title.texNo = 0;
	title.pop = false;

	//�J�[�u�̓x����
	x_curve_frequency = 2;
	y_curve_frequency = 2;
	x_curve_size = 1.0f;
	y_curve_size = 4.0f;
	x_rad = 0.0f;
	y_rad = 0.0f;
	add_x_rad = 360.0f * rad * x_curve_frequency / TEXTURE_DIVIDE_Y;
	add_y_rad = 360.0f * rad * y_curve_frequency / TEXTURE_DIVIDE_X;

	//�^�C�g���̔{��
	g_mag = 5.8f;

	g_time = 0;

	return S_OK;
}

//=============================================================================
// �I������
//=============================================================================
void UninitTitle(void)
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
void UpdateTitle(void)
{
	g_time++;

	if (g_time > 480)	//���o���I��������Enter���󂯕t����
	{
		if (GetKeyboardTrigger(DIK_RETURN))
		{
			SetFade(FADE_OUT, MODE_SELECT);
			PlaySound(SOUND_LABEL_SE_decision);
		}
	}
	else
	{
		//�{�������������Ă���
		g_mag -= 0.015f;
		if (g_mag < 1.0f)
		{
			g_mag = 1.0f;
		}
	}

	if (g_time % 3 == 0)
	{
		//3�t���[�������ɗh�炷
		x_rad += add_x_rad;
		y_rad += add_y_rad;
	}

//#ifdef _DEBUG	// �f�o�b�O����\������
//	char *str = GetDebugStr();
//	sprintf(&str[strlen(str)], " R%.2f G%.2f B%.2f are%.6f", uo.r, uo.g, uo.b,are);
//
//#endif

}

//=============================================================================
// �`�揈��
//=============================================================================
void DrawTitle(void)
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

	// �e�N�X�`���ݒ�
	GetDeviceContext()->PSSetShaderResources(0, 1, &g_Texture[0]);

	// �X�R�A�̈ʒu��e�N�X�`���[���W�𔽉f
	float px = title.pos.x;		// �^�C�g���̕\���ʒuX
	float py = title.pos.y;		// �^�C�g���̕\���ʒuY
	float pw = title.w / TEXTURE_DIVIDE_X * g_mag;			// �^�C�g���̕\����
	float ph = title.h / TEXTURE_DIVIDE_Y * g_mag;			// �^�C�g���̕\������

	float tw = 1.0f / TEXTURE_DIVIDE_X;		// �e�N�X�`���̕�
	float th = 1.0f / TEXTURE_DIVIDE_Y;		// �e�N�X�`���̍���
	float tx = 0.0f;		// �e�N�X�`���̍���X���W
	float ty = 0.0f;		// �e�N�X�`���̍���Y���W

	int cs;	//�F�����肷�邽�߂̓Y��
	D3DXCOLOR color;

	float npx = title.pos.x - pw * (TEXTURE_DIVIDE_X / 2.0f - 0.5f);	//�������ꂽ���̂́A����̍��W
	float npy = title.pos.y - ph * (TEXTURE_DIVIDE_Y / 2.0f - 0.5f);

	for (int i = 0; i < TEXTURE_DIVIDE_X; i++)	//�c�̕���
	{
		cs = (g_time + i) % TEXTURE_DIVIDE_X;
		color = SetColor(cs);	//�F���Z�b�g
		for (int j = 0; j < TEXTURE_DIVIDE_Y; j++)	//���̕���
		{
			// �X�R�A�̈ʒu��e�N�X�`���[���W�𔽉f
			px = npx + pw * i - sinf(x_rad + add_x_rad * j) * x_curve_size;		// �^�C�g���̕\���ʒuX
			py = npy + ph * j - sinf(y_rad + add_y_rad * i) * y_curve_size;		// �^�C�g���̕\���ʒuY
			tx = tw * i;		// �e�N�X�`���̍���X���W
			ty = th * j;		// �e�N�X�`���̍���Y���W
			// �P���̃|���S���̒��_�ƃe�N�X�`�����W��ݒ�
			SetSpriteColor(g_VertexBuffer, px, py, pw, ph, tx, ty, tw, th, color, true);
			//SetSprite(g_VertexBuffer, px, py, pw, ph, tx, ty, tw, th);

			// �|���S���`��
			GetDeviceContext()->Draw(4, 0);
		}
	}

	if (g_time > 420 && g_time <= 640)	//�t���b�V���I�ȕ`��
	{
		GetDeviceContext()->PSSetShaderResources(0, 1, &g_Texture[2]);

		float px = SCREEN_CENTER_X;			//�\���ʒuX
		float py = SCREEN_CENTER_Y;			//�\���ʒuY
		float pw = SCREEN_WIDTH;			//�\����
		float ph = SCREEN_HEIGHT;			//�\������

		float tw = 1.0f;		// �e�N�X�`���̕�
		float th = 1.0f;		// �e�N�X�`���̍���
		float tx = 0.0f;		// �e�N�X�`���̍���X���W
		float ty = 0.0f;		// �e�N�X�`���̍���Y���W

		float alpha = g_time <= 480 ? (g_time - 420) / 60.0f : (640 - g_time) / 180.0f;	//�s�����x��������
		SetSpriteColor(g_VertexBuffer, px, py, pw, ph, tx, ty, tw, th, D3DXCOLOR(1.0f, 1.0f, 1.0f, alpha), true);

		// �|���S���`��
		GetDeviceContext()->Draw(4, 0);
	}
	
	if (g_time > 480)	//���o��Ƀv���X�G���^�[��\��
	{
		GetDeviceContext()->PSSetShaderResources(0, 1, &g_Texture[1]);

		float px = 480.0f;					// �\���ʒuX
		float py = 510.0f;					// �\���ʒuY
		float pw = PRESSENTER_WIDTH;			//�\����
		float ph = PRESSENTER_HEIGHT;			//�\������

		float tw = 1.0f;		// �e�N�X�`���̕�
		float th = 1.0f;		// �e�N�X�`���̍���
		float tx = 0.0f;		// �e�N�X�`���̍���X���W
		float ty = 0.0f;		// �e�N�X�`���̍���Y���W

		float alpha = g_time % 20 < 10 ? 1.0f : 0.0f;
		SetSpriteColor(g_VertexBuffer, px, py, pw, ph, tx, ty, tw, th, D3DXCOLOR(1.0f, 1.0f, 1.0f, alpha), true);

		// �|���S���`��
		GetDeviceContext()->Draw(4, 0);

	}
}

D3DXCOLOR SetColor(int start)	//�F��120�i�K�Őݒ�@���F��
{
	if (start < 20)
	{
		return D3DXCOLOR(1.0f, 0.0f, addcolor*(start - 120), 1.0f);
	}
	else if (start < 40)
	{
		return D3DXCOLOR(1.0f,addcolor*(start - 20), 0.0f, 1.0f);
	}
	else if (start < 60)
	{
		return D3DXCOLOR(1.0f - addcolor*(start - 40), 1.0f, 0.0f, 1.0f);
	}
	else if (start < 80)
	{
		return D3DXCOLOR(0.0f, 1.0f, addcolor*(start - 60), 1.0f);
	}
	else if (start < 100)
	{
		return D3DXCOLOR(0.0f, 1.0f - addcolor*(start - 80), 1.0f, 1.0f);
	}
	else
	{
		return D3DXCOLOR(addcolor*(start - 100), 0.0f, 1.0f, 1.0f);
	}
}





