#ifndef Engine_Enum_h__
#define Engine_Enum_h__

namespace Engine
{
	enum class WINMODE { FULL, WIN, END };	
	enum class PROTOTYPE { GAMEOBJECT, COMPONENT };
	enum class STATE { RIGHT, UP, LOOK, POSITION, END };
	enum class RENDER { PRIORITY, NONBLEND, NONLIGHT, BLEND, UI, END};
	enum class TEXTURE { PLANE, CUBE, END };
	enum class MODEL { ANIM, NONANIM, COLLISION, END };
	enum class RAY { LOCAL, WORLD, CENTER, END };
	enum class D3DTS { VIEW, PROJ, END };
	/* Spot은 미구현 */
	enum class LIGHT { DIRECTIONAL, POINT, SPOT, END };
	enum class CHANNELID { SOUND_EFFECT, SOUND_BGM, SOUND_END };
	enum class IMGUI_VISIBLE { GAMEINFO, OBJECT_INSPECTOR, END };
	enum class MOUSEKEYSTATE { LBUTTON, RBUTTON, WBUTTON, XBUTTON };
	//W -> Wheel
	enum class MOUSEMOVESTATE { X, Y, W };
	enum class NAVI_POINT { A, B, C, END };
	enum class NAVI_LINE { AB, BC, CA, END };
	enum class COLLIDER { AABB, OBB, SPHERE, END };
}
#endif // Engine_Enum_h__
