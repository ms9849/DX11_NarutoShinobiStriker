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

	/*
	FGHC? -> 각각 함수의 인자로 받기 위한 

	1~3번째 벡터,
	4~5번째 벡터,
	6번째 벡터
	7번째 벡터
	
	를 의미한다.	
	*/

	/*XMVECTOR*/
	typedef		XMVECTOR					_vector;

	typedef		FXMVECTOR					_fvector;
	typedef		GXMVECTOR					_gvector;
	typedef		HXMVECTOR					_hvector;
	typedef		CXMVECTOR					_cvector;

	/*XMFLOAT4X4*/
	typedef		XMFLOAT4X4					_float4x4;

	/*XMMATRIX*/
	typedef		XMMATRIX					_matrix;

	/*FXMMATRIX*/
	typedef		FXMMATRIX					_fmatrix;

	/*CXMMATRIX*/
	typedef		CXMMATRIX					_cmatrix;

}

#endif // Engine_Typedef_h__
