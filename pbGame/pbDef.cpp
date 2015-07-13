#include "pbDef.h"

float lineDistanceSquare(pbVecf point0, pbVecf point1)
{
	return ((point0.x - point1.x ) * (point0.x - point1.x ))
		+ ((point0.y - point1.y ) * (point0.y - point1.y ))
		+ ((point0.z - point1.z ) * (point0.z - point1.z ));
}