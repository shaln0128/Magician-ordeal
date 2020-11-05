//=============================================================================
//
// ���[�U�[�̏����ylaser.cpp�z
// Author :GP11B132 36  ���c�x��
//
//=============================================================================
#include "sprite.h"
#include "input.h"
#include "laser.h"
//*****************************************************************************
// �}�N����`
//*****************************************************************************
#define TEXTURE_SIZE				(60)	// �L�����T�C�Y
#define TEXTURE_LONG				(500)	// �L�����T�C�Y
#define TEXTURE_MAX					(2)		// �e�N�X�`���̐�

#define BALL_WIDTH				(80)		//�����̕�
#define BALL_HEIGHT				(30)
//*****************************************************************************
// �v���g�^�C�v�錾
//*****************************************************************************

//*****************************************************************************
// �O���[�o���ϐ�
//*****************************************************************************
static ID3D11Buffer				*g_VertexBuffer = NULL;		// ���_���
static ID3D11ShaderResourceView	*g_Texture[TEXTURE_MAX] = { NULL };	// �e�N�X�`�����

static char *g_TexturName[] = {
	"data/TEXTURE/effect_laser.jpg",
	"data/TEXTURE/effect_ball.jpg",
};

static PARTICLE laser[LASER_MAX];	//���[�U�[�̍\����
//=============================================================================
// ����������
//=============================================================================
HRESULT InitLaser(void)
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

	// �v���C���[�̏�����
	for (int i = 0; i < LASER_MAX; i++)
	{
		laser[i].use = false;
		laser[i].hit = false;	//�������ĂȂ�����
		laser[i].w = 960.0f;
		laser[i].h = 540.0f;
		laser[i].pos = D3DXVECTOR3(0.0f, 0.0f, 0.0f);
		laser[i].move = D3DXVECTOR3(0.0f, 0.0f, 0.0f);
		laser[i].time = 0;
	}

	return S_OK;
}

//=============================================================================
// �I������
//=============================================================================
void UninitLaser(void)
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
void UpdateLaser(void)
{

	for (int i = 0; i < LASER_MAX; i++)
	{
		if (laser[i].use == true)
		{
			laser[i].time -= 1;
			if (laser[i].time == 0)	//�������Ԃ��؂ꂽ�����
			{
				laser[i].use = false;
			}
		}
	}

#ifdef _DEBUG	// �f�o�b�O����\������
	/*char *str = GetDebugStr();
	sprintf(&str[strlen(str)], " R%.2f G%.2f B%.2f are%.6f", uo.r, uo.g, uo.b, are);
*/
#endif

}

