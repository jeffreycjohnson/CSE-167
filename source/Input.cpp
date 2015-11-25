#include "Input.h"
#include "Timer.h"
#include "MathFunc.h"
#include <iostream>
#include <SOIL/SOIL.h>

using namespace std;

GLFWcursor* Input::cursor = nullptr;
GLFWwindow* Input::window = nullptr;
unordered_map<string, GLFWinput> Input::inputMap; // Used to translate button names to GLFW values
unordered_map<string, InputData> Input::inputs; // Finds custom named inputs
unordered_map<int, Button> Input::keyboardMap;
unordered_map<int, Button> Input::mouseMap;
unordered_map<int, Button> Input::joystickMap;
glm::vec2 Input::scrollBuff, Input::scrollAmount;

Input::Input()
{
	// Initialize cursor to default
	cursor = glfwCreateStandardCursor(GLFW_ARROW_CURSOR);
}

Input::~Input()
{
	glfwDestroyCursor(cursor);
}

void Input::init(GLFWwindow* win)
{
	window = win;

	// Initialize key map
	// Special Characters
	inputMap.insert({ "space", {InputType::KEYBOARD, 32} });
	inputMap.insert({ "'",{ InputType::KEYBOARD, 39 } });
	inputMap.insert({ ";",{ InputType::KEYBOARD, 59 } });
	inputMap.insert({ "=",{ InputType::KEYBOARD, 61 } });
	inputMap.insert({ "`",{ InputType::KEYBOARD, 96 } });
	inputMap.insert({ "escape",{ InputType::KEYBOARD, 256 } });
	inputMap.insert({ "enter",{ InputType::KEYBOARD, 257 } });
	inputMap.insert({ "tab",{ InputType::KEYBOARD, 258 } });
	inputMap.insert({ "backspace",{ InputType::KEYBOARD, 259 } });
	inputMap.insert({ "insert",{ InputType::KEYBOARD, 260 } });
	inputMap.insert({ "delete",{ InputType::KEYBOARD, 261 } });
	inputMap.insert({ "right",{ InputType::KEYBOARD, 262 } });
	inputMap.insert({ "left",{ InputType::KEYBOARD, 263 } });
	inputMap.insert({ "down",{ InputType::KEYBOARD, 264 } });
	inputMap.insert({ "up",{ InputType::KEYBOARD, 265 } });
	inputMap.insert({ "page up",{ InputType::KEYBOARD, 266 } });
	inputMap.insert({ "page down",{ InputType::KEYBOARD, 267 } });
	inputMap.insert({ "home",{ InputType::KEYBOARD, 268 } });
	inputMap.insert({ "end",{ InputType::KEYBOARD, 269 } });
	inputMap.insert({ "caps lock",{ InputType::KEYBOARD, 280 } });
	inputMap.insert({ "scroll lock",{ InputType::KEYBOARD, 281 } });
	inputMap.insert({ "num lock",{ InputType::KEYBOARD, 282 } });
	inputMap.insert({ "print screen",{ InputType::KEYBOARD, 283 } });
	inputMap.insert({ "pause",{ InputType::KEYBOARD, 284 } });
	inputMap.insert({ "left shift",{ InputType::KEYBOARD, 340 } });
	inputMap.insert({ "left control",{ InputType::KEYBOARD, 341 } });
	inputMap.insert({ "left alt",{ InputType::KEYBOARD, 342 } });
	inputMap.insert({ "left super",{ InputType::KEYBOARD, 343 } });
	inputMap.insert({ "right shift",{ InputType::KEYBOARD, 344 } });
	inputMap.insert({ "right control",{ InputType::KEYBOARD, 345 } });
	inputMap.insert({ "right alt",{ InputType::KEYBOARD, 346 } });
	inputMap.insert({ "right super",{ InputType::KEYBOARD, 347 } });
	inputMap.insert({ "menu",{ InputType::KEYBOARD, 348 } });

	// Keypad
	for (int i = 320; i < 330; i++)
	{
		inputMap.insert({ "[" + to_string(i - 320) + "]",{ InputType::KEYBOARD, 330 } });
	}
	inputMap.insert({ "[.]",{ InputType::KEYBOARD, 330 } });
	inputMap.insert({ "[/]",{ InputType::KEYBOARD, 331 } });
	inputMap.insert({ "[*]",{ InputType::KEYBOARD, 332 } });
	inputMap.insert({ "[-]",{ InputType::KEYBOARD, 333 } });
	inputMap.insert({ "[+]",{ InputType::KEYBOARD, 334 } });
	inputMap.insert({ "[enter]",{ InputType::KEYBOARD, 330 } });
	inputMap.insert({ "[=]",{ InputType::KEYBOARD, 330 } });

	// Numbers
	for (int i = 44; i < 58; i++) // comma, minus, period, and forward slash included
	{
		inputMap.insert({ to_string((char)i),{ InputType::KEYBOARD, i } });
	}

	// Letters
	for (int i = 65; i < 91; i++)
	{
		string name(1, (char)i);
		transform(name.begin(), name.end(), name.begin(), ::tolower);
		inputMap.insert({ name,{ InputType::KEYBOARD, i } });
	}

	// Function Keys
	for (int i = 290; i < 314; i++)
	{
		inputMap.insert({ "f" + to_string(i - 289),{ InputType::KEYBOARD, i } });
	}

	// Mouse Buttons
	for (int i = 0; i < 7; i++)
	{
		inputMap.insert({ "mouse " + to_string(i),{ InputType::MOUSE, i } });
	}

	// Joystick Buttons
	for (int i = 0; i < 20; i++)
	{
		inputMap.insert({ "joystick button " + to_string(i), { InputType::JOYSTICK, i } });
	}

	// Initialize button maps
	unordered_map<std::string, GLFWinput>::iterator it;
	for (it = inputMap.begin(); it != inputMap.end(); it++)
	{
		if (it->second.type == InputType::KEYBOARD)
		{
			keyboardMap.insert({ it->second.id, {InputState::IDLE, false} });
		}
		else if (it->second.type == InputType::MOUSE)
		{
			mouseMap.insert({ it->second.id,{ InputState::IDLE, false } });
		}
		else if (it->second.type == InputType::JOYSTICK)
		{
			joystickMap.insert({ it->second.id,{ InputState::IDLE, false } });
		}
	}

	// Add custom inputs
	InputData data;
	data.name = "horizontal";
	data.positiveButton = "d";
	data.negativeButton = "a";
	data.altPositiveButton = "joystick button 11";
	data.altNegativeButton = "joystick button 13";
	data.joystick = Joystick::JOYSTICK_1;
	data.axis = AxisType::X;
	data.dead = 0.1f;
	data.sensitivity = 0.25f;
	data.invert = false;
	addInput(data);

	data.name = "vertical";
	data.positiveButton = "w";
	data.negativeButton = "s";
	data.altPositiveButton = "up";
	data.altNegativeButton = "down";
	data.joystick = Joystick::JOYSTICK_3;
	data.axis = AxisType::Y;
	data.dead = 0.1f;
	data.sensitivity = 0.25f;
	data.invert = true;
	addInput(data);
}

