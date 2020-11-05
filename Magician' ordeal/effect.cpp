//=============================================================================
//
// effect�����yeffect.cpp�z
// Author : GP11B132 36 ���c�x��
//
//=============================================================================
#include "sprite.h"
#include "input.h"
#include "effect.h"
#include "sound.h"
//*****************************************************************************
// �}�N����`
//*****************************************************************************
#define TEXTURE_WIDTH				(50)	// �L�����T�C�Y
#define TEXTURE_HEIGHT				(50)	// �L�����T�C�Y
#define TEXTURE_MAX					(EFFECTTYPE_MAX)				// �e�N�X�`���̐��@�G�t�F�N�g�̎��

#define TEXTURE_PATTERN_DIVIDE_X	(8)
#define TEXTURE_PATTERN_DIVIDE_Y	(8)

#define ANIM_PATTERN_NUM			(TEXTURE_PATTERN_DIVIDE_X*TEXTURE_PATTERN_DIVIDE_Y)	// �A�j���[�V�����p�^�[����
#define ANIM_WAIT					(1)		// �A�j���[�V�����̐؂�ւ��Wait�l
//*****************************************************************************
// �v���g�^�C�v�錾
//*****************************************************************************

//*****************************************************************************
// �O���[�o���ϐ�
//*****************************************************************************
static ID3D11Buffer				*g_VertexBuffer = NULL;		// ���_���
static ID3D11ShaderResourceView	*g_Texture[TEXTURE_MAX] = { NULL };	// �e�N�X�`�����

static char *g_TexturName[] = {
	"data/TEXTURE/vanish.png",
	"data/TEXTURE/shot.png",
	"data/TEXTURE/hit.png",
};

static EFFECT effect[EFFECT_MAX];	//�G�t�F�N�g�̍\����
//=============================================================================
// ����������
//=============================================================================
HRESULT InitEffect(void)
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

	// �G�t�F�N�g�̏�����
	for (int i = 0; i <EFFECT_MAX; i++)
	{
		effect[i].use = false;
		effect[i].w = TEXTURE_WIDTH;
		effect[i].h = TEXTURE_HEIGHT;
		effect[i].pos = D3DXVECTOR3(0.0f, 0.0f, 0.0f);	// ���S�_����\���ɂ���
		effect[i].countAnim = 0;
		effect[i].patternAnim = 0;
		effect[i].type = VANISH;	//�G�t�F�N�g�̎��
	}

	return S_OK;
}

//=============================================================================
// �I������
//=============================================================================
void UninitEffect(void)
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
void UpdateEffect(void)
{
	for (int i = 0; i < EFFECT_MAX; i++)
	{
		if (effect[i].use == true)		// �G�t�F�N�g���g���Ă���H
		{						// Yes
			// �A�j���[�V����  
			effect[i].countAnim++;
			if ((effect[i].countAnim % ANIM_WAIT) == 0)
			{
				// �p�^�[���̐؂�ւ�
				effect[i].patternAnim++;

				if (effect[i].patternAnim == ANIM_PATTERN_NUM)	//�Ō�܂ŕ\���������������
				{
					effect[i].use = false;
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
void DrawEffect(void)
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

	SetBlendState(BLEND_MODE_ADD);

	for (int i = 0; i < EFFECT_MAX; i++)
	{
		if (effect[i].use == true)
		{
			// �e�N�X�`���ݒ�	type�ɕۑ����ꂽenum�Ŏw��\
			GetDeviceContext()->PSSetShaderResources(0, 1, &g_Texture[effect[i].type]);
			// �P���̃|���S���̒��_�ƃe�N�X�`�����W��ݒ�
			float dx = effect[i].pos.x;
			float dy = effect[i].pos.y;
			float dw = effect[i].w;
			float dh = effect[i].h;

			float tw = 1.0f / TEXTURE_PATTERN_DIVIDE_X;	// �e�N�X�`���̕�
			float th = 1.0f / TEXTURE_PATTERN_DIVIDE_Y;	// �e�N�X�`���̍���
			float tx = tw * (effect[i].patternAnim % 8);// �e�N�X�`���̍���X���W
			float ty = th * (effect[i].patternAnim / 8);// �e�N�X�`���̍���Y���W

			SetSprite(g_VertexBuffer, dx, dy, dw, dh, tx, ty, tw, th);

			// �|���S���`��
			GetDeviceContext()->Draw(4, 0);
		}
	}

	SetBlendState(BLEND_MODE_ALPHABLEND);
}

EFFECT *GetEffect(void)	//�G�t�F�N�g�\���̂ւ̃A�h���X
{
	return(&effect[0]);
}

void SetEffect(D3DXVECTOR3 pos,EFFECTTYPE type)	//�G�t�F�N�g�̃Z�b�g
{
	if (type == VANISH)	//�S�[�X�g���|���ꂽ���̏���
	{
		PlaySound(SOUND_LABEL_SE_vanish);
	}

	for (int i = 0; i < EFFECT_MAX; i++)
	{
		if (effect[i].use == false)
		{
			effect[i].use = true;
			effect[i].pos = pos;
			effect[i].countAnim = 0;
			effect[i].patternAnim = 0;
			effect[i].type = type;	//��ނ��w��
			return;
		}
	}

	return;
}






