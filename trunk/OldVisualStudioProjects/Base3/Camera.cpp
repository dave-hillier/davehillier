#include "Camera.h"
#include <limits.h>

#define WM_MOUSEWHEEL 0x020A // TODO Why isnt this here?

//--------------------------------------------------------------------------------------
// Constructor
//--------------------------------------------------------------------------------------
Camera::Camera()
{
    ZeroMemory( m_aKeys, sizeof(uchar)*CAM_MAX_KEYS );

    // Set attributes for the view matrix
    vector3 vEyePt    = vector3(0.0f,0.0f,0.0f);
    vector3 vLookatPt = vector3(0.0f,0.0f,1.0f);

    // Setup the view matrix
    SetViewParams( vEyePt, vLookatPt );

    // Setup the projection matrix
	SetProjParams( floatPi/4.0f, 4.f/3.f, 1.0f, 1000.0f ); // TODO Aspect on screen res

    GetCursorPos( &m_ptLastMousePosition );
    m_bMouseLButtonDown = false;
    m_bMouseMButtonDown = false;
    m_bMouseRButtonDown = false;
    m_nCurrentButtonMask = 0;
    m_nMouseWheelDelta = 0;

    m_fCameraYawAngle = 0.0f;
    m_fCameraPitchAngle = 0.0f;

    SetRect( &m_rcDrag, LONG_MIN, LONG_MIN, LONG_MAX, LONG_MAX );
    m_vVelocity     = vector3(0,0,0);
    m_bMovementDrag = false;
    m_vVelocityDrag = vector3(0,0,0);
    m_fDragTimer    = 0.0f;
    m_fTotalDragTimeToZero = 0.25;
    m_vRotVelocity = vector2(0,0);

    m_fRotationScaler = 0.01f;           
    m_fMoveScaler = 5.0f;           

    m_bInvertPitch = false;
    m_bEnableYAxisMovement = true;
    m_bEnablePositionMovement = true;

    m_vMouseDelta   = vector2(0,0);
    m_fFramesToSmoothMouseData = 2.0f;

    m_bClipToBoundary = false;
    m_vMinBoundary = vector3(-1,-1,-1);
    m_vMaxBoundary = vector3(1,1,1);

    m_bResetCursorAfterMove = false;
}


//--------------------------------------------------------------------------------------
// Client can call this to change the position and direction of camera
//--------------------------------------------------------------------------------------
void Camera::SetViewParams( const vector3 &vEyePt, const vector3 &vLookatPt )
{
    m_vDefaultEye = m_vEye = vEyePt;
    m_vDefaultLookAt = m_vLookAt = vLookatPt;

    // Calc the view matrix
    vector3 vUp(0,1,0);
	m_mView.ident();
	m_mView.set_translation ( m_vEye );
	//m_mView.lookatRh( m_vLookAt, vUp );
    matrix44 mCameraRot;
	mCameraRot.rotate_y( m_fCameraYawAngle );
	mCameraRot.rotate_x( m_fCameraPitchAngle );
	mCameraRot.rotate_z( 0 );

	m_mView *= mCameraRot;

	matrix44 mInvView;
	mInvView = m_mView;
	mInvView.invert();

    // The axis basis vectors and camera position are stored inside the 
    // position matrix in the 4 rows of the camera's world matrix.
    // To figure out the yaw/pitch of the camera, we just need the Z basis vector
    vector3* pZBasis = (vector3*) &mInvView.M31;

    m_fCameraYawAngle   = atan2f( pZBasis->x, pZBasis->z );
    float fLen = sqrtf(pZBasis->z*pZBasis->z + pZBasis->x*pZBasis->x);
    m_fCameraPitchAngle = atan2f( pZBasis->y, fLen );
}




