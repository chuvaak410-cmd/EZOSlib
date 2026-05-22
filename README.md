# EZOSlib
An eazy lib for create your OS right now
# EXAMPLE:
```c
#include "kernel.h"
#include "memory.h"
#include "disk.h"
#include "kalloc.h"
#include "bool.h"

/* Full demonstration of EZOS capabilities */
void OSmain() {
    /* 1. Setup the display environment */
    OSclear();
    OScolor(WHITE, BLUE);
    OSprint("--- EZOS FULL FEATURE DEMO ---\n");
    OScolor(WHITE, BLACK);

    /* 2. Demonstrate Memory Management (kalloc & OSalloc) */
    OSprint("Testing dynamic memory allocation...\n");
    char *dyn_ptr = (char *)kalloc(32);
    if (dyn_ptr != 0) {
        OSprint("kalloc success at: ");
        OSprint_hex((uint32_t)dyn_ptr);
        OSprint("\n");
    }

    /* 3. Demonstrate Disk Access */
    OSprint("Reading sector 0 from disk...\n");
    uint16_t sector_buffer[256];
    OSread_sector(sector_buffer, 0);
    OSprint("Disk sector read completed.\n");

    /* 4. Demonstrate Math and Conversion Utils */
    OSprint("Testing OSitoa (converting 12345): ");
    char num_buf[12];
    OSitoa(12345, num_buf);
    OSprint(num_buf);
    OSprint("\n");

    /* 5. Main Control Loop */
    OSprint("System ready. Type 'help' for commands.\n");
    char input_buf[64];
    
    while(true) {
        OSprint("> ");
        OSinput(input_buf, 64);
        OSprint("\n");

        if (OSstrcmp(input_buf, "help") == 0) {
            OSprint("Commands: reboot, clear, shutdown, hex_test\n");
        } 
        else if (OSstrcmp(input_buf, "reboot") == 0) {
            OSreboot();
        }
        else if (OSstrcmp(input_buf, "clear") == 0) {
            OSclear();
        }
        else if (OSstrcmp(input_buf, "shutdown") == 0) {
            OSshutdown();
        }
        else if (OSstrcmp(input_buf, "hex_test") == 0) {
            OSprint_hex(0xDEADBEEF);
            OSprint("\n");
        }
        else {
            OSprint("Command not found.\n");
        }
    }
}```
