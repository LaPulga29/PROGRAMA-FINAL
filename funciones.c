#include <stdio.h>
#include <string.h>
#include "funciones.h"

int ZONAS = 5;
int DIAS = 30;
int CONTAMINANTES = 4;
float limites_oms[4] = { 3490.0, 15.0, 13.3, 15.0 };
char* nombres_contaminantes[4] = { "CO(ppb)", "SO2(ppb)", "NO2(ppb)", "PM2.5(µg/m³)" };
char* nombres_zonas[5] = { "Norte", "Centro Norte", "Sur", "Centro Sur", "Valles" };

int menu(){
    int opcion = 0, val = 0;
    printf("\nSistema Integral de Gestion y Prediccion de Contaminacion del Aire\n");
    printf("1. Ingresar datos\n");
    printf("2. Calcular promedios historicos\n");
    printf("3. Predecir niveles de contaminacion, alertas y recomendaciones\n");
    printf("4. Exportar reporte\n");
    printf("5. Reiniciar todos los datos\n");
    printf("6. Ver limites de la OMS\n");
    printf("7. Buscar datos por fecha\n");
    printf("8. Ver todos los datos registrados\n");
    printf("0. Salir\n");
    printf("Seleccione una opcion: ");
    fflush(stdin);
    val = scanf("%d", &opcion);
    if (val !=1 || opcion > 8 || opcion < 0) {
        printf("Opcion invalida. Por favor, intente de nuevo.\n");
        return menu(); // Volver a mostrar el menu
    }
    return opcion;
}

void cargar_datos(struct Zona zonas[]) { //NOOOOOOOO
    FILE* f = fopen("datos_zonas.dat", "rb"); 
    if (f == NULL) {
        for(int i=0; i<ZONAS; i++) {
            strcpy(zonas[i].nombre, nombres_zonas[i]);
            zonas[i].actual.fecha[0] = '\0';
            for(int d=0; d<DIAS; d++) {
                zonas[i].historico[d].fecha[0] = '\0';
                for(int j=0; j<CONTAMINANTES; j++) {
                    zonas[i].historico[d].contaminantes[j] = 0;
                }
                zonas[i].historico[d].temperatura = 0;
                zonas[i].historico[d].humedad = 0;
                zonas[i].historico[d].viento = 0;
            }
            for(int j=0; j<CONTAMINANTES; j++) {
                zonas[i].actual.contaminantes[j] = 0;
            }
            zonas[i].actual.temperatura = 0;
            zonas[i].actual.humedad = 0;
            zonas[i].actual.viento = 0;
        }
        return;
    }
    fread(zonas, sizeof(struct Zona), ZONAS, f);
    for(int i=0; i<ZONAS; i++) {
        strcpy(zonas[i].nombre, nombres_zonas[i]);
    }
    fclose(f);
}

void guardar_datos(struct Zona zonas[]) { // Guarda los datos de las zonas en un archivo binario
    FILE* f = fopen("datos_zonas.dat", "wb"); //crea un archivo binario para escritura
    if (f == NULL) {
        printf("Error al guardar datos.\n");
        return;
    }
    fwrite(zonas, sizeof(struct Zona), ZONAS, f);
    fclose(f);
}