//--------------------------------------------------------------------------------------
// Calculates the projection matrix based on input params
//--------------------------------------------------------------------------------------
void Camera::SetProjParams( float fFOV, float fAspect, float fNearPlane,
                                   float fFarPlane )
{
    // Set attributes for the projection matrix
    m_fFOV        = fFOV;
    m_fAspect     = fAspect;
    m_fNearPlane  = fNearPlane;
    m_fFarPlane   = fFarPlane;

    m_mProj.perspFovRh( fFOV, fAspect, fNearPlane, fFarPlane );
}




//--------------------------------------------------------------------------------------
// Call this from your message proc so this class can handle window messages
//--------------------------------------------------------------------------------------
LRESULT Camera::HandleMessages( HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam )
{
    UNREFERENCED_PARAMETER( hWnd );
    UNREFERENCED_PARAMETER( lParam );

    switch( uMsg )
    {
        case WM_KEYDOWN:
        {
            // Map this key to a CameraKeys enum and update the
            // state of m_aKeys[] by adding the KEY_WAS_DOWN_MASK|KEY_IS_DOWN_MASK mask
            // only if the key is not down
            CameraKeys mappedKey = MapKey( (UINT)wParam );
            if( mappedKey != CAM_UNKNOWN )
            {
                if( FALSE == IsKeyDown(m_aKeys[mappedKey]) )
                    m_aKeys[ mappedKey ] = KEY_WAS_DOWN_MASK | KEY_IS_DOWN_MASK;
            }
            break;
        }

        case WM_KEYUP:
        {
            // Map this key to a CameraKeys enum and update the
            // state of m_aKeys[] by removing the KEY_IS_DOWN_MASK mask.
            CameraKeys mappedKey = MapKey( (UINT)wParam );
            if( mappedKey != CAM_UNKNOWN )
                m_aKeys[ mappedKey ] &= ~KEY_IS_DOWN_MASK;
            break;
        }

        case WM_RBUTTONDOWN:
        case WM_MBUTTONDOWN:
        case WM_LBUTTONDOWN:
        case WM_RBUTTONDBLCLK:
        case WM_MBUTTONDBLCLK:
        case WM_LBUTTONDBLCLK:
        {
            // Compute the drag rectangle in screen coord.
            POINT ptCursor = { (short)LOWORD(lParam), (short)HIWORD(lParam) };

            // Update member var state
            if( ( uMsg == WM_LBUTTONDOWN || uMsg == WM_LBUTTONDBLCLK ) && PtInRect( &m_rcDrag, ptCursor ) )
                { m_bMouseLButtonDown = true; m_nCurrentButtonMask |= MOUSE_LEFT_BUTTON; }
            if( ( uMsg == WM_MBUTTONDOWN || uMsg == WM_MBUTTONDBLCLK ) && PtInRect( &m_rcDrag, ptCursor ) )
                { m_bMouseMButtonDown = true; m_nCurrentButtonMask |= MOUSE_MIDDLE_BUTTON; }
            if( ( uMsg == WM_RBUTTONDOWN || uMsg == WM_RBUTTONDBLCLK ) && PtInRect( &m_rcDrag, ptCursor ) )
                { m_bMouseRButtonDown = true; m_nCurrentButtonMask |= MOUSE_RIGHT_BUTTON; }

            // Capture the mouse, so if the mouse button is 
            // released outside the window, we'll get the WM_LBUTTONUP message
            SetCapture(hWnd);
            GetCursorPos( &m_ptLastMousePosition );
            return TRUE;
        }

        case WM_RBUTTONUP: 
        case WM_MBUTTONUP: 
        case WM_LBUTTONUP:   
        {
            // Update member var state
            if( uMsg == WM_LBUTTONUP ) { m_bMouseLButtonDown = false; m_nCurrentButtonMask &= ~MOUSE_LEFT_BUTTON; }
            if( uMsg == WM_MBUTTONUP ) { m_bMouseMButtonDown = false; m_nCurrentButtonMask &= ~MOUSE_MIDDLE_BUTTON; }
            if( uMsg == WM_RBUTTONUP ) { m_bMouseRButtonDown = false; m_nCurrentButtonMask &= ~MOUSE_RIGHT_BUTTON; }

            // Release the capture if no mouse buttons down
            if( !m_bMouseLButtonDown  && 
                !m_bMouseRButtonDown &&
                !m_bMouseMButtonDown )
            {
                ReleaseCapture();
            }
            break;
        }

        case WM_MOUSEWHEEL: 
            // Update member var state
            m_nMouseWheelDelta = (short)HIWORD(wParam) / 120;
            break;
    }

    return FALSE;
}




