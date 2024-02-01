#include <stdio.h>
#include <stdlib.h>
#include <string.h>


typedef enum { INPUT, OUTPUT, AND, OR, NAND, NOR, XOR, NOT, PASS, DECODER, MULTIPLEXER } kind_t;

struct gate {
    kind_t kind;
    int size; // indicates size of DECODER and MULTIPLEXER
    int *params;
    struct gate *next; // length determined by kind and size;
    // includes inputs and outputs, indicated by variable numbers
};

struct variable {
    char name[17];
    int val;
    int ttvalue;
    struct variable *next;
};

void evaluate(struct variable *vars, struct gate *gates, int gateIndex) {

    struct gate *current = gates;

    for (int i = 0; i < gateIndex; i++) {
        current = current->next;
    }

    if (strcmp(vars[current->params[0]].name,"0") == 0) {
        vars[current->params[0]].ttvalue = 0;
    } else if (strcmp(vars[current->params[0]].name,"1") == 0) {
        vars[current->params[0]].ttvalue = 1;
    } 

     if (strcmp(vars[current->params[1]].name,"0") == 0) {
        vars[current->params[1]].ttvalue = 0;
    } else if (strcmp(vars[current->params[1]].name,"1") == 0) {
        vars[current->params[1]].ttvalue = 1;
    }
    

        if (current->kind == AND) {
            vars[current->params[2]].ttvalue = vars[current->params[0]].ttvalue && vars[current->params[1]].ttvalue;
        } else if (current->kind == NOT) {
            vars[current->params[1]].ttvalue = !vars[current->params[0]].ttvalue;
        } else if (current->kind == OR) {
            vars[current->params[2]].ttvalue = vars[current->params[0]].ttvalue || vars[current->params[1]].ttvalue;
        } else if (current->kind == NAND) {
            vars[current->params[2]].ttvalue = !(vars[current->params[0]].ttvalue && vars[current->params[1]].ttvalue);
        } else if (current->kind == NOR) {
            vars[current->params[2]].ttvalue = !(vars[current->params[0]].ttvalue || vars[current->params[1]].ttvalue);
        } else if (current->kind == XOR) {
            vars[current->params[2]].ttvalue = vars[current->params[0]].ttvalue ^ vars[current->params[1]].ttvalue;
        } else if (current->kind == PASS) {
            vars[current->params[1]].ttvalue = vars[current->params[0]].ttvalue;
        } else if (current->kind == DECODER) {
            int inputIndex = 0;
            int outputIndex = current->size;
            int inputVal= 0;
            for (int i = 0; i < current->size; i++) {
                inputVal += vars[current->params[inputIndex + i]].ttvalue << (current->size - 1 - i);
            }
            for (int i = 0; i < (1 << current->size); i++) {
                if (i == inputVal) {
                vars[current->params[outputIndex + i]].ttvalue = 1;
                } else {
                vars[current->params[outputIndex + i]].ttvalue = 0;
                }
            }
        } else if (current->kind == MULTIPLEXER) {
            int numInput = 1 << current->size;  
            int numSelector = current->size;     
            int selectorIndex = numInput;
            int outputIndex = selectorIndex + numSelector;
            int selectedInput = 0;
            for (int i = 0; i < numSelector; i++) {
                selectedInput = (selectedInput << 1) + vars[current->params[i + selectorIndex]].ttvalue;
            }
            vars[current->params[outputIndex]].ttvalue = vars[current->params[selectedInput]].ttvalue;
        }
    }


void printTable(struct variable *vars, struct gate *gates, int sizeInput, int sizeOutput, int numOfGates) {

    int base = 1 << sizeInput;

    for (int row = 0; row < base; row++) {
        
        for (int i = 0; i < sizeInput; i++) {
            vars[i].ttvalue = (row >> (sizeInput - 1 - i)) & 1;
        }

        for (int i = 0; i < numOfGates; i++) {
            evaluate(vars, gates, i);
        }

        for (int i = 0; i < sizeInput; i++) {
            printf("%d ", vars[i].ttvalue); 
        }
        printf("%c ", '|');
        for (int i = sizeInput; i < sizeInput + sizeOutput; i++) {
            if (i == sizeInput + sizeOutput - 1) {
                printf("%d", vars[i].ttvalue);
            } else {
                 printf("%d ", vars[i].ttvalue);
            }
        }

        printf("\n");

    }
 
}

void freeVariableList(struct variable *head) {
    struct variable *current = head;
    struct variable *next;

    while (current != NULL) {
        next = current->next;
        free(current);
        current = next;
    }
}

void freeGateList(struct gate *head) {
    struct gate *current = head;
    struct gate *next;

    while (current != NULL) {
        next = current->next;
        free(current->params); 
        free(current);
        current = next;
    }
}