void ingresar_datos_actuales(struct Zona zonas[]) { // 1.
    int opcion_zona, val = 0;
    char fecha[20];
    printf("\nSeleccione la zona para ingresar datos:\n");
    for (int i = 0; i < ZONAS; i++) {
        printf("%d. %s\n", i + 1, nombres_zonas[i]); //LISTAR LAS ZONAS
    }
    printf("Ingrese el numero de la zona (1-%d): ", ZONAS);
    fflush(stdin);
    val = scanf("%d", &opcion_zona);
    if (opcion_zona < 1 || opcion_zona > ZONAS || val != 1) {
        printf("Zona invalida.\n");
        return;
    }

    int idx = opcion_zona - 1;

    printf("Ingrese la fecha (DDMMAAAA): ");
    fflush(stdin);
    val = scanf("%s", fecha);
    if (val != 1 || strlen(fecha) != 8) //Calcula el tamaño del string y no funciona sino tiene 8 caracteres 
    {
        printf("Fecha invalida. Debe tener exactamente 8 digitos (DDMMAAAA).\n");
        return;
    }
    for (int i = 0; i < 8; i++)
    {
        if (fecha[i] < '0' || fecha[i] > '9')
        {
            printf("Fecha invalida. Debe contener solo digitos.\n");
            return;
        }
    }
    strcpy(zonas[idx].actual.fecha, fecha); //Copia una cadena a otra incluyendo CN
    // Copia a la estructura

    printf("\nZona: %s\n", zonas[idx].nombre);
    for (int j = 0; j < CONTAMINANTES; j++)
    {
        printf("Ingrese nivel de %s: ", nombres_contaminantes[j]);
        fflush(stdin); // PREGUNTAR POR CADA CONTAMINANTE
        val = scanf("%f", &zonas[idx].actual.contaminantes[j]);
        do
        {
            if (val != 1.0 || zonas[idx].actual.contaminantes[j] < 0.0)
            {
                printf("Entrada invalida. Por favor, ingrese un numero.\n");
                zonas[idx].actual.contaminantes[j] = 0;
            }
        } while (val != 1.0 || zonas[idx].actual.contaminantes[j] < 0);
    }

    do
    {
        printf("Ingrese temperatura actual (grados Centigrados): ");
        fflush(stdin);
        val = scanf("%f", &zonas[idx].actual.temperatura);
        printf("Ingrese humedad actual (%%): ");
        fflush(stdin);
        val = scanf("%f", &zonas[idx].actual.humedad);
        printf("Ingrese velocidad del viento actual(km/h): ");
        fflush(stdin);
        val = scanf("%f", &zonas[idx].actual.viento);
    } while (val != 1 || zonas[idx].actual.temperatura < -50 || zonas[idx].actual.temperatura > 100 || zonas[idx].actual.humedad < 0 || zonas[idx].actual.humedad > 100 || zonas[idx].actual.viento < 0 || zonas[idx].actual.viento > 200);

    // Busca el primer espacio vacío en el histórico
    int pos = -1;
    for(int d=0; d<DIAS; d++) {
        if(zonas[idx].historico[d].fecha[0] == '\0') {
            pos = d;
            break;
        }
    }
    if(pos != -1) {
        zonas[idx].historico[pos] = zonas[idx].actual;
    }
    printf("Datos de la zona '%s' para la fecha %s ingresados correctamente.\n", zonas[idx].nombre, fecha);
}

void calcular_promedios(struct Zona zonas[], float promedios[5][4]) { //2.
    for(int i=0; i<ZONAS; i++) {
        for(int j=0; j<CONTAMINANTES; j++) {
            float suma = 0;
            for(int d=0; d<DIAS; d++) {
                suma += zonas[i].historico[d].contaminantes[j];
            }
            promedios[i][j] = suma / DIAS;
        }
    }
}

void predecir_contaminacion(struct Zona zonas[], float prediccion[5][4]) { //3. 
    for(int i=0; i<ZONAS; i++) {
        for(int j=0; j<CONTAMINANTES; j++) {
            float suma = 0;
            float peso_total = 0;
            for(int d=0; d<DIAS; d++) {
                float peso = DIAS - d;
                suma += zonas[i].historico[d].contaminantes[j] * peso;
                peso_total += peso;
            }
            float ajuste = 1.0; //PROMEDIO PONDERADO
            if(zonas[i].actual.temperatura > 30) ajuste += 0.05;
            if(zonas[i].actual.humedad < 40) ajuste += 0.05;
            if(zonas[i].actual.viento < 5) ajuste += 0.05;
            prediccion[i][j] = (suma / peso_total) * ajuste;
        }
    }
}

void exportar_reporte(struct Zona zonas[], float prediccion[5][4]) {
    for(int i=0; i<ZONAS; i++) {
        if(zonas[i].actual.fecha[0] == '\0') continue;

        // Construir nombre de archivo dinámico: reporte_NombreZona_Fecha.txt
        char nombre_archivo[100];
        char zona_sin_espacios[30];
        strcpy(zona_sin_espacios, zonas[i].nombre);

        // Reemplazar espacios por guiones bajos
        for(int j=0; zona_sin_espacios[j] != '\0'; j++) {
            if(zona_sin_espacios[j] == ' ') zona_sin_espacios[j] = '_';
        }

        sprintf(nombre_archivo, "reporte_%s_%s.txt", zona_sin_espacios, zonas[i].actual.fecha);

        FILE* f = fopen(nombre_archivo, "w");
        if (f == NULL) {
            printf("Error al exportar reporte para zona %s.\n", zonas[i].nombre);
            continue;
        }

        fprintf(f, "Reporte de Contaminacion y Prediccion\n");
        fprintf(f, "Zona: %s\n", zonas[i].nombre);
        fprintf(f, "Fecha de ingreso: %s\n\n", zonas[i].actual.fecha);

        fprintf(f, "Niveles Actuales:\n");
        for(int j=0; j<CONTAMINANTES; j++) {
            fprintf(f, "%s: %.2f\n", nombres_contaminantes[j], zonas[i].actual.contaminantes[j]);
        }

        fprintf(f, "\nCondiciones ambientales:\n");
        fprintf(f, "Temperatura: %.2f °C\n", zonas[i].actual.temperatura);
        fprintf(f, "Humedad: %.2f %%\n", zonas[i].actual.humedad);
        fprintf(f, "Viento: %.2f km/h\n", zonas[i].actual.viento);

        fprintf(f, "\nPrediccion para las proximas 24h:\n");
        for(int j=0; j<CONTAMINANTES; j++) {
            fprintf(f, "%s: %.2f\n", nombres_contaminantes[j], prediccion[i][j]);
        }

        fclose(f);
        printf("Reporte exportado: %s\n", nombre_archivo);
    }
}

