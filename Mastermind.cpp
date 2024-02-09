#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <time.h>
#include <string.h>
#include <windows.h>

//colors!
#define RED		"\x1B[31m"
#define YELLOW	"\x1B[33m"
#define GREEN	"\x1B[32m"
#define MAGENTA "\x1B[35m"
#define CYAN	"\x1B[36m"
#define WHITE   "\x1B[37m"
#define BLUE	"\x1B[34m"
#define RESET   "\x1B[0m"


///////////////////////////////////////////////////
struct user{
	char id[5];
	char username[100];
	char password[100];
	char bestScore[30];
};

struct settings{
	int length;
	int hasDuplicateDigits;
	int showsLastGuesses;
};

struct log{
	char id[5];
	int score;
	char time[20];
};
///////////////////////////////////////////////////
long generateAnswer(int len, bool duplicate);
struct user readUser(char userInfo[]);
struct settings readSettings();
struct log readLog(char logLine[]);
void type(char *str, int time);
void signUp();
bool isUsernameInUsers(char *username);
int getLastId();
void showSettings();
void editSettings();
void setSettings(struct settings settings);
void startGame(int id);
int login();
int checkUsernameAndPassword(char *user, char *pass);
int calculateTime(int answer, int length, bool duplicate);
char *checkAnswer(char ans[10], char guess[10]);
int len(int n);
int calculateScore(struct settings settings, int total, int counter);
void appendLog(int id, int score, int startTime);
void showLeaderboard();
void getNameWithId(char id[]);
void swapLogs(struct log *one, struct log *two);
void updateBestScore(char id[]);
int getBestScore(char id[]);
void adminPanel();
void deleteUser();
void resetSettings();
///////////////////////////////////////////////////

int main(){
	
	type("Hi!\nWelcome to ", 1);
	type(RED "M" YELLOW "A" GREEN "S" MAGENTA "T" CYAN "E" RED "R "
			YELLOW "M" GREEN "I" MAGENTA "N" CYAN "D!\n\n" RESET, 0);
			
	while(true){
		int i;
		type(GREEN "0." RESET " ReadMe (How to play!)\n" 
				GREEN "1." RESET " Signup\n" 
				GREEN "2." RESET " Settings\n" 
				GREEN "3." RESET " Start the game\n" 
				GREEN "4." RESET " Leaderboard\n" 
				GREEN "5." RESET " Exit\n-> ", 1);
		scanf("%d", &i);
		fflush(stdin);
		switch(i){
			case 0:
				ShellExecute(NULL, "open", "ReadMe.pdf", NULL, NULL, SW_SHOWNORMAL);
				system("cls");
    			break;
    		case 1:
    			signUp();
    			break;
			case 2:
    			showSettings();
    			break;
			case 3:{
				int id = login();
				if(id == 1)
					adminPanel();
    			else
					startGame(id);
    			break;
			}
    		case 4:
    			showLeaderboard();
    			break;
			case 5:
				system("cls");
				type("See you next time!\n", 1);
				return 0;
				break;
			
			default:
				system("cls");
				type(RED"The input is wrong, please try again.\n" RESET, 1);
		}
		
	}
	return 0;
}

long generateAnswer(int len, bool duplicate){
	srand(time(0));
	int ans = 0;
	
	if(duplicate)
		for(int i = 0; i < len; i++){
			int r;
			if(i != 0)
				r = rand() % 10;
			else
				r = (rand() % 9) + 1;
				
			ans = (ans * 10) + r;
		}
	else{
		int nums[10] = {0};
		for(int i = 0; i < len; i++){
			int r;
			if(i != 0){
				r = rand() % 10;
				while(nums[r] == 1)
					r = rand() % 10;	
			}
			else{
				r = (rand() % 9) + 1;
			}
			nums[r]++;
			ans = (ans * 10) + r;
		}
	}
	//printf("\nans = %d\n", ans);
	return ans;
}


struct user readUser(char userInfo[]){
	// ID,Username,Password,Best Score
	struct user temp;
	char u[200];
	strcpy(u, userInfo);
	
	char *s = strtok(u,",");
    strcpy(temp.id, s);
   	s = strtok (NULL, ",");
  	strcpy(temp.username, s);
   	s = strtok (NULL, ",");
  	strcpy(temp.password, s);
   	s = strtok (NULL, ",");
  	strcpy(temp.bestScore, s);

	return temp;
}

struct log readLog(char logLine[]){
	// ID, Score, Time
	struct log temp;
	
