//=============================================================================
//
// ���������ysummon.cpp�z
// Author : GP11B132 36 ���c�x��
//
//=============================================================================
#include "sprite.h"
#include "input.h"
#include "summon.h"
//*****************************************************************************
// �}�N����`
//*****************************************************************************
#define TEXTURE_WIDTH				(30)	// �L�����T�C�Y
#define TEXTURE_HEIGHT				(60)	// �L�����T�C�Y
#define TEXTURE_MAX					(3)		// �e�N�X�`���̐�

#define TEXTURE_PATTERN_DIVIDE_X	(10)
#define TEXTURE_PATTERN_DIVIDE_Y	(1)

#define ANIM_PATTERN_NUM			(TEXTURE_PATTERN_DIVIDE_X)	// �A�j���[�V�����p�^�[����
#define ANIM_WAIT					(6)		// �A�j���[�V�����̐؂�ւ��Wait�l
//*****************************************************************************
// �v���g�^�C�v�錾
//*****************************************************************************

//*****************************************************************************
// �O���[�o���ϐ�
//*****************************************************************************
static ID3D11Buffer				*g_VertexBuffer = NULL;		// ���_���
static ID3D11ShaderResourceView	*g_Texture[TEXTURE_MAX] = { NULL };	// �e�N�X�`�����

static char *g_TexturName[] = {
	"data/TEXTURE/summon.png",
	"data/TEXTURE/summon_red.png",
	"data/TEXTURE/summon_blue.png",
};

static EFFECT summon[SUMMON_MAX];	//�����w�̍\����
//=============================================================================
// ����������
//=============================================================================
HRESULT InitSummon(void)
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

	// �����w�̏�����
	for (int i = 0; i < SUMMON_MAX; i++)
	{
		summon[i].use = false;
		summon[i].w = TEXTURE_WIDTH;
		summon[i].h = TEXTURE_HEIGHT;
		summon[i].pos = D3DXVECTOR3(0.0f, 0.0f, 0.0f);
		summon[i].countAnim = 0;
		summon[i].patternAnim = 0;
		summon[i].texNo = 0;
	}

	return S_OK;
}

//=============================================================================
// �I������
//=============================================================================
void UninitSummon(void)
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
void UpdateSummon(void)
{
	for (int i = 0; i < SUMMON_MAX; i++)
	{
		if (summon[i].use == true)		// �����w���g���Ă���H
		{						// Yes
			// �A�j���[�V����  
			summon[i].countAnim++;
			if ((summon[i].countAnim % ANIM_WAIT) == 0)
			{
				// �p�^�[���̐؂�ւ�
				summon[i].patternAnim++;

				if (summon[i].patternAnim == ANIM_PATTERN_NUM)	//�Ō�܂ŕ\�����������
				{
					summon[i].use = false;
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
void DrawSummon(void)
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
		if (summon[i].use == true)
		{
			// �e�N�X�`���ݒ�
			GetDeviceContext()->PSSetShaderResources(0, 1, &g_Texture[summon[i].texNo]);
			// �P���̃|���S���̒��_�ƃe�N�X�`�����W��ݒ�
			float dx = summon[i].pos.x;
			float dy = summon[i].pos.y;
			float dw = summon[i].w;
			float dh = summon[i].h;

			float tw = 1.0f / TEXTURE_PATTERN_DIVIDE_X;	// �e�N�X�`���̕�
			float th = 1.0f / TEXTURE_PATTERN_DIVIDE_Y;	// �e�N�X�`���̍���
			float tx = tw * summon[i].patternAnim;// �e�N�X�`���̍���X���W
			float ty = 0.0f;// �e�N�X�`���̍���Y���W

			SetSpriteColor(g_VertexBuffer, dx, dy, dw, dh, tx, ty, tw, th, D3DXCOLOR(0.5f,0.5f,0.5f,1.0f), true);

			// �|���S���`��
			GetDeviceContext()->Draw(4, 0);
		}
	}

	SetBlendState(BLEND_MODE_ALPHABLEND);
}

EFFECT *GetSummon(void)	//�����w�̍\���̂ւ̃A�h���X
{
	return(&summon[0]);
}

void SetSummon(D3DXVECTOR3 pos, int type)	//�����w�̃Z�b�g
{
	for (int i = 0; i < SUMMON_MAX; i++)
	{
		if (summon[i].use == false)
		{
			summon[i].use = true;
			summon[i].pos = pos + D3DXVECTOR3(0.0f, -10.0f,0.0f);	//�������ɕ\��
			summon[i].countAnim = 0;
			summon[i].patternAnim = 0;
			summon[i].texNo = type;
			return;
		}
	}

	return;
}






