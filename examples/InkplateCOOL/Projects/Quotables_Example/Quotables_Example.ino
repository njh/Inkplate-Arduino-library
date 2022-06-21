/*
    Quotables example for Soldered Inkplate COOL
    For this example you will need only USB cable and Inkplate COOL.
    Select "Inkplate COOL(ESP32)" from Tools -> Board menu.
    Don't have "Inkplate COOL(ESP32)" option? Follow our tutorial and add it:
    https://e-radionica.com/en/blog/add-inkplate-6-to-arduino-ide/

    This example shows you how to use simple API call without API key. Response
    from server is in JSON format, so that will be shown too how it is used. What happens
    here is basically ESP32 connects to WiFi and sends API call and server returns HTML
    document containing one quote and some information about it, then using library ArduinoJSON
    we extract only quote from JSON data and show it on Inkplate 6PLUS. After displaying quote
    ESP32 goes to sleep and wakes up every 300 seconds to show new quote(you can change time interval).

    IMPORTANT:
    Make sure to change wifi credentials below
    Also have ArduinoJSON installed in your Arduino libraries, download here: https://arduinojson.org/
    You can deserialize JSON data easily using JSON assistant https://arduinojson.org/v6/assistant/

    Want to learn more about Inkplate? Visit www.inkplate.io
    Looking to get support? Write on our forums: http://forum.e-radionica.com/en/
    21 June 2022 by Soldered.com
*/

// Next 3 lines are a precaution, you can ignore those, and the example would also work without them
#ifndef ARDUINO_INKPLATECOOL
#error "Wrong board selection for this example, please select Inkplate COOL in the boards menu."
#endif

//---------- CHANGE HERE  -------------:

// Put in your ssid and password
char ssid[] = "";
char pass[] = "";

//----------------------------------

// Include Inkplate library to the sketch
#include "Inkplate.h"

// Include fonts used
#include "Fonts/exmouth_32pt7b.h"

// Our networking functions, declared in Network.cpp
#include "Network.h"
#include "driver/rtc_io.h" // Include ESP32 library for RTC pin I/O (needed for rtc_gpio_isolate() function)
#include <rom/rtc.h>       // Include ESP32 library for RTC (needed for rtc_get_reset_reason() function)

// create object with all networking functions
Network network;

// create display object
Inkplate display(INKPLATE_1BIT);

// Delay between API calls in seconds, 300 seconds is 5 minutes
#define DELAY_S 10

// Our functions declared below setup and loop
void drawAll();

char quote[128]; // Buffer to store quote
char author[64];

void setup()
{
    // Begin serial communitcation, sed for debugging
    Serial.begin(115200);

    // Initial display settings
    display.begin();
    display.setTextWrap(false); // Set text wrapping to true
    display.setTextColor(BLACK);
    display.setTextSize(3);

    display.clearDisplay();
    display.display();

    // Our begin function
    network.begin();

    while (!network.getData(quote, author))
    {
        //display.println("Retrying retriving data!");
        //display.partialUpdate(true);
        delay(1000);
    }

    display.clearDisplay();
    drawAll(); //Call funtion to draw screen
    display.display();

    // Go to sleep before checking again
    // This is set in microseconds, so it needs to be
    // multiplied by million to get seconds
    esp_sleep_enable_timer_wakeup(1000000 * DELAY_S);
    (void)esp_deep_sleep_start();
}

void loop()
{
    // Never here
}

// Our main drawing function
void drawAll()
{
    uint8_t rows = strlen(quote) / 60, row = 0;
    display.setFont(&exmouth_32pt7b); // Set custom font
    display.setTextSize(1);
    display.setTextColor(BLACK); //Set text color to black
    display.setCursor(30, display.height() / 2 - 30 * rows); //Place text in the middle
    uint16_t cnt = 0;
    while (quote[cnt] != '\0')
    {
        if (display.getCursorX() > display.width() - 100 && quote[cnt] == ' ')
        {
            row++;
            display.setCursor(30, display.height() / 2 - 30 * rows + row * 50);
        }
        display.print(quote[cnt]);
        cnt++;
    }
    uint16_t w, h;
    int16_t x, y;
    display.getTextBounds(author, 0, 0, &x, &y, &w, &h);
    display.setCursor(display.width() - w - 50, display.height() - 20); // Set cursor to fit author name in lower right corner
    display.print("-");
    display.println(author); // Print author
}
