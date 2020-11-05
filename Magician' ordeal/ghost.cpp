//=============================================================================
//
// ゴースト処理 [ghost.cpp]
// Author : GP11B132 36 向田駿平
//
//=============================================================================
#include "sprite.h"
#include "ghost.h"
#include "summon.h"
#include "sound.h"
//*****************************************************************************
// マクロ定義
//*****************************************************************************
#define TEXTURE_WIDTH				(15)	// キャラサイズ
#define TEXTURE_HEIGHT				(30)	// 
#define TEXTURE_MAX					(3)		// テクスチャの数

#define TEXTURE_PATTERN_DIVIDE_X	(3)		// アニメパターンのテクスチャ内分割数（X)
#define TEXTURE_PATTERN_DIVIDE_Y	(4)		// アニメパターンのテクスチャ内分割数（Y)
#define ANIM_PATTERN_NUM			(TEXTURE_PATTERN_DIVIDE_X)	// アニメーションパターン数
#define ANIM_WAIT					(4)		// アニメーションの切り替わるWait値

#define ABS_FUNC(x)					((x) < 0 ? -(x) : (x));	//絶対値に変換するマクロ
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
	"data/TEXTURE/ghost.png",
	"data/TEXTURE/ghost_red.png",
	"data/TEXTURE/ghost_blue.png",
};

ENEMY ghost[GHOST_MAX];	//ゴーストの構造体

static float rad = pi / 180;	//ラジアン変換用
//=============================================================================
// 初期化処理
//=============================================================================
HRESULT InitGhost(void)
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


	// ゴーストの初期化

	for (int i = 0; i < GHOST_MAX; i++)
	{
		ghost[i].use = false;
		ghost[i].w = TEXTURE_WIDTH;
		ghost[i].h = TEXTURE_HEIGHT;
		ghost[i].pos = D3DXVECTOR3(SCREEN_WIDTH * 0.625f, SCREEN_HEIGHT / 2.0f, 0.0f);
		ghost[i].rot = D3DXVECTOR3(0.0f, 0.0f, 0.0f);
		ghost[i].texNo = 0;

		ghost[i].countAnim = 0;
		ghost[i].patternAnim = 0;

		ghost[i].direction = DOWN;	//初期向きは下

		ghost[i].color_R = 1.0f;
		ghost[i].color_G = 1.0f;
		ghost[i].color_B = 1.0f;

		ghost[i].rad = 0.0f;
		ghost[i].rotation = 0.0f;

		ghost[i].hp = GHOST_NORMAL_HP_MAX;

		ghost[i].invincible_time = 60;	//召喚時は1秒無敵
		ghost[i].chase = false;

	}

	return S_OK;
}

//=============================================================================
// 終了処理
//=============================================================================
void UninitGhost(void)
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
void UpdateGhost(void)
{
	PLAYER *player = GetPlayer();
	int ghost_count = 0;

	for (int i = 0; i < GHOST_MAX; i++)
	{
		if (ghost[i].use == true)		// ゴーストが使われている？
		{						// Yes
			// アニメーション  
			ghost[i].countAnim++;
			if ((ghost[i].countAnim % ANIM_WAIT) == 0)
			{
				// パターンの切り替え
				ghost[i].patternAnim = (ghost[i].patternAnim + 1) % ANIM_PATTERN_NUM;
			}

			if (ghost[i].invincible_time > 0)
			{
				ghost[i].invincible_time--;
			}
			else
			{
				if (ghost[i].chase == true)	//プレイヤーを追尾
				{
					D3DXVECTOR3 b_dif = player[0].pos - ghost[i].pos; //行きたい場所　－　いる場所　＝　距離
					float angle = atan2f(b_dif.y, b_dif.x);//その角度

					ghost[i].move.x = cosf(angle) * ghost[i].speed;
					ghost[i].move.y = sinf(angle) * ghost[i].speed;
				}
				else if (GetMode() == MODE_GAME)	//追尾中でなくて、ゲームモードの場合
				{
					ghost[i].time--;

					if (ghost[i].time == 0)	//移動時間が切れたら再度角度・時間を乱数で生成
					{
						float angle = (rand() % 360) * rad;
						ghost[i].move = D3DXVECTOR3(cosf(angle) * ghost[i].speed, sinf(angle) * ghost[i].speed, 0.0f);
						ghost[i].time = (rand() % 4 + 2) * 60;//(0～3) + 2秒
					}
				}

				ghost[i].pos += ghost[i].move;

				float move_x = ghost[i].move.x;	//ｘの移動量を保存
				float abs_x = ABS_FUNC(move_x);	//絶対値に
				float move_y = ghost[i].move.y;	//ｙの移動量を保存
				float abs_y = ABS_FUNC(move_y);	//絶対値に

				if (abs_x > abs_y)
				{
					ghost[i].direction = move_x > 0 ? RIGHT : LEFT;	//移動量が正なら右、負なら左
				}
				else
				{
					ghost[i].direction = move_y > 0 ? DOWN : UP;	//移動量が正なら下、負なら上
				}
			}	
		}
	}

}