void Input::update()
{
	// POSSIBLE RACE CONDITION WITH LOW FPS: Input key down and key up before the next frame loses input!
	// Solution: Use a queue and change the update function to use callbacks

	cout << getAxis("vertical") << endl;

	int count;
	const unsigned char* joystickArray = glfwGetJoystickButtons(0, &count); // Only deal with single controllers for now

	unordered_map<std::string, GLFWinput>::iterator it;

	for (it = inputMap.begin(); it != inputMap.end(); it++)
	{
		if (it->second.type == InputType::KEYBOARD)
		{
			// TODO: Add checks for end pointer
			changeState(keyboardMap.find(it->second.id), glfwGetKey(window, it->second.id));
		}
		else if (it->second.type == InputType::MOUSE)
		{
			// TODO: Add checks for end pointer
			changeState(mouseMap.find(it->second.id), glfwGetMouseButton(window, it->second.id));
		}
		else if (it->second.type == InputType::JOYSTICK)
		{
			// TODO: Add joystick support
			if (count > 0)
				changeState(joystickMap.find(it->second.id), joystickArray[it->second.id]);
		}
	}

	if (scrollBuff != scrollAmount && (abs(scrollAmount.x) > 0 || abs(scrollAmount.y) > 0))
		cout << "Scroll: x = " << scrollAmount.x << ", y = " << scrollAmount.y << endl;

	scrollAmount = scrollBuff;
	scrollBuff.x = 0;
	scrollBuff.y = 0;
}

