//====== Copyright © 1996-2004, Valve Corporation, All rights reserved. =======
//
// Purpose: Shared player code.
//
//=============================================================================
#ifndef CS_PLAYER_SHARED_H
#define CS_PLAYER_SHARED_H
#ifdef _WIN32
#pragma once
#endif

#include "networkvar.h"
#include "cs_shareddefs.h"
#include "weapon_csbase.h"
#include "basegrenade_shared.h"

#ifdef CLIENT_DLL
	class C_CSPlayer;
#else
	class CCSPlayer;
#endif

//=============================================================================
//
// Tables.
//
#ifdef CLIENT_DLL
	EXTERN_RECV_TABLE( DT_CSPlayerShared );
#else
	EXTERN_SEND_TABLE( DT_CSPlayerShared );
#endif

//=============================================================================
//
// Shared player class.
//
class CCSPlayerShared
{
public:

#ifdef CLIENT_DLL
	friend class C_CSPlayer;
	typedef C_CSPlayer OuterClass;
	DECLARE_PREDICTABLE();
#else
	friend class CCSPlayer;
	typedef CCSPlayer OuterClass;
#endif
	
	DECLARE_EMBEDDED_NETWORKVAR()
	DECLARE_CLASS_NOBASE( CCSPlayerShared );

	// Initialization.
	CCSPlayerShared();

	virtual void Init( OuterClass *pOuter );

	CWeaponCSBase *GetActiveCSWeapon() const;

	// Utils.
	bool	IsDucking( void ) const; 
	bool	IsOnGround( void ) const;
	bool	IsOnGodMode() const;
	int		GetButtons();
	bool	IsButtonPressing( int btn );
	bool	IsButtonPressed( int btn );
	bool	IsButtonReleased( int btn );

private:

	OuterClass *m_pOuter;
};			   

#endif // CS_PLAYER_SHARED_H
