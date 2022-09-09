#include "esphome.h"

// Copied and adapted from https://esphome.io/cookbook/uart_text_sensor.html - thanks!
class UARTSensor : public Component, public UARTDevice, public TextSensor {
    public:
        UARTSensor(UARTComponent *parent) : UARTDevice(parent) {}

        void setup() override {
            // nothing to do here
        }

        int readline(int readch, char *buffer, int len)
        {
            static int pos = 0;
            int rpos;

            if (readch > 0) {
                switch (readch) {
                    case '\n': // Ignore new-lines
                        break;
                    case 'x': // Include x and return
                        buffer[pos++] = readch;
                        buffer[pos] = 0;
                        rpos = pos;
                        pos = 0;  // Reset position index ready for next time
                        return rpos;
                    case 0xFF: // Just return on 0xFF
                        rpos = pos;
                        pos = 0;
                        return rpos;
                    default:
                        if (pos < len-1) {
                            buffer[pos++] = readch;
                            buffer[pos] = 0;
                        }
                }
            }
            // No end of line has been found, so return -1.
            return -1;
        }

        void loop() override {
            const int max_line_length = 80;
            static char buffer[max_line_length];
            while (available()) {
                if(readline(read(), buffer, max_line_length) > 0) {
                    publish_state(buffer);
                }
            }
        }
};