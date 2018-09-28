/*
 * text-file
 *
 * github.com/01mu
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>

#define MAX_LINE 1024

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
//int check_word_exists(int * counts, char ** chars, char * check, int found);
int check_word_exists(int wc, struct UniqueWord ** unique_words, char * check, int found);

void free_lines(char ** lines, int line_count);
void free_search(struct SearchResult ** search_results, int search_count);
void free_file_text(int char_count, char * file_text);
void free_unique(struct File * const fl);
void txt_alloc_fail();

void file_init(struct File * const fl, char * file, char * type);
void file_close(struct File * const fl);
void file_search(struct File * const fl, char * query);
void file_explode(struct File * const fl, char * arr, char * delim);
void file_get_text(struct File * const fl);
void file_get_unique(struct File * const fl);
void file_show_search(struct File * const fl);
void file_get_word_count(struct File * const fl);
void file_get_unique_words(struct File * const fl);

int main(int argc, char ** argv)
{
    struct File fl;

    clock_t start = clock();
    double end;

    if(argv[1] == NULL || argv[2] == NULL)
    {
        exit(0);
    }

    file_init(&fl, argv[1], "r");

    if(strcmp(argv[2], "-s") == 0)
    {
        char * query = argv[3];

        if(query != NULL)
        {
            printf("%s - \"%s\"\n\n", argv[1], query);

            file_search(&fl, query);
            file_show_search(&fl);

            printf("\nSearch count: %i\n", fl.search_count);
        }
    }

    if(strcmp(argv[2], "-c") == 0 || strcmp(argv[2], "-cs") == 0)
    {
        file_get_word_count(&fl);
        file_get_unique_words(&fl);

        printf("%s\n\n", argv[1]);
        printf("Char count: %i\n", fl.char_count);
        printf("Line count: %i\n", fl.line_count);
        printf("Word count: %i\n", fl.word_count);
        printf("Unique word count: %i\n", fl.uw_count);

        if(strcmp(argv[2], "-cs") == 0)
        {
            struct UniqueWord * uw;

            printf("\n");

            for(int i = 0; i < fl.uw_count; i++)
            {
                uw = fl.unique_words[i];

                printf("%s %i %f\n", uw->word, uw->count, uw->percentage);
            }
        }
    }

    file_close(&fl);

    end = ((double) (clock() - start) / CLOCKS_PER_SEC);

    printf("\n%f\n", end);
}

void free_unique(struct File * const fl)
{
    for(int i = 0; i < fl->uw_count; i++)
    {
        free(fl->unique_words[i]->word);
        free(fl->unique_words[i]);
    }

    free(fl->unique_words);
}

void file_get_unique_words(struct File * const fl)
{
    int found = 0;

    const int wc = fl->word_count;
    const int lc = fl->line_count;

    struct UniqueWord ** uws;

    uws = malloc(sizeof(struct UniqueWord *) * wc);

    for(int i = 0; i < lc; i++)
    {
        file_explode(fl, fl->lines[i], " ");

        for(int z = 0; z < fl->explode_total; z++)
        {
            if(strlen(fl->explode[z]) > 0)
            {
                found = check_word_exists(wc, uws, fl->explode[z], found);
            }
        }
    }

    fl->unique_words = uws;
    fl->uw_count = found;

}

int check_word_exists(int wc, struct UniqueWord ** uws, char * check, int found)
{
    int exists = 0;

    struct UniqueWord * uw;

    for(int i = 0; i < found; i++)
    {
        if(strcmp(uws[i]->word, check) == 0)
        {
            exists = 1;

            break;
        }
    }

    if(exists == 0)
    {
        uw = malloc(sizeof(struct UniqueWord *));

        uw->word = malloc(sizeof(char *));
        uw->count = 1;
        uw->percentage = ((float) 1 / (float) wc) * 100;

        strcpy(uw->word, check);

        uws[found] = uw;

        found += 1;
    }
    else
    {
        for(int i = 0; i < found; i++)
        {
            if(strcmp(uws[i]->word, check) == 0)
            {
                uws[i]->count += 1;
                uws[i]->percentage = ((float) uws[i]->count / (float) wc) * 100;

                break;
            }
        }
    }

    return found;
}

void file_get_word_count(struct File * const fl)
{
    char * str;
    int z = 0;

    for(int i = 0; i < fl->line_count; i++)
    {
        str = fl->lines[i];

        if(str[0] != '\n')
        {
            while(str[z] != '\0')
            {
                if(str[z] == ' ')
                {
                    fl->word_count += 1;
                }

                z++;
            }

            fl->word_count += 1;
        }

        z = 0;
    }
}

void file_get_unique(struct File * const fl)
{
    char * ptr;
    char ** unique_char_ptr;

    size_t char_arr_size = (fl->line_count * sizeof(char *));

    if(unique_char_ptr = malloc(char_arr_size))
    {
        char * msg = "Memory allocated for array: %li Address: %p\n";
        //printf(msg, char_arr_size, char_ptr);
    }
    else
    {
        txt_alloc_fail();
    }

    for(int i = 0; i < fl->line_count; i++)
    {
        ptr = fl->lines[i];
        unique_char_ptr[i] = unique_arr(ptr);
    }

    fl->unique_chars = unique_char_ptr;
}

int check_char_exists(char * chars, char check, int found)
{
    int exists = 0;

    for(int i = 0; i < found; i++)
    {
        if(chars[i] == check)
        {
            exists = 1;
            break;
        }
    }

    if(exists == 0)
    {
        chars[found] = check;
        found += 1;
    }

    return found;
}

char * unique_arr(char * check)
{
    char * chars = malloc(MAX_LINE);

    int i = 1;
    int found = 1;

    chars[0] = check[0];

    while(check[i] != '\0')
    {
        found = check_char_exists(chars, check[i], found);

        i++;
    }

    chars[found] = '\0';

    return chars;
}

void file_show_search(struct File * const fl)
{
    struct SearchResult * search;

    for(int i = 0; i < fl->search_count; i++)
    {
        search = fl->search_arr[i];

        printf("%i: %s", search->line, search->string);
    }
}

void file_explode(struct File * const fl, char * arr,  char * delim)
{
    int idx = 0;

    char * q;
    char a[MAX_LINE];
    char s[MAX_LINE];

    strcpy(s, arr);

    q = strtok(s, delim);

    if(fl->explode_check == 1)
    {
        free(fl->explode);
    }

    if(fl->explode = malloc(MAX_LINE * sizeof(char *)))
    {
        char * msg = "Memory allocated for explode array: %li Address: %p\n";
        //printf(msg, results_idx * sizeof(char *), results);
    }
    else
    {
        txt_alloc_fail();
    }

    while(q != NULL)
    {
        fl->explode[idx] = q;
        q = strtok(NULL, delim);

        idx++;
    }

    fl->explode[idx - 1][strlen(fl->explode[idx - 1]) - 1] = '\0';

    fl->explode_total = idx;
    fl->explode_check = 1;
}

void file_search(struct File * const fl, char * query)
{
    size_t query_len = strlen(query);
    char f = query[0];

    int z = 0;
    int found_flag = 0;
    char * line;
    char a;

    struct SearchResult ** results;
    int results_idx = 0;

    int * line_arr;

    if(fl->search_count > 0)
    {
        free_search(fl->search_arr, fl->search_count);
    }

    if(line_arr = malloc(fl->line_count * sizeof(int)))
    {
        char * msg = "Memory allocated for char: %li Address: %p\n";
        //printf(msg, str_len, ptr);
    }
    else
    {
        txt_alloc_fail();
    }

    for(int i = 0; i < fl->line_count; i++)
    {
        line = fl->lines[i];

        for(int j = 0; j < strlen(line); j++)
        {
            a = line[j];

            if(a == f)
            {
                line += j;

                while(line[z] == query[z])
                {
                    if(z + 1 == query_len)
                    {
                        found_flag = 1;
                        line_arr[results_idx] = i;
                        results_idx++;
                        break;
                    }

                    z++;
                }

                z = 0;
                line -= j;
            }

            if(found_flag == 1)
            {
                found_flag = 0;
                break;
            }
        }
    }

    search_init(fl, line_arr, results_idx);
}

void search_init(struct File * const fl, int * line_arr, int results_idx)
{
    int search_count = 0;
    struct SearchResult ** results;
    size_t search_malloc = results_idx * sizeof(struct SearchResult);

    if(results = malloc(search_malloc))
    {
        char * msg = "Memory allocated for results array: %li Address: %p\n";
        //printf(msg, results_idx * sizeof(char *), results);
    }
    else
    {
        txt_alloc_fail();
    }

    for(int i = 0; i < results_idx; i++)
    {
        int str_loc = line_arr[i];

        char * ptr;
        struct SearchResult * search_res_ptr;

        char * line_result = fl->lines[str_loc];

        size_t str_len = strlen(line_result);

        if(ptr = malloc(MAX_LINE))
        {
            char * msg = "Memory allocated for char: %li Address: %p\n";
            //printf(msg, str_len, ptr);
        }
        else
        {
            txt_alloc_fail();
        }

        if(search_res_ptr = malloc(search_malloc))
        {
            char * msg = "Memory allocated for char: %li Address: %p\n";
            //printf(msg, str_len, ptr);
        }
        else
        {
            txt_alloc_fail();
        }

        search_res_ptr->string = strcpy(ptr, line_result);
        search_res_ptr->line = str_loc + 1;

        results[i] = search_res_ptr;
        search_count++;
    }

    fl->search_count = search_count;
    fl->search_arr = results;

    free(line_arr);
}

void file_init(struct File * const fl, char * file, char * type)
{
    fl->file_text = NULL;
    fl->lines = NULL;
    fl->unique_chars = NULL;
    fl->explode = NULL;
    fl->search_arr = NULL;
    fl->unique_words = NULL;

    fl->char_count = 0;
    fl->line_count = 0;
    fl->search_count = 0;
    fl->explode_total = 0;
    fl->explode_check = 0;
    fl->word_count = 0;
    fl->uw_count = 0;

    fl->file_ptr = open_file(file, type);
    get_file_info(fl);
    get_line_arr(fl);
}

void file_close(struct File * const fl)
{
    if(fl->file_text != NULL)
    {
        free_file_text(fl->char_count, fl->file_text);
    }

    if(fl->unique_chars != NULL)
    {
        free_lines(fl->unique_chars, fl->line_count);
    }

    if(fl->line_count > 0)
    {
        free_lines(fl->lines, fl->line_count);
    }

    if(fl->search_count > 0)
    {
        free_search(fl->search_arr, fl->search_count);
    }

    if(fl->explode_check == 1)
    {
        free(fl->explode);
    }

    if(fl->uw_count > 0)
    {
        free_unique(fl);
    }

    close_file(fl->file_ptr);
}

void free_file_text(int char_count, char * file_text)
{
    char * msg = "Memory freed for file text: %li Address: %p \n";
    //printf(msg, sizeof(char_count), file_text);

    free(file_text);
}

void file_get_text(struct File * fl)
{
    char * file_text;

    if(file_text = malloc(fl->char_count))
    {
        char * msg = "Memory allocated for file text: %li Address: %p\n";
        //printf(msg, char_arr_size, file_text);
    }

    for(int i = 0; i < fl->line_count; i++)
    {
        strcat(file_text, fl->lines[i]);
    }

    fl->file_text = file_text;
}

void free_lines(char ** lines, int line_count)
{
    char * arr_msg = "Memory freed for array: %li Address: %p\n";
    char * free_addr;

    for(int i = 0; i < line_count; i++)
    {
        char * msg = "Memory freed for char: %li Address: %p\n";
        free_addr = lines[i];

        size_t char_size = sizeof(free_addr);

        //printf(msg, char_size, free_addr);
        free(lines[i]);
    }

    //printf(arr_msg, sizeof(lines), lines);
    free(lines);
}

void free_search(struct SearchResult ** search_results, int search_count)
{
    char * arr_msg = "Memory freed for array: %li Address: %p\n";
    size_t char_arr_size = sizeof(search_results);
    struct SearchResult * free_addr;

    for(int i = 0; i < search_count; i++)
    {
        char * msg = "Memory freed for char: %li Address: %p\n";
        free_addr = search_results[i];
        size_t char_size = sizeof(free_addr);

        //printf(msg, char_size, free_addr);
        free(search_results[i]->string);
        free(search_results[i]);
    }

    //printf(arr_msg, char_arr_size, lines);
    free(search_results);
}

void get_file_info(struct File * const fl)
{
    int line_count = 0;
    int char_count = 0;

    char line[MAX_LINE];

    while(fgets(line, MAX_LINE, fl->file_ptr) != NULL)
    {
        char_count += strlen(line);
        line_count++;
    }

    rewind(fl->file_ptr);

    fl->line_count = line_count;
    fl->char_count = (char_count - fl->line_count);
}

void get_line_arr(struct File * const fl)
{
    int i = 0;

    char line[MAX_LINE];
    char ** char_ptr;

    size_t char_size = (sizeof(char *));
    size_t char_arr_size = (fl->line_count * sizeof(char *));

    if(char_ptr = malloc(char_arr_size))
    {
        char * msg = "Memory allocated for array: %li Address: %p\n";
        //printf(msg, char_arr_size, char_ptr);
    }
    else
    {
        txt_alloc_fail();
    }

    while(fgets(line, MAX_LINE, fl->file_ptr) != NULL)
    {
        char * ptr;

        if(ptr = malloc(MAX_LINE))
        {
            char * msg = "Memory allocated for char: %li Address: %p\n";
            //printf(msg, strlen(line), ptr);
        }
        else
        {
            txt_alloc_fail();
        }

        strcpy(ptr, line);

        char_ptr[i] = ptr;

        i++;
    }

    fl->lines = char_ptr;
}

FILE * open_file(char * file, char * type)
{
    FILE *fp;

    if((fp = fopen(file, type)) != NULL)
    {
        //printf("File \"%s\" opened. Address: %p\n", file, fp);
    }
    else
    {
        //printf("File \"%s\" failed to open.\n", file);
    }

    return fp;
}

void close_file(FILE * fp)
{
    if(fp != NULL && fclose(fp) == 0)
    {
        //printf("File closed. Address: %p\n", fp);
    }
    else
    {
        //printf("File failed to close.\n");
    }
}

void txt_alloc_fail()
{
    //printf("Failed to allocate memory.\n");
}
