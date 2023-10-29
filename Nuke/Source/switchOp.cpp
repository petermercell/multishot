/***********************************************************************************

MIT License

Copyright (c) 2020 guillerodriguezvfx
mail: guille.rodriguez93@outlook.es

Permission is hereby granted, free of charge, to any person obtaining a copy
of this software and associated documentation files (the "Software"), to deal
in the Software without restriction, including without limitation the rights
to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
copies of the Software, and to permit persons to whom the Software is
furnished to do so, subject to the following conditions:

The above copyright notice and this permission notice shall be included in all
copies or substantial portions of the Software.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
SOFTWARE.

***********************************************************************************/
// switchOp.C

static const char* const CLASS = "switchOp";
static const char* const HELP =
"Switches inputs using expressions. Each input has assigned one expression, created with leftvalue, operand and rightvalue. "
"First True expression will be the active input.";

#include "DDImage/NoIop.h"
#include "DDImage/Iop.h"
#include "DDImage/Row.h"
#include "DDImage/Knobs.h"
#include "DDImage/NukeWrapper.h"

#include<iostream>
#include <string.h>
#include <sstream>
#include <map>
#include<list> 

using namespace DD::Image;

struct LessThanStr
{
	bool operator()(const char* s1, const char* s2) const;
};

static const char* const operator_names[] = {
	"IN", "NOT IN", nullptr
};

static const int MAX_INPUTS = 16;

class switchOp : public NoIop
{
public:

	ChannelSet channels;
	Channel maskChannel;

	std::list<const char*> _stringLeftKnob{ "", "", "", "", "", "", "", "", "", "", "", "", "", "", "", "" };
	std::list<const char*> _stringRightKnob{ "", "", "", "", "", "", "", "", "", "", "", "", "", "", "", "" };

	std::list<int> op_type{ 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0 };

	std::list<const char*>::iterator itrLeftKnob = _stringLeftKnob.begin();
	std::list<const char*>::iterator itrRightKnob = _stringRightKnob.begin();

	std::list<int>::iterator itrOpT = op_type.begin();

	int minimum_inputs() const { return 2; }
	int maximum_inputs() const { return MAX_INPUTS; }


	bool invertMask;
	float mix;
	// what to do with bbox:
	enum { UNION, BBOX, ABOX };
	int inputNum = 0;

	switchOp(Node* node) : NoIop(node)
	{
		channels = Mask_All;
	}


	void _validate(bool for_real) override
	{

		// That Looks for the first True expression in order to set the proper input

		itrLeftKnob = _stringLeftKnob.begin();
		itrRightKnob = _stringRightKnob.begin();
		itrOpT = op_type.begin();

		int n = 1;
		bool _findedTrue;
		std::string valueLeft, valueRight;
		std::size_t found;


		while (n < MAX_INPUTS){
			valueRight = *itrRightKnob;
			valueLeft = *itrLeftKnob;
			found = valueRight.find(valueLeft);
			if (input(n) == NULL || valueRight.empty() || valueLeft.empty()) {
				++itrLeftKnob; ++itrRightKnob; ++itrOpT;
				n++;
				continue;
			}

			if (found != std::string::npos) {
				if (*itrOpT == 0) break;
			}
			else {
				if (*itrOpT == 1) break;
			}

			++itrLeftKnob; ++itrRightKnob; ++itrOpT;
			n++;
		}

		if (n < MAX_INPUTS) {
			inputNum = n;

		}
		else {
			inputNum = 0;
		}		

		input(inputNum)->validate(for_real);
		copy_info(inputNum);
		raw_channels_from(inputNum);
		info_.set(input(inputNum)->info());
		set_out_channels(Mask_None);
		
	}


	void _request(int x, int y, int r, int t, ChannelMask channels, int count) override
	{
		// request from input 0 and input 1
		input(inputNum)->request(x, y, r, t, channels, count);

	}


	virtual float uses_input(int input) const {
		if (input == inputNum) return 1.0f;
		return .01f;
	}



