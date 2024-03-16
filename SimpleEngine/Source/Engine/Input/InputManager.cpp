#include "Engine/Precomplied/EnginePch.hpp"
#include "Engine/Input/InputManager.hpp"
#include <windowsx.h>
#include <string>

namespace SimpleUtilities
{
	InputManager::InputManager()
		: myMouseIsHidden(false)
		, myAKeyIsPressed(false)
		, myTentativeMousePosition(0, 0)
		, myCurrentMousePosition(0, 0)
		, myPreviousMousePosition(0, 0)
		, myMouseDelta(0, 0)
	{
		myOwnerHWND = GetActiveWindow();

		myKeyLiveState = { 0 };
		myKeyState = { 0 };
		myKeyPreviousState = { 0 };
	}

	bool InputManager::IsKeyPressed(const int aKeyCode) const
	{
		return myKeyState[aKeyCode] && !myKeyPreviousState[aKeyCode];
	}

	bool InputManager::IsKeyHeld(const int aKeyCode) const
	{
		return myKeyState[aKeyCode] && myKeyPreviousState[aKeyCode];
	}

	bool InputManager::IsKeyReleased(const int aKeyCode) const
	{
		return !myKeyState[aKeyCode] && myKeyPreviousState[aKeyCode];
	}

	bool InputManager::IsKeyDown(const int aKeyCode) const
	{
		return myKeyState[aKeyCode];
	}

	void InputManager::Update()
	{
		myKeyPreviousState = myKeyState;
		myKeyState = myKeyLiveState;

		myPreviousMousePosition = myCurrentMousePosition;
		myCurrentMousePosition = myTentativeMousePosition;

		myMouseDelta = myTentativeMouseDelta;
		myMouseDelta.y *= -1; //Windows have y = 0 at top :(

		myTentativeMouseDelta = { 0, 0 };

		if (myKeyState.any())
		{
			myAKeyIsPressed = true;
		}
		else
		{
			myAKeyIsPressed = false;
		}
	}

	std::string InputManager::GetKeyAsString(std::string& aString, const int aCharactersLimit) const
	{
		if (this->IsKeyPressed(0x08) && aString.length() > 0)
		{
			aString.pop_back();
			return "";
		}

		if (aString.length() > aCharactersLimit)
		{
			return "";
		}

		return this->GetKeyAsString();
	}

	std::string InputManager::GetKeyAsString() const
	{
		if (this->IsKeyPressed(0x30))
		{
			return "0";
		}

		if (this->IsKeyPressed(0x31))
		{
			return "1";
		}

		if (this->IsKeyPressed(0x32))
		{
			return "2";
		}

		if (this->IsKeyPressed(0x33))
		{
			return "3";
		}

		if (this->IsKeyPressed(0x34))
		{
			return "4";
		}

		if (this->IsKeyPressed(0x35))
		{
			return "5";
		}

		if (this->IsKeyPressed(0x36))
		{
			return "6";
		}

		if (this->IsKeyPressed(0x37))
		{
			return "7";
		}

		if (this->IsKeyPressed(0x38))
		{
			return "8";
		}

		if (this->IsKeyPressed(0x39))
		{
			return "9";
		}

		if (this->IsKeyPressed(0x41))
		{
			return "A";
		}

		if (this->IsKeyPressed(0x42))
		{
			return "B";
		}

		if (this->IsKeyPressed(0x43))
		{
			return "C";
		}

		if (this->IsKeyPressed(0x44))
		{
			return "D";
		}

		if (this->IsKeyPressed(0x45))
		{
			return "E";
		}

		if (this->IsKeyPressed(0x46))
		{
			return "F";
		}

		if (this->IsKeyPressed(0x47))
		{
			return "G";
		}

		if (this->IsKeyPressed(0x48))
		{
			return "H";
		}

		if (this->IsKeyPressed(0x49))
		{
			return "I";
		}

		if (this->IsKeyPressed(0x4A))
		{
			return "J";
		}

		if (this->IsKeyPressed(0x4B))
		{
			return "K";
		}

		if (this->IsKeyPressed(0x4C))
		{
			return "L";
		}

		if (this->IsKeyPressed(0x4D))
		{
			return "M";
		}

		if (this->IsKeyPressed(0x4E))
		{
			return "N";
		}

		if (this->IsKeyPressed(0x4F))
		{
			return "O";
		}

		if (this->IsKeyPressed(0x50))
		{
			return "P";
		}

		if (this->IsKeyPressed(0x51))
		{
			return "Q";
		}

		if (this->IsKeyPressed(0x52))
		{
			return "R";
		}

		if (this->IsKeyPressed(0x53))
		{
			return "S";
		}

		if (this->IsKeyPressed(0x54))
		{
			return "T";
		}

		if (this->IsKeyPressed(0x55))
		{
			return "U";
		}

		if (this->IsKeyPressed(0x56))
		{
			return "V";
		}

		if (this->IsKeyPressed(0x57))
		{
			return "W";
		}

		if (this->IsKeyPressed(0x58))
		{
			return "X";
		}

		if (this->IsKeyPressed(0x59))
		{
			return "Y";
		}

		if (this->IsKeyPressed(0x5A))
		{
			return "Z";
		}

		if (this->IsKeyPressed(VK_SPACE))
		{
			return " ";
		}

		return "";
	}

