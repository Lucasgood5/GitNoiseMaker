#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include <math.h>

// Parse date string YYYY-MM-DD to struct tm
int parse_date(const char *str, struct tm *out) {
    memset(out, 0, sizeof(struct tm));
    int n = sscanf(str, "%d-%d-%d", &out->tm_year, &out->tm_mon, &out->tm_mday);
    if (n != 3) return 0;
    out->tm_year -= 1900;
    out->tm_mon -= 1;
    out->tm_hour = 0;
    out->tm_min = 0;
    out->tm_sec = 0;
    out->tm_isdst = -1;
    return 1;
}

// Set random hour and minute
void set_random_time(struct tm *t) {
    t->tm_hour = rand() % 24;
    t->tm_min = rand() % 60;
    t->tm_sec = rand() % 60;
}

int main(int argc, char *argv[]) {
    if (argc < 4) {
        fprintf(stderr, "Usage: %s <start-date> <end-date> <author> [mean-commits-per-day]\n", argv[0]);
        fprintf(stderr, "Date format: YYYY-MM-DD\n");
        fprintf(stderr, "exemple : GitNoiseMaker 2022-01-01 2022-12-31 \"Lucasgood5 <00000000+Lucasgood5@users.noreply.github.com>\"\n");
        fprintf(stderr, "exemple : GitNoiseMaker 2022-01-01 2022-12-31 \"Lucasgood5 <00000000+Lucasgood5@users.noreply.github.com>\" 25\n");
        return 1;
    }
    struct tm start_tm, end_tm;
    if (!parse_date(argv[1], &start_tm) || !parse_date(argv[2], &end_tm)) {
        fprintf(stderr, "Invalid date format. Use YYYY-MM-DD.\n");
        fprintf(stderr, "exemple : GitNoiseMaker 2022-01-01 2022-12-31 \"Lucasgood5 <00000000+Lucasgood5@users.noreply.github.com>\"\n");
        fprintf(stderr, "exemple : GitNoiseMaker 2022-01-01 2022-12-31 \"Lucasgood5 <00000000+Lucasgood5@users.noreply.github.com>\" 25\n");
        return 1;
    }
    const char *author = argv[3];
    double mean = 10.0;
    if (argc >= 5) {
        mean = atof(argv[4]);
        if (mean <= 0) {
            fprintf(stderr, "mean-commits-per-day must be positive.\n");
            return 1;
        }
    }
    srand((unsigned int)time(NULL));
    time_t start = mktime(&start_tm);
    time_t end = mktime(&end_tm);
    if (start == (time_t)-1 || end == (time_t)-1 || start > end) {
        fprintf(stderr, "Invalid date range.\n");
        return 1;
    }
    int markIndex = 1;
    int index = 1;
    for (time_t day = start; day <= end; day += 86400) {
        struct tm t;
        localtime_r(&day, &t);
        int count = rand() % (int)(2 * mean + 1); 
        if (count > 0) {
            time_t commit_times[count];
            for (int i = 0; i < count; ++i) {
                struct tm commit_tm = t;
                set_random_time(&commit_tm);
                commit_times[i] = mktime(&commit_tm);
            }
            // Sort commit_times
            for (int i = 0; i < count - 1; ++i) {
                for (int j = i + 1; j < count; ++j) {
                    if (commit_times[i] > commit_times[j]) {
                        time_t tmp = commit_times[i];
                        commit_times[i] = commit_times[j];
                        commit_times[j] = tmp;
                    }
                }
            }
            for (int i = 0; i < count; ++i) {
                time_t commit_time = commit_times[i];
                printf("blob\n");
                int blobmark = markIndex++;
                printf("mark :%d\n", blobmark);
                char content[128];
                int clen = snprintf(content, sizeof(content), "Auto generated content %d\n", index);
                printf("data %d\n%s", clen, content);
                if (index == 1) printf("reset refs/heads/auto\n");
                printf("commit refs/heads/auto\n");
                int commark = markIndex++;
                printf("mark :%d\n", commark);
                printf("author %s %ld +0100\n", author, (long)commit_time);
                printf("committer %s %ld +0100\n", author, (long)commit_time);
                char message[128];
                int mlen = snprintf(message, sizeof(message), "Auto generated commit %d", index);
                printf("data %d\n%s\n", mlen, message);
                printf("M 100644 :%d autodata.txt\n\n", blobmark);
                index++;
            }
        }
    }
    return 0;
}