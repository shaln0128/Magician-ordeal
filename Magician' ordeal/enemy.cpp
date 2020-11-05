//=============================================================================
//
// メイジ処理 [enemy.cpp]
// Author : GP11B132 36 向田駿平
//
//=============================================================================
#include "enemy.h"
#include "sprite.h"

//*****************************************************************************
// マクロ定義
//*****************************************************************************
#define TEXTURE_WIDTH				(15)	// キャラサイズ
#define TEXTURE_HEIGHT				(30)	// 
#define TEXTURE_MAX					(1)		// テクスチャの数

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
	"data/TEXTURE/mage001.png",
};

ENEMY enemy[ENEMY_MAX];	//メイジの構造体

static float rad = pi / 180;	//ラジアン変換用
//=============================================================================
// 初期化処理
//=============================================================================
HRESULT InitEnemy(void)
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


	// メイジの初期化

	for (int i = 0; i < ENEMY_MAX; i++)
	{
		enemy[i].use = i == 0 ? true : false;
		enemy[i].w = TEXTURE_WIDTH;
		enemy[i].h = TEXTURE_HEIGHT;
		enemy[i].pos = D3DXVECTOR3(SCREEN_WIDTH * 0.625f, SCREEN_HEIGHT / 2.0f, 0.0f);	//初期位置　右らへん
		enemy[i].rot = D3DXVECTOR3(0.0f, 0.0f, 0.0f);
		enemy[i].texNo = 0;

		enemy[i].countAnim = 0;
		enemy[i].patternAnim = 0;

		enemy[i].direction = LEFT;	//左向きで配置

		enemy[i].color_R = 1.0f;
		enemy[i].color_G = 1.0f;
		enemy[i].color_B = 1.0f;

		enemy[i].speed = 1.0f;

		enemy[i].rad = 0.0f;
		enemy[i].rotation = 0.0f;

		enemy[i].time = 0;

		if (GetMode() == MODE_GAME)	//ゲームモードの場合
		{
			float angle = (rand() % 360) * rad;	//乱数で角度を指定
			enemy[i].move = D3DXVECTOR3(cosf(angle) * enemy[i].speed, sinf(angle) * enemy[i].speed, 0.0f);	//その角度へと進む
			enemy[i].time = (rand() % 4 + 2) * 60;	//進む時間も乱数　２～５秒
		}
		else	//チュートリアルの場合
		{
			enemy[i].direction = DOWN;	//下向きで配置
			enemy[i].pos = D3DXVECTOR3(150.0f, 150.0f, 0.0f);	//初期位置は左上
			enemy[i].move = D3DXVECTOR3(0.0f, 0.0f, 0.0f);	//動かない
			enemy[i].time = 0;
		}
	}

	return S_OK;
}

//=============================================================================
// 終了処理
//=============================================================================
void UninitEnemy(void)
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
void UpdateEnemy(void)
{
	switch (GetMode())
	{
	case MODE_GAME:	//ゲームモードの時
		for (int i = 0; i < ENEMY_MAX; i++)
		{
			if (enemy[i].use == true)		// メイジが使われている？
			{						// Yes
				// アニメーション  
				enemy[i].countAnim++;
				if ((enemy[i].countAnim % ANIM_WAIT) == 0)
				{
					// パターンの切り替え
					enemy[i].patternAnim = (enemy[i].patternAnim + 1) % ANIM_PATTERN_NUM;
				}

				if (enemy[i].invincible_time > 0)	//無敵時間を減らす処理
				{
					enemy[i].invincible_time -= 1;
				}

				enemy[i].time--;	//進む時間を減らす処理

				if (enemy[i].time == 0)	//0になった時、再度進む方向と時間を乱数で指定
				{
					float angle = (rand() % 360) * rad;
					enemy[i].move = D3DXVECTOR3(cosf(angle) * enemy[i].speed, sinf(angle) * enemy[i].speed, 0.0f);
					enemy[i].time = (rand() % 4 + 2) * 60;//(0～3) + 2秒
				}

				enemy[i].pos += enemy[i].move;	//移動処理

				float move_x = enemy[i].move.x;	//ｘの移動量を保存
				float abs_x = ABS_FUNC(move_x);	//絶対値に
				float move_y = enemy[i].move.y;	//ｙの移動量を保存
				float abs_y = ABS_FUNC(move_y);	//絶対値に
											   	
				if (abs_x > abs_y)	//ｘの方が移動量が大きければ
				{
					enemy[i].direction = move_x > 0 ? RIGHT : LEFT;	//移動量が正なら右、負なら左
				}
				else
				{
					enemy[i].direction = move_y > 0 ? DOWN : UP;	//移動量が正なら下、負なら上
				}

			}
		}
		break;
	case MODE_TUTORIAL:	//チュートリアルの時
		for (int i = 0; i < ENEMY_MAX; i++)
		{
			if (enemy[i].use == true)		// メイジが使われている？
			{						// Yes
				// アニメーション  
				enemy[i].countAnim++;
				if ((enemy[i].countAnim % ANIM_WAIT) == 0)
				{
					// パターンの切り替え
					enemy[i].patternAnim = (enemy[i].patternAnim + 1) % ANIM_PATTERN_NUM;
				}

				if (enemy[i].invincible_time > 0)
				{
					enemy[i].invincible_time -= 1;
				}
			}
		}

		break;
	}

}

//=============================================================================
// 描画処理
//=============================================================================
void DrawEnemy(void)
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

	for (int i = 0; i < ENEMY_MAX; i++)
	{
	
		if (enemy[i].use == true)		// メイジが使われている？
			{						// Yes
				// テクスチャ設定
				GetDeviceContext()->PSSetShaderResources(0, 1, &g_Texture[enemy[i].texNo]);

				// プレイヤーの位置やテクスチャー座標を反映
				float px = enemy[i].pos.x;	// メイジの表示位置X
				float py = enemy[i].pos.y;	// メイジの表示位置Y
				float pw = enemy[i].w;		// メイジの表示幅
				float ph = enemy[i].h;		// メイジの表示高さ

				float tw = 1.0f / TEXTURE_PATTERN_DIVIDE_X;	// テクスチャの幅
				float th = 1.0f / TEXTURE_PATTERN_DIVIDE_Y;	// テクスチャの高さ
				float tx = tw * enemy[i].patternAnim;// テクスチャの左上X座標
				float ty = th * enemy[i].direction;// テクスチャの左上Y座標

				float r = enemy[i].color_R;
				float g = enemy[i].color_G;
				float b = enemy[i].color_B;

				float alpha = (enemy[i].invincible_time / 10) % 2 == 0 ? 1.0f : 0.3f;	//無敵時間は点滅

				float e_rad = enemy[i].rad;

				SetSpriteColorRotation(g_VertexBuffer, px, py, pw, ph, tx, ty, tw, th, D3DXCOLOR(r,g,b,alpha), e_rad);

				// ポリゴン描画
				GetDeviceContext()->Draw(4, 0);
			}
	}
	
}


//=============================================================================
// メイジの座標を取得
//=============================================================================
ENEMY *GetEnemy(void)
{
	return(&enemy[0]);
}
