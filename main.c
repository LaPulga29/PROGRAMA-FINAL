// main.c
#include <stdio.h>
#include "funciones.h"
int main() {
    struct Zona zonas[5];
    float promedios[5][4] = {0};
    float prediccion[5][4] = {0};
    int opc = 0;
    cargar_datos(zonas);
    do {
        opc = menu();
        switch(opc) {
            case 1:
                ingresar_datos_actuales(zonas);
                guardar_datos(zonas);
                break;
            case 2:
                mostrar_promedios_historicos(zonas);
                break;
            case 3:
                mostrar_prediccion_alerta_recomendacion(zonas);
                break;
            case 4:
                predecir_contaminacion(zonas, prediccion);
                exportar_reporte(zonas, prediccion);
                printf("Reporte exportado.\n");
                break;
            case 5:
                reiniciar_datos(zonas);
                break;
            case 6:
                mostrar_limites_oms();
                break;
            case 7:
                buscar_por_fecha(zonas);
                break;
            case 8:
                mostrar_todos_los_datos(zonas);
                break;
            case 0:
                printf("Saliendo...\n");
                break;
            default:
                printf("Opcion invalida.\n");
        }
    } while(opc != 0);
    return 0;
}