#include <wups.h>
#include <gx2/common.h>
#include <nsysnet/socket.h> // Red nativa de Wii U, adiós Linux
#include <string.h>
#include <stdint.h>
#include <coreinit/thread.h>

// Puerto donde tu programa de Windows estará escuchando
#define PORT_VIDEO 5000

static int video_sock = -1;
static struct sockaddr_in win_addr;

WUPS_PLUGIN_NAME("XtremeStreamer");
WUPS_PLUGIN_AUTHOR("User");
WUPS_PLUGIN_VERSION("1.6");
WUPS_PLUGIN_LICENSE("GPL");

WUPS_INITIALIZE() {
    // Inicializar librerías de red de la consola
    socket_lib_init();

    video_sock = socket(AF_INET, SOCK_DGRAM, IPPROTO_UDP);
    if (video_sock < 0) return WUPS_INIT_FAILED;

    memset(&win_addr, 0, sizeof(win_addr));
    win_addr.sin_family = AF_INET;
    win_addr.sin_port = htons(PORT_VIDEO);
    
    // Dirección de Broadcast: Manda la señal a toda la red para que Windows la encuentre
    win_addr.sin_addr.s_addr = 0xFFFFFFFF; 

    int broadcastEnable = 1;
    setsockopt(video_sock, SOL_SOCKET, SO_BROADCAST, &broadcastEnable, sizeof(broadcastEnable));

    return WUPS_INIT_OK;
}

DECL_FUNCTION(void, GX2CopyScanBuffer, void* buffer, uint32_t scanTarget) {
    real_GX2CopyScanBuffer(buffer, scanTarget);

    if (buffer != NULL && video_sock >= 0) {
        // Enviamos paquetes de 1400 bytes (estándar de red de Windows para evitar errores)
        sendto(video_sock, buffer, 1400, 0, (struct sockaddr*)&win_addr, sizeof(win_addr));
    }
}

WUPS_MUST_REPLACE(GX2CopyScanBuffer, WUPS_LOADER_LIBRARY_GX2, GX2CopyScanBuffer);
