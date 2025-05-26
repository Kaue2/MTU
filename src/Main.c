/*projeto de implementar uma MTU em C */

#include<stdlib.h>
#include<stdio.h>
#include<string.h>
#include<stdbool.h>

#define MAX_TRANSICOES 100
#define MAX_FITA 1000



typedef struct {
	char crr_state [8];
	char read_symbol [8];
	char final_state [8];
	char writen_symbol [8];
	char direction;
}Transition;

Transition* initialize_transition(char* i_state, char* r_symbol, char* f_state, char* w_symbol, char* direction) {
	Transition* transition = (Transition*)malloc(sizeof(Transition));
	if (!transition) return NULL;

	strcpy(transition->crr_state, i_state);
	strcpy(transition->read_symbol, r_symbol);
	strcpy(transition->final_state, f_state);
	strcpy(transition->writen_symbol, w_symbol);
	transition->direction = *direction;

	return transition;
}

typedef struct {
	Transition* transitions;
	int index;
}MT;

MT* initialize_machine() {
	MT* machine = (MT*)malloc(sizeof(MT));
	if (!machine) return NULL;

	machine->transitions = (Transition*)malloc(20 * sizeof(Transition));
	machine->index = 0;

	return machine;
}

void change_state(char* str, int* pos, char flag);
void write_symbol(char* str, int* pos, Transition t, int middle);
void translate_str(char* destiny, char* src, int pos);

void add_transition(MT *machine, Transition *transition) {
	machine->transitions[machine->index++] = *transition;
}

void destruct_machine(MT* m) {
	free(m->transitions);
	free(m);
}

char* fetch(int *crr_pos, char *str, char flag) {

	char* aux = (char*)(malloc(8 * sizeof(char)));
	if (!aux) return NULL;
	aux[0] = '\0';

	char *stop_chars = (char*)(malloc(8 * sizeof(char)));
	if (!stop_chars) return NULL;
	stop_chars[0] = '\0';

	switch (flag)
	{
	case 'e': // states
		strcpy(stop_chars, "s");
		break;
	case 't': // terminal characters
		strcpy(stop_chars, "fde");
		break;
	case 'd': // direction
		strcpy(stop_chars, "f#") ;
		break;
	default:
		return NULL;
	}

	while (str[*crr_pos] != '\0' && !strchr(stop_chars, str[*crr_pos])) {
		char temp[2];
		temp[0] = str[*crr_pos];
		temp[1] = '\0';
		strcat(aux, temp);
		(*crr_pos)++;
	}
	free(stop_chars);
	return aux;
}

bool execute_machine(int *pos, char *str, char *state, MT *machine, int middle) {
	bool accept = false;
	char* symbol = (char*)malloc(10 * sizeof(char));
	if (!symbol) return false;
	symbol[0] = '\0';

	char aux[2];
	(*pos)++;

	while (str[*pos] != '\0') {
		aux[0] = str[*pos];
		aux[1] = '\0';

		strcat(symbol, aux);

		if (str[*pos + 1] == 's' || str[*pos + 1] == '\0') {
			for (int i = 0; i < machine->index; i++){
				if (strcmp(symbol, machine->transitions[i].read_symbol) == 0 
					&& strcmp(state, machine->transitions[i].crr_state) == 0) {
					printf("\nTransicao encontrada\n");
					printf("(%s, %s) -> (%s, %s, %c)\n", state, symbol, machine->transitions[i].final_state,
						machine->transitions[i].writen_symbol,
						machine->transitions[i].direction);
					strcpy(symbol, "");
					strcpy(state, machine->transitions[i].final_state);
					printf("Escrevendo. String:\n");
					write_symbol(str, pos, machine->transitions[i], middle);
					change_state(str, pos, machine->transitions[i].direction);
					break;
				}
			}
		}
		(*pos)++;
	}
	return accept;
}

