#pragma once

#include <cstdint>

namespace Stellar {

    // Values match GLFW so the Windows backend can pass them straight through.
    enum class KeyCode : uint16_t
    {
        Space = 32, Apostrophe = 39, Comma = 44, Minus = 45, Period = 46, Slash = 47,

        D0 = 48, D1, D2, D3, D4, D5, D6, D7, D8, D9,

        Semicolon = 59, Equal = 61,

        A = 65, B, C, D, E, F, G, H, I, J, K, L, M,
        N, O, P, Q, R, S, T, U, V, W, X, Y, Z,

        LeftBracket = 91, Backslash = 92, RightBracket = 93, GraveAccent = 96,

        Escape = 256, Enter, Tab, Backspace, Insert, Delete,
        Right, Left, Down, Up,
        PageUp, PageDown, Home, End,

        CapsLock = 280, ScrollLock, NumLock, PrintScreen, Pause,

        F1 = 290, F2, F3, F4, F5, F6, F7, F8, F9, F10, F11, F12,

        KP0 = 320, KP1, KP2, KP3, KP4, KP5, KP6, KP7, KP8, KP9,
        KPDecimal, KPDivide, KPMultiply, KPSubtract, KPAdd, KPEnter, KPEqual,

        LeftShift = 340, LeftControl, LeftAlt, LeftSuper,
        RightShift, RightControl, RightAlt, RightSuper, Menu
    };

    enum class MouseButton : uint16_t
    {
        Button0 = 0, Button1, Button2, Button3, Button4, Button5, Button6, Button7,

        Left = Button0,
        Right = Button1,
        Middle = Button2
    };

}
