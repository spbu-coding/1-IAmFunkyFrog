#include <stdio.h>
#include <math.h>
#include <malloc.h>

#define MAX_RESULT_STRING_LENGTH 80
#define array_size(x) (sizeof(x) / sizeof(x[0]))
#define error(...) (fprintf(stderr, __VA_ARGS__))
#define PI 3.14

struct interval_t {
    double left_border;
    double right_border;
};

void free_array_of_string(char** array, size_t size);

int read_interval(struct interval_t *interval) {
    if(printf("Enter left and right borders:\n") < 0) {
        error("Cannot write in stdout");
        return -1;
    }
    if(scanf("%lf", &(interval->left_border)) != 1) {
        error("Cannot read from stdin");
        return -1;
    }
    if(scanf("%lf", &(interval->right_border)) != 1) {
        error("Cannot read from stdin");
        return -1;
    }
    if(interval->right_border > PI) {
        error("Right border must be <= PI");
        return -1;
    }
    if(interval->left_border < 0) {
        error("Left border must be >= 0");
        return -1;
    }
    if(interval->left_border > interval->right_border) {
        error("Right border must be > left border");
        return -1;
    }
    return 0;
}

double square_function(struct interval_t interval) {
    return sin((interval.right_border + interval.left_border) / 2) * (interval.right_border - interval.left_border);
}

double sympson_function(struct interval_t interval) {
    return (interval.right_border - interval.left_border) * (sin(interval.left_border) + 4 * sin((interval.left_border + interval.right_border) / 2) + sin(interval.right_border)) / 6;
}

double integrate(struct interval_t interval, unsigned int accuracy, double (*integral_function)(struct interval_t)) {
    double S = 0;
    double step = (interval.right_border - interval.left_border) / (double)accuracy;
    for(unsigned int i = 0; i < accuracy; i++) {
        interval.right_border = interval.left_border + step;
        S += integral_function(interval);
        interval.left_border = interval.right_border;
    }
    return S;
}

char** calculate_integrals(struct interval_t interval, const unsigned int accuracies[], size_t experiments_count) {
    //Подготовка массива строк нужной длины
    char** results = (char**)malloc(experiments_count * sizeof(char*));
    if(!results) {
        error("Cannot get memory from malloc");
        return NULL;
    }
    for(unsigned int i = 0; i < experiments_count; i++) {
        results[i] = (char*)malloc(MAX_RESULT_STRING_LENGTH);
        if(!results[i]) {
            free_array_of_string(results, i);
            error("Cannot get memory from malloc to %d string", i);
            return NULL;
        }
    }
    //Интегрирование и заполнение массива строк
    for(int i = 0; i < experiments_count; i++) {
        int accuracy = accuracies[i];
        double S1 = integrate(interval, accuracy, square_function);
        double S2 = integrate(interval, accuracy, sympson_function);
        if(sprintf(results[i], "%d %.5lf %.5lf", accuracy, S1, S2) == 0) {
            free_array_of_string(results, experiments_count);
            error("Cannot write result %d in results", i);
            return NULL;
        }
    }

    return results;
}

void free_array_of_string(char** array, size_t size) {
    for(unsigned int i = 0; i < size; i++)
        free(array[i]);
    free(array);
}

int main() {
    struct interval_t interval = {0.0, 0.0};
    unsigned int accuracies[] = {5, 10, 20, 100, 500, 1000};
    size_t experiments_count = array_size(accuracies);
    //считывание значений границ
    if(read_interval(&interval) < 0)
        return 1;
    //проинтегрируем и получим результаты
    char** results = calculate_integrals(interval, accuracies, experiments_count);
    if(!results)
        return 1;
    //Выведем результаты на экран
    if(printf("Results:\n") < 0) {
        error("Cannot write in stdout");
        return 1;
    }
    for(unsigned int i = 0; i < experiments_count; i++)
        puts(results[i]);
    //Освободим память
    free_array_of_string(results, (size_t)experiments_count);

    return 0;
}
