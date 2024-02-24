#pragma once
#include "Engine/Math/Vector2.hpp"
#include <bitset>
#include <Windows.h>
#include <string>

namespace SimpleUtilities
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

		bool UpdateEvents(UINT message, WPARAM wParam, LPARAM lParam);
		void Update();

		bool IsDown(const int aKeyCode) const;
		bool IsPressed(const int aKeyCode) const;
		bool IsHold(const int aKeyCode) const;
		bool IsReleased(const int aKeyCode) const;

		void ShowMouse();
		void HideMouse();
		void CaptureMouse() const;
		void ReleaseMouse() const;
		void ResetKeyStates();
	public:
		Math::Vector2f GetMouseDelta() const;
		Math::Vector2f GetMousePosition() const;
		std::string GetKeyAsString(std::string& aString, const int aCharactersLimit = 30) const;
		std::string GetKeyAsString() const;
		bool GetAKeyIsPressed() const;
		bool GetMouseIsHidden() const;
	public:
		void SetHWND(HWND& aWindowHandle);
	private:
		InputManager();
	private:
		HWND myOwnerHWND = {};

		std::bitset<256> myKeyState;
		std::bitset<256> myKeyPreviousState;
		std::bitset<256> myKeyLiveState;

		Math::Vector2i myTentativeMousePosition;
		Math::Vector2i myCurrentMousePosition;
		Math::Vector2i myPreviousMousePosition;

		Math::Vector2i myTentativeMouseDelta;
		Math::Vector2i myMouseDelta;

		bool myMouseIsHidden;
		bool myAKeyIsPressed;
	};
}