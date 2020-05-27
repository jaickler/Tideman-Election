#include <cs50.h>
#include <stdio.h>
#include <string.h>

// Max number of candidates
#define MAX 9

// preferences[i][j] is number of voters who prefer i over j
int preferences[MAX][MAX];

// locked[i][j] means i is locked in over j
bool locked[MAX][MAX];

// Each pair has a winner, loser
typedef struct
{
    int winner;
    int loser;
}
pair;

// Array of candidates
string candidates[MAX];
pair pairs[MAX * (MAX - 1) / 2];

int pair_count;
int candidate_count;

// Function prototypes
bool vote(int rank, string name, int ranks[]);
void record_preferences(int ranks[]);
void add_pairs(void);
void sort_pairs(void);
void lock_pairs(void);
void print_winner(void);
bool check_loop(int iteration, int loser_index);
int find_winner(int root);

int main(int argc, string argv[])
{
    // Check for invalid usage
    if (argc < 2)
    {
        printf("Usage: tideman [candidate ...]\n");
        return 1;
    }

    // Populate array of candidates
    candidate_count = argc - 1;
    if (candidate_count > MAX)
    {
        printf("Maximum number of candidates is %i\n", MAX);
        return 2;
    }
    for (int i = 0; i < candidate_count; i++)
    {
        candidates[i] = argv[i + 1];
    }

    // Clear graph of locked in pairs
    for (int i = 0; i < candidate_count; i++)
    {
        for (int j = 0; j < candidate_count; j++)
        {
            locked[i][j] = false;
        }
    }

    pair_count = 0;
    int voter_count = get_int("Number of voters: ");

    // Query for votes
    for (int i = 0; i < voter_count; i++)
    {
        // ranks[i] is voter's ith preference
        int ranks[candidate_count];

        // Query for each rank
        for (int j = 0; j < candidate_count; j++)
        {
            string name = get_string("Rank %i: ", j + 1);

            if (!vote(j, name, ranks))
            {
                printf("Invalid vote.\n");
                return 3;
            }
        }

        record_preferences(ranks);

        printf("\n");
    }

    add_pairs();
    sort_pairs();
    lock_pairs();
    print_winner();
    return 0;
}

// Update ranks given a new vote
bool vote(int rank, string name, int ranks[])
{
    // TODO
    for (int canname = 0; canname < candidate_count; canname++)
    {
        if (strcmp(candidates[canname], name) == 0)
        {
            ranks[rank] = canname;
            return true;
        }
    }
    return false;
}

// Update preferences given one voter's ranks
void record_preferences(int ranks[])
{
    for (int rank = 0; rank < candidate_count; rank++)
    {
        for (int can = rank + 1; can < candidate_count; can++)
        {
            preferences[ranks[rank]][ranks[can]]++;
        }
    }
    return;
}

// Record pairs of candidates where one is preferred over the other
void add_pairs(void)
{
    pair_count = 0;
    for (int i = 0; i < candidate_count; i++)
    {
        for (int j = 0; j < candidate_count; j++)
        {
            if (preferences[i][j] > preferences[j][i])
            {
                pairs[pair_count].winner = i;
                pairs[pair_count].loser = j;
                pair_count++;
            }
        }
    }
    return;
}

// Sort pairs in decreasing order by strength of victory
void sort_pairs(void)
{
    int largest_win_index;
    for (int i = 0; i < pair_count; i++)
    {
        largest_win_index = i;
        for (int j = i; j < pair_count; j++)
        {
            if (preferences[pairs[largest_win_index].winner][pairs[largest_win_index].loser] -
                preferences[pairs[largest_win_index].loser][pairs[largest_win_index].winner] < preferences[pairs[j].winner][pairs[j].loser] -
                preferences[pairs[j].loser][pairs[j].winner])
            {
                largest_win_index = j;
            }
        }
        if (largest_win_index != i)
        {
            pair temporary = pairs[i];
            pairs[i] = pairs[largest_win_index];
            pairs[largest_win_index] = temporary;
        }
    }
    return;
}

// Lock pairs into the candidate graph in order, without creating cycles
void lock_pairs(void)
{

    locked[pairs[0].winner][pairs[0].loser] = true;
    for (int i = 1; i < pair_count; i++)
    {
        locked[pairs[i].winner][pairs[i].loser] = true;

        if (check_loop(1, pairs[i].loser))
        {
            locked[pairs[i].winner][pairs[i].loser] = true;
        }
        else
        {
            locked[pairs[i].winner][pairs[i].loser] = false;
        }
    }
    return;
}

// Print the winner of the election
void print_winner(void)
{
    printf("%s\n", candidates[find_winner(0)]);
    return;
}

// Checks a pair to see if it creates a loop in the locked graph.
bool check_loop(int iteration, int loser_index)
{
    if (iteration >= candidate_count)
    {
        return false;
    }

    for (int m = 0; m < candidate_count; m++)
    {
        if (locked[loser_index][m] == true)
        {
            iteration++;
            return check_loop(iteration, m);
        }

    }
    return true;
}

// Finds the winner based on the locked graph.
int find_winner(int root)
{
    for (int i = 0; i < candidate_count; i++)
    {
        if (locked[i][root] == true)
        {
            return find_winner(i);
        }
    }
    return root;
}