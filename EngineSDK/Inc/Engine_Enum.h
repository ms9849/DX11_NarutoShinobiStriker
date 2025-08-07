#ifndef Engine_Enum_h__
#define Engine_Enum_h__

namespace Engine
{
	enum class WINMODE { FULL, WIN, END };	
	enum class PROTOTYPE { GAMEOBJECT, COMPONENT };
	enum class STATE { RIGHT, UP, LOOK, POSITION, END };
	enum class RENDER { PRIORITY, NONBLEND, BLEND, UI, END};
	enum class TEXTURE { PLANE, CUBE, END };
	enum class RAY { LOCAL, WORLD, END };
	enum class D3DTS { VIEW, PROJ, END };
	enum class CHANNELID { SOUND_EFFECT, SOUND_BGM, SOUND_END };
	enum class IMGUI_VISIBLE { MANAGER, GAMEINFO, OBJECT_INSPECTOR , END };
	enum class MOUSEKEYSTATE { LBUTTON, RBUTTON, WBUTTON, XBUTTON };
	//W -> Wheel
	enum class MOUSEMOVESTATE { X, Y, W };
}
#endif // Engine_Enum_h__
