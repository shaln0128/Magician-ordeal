//=============================================================================
//
// ���e�I�����ymeteor.cpp�z
// Author : GP11B132 36 ���c�x��
//
//=============================================================================
#include "sprite.h"
#include "input.h"
#include "bg.h"
#include "fade.h"
#include "meteor.h"
#include "player.h"
#include "effect.h"
#include "game.h"
#include "score.h"
#include "tutorialflag.h"
#include "tutorial.h"
#include "sound.h"
//*****************************************************************************
// �}�N����`
//*****************************************************************************
#define TEXTURE_WIDTH				(40)	// �L�����T�C�Y
#define TEXTURE_HEIGHT				(40)	// �L�����T�C�Y
#define TEXTURE_SIZE				(20)
#define TEXTURE_MAX					(3)		// �e�N�X�`���̐�

#define PARTICLE_MAX				(120)	//�p�[�e�B�N���̍ő�l

#define METEOR_SPEED				(2.0f)

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
	"data/TEXTURE/target.png",
	"data/TEXTURE/meteor.png",
};

static METEOR meteor[METEOR_MAX];					//���e�I�̍\����
static PARTICLE meteor_particle[PARTICLE_MAX];		//�p�[�e�B�N���̍\����
static METEOR landing_point[METEOR_MAX];			//���n�_�̍\����
static float rad = pi / 180.0f;						//���W�A���ϊ��p
static int meteor_count;							//���e�I�̐�����
static D3DXVECTOR3 *g_Pos;							//���e�I���Q�Ƃ���ʒu�ւ̃|�C���^
//=============================================================================
// ����������
//=============================================================================
HRESULT InitMeteor(void)
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

	// ���e�I�̏�����
	for (int i = 0; i < METEOR_MAX; i++)
	{
		meteor[i].use = false;
		meteor[i].w = TEXTURE_WIDTH;
		meteor[i].h = TEXTURE_HEIGHT;
		meteor[i].pos = D3DXVECTOR3(0.0f, 0.0f, 0.0f);
		meteor[i].move = D3DXVECTOR3(0.0f, 0.0f, 0.0f);
		meteor[i].rot = D3DXVECTOR3(0.0f, 0.0f, 0.0f);
		meteor[i].time = 0;
		meteor[i].arrival_time = 0;
		meteor[i].color = D3DXCOLOR(1.0f, 1.0f, 1.0f, 1.0f);
		meteor[i].state = DIFFUSION;

		
	}

	//�@�p�[�e�B�N���̏�����
	for (int i = 0; i < PARTICLE_MAX; i++)
	{
		meteor_particle[i].use = false;
		meteor_particle[i].w = TEXTURE_WIDTH;
		meteor_particle[i].h = TEXTURE_HEIGHT;
		meteor_particle[i].move = D3DXVECTOR3(0.0f, 0.2f, 0.0f);
		meteor_particle[i].time = 0;
		meteor_particle[i].color = D3DXCOLOR(1.0f, 1.0f, 1.0f, 1.0f);
	}

	g_Pos = NULL;	//�Q�Ƃ���ʒu�ւ̃|�C���^

	return S_OK;
}

