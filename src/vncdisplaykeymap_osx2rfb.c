static const guint16 keymap_osx2rfb[] = {
  [0x0] = 0x1e,        /* 0 (ANSI_A) => 30 via 30 (KEY_A) */
  [0x1] = 0x1f,        /* 1 (ANSI_S) => 31 via 31 (KEY_S) */
  [0x2] = 0x20,        /* 2 (ANSI_D) => 32 via 32 (KEY_D) */
  [0x3] = 0x21,        /* 3 (ANSI_F) => 33 via 33 (KEY_F) */
  [0x4] = 0x23,        /* 4 (ANSI_H) => 35 via 35 (KEY_H) */
  [0x5] = 0x22,        /* 5 (ANSI_G) => 34 via 34 (KEY_G) */
  [0x6] = 0x2c,        /* 6 (ANSI_Z) => 44 via 44 (KEY_Z) */
  [0x7] = 0x2d,        /* 7 (ANSI_X) => 45 via 45 (KEY_X) */
  [0x8] = 0x2e,        /* 8 (ANSI_C) => 46 via 46 (KEY_C) */
  [0x9] = 0x2f,        /* 9 (ANSI_V) => 47 via 47 (KEY_V) */
  [0xa] = 0x70,        /* 10 (ISO_Section) => 112 via 170 (KEY_ISO) */
  [0xb] = 0x30,        /* 11 (ANSI_B) => 48 via 48 (KEY_B) */
  [0xc] = 0x10,        /* 12 (ANSI_Q) => 16 via 16 (KEY_Q) */
  [0xd] = 0x11,        /* 13 (ANSI_W) => 17 via 17 (KEY_W) */
  [0xe] = 0x12,        /* 14 (ANSI_E) => 18 via 18 (KEY_E) */
  [0xf] = 0x13,        /* 15 (ANSI_R) => 19 via 19 (KEY_R) */
  [0x10] = 0x15,       /* 16 (ANSI_Y) => 21 via 21 (KEY_Y) */
  [0x11] = 0x14,       /* 17 (ANSI_T) => 20 via 20 (KEY_T) */
  [0x12] = 0x2,        /* 18 (ANSI_1) => 2 via 2 (KEY_1) */
  [0x13] = 0x3,        /* 19 (ANSI_2) => 3 via 3 (KEY_2) */
  [0x14] = 0x4,        /* 20 (ANSI_3) => 4 via 4 (KEY_3) */
  [0x15] = 0x5,        /* 21 (ANSI_4) => 5 via 5 (KEY_4) */
  [0x16] = 0x7,        /* 22 (ANSI_6) => 7 via 7 (KEY_6) */
  [0x17] = 0x6,        /* 23 (ANSI_5) => 6 via 6 (KEY_5) */
  [0x18] = 0xd,        /* 24 (ANSI_Equal) => 13 via 13 (KEY_EQUAL) */
  [0x19] = 0xa,        /* 25 (ANSI_9) => 10 via 10 (KEY_9) */
  [0x1a] = 0x8,        /* 26 (ANSI_7) => 8 via 8 (KEY_7) */
  [0x1b] = 0xc,        /* 27 (ANSI_Minus) => 12 via 12 (KEY_MINUS) */
  [0x1c] = 0x9,        /* 28 (ANSI_8) => 9 via 9 (KEY_8) */
  [0x1d] = 0xb,        /* 29 (ANSI_0) => 11 via 11 (KEY_0) */
  [0x1e] = 0x1b,       /* 30 (ANSI_RightBracket) => 27 via 27 (KEY_RIGHTBRACE) */
  [0x1f] = 0x18,       /* 31 (ANSI_O) => 24 via 24 (KEY_O) */
  [0x20] = 0x16,       /* 32 (ANSI_U) => 22 via 22 (KEY_U) */
  [0x21] = 0x1a,       /* 33 (ANSI_LeftBracket) => 26 via 26 (KEY_LEFTBRACE) */
  [0x22] = 0x17,       /* 34 (ANSI_I) => 23 via 23 (KEY_I) */
  [0x23] = 0x19,       /* 35 (ANSI_P) => 25 via 25 (KEY_P) */
  [0x24] = 0x1c,       /* 36 (Return) => 28 via 28 (KEY_ENTER) */
  [0x25] = 0x26,       /* 37 (ANSI_L) => 38 via 38 (KEY_L) */
  [0x26] = 0x24,       /* 38 (ANSI_J) => 36 via 36 (KEY_J) */
  [0x27] = 0x28,       /* 39 (ANSI_Quote) => 40 via 40 (KEY_APOSTROPHE) */
  [0x28] = 0x25,       /* 40 (ANSI_K) => 37 via 37 (KEY_K) */
  [0x29] = 0x27,       /* 41 (ANSI_Semicolon) => 39 via 39 (KEY_SEMICOLON) */
  [0x2a] = 0x2b,       /* 42 (ANSI_Backslash) => 43 via 43 (KEY_BACKSLASH) */
  [0x2b] = 0x33,       /* 43 (ANSI_Comma) => 51 via 51 (KEY_COMMA) */
  [0x2c] = 0x35,       /* 44 (ANSI_Slash) => 53 via 53 (KEY_SLASH) */
  [0x2d] = 0x31,       /* 45 (ANSI_N) => 49 via 49 (KEY_N) */
  [0x2e] = 0x32,       /* 46 (ANSI_M) => 50 via 50 (KEY_M) */
  [0x2f] = 0x34,       /* 47 (ANSI_Period) => 52 via 52 (KEY_DOT) */
  [0x30] = 0xf,        /* 48 (Tab) => 15 via 15 (KEY_TAB) */
  [0x31] = 0x39,       /* 49 (Space) => 57 via 57 (KEY_SPACE) */
  [0x32] = 0x29,       /* 50 (ANSI_Grave) => 41 via 41 (KEY_GRAVE) */
  [0x33] = 0xe,        /* 51 (Delete) => 14 via 14 (KEY_BACKSPACE) */
  [0x35] = 0x1,        /* 53 (Escape) => 1 via 1 (KEY_ESC) */
  [0x37] = 0xdb,       /* 55 (Command) => 219 via 125 (KEY_LEFTMETA) */
  [0x38] = 0x2a,       /* 56 (Shift) => 42 via 42 (KEY_LEFTSHIFT) */
  [0x39] = 0x3a,       /* 57 (CapsLock) => 58 via 58 (KEY_CAPSLOCK) */
  [0x3a] = 0x38,       /* 58 (Option) => 56 via 56 (KEY_LEFTALT) */
  [0x3b] = 0x1d,       /* 59 (Control) => 29 via 29 (KEY_LEFTCTRL) */
  [0x3c] = 0x36,       /* 60 (RightShift) => 54 via 54 (KEY_RIGHTSHIFT) */
  [0x3d] = 0xb8,       /* 61 (RightOption) => 184 via 100 (KEY_RIGHTALT) */
  [0x3e] = 0x9d,       /* 62 (RightControl) => 157 via 97 (KEY_RIGHTCTRL) */
  [0x3f] = 0xdd,       /* 63 (Function) => 221 via 127 (KEY_COMPOSE) */
  [0x40] = 0x83,       /* 64 (F17) => 131 via 187 (KEY_F17) */
  [0x41] = 0x53,       /* 65 (ANSI_KeypadDecimal) => 83 via 83 (KEY_KPDOT) */
  [0x43] = 0x37,       /* 67 (ANSI_KeypadMultiply) => 55 via 55 (KEY_KPASTERISK) */
  [0x45] = 0x4e,       /* 69 (ANSI_KeypadPlus) => 78 via 78 (KEY_KPPLUS) */
  [0x47] = 0x7e,       /* 71 (ANSI_KeypadClear????) => 126 via 121 (KEY_KPCOMMA) */
  [0x48] = 0xb0,       /* 72 (VolumeUp) => 176 via 115 (KEY_VOLUMEUP) */
  [0x49] = 0xae,       /* 73 (VolumeDown) => 174 via 114 (KEY_VOLUMEDOWN) */
  [0x4a] = 0xa0,       /* 74 (Mute) => 160 via 113 (KEY_MUTE) */
  [0x4b] = 0xb5,       /* 75 (ANSI_KeypadDivide) => 181 via 98 (KEY_KPSLASH) */
  [0x4c] = 0x9c,       /* 76 (ANSI_KeypadEnter) => 156 via 96 (KEY_KPENTER) */
  [0x4e] = 0x4a,       /* 78 (ANSI_KeypadMinus) => 74 via 74 (KEY_KPMINUS) */
  [0x4f] = 0xf7,       /* 79 (F18) => 247 via 188 (KEY_F18) */
  [0x50] = 0x84,       /* 80 (F19) => 132 via 189 (KEY_F19) */
  [0x51] = 0x59,       /* 81 (ANSI_KeypadEquals) => 89 via 117 (KEY_KPEQUAL) */
  [0x52] = 0x52,       /* 82 (ANSI_Keypad0) => 82 via 82 (KEY_KP0) */
  [0x53] = 0x4f,       /* 83 (ANSI_Keypad1) => 79 via 79 (KEY_KP1) */
  [0x54] = 0x50,       /* 84 (ANSI_Keypad2) => 80 via 80 (KEY_KP2) */
  [0x55] = 0x51,       /* 85 (ANSI_Keypad3) => 81 via 81 (KEY_KP3) */
  [0x56] = 0x4b,       /* 86 (ANSI_Keypad4) => 75 via 75 (KEY_KP4) */
  [0x57] = 0x4c,       /* 87 (ANSI_Keypad5) => 76 via 76 (KEY_KP5) */
  [0x58] = 0x4d,       /* 88 (ANSI_Keypad6) => 77 via 77 (KEY_KP6) */
  [0x59] = 0x47,       /* 89 (ANSI_Keypad7) => 71 via 71 (KEY_KP7) */
  [0x5a] = 0x5a,       /* 90 (F20) => 90 via 190 (KEY_F20) */
  [0x5b] = 0x48,       /* 91 (ANSI_Keypad8) => 72 via 72 (KEY_KP8) */
  [0x5c] = 0x49,       /* 92 (ANSI_Keypad9) => 73 via 73 (KEY_KP9) */
  [0x5d] = 0x7d,       /* 93 (JIS_Yen) => 125 via 124 (KEY_YEN) */
  [0x5f] = 0x5c,       /* 95 (JIS_KeypadComma) => 92 via 95 (KEY_KPJPCOMMA) */
  [0x60] = 0x3f,       /* 96 (F5) => 63 via 63 (KEY_F5) */
  [0x61] = 0x40,       /* 97 (F6) => 64 via 64 (KEY_F6) */
  [0x62] = 0x41,       /* 98 (F7) => 65 via 65 (KEY_F7) */
  [0x63] = 0x3d,       /* 99 (F3) => 61 via 61 (KEY_F3) */
  [0x64] = 0x42,       /* 100 (F8) => 66 via 66 (KEY_F8) */
  [0x65] = 0x43,       /* 101 (F9) => 67 via 67 (KEY_F9) */
  [0x67] = 0x57,       /* 103 (F11) => 87 via 87 (KEY_F11) */
  [0x68] = 0x78,       /* 104 (JIS_Kana????) => 120 via 90 (KEY_KATAKANA) */
  [0x69] = 0x5d,       /* 105 (F13) => 93 via 183 (KEY_F13) */
  [0x6a] = 0x55,       /* 106 (F16) => 85 via 186 (KEY_F16) */
  [0x6b] = 0x5e,       /* 107 (F14) => 94 via 184 (KEY_F14) */
  [0x6d] = 0x44,       /* 109 (F10) => 68 via 68 (KEY_F10) */
  [0x6f] = 0x58,       /* 111 (F12) => 88 via 88 (KEY_F12) */
  [0x71] = 0x5f,       /* 113 (F15) => 95 via 185 (KEY_F15) */
  [0x73] = 0xc7,       /* 115 (Home) => 199 via 102 (KEY_HOME) */
  [0x74] = 0xc9,       /* 116 (PageUp) => 201 via 104 (KEY_PAGEUP) */
  [0x75] = 0xd3,       /* 117 (ForwardDelete) => 211 via 111 (KEY_DELETE) */
  [0x76] = 0x3e,       /* 118 (F4) => 62 via 62 (KEY_F4) */
  [0x77] = 0xcf,       /* 119 (End) => 207 via 107 (KEY_END) */
  [0x78] = 0x3c,       /* 120 (F2) => 60 via 60 (KEY_F2) */
  [0x79] = 0xd1,       /* 121 (PageDown) => 209 via 109 (KEY_PAGEDOWN) */
  [0x7a] = 0x3b,       /* 122 (F1) => 59 via 59 (KEY_F1) */
  [0x7b] = 0xcb,       /* 123 (LeftArrow) => 203 via 105 (KEY_LEFT) */
  [0x7c] = 0xcd,       /* 124 (RightArrow) => 205 via 106 (KEY_RIGHT) */
  [0x7d] = 0xd0,       /* 125 (DownArrow) => 208 via 108 (KEY_DOWN) */
  [0x7e] = 0xc8,       /* 126 (UpArrow) => 200 via 103 (KEY_UP) */
};
