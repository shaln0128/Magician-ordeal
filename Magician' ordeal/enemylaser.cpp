//=============================================================================
//
// �G�̃��[�U�[�����yenemylaser.cpp�z
// Author : GP11B132 36 ���c�x��
//
//=============================================================================
#include "sprite.h"
#include "input.h"
#include "enemylaser.h"
#include "sound.h"
//*****************************************************************************
// �}�N����`
//*****************************************************************************
#define TEXTURE_SIZE					(40)	// �Z���ق��̒���
#define TEXTURE_LONG_WIDTH				(840)	// �����̏ꍇ
#define TEXTURE_LONG_HEIGHT				(490)	// �c���̏ꍇ
#define TEXTURE_MAX					(2)				// �e�N�X�`���̐�

#define BALL_WIDTH				(60)	//�����̑傫��
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

static PARTICLE enemylaser[ENEMY_LASER_MAX];	//�G�l�~�[���[�U�[�̍\����
//=============================================================================
// ����������
//=============================================================================
HRESULT InitEnemyLaser(void)
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

	// �G�l�~�[���[�U�[�̏�����
	for (int i = 0; i < ENEMY_LASER_MAX; i++)
	{
		enemylaser[i].use = false;
		enemylaser[i].hit = false;	//�������Ă��邩�ǂ���
		enemylaser[i].w = TEXTURE_SIZE;
		enemylaser[i].h = TEXTURE_LONG_HEIGHT;
		enemylaser[i].pos = D3DXVECTOR3(0.0f, 0.0f, 0.0f);
		enemylaser[i].move = D3DXVECTOR3(0.0f, 0.0f, 0.0f);
		enemylaser[i].time = 0;
		enemylaser[i].color = D3DXCOLOR(0.0f, 0.0f, 0.0f,0.0f);
	}

	return S_OK;
}

