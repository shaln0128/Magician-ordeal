//=============================================================================
//
// スコア処理 [score.cpp]
// Author : GP11B132 36 向田駿平
//
//=============================================================================
#include "sprite.h"
#include "score.h"
//*****************************************************************************
// マクロ定義
//***************************************************************************** 
#define TEXTURE_MAX					(2)		// テクスチャの数

#define TEXTURE_PATTERN_DIVIDE_X	(5)		// アニメパターンのテクスチャ内分割数（X)
#define TEXTURE_PATTERN_DIVIDE_Y	(2)		// アニメパターンのテクスチャ内分割数（Y)

#define TEXTURE_WIDTH				(25)	// キャラサイズ
#define TEXTURE_HEIGHT				(50)

#define TEXTURE_POS_X				(SCREEN_WIDTH - TEXTURE_WIDTH / 2)
#define TEXTURE_POS_Y				(SCREEN_HEIGHT - TEXTURE_HEIGHT / 2)

#define SCORE_MAX					(999999)	//スコアの最大値
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
	"data/TEXTURE/white.png",
};

static int score_data[TYPEMAX] = {0,0,0,0,0};					//討伐数などのデータを格納
static int score_rank[SCORE_RANK_MAX] = { -1,-1,-1,-1,-1 };	//ランキングを格納
static float g_texture_w;	//表示位置など
static float g_texture_h;
static float g_texture_x;
static float g_texture_y;
//=============================================================================
// 初期化処理
//=============================================================================
HRESULT InitScore(void)
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


	// スコア配列の初期化

	for (int i = 0; i < TYPEMAX; i++)
	{
		score_data[i] = 0;
	}

	//ビューポートの数値に応じて表示位置を更新
	SetScorePos();

	return S_OK;
}

//=============================================================================
// 終了処理
//=============================================================================
void UninitScore(void)
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
void UpdateScore(void)
{
	//ビューポートの数値に応じて表示位置を更新
	SetScorePos();

#ifdef _DEBUG	// デバッグ情報を表示する
	char *str = GetDebugStr();
	sprintf(&str[strlen(str)], " SCORE:%d", score_data[3]);

#endif
}

//=============================================================================
// 描画処理
//=============================================================================
void DrawScore(void)
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
		GetDeviceContext()->PSSetShaderResources(0, 1, &g_Texture[1]);

		// カバーの位置やテクスチャー座標を反映
		float px = g_texture_x - g_texture_w * 2.5f;
		float py = g_texture_y;
		float pw = g_texture_w * 6.5f;
		float ph = g_texture_h * 1.1f;

		// １枚のポリゴンの頂点とテクスチャ座標を設定
		SetSpriteColor(g_VertexBuffer, px, py, pw, ph, 0.0f, 0.0f, 1.0f, 1.0f, D3DXCOLOR(0.8f, 0.8f, 0.8f, 0.3f), true);

		// ポリゴン描画
		GetDeviceContext()->Draw(4, 0);
	}

	//現在の合計スコアを格納
	int number = score_data[TOTALSCORE];

	if (number < 0)
	{
		number = 0;
	}
	else if (number > SCORE_MAX)	//最大値を超えさせない処理
	{
		number = SCORE_MAX;
	}

	//スコアの描画
	for (int i = 0; i < SCORE_DIGIT_MAX; i++)
	{					
		int x = 0;

		x = number % 10;

		number /= 10;

		// テクスチャ設定
		GetDeviceContext()->PSSetShaderResources(0, 1, &g_Texture[0]);

		// スコアの位置やテクスチャー座標を反映
		float px = g_texture_x - g_texture_w *float(i);		// スコアの表示位置X
		float py = g_texture_y;								// スコアの表示位置Y
		float pw = g_texture_w;								// スコアの表示幅
		float ph = g_texture_h;								// スコアの表示高さ

		float tw = 1.0f / TEXTURE_PATTERN_DIVIDE_X;	// テクスチャの幅
		float th = 1.0f / TEXTURE_PATTERN_DIVIDE_Y;	// テクスチャの高さ
		float tx = tw * float(x % 5);	// テクスチャの左上X座標
		float ty = th * float(x / 5);	// テクスチャの左上Y座標

		// １枚のポリゴンの頂点とテクスチャ座標を設定
		SetSprite(g_VertexBuffer, px, py, pw, ph, tx, ty, tw, th);

		// ポリゴン描画
		GetDeviceContext()->Draw(4, 0);

	}
}


void AddScore(int n, SCORETYPE type)	//スコアを加算　討伐数やダメージなどを場合分け
{
	score_data[TOTALSCORE] += n;

	if (type == DAMAGE)
	{
		score_data[type] += n;
	}
	else
	{
		score_data[type]++;
	}
}

void RecordScore(void)	//スコアをランキング配列へと格納
{
	int min = score_data[TOTALSCORE];
	int temp = 0;

	//ランキング配列のソート
	for (int i = 0; i < SCORE_RANK_MAX; i++)
	{
		if (min >= score_rank[i])
		{
			temp = score_rank[i];
			score_rank[i] = min;
			min = temp;
		}
	}
}

int *GetScoreRank(void)	//ランキングへのアドレス
{
	return score_rank;
}

int *GetScoreData(void)	//スコアデータへのアドレス
{
	return score_data;
}

void SetScorePos(void)	//ビューポートの数値に応じて表示位置を更新
{
	VIEWPORT *viewport = GetViewports();
	float mag = viewport->mag;	//倍率
	float tl_x = viewport->topleft.x;	//左上座標
	float tl_y = viewport->topleft.y;

	g_texture_w = TEXTURE_WIDTH / mag;
	g_texture_h = TEXTURE_HEIGHT / mag;
	g_texture_x = (-tl_x + SCREEN_WIDTH) / mag - g_texture_w / 2.0f;
	g_texture_y = (-tl_y)/ mag + g_texture_h / 2.0f;
}