	char *s = strtok(logLine,",");
    strcpy(temp.id, s);
   	s = strtok (NULL, ",");
  	temp.score = atoi(s);
	s = strtok (NULL, ",");
  	strcpy(temp.time, s);

	return temp;
}

struct settings readSettings(){
	FILE *f = fopen("settings.txt", "r");
	char settings[100];
	fgets(settings, sizeof(settings), f);
	
	struct settings temp;
	char *s = strtok(settings,",");
    temp.length = atoi(s);
   	s = strtok (NULL, ",");
	temp.hasDuplicateDigits = atoi(s);
   	s = strtok (NULL, ",");
    temp.showsLastGuesses = atoi(s);
    
    fclose(f);
	return temp;
}

void type(char *str, int time){
	// time -> 0: very fast , 1: fast, 2: normal
	
	for(int i = 0; str[i]; i++){
		printf("%c", str[i]);
		if(time == 2)
			Sleep(60);
		else if(time == 1)
			Sleep(5);
		else
			Sleep(2);
	}
	
}

void signUp(){
	char username[100], password[100];
	system("cls");
	fflush(stdin);
	type("Username: ", 1);
	fgets(username, sizeof(username), stdin);
	username[strlen(username) - 1] = '\0';
	while(isUsernameInUsers(username)){
		type("Sorry. This username is already taken. Choose another one\n", 1);
		type("Username: ", 1);
		fgets(username, sizeof(username), stdin);
		username[strlen(username) - 1] = '\0';
	}
	
	type("Password: ", 1);
	fgets(password, sizeof(password), stdin);
	password[strlen(password) - 1] = '\0';

	char user[202];
	int id = getLastId();
	char newId[5];
	sprintf(newId, "%d", ++id);
	strcpy(user, newId);
	strcat(user, ",");
	strcat(user, username);
	strcat(user, ",");
	strcat(user, password);
	strcat(user, ",0");
	FILE *users = fopen("users.txt", "a");
	fputs(user, users);
	
	system("cls");
	type("Signed up Succesfully!\n", 1);
	fclose(users);
}


bool isUsernameInUsers(char *username){
	FILE *users = fopen("users.txt", "r");
	char user[250];
	fgets(user, sizeof(user), users);
	if(strcmp(username,"Admin") == 0)
		return true;
	while(fgets(user, sizeof(user), users)){	
		struct user u = readUser(user);
		char temp[100];
		strcpy(temp, u.username);
		if(strcmp(temp, username) == 0){
			fclose(users);
			return true;
		}
	}
	fclose(users);
	return false;	
}


int getLastId(){
	FILE *users = fopen("users.txt", "r");
	char user[250];
	while(fgets(user, sizeof(user), users));
	struct user u = readUser(user);
	fclose(users);
	return atoi(u.id);
}


void showSettings(){
	// length,hasDuplicateDigits,showsLastGuesses
	system("cls");
	struct settings sets = readSettings();
	type("Current Settings: \nLength of the number: ",1);
	char length[5];
	sprintf(length, "%d", sets.length);
	type(length, 1);
	type("\nHas duplicate digits: ",1);
	if(sets.hasDuplicateDigits)
		type(GREEN "Yes\n" RESET, 1);
	else
		type(YELLOW "No\n" RESET, 1);
	type("Shows last gueeses: ",1);
	if(sets.showsLastGuesses)
		type(GREEN "Yes\n" RESET, 1);
	else
		type(YELLOW "No\n" RESET, 1);
	
	while(true){
		
		type("\n" GREEN "1. " RESET "Edit the settings\n" GREEN "2. " RESET "Return to the main menu\n-> ", 1);
		int i;
		scanf("%d", &i);
		
		switch(i){
			case 1:
				editSettings();
				return; 
				break;
				
			case 2:
				system("cls");
				return;
	
			default:
				system("cls");
				type(RED"The input is wrong, please try again.\n" RESET, 1);
				
		}
	}
}

