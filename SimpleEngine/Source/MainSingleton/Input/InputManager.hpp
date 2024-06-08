#pragma once
#include "Engine/Math/Vector2.hpp"
#include <bitset>
#include <Windows.h>
#include <string>

namespace Simpleton
{
	class InputManager final
	{
	public:
		static InputManager& GetInstance()
		{
			static InputManager myInstance;
			return myInstance;
		}

		~InputManager() = default;

		InputManager(const InputManager&) = delete;
		InputManager& operator=(const InputManager&) = delete;

		InputManager(InputManager&&) = default;
		InputManager& operator=(InputManager&&) = default;

		void AppendKeyAsString(std::string& aString, const int aCharactersLimit = 30) const;

		bool UpdateEvents(UINT message, WPARAM wParam, LPARAM lParam);
		void Update();

		bool IsKeyDown(const int aKeyCode) const;
		bool IsKeyReleased(const int aKeyCode) const;
		bool IsKeyPressed(const int aKeyCode) const;
		bool IsKeyHeld(const int aKeyCode) const;

		void ShowMouse();
		void HideMouse();
		void CaptureMouse();
		void ReleaseMouse() const;
		void ResetKeyStates();
	public:
		Math::Vector2f GetMouseDelta() const;
		Math::Vector2f GetMousePosition() const;
		std::string GetKeyAsString() const;
		int GetMouseWheelDelta() const;
		bool GetAKeyIsPressed() const;
		bool GetMouseIsHidden() const;
	public:
		void SetHWND(HWND& aWindowHandle);
	private:
		InputManager();
		void SetCapturedMousePosition();
	private:
		HWND myOwnerHWND = {};

		std::bitset<256> myKeyState;
		std::bitset<256> myKeyPreviousState;
		std::bitset<256> myKeyLiveState;

		Math::Vector2i myTentativeMousePosition;
		Math::Vector2i myCurrentMousePosition; //NOTE(v10.0.2): Relative to Client/HWND Window
		Math::Vector2i myPreviousMousePosition;
		Math::Vector2i myCapturedMousePosition; //NOTE(v10.0.2): Relative to Monitor

		Math::Vector2i myTentativeMouseDelta;
		Math::Vector2i myMouseDelta;

		int myMouseWheelDelta;
		int myTentativeMouseWheelDelta;

		bool myMouseIsHidden;
		bool myAKeyIsPressed;
	};
}