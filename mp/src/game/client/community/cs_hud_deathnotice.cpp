//========= Copyright © 1996-2005, Valve Corporation, All rights reserved. ============//
//
// Purpose: Draws CSPort's death notices
//
// $NoKeywords: $
//=============================================================================//

#include "cbase.h"
#include "hudelement.h"
#include "hud_macros.h"
#include "c_playerresource.h"
#include "iclientmode.h"
#include <vgui_controls/Controls.h>
#include <vgui_controls/Panel.h>
#include <vgui/ISurface.h>
#include <vgui/ILocalize.h>
#include <KeyValues.h>
#include "c_baseplayer.h"
#include "c_team.h"
#include "hud_basedeathnotice.h"
#include "cs_shareddefs.h"
#include "clientmode_csnormal.h"
#include "c_cs_player.h"

// memdbgon must be the last include file in a .cpp file!!!
#include "tier0/memdbgon.h"

class CCSHudDeathNotice : public CHudBaseDeathNotice
{
	DECLARE_CLASS_SIMPLE( CCSHudDeathNotice, CHudBaseDeathNotice );
public:
	CCSHudDeathNotice( const char *pElementName ) : CHudBaseDeathNotice( pElementName ) {};
	virtual void Paint( void );

	virtual Color GetInfoTextColor(int iDeathNoticeMsg, bool bLocalPlayerInvolved){ return bLocalPlayerInvolved ? Color(0, 0, 0, 255) : Color(255, 255, 255, 255); }

protected:	
	virtual void OnGameEvent( IGameEvent *event, int iDeathNoticeMsg );
	virtual Color GetTeamColor( int iTeamNumber, bool bLocalPlayerInvolved /* = false */ );

private:
	void AddAdditionalMsg( int iKillerID, int iVictimID, const char *pMsgKey );

	CPanelAnimationVar( Color, m_clrCTText, "CTTextColor", "CT_Blue" );
	CPanelAnimationVar( Color, clrTERRText, "TerroristTextColor", "T_Red" );

};

DECLARE_HUDELEMENT( CCSHudDeathNotice );

//-----------------------------------------------------------------------------
// Purpose: Called when a game event happens and a death notice is about to be 
//			displayed.  This method can examine the event and death notice and
//			make game-specific tweaks to it before it is displayed
//-----------------------------------------------------------------------------
void CCSHudDeathNotice::OnGameEvent(IGameEvent *event, int iDeathNoticeMsg)
{
	const char *pszEventName = event->GetName();

	if ( FStrEq( pszEventName, "player_death" ) )
	{
		int iCustomDamage = event->GetInt( "customkill" );

		//const wchar_t *pMsg = NULL;
		switch ( iCustomDamage )
		{
		case DMG_HEADSHOT:
			Q_strncpy(m_DeathNotices[iDeathNoticeMsg].szIcon, "d_headshot", ARRAYSIZE(m_DeathNotices[iDeathNoticeMsg].szIcon));
			break;
		/*case DMG_SUICIDE:
			{
				// display a different message if this was suicide, or assisted suicide (suicide w/recent damage, kill awarded to damager)
				bool bAssistedSuicide = event->GetInt( "userid" ) != event->GetInt( "attacker" );
				pMsg = g_pVGuiLocalize->Find( bAssistedSuicide ? "#DeathMsg_AssistedSuicide" : "#DeathMsg_Suicide" );
				if ( pMsg )
				{
					V_wcsncpy(m_DeathNotices[iDeathNoticeMsg].wzInfoText, pMsg, sizeof(m_DeathNotices[iDeathNoticeMsg].wzInfoText));
				}			
				break;
			}*/
		default:
			break;
		}
	} 
}

//-----------------------------------------------------------------------------
// Purpose: Adds an additional death message
//-----------------------------------------------------------------------------
void CCSHudDeathNotice::AddAdditionalMsg( int iKillerID, int iVictimID, const char *pMsgKey )
{
	DeathNoticeItem &msg2 = m_DeathNotices[AddDeathNoticeItem()];
	Q_strncpy( msg2.Killer.szName, g_PR->GetPlayerName( iKillerID ), ARRAYSIZE( msg2.Killer.szName ) );
	Q_strncpy( msg2.Victim.szName, g_PR->GetPlayerName( iVictimID ), ARRAYSIZE( msg2.Victim.szName ) );
	
	msg2.Killer.iTeam = g_PR->GetTeam(iKillerID);
	msg2.Victim.iTeam = g_PR->GetTeam(iVictimID);

	const wchar_t *wzMsg =  g_pVGuiLocalize->Find( pMsgKey );
	if ( wzMsg )
		V_wcsncpy( msg2.wzInfoText, wzMsg, sizeof( msg2.wzInfoText ) );

	int iLocalPlayerIndex = GetLocalPlayerIndex();
	if ( iLocalPlayerIndex == iVictimID || iLocalPlayerIndex == iKillerID )
		msg2.bLocalPlayerInvolved = true;
}