void editSettings(){
	struct settings newSets;
	int n;
	fflush(stdin);
	bool b = false;
	type("----------------------\nLength: ",1);
		scanf("%d", &newSets.length);
		if(newSets.length >=4 && newSets.length <= 9)
				b = true;
	while(!b){
		type( YELLOW "ERROR: " RESET "Length is between 4 and 9. Try again\nLength: ",1);
		scanf("%d", &newSets.length);
		if(newSets.length >=4 && newSets.length <= 9)
				b = true;
	}
	b = false;
	type("Has duplicate digits? \n\t" GREEN "1. Yes\n\t" YELLOW "2. No\n" RESET "->" ,1);
	scanf("%d", &n);
	if(n == 1){
		newSets.hasDuplicateDigits = 1;
		b = true;
	} else if(n == 2){
		newSets.hasDuplicateDigits = 0;
		b = true;
	}
	while(!b){
		type(YELLOW"ERROR: " RESET "The input is wrong. Try again\n",1);
		type("Has duplicate digits? \n\t" GREEN "1. Yes\n\t" YELLOW "2. No\n" RESET "->" ,1);
		scanf("%d", &n);
		if(n == 1){
			newSets.hasDuplicateDigits = 1;
			b = true;
		} else if(n == 2){
			newSets.hasDuplicateDigits = 0;
			b = true;
		}
	}
	b = false;
	type("Shows last guesses? \n\t" GREEN "1. Yes\n\t" YELLOW "2. No\n" RESET "->" ,1);
	scanf("%d", &n);
	if(n == 1){
		newSets.showsLastGuesses = 1;
		b = true;
	} else if(n == 2){
		newSets.showsLastGuesses = 0;
		b = true;
	}
	while(!b){
		type(YELLOW"ERROR: " RESET "The input is wrong. Try again\n",1);
		type("Shows last guesses? \n\t" GREEN "1. Yes\n\t" YELLOW "2. No\n" RESET "->" ,1);
		scanf("%d", &n);
		if(n == 1){
			newSets.showsLastGuesses = 1;
			b = true;
		} else if(n == 2){
			newSets.showsLastGuesses = 0;
			b = true;
		}
	}
	setSettings(newSets);
}

void setSettings(struct settings settings){
	FILE *f = fopen("settings.txt", "w");
	
	char newSets[20], temp[10];
	sprintf(temp, "%d", settings.length);
	strcpy(newSets, temp);
	strcat(newSets, ",");
	sprintf(temp, "%d", settings.hasDuplicateDigits);
	strcat(newSets, temp);
	strcat(newSets, ",");
	sprintf(temp, "%d", settings.showsLastGuesses);
	strcat(newSets, temp);
	
	fputs(newSets, f);
	
	system("cls");
	type(GREEN"\nDone!\n" RESET,1);
	fclose(f);
}


int login(){
	char username[100], password[100];
	system("cls");
	fflush(stdin);
	
	type("Username: ", 1);
	fgets(username, sizeof(username), stdin);
	username[strlen(username) - 1] = '\0';
	
	type("Password: ", 1);
	fgets(password, sizeof(password), stdin);
	password[strlen(password) - 1] = '\0';
	
	
	int id = checkUsernameAndPassword(username,password);
	while(id == -1){
		type(YELLOW "EROOR: " RESET "Username or password is wrong! Try again\n", 1);
		type("Username: ", 1);
		fgets(username, sizeof(username), stdin);
		username[strlen(username) - 1] = '\0';
		type("Password: ", 1);
		fgets(password, sizeof(password), stdin);
		password[strlen(password) - 1] = '\0';
		id = checkUsernameAndPassword(username,password);
	}
	system("cls");
	type("Logged in successfully\n", 1);
	return id;
	
	
}
int checkUsernameAndPassword(char *user, char *pass){
	FILE *users = fopen("users.txt", "r");
	char temp[250];
	while(fgets(temp, sizeof(temp), users)){	
		struct user u = readUser(temp);
		//char temp[100];
		//strcpy(temp, u.username);
		if(strcmp(u.username, user) == 0 && strcmp(u.password, pass) == 0){
			fclose(users);
			return atoi(u.id);
		}
	}
	fclose(users);
	return -1;	
}