//=============================================================================
// �I������
//=============================================================================
void UninitMeteor(void)
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
void UpdateMeteor(void)
{
	if (meteor_count > 0)
	{
		for (int i = 0; i < meteor_count; i++)
		{
			if (meteor[i].use == false) continue;

			PLAYER *player = GetPlayer();
			switch (meteor[i].state)	//���݂̏�Ԃɉ����ď����𕪂���
			{
			case DIFFUSION:	//�g�U���
				if (meteor[i].time == 60)	//���̏�Ԃ�
				{
					meteor[i].time = 0;
					meteor[i].state = CIRCLING;
				}
				else
				{
					meteor[i].time++;
					meteor[i].pos = *g_Pos + meteor[i].move * float(meteor[i].time);	//��_���痣��Ă���
				}
				break;
			case CIRCLING:	//��]���
				if (meteor[i].time == 150 + (120 / meteor_count)*i)	//��_�̓���ɗ����玟�̏�Ԃֈڍs
				{
					PlaySound(SOUND_LABEL_SE_throwing);
					D3DXVECTOR3 target = player[0].pos + D3DXVECTOR3(float(rand() % 200 - 100), float(rand() % 200 - 100),0.0f);	//�v���C���[�t�߂߂����Ĕ���
					D3DXVECTOR3 dif = target - meteor[i].pos;
					float rot = atan2f(dif.y, dif.x);
					D3DXVECTOR2 temp = D3DXVECTOR2(dif.x, dif.y);
					float distance = D3DXVec2Length(&temp);

					meteor[i].landingpoint = target;	//���e�n�_
					meteor[i].arrival_time = int(distance / METEOR_SPEED);	//�����\�莞��
					meteor[i].move = D3DXVECTOR3(cosf(rot) * METEOR_SPEED, sinf(rot) * METEOR_SPEED, 0.0f);	//�ړ���
					meteor[i].time = 0;
					meteor[i].state = FALLING;
					SetEffect(meteor[i].pos, SHOT);	//���ˎ��̉���炷
				}
				else
				{
					meteor[i].time++;
					meteor[i].rot.y -= rad * 3.0f;	//��_�ɑ΂���ʒu�̊p�x�̉�]
					meteor[i].rot.z += rad * 5.0f;	//�e�N�X�`���̉�]
					meteor[i].pos = *g_Pos + D3DXVECTOR3(cosf(meteor[i].rot.y) * 60.0f,sinf(meteor[i].rot.y) * 60.0f,0.0f);	//��_�����]���Ă���
				}
				break;
			case FALLING:
				if (meteor[i].time == meteor[i].arrival_time)
				{
					meteor[i].use = false;
					SetEffect(meteor[i].pos, HIT);
					PlaySound(SOUND_LABEL_SE_hitmeteor);
					bool ans = HitCheckBC(meteor[i].pos, player[0].pos, 20.0f, 8.0f);
					if (ans == true)
					{
						PlaySound(SOUND_LABEL_SE_hitenemyattack);
						player[0].damage_time = 60;
						AddScore(-300,HITMINUS);
						if (GetTutorialState() == MAGE_SKILL)
						{
							SetFailure();
						}
					}
				}
				else
				{
					if (meteor[i].time % 4 == 0)
					{
						SetMeteorParticle(meteor[i].pos - D3DXVECTOR3(0.0f, sinf(180.0f * rad * (meteor[i].time / float(meteor[i].arrival_time))) * 60.0f, 0.0f), meteor[i].color);
					}
					meteor[i].time++;
					meteor[i].rot.z += rad * 5.0f;
					meteor[i].pos += meteor[i].move;
				}
				break;
			}
		}
	}

	for (int i = 0; i < PARTICLE_MAX; i++)	//�p�[�e�B�N���̏���
	{
		if (meteor_particle[i].use == true)
		{
			if (meteor_particle[i].time == 40)	//�������Ԃ�����ɒB���������
			{
				meteor_particle[i].use = false;
			}
			else
			{
				meteor_particle[i].time++;
				meteor_particle[i].pos += meteor_particle[i].move;
				meteor_particle[i].w *= 0.98f;	//�T�C�Y��2���������Ă���
				meteor_particle[i].h *= 0.98f;
			}
		}
	}
#ifdef _DEBUG	// �f�o�b�O����\������
	char *str = GetDebugStr();
	sprintf(&str[strlen(str)], " fire%d", meteor_count);
#endif

}

