#ifndef FL_VALUE_INPUT_3DIGIT_HPP
#define	FL_VALUE_INPUT_3DIGIT_HPP

#include <FL/Fl_Value_Output.H>

class Fl_Value_Output_3Digit : public Fl_Value_Output {
public:
	
	Fl_Value_Output_3Digit(int x, int y, int w, int h, const char *label = 0):
		Fl_Value_Output(x, y, w, h, label)
	{
	}

	int format(char *p) {
		return sprintf(p, "%.3f", value());
	}
};

#endif	/* FL_VALUE_INPUT_3DIGIT_HPP */

