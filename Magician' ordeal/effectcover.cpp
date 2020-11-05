//=============================================================================
//
// �w�i�ɉ��p�[�e�B�N�������Z���������Ȃ����߂̃J�o�[�yeffectcover.cpp�z
// Author :		GP11B132 36�@���c�x��
//
//=============================================================================
#include "sprite.h"
#include "input.h"
#include "bg.h"
#include "fade.h"
#include "effectcover.h"
#include "fire.h"
//*****************************************************************************
// �}�N����`
//*****************************************************************************
#define TEXTURE_WIDTH				(SCREEN_WIDTH)			//�T�C�Y
#define TEXTURE_HEIGHT				(SCREEN_HEIGHT)
#define TEXTURE_MAX					(1)				// �e�N�X�`���̐�
//*****************************************************************************
// �v���g�^�C�v�錾
//*****************************************************************************

//*****************************************************************************
// �O���[�o���ϐ�
//*****************************************************************************
static ID3D11Buffer				*g_VertexBuffer = NULL;		// ���_���
static ID3D11ShaderResourceView	*g_Texture[TEXTURE_MAX] = { NULL };	// �e�N�X�`�����

static char *g_TexturName[] = {
	"data/TEXTURE/effect_cover.png",
};

//=============================================================================
// ����������
//=============================================================================
HRESULT InitEffectCover(void)
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

	return S_OK;
}

//=============================================================================
// �I������
//=============================================================================
void UninitEffectCover(void)
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
void UpdateEffectCover(void)
{


#ifdef _DEBUG	// �f�o�b�O����\������
	/*char *str = GetDebugStr();
	sprintf(&str[strlen(str)], " R%.2f G%.2f B%.2f are%.6f", uo.r, uo.g, uo.b, are);*/

#endif

}

//=============================================================================
// �`�揈��
//=============================================================================
void DrawEffectCover(void)
{
	PARTICLE *fire = GetFire();

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

	for (int i = 0; i < FIRE_MAX; i++)	//���p�[�e�B�N���ւ̏���
	{
		if (fire[i].use == true)
		{
			// �P���̃|���S���̒��_�ƃe�N�X�`�����W��ݒ�
			float dx = fire[i].pos.x;
			float dy = fire[i].pos.y;

			float size = fire[i].size;

			// �e�N�X�`���ݒ�
			GetDeviceContext()->PSSetShaderResources(0, 1, &g_Texture[0]);

			SetSprite(g_VertexBuffer, dx, dy, size, size, 0.0f, 0.0f, 1.0f, 1.0f);

			// �|���S���`��
			GetDeviceContext()->Draw(4, 0);
			
		}
	}
	
}






