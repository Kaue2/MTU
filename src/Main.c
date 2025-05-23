/*projeto de implementar uma MTU em C */

#include<stdlib.h>
#include<stdio.h>
#include<string.h>
#include<stdbool.h>

#define MAX_TRANSICOES 100
#define MAX_FITA 1000

void change_state(char* str, int* pos, char flag);

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

bool execute_machine(int *pos, char *str, char *state, MT *machine) {
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
			for (int i = 0; i < machine->index; i++)
			{
				if (strcmp(symbol, machine->transitions[i].read_symbol) == 0 
					&& strcmp(state, machine->transitions[i].crr_state) == 0) {
					printf("\nTransicao encontrada\n");
					printf("(%s, %s) -> (%s, %s, %c)\n", state, symbol, machine->transitions[i].final_state,
						machine->transitions[i].writen_symbol,
						machine->transitions[i].direction);
					strcpy(symbol, "");
					change_state(str, pos, machine->transitions[i].direction);
					break;
				}
			}
		}
	}
	return accept;
}

void change_state(char* str, int* pos, char flag) {
	if (flag == 'd') {
		while (str[*pos] != '\0') {
			(*pos)++;
			if (str[*pos] == 's') {
				return;
			}
		}
	}
	else {
		while (*pos >= 0 && str[*pos] != '#') {
			(*pos)--;
			if (str[*pos] == 's') {
				return;
			}
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
	char string[] = "fascfascdfasccfbsccdfbsccfbsccd#scscsccscc";
	char* decoded_str = (char*)malloc(100 * sizeof(char));
	if (!decoded_str) return NULL;
	decoded_str[0] = '\0';

	int pos;
	char* initial_state = "fa";
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
	
	execute_machine(&pos, string, initial_state, machine);

	destruct_machine(machine);
	return 0;
}