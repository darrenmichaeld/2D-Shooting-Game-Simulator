#include <iostream>
#include <cstring>
using namespace std;

const int MAX_ROWS = 20;
const int MAX_COLS = 30;
const int MAX_ACTION_TEXT_LENGTH = 10 + 1;    // array size for the action text, +1 for the NULL character
const int MAX_DIRECTION_TEXT_LENGTH = 10 + 1; // array size for the direction text, +1 for the NULL character
const int MAX_NUM_ROBOTS = 26;                // Note: Robots are named from A..Z
const char CHAR_EMPTY = '.';
const char CHAR_END_INPUT = '!';
const char DIRECTION_EAST = 'e';
const char DIRECTION_WEST = 'w';
const char DIRECTION_SOUTH = 's';
const char DIRECTION_NORTH = 'n';
const char DIRECTION_ERROR = '!';
const char DIRECTION_EAST_TEXT[] = "east";
const char DIRECTION_WEST_TEXT[] = "west";
const char DIRECTION_SOUTH_TEXT[] = "south";
const char DIRECTION_NORTH_TEXT[] = "north";
const char DIRECTION_ERROR_TEXT[] = "error";
const char ACTION_MOVE = 'm';
const char ACTION_HIT = 'h';
const char ACTION_SHOOT = 's';
const char ACTION_ERROR = '!';
const char ACTION_MOVE_TEXT[] = "moves";
const char ACTION_HIT_TEXT[] = "hits";
const char ACTION_SHOOT_TEXT[] = "shoots";
const int STATUS_ACTION_MOVE_NOT_IMPLMENTED = 0;
const int STATUS_ACTION_MOVE_SUCCESS = 1;
const int STATUS_ACTION_MOVE_OUTSIDE_BOUNDARY = 2;
const int STATUS_ACTION_MOVE_HIT_ANOTHER_ROBOT_ALONG_PATH = 3;

// Weapon: for the hit action
const int WEAPON_HIT_DAMAGE = 200;

// Weapon: for the shoot action
const int WEAPON_SHOOT_DAMAGE = 100;
const int WEAPON_SHOOT_RANGE = 5;

const int STATUS_ACTION_WEAPON_NOT_IMPLEMENTED = 0;
const int STATUS_ACTION_WEAPON_SUCCESS = 1;
const int STATUS_ACTION_WEAPON_FAIL = 2;

// Helper function: mapping the robotLetter to the correct array index
// e.g., 'A'=>0, 'B'=>1, ...
int robotLetterToArrayIndex(const char robotLetter)
{
    return int(robotLetter - 'A');
}

// Helper function: mapping array index to the robot letter
// e.g., 0=>'A', 1=>'B', ...
char arrayIndexToRobotLetter(const int index)
{
    return char(index + 'A');
}

// Helper function: convert an action text to a letter
// It is easier to compare characters than strings
char actionTextToLetter(const char actionText[MAX_ACTION_TEXT_LENGTH])
{
    // In this game, only 3 possible values
    if (strcmp(actionText, ACTION_MOVE_TEXT) == 0)
    {
        return ACTION_MOVE;
    }
    else if (strcmp(actionText, ACTION_HIT_TEXT) == 0)
    {
        return ACTION_HIT;
    }
    else if (strcmp(actionText, ACTION_SHOOT_TEXT) == 0)
    {
        return ACTION_SHOOT;
    }
    return ACTION_ERROR;
}

// Helper function: convert a direction text to a letter
// It is easier to compare characters than strings
char directionTextToLetter(const char directionText[MAX_DIRECTION_TEXT_LENGTH])
{
    // In this game, only 4 possible directions
    if (strcmp(directionText, DIRECTION_EAST_TEXT) == 0)
    {
        return DIRECTION_EAST;
    }
    else if (strcmp(directionText, DIRECTION_SOUTH_TEXT) == 0)
    {
        return DIRECTION_SOUTH;
    }
    else if (strcmp(directionText, DIRECTION_WEST_TEXT) == 0)
    {
        return DIRECTION_WEST;
    }
    else if (strcmp(directionText, DIRECTION_NORTH_TEXT) == 0)
    {
        return DIRECTION_NORTH;
    }
    return DIRECTION_ERROR;
}