	void knobs(Knob_Callback f) override
	{

		Text_knob(f, "input1", "First input will be active if all conditions are False"); SetFlags(f, Knob::STARTLINE);

		itrLeftKnob = _stringLeftKnob.begin();
		itrRightKnob = _stringRightKnob.begin();
		itrOpT = op_type.begin();

		String_knob(f, &*itrLeftKnob, "leftvalue2", "input2 expression"); SetFlags(f, Knob::STARTLINE); SetFlags(f, Knob::EARLY_STORE);
		Enumeration_knob(f, &*itrOpT, operator_names, "op2", ""); SetFlags(f, Knob::EARLY_STORE);
		Tooltip(f, "Select the binary operation between the fields.\nIN: true if left value is in right list \nNOT IN: inverse as in");
		String_knob(f, &*itrRightKnob, "rightvalue2", ""); SetFlags(f, Knob::EARLY_STORE);

		++itrLeftKnob; ++itrRightKnob; ++itrOpT;

		String_knob(f, &*itrLeftKnob, "leftvalue3", "input3 expression"); SetFlags(f, Knob::STARTLINE); SetFlags(f, Knob::HIDDEN); SetFlags(f, Knob::EARLY_STORE);
		Enumeration_knob(f, &*itrOpT, operator_names, "op3", ""); SetFlags(f, Knob::EARLY_STORE); SetFlags(f, Knob::HIDDEN);
		Tooltip(f, "Select the binary operation between the fields.\nIN: true if left value is in right list \nNOT IN: inverse as in");
		String_knob(f, &*itrRightKnob, "rightvalue3", ""); SetFlags(f, Knob::EARLY_STORE); SetFlags(f, Knob::HIDDEN);

		++itrLeftKnob; ++itrRightKnob; ++itrOpT;

		String_knob(f, &*itrLeftKnob, "leftvalue4", "input4 expression"); SetFlags(f, Knob::STARTLINE); SetFlags(f, Knob::HIDDEN); SetFlags(f, Knob::EARLY_STORE);
		Enumeration_knob(f, &*itrOpT, operator_names, "op4", ""); SetFlags(f, Knob::HIDDEN);
		Tooltip(f, "Select the binary operation between the fields.\nIN: true if left value is in right list \nNOT IN: inverse as in");
		String_knob(f, &*itrRightKnob, "rightvalue4", ""); SetFlags(f, Knob::HIDDEN); SetFlags(f, Knob::EARLY_STORE);

		++itrLeftKnob; ++itrRightKnob; ++itrOpT;

		String_knob(f, &*itrLeftKnob, "leftvalue5", "input5 expression"); SetFlags(f, Knob::STARTLINE); SetFlags(f, Knob::HIDDEN); SetFlags(f, Knob::EARLY_STORE);
		Enumeration_knob(f, &*itrOpT, operator_names, "op5", ""); SetFlags(f, Knob::HIDDEN);
		Tooltip(f, "Select the binary operation between the fields.\nIN: true if left value is in right list \nNOT IN: inverse as in");
		String_knob(f, &*itrRightKnob, "rightvalue5", ""); SetFlags(f, Knob::HIDDEN); SetFlags(f, Knob::EARLY_STORE);

		++itrLeftKnob; ++itrRightKnob; ++itrOpT;

		String_knob(f, &*itrLeftKnob, "leftvalue6", "input6 expression"); SetFlags(f, Knob::STARTLINE); SetFlags(f, Knob::HIDDEN); SetFlags(f, Knob::EARLY_STORE);
		Enumeration_knob(f, &*itrOpT, operator_names, "op6", ""); SetFlags(f, Knob::HIDDEN);
		Tooltip(f, "Select the binary operation between the fields.\nIN: true if left value is in right list \nNOT IN: inverse as in");
		String_knob(f, &*itrRightKnob, "rightvalue6", ""); SetFlags(f, Knob::HIDDEN); SetFlags(f, Knob::EARLY_STORE);

		++itrLeftKnob; ++itrRightKnob; ++itrOpT;

		String_knob(f, &*itrLeftKnob, "leftvalue7", "input7 expression"); SetFlags(f, Knob::STARTLINE); SetFlags(f, Knob::HIDDEN); SetFlags(f, Knob::EARLY_STORE);
		Enumeration_knob(f, &*itrOpT, operator_names, "op7", ""); SetFlags(f, Knob::HIDDEN);
		Tooltip(f, "Select the binary operation between the fields.\nIN: true if left value is in right list \nNOT IN: inverse as in");
		String_knob(f, &*itrRightKnob, "rightvalue7", ""); SetFlags(f, Knob::HIDDEN); SetFlags(f, Knob::EARLY_STORE);

		++itrLeftKnob; ++itrRightKnob; ++itrOpT;

		String_knob(f, &*itrLeftKnob, "leftvalue8", "input8 expression"); SetFlags(f, Knob::STARTLINE); SetFlags(f, Knob::HIDDEN); SetFlags(f, Knob::EARLY_STORE);
		Enumeration_knob(f, &*itrOpT, operator_names, "op8", ""); SetFlags(f, Knob::HIDDEN);
		Tooltip(f, "Select the binary operation between the fields.\nIN: true if left value is in right list \nNOT IN: inverse as in");
		String_knob(f, &*itrRightKnob, "rightvalue8", ""); SetFlags(f, Knob::HIDDEN); SetFlags(f, Knob::EARLY_STORE);

		++itrLeftKnob; ++itrRightKnob; ++itrOpT;

		String_knob(f, &*itrLeftKnob, "leftvalue9", "input9 expression"); SetFlags(f, Knob::STARTLINE); SetFlags(f, Knob::HIDDEN); SetFlags(f, Knob::EARLY_STORE);
		Enumeration_knob(f, &*itrOpT, operator_names, "op9", ""); SetFlags(f, Knob::HIDDEN);
		Tooltip(f, "Select the binary operation between the fields.\nIN: true if left value is in right list \nNOT IN: inverse as in");
		String_knob(f, &*itrRightKnob, "rightvalue9", ""); SetFlags(f, Knob::HIDDEN); SetFlags(f, Knob::EARLY_STORE);

		++itrLeftKnob; ++itrRightKnob; ++itrOpT;
		
		String_knob(f, &*itrLeftKnob, "leftvalue10", "input10 expression"); SetFlags(f, Knob::STARTLINE); SetFlags(f, Knob::HIDDEN); SetFlags(f, Knob::EARLY_STORE);
		Enumeration_knob(f, &*itrOpT, operator_names, "op10", ""); SetFlags(f, Knob::HIDDEN);
		Tooltip(f, "Select the binary operation between the fields.\nIN: true if left value is in right list \nNOT IN: inverse as in");
		String_knob(f, &*itrRightKnob, "rightvalue10", ""); SetFlags(f, Knob::HIDDEN); SetFlags(f, Knob::EARLY_STORE);

		++itrLeftKnob; ++itrRightKnob; ++itrOpT;

		String_knob(f, &*itrLeftKnob, "leftvalue11", "input11 expression"); SetFlags(f, Knob::STARTLINE); SetFlags(f, Knob::HIDDEN); SetFlags(f, Knob::EARLY_STORE);
		Enumeration_knob(f, &*itrOpT, operator_names, "op11", ""); SetFlags(f, Knob::HIDDEN);
		Tooltip(f, "Select the binary operation between the fields.\nIN: true if left value is in right list \nNOT IN: inverse as in");
		String_knob(f, &*itrRightKnob, "rightvalue11", ""); SetFlags(f, Knob::HIDDEN); SetFlags(f, Knob::EARLY_STORE);

		++itrLeftKnob; ++itrRightKnob; ++itrOpT;

		String_knob(f, &*itrLeftKnob, "leftvalue12", "input12 expression"); SetFlags(f, Knob::STARTLINE); SetFlags(f, Knob::HIDDEN); SetFlags(f, Knob::EARLY_STORE);
		Enumeration_knob(f, &*itrOpT, operator_names, "op12", ""); SetFlags(f, Knob::HIDDEN);
		Tooltip(f, "Select the binary operation between the fields.\nIN: true if left value is in right list \nNOT IN: inverse as in");
		String_knob(f, &*itrRightKnob, "rightvalue12", ""); SetFlags(f, Knob::HIDDEN); SetFlags(f, Knob::EARLY_STORE);

		++itrLeftKnob; ++itrRightKnob; ++itrOpT;

		String_knob(f, &*itrLeftKnob, "leftvalue13", "input13expression"); SetFlags(f, Knob::STARTLINE); SetFlags(f, Knob::HIDDEN); SetFlags(f, Knob::EARLY_STORE);
		Enumeration_knob(f, &*itrOpT, operator_names, "op13", ""); SetFlags(f, Knob::HIDDEN);
		Tooltip(f, "Select the binary operation between the fields.\nIN: true if left value is in right list \nNOT IN: inverse as in");
		String_knob(f, &*itrRightKnob, "rightvalue13", ""); SetFlags(f, Knob::HIDDEN); SetFlags(f, Knob::EARLY_STORE);

		++itrLeftKnob; ++itrRightKnob; ++itrOpT;

		String_knob(f, &*itrLeftKnob, "leftvalue14", "input14 expression"); SetFlags(f, Knob::STARTLINE); SetFlags(f, Knob::HIDDEN); SetFlags(f, Knob::EARLY_STORE);
		Enumeration_knob(f, &*itrOpT, operator_names, "op14", ""); SetFlags(f, Knob::HIDDEN);
		Tooltip(f, "Select the binary operation between the fields.\nIN: true if left value is in right list \nNOT IN: inverse as in");
		String_knob(f, &*itrRightKnob, "rightvalue14", ""); SetFlags(f, Knob::HIDDEN); SetFlags(f, Knob::EARLY_STORE);

		++itrLeftKnob; ++itrRightKnob; ++itrOpT;

		String_knob(f, &*itrLeftKnob, "leftvalue15", "input15 expression"); SetFlags(f, Knob::STARTLINE); SetFlags(f, Knob::HIDDEN); SetFlags(f, Knob::EARLY_STORE);
		Enumeration_knob(f, &*itrOpT, operator_names, "op15", ""); SetFlags(f, Knob::HIDDEN);
		Tooltip(f, "Select the binary operation between the fields.\nIN: true if left value is in right list \nNOT IN: inverse as in");
		String_knob(f, &*itrRightKnob, "rightvalue15", ""); SetFlags(f, Knob::HIDDEN); SetFlags(f, Knob::EARLY_STORE);	
		
	}

	const char* Class() const override { return CLASS; }
	const char* node_help() const override { return HELP; }
	static const Description d;

};

static Iop* build(Node* node) { return new switchOp(node); }
const Iop::Description switchOp::d(CLASS, "Merge/switchOp", build);