//
// Created by Linda Wang on 2021-04-07.
//

#include "stdlib.h"
#include "seamcarving.h"
#include "c_img.h"
#include "math.h"

int array_min(double *arr, int width, int row, int row_start, int row_end){
    int abs_start = (row * width) + row_start;
    int abs_end = (row * width) + row_end;
    double min = 10000000;
    int min_index = abs_start;
    for (int i=abs_start; i<(abs_end+1); i++){
        if (arr[i] < min){
            min = arr[i];
            min_index = i;
        }
    }
    return min_index%width;
}
void calc_energy(struct rgb_img *im, struct rgb_img **grad){

    create_img(grad, im->height, im->width);
    double col_x[3], col_y[3], temp_left, temp_right, temp_top, temp_bottom;

    //height: i     width: j
    for (int i = 0; i < (im ->height); i++){

        for(int j = 0; j < (im ->width); j++){

            double sum = 0;

            for(int k = 0; k<3; k++){

                if (j == 0){
                    temp_left = get_pixel(im, i, (im ->width)-1, k);
                }else{
                    temp_left = get_pixel(im, i, j-1, k);
                }

                if (j == (im ->width)-1){
                    temp_right = get_pixel(im, i, 0, k);
                }else{
                    temp_right = get_pixel(im, i, j+1, k);
                }

                if (i == 0){
                    temp_top = get_pixel(im, (im ->height)-1, j, k);
                }else{
                    temp_top = get_pixel(im, i-1, j, k);
                }

                if (i == (im ->height)-1){
                    temp_bottom = get_pixel(im, 0, j, k);
                }else{
                    temp_bottom= get_pixel(im, i+1, j, k);
                }

                col_x[k]= pow(temp_right - temp_left , 2);
                col_y[k]=pow(temp_bottom - temp_top , 2);
            }
            for (int x=0; x<3; x++){
                sum += col_x[x] + col_y[x];
            }
            sum = sqrt(sum);
            uint8_t dual_gradient_energy = (uint8_t)(sum/10);
            set_pixel(*grad, i, j, dual_gradient_energy, dual_gradient_energy, dual_gradient_energy);
        }
    }
}

void dynamic_seam(struct rgb_img *grad, double **best_arr){

    *best_arr = (double *)malloc((grad->width) * (grad->height) * sizeof(double));

    for(int h=0;h<(grad->height);h++){
        for(int w=0;w<(grad->width);w++){
            if(h==0){
                (*best_arr)[w] = (double)get_pixel(grad,h,w,0);
            }else{
                double min;
                if(w==0){
                    double cmp1= (*best_arr)[(h-1)*(grad->width)+w], cmp2 = (*best_arr)[(h-1)*(grad->width)+w+1];
                    min = fmin( cmp1 , cmp2) + get_pixel(grad,h,w,0);

                }else if(w == (grad->width)-1){
                    double cmp1 = (*best_arr)[(h-1)*(grad->width)+w], cmp2=(*best_arr)[(h-1)*(grad->width)+w-1];
                    min = fmin(cmp1 , cmp2) + get_pixel(grad,h,w,0);
                }else{
                    double cmp1 = fmin((*best_arr)[(h-1)*(grad->width)+w+1],(*best_arr)[(h-1)*(grad->width)+w]);
                    double cmp2 = (*best_arr)[(h-1)*(grad->width)+w-1];
                    min = fmin(cmp1 , cmp2) + get_pixel(grad,h,w,0);
                }
                (*best_arr)[h*(grad->width)+w] = min;
            }
        }
    }
}

void recover_path(double *best, int height, int width, int **path){
    int last_row_index, prev_row_index, cur_row_index;
    int start_index, end_index;

    *path = (int *)malloc(sizeof(int) * height);

    last_row_index = array_min(best, width, height-1, 0, width-1);
    (*path)[height-1] = last_row_index;
    prev_row_index = last_row_index;

    for (int row=(height-2); row>-1; row--){
        start_index = prev_row_index - 1;
        end_index = prev_row_index + 1;
        if (prev_row_index == 0){
            start_index = 0;
        } else if (prev_row_index == (width-1)){
            end_index = width - 1;
        }
        cur_row_index = array_min(best, width, row, start_index, end_index);
        (*path)[row] = cur_row_index;
        prev_row_index = cur_row_index;
    }
}

void remove_seam(struct rgb_img *src, struct rgb_img **dest, int *path){
    create_img(dest, src->height, src->width-1);
    int y,x;
    for (y=0; y < src->height ;y++){
        for (x=0; x < src->width ;x++){
            if(x < path[y]){
                set_pixel(*dest,y,x,get_pixel(src,y,x,0),get_pixel(src,y,x,1),get_pixel(src,y,x,2));
            }else if (x>path[y]){
                set_pixel(*dest,y,x-1,get_pixel(src,y,x,0),get_pixel(src,y,x,1),get_pixel(src,y,x,2));
            }
        }
    }
}

//int main(void){
//
//    struct rgb_img *im;
//    struct rgb_img *grad;
//    double *best_arr;
//
//    char filename[] = "/Users/lindawang/CLionProjects/steam_caving/6x5.bin";
//
//    read_in_img(&im, filename);
//
//    //part 1
//    calc_energy(im, &grad);
//    print_grad(grad);
//
//    //part 2
//    dynamic_seam(grad, &best_arr);
//
//    for (int h = 0; h < grad->height; h++) {
//        printf("\n");
//        for (int w = 0; w < grad->width; w++) {
//            printf("%f ", (best_arr)[h * grad->width + w]);
//        }
//    }
//    printf("\n");
//
//    //part 3
//
//    int *path;
//    recover_path(best_arr, grad->height, grad->width, &path);
//    for (int i = 0; i < grad->height; i++) {
//        printf("%d ", path[i]);
//    }
//
//    return 0;
//}