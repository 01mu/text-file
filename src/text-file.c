/*
 * text-file
 * github.com/01mu
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "lib/tf.c"

int main(int argc, char ** argv)
{
    struct File fl;

    double end;
    int valid_file;

    int uw_count;
    struct UniqueWord * uw;
    size_t uw_size;

    char query[TF_MAX_LINE];
    char to[TF_MAX_LINE];

    if(argv[1] == NULL || argv[2] == NULL)
    {
        exit(0);
    }

    valid_file = file_init(&fl, argv[1], "r");

    if(valid_file == 0)
    {
        return 0;
    }

    if(strcmp(argv[2], "-s") == 0 || strcmp(argv[2], "-r") == 0)
    {
        printf("Query: ");
        scanf("%[^\n]%*c", query);

        if(query != NULL)
        {
            file_search(&fl, query);

            if(strcmp(argv[2], "-s") == 0)
            {
                file_show_search(&fl);
                printf("Result count: %i\n", fl.search_count);
            }

            if(strcmp(argv[2], "-r") == 0)
            {
                printf("To: ");
                scanf("%[^\n]%*c", to);

                file_do_replace(&fl, query, to);
                file_update_replace(&fl);
                printf("Replace count: %i\n", fl.search_count);
            }
        }
    }

    if(strcmp(argv[2], "-c") == 0 || strcmp(argv[2], "-cs") == 0)
    {
        file_get_word_count(&fl);
        file_get_unique_words(&fl);

        if(strcmp(argv[2], "-c") == 0)
        {
            printf("Char count: %i\n", fl.char_count);
            printf("Line count: %i\n", fl.line_count);
            printf("Word count: %i\n", fl.word_count);
            printf("Unique word count: %i\n", fl.uw_count);
        }

        if(strcmp(argv[2], "-cs") == 0)
        {
            uw_size = sizeof(struct UniqueWord *);
            uw_count = fl.uw_count;

            qsort(fl.unique_words, uw_count, uw_size, sort_uw);

            for(int i = 0; i < uw_count; i++)
            {
                uw = fl.unique_words[i];
                printf("%s %i %f\n", uw->word, uw->count, uw->percentage);
            }
        }
    }

    file_close(&fl);
}

