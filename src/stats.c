/* Подсчет статистик для INT и DOUBLE и вывод результатов по каждому столбцу. */

#include <stdio.h>
#include <math.h>
#include "stats.h"

#define BASE 1000000000LL

void add_int_val(Column *c, long long v) {
    if (c->count == 0 || v < c->min) {
        c->min = v;
    }
    if (c->count == 0 || v > c->max) {
        c->max = v;
    }
    c->count++;

    /* Разбиваем число на миллиарды и остаток, чтобы сумма не переполнилась */
    c->sum_billions += v / BASE;
    c->sum_rem += v % BASE;
    c->sum_billions += c->sum_rem / BASE;
    c->sum_rem %= BASE;

    if (v > 0) {
        c->positive++;
    } else if (v < 0) {
        c->negative++;
    }

    if (v % 2 == 0) {
        c->even++;
    }
}

void add_double_val(Column *c, double d) {
    if (c->count == 0 || d < c->dmin) {
        c->dmin = d;
    }
    if (c->count == 0 || d > c->dmax) {
        c->dmax = d;
    }
    c->count++;
    c->dsum += d;

    if (d > 0.0) {
        c->positive++;
    } else if (d < 0.0) {
        c->negative++;
    }

    if (floor(d) == d) {
        c->integer++;
    }
}

static void print_double_val(double d) {
    if (d >= -0.0000005 && d <= 0.0000005) {
        d = 0.0;
    }
    printf("%.6f", d);
}

/* Вычисление и вывод среднего для INT методом деления в столбик. */
static void print_int_mean(const Column *c) {
    if (c->count == 0) {
        printf("0.000000");
        return;
    }

    long long b = c->sum_billions;
    long long r = c->sum_rem;

    if (b > 0 && r < 0) {
        b--;
        r += BASE;
    } else if (b < 0 && r > 0) {
        b++;
        r -= BASE;
    }

    int is_neg = 0;
    unsigned long long ub, ur;

    if (b < 0 || r < 0) {
        is_neg = 1;
        ub = (unsigned long long)(-(b + 1)) + 1;
        ur = (unsigned long long)(-(r + 1)) + 1;
    } else {
        ub = (unsigned long long)b;
        ur = (unsigned long long)r;
    }

    unsigned long long n = (unsigned long long)c->count;

    unsigned long long whole = (ub / n) * (unsigned long long)BASE;
    unsigned long long rem = (ub % n) * (unsigned long long)BASE + ur;

    whole += rem / n;
    rem %= n;

    unsigned long long frac = 0;
    for (int i = 0; i < 6; i++) {
        rem *= 10;
        frac = frac * 10 + (rem / n);
        rem %= n;
    }

    if (rem * 2 > n || (rem * 2 == n && frac % 2 == 1)) {
        frac++;
        if (frac == 1000000ULL) {
            frac = 0;
            whole++;
        }
    }

    if (is_neg && (whole > 0 || frac > 0)) {
        printf("-");
    }
    printf("%llu.%06llu", whole, frac);
}

void print_col_stats(size_t index, const Column *c) {
    printf("column %zu\n", index);
    printf("name \"%s\"\n", c->name);
    printf("type %s\n", type_to_str(c->type));
    printf("nulls 0\n");

    if (c->type == TYPE_DOUBLE) {
        printf("min ");
        print_double_val(c->dmin);
        printf("\nmax ");
        print_double_val(c->dmax);
        printf("\nmean ");
        print_double_val(c->dsum / (double)c->count);
        printf("\n");
        printf("positive %lld\n", c->positive);
        printf("negative %lld\n", c->negative);
        printf("integer %lld\n", c->integer);
    } else {
        printf("min %lld\n", c->min);
        printf("max %lld\n", c->max);
        printf("mean ");
        print_int_mean(c);
        printf("\n");
        printf("positive %lld\n", c->positive);
        printf("negative %lld\n", c->negative);
        printf("even %lld\n", c->even);
    }
}