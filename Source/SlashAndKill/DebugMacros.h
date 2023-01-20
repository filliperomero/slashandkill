#pragma once
#include "DrawDebugHelpers.h"

#define DRAW_SPHERE(Location) if (GetWorld()) DrawDebugSphere(GetWorld(), Location, 25.f, 30, FColor::Red, true, 30.f);
#define DRAW_SPHERE_SINGLE_FRAME(Location) if (GetWorld()) DrawDebugSphere(GetWorld(), Location, 25.f, 30, FColor::Red, false, -1.f);
#define DRAW_LINE(StartLocation, EndLocation) if (GetWorld()) DrawDebugLine(GetWorld(), StartLocation, EndLocation, FColor::Blue, true, 30.f);
#define DRAW_LINE_SINGLE_FRAME(StartLocation, EndLocation) if (GetWorld()) DrawDebugLine(GetWorld(), StartLocation, EndLocation, FColor::Blue, false, -1.f, 0, 1.f);
#define DRAW_POINT(Location) if (GetWorld()) DrawDebugPoint(GetWorld(), Location, 10.f, FColor::Green, true, 30.f);
#define DRAW_POINT_SINGLE_FRAME(Location) if (GetWorld()) DrawDebugPoint(GetWorld(), Location, 10.f, FColor::Green, false, -1.f);
#define DRAW_VECTOR(StartLocation, EndLocation) if (GetWorld()) \
	{	\
		DrawDebugLine(GetWorld(), StartLocation, EndLocation, FColor::Blue, true, 30.f);	\
		DrawDebugPoint(GetWorld(), EndLocation, 10.f, FColor::Green, true, 30.f);	\
	}
#define DRAW_VECTOR_SINGLE_FRAME(StartLocation, EndLocation) if (GetWorld()) \
	{	\
		DrawDebugLine(GetWorld(), StartLocation, EndLocation, FColor::Blue, false, -1.f);	\
		DrawDebugPoint(GetWorld(), EndLocation, 10.f, FColor::Green, false, -1.f);	\
	}