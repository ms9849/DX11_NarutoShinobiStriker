#ifndef Engine_Typedef_h__
#define Engine_Typedef_h__

namespace Engine
{
	typedef		bool						_bool;

	typedef		signed char					_byte;
	typedef		unsigned char				_ubyte;
	
	typedef		signed short				_short;
	typedef		unsigned short				_ushort;

	typedef		signed int					_int;
	typedef		unsigned int				_uint;

	typedef		signed long					_long;
	typedef		unsigned long				_ulong;

	typedef		char						_char;
	typedef		wchar_t						_tchar;

	typedef		float						_float;
	typedef		double						_double;
	typedef		wstring						_wstring;

	typedef		XMFLOAT2					_float2;
	typedef		XMFLOAT3					_float3;
	typedef		XMFLOAT4					_float4;

	/* 
	같이 쓰면 안되나요? 
	-> ㅇㅇ, 하난 연산용, 하난 저장용임.
	*/
	typedef		XMFLOAT4X4					_float4x4;
	typedef		XMMATRIX					_matrix;

}

#endif // Engine_Typedef_h__
