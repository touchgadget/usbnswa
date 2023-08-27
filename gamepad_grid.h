#include <stdint.h>

const int MAX_ROWS = 4;
const int MAX_COLS = 10;

// Set member alignment to 1. No pad bytes between members.
#pragma pack(push, 1)
typedef struct {
    const char *cell_name;
    const char *key_name;
    uint8_t gamepad_control;
} Touch_Cell_t;

typedef const struct {
    uint8_t rows;
    uint8_t cols;
    const Touch_Cell_t *touch_cells;
} Touch_Page_t;

typedef struct {
  const char *key_code;
  uint8_t gamepad_control;
} Key_Code_t;

#include "switch_ESP32.h"

#define NSDPad_Up       NSButton_Reserved2+1
#define NSDPad_Down     NSDPad_Up+1
#define NSDPad_Left     NSDPad_Down+1
#define NSDPad_Right    NSDPad_Left+1

#define NSLeftStick_Up      NSDPad_Right+1
#define NSLeftStick_Down    NSLeftStick_Up+1
#define NSLeftStick_Left    NSLeftStick_Down+1
#define NSLeftStick_Right   NSLeftStick_Left+1
#define NSRightStick_Up     NSLeftStick_Right+1
#define NSRightStick_Down   NSRightStick_Up+1
#define NSRightStick_Left   NSRightStick_Down+1
#define NSRightStick_Right  NSRightStick_Left+1

const Key_Code_t Key_Codes[] = {
  {"ArrowUp", NSRightStick_Up},
  {"ArrowDown", NSRightStick_Down},
  {"ArrowLeft", NSRightStick_Left},
  {"ArrowRight", NSRightStick_Right},
  {"KeyW", NSLeftStick_Up},
  {"KeyS", NSLeftStick_Down},
  {"KeyA", NSLeftStick_Left},
  {"KeyD", NSLeftStick_Right},
};

const Touch_Cell_t Gamepad_Cells[MAX_ROWS][MAX_COLS] = {
    // Row 0, top row
    {
        {"LSB", "", NSButton_LeftStick},
        {"", "", 255},
        {"", "", 255},
        {"", "", 255},
        {"<b>&uarr;</b><br>DPad", "", NSDPad_Up},
        {"", "", 255},
        {"", "", 255},
        {"", "", 255},
        {"", "", 255},
        {"RSB", "", NSButton_RightStick},
    },
    // Row 1
    {
        {"ZL", "", NSButton_LeftThrottle},
        {"", "", 255},
        {"", "", 255},
        {"<b>&larr;</b>", "", NSDPad_Left},
        {"<b>&darr;</b>", "", NSDPad_Down},
        {"<b>&rarr;</b>", "", NSDPad_Right},
        {"", "", 255},
        {"", "", 255},
        {"", "", 255},
        {"ZR", "", NSButton_RightThrottle},
    },
    // Row 2
    {
        {"L", "", NSButton_LeftTrigger},
        {"<b>&uarr;</b><br>w", "", NSLeftStick_Up},
        {"", "", 255},
        {"<b>-</b>", "", NSButton_Minus},
        {"Capture", "", NSButton_Capture},
        {"Home", "", NSButton_Home},
        {"<b>+</b>", "", NSButton_Plus},
        {"", "", 255},
        {"<b>&uarr;</b>", "", NSRightStick_Up},
        {"R", "", NSButton_RightTrigger},
    },
    // Row 3
    {
        {"<b>&larr;</b><br>a", "", NSLeftStick_Left},
        {"<b>&darr;</b><br>s", "", NSLeftStick_Down},
        {"<b>&rarr;</b><br>d", "", NSLeftStick_Right},
        {"A", "", NSButton_A},
        {"B", "", NSButton_B},
        {"X", "", NSButton_X},
        {"Y", "", NSButton_Y},
        {"<b>&larr;</b>", "", NSRightStick_Left},
        {"<b>&darr;</b>", "", NSRightStick_Down},
        {"<b>&rarr;</b>", "", NSRightStick_Right},
    },
};

Touch_Page_t Gamepad_Page = {
    MAX_ROWS, MAX_COLS, (const Touch_Cell_t *)Gamepad_Cells
};

void Json_touch_grid(uint8_t client, Touch_Page_t &page)
{
  // Build remote control grid for web socket client. Encoded in JSON format.
  String json = "[";
  for (int r = 0; r < page.rows; r++) {
    json.concat('[');
    for (int c = 0; c < page.cols; c++) {
      Touch_Cell_t *cell = (Touch_Cell_t *)page.touch_cells + ((r * page.cols) + c);
      if (cell->cell_name != NULL) {
        DBG_printf("c = %d, r = %d, cell_name = %s\n", c, r, cell->cell_name);
        if (c != 0) json.concat(',');
        json.concat("{\"cellLabel\":\"");
        json.concat(cell->cell_name);
        json.concat('"');
        json.concat('}');
      }
    }
    if (r == (page.rows - 1)) {
      json.concat(']');
    }
    else {
      json.concat("],");
    }
  }
  json.concat(']');
  DBG_println(json);
  webSocket.sendTXT(client, json.c_str(), json.length());
}

#pragma pack(pop)