//=============================================================================
// �`�揈��
//=============================================================================
void DrawMeteor(void)
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

	for (int i = 0; i < METEOR_MAX; i++)
	{
		if (meteor[i].use == true)
		{
			GetDeviceContext()->PSSetShaderResources(0, 1, &g_Texture[0]);
			// �P���̃|���S���̒��_�ƃe�N�X�`�����W��ݒ�
			float dx = meteor[i].pos.x;
			float dy = meteor[i].pos.y;
			float sy = meteor[i].state == FALLING ? sinf(180.0f * rad * (meteor[i].time / float(meteor[i].arrival_time))) * 60.0f: 0.0f;	//�~�����̓T�C���J�[�u�̒l��^���A�ʂ�`������
			float dw = meteor[i].w;
			float dh = meteor[i].h;
			D3DXCOLOR color = meteor[i].color;
			float rot = meteor[i].rot.z;

			SetSpriteColorRotation(g_VertexBuffer, dx, dy - sy, dw * 1.2f, dh * 1.2f, 0.0f, 0.0f, 1.0f, 1.0f,
				color, rot);

			// �܂��͉~��`��
			GetDeviceContext()->Draw(4, 0);

			GetDeviceContext()->PSSetShaderResources(0, 1, &g_Texture[2]);

			//3�̑ȉ~��`��
			SetSpriteColorRotation(g_VertexBuffer, dx, dy - sy, dw, dh, 0.0f, 0.0f, 1.0f, 1.0f,
				color, rot);

			GetDeviceContext()->Draw(4, 0);

			rot += 60.0f * rad;

			SetSpriteColorRotation(g_VertexBuffer, dx, dy - sy, dw, dh, 0.0f, 0.0f, 1.0f, 1.0f,
				color, rot);

			GetDeviceContext()->Draw(4, 0);

			rot += 60.0f * rad;

			SetSpriteColorRotation(g_VertexBuffer, dx, dy - sy, dw, dh, 0.0f, 0.0f, 1.0f, 1.0f,
				color, rot);

			GetDeviceContext()->Draw(4, 0);

			if (meteor[i].state == FALLING)	//�~�����͒��e�n�_�ɃT�[�N����`��
			{
				GetDeviceContext()->PSSetShaderResources(0, 1, &g_Texture[1]);

				dx = meteor[i].landingpoint.x;
				dy = meteor[i].landingpoint.y;
				float size = 60.0f - (TEXTURE_SIZE * meteor[i].time / float(meteor[i].arrival_time));

				SetSpriteColor(g_VertexBuffer, dx, dy, size, size, 0.0f, 0.0f, 1.0f, 1.0f,
					color, true);

				GetDeviceContext()->Draw(4, 0);
			}
		
		}
	}

	GetDeviceContext()->PSSetShaderResources(0, 1, &g_Texture[0]);

	for (int i = 0; i < PARTICLE_MAX; i++)	//�p�[�e�B�N���̕`��
	{
		if (meteor_particle[i].use == true)
		{
			float dx = meteor_particle[i].pos.x;
			float dy = meteor_particle[i].pos.y;
			float dw = meteor_particle[i].w;
			float dh = meteor_particle[i].h;
			D3DXCOLOR color = meteor_particle[i].color;

			SetSpriteColor(g_VertexBuffer, dx, dy, dw, dh, 0.0f, 0.0f, 1.0f, 1.0f,
				color, true);

			// �|���S���`��
			GetDeviceContext()->Draw(4, 0);
		}
	}

	SetBlendState(BLEND_MODE_ALPHABLEND);

}

void SetMeteor(D3DXVECTOR3 *pos, int num)	//���e�I�̃Z�b�g
{
	PlaySound(SOUND_LABEL_SE_meteorcharge);	//�r���̉���炷

	g_Pos = pos;	//���C�W�̈ʒu�ւ̃|�C���^���i�[
	meteor_count = num;	//�������郁�e�I�̌����i�[

	float angle = 360.0f * rad / num;

	for (int i = 0; i < num; i++)
	{
		meteor[i].use = true;
		meteor[i].w = TEXTURE_WIDTH;
		meteor[i].h = TEXTURE_HEIGHT;
		meteor[i].pos = *pos;
		meteor[i].move = D3DXVECTOR3(cosf(angle*i), sinf(angle*i), 0.0f);	//������������A�~�𕪊�
		meteor[i].rot = D3DXVECTOR3(0.0f, angle*i, 0.0f);
		meteor[i].time = 0;
		meteor[i].arrival_time = 0;
		meteor[i].color = D3DXCOLOR(0.8f, 0.2f, 0.8f, 1.0f);
		meteor[i].state = DIFFUSION;	//�g�U��Ԃ���X�^�[�g
	}

	return;
}

void SetMeteorParticle(D3DXVECTOR3 pos, D3DXCOLOR color)	//�p�[�e�B�N���̃Z�b�g
{
	for (int i = 0; i < PARTICLE_MAX; i++)
	{
		if (meteor_particle[i].use == false)
		{
			meteor_particle[i].use = true;
			meteor_particle[i].pos = pos;
			meteor_particle[i].w = TEXTURE_WIDTH;
			meteor_particle[i].h = TEXTURE_HEIGHT;
			meteor_particle[i].color = color;
			meteor_particle[i].time = 0;

			return;
		}
	}
	return;
}

METEOR *GetMeteor(void)	//���e�I�̍\���̂ւ̃A�h���X
{
	return(&meteor[0]);
}