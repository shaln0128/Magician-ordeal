//=============================================================================
//
// モードセレクト処理[modeselect.cpp]
// Author : GP11B132 36 向田駿平
//
//=============================================================================
#include "title.h"
#include "modeselect.h"
#include "sprite.h"
#include "input.h"
#include "bg.h"
#include "fade.h"
#include "sound.h"
//*****************************************************************************
// マクロ定義
//*****************************************************************************
#define MODE_TEXTURE_WIDTH				(600)	// キャラサイズ
#define MODE_TEXTURE_HEIGHT				(80)
#define TEXTURE_MAX						(2)		// テクスチャの数


//*****************************************************************************
// プロトタイプ宣言
//*****************************************************************************

typedef enum	//次に移行するモードのステート
{
	SELECTMODE_TUTORIAL,
	SELECTMODE_PLAYGAME,
	SELECTMODE_NONE,
}SELECTMODE;

//*****************************************************************************
// グローバル変数
//*****************************************************************************
static ID3D11Buffer				*g_VertexBuffer = NULL;		// 頂点情報
static ID3D11ShaderResourceView	*g_Texture[TEXTURE_MAX] = { NULL };	// テクスチャ情報

static char *g_TexturName[] = {
	"data/TEXTURE/tutorial.png",
	"data/TEXTURE/playgame.png",
};

static SELECTMODE g_flag;		//現在の状態
CURSORPOP mode[TEXTURE_MAX];	//モード画像の構造体
static int g_time;	//経過時間
//=============================================================================
// 初期化処理
//=============================================================================
HRESULT InitModeSelect(void)
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


	g_flag = SELECTMODE_NONE;
	g_time = 0;

	//モード画像の構造体の初期化
	for (int i = 0; i < TEXTURE_MAX; i++)
	{
		mode[i].use = true;
		mode[i].w = MODE_TEXTURE_WIDTH;
		mode[i].h = MODE_TEXTURE_HEIGHT;
		mode[i].pos = D3DXVECTOR3(480.0f, 280.0f + 120.0f * i, 0.0f);
		mode[i].texNo = i;
	}

	return S_OK;
}

//=============================================================================
// 終了処理
//=============================================================================
void UninitModeSelect(void)
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
void UpdateModeSelect(void)
{
	if (g_flag == SELECTMODE_NONE)	//何も選択されていない状態
	{
		//入力されたキーに応じて状態を変化
		if (GetKeyboardTrigger(DIK_T))
		{
			g_flag = SELECTMODE_TUTORIAL;
			PlaySound(SOUND_LABEL_SE_decision);
		}
		else if (GetKeyboardTrigger(DIK_G))
		{
			g_flag = SELECTMODE_PLAYGAME;
			PlaySound(SOUND_LABEL_SE_decision);
		}
	}
	else
	{
		g_time++;
		if (g_time >= 100)	//100フレーム後からFADE処理に移行
		{
			if (g_flag == SELECTMODE_TUTORIAL)
			{
				SetFade(FADE_OUT, MODE_TUTORIAL);
			}
			else if (g_flag == SELECTMODE_PLAYGAME)
			{
				SetFade(FADE_OUT, MODE_GAME);
			}
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
void DrawModeSelect(void)
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

	
	for (int i = 0; i < TEXTURE_MAX; i++)
	{
		GetDeviceContext()->PSSetShaderResources(0, 1, &g_Texture[mode[i].texNo]);

		// スコアの位置やテクスチャー座標を反映
		float px = mode[i].pos.x;		// モード画像の表示位置X
		float py = mode[i].pos.y;		// モード画像の表示位置Y
		float pw = mode[i].w;			// モード画像の表示幅
		float ph = mode[i].h;			// モード画像の表示高さ

		float tw = 1.0f;		// テクスチャの幅
		float th = 1.0f;		// テクスチャの高さ
		float tx = 0.0f;		// テクスチャの左上X座標
		float ty = 0.0f;		// テクスチャの左上Y座標

		float alpha = g_flag == mode[i].texNo && g_time / 8 % 2 == 1 ? 0.2f : 1.0f;	//移行中は点滅
		// １枚のポリゴンの頂点とテクスチャ座標を設定
		SetSpriteColor(g_VertexBuffer, px, py, pw, ph, tx, ty, tw, th,D3DXCOLOR(1.0f,1.0f,1.0f,alpha),true);

		// ポリゴン描画
		GetDeviceContext()->Draw(4, 0);

	}

}