// Helper function: convert a direction letter to text
const char *letterToDirectionText(char direction)
{
    if (direction == DIRECTION_EAST)
        return DIRECTION_EAST_TEXT;
    else if (direction == DIRECTION_SOUTH)
        return DIRECTION_SOUTH_TEXT;
    else if (direction == DIRECTION_WEST)
        return DIRECTION_WEST_TEXT;
    else if (direction == DIRECTION_NORTH)
        return DIRECTION_NORTH_TEXT;
    return DIRECTION_ERROR_TEXT;
}

// Helper function: Initialize the 2D array with an empty character
void initializeMap(char map[MAX_ROWS][MAX_COLS], const int numRows, const int numCols)
{
    int r, c;
    for (r = 0; r < numRows; r++)
        for (c = 0; c < numCols; c++)
            map[r][c] = CHAR_EMPTY;
}

// Helper function: Initialize the 1D array with zeros
void initializeHealthPoints(int healthPoints[MAX_NUM_ROBOTS])
{
    int i;
    for (i = 0; i < MAX_NUM_ROBOTS; i++)
        healthPoints[i] = 0;
}

// Helper function: Read the map information
void readInputMap(char map[MAX_ROWS][MAX_COLS], int &mapRows, int &mapCols)
{
    int r, c;
    char line[MAX_COLS + 1]; // character array to store each line

    // Read the game map
    cin >> mapRows >> mapCols;
    initializeMap(map, mapRows, mapCols);

    cin.ignore(); // ignore the end line character in this line
    for (r = 0; r < mapRows; r++)
    {
        cin.getline(line, MAX_COLS + 1);
        for (c = 0; c < mapCols; c++)
            map[r][c] = line[c];
    }
}

// Helper function: Read the robot information
void readInputHealthPoints(int healthPoints[MAX_NUM_ROBOTS])
{
    int numRobots, hp, i;
    char robotLetter;

    initializeHealthPoints(healthPoints);
    // Read robot letters and health points
    cin >> numRobots;
    for (i = 0; i < numRobots; i++)
    {
        cin >> robotLetter >> hp;
        // cout << robotLetter << hp << endl;
        healthPoints[robotLetterToArrayIndex(robotLetter)] = hp;
    }
}

// Helper function: Display the game map
void displayMap(const char map[MAX_ROWS][MAX_COLS], const int mapRows, const int mapCols)
{
    int r, c;
    for (r = 0; r < mapRows; r++)
    {
        for (c = 0; c < mapCols; c++)
            cout << map[r][c] << " ";
        cout << endl;
    }
}

// Helper function: Display the health point information
void displayHealthPoints(const int healthPoints[MAX_NUM_ROBOTS])
{
    int i, hp;
    for (i = 0; i < MAX_NUM_ROBOTS; i++)
    {
        hp = healthPoints[i];
        if (hp > 0)
        {
            cout << "Robot " << arrayIndexToRobotLetter(i) << " HP=" << hp << endl;
        }
    }
}

