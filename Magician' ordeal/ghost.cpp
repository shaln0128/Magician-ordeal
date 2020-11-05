//=============================================================================
//
// �S�[�X�g���� [ghost.cpp]
// Author : GP11B132 36 ���c�x��
//
//=============================================================================
#include "sprite.h"
#include "ghost.h"
#include "summon.h"
#include "sound.h"
//*****************************************************************************
// �}�N����`
//*****************************************************************************
#define TEXTURE_WIDTH				(15)	// �L�����T�C�Y
#define TEXTURE_HEIGHT				(30)	// 
#define TEXTURE_MAX					(3)		// �e�N�X�`���̐�

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
	"data/TEXTURE/ghost.png",
	"data/TEXTURE/ghost_red.png",
	"data/TEXTURE/ghost_blue.png",
};

ENEMY ghost[GHOST_MAX];	//�S�[�X�g�̍\����

static float rad = pi / 180;	//���W�A���ϊ��p
//=============================================================================
// ����������
//=============================================================================
HRESULT InitGhost(void)
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


	// �S�[�X�g�̏�����

	for (int i = 0; i < GHOST_MAX; i++)
	{
		ghost[i].use = false;
		ghost[i].w = TEXTURE_WIDTH;
		ghost[i].h = TEXTURE_HEIGHT;
		ghost[i].pos = D3DXVECTOR3(SCREEN_WIDTH * 0.625f, SCREEN_HEIGHT / 2.0f, 0.0f);
		ghost[i].rot = D3DXVECTOR3(0.0f, 0.0f, 0.0f);
		ghost[i].texNo = 0;

		ghost[i].countAnim = 0;
		ghost[i].patternAnim = 0;

		ghost[i].direction = DOWN;	//���������͉�

		ghost[i].color_R = 1.0f;
		ghost[i].color_G = 1.0f;
		ghost[i].color_B = 1.0f;

		ghost[i].rad = 0.0f;
		ghost[i].rotation = 0.0f;

		ghost[i].hp = GHOST_NORMAL_HP_MAX;

		ghost[i].invincible_time = 60;	//��������1�b���G
		ghost[i].chase = false;

	}

	return S_OK;
}

//=============================================================================
// �I������
//=============================================================================
void UninitGhost(void)
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
void UpdateGhost(void)
{
	PLAYER *player = GetPlayer();
	int ghost_count = 0;

	for (int i = 0; i < GHOST_MAX; i++)
	{
		if (ghost[i].use == true)		// �S�[�X�g���g���Ă���H
		{						// Yes
			// �A�j���[�V����  
			ghost[i].countAnim++;
			if ((ghost[i].countAnim % ANIM_WAIT) == 0)
			{
				// �p�^�[���̐؂�ւ�
				ghost[i].patternAnim = (ghost[i].patternAnim + 1) % ANIM_PATTERN_NUM;
			}

			if (ghost[i].invincible_time > 0)
			{
				ghost[i].invincible_time--;
			}
			else
			{
				if (ghost[i].chase == true)	//�v���C���[��ǔ�
				{
					D3DXVECTOR3 b_dif = player[0].pos - ghost[i].pos; //�s�������ꏊ�@�|�@����ꏊ�@���@����
					float angle = atan2f(b_dif.y, b_dif.x);//���̊p�x

					ghost[i].move.x = cosf(angle) * ghost[i].speed;
					ghost[i].move.y = sinf(angle) * ghost[i].speed;
				}
				else if (GetMode() == MODE_GAME)	//�ǔ����łȂ��āA�Q�[�����[�h�̏ꍇ
				{
					ghost[i].time--;

					if (ghost[i].time == 0)	//�ړ����Ԃ��؂ꂽ��ēx�p�x�E���Ԃ𗐐��Ő���
					{
						float angle = (rand() % 360) * rad;
						ghost[i].move = D3DXVECTOR3(cosf(angle) * ghost[i].speed, sinf(angle) * ghost[i].speed, 0.0f);
						ghost[i].time = (rand() % 4 + 2) * 60;//(0�`3) + 2�b
					}
				}

				ghost[i].pos += ghost[i].move;

				float move_x = ghost[i].move.x;	//���̈ړ��ʂ�ۑ�
				float abs_x = ABS_FUNC(move_x);	//��Βl��
				float move_y = ghost[i].move.y;	//���̈ړ��ʂ�ۑ�
				float abs_y = ABS_FUNC(move_y);	//��Βl��

				if (abs_x > abs_y)
				{
					ghost[i].direction = move_x > 0 ? RIGHT : LEFT;	//�ړ��ʂ����Ȃ�E�A���Ȃ獶
				}
				else
				{
					ghost[i].direction = move_y > 0 ? DOWN : UP;	//�ړ��ʂ����Ȃ牺�A���Ȃ��
				}
			}	
		}
	}

}

