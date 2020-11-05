//=============================================================================
//
// �v���C���[���� [player.cpp]
// Author : GP11B132 36 ���c�x��
//
//=============================================================================
#include "player.h"
#include "sprite.h"
#include "input.h"
#include "attack.h"
#include "time.h"
#include "score.h"
#include "fire.h"
#include "laser.h"
#include "sound.h"
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
	"data/TEXTURE/mage000.png",
};

PLAYER player[PLAYER_MAX];	//�v���C���[�̍\����

static float rad = 45 * pi / 180;	//�΂߈ړ��p�̐��l
static float g_cos = cosf(rad);
static float g_sin = sinf(rad);

//=============================================================================
// ����������
//=============================================================================
HRESULT InitPlayer(void)
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

	for (int i = 0; i < PLAYER_MAX; i++)
	{
		player[i].use = true;
		player[i].w = TEXTURE_WIDTH;
		player[i].h = TEXTURE_HEIGHT;
		player[i].pos = D3DXVECTOR3(SCREEN_WIDTH / 2, SCREEN_HEIGHT / 2, 0.0f);	//�����ʒu�͐^��
		player[i].rot = D3DXVECTOR3(0.0f, 0.0f, 0.0f);
		player[i].texNo = 0;

		player[i].countAnim = 0;
		player[i].patternAnim = 0;

		player[i].direction = DOWN;	//�����͉�������

		player[i].free = true;

		player[i].speed = 1.8f;

		player[i].flash_time = 0;
		player[i].laser_stock = 0;

		player[i].hp = PLAYER_HP_MAX;
	}
	
	VIEWPORT *viewport = GetViewports();

	//�r���[�|�[�g�̔{���A������W���v���C���[��ōX�V
	viewport->mag = 2.0f;	//������2�{
	viewport->topleft.x = SCREEN_CENTER_X - player[0].pos.x * viewport->mag;
	viewport->topleft.y = SCREEN_CENTER_Y - player[0].pos.y * viewport->mag;

	SetViewports();

	return S_OK;
}

//=============================================================================
// �I������
//=============================================================================
void UninitPlayer(void)
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
void UpdatePlayer(void)
{

	for (int i = 0; i < PLAYER_MAX; i++)
	{
		if (player[i].use == true)		// �v���C���[���g���Ă���H
		{						// Yes
			// �A�j���[�V����  
			player[i].countAnim++;
			if ((player[i].countAnim % ANIM_WAIT) == 0)
			{
				// �p�^�[���̐؂�ւ�
				player[i].patternAnim = (player[i].patternAnim + 1) % ANIM_PATTERN_NUM;
			}

		}
		
		player[i].prev_pos = player[i].pos;

		if (player[i].free == true)
		{	

			if (player[i].laser_stock > 0 && GetKeyboardTrigger(DIK_L))	//�X�^�b�N�������L�Ń��[�U�[����
			{
				SetLaser(player[i].pos,player[i].direction);
				player[i].laser_stock--;
				PlaySound(SOUND_LABEL_SE_laser2);
			}

			if (player[i].damage_time > 0)	//��e���o���Ԃ̍X�V
			{
				player[i].damage_time--;
			}

			if (player[i].flash_time == 0)	//�U���Ԋu����������
			{
				if (GetKeyboardTrigger(DIK_K))	//K�ōU��
				{
					SetAttack(player[i].pos,player[i].direction);
					player[i].flash_time = 30;	//30�t���[���̍U���Ԋu��t�^
					PlaySound(SOUND_LABEL_SE_attack);
				}
			}
			else
			{
				player[i].flash_time--;
			}
			

			//�����ꂽ�L�[�ɉ����Č�����ύX
			if (GetKeyboardPress(DIK_S))
			{
				player[i].direction = DOWN;
				if (GetKeyboardPress(DIK_D))
				{
					player[i].pos.x += g_cos * player[i].speed;
					player[i].pos.y += g_sin * player[i].speed;
				}
				else if (GetKeyboardPress(DIK_A))
				{
					player[i].pos.x -= g_cos * player[i].speed;
					player[i].pos.y += g_sin * player[i].speed;
				}
				else
				{
					player[i].pos.y += player[i].speed;
				}
			}
			else if (GetKeyboardPress(DIK_W))
			{
				player[i].direction = UP;
				if (GetKeyboardPress(DIK_D))
				{
					player[i].pos.x += g_cos * player[i].speed;
					player[i].pos.y -= g_sin * player[i].speed;
				}
				else if (GetKeyboardPress(DIK_A))
				{
					player[i].pos.x -= g_cos * player[i].speed;
					player[i].pos.y -= g_sin * player[i].speed;
				}
				else
				{
					player[i].pos.y -= player[i].speed;
				}
			}
			else if (GetKeyboardPress(DIK_D))
			{
				player[i].direction = RIGHT;
				player[i].pos.x += player[i].speed;
			}
			else if (GetKeyboardPress(DIK_A))
			{
				player[i].direction = LEFT;
				player[i].pos.x -= player[i].speed;
			}

			//�r���[�|�[�g�̔{���A������W���v���C���[��ōX�V

			VIEWPORT *viewport = GetViewports();

			if (GetKeyboardPress(DIK_I) && viewport->mag < 3.0f)
			{
				viewport->mag += 0.02f;
			}
			else if (GetKeyboardPress(DIK_O) && viewport->mag > 1.0f)
			{
				viewport->mag -= 0.02f;

				if (viewport->mag < 1.0f)
				{
					viewport->mag = 1.0f;
				}
			}

			viewport->topleft.x = SCREEN_CENTER_X - player[i].pos.x * viewport->mag;
			viewport->topleft.y = SCREEN_CENTER_Y - player[i].pos.y * viewport->mag;

			if (viewport->topleft.x > 0)
			{
				viewport->topleft.x = 0.0f;
			}
			else if (viewport->topleft.x < SCREEN_WIDTH * (1 - viewport->mag))
			{
				viewport->topleft.x = SCREEN_WIDTH * (1 - viewport->mag);
			}

			if (viewport->topleft.y > 0)
			{
				viewport->topleft.y = 0.0f;
			}
			else if (viewport->topleft.y < SCREEN_HEIGHT * (1 - viewport->mag))
			{
				viewport->topleft.y = SCREEN_HEIGHT * (1 - viewport->mag);
			}

		}

	}


#ifdef _DEBUG	// �f�o�b�O����\������
	char *str = GetDebugStr();
	VIEWPORT *viewport = GetViewports();
	float mag = viewport->mag;

	sprintf(&str[strlen(str)], " PX:%.2f PY:%.2f MAG:%d", player[0].pos.x, player[0].pos.y, player[0].damage_time);
	
#endif

}