int updateHealthPointsForHitAction(int healthPoints[MAX_NUM_ROBOTS],
                                   char map[MAX_ROWS][MAX_COLS],
                                   const int mapRows, const int mapCols,
                                   const char robotLetter,
                                   const char directionLetter,
                                   char &targetRobotLetter,
                                   int &targetOriginalHealthPoint,
                                   int &targetUpdatedHealthPoint)
{

    int xloc = 0;
    int yloc = 0;
    int stop = 0;
    for(int i = 0; i < mapRows; i++)
    {
        for(int j = 0; j < mapCols; j++)
        {
            if(map[i][j] == robotLetter && stop == 0)
            {
                xloc = i;
                yloc = j;
                stop += 1;
            }
        }
    }

    if(directionLetter == DIRECTION_SOUTH){
        if(xloc+1 < mapRows)
        {
            if(map[xloc+1][yloc] != CHAR_EMPTY)
            {
                targetRobotLetter = map[xloc+1][yloc];
                targetOriginalHealthPoint = healthPoints[robotLetterToArrayIndex(map[xloc+1][yloc])];
                targetUpdatedHealthPoint = healthPoints[robotLetterToArrayIndex(map[xloc+1][yloc])] - WEAPON_HIT_DAMAGE;
                healthPoints[robotLetterToArrayIndex(map[xloc+1][yloc])] -= WEAPON_HIT_DAMAGE;
                if(targetUpdatedHealthPoint <= 0)
                {
                    targetUpdatedHealthPoint = 0;
                    healthPoints[robotLetterToArrayIndex(map[xloc+1][yloc])] -= WEAPON_HIT_DAMAGE;
                    map[xloc+1][yloc]='.';
                }

                return STATUS_ACTION_WEAPON_SUCCESS;
            }
            else
            {
                return STATUS_ACTION_WEAPON_FAIL;
            }

        }
        else
        {
            return STATUS_ACTION_WEAPON_FAIL;
        }
    }
    else if(directionLetter == DIRECTION_NORTH){
        if(xloc-1 >= 0)
        {
            if(map[xloc-1][yloc] != CHAR_EMPTY)
            {
                targetRobotLetter = map[xloc-1][yloc];
                targetOriginalHealthPoint = healthPoints[robotLetterToArrayIndex(map[xloc-1][yloc])];
                targetUpdatedHealthPoint = healthPoints[robotLetterToArrayIndex(map[xloc-1][yloc])] - WEAPON_HIT_DAMAGE;
                healthPoints[robotLetterToArrayIndex(map[xloc-1][yloc])] -= WEAPON_HIT_DAMAGE;
                if(targetUpdatedHealthPoint <= 0)
                {
                    targetUpdatedHealthPoint = 0;
                    healthPoints[robotLetterToArrayIndex(map[xloc-1][yloc])] = 0;
                    map[xloc-1][yloc]='.';
                }
                return STATUS_ACTION_WEAPON_SUCCESS;
            }
            else
            {
                return STATUS_ACTION_WEAPON_FAIL;
            }

        }
        else
        {
            return STATUS_ACTION_WEAPON_FAIL;
        }
    }
    else if(directionLetter == DIRECTION_EAST){
        if(yloc+1 < mapCols)
        {
            if(map[xloc][yloc+1] != CHAR_EMPTY)
            {
                targetRobotLetter = map[xloc][yloc+1];
                targetOriginalHealthPoint = healthPoints[robotLetterToArrayIndex(map[xloc][yloc+1])];
                targetUpdatedHealthPoint = healthPoints[robotLetterToArrayIndex(map[xloc][yloc+1])] - WEAPON_HIT_DAMAGE;
                healthPoints[robotLetterToArrayIndex(map[xloc][yloc+1])] -= WEAPON_HIT_DAMAGE;
                if(targetUpdatedHealthPoint <= 0)
                {
                    targetUpdatedHealthPoint = 0;
                    healthPoints[robotLetterToArrayIndex(map[xloc][yloc+1])] = 0; 
                    map[xloc][yloc+1]='.'; 
                }
                return STATUS_ACTION_WEAPON_SUCCESS;
            }
            else
            {
                return STATUS_ACTION_WEAPON_FAIL;
            }

        }
        else
        {
            return STATUS_ACTION_WEAPON_FAIL;
        }
    }
    else if(directionLetter == DIRECTION_WEST){
        if(yloc-1 <= 0)
        {
            if(map[xloc][yloc-1] != CHAR_EMPTY)
            {
                targetRobotLetter = map[xloc][yloc-1];
                targetOriginalHealthPoint = healthPoints[robotLetterToArrayIndex(map[xloc][yloc-1])];
                targetUpdatedHealthPoint = healthPoints[robotLetterToArrayIndex(map[xloc][yloc-1])] - WEAPON_HIT_DAMAGE;
                healthPoints[robotLetterToArrayIndex(map[xloc][yloc-1])] -= WEAPON_HIT_DAMAGE;
                
                if(targetUpdatedHealthPoint <= 0)
                {
                    targetUpdatedHealthPoint = 0;
                    healthPoints[robotLetterToArrayIndex(map[xloc][yloc-1])] = 0;
                    map[xloc][yloc-1]='.';
                }
                return STATUS_ACTION_WEAPON_SUCCESS;
            }
            else
            {
                return STATUS_ACTION_WEAPON_FAIL;
            }

        }
        else
        {
            return STATUS_ACTION_WEAPON_FAIL;
        }
    }
    else
    {
        return STATUS_ACTION_WEAPON_FAIL;
    }

    return STATUS_ACTION_WEAPON_NOT_IMPLEMENTED;
}

