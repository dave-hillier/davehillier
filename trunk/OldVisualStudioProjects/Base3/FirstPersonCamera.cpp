#include "FirstPersonCamera.h"
#include "Timer.h"
#include "DebugLog.h"
#include <gl/gl.h>

#ifdef max
#undef max
#endif
#ifdef min
#undef min
#endif

#include <xutility>


//--------------------------------------------------------------------------------------
// Constructor
//--------------------------------------------------------------------------------------
FirstPersonCamera::FirstPersonCamera() :
    m_nActiveButtonMask( 0x07 )
{
}




//--------------------------------------------------------------------------------------
// Update the view matrix based on user input & elapsed time
//--------------------------------------------------------------------------------------
void FirstPersonCamera::FrameMove( float fElapsedTime )
{
//	if( Base::Timer::GlobalTimer().IsStopped() )         
//        fElapsedTime = static_cast<float>(Base::Timer::GlobalTimer().FrameDelta());

    if( IsKeyDown(m_aKeys[CAM_RESET]) )
        Reset();

    // Get the mouse movement (if any) if the mouse button are down
    if( m_nActiveButtonMask & m_nCurrentButtonMask )
        UpdateMouseDelta( fElapsedTime );

    // Get amount of velocity based on the keyboard input and drag (if any)
    UpdateVelocity( fElapsedTime );

    // Simple euler method to calculate position delta
    vector3 vPosDelta = m_vVelocity * fElapsedTime;

    // If rotating the camera 
    if( m_nActiveButtonMask & m_nCurrentButtonMask )
    {
        // Update the pitch & yaw angle based on mouse movement
        float fYawDelta   = m_vRotVelocity.x;
        float fPitchDelta = m_vRotVelocity.y;

        // Invert pitch if requested
        if( m_bInvertPitch )
            fPitchDelta = -fPitchDelta;

        m_fCameraPitchAngle += fPitchDelta;
        m_fCameraYawAngle   += fYawDelta;

        // Limit pitch to straight up or straight down
		m_fCameraPitchAngle = std::max( -floatPi/2.0f,  m_fCameraPitchAngle );
		m_fCameraPitchAngle = std::min( +floatPi/2.0f,  m_fCameraPitchAngle );
    }

    // Make a rotation matrix based on the camera's yaw & pitch
    matrix44 mCameraRot;
	mCameraRot.rotate_y( m_fCameraYawAngle );
	mCameraRot.rotate_x( m_fCameraPitchAngle );
	mCameraRot.rotate_z( 0 );

    // Transform vectors based on camera's rotation matrix
    vector3 vLocalUp    = vector3(0,1,0);
    vector3 vLocalAhead = vector3(0,0,1);
	vector3 vWorldUp = mCameraRot.transform_coord( vLocalUp );
	vector3 vWorldAhead = mCameraRot.transform_coord( vLocalAhead );

    // Transform the position delta by the camera's rotation 
    vector3 vPosDeltaWorld = mCameraRot.transform_coord(vPosDelta);
	vPosDeltaWorld.x *= -1;
	//vPosDeltaWorld.y *= -1;

//	Base::debugLog << "y delta " << vPosDeltaWorld.y << std::endl;

    if( !m_bEnableYAxisMovement )
        vPosDeltaWorld.y = 0.0f;

    // Move the eye position 
    m_vEye += vPosDeltaWorld;
    if( m_bClipToBoundary )
        ConstrainToBoundary( &m_vEye );

    // Update the lookAt position based on the eye position 
    m_vLookAt = m_vEye + vWorldAhead;

    // Update the view matrix
	m_mView.ident();
	m_mView.set_translation ( m_vEye );
	//m_mView.lookatRh( m_vLookAt, vLocalUp );
	m_mView *= mCameraRot;

	m_mCameraWorld = m_mView;
	m_mCameraWorld.invert();
}


//--------------------------------------------------------------------------------------
// Enable or disable each of the mouse buttons for rotation drag.
//--------------------------------------------------------------------------------------
void FirstPersonCamera::SetRotateButtons( bool bLeft, bool bMiddle, bool bRight )
{
    m_nActiveButtonMask = ( bLeft ? MOUSE_LEFT_BUTTON : 0 ) |
                          ( bMiddle ? MOUSE_MIDDLE_BUTTON : 0 ) |
                          ( bRight ? MOUSE_RIGHT_BUTTON : 0 );
}