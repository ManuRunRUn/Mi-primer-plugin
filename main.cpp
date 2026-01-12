// Inicialización del sistema
ON_PLUGIN_LOAD() {
    video_sock = socket(AF_INET, SOCK_DGRAM, IPPROTO_UDP);
    
    memset(&laptop_addr, 0, sizeof(laptop_addr));
    laptop_addr.sin_family = AF_INET;
    laptop_addr.sin_port = htons(PORT_VIDEO);
    // Usamos Broadcast para que encuentre la laptop en cualquier red local
    laptop_addr.sin_addr.s_addr = INADDR_BROADCAST; 

    int broadcastEnable = 1;
    setsockopt(video_sock, SOL_SOCKET, SO_BROADCAST, &broadcastEnable, sizeof(broadcastEnable));

    return WUPS_INIT_OK;
}

// Función de captura: Revisa que cada llave { } esté cerrada
DECL_FUNCTION(void, GX2CopyScanBuffer, void* buffer, uint32_t scanTarget) {
    // 1. Ejecución original obligatoria
    real_GX2CopyScanBuffer(buffer, scanTarget);

    // 2. Envío de datos si el buffer existe
    if (buffer != NULL && video_sock != -1) {
        // Enviamos el frame en fragmentos de 8KB (Estándar UDP estable)
        sendto(video_sock, buffer, 8192, 0, (struct sockaddr*)&laptop_addr, sizeof(laptop_addr));
    }
} 

// Registro del reemplazo (El "gancho" del sistema)
WUPS_MUST_REPLACE(GX2CopyScanBuffer, WUPS_LOADER_LIBRARY_GX2, GX2CopyScanBuffer);