int updateHealthPointsForShootAction(int healthPoints[MAX_NUM_ROBOTS],
                                     char map[MAX_ROWS][MAX_COLS],
                                     const int mapRows, const int mapCols,
                                     const char robotLetter,
                                     const char directionLetter,
                                     char &targetRobotLetter,
                                     int &targetOriginalHealthPoint,
                                     int &targetUpdatedHealthPoint)
{
    int xloc = 0;
    int yloc = 0;
    int stop = 0;
    for(int i = 0; i < mapRows; i++)
    {
        for(int j = 0; j < mapCols; j++)
        {
            if(map[i][j] == robotLetter && stop == 0)
            {
                xloc = i;
                yloc = j;
                stop += 1;
            }
        }
    }
    if(directionLetter == DIRECTION_SOUTH){
        if(xloc+1 < mapRows)
        {
            if(xloc+5 < mapRows)
            {
                int maxshoot = 0;
                for(int range = 1; range <= WEAPON_SHOOT_RANGE; range++)
                {
                    if(map[xloc+range][yloc]!= CHAR_EMPTY && maxshoot == 0)
                    {
                        
                        targetRobotLetter = map[xloc+range][yloc];
                        targetOriginalHealthPoint = healthPoints[robotLetterToArrayIndex(map[xloc+range][yloc])];
                        targetUpdatedHealthPoint = healthPoints[robotLetterToArrayIndex(map[xloc+range][yloc])] - WEAPON_SHOOT_DAMAGE;
                        healthPoints[robotLetterToArrayIndex(map[xloc+range][yloc])] -= WEAPON_SHOOT_DAMAGE;

                        if(targetUpdatedHealthPoint <= 0)
                        {
                            targetUpdatedHealthPoint = 0;
                            healthPoints[robotLetterToArrayIndex(map[xloc+range][yloc])] = 0;
                            map[xloc+range][yloc]='.';
                        }
                        maxshoot += 1;
                        return STATUS_ACTION_WEAPON_SUCCESS;
                                
                    }
                }
                return STATUS_ACTION_WEAPON_FAIL;
                
            }
            else if(xloc+5 >= mapRows)
            {
                int maxshoot = 0;
                for(int range = 1; range < mapRows-xloc; range++)
                {
                    if(map[xloc+range][yloc]!= CHAR_EMPTY && maxshoot == 0)
                    {
                        targetOriginalHealthPoint = healthPoints[robotLetterToArrayIndex(map[xloc+range][yloc])];
                        targetUpdatedHealthPoint = healthPoints[robotLetterToArrayIndex(map[xloc+range][yloc])] -= WEAPON_SHOOT_DAMAGE;
                        targetRobotLetter = map[xloc+range][yloc];
                        
                        if(targetUpdatedHealthPoint <= 0)
                        {
                            targetUpdatedHealthPoint = 0;
                            healthPoints[robotLetterToArrayIndex(map[xloc+range][yloc])] = 0;
                            map[xloc+range][yloc]='.';
                        }
                        return STATUS_ACTION_WEAPON_SUCCESS;
                    }
                    
                }
                return STATUS_ACTION_WEAPON_FAIL;
            }
            else
            {
                return STATUS_ACTION_WEAPON_FAIL;
            }
        }
        else
        {
            return STATUS_ACTION_WEAPON_FAIL;
        }
    }
    else if(directionLetter == DIRECTION_NORTH){
        if(xloc-1 >= 0)
        {
            if(xloc-5 >= 0)
            {
                int maxshoot = 0;
                for(int range = 1; range <= WEAPON_SHOOT_RANGE; range++)
                {
                    if(map[xloc-range][yloc]!= CHAR_EMPTY && maxshoot == 0)
                    {
                        
                        targetRobotLetter = map[xloc-range][yloc];
                        targetOriginalHealthPoint = healthPoints[robotLetterToArrayIndex(map[xloc-range][yloc])];
                        targetUpdatedHealthPoint = healthPoints[robotLetterToArrayIndex(map[xloc-range][yloc])] - WEAPON_SHOOT_DAMAGE;
                        healthPoints[robotLetterToArrayIndex(map[xloc-range][yloc])] -= WEAPON_SHOOT_DAMAGE;

                        if(targetUpdatedHealthPoint <= 0)
                        {
                            targetUpdatedHealthPoint = 0;
                            healthPoints[robotLetterToArrayIndex(map[xloc-range][yloc])] = 0;
                            map[xloc-range][yloc]='.';
                        }
                        maxshoot += 1;
                        return STATUS_ACTION_WEAPON_SUCCESS;
                                
                    }
                     
                }
                return STATUS_ACTION_WEAPON_FAIL;
                
            }
            else if(xloc-5 < 0)
            {
                int maxshoot = 0;
                for(int range = xloc-1; range >= 0; range--)
                {
                    if(map[range][yloc]!= CHAR_EMPTY && maxshoot == 0)
                    {
                        targetOriginalHealthPoint = healthPoints[robotLetterToArrayIndex(map[range][yloc])];
                        targetUpdatedHealthPoint = healthPoints[robotLetterToArrayIndex(map[range][yloc])] -= WEAPON_SHOOT_DAMAGE;
                        targetRobotLetter = map[range][yloc];
                        
                        if(targetUpdatedHealthPoint <= 0)
                        {
                            targetUpdatedHealthPoint = 0;
                            healthPoints[robotLetterToArrayIndex(map[range][yloc])] = 0;
                            map[range][yloc]='.';
                        }
                        return STATUS_ACTION_WEAPON_SUCCESS;
                    }
                    
                }
                return STATUS_ACTION_WEAPON_FAIL;
                
            }
            else
            {
                return STATUS_ACTION_WEAPON_FAIL;
            }
        }
        else
        {
            return STATUS_ACTION_WEAPON_FAIL;
        }
    }
    else if(directionLetter == DIRECTION_EAST){
        if(yloc+1 < mapCols)
        {
            if(yloc+5 < mapCols)
            {
                int maxshoot = 0;
                for(int range = 1; range <= WEAPON_SHOOT_RANGE; range++)
                {
                    if(map[xloc][yloc+range]!= CHAR_EMPTY && maxshoot == 0)
                    {
                        
                        targetRobotLetter = map[xloc][yloc+range];
                        targetOriginalHealthPoint = healthPoints[robotLetterToArrayIndex(map[xloc][yloc+range])];
                        targetUpdatedHealthPoint = healthPoints[robotLetterToArrayIndex(map[xloc][yloc+range])] - WEAPON_SHOOT_DAMAGE;
                        healthPoints[robotLetterToArrayIndex(map[xloc][yloc+range])] -= WEAPON_SHOOT_DAMAGE;

                        if(targetUpdatedHealthPoint <= 0)
                        {
                            targetUpdatedHealthPoint = 0;
                            healthPoints[robotLetterToArrayIndex(map[xloc][yloc+range])] = 0;
                            map[xloc][yloc+range]='.';
                        }
                        maxshoot += 1;
                        return STATUS_ACTION_WEAPON_SUCCESS;           
                    }
                    
                }
                return STATUS_ACTION_WEAPON_FAIL;
               
            }
            else if(yloc+5 >= mapCols)
            {
                int maxshoot = 0;
                for(int range = 1; range < mapCols-yloc; range++)
                {
                    if(map[xloc][yloc+range]!= CHAR_EMPTY && maxshoot == 0)
                    {
                        targetOriginalHealthPoint = healthPoints[robotLetterToArrayIndex(map[xloc][yloc+range])];
                        targetUpdatedHealthPoint = healthPoints[robotLetterToArrayIndex(map[xloc][yloc+range])] -= WEAPON_SHOOT_DAMAGE;
                        targetRobotLetter = map[xloc][yloc+range];  
                        if(targetUpdatedHealthPoint <= 0)
                        {
                            targetUpdatedHealthPoint = 0;
                            healthPoints[robotLetterToArrayIndex(map[xloc][yloc+range])] = 0;
                            map[xloc][yloc+range]='.';
                        }
                        maxshoot += 1;
                        return STATUS_ACTION_WEAPON_SUCCESS;   
                    }
                    
                }
                return STATUS_ACTION_WEAPON_FAIL;
            }
        }
        else
        {
            return STATUS_ACTION_WEAPON_FAIL;
        }
    }
    else if(directionLetter == DIRECTION_WEST){
        if(yloc-1 >= 0)
        {
            if(yloc-5 >= 0)
            {
                int maxshoot = 0;
                for(int range = 1; range <= WEAPON_SHOOT_RANGE; range++)
                {
                    if(map[xloc][yloc-range]!= CHAR_EMPTY && maxshoot == 0)
                    {
                        
                        targetRobotLetter = map[xloc][yloc-range];
                        targetOriginalHealthPoint = healthPoints[robotLetterToArrayIndex(map[xloc][yloc-range])];
                        targetUpdatedHealthPoint = healthPoints[robotLetterToArrayIndex(map[xloc][yloc-range])] - WEAPON_SHOOT_DAMAGE;
                        healthPoints[robotLetterToArrayIndex(map[xloc][yloc-range])] -= WEAPON_SHOOT_DAMAGE;

                        if(targetUpdatedHealthPoint <= 0)
                        {
                            targetUpdatedHealthPoint = 0;
                            healthPoints[robotLetterToArrayIndex(map[xloc][yloc-range])] = 0;
                            map[xloc][yloc-range]='.';
                        }
                        maxshoot += 1;
                        return STATUS_ACTION_WEAPON_SUCCESS;     
                    }
                
                }
                return STATUS_ACTION_WEAPON_FAIL;
            }
            else if(yloc-5 < 0)
            {
                int maxshoot = 0;
                for(int range = yloc-1; range >= 0; range--)
                {
                    if(map[xloc][range]!= CHAR_EMPTY && maxshoot == 0)
                    {
                        targetOriginalHealthPoint = healthPoints[robotLetterToArrayIndex(map[xloc][range])];
                        targetUpdatedHealthPoint = healthPoints[robotLetterToArrayIndex(map[xloc][range])] -= WEAPON_SHOOT_DAMAGE;
                        targetRobotLetter = map[xloc][range];
                        
                        if(targetUpdatedHealthPoint <= 0)
                        {
                            targetUpdatedHealthPoint = 0;
                            healthPoints[robotLetterToArrayIndex(map[xloc][range])] = 0;
                            map[xloc][range]='.';
                        }
                        return STATUS_ACTION_WEAPON_SUCCESS;
                    }
                }
                return STATUS_ACTION_WEAPON_FAIL;
               
            }
            else
            {
                return STATUS_ACTION_WEAPON_FAIL;
            }
        }
        else
        {
            return STATUS_ACTION_WEAPON_FAIL;
        }
    }
    else
    {
        return STATUS_ACTION_WEAPON_FAIL;
    }

    return STATUS_ACTION_WEAPON_NOT_IMPLEMENTED;
}

