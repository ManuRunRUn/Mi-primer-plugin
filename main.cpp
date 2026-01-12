#include <wups.h>
#include <gx2/common.h>
#include <nsysnet/socket.h>
#include <string.h>

#define PORT 5001 // Puerto de tu receptor_pro.py

static int video_sock = -1;
static struct sockaddr_in app_addr;
static bool link_established = false;

WUPS_PLUGIN_NAME("XtremeStreamer");
WUPS_PLUGIN_AUTHOR("User");
WUPS_PLUGIN_VERSION("3.0");

WUPS_INITIALIZE() {
    socket_lib_init();
    video_sock = socket(AF_INET, SOCK_DGRAM, IPPROTO_UDP);
    
    // Evitar que el juego se trabe si Windows no responde
    int flag = 1;
    setsockopt(video_sock, SOL_SOCKET, SO_NONBLOCK, &flag, sizeof(flag));
    
    return WUPS_INIT_OK;
}

DECL_FUNCTION(void, GX2CopyScanBuffer, void* buffer, uint32_t target) {
    real_GX2CopyScanBuffer(buffer, target);

    if (video_sock < 0 || buffer == NULL) return;

    // Detectar si tu receptor_pro.py está encendido
    char check[10];
    struct sockaddr_in from;
    int from_len = sizeof(from);
    
    // Si recibimos "WIIU_ID" desde Python, activamos el envío
    if (recvfrom(video_sock, check, sizeof(check), 0, (struct sockaddr*)&from, &from_len) > 0) {
        app_addr = from;
        app_addr.sin_port = htons(PORT);
        link_established = true;
    }

    if (link_established) {
        // Enviamos los datos de la pantalla a tu App
        sendto(video_sock, buffer, 1400, 0, (struct sockaddr*)&app_addr, sizeof(app_addr));
    }
}

WUPS_MUST_REPLACE(GX2CopyScanBuffer, WUPS_LOADER_LIBRARY_GX2, GX2CopyScanBuffer);