void reiniciar_datos(struct Zona zonas[]) { // 5. Reinicia todos los datos de las zonas
    for(int i=0; i<ZONAS; i++) {
        strcpy(zonas[i].nombre, nombres_zonas[i]);
        zonas[i].actual.fecha[0] = '\0';
        for(int d=0; d<DIAS; d++) {
            zonas[i].historico[d].fecha[0] = '\0';
            for(int j=0; j<CONTAMINANTES; j++) {
                zonas[i].historico[d].contaminantes[j] = 0;
            }
            zonas[i].historico[d].temperatura = 0;
            zonas[i].historico[d].humedad = 0;
            zonas[i].historico[d].viento = 0;
        }
        for(int j=0; j<CONTAMINANTES; j++) {
            zonas[i].actual.contaminantes[j] = 0;
        }
        zonas[i].actual.temperatura = 0;
        zonas[i].actual.humedad = 0;
        zonas[i].actual.viento = 0;
    }
    guardar_datos(zonas);
    printf("Todos los datos han sido reiniciados.\n");
}

void mostrar_promedios_historicos(struct Zona zonas[]) {
    int zona_opcion, val = 0;
    printf("\nSeleccione la zona para ver promedios historicos:\n");
    for(int i=0; i<5; i++) {
        printf("%d. %s\n", i+1, nombres_zonas[i]);
    }
    printf("Ingrese el numero de la zona (1-5): ");
    fflush(stdin);
    val = scanf("%d", &zona_opcion);
    if(zona_opcion < 1 || zona_opcion > 5 || val != 1) {
        printf("Zona invalida.\n");
        return;
    }
    int idx = zona_opcion - 1;

    FILE* f = fopen("datoshistoricos.txt", "r");
    float suma[4] = {0};
    int contador = 0;
    int zona_encontrada = 0;
    char linea[256];

    if (f != NULL) {
        while(fgets(linea, sizeof(linea), f)) {
            if(linea[0] == '#' || linea[0] == '\n') continue;
            int len = strlen(linea);
            if(len > 0 && linea[len-1] == '\n') linea[len-1] = '\0';

            if(zona_encontrada == 0 && strcmp(linea, nombres_zonas[idx]) == 0) {
                zona_encontrada = 1;
                continue;
            }

            if(zona_encontrada == 1) {
                int es_otra_zona = 0;
                for(int z=0; z<5; z++) {
                    if(z != idx && strcmp(linea, nombres_zonas[z]) == 0) {
                        es_otra_zona = 1;
                        break;
                    }
                }
                if(es_otra_zona) break;

                char fecha[20], hora[20];
                float c1, c2, c3, c4;
                int leidos = sscanf(linea, "%[^,],%[^,],%f,%f,%f,%f", fecha, hora, &c1, &c2, &c3, &c4);
                if(leidos == 6) {
                    suma[0] += c1;
                    suma[1] += c2;
                    suma[2] += c3;
                    suma[3] += c4;
                    contador++;
                }
            }
        }
        fclose(f);
    } else {
        printf("No se encontro el archivo datoshistoricos.txt. Usando datos actuales.\n");
        return;
    }

    printf("\nPromedio de los ultimos 30 dias para zona %s:\n", nombres_zonas[idx]);
    char* simbolos[4] = {"CO", "SO2", "NO2", "PM2.5"};
    float limites[4] = {3490.0, 15.0, 13.3, 15.0};

    for(int j=0; j<4; j++) {
        float promedio = (contador > 0) ? (suma[j] / contador) : 0;
        printf("%s: %.2f (Limite OMS: %.2f)", simbolos[j], promedio, limites[j]);
        if(promedio > limites[j]) {
            printf("  -> ALERTA\n");
        } else {
            printf("\n");
        }
    }
}

