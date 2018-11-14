/*
 * text-file
 * github.com/01mu
 */

#ifndef TF_H_
#define TF_H_

#define TF_MAX_LINE 1024

struct File
{
    FILE * file_ptr;

    char * file_text;

    char ** lines;
    char ** unique_chars;
    char ** explode;

    int char_count;
    int line_count;
    int search_count;
    int explode_total;
    int explode_check;
    int word_count;
    int uw_count;

    struct SearchResult ** search_arr;
    struct UniqueWord ** unique_words;
};

struct SearchResult
{
    int line;
    char * string;
};

struct UniqueWord
{
    char * word;
    int count;
    float percentage;
};

FILE * open_file(char * file, char * type);
void close_file(FILE * fp);
void get_file_info(struct File * const fl);
void get_line_arr(struct File * const fl);
void search_init(struct File * const fl, int * line_arr, int results_idx);
int check_char_exists(char * chars, char check, int found);
char * unique_arr(char * check);
int check_word_exists(int wc, struct UniqueWord ** uw, char * check, int found);

void free_lines(char ** lines, int line_count);
void free_search(struct SearchResult ** search_results, int search_count);
void free_file_text(int char_count, char * file_text);
void free_unique(struct File * const fl);

int file_init(struct File * const fl, char * file, char * type);
void file_close(struct File * const fl);
void file_search(struct File * const fl, char * query);
void file_explode(struct File * const fl, char * arr, char * delim);
void file_get_text(struct File * const fl);
void file_get_unique(struct File * const fl);
void file_show_search(struct File * const fl);
void file_get_word_count(struct File * const fl);
void file_get_unique_words(struct File * const fl);

int sort_uw(const void *v1, const void *v2);

#endif
