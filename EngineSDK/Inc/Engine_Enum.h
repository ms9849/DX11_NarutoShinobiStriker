#ifndef Engine_Enum_h__
#define Engine_Enum_h__

namespace Engine
{
	enum class WINMODE { FULL, WIN, END };	
	enum class PROTOTYPE { GAMEOBJECT, COMPONENT };
	enum class STATE { RIGHT, UP, LOOK, POSITION, END };
	/* Shadow ·»´õÅ¸°Ù Ãß°¡. */
	enum class RENDER { PRIORITY, SHADOW, NONBLEND, NONLIGHT, BLUR, BLUR_SMALL, DISTORTION, BLEND, WORLD_UI, UI, END};
	enum class TEXTURE { PLANE, CUBE, END };
	enum class MODEL { ANIM, NONANIM, COLLISION, END };
	enum class RAY { LOCAL, WORLD, CENTER, END };
	enum class D3DTS { VIEW, PROJ, END };
	/* SpotÀº ¹Ì±¸Çö */
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
