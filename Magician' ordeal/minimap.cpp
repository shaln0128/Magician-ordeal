//=============================================================================
//
// ミニマップの処理[minimap.cpp]
// Author : GP11B132 36 向田駿平
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
// マクロ定義
//*****************************************************************************
#define TEXTURE_WIDTH				(245)	// キャラサイズ
#define TEXTURE_HEIGHT				(135)		// 
#define TEXTURE_SIZE				(6)			// 

#define MINIMAP_X					(115)	//表示位置
#define MINIMAP_Y					(480)
#define TEXTURE_MAX					(2)				// テクスチャの数



//*****************************************************************************
// プロトタイプ宣言
//*****************************************************************************

//*****************************************************************************
// グローバル変数
//*****************************************************************************
static ID3D11Buffer				*g_VertexBuffer = NULL;		// 頂点情報
static ID3D11ShaderResourceView	*g_Texture[TEXTURE_MAX] = { NULL };	// テクスチャ情報

static char *g_TexturName[] = {
	"data/TEXTURE/minimap.png",
	"data/TEXTURE/chip.png",
};

//ミニマップの幅、位置、左上座標
static float minimap_w;
static float minimap_h;
static D3DXVECTOR3 minimap_pos;
static bool minimap_use;
static D3DXVECTOR3 minimap_topleft;
static float chip_size;	//ミニマップ上のチップの大きさ
//=============================================================================
// 初期化処理
//=============================================================================
HRESULT InitMiniMap(void)
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

	// 位置や幅等をビューポートをもとに設定
	SetMiniMapPos();
	minimap_use = true;

	return S_OK;
}

//=============================================================================
// 終了処理
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
// 更新処理
//=============================================================================
void UpdateMiniMap(void)
{
	SetMiniMapPos();

	if (GetKeyboardTrigger(DIK_M))	//Mキーで表示ON/OFF
	{
		minimap_use = minimap_use == true ? false : true;
		PlaySound(SOUND_LABEL_SE_switch);
	}
	
#ifdef _DEBUG	// デバッグ情報を表示する
	//char *str = GetDebugStr();
	//sprintf(&str[strlen(str)], " PX:%.2f PY:%.2f", g_Pos.x, g_Pos.y);

#endif

}

//=============================================================================
// 描画処理
//=============================================================================
void DrawMiniMap(void)
{
	PLAYER *player = GetPlayer();
	ENEMY *enemy = GetEnemy();
	ENEMY *ghost = GetGhost();
	VIEWPORT *viewport = GetViewports();
	float mag = viewport->mag;

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

	//テキストボックスの描画
	if (minimap_use == true)
	{
		// テクスチャ設定
		GetDeviceContext()->PSSetShaderResources(0, 1, &g_Texture[0]);

		// スコアの位置やテクスチャー座標を反映
		float px = minimap_pos.x;		// ミニマップの表示位置X
		float py = minimap_pos.y;		// ミニマップの表示位置Y
		float pw = minimap_w;			// ミニマップの表示幅
		float ph = minimap_h;			// ミニマップの表示高さ

		float tw = 1.0f;		// テクスチャの幅
		float th = 1.0f;		// テクスチャの高さ
		float tx = 0.0f;		// テクスチャの左上X座標
		float ty = 0.0f;		// テクスチャの左上Y座標

		// １枚のポリゴンの頂点とテクスチャ座標を設定
		SetSpriteColor(g_VertexBuffer, px, py, pw, ph, tx, ty, tw, th, D3DXCOLOR(1.0f, 1.0f, 1.0f, 0.5f), true);

		// ポリゴン描画
		GetDeviceContext()->Draw(4, 0);

		// テクスチャ設定
		GetDeviceContext()->PSSetShaderResources(0, 1, &g_Texture[1]);

		float size = chip_size;	//チップの大きさ

		for (int i = 0; i < PLAYER_MAX; i++)	//プレイヤー分のチップの表示
		{
			if (player[i].use == true)
			{
				// スコアの位置やテクスチャー座標を反映
				px = minimap_topleft.x + player[i].pos.x / (mag * 5.0f);		// プレイヤーの表示位置X
				py = minimap_topleft.y + player[i].pos.y / (mag * 5.0f);		// プレイヤーの表示位置Y

				// １枚のポリゴンの頂点とテクスチャ座標を設定
				SetSpriteColor(g_VertexBuffer, px, py, size * 2.0f, size * 2.0f, tx, ty, tw, th, D3DXCOLOR(0.2f, 1.0f, 0.2f, 0.7f), true);

				GetDeviceContext()->Draw(4, 0);
			}
		}

		for (int i = 0; i < ENEMY_MAX; i++)	//メイジ分のチップの表示
		{
			if (enemy[i].use == true)
			{
				// スコアの位置やテクスチャー座標を反映
				px = minimap_topleft.x + enemy[i].pos.x / (mag * 5.0f);		// メイジの表示位置X
				py = minimap_topleft.y + enemy[i].pos.y / (mag * 5.0f);		// メイジの表示位置Y

				// １枚のポリゴンの頂点とテクスチャ座標を設定
				SetSpriteColor(g_VertexBuffer, px, py, size * 2.0f, size * 2.0f, tx, ty, tw, th, D3DXCOLOR(1.0f, 0.2f, 1.0f, 0.7f), true);

				GetDeviceContext()->Draw(4, 0);
			}
		}

		for (int i = 0; i < GHOST_MAX; i++)	//ゴースト分のチップの表示
		{
			if (ghost[i].use == true)
			{
				// スコアの位置やテクスチャー座標を反映
				px = minimap_topleft.x + ghost[i].pos.x / (mag * 5.0f);		// ゴーストの表示位置X
				py = minimap_topleft.y + ghost[i].pos.y / (mag * 5.0f);		// ゴーストの表示位置Y

				D3DXCOLOR color;

				switch (ghost[i].texNo)	//種類に応じて色を分ける
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
				// １枚のポリゴンの頂点とテクスチャ座標を設定
				SetSpriteColor(g_VertexBuffer, px, py, size, size, tx, ty, tw, th, color, true);

				GetDeviceContext()->Draw(4, 0);
			}
		}
	}

}

void SetMiniMap(bool flag)	//ミニマップのON/OFF用
{
	minimap_use = flag;
	return;
}


void SetMiniMapPos(void)	//ビューポートに応じて位置などを変更
{
	VIEWPORT *viewport = GetViewports();
	float mag = viewport->mag;	//倍率
	float tl_x = viewport->topleft.x;	//左上座標
	float tl_y = viewport->topleft.y;

	minimap_w = TEXTURE_WIDTH / mag;
	minimap_h = TEXTURE_HEIGHT / mag;
	minimap_pos.x = -tl_x / mag + MINIMAP_X / mag;
	minimap_pos.y = -tl_y / mag + MINIMAP_Y / mag;
	minimap_topleft = minimap_pos - D3DXVECTOR3(minimap_w * 0.5f, minimap_h * 0.5f, 0.0f);
	chip_size = TEXTURE_SIZE / mag;
}

