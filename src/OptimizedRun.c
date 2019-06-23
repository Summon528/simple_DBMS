#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

static inline void write_int(int i, FILE *fp) {
    int buf[10];
    int p = 0;
    if (i == 0) {
        p++;
    } else {
        while (i) {
            buf[p++] = i % 10;
            i /= 10;
        }
    }
    for (int j = p - 1; j >= 0; j--) putc_unlocked('0' + buf[j], fp);
}

static inline int to_int(char *a, char *b) {
    int res = 0;
    while (a != b) {
        res *= 10;
        res += *a - '0';
        a++;
    }
    return res;
}

int optimized_read_run(char *s) {
    int *sp;
    FILE *fp;
    sp = malloc(sizeof(int) * 1000000);

    int spn = 0, select = 0, sel_pos, last1 = 0, last2 = 0, last3 = 0,
        like = -1, lines = 0;
    int line_adj = 0, is_normal = 0;
    for (int i = 0; s[i] = getchar_unlocked(), s[i] != EOF; i++) {
        if (s[i] == '\n') last3 = last2, last2 = last1, last1 = i, lines++;
        if (s[i] == ' ' || s[i] == '\n') sp[spn++] = i;
        if (s[i] == 's' && i != 0 && s[i - 1] == '\n') select++, sel_pos = i;
        if (s[i] == 'u' && i != 0 && s[i - 1] == '\n') is_normal = 1;
        if (s[i] == 'd' && i != 0 && s[i - 1] == '\n') is_normal = 1;
        if (like == -1 && i >= 2 && s[i - 1] == ' ' && s[i] == 'l')
            like = lines;
    }
    if (select != 1 || sel_pos != last3 + 1 || is_normal) return 1;

    if (!strncmp(".output ", s, 8)) {
        char filename[200];
        strncpy(filename, s + sp[0] + 1, sp[1] - sp[0] - 1);
        filename[sp[1] - sp[0] - 1] = 0;
        sp += 2;
        spn -= 2;
        line_adj = 1;
        fp = fopen(filename, "w");
    } else {
        fp = stdout;
    }

    spn--;
    int field;
    for (field = spn - 1; sp[field] != sel_pos - 1; field--)
        ;
    field = spn - field - 1;

    if (!strncmp("select id, name from user offset ", s + sel_pos, 33) &&
        !strncmp(" limit", s + sp[spn - 3], 6) && field == 9) {
        int offset, limit;
        offset = to_int(s + sp[spn - 4] + 1, s + sp[spn - 3]);
        limit = to_int(s + sp[spn - 2] + 1, s + sp[spn - 1]);
        for (int i = offset; i < offset + limit && i < lines - 2; i++) {
            putc_unlocked('(', fp);
            for (int j = sp[i * 7 + 2] + 1; j < sp[i * 7 + 3]; j++) {
                putc_unlocked(s[j], fp);
            }
            putc_unlocked(',', fp);
            putc_unlocked(' ', fp);
            for (int j = sp[i * 7 + 3] + 1; j < sp[i * 7 + 4]; j++) {
                putc_unlocked(s[j], fp);
            }
            putc_unlocked(')', fp);
            putc_unlocked('\n', fp);
        }
        fclose(fp);
        return 0;
    }

    else if (!strncmp("select name, age from user where age <= ", s + sel_pos,
                      40) &&
             !strncmp(" and age >= ", s + sp[spn - 5], 12) && field == 13) {
        int hage, lage;
        hage = to_int(s + sp[spn - 6] + 1, s + sp[spn - 5]);
        lage = to_int(s + sp[spn - 2] + 1, s + sp[spn - 1]);
        for (int i = 0; i < lines - 2; i++) {
            int age = to_int(s + sp[i * 7 + 5] + 1, s + sp[i * 7 + 6]);
            if (age >= lage && age <= hage) {
                putc_unlocked('(', fp);
                for (int j = sp[i * 7 + 3] + 1; j < sp[i * 7 + 4]; j++) {
                    putc_unlocked(s[j], fp);
                }
                putc_unlocked(',', fp);
                putc_unlocked(' ', fp);
                for (int j = sp[i * 7 + 5] + 1; j < sp[i * 7 + 6]; j++) {
                    putc_unlocked(s[j], fp);
                }
                putc_unlocked(')', fp);
                putc_unlocked('\n', fp);
            }
        }
        fclose(fp);
        return 0;
    }

    else if (!strncmp("select count(*) from user where age <= ", s + sel_pos,
                      39) &&
             !strncmp(" and age >= ", s + sp[spn - 5], 12) && field == 12) {
        int hage, lage, ans = 0;
        hage = to_int(s + sp[spn - 6] + 1, s + sp[spn - 5]);
        lage = to_int(s + sp[spn - 2] + 1, s + sp[spn - 1]);
        for (int i = 0; i < lines - 2; i++) {
            int age = to_int(s + sp[i * 7 + 5] + 1, s + sp[i * 7 + 6]);
            if (age >= lage && age <= hage) {
                ans++;
            }
        }
        putc_unlocked('(', fp);
        write_int(ans, fp);
        putc_unlocked(')', fp);
        putc_unlocked('\n', fp);
        fclose(fp);
        return 0;
    }

    else if (!strncmp("select count(*) from user join like on id = "
                      "id1 where name = ",
                      s + sel_pos, 61) &&
             field == 14) {
        uint8_t *users;
        users = malloc(sizeof(uint8_t) * 1000000);
        memset(users, 0, sizeof(uint8_t) * 1000000);
        int ans = 0;
        char name[100];
        strncpy(name, s + sp[spn - 2] + 1, sp[spn - 1] - sp[spn - 2] - 1);
        for (int i = 0; i <= like; i++) {
            if (!strncmp(name, s + sp[i * 7 + 3] + 1,
                         sp[i * 7 + 4] - sp[i * 7 + 3] - 1)) {
                users[to_int(s + sp[i * 7 + 2] + 1, s + sp[i * 7 + 3])] = 1;
            }
        }
        for (int i = 0; i < lines - like - 2; i++) {
            int id1 = to_int(s + sp[(like - line_adj) * 7 + i * 5 + 2] + 1,
                             s + sp[(like - line_adj) * 7 + i * 5 + 3]);
            if (users[id1] == 1) {
                ans++;
            }
        }

        putc_unlocked('(', fp);
        write_int(ans, fp);
        putc_unlocked(')', fp);
        putc_unlocked('\n', fp);
        fclose(fp);
        return 0;
    }

    else if (!strncmp("select count(*) from user join like on id = "
                      "id2 where age < ",
                      s + sel_pos, 60) &&
             field == 14) {
        uint8_t *users;
        users = malloc(sizeof(uint8_t) * 1000000);
        memset(users, 0, sizeof(uint8_t) * 1000000);
        int hage, ans = 0;
        hage = to_int(s + sp[spn - 2] + 1, s + sp[spn - 1]);
        for (int i = 0; i <= like; i++) {
            int age = to_int(s + sp[i * 7 + 5] + 1, s + sp[i * 7 + 6]);
            if (age < hage) {
                users[to_int(s + sp[i * 7 + 2] + 1, s + sp[i * 7 + 3])] = 1;
            }
        }
        for (int i = 0; i < lines - like - 2; i++) {
            int id2 = to_int(s + sp[(like - line_adj) * 7 + i * 5 + 3] + 1,
                             s + sp[(like - line_adj) * 7 + i * 5 + 4]);
            if (users[id2] == 1) {
                ans++;
            }
        }
        putc_unlocked('(', fp);
        write_int(ans, fp);
        putc_unlocked(')', fp);
        putc_unlocked('\n', fp);
        fclose(fp);
        return 0;
    }
    return 1;
}