	bool InputManager::GetAKeyIsPressed() const
	{
		return myAKeyIsPressed;
	}

	bool InputManager::GetMouseIsHidden() const
	{
		return myMouseIsHidden;
	}

	void InputManager::SetHWND(HWND& aWindowHandle)
	{
		myOwnerHWND = aWindowHandle;

#ifndef HID_USAGE_PAGE_GENERIC
#define HID_USAGE_PAGE_GENERIC         ((USHORT) 0x01)
#endif
#ifndef HID_USAGE_GENERIC_MOUSE
#define HID_USAGE_GENERIC_MOUSE        ((USHORT) 0x02)
#endif

		RAWINPUTDEVICE Rid[1];
		Rid[0].usUsagePage = HID_USAGE_PAGE_GENERIC;
		Rid[0].usUsage = HID_USAGE_GENERIC_MOUSE;
		Rid[0].dwFlags = RIDEV_INPUTSINK;
		Rid[0].hwndTarget = myOwnerHWND;
		RegisterRawInputDevices(Rid, 1, sizeof(Rid[0]));
	}

	bool InputManager::UpdateEvents(UINT message, WPARAM wParam, LPARAM lParam)
	{
		switch (message)
		{
		case WM_MOUSEMOVE:
		{
			RECT clientRect;
			GetClientRect(myOwnerHWND, &clientRect);
			const int windowHeight = clientRect.bottom - clientRect.top;

			const int xPos = GET_X_LPARAM(lParam);
			const int yPos = windowHeight - GET_Y_LPARAM(lParam);

			myTentativeMousePosition.x = xPos;
			myTentativeMousePosition.y = yPos;
		}
		break;
		case WM_INPUT:
		{
			UINT dwSize = sizeof(RAWINPUT);
			static BYTE lpb[sizeof(RAWINPUT)];

			GetRawInputData((HRAWINPUT)lParam, RID_INPUT, lpb, &dwSize, sizeof(RAWINPUTHEADER));

			RAWINPUT* raw = (RAWINPUT*)lpb;

			if (raw->header.dwType == RIM_TYPEMOUSE)
			{
				myTentativeMouseDelta.x += raw->data.mouse.lLastX;
				myTentativeMouseDelta.y += raw->data.mouse.lLastY;
			}
		}
		break;
		case WM_KEYDOWN:
			myKeyLiveState[wParam] = true;
			break;
		case WM_LBUTTONDOWN:
			myKeyLiveState[VK_LBUTTON] = true;
			break;
		case WM_RBUTTONDOWN:
			myKeyLiveState[VK_RBUTTON] = true;
			break;
		case WM_MBUTTONDOWN:
			myKeyLiveState[VK_MBUTTON] = true;
			break;
		case WM_KEYUP:
			myKeyLiveState[wParam] = false;
			break;
		case WM_LBUTTONUP:
			myKeyLiveState[VK_LBUTTON] = false;
			break;
		case WM_RBUTTONUP:
			myKeyLiveState[VK_RBUTTON] = false;
			break;
		case WM_MBUTTONUP:
			myKeyLiveState[VK_MBUTTON] = false;
			break;
		default:
			return false;
		}

		return true;
	}

	void InputManager::ShowMouse()
	{
		ShowCursor(true);
		myMouseIsHidden = false;
	}

	void InputManager::HideMouse()
	{
		ShowCursor(false);
		myMouseIsHidden = true;
	}

	Math::Vector2f InputManager::GetMouseDelta() const
	{
		return Math::Vector2f((float)myMouseDelta.x, (float)myMouseDelta.y);
	}

	Math::Vector2f InputManager::GetMousePosition() const
	{
		return Math::Vector2f((float)myCurrentMousePosition.x, (float)myCurrentMousePosition.y);
	}

	void InputManager::CaptureMouse() const
	{
		RECT clipRect;
		GetClientRect(myOwnerHWND, &clipRect);

		POINT upperLeft = { 0 };
		upperLeft.x = clipRect.left;
		upperLeft.y = clipRect.top;

		POINT lowerRight = { 0 };
		lowerRight.x = clipRect.right;
		lowerRight.y = clipRect.bottom;

		MapWindowPoints(myOwnerHWND, nullptr, &upperLeft, 1);
		MapWindowPoints(myOwnerHWND, nullptr, &lowerRight, 1);

		clipRect.left = upperLeft.x;
		clipRect.top = upperLeft.y;
		clipRect.right = lowerRight.x;
		clipRect.bottom = lowerRight.y;

		ClipCursor(&clipRect);
	}

	void InputManager::ReleaseMouse() const
	{
		ClipCursor(nullptr);
	}

	void InputManager::ResetKeyStates()
	{
		myKeyState.reset();
		myKeyPreviousState.reset();
		myKeyLiveState.reset();
	}
}
