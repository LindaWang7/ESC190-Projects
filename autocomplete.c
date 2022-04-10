//March 04 2021
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include "autocomplete.h"

static int compare(const void * term1, const void * term2);

static int compare_weight(const void * term1, const void * term2);

void read_in_terms(struct term **terms, int *pnterms, char *filename){
    //define variable and open the file
    char line[200];
    double weight_temp;
    FILE *fp = fopen(filename,"r");

    //read first line to get the number of terms
    fgets(line,sizeof(line),fp);
    *pnterms = atoi(line);
    int N = *pnterms;

    //allocate space
    struct term * terms_array = (struct term*) malloc(N* sizeof(struct term));

    for(int i = 0; i < *pnterms; i++){
        fgets(line, sizeof(line),fp);

        //skil through spaces
        int j = 0;
        while (isdigit(line[j])==0){
            j++;
        }

        //process weight
        weight_temp = 0;
        while(isdigit(line[j]) > 0){
            weight_temp = weight_temp *10 +(line[j] - '0');
            j++;
        }
        terms_array[i].weight = weight_temp;
        //printf("%lf",weight_temp);

        //process term
        while (line[j] == '\t'){
            j++;
        }
        //skip through empty spaces after weight
        int k = 0;
        while (line[j] != '\0'){
            terms_array[i].term[k] = line [j];
            j++;
            k++;
        }
        terms_array[i].term[k-1] = line[j];



    }
    fclose(fp);
    qsort(terms_array ,*pnterms, sizeof(struct term), compare);
    *terms = terms_array;
    return;

}

int lowest_match(struct term *terms, int nterms, char *substr){
    int low = 0;
    int high = nterms - 1;
    int mid;
    int l = strlen(substr);
    int comparison;
    int cmp_prev_term;

    while (low <= high){
        mid = (high + low) / 2;
        comparison = strncmp(substr, (terms+mid)->term, l);
        if (comparison == 0){
            cmp_prev_term = strncmp(substr, (terms+mid-1)->term, l);
            if (cmp_prev_term != 0 || mid== low){
                return mid;
            }
            high = mid - 1;
        }
        else if (comparison < 0){
            high = mid - 1;
        }
        else{
            low = mid + 1;
        }
    }
    return -1;
}

int highest_match(struct term *terms, int nterms, char *substr){
    int low = 0;
    int high = nterms -1;
    int mid;
    int l = strlen(substr);
    int comparison;
    int cmp_prev_term;

    while (low <= high){
        mid = (high + low) / 2;
        comparison = strncmp(substr, (terms+mid)->term, l);
        if (comparison == 0){
            cmp_prev_term = strncmp(substr, (terms+mid+1)->term, l);
            if (cmp_prev_term != 0 || mid == high){
                return mid;
            }
            low = mid + 1;
        }
        else if (comparison < 0){
            high = mid - 1;
        }
        else{
            low = mid + 1;
        }
    }
    return -1;
}

void autocomplete(struct term **answer, int *n_answer, struct term *terms, int nterms, char *substr) {
    int low = lowest_match(terms, nterms, substr);
    int high = highest_match(terms, nterms, substr);
    char *temp;
    double weight;

    if(low!=-1 || high!=-1){
        *n_answer = high - low + 1;
    }else{
        *n_answer =0;
        return;
    }
    *answer = (struct term *) malloc(sizeof(struct term) * (*n_answer));

    for(int i = 0; i < *n_answer; i++, low++){
        temp = terms[low].term;
        strcpy((*answer)[i].term, temp);
        weight = terms[low].weight;
        (*answer)[i].weight = weight;
    }
    qsort(*answer,*n_answer, sizeof(struct term), compare_weight);

}

int compare(const void * term1, const void * term2){
    return strcmp(((struct term*)term1)->term,((struct term*)term2)->term);
}

int compare_weight(const void * term1, const void*term2){
    double weight1 = ((struct term*)term1)->weight;
    double weight2 = ((struct term*)term2)->weight;
    if(weight2-weight1<0){
        return -1;
    }else if(weight2-weight1>0){
        return 1;
    }else{
        return 0;
    }
}