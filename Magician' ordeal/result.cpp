//=============================================================================
//
// リザルト画面処理 [result.cpp]
// Author : GP11B132　36　向田駿平
//
//=============================================================================
#include "result.h"
#include "input.h"
#include "fade.h"
#include "sound.h"
#include "sprite.h"
#include "ready.h"
#include "sound.h"
#include "score.h"
//*****************************************************************************
// マクロ定義
//*****************************************************************************
#define TEXTURE_MAX					(3)				// テクスチャの数

#define TEXTURE_PATTERN_DIVIDE_X	(5)		// アニメパターンのテクスチャ内分割数（X)
#define TEXTURE_PATTERN_DIVIDE_Y	(2)		// アニメパターンのテクスチャ内分割数（Y)

#define TEXTURE_WIDTH_RESULT		(32)			// キャラサイズ
#define TEXTURE_HEIGHT_RESULT		(32)

#define TEXTURE_WIDTH				(800)			// キャラサイズ
#define TEXTURE_HEIGHT				(40)

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
	"data/TEXTURE/number_red.png",
	"data/TEXTURE/result01.png",
};


static bool						g_Use;						// true:使っている  false:未使用
static float					g_w, g_h;					// 幅と高さ
static D3DXVECTOR3				g_Pos;						// ポリゴンの座標
static int						g_TexNo;					// テクスチャ番号

static int						g_time;						//経過時間
static int						new_score;					//新規スコアの添字
//=============================================================================
// 初期化処理
//=============================================================================
HRESULT InitResult(void)
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


	// プレイヤーの初期化
	
	new_score = -1;
	g_time = 0;

	int *score_rank = GetScoreRank();
	int *score_data = GetScoreData();

	for (int i = 0; i < SCORE_RANK_MAX; i++)
	{
		if (score_rank[i] == score_data[TOTALSCORE])	//もし合計点とランキングの数字が同じなら、添字を格納する
		{
			new_score = i;
			break;
		}		
	}
	PlaySound(SOUND_LABEL_SE_drumroll);	//ドラムロールを鳴らす
	return S_OK;
}

//=============================================================================
// 終了処理
//=============================================================================
void UninitResult(void)
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
void UpdateResult(void)
{
	g_time++;

	if (g_time == 24 ||
		g_time == 68 ||
		g_time == 112 ||
		g_time == 156 ||
		g_time == 200)
	{
		PlaySound(SOUND_LABEL_SE_scoredecision);	//スコアが表示されきる度に音を鳴らす
	}

	if (g_time >= 220 && 
		g_time <= 300 &&
		g_time % 20 == 0)	//ランキングが表示される度に音を鳴らす
	{
		if ((300 - g_time) / 20 == new_score)	//もし新たに登録されたスコアのタイミングであれば、効果音を変える
		{
			PlaySound(SOUND_LABEL_SE_rankindecision);
		}
		else
		{
			PlaySound(SOUND_LABEL_SE_rankdecision);
		}
	}

	if (GetKeyboardTrigger(DIK_RETURN))
	{
		SetFade(FADE_OUT, MODE_TITLE);
		PlaySound(SOUND_LABEL_SE_decision);
	}
	else if (GetKeyboardTrigger(DIK_R))
	{
		SetFade(FADE_OUT, MODE_GAME);
		PlaySound(SOUND_LABEL_SE_decision);
	}

#ifdef _DEBUG	// デバッグ情報を表示する
	//char *str = GetDebugStr();
	//sprintf(&str[strlen(str)], " PX:%.2f PY:%.2f", g_Pos.x, g_Pos.y);

#endif

}

