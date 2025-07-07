
struct RegistroDia {
    char fecha[20]; 
    float contaminantes[4];
    float temperatura;
    float humedad;
    float viento;
};

struct Zona {
    char nombre[30];
    struct RegistroDia historico[30];
    struct RegistroDia actual;
};

// Prototipos
int menu();
void cargar_datos(struct Zona zonas[]);
void guardar_datos(struct Zona zonas[]);
void ingresar_datos_actuales(struct Zona zonas[]);
void calcular_promedios(struct Zona zonas[], float promedios[5][4]);
void predecir_contaminacion(struct Zona zonas[], float prediccion[5][4]);
void exportar_reporte(struct Zona zonas[], float prediccion[5][4]);
void reiniciar_datos(struct Zona zonas[]);
void mostrar_promedios_historicos(struct Zona zonas[]);
void mostrar_prediccion_alerta_recomendacion(struct Zona zonas[]);
void mostrar_limites_oms();
void buscar_por_fecha(struct Zona zonas[]);
void mostrar_todos_los_datos(struct Zona zonas[]);