//=============================================================================
// �`�揈��
//=============================================================================
void DrawPlayer(void)
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

	for (int i = 0; i < PLAYER_MAX; i++)
	{
		if (player[i].use == true)		// �v���C���[���g���Ă���H
			{						// Yes
				// �e�N�X�`���ݒ�
				GetDeviceContext()->PSSetShaderResources(0, 1, &g_Texture[player[i].texNo]);

				// �v���C���[�̈ʒu��e�N�X�`���[���W�𔽉f
				float px = player[i].pos.x;	// �v���C���[�̕\���ʒuX
				float py = player[i].pos.y;	// �v���C���[�̕\���ʒuY
				float pw = player[i].w;		// �v���C���[�̕\����
				float ph = player[i].h;		// �v���C���[�̕\������

				float tw = 1.0f / TEXTURE_PATTERN_DIVIDE_X;	// �e�N�X�`���̕�
				float th = 1.0f / TEXTURE_PATTERN_DIVIDE_Y;	// �e�N�X�`���̍���
				float tx = tw* player[i].patternAnim;	// �e�N�X�`���̍���X���W
				float ty = th*player[i].direction;	// �e�N�X�`���̍���Y���W

				float r = 1.0f;
				float g = player[i].damage_time > 0 ? 0.2f : 1.0f;	//��e���o���͐Ԃ��Ȃ�
				float b = player[i].damage_time > 0 ? 0.2f : 1.0f;

				float alpha = (player[i].damage_time / 10) % 2 == 0 ? 1.0f : 0.3f;	//��e���o���͓_��

				// �P���̃|���S���̒��_�ƃe�N�X�`�����W��ݒ�
				SetSpriteColor(g_VertexBuffer, px, py, pw, ph, tx, ty, tw, th,D3DXCOLOR(r,g,b,alpha),true);

				// �|���S���`��
				GetDeviceContext()->Draw(4, 0);
			}
	}
	
}


//=============================================================================
// �v���C���[�̍��W���擾
//=============================================================================
PLAYER *GetPlayer(void)
{
	return(player);
}




