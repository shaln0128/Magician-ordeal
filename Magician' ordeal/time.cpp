//////=============================================================================
//
// タイムなどのUI処理 [time.cpp]
// Author : GP11B132 36 向田駿平
//
//=============================================================================
#include "sprite.h"
#include "score.h"
#include "time.h"
#include "fade.h"
#include "ready.h"
#include "fire.h"
//*****************************************************************************
// マクロ定義
//***************************************************************************** 
#define TEXTURE_MAX					(10)		// テクスチャの数

#define TEXTURE_PATTERN_DIVIDE_X	(5)			// アニメパターンのテクスチャ内分割数（X)
#define TEXTURE_PATTERN_DIVIDE_Y	(2)			// アニメパターンのテクスチャ内分割数（Y)

#define TEXTURE_WIDTH				(25)		// キャラサイズ
#define TEXTURE_HEIGHT				(50)

#define SECOND_WIDTH				(8)			// キャラサイズ
#define SECOND_HEIGHT				(2)
#define SECOND_REACH				(45)
	
#define FIRE_WIDTH					(40)			// キャラサイズ
#define FIRE_HEIGHT					(40)			// キャラサイズ

#define FIREBAR_WIDTH				(200)			// キャラサイズ
#define FIREBAR_HEIGHT				(FIREBAR_WIDTH / 10)

#define LASERICON_WIDTH				(40)			// キャラサイズ
#define LASERICON_HEIGHT			(20)			// キャラサイズ

#define COVER_WIDTH					(250)			// キャラサイズ
#define COVER_HEIGHT				(70)			// キャラサイズ


#define NAVI_WIDTH					(540)			// キャラサイズ
#define NAVI_HEIGHT					(40)			// キャラサイズ

#define TEXTURE_POS_X				(TEXTURE_WIDTH + TEXTURE_WIDTH / 2)
#define TEXTURE_POS_Y				(TEXTURE_HEIGHT / 2)

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
	"data/TEXTURE/number_black.png",
	"data/TEXTURE/black.png",
	"data/TEXTURE/white.png",
	"data/TEXTURE/red.png",
	"data/TEXTURE/chip.png",
	"data/TEXTURE/fire_icon.png",
	"data/TEXTURE/fire_bar.png",
	"data/TEXTURE/laser_icon.png",
	"data/TEXTURE/laser_stock.png",
	"data/TEXTURE/navi.png",
};

static int g_time;	//経過時間

//それぞれのテクスチャの幅と位置
static float g_texture_w;
static float g_texture_h;
static float g_texture_x;
static float g_texture_y;

static float g_second_w;
static float g_second_h;
static float g_second_x;
static float g_second_y;
static float g_second_reach;

static float g_fire_w;
static float g_fire_h;
static float g_fire_x;
static float g_fire_y;

static float g_firebar_w;
static float g_firebar_h;
static float g_firebar_x;
static float g_firebar_y;

static float g_lasericon_w;
static float g_lasericon_h;
static float g_lasericon_x;
static float g_lasericon_y;

static float g_cover_w;
static float g_cover_h;
static float g_cover_x;
static float g_cover_y;

static float g_navi_w;
static float g_navi_h;
static float g_navi_x;
static float g_navi_y;

static float rad = pi / 180;	//ラジアン変換用
//=============================================================================
// 初期化処理
//=============================================================================
HRESULT InitTime(void)
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

	g_time = 3600; 	//制限時間は60秒

	// ビューポートの数値に応じて表示位置を更新
	SetTimePos();

	return S_OK;
}

//=============================================================================
// 終了処理
//=============================================================================
void UninitTime(void)
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
void UpdateTime(void)
{
	if (GetMode() == MODE_GAME)	//ゲームモードの場合
	{
		g_time -= 1;

		if (g_time < 0)
		{
			g_time = 0;
		}
	}
	
	// ビューポートの数値に応じて表示位置を更新
	SetTimePos();
#ifdef _DEBUG	// デバッグ情報を表示する
	char *str = GetDebugStr();
	sprintf(&str[strlen(str)], " TIME:%d", g_time);

#endif
}