//=============================================================================
// �`�揈��
//=============================================================================
void DrawGhost(void)
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

	for (int i = 0; i < GHOST_MAX; i++)
	{

		if (ghost[i].use == true)		// �S�[�X�g���g���Ă���H
		{						// Yes
			// �e�N�X�`���ݒ�
			GetDeviceContext()->PSSetShaderResources(0, 1, &g_Texture[ghost[i].texNo]);

			// �v���C���[�̈ʒu��e�N�X�`���[���W�𔽉f
			float px = ghost[i].pos.x;	// �S�[�X�g�̕\���ʒuX
			float py = ghost[i].pos.y;	// �S�[�X�g�̕\���ʒuY
			float pw = ghost[i].w;		// �S�[�X�g�̕\����
			float ph = ghost[i].h;		// �S�[�X�g�̕\������

			float tw = 1.0f / TEXTURE_PATTERN_DIVIDE_X;	// �e�N�X�`���̕�
			float th = 1.0f / TEXTURE_PATTERN_DIVIDE_Y;	// �e�N�X�`���̍���
			float tx = tw * ghost[i].patternAnim;// �e�N�X�`���̍���X���W
			float ty = th * ghost[i].direction;// �e�N�X�`���̍���Y���W

			float r = ghost[i].color_R;
			float g = ghost[i].color_G;
			float b = ghost[i].color_B;

			float alpha = 1.0f - ghost[i].invincible_time / GHOST_INVINCIBLE_TIME;	//���G���Ԃ͏��X�ɕs�����x���グ�Ă���

			float e_rad = ghost[i].rad;

			SetSpriteColorRotation(g_VertexBuffer, px, py, pw, ph, tx, ty, tw, th, D3DXCOLOR(r, g, b, alpha), e_rad);

			// �|���S���`��
			GetDeviceContext()->Draw(4, 0);
		}
	}

}



//=============================================================================
// �S�[�X�g�̍��W���擾
//=============================================================================
ENEMY *GetGhost(void)
{
	return(&ghost[0]);
}

void SetGhost(int normal,int rare)	//�S�[�X�g�̐����@�m�[�}���A���A�̌����w��
{
	int count = 0;

	PlaySound(SOUND_LABEL_SE_ghost);	//�������ɉ���炷

	for (int i = 0; i < GHOST_MAX; i++)
	{
		if (count == normal + rare)return;	//������������ɒB������
		if (ghost[i].use == false)
		{
			if (count < normal)
			{
				ghost[i].speed = GHOST_NORMAL_SPEED;
				ghost[i].hp = GHOST_NORMAL_HP_MAX;
				ghost[i].texNo = NORMAL;
			}
			else
			{
				ghost[i].speed = GHOST_RARE_SPEED;
				ghost[i].hp = GHOST_RARE_HP_MAX;
				ghost[i].texNo = rand() % 2 == 0 ? RED : BLUE;	//�Ԃ����͗���
			}

			//�������Ɠ���
			ghost[i].use = true;
			ghost[i].chase = false;
			ghost[i].direction = DOWN;
			float angle = (rand() % 360) * rad;
			ghost[i].pos = D3DXVECTOR3(float(rand() % 680 + 140), float(rand() % 275 + 95), 0.0f);
			ghost[i].move = D3DXVECTOR3(cosf(angle) * ghost[i].speed, sinf(angle) * ghost[i].speed, 0.0f);
			ghost[i].time = (rand() % 4 + 2) * 60;
			ghost[i].invincible_time = GHOST_INVINCIBLE_TIME;
			SetSummon(ghost[i].pos, ghost[i].texNo);	//�����w�̐���
			count++;
		}	
	}
}
