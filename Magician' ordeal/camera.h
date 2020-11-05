//=============================================================================
//
// �J�����̏��� [camera.h]
// Author : GP11B132 36 ���c�x��
//
//=============================================================================
#pragma once



//*****************************************************************************
// �v���g�^�C�v�錾
//*****************************************************************************
void InitCamera(void);
void UninitCamera(void);
void UpdateCamera(void);
void SetCamera(void);

D3DXMATRIX GetCameraViewMatrix();
D3DXMATRIX GetCameraInvViewMatrix();
D3DXMATRIX GetCameraProjectionMatrix();
D3DXVECTOR3 GetCameraPosition();