//=============================================================================
// 描画処理
//=============================================================================
void DrawTime(void)
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


	{
		GetDeviceContext()->PSSetShaderResources(0, 1, &g_Texture[4]);

		// 時計盤の位置やテクスチャー座標を反映
		float px = g_second_x;	// 時計盤の表示位置X
		float py = g_second_y;	// 時計盤の表示位置Y
		float size = g_second_reach * 2.5f;

		// １枚のポリゴンの頂点とテクスチャ座標を設定
		SetSpriteColor(g_VertexBuffer, px, py, size, size, 0.0f, 0.0f, 1.0f, 1.0f, D3DXCOLOR(0.8f, 0.8f, 0.8f, 0.3f), true);

		// ポリゴン描画
		GetDeviceContext()->Draw(4, 0);
	}

	int number = g_time;

	int x, y;

	number /= 60;
	x = number / 10;
	y = number % 10;

	{
		// テクスチャ設定
		GetDeviceContext()->PSSetShaderResources(0, 1, &g_Texture[0]);

		// 時計の数字の位置やテクスチャー座標を反映
		float px = g_texture_x - g_texture_w;	// 時計の数字の表示位置X
		float py = g_texture_y;	// 時計の数字の表示位置Y
		float pw = g_texture_w;		// 時計の数字の表示幅
		float ph = g_texture_h;		// 時計の数字の表示高さ

		float tw = 1.0f / TEXTURE_PATTERN_DIVIDE_X;	// テクスチャの幅
		float th = 1.0f / TEXTURE_PATTERN_DIVIDE_Y;	// テクスチャの高さ
		float tx = tw * float(x % 5);	// テクスチャの左上X座標
		float ty = th * float(x / 5);	// テクスチャの左上Y座標

		// １枚のポリゴンの頂点とテクスチャ座標を設定
		SetSprite(g_VertexBuffer, px, py, pw, ph, tx, ty, tw, th);

		GetDeviceContext()->Draw(4, 0);
	}

	{
		GetDeviceContext()->PSSetShaderResources(0, 1, &g_Texture[0]);

		// 時計の数字の位置やテクスチャー座標を反映
		float px = g_texture_x;	// 時計の数字の表示位置X
		float py = g_texture_y;	// 時計の数字の表示位置Y
		float pw = g_texture_w;		// 時計の数字の表示幅
		float ph = g_texture_h;		// 時計の数字の表示高さ

		float tw = 1.0f / TEXTURE_PATTERN_DIVIDE_X;	// テクスチャの幅
		float th = 1.0f / TEXTURE_PATTERN_DIVIDE_Y;	// テクスチャの高さ
		float tx = tw * float(y % 5);	// テクスチャの左上X座標
		float ty = th * float(y / 5);	// テクスチャの左上Y座標

		// １枚のポリゴンの頂点とテクスチャ座標を設定
		SetSprite(g_VertexBuffer, px, py, pw, ph, tx, ty, tw, th);

		// ポリゴン描画
		GetDeviceContext()->Draw(4, 0);
	}
	
	for (int i = 0; i < 60; i++)
	{
		int texNo = i < g_time / 60 ? 1 : 2;

		texNo = ((texNo == 1) && (i % 5 == 0)) ? 3 : texNo;

		GetDeviceContext()->PSSetShaderResources(0, 1, &g_Texture[texNo]);

		// 時計の針の位置やテクスチャー座標を反映
		float px = g_second_x;	// 時計の針の表示位置X
		float py = g_second_y;	// 時計の針の表示位置Y
		float pw = i % 5 == 0 ? g_second_w * 1.8f : g_second_w;		// 時計の針の表示幅
		float ph = g_second_h;		// 時計の針の表示高さ
		float angle = (-6 * i - 90) * rad;
		float sx = cosf(angle) * g_second_reach;	// 時計の針の表示位置X
		float sy = sinf(angle) * g_second_reach;	// 時計の針の表示位置Y

		// １枚のポリゴンの頂点とテクスチャ座標を設定
		SetSpriteColorRotation(g_VertexBuffer, px + sx, py + sy, pw, ph, 1.0f, 1.0f, 0.0f, 0.0f, D3DXCOLOR(1.0f, 1.0f, 1.0f, 1.0f), angle);

		// ポリゴン描画
		GetDeviceContext()->Draw(4, 0);
	}


	{
		GetDeviceContext()->PSSetShaderResources(0, 1, &g_Texture[2]);

		// カバーの位置やテクスチャー座標を反映
		float px = g_cover_x;	// カバーの表示位置X
		float py = g_cover_y;	// カバーの表示位置Y
		float pw = g_cover_w;
		float ph = g_cover_h;

		// １枚のポリゴンの頂点とテクスチャ座標を設定
		SetSpriteColor(g_VertexBuffer, px, py, pw, ph, 0.0f, 0.0f, 1.0f, 1.0f, D3DXCOLOR(0.8f, 0.8f, 0.8f, 0.3f), true);

		// ポリゴン描画
		GetDeviceContext()->Draw(4, 0);
	}

	{
		GetDeviceContext()->PSSetShaderResources(0, 1, &g_Texture[5]);

		// 炎アイコンの位置やテクスチャー座標を反映
		float px = g_fire_x;	// 炎アイコンの表示位置X
		float py = g_fire_y;	// 炎アイコンの表示位置Y
		float pw = g_fire_w;
		float ph = g_fire_h;
		// １枚のポリゴンの頂点とテクスチャ座標を設定
		SetSpriteColor(g_VertexBuffer, px, py, pw, ph, 0.0f, 0.0f, 1.0f, 1.0f, D3DXCOLOR(1.0f, 1.0f, 1.0f, 1.0f), true);

		// ポリゴン描画
		GetDeviceContext()->Draw(4, 0);
	}

	{
		int *breath_time = GetBreathTime();

		GetDeviceContext()->PSSetShaderResources(0, 1, &g_Texture[3]);

		float hp_p = *breath_time / float(BREATH_TIME_MAX);
		float px = g_firebar_x;
		float py = g_firebar_y;
		float dw = -g_firebar_w / 2;
		float dh = -g_firebar_h / 2;
			
		// １枚のポリゴンの頂点とテクスチャ座標を設定
		SetSpriteHpbar(g_VertexBuffer, hp_p, px, py, dw, dh);

		GetDeviceContext()->Draw(4, 0);
	}

	{
		GetDeviceContext()->PSSetShaderResources(0, 1, &g_Texture[6]);

		// 炎ゲージの位置やテクスチャー座標を反映
		float px = g_firebar_x;	// 炎ゲージの表示位置X
		float py = g_firebar_y;	// 炎ゲージの表示位置Y
		float pw = g_firebar_w;
		float ph = g_firebar_h;
		// １枚のポリゴンの頂点とテクスチャ座標を設定
		SetSpriteColor(g_VertexBuffer, px, py, pw, ph, 0.0f, 0.0f, 1.0f, 1.0f, D3DXCOLOR(1.0f, 1.0f, 1.0f, 1.0f), true);

		// ポリゴン描画
		GetDeviceContext()->Draw(4, 0);
	}

	{
		GetDeviceContext()->PSSetShaderResources(0, 1, &g_Texture[8]);

		// レーザーアイコンの位置やテクスチャー座標を反映
		float px = g_lasericon_x;	// レーザーアイコンの表示位置X
		float py = g_lasericon_y;	// レーザーアイコンの表示位置Y
		float pw = g_lasericon_w;
		float ph = g_lasericon_h;
		// １枚のポリゴンの頂点とテクスチャ座標を設定
		SetSpriteColor(g_VertexBuffer, px, py, pw, ph, 0.0f, 0.0f, 1.0f, 1.0f, D3DXCOLOR(1.0f, 1.0f, 1.0f, 1.0f), true);

		// ポリゴン描画
		GetDeviceContext()->Draw(4, 0);
	}

	if (GetMode() == MODE_GAME)
	{
		GetDeviceContext()->PSSetShaderResources(0, 1, &g_Texture[9]);

		// ナビの位置やテクスチャー座標を反映
		float px = g_navi_x;	// ナビの表示位置X
		float py = g_navi_y;	// ナビの表示位置Y
		float pw = g_navi_w;
		float ph = g_navi_h;
		// １枚のポリゴンの頂点とテクスチャ座標を設定
		SetSpriteColor(g_VertexBuffer, px, py, pw, ph, 0.0f, 0.0f, 1.0f, 1.0f, D3DXCOLOR(1.0f, 1.0f, 1.0f, 1.0f), true);

		// ポリゴン描画
		GetDeviceContext()->Draw(4, 0);
	}

	PLAYER *player = GetPlayer();

	for (int i = 1; i <= player[0].laser_stock; i++)
	{
		GetDeviceContext()->PSSetShaderResources(0, 1, &g_Texture[7]);
		// レーザーアイコンの位置やテクスチャー座標を反映
		float px = g_lasericon_x + g_lasericon_w * 1.25f * i;	// レーザーアイコンの表示位置X
		float py = g_lasericon_y;	// レーザーアイコンの表示位置Y
		float pw = g_lasericon_w; 
		float ph = g_lasericon_h;
		// １枚のポリゴンの頂点とテクスチャ座標を設定
		SetSpriteColor(g_VertexBuffer, px, py, pw, ph, 0.0f, 0.0f, 1.0f, 1.0f, D3DXCOLOR(1.0f, 1.0f, 1.0f, 1.0f), true);

		// ポリゴン描画
		GetDeviceContext()->Draw(4, 0);
	}
}