//=============================================================================
// 描画処理
//=============================================================================
void DrawGhost(void)
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

	for (int i = 0; i < GHOST_MAX; i++)
	{

		if (ghost[i].use == true)		// ゴーストが使われている？
		{						// Yes
			// テクスチャ設定
			GetDeviceContext()->PSSetShaderResources(0, 1, &g_Texture[ghost[i].texNo]);

			// プレイヤーの位置やテクスチャー座標を反映
			float px = ghost[i].pos.x;	// ゴーストの表示位置X
			float py = ghost[i].pos.y;	// ゴーストの表示位置Y
			float pw = ghost[i].w;		// ゴーストの表示幅
			float ph = ghost[i].h;		// ゴーストの表示高さ

			float tw = 1.0f / TEXTURE_PATTERN_DIVIDE_X;	// テクスチャの幅
			float th = 1.0f / TEXTURE_PATTERN_DIVIDE_Y;	// テクスチャの高さ
			float tx = tw * ghost[i].patternAnim;// テクスチャの左上X座標
			float ty = th * ghost[i].direction;// テクスチャの左上Y座標

			float r = ghost[i].color_R;
			float g = ghost[i].color_G;
			float b = ghost[i].color_B;

			float alpha = 1.0f - ghost[i].invincible_time / GHOST_INVINCIBLE_TIME;	//無敵時間は徐々に不透明度を上げていく

			float e_rad = ghost[i].rad;

			SetSpriteColorRotation(g_VertexBuffer, px, py, pw, ph, tx, ty, tw, th, D3DXCOLOR(r, g, b, alpha), e_rad);

			// ポリゴン描画
			GetDeviceContext()->Draw(4, 0);
		}
	}

}



//=============================================================================
// ゴーストの座標を取得
//=============================================================================
ENEMY *GetGhost(void)
{
	return(&ghost[0]);
}

void SetGhost(int normal,int rare)	//ゴーストの生成　ノーマル、レアの個数を指定
{
	int count = 0;

	PlaySound(SOUND_LABEL_SE_ghost);	//生成時に音を鳴らす

	for (int i = 0; i < GHOST_MAX; i++)
	{
		if (count == normal + rare)return;	//生成数が上限に達したら
		if (ghost[i].use == false)
		{
			if (count < normal)
			{
				ghost[i].speed = GHOST_NORMAL_SPEED;
				ghost[i].hp = GHOST_NORMAL_HP_MAX;
				ghost[i].texNo = NORMAL;
			}
			else
			{
				ghost[i].speed = GHOST_RARE_SPEED;
				ghost[i].hp = GHOST_RARE_HP_MAX;
				ghost[i].texNo = rand() % 2 == 0 ? RED : BLUE;	//赤か青かは乱数
			}

			//初期化と同じ
			ghost[i].use = true;
			ghost[i].chase = false;
			ghost[i].direction = DOWN;
			float angle = (rand() % 360) * rad;
			ghost[i].pos = D3DXVECTOR3(float(rand() % 680 + 140), float(rand() % 275 + 95), 0.0f);
			ghost[i].move = D3DXVECTOR3(cosf(angle) * ghost[i].speed, sinf(angle) * ghost[i].speed, 0.0f);
			ghost[i].time = (rand() % 4 + 2) * 60;
			ghost[i].invincible_time = GHOST_INVINCIBLE_TIME;
			SetSummon(ghost[i].pos, ghost[i].texNo);	//召喚陣の生成
			count++;
		}	
	}
}