int updateMapForMoveAction(char map[MAX_ROWS][MAX_COLS], const int mapRows, const int mapCols,
                           const char robotLetter, const char directionLetter, const int moveSteps)
{
    // remove this line to start your work
    for(int i = 0; i < mapRows; i++)
    {
        for(int j = 0; j < mapCols; j++)
        {
            if(map[i][j] == robotLetter)
            {
                int xloc = i;
                int yloc = j;
                int block = 0;
                if(directionLetter == DIRECTION_EAST)
                {
                    if(yloc+1 < mapCols)
                    {
                        int checkblock = 0;
                        for(int y = yloc+1; y <= moveSteps; y++)
                        {
                            if(map[xloc][y] != CHAR_EMPTY)
                            {
                                checkblock += 1;
                            } 
                        }
                        if(checkblock == 0)
                        {
                            if(yloc+moveSteps < mapCols)
                            {
                                map[xloc][yloc]= CHAR_EMPTY;
                                map[xloc][yloc+moveSteps] = robotLetter;
                                return STATUS_ACTION_MOVE_SUCCESS;
                            }
                            else
                            {
                                return STATUS_ACTION_MOVE_OUTSIDE_BOUNDARY;
                            }
                        }
                        else
                        {
                            return STATUS_ACTION_MOVE_HIT_ANOTHER_ROBOT_ALONG_PATH;
                        }
                    }
                    else
                    {
                        return STATUS_ACTION_MOVE_OUTSIDE_BOUNDARY;
                    }
                }
                else if(directionLetter == DIRECTION_WEST)
                {
                    if(yloc-1>= 0)
                    {
                        int checkblock = 0;
                        for(int y = yloc-1; y >= yloc-moveSteps; y--)
                        {
                           if(map[xloc][y] != CHAR_EMPTY)
                            {
                                checkblock += 1;
                            } 
                        }
                        if(checkblock == 0)
                        {   if(yloc-moveSteps >= 0)
                            {
                                map[xloc][yloc]= CHAR_EMPTY;
                                map[xloc][yloc-moveSteps] = robotLetter;
                                return STATUS_ACTION_MOVE_SUCCESS;
                            }
                            else
                            {
                                return STATUS_ACTION_MOVE_OUTSIDE_BOUNDARY;
                            }
                        }
                        else
                        {
                            return STATUS_ACTION_MOVE_HIT_ANOTHER_ROBOT_ALONG_PATH;
                        }
                    }
                    else
                    {
                        return STATUS_ACTION_MOVE_OUTSIDE_BOUNDARY;
                    }
                }
                else if(directionLetter == DIRECTION_SOUTH)
                {
                    if(xloc+1 < mapRows)
                    {
                        int checkblock = 0;
                        for(int x = xloc+1; x <= moveSteps; x++)
                        {
                           if(map[x][yloc] != CHAR_EMPTY)
                            {
                                checkblock += 1;
                            } 
                        }
                        if(checkblock == 0)
                        {
                            if(xloc+moveSteps < mapRows)
                            {
                                map[xloc][yloc]= CHAR_EMPTY;
                                map[xloc+moveSteps][yloc] = robotLetter;
                                return STATUS_ACTION_MOVE_SUCCESS;
                            }
                            else
                            {
                                return STATUS_ACTION_MOVE_OUTSIDE_BOUNDARY;
                            }
                        }
                        else
                        {
                            return STATUS_ACTION_MOVE_HIT_ANOTHER_ROBOT_ALONG_PATH;
                        }
                    }
                    else
                    {
                        return STATUS_ACTION_MOVE_OUTSIDE_BOUNDARY;
                    }
                        
                }
                else if(directionLetter == DIRECTION_NORTH)
                {
                
                    if(xloc-1 >= 0)
                    {
                        int checkblock = 0;
                        for(int x = xloc-1; x >= xloc-moveSteps; x--)
                        {
                           if(map[x][yloc] != CHAR_EMPTY)
                            {
                                checkblock += 1;
                            } 
                        }
                        if(checkblock == 0)
                        {
                            if(xloc-moveSteps >= 0)
                            {
                                map[xloc][yloc]= CHAR_EMPTY;
                                map[xloc-moveSteps][yloc] = robotLetter;
                                return STATUS_ACTION_MOVE_SUCCESS;
                            }
                            {
                                return STATUS_ACTION_MOVE_OUTSIDE_BOUNDARY;
                            }
                        }
                        else
                        {
                            return STATUS_ACTION_MOVE_HIT_ANOTHER_ROBOT_ALONG_PATH;
                        }
                    }
                    else
                    {
                        return STATUS_ACTION_MOVE_OUTSIDE_BOUNDARY;
                    }
                }
            }
        }
    }
    return STATUS_ACTION_MOVE_NOT_IMPLMENTED;
}

