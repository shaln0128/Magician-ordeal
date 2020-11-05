//=============================================================================
//
// �J�������� [camera.cpp]
// Author :  GP11B132 36 ���c�x��
//
//=============================================================================
#include "main.h"
#include "renderer.h"
#include "camera.h"
#include "player.h"



D3DXMATRIX				g_CameraViewMatrix;
D3DXMATRIX				g_CameraInvViewMatrix;
D3DXMATRIX				g_CameraProjectionMatrix;

D3DXVECTOR3				g_CameraPosition;
D3DXVECTOR3				g_CameraTarget;

D3DXVECTOR3				g_CameraRotation;



D3DXMATRIX GetCameraViewMatrix()
{
	return g_CameraViewMatrix;
}

D3DXMATRIX GetCameraInvViewMatrix()
{
	return g_CameraInvViewMatrix;
}

D3DXMATRIX GetCameraProjectionMatrix()
{
	return g_CameraProjectionMatrix;
}

D3DXVECTOR3 GetCameraPosition()
{
	return g_CameraPosition;
}

//=============================================================================
// ����������
//=============================================================================
void InitCamera(void)
{
	/*g_CameraPosition = D3DXVECTOR3( 0.0f, 50.0f, -100.0f );
	g_CameraTarget = D3DXVECTOR3( 0.0f, 0.0f, 0.0f );
	g_CameraRotation = D3DXVECTOR3( 0.0f, 0.0f, 0.0f );*/
}


//=============================================================================
// �I������
//=============================================================================
void UninitCamera(void)
{


}


//=============================================================================
// �X�V����
//=============================================================================
void UpdateCamera(void)
{
	if( GetKeyState( VK_LEFT ) & 0x80 )
	{
		g_CameraRotation.y -= 0.02f;
	}
	else if( GetKeyState( VK_RIGHT ) & 0x80 )
	{
		g_CameraRotation.y += 0.02f;
	}



	D3DXVECTOR3 playerPosition;

	PLAYER *player = GetPlayer(); 

	playerPosition = player[0].pos;

	g_CameraTarget.x = playerPosition.x * 0.5f;
	g_CameraTarget.z = playerPosition.z * 0.5f;

	g_CameraPosition.x = g_CameraTarget.x + sinf( g_CameraRotation.y ) * 100.0f;
	g_CameraPosition.z = g_CameraTarget.z - cosf( g_CameraRotation.y ) * 100.0f;

}


//=============================================================================
// �`�揈��
//=============================================================================
void SetCamera(void)
{

	// �r���[�}�g���b�N�X�ݒ�
	D3DXMatrixLookAtLH( &g_CameraViewMatrix, &g_CameraPosition, &g_CameraTarget, &D3DXVECTOR3(0.0f, 1.0f, 0.0f) );

	SetViewMatrix( &g_CameraViewMatrix );


	float det;
	D3DXMatrixInverse(&g_CameraInvViewMatrix, &det, &g_CameraViewMatrix);


	// �v���W�F�N�V�����}�g���b�N�X�ݒ�
	D3DXMatrixPerspectiveFovLH( &g_CameraProjectionMatrix, 1.0f, (float)SCREEN_WIDTH / (float)SCREEN_HEIGHT, 1.0f, 1000.0f);

	SetProjectionMatrix( &g_CameraProjectionMatrix );

}