void mostrar_prediccion_alerta_recomendacion(struct Zona zonas[]) {
    // Verifica si hay al menos un dato registrado por el usuario
    int hay_datos = 0;
    for(int i=0; i<ZONAS; i++) {
        for(int d=0; d<DIAS; d++) {
            if(zonas[i].historico[d].fecha[0] != '\0') {
                hay_datos = 1;
                break;
            }
        }
        if(hay_datos) break;
    }
    if(!hay_datos) {
        printf("No hay datos ingresados por el usuario. Regresando al menu principal.\n");
        return;
    }

    int zona_opcion, val = 0;
    printf("\nSeleccione la zona para ver prediccion, alertas y recomendaciones:\n");
    for(int i=0; i<ZONAS; i++) {
        printf("%d. %s\n", i+1, nombres_zonas[i]);
    }
    printf("Ingrese el numero de la zona (1-%d): ", ZONAS);
    fflush(stdin);
    val = scanf("%d", &zona_opcion);
    if(zona_opcion < 1 || zona_opcion > ZONAS || val != 1) {
        printf("Zona invalida.\n");
        return;
    }
    int idx = zona_opcion - 1;

    // Leer datos históricos SOLO de la zona seleccionada
    FILE* f = fopen("datoshistoricos.txt", "r");
    float historico[30][4] = {0};
    int d = 0;
    int zona_encontrada = 0;
    char linea[256];

    if (f != NULL) {
        while(fgets(linea, sizeof(linea), f)) {
            if(linea[0] == '#' || linea[0] == '\n') continue;
            // Elimina el salto de línea al final de la línea
            int len = strlen(linea);
            if(len > 0 && linea[len-1] == '\n') {
                linea[len-1] = '\0';
            }
            // Buscar el nombre de la zona como encabezado
            if(zona_encontrada == 0 && strcmp(linea, nombres_zonas[idx]) == 0) {
                zona_encontrada = 1;
                continue;
            }
            // Si encontramos otra zona, salimos
            if(zona_encontrada == 1) {
                int es_otra_zona = 0;
                for(int z=0; z<ZONAS; z++) {
                    if(z != idx) {
                        // Elimina salto de línea de la línea actual antes de comparar
                        if(strcmp(linea, nombres_zonas[z]) == 0) {
                            es_otra_zona = 1;
                            break;
                        }
                    }
                }
                if(es_otra_zona) break;
                // Leer datos de la zona seleccionada
                char fecha[20], hora[20];
                float c1, c2, c3, c4;
                int leidos = sscanf(linea, "%[^,],%[^,],%f,%f,%f,%f", fecha, hora, &c1, &c2, &c3, &c4);
                if(leidos == 6 && d < 30) {
                    historico[d][0] = c1;
                    historico[d][1] = c2;
                    historico[d][2] = c3;
                    historico[d][3] = c4;
                    d++;
                }
            }
        }
        fclose(f);
    } else {
        printf("No se encontro el archivo datoshistoricos.txt. Usando datos actuales.\n");
    }

    // Aplicar la fórmula de predicción para cada contaminante
    float prediccion[4] = {0};
    float suma_pesos = 0;
    for (int i = 0; i < 30; i++)
    {
        suma_pesos += (30 - i);
    }

    for (int j = 0; j < 4; j++)
    {
        float suma = 0;
        for (int i = 0; i < 30; i++)
        {
            suma += historico[i][j] * (30 - i);
        }

        float promedio_ponderado;
        if (suma_pesos > 0)
        {
            promedio_ponderado = suma / suma_pesos;
        }
        else
        {
            promedio_ponderado = 0;
        }

        float ajuste = 1.0;
        if (zonas[idx].actual.temperatura > 30)
            ajuste += 0.05;
        if (zonas[idx].actual.humedad < 40)
            ajuste += 0.05;
        if (zonas[idx].actual.viento < 5)
            ajuste += 0.05;

        prediccion[j] = promedio_ponderado * ajuste;
    }

    printf("\nPrediccion para zona %s (proximas 24h):\n", nombres_zonas[idx]);
    char* simbolos[4] = {"CO", "SO2", "NO2", "PM2.5"};
    for(int j=0; j<4; j++) {
        printf("%s: %.2f\n", simbolos[j], prediccion[j]);
    }

    int alerta = 0;
    float limites[4] = {3490.0, 15.0, 13.3, 15.0};
    for(int j=0; j<4; j++) {
        if(prediccion[j] > limites[j]) {
            alerta = 1;
            printf("ALERTA: %s predicho %.2f supera limite OMS %.2f\n", simbolos[j], prediccion[j], limites[j]);
        }
    }
    if(alerta) {
        printf("Recomendaciones:\n");
        printf("- Reducir trafico vehicular\n");
        printf("- Considerar cierre temporal de industrias\n");
        printf("- Suspender actividades al aire libre\n");
        printf("- Promover uso de transporte publico\n");
    } else {
        printf("Niveles dentro de limites. Mantener vigilancia.\n");
    }
}

