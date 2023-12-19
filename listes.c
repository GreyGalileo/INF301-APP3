#include <stdlib.h>
#include <stdio.h>
#include <assert.h>
#include <string.h>
#include "listes.h"

/* fichier à compléter au besoin */

void init_liste_vide(liste_t* L) {
   if(L == NULL){
    L = (liste_t *) malloc(sizeof(liste_t));
    L->tete = NULL;
   } else {
    printf("Pointer to uninitialised linked list is not a null pointer\n");
   }
}

void liberer_liste(liste_t* L) {
    if(L == NULL){return;}
   cellule_t *next = NULL;
   for (cellule_t *curr = L->tete; curr != NULL;){
    next = curr->suivant;
    free(curr->val);
    free(curr);
    curr = next;
   }
   //free(L);
   L->tete = NULL;
}


int ajouter_tete(liste_t* L, string c) { /* retourne 0 si OK, 1 sinon  */
    if(L == NULL){ return 1; }
    cellule_t *newcell = (cellule_t *) malloc(sizeof(cellule_t));
    if (newcell == NULL){ return 1; }
    string new_str = strdup(c);
    newcell->val = new_str;
    newcell->suivant = L->tete;
    L->tete = newcell;
    return 0;
}

