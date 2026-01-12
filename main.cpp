#include <wups.h>
#include <gx2/common.h>
#include <nsysnet/socket.h> // Red oficial de Wii U (Nativo)
#include <string.h>
#include <stdint.h>
#include <coreinit/thread.h>

#define PORT_VIDEO 5000

static int video_sock = -1;
static struct sockaddr_in laptop_addr;

WUPS_PLUGIN_NAME("XtremeStreamer");
WUPS_PLUGIN_AUTHOR("User");
WUPS_PLUGIN_VERSION("1.0");
WUPS_PLUGIN_LICENSE("GPL");

WUPS_INITIALIZE() {
    // Inicializar la red de la consola
    socket_lib_init();

    video_sock = socket(AF_INET, SOCK_DGRAM, IPPROTO_UDP);
    if (video_sock < 0) return WUPS_INIT_FAILED;

    memset(&laptop_addr, 0, sizeof(laptop_addr));
    laptop_addr.sin_family = AF_INET;
    laptop_addr.sin_port = htons(PORT_VIDEO);
    
    // Broadcast para que tu programa de Windows encuentre la consola
    laptop_addr.sin_addr.s_addr = 0xFFFFFFFF; 

    int broadcastEnable = 1;
    setsockopt(video_sock, SOL_SOCKET, SO_BROADCAST, &broadcastEnable, sizeof(broadcastEnable));

    return WUPS_INIT_OK;
}

DECL_FUNCTION(void, GX2CopyScanBuffer, void* buffer, uint32_t scanTarget) {
    real_GX2CopyScanBuffer(buffer, scanTarget);

    if (buffer != NULL && video_sock >= 0) {
        // Enviamos paquetes de 1400 bytes (MTU ideal para Windows)
        sendto(video_sock, buffer, 1400, 0, (struct sockaddr*)&laptop_addr, sizeof(laptop_addr));
    }
} 

WUPS_MUST_REPLACE(GX2CopyScanBuffer, WUPS_LOADER_LIBRARY_GX2, GX2CopyScanBuffer);
