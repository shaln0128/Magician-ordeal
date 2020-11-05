//=============================================================================
//
// ���C�W���� [enemy.cpp]
// Author : GP11B132 36 ���c�x��
//
//=============================================================================
#include "enemy.h"
#include "sprite.h"

//*****************************************************************************
// �}�N����`
//*****************************************************************************
#define TEXTURE_WIDTH				(15)	// �L�����T�C�Y
#define TEXTURE_HEIGHT				(30)	// 
#define TEXTURE_MAX					(1)		// �e�N�X�`���̐�

#define TEXTURE_PATTERN_DIVIDE_X	(3)		// �A�j���p�^�[���̃e�N�X�`�����������iX)
#define TEXTURE_PATTERN_DIVIDE_Y	(4)		// �A�j���p�^�[���̃e�N�X�`�����������iY)
#define ANIM_PATTERN_NUM			(TEXTURE_PATTERN_DIVIDE_X)	// �A�j���[�V�����p�^�[����
#define ANIM_WAIT					(4)		// �A�j���[�V�����̐؂�ւ��Wait�l

#define ABS_FUNC(x)					((x) < 0 ? -(x) : (x));	//��Βl�ɕϊ�����}�N��
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
	"data/TEXTURE/mage001.png",
};

ENEMY enemy[ENEMY_MAX];	//���C�W�̍\����

static float rad = pi / 180;	//���W�A���ϊ��p
//=============================================================================
// ����������
//=============================================================================
HRESULT InitEnemy(void)
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


	// ���C�W�̏�����

	for (int i = 0; i < ENEMY_MAX; i++)
	{
		enemy[i].use = i == 0 ? true : false;
		enemy[i].w = TEXTURE_WIDTH;
		enemy[i].h = TEXTURE_HEIGHT;
		enemy[i].pos = D3DXVECTOR3(SCREEN_WIDTH * 0.625f, SCREEN_HEIGHT / 2.0f, 0.0f);	//�����ʒu�@�E��ւ�
		enemy[i].rot = D3DXVECTOR3(0.0f, 0.0f, 0.0f);
		enemy[i].texNo = 0;

		enemy[i].countAnim = 0;
		enemy[i].patternAnim = 0;

		enemy[i].direction = LEFT;	//�������Ŕz�u

		enemy[i].color_R = 1.0f;
		enemy[i].color_G = 1.0f;
		enemy[i].color_B = 1.0f;

		enemy[i].speed = 1.0f;

		enemy[i].rad = 0.0f;
		enemy[i].rotation = 0.0f;

		enemy[i].time = 0;

		if (GetMode() == MODE_GAME)	//�Q�[�����[�h�̏ꍇ
		{
			float angle = (rand() % 360) * rad;	//�����Ŋp�x���w��
			enemy[i].move = D3DXVECTOR3(cosf(angle) * enemy[i].speed, sinf(angle) * enemy[i].speed, 0.0f);	//���̊p�x�ւƐi��
			enemy[i].time = (rand() % 4 + 2) * 60;	//�i�ގ��Ԃ������@�Q�`�T�b
		}
		else	//�`���[�g���A���̏ꍇ
		{
			enemy[i].direction = DOWN;	//�������Ŕz�u
			enemy[i].pos = D3DXVECTOR3(150.0f, 150.0f, 0.0f);	//�����ʒu�͍���
			enemy[i].move = D3DXVECTOR3(0.0f, 0.0f, 0.0f);	//�����Ȃ�
			enemy[i].time = 0;
		}
	}

	return S_OK;
}

