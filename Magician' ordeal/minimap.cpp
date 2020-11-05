//=============================================================================
//
// �~�j�}�b�v�̏���[minimap.cpp]
// Author : GP11B132 36 ���c�x��
//
//=============================================================================
#include "sprite.h"
#include "input.h"
#include "minimap.h"
#include "player.h"
#include "enemy.h"
#include "ghost.h"
#include "sound.h"
//*****************************************************************************
// �}�N����`
//*****************************************************************************
#define TEXTURE_WIDTH				(245)	// �L�����T�C�Y
#define TEXTURE_HEIGHT				(135)		// 
#define TEXTURE_SIZE				(6)			// 

#define MINIMAP_X					(115)	//�\���ʒu
#define MINIMAP_Y					(480)
#define TEXTURE_MAX					(2)				// �e�N�X�`���̐�



//*****************************************************************************
// �v���g�^�C�v�錾
//*****************************************************************************

//*****************************************************************************
// �O���[�o���ϐ�
//*****************************************************************************
static ID3D11Buffer				*g_VertexBuffer = NULL;		// ���_���
static ID3D11ShaderResourceView	*g_Texture[TEXTURE_MAX] = { NULL };	// �e�N�X�`�����

static char *g_TexturName[] = {
	"data/TEXTURE/minimap.png",
	"data/TEXTURE/chip.png",
};

//�~�j�}�b�v�̕��A�ʒu�A������W
static float minimap_w;
static float minimap_h;
static D3DXVECTOR3 minimap_pos;
static bool minimap_use;
static D3DXVECTOR3 minimap_topleft;
static float chip_size;	//�~�j�}�b�v��̃`�b�v�̑傫��
//=============================================================================
// ����������
//=============================================================================
HRESULT InitMiniMap(void)
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

	// �ʒu�╝�����r���[�|�[�g�����Ƃɐݒ�
	SetMiniMapPos();
	minimap_use = true;

	return S_OK;
}

//=============================================================================
// �I������
//=============================================================================
void UninitMiniMap(void)
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
void UpdateMiniMap(void)
{
	SetMiniMapPos();

	if (GetKeyboardTrigger(DIK_M))	//M�L�[�ŕ\��ON/OFF
	{
		minimap_use = minimap_use == true ? false : true;
		PlaySound(SOUND_LABEL_SE_switch);
	}
	
#ifdef _DEBUG	// �f�o�b�O����\������
	//char *str = GetDebugStr();
	//sprintf(&str[strlen(str)], " PX:%.2f PY:%.2f", g_Pos.x, g_Pos.y);

#endif

}