//--------------------------------------------------------------------------------------
// Figure out the mouse delta based on mouse movement
//--------------------------------------------------------------------------------------
void Camera::UpdateMouseDelta( float fElapsedTime )
{
    UNREFERENCED_PARAMETER( fElapsedTime );

    POINT ptCurMouseDelta;
    POINT ptCurMousePos;
    
    // Get current position of mouse
    GetCursorPos( &ptCurMousePos );

    // Calc how far it's moved since last frame
    ptCurMouseDelta.x = ptCurMousePos.x - m_ptLastMousePosition.x;
    ptCurMouseDelta.y = ptCurMousePos.y - m_ptLastMousePosition.y;

    // Record current position for next time
    m_ptLastMousePosition = ptCurMousePos;

    if( m_bResetCursorAfterMove )
    {
        // Set position of camera to center of desktop, 
        // so it always has room to move.  This is very useful
        // if the cursor is hidden.  If this isn't done and cursor is hidden, 
        // then invisible cursor will hit the edge of the screen 
        // and the user can't tell what happened
        POINT ptCenter;
        RECT rcDesktop;
        GetWindowRect( GetDesktopWindow(), &rcDesktop );
        ptCenter.x = (rcDesktop.right - rcDesktop.left) / 2;
        ptCenter.y = (rcDesktop.bottom - rcDesktop.top) / 2;   
        SetCursorPos( ptCenter.x, ptCenter.y );
        m_ptLastMousePosition = ptCenter;
    }

    // Smooth the relative mouse data over a few frames so it isn't 
    // jerky when moving slowly at low frame rates.
    float fPercentOfNew =  1.0f / m_fFramesToSmoothMouseData;
    float fPercentOfOld =  1.0f - fPercentOfNew;
    m_vMouseDelta.x = m_vMouseDelta.x*fPercentOfOld + ptCurMouseDelta.x*fPercentOfNew;
    m_vMouseDelta.y = m_vMouseDelta.y*fPercentOfOld + ptCurMouseDelta.y*fPercentOfNew;

    m_vRotVelocity = m_vMouseDelta * m_fRotationScaler;
}




