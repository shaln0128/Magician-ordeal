//=============================================================================
//
// �q�b�g�G�t�F�N�g�̏����yhiteffect.cpp�z
// Author : GP11B132 36 ���c�x��
//
//=============================================================================
#include "sprite.h"
#include "input.h"
#include "hiteffect.h"
#include "sound.h"
//*****************************************************************************
// �}�N����`
//*****************************************************************************
#define TEXTURE_WIDTH				(50)	// �L�����T�C�Y
#define TEXTURE_HEIGHT				(50)	// �L�����T�C�Y
#define TEXTURE_MAX					(1)		// �e�N�X�`���̐�

#define TEXTURE_PATTERN_DIVIDE_X	(9)
#define TEXTURE_PATTERN_DIVIDE_Y	(1)

#define ANIM_PATTERN_NUM			(TEXTURE_PATTERN_DIVIDE_X)	// �A�j���[�V�����p�^�[����
#define ANIM_WAIT					(2)		// �A�j���[�V�����̐؂�ւ��Wait�l

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
	"data/TEXTURE/hiteffect.png",
};

static EFFECT hiteffect[HITEFFECT_MAX];	//�q�b�g�G�t�F�N�g�̍\����

static float rad = pi / 180.0f;	//���W�A���ϊ��p
//=============================================================================
// ����������
//=============================================================================
HRESULT InitHitEffect(void)
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

	// �q�b�g�G�t�F�N�g�̏�����
	for (int i = 0; i < HITEFFECT_MAX; i++)
	{
		hiteffect[i].use = false;
		hiteffect[i].w = TEXTURE_WIDTH;
		hiteffect[i].h = TEXTURE_HEIGHT;
		hiteffect[i].pos = D3DXVECTOR3(0.0f, 0.0f, 0.0f);
		hiteffect[i].countAnim = 0;
		hiteffect[i].patternAnim = 0;
	}

	return S_OK;
}

//=============================================================================
// �I������
//=============================================================================
void UninitHitEffect(void)
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
void UpdateHitEffect(void)
{
	for (int i = 0; i < HITEFFECT_MAX; i++)
	{
		if (hiteffect[i].use == true)		// �v���C���[���g���Ă���H
		{						// Yes
			// �A�j���[�V����  
			hiteffect[i].countAnim++;
			if ((hiteffect[i].countAnim % ANIM_WAIT) == 0)
			{
				// �p�^�[���̐؂�ւ�
				hiteffect[i].patternAnim++;

				if (hiteffect[i].patternAnim == ANIM_PATTERN_NUM)	//�Ō�܂ŕ\�����������
				{
					hiteffect[i].use = false;
				}
			}
		}
	}

#ifdef _DEBUG	// �f�o�b�O����\������
	/*char *str = GetDebugStr();
	sprintf(&str[strlen(str)], " fire%d", fire_count);*/
#endif

}

//=============================================================================
// �`�揈��
//=============================================================================
void DrawHitEffect(void)
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

	for (int i = 0; i < SUMMON_MAX; i++)
	{
		if (hiteffect[i].use == true)
		{
			// �e�N�X�`���ݒ�
			GetDeviceContext()->PSSetShaderResources(0, 1, &g_Texture[0]);
			// �P���̃|���S���̒��_�ƃe�N�X�`�����W��ݒ�
			float dx = hiteffect[i].pos.x;
			float dy = hiteffect[i].pos.y;
			float dw = hiteffect[i].w;
			float dh = hiteffect[i].h;

			float tw = 1.0f / TEXTURE_PATTERN_DIVIDE_X;	// �e�N�X�`���̕�
			float th = 1.0f / TEXTURE_PATTERN_DIVIDE_Y;	// �e�N�X�`���̍���
			float tx = tw * hiteffect[i].patternAnim;// �e�N�X�`���̍���X���W
			float ty = 0.0f;// �e�N�X�`���̍���Y���W

			SetSpriteColor(g_VertexBuffer, dx, dy, dw, dh, tx, ty, tw, th, D3DXCOLOR(1.0f, 1.0f, 1.0f, 1.0f), true);

			// �|���S���`��
			GetDeviceContext()->Draw(4, 0);
		}
	}

	SetBlendState(BLEND_MODE_ALPHABLEND);
}

EFFECT *GetHitEffect(void)	//�q�b�g�G�t�F�N�g�̍\���̂ւ̃A�h���X
{
	return(&hiteffect[0]);
}

void SetHitEffect(D3DXVECTOR3 pos)	//�q�b�g�G�t�F�N�g�̐���
{
	PlaySound(SOUND_LABEL_SE_hitattack);
	for (int i = 0; i < HITEFFECT_MAX; i++)
	{
		if (hiteffect[i].use == false)
		{
			hiteffect[i].use = true;
			hiteffect[i].pos = pos;
			hiteffect[i].countAnim = 0;
			hiteffect[i].patternAnim = 0;
			return;
		}
	}

	return;
}