void mostrar_limites_oms() { // 6. Muestra los limites de la OMS para contaminantes
    printf("\n-----------------------------------------------\n");
    printf(" Limites de la OMS para contaminantes (24h)\n");
    printf("-----------------------------------------------\n");
    printf("%-10s %-20s %-15s\n", "Contaminante", "Limite OMS (24h)", "ppb");
    printf("-----------------------------------------------\n");
    printf("%-10s %-20s %-15s\n", "CO", "4 mg/m3", "3490");
    printf("%-10s %-20s %-15s\n", "NO2", "25 ug/m3", "13.3");
    printf("%-10s %-20s %-15s\n", "SO2", "40 ug/m3", "15");
    printf("%-10s %-20s %-15s\n", "PM2.5", "15 ug/m3", "");
    printf("-----------------------------------------------\n");
}

void buscar_por_fecha(struct Zona zonas[]) { // 7. Busca datos ingresados por el usuario por fecha
    int hay_datos = 0;
    for(int i=0; i<ZONAS; i++) {
        if(zonas[i].actual.fecha[0] != '\0') {
            hay_datos = 1;
            break;
        }
    }
    if(!hay_datos) {
        printf("No hay datos ingresados por el usuario. Regresando al menu principal.\n");
        return;
    }

    int val = 0;
    char fecha_buscar[20];
    printf("\nIngrese la fecha a buscar (formato DDMMAAAA): ");
    fflush(stdin);
    val = scanf("%s", fecha_buscar);
    if (val != 1 || fecha_buscar == 0)
    {
        printf("Fecha invalida. Por favor, intente de nuevo.\n");
        return;
    }
    

    int encontrados = 0;
    for(int i=0; i<ZONAS; i++) {
        if(zonas[i].actual.fecha[0] != '\0' && strcmp(zonas[i].actual.fecha, fecha_buscar) == 0) {
            printf("Zona: %s | Fecha: %s\n", zonas[i].nombre, zonas[i].actual.fecha);
            printf("  CO: %.2f | SO2: %.2f | NO2: %.2f | PM2.5: %.2f | Temp: %.2f | Humedad: %.2f | Viento: %.2f\n\n",
                zonas[i].actual.contaminantes[0],
                zonas[i].actual.contaminantes[1],
                zonas[i].actual.contaminantes[2],
                zonas[i].actual.contaminantes[3],
                zonas[i].actual.temperatura,
                zonas[i].actual.humedad,
                zonas[i].actual.viento
            );
            encontrados = 1;
        }
    }
    if(!encontrados) {
        printf("No se encontraron datos ingresados por el usuario para la fecha %s.\n", fecha_buscar);
    }
}

void mostrar_todos_los_datos(struct Zona zonas[]) { // 8. Muestra todos los datos ingresados por el usuario
    printf("\n--- Lista de datos ingresados por el usuario ---\n");
    int hay_datos = 0;
    for(int i=0; i<ZONAS; i++) {
        for(int d=0; d<DIAS; d++) {
            if(zonas[i].historico[d].fecha[0] != '\0') {
                hay_datos = 1;
                printf("Zona: %s | Fecha: %s\n", zonas[i].nombre, zonas[i].historico[d].fecha);
                printf("  CO: %.2f | SO2: %.2f | NO2: %.2f | PM2.5: %.2f | Temp: %.2f | Humedad: %.2f | Viento: %.2f\n\n",
                    zonas[i].historico[d].contaminantes[0],
                    zonas[i].historico[d].contaminantes[1],
                    zonas[i].historico[d].contaminantes[2],
                    zonas[i].historico[d].contaminantes[3],
                    zonas[i].historico[d].temperatura,
                    zonas[i].historico[d].humedad,
                    zonas[i].historico[d].viento
                );
            }
        }
    }
    if(!hay_datos) {
        printf("No hay datos ingresados por el usuario.\n");
    }
}