//=============================================================================
//
// �U������ [attack.cpp]
// Author : GP11B132 36 ���c�x��
//
//=============================================================================
#include "attack.h"
#include "sprite.h"
#include "enemy.h"
#include "ghost.h"
//*****************************************************************************
// �}�N����`
//****************************************************************************
#define TEXTURE_SIZE				(20)	// �T�C�Y
#define TEXTURE_MAX					(1)		// �e�N�X�`���̐�

#define TEXTURE_PATTERN_DIVIDE_X	(1)		// �A�j���p�^�[���̃e�N�X�`�����������iX)
#define TEXTURE_PATTERN_DIVIDE_Y	(1)		// �A�j���p�^�[���̃e�N�X�`�����������iY)
#define ANIM_PATTERN_NUM			(TEXTURE_PATTERN_DIVIDE_X*TEXTURE_PATTERN_DIVIDE_Y)	// �A�j���[�V�����p�^�[����
#define ANIM_WAIT					(4)		// �A�j���[�V�����̐؂�ւ��Wait�l

#define pi							float(3.14159265)

#define EMITTER_ROTATION			(4) //�G�~�b�^�[�i��[�j�̉�]���x
#define STAR_ROTATION				(8)//�p�[�e�B�N���̉�]���x
//*****************************************************************************
// �v���g�^�C�v�錾
//*****************************************************************************


//*****************************************************************************
// �O���[�o���ϐ�
//*****************************************************************************
static ID3D11Buffer				*g_VertexBuffer = NULL;		// ���_���
static ID3D11ShaderResourceView	*g_Texture[TEXTURE_MAX] = { NULL };	// �e�N�X�`�����

static char *g_TexturName[] = {
	"data/TEXTURE/star.png",
};

static PARTICLE attack[ATTACK_MAX];						// �G�~�b�^�[�̍\����
static PARTICLE star[STAR_MAX];							// �p�[�e�B�N���̍\����

static float rad = pi / 180.0f; //�@���W�A���ϊ��p
//=============================================================================
// ����������
//=============================================================================
HRESULT InitAttack(void)
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


	// �G�~�b�^�[�\���̂̏�����
	for (int i = 0; i < ATTACK_MAX; i++)
	{
		attack[i].use = false;	// ���g�p�i���˂���Ă��Ȃ��e�j
	
		attack[i].size = TEXTURE_SIZE;
		attack[i].pos = D3DXVECTOR3(0, 0.0f, 0.0f);
		attack[i].rot = D3DXVECTOR3(0.0f, 0.0f, 0.0f);
		attack[i].move = D3DXVECTOR3(0.0f, 0.0f, 0.0f);	// �ړ��ʂ�������
		attack[i].color = D3DXCOLOR(1.0f, 0.8f, 0.1f,1.0f);	//�@���F
		attack[i].time = 0;
	}

	for (int i = 0; i < STAR_MAX; i++)
	{
		star[i].use = false;	// ���g�p�i���˂���Ă��Ȃ��e�j

		star[i].size = TEXTURE_SIZE;
		star[i].pos = D3DXVECTOR3(0, 0.0f, 0.0f);
		star[i].rot = D3DXVECTOR3(0.0f, 0.0f, 0.0f);
		star[i].move = D3DXVECTOR3(0.0f, 0.0f, 0.0f);	// �ړ��ʂ�������
		star[i].color = D3DXCOLOR(1.0f, 0.8f, 0.1f, 1.0f);	//���F
		star[i].time = 0;
	}	

	return S_OK;
}

//=============================================================================
// �I������
//=============================================================================
void UninitAttack(void)
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
void UpdateAttack(void)
{
	ENEMY *enemy = GetEnemy();
	ENEMY *ghost = GetGhost();

	for (int i = 0; i < ATTACK_MAX; i++)	//�G�~�b�^�[�̍X�V����
	{
		if (attack[i].use == true)	// ���̃G�~�b�^�[���g���Ă���H
		{	
			//�@��ԋ߂��G���\�[�g
			float dif;
			D3DXVECTOR3 b_dif;
			float min_dif = 1000000;
			D3DXVECTOR3 min_b_dif;

			for (int j = 0; j < GHOST_MAX; j++)
			{
				if (ghost[j].use == true && ghost[j].invincible_time == 0)
				{
					b_dif = ghost[j].pos - attack[i].pos;

					dif = b_dif.x * b_dif.x + b_dif.y * b_dif.y;

					if (dif < min_dif)	//��r���ċ������Z�����������ւ�
					{
						min_dif = dif;
						min_b_dif = b_dif;
					}
				}
			}

			if (min_dif == 1000000)	//�����S�[�X�g����̂����Ȃ���΃��C�W�ւƔ�΂�
			{
				min_b_dif = enemy[0].pos - attack[i].pos;
			}
			float rot = atan2f(min_b_dif.y, min_b_dif.x);

			attack[i].move.x = cosf(rot) * 3.0f;		//�ړ��ʂ�ۑ�
			attack[i].move.y = sinf(rot) * 3.0f;

			
			attack[i].pos += attack[i].move;			// �G�~�b�^�[�̈ړ�����

			attack[i].rot.z += EMITTER_ROTATION * rad;	//��]����

			attack[i].time++;							//���Ԃ̍X�V

			if (attack[i].time % 3 == 0)				//3F��1��A�p�[�e�B�N���𐶐�
			{
				for (int j = 0; j < STAR_MAX; j++)
				{
					if (star[j].use == true) continue;
					else
					{
						star[j].use = true;
						star[j].pos = attack[i].pos + D3DXVECTOR3(float(rand() % 10 - 5), float(rand() % 10 - 5), 0.0f);	//�G�~�b�^�[�̈ʒu���痐���ł��炷
						star[j].move = -attack[i].move * 0.2f;	//�G�~�b�^�[�̔��Α���0.2�{�̑����ňړ�������
						star[j].size = TEXTURE_SIZE;
						star[j].time = 60;
						break;
					}
				}
			}

		}

	}


	for (int j = 0; j < STAR_MAX; j++)	//�p�[�e�B�N���̍X�V����
	{
		if (star[j].use == false) continue;
		
		star[j].pos += star[j].move;
		star[j].size *= 0.95f;					//�傫����5���k��
		star[j].rot.z += STAR_ROTATION * rad;	//��]
		star[j].time--;							//��������

		if (star[j].time <= 0)					//0�ɂȂ��������
		{
			star[j].use = false;
		}
	}
}

