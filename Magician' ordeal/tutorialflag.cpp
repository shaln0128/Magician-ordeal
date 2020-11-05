//=============================================================================
//
//チュートリアルフラグ処理 [tutorialflag.cpp]
// Author : GP11B132 36 向田駿平
//
//=============================================================================
#include "sprite.h"
#include "input.h"
#include "tutorialflag.h"
#include "sound.h"
//*****************************************************************************
// マクロ定義
//*****************************************************************************
#define TEXTURE_WIDTH				(960)	// キャラサイズTutorialFlag
#define TEXTURE_HEIGHT				(540)	
#define TEXTURE_MAX					(7)		// テクスチャの数

#define JUDG_WIDTH					(250)
#define JUDG_HEIGHT					(70)

//*****************************************************************************
// プロトタイプ宣言
//*****************************************************************************

//*****************************************************************************
// グローバル変数
//*****************************************************************************
static ID3D11Buffer				*g_VertexBuffer = NULL;		// 頂点情報
static ID3D11ShaderResourceView	*g_Texture[TEXTURE_MAX] = { NULL };	// テクスチャ情報

static char *g_TexturName[] = {
	"data/TEXTURE/success.png",
	"data/TEXTURE/failure.png",
	"data/TEXTURE/tutorial01.png",
	"data/TEXTURE/tutorial02.png",
	"data/TEXTURE/tutorial03.png",
	"data/TEXTURE/tutorial04.png",
	"data/TEXTURE/tutorial05.png",
};

//経過時間
static int g_time;

//判定を出す位置と幅
static float g_judg_w;
static float g_judg_h;
static float g_tutorialflag_w;
static float g_tutorialflag_h;
static D3DXVECTOR3 g_tutorialflag_pos;
static int g_tutorialflag_texno;

//各種フラグ
static bool g_tutorialflag_success;
static TUTORIALSTATE tutorialstate;
static TUTORIALSTATE next_tutorialstate;
//=============================================================================
// 初期化処理
//=============================================================================
HRESULT InitTutorialFlag(void)
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

	//ビューポートに応じて位置と幅を更新
	SetTutorialFlagPos();
	g_time = 0;
	g_tutorialflag_texno = 0;
	tutorialstate = WASD_CAMERA;
	

	return S_OK;
}

//=============================================================================
// 終了処理
//=============================================================================
void UninitTutorialFlag(void)
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
void UpdateTutorialFlag(void)
{
	//ビューポートに応じて位置と幅を更新
	SetTutorialFlagPos();

	g_time++;

	if (tutorialstate == CONVERSION)	//移行中
	{
		if (g_time >= 120)	//2秒後に次のチュートリアルへ
		{
			tutorialstate = next_tutorialstate;
			g_time = 0;
		}
	}
#ifdef _DEBUG	// デバッグ情報を表示する
	char *str = GetDebugStr();
	sprintf(&str[strlen(str)], " state = %d", tutorialstate);

#endif

}

//=============================================================================
// 描画処理
//=============================================================================
void DrawTutorialFlag(void)
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

	//判定の描画
	if (tutorialstate == CONVERSION)
	{
		// テクスチャ設定
		GetDeviceContext()->PSSetShaderResources(0, 1, &g_Texture[g_tutorialflag_texno]);

		// スコアの位置やテクスチャー座標を反映
		float px = g_tutorialflag_pos.x;		// 判定の表示位置X
		float py = g_tutorialflag_pos.y;		// 判定の表示位置Y
		float pw = g_judg_w;			// 判定の表示幅
		float ph = g_judg_h;			// 判定の表示高さ

		float tw = 1.0f;		// テクスチャの幅
		float th = 1.0f;		// テクスチャの高さ
		float tx = 0.0f;		// テクスチャの左上X座標
		float ty = 0.0f;		// テクスチャの左上Y座標

		float alpha = g_time < 90 ? 1.0f : (120 - g_time) / 30.0f;		// テクスチャの左上Y座標
		// １枚のポリゴンの頂点とテクスチャ座標を設定
		SetSpriteColor(g_VertexBuffer, px, py, pw, ph, tx, ty, tw, th, D3DXCOLOR(1.0f,1.0f,1.0f,alpha),true);

		// ポリゴン描画
		GetDeviceContext()->Draw(4, 0);
	}
	else
	{
		// テクスチャ設定
		GetDeviceContext()->PSSetShaderResources(0, 1, &g_Texture[tutorialstate + 2]);

		// スコアの位置やテクスチャー座標を反映
		float px = g_tutorialflag_pos.x;		// チュートリアル画像の表示位置X
		float py = g_tutorialflag_pos.y;		// チュートリアル画像の表示位置Y
		float pw = g_tutorialflag_w;			// チュートリアル画像の表示幅
		float ph = g_tutorialflag_h;			// チュートリアル画像の表示高さ

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

void SetTutorialFlagPos(void)	//ビューポートに応じて位置と幅を更新
{
	VIEWPORT *viewport = GetViewports();
	float mag = viewport->mag;
	float tl_x = viewport->topleft.x;
	float tl_y = viewport->topleft.y;

	g_judg_w = JUDG_WIDTH / mag;
	g_judg_h = JUDG_HEIGHT / mag;
	g_tutorialflag_w = TEXTURE_WIDTH / mag;
	g_tutorialflag_h = TEXTURE_HEIGHT / mag;
	g_tutorialflag_pos.x = (-tl_x + SCREEN_CENTER_X) / mag;
	g_tutorialflag_pos.y = (-tl_y + SCREEN_CENTER_Y) / mag;
}

TUTORIALSTATE GetTutorialState(void)	//現在のチュートリアルの状態を返す
{
	return tutorialstate;
}

void ConversionTutorial(TUTORIALSTATE state, bool flag)	//状態の移行
{
	g_time = 0;
	tutorialstate = CONVERSION;
	next_tutorialstate = state;
	g_tutorialflag_success = flag;
	g_tutorialflag_texno = flag ? 0 : 1;
	if (flag == true)	//フラグに応じて効果音を鳴らす
	{
		PlaySound(SOUND_LABEL_SE_success);
	}
	else
	{
		PlaySound(SOUND_LABEL_SE_failure);
	}
}

int GetTutorialTime(void)	//経過時間を返す
{
	return g_time;
}

void SkipTime(void)	//移行をスキップ
{
	g_time = 120;
}