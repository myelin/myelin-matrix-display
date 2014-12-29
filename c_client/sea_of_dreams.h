#include "matrix.h"

class SeaOfDreamsOverlay {
protected:
	ScreenBuffer *output;
	int tx;

public:
	SeaOfDreamsOverlay(ScreenBuffer *main);
	ScreenBuffer *overlay(ScreenBuffer *screen, int frame);
};
