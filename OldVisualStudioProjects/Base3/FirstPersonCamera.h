#pragma once
//--------------------------------------------------------------------------------------
// Simple first person camera class that moves and rotates.
//       It allows yaw and pitch but not roll.  It uses WM_KEYDOWN and 
//       GetCursorPos() to respond to keyboard and mouse input and updates the 
//       view matrix based on input.  
//--------------------------------------------------------------------------------------

#include "Camera.h"

class FirstPersonCamera : public Camera
{
public:
    FirstPersonCamera();

    // Call these from client and use Get*Matrix() to read new matrices
    virtual void FrameMove( float fElapsedTime );

    // Functions to change behavior
    void SetRotateButtons( bool bLeft, bool bMiddle, bool bRight );

    // Functions to get state
    matrix44*  GetWorldMatrix()            { return &m_mCameraWorld; }

    const vector3* GetWorldRight() const { return (vector3*)&m_mCameraWorld.M11; } 
    const vector3* GetWorldUp() const    { return (vector3*)&m_mCameraWorld.M21; }
    const vector3* GetWorldAhead() const { return (vector3*)&m_mCameraWorld.M31; }
    const vector3* GetEyePt() const      { return (vector3*)&m_mCameraWorld.M41; }

protected:
    matrix44 m_mCameraWorld;       // World matrix of the camera (inverse of the view matrix)

    int        m_nActiveButtonMask;  // Mask to determine which button to enable for rotation
};
