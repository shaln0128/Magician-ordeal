//=============================================================================
//
// ���p�[�e�B�N���̏����yfire.cpp�z
// Author : GP11B132 36 ���c�x��
//
//=============================================================================
#include "sprite.h"
#include "input.h"
#include "bg.h"
#include "fade.h"
#include "fire.h"
//*****************************************************************************
// �}�N����`
//*****************************************************************************
#define TEXTURE_SIZE				(20)	// �L�����T�C�Y
#define TEXTURE_MAX					(2)		// �e�N�X�`���̐�

#define MAHOUJIN_WIDTH				(60)	//���@�w�̃T�C�Y
#define MAHOUJIN_HEIGHT				(20)

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
	"data/TEXTURE/effect_ball.jpg",
	"data/TEXTURE/mahoujin.png"
};

static PARTICLE fire[FIRE_MAX];	//���p�[�e�B�N���̍\����
static int breath_time;			//���̐�������
static float rad = pi / 180.0f;	//���W�A���ւ̕ϊ�
static D3DXVECTOR3 g_Pos;		//���@�w�̕\���ʒu
static float g_w, g_h;			//���@�w�̃T�C�Y
//=============================================================================
// ����������
//=============================================================================
HRESULT InitFire(void)
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

	breath_time = 0;

	// ���p�[�e�B�N���̏�����
	for (int i = 0; i < FIRE_MAX; i++)
	{
		fire[i].use = false;
		fire[i].size = TEXTURE_SIZE;
		fire[i].pos = D3DXVECTOR3(0.0f, 0.0f, 0.0f);
		fire[i].move = D3DXVECTOR3(0.0f, 0.0f, 0.0f);
		fire[i].time = 0;
	}

	return S_OK;
}

//=============================================================================
// �I������
//=============================================================================
void UninitFire(void)
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
void UpdateFire(void)
{
	PLAYER *player = GetPlayer();

	int fire_count = 0;
	int count = 0;	//1�t���[���Ő������ꂽ�����J�E���g

	for (int i = 0; i < FIRE_MAX; i++)
	{
		if (breath_time > 0 &&
			fire[i].use == false &&
			count < 5)	//�����������ԓ��ŁA�t���[�����ł̐����\���ɒB���Ă��Ȃ��Ȃ�
		{
			float angle = (rand() % 120 - 60) * rad;	//�|60�`60�x
			fire[i].use = true;
			fire[i].size = TEXTURE_SIZE;
			float speed = angle > 0 ? 2.2f - angle : 2.2f + angle;	//�p�x�ɉ����đ��x��ω��@�^�񒆂قǑ���

			switch (player[0].direction)	//�v���C���[�̌����Ă�����ɐ���
			{
			case DOWN:
				fire[i].pos = player[0].pos + D3DXVECTOR3(rand() % 40 - 20.0f, 30.0f, 0.0f);
				angle += 90 * rad;
				break;
			case LEFT:
				fire[i].pos = player[0].pos + D3DXVECTOR3(-20.0f, rand() % 40 - 20.0f, 0.0f);
				angle += 180 * rad;
				break;
			case RIGHT:
				fire[i].pos = player[0].pos + D3DXVECTOR3(20.0f, rand() % 40 - 20.0f, 0.0f);
				break;
			case UP:
				fire[i].pos = player[0].pos + D3DXVECTOR3(rand() % 40 - 20.0f, -30.0f, 0.0f);
				angle -= 90 * rad;
				break;
			}

			fire[i].move = D3DXVECTOR3(cosf(angle)*speed, sinf(angle)*speed, 0.0f);
			fire[i].time = rand() % 10 + 40;	//�������Ԃɂ��u������������
			count++;
		}
		else if (fire[i].use == true)	//�������ꂽ���̂̏���
		{
			fire[i].time -= 1;
			fire[i].pos += fire[i].move;
			
			if (fire[i].time == 0)	//�������Ԃ��؂ꂽ�����
			{
				fire[i].use = false;
			}
			else 
			{
				if (fire[i].time < 10)	//�c��10�t���[���̊Ԃ͏���������
				{
					fire[i].size *= 0.95f;	//5������
				}

				fire_count += 1;
			}
		}
	}

	if (breath_time > 0)	//�������Ԓ�
	{
		breath_time--;
		switch (player[0].direction)	//�v���C���[�̌����ɉ����Ė��@�w�̈ʒu�ƃT�C�Y��ω�
		{
		case DOWN:
			g_Pos = player[0].pos + D3DXVECTOR3(0.0f, 30.0f, 0.0f);
			g_w = MAHOUJIN_WIDTH;
			g_h = MAHOUJIN_HEIGHT;
			break;
		case LEFT:
			g_Pos = player[0].pos + D3DXVECTOR3(-20.0f, 0.0f, 0.0f);
			g_w = MAHOUJIN_HEIGHT;
			g_h = MAHOUJIN_WIDTH;
			break;
		case RIGHT:
			g_Pos = player[0].pos + D3DXVECTOR3(20.0f, 0.0f, 0.0f);
			g_w = MAHOUJIN_HEIGHT;
			g_h = MAHOUJIN_WIDTH;
			break;
		case UP:
			g_Pos = player[0].pos + D3DXVECTOR3(0.0f, -30.0f, 0.0f);
			g_w = MAHOUJIN_WIDTH;
			g_h = MAHOUJIN_HEIGHT;
			break;
		}
	}
	
#ifdef _DEBUG	// �f�o�b�O����\������
	char *str = GetDebugStr();
	sprintf(&str[strlen(str)], " fire%d", fire_count);
#endif

}

//=============================================================================
// �`�揈��
//=============================================================================
void DrawFire(void)
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

	SetBlendState(BLEND_MODE_ADD);

	for (int i = 0; i < FIRE_MAX; i++)
	{
		if (fire[i].use == true)	//�p�[�e�B�N���̕`��
		{
			// �P���̃|���S���̒��_�ƃe�N�X�`�����W��ݒ�
			float dx = fire[i].pos.x;
			float dy = fire[i].pos.y;

			float size = fire[i].size;

			SetSpriteColor(g_VertexBuffer, dx, dy, size, size, 0.0f, 0.0f, 1.0f, 1.0f,
					D3DXCOLOR(1.0f, 0.2f, 0.1f, 1.0f),true);

				// �|���S���`��
			GetDeviceContext()->Draw(4, 0);
		}
	}

	if (breath_time > 0)	//���@�w�̕`��
	{
		GetDeviceContext()->PSSetShaderResources(0, 1, &g_Texture[1]);

		float dx = g_Pos.x;
		float dy = g_Pos.y;
		float dw = g_w;
		float dh = g_h;

		SetSpriteColor(g_VertexBuffer, dx, dy, dw, dh, 0.0f, 0.0f, 1.0f, 1.0f,
			D3DXCOLOR(1.0f, 1.0f, 1.0f, 1.0f), true);

		// �|���S���`��
		GetDeviceContext()->Draw(4, 0);
	}

	SetBlendState(BLEND_MODE_ALPHABLEND);

}

int *GetBreathTime(void)	//�������Ԃւ̃A�h���X
{
	return(&breath_time);
}

PARTICLE *GetFire(void)	//���p�[�e�B�N���ւ̃A�h���X
{
	return(&fire[0]);
}