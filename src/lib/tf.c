/*
 * text-file
 * github.com/01mu
 */

#include "tf.h"

int sort_uw(const void * v1, const void * v2)
{
    struct UniqueWord * const * p1 = v1;
    struct UniqueWord * const * p2 = v2;

    return ((*p1)->count - (*p2)->count) - ((*p2)->count - (*p1)->count);
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

int check_word_exists(int wc, struct UniqueWord ** uw, char * check, int found)
{
    int exists = 0;

    struct UniqueWord * unique_word;

    for(int i = 0; i < found; i++)
    {
        if(strcmp(uw[i]->word, check) == 0)
        {
            exists = 1;
            break;
        }
    }

    if(exists == 0)
    {
        unique_word = malloc(sizeof(struct UniqueWord));

        unique_word->word = malloc((sizeof(char) * strlen(check)) + 1);
        unique_word->count = 1;
        unique_word->percentage = ((float) 1 / (float) wc) * 100;

        strcpy(unique_word->word, check);
        uw[found] = unique_word;

        found += 1;
    }
    else
    {
        for(int i = 0; i < found; i++)
        {
            if(strcmp(uw[i]->word, check) == 0)
            {
                uw[i]->count += 1;
                uw[i]->percentage = ((float) uw[i]->count / (float) wc) * 100;
                break;
            }
        }
    }

    return found;
}

void file_get_word_count(struct File * const fl)
{
    for(int i = 0; i < fl->line_count; i++)
    {
        if(fl->lines[i][0] == '\n')
        {
            continue;
        }

        file_explode(fl, fl->lines[i], " ");
        fl->word_count += fl->explode_total;
    }
}

void file_get_unique(struct File * const fl)
{
    char * ptr;
    char ** unique_char_ptr;

    size_t char_arr_size = (fl->line_count * sizeof(char *));

    unique_char_ptr = malloc(char_arr_size);

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
    char * chars = malloc(TF_MAX_LINE);

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

void file_do_replace(struct File * const fl, char * query, char * to)
{
    int line;
    char * original;
    int has_replace;
    struct SearchResult * search;
    int bound;

    for(int i = 0; i < fl->search_count; i++)
    {
        bound = -1;
        search = fl->search_arr[i];

        has_replace = file_str_replace(search, query, to, &bound);

        while(has_replace > 0)
        {
            has_replace = file_str_replace(search, query, to, &bound);
            break;
        }
    }
}

int file_str_replace(struct SearchResult * res, char * q, char * to, int * bd)
{
    int line = res->line;
    char * str = res->string;
    char * right = strstr(str, q) + strlen(q);
    int z;
    int d = 0;
    int loc;
    int count = 0;

    int i;

    if(*bd != -1)
    {
        i = *bd;
    }
    else
    {
        i = 0;
    }

    for( ; i < strlen(str); i++)
    {
        if(str[i] == q[0])
        {
            loc = i;
            z = i;

            while(str[z] == q[d])
            {
                z++;
                d++;

                if(d == strlen(q))
                {
                    *bd = loc + strlen(to);
                    count++;
                    do_replace(res, loc, q, to, *bd);
                }
            }

            z = 0;
            d = 0;
        }

        if(count > 0)
        {
            break;
        }
    }

    return count;
}

char * strstr_new(char * str, int begin)
{
    int len = strlen(str);
    int i = begin;
    int count = 1;
    char * new = malloc(sizeof(str) * count);
    int d = 0;

    while(i != len)
    {
        new[d] = str[i];
        new = realloc(new, sizeof(str) * (count + 1));
        count++;
        i++, d++;
    }

    return new;
}

void do_replace(struct SearchResult * res, int loc, char * q, char * to, int bd)
{
    char * left = malloc(sizeof(char) * loc +
        (strlen(strstr_new(res->string, loc) + strlen(q)) * sizeof(char *)));

    int i = 0;

    while(i < loc)
    {
        left[i] = res->string[i];
        i++;
    }

    strcat(left, to);
    strcat(left, strstr_new(res->string, loc) + strlen(q));

    res->string = realloc(res->string, sizeof(char) * strlen(left));
    strcpy(res->string, left);

    free(left);
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

    char a[TF_MAX_LINE];
    char s[TF_MAX_LINE];

    strcpy(s, arr);

    q = strtok(s, delim);

    if(fl->explode_check == 1)
    {
        free(fl->explode);
    }

    fl->explode = malloc(TF_MAX_LINE * sizeof(char *));

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

    line_arr = malloc(fl->line_count * sizeof(int));

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

    results = malloc(search_malloc);

    for(int i = 0; i < results_idx; i++)
    {
        int str_loc = line_arr[i];

        char * ptr;
        struct SearchResult * search_res_ptr;

        char * line_result = fl->lines[str_loc];

        size_t str_len = strlen(line_result);

        ptr = malloc(TF_MAX_LINE);

        search_res_ptr = malloc(search_malloc);
        search_res_ptr->string = strcpy(ptr, line_result);
        search_res_ptr->line = str_loc + 1;

        results[i] = search_res_ptr;
        search_count++;
    }

    fl->search_count = search_count;
    fl->search_arr = results;

    free(line_arr);
}

int file_init(struct File * const fl, char * file, char * type)
{
    int valid;

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

    if(fl->file_ptr == NULL)
    {
        valid = 0;
    }
    else
    {
        get_file_info(fl);
        get_line_arr(fl);

        valid = 1;
    }

    return valid;
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
    free(file_text);
}

void file_get_text(struct File * fl)
{
    char * file_text = malloc(fl->char_count);

    for(int i = 0; i < fl->line_count; i++)
    {
        strcat(file_text, fl->lines[i]);
    }

    fl->file_text = file_text;
}

void free_lines(char ** lines, int line_count)
{
    char * free_addr;

    for(int i = 0; i < line_count; i++)
    {
        free_addr = lines[i];
        size_t char_size = sizeof(free_addr);

        free(lines[i]);
    }

    free(lines);
}

void free_search(struct SearchResult ** search_results, int search_count)
{
    size_t char_arr_size = sizeof(search_results);
    struct SearchResult * free_addr;

    for(int i = 0; i < search_count; i++)
    {
        free_addr = search_results[i];
        size_t char_size = sizeof(free_addr);

        free(search_results[i]->string);
        free(search_results[i]);
    }

    free(search_results);
}

void get_file_info(struct File * const fl)
{
    int line_count = 0;
    int char_count = 0;

    char line[TF_MAX_LINE];

    while(fgets(line, TF_MAX_LINE, fl->file_ptr) != NULL)
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

    char line[TF_MAX_LINE];
    char ** char_ptr;

    size_t char_size = (sizeof(char *));
    size_t char_arr_size = (fl->line_count * sizeof(char *));

    char_ptr = malloc(char_arr_size);

    while(fgets(line, TF_MAX_LINE, fl->file_ptr) != NULL)
    {
        char * ptr = malloc(TF_MAX_LINE);

        strcpy(ptr, line);

        char_ptr[i] = ptr;

        i++;
    }

    fl->lines = char_ptr;
}

FILE * open_file(char * file, char * type)
{
    FILE *fp = fopen(file, type);

    return fp;
}

void close_file(FILE * fp)
{
    fclose(fp);
}