int GetTime(void)	//経過時間を返す
{
	return g_time;
}

// ビューポートの数値に応じて表示位置を更新
void SetTimePos(void)
{
	VIEWPORT *viewport = GetViewports();
	float mag = viewport->mag;	//倍率
	float tl_x = viewport->topleft.x;	//左上座標
	float tl_y = viewport->topleft.y;

	g_texture_w = TEXTURE_WIDTH / mag;
	g_texture_h = TEXTURE_HEIGHT / mag;
	g_texture_x = (-tl_x + 60.0f) / mag + g_texture_w * 0.5f;
	g_texture_y = (-tl_y + 60.0f) / mag;
	
	g_second_w = SECOND_WIDTH / mag;
	g_second_h = SECOND_HEIGHT / mag;
	g_second_x = (-tl_x + 60.0f) / mag;
	g_second_y = (-tl_y + 60.0f) / mag;
	g_second_reach = SECOND_REACH / mag;

	g_fire_w = FIRE_WIDTH / mag;
	g_fire_h = FIRE_HEIGHT / mag;
	g_fire_x = (-tl_x + 145.0f) / mag;
	g_fire_y = (-tl_y + 40.0f) / mag;

	g_firebar_w = FIREBAR_WIDTH / mag;
	g_firebar_h = FIREBAR_HEIGHT / mag;
	g_firebar_x = (-tl_x + 268.0f) / mag;
	g_firebar_y = g_fire_y;

	g_lasericon_w = LASERICON_WIDTH / mag;
	g_lasericon_h = LASERICON_HEIGHT / mag;
	g_lasericon_x = (-tl_x + 145.0f) / mag;
	g_lasericon_y = (-tl_y + 80.0f) / mag;

	g_cover_w = COVER_WIDTH / mag;
	g_cover_h = COVER_HEIGHT / mag;
	g_cover_x = (-tl_x + 250.0f) / mag;
	g_cover_y = (-tl_y + 60.0f) / mag;

	g_navi_w = NAVI_WIDTH / mag;
	g_navi_h = NAVI_HEIGHT / mag;
	g_navi_x = (-tl_x + 640.0f) / mag;
	g_navi_y = (-tl_y + 510.0f) / mag;
}