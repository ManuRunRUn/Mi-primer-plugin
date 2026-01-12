#include <wups.h>
#include <gx2/common.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <string.h>
#include <stdint.h>
#include <stdio.h>
#include <coreinit/thread.h>

// Configuración del Streamer
#define PORT_VIDEO 5000

static int video_sock = -1;
static struct sockaddr_in laptop_addr;

// Información del Plugin para Aroma
WUPS_PLUGIN_NAME("XtremeStreamer");
WUPS_PLUGIN_AUTHOR("User");
WUPS_PLUGIN_VERSION("1.0");
WUPS_PLUGIN_LICENSE("GPL");

// Esto se ejecuta cuando enciendes la consola
WUPS_INITIALIZE() {
    video_sock = socket(AF_INET, SOCK_DGRAM, IPPROTO_UDP);
    if (video_sock < 0) {
        perror("socket");
        return WUPS_INIT_FAILED;
    }

    memset(&laptop_addr, 0, sizeof(laptop_addr));
    laptop_addr.sin_family = AF_INET;
    laptop_addr.sin_port = htons(PORT_VIDEO);
    laptop_addr.sin_addr.s_addr = htonl(INADDR_BROADCAST);

    int broadcastEnable = 1;
    if (setsockopt(video_sock, SOL_SOCKET, SO_BROADCAST, &broadcastEnable, sizeof(broadcastEnable)) < 0) {
        perror("setsockopt SO_BROADCAST");
    }

    return WUPS_INIT_OK;
}

// Función que captura el video de la pantalla
DECL_FUNCTION(void, GX2CopyScanBuffer, void* buffer, uint32_t scanTarget) {
    real_GX2CopyScanBuffer(buffer, scanTarget);

    // Si hay datos, los enviamos por red
    if (buffer != NULL && video_sock != -1) {
        ssize_t sent = sendto(video_sock, buffer, 8192, 0, (struct sockaddr*)&laptop_addr, sizeof(laptop_addr));
        if (sent < 0) {
            perror("sendto");
        }
    }
} 

// Registro del "gancho" para capturar el video
WUPS_MUST_REPLACE(GX2CopyScanBuffer, WUPS_LOADER_LIBRARY_GX2, GX2CopyScanBuffer);
