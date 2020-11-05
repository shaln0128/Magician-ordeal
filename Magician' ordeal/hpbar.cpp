//=============================================================================
//
// HP�o�[���� [hpbar.cpp]
// Author : GP11B132 36 ���c�x��
//
//=============================================================================
#include "player.h"
#include "sprite.h"
#include "enemy.h"
#include "hpbar.h"
#include "ghost.h"

//*****************************************************************************
// �}�N����`
//***************************************************************************** 
#define TEXTURE_MAX					(2)		// �e�N�X�`���̐�



//*****************************************************************************
// �v���g�^�C�v�錾
//*****************************************************************************


//*****************************************************************************
// �O���[�o���ϐ�
//*****************************************************************************
static ID3D11Buffer				*g_VertexBuffer = NULL;		// ���_���
static ID3D11ShaderResourceView	*g_Texture[TEXTURE_MAX] = { NULL };	// �e�N�X�`�����

static char *g_TexturName[] = {
	"data/TEXTURE/hpp.png",
	"data/TEXTURE/red.png",
};


//=============================================================================
// ����������
//=============================================================================
HRESULT InitHpbar(void)
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
	
	return S_OK;
}

//=============================================================================
// �I������
//=============================================================================
void UninitHpbar(void)
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
void UpdateHpbar(void)
{

}

//=============================================================================
// �`�揈��
//=============================================================================
void DrawHpbar(void)
{
	ENEMY *ghost = GetGhost();
	
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
		if (ghost[i].use == true)	// ���̃S�[�X�g���g���Ă���H
		{								// Yes
			// �e�N�X�`���ݒ�	�ǔ���Ԃ̓G��HP�o�[��Ԃ�
			GetDeviceContext()->PSSetShaderResources(0, 1, &g_Texture[ghost[i].chase == true ? 1 : 0]);

			//�S�[�X�g�̈ʒu��e�N�X�`���[���W�𔽉f
			float hp_p = ghost[i].hp / (float)(ghost[i].texNo == 0 ? GHOST_NORMAL_HP_MAX : GHOST_RARE_HP_MAX);	//�c��HP�̊���
			float px = ghost[i].pos.x;
			float py = ghost[i].pos.y;
			float dw = -ghost[i].w / 2;				//��_����\�����������ʒu�ւ�X�̈ړ���
			float dh = -ghost[i].h*1.06f / 2;		//��_����\�����������ʒu�ւ�Y�̈ړ���

			// �P���̃|���S���̒��_�ƃe�N�X�`�����W��ݒ�
			SetSpriteHpbar(g_VertexBuffer, hp_p, px, py, dw, dh);

			// �|���S���`��
			GetDeviceContext()->Draw(4, 0);
		}
	}
}