void Input::changeState(unordered_map<int, Button>::iterator it, int newValue)
{
	if (it->second.edge)
	{
		if (it->second.state == InputState::BUTTON_DOWN)
		{
			it->second.edge = false;
			it->second.state = InputState::PRESSED;
			it->second.startTime = (float)Timer::time();
		}
		else if (it->second.state == InputState::BUTTON_UP)
		{
			it->second.edge = false;
			it->second.state = InputState::IDLE;
			it->second.startTime = (float)Timer::time();
		}
	}
	else
	{
		if (it->second.state == InputState::IDLE && newValue == GLFW_PRESS) // IDLE to PRESSED edge
		{
			it->second.edge = true;
			it->second.state = InputState::BUTTON_DOWN;
			it->second.startTime = (float)Timer::time();
		}
		else if (it->second.state == InputState::PRESSED && newValue == GLFW_RELEASE) // PRESSED to IDLE edge
		{
			it->second.edge = true;
			it->second.state = InputState::BUTTON_UP;
			it->second.startTime = (float)Timer::time();
		}
	}
}

void Input::addInput(InputData data)
{
	inputs.insert({data.name, data});
}

// ------------ Keymapped Inputs ------------ 

float Input::getAxis(std::string name)
{
	// TODO: Error handling for nonexistent input
	InputData data = inputs[name];
	float result = 0;

	float pos = getAxisHelper(inputMap[data.positiveButton], data);
	float neg = -getAxisHelper(inputMap[data.negativeButton], data);
	float altpos = getAxisHelper(inputMap[data.altPositiveButton], data);
	float altneg = -getAxisHelper(inputMap[data.altNegativeButton], data);

	// Find max value of each button
	if (abs(pos) < abs(neg))
		pos = neg;
	if (abs(altpos) < abs(altneg))
		altpos = altneg;
	if (abs(pos) < abs(altpos))
		pos = altpos;

	float joystick = 0;
	int count;
	const float* axisArray = glfwGetJoystickAxes(0, &count); // Only deal with single controllers for now

	if (count > 0)
	{
		if (data.joystick == Joystick::JOYSTICK_ALL)
		{
			for (int i = 0; i < count; i++)
			{
				if (abs(axisArray[i]) > abs(joystick))
					joystick = axisArray[i];
			}
		}
		else if (data.axis != AxisType::SCROLL && data.joystick + data.axis < count)
		{
			joystick = axisArray[data.joystick + data.axis];
		}
	}

	if (abs(pos) > abs(joystick))
		result = pos;
	else
		result = joystick;

	if (abs(result) < data.dead)
		result = 0;

	if (data.invert)
		result = -result;

	return result;
}