//=============================================================================
// �I������
//=============================================================================
void UninitEnemyLaser(void)
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
void UpdateEnemyLaser(void)
{

	for (int i = 0; i < ENEMY_LASER_MAX; i++)
	{
		if (enemylaser[i].use == true)
		{
			enemylaser[i].time -= 1;

			if (enemylaser[i].time == 0)	//�������Ԃ��؂ꂽ�����
			{
				enemylaser[i].use = false;
			}
			else if (enemylaser[i].time == 60)	//���˂̒i�K�ŉ���炷
			{
				PlaySound(SOUND_LABEL_SE_laser);
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
void DrawEnemyLaser(void)
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

	for (int i = 0; i < ENEMY_LASER_MAX; i++)
	{
		//�e�N�X�`���ݒ�
		GetDeviceContext()->PSSetShaderResources(0, 1, &g_Texture[0]);

		if (enemylaser[i].use == true)
		{
			// �P���̃|���S���̒��_�ƃe�N�X�`�����W��ݒ�
			float dx = enemylaser[i].pos.x;
			float dy = enemylaser[i].pos.y;

			float dw = enemylaser[i].w;
			float dh = enemylaser[i].h;
			float sx, sy;	//���ꕝ

			D3DXCOLOR color = enemylaser[i].color;
			color.a = enemylaser[i].time > 60 ? (180 - enemylaser[i].time) / 120.0f : 1.0f;	//���X�ɕs�����x���グ��

			switch (enemylaser[i].direction)	//�^����ꂽ�����ɉ����ĕ\���T�C�Y��ύX
			{
			case DOWN:	//�c��
			case UP:
				dh = enemylaser[i].time > 60 ? dh * (180 - enemylaser[i].time) / 120.0f : dh;
				dw = enemylaser[i].time > 60 ? dw : dw + rand() % 10 - 5;	//�Z���ق��ɂ̓u���𐶂�������
				break;
			case LEFT:	//����
			case RIGHT:
				dh = enemylaser[i].time > 60 ? dh : dh + rand() % 10 - 5;	//�Z���ق��ɂ̓u���𐶂�������
				dw = enemylaser[i].time > 60 ? dw * (180 - enemylaser[i].time) / 120.0f : dw;
				break;
			}

			SetSpriteColor(g_VertexBuffer, dx, dy, dw, dh, 0.0f, 0.0f, 1.0f, 1.0f,
				color, true);

			// �|���S���`��
			GetDeviceContext()->Draw(4, 0);

			if (enemylaser[i].time <= 60)	//���˒��̓p�[�e�B�N���𐶐�
			{
				for (int j = 0; j < PARTICLE_MAX; j++)
				{

					// �F�̓����_��
					float cr = rand() % (10 + 1) / 10.0f;
					float cg = rand() % (10 + 1) / 10.0f;
					float cb = rand() % (10 + 1) / 10.0f;
					float pdw, pdh;	//�p�[�e�B�N���̕\����

					switch (enemylaser[i].direction)	//�^����ꂽ�����ɉ����ė����̐����͈͂�ς���
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

			}
			
			//�����̕`��ݒ�
			//�e�N�X�`���ݒ�
			GetDeviceContext()->PSSetShaderResources(0, 1, &g_Texture[1]);

			dx = enemylaser[i].pos.x;
			dy = enemylaser[i].pos.y;
			sx = 0.0f;
			sy = 0.0f;
			color.a = 1.0f;

			switch (enemylaser[i].direction)	//�^����ꂽ�����ɉ����ĕ\���ʒu�ƕ���ς���
			{
			case DOWN:
				sy = enemylaser[i].h * 0.5f;
				dw = BALL_WIDTH;
				dh = BALL_HEIGHT;
				break;
			case LEFT:
				sx = enemylaser[i].w * 0.5f;
				dw = BALL_HEIGHT;
				dh = BALL_WIDTH;
				break;
			}

			sx = enemylaser[i].time > 60 ? sx * (180 - enemylaser[i].time) / 120.0f : sx;
			sy = enemylaser[i].time > 60 ? sy * (180 - enemylaser[i].time) / 120.0f : sy;

			for (int j = 0; j < 5; j++)	//�����_���Ȕ{���i0.9�`1.1�j�̂��̂�5���d�˂ĕ`��
			{
				float mag = (rand() % 20 + 90) * 0.01f;

				SetSpriteColor(g_VertexBuffer, dx + sx, dy + sy, dw * mag, dh * mag, 0.0f, 0.0f, 1.0f, 1.0f,
					color, true);

				// �|���S���`��
				GetDeviceContext()->Draw(4, 0);

				SetSpriteColor(g_VertexBuffer, dx - sx, dy - sy, dw * mag, dh * mag, 0.0f, 0.0f, 1.0f, 1.0f,
					color, true);

				// �|���S���`��
				GetDeviceContext()->Draw(4, 0);
			}
		}
	}

	SetBlendState(BLEND_MODE_ALPHABLEND);
}

PARTICLE *GetEnemyLaser(void)	//�G�l�~�[���[�U�[�̍\���̂ւ̃A�h���X
{
	return(&enemylaser[0]);
}

void SetEnemyLaser(D3DXVECTOR3 pos, DIRECTION vect)	//�G�l�~�[���[�U�[�̃Z�b�g
{
	PlaySound(SOUND_LABEL_SE_enemylasercharge);	//�Z�b�g���ɉ���炷
	for (int i = 0; i < ENEMY_LASER_MAX; i++)
	{
		if (enemylaser[i].use == false)
		{
			enemylaser[i].use = true;
			enemylaser[i].hit = false;
			enemylaser[i].time = 180;
			enemylaser[i].direction = vect;
			enemylaser[i].color = D3DXCOLOR( 1.0f, 0.2f, 0.2f,1.0f);

			switch (vect)
			{
			case DOWN:
				enemylaser[i].pos = pos;
				enemylaser[i].w = TEXTURE_SIZE;
				enemylaser[i].h = TEXTURE_LONG_HEIGHT;
				break;
			case LEFT:
				enemylaser[i].pos = pos;
				enemylaser[i].w = TEXTURE_LONG_WIDTH;
				enemylaser[i].h = TEXTURE_SIZE;
				break;
			}
			return;
		}
	}
}






