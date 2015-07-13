#pragma once

template <typename T>
struct _pbVec {
	T x;
	T y;
	T z;

	_pbVec()
	{
		x = 0;
		y = 0;
		z = 0;
	}

	_pbVec(T _x, T _y, T _z)
	{
		x = _x;
		y = _y;
		z = _z;
	}

	_pbVec<T> convertToUnit()
	{
		float rate = (float)sqrt( (x*x) + (y*y) + (z*z) );
		if ( rate == 0 )
			return _pbVec<T>(0,0,0);
		else
			return _pbVec<T>(x/rate, y/rate, z/rate);

	};

};

typedef _pbVec<float> pbVecf;

float lineDistanceSquare(pbVecf point0, pbVecf point1);