int main()
{
    char map[MAX_ROWS][MAX_COLS];
    int healthPoints[MAX_NUM_ROBOTS];
    int mapRows, mapCols, moveSteps;
    int statusMove, statusHit, statusShoot;
    char robotLetter, actionLetter, directionLetter;
    char actionText[MAX_ACTION_TEXT_LENGTH];
    char directionText[MAX_DIRECTION_TEXT_LENGTH];
    char targetRobotLetter;
    int targetOriginalHealthPoint;
    int targetUpdatedHealthPoint;

    readInputHealthPoints(healthPoints);
    readInputMap(map, mapRows, mapCols);

    cout << "The initial game information:" << endl;
    displayHealthPoints(healthPoints);
    displayMap(map, mapRows, mapCols);

    while (true)
    {
        cin >> robotLetter;
        if (robotLetter == CHAR_END_INPUT)
            break; // End of the game

        // if the robot letter is not the end of the game
        // read the action text and the direction letter
        cin >> actionText >> directionText;
        actionLetter = actionTextToLetter(actionText);
        directionLetter = directionTextToLetter(directionText);
        if (actionLetter == ACTION_MOVE)
        {
            // need to read the steps if the action is ACTION_MOVE
            cin >> moveSteps;

            statusMove = updateMapForMoveAction(map, mapRows, mapCols, robotLetter, directionLetter, moveSteps);

            if (statusMove == STATUS_ACTION_MOVE_NOT_IMPLMENTED)
            {
                cout << "The move action is not implemented yet" << endl;
            }
            else if (statusMove == STATUS_ACTION_MOVE_SUCCESS)
            {
                cout << "Success: "
                     << "Robot " << robotLetter << " moves along the direction " << letterToDirectionText(directionLetter) << " by " << moveSteps << " step(s)" << endl;
                displayMap(map, mapRows, mapCols);
            }
            else if (statusMove == STATUS_ACTION_MOVE_OUTSIDE_BOUNDARY)
            {
                cout << "Fail: "
                     << "If robot " << robotLetter << " moves along the direction " << letterToDirectionText(directionLetter) << " by " << moveSteps << " step(s)"
                     << ", it will move outside a boundary, so the position remains unchanged" << endl;
                displayMap(map, mapRows, mapCols);
            }
            else if (statusMove == STATUS_ACTION_MOVE_HIT_ANOTHER_ROBOT_ALONG_PATH)
            {
                cout << "Fail: "
                     << "If robot " << robotLetter << " moves along the direction " << letterToDirectionText(directionLetter) << " by " << moveSteps << " step(s)"
                     << ", it will hit another robot along the path, so the position remains unchanged" << endl;
                displayMap(map, mapRows, mapCols);
            }
        } 
        else if (actionLetter == ACTION_HIT)
        {
            statusHit = updateHealthPointsForHitAction(healthPoints, map, mapRows, mapCols, robotLetter, directionLetter, targetRobotLetter, targetOriginalHealthPoint, targetUpdatedHealthPoint);
            if (statusHit == STATUS_ACTION_WEAPON_NOT_IMPLEMENTED)
            {
                cout << "The hit action is not implemented yet" << endl;
            }
            else if (statusHit == STATUS_ACTION_WEAPON_SUCCESS)
            {

                cout << "Success: "
                     << "Robot " << robotLetter << " hits " << targetRobotLetter << endl;
                cout << "Robot " << targetRobotLetter << " health point is reduced from " << targetOriginalHealthPoint << " to " << targetUpdatedHealthPoint << endl;
                if (targetUpdatedHealthPoint == 0)
                {
                    cout << "Robot " << targetRobotLetter << " is being destroyed by " << robotLetter << endl;
                    displayMap(map, mapRows, mapCols);
                }
                cout << "== Health points of alive robots ==" << endl;
                displayHealthPoints(healthPoints);
            }
            else if (statusHit == STATUS_ACTION_WEAPON_FAIL)
            {
                cout << "Fail: "
                     << "Robot " << robotLetter << " cannot hit any target" << endl;
                cout << "== Health points of alive robots ==" << endl;
                displayHealthPoints(healthPoints);
            }
        }
        else if (actionLetter == ACTION_SHOOT)
        {
            statusShoot = updateHealthPointsForShootAction(healthPoints, map, mapRows, mapCols, robotLetter, directionLetter, targetRobotLetter, targetOriginalHealthPoint, targetUpdatedHealthPoint);
            if (statusShoot == STATUS_ACTION_WEAPON_NOT_IMPLEMENTED)
            {
                cout << "The shoot action is not implemented yet" << endl;
            }
            else if (statusShoot == STATUS_ACTION_WEAPON_SUCCESS)
            {
                cout << "Success: "
                     << "Robot " << robotLetter << " shoots " << targetRobotLetter << endl;
                cout << "Robot " << targetRobotLetter << " health point is reduced from " << targetOriginalHealthPoint << " to " << targetUpdatedHealthPoint << endl;
                if (targetUpdatedHealthPoint == 0)
                {
                    cout << "Robot " << targetRobotLetter << " is being destroyed by " << robotLetter << endl;
                    displayMap(map, mapRows, mapCols);
                }
                cout << "== Health points of alive robots ==" << endl;
                displayHealthPoints(healthPoints);
            }
            else if (statusShoot == STATUS_ACTION_WEAPON_FAIL)
            {
                cout << "Fail: "
                     << "Robot " << robotLetter << " cannot shoot any target" << endl;
                cout << "== Health points of alive robots ==" << endl;
                displayHealthPoints(healthPoints);
            }
        }
    }
    cout << "=== Game Ended ===" << endl;
    return 0;
}