void write_symbol(char *str, int *pos, Transition t, int middle) {
	int initial_index = *pos;
	char* post_symbol = (char*)malloc(100 * sizeof(char));
	char* pre_symbol = (char*)malloc(100 * sizeof(char));
	char* new_str = (char*)malloc(200 * sizeof(char));
	char* translated_str = (char*)malloc(200 * sizeof(char));
	char* aux1 = (char*)malloc(200 * sizeof(char));
	if (!post_symbol) return;
	if (!pre_symbol) return; 
	if (!new_str) return;
	if (!translated_str) return;
	if (!aux1) return;

	post_symbol[0] = '\0';
	pre_symbol[0] = '\0';
	new_str[0] = '\0';
	translated_str[0] = '\0';
	aux1[0] = '\0';

	char* stop_chars = "s#";

	for (initial_index; !strchr(stop_chars, str[initial_index]); initial_index--) {}

	for (int i = 0; i < initial_index; i++) {
		char* aux[2];
		aux[0] = str[i];
		aux[1] = '\0';

		strcat(pre_symbol, aux);
	}

	for (int i = *pos + 1; str[i] != '\0'; i++){
		char* aux[2];
		aux[0] = str[i];
		aux[1] = '\0';

		strcat(post_symbol, aux);
	}

	strcat(new_str, pre_symbol);
	strcat(new_str, t.writen_symbol);
	strcat(new_str, post_symbol);
	strcpy(str, new_str);

	for (int i = middle + 1; str[i] != '\0'; i++) {
		char aux[2]; 
		aux[0] = str[i];
		aux[1] = '\0';

		strcat(aux1, aux);
	}

	translate_str(translated_str, aux1, -1);
	printf("%s\n", translated_str);
}

void change_state(char* str, int* pos, char flag) {
	if (flag == 'd') {
		while (str[*pos] != '\0') {
			if (str[*pos + 1] == 's') {
				return;
			}
			(*pos)++;
		}
	}
	else {
		while (*pos >= 0 && str[*pos] != '#') {
			if (str[*pos] == 's') {
				break;
			}
			(*pos)--;
		}
		while (*pos >= 0 && str[*pos] != '#') {
			if (str[*pos] == 's') {
				(*pos)--;
				break;
			}
			(*pos)--;
		}
	}
}

char* convert_str(const char* str) {
	if (strcmp(str, "sc") == 0) {
		return "a";
	}
	if (strcmp(str, "scc") == 0) {
		return "b";
	}
	if (strcmp(str, "sccc") == 0) {
		return "c";
	}
}

void translate_str(char *destiny, char *src, int pos) {
	char* aux = (char*)malloc(100 * sizeof(char));
	if (!aux) return NULL;
	aux[0] = '\0';

	for (int i = pos + 1; src[i] != '\0'; i++) {
		char letter[2];
		letter[0] = src[i];
		letter[1] = '\0';
		strcat(aux, letter);
		if (src[i + 1] == 's' || src[i + 1] == '\0') {
			strcat(destiny, convert_str(aux));
			strcpy(aux, "");
		}
	}
	free(aux);
}

int main() {
	char string[200] = "fascfascccdfasccfbscccdfbsccfbscccd#scscsccscc";
	int middle;
	char* decoded_str = (char*)malloc(100 * sizeof(char));
	if (!decoded_str) return NULL;
	decoded_str[0] = '\0';

	int pos;
	char* initial_state = (char*)malloc(8 * sizeof(char));
	if (!initial_state) return;
	strcpy(initial_state, "fa");


	MT* machine = initialize_machine();

	char* delimiter = strchr(string, '#');
	
	if (!delimiter) {
		printf("Erro: não havia delimitador");
		return 1;
	}

	for (pos = 0; string[pos] != '#';) {
		char* i_state = fetch(&pos, string, 'e');
		char* r_char = fetch(&pos, string, 't');
		char* f_state = fetch(&pos, string, 'e');
		char* w_char = fetch(&pos, string, 't');
		char* direction = fetch(&pos, string, 'd');

		Transition *t = initialize_transition(i_state, r_char, f_state, w_char, direction);

		add_transition(machine, t);
		free(t);
	}

	middle = pos;

	for (int i = 0; i < machine->index; i++) {
		printf("(%s, %s) -> (%s, %s, %c)\n",
			machine->transitions[i].crr_state,
			machine->transitions[i].read_symbol,
			machine->transitions[i].final_state,
			machine->transitions[i].writen_symbol,
			machine->transitions[i].direction);
	}

	translate_str(decoded_str, string, pos);
	printf("%s", decoded_str);
	
	bool accept = execute_machine(&pos, string, initial_state, machine, middle);

	if(accept){
		printf("\nboa dog!");
	}
	else
	{
		printf("\nfracasso");
	}

	destruct_machine(machine);
	return 0;
}