#pragma once

namespace thm {
// ##########
//   Window
// ##########

	inline constexpr unsigned int wndBaseColorLight       = 0xFF1F1f21;
	inline constexpr unsigned int wndBaseColorDark        = 0xFF1E1E1E;
	inline constexpr unsigned int textColor               = 0xFFA2A4A5;


// ##########
//   Button
// ##########

	inline constexpr unsigned int bttnBaseColorLight      = 0xFF353535;
	inline constexpr unsigned int bttnBaseColorDark       = 0xFF343436;

	inline constexpr unsigned int bttnHoverColorLight     = 0xFF404040;
	inline constexpr unsigned int bttnHoverColorDark      = 0xFF3F3F41;

	inline constexpr unsigned int bttnTextColor           = 0xFFFFFFFF;


// ###########
//  Selectors
// ###########

	inline constexpr unsigned int selBorderColor          = 0xFF8E8E90;
	inline constexpr float		  selBorderWidth          = 1.25;

	inline constexpr unsigned int selBaseColorLight       = 0xFF1E1E20;
	inline constexpr unsigned int selBaseColorDark        = 0xFF1B1B1C;

	inline constexpr unsigned int selHoverColorLight      = 0xFF2E2E31;
	inline constexpr unsigned int selHoverColorDark       = 0xFF2B2B2D;

	inline constexpr unsigned int selSelColor             = 0xFF8170F8;


// ############
//  TreeDrawer 
// ############

	inline constexpr unsigned int trdwBaseColorLight      = 0xFF1F1F22;
	inline constexpr unsigned int trdwBaseColorDark       = 0xFF212121;

	inline constexpr unsigned int trdwNodeColorLight      = 0xFF2C2C2F;
	inline constexpr unsigned int trdwNodeColorDark       = 0xFF2A2A2C;

	inline constexpr unsigned int trdwLinkColor           = 0xFF3C3C48;
	inline constexpr float		  trdwLinkWidth           = 1.5f;

	inline constexpr unsigned int trdwRingColor           = 0xFF605D96;
	inline constexpr float		  trdwRingWidth           = 1.75f;

	inline constexpr unsigned int trdwTextColor           = 0xFFCECFD0;

// #########
//   Plot
// #########

	inline constexpr unsigned int pltBgColor              = 0xFF17171A;
	inline constexpr unsigned int pltGridColor            = 0xFF262630;
	inline constexpr unsigned int pltAxisColor            = 0xFF58586A;
	inline constexpr unsigned int pltFuncColor            = 0xFF8170F8;
	inline constexpr unsigned int pltPointColor           = 0xFFA89EFF;
	inline constexpr unsigned int pltLabelColor           = 0xFF6E7075;

	inline constexpr float        pltGridLineWidth        = 0.75f;
	inline constexpr float        pltAxisLineWidth        = 1.25f;
	inline constexpr float        pltFuncLineWidth        = 1.5f;
	inline constexpr float        pltPointRadius          = 3.0f;
	inline constexpr float        pltTickSize             = 4.0f;
}