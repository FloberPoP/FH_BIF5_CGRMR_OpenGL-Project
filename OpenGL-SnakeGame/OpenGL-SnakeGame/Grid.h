#pragma once
class Grid
{
public: 
	const int GRID_SIZE = 30; // Number of fields along one axis
	const float GRID_STEP = 1.8f / GRID_SIZE; // Size of each field (normalized coordinates)
	const float BORDER_OFFSET = 0.9f; // Border boundarys
};

