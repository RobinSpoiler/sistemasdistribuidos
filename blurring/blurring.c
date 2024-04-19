#include <stdio.h>
#include <stdlib.h>
#include <omp.h>

void main()
{
    FILE *image, *outputImage_GrayScale, *outputImage_Flip_vertical, *outputImage_Flip_horizontal;
    image = fopen("blackbuck.bmp", "rb"); // Imagen  gran formato (más de 2000 pixeles en ancho o alto) a transformar
    
    // Asignamos un formato a los espacios reservados para las imágenes
    outputImage_GrayScale = fopen("GrayScale.bmp", "wb"); //Imagen en escala de grises
    outputImage_Flip_vertical = fopen("Flip_vertical.bmp", "wb"); // Imagen transformada rotada verticalmente
    outputImage_Flip_horizontal = fopen("Flip_horizontal.bmp", "wb"); // Imagen transformada rotada horizontalmente
    long ancho;
    long alto;
    unsigned char r, g, b; // Pixels (b,g,r) 8 bits 
    unsigned char pixel; // 8 bits

    //Numero de hilos asignados
    omp_set_num_threads(2);

    unsigned char cabecera[54];
    for (int i = 0; i < 54; i++)
    {
        cabecera[i] = fgetc(image);
        fputc(cabecera[i], outputImage_GrayScale); // Copia cabecera a nueva imagen
        fputc(cabecera[i], outputImage_Flip_vertical);
        fputc(cabecera[i], outputImage_Flip_horizontal);
    }

    ancho = (long)cabecera[20] * 65536 + (long)cabecera[19] * 256 + (long)cabecera[18];
    alto = (long)cabecera[24] * 65536 + (long)cabecera[23] * 256 + (long)cabecera[22];
    printf("largo img %li\n", alto);
    printf("ancho img %li\n", ancho);

    //DIMENSIONES MANUALES
    int alto_N = alto; //Preguntar por que se pudo almacenar un long long en un int
    int ancho_N = ancho; 
    long dimension_vector =  ancho_N * alto_N; //Cantidad total de pixeles RGB

    //Se reserva memoria considerando que cada pixel es RGB y ocupa 8 bits
    unsigned char* tmp_pixel_vector = malloc(((dimension_vector)*3)*sizeof (unsigned char));
    //Vector usado para guardar temporalmente el vector girado verticalmente
    unsigned char* tmp_flip_V_vector = malloc(((dimension_vector)*3)*sizeof (unsigned char)); //[0,255].

    if(tmp_pixel_vector == NULL || tmp_flip_V_vector == NULL )
    {
        printf("Error! Memory not allocated.");
        exit(0);
    }

//Uso de clausulas para eliminar "Race condition"
#pragma omp parallel
    {
        #pragma omp for schedule(dynamic)
        //LECTURA
        for (int i = 0; i < dimension_vector; i++)
        {
            // Debe ser ejecutado solo por un solo thread PREGUNTAR COMO SE PUEDE DYNAMIC Y CRITICAL
            //#pragma omp region {} no fue utilizado ya que agrega mas ruido a la imagen
            #pragma omp critical //Critical por que solo un hilo puede modificar estas variables 
            b = fgetc(image);
            #pragma omp critical
            g = fgetc(image);
            #pragma omp critical
            r = fgetc(image);

            //Conversion a escala de Grises
            pixel = 0.21 * r + 0.72 * g + 0.07 * b;

            tmp_pixel_vector [i] = pixel;
            tmp_pixel_vector [i+1] = pixel;
            tmp_pixel_vector [i+2] = pixel;
        }

        //Imagen en escala de grises (o no) sin rotacion
        #pragma omp for schedule(dynamic)
        for (int i = 0; i < dimension_vector*3; i++)
        {
            fputc(tmp_pixel_vector[i], outputImage_GrayScale);
            fputc(tmp_pixel_vector[i+1], outputImage_GrayScale);
            fputc(tmp_pixel_vector[i+2], outputImage_GrayScale);
        }

        //Rotado horizontalmente sobre su eje de simetria
        #pragma omp for schedule(dynamic)
        for (int i = dimension_vector; i > 0; i--)
        {
            fputc(tmp_pixel_vector[(i)], outputImage_Flip_horizontal);
            fputc(tmp_pixel_vector[(i-1)], outputImage_Flip_horizontal);
            fputc(tmp_pixel_vector[(i-2)], outputImage_Flip_horizontal);
        }

        //Rotado verticalmente sobre su eje de simetria
        #pragma omp for schedule(dynamic) collapse(2)
        for (int i = 0; i < (alto_N); i++)
        {
            for (int j = 0; j < (ancho_N); j++)
            {
                tmp_flip_V_vector [j+(ancho_N*i)] = tmp_pixel_vector [(ancho_N-j)+(i*ancho_N)];
                tmp_flip_V_vector [(j+(ancho_N*i))+1]=  tmp_pixel_vector [((ancho_N-j)+(i*ancho_N))-1];
                tmp_flip_V_vector [(j+(ancho_N*i))+2]=  tmp_pixel_vector [((ancho_N-j)+(i*ancho_N))-2];

                fputc(tmp_flip_V_vector[j+(ancho_N*i)], outputImage_Flip_vertical);
                fputc(tmp_flip_V_vector[(j+(ancho_N*i) - 1)], outputImage_Flip_vertical);
                fputc(tmp_flip_V_vector[(j+(ancho_N*i) - 2)], outputImage_Flip_vertical);

            }
        }        
    }


    free(tmp_pixel_vector);
    free(tmp_flip_V_vector);
    fclose(image);
    fclose(outputImage_GrayScale);
    fclose(outputImage_Flip_horizontal);
    fclose(outputImage_Flip_vertical);
}

/*
//V9: 3 operaciones paralelizadas
*/
