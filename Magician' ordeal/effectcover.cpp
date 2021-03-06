//=============================================================================
//
// 背景に炎パーティクルを加算合成させないためのカバー【effectcover.cpp】
// Author :		GP11B132 36　向田駿平
//
//=============================================================================
#include "sprite.h"
#include "input.h"
#include "bg.h"
#include "fade.h"
#include "effectcover.h"
#include "fire.h"
//*****************************************************************************
// マクロ定義
//*****************************************************************************
#define TEXTURE_WIDTH				(SCREEN_WIDTH)			//サイズ
#define TEXTURE_HEIGHT				(SCREEN_HEIGHT)
#define TEXTURE_MAX					(1)				// テクスチャの数
//*****************************************************************************
// プロトタイプ宣言
//*****************************************************************************

//*****************************************************************************
// グローバル変数
//*****************************************************************************
static ID3D11Buffer				*g_VertexBuffer = NULL;		// 頂点情報
static ID3D11ShaderResourceView	*g_Texture[TEXTURE_MAX] = { NULL };	// テクスチャ情報

static char *g_TexturName[] = {
	"data/TEXTURE/effect_cover.png",
};

//=============================================================================
// 初期化処理
//=============================================================================
HRESULT InitEffectCover(void)
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

	return S_OK;
}

//=============================================================================
// 終了処理
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
// 更新処理
//=============================================================================
void UpdateEffectCover(void)
{


#ifdef _DEBUG	// デバッグ情報を表示する
	/*char *str = GetDebugStr();
	sprintf(&str[strlen(str)], " R%.2f G%.2f B%.2f are%.6f", uo.r, uo.g, uo.b, are);*/

#endif

}

//=============================================================================
// 描画処理
//=============================================================================
void DrawEffectCover(void)
{
	PARTICLE *fire = GetFire();

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

	// テクスチャ設定
	GetDeviceContext()->PSSetShaderResources(0, 1, &g_Texture[0]);

	for (int i = 0; i < FIRE_MAX; i++)	//炎パーティクルへの処理
	{
		if (fire[i].use == true)
		{
			// １枚のポリゴンの頂点とテクスチャ座標を設定
			float dx = fire[i].pos.x;
			float dy = fire[i].pos.y;

			float size = fire[i].size;

			// テクスチャ設定
			GetDeviceContext()->PSSetShaderResources(0, 1, &g_Texture[0]);

			SetSprite(g_VertexBuffer, dx, dy, size, size, 0.0f, 0.0f, 1.0f, 1.0f);

			// ポリゴン描画
			GetDeviceContext()->Draw(4, 0);
			
		}
	}
	
}