//--------------------------------------------------------------------------------------
// Figure out the velocity based on keyboard input & drag if any
//--------------------------------------------------------------------------------------
void Camera::UpdateVelocity( float fElapsedTime )
{
    matrix44 mRotDelta;
    vector3 vAccel = vector3(0,0,0);

    if( m_bEnablePositionMovement )
    {
        // Update acceleration vector based on keyboard state
        if( IsKeyDown(m_aKeys[CAM_MOVE_FORWARD]) )
            vAccel.z += 1.0f;
        if( IsKeyDown(m_aKeys[CAM_MOVE_BACKWARD]) )
            vAccel.z -= 1.0f;
        if( m_bEnableYAxisMovement )
        {
            if( IsKeyDown(m_aKeys[CAM_MOVE_UP]) )
                vAccel.y += 1.0f;
            if( IsKeyDown(m_aKeys[CAM_MOVE_DOWN]) )
                vAccel.y -= 1.0f;
        }
        if( IsKeyDown(m_aKeys[CAM_STRAFE_RIGHT]) )
            vAccel.x += 1.0f;
        if( IsKeyDown(m_aKeys[CAM_STRAFE_LEFT]) )
            vAccel.x -= 1.0f;
    }

    // Normalize vector so if moving 2 dirs (left & forward), 
    // the camera doesn't move faster than if moving in 1 dir
	vAccel.norm();

    // Scale the acceleration vector
    vAccel *= m_fMoveScaler;

    if( m_bMovementDrag )
    {
        // Is there any acceleration this frame?
        if( vAccel.lensquared() > 0 )
        {
            // If so, then this means the user has pressed a movement key\
            // so change the velocity immediately to acceleration 
            // upon keyboard input.  This isn't normal physics
            // but it will give a quick response to keyboard input
            m_vVelocity = vAccel;
            m_fDragTimer = m_fTotalDragTimeToZero;
            m_vVelocityDrag = vAccel / m_fDragTimer;
        }
        else 
        {
            // If no key being pressed, then slowly decrease velocity to 0
            if( m_fDragTimer > 0 )
            {
                // Drag until timer is <= 0
                m_vVelocity -= m_vVelocityDrag * fElapsedTime;
                m_fDragTimer -= fElapsedTime;
            }
            else
            {
                // Zero velocity
                m_vVelocity = vector3(0,0,0);
            }
        }
    }
    else
    {
        // No drag, so immediately change the velocity
        m_vVelocity = vAccel;
    }
}




//--------------------------------------------------------------------------------------
// Clamps pV to lie inside m_vMinBoundary & m_vMaxBoundary
//--------------------------------------------------------------------------------------
void Camera::ConstrainToBoundary( vector3* pV )
{
    // Constrain vector to a bounding box 
    pV->x = max(pV->x, m_vMinBoundary.x);
    pV->y = max(pV->y, m_vMinBoundary.y);
    pV->z = max(pV->z, m_vMinBoundary.z);

    pV->x = min(pV->x, m_vMaxBoundary.x);
    pV->y = min(pV->y, m_vMaxBoundary.y);
    pV->z = min(pV->z, m_vMaxBoundary.z);
}




//--------------------------------------------------------------------------------------
// Maps a windows virtual key to an enum
//--------------------------------------------------------------------------------------
CameraKeys Camera::MapKey( UINT nKey )
{
    // This could be upgraded to a method that's user-definable but for 
    // simplicity, we'll use a hardcoded mapping.
    switch( nKey )
    {
        case VK_CONTROL:  return CAM_CONTROLDOWN;
        case VK_LEFT:  return CAM_STRAFE_LEFT;
        case VK_RIGHT: return CAM_STRAFE_RIGHT;
        case VK_UP:    return CAM_MOVE_FORWARD;
        case VK_DOWN:  return CAM_MOVE_BACKWARD;
        case VK_PRIOR: return CAM_MOVE_UP;        // pgup
        case VK_NEXT:  return CAM_MOVE_DOWN;      // pgdn

        case 'A':      return CAM_STRAFE_LEFT;
        case 'D':      return CAM_STRAFE_RIGHT;
        case 'W':      return CAM_MOVE_FORWARD;
        case 'S':      return CAM_MOVE_BACKWARD;
        case 'Q':      return CAM_MOVE_DOWN;
        case 'E':      return CAM_MOVE_UP;

        case VK_NUMPAD4: return CAM_STRAFE_LEFT;
        case VK_NUMPAD6: return CAM_STRAFE_RIGHT;
        case VK_NUMPAD8: return CAM_MOVE_FORWARD;
        case VK_NUMPAD2: return CAM_MOVE_BACKWARD;
        case VK_NUMPAD9: return CAM_MOVE_UP;        
        case VK_NUMPAD3: return CAM_MOVE_DOWN;      

        case VK_HOME:   return CAM_RESET;
    }

    return CAM_UNKNOWN;
}




//--------------------------------------------------------------------------------------
// Reset the camera's position back to the default
//--------------------------------------------------------------------------------------
void Camera::Reset()
{
    SetViewParams( m_vDefaultEye, m_vDefaultLookAt );
}
