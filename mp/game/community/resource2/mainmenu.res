"MainMenu"
{	
	"ResumeGame"
	{
		"text"			"#GameUI2_ResumeGame"
		"description"	"#GameUI2_ResumeGameDescription"
		"command"		"cvar gamemenucommand ResumeGame"
		"priority"		"7"
		"specifics"		"ingame"
	}
	
	"Disconnect"
	{
		"text"			"#GameUI2_Disconnect"
		"description"	"#GameUI2_DisconnectDescription"
		"command"		"cvar gamemenucommand Disconnect"
		"priority"		"6"
		"specifics"		"ingame"
	}
	
	"PlayerList"
	{
		"text"			"#GameUI2_PlayerList"
		"description"	"#GameUI2_PlayerListDescription"
		"command"		"cvar gamemenucommand OpenPlayerListDialog"
		"priority"		"5"
		"specifics"		"ingame"
	}
	
	"FindServrer"
	{
		"text"			"#GameUI2_FindServer"
		"description"	"#GameUI2_FindServerDescription"
		"command"		"cvar gamemenucommand OpenServerBrowser"
		"priority"		"4"
		"specifics"		"shared"
	}
	
	"CreateServer"
	{
		"text"			"#GameUI2_CreateServer"
		"description"	"#GameUI2_CreateServerDescription"
		"command"		"cvar gamemenucommand OpenCreateMultiplayerGameDialog"
		"priority"		"3"
		"specifics"		"shared"
	}
	
	"Options"
	{
		"text"			"#GameUI2_Options"
		"description"	"#GameUI2_OptionsDescription"
		"command"		"cvar gamemenucommand openoptionsdialog" //gameui2_openoptionsdialog
		"priority"		"2"
		"specifics"		"shared"
	}

	"Quit"
	{
		"text"			"#GameUI2_Quit"
		"description"	"#GameUI2_QuitDescription"
		"command"		"cvar gamemenucommand quit"
		"priority"		"1"
		"specifics"		"shared"
	}
}