#include <wups.h>
#include <nsysnet/socket.h>
#include <string.h>

// El puerto que configuraste en tu código Python
#define PORT 5001 

static int s = -1;
static struct sockaddr_in addr;

WUPS_PLUGIN_NAME("XtremeStreamer");
WUPS_PLUGIN_AUTHOR("User");
WUPS_PLUGIN_VERSION("1.0");

WUPS_INITIALIZE() {
    socket_lib_init();
    s = socket(AF_INET, SOCK_DGRAM, IPPROTO_UDP);
    
    // Configurar dirección para el broadcast
    memset(&addr, 0, sizeof(addr));
    addr.sin_family = AF_INET;
    addr.sin_port = htons(PORT);
    addr.sin_addr.s_addr = 0xFFFFFFFF; // Manda a todos para que tu app lo vea

    return WUPS_INIT_OK;
}

// Esta función es la que captura la pantalla
DECL_FUNCTION(void, GX2CopyScanBuffer, void* buffer, uint32_t target) {
    real_GX2CopyScanBuffer(buffer, target);

    if (s >= 0 && buffer != NULL) {
        // Enviamos el ID que tu aplicación receptor_pro.py está esperando
        sendto(s, "WIIU_ID", 7, 0, (struct sockaddr*)&addr, sizeof(addr));
    }
}

WUPS_MUST_REPLACE(GX2CopyScanBuffer, WUPS_LOADER_LIBRARY_GX2, GX2CopyScanBuffer);