float Input::getAxisHelper(GLFWinput in, InputData data)
{
	// Similar code, but uses different maps
	if (in.type == InputType::KEYBOARD)
	{
		Button button = keyboardMap[in.id];
		if (button.state == InputState::BUTTON_DOWN || button.state == InputState::PRESSED)
		{
			keyboardMap[in.id].value = MathFunc::Lerp(button.value, 1, (float)(Timer::time() - button.startTime) / data.sensitivity);
			return keyboardMap[in.id].value;
		}
		else if (button.state == InputState::BUTTON_UP || button.state == InputState::IDLE)
		{
			keyboardMap[in.id].value = MathFunc::Lerp(button.value, 0, (float)(Timer::time() - button.startTime) / data.sensitivity);
			return keyboardMap[in.id].value;
		}
	}
	else if (in.type == InputType::MOUSE)
	{
		Button button = mouseMap[in.id];
		if (button.state == InputState::BUTTON_DOWN || button.state == InputState::PRESSED)
		{
			mouseMap[in.id].value = MathFunc::Lerp(button.value, 1, (float)(Timer::time() - button.startTime) / data.sensitivity);
			return mouseMap[in.id].value;
		}
		else if (button.state == InputState::BUTTON_UP || button.state == InputState::IDLE)
		{
			mouseMap[in.id].value = MathFunc::Lerp(button.value, 0, (float)(Timer::time() - button.startTime) / data.sensitivity);
			return mouseMap[in.id].value;
		}
	}
	else // Joystick
	{
		Button button = joystickMap[in.id];
		if (button.state == InputState::BUTTON_DOWN || button.state == InputState::PRESSED)
		{
			joystickMap[in.id].value = MathFunc::Lerp(button.value, 1, (float)(Timer::time() - button.startTime) / data.sensitivity);
			return joystickMap[in.id].value;
		}
		else if (button.state == InputState::BUTTON_UP || button.state == InputState::IDLE)
		{
			joystickMap[in.id].value = MathFunc::Lerp(button.value, 0, (float)(Timer::time() - button.startTime) / data.sensitivity);
			return joystickMap[in.id].value;
		}
	}
	return 0;
}

// ------------ Basic Input Functions ------------ 

bool Input::getKeyDown(string button)
{
	return keyboardMap[inputMap[button].id].state == InputState::BUTTON_DOWN;
}

bool Input::getKey(string button)
{
	return keyboardMap[inputMap[button].id].state == InputState::PRESSED;
}

bool Input::getKeyUp(string button)
{
	return keyboardMap[inputMap[button].id].state == InputState::BUTTON_UP;
}

bool Input::getKeyIdle(string button)
{
	return keyboardMap[inputMap[button].id].state == InputState::IDLE;
}

bool Input::getMouseDown(string button)
{
	return mouseMap[inputMap[button].id].state == InputState::BUTTON_DOWN;
}

bool Input::getMouse(string button)
{
	return mouseMap[inputMap[button].id].state == InputState::PRESSED;
}

bool Input::getMouseUp(string button)
{
	return mouseMap[inputMap[button].id].state == InputState::BUTTON_UP;
}

bool Input::getMouseIdle(string button)
{
	return mouseMap[inputMap[button].id].state == InputState::IDLE;
}


// ------------ Cursor Functions ------------ 

bool Input::setCursor(string filename, int width, int height, int xhot, int yhot)
{
	unsigned char* file = SOIL_load_image(filename.c_str(), &width, &height, 0, SOIL_LOAD_AUTO);
	if (file == nullptr)
	{
		return false;
	}

	GLFWimage image;
	image.pixels = file;
	image.width = width;
	image.height = height;

	return setCursor(&image, xhot, yhot);
}

bool Input::setCursor(const GLFWimage* image, int xhot, int yhot)
{
	GLFWcursor* tmp = glfwCreateCursor(image, xhot, yhot);
	if (tmp == nullptr)
	{
		return false;
	}
	else
	{
		glfwSetCursor(window, tmp);
		cursor = tmp;
		return true;
	}
}

bool Input::setCursor(int standardCursor)
{
	GLFWcursor* tmp = glfwCreateStandardCursor(standardCursor);
	if (tmp == nullptr)
	{
		return false;
	}
	else
	{
		glfwSetCursor(window, tmp);
		cursor = tmp;
		glfwDestroyCursor(tmp);
		return true;
	}
}

void Input::hideCursor()
{
	glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_HIDDEN);
}

void Input::showCursor()
{
	glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_NORMAL);
}

void Input::scroll_callback(GLFWwindow* window, double xoffset, double yoffset)
{
	scrollBuff.x += (float) xoffset;
	scrollBuff.y += (float) yoffset;
}