//-----------------------------------------------------------------------------
// Purpose: 
//-----------------------------------------------------------------------------
void CCSHudDeathNotice::Paint()
{
	// Retire any death notices that have expired
	RetireExpiredDeathNotices();

	CBaseViewport *pViewport = dynamic_cast<CBaseViewport *>( GetClientModeNormal()->GetViewport() );
	int yStart = pViewport->GetDeathMessageStartHeight();

	surface()->DrawSetTextFont( m_hTextFont );

	int xMargin = XRES( 10 );
	int xSpacing = UTIL_ComputeStringWidth( m_hTextFont, L" " );

	int iCount = m_DeathNotices.Count();
	for ( int i = 0; i < iCount; i++ )
	{
		DeathNoticeItem &msg = m_DeathNotices[i];
		
		CHudTexture *icon = msg.iconDeath;
		CHudTexture *iconPostKillerName = msg.iconPostKillerName;
		CHudTexture *iconPreKillerName = msg.iconPreKillerName;
		CHudTexture *iconPostVictimName = msg.iconPostVictimName;

		wchar_t victim[256] = L"";
		wchar_t killer[256] = L"";

		// TEMP - print the death icon name if we don't have a material for it

		g_pVGuiLocalize->ConvertANSIToUnicode( msg.Victim.szName, victim, sizeof( victim ) );
		g_pVGuiLocalize->ConvertANSIToUnicode( msg.Killer.szName, killer, sizeof( killer ) );

		int iVictimTextWide = UTIL_ComputeStringWidth( m_hTextFont, victim ) + xSpacing;
		int iDeathInfoTextWide= msg.wzInfoText[0] ? UTIL_ComputeStringWidth( m_hTextFont, msg.wzInfoText ) + xSpacing : 0;
		int iDeathInfoEndTextWide= msg.wzInfoTextEnd[0] ? UTIL_ComputeStringWidth( m_hTextFont, msg.wzInfoTextEnd ) + xSpacing : 0;

		int iKillerTextWide = killer[0] ? UTIL_ComputeStringWidth( m_hTextFont, killer ) + xSpacing : 0;
		int iLineTall = m_flLineHeight;
		int iTextTall = surface()->GetFontTall( m_hTextFont );
		int iconWide = 0, iconTall = 0, iDeathInfoOffset = 0, iVictimTextOffset = 0, iconActualWide = 0;

		int iPreKillerTextWide = msg.wzPreKillerText[0] ? UTIL_ComputeStringWidth( m_hTextFont, msg.wzPreKillerText ) - xSpacing : 0;
		
		int iconPrekillerWide = 0, iconPrekillerActualWide = 0, iconPrekillerTall = 0;
		int iconPostkillerWide = 0, iconPostkillerActualWide = 0, iconPostkillerTall = 0;

		int iconPostVictimWide = 0, iconPostVictimActualWide = 0, iconPostVictimTall = 0;

		// Get the local position for this notice
		if ( icon )
		{			
			iconActualWide = icon->EffectiveWidth( 1.0f );
			iconWide = iconActualWide + xSpacing;
			iconTall = icon->EffectiveHeight( 1.0f );
			
			int iconTallDesired = iLineTall-YRES(2);
			Assert( 0 != iconTallDesired );
			float flScale = (float) iconTallDesired / (float) iconTall;

			iconActualWide *= flScale;
			iconTall *= flScale;
			iconWide *= flScale;
		}

		if ( iconPreKillerName )
		{
			iconPrekillerActualWide = iconPreKillerName->EffectiveWidth( 1.0f );
			iconPrekillerWide = iconPrekillerActualWide;
			iconPrekillerTall = iconPreKillerName->EffectiveHeight( 1.0f );

			int iconTallDesired = iLineTall - YRES( 2 );
			Assert( 0 != iconTallDesired );
			float flScale = (float)iconTallDesired / (float)iconPrekillerTall;

			iconPrekillerActualWide *= flScale;
			iconPrekillerTall *= flScale;
			iconPrekillerWide *= flScale;
		}

		if ( iconPostKillerName )
		{
			iconPostkillerActualWide = iconPostKillerName->EffectiveWidth( 1.0f );
			iconPostkillerWide = iconPostkillerActualWide;
			iconPostkillerTall = iconPostKillerName->EffectiveHeight( 1.0f );

			int iconTallDesired = iLineTall-YRES(2);
			Assert( 0 != iconTallDesired );
			float flScale = (float) iconTallDesired / (float) iconPostkillerTall;

			iconPostkillerActualWide *= flScale;
			iconPostkillerTall *= flScale;
			iconPostkillerWide *= flScale;
		}
		
		if ( iconPostVictimName )
		{
			iconPostVictimActualWide = iconPostVictimName->EffectiveWidth( 1.0f );
			iconPostVictimWide = iconPostVictimActualWide;
			iconPostVictimTall = iconPostVictimName->EffectiveHeight( 1.0f );

			int iconTallDesired = iLineTall - YRES( 2 );
			Assert( 0 != iconTallDesired );
			float flScale = (float)iconTallDesired / (float)iconPostVictimTall;

			iconPostVictimActualWide *= flScale;
			iconPostVictimTall *= flScale;
			iconPostVictimWide *= flScale;
		}

		int iTotalWide = iKillerTextWide + 0 + iconWide + iVictimTextWide + iDeathInfoTextWide + iDeathInfoEndTextWide + ( xMargin * 2 );
		iTotalWide += iconPrekillerWide + iconPostkillerWide + iPreKillerTextWide + iconPostVictimWide;

		int y = yStart + ( ( iLineTall + m_flLineSpacing ) * i );				
		int yText = y + ( ( iLineTall - iTextTall ) / 2 );
		int yIcon = y + ( ( iLineTall - iconTall ) / 2 );

		int x=0;
		if ( m_bRightJustify )
			x =	GetWide() - iTotalWide;

		// draw a background panel for the message
		Vertex_t vert[NUM_BACKGROUND_COORD];
		GetBackgroundPolygonVerts( x, y+1, x+iTotalWide, y+iLineTall-1, ARRAYSIZE( vert ), vert );		
		surface()->DrawSetTexture( -1 );
		surface()->DrawSetColor( GetBackgroundColor ( i ) );
		surface()->DrawTexturedPolygon( ARRAYSIZE( vert ), vert );

		x += xMargin;

		// prekiller icon
		if ( iconPreKillerName )
		{
			int yPreIconTall = y + ( ( iLineTall - iconPrekillerTall ) / 2 );
			iconPreKillerName->DrawSelf( x, yPreIconTall, iconPrekillerActualWide, iconPrekillerTall, m_clrIcon);
			x += iconPrekillerWide + xSpacing;
		}

		if ( killer[0] )
		{
			// Draw killer's name
			Color clr = GetTeamColor( msg.Killer.iTeam, msg.bLocalPlayerInvolved );
			DrawText( x, yText, m_hTextFont, clr, killer );
			x += iKillerTextWide;
		}

		// prekiller text
		if ( msg.wzPreKillerText[0] )
		{
			x += xSpacing;
			DrawText( x + iDeathInfoOffset, yText, m_hTextFont, GetInfoTextColor( i, msg.bLocalPlayerInvolved ), msg.wzPreKillerText );
			x += iPreKillerTextWide;
		}

		// postkiller icon
		if ( iconPostKillerName )
		{
			int yPreIconTall = y + ( ( iLineTall - iconPostkillerTall ) / 2 );
			iconPostKillerName->DrawSelf( x, yPreIconTall, iconPostkillerActualWide, iconPostkillerTall, m_clrIcon );
			x += iconPostkillerWide + xSpacing;
		}

		// Draw death icon
		if ( icon )
		{
			icon->DrawSelf( x, yIcon, iconActualWide, iconTall, m_clrIcon );
			x += iconWide;
		}

		// Draw additional info text next to death icon 
		if ( msg.wzInfoText[0] )
		{
			if ( msg.bSelfInflicted )
			{
				iDeathInfoOffset += iVictimTextWide;
				iVictimTextOffset -= iDeathInfoTextWide;
			}

			DrawText( x + iDeathInfoOffset, yText, m_hTextFont, GetInfoTextColor( i, msg.bLocalPlayerInvolved ), msg.wzInfoText );
			x += iDeathInfoTextWide;
		}

		// Draw victims name
		Color clr = GetTeamColor(msg.Victim.iTeam, msg.bLocalPlayerInvolved);
		DrawText(x + iVictimTextOffset, yText, m_hTextFont, clr, victim);
		x += iVictimTextWide;

		// postkiller icon
		if ( iconPostVictimName )
		{
			int yPreIconTall = y + ( ( iLineTall - iconPostVictimTall ) / 2 );
			iconPostVictimName->DrawSelf( x, yPreIconTall, iconPostVictimActualWide, iconPostVictimTall, m_clrIcon );
			x += iconPostkillerWide + xSpacing;
		}

		// Draw Additional Text on the end of the victims name
		if ( msg.wzInfoTextEnd[0] )
			DrawText( x , yText, m_hTextFont, GetInfoTextColor( i, msg.bLocalPlayerInvolved ), msg.wzInfoTextEnd );
	}
}

//-----------------------------------------------------------------------------
// Purpose: returns the color to draw text in for this team.  
//-----------------------------------------------------------------------------
Color CCSHudDeathNotice::GetTeamColor( int iTeamNumber, bool bLocalPlayerInvolved /* = false */ )
{
	switch ( iTeamNumber )
	{
	case TEAM_CT:
		return m_clrCTText;
		break;
	case TEAM_TERRORIST:
		return clrTERRText;
		break;
	case TEAM_UNASSIGNED:		
		return bLocalPlayerInvolved ? Color(0, 0, 0, 255) : Color(255, 255, 255, 255);
		break;
	default:
		AssertOnce( false );	// invalid team
		return bLocalPlayerInvolved ? Color(0, 0, 0, 255) : Color(255, 255, 255, 255);
		break;
	}
}