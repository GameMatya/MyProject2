#pragma once

//******************************************************************************
//
//		Easing.h
//
//******************************************************************************

#include <math.h>

//イージング関数
class Sine {

public:

    static float easeIn(float t, float b, float c, float d);
    static float easeOut(float t, float b, float c, float d);
    static float easeInOut(float t, float b, float c, float d);

};

class Quad {

public:

	static float easeIn(float t, float b, float c, float d);
	static float easeOut(float t, float b, float c, float d);
	static float easeInOut(float t, float b, float c, float d);
};

class Cubic {

public:

	static float easeIn(float t, float b, float c, float d);
	static float easeOut(float t, float b, float c, float d);
	static float easeInOut(float t, float b, float c, float d);
};

class Quart {

public:

	static float easeIn(float t, float b, float c, float d);
	static float easeOut(float t, float b, float c, float d);
	static float easeInOut(float t, float b, float c, float d);
};

class Quint {

public:

	static float easeIn(float t, float b, float c, float d);
	static float easeOut(float t, float b, float c, float d);
	static float easeInOut(float t, float b, float c, float d);
};

class Expo {

public:

	static float easeIn(float t, float b, float c, float d);
	static float easeOut(float t, float b, float c, float d);
	static float easeInOut(float t, float b, float c, float d);
};

class Circ {

public:

	static float easeIn(float t, float b, float c, float d);
	static float easeOut(float t, float b, float c, float d);
	static float easeInOut(float t, float b, float c, float d);
};

class Back {

public:

	static float easeIn(float t, float b, float c, float d);
	static float easeOut(float t, float b, float c, float d);
	static float easeInOut(float t, float b, float c, float d);

};

class Elastic {

public:

	static float easeIn(float t, float b, float c, float d);
	static float easeOut(float t, float b, float c, float d);
	static float easeInOut(float t, float b, float c, float d);
};

class Bounce {

public:

	static float easeIn(float t, float b, float c, float d);
	static float easeOut(float t, float b, float c, float d);
	static float easeInOut(float t, float b, float c, float d);
};
