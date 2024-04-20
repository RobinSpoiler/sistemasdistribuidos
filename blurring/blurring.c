#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/stat.h>
#include <dirent.h>
#include <omp.h> // Librería para paralelización
// Declaración del prototipo de la función
void outputImagesFolder();
void blurreImageFunction(char* inputfileName, char* outputfileName, char* currentFileName);

void main()
{

    /* Hacer una función que reciba una carpeta con imágenes que:
        1) Se corra una función principal
        2) Se cree otra carpeta con las imagenes modificadas
    */

    // 1) Crear una nueva carpeta y updatear los archivos

    outputImagesFolder();

    // 2) Abrir carpeta con las imagenes originales  y leer todos los archivos en un ciclo

    DIR *dir;
    dir = opendir("originalImages");
    struct dirent *currentfile;
    if (dir == NULL)
    {
        printf("Error al abrir el directorio");
    }
    /* Leyendo uno a uno todos los archivos que hay */
    while ((currentfile = readdir(dir)) != NULL)
    {
        /* Nos devolverá el directorio actual (.) y el anterior (..), como hace ls */
        if ((strcmp(currentfile->d_name, ".") != 0) && (strcmp(currentfile->d_name, "..") != 0))
        {
            // Numero de hilos asignados
            omp_set_num_threads(1);

            // Currentfile name
            char inputfileName[300];  // 300 bytes porque 1 char = 1 byte
            char outputfileName[300]; // 300 bytes porque 1 char = 1 byte
            char currentfilename[300]; // 300 bytes porque 1 char = 1 byte
            sprintf(inputfileName, "originalImages/%s", currentfile->d_name);
            sprintf(outputfileName, "blurredImages/blurred%s", currentfile->d_name);
            sprintf(currentfilename, "%s",currentfile->d_name);
            printf("Nombre del archivo %s---------------\n", inputfileName);

            // Lógica principal
            blurreImageFunction(inputfileName, outputfileName, currentfilename);
        }
    }

    closedir(dir);

    //     free(tmp_pixel_vector);
    //     free(tmp_flip_V_vector);
    //     fclose(image);
    // fclose(outputImage_Blurred);
    //     fclose(outputImage_Flip_horizontal);
    //     fclose(outputImage_Flip_vertical);
    // closedir(dir);
}

void outputImagesFolder()
{
    const char *newfolder = "blurredImages";
    struct stat st;
    if (stat(newfolder, &st) == 0 && S_ISDIR(st.st_mode))
    {
        // Directory exists
        DIR *dir = opendir(newfolder);
        if (dir != NULL)
        {
            // Directory is not empty
            // closedir(dir);
            printf("Directory '%s' already exists and is not empty.\n", newfolder);
        }
        else
        {
            // Failed to open directory
            perror("opendir");
            printf("womp womp Failed to open directory");
        }
    }
    else
    {
        // Directory does not exist or is not a directory, create it
        if (mkdir(newfolder, 0777) == -1)
        {
            // Failed to create directory
            printf("womp womp");
        }
        printf("Directory '%s' created successfully.\n", newfolder);
    }
}

void blurreImageFunction(char* inputfileName, char* outputfileName, char* currentFileName)
{

    FILE *image, *outputImage_Blurred;
    image = fopen(inputfileName, "rb");
    // Asignamos un formato a los espacios reservados para las imágenes
    outputImage_Blurred = fopen(outputfileName, "wb"); // Espacio para output image
    long ancho;
    long alto;
    unsigned char r, g, b; // Pixels (b,g,r) 8 bits
    unsigned char pixel;   // 8 bits

    unsigned char cabecera[54];
    for (int i = 0; i < 54; i++)
    {
        cabecera[i] = fgetc(image);              // Image es la imagen actual
        fputc(cabecera[i], outputImage_Blurred); // Copia cabecera a nueva imagen
    }

    ancho = (long)cabecera[20] * 65536 + (long)cabecera[19] * 256 + (long)cabecera[18];
    alto = (long)cabecera[24] * 65536 + (long)cabecera[23] * 256 + (long)cabecera[22];
    printf("largo img %li\n", alto);
    printf("ancho img %li\n", ancho);

    long imageDimensions = ancho * alto; // Cantidad total de pixeles RGB y esto es un vector

    // Se reserva memoria considerando que cada pixel es RGB (por eso x3) y ocupa 8 bits
    unsigned char *tmp_pixel_vector = malloc(((imageDimensions) * 3) * sizeof(unsigned char));
    // Vector usado para guardar temporalmente el vector modificado/manipulado
    unsigned char *tmp_blurred_vector = malloc(((imageDimensions) * 3) * sizeof(unsigned char)); //(imageDimensions)*3 rgb

    if (tmp_pixel_vector == NULL || tmp_blurred_vector == NULL)
    {
        printf("Error! Memory not allocated.");
        exit(0);
    }

#pragma omp parallel
    {
#pragma omp for schedule(dynamic)
        // LECTURA
        for (int i = 0; i < imageDimensions * 3; i++) // por que cada +1 es como recorrer 3chars porque 1 long son 8 bytes
        {
            // Debe ser ejecutado solo por un solo thread PREGUNTAR COMO SE PUEDE DYNAMIC Y CRITICAL
#pragma omp critical // Critical por que solo un hilo puede modificar estas variables
            b = fgetc(image);
#pragma omp critical
            g = fgetc(image);
#pragma omp critical
            r = fgetc(image);
            // printf("i: %li\t", sizeof(i)); //i recorre 4 bytes 32 bits
            // printf("r|g|b: %ld\t", sizeof(b)); //rgb miden 1 byte

            // Conversion a escala de Grises
            pixel = 0.21 * r + 0.72 * g + 0.07 * b; // 1 byte

            tmp_pixel_vector[i] = pixel;
            tmp_pixel_vector[i + 1] = pixel; //
            tmp_pixel_vector[i + 2] = pixel;
            // tmp_pixel_vector [i+3] = pixel;
            //  de los 4 byes i = los primeros 8 bits, i + 1 = los siguientes 8 bits, i +2 = los siguientes 8 bits 24 bits
        }
// Imagen en escala de grises (o no) sin rotaciomn
#pragma omp for schedule(dynamic)
        for (int i = 0; i < imageDimensions * 3; i++) // eran altoxancho pixeles = 24 bits
        {
            fputc(tmp_pixel_vector[i], outputImage_Blurred);
            fputc(tmp_pixel_vector[i + 1], outputImage_Blurred);
            fputc(tmp_pixel_vector[i + 2], outputImage_Blurred);
        }
    }
    free(tmp_pixel_vector);
    free(tmp_blurred_vector);
    fclose(image);
    fclose(outputImage_Blurred);
}
/* Nos devolverá el directorio actual (.) y el anterior (..), como hace ls */
// TO DO linea 36 infvestigar a que se refiere