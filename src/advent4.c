/*
 ============================================================================
 Name        : advent4.c
 Author      : 
 Version     :
 Copyright   : Your copyright notice
 Description : Hello World in C, Ansi-style
 ============================================================================
 */

#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <stdbool.h>
#include <assert.h>

#define ENTRY_COUNT (2000)
#define GUARD_COUNT (10240)

typedef struct
{
    uint64_t datecode;
    uint32_t id;
    enum
    {
        start_shift,
        snooze,
        wakeup,
    } action;
} entry;

static entry entries[ENTRY_COUNT];
static uint32_t guards[GUARD_COUNT][62];

int sorter(void const * a, void const * b);

int main(void)
{
    FILE * input;

    input = fopen("input.txt", "r");
    if (input)
    {
        uint32_t index = 0;
        int result;
        while (!feof(input))
        {
            char line[64];
            char entrystr[64];
            int Y, M, D, h, m;
            fgets(line, sizeof(line), input);
            line[0] = ' ';
            line[5] = ' ';
            line[8] = ' ';
            line[14] = ' ';
            line[17] = ' ';
            if ((result = sscanf(line, " %d %d %d %d %d %[^\n]\n", &Y, &M, &D, &h, &m, entrystr)) == 6)
            {
                int id;
                /* construct datecode */
                uint64_t datecode = ((((uint64_t)Y * 100 + M) * 100 + D) * 100 + h) * 100 + m;
                entries[index].datecode = datecode;
                if (sscanf(entrystr, "Guard #%i begins shift", &id) == 1)
                {
                    /* found id */
                    entries[index].id = id;
                    entries[index].action = start_shift;
                }
                else if (entrystr[0] == 'f')
                {
                    /* fell asleep */
                    entries[index].action = snooze;
                }
                else
                {
                    /* woke up */
                    entries[index].action = wakeup;
                }
                index++;
            }
        }

        /* done with parse */
        qsort(entries,index,sizeof(entry),sorter);

        uint32_t guard = 0;
        uint64_t date = 0;
        int32_t snoozing = -1;
        uint32_t winner = 0;
        uint32_t winnermins = 0;
        uint32_t maxsleeper = 0, maxsleep = 0;
        for (int i  = 0; i < index; i++)
        {
            /* shift started, log the next day */
            if (entries[i].action == start_shift)
            {
                guard = entries[i].id;
                guards[guard][61]++;
                /* date of shift may be tomorrow if we're in the 23rd hour. FIXME: broken logic with month wrapping */
                date = entries[i].datecode / (100*100) + (((entries[i].datecode / 100) % 100 == 0) ? 0 : 1);
                if (guards[guard][61] > maxsleep)
                {
                    maxsleep = guards[guard][61];
                    maxsleeper = guard;
                }
            }
            else if (entries[i].action == snooze)
            {
//                if (date != entries[i].datecode / (100*100))
//                {
//                    printf("date mismatch!\n");
//                }
                if ((snoozing < 0))
                {
                    snoozing = entries[i].datecode % 100;
                }
            }
            else
            {
//                if (date != entries[i].datecode / (100*100))
//                {
//                    printf("date mismatch!\n");
//                }
                if (snoozing > 0)
                {
                    uint32_t waketime = entries[i].datecode % 100;
                    for (int j = snoozing; j < waketime; j++)
                    {
                        guards[guard][j]++;
                        guards[guard][60]++;
                    }
                    if (guards[guard][60] > winnermins)
                    {
                        winner = guard;
                        winnermins = guards[guard][60];
                    }
                    snoozing = -1;
                }
            }
        }

        printf("i caught %i sleeping the most number of times. %i times\n", maxsleeper, maxsleep);

        uint32_t winnermin = 60;
        winnermins = 0;
        for (int k = 0; k < 60; k++)
        {
            if ( guards[winner][k] > winnermins )
            {
                winnermins = guards[winner][k];
                winnermin = k;
            }
        }

        printf ("winning guard is %i with %i mins slept, most commonly %i with %i occurrences. seen sleeping %i times. answer: %i\n",
                winner, guards[winner][60], winnermin, winnermins, guards[winner][61], winner * winnermin);

        return 0;
    }
    return 1;
}


int sorter(void const * a, void const * b)
{
    entry const * ae = a;
    entry const * be = b;

    if ( ae->datecode < be->datecode )
    {
        return -1;
    }
    else if (ae->datecode == be->datecode )
    {
        return 0;
    }
    else
    {
        return 1;
    }
}
