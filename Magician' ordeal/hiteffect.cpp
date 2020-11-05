//=============================================================================
//
// ヒットエフェクトの処理【hiteffect.cpp】
// Author : GP11B132 36 向田駿平
//
//=============================================================================
#include "sprite.h"
#include "input.h"
#include "hiteffect.h"
#include "sound.h"
//*****************************************************************************
// マクロ定義
//*****************************************************************************
#define TEXTURE_WIDTH				(50)	// キャラサイズ
#define TEXTURE_HEIGHT				(50)	// キャラサイズ
#define TEXTURE_MAX					(1)		// テクスチャの数

#define TEXTURE_PATTERN_DIVIDE_X	(9)
#define TEXTURE_PATTERN_DIVIDE_Y	(1)

#define ANIM_PATTERN_NUM			(TEXTURE_PATTERN_DIVIDE_X)	// アニメーションパターン数
#define ANIM_WAIT					(2)		// アニメーションの切り替わるWait値

#define pi							float(3.14159265)	//ぱい
//*****************************************************************************
// プロトタイプ宣言
//*****************************************************************************

//*****************************************************************************
// グローバル変数
//*****************************************************************************
static ID3D11Buffer				*g_VertexBuffer = NULL;		// 頂点情報
static ID3D11ShaderResourceView	*g_Texture[TEXTURE_MAX] = { NULL };	// テクスチャ情報

static char *g_TexturName[] = {
	"data/TEXTURE/hiteffect.png",
};

static EFFECT hiteffect[HITEFFECT_MAX];	//ヒットエフェクトの構造体

static float rad = pi / 180.0f;	//ラジアン変換用
//=============================================================================
// 初期化処理
//=============================================================================
HRESULT InitHitEffect(void)
{
	ID3D11Device *pDevice = GetDevice();

	//テクスチャ生成
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


	// 頂点バッファ生成
	D3D11_BUFFER_DESC bd;
	ZeroMemory(&bd, sizeof(bd));
	bd.Usage = D3D11_USAGE_DYNAMIC;
	bd.ByteWidth = sizeof(VERTEX_3D) * 4;
	bd.BindFlags = D3D11_BIND_VERTEX_BUFFER;
	bd.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
	GetDevice()->CreateBuffer(&bd, NULL, &g_VertexBuffer);

	// ヒットエフェクトの初期化
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
// 終了処理
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
// 更新処理
//=============================================================================
void UpdateHitEffect(void)
{
	for (int i = 0; i < HITEFFECT_MAX; i++)
	{
		if (hiteffect[i].use == true)		// プレイヤーが使われている？
		{						// Yes
			// アニメーション  
			hiteffect[i].countAnim++;
			if ((hiteffect[i].countAnim % ANIM_WAIT) == 0)
			{
				// パターンの切り替え
				hiteffect[i].patternAnim++;

				if (hiteffect[i].patternAnim == ANIM_PATTERN_NUM)	//最後まで表示したら消す
				{
					hiteffect[i].use = false;
				}
			}
		}
	}

#ifdef _DEBUG	// デバッグ情報を表示する
	/*char *str = GetDebugStr();
	sprintf(&str[strlen(str)], " fire%d", fire_count);*/
#endif

}

//=============================================================================
// 描画処理
//=============================================================================
void DrawHitEffect(void)
{
	// 頂点バッファ設定
	UINT stride = sizeof(VERTEX_3D);
	UINT offset = 0;
	GetDeviceContext()->IASetVertexBuffers(0, 1, &g_VertexBuffer, &stride, &offset);

	// マトリクス設定
	SetWorldViewProjection2D();

	// プリミティブトポロジ設定
	GetDeviceContext()->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLESTRIP);

	// マテリアル設定
	MATERIAL material;
	ZeroMemory(&material, sizeof(material));
	material.Diffuse = D3DXCOLOR(1.0f, 1.0f, 1.0f, 1.0f);
	SetMaterial(material);

	SetBlendState(BLEND_MODE_ADD);	//加算合成

	for (int i = 0; i < SUMMON_MAX; i++)
	{
		if (hiteffect[i].use == true)
		{
			// テクスチャ設定
			GetDeviceContext()->PSSetShaderResources(0, 1, &g_Texture[0]);
			// １枚のポリゴンの頂点とテクスチャ座標を設定
			float dx = hiteffect[i].pos.x;
			float dy = hiteffect[i].pos.y;
			float dw = hiteffect[i].w;
			float dh = hiteffect[i].h;

			float tw = 1.0f / TEXTURE_PATTERN_DIVIDE_X;	// テクスチャの幅
			float th = 1.0f / TEXTURE_PATTERN_DIVIDE_Y;	// テクスチャの高さ
			float tx = tw * hiteffect[i].patternAnim;// テクスチャの左上X座標
			float ty = 0.0f;// テクスチャの左上Y座標

			SetSpriteColor(g_VertexBuffer, dx, dy, dw, dh, tx, ty, tw, th, D3DXCOLOR(1.0f, 1.0f, 1.0f, 1.0f), true);

			// ポリゴン描画
			GetDeviceContext()->Draw(4, 0);
		}
	}

	SetBlendState(BLEND_MODE_ALPHABLEND);
}

EFFECT *GetHitEffect(void)	//ヒットエフェクトの構造体へのアドレス
{
	return(&hiteffect[0]);
}

void SetHitEffect(D3DXVECTOR3 pos)	//ヒットエフェクトの生成
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