//=============================================================================
// �I������
//=============================================================================
void UninitEnemy(void)
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
void UpdateEnemy(void)
{
	switch (GetMode())
	{
	case MODE_GAME:	//�Q�[�����[�h�̎�
		for (int i = 0; i < ENEMY_MAX; i++)
		{
			if (enemy[i].use == true)		// ���C�W���g���Ă���H
			{						// Yes
				// �A�j���[�V����  
				enemy[i].countAnim++;
				if ((enemy[i].countAnim % ANIM_WAIT) == 0)
				{
					// �p�^�[���̐؂�ւ�
					enemy[i].patternAnim = (enemy[i].patternAnim + 1) % ANIM_PATTERN_NUM;
				}

				if (enemy[i].invincible_time > 0)	//���G���Ԃ����炷����
				{
					enemy[i].invincible_time -= 1;
				}

				enemy[i].time--;	//�i�ގ��Ԃ����炷����

				if (enemy[i].time == 0)	//0�ɂȂ������A�ēx�i�ޕ����Ǝ��Ԃ𗐐��Ŏw��
				{
					float angle = (rand() % 360) * rad;
					enemy[i].move = D3DXVECTOR3(cosf(angle) * enemy[i].speed, sinf(angle) * enemy[i].speed, 0.0f);
					enemy[i].time = (rand() % 4 + 2) * 60;//(0�`3) + 2�b
				}

				enemy[i].pos += enemy[i].move;	//�ړ�����

				float move_x = enemy[i].move.x;	//���̈ړ��ʂ�ۑ�
				float abs_x = ABS_FUNC(move_x);	//��Βl��
				float move_y = enemy[i].move.y;	//���̈ړ��ʂ�ۑ�
				float abs_y = ABS_FUNC(move_y);	//��Βl��
											   	
				if (abs_x > abs_y)	//���̕����ړ��ʂ��傫�����
				{
					enemy[i].direction = move_x > 0 ? RIGHT : LEFT;	//�ړ��ʂ����Ȃ�E�A���Ȃ獶
				}
				else
				{
					enemy[i].direction = move_y > 0 ? DOWN : UP;	//�ړ��ʂ����Ȃ牺�A���Ȃ��
				}

			}
		}
		break;
	case MODE_TUTORIAL:	//�`���[�g���A���̎�
		for (int i = 0; i < ENEMY_MAX; i++)
		{
			if (enemy[i].use == true)		// ���C�W���g���Ă���H
			{						// Yes
				// �A�j���[�V����  
				enemy[i].countAnim++;
				if ((enemy[i].countAnim % ANIM_WAIT) == 0)
				{
					// �p�^�[���̐؂�ւ�
					enemy[i].patternAnim = (enemy[i].patternAnim + 1) % ANIM_PATTERN_NUM;
				}

				if (enemy[i].invincible_time > 0)
				{
					enemy[i].invincible_time -= 1;
				}
			}
		}

		break;
	}

}

//=============================================================================
// �`�揈��
//=============================================================================
void DrawEnemy(void)
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

	for (int i = 0; i < ENEMY_MAX; i++)
	{
	
		if (enemy[i].use == true)		// ���C�W���g���Ă���H
			{						// Yes
				// �e�N�X�`���ݒ�
				GetDeviceContext()->PSSetShaderResources(0, 1, &g_Texture[enemy[i].texNo]);

				// �v���C���[�̈ʒu��e�N�X�`���[���W�𔽉f
				float px = enemy[i].pos.x;	// ���C�W�̕\���ʒuX
				float py = enemy[i].pos.y;	// ���C�W�̕\���ʒuY
				float pw = enemy[i].w;		// ���C�W�̕\����
				float ph = enemy[i].h;		// ���C�W�̕\������

				float tw = 1.0f / TEXTURE_PATTERN_DIVIDE_X;	// �e�N�X�`���̕�
				float th = 1.0f / TEXTURE_PATTERN_DIVIDE_Y;	// �e�N�X�`���̍���
				float tx = tw * enemy[i].patternAnim;// �e�N�X�`���̍���X���W
				float ty = th * enemy[i].direction;// �e�N�X�`���̍���Y���W

				float r = enemy[i].color_R;
				float g = enemy[i].color_G;
				float b = enemy[i].color_B;

				float alpha = (enemy[i].invincible_time / 10) % 2 == 0 ? 1.0f : 0.3f;	//���G���Ԃ͓_��

				float e_rad = enemy[i].rad;

				SetSpriteColorRotation(g_VertexBuffer, px, py, pw, ph, tx, ty, tw, th, D3DXCOLOR(r,g,b,alpha), e_rad);

				// �|���S���`��
				GetDeviceContext()->Draw(4, 0);
			}
	}
	
}


//=============================================================================
// ���C�W�̍��W���擾
//=============================================================================
ENEMY *GetEnemy(void)
{
	return(&enemy[0]);
}
