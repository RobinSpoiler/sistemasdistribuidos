#include <stdio.h>
#include <stdlib.h>
#include <sys/stat.h>
#include <dirent.h>
#include <omp.h> // Librería para paralelización
// Declaración del prototipo de la función
void outputImagesFolder();

void main()
{

    /* Hacer una función que reciba una carpeta con imágenes que:
        1) Se corra una función principal
        2) Se cree otra carpeta con las imagenes modificadas
    */

    // Función principal
    // 1) Abrir carpeta con las imagenes originales
    // y leer todos los archivos en un ciclo

    FILE *image, *outputImage_Blurred;
    image = fopen("blackbuck.bmp", "rb");
    // Asignamos un formato a los espacios reservados para las imágenes
    outputImage_Blurred = fopen("blurredImages/GrayScale.bmp", "wb"); // Espacio para imagen en escala de grises
    long ancho;
    long alto;
    unsigned char r, g, b; // Pixels (b,g,r) 8 bits
    unsigned char pixel;   // 8 bits

    // 2) Crear una nueva carpeta y updatear los archivos

    outputImagesFolder();

    //     //Numero de hilos asignados
    //     omp_set_num_threads(2);

    //     unsigned char cabecera[54];
    //     for (int i = 0; i < 54; i++)
    //     {
    //         cabecera[i] = fgetc(image);
    //         fputc(cabecera[i], outputImage_GrayScale); // Copia cabecera a nueva imagen
    //         fputc(cabecera[i], outputImage_Flip_vertical);
    //         fputc(cabecera[i], outputImage_Flip_horizontal);
    //     }

    //     ancho = (long)cabecera[20] * 65536 + (long)cabecera[19] * 256 + (long)cabecera[18];
    //     alto = (long)cabecera[24] * 65536 + (long)cabecera[23] * 256 + (long)cabecera[22];
    //     printf("largo img %li\n", alto);
    //     printf("ancho img %li\n", ancho);

    //     //DIMENSIONES MANUALES
    //     int alto_N = alto; //Preguntar por que se pudo almacenar un long long en un int
    //     int ancho_N = ancho;
    //     long dimension_vector =  ancho_N * alto_N; //Cantidad total de pixeles RGB

    //     //Se reserva memoria considerando que cada pixel es RGB y ocupa 8 bits
    //     unsigned char* tmp_pixel_vector = malloc(((dimension_vector)*3)*sizeof (unsigned char));
    //     //Vector usado para guardar temporalmente el vector girado verticalmente
    //     unsigned char* tmp_flip_V_vector = malloc(((dimension_vector)*3)*sizeof (unsigned char)); //[0,255].

    //     if(tmp_pixel_vector == NULL || tmp_flip_V_vector == NULL )
    //     {
    //         printf("Error! Memory not allocated.");
    //         exit(0);
    //     }

    // //Uso de clausulas para eliminar "Race condition"
    // #pragma omp parallel
    //     {
    //         #pragma omp for schedule(dynamic)
    //         //LECTURA
    //         for (int i = 0; i < dimension_vector; i++)
    //         {
    //             // Debe ser ejecutado solo por un solo thread PREGUNTAR COMO SE PUEDE DYNAMIC Y CRITICAL
    //             //#pragma omp region {} no fue utilizado ya que agrega mas ruido a la imagen
    //             #pragma omp critical //Critical por que solo un hilo puede modificar estas variables
    //             b = fgetc(image);
    //             #pragma omp critical
    //             g = fgetc(image);
    //             #pragma omp critical
    //             r = fgetc(image);

    //             //Conversion a escala de Grises
    //             pixel = 0.21 * r + 0.72 * g + 0.07 * b;

    //             tmp_pixel_vector [i] = pixel;
    //             tmp_pixel_vector [i+1] = pixel;
    //             tmp_pixel_vector [i+2] = pixel;
    //         }

    //         //Imagen en escala de grises (o no) sin rotacion
    //         #pragma omp for schedule(dynamic)
    //         for (int i = 0; i < dimension_vector*3; i++)
    //         {
    //             fputc(tmp_pixel_vector[i], outputImage_GrayScale);
    //             fputc(tmp_pixel_vector[i+1], outputImage_GrayScale);
    //             fputc(tmp_pixel_vector[i+2], outputImage_GrayScale);
    //         }

    //         //Rotado horizontalmente sobre su eje de simetria
    //         #pragma omp for schedule(dynamic)
    //         for (int i = dimension_vector; i > 0; i--)
    //         {
    //             fputc(tmp_pixel_vector[(i)], outputImage_Flip_horizontal);
    //             fputc(tmp_pixel_vector[(i-1)], outputImage_Flip_horizontal);
    //             fputc(tmp_pixel_vector[(i-2)], outputImage_Flip_horizontal);
    //         }

    //         //Rotado verticalmente sobre su eje de simetria
    //         #pragma omp for schedule(dynamic) collapse(2)
    //         for (int i = 0; i < (alto_N); i++)
    //         {
    //             for (int j = 0; j < (ancho_N); j++)
    //             {
    //                 tmp_flip_V_vector [j+(ancho_N*i)] = tmp_pixel_vector [(ancho_N-j)+(i*ancho_N)];
    //                 tmp_flip_V_vector [(j+(ancho_N*i))+1]=  tmp_pixel_vector [((ancho_N-j)+(i*ancho_N))-1];
    //                 tmp_flip_V_vector [(j+(ancho_N*i))+2]=  tmp_pixel_vector [((ancho_N-j)+(i*ancho_N))-2];

    //                 fputc(tmp_flip_V_vector[j+(ancho_N*i)], outputImage_Flip_vertical);
    //                 fputc(tmp_flip_V_vector[(j+(ancho_N*i) - 1)], outputImage_Flip_vertical);
    //                 fputc(tmp_flip_V_vector[(j+(ancho_N*i) - 2)], outputImage_Flip_vertical);

    //             }
    //         }
    //     }

    //     free(tmp_pixel_vector);
    //     free(tmp_flip_V_vector);
    //     fclose(image);
    fclose(outputImage_Blurred);
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
