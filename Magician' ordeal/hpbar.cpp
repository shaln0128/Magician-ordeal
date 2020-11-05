//=============================================================================
//
// HPバー処理 [hpbar.cpp]
// Author : GP11B132 36 向田駿平
//
//=============================================================================
#include "player.h"
#include "sprite.h"
#include "enemy.h"
#include "hpbar.h"
#include "ghost.h"

//*****************************************************************************
// マクロ定義
//***************************************************************************** 
#define TEXTURE_MAX					(2)		// テクスチャの数



//*****************************************************************************
// プロトタイプ宣言
//*****************************************************************************


//*****************************************************************************
// グローバル変数
//*****************************************************************************
static ID3D11Buffer				*g_VertexBuffer = NULL;		// 頂点情報
static ID3D11ShaderResourceView	*g_Texture[TEXTURE_MAX] = { NULL };	// テクスチャ情報

static char *g_TexturName[] = {
	"data/TEXTURE/hpp.png",
	"data/TEXTURE/red.png",
};


//=============================================================================
// 初期化処理
//=============================================================================
HRESULT InitHpbar(void)
{
	ID3D11Device *pDevice = GetDevice();

	//テクスチャ生成
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
// 更新処理
//=============================================================================
void UpdateHpbar(void)
{

}

//=============================================================================
// 描画処理
//=============================================================================
void DrawHpbar(void)
{
	ENEMY *ghost = GetGhost();
	
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

	for (int i = 0; i < GHOST_MAX; i++)
	{
		if (ghost[i].use == true)	// このゴーストが使われている？
		{								// Yes
			// テクスチャ設定	追尾状態の敵はHPバーを赤に
			GetDeviceContext()->PSSetShaderResources(0, 1, &g_Texture[ghost[i].chase == true ? 1 : 0]);

			//ゴーストの位置やテクスチャー座標を反映
			float hp_p = ghost[i].hp / (float)(ghost[i].texNo == 0 ? GHOST_NORMAL_HP_MAX : GHOST_RARE_HP_MAX);	//残りHPの割合
			float px = ghost[i].pos.x;
			float py = ghost[i].pos.y;
			float dw = -ghost[i].w / 2;				//基準点から表示させたい位置へのXの移動量
			float dh = -ghost[i].h*1.06f / 2;		//基準点から表示させたい位置へのYの移動量

			// １枚のポリゴンの頂点とテクスチャ座標を設定
			SetSpriteHpbar(g_VertexBuffer, hp_p, px, py, dw, dh);

			// ポリゴン描画
			GetDeviceContext()->Draw(4, 0);
		}
	}
}