void startGame(int id){
	struct settings settings = readSettings();
	
	int answer = generateAnswer(settings.length, settings.hasDuplicateDigits);
	int totalTime = calculateTime(answer, settings.length, settings.hasDuplicateDigits);
	type("Your total time: " GREEN ,1);
	char temp[10],temp2[10];
	sprintf(temp, "%d", totalTime);
	type(temp, 1);
	type("s\n" RESET, 1);
	int startTime = time(0);
	type("Game started!\n",1);
	//int guesses[100], index = 0;
	int guess, counter = 0;
	while(time(0) - startTime < totalTime){
		if(counter == 12)
			break;
			
		type(CYAN "Guess -> " RESET, 0);
		scanf("%d", &guess);
		counter++;
		
		if(len(guess) != len(answer)){
			type(YELLOW "Error: " RESET "The length should be ",1);
			sprintf(temp, "%d", settings.length);
			type(temp, 1);
			type("\n", 1);
		} 
		else if(answer == guess){
				system("cls");
				type( GREEN "YES! Congratulations" RESET "\nTotal time: ", 1);
				int total = time(0) - startTime;
				printf("%d s\n", total);
				int score = calculateScore(settings, total, counter);
				appendLog(id, score, startTime);
				sprintf(temp, "%d", id);
				updateBestScore(temp);
				return;
		}
		else{
			sprintf(temp, "%d", answer);
			sprintf(temp2, "%d", guess);
			
			char output[10];
			strcpy(output, checkAnswer(temp, temp2));
			
			if(!settings.showsLastGuesses)
				system("cls");
			
			for(int i = 0; output[i]; i++){
				char c = output[i], c2 = temp2[i];
				switch(c){
					case '1':
						printf(GREEN "%c" RESET, c2);
						break;
					case '2':
						printf(YELLOW "%c" RESET, c2);
						break;
					case '3':
						printf("_");
						break;
					default:
						printf("ERROR");
				}
			}
			printf("\n");
		}
			
		
		
	}
	system("cls");
	type(RED "Oh no! " RESET"You sadly did not find the answer. try better next time!\n\n", 1);
}

int calculateTime(int answer, int length, bool duplicate){
	if(!duplicate)
		return 10 * length;
	int time = 0;
	int nums[10] = {0};
	char ans[10];
	sprintf(ans, "%d", answer);
	for(int i = 0; ans[i]; i++)
		nums[ans[i] - 48]++;
	
	
	
	for(int i = 0; i < 10; i++){
		if(nums[i] > 1)
			time += 7 * nums[i];
		else
			time += 10 * nums[i];
	}
	return time;
}


int len(int n){
	
	int i;
	for( i = 0; n > 0; i++)
		n /= 10;

	//printf("%d\n", i);
	return i;
	
}

char *checkAnswer(char ans[10], char guess[10]){
	/*
		1: correct (green)
		2: correct but wrong place (yellow)
		3: wrong ( _ )
	*/
	char *output = (char *) malloc(10 * sizeof(char));
	int i;
	for(i = 0; ans[i]; i++){
		if(ans[i] == guess[i])
			output[i] = '1';
		else {
			output[i] = '3';
			for(int j = 0; ans[j]; j++)
				if(ans[j] == guess[i])
					output[i] = '2';
		}
	}
	output[i] = '\0';
	//printf("%s\n", output);
	
	return output;
}

int calculateScore(struct settings settings, int total, int counter){
	/* 
		for each digit -> 20 points
		having duplicate digits -> 10 points
		not showing the last guesses -> 10 points
		each time guessing -> -5 points
	*/
	int score = 0;
	score += 20 * settings.length;
	
	if(settings.hasDuplicateDigits)
		score += 10;
		
	if(!settings.showsLastGuesses)
		score += 10;
		
	score -= 5 * counter;
	
	return score;
}

void appendLog(int id, int score, int startTime){
	FILE *log = fopen("log.txt", "a");
	char data[100];
	sprintf(data, "%d,%d,%d\n", id, score, startTime);
	fputs(data, log);
	fclose(log);
}


void getNameWithId(char id[]){
	FILE *users = fopen("users.txt", "r");
	char line[200];
	fgets(line, sizeof(line), users);
	while(fgets(line, sizeof(line), users)){
		struct user u = readUser(line);
		if(strcmp(u.id, id) == 0){
			strcpy(id, u.username);
		}
	}
	fclose(users);
}

void swapLogs(struct log *one, struct log *two){
	struct log temp;
	
	strcpy(temp.id, one->id);
	temp.score = one->score;
	
	strcpy(one->id, two->id);
	one->score = two->score;
	
	strcpy(two->id, temp.id);
	two->score = temp.score;
}


void showLeaderboard(){
	system("cls");
	struct log logs[500];
	int index = 0;
	FILE *log = fopen("log.txt", "r");
	char line[200];
	while(fgets(line, sizeof(line), log))
		logs[index++] = readLog(line);
	
	for(int i = 0; i < index; i++)
		for(int j = i; j < index; j++)
			if(logs[i].score < logs[j].score)
				swapLogs(&logs[i], &logs[j]);
	
	int indexes[500] = {0};
	printf(CYAN "-----------------------------\n" RESET);
	for(int i = 0; i < index; i++){
		if(indexes[atoi(logs[i].id)] == 0){
			char s[100];
			strcpy(s, logs[i].id);
			getNameWithId(s);
			printf(CYAN "| " RESET "%-15s" CYAN "|" RESET "\t%-3d " CYAN "|\n" RESET, s, logs[i].score);
			indexes[atoi(logs[i].id)]++;
		}
	}
	printf(CYAN "-----------------------------\n\n" RESET);
	fclose(log);
}

