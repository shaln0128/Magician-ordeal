//=============================================================================
//
// レディー・ポーズの処理[ready.cpp]
// Author : GP11B132 36 向田駿平
//
//=============================================================================
#include "sprite.h"
#include "input.h"
#include "ready.h"
#include "fade.h"
#include "sound.h"
//*****************************************************************************
// マクロ定義
//*****************************************************************************
#define TEXTURE_WIDTH				(420)	// キャラサイズ
#define TEXTURE_HEIGHT				(80) 
#define TEXTURE_MAX					(3)		// テクスチャの数

#define PRESSENTER_WIDTH			(360)
#define PRESSENTER_HEIGHT			(40)

//*****************************************************************************
// プロトタイプ宣言
//*****************************************************************************

//*****************************************************************************
// グローバル変数
//*****************************************************************************
static ID3D11Buffer				*g_VertexBuffer = NULL;		// 頂点情報
static ID3D11ShaderResourceView	*g_Texture[TEXTURE_MAX] = { NULL };	// テクスチャ情報

static char *g_TexturName[] = {
	"data/TEXTURE/ready.png",
	"data/TEXTURE/pause.png",
	"data/TEXTURE/press_enter.png",
};


static bool	ready;						// true:使っている  false:未使用
static float g_ready_w;
static float g_ready_h;
static float g_x;
static float g_y;

static bool	pause;						// true:使っている  false:未使用
static float g_pause_w;
static float g_pause_h;

static float g_pressenter_w;
static float g_pressenter_h;

static float s_y;
//=============================================================================
// 初期化処理
//=============================================================================
HRESULT InitReady(void)
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

	// ビューポートの数値に応じて表示位置を更新
	SetReadyPos();

	ready = true;
	PlaySound(SOUND_LABEL_SE_pause);
	pause = false;

	return S_OK;
}

//=============================================================================
// 終了処理
//=============================================================================
void UninitReady(void)
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
void UpdateReady(void)
{
	SetReadyPos();	//ビューポートの数値に応じて表示位置を更新

	if (GetKeyboardTrigger(DIK_RETURN))
	{
		ready = false;
	}

	if (ready == false)
	{
		if (GetKeyboardTrigger(DIK_P))	//Pでポーズ画面のON/OFF
		{
			pause = pause == true ? false : true;
			PlaySound(SOUND_LABEL_SE_pause);
		}
	}

	if (pause == true)
	{
		//ポーズ中はキー入力に応じて画面遷移
		if (GetKeyboardTrigger(DIK_R))
		{
			SetFade(FADE_OUT, MODE_GAME);
			PlaySound(SOUND_LABEL_SE_decision);
		}
		else if (GetKeyboardTrigger(DIK_T))
		{
			SetFade(FADE_OUT, MODE_TITLE);
			PlaySound(SOUND_LABEL_SE_decision);
		}
	}


#ifdef _DEBUG	// デバッグ情報を表示する
	//char *str = GetDebugStr();
	//sprintf(&str[strlen(str)], " PX:%.2f PY:%.2f", g_Pos.x, g_Pos.y);

#endif

}

//=============================================================================
// 描画処理
//=============================================================================
void DrawReady(void)
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

	if (ready == true)
	{
		// テクスチャ設定
		GetDeviceContext()->PSSetShaderResources(0, 1, &g_Texture[0]);

		// スコアの位置やテクスチャー座標を反映
		float px = g_x;			// レディーの表示位置X
		float py = g_y;			// レディーの表示位置Y
		float pw = g_ready_w;	// レディーの表示幅
		float ph = g_ready_h;	// レディーの表示高さ

		float tw = 1.0f;		// テクスチャの幅
		float th = 1.0f;		// テクスチャの高さ
		float tx = 0.0f;		// テクスチャの左上X座標
		float ty = 0.0f;		// テクスチャの左上Y座標

		// １枚のポリゴンの頂点とテクスチャ座標を設定
		SetSprite(g_VertexBuffer, px, py, pw, ph, tx, ty, tw, th);

		// ポリゴン描画
		GetDeviceContext()->Draw(4, 0);

		// テクスチャ設定
		GetDeviceContext()->PSSetShaderResources(0, 1, &g_Texture[2]);

		pw = g_pressenter_w;	// プレスエンターの表示幅
		ph = g_pressenter_h;	// プレスエンターの表示高さ

		// １枚のポリゴンの頂点とテクスチャ座標を設定
		SetSprite(g_VertexBuffer, px, py + s_y, pw, ph, tx, ty, tw, th);

		// ポリゴン描画
		GetDeviceContext()->Draw(4, 0);
	}
	else if (pause == true)	//ポーズ画面の描画
	{
		// テクスチャ設定
		GetDeviceContext()->PSSetShaderResources(0, 1, &g_Texture[1]);

		// スコアの位置やテクスチャー座標を反映
		float px = g_x;			// ポーズ画面の表示位置X
		float py = g_y;			// ポーズ画面の表示位置Y
		float pw = g_pause_w;	// ポーズ画面の表示幅
		float ph = g_pause_h;	// ポーズ画面の表示高さ

		float tw = 1.0f;		// テクスチャの幅
		float th = 1.0f;		// テクスチャの高さ
		float tx = 0.0f;		// テクスチャの左上X座標
		float ty = 0.0f;		// テクスチャの左上Y座標

		// １枚のポリゴンの頂点とテクスチャ座標を設定
		SetSprite(g_VertexBuffer, px, py, pw, ph, tx, ty, tw, th);

		// ポリゴン描画
		GetDeviceContext()->Draw(4, 0);
	}
}

bool AreYouReady(void)	//レディー画面かポーズ画面ならtrueを返す
{
	bool ans = ready == true || pause == true ? true : false;
	return ans;
}

void SetReadyPos(void)	//ビューポートの数値に応じて表示位置を更新
{
	VIEWPORT *viewport = GetViewports();
	float mag = viewport->mag;	//倍率
	float tl_x = viewport->topleft.x;	//左上座標
	float tl_y = viewport->topleft.y;

	g_ready_w = TEXTURE_WIDTH / mag;
	g_ready_h = TEXTURE_HEIGHT / mag;
	g_x = (-tl_x + SCREEN_CENTER_X) / mag;
	g_y = (-tl_y + SCREEN_CENTER_Y) / mag;

	g_pause_w = SCREEN_WIDTH / mag;
	g_pause_h = SCREEN_HEIGHT / mag;

	g_pressenter_w = PRESSENTER_WIDTH / mag;
	g_pressenter_h = PRESSENTER_HEIGHT / mag;
	s_y = 50.0f / mag;
}