//=============================================================================
// 描画処理
//=============================================================================
void DrawResult(void)
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
	
	int *score_rank = GetScoreRank();
	int *score_data = GetScoreData();

	//スコアの表示
	for (int i = 0; i < TYPEMAX; i++)
	{
		int number = score_data[i];

		number = number >= 0 ? number : 0;

		// テクスチャ設定
		GetDeviceContext()->PSSetShaderResources(0, 1, &g_Texture[0]);

		for (int j = 0; j < SCORE_DIGIT_MAX; j++)
		{
			int x = 0;

			x = g_time < 4 * (i * SCORE_DIGIT_MAX + j) + i * 20 ? rand() % 10 : number % 10;	//表示時間が来るまでは乱数で表示

			number /= 10;

			// プレイヤーの位置やテクスチャー座標を反映
			float px = 460.0f - (TEXTURE_WIDTH_RESULT)* float(j);	// プレイヤーの表示位置X
			float py = 250.0f + 44.0f * i;	// プレイヤーの表示位置Y
			float pw = TEXTURE_WIDTH_RESULT;		// プレイヤーの表示幅
			float ph = TEXTURE_HEIGHT_RESULT;		// プレイヤーの表示高さ

			float tw = 1.0f / TEXTURE_PATTERN_DIVIDE_X;	// テクスチャの幅
			float th = 1.0f / TEXTURE_PATTERN_DIVIDE_Y;	// テクスチャの高さ
			float tx = tw * (x % TEXTURE_PATTERN_DIVIDE_X);	// テクスチャの左上X座標
			float ty = th * (x / TEXTURE_PATTERN_DIVIDE_X);	// テクスチャの左上Y座標

			// １枚のポリゴンの頂点とテクスチャ座標を設定
			SetSprite(g_VertexBuffer, px, py, pw, ph, tx, ty, tw, th);

			// ポリゴン描画
			GetDeviceContext()->Draw(4, 0);

		}
	}

	//ランキングの描画
	for (int i = 0; i < SCORE_RANK_MAX; i++)
	{
		int number = score_rank[i];

		number = number >= 0 ? number: 0;
		
		int texNo;

		texNo = new_score == i ? 1 : 0;

		float sx = float(200 + (5 - i) * 20 - g_time);
		sx = sx > 0 ? sx * 6.0f : 0.0f;	//表示時間が来るまでは画面外に描画　右から流れてくるように

		// テクスチャ設定
		GetDeviceContext()->PSSetShaderResources(0, 1, &g_Texture[texNo]);

		for (int j = 0; j < SCORE_DIGIT_MAX; j++)
		{
			int x = 0;

			x = number % 10;

			number /= 10;

			// プレイヤーの位置やテクスチャー座標を反映
			float px = 810.0f - (TEXTURE_WIDTH_RESULT)* float(j);	// スコアの表示位置X
			float py = 250.0f + 44.0f * i;							// スコアの表示位置Y
			float pw = TEXTURE_WIDTH_RESULT;						// スコアの表示幅
			float ph = TEXTURE_HEIGHT_RESULT;						// スコアの表示高さ

			float tw = 1.0f / TEXTURE_PATTERN_DIVIDE_X;				// テクスチャの幅
			float th = 1.0f / TEXTURE_PATTERN_DIVIDE_Y;				// テクスチャの高さ
			float tx = tw * (x % TEXTURE_PATTERN_DIVIDE_X);			// テクスチャの左上X座標
			float ty = th * (x / TEXTURE_PATTERN_DIVIDE_X);			// テクスチャの左上Y座標

			// １枚のポリゴンの頂点とテクスチャ座標を設定
			SetSprite(g_VertexBuffer, px + sx, py, pw, ph, tx, ty, tw, th);

			// ポリゴン描画
			GetDeviceContext()->Draw(4, 0);

		}
	}
	if (g_time > 420)	//表示しきったらナビを表示
	{
		GetDeviceContext()->PSSetShaderResources(0, 1, &g_Texture[2]);

		float px = 480.0f;				// ナビの表示位置X
		float py = 480.0f;				// ナビの表示位置Y
		float pw = TEXTURE_WIDTH;		// ナビの表示幅
		float ph = TEXTURE_HEIGHT;		// ナビの表示高さ

		float tw = 1.0f;	// テクスチャの幅
		float th = 1.0f;	// テクスチャの高さ
		float tx = 0.0f;	// テクスチャの左上X座標
		float ty = 0.0f;	// テクスチャの左上Y座標

		// １枚のポリゴンの頂点とテクスチャ座標を設定
		SetSprite(g_VertexBuffer, px, py, pw, ph, tx, ty, tw, th);

		// ポリゴン描画
		GetDeviceContext()->Draw(4, 0);

	}
}