//=============================================================================
// �`�揈��
//=============================================================================
void DrawLaser(void)
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


	SetBlendState(BLEND_MODE_ADD);	//���Z����

	for (int i = 0; i < LASER_MAX; i++)
	{
		//�e�N�X�`���ݒ�
		GetDeviceContext()->PSSetShaderResources(0, 1, &g_Texture[0]);

		if (laser[i].use == true)
		{
			// �P���̃|���S���̒��_�ƃe�N�X�`�����W��ݒ�
			float dx = laser[i].pos.x;
			float dy = laser[i].pos.y;

			float dw = laser[i].w;
			float dh = laser[i].h;

			switch (laser[i].direction)	//�^����ꂽ�����ɂ������ĕ��𒲐�
			{
			case DOWN:
			case UP:
				dw = laser[i].time > 50 ? (60 - laser[i].time) * 6.0f : dw + rand() % 10 - 5;
				break;
			case LEFT:
			case RIGHT:
				dh = laser[i].time > 50 ? (60 - laser[i].time) * 6.0f : dh + rand() % 10 - 5;
				break;
			}

			SetSpriteColor(g_VertexBuffer, dx, dy, dw, dh, 0.0f, 0.0f, 1.0f, 1.0f,
				D3DXCOLOR(0.3f, 0.3f, 1.0f, 1.0f), true);

			// �|���S���`��
			GetDeviceContext()->Draw(4, 0);

			for (int j = 0; j < PARTICLE_MAX; j++)	//�p�[�e�B�N���̕`��
			{
				// �P���̃|���S���̒��_�ƃe�N�X�`�����W��ݒ�
				float sx, sy;
				float cr = rand() % (10 + 1) / 10.0f;
				float cg = rand() % (10 + 1) / 10.0f;
				float cb = rand() % (10 + 1) / 10.0f;
				float pdw, pdh;

				switch (laser[i].direction)	//�^����ꂽ�����ɉ����Đ����͈͂ƕ��𒲐�
				{
				case DOWN:
				case UP:
					sx = (float)(rand() % int(dw)) - dw / 2;
					sy = ((float)(rand() % int(dh)) - dh / 2) * 0.95f;
					pdw = 9.0f;
					pdh = (float)(rand() % 80) + 20.0f;
					break;
				case LEFT:
				case RIGHT:
					sx = ((float)(rand() % int(dw)) - dw / 2) * 0.95f;
					sy = (float)(rand() % int(dh)) - dh / 2;
					pdw = (float)(rand() % 80) + 20.0f;
					pdh = 9.0f;
					break;
				}

				SetSpriteColor(g_VertexBuffer, dx + sx, dy + sy, pdw, pdh, 0.0f, 0.0f, 1.0f, 1.0f,
					D3DXCOLOR(cr, cg, cb, 1.0f), true);

				// �|���S���`��
				GetDeviceContext()->Draw(4, 0);
			}

			//�����̕`��
			//�e�N�X�`���ݒ�
			GetDeviceContext()->PSSetShaderResources(0, 1, &g_Texture[1]);

			dx = laser[i].pos.x;
			dy = laser[i].pos.y;

			switch (laser[i].direction)
			{
			case DOWN:
				dy += 10.0f - laser[i].h * 0.5f;
				dw = BALL_WIDTH;
				dh = BALL_HEIGHT;
				break;
			case LEFT:
				dx += laser[i].w * 0.5f;
				dw = BALL_HEIGHT;
				dh = BALL_WIDTH;
				break;
			case RIGHT:
				dx += -laser[i].w * 0.5f;
				dw = BALL_HEIGHT;
				dh = BALL_WIDTH;
				break;
			case UP:
				dy += -10.0f + laser[i].h * 0.5f;
				dw = BALL_WIDTH;
				dh = BALL_HEIGHT;
				break;
			}

			for (int j = 0; j < 5; j++)
			{
				float mag = (rand() % 20 + 90) * 0.01f;

				SetSpriteColor(g_VertexBuffer, dx, dy, dw * mag, dh * mag, 0.0f, 0.0f, 1.0f, 1.0f,
					D3DXCOLOR(0.3f, 0.3f, 1.0f, 1.0f), true);

				// �|���S���`��
				GetDeviceContext()->Draw(4, 0);
			}
		}
	}
	
	SetBlendState(BLEND_MODE_ALPHABLEND);
}

PARTICLE *GetLaser(void)	//���[�U�[�̍\���̂ւ̃A�h���X
{
	return(&laser[0]);
}

void SetLaser(D3DXVECTOR3 pos,DIRECTION vect)	//���[�U�[�̃Z�b�g
{
	for (int i = 0; i < LASER_MAX; i++)
	{
		if (laser[i].use == false)
		{
			laser[i].use = true;
			laser[i].hit = false;
			laser[i].time = 60;
			laser[i].direction = vect;

			switch (vect)	//�^����ꂽ�����ɉ����ĕ��ƈʒu�𒲐�
			{
			case DOWN:
				laser[i].pos = pos + D3DXVECTOR3(0.0f, TEXTURE_LONG / 2.0f + 15.0f, 0.0f);
				laser[i].w = TEXTURE_SIZE;
				laser[i].h = TEXTURE_LONG - 50.0f;
				break;
			case LEFT:
				laser[i].pos = pos + D3DXVECTOR3(-TEXTURE_LONG / 2.0f - 8.0f, 0.0f, 0.0f);
				laser[i].w = TEXTURE_LONG - 50.0f;
				laser[i].h = TEXTURE_SIZE;
				break;
			case RIGHT:
				laser[i].pos = pos + D3DXVECTOR3(TEXTURE_LONG / 2.0f + 8.0f, 0.0f, 0.0f);
				laser[i].w = TEXTURE_LONG - 50.0f;
				laser[i].h = TEXTURE_SIZE;
				break;
			case UP:
				laser[i].pos = pos + D3DXVECTOR3(0.0f, -TEXTURE_LONG / 2.0f - 15.0f, 0.0f);
				laser[i].w = TEXTURE_SIZE;
				laser[i].h = TEXTURE_LONG - 50.0f;
				break;
			}

			return;
		}
	}
}