//=============================================================================
// �`�揈��
//=============================================================================
void DrawAttack(void)
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

	for (int i = 0; i < ATTACK_MAX; i++)
	{
		if (attack[i].use == true)			// ���̃G�~�b�^�[���g���Ă���H
		{									// Yes
			//�G�~�b�^�[�̈ʒu��e�N�X�`���[���W�𔽉f
			float px = attack[i].pos.x;		// �G�~�b�^�[�̕\���ʒuX
			float py = attack[i].pos.y;		// �G�~�b�^�[�̕\���ʒuY
			float pw = attack[i].size;		// �G�~�b�^�[�̕\����
			float ph = attack[i].size;		// �G�~�b�^�[�̕\������

			D3DXCOLOR color = attack[i].color;
			float pb_rad = attack[i].rot.z;

			// �P���̃|���S���̒��_�ƃe�N�X�`�����W��ݒ�
			SetSpriteColorRotation(g_VertexBuffer,
				px, py, pw, ph,
				0.0f, 0.0f, 1.0f, 1.0f,
				color,pb_rad);

			// �|���S���`��
			GetDeviceContext()->Draw(4, 0);
		}
	}

	for (int i = 0; i < STAR_MAX; i++)
	{
		if (star[i].use == true)			// ���̃p�[�e�B�N�����g���Ă���H
		{									// Yes
			//�p�[�e�B�N���̈ʒu��e�N�X�`���[���W�𔽉f
			float px = star[i].pos.x;		// �p�[�e�B�N���̕\���ʒuX
			float py = star[i].pos.y;		// �p�[�e�B�N���̕\���ʒuY
			float pw = star[i].size;		// �p�[�e�B�N���̕\����
			float ph = star[i].size;		// �p�[�e�B�N���̕\������

			D3DXCOLOR color = star[i].color;
			float pb_rad = star[i].rot.z;

			// �P���̃|���S���̒��_�ƃe�N�X�`�����W��ݒ�
			SetSpriteColorRotation(g_VertexBuffer,
				px, py, pw, ph,
				0.0f, 0.0f, 1.0f, 1.0f,
				color, pb_rad);

			// �|���S���`��
			GetDeviceContext()->Draw(4, 0);
		}
	}

	SetBlendState(BLEND_MODE_ALPHABLEND);

}


//=============================================================================
// �G�~�b�^�[�\���̂̐擪�A�h���X���擾
//=============================================================================
PARTICLE *GetAttack(void)
{
	return &attack[0];
}

//=============================================================================
// �G�~�b�^�[�̔��ːݒ�
//=============================================================================
void SetAttack(D3DXVECTOR3 pos,DIRECTION vect)
{
	// �������g�p�̒e�����������甭�˂��Ȃ�( =����ȏ㌂�ĂȂ����Ď� )
	for (int i = 0; i < ATTACK_MAX; i++)
	{
		if (attack[i].use == false)		// ���g�p��Ԃ̃G�~�b�^�[��������
		{
			attack[i].use = true;			// �g�p��Ԃ֕ύX����
			attack[i].pos = pos;			// ���W���Z�b�g
			attack[i].time = 0;

			switch (vect)
			{
			case DOWN:
				attack[i].pos += D3DXVECTOR3(0.0f, 30.0f, 0.0f);
				break;
			case LEFT:
				attack[i].pos += D3DXVECTOR3(-20.0f, 0.0f, 0.0f);
				break;
			case RIGHT:
				attack[i].pos += D3DXVECTOR3(20.0f, 0.0f, 0.0f);
				break;
			case UP:
				attack[i].pos += D3DXVECTOR3(0.0f, -30.0f, 0.0f);
				break;
			}

			return;							// 1���Z�b�g�����̂ŏI������
		}
	}

	return;
}