//=============================================================================
// �`�揈��
//=============================================================================
void DrawMiniMap(void)
{
	PLAYER *player = GetPlayer();
	ENEMY *enemy = GetEnemy();
	ENEMY *ghost = GetGhost();
	VIEWPORT *viewport = GetViewports();
	float mag = viewport->mag;

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

	//�e�L�X�g�{�b�N�X�̕`��
	if (minimap_use == true)
	{
		// �e�N�X�`���ݒ�
		GetDeviceContext()->PSSetShaderResources(0, 1, &g_Texture[0]);

		// �X�R�A�̈ʒu��e�N�X�`���[���W�𔽉f
		float px = minimap_pos.x;		// �~�j�}�b�v�̕\���ʒuX
		float py = minimap_pos.y;		// �~�j�}�b�v�̕\���ʒuY
		float pw = minimap_w;			// �~�j�}�b�v�̕\����
		float ph = minimap_h;			// �~�j�}�b�v�̕\������

		float tw = 1.0f;		// �e�N�X�`���̕�
		float th = 1.0f;		// �e�N�X�`���̍���
		float tx = 0.0f;		// �e�N�X�`���̍���X���W
		float ty = 0.0f;		// �e�N�X�`���̍���Y���W

		// �P���̃|���S���̒��_�ƃe�N�X�`�����W��ݒ�
		SetSpriteColor(g_VertexBuffer, px, py, pw, ph, tx, ty, tw, th, D3DXCOLOR(1.0f, 1.0f, 1.0f, 0.5f), true);

		// �|���S���`��
		GetDeviceContext()->Draw(4, 0);

		// �e�N�X�`���ݒ�
		GetDeviceContext()->PSSetShaderResources(0, 1, &g_Texture[1]);

		float size = chip_size;	//�`�b�v�̑傫��

		for (int i = 0; i < PLAYER_MAX; i++)	//�v���C���[���̃`�b�v�̕\��
		{
			if (player[i].use == true)
			{
				// �X�R�A�̈ʒu��e�N�X�`���[���W�𔽉f
				px = minimap_topleft.x + player[i].pos.x / (mag * 5.0f);		// �v���C���[�̕\���ʒuX
				py = minimap_topleft.y + player[i].pos.y / (mag * 5.0f);		// �v���C���[�̕\���ʒuY

				// �P���̃|���S���̒��_�ƃe�N�X�`�����W��ݒ�
				SetSpriteColor(g_VertexBuffer, px, py, size * 2.0f, size * 2.0f, tx, ty, tw, th, D3DXCOLOR(0.2f, 1.0f, 0.2f, 0.7f), true);

				GetDeviceContext()->Draw(4, 0);
			}
		}

		for (int i = 0; i < ENEMY_MAX; i++)	//���C�W���̃`�b�v�̕\��
		{
			if (enemy[i].use == true)
			{
				// �X�R�A�̈ʒu��e�N�X�`���[���W�𔽉f
				px = minimap_topleft.x + enemy[i].pos.x / (mag * 5.0f);		// ���C�W�̕\���ʒuX
				py = minimap_topleft.y + enemy[i].pos.y / (mag * 5.0f);		// ���C�W�̕\���ʒuY

				// �P���̃|���S���̒��_�ƃe�N�X�`�����W��ݒ�
				SetSpriteColor(g_VertexBuffer, px, py, size * 2.0f, size * 2.0f, tx, ty, tw, th, D3DXCOLOR(1.0f, 0.2f, 1.0f, 0.7f), true);

				GetDeviceContext()->Draw(4, 0);
			}
		}

		for (int i = 0; i < GHOST_MAX; i++)	//�S�[�X�g���̃`�b�v�̕\��
		{
			if (ghost[i].use == true)
			{
				// �X�R�A�̈ʒu��e�N�X�`���[���W�𔽉f
				px = minimap_topleft.x + ghost[i].pos.x / (mag * 5.0f);		// �S�[�X�g�̕\���ʒuX
				py = minimap_topleft.y + ghost[i].pos.y / (mag * 5.0f);		// �S�[�X�g�̕\���ʒuY

				D3DXCOLOR color;

				switch (ghost[i].texNo)	//��ނɉ����ĐF�𕪂���
				{
				case NORMAL:
					color = D3DXCOLOR(0.5f, 0.5f, 0.5f, 0.7f);
					break;
				case RED:
					color = D3DXCOLOR(1.0f, 0.0f, 0.0f, 0.7f);
					break;
				case BLUE:
					color = D3DXCOLOR(0.0f, 0.0f, 1.0f, 0.7f);
					break;
				}
				// �P���̃|���S���̒��_�ƃe�N�X�`�����W��ݒ�
				SetSpriteColor(g_VertexBuffer, px, py, size, size, tx, ty, tw, th, color, true);

				GetDeviceContext()->Draw(4, 0);
			}
		}
	}

}

void SetMiniMap(bool flag)	//�~�j�}�b�v��ON/OFF�p
{
	minimap_use = flag;
	return;
}


void SetMiniMapPos(void)	//�r���[�|�[�g�ɉ����Ĉʒu�Ȃǂ�ύX
{
	VIEWPORT *viewport = GetViewports();
	float mag = viewport->mag;	//�{��
	float tl_x = viewport->topleft.x;	//������W
	float tl_y = viewport->topleft.y;

	minimap_w = TEXTURE_WIDTH / mag;
	minimap_h = TEXTURE_HEIGHT / mag;
	minimap_pos.x = -tl_x / mag + MINIMAP_X / mag;
	minimap_pos.y = -tl_y / mag + MINIMAP_Y / mag;
	minimap_topleft = minimap_pos - D3DXVECTOR3(minimap_w * 0.5f, minimap_h * 0.5f, 0.0f);
	chip_size = TEXTURE_SIZE / mag;
}

