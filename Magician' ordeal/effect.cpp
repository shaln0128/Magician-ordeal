//=============================================================================
//
// effect処理【effect.cpp】
// Author : GP11B132 36 向田駿平
//
//=============================================================================
#include "sprite.h"
#include "input.h"
#include "effect.h"
#include "sound.h"
//*****************************************************************************
// マクロ定義
//*****************************************************************************
#define TEXTURE_WIDTH				(50)	// キャラサイズ
#define TEXTURE_HEIGHT				(50)	// キャラサイズ
#define TEXTURE_MAX					(EFFECTTYPE_MAX)				// テクスチャの数　エフェクトの種類

#define TEXTURE_PATTERN_DIVIDE_X	(8)
#define TEXTURE_PATTERN_DIVIDE_Y	(8)

#define ANIM_PATTERN_NUM			(TEXTURE_PATTERN_DIVIDE_X*TEXTURE_PATTERN_DIVIDE_Y)	// アニメーションパターン数
#define ANIM_WAIT					(1)		// アニメーションの切り替わるWait値
//*****************************************************************************
// プロトタイプ宣言
//*****************************************************************************

//*****************************************************************************
// グローバル変数
//*****************************************************************************
static ID3D11Buffer				*g_VertexBuffer = NULL;		// 頂点情報
static ID3D11ShaderResourceView	*g_Texture[TEXTURE_MAX] = { NULL };	// テクスチャ情報

static char *g_TexturName[] = {
	"data/TEXTURE/vanish.png",
	"data/TEXTURE/shot.png",
	"data/TEXTURE/hit.png",
};

static EFFECT effect[EFFECT_MAX];	//エフェクトの構造体
//=============================================================================
// 初期化処理
//=============================================================================
HRESULT InitEffect(void)
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

	// エフェクトの初期化
	for (int i = 0; i <EFFECT_MAX; i++)
	{
		effect[i].use = false;
		effect[i].w = TEXTURE_WIDTH;
		effect[i].h = TEXTURE_HEIGHT;
		effect[i].pos = D3DXVECTOR3(0.0f, 0.0f, 0.0f);	// 中心点から表示にした
		effect[i].countAnim = 0;
		effect[i].patternAnim = 0;
		effect[i].type = VANISH;	//エフェクトの種類
	}

	return S_OK;
}

//=============================================================================
// 終了処理
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
// 更新処理
//=============================================================================
void UpdateEffect(void)
{
	for (int i = 0; i < EFFECT_MAX; i++)
	{
		if (effect[i].use == true)		// エフェクトが使われている？
		{						// Yes
			// アニメーション  
			effect[i].countAnim++;
			if ((effect[i].countAnim % ANIM_WAIT) == 0)
			{
				// パターンの切り替え
				effect[i].patternAnim++;

				if (effect[i].patternAnim == ANIM_PATTERN_NUM)	//最後まで表示しきったら消す
				{
					effect[i].use = false;
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
void DrawEffect(void)
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

	SetBlendState(BLEND_MODE_ADD);

	for (int i = 0; i < EFFECT_MAX; i++)
	{
		if (effect[i].use == true)
		{
			// テクスチャ設定	typeに保存されたenumで指定可能
			GetDeviceContext()->PSSetShaderResources(0, 1, &g_Texture[effect[i].type]);
			// １枚のポリゴンの頂点とテクスチャ座標を設定
			float dx = effect[i].pos.x;
			float dy = effect[i].pos.y;
			float dw = effect[i].w;
			float dh = effect[i].h;

			float tw = 1.0f / TEXTURE_PATTERN_DIVIDE_X;	// テクスチャの幅
			float th = 1.0f / TEXTURE_PATTERN_DIVIDE_Y;	// テクスチャの高さ
			float tx = tw * (effect[i].patternAnim % 8);// テクスチャの左上X座標
			float ty = th * (effect[i].patternAnim / 8);// テクスチャの左上Y座標

			SetSprite(g_VertexBuffer, dx, dy, dw, dh, tx, ty, tw, th);

			// ポリゴン描画
			GetDeviceContext()->Draw(4, 0);
		}
	}

	SetBlendState(BLEND_MODE_ALPHABLEND);
}

EFFECT *GetEffect(void)	//エフェクト構造体へのアドレス
{
	return(&effect[0]);
}

void SetEffect(D3DXVECTOR3 pos,EFFECTTYPE type)	//エフェクトのセット
{
	if (type == VANISH)	//ゴーストが倒された時の処理
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
			effect[i].type = type;	//種類を指定
			return;
		}
	}

	return;
}