int getBestScore(char id[]){
	struct log logs[500];
	int index = 0;
	FILE *log = fopen("log.txt", "r");
	char line[200];
	while(fgets(line, sizeof(line), log))
		logs[index++] = readLog(line);
	
	for(int i = 0; i < index; i++)
		for(int j = i; j < index; j++)
			if(logs[i].score < logs[j].score)
				swapLogs(&logs[i], &logs[j]);
		
	for(int i = 0; i < index; i++){
		//printf("%s\t%d\n",logs[i].id, logs[i].score);
		if(strcmp(id, logs[i].id) == 0){
			fclose(log);
			return logs[i].score;
		}
	}
	
	return 0;
}

void updateBestScore(char id[]){
	FILE *users = fopen("users.txt", "r");
	int index = 0;
	char line[200][200];
	fgets(line[index++], sizeof(line[index]), users);
	while(fgets(line[index], sizeof(line[index]), users)){
		struct user temp = readUser(line[index]);
		//printf("line-> %s\n", line[index]);
		if(strcmp(temp.id, id) == 0)
			sprintf(line[index], "%s,%s,%s,%d\n", temp.id, temp.username, temp.password, getBestScore(temp.id));
		//printf("%d -> %s\n",index, line[index]);
		index++;
	}
	fclose(users);
	FILE *u = fopen("users.txt", "w");
	
	for(int i = 0; i < index; i++){
		//printf("line-> %s\n", line[i]);
		fputs(line[i], u);
	}
	fclose(u);
}

void adminPanel(){
	type(MAGENTA "\nWelcome to Admin panel!\n" RESET, 1);
	while(true){
	
		type(GREEN "1. " MAGENTA "Delete a user \n" 
			 GREEN "2. " MAGENTA "Reset the settings\n" 
			 GREEN "3. " MAGENTA "Return to main menu\n" RESET "-> ", 1);
		int i;
		scanf("%d", &i);	
		
		switch(i){
			case 1:
				deleteUser();
				break;
			case 2:
				resetSettings();
				break;
			case 3:
				system("cls");
				return;
			default:
				system("cls");
				type(RED"The input is wrong, please try again.\n" RESET, 1);
		}
	}
}


void deleteUser(){
	fflush(stdin);
	char username[100];
	type("Enter the username to delete: ",1);
	fgets(username, sizeof(username), stdin);
	username[strlen(username) - 1] = '\0';
	bool b = isUsernameInUsers(username);
	while(!b){
		type(YELLOW "Username does not exist! Try again\n" RESET "Enter the username to delete: ",1);
		fgets(username, sizeof(username), stdin);
		username[strlen(username) - 1] = '\0';
		b = isUsernameInUsers(username);
	}
	char id[5];
	FILE *users = fopen("users.txt", "r");
	int index = 0;
	char line[200][200];
	while(fgets(line[index++], sizeof(line[index]), users));
	fclose(users);
	FILE *u = fopen("users.txt", "w");
	
	for(int i = 0; i < index-1; i++){
		struct user temp = readUser(line[i]);
		if(strcmp(temp.username, username) != 0)
			fputs(line[i], u);
		else
			strcpy(id, temp.id);
	}
	fclose(u);
	
	type("User deleted from Users!\n",1);
	
	FILE *logs = fopen("log.txt", "r");
	index = 0;
	char logsLine[200][200];
	while(fgets(logsLine[index++], sizeof(logsLine[index]), logs));
	fclose(logs);
	
	FILE *l = fopen("log.txt", "w");
	
	for(int i = 0; i < index-1; i++){
		char tempLine[200];
		strcpy(tempLine, logsLine[i]);
		struct log temp = readLog(tempLine);
		if(strcmp(temp.id, id) != 0)
			fputs(logsLine[i], l);
	}
	fclose(l);
	
	type("User deleted from Logs!\n",1);
	
}
void resetSettings(){
	FILE *settings = fopen("settings.txt", "w");
	char defaults[] = "6,0,0";
	fputs(defaults, settings);
	fclose(settings);
	system("cls");
	type("Done!\n",1);
}