int main(int argc, char **argv) {
    if (argc != 2) {
        exit(EXIT_FAILURE);
    }

    FILE *inputFile = fopen(argv[1], "r");

    if (inputFile == NULL) {
        exit(EXIT_FAILURE);
    }

    char type[17];
    int sizeInput, sizeOutput;
    int value = 0;
    struct variable *variables = NULL;
    struct gate *gates = NULL;
    

    while (fscanf(inputFile, "%16s", type) == 1) {
        if (strcmp(type, "INPUT") == 0 || strcmp(type, "OUTPUT") == 0) {
            if (strcmp(type, "INPUT") == 0) {
                fscanf(inputFile, "%d", &sizeInput);
                for (int i = 0; i < sizeInput; i++) {
                struct variable *temp = malloc(sizeof(struct variable));
                fscanf(inputFile, "%16s", temp->name);
                temp->val = value;
                temp->next = NULL;
                if (variables == NULL) {
                    variables = temp;
                } else {
                    struct variable *current = variables;
                    while (current->next != NULL) {
                        current = current->next;
                    }
                    current->next = temp;
                }
                value++;
            } 
            } else {
                fscanf(inputFile, "%d", &sizeOutput);
                for (int i = 0; i < sizeOutput; i++) {
                struct variable *temp = malloc(sizeof(struct variable));
                fscanf(inputFile, "%16s", temp->name);
                temp->val = value;
                temp->next = NULL;
                if (variables == NULL) {
                    variables = temp;
                } else {
                    struct variable *current = variables;
                    while (current->next != NULL) {
                        current = current->next;
                    }
                    current->next = temp;
                }
                value++;
                 }  
            }
           
       } else if (strcmp(type, "NOT") == 0 || strcmp(type, "PASS") == 0) {
            struct gate *temp = malloc(sizeof(struct gate));
            temp->next = NULL;
            temp->size = 0;
            if (strcmp(type, "NOT") == 0) {
                temp->kind = NOT;
            } else {
                temp->kind = PASS;
            }
            temp->params = malloc(2 * sizeof(int));
            char v1[17], v2[17];
            fscanf(inputFile, "%16s %16s",v1,v2);
            struct variable *ptr = variables;
            while (ptr != NULL) {
                if (strcmp(ptr->name,v1) == 0) {
                    temp->params[0] = ptr->val;
                    break;
                }
                ptr = ptr->next;
            }

            if (ptr == NULL) {
                struct variable *tempVar = malloc(sizeof(struct variable));
                strcpy(tempVar->name,v1);
                tempVar->val = value;
                tempVar->next = NULL;
                value++;
                struct variable *current = variables;
                while (current->next != NULL) {
                    current = current->next;
                }
                current->next = tempVar;

                temp->params[0] = tempVar->val;
                
            }

            struct variable *ptr2 = variables;
            while (ptr2 != NULL) {
                if (strcmp(ptr2->name,v2) == 0) {
                    temp->params[1] = ptr2->val;
                    break;
                }
                ptr2 = ptr2->next;
            }

            if (ptr2 == NULL) {
                struct variable *tempVar = malloc(sizeof(struct variable));
                strcpy(tempVar->name,v2);
                tempVar->val = value;
                tempVar->next = NULL;
                value++;
                struct variable *current = variables;
                while (current->next != NULL) {
                    current = current->next;
                }
                current->next = tempVar;

                temp->params[1] = tempVar->val;
                
            }

            if (gates == NULL) {
                    gates = temp;
                } else {
                    struct gate *ptr = gates;
                    while (ptr->next != NULL) {
                        ptr = ptr->next;
                    }
                    ptr->next = temp;
                   
                }


       } else if (strcmp(type, "AND") == 0 || strcmp(type, "OR") == 0 || strcmp(type, "NAND") == 0 || strcmp(type, "NOR") == 0 || strcmp(type, "XOR") == 0) {
            struct gate *temp = malloc(sizeof(struct gate));
            temp->next = NULL;
            temp->size = 0;
            if (strcmp(type, "AND") == 0) {
                temp->kind = AND;
            } else if (strcmp(type, "OR") == 0) {
                temp->kind = OR;
            } else if (strcmp(type, "NAND") == 0) {
                temp->kind = NAND;
            } else if (strcmp(type, "NOR") == 0) {
                temp->kind = NOR;
            } else if (strcmp(type, "XOR") == 0) {
                temp->kind = XOR;
            }

            temp->params = malloc(3 * sizeof(int));
            char v1[17], v2[17], v3[17];
            fscanf(inputFile, "%16s %16s %16s",v1,v2,v3);
            struct variable *ptr = variables;
            while (ptr != NULL) {
                if (strcmp(ptr->name,v1) == 0) {
                    temp->params[0] = ptr->val;
                    break;
                }
                ptr = ptr->next;
            }

            if (ptr == NULL) {
                struct variable *tempVar = malloc(sizeof(struct variable));
                strcpy(tempVar->name,v1);
                tempVar->val = value;
                tempVar->next = NULL;
                value++;
                struct variable *current = variables;
                while (current->next != NULL) {
                    current = current->next;
                }
                current->next = tempVar;

                temp->params[0] = tempVar->val;
               
            }

            struct variable *ptr2 = variables;
            while (ptr2 != NULL) {
                if (strcmp(ptr2->name,v2) == 0) {
                    temp->params[1] = ptr2->val;
                    break;
                }
                ptr2 = ptr2->next;
            }

            if (ptr2 == NULL) {
                struct variable *tempVar = malloc(sizeof(struct variable));
                strcpy(tempVar->name,v2);
                tempVar->val = value;
                tempVar->next = NULL;
                value++;
                struct variable *current = variables;
                while (current->next != NULL) {
                    current = current->next;
                }
                current->next = tempVar;

                temp->params[1] = tempVar->val;
               
            }

            struct variable *ptr3 = variables;
            while (ptr3 != NULL) {
                if (strcmp(ptr3->name,v3) == 0) {
                    temp->params[2] = ptr3->val;
                    break;
                }
                ptr3 = ptr3->next;
            }

            if (ptr3 == NULL) {
                struct variable *tempVar = malloc(sizeof(struct variable));
                strcpy(tempVar->name,v3);
                tempVar->val = value;
                tempVar->next = NULL;
                value++;
                struct variable *current = variables;
                while (current->next != NULL) {
                    current = current->next;
                }
                current->next = tempVar;

                temp->params[2] = tempVar->val;
                
            }

            if (gates == NULL) {
                    gates = temp;
                } else {
                    struct gate *ptr = gates;
                    while (ptr->next != NULL) {
                        ptr = ptr->next;
                    }
                    ptr->next = temp;
                   
                }                
        } else if (strcmp(type, "DECODER") == 0) {
            struct gate *temp = malloc(sizeof(struct gate));
            temp->kind = DECODER;
            temp->next = NULL;
            fscanf(inputFile, "%d", &temp->size);
            int pLength = 1 << temp->size;
            temp->params = malloc((temp->size + pLength) * sizeof(int));
            char v1[17];
            for (int i = 0; i < temp->size + pLength; i++) {
                fscanf(inputFile, "%16s", v1);
                struct variable *ptr = variables;
                while (ptr != NULL) {
                    if (strcmp(ptr->name,v1) == 0) {
                        temp->params[i] = ptr->val;
                        break;
                    }
                    ptr = ptr->next;
                }

                if (ptr == NULL) {
                    struct variable *newVar = malloc(sizeof(struct variable));
                    strcpy(newVar->name,v1);
                    newVar->val = value;
                    newVar->next = NULL;
                    value++;
                    struct variable *current = variables;
                    while (current->next != NULL) {
                        current = current->next;
                    }
                    current->next = newVar;

                    temp->params[i] = newVar->val;
                }

            }

            if (gates == NULL) {
                    gates = temp;
                } else {
                    struct gate *ptr = gates;
                    while (ptr->next != NULL) {
                        ptr = ptr->next;
                    }
                    ptr->next = temp;
                   
                }  
        } else if (strcmp(type, "MULTIPLEXER") == 0) {
            struct gate *temp = malloc(sizeof(struct gate));
            temp->kind = MULTIPLEXER;
            temp->next = NULL;
            fscanf(inputFile, "%d", &temp->size);
            int pLength = ((1 << temp->size) + temp->size + 1);
            temp->params = malloc(pLength * sizeof(int));
            char v1[17];
            for (int i = 0; i < pLength; i++) {
                fscanf(inputFile, "%16s", v1);
                struct variable *ptr = variables;
                while (ptr != NULL) {
                    if (strcmp(ptr->name,v1) == 0) {
                        temp->params[i] = ptr->val;
                        break;
                    }
                    ptr = ptr->next;
                }

                if (ptr == NULL) {
                    struct variable *newVar = malloc(sizeof(struct variable));
                    strcpy(newVar->name,v1);
                    newVar->val = value;
                    value++;
                    newVar->next = NULL;
                    struct variable *current = variables;
                    while (current->next != NULL) {
                        current = current->next;
                    }
                    current->next = newVar;

                    temp->params[i] = newVar->val;
                }
            }

               if (gates == NULL) {
                    gates = temp;
                } else {
                    struct gate *ptr = gates;
                    while (ptr->next != NULL) {
                        ptr = ptr->next;
                    }
                    ptr->next = temp;
                   
                }  
            
            }

        }



    struct variable *varPtr = variables;
    struct gate *gatePtr = gates;
    int numGates = 0;
    int numVars = 0;
    while (varPtr != NULL) {
        numVars++;
        varPtr = varPtr->next;
    }

    while (gatePtr != NULL) {
        numGates++;
        gatePtr = gatePtr->next;
    }

   
    struct variable *varArray = malloc(numVars * sizeof(struct variable));

    
    varPtr = variables;
    for (int i = 0; i < numVars; i++) {
        strcpy(varArray[i].name, varPtr->name);
        varArray[i].val = varPtr->val;
        varArray[i].next = NULL;  
        varPtr = varPtr->next;
    }

    printTable(varArray, gates, sizeInput, sizeOutput, numGates);
    fclose(inputFile);
    free(varArray);
    freeVariableList(variables);
    freeGateList(gates);
    return EXIT_SUCCESS;

}

   

  

    

    
   
   




  

