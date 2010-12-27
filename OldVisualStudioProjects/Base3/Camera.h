#pragma once
#include <windows.h>
#include "Common.h"
#include "Mathlib/vector.h"
#include "Mathlib/matrix.h"

//--------------------------------------------------------------------------------------
// used by CCamera to map WM_KEYDOWN keys
//--------------------------------------------------------------------------------------
enum CameraKeys
{
    CAM_STRAFE_LEFT = 0,
    CAM_STRAFE_RIGHT,
    CAM_MOVE_FORWARD,
    CAM_MOVE_BACKWARD,
    CAM_MOVE_UP,
    CAM_MOVE_DOWN,
    CAM_RESET,
    CAM_CONTROLDOWN,
    CAM_MAX_KEYS,
    CAM_UNKNOWN = 0xFF
};

// TODO enum
#define KEY_WAS_DOWN_MASK 0x80
#define KEY_IS_DOWN_MASK  0x01

#define MOUSE_LEFT_BUTTON   0x01
#define MOUSE_MIDDLE_BUTTON 0x02
#define MOUSE_RIGHT_BUTTON  0x04
#define MOUSE_WHEEL         0x08


//--------------------------------------------------------------------------------------
// Simple base camera class that moves and rotates.  The base class
//       records mouse and keyboard input for use by a derived class, and 
//       keeps common state.
//--------------------------------------------------------------------------------------
class Camera
{
public:
    Camera();

    // Call these from client and use Get*Matrix() to read new matrices
    virtual LRESULT HandleMessages( HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam );
    virtual void    FrameMove( float fElapsedTime ) = 0;

    // Functions to change camera matrices
    virtual void Reset(); 
    virtual void SetViewParams( const vector3 &vEyePt, const vector3 &vLookatPt );
    virtual void SetProjParams( float fFOV, float fAspect, float fNearPlane, float fFarPlane );

    // Functions to change behavior
    virtual void SetDragRect( RECT &rc ) { m_rcDrag = rc; }
    void SetInvertPitch( bool bInvertPitch ) { m_bInvertPitch = bInvertPitch; }
    void SetDrag( bool bMovementDrag, float fTotalDragTimeToZero = 0.25f ) { m_bMovementDrag = bMovementDrag; m_fTotalDragTimeToZero = fTotalDragTimeToZero; }
    void SetEnableYAxisMovement( bool bEnableYAxisMovement ) { m_bEnableYAxisMovement = bEnableYAxisMovement; }
    void SetEnablePositionMovement( bool bEnablePositionMovement ) { m_bEnablePositionMovement = bEnablePositionMovement; }
    void SetClipToBoundary( bool bClipToBoundary, vector3* pvMinBoundary, vector3* pvMaxBoundary ) { m_bClipToBoundary = bClipToBoundary; if( pvMinBoundary ) m_vMinBoundary = *pvMinBoundary; if( pvMaxBoundary ) m_vMaxBoundary = *pvMaxBoundary; }
    void SetScalers( float fRotationScaler = 0.01f, float fMoveScaler = 5.0f )  { m_fRotationScaler = fRotationScaler; m_fMoveScaler = fMoveScaler; }
    void SetNumberOfFramesToSmoothMouseData( int nFrames ) { if( nFrames > 0 ) m_fFramesToSmoothMouseData = (float)nFrames; }
    void SetResetCursorAfterMove( bool bResetCursorAfterMove ) { m_bResetCursorAfterMove = bResetCursorAfterMove; }

    // Functions to get state
    const matrix44*  GetViewMatrix() const { return &m_mView; }
    const matrix44*  GetProjMatrix() const { return &m_mProj; }
    const vector3* GetEyePt() const      { return &m_vEye; }
    const vector3* GetLookAtPt() const   { return &m_vLookAt; }

    bool IsBeingDragged() { return (m_bMouseLButtonDown || m_bMouseMButtonDown || m_bMouseRButtonDown); }
    bool IsMouseLButtonDown() { return m_bMouseLButtonDown; } 
    bool IsMouseMButtonDown() { return m_bMouseMButtonDown; } 
    bool IsMouseRButtonDown() { return m_bMouseRButtonDown; } 

protected:
    // Functions to map a WM_KEYDOWN key to a CameraKeys enum
    virtual CameraKeys MapKey( UINT nKey );    
    bool IsKeyDown( uchar key )  { return( (key & KEY_IS_DOWN_MASK) == KEY_IS_DOWN_MASK ); }
    bool WasKeyDown( uchar key ) { return( (key & KEY_WAS_DOWN_MASK) == KEY_WAS_DOWN_MASK ); }

    void ConstrainToBoundary( vector3* pV );
    void UpdateMouseDelta( float fElapsedTime );
    void UpdateVelocity( float fElapsedTime );

    matrix44            m_mView;              // View matrix 
    matrix44            m_mProj;              // Projection matrix

    uchar                  m_aKeys[CAM_MAX_KEYS];  // State of input - KEY_WAS_DOWN_MASK|KEY_IS_DOWN_MASK
    POINT                 m_ptLastMousePosition;  // Last absolute position of mouse cursor
    bool                  m_bMouseLButtonDown;    // True if left button is down 
    bool                  m_bMouseMButtonDown;    // True if middle button is down 
    bool                  m_bMouseRButtonDown;    // True if right button is down 
    int                   m_nCurrentButtonMask;   // mask of which buttons are down
    int                   m_nMouseWheelDelta;     // Amount of middle wheel scroll (+/-) 
    vector2           m_vMouseDelta;          // Mouse relative delta smoothed over a few frames
    float                 m_fFramesToSmoothMouseData; // Number of frames to smooth mouse data over

    vector3           m_vDefaultEye;          // Default camera eye position
    vector3           m_vDefaultLookAt;       // Default LookAt position
    vector3           m_vEye;                 // Camera eye position
    vector3           m_vLookAt;              // LookAt position
    float                 m_fCameraYawAngle;      // Yaw angle of camera
    float                 m_fCameraPitchAngle;    // Pitch angle of camera

    RECT                  m_rcDrag;               // Rectangle within which a drag can be initiated.
    vector3           m_vVelocity;            // Velocity of camera
    bool                  m_bMovementDrag;        // If true, then camera movement will slow to a stop otherwise movement is instant
    vector3           m_vVelocityDrag;        // Velocity drag force
    float                 m_fDragTimer;           // Countdown timer to apply drag
    float                 m_fTotalDragTimeToZero; // Time it takes for velocity to go from full to 0
    vector2           m_vRotVelocity;         // Velocity of camera

    float                 m_fFOV;                 // Field of view
    float                 m_fAspect;              // Aspect ratio
    float                 m_fNearPlane;           // Near plane
    float                 m_fFarPlane;            // Far plane

    float                 m_fRotationScaler;      // Scaler for rotation
    float                 m_fMoveScaler;          // Scaler for movement

    bool                  m_bInvertPitch;         // Invert the pitch axis
    bool                  m_bEnablePositionMovement; // If true, then the user can translate the camera/model 
    bool                  m_bEnableYAxisMovement; // If true, then camera can move in the y-axis

    bool                  m_bClipToBoundary;      // If true, then the camera will be clipped to the boundary
    vector3           m_vMinBoundary;         // Min point in clip boundary
    vector3           m_vMaxBoundary;         // Max point in clip boundary

    bool                  m_bResetCursorAfterMove;// If true, the class will reset the cursor position so that the cursor always has space to